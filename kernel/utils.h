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
sa_nracanaa stackmap
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
		အတွက် (auto &it : current_state)
			အကယ်၍ (!backup_state.empty() && backup_state.back().count(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		အတွက် (auto &it : other)
			set(it.first, it.second);
	}

	bool has(const Key &k)
	{
		ပြန်ပေး current_state.count(k) != 0;
	}

	void set(const Key &k, const T &v)
	{
		အကယ်၍ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	void unset(const Key &k)
	{
		အကယ်၍ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	const T &get(const Key &k)
	{
		အကယ်၍ (current_state.count(k) == 0)
			ပြန်ပေး empty_tuple;
		ပြန်ပေး current_state.at(k);
	}

	void reset(const Key &k)
	{
		အတွက် (ကိန်းပြည့် i = GetSize(backup_state)-1; i >= 0; i--)
			အကယ်၍ (backup_state[i].count(k) != 0) {
				အကယ်၍ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				မဟုတ်ပါက
					current_state[k] = *backup_state[i].at(k);
				ပြန်ပေး;
			}
		current_state.erase(k);
	}

	const dict<Key, T> &stdmap()
	{
		ပြန်ပေး current_state;
	}

	void save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	void restore()
	{
		log_assert(!backup_state.empty());
		အတွက် (auto &it : backup_state.back())
			အကယ်၍ (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} မဟုတ်ပါက
				current_state.erase(it.first);
		backup_state.pop_back();
	}

	~stackmap()
	{
		နေစဉ် (!backup_state.empty())
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
	sa_nracanaa IndirectCmp {
		IndirectCmp(const std::vector<T> &nodes, const C &node_cmp) : node_cmp_(node_cmp), nodes_(nodes) {}
		bool operator()(ကိန်းပြည့် a, ကိန်းပြည့် b) const
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			ပြန်ပေး node_cmp_(nodes_[a], nodes_[b]);
		}
		const C node_cmp_;
		const std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ကိန်းပြည့်, C> node_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<ကိန်းပြည့်, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(const C &cmp) : node_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	ကိန်းပြည့် node(T n)
	{
		auto rv = node_to_index.emplace(n, static_cast<ကိန်းပြည့်>(nodes.size()));
		အကယ်၍ (rv.second) {
			nodes.push_back(n);
			edges.push_back(std::set<ကိန်းပြည့်, IndirectCmp>(indirect_cmp));
		}
		ပြန်ပေး rv.first->second;
	}

	void edge(ကိန်းပြည့် l_index, ကိန်းပြည့် r_index) { edges[r_index].insert(l_index); }

	void edge(T left, T right) { edge(node(left), node(right)); }

	bool has_node(const T &node) { ပြန်ပေး node_to_index.find(node) != node_to_index.end(); }

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
		std::vector<ကိန်းပြည့်> stack;
		sorted.reserve(edges.size());

		အတွက် (const auto &it : node_to_index)
			sort_worker(it.second, node_is_sorted, node_is_on_stack, stack);

		log_assert(GetSize(sorted) == GetSize(nodes));

		ပြန်ပေး !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(node_to_index.key_comp());
		အတွက် (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(node_to_index.key_comp());
			အတွက် (ကိန်းပြည့် other_node : edges[i]) {
				converted_edge_set.insert(nodes[other_node]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		ပြန်ပေး database;
	}

private:
	bool found_loops;
	std::vector<T> nodes;
	const IndirectCmp indirect_cmp;

	void sort_worker(const ကိန်းပြည့် root_index, std::vector<bool> &node_is_sorted, std::vector<bool> &node_is_on_stack, std::vector<ကိန်းပြည့်> &stack)
	{
		အကယ်၍ (node_is_on_stack[root_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			အကယ်၍ (analyze_loops) {
				std::vector<T> loop;
				အတွက် (ကိန်းပြည့် i = GetSize(stack) - 1; i >= 0; i--) {
					const ကိန်းပြည့် index = stack[i];
					loop.push_back(nodes[index]);
					အကယ်၍ (index == root_index)
						bha_nga;
				}
				loops.insert(loop);
			}
			ပြန်ပေး;
		}

		// We're done if we've already sorted this subgraph
		အကယ်၍ (node_is_sorted[root_index])
			ပြန်ပေး;

		အကယ်၍ (!edges[root_index].empty()) {
			အကယ်၍ (analyze_loops)
				stack.push_back(root_index);
			node_is_on_stack[root_index] = true;

			အတွက် (ကိန်းပြည့် left_n : edges[root_index])
				sort_worker(left_n, node_is_sorted, node_is_on_stack, stack);

			အကယ်၍ (analyze_loops)
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
sa_nracanaa arrow_proxy {
	T v;
	explicit arrow_proxy(T const & v) : v(v) {}
	T* operator->() { ပြန်ပေး &v; }
};

anta_hstha ကိန်းပြည့် ceil_log2(ကိန်းပြည့် x)
{
#အကယ်၍ defined(__GNUC__)
        ပြန်ပေး x > 1 ? (8*parimaa_na(ကိန်းပြည့်)) - __builtin_clz(x-1) : 0;
#မဟုတ်ပါက
	အကယ်၍ (x <= 0)
		ပြန်ပေး 0;
	အတွက် (ကိန်းပြည့် i = 0; i < 32; i++)
		အကယ်၍ (((x-1) >> i) == 0)
			ပြန်ပေး i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	sa_nracanaa reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() const { ပြန်ပေး container.rbegin(); }
		auto end() const { ပြန်ပေး container.rend(); }
		T& container;
	};
	ပြန်ပေး reverse_view{container};
}

template <typename T>
auto reversed(const T& container) {
	sa_nracanaa reverse_view {
		reverse_view(const T& container) : container(container) {}
		auto begin() const { ပြန်ပေး container.rbegin(); }
		auto end() const { ပြန်ပေး container.rend(); }
		const T& container;
	};
	ပြန်ပေး reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
sa_nracanaa IntRange {
	ကိန်းပြည့် start_;
	ကိန်းပြည့် end_;
	sa_nracanaa Int {
		ကိန်းပြည့် v;
		ကိန်းပြည့် operator*() const { ပြန်ပေး v; }
		Int &operator++() { ++v; ပြန်ပေး *this; }
		bool operator!=(const Int &other) const { ပြန်ပေး v != other.v; }
	};
	Int begin() const { ပြန်ပေး {start_}; }
	Int end() const { ပြန်ပေး {end_}; }

	bool operator==(const IntRange &other) const { ပြန်ပေး start_ == other.start_ && end_ == other.end_; }
	bool operator!=(const IntRange &other) const { ပြန်ပေး !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
