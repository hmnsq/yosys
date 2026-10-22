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
		для (auto &it : current_state)
			якщо (!backup_state.empty() && backup_state.back().count(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		для (auto &it : other)
			set(it.first, it.second);
	}

	bool has(const Key &k)
	{
		повернути current_state.count(k) != 0;
	}

	void set(const Key &k, const T &v)
	{
		якщо (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	void unset(const Key &k)
	{
		якщо (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	const T &get(const Key &k)
	{
		якщо (current_state.count(k) == 0)
			повернути empty_tuple;
		повернути current_state.at(k);
	}

	void reset(const Key &k)
	{
		для (ціле i = GetSize(backup_state)-1; i >= 0; i--)
			якщо (backup_state[i].count(k) != 0) {
				якщо (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				інакше
					current_state[k] = *backup_state[i].at(k);
				повернути;
			}
		current_state.erase(k);
	}

	const dict<Key, T> &stdmap()
	{
		повернути current_state;
	}

	void save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	void restore()
	{
		log_assert(!backup_state.empty());
		для (auto &it : backup_state.back())
			якщо (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} інакше
				current_state.erase(it.first);
		backup_state.pop_back();
	}

	~stackmap()
	{
		поки (!backup_state.empty())
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
		bool operator()(ціле a, ціле b) const
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			повернути node_cmp_(nodes_[a], nodes_[b]);
		}
		const C node_cmp_;
		const std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ціле, C> node_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<ціле, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(const C &cmp) : node_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	ціле node(T n)
	{
		auto rv = node_to_index.emplace(n, static_cast<ціле>(nodes.size()));
		якщо (rv.second) {
			nodes.push_back(n);
			edges.push_back(std::set<ціле, IndirectCmp>(indirect_cmp));
		}
		повернути rv.first->second;
	}

	void edge(ціле l_index, ціле r_index) { edges[r_index].insert(l_index); }

	void edge(T left, T right) { edge(node(left), node(right)); }

	bool has_node(const T &node) { повернути node_to_index.find(node) != node_to_index.end(); }

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
		std::vector<ціле> stack;
		sorted.reserve(edges.size());

		для (const auto &it : node_to_index)
			sort_worker(it.second, node_is_sorted, node_is_on_stack, stack);

		log_assert(GetSize(sorted) == GetSize(nodes));

		повернути !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(node_to_index.key_comp());
		для (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(node_to_index.key_comp());
			для (ціле other_node : edges[i]) {
				converted_edge_set.insert(nodes[other_node]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		повернути database;
	}

private:
	bool found_loops;
	std::vector<T> nodes;
	const IndirectCmp indirect_cmp;

	void sort_worker(const ціле root_index, std::vector<bool> &node_is_sorted, std::vector<bool> &node_is_on_stack, std::vector<ціле> &stack)
	{
		якщо (node_is_on_stack[root_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			якщо (analyze_loops) {
				std::vector<T> loop;
				для (ціле i = GetSize(stack) - 1; i >= 0; i--) {
					const ціле index = stack[i];
					loop.push_back(nodes[index]);
					якщо (index == root_index)
						break;
				}
				loops.insert(loop);
			}
			повернути;
		}

		// We're done if we've already sorted this subgraph
		якщо (node_is_sorted[root_index])
			повернути;

		якщо (!edges[root_index].empty()) {
			якщо (analyze_loops)
				stack.push_back(root_index);
			node_is_on_stack[root_index] = true;

			для (ціле left_n : edges[root_index])
				sort_worker(left_n, node_is_sorted, node_is_on_stack, stack);

			якщо (analyze_loops)
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
	T* operator->() { повернути &v; }
};

inline ціле ceil_log2(ціле x)
{
#якщо defined(__GNUC__)
        повернути x > 1 ? (8*sizeof(ціле)) - __builtin_clz(x-1) : 0;
#інакше
	якщо (x <= 0)
		повернути 0;
	для (ціле i = 0; i < 32; i++)
		якщо (((x-1) >> i) == 0)
			повернути i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	struct reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() const { повернути container.rbegin(); }
		auto end() const { повернути container.rend(); }
		T& container;
	};
	повернути reverse_view{container};
}

template <typename T>
auto reversed(const T& container) {
	struct reverse_view {
		reverse_view(const T& container) : container(container) {}
		auto begin() const { повернути container.rbegin(); }
		auto end() const { повернути container.rend(); }
		const T& container;
	};
	повернути reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
struct IntRange {
	ціле start_;
	ціле end_;
	struct Int {
		ціле v;
		ціле operator*() const { повернути v; }
		Int &operator++() { ++v; повернути *this; }
		bool operator!=(const Int &other) const { повернути v != other.v; }
	};
	Int begin() const { повернути {start_}; }
	Int end() const { повернути {end_}; }

	bool operator==(const IntRange &other) const { повернути start_ == other.start_ && end_ == other.end_; }
	bool operator!=(const IntRange &other) const { повернути !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
