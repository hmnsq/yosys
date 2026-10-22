/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

// This file contains various c++ utility routines and helper classes that
// do not depend on any other components of yosys (except stuff like log_*).

#include "kernel/yosys.h"
#include <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename Key, typename T>
struct stackmap
{
private:
	std::vector<dict<Key, T*>> backup_state;
	dict<Key, T> current_state;
	static T empty_tuple;

public:
	stackmap() { }
	stackmap(const dict<Key, T> &other) : current_state(other) { }

	template<typename Other>
	void operator=(const Other &other)
	{
		위해 (auto &it : current_state)
			만약 (!backup_state.empty() && backup_state.back().count(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		위해 (auto &it : other)
			set(it.first, it.second);
	}

	bool has(const Key &k)
	{
		반환 current_state.count(k) != 0;
	}

	void set(const Key &k, const T &v)
	{
		만약 (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	void unset(const Key &k)
	{
		만약 (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	const T &get(const Key &k)
	{
		만약 (current_state.count(k) == 0)
			반환 empty_tuple;
		반환 current_state.at(k);
	}

	void reset(const Key &k)
	{
		위해 (정수 i = GetSize(backup_state)-1; i >= 0; i--)
			만약 (backup_state[i].count(k) != 0) {
				만약 (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				아니면
					current_state[k] = *backup_state[i].at(k);
				반환;
			}
		current_state.erase(k);
	}

	const dict<Key, T> &stdmap()
	{
		반환 current_state;
	}

	void save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	void restore()
	{
		log_assert(!backup_state.empty());
		위해 (auto &it : backup_state.back())
			만약 (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} 아니면
				current_state.erase(it.first);
		backup_state.pop_back();
	}

	~stackmap()
	{
		동안 (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::value && std::is_same<C, std::less<T>>::value),
			"std::less is run-to-run unstable for pointers");
public:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	struct IndirectCmp {
		IndirectCmp(const std::vector<T> &nodes, const C &node_cmp) : node_cmp_(node_cmp), nodes_(nodes) {}
		bool operator()(정수 a, 정수 b) const
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			반환 node_cmp_(nodes_[a], nodes_[b]);
		}
		const C node_cmp_;
		const std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, 정수, C> node_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<정수, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(const C &cmp) : node_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	정수 node(T n)
	{
		auto rv = node_to_index.emplace(n, static_cast<정수>(nodes.size()));
		만약 (rv.second) {
			nodes.push_back(n);
			edges.push_back(std::set<정수, IndirectCmp>(indirect_cmp));
		}
		반환 rv.first->second;
	}

	void edge(정수 l_index, 정수 r_index) { edges[r_index].insert(l_index); }

	void edge(T left, T right) { edge(node(left), node(right)); }

	bool has_node(const T &node) { 반환 node_to_index.find(node) != node_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(node_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> node_is_sorted(edges.size(), false);
		std::vector<bool> node_is_on_stack(edges.size(), false);
		// Only used with analyze_loops
		std::vector<정수> stack;
		sorted.reserve(edges.size());

		위해 (const auto &it : node_to_index)
			sort_worker(it.second, node_is_sorted, node_is_on_stack, stack);

		log_assert(GetSize(sorted) == GetSize(nodes));

		반환 !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(node_to_index.key_comp());
		위해 (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(node_to_index.key_comp());
			위해 (정수 other_node : edges[i]) {
				converted_edge_set.insert(nodes[other_node]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		반환 database;
	}

private:
	bool found_loops;
	std::vector<T> nodes;
	const IndirectCmp indirect_cmp;

	void sort_worker(const 정수 root_index, std::vector<bool> &node_is_sorted, std::vector<bool> &node_is_on_stack, std::vector<정수> &stack)
	{
		만약 (node_is_on_stack[root_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			만약 (analyze_loops) {
				std::vector<T> loop;
				위해 (정수 i = GetSize(stack) - 1; i >= 0; i--) {
					const 정수 index = stack[i];
					loop.push_back(nodes[index]);
					만약 (index == root_index)
						break;
				}
				loops.insert(loop);
			}
			반환;
		}

		// We're done if we've already sorted this subgraph
		만약 (node_is_sorted[root_index])
			반환;

		만약 (!edges[root_index].empty()) {
			만약 (analyze_loops)
				stack.push_back(root_index);
			node_is_on_stack[root_index] = true;

			위해 (정수 left_n : edges[root_index])
				sort_worker(left_n, node_is_sorted, node_is_on_stack, stack);

			만약 (analyze_loops)
				stack.pop_back();
			node_is_on_stack[root_index] = false;
		}

		node_is_sorted[root_index] = true;
		sorted.push_back(nodes[root_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
struct arrow_proxy {
	T v;
	explicit arrow_proxy(T const & v) : v(v) {}
	T* operator->() { 반환 &v; }
};

inline 정수 ceil_log2(정수 x)
{
#만약 defined(__GNUC__)
        반환 x > 1 ? (8*sizeof(정수)) - __builtin_clz(x-1) : 0;
#아니면
	만약 (x <= 0)
		반환 0;
	위해 (정수 i = 0; i < 32; i++)
		만약 (((x-1) >> i) == 0)
			반환 i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	struct reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() const { 반환 container.rbegin(); }
		auto end() const { 반환 container.rend(); }
		T& container;
	};
	반환 reverse_view{container};
}

template <typename T>
auto reversed(const T& container) {
	struct reverse_view {
		reverse_view(const T& container) : container(container) {}
		auto begin() const { 반환 container.rbegin(); }
		auto end() const { 반환 container.rend(); }
		const T& container;
	};
	반환 reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
struct IntRange {
	정수 start_;
	정수 end_;
	struct Int {
		정수 v;
		정수 operator*() const { 반환 v; }
		Int &operator++() { ++v; 반환 *this; }
		bool operator!=(const Int &other) const { 반환 v != other.v; }
	};
	Int begin() const { 반환 {start_}; }
	Int end() const { 반환 {end_}; }

	bool operator==(const IntRange &other) const { 반환 start_ == other.start_ && end_ == other.end_; }
	bool operator!=(const IntRange &other) const { 반환 !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
