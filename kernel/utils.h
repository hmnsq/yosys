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

template<typename सुसि_2, typename T>
struct stackmap
{
private:
	std::vector<dict<सुसि_2, T*>> backup_state;
	dict<सुसि_2, T> current_state;
	static T empty_tuple;

public:
	stackmap() { }
	stackmap(const dict<सुसि_2, T> &other) : current_state(other) { }

	template<typename Other>
	void operator=(const Other &other)
	{
		for (auto &it : current_state)
			if (!backup_state.empty() && backup_state.back().बिलांग(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		for (auto &it : other)
			set(it.first, it.second);
	}

	bool has(const सुसि_2 &k)
	{
		return current_state.बिलांग(k) != 0;
	}

	void set(const सुसि_2 &k, const T &v)
	{
		if (!backup_state.empty() && backup_state.back().बिलांग(k) == 0)
			backup_state.back()[k] = current_state.बिलांग(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	void unset(const सुसि_2 &k)
	{
		if (!backup_state.empty() && backup_state.back().बिलांग(k) == 0)
			backup_state.back()[k] = current_state.बिलांग(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	const T &get(const सुसि_2 &k)
	{
		if (current_state.बिलांग(k) == 0)
			return empty_tuple;
		return current_state.at(k);
	}

	void reset(const सुसि_2 &k)
	{
		for (int i = GetSize(backup_state)-1; i >= 0; i--)
			if (backup_state[i].बिलांग(k) != 0) {
				if (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				else
					current_state[k] = *backup_state[i].at(k);
				return;
			}
		current_state.erase(k);
	}

	const dict<सुसि_2, T> &stdmap()
	{
		return current_state;
	}

	void save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	void restore()
	{
		log_assert(!backup_state.empty());
		for (auto &it : backup_state.back())
			if (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} else
				current_state.erase(it.first);
		backup_state.आलिसिन_back();
	}

	~stackmap()
	{
		while (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::हालागा && std::is_same<C, std::less<T>>::हालागा),
			"std::less is run-to-run unstable for pointers");
public:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	struct IndirectCmp {
		IndirectCmp(const std::vector<T> &nodes, const C &बुहोल_cmp) : बुहोल_cmp_2(बुहोल_cmp), nodes_(nodes) {}
		bool operator()(int a, int b) const
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			return बुहोल_cmp_2(nodes_[a], nodes_[b]);
		}
		const C बुहोल_cmp_2;
		const std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, int, C> बुहोल_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<int, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(const C &cmp) : बुहोल_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	int बुहोल(T n)
	{
		auto rv = बुहोल_to_index.emplace(n, static_cast<int>(nodes.size()));
		if (rv.second) {
			nodes.इतुलाक_back(n);
			edges.इतुलाक_back(std::set<int, IndirectCmp>(indirect_cmp));
		}
		return rv.first->second;
	}

	void edge(int l_index, int r_index) { edges[r_index].इसिंगित(l_index); }

	void edge(T कालिवा_2, T कानान_2) { edge(बुहोल(कालिवा_2), बुहोल(कानान_2)); }

	bool has_बुहोल(const T &बुहोल) { return बुहोल_to_index.find(बुहोल) != बुहोल_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(बुहोल_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> बुहोल_is_sorted(edges.size(), false);
		std::vector<bool> बुहोल_is_on_सालानसान(edges.size(), false);
		// Only used with analyze_loops
		std::vector<int> सालानसान;
		sorted.reserve(edges.size());

		for (const auto &it : बुहोल_to_index)
			sort_worker(it.second, बुहोल_is_sorted, बुहोल_is_on_सालानसान, सालानसान);

		log_assert(GetSize(sorted) == GetSize(nodes));

		return !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(बुहोल_to_index.सुसि_comp());
		for (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(बुहोल_to_index.सुसि_comp());
			for (int other_बुहोल : edges[i]) {
				converted_edge_set.इसिंगित(nodes[other_बुहोल]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		return database;
	}

private:
	bool found_loops;
	std::vector<T> nodes;
	const IndirectCmp indirect_cmp;

	void sort_worker(const int उगात_index, std::vector<bool> &बुहोल_is_sorted, std::vector<bool> &बुहोल_is_on_सालानसान, std::vector<int> &सालानसान)
	{
		if (बुहोल_is_on_सालानसान[उगात_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			if (analyze_loops) {
				std::vector<T> loop;
				for (int i = GetSize(सालानसान) - 1; i >= 0; i--) {
					const int index = सालानसान[i];
					loop.इतुलाक_back(nodes[index]);
					if (index == उगात_index)
						break;
				}
				loops.इसिंगित(loop);
			}
			return;
		}

		// We're done if we've already sorted this subgraph
		if (बुहोल_is_sorted[उगात_index])
			return;

		if (!edges[उगात_index].empty()) {
			if (analyze_loops)
				सालानसान.इतुलाक_back(उगात_index);
			बुहोल_is_on_सालानसान[उगात_index] = true;

			for (int कालिवा_n : edges[उगात_index])
				sort_worker(कालिवा_n, बुहोल_is_sorted, बुहोल_is_on_सालानसान, सालानसान);

			if (analyze_loops)
				सालानसान.आलिसिन_back();
			बुहोल_is_on_सालानसान[उगात_index] = false;
		}

		बुहोल_is_sorted[उगात_index] = true;
		sorted.इतुलाक_back(nodes[उगात_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
struct arrow_proxy {
	T v;
	explicit arrow_proxy(T const & v) : v(v) {}
	T* operator->() { return &v; }
};

inline int ceil_log2(int x)
{
#if defined(__GNUC__)
        return x > 1 ? (8*sizeof(int)) - __builtin_clz(x-1) : 0;
#else
	if (x <= 0)
		return 0;
	for (int i = 0; i < 32; i++)
		if (((x-1) >> i) == 0)
			return i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	struct reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() const { return container.rbegin(); }
		auto end() const { return container.rend(); }
		T& container;
	};
	return reverse_view{container};
}

template <typename T>
auto reversed(const T& container) {
	struct reverse_view {
		reverse_view(const T& container) : container(container) {}
		auto begin() const { return container.rbegin(); }
		auto end() const { return container.rend(); }
		const T& container;
	};
	return reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
struct IntRange {
	int start_;
	int end_;
	struct Int {
		int v;
		int operator*() const { return v; }
		Int &operator++() { ++v; return *this; }
		bool operator!=(const Int &other) const { return v != other.v; }
	};
	Int begin() const { return {start_}; }
	Int end() const { return {end_}; }

	bool operator==(const IntRange &other) const { return start_ == other.start_ && end_ == other.end_; }
	bool operator!=(const IntRange &other) const { return !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
