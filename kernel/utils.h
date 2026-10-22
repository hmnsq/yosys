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
		за (auto &it : current_state)
			ако (!backup_state.empty() && backup_state.back().count(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		за (auto &it : other)
			set(it.first, it.second);
	}

	bool has(const Key &k)
	{
		врати current_state.count(k) != 0;
	}

	void set(const Key &k, const T &v)
	{
		ако (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	void unset(const Key &k)
	{
		ако (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	const T &get(const Key &k)
	{
		ако (current_state.count(k) == 0)
			врати empty_tuple;
		врати current_state.at(k);
	}

	void reset(const Key &k)
	{
		за (цео i = GetSize(backup_state)-1; i >= 0; i--)
			ако (backup_state[i].count(k) != 0) {
				ако (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				инако
					current_state[k] = *backup_state[i].at(k);
				врати;
			}
		current_state.erase(k);
	}

	const dict<Key, T> &stdmap()
	{
		врати current_state;
	}

	void save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	void restore()
	{
		log_assert(!backup_state.empty());
		за (auto &it : backup_state.back())
			ако (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} инако
				current_state.erase(it.first);
		backup_state.pop_back();
	}

	~stackmap()
	{
		док (!backup_state.empty())
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
		bool operator()(цео a, цео b) const
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			врати node_cmp_(nodes_[a], nodes_[b]);
		}
		const C node_cmp_;
		const std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, цео, C> node_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<цео, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(const C &cmp) : node_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	цео node(T n)
	{
		auto rv = node_to_index.emplace(n, static_cast<цео>(nodes.size()));
		ако (rv.second) {
			nodes.push_back(n);
			edges.push_back(std::set<цео, IndirectCmp>(indirect_cmp));
		}
		врати rv.first->second;
	}

	void edge(цео l_index, цео r_index) { edges[r_index].insert(l_index); }

	void edge(T left, T right) { edge(node(left), node(right)); }

	bool has_node(const T &node) { врати node_to_index.find(node) != node_to_index.end(); }

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
		std::vector<цео> stack;
		sorted.reserve(edges.size());

		за (const auto &it : node_to_index)
			sort_worker(it.second, node_is_sorted, node_is_on_stack, stack);

		log_assert(GetSize(sorted) == GetSize(nodes));

		врати !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(node_to_index.key_comp());
		за (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(node_to_index.key_comp());
			за (цео other_node : edges[i]) {
				converted_edge_set.insert(nodes[other_node]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		врати database;
	}

private:
	bool found_loops;
	std::vector<T> nodes;
	const IndirectCmp indirect_cmp;

	void sort_worker(const цео root_index, std::vector<bool> &node_is_sorted, std::vector<bool> &node_is_on_stack, std::vector<цео> &stack)
	{
		ако (node_is_on_stack[root_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			ако (analyze_loops) {
				std::vector<T> loop;
				за (цео i = GetSize(stack) - 1; i >= 0; i--) {
					const цео index = stack[i];
					loop.push_back(nodes[index]);
					ако (index == root_index)
						break;
				}
				loops.insert(loop);
			}
			врати;
		}

		// We're done if we've already sorted this subgraph
		ако (node_is_sorted[root_index])
			врати;

		ако (!edges[root_index].empty()) {
			ако (analyze_loops)
				stack.push_back(root_index);
			node_is_on_stack[root_index] = true;

			за (цео left_n : edges[root_index])
				sort_worker(left_n, node_is_sorted, node_is_on_stack, stack);

			ако (analyze_loops)
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
	T* operator->() { врати &v; }
};

inline цео ceil_log2(цео x)
{
#ако defined(__GNUC__)
        врати x > 1 ? (8*sizeof(цео)) - __builtin_clz(x-1) : 0;
#инако
	ако (x <= 0)
		врати 0;
	за (цео i = 0; i < 32; i++)
		ако (((x-1) >> i) == 0)
			врати i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	struct reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() const { врати container.rbegin(); }
		auto end() const { врати container.rend(); }
		T& container;
	};
	врати reverse_view{container};
}

template <typename T>
auto reversed(const T& container) {
	struct reverse_view {
		reverse_view(const T& container) : container(container) {}
		auto begin() const { врати container.rbegin(); }
		auto end() const { врати container.rend(); }
		const T& container;
	};
	врати reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
struct IntRange {
	цео start_;
	цео end_;
	struct Int {
		цео v;
		цео operator*() const { врати v; }
		Int &operator++() { ++v; врати *this; }
		bool operator!=(const Int &other) const { врати v != other.v; }
	};
	Int begin() const { врати {start_}; }
	Int end() const { врати {end_}; }

	bool operator==(const IntRange &other) const { врати start_ == other.start_ && end_ == other.end_; }
	bool operator!=(const IntRange &other) const { врати !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
