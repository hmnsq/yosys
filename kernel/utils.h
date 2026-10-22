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

template<typename مفتاح_2, typename T>
بنية stackmap
{
خاص:
	std::vector<dict<مفتاح_2, T*>> backup_state;
	dict<مفتاح_2, T> current_state;
	ساكن T empty_tuple;

عام:
	stackmap() { }
	stackmap(ثابت dict<مفتاح_2, T> &other) : current_state(other) { }

	template<typename Other>
	فراغ operator=(ثابت Other &other)
	{
		لكل (auto &it : current_state)
			إذا (!backup_state.empty() && backup_state.back().عدد(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		لكل (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ثابت مفتاح_2 &k)
	{
		أرجع current_state.عدد(k) != 0;
	}

	فراغ set(ثابت مفتاح_2 &k, ثابت T &v)
	{
		إذا (!backup_state.empty() && backup_state.back().عدد(k) == 0)
			backup_state.back()[k] = current_state.عدد(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	فراغ unset(ثابت مفتاح_2 &k)
	{
		إذا (!backup_state.empty() && backup_state.back().عدد(k) == 0)
			backup_state.back()[k] = current_state.عدد(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت مفتاح_2 &k)
	{
		إذا (current_state.عدد(k) == 0)
			أرجع empty_tuple;
		أرجع current_state.at(k);
	}

	فراغ reset(ثابت مفتاح_2 &k)
	{
		لكل (صحيح i = GetSize(backup_state)-1; i >= 0; i--)
			إذا (backup_state[i].عدد(k) != 0) {
				إذا (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				وإلا
					current_state[k] = *backup_state[i].at(k);
				أرجع;
			}
		current_state.erase(k);
	}

	ثابت dict<مفتاح_2, T> &stdmap()
	{
		أرجع current_state;
	}

	فراغ save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	فراغ restore()
	{
		log_assert(!backup_state.empty());
		لكل (auto &it : backup_state.back())
			إذا (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} وإلا
				current_state.erase(it.first);
		backup_state.اسحب_back();
	}

	~stackmap()
	{
		طالما (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::قيمة && std::is_same<C, std::less<T>>::قيمة),
			"std::less is run-to-run unstable for pointers");
عام:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	بنية IndirectCmp {
		IndirectCmp(ثابت std::vector<T> &nodes, ثابت C &عقدة_cmp) : عقدة_cmp_2(عقدة_cmp), nodes_(nodes) {}
		bool operator()(صحيح a, صحيح b) ثابت
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			أرجع عقدة_cmp_2(nodes_[a], nodes_[b]);
		}
		ثابت C عقدة_cmp_2;
		ثابت std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحيح, C> عقدة_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<صحيح, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ثابت C &cmp) : عقدة_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	صحيح عقدة(T n)
	{
		auto rv = عقدة_to_index.emplace(n, static_cast<صحيح>(nodes.size()));
		إذا (rv.second) {
			nodes.ادفع_back(n);
			edges.ادفع_back(std::set<صحيح, IndirectCmp>(indirect_cmp));
		}
		أرجع rv.first->second;
	}

	فراغ edge(صحيح l_index, صحيح r_index) { edges[r_index].أدرج(l_index); }

	فراغ edge(T يسار_2, T يمين_2) { edge(عقدة(يسار_2), عقدة(يمين_2)); }

	bool has_عقدة(ثابت T &عقدة) { أرجع عقدة_to_index.find(عقدة) != عقدة_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(عقدة_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> عقدة_is_sorted(edges.size(), false);
		std::vector<bool> عقدة_is_on_مكدس(edges.size(), false);
		// Only used with analyze_loops
		std::vector<صحيح> مكدس;
		sorted.reserve(edges.size());

		لكل (ثابت auto &it : عقدة_to_index)
			sort_worker(it.second, عقدة_is_sorted, عقدة_is_on_مكدس, مكدس);

		log_assert(GetSize(sorted) == GetSize(nodes));

		أرجع !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(عقدة_to_index.مفتاح_comp());
		لكل (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(عقدة_to_index.مفتاح_comp());
			لكل (صحيح other_عقدة : edges[i]) {
				converted_edge_set.أدرج(nodes[other_عقدة]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		أرجع database;
	}

خاص:
	bool found_loops;
	std::vector<T> nodes;
	ثابت IndirectCmp indirect_cmp;

	فراغ sort_worker(ثابت صحيح جذر_index, std::vector<bool> &عقدة_is_sorted, std::vector<bool> &عقدة_is_on_مكدس, std::vector<صحيح> &مكدس)
	{
		إذا (عقدة_is_on_مكدس[جذر_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			إذا (analyze_loops) {
				std::vector<T> loop;
				لكل (صحيح i = GetSize(مكدس) - 1; i >= 0; i--) {
					ثابت صحيح index = مكدس[i];
					loop.ادفع_back(nodes[index]);
					إذا (index == جذر_index)
						اكسر;
				}
				loops.أدرج(loop);
			}
			أرجع;
		}

		// We're done if we've already sorted this subgraph
		إذا (عقدة_is_sorted[جذر_index])
			أرجع;

		إذا (!edges[جذر_index].empty()) {
			إذا (analyze_loops)
				مكدس.ادفع_back(جذر_index);
			عقدة_is_on_مكدس[جذر_index] = true;

			لكل (صحيح يسار_n : edges[جذر_index])
				sort_worker(يسار_n, عقدة_is_sorted, عقدة_is_on_مكدس, مكدس);

			إذا (analyze_loops)
				مكدس.اسحب_back();
			عقدة_is_on_مكدس[جذر_index] = false;
		}

		عقدة_is_sorted[جذر_index] = true;
		sorted.ادفع_back(nodes[جذر_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
بنية arrow_proxy {
	T v;
	explicit arrow_proxy(T ثابت & v) : v(v) {}
	T* operator->() { أرجع &v; }
};

مضمن صحيح ceil_log2(صحيح x)
{
#إذا defined(__GNUC__)
        أرجع x > 1 ? (8*حجم(صحيح)) - __builtin_clz(x-1) : 0;
#وإلا
	إذا (x <= 0)
		أرجع 0;
	لكل (صحيح i = 0; i < 32; i++)
		إذا (((x-1) >> i) == 0)
			أرجع i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	بنية reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ثابت { أرجع container.rbegin(); }
		auto end() ثابت { أرجع container.rend(); }
		T& container;
	};
	أرجع reverse_view{container};
}

template <typename T>
auto reversed(ثابت T& container) {
	بنية reverse_view {
		reverse_view(ثابت T& container) : container(container) {}
		auto begin() ثابت { أرجع container.rbegin(); }
		auto end() ثابت { أرجع container.rend(); }
		ثابت T& container;
	};
	أرجع reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
بنية IntRange {
	صحيح start_;
	صحيح end_;
	بنية Int {
		صحيح v;
		صحيح operator*() ثابت { أرجع v; }
		Int &operator++() { ++v; أرجع *this; }
		bool operator!=(ثابت Int &other) ثابت { أرجع v != other.v; }
	};
	Int begin() ثابت { أرجع {start_}; }
	Int end() ثابت { أرجع {end_}; }

	bool operator==(ثابت IntRange &other) ثابت { أرجع start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ثابت IntRange &other) ثابت { أرجع !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
