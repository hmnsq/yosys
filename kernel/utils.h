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
কাঠামো stackmap
{
ব্যক্তিগত:
	std::vector<dict<চাবি_2, T*>> backup_state;
	dict<চাবি_2, T> current_state;
	স্থির T empty_tuple;

সর্বজনীন:
	stackmap() { }
	stackmap(ধ্রুবক dict<চাবি_2, T> &other) : current_state(other) { }

	template<typename Other>
	শূন্য operator=(ধ্রুবক Other &other)
	{
		জন্য (auto &it : current_state)
			যদি (!backup_state.empty() && backup_state.back().গুনতি(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		জন্য (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ধ্রুবক চাবি_2 &k)
	{
		ফেরত current_state.গুনতি(k) != 0;
	}

	শূন্য set(ধ্রুবক চাবি_2 &k, ধ্রুবক T &v)
	{
		যদি (!backup_state.empty() && backup_state.back().গুনতি(k) == 0)
			backup_state.back()[k] = current_state.গুনতি(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	শূন্য unset(ধ্রুবক চাবি_2 &k)
	{
		যদি (!backup_state.empty() && backup_state.back().গুনতি(k) == 0)
			backup_state.back()[k] = current_state.গুনতি(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ধ্রুবক T &get(ধ্রুবক চাবি_2 &k)
	{
		যদি (current_state.গুনতি(k) == 0)
			ফেরত empty_tuple;
		ফেরত current_state.at(k);
	}

	শূন্য reset(ধ্রুবক চাবি_2 &k)
	{
		জন্য (পূর্ণসংখ্যা i = Get_আকার(backup_state)-1; i >= 0; i--)
			যদি (backup_state[i].গুনতি(k) != 0) {
				যদি (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				অন্যথা
					current_state[k] = *backup_state[i].at(k);
				ফেরত;
			}
		current_state.erase(k);
	}

	ধ্রুবক dict<চাবি_2, T> &stdmap()
	{
		ফেরত current_state;
	}

	শূন্য save()
	{
		backup_state.resize(backup_state.আকার_2()+1);
	}

	শূন্য restore()
	{
		log_assert(!backup_state.empty());
		জন্য (auto &it : backup_state.back())
			যদি (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} অন্যথা
				current_state.erase(it.first);
		backup_state.বের_করো_back();
	}

	~stackmap()
	{
		যখন (!backup_state.empty())
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
সর্বজনীন:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	কাঠামো IndirectCmp {
		IndirectCmp(ধ্রুবক std::vector<T> &nodes, ধ্রুবক C &গ্রন্থি_cmp) : গ্রন্থি_cmp_2(গ্রন্থি_cmp), nodes_(nodes) {}
		bool operator()(পূর্ণসংখ্যা a, পূর্ণসংখ্যা b) ধ্রুবক
		{
			log_assert(static_cast<আকার_t>(a) < nodes_.আকার_2());
			log_assert(static_cast<আকার_t>(b) < nodes_.আকার_2());
			ফেরত গ্রন্থি_cmp_2(nodes_[a], nodes_[b]);
		}
		ধ্রুবক C গ্রন্থি_cmp_2;
		ধ্রুবক std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, পূর্ণসংখ্যা, C> গ্রন্থি_to_সূচক;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<পূর্ণসংখ্যা, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ধ্রুবক C &cmp) : গ্রন্থি_to_সূচক(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	পূর্ণসংখ্যা গ্রন্থি(T n)
	{
		auto rv = গ্রন্থি_to_সূচক.emplace(n, static_cast<পূর্ণসংখ্যা>(nodes.আকার_2()));
		যদি (rv.second) {
			nodes.ঠেলো_back(n);
			edges.ঠেলো_back(std::set<পূর্ণসংখ্যা, IndirectCmp>(indirect_cmp));
		}
		ফেরত rv.first->second;
	}

	শূন্য edge(পূর্ণসংখ্যা l_সূচক, পূর্ণসংখ্যা r_সূচক) { edges[r_সূচক].ঢোকাও(l_সূচক); }

	শূন্য edge(T বাম_2, T ডান_2) { edge(গ্রন্থি(বাম_2), গ্রন্থি(ডান_2)); }

	bool has_গ্রন্থি(ধ্রুবক T &গ্রন্থি) { ফেরত গ্রন্থি_to_সূচক.find(গ্রন্থি) != গ্রন্থি_to_সূচক.end(); }

	bool sort()
	{
		log_assert(Get_আকার(গ্রন্থি_to_সূচক) == Get_আকার(edges));
		log_assert(Get_আকার(nodes) == Get_আকার(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> গ্রন্থি_is_sorted(edges.আকার_2(), false);
		std::vector<bool> গ্রন্থি_is_on_স্তূপ(edges.আকার_2(), false);
		// Only used with analyze_loops
		std::vector<পূর্ণসংখ্যা> স্তূপ;
		sorted.reserve(edges.আকার_2());

		জন্য (ধ্রুবক auto &it : গ্রন্থি_to_সূচক)
			sort_worker(it.second, গ্রন্থি_is_sorted, গ্রন্থি_is_on_স্তূপ, স্তূপ);

		log_assert(Get_আকার(sorted) == Get_আকার(nodes));

		ফেরত !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(গ্রন্থি_to_সূচক.চাবি_comp());
		জন্য (আকার_t i = 0; i < nodes.আকার_2(); ++i) {
			std::set<T, C> converted_edge_set(গ্রন্থি_to_সূচক.চাবি_comp());
			জন্য (পূর্ণসংখ্যা other_গ্রন্থি : edges[i]) {
				converted_edge_set.ঢোকাও(nodes[other_গ্রন্থি]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		ফেরত database;
	}

ব্যক্তিগত:
	bool found_loops;
	std::vector<T> nodes;
	ধ্রুবক IndirectCmp indirect_cmp;

	শূন্য sort_worker(ধ্রুবক পূর্ণসংখ্যা মূল_সূচক, std::vector<bool> &গ্রন্থি_is_sorted, std::vector<bool> &গ্রন্থি_is_on_স্তূপ, std::vector<পূর্ণসংখ্যা> &স্তূপ)
	{
		যদি (গ্রন্থি_is_on_স্তূপ[মূল_সূচক]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			যদি (analyze_loops) {
				std::vector<T> loop;
				জন্য (পূর্ণসংখ্যা i = Get_আকার(স্তূপ) - 1; i >= 0; i--) {
					ধ্রুবক পূর্ণসংখ্যা সূচক = স্তূপ[i];
					loop.ঠেলো_back(nodes[সূচক]);
					যদি (সূচক == মূল_সূচক)
						বিরতি;
				}
				loops.ঢোকাও(loop);
			}
			ফেরত;
		}

		// We're done if we've already sorted this subgraph
		যদি (গ্রন্থি_is_sorted[মূল_সূচক])
			ফেরত;

		যদি (!edges[মূল_সূচক].empty()) {
			যদি (analyze_loops)
				স্তূপ.ঠেলো_back(মূল_সূচক);
			গ্রন্থি_is_on_স্তূপ[মূল_সূচক] = true;

			জন্য (পূর্ণসংখ্যা বাম_n : edges[মূল_সূচক])
				sort_worker(বাম_n, গ্রন্থি_is_sorted, গ্রন্থি_is_on_স্তূপ, স্তূপ);

			যদি (analyze_loops)
				স্তূপ.বের_করো_back();
			গ্রন্থি_is_on_স্তূপ[মূল_সূচক] = false;
		}

		গ্রন্থি_is_sorted[মূল_সূচক] = true;
		sorted.ঠেলো_back(nodes[মূল_সূচক]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
কাঠামো arrow_proxy {
	T v;
	explicit arrow_proxy(T ধ্রুবক & v) : v(v) {}
	T* operator->() { ফেরত &v; }
};

অন্তঃরেখ পূর্ণসংখ্যা ceil_log2(পূর্ণসংখ্যা x)
{
#যদি defined(__GNUC__)
        ফেরত x > 1 ? (8*আকার(পূর্ণসংখ্যা)) - __builtin_clz(x-1) : 0;
#অন্যথা
	যদি (x <= 0)
		ফেরত 0;
	জন্য (পূর্ণসংখ্যা i = 0; i < 32; i++)
		যদি (((x-1) >> i) == 0)
			ফেরত i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	কাঠামো reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ধ্রুবক { ফেরত container.rbegin(); }
		auto end() ধ্রুবক { ফেরত container.rend(); }
		T& container;
	};
	ফেরত reverse_view{container};
}

template <typename T>
auto reversed(ধ্রুবক T& container) {
	কাঠামো reverse_view {
		reverse_view(ধ্রুবক T& container) : container(container) {}
		auto begin() ধ্রুবক { ফেরত container.rbegin(); }
		auto end() ধ্রুবক { ফেরত container.rend(); }
		ধ্রুবক T& container;
	};
	ফেরত reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
কাঠামো IntRange {
	পূর্ণসংখ্যা start_;
	পূর্ণসংখ্যা end_;
	কাঠামো Int {
		পূর্ণসংখ্যা v;
		পূর্ণসংখ্যা operator*() ধ্রুবক { ফেরত v; }
		Int &operator++() { ++v; ফেরত *this; }
		bool operator!=(ধ্রুবক Int &other) ধ্রুবক { ফেরত v != other.v; }
	};
	Int begin() ধ্রুবক { ফেরত {start_}; }
	Int end() ধ্রুবক { ফেরত {end_}; }

	bool operator==(ধ্রুবক IntRange &other) ধ্রুবক { ফেরত start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ধ্রুবক IntRange &other) ধ্রুবক { ফেরত !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
