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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename ଚାବି_2, typename T>
ସଂରଚନା stackmap
{
ବ୍ୟକ୍ତିଗତ:
	std::vector<dict<ଚାବି_2, T*>> backup_state;
	dict<ଚାବି_2, T> current_state;
	ସ୍ଥିର T empty_tuple;

ସର୍ବସାଧାରଣ:
	stackmap() { }
	stackmap(ଅଚଳ dict<ଚାବି_2, T> &other) : current_state(other) { }

	template<typename Other>
	ଶୂନ୍ୟ operator=(ଅଚଳ Other &other)
	{
		ପାଇଁ (auto &it : current_state)
			ଯଦି (!backup_state.empty() && backup_state.back().ଗଣନା_2(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		ପାଇଁ (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ଅଚଳ ଚାବି_2 &k)
	{
		ଫେରନ୍ତୁ current_state.ଗଣନା_2(k) != 0;
	}

	ଶୂନ୍ୟ set(ଅଚଳ ଚାବି_2 &k, ଅଚଳ T &v)
	{
		ଯଦି (!backup_state.empty() && backup_state.back().ଗଣନା_2(k) == 0)
			backup_state.back()[k] = current_state.ଗଣନା_2(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	ଶୂନ୍ୟ unset(ଅଚଳ ଚାବି_2 &k)
	{
		ଯଦି (!backup_state.empty() && backup_state.back().ଗଣନା_2(k) == 0)
			backup_state.back()[k] = current_state.ଗଣନା_2(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ଅଚଳ T &get(ଅଚଳ ଚାବି_2 &k)
	{
		ଯଦି (current_state.ଗଣନା_2(k) == 0)
			ଫେରନ୍ତୁ empty_tuple;
		ଫେରନ୍ତୁ current_state.at(k);
	}

	ଶୂନ୍ୟ reset(ଅଚଳ ଚାବି_2 &k)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = GetSize(backup_state)-1; i >= 0; i--)
			ଯଦି (backup_state[i].ଗଣନା_2(k) != 0) {
				ଯଦି (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				ଅନ୍ୟଥା
					current_state[k] = *backup_state[i].at(k);
				ଫେରନ୍ତୁ;
			}
		current_state.erase(k);
	}

	ଅଚଳ dict<ଚାବି_2, T> &stdmap()
	{
		ଫେରନ୍ତୁ current_state;
	}

	ଶୂନ୍ୟ save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	ଶୂନ୍ୟ restore()
	{
		log_assert(!backup_state.empty());
		ପାଇଁ (auto &it : backup_state.back())
			ଯଦି (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} ଅନ୍ୟଥା
				current_state.erase(it.first);
		backup_state.କାଢ଼_back();
	}

	~stackmap()
	{
		ଯେତେବେଳେ (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::ମୂଲ୍ୟ && std::is_same<C, std::less<T>>::ମୂଲ୍ୟ),
			"std::less is run-to-run unstable for pointers");
ସର୍ବସାଧାରଣ:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	ସଂରଚନା IndirectCmp {
		IndirectCmp(ଅଚଳ std::vector<T> &nodes, ଅଚଳ C &ଗଣ୍ଠି_cmp) : ଗଣ୍ଠି_cmp_2(ଗଣ୍ଠି_cmp), nodes_(nodes) {}
		bool operator()(ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b) ଅଚଳ
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			ଫେରନ୍ତୁ ଗଣ୍ଠି_cmp_2(nodes_[a], nodes_[b]);
		}
		ଅଚଳ C ଗଣ୍ଠି_cmp_2;
		ଅଚଳ std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, C> ଗଣ୍ଠି_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ଅଚଳ C &cmp) : ଗଣ୍ଠି_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି(T n)
	{
		auto rv = ଗଣ୍ଠି_to_index.emplace(n, static_cast<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(nodes.size()));
		ଯଦି (rv.second) {
			nodes.ଠେଲ_back(n);
			edges.ଠେଲ_back(std::set<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, IndirectCmp>(indirect_cmp));
		}
		ଫେରନ୍ତୁ rv.first->second;
	}

	ଶୂନ୍ୟ edge(ପୂର୍ଣ୍ଣସଂଖ୍ୟା l_index, ପୂର୍ଣ୍ଣସଂଖ୍ୟା r_index) { edges[r_index].ଭର୍ତ୍ତି(l_index); }

	ଶୂନ୍ୟ edge(T ବାମ_2, T ଡାହାଣ_2) { edge(ଗଣ୍ଠି(ବାମ_2), ଗଣ୍ଠି(ଡାହାଣ_2)); }

	bool has_ଗଣ୍ଠି(ଅଚଳ T &ଗଣ୍ଠି) { ଫେରନ୍ତୁ ଗଣ୍ଠି_to_index.find(ଗଣ୍ଠି) != ଗଣ୍ଠି_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(ଗଣ୍ଠି_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> ଗଣ୍ଠି_is_sorted(edges.size(), false);
		std::vector<bool> ଗଣ୍ଠି_is_on_ଗଦା(edges.size(), false);
		// Only used with analyze_loops
		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> ଗଦା;
		sorted.reserve(edges.size());

		ପାଇଁ (ଅଚଳ auto &it : ଗଣ୍ଠି_to_index)
			sort_worker(it.second, ଗଣ୍ଠି_is_sorted, ଗଣ୍ଠି_is_on_ଗଦା, ଗଦା);

		log_assert(GetSize(sorted) == GetSize(nodes));

		ଫେରନ୍ତୁ !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(ଗଣ୍ଠି_to_index.ଚାବି_comp());
		ପାଇଁ (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(ଗଣ୍ଠି_to_index.ଚାବି_comp());
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା other_ଗଣ୍ଠି : edges[i]) {
				converted_edge_set.ଭର୍ତ୍ତି(nodes[other_ଗଣ୍ଠି]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		ଫେରନ୍ତୁ database;
	}

ବ୍ୟକ୍ତିଗତ:
	bool found_loops;
	std::vector<T> nodes;
	ଅଚଳ IndirectCmp indirect_cmp;

	ଶୂନ୍ୟ sort_worker(ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_index, std::vector<bool> &ଗଣ୍ଠି_is_sorted, std::vector<bool> &ଗଣ୍ଠି_is_on_ଗଦା, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &ଗଦା)
	{
		ଯଦି (ଗଣ୍ଠି_is_on_ଗଦା[ମୂଳ_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			ଯଦି (analyze_loops) {
				std::vector<T> loop;
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = GetSize(ଗଦା) - 1; i >= 0; i--) {
					ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = ଗଦା[i];
					loop.ଠେଲ_back(nodes[index]);
					ଯଦି (index == ମୂଳ_index)
						ଭାଙ୍ଗ;
				}
				loops.ଭର୍ତ୍ତି(loop);
			}
			ଫେରନ୍ତୁ;
		}

		// We're done if we've already sorted this subgraph
		ଯଦି (ଗଣ୍ଠି_is_sorted[ମୂଳ_index])
			ଫେରନ୍ତୁ;

		ଯଦି (!edges[ମୂଳ_index].empty()) {
			ଯଦି (analyze_loops)
				ଗଦା.ଠେଲ_back(ମୂଳ_index);
			ଗଣ୍ଠି_is_on_ଗଦା[ମୂଳ_index] = true;

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ବାମ_n : edges[ମୂଳ_index])
				sort_worker(ବାମ_n, ଗଣ୍ଠି_is_sorted, ଗଣ୍ଠି_is_on_ଗଦା, ଗଦା);

			ଯଦି (analyze_loops)
				ଗଦା.କାଢ଼_back();
			ଗଣ୍ଠି_is_on_ଗଦା[ମୂଳ_index] = false;
		}

		ଗଣ୍ଠି_is_sorted[ମୂଳ_index] = true;
		sorted.ଠେଲ_back(nodes[ମୂଳ_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
ସଂରଚନା arrow_proxy {
	T v;
	explicit arrow_proxy(T ଅଚଳ & v) : v(v) {}
	T* operator->() { ଫେରନ୍ତୁ &v; }
};

ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ceil_log2(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x)
{
#ଯଦି defined(__GNUC__)
        ଫେରନ୍ତୁ x > 1 ? (8*ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)) - __builtin_clz(x-1) : 0;
#ଅନ୍ୟଥା
	ଯଦି (x <= 0)
		ଫେରନ୍ତୁ 0;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 32; i++)
		ଯଦି (((x-1) >> i) == 0)
			ଫେରନ୍ତୁ i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	ସଂରଚନା reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ଅଚଳ { ଫେରନ୍ତୁ container.rbegin(); }
		auto end() ଅଚଳ { ଫେରନ୍ତୁ container.rend(); }
		T& container;
	};
	ଫେରନ୍ତୁ reverse_view{container};
}

template <typename T>
auto reversed(ଅଚଳ T& container) {
	ସଂରଚନା reverse_view {
		reverse_view(ଅଚଳ T& container) : container(container) {}
		auto begin() ଅଚଳ { ଫେରନ୍ତୁ container.rbegin(); }
		auto end() ଅଚଳ { ଫେରନ୍ତୁ container.rend(); }
		ଅଚଳ T& container;
	};
	ଫେରନ୍ତୁ reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
ସଂରଚନା IntRange {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_;
	ସଂରଚନା Int {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା operator*() ଅଚଳ { ଫେରନ୍ତୁ v; }
		Int &operator++() { ++v; ଫେରନ୍ତୁ *this; }
		bool operator!=(ଅଚଳ Int &other) ଅଚଳ { ଫେରନ୍ତୁ v != other.v; }
	};
	Int begin() ଅଚଳ { ଫେରନ୍ତୁ {start_}; }
	Int end() ଅଚଳ { ଫେରନ୍ତୁ {end_}; }

	bool operator==(ଅଚଳ IntRange &other) ଅଚଳ { ଫେରନ୍ତୁ start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ଅଚଳ IntRange &other) ଅଚଳ { ଫେରନ୍ତୁ !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
