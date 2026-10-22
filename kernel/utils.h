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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename ಕೀಲಿ_2, typename T>
ಸಂರಚನಾ stackmap
{
ಗುಪ್ತ:
	std::vector<dict<ಕೀಲಿ_2, T*>> backup_state;
	dict<ಕೀಲಿ_2, T> current_state;
	ಸ್ಥಿರ T empty_tuple;

ಸಾರ್ವಜನಿಕ:
	stackmap() { }
	stackmap(ಧ್ರುವ dict<ಕೀಲಿ_2, T> &other) : current_state(other) { }

	template<typename Other>
	ಶೂನ್ಯ operator=(ಧ್ರುವ Other &other)
	{
		ಪ್ರತಿ (auto &it : current_state)
			ಯದಿ (!backup_state.empty() && backup_state.back().ಎಣಿಕೆ(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		ಪ್ರತಿ (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ಧ್ರುವ ಕೀಲಿ_2 &k)
	{
		ಪ್ರತ್ಯಾಗಮ current_state.ಎಣಿಕೆ(k) != 0;
	}

	ಶೂನ್ಯ set(ಧ್ರುವ ಕೀಲಿ_2 &k, ಧ್ರುವ T &v)
	{
		ಯದಿ (!backup_state.empty() && backup_state.back().ಎಣಿಕೆ(k) == 0)
			backup_state.back()[k] = current_state.ಎಣಿಕೆ(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	ಶೂನ್ಯ unset(ಧ್ರುವ ಕೀಲಿ_2 &k)
	{
		ಯದಿ (!backup_state.empty() && backup_state.back().ಎಣಿಕೆ(k) == 0)
			backup_state.back()[k] = current_state.ಎಣಿಕೆ(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ಧ್ರುವ T &get(ಧ್ರುವ ಕೀಲಿ_2 &k)
	{
		ಯದಿ (current_state.ಎಣಿಕೆ(k) == 0)
			ಪ್ರತ್ಯಾಗಮ empty_tuple;
		ಪ್ರತ್ಯಾಗಮ current_state.at(k);
	}

	ಶೂನ್ಯ reset(ಧ್ರುವ ಕೀಲಿ_2 &k)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = GetSize(backup_state)-1; i >= 0; i--)
			ಯದಿ (backup_state[i].ಎಣಿಕೆ(k) != 0) {
				ಯದಿ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				ಅನ್ಯಥಾ
					current_state[k] = *backup_state[i].at(k);
				ಪ್ರತ್ಯಾಗಮ;
			}
		current_state.erase(k);
	}

	ಧ್ರುವ dict<ಕೀಲಿ_2, T> &stdmap()
	{
		ಪ್ರತ್ಯಾಗಮ current_state;
	}

	ಶೂನ್ಯ save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	ಶೂನ್ಯ restore()
	{
		log_assert(!backup_state.empty());
		ಪ್ರತಿ (auto &it : backup_state.back())
			ಯದಿ (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} ಅನ್ಯಥಾ
				current_state.erase(it.first);
		backup_state.ತೆಗೆ_back();
	}

	~stackmap()
	{
		ಯಾವತ್ (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::ಮೌಲ್ಯ && std::is_same<C, std::less<T>>::ಮೌಲ್ಯ),
			"std::less is run-to-run unstable for pointers");
ಸಾರ್ವಜನಿಕ:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	ಸಂರಚನಾ IndirectCmp {
		IndirectCmp(ಧ್ರುವ std::vector<T> &nodes, ಧ್ರುವ C &ಗಂಟು_cmp) : ಗಂಟು_cmp_2(ಗಂಟು_cmp), nodes_(nodes) {}
		bool operator()(ಪೂರ್ಣಾಙ್ಕ a, ಪೂರ್ಣಾಙ್ಕ b) ಧ್ರುವ
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			ಪ್ರತ್ಯಾಗಮ ಗಂಟು_cmp_2(nodes_[a], nodes_[b]);
		}
		ಧ್ರುವ C ಗಂಟು_cmp_2;
		ಧ್ರುವ std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ಪೂರ್ಣಾಙ್ಕ, C> ಗಂಟು_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<ಪೂರ್ಣಾಙ್ಕ, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ಧ್ರುವ C &cmp) : ಗಂಟು_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	ಪೂರ್ಣಾಙ್ಕ ಗಂಟು(T n)
	{
		auto rv = ಗಂಟು_to_index.emplace(n, static_cast<ಪೂರ್ಣಾಙ್ಕ>(nodes.size()));
		ಯದಿ (rv.second) {
			nodes.ತಳ್ಳು_back(n);
			edges.ತಳ್ಳು_back(std::set<ಪೂರ್ಣಾಙ್ಕ, IndirectCmp>(indirect_cmp));
		}
		ಪ್ರತ್ಯಾಗಮ rv.first->second;
	}

	ಶೂನ್ಯ edge(ಪೂರ್ಣಾಙ್ಕ l_index, ಪೂರ್ಣಾಙ್ಕ r_index) { edges[r_index].ಸೇರಿಸು(l_index); }

	ಶೂನ್ಯ edge(T ಎಡ_2, T ಬಲ_2) { edge(ಗಂಟು(ಎಡ_2), ಗಂಟು(ಬಲ_2)); }

	bool has_ಗಂಟು(ಧ್ರುವ T &ಗಂಟು) { ಪ್ರತ್ಯಾಗಮ ಗಂಟು_to_index.find(ಗಂಟು) != ಗಂಟು_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(ಗಂಟು_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> ಗಂಟು_is_sorted(edges.size(), false);
		std::vector<bool> ಗಂಟು_is_on_ರಾಶಿ(edges.size(), false);
		// Only used with analyze_loops
		std::vector<ಪೂರ್ಣಾಙ್ಕ> ರಾಶಿ;
		sorted.reserve(edges.size());

		ಪ್ರತಿ (ಧ್ರುವ auto &it : ಗಂಟು_to_index)
			sort_worker(it.second, ಗಂಟು_is_sorted, ಗಂಟು_is_on_ರಾಶಿ, ರಾಶಿ);

		log_assert(GetSize(sorted) == GetSize(nodes));

		ಪ್ರತ್ಯಾಗಮ !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(ಗಂಟು_to_index.ಕೀಲಿ_comp());
		ಪ್ರತಿ (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(ಗಂಟು_to_index.ಕೀಲಿ_comp());
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ other_ಗಂಟು : edges[i]) {
				converted_edge_set.ಸೇರಿಸು(nodes[other_ಗಂಟು]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		ಪ್ರತ್ಯಾಗಮ database;
	}

ಗುಪ್ತ:
	bool found_loops;
	std::vector<T> nodes;
	ಧ್ರುವ IndirectCmp indirect_cmp;

	ಶೂನ್ಯ sort_worker(ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ ಬೇರು_index, std::vector<bool> &ಗಂಟು_is_sorted, std::vector<bool> &ಗಂಟು_is_on_ರಾಶಿ, std::vector<ಪೂರ್ಣಾಙ್ಕ> &ರಾಶಿ)
	{
		ಯದಿ (ಗಂಟು_is_on_ರಾಶಿ[ಬೇರು_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			ಯದಿ (analyze_loops) {
				std::vector<T> loop;
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = GetSize(ರಾಶಿ) - 1; i >= 0; i--) {
					ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ index = ರಾಶಿ[i];
					loop.ತಳ್ಳು_back(nodes[index]);
					ಯದಿ (index == ಬೇರು_index)
						ಭಙ್ಗ;
				}
				loops.ಸೇರಿಸು(loop);
			}
			ಪ್ರತ್ಯಾಗಮ;
		}

		// We're done if we've already sorted this subgraph
		ಯದಿ (ಗಂಟು_is_sorted[ಬೇರು_index])
			ಪ್ರತ್ಯಾಗಮ;

		ಯದಿ (!edges[ಬೇರು_index].empty()) {
			ಯದಿ (analyze_loops)
				ರಾಶಿ.ತಳ್ಳು_back(ಬೇರು_index);
			ಗಂಟು_is_on_ರಾಶಿ[ಬೇರು_index] = true;

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ಎಡ_n : edges[ಬೇರು_index])
				sort_worker(ಎಡ_n, ಗಂಟು_is_sorted, ಗಂಟು_is_on_ರಾಶಿ, ರಾಶಿ);

			ಯದಿ (analyze_loops)
				ರಾಶಿ.ತೆಗೆ_back();
			ಗಂಟು_is_on_ರಾಶಿ[ಬೇರು_index] = false;
		}

		ಗಂಟು_is_sorted[ಬೇರು_index] = true;
		sorted.ತಳ್ಳು_back(nodes[ಬೇರು_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
ಸಂರಚನಾ arrow_proxy {
	T v;
	explicit arrow_proxy(T ಧ್ರುವ & v) : v(v) {}
	T* operator->() { ಪ್ರತ್ಯಾಗಮ &v; }
};

ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ceil_log2(ಪೂರ್ಣಾಙ್ಕ x)
{
#ಯದಿ defined(__GNUC__)
        ಪ್ರತ್ಯಾಗಮ x > 1 ? (8*ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ)) - __builtin_clz(x-1) : 0;
#ಅನ್ಯಥಾ
	ಯದಿ (x <= 0)
		ಪ್ರತ್ಯಾಗಮ 0;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 32; i++)
		ಯದಿ (((x-1) >> i) == 0)
			ಪ್ರತ್ಯಾಗಮ i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	ಸಂರಚನಾ reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ container.rbegin(); }
		auto end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ container.rend(); }
		T& container;
	};
	ಪ್ರತ್ಯಾಗಮ reverse_view{container};
}

template <typename T>
auto reversed(ಧ್ರುವ T& container) {
	ಸಂರಚನಾ reverse_view {
		reverse_view(ಧ್ರುವ T& container) : container(container) {}
		auto begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ container.rbegin(); }
		auto end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ container.rend(); }
		ಧ್ರುವ T& container;
	};
	ಪ್ರತ್ಯಾಗಮ reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
ಸಂರಚನಾ IntRange {
	ಪೂರ್ಣಾಙ್ಕ start_;
	ಪೂರ್ಣಾಙ್ಕ end_;
	ಸಂರಚನಾ Int {
		ಪೂರ್ಣಾಙ್ಕ v;
		ಪೂರ್ಣಾಙ್ಕ operator*() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ v; }
		Int &operator++() { ++v; ಪ್ರತ್ಯಾಗಮ *this; }
		bool operator!=(ಧ್ರುವ Int &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ v != other.v; }
	};
	Int begin() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ {start_}; }
	Int end() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ {end_}; }

	bool operator==(ಧ್ರುವ IntRange &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ಧ್ರುವ IntRange &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
