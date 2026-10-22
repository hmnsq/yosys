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
		សម្រាប់ (auto &it : current_state)
			ប្រសិនបើ (!backup_state.empty() && backup_state.back().count(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		សម្រាប់ (auto &it : other)
			set(it.first, it.second);
	}

	bool has(const Key &k)
	{
		ត្រឡប់ current_state.count(k) != 0;
	}

	void set(const Key &k, const T &v)
	{
		ប្រសិនបើ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	void unset(const Key &k)
	{
		ប្រសិនបើ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	const T &get(const Key &k)
	{
		ប្រសិនបើ (current_state.count(k) == 0)
			ត្រឡប់ empty_tuple;
		ត្រឡប់ current_state.at(k);
	}

	void reset(const Key &k)
	{
		សម្រាប់ (ចំនួនគត់ i = GetSize(backup_state)-1; i >= 0; i--)
			ប្រសិនបើ (backup_state[i].count(k) != 0) {
				ប្រសិនបើ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				បើមិនដូច្នេះ
					current_state[k] = *backup_state[i].at(k);
				ត្រឡប់;
			}
		current_state.erase(k);
	}

	const dict<Key, T> &stdmap()
	{
		ត្រឡប់ current_state;
	}

	void save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	void restore()
	{
		log_assert(!backup_state.empty());
		សម្រាប់ (auto &it : backup_state.back())
			ប្រសិនបើ (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} បើមិនដូច្នេះ
				current_state.erase(it.first);
		backup_state.pop_back();
	}

	~stackmap()
	{
		ខណៈពេល (!backup_state.empty())
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
		bool operator()(ចំនួនគត់ a, ចំនួនគត់ b) const
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			ត្រឡប់ node_cmp_(nodes_[a], nodes_[b]);
		}
		const C node_cmp_;
		const std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ចំនួនគត់, C> node_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<ចំនួនគត់, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(const C &cmp) : node_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	ចំនួនគត់ node(T n)
	{
		auto rv = node_to_index.emplace(n, static_cast<ចំនួនគត់>(nodes.size()));
		ប្រសិនបើ (rv.second) {
			nodes.push_back(n);
			edges.push_back(std::set<ចំនួនគត់, IndirectCmp>(indirect_cmp));
		}
		ត្រឡប់ rv.first->second;
	}

	void edge(ចំនួនគត់ l_index, ចំនួនគត់ r_index) { edges[r_index].insert(l_index); }

	void edge(T left, T right) { edge(node(left), node(right)); }

	bool has_node(const T &node) { ត្រឡប់ node_to_index.find(node) != node_to_index.end(); }

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
		std::vector<ចំនួនគត់> stack;
		sorted.reserve(edges.size());

		សម្រាប់ (const auto &it : node_to_index)
			sort_worker(it.second, node_is_sorted, node_is_on_stack, stack);

		log_assert(GetSize(sorted) == GetSize(nodes));

		ត្រឡប់ !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(node_to_index.key_comp());
		សម្រាប់ (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(node_to_index.key_comp());
			សម្រាប់ (ចំនួនគត់ other_node : edges[i]) {
				converted_edge_set.insert(nodes[other_node]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		ត្រឡប់ database;
	}

private:
	bool found_loops;
	std::vector<T> nodes;
	const IndirectCmp indirect_cmp;

	void sort_worker(const ចំនួនគត់ root_index, std::vector<bool> &node_is_sorted, std::vector<bool> &node_is_on_stack, std::vector<ចំនួនគត់> &stack)
	{
		ប្រសិនបើ (node_is_on_stack[root_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			ប្រសិនបើ (analyze_loops) {
				std::vector<T> loop;
				សម្រាប់ (ចំនួនគត់ i = GetSize(stack) - 1; i >= 0; i--) {
					const ចំនួនគត់ index = stack[i];
					loop.push_back(nodes[index]);
					ប្រសិនបើ (index == root_index)
						bha_nga;
				}
				loops.insert(loop);
			}
			ត្រឡប់;
		}

		// We're done if we've already sorted this subgraph
		ប្រសិនបើ (node_is_sorted[root_index])
			ត្រឡប់;

		ប្រសិនបើ (!edges[root_index].empty()) {
			ប្រសិនបើ (analyze_loops)
				stack.push_back(root_index);
			node_is_on_stack[root_index] = true;

			សម្រាប់ (ចំនួនគត់ left_n : edges[root_index])
				sort_worker(left_n, node_is_sorted, node_is_on_stack, stack);

			ប្រសិនបើ (analyze_loops)
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
	T* operator->() { ត្រឡប់ &v; }
};

anta_hstha ចំនួនគត់ ceil_log2(ចំនួនគត់ x)
{
#ប្រសិនបើ defined(__GNUC__)
        ត្រឡប់ x > 1 ? (8*parimaa_na(ចំនួនគត់)) - __builtin_clz(x-1) : 0;
#បើមិនដូច្នេះ
	ប្រសិនបើ (x <= 0)
		ត្រឡប់ 0;
	សម្រាប់ (ចំនួនគត់ i = 0; i < 32; i++)
		ប្រសិនបើ (((x-1) >> i) == 0)
			ត្រឡប់ i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	sa_nracanaa reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() const { ត្រឡប់ container.rbegin(); }
		auto end() const { ត្រឡប់ container.rend(); }
		T& container;
	};
	ត្រឡប់ reverse_view{container};
}

template <typename T>
auto reversed(const T& container) {
	sa_nracanaa reverse_view {
		reverse_view(const T& container) : container(container) {}
		auto begin() const { ត្រឡប់ container.rbegin(); }
		auto end() const { ត្រឡប់ container.rend(); }
		const T& container;
	};
	ត្រឡប់ reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
sa_nracanaa IntRange {
	ចំនួនគត់ start_;
	ចំនួនគត់ end_;
	sa_nracanaa Int {
		ចំនួនគត់ v;
		ចំនួនគត់ operator*() const { ត្រឡប់ v; }
		Int &operator++() { ++v; ត្រឡប់ *this; }
		bool operator!=(const Int &other) const { ត្រឡប់ v != other.v; }
	};
	Int begin() const { ត្រឡប់ {start_}; }
	Int end() const { ត្រឡប់ {end_}; }

	bool operator==(const IntRange &other) const { ត្រឡប់ start_ == other.start_ && end_ == other.end_; }
	bool operator!=(const IntRange &other) const { ត្រឡប់ !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
