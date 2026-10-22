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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename ચાવી_2, typename T>
સંરચના stackmap
{
ખાનગી:
	std::vector<dict<ચાવી_2, T*>> backup_state;
	dict<ચાવી_2, T> current_state;
	સ્થિર T empty_tuple;

જાહેર:
	stackmap() { }
	stackmap(અચલ dict<ચાવી_2, T> &other) : current_state(other) { }

	template<typename Other>
	શૂન્ય operator=(અચલ Other &other)
	{
		માટે (auto &it : current_state)
			જો (!backup_state.empty() && backup_state.back().ગણતરી(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		માટે (auto &it : other)
			set(it.first, it.second);
	}

	bool has(અચલ ચાવી_2 &k)
	{
		પાછા current_state.ગણતરી(k) != 0;
	}

	શૂન્ય set(અચલ ચાવી_2 &k, અચલ T &v)
	{
		જો (!backup_state.empty() && backup_state.back().ગણતરી(k) == 0)
			backup_state.back()[k] = current_state.ગણતરી(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	શૂન્ય unset(અચલ ચાવી_2 &k)
	{
		જો (!backup_state.empty() && backup_state.back().ગણતરી(k) == 0)
			backup_state.back()[k] = current_state.ગણતરી(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	અચલ T &get(અચલ ચાવી_2 &k)
	{
		જો (current_state.ગણતરી(k) == 0)
			પાછા empty_tuple;
		પાછા current_state.at(k);
	}

	શૂન્ય reset(અચલ ચાવી_2 &k)
	{
		માટે (પૂર્ણાંક i = GetSize(backup_state)-1; i >= 0; i--)
			જો (backup_state[i].ગણતરી(k) != 0) {
				જો (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				નહીંતર
					current_state[k] = *backup_state[i].at(k);
				પાછા;
			}
		current_state.erase(k);
	}

	અચલ dict<ચાવી_2, T> &stdmap()
	{
		પાછા current_state;
	}

	શૂન્ય save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	શૂન્ય restore()
	{
		log_assert(!backup_state.empty());
		માટે (auto &it : backup_state.back())
			જો (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} નહીંતર
				current_state.erase(it.first);
		backup_state.કાઢો_back();
	}

	~stackmap()
	{
		જ્યારે (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::કિંમત && std::is_same<C, std::less<T>>::કિંમત),
			"std::less is run-to-run unstable for pointers");
જાહેર:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	સંરચના IndirectCmp {
		IndirectCmp(અચલ std::vector<T> &nodes, અચલ C &ગાંઠ_cmp) : ગાંઠ_cmp_2(ગાંઠ_cmp), nodes_(nodes) {}
		bool operator()(પૂર્ણાંક a, પૂર્ણાંક b) અચલ
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			પાછા ગાંઠ_cmp_2(nodes_[a], nodes_[b]);
		}
		અચલ C ગાંઠ_cmp_2;
		અચલ std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, પૂર્ણાંક, C> ગાંઠ_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<પૂર્ણાંક, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(અચલ C &cmp) : ગાંઠ_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	પૂર્ણાંક ગાંઠ(T n)
	{
		auto rv = ગાંઠ_to_index.emplace(n, static_cast<પૂર્ણાંક>(nodes.size()));
		જો (rv.second) {
			nodes.ધકેલો_back(n);
			edges.ધકેલો_back(std::set<પૂર્ણાંક, IndirectCmp>(indirect_cmp));
		}
		પાછા rv.first->second;
	}

	શૂન્ય edge(પૂર્ણાંક l_index, પૂર્ણાંક r_index) { edges[r_index].ઉમેરો(l_index); }

	શૂન્ય edge(T ડાબું_2, T જમણું_2) { edge(ગાંઠ(ડાબું_2), ગાંઠ(જમણું_2)); }

	bool has_ગાંઠ(અચલ T &ગાંઠ) { પાછા ગાંઠ_to_index.find(ગાંઠ) != ગાંઠ_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(ગાંઠ_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> ગાંઠ_is_sorted(edges.size(), false);
		std::vector<bool> ગાંઠ_is_on_ઢગલો(edges.size(), false);
		// Only used with analyze_loops
		std::vector<પૂર્ણાંક> ઢગલો;
		sorted.reserve(edges.size());

		માટે (અચલ auto &it : ગાંઠ_to_index)
			sort_worker(it.second, ગાંઠ_is_sorted, ગાંઠ_is_on_ઢગલો, ઢગલો);

		log_assert(GetSize(sorted) == GetSize(nodes));

		પાછા !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(ગાંઠ_to_index.ચાવી_comp());
		માટે (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(ગાંઠ_to_index.ચાવી_comp());
			માટે (પૂર્ણાંક other_ગાંઠ : edges[i]) {
				converted_edge_set.ઉમેરો(nodes[other_ગાંઠ]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		પાછા database;
	}

ખાનગી:
	bool found_loops;
	std::vector<T> nodes;
	અચલ IndirectCmp indirect_cmp;

	શૂન્ય sort_worker(અચલ પૂર્ણાંક મૂળ_index, std::vector<bool> &ગાંઠ_is_sorted, std::vector<bool> &ગાંઠ_is_on_ઢગલો, std::vector<પૂર્ણાંક> &ઢગલો)
	{
		જો (ગાંઠ_is_on_ઢગલો[મૂળ_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			જો (analyze_loops) {
				std::vector<T> loop;
				માટે (પૂર્ણાંક i = GetSize(ઢગલો) - 1; i >= 0; i--) {
					અચલ પૂર્ણાંક index = ઢગલો[i];
					loop.ધકેલો_back(nodes[index]);
					જો (index == મૂળ_index)
						તોડો;
				}
				loops.ઉમેરો(loop);
			}
			પાછા;
		}

		// We're done if we've already sorted this subgraph
		જો (ગાંઠ_is_sorted[મૂળ_index])
			પાછા;

		જો (!edges[મૂળ_index].empty()) {
			જો (analyze_loops)
				ઢગલો.ધકેલો_back(મૂળ_index);
			ગાંઠ_is_on_ઢગલો[મૂળ_index] = true;

			માટે (પૂર્ણાંક ડાબું_n : edges[મૂળ_index])
				sort_worker(ડાબું_n, ગાંઠ_is_sorted, ગાંઠ_is_on_ઢગલો, ઢગલો);

			જો (analyze_loops)
				ઢગલો.કાઢો_back();
			ગાંઠ_is_on_ઢગલો[મૂળ_index] = false;
		}

		ગાંઠ_is_sorted[મૂળ_index] = true;
		sorted.ધકેલો_back(nodes[મૂળ_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
સંરચના arrow_proxy {
	T v;
	explicit arrow_proxy(T અચલ & v) : v(v) {}
	T* operator->() { પાછા &v; }
};

અંતઃરેખ પૂર્ણાંક ceil_log2(પૂર્ણાંક x)
{
#જો defined(__GNUC__)
        પાછા x > 1 ? (8*કદ(પૂર્ણાંક)) - __builtin_clz(x-1) : 0;
#નહીંતર
	જો (x <= 0)
		પાછા 0;
	માટે (પૂર્ણાંક i = 0; i < 32; i++)
		જો (((x-1) >> i) == 0)
			પાછા i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	સંરચના reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() અચલ { પાછા container.rbegin(); }
		auto end() અચલ { પાછા container.rend(); }
		T& container;
	};
	પાછા reverse_view{container};
}

template <typename T>
auto reversed(અચલ T& container) {
	સંરચના reverse_view {
		reverse_view(અચલ T& container) : container(container) {}
		auto begin() અચલ { પાછા container.rbegin(); }
		auto end() અચલ { પાછા container.rend(); }
		અચલ T& container;
	};
	પાછા reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
સંરચના IntRange {
	પૂર્ણાંક start_;
	પૂર્ણાંક end_;
	સંરચના Int {
		પૂર્ણાંક v;
		પૂર્ણાંક operator*() અચલ { પાછા v; }
		Int &operator++() { ++v; પાછા *this; }
		bool operator!=(અચલ Int &other) અચલ { પાછા v != other.v; }
	};
	Int begin() અચલ { પાછા {start_}; }
	Int end() અચલ { પાછા {end_}; }

	bool operator==(અચલ IntRange &other) અચલ { પાછા start_ == other.start_ && end_ == other.end_; }
	bool operator!=(અચલ IntRange &other) અચલ { પાછા !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
