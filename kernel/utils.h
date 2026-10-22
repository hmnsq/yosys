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

#شامل "kernel/yosys.h"
#شامل <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename کلید_2, typename T>
ساختار stackmap
{
خصوصی:
	std::vector<dict<کلید_2, T*>> backup_state;
	dict<کلید_2, T> current_state;
	ایستا T empty_tuple;

عمومی:
	stackmap() { }
	stackmap(ثابت dict<کلید_2, T> &other) : current_state(other) { }

	template<typename Other>
	تهی operator=(ثابت Other &other)
	{
		برای (auto &it : current_state)
			اگر (!backup_state.empty() && backup_state.back().شمار(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		برای (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ثابت کلید_2 &k)
	{
		بازگشت current_state.شمار(k) != 0;
	}

	تهی set(ثابت کلید_2 &k, ثابت T &v)
	{
		اگر (!backup_state.empty() && backup_state.back().شمار(k) == 0)
			backup_state.back()[k] = current_state.شمار(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	تهی unset(ثابت کلید_2 &k)
	{
		اگر (!backup_state.empty() && backup_state.back().شمار(k) == 0)
			backup_state.back()[k] = current_state.شمار(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ثابت T &get(ثابت کلید_2 &k)
	{
		اگر (current_state.شمار(k) == 0)
			بازگشت empty_tuple;
		بازگشت current_state.at(k);
	}

	تهی reset(ثابت کلید_2 &k)
	{
		برای (صحیح i = GetSize(backup_state)-1; i >= 0; i--)
			اگر (backup_state[i].شمار(k) != 0) {
				اگر (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				وگرنه
					current_state[k] = *backup_state[i].at(k);
				بازگشت;
			}
		current_state.erase(k);
	}

	ثابت dict<کلید_2, T> &stdmap()
	{
		بازگشت current_state;
	}

	تهی save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	تهی restore()
	{
		log_assert(!backup_state.empty());
		برای (auto &it : backup_state.back())
			اگر (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} وگرنه
				current_state.erase(it.first);
		backup_state.برداشت_back();
	}

	~stackmap()
	{
		وقتی (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::مقدار && std::is_same<C, std::less<T>>::مقدار),
			"std::less is run-to-run unstable for pointers");
عمومی:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	ساختار IndirectCmp {
		IndirectCmp(ثابت std::vector<T> &nodes, ثابت C &گره_cmp) : گره_cmp_2(گره_cmp), nodes_(nodes) {}
		bool operator()(صحیح a, صحیح b) ثابت
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			بازگشت گره_cmp_2(nodes_[a], nodes_[b]);
		}
		ثابت C گره_cmp_2;
		ثابت std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, صحیح, C> گره_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<صحیح, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ثابت C &cmp) : گره_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	صحیح گره(T n)
	{
		auto rv = گره_to_index.emplace(n, static_cast<صحیح>(nodes.size()));
		اگر (rv.second) {
			nodes.افزودن_back(n);
			edges.افزودن_back(std::set<صحیح, IndirectCmp>(indirect_cmp));
		}
		بازگشت rv.first->second;
	}

	تهی edge(صحیح l_index, صحیح r_index) { edges[r_index].درج(l_index); }

	تهی edge(T چپ_2, T راست_2) { edge(گره(چپ_2), گره(راست_2)); }

	bool has_گره(ثابت T &گره) { بازگشت گره_to_index.find(گره) != گره_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(گره_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> گره_is_sorted(edges.size(), false);
		std::vector<bool> گره_is_on_پشته(edges.size(), false);
		// Only used with analyze_loops
		std::vector<صحیح> پشته;
		sorted.reserve(edges.size());

		برای (ثابت auto &it : گره_to_index)
			sort_worker(it.second, گره_is_sorted, گره_is_on_پشته, پشته);

		log_assert(GetSize(sorted) == GetSize(nodes));

		بازگشت !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(گره_to_index.کلید_comp());
		برای (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(گره_to_index.کلید_comp());
			برای (صحیح other_گره : edges[i]) {
				converted_edge_set.درج(nodes[other_گره]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		بازگشت database;
	}

خصوصی:
	bool found_loops;
	std::vector<T> nodes;
	ثابت IndirectCmp indirect_cmp;

	تهی sort_worker(ثابت صحیح ریشه_index, std::vector<bool> &گره_is_sorted, std::vector<bool> &گره_is_on_پشته, std::vector<صحیح> &پشته)
	{
		اگر (گره_is_on_پشته[ریشه_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			اگر (analyze_loops) {
				std::vector<T> loop;
				برای (صحیح i = GetSize(پشته) - 1; i >= 0; i--) {
					ثابت صحیح index = پشته[i];
					loop.افزودن_back(nodes[index]);
					اگر (index == ریشه_index)
						بشکن;
				}
				loops.درج(loop);
			}
			بازگشت;
		}

		// We're done if we've already sorted this subgraph
		اگر (گره_is_sorted[ریشه_index])
			بازگشت;

		اگر (!edges[ریشه_index].empty()) {
			اگر (analyze_loops)
				پشته.افزودن_back(ریشه_index);
			گره_is_on_پشته[ریشه_index] = true;

			برای (صحیح چپ_n : edges[ریشه_index])
				sort_worker(چپ_n, گره_is_sorted, گره_is_on_پشته, پشته);

			اگر (analyze_loops)
				پشته.برداشت_back();
			گره_is_on_پشته[ریشه_index] = false;
		}

		گره_is_sorted[ریشه_index] = true;
		sorted.افزودن_back(nodes[ریشه_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
ساختار arrow_proxy {
	T v;
	explicit arrow_proxy(T ثابت & v) : v(v) {}
	T* operator->() { بازگشت &v; }
};

درونخطی صحیح ceil_log2(صحیح x)
{
#اگر defined(__GNUC__)
        بازگشت x > 1 ? (8*اندازه(صحیح)) - __builtin_clz(x-1) : 0;
#وگرنه
	اگر (x <= 0)
		بازگشت 0;
	برای (صحیح i = 0; i < 32; i++)
		اگر (((x-1) >> i) == 0)
			بازگشت i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	ساختار reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ثابت { بازگشت container.rbegin(); }
		auto end() ثابت { بازگشت container.rend(); }
		T& container;
	};
	بازگشت reverse_view{container};
}

template <typename T>
auto reversed(ثابت T& container) {
	ساختار reverse_view {
		reverse_view(ثابت T& container) : container(container) {}
		auto begin() ثابت { بازگشت container.rbegin(); }
		auto end() ثابت { بازگشت container.rend(); }
		ثابت T& container;
	};
	بازگشت reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
ساختار IntRange {
	صحیح start_;
	صحیح end_;
	ساختار Int {
		صحیح v;
		صحیح operator*() ثابت { بازگشت v; }
		Int &operator++() { ++v; بازگشت *this; }
		bool operator!=(ثابت Int &other) ثابت { بازگشت v != other.v; }
	};
	Int begin() ثابت { بازگشت {start_}; }
	Int end() ثابت { بازگشت {end_}; }

	bool operator==(ثابت IntRange &other) ثابت { بازگشت start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ثابت IntRange &other) ثابت { بازگشت !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
