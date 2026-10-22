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
		ສຳລັບ (auto &it : current_state)
			ຖ້າ (!backup_state.empty() && backup_state.back().count(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		ສຳລັບ (auto &it : other)
			set(it.first, it.second);
	}

	bool has(const Key &k)
	{
		ສົ່ງຄືນ current_state.count(k) != 0;
	}

	void set(const Key &k, const T &v)
	{
		ຖ້າ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	void unset(const Key &k)
	{
		ຖ້າ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	const T &get(const Key &k)
	{
		ຖ້າ (current_state.count(k) == 0)
			ສົ່ງຄືນ empty_tuple;
		ສົ່ງຄືນ current_state.at(k);
	}

	void reset(const Key &k)
	{
		ສຳລັບ (ຈຳນວນເຕັມ i = GetSize(backup_state)-1; i >= 0; i--)
			ຖ້າ (backup_state[i].count(k) != 0) {
				ຖ້າ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				ບໍ່ດັ່ງນັ້ນ
					current_state[k] = *backup_state[i].at(k);
				ສົ່ງຄືນ;
			}
		current_state.erase(k);
	}

	const dict<Key, T> &stdmap()
	{
		ສົ່ງຄືນ current_state;
	}

	void save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	void restore()
	{
		log_assert(!backup_state.empty());
		ສຳລັບ (auto &it : backup_state.back())
			ຖ້າ (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} ບໍ່ດັ່ງນັ້ນ
				current_state.erase(it.first);
		backup_state.pop_back();
	}

	~stackmap()
	{
		ໃນຂະນະທີ່ (!backup_state.empty())
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
		bool operator()(ຈຳນວນເຕັມ a, ຈຳນວນເຕັມ b) const
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			ສົ່ງຄືນ node_cmp_(nodes_[a], nodes_[b]);
		}
		const C node_cmp_;
		const std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ຈຳນວນເຕັມ, C> node_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<ຈຳນວນເຕັມ, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(const C &cmp) : node_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	ຈຳນວນເຕັມ node(T n)
	{
		auto rv = node_to_index.emplace(n, static_cast<ຈຳນວນເຕັມ>(nodes.size()));
		ຖ້າ (rv.second) {
			nodes.push_back(n);
			edges.push_back(std::set<ຈຳນວນເຕັມ, IndirectCmp>(indirect_cmp));
		}
		ສົ່ງຄືນ rv.first->second;
	}

	void edge(ຈຳນວນເຕັມ l_index, ຈຳນວນເຕັມ r_index) { edges[r_index].insert(l_index); }

	void edge(T left, T right) { edge(node(left), node(right)); }

	bool has_node(const T &node) { ສົ່ງຄືນ node_to_index.find(node) != node_to_index.end(); }

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
		std::vector<ຈຳນວນເຕັມ> stack;
		sorted.reserve(edges.size());

		ສຳລັບ (const auto &it : node_to_index)
			sort_worker(it.second, node_is_sorted, node_is_on_stack, stack);

		log_assert(GetSize(sorted) == GetSize(nodes));

		ສົ່ງຄືນ !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(node_to_index.key_comp());
		ສຳລັບ (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(node_to_index.key_comp());
			ສຳລັບ (ຈຳນວນເຕັມ other_node : edges[i]) {
				converted_edge_set.insert(nodes[other_node]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		ສົ່ງຄືນ database;
	}

private:
	bool found_loops;
	std::vector<T> nodes;
	const IndirectCmp indirect_cmp;

	void sort_worker(const ຈຳນວນເຕັມ root_index, std::vector<bool> &node_is_sorted, std::vector<bool> &node_is_on_stack, std::vector<ຈຳນວນເຕັມ> &stack)
	{
		ຖ້າ (node_is_on_stack[root_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			ຖ້າ (analyze_loops) {
				std::vector<T> loop;
				ສຳລັບ (ຈຳນວນເຕັມ i = GetSize(stack) - 1; i >= 0; i--) {
					const ຈຳນວນເຕັມ index = stack[i];
					loop.push_back(nodes[index]);
					ຖ້າ (index == root_index)
						bha_nga;
				}
				loops.insert(loop);
			}
			ສົ່ງຄືນ;
		}

		// We're done if we've already sorted this subgraph
		ຖ້າ (node_is_sorted[root_index])
			ສົ່ງຄືນ;

		ຖ້າ (!edges[root_index].empty()) {
			ຖ້າ (analyze_loops)
				stack.push_back(root_index);
			node_is_on_stack[root_index] = true;

			ສຳລັບ (ຈຳນວນເຕັມ left_n : edges[root_index])
				sort_worker(left_n, node_is_sorted, node_is_on_stack, stack);

			ຖ້າ (analyze_loops)
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
	T* operator->() { ສົ່ງຄືນ &v; }
};

anta_hstha ຈຳນວນເຕັມ ceil_log2(ຈຳນວນເຕັມ x)
{
#ຖ້າ defined(__GNUC__)
        ສົ່ງຄືນ x > 1 ? (8*parimaa_na(ຈຳນວນເຕັມ)) - __builtin_clz(x-1) : 0;
#ບໍ່ດັ່ງນັ້ນ
	ຖ້າ (x <= 0)
		ສົ່ງຄືນ 0;
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < 32; i++)
		ຖ້າ (((x-1) >> i) == 0)
			ສົ່ງຄືນ i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	sa_nracanaa reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() const { ສົ່ງຄືນ container.rbegin(); }
		auto end() const { ສົ່ງຄືນ container.rend(); }
		T& container;
	};
	ສົ່ງຄືນ reverse_view{container};
}

template <typename T>
auto reversed(const T& container) {
	sa_nracanaa reverse_view {
		reverse_view(const T& container) : container(container) {}
		auto begin() const { ສົ່ງຄືນ container.rbegin(); }
		auto end() const { ສົ່ງຄືນ container.rend(); }
		const T& container;
	};
	ສົ່ງຄືນ reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
sa_nracanaa IntRange {
	ຈຳນວນເຕັມ start_;
	ຈຳນວນເຕັມ end_;
	sa_nracanaa Int {
		ຈຳນວນເຕັມ v;
		ຈຳນວນເຕັມ operator*() const { ສົ່ງຄືນ v; }
		Int &operator++() { ++v; ສົ່ງຄືນ *this; }
		bool operator!=(const Int &other) const { ສົ່ງຄືນ v != other.v; }
	};
	Int begin() const { ສົ່ງຄືນ {start_}; }
	Int end() const { ສົ່ງຄືນ {end_}; }

	bool operator==(const IntRange &other) const { ສົ່ງຄືນ start_ == other.start_ && end_ == other.end_; }
	bool operator!=(const IntRange &other) const { ສົ່ງຄືນ !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
