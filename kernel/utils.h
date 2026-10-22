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

#ضمن "kernel/yosys.h"
#ضمن <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename Key, typename T>
بنية stackmap
{
خاص:
	std::vector<dict<Key, T*>> backup_state;
	dict<Key, T> current_state;
	ساكن T empty_tuple;

عام:
	stackmap() { }
	stackmap(ثابت dict<Key, T> &other) : current_state(other) { }

	template<typename Other>
	فراغ operator=(ثابت Other &other)
	{
		لكل (auto &it : current_state)
			ܐܢ (!backup_state.empty() && backup_state.back().count(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		لكل (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ثابت Key &k)
	{
		ܦܢܐ current_state.count(k) != 0;
	}

	فراغ set(ثابت Key &k, ثابت T &v)
	{
		ܐܢ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	فراغ unset(ثابت Key &k)
	{
		ܐܢ (!backup_state.empty() && backup_state.back().count(k) == 0)
			backup_state.back()[k] = current_state.count(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت Key &k)
	{
		ܐܢ (current_state.count(k) == 0)
			ܦܢܐ empty_tuple;
		ܦܢܐ current_state.at(k);
	}

	فراغ reset(ثابت Key &k)
	{
		لكل (صحيح i = GetSize(backup_state)-1; i >= 0; i--)
			ܐܢ (backup_state[i].count(k) != 0) {
				ܐܢ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				ܐܚܪܢܐ
					current_state[k] = *backup_state[i].at(k);
				ܦܢܐ;
			}
		current_state.erase(k);
	}

	ثابت dict<Key, T> &stdmap()
	{
		ܦܢܐ current_state;
	}

	فراغ save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	فراغ restore()
	{
		log_assert(!backup_state.empty());
		لكل (auto &it : backup_state.back())
			ܐܢ (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} ܐܚܪܢܐ
				current_state.erase(it.first);
		backup_state.pop_back();
	}

	~stackmap()
	{
		ܟܡܐ (!backup_state.empty())
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
عام:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	بنية IndirectCmp {
		IndirectCmp(ثابت std::vector<T> &nodes, ثابت C &node_cmp) : node_cmp_(node_cmp), nodes_(nodes) {}
		bool operator()(صحيح a, صحيح b) ثابت
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			ܦܢܐ node_cmp_(nodes_[a], nodes_[b]);
		}
		ثابت C node_cmp_;
		ثابت std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحيح, C> node_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<صحيح, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ثابت C &cmp) : node_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	صحيح node(T n)
	{
		auto rv = node_to_index.emplace(n, static_cast<صحيح>(nodes.size()));
		ܐܢ (rv.second) {
			nodes.push_back(n);
			edges.push_back(std::set<صحيح, IndirectCmp>(indirect_cmp));
		}
		ܦܢܐ rv.first->second;
	}

	فراغ edge(صحيح l_index, صحيح r_index) { edges[r_index].insert(l_index); }

	فراغ edge(T left, T right) { edge(node(left), node(right)); }

	bool has_node(ثابت T &node) { ܦܢܐ node_to_index.find(node) != node_to_index.end(); }

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
		std::vector<صحيح> stack;
		sorted.reserve(edges.size());

		لكل (ثابت auto &it : node_to_index)
			sort_worker(it.second, node_is_sorted, node_is_on_stack, stack);

		log_assert(GetSize(sorted) == GetSize(nodes));

		ܦܢܐ !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(node_to_index.key_comp());
		لكل (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(node_to_index.key_comp());
			لكل (صحيح other_node : edges[i]) {
				converted_edge_set.insert(nodes[other_node]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		ܦܢܐ database;
	}

خاص:
	bool found_loops;
	std::vector<T> nodes;
	ثابت IndirectCmp indirect_cmp;

	فراغ sort_worker(ثابت صحيح root_index, std::vector<bool> &node_is_sorted, std::vector<bool> &node_is_on_stack, std::vector<صحيح> &stack)
	{
		ܐܢ (node_is_on_stack[root_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			ܐܢ (analyze_loops) {
				std::vector<T> loop;
				لكل (صحيح i = GetSize(stack) - 1; i >= 0; i--) {
					ثابت صحيح index = stack[i];
					loop.push_back(nodes[index]);
					ܐܢ (index == root_index)
						اكسر;
				}
				loops.insert(loop);
			}
			ܦܢܐ;
		}

		// We're done if we've already sorted this subgraph
		ܐܢ (node_is_sorted[root_index])
			ܦܢܐ;

		ܐܢ (!edges[root_index].empty()) {
			ܐܢ (analyze_loops)
				stack.push_back(root_index);
			node_is_on_stack[root_index] = true;

			لكل (صحيح left_n : edges[root_index])
				sort_worker(left_n, node_is_sorted, node_is_on_stack, stack);

			ܐܢ (analyze_loops)
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
بنية arrow_proxy {
	T v;
	explicit arrow_proxy(T ثابت & v) : v(v) {}
	T* operator->() { ܦܢܐ &v; }
};

مضمن صحيح ceil_log2(صحيح x)
{
#ܐܢ defined(__GNUC__)
        ܦܢܐ x > 1 ? (8*حجم(صحيح)) - __builtin_clz(x-1) : 0;
#ܐܚܪܢܐ
	ܐܢ (x <= 0)
		ܦܢܐ 0;
	لكل (صحيح i = 0; i < 32; i++)
		ܐܢ (((x-1) >> i) == 0)
			ܦܢܐ i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	بنية reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ثابت { ܦܢܐ container.rbegin(); }
		auto end() ثابت { ܦܢܐ container.rend(); }
		T& container;
	};
	ܦܢܐ reverse_view{container};
}

template <typename T>
auto reversed(ثابت T& container) {
	بنية reverse_view {
		reverse_view(ثابت T& container) : container(container) {}
		auto begin() ثابت { ܦܢܐ container.rbegin(); }
		auto end() ثابت { ܦܢܐ container.rend(); }
		ثابت T& container;
	};
	ܦܢܐ reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
بنية IntRange {
	صحيح start_;
	صحيح end_;
	بنية Int {
		صحيح v;
		صحيح operator*() ثابت { ܦܢܐ v; }
		Int &operator++() { ++v; ܦܢܐ *this; }
		bool operator!=(ثابت Int &other) ثابت { ܦܢܐ v != other.v; }
	};
	Int begin() ثابت { ܦܢܐ {start_}; }
	Int end() ثابت { ܦܢܐ {end_}; }

	bool operator==(ثابت IntRange &other) ثابت { ܦܢܐ start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ثابت IntRange &other) ثابت { ܦܢܐ !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
