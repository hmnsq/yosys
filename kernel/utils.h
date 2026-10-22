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

#অন্তর্ভাব "kernel/yosys.h"
#অন্তর্ভাব <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename চাবি_2, typename T>
সংরচনা stackmap
{
গুপ্ত:
	std::vector<dict<চাবি_2, T*>> backup_state;
	dict<চাবি_2, T> current_state;
	স্থির T empty_tuple;

সার্বজনিক:
	stackmap() { }
	stackmap(ধ্রুব dict<চাবি_2, T> &other) : current_state(other) { }

	template<typename Other>
	শূন্য operator=(ধ্রুব Other &other)
	{
		প্রতি (auto &it : current_state)
			যদি (!backup_state.empty() && backup_state.back().গুনতি(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		প্রতি (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ধ্রুব চাবি_2 &k)
	{
		প্রত্যাগম current_state.গুনতি(k) != 0;
	}

	শূন্য set(ধ্রুব চাবি_2 &k, ধ্রুব T &v)
	{
		যদি (!backup_state.empty() && backup_state.back().গুনতি(k) == 0)
			backup_state.back()[k] = current_state.গুনতি(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	শূন্য unset(ধ্রুব চাবি_2 &k)
	{
		যদি (!backup_state.empty() && backup_state.back().গুনতি(k) == 0)
			backup_state.back()[k] = current_state.গুনতি(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ধ্রুব T &get(ধ্রুব চাবি_2 &k)
	{
		যদি (current_state.গুনতি(k) == 0)
			প্রত্যাগম empty_tuple;
		প্রত্যাগম current_state.at(k);
	}

	শূন্য reset(ধ্রুব চাবি_2 &k)
	{
		প্রতি (পূর্ণাঙ্ক i = GetSize(backup_state)-1; i >= 0; i--)
			যদি (backup_state[i].গুনতি(k) != 0) {
				যদি (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				অন্যথা
					current_state[k] = *backup_state[i].at(k);
				প্রত্যাগম;
			}
		current_state.erase(k);
	}

	ধ্রুব dict<চাবি_2, T> &stdmap()
	{
		প্রত্যাগম current_state;
	}

	শূন্য save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	শূন্য restore()
	{
		log_assert(!backup_state.empty());
		প্রতি (auto &it : backup_state.back())
			যদি (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} অন্যথা
				current_state.erase(it.first);
		backup_state.বের_করো_back();
	}

	~stackmap()
	{
		যাবত্ (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::মান && std::is_same<C, std::less<T>>::মান),
			"std::less is run-to-run unstable for pointers");
সার্বজনিক:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	সংরচনা IndirectCmp {
		IndirectCmp(ধ্রুব std::vector<T> &nodes, ধ্রুব C &গ্রন্থি_cmp) : গ্রন্থি_cmp_2(গ্রন্থি_cmp), nodes_(nodes) {}
		bool operator()(পূর্ণাঙ্ক a, পূর্ণাঙ্ক b) ধ্রুব
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			প্রত্যাগম গ্রন্থি_cmp_2(nodes_[a], nodes_[b]);
		}
		ধ্রুব C গ্রন্থি_cmp_2;
		ধ্রুব std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, পূর্ণাঙ্ক, C> গ্রন্থি_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<পূর্ণাঙ্ক, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ধ্রুব C &cmp) : গ্রন্থি_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	পূর্ণাঙ্ক গ্রন্থি(T n)
	{
		auto rv = গ্রন্থি_to_index.emplace(n, static_cast<পূর্ণাঙ্ক>(nodes.size()));
		যদি (rv.second) {
			nodes.ঠেলো_back(n);
			edges.ঠেলো_back(std::set<পূর্ণাঙ্ক, IndirectCmp>(indirect_cmp));
		}
		প্রত্যাগম rv.first->second;
	}

	শূন্য edge(পূর্ণাঙ্ক l_index, পূর্ণাঙ্ক r_index) { edges[r_index].ঢোকাও(l_index); }

	শূন্য edge(T বাম_2, T ডান_2) { edge(গ্রন্থি(বাম_2), গ্রন্থি(ডান_2)); }

	bool has_গ্রন্থি(ধ্রুব T &গ্রন্থি) { প্রত্যাগম গ্রন্থি_to_index.find(গ্রন্থি) != গ্রন্থি_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(গ্রন্থি_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> গ্রন্থি_is_sorted(edges.size(), false);
		std::vector<bool> গ্রন্থি_is_on_স্তূপ(edges.size(), false);
		// Only used with analyze_loops
		std::vector<পূর্ণাঙ্ক> স্তূপ;
		sorted.reserve(edges.size());

		প্রতি (ধ্রুব auto &it : গ্রন্থি_to_index)
			sort_worker(it.second, গ্রন্থি_is_sorted, গ্রন্থি_is_on_স্তূপ, স্তূপ);

		log_assert(GetSize(sorted) == GetSize(nodes));

		প্রত্যাগম !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(গ্রন্থি_to_index.চাবি_comp());
		প্রতি (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(গ্রন্থি_to_index.চাবি_comp());
			প্রতি (পূর্ণাঙ্ক other_গ্রন্থি : edges[i]) {
				converted_edge_set.ঢোকাও(nodes[other_গ্রন্থি]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		প্রত্যাগম database;
	}

গুপ্ত:
	bool found_loops;
	std::vector<T> nodes;
	ধ্রুব IndirectCmp indirect_cmp;

	শূন্য sort_worker(ধ্রুব পূর্ণাঙ্ক মূল_index, std::vector<bool> &গ্রন্থি_is_sorted, std::vector<bool> &গ্রন্থি_is_on_স্তূপ, std::vector<পূর্ণাঙ্ক> &স্তূপ)
	{
		যদি (গ্রন্থি_is_on_স্তূপ[মূল_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			যদি (analyze_loops) {
				std::vector<T> loop;
				প্রতি (পূর্ণাঙ্ক i = GetSize(স্তূপ) - 1; i >= 0; i--) {
					ধ্রুব পূর্ণাঙ্ক index = স্তূপ[i];
					loop.ঠেলো_back(nodes[index]);
					যদি (index == মূল_index)
						ভঙ্গ;
				}
				loops.ঢোকাও(loop);
			}
			প্রত্যাগম;
		}

		// We're done if we've already sorted this subgraph
		যদি (গ্রন্থি_is_sorted[মূল_index])
			প্রত্যাগম;

		যদি (!edges[মূল_index].empty()) {
			যদি (analyze_loops)
				স্তূপ.ঠেলো_back(মূল_index);
			গ্রন্থি_is_on_স্তূপ[মূল_index] = true;

			প্রতি (পূর্ণাঙ্ক বাম_n : edges[মূল_index])
				sort_worker(বাম_n, গ্রন্থি_is_sorted, গ্রন্থি_is_on_স্তূপ, স্তূপ);

			যদি (analyze_loops)
				স্তূপ.বের_করো_back();
			গ্রন্থি_is_on_স্তূপ[মূল_index] = false;
		}

		গ্রন্থি_is_sorted[মূল_index] = true;
		sorted.ঠেলো_back(nodes[মূল_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
সংরচনা arrow_proxy {
	T v;
	explicit arrow_proxy(T ধ্রুব & v) : v(v) {}
	T* operator->() { প্রত্যাগম &v; }
};

অন্তঃস্থ পূর্ণাঙ্ক ceil_log2(পূর্ণাঙ্ক x)
{
#যদি defined(__GNUC__)
        প্রত্যাগম x > 1 ? (8*পরিমাণ(পূর্ণাঙ্ক)) - __builtin_clz(x-1) : 0;
#অন্যথা
	যদি (x <= 0)
		প্রত্যাগম 0;
	প্রতি (পূর্ণাঙ্ক i = 0; i < 32; i++)
		যদি (((x-1) >> i) == 0)
			প্রত্যাগম i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	সংরচনা reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ধ্রুব { প্রত্যাগম container.rbegin(); }
		auto end() ধ্রুব { প্রত্যাগম container.rend(); }
		T& container;
	};
	প্রত্যাগম reverse_view{container};
}

template <typename T>
auto reversed(ধ্রুব T& container) {
	সংরচনা reverse_view {
		reverse_view(ধ্রুব T& container) : container(container) {}
		auto begin() ধ্রুব { প্রত্যাগম container.rbegin(); }
		auto end() ধ্রুব { প্রত্যাগম container.rend(); }
		ধ্রুব T& container;
	};
	প্রত্যাগম reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
সংরচনা IntRange {
	পূর্ণাঙ্ক start_;
	পূর্ণাঙ্ক end_;
	সংরচনা Int {
		পূর্ণাঙ্ক v;
		পূর্ণাঙ্ক operator*() ধ্রুব { প্রত্যাগম v; }
		Int &operator++() { ++v; প্রত্যাগম *this; }
		bool operator!=(ধ্রুব Int &other) ধ্রুব { প্রত্যাগম v != other.v; }
	};
	Int begin() ধ্রুব { প্রত্যাগম {start_}; }
	Int end() ধ্রুব { প্রত্যাগম {end_}; }

	bool operator==(ধ্রুব IntRange &other) ধ্রুব { প্রত্যাগম start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ধ্রুব IntRange &other) ধ্রুব { প্রত্যাগম !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
