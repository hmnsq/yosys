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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename சாவி_2, typename T>
கட்டமைப்பு stackmap
{
தனிப்பட்ட:
	std::vector<dict<சாவி_2, T*>> backup_state;
	dict<சாவி_2, T> current_state;
	நிலையான T empty_tuple;

பொது:
	stackmap() { }
	stackmap(மாறிலி dict<சாவி_2, T> &other) : current_state(other) { }

	template<typename Other>
	வெறுமை operator=(மாறிலி Other &other)
	{
		க்கு (auto &it : current_state)
			எனில் (!backup_state.empty() && backup_state.back().மொத்தம்(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		க்கு (auto &it : other)
			set(it.first, it.second);
	}

	bool has(மாறிலி சாவி_2 &k)
	{
		திரும்பு current_state.மொத்தம்(k) != 0;
	}

	வெறுமை set(மாறிலி சாவி_2 &k, மாறிலி T &v)
	{
		எனில் (!backup_state.empty() && backup_state.back().மொத்தம்(k) == 0)
			backup_state.back()[k] = current_state.மொத்தம்(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	வெறுமை unset(மாறிலி சாவி_2 &k)
	{
		எனில் (!backup_state.empty() && backup_state.back().மொத்தம்(k) == 0)
			backup_state.back()[k] = current_state.மொத்தம்(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	மாறிலி T &get(மாறிலி சாவி_2 &k)
	{
		எனில் (current_state.மொத்தம்(k) == 0)
			திரும்பு empty_tuple;
		திரும்பு current_state.at(k);
	}

	வெறுமை reset(மாறிலி சாவி_2 &k)
	{
		க்கு (முழு i = GetSize(backup_state)-1; i >= 0; i--)
			எனில் (backup_state[i].மொத்தம்(k) != 0) {
				எனில் (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				இல்லையெனில்
					current_state[k] = *backup_state[i].at(k);
				திரும்பு;
			}
		current_state.erase(k);
	}

	மாறிலி dict<சாவி_2, T> &stdmap()
	{
		திரும்பு current_state;
	}

	வெறுமை save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	வெறுமை restore()
	{
		log_assert(!backup_state.empty());
		க்கு (auto &it : backup_state.back())
			எனில் (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} இல்லையெனில்
				current_state.erase(it.first);
		backup_state.எடு_back();
	}

	~stackmap()
	{
		போது (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::மதிப்பு && std::is_same<C, std::less<T>>::மதிப்பு),
			"std::less is run-to-run unstable for pointers");
பொது:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	கட்டமைப்பு IndirectCmp {
		IndirectCmp(மாறிலி std::vector<T> &nodes, மாறிலி C &முனை_cmp) : முனை_cmp_2(முனை_cmp), nodes_(nodes) {}
		bool operator()(முழு a, முழு b) மாறிலி
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			திரும்பு முனை_cmp_2(nodes_[a], nodes_[b]);
		}
		மாறிலி C முனை_cmp_2;
		மாறிலி std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, முழு, C> முனை_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<முழு, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(மாறிலி C &cmp) : முனை_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	முழு முனை(T n)
	{
		auto rv = முனை_to_index.emplace(n, static_cast<முழு>(nodes.size()));
		எனில் (rv.second) {
			nodes.தள்ளு_back(n);
			edges.தள்ளு_back(std::set<முழு, IndirectCmp>(indirect_cmp));
		}
		திரும்பு rv.first->second;
	}

	வெறுமை edge(முழு l_index, முழு r_index) { edges[r_index].செருகு(l_index); }

	வெறுமை edge(T இடது_2, T வலது_2) { edge(முனை(இடது_2), முனை(வலது_2)); }

	bool has_முனை(மாறிலி T &முனை) { திரும்பு முனை_to_index.find(முனை) != முனை_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(முனை_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> முனை_is_sorted(edges.size(), false);
		std::vector<bool> முனை_is_on_அடுக்கு(edges.size(), false);
		// Only used with analyze_loops
		std::vector<முழு> அடுக்கு;
		sorted.reserve(edges.size());

		க்கு (மாறிலி auto &it : முனை_to_index)
			sort_worker(it.second, முனை_is_sorted, முனை_is_on_அடுக்கு, அடுக்கு);

		log_assert(GetSize(sorted) == GetSize(nodes));

		திரும்பு !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(முனை_to_index.சாவி_comp());
		க்கு (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(முனை_to_index.சாவி_comp());
			க்கு (முழு other_முனை : edges[i]) {
				converted_edge_set.செருகு(nodes[other_முனை]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		திரும்பு database;
	}

தனிப்பட்ட:
	bool found_loops;
	std::vector<T> nodes;
	மாறிலி IndirectCmp indirect_cmp;

	வெறுமை sort_worker(மாறிலி முழு வேர்_index, std::vector<bool> &முனை_is_sorted, std::vector<bool> &முனை_is_on_அடுக்கு, std::vector<முழு> &அடுக்கு)
	{
		எனில் (முனை_is_on_அடுக்கு[வேர்_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			எனில் (analyze_loops) {
				std::vector<T> loop;
				க்கு (முழு i = GetSize(அடுக்கு) - 1; i >= 0; i--) {
					மாறிலி முழு index = அடுக்கு[i];
					loop.தள்ளு_back(nodes[index]);
					எனில் (index == வேர்_index)
						நிறுத்து;
				}
				loops.செருகு(loop);
			}
			திரும்பு;
		}

		// We're done if we've already sorted this subgraph
		எனில் (முனை_is_sorted[வேர்_index])
			திரும்பு;

		எனில் (!edges[வேர்_index].empty()) {
			எனில் (analyze_loops)
				அடுக்கு.தள்ளு_back(வேர்_index);
			முனை_is_on_அடுக்கு[வேர்_index] = true;

			க்கு (முழு இடது_n : edges[வேர்_index])
				sort_worker(இடது_n, முனை_is_sorted, முனை_is_on_அடுக்கு, அடுக்கு);

			எனில் (analyze_loops)
				அடுக்கு.எடு_back();
			முனை_is_on_அடுக்கு[வேர்_index] = false;
		}

		முனை_is_sorted[வேர்_index] = true;
		sorted.தள்ளு_back(nodes[வேர்_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
கட்டமைப்பு arrow_proxy {
	T v;
	explicit arrow_proxy(T மாறிலி & v) : v(v) {}
	T* operator->() { திரும்பு &v; }
};

உள்வரி முழு ceil_log2(முழு x)
{
#எனில் defined(__GNUC__)
        திரும்பு x > 1 ? (8*அளவு(முழு)) - __builtin_clz(x-1) : 0;
#இல்லையெனில்
	எனில் (x <= 0)
		திரும்பு 0;
	க்கு (முழு i = 0; i < 32; i++)
		எனில் (((x-1) >> i) == 0)
			திரும்பு i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	கட்டமைப்பு reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() மாறிலி { திரும்பு container.rbegin(); }
		auto end() மாறிலி { திரும்பு container.rend(); }
		T& container;
	};
	திரும்பு reverse_view{container};
}

template <typename T>
auto reversed(மாறிலி T& container) {
	கட்டமைப்பு reverse_view {
		reverse_view(மாறிலி T& container) : container(container) {}
		auto begin() மாறிலி { திரும்பு container.rbegin(); }
		auto end() மாறிலி { திரும்பு container.rend(); }
		மாறிலி T& container;
	};
	திரும்பு reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
கட்டமைப்பு IntRange {
	முழு start_;
	முழு end_;
	கட்டமைப்பு Int {
		முழு v;
		முழு operator*() மாறிலி { திரும்பு v; }
		Int &operator++() { ++v; திரும்பு *this; }
		bool operator!=(மாறிலி Int &other) மாறிலி { திரும்பு v != other.v; }
	};
	Int begin() மாறிலி { திரும்பு {start_}; }
	Int end() மாறிலி { திரும்பு {end_}; }

	bool operator==(மாறிலி IntRange &other) மாறிலி { திரும்பு start_ == other.start_ && end_ == other.end_; }
	bool operator!=(மாறிலி IntRange &other) மாறிலி { திரும்பு !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
