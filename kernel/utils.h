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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename कुञ्चिका_2, typename T>
संरचना stackmap
{
गुप्त:
	std::vector<dict<कुञ्चिका_2, T*>> backup_state;
	dict<कुञ्चिका_2, T> current_state;
	स्थिर T empty_tuple;

सार्वजनिक:
	stackmap() { }
	stackmap(ध्रुव dict<कुञ्चिका_2, T> &other) : current_state(other) { }

	template<typename Other>
	शून्य operator=(ध्रुव Other &other)
	{
		अत्थाय (auto &it : current_state)
			सचे (!backup_state.empty() && backup_state.back().गणनम्(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		अत्थाय (auto &it : other)
			set(it.first, it.second);
	}

	bool has(ध्रुव कुञ्चिका_2 &k)
	{
		पटिनिवत्त current_state.गणनम्(k) != 0;
	}

	शून्य set(ध्रुव कुञ्चिका_2 &k, ध्रुव T &v)
	{
		सचे (!backup_state.empty() && backup_state.back().गणनम्(k) == 0)
			backup_state.back()[k] = current_state.गणनम्(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	शून्य unset(ध्रुव कुञ्चिका_2 &k)
	{
		सचे (!backup_state.empty() && backup_state.back().गणनम्(k) == 0)
			backup_state.back()[k] = current_state.गणनम्(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ध्रुव T &get(ध्रुव कुञ्चिका_2 &k)
	{
		सचे (current_state.गणनम्(k) == 0)
			पटिनिवत्त empty_tuple;
		पटिनिवत्त current_state.at(k);
	}

	शून्य reset(ध्रुव कुञ्चिका_2 &k)
	{
		अत्थाय (पूर्णाङ्क i = GetSize(backup_state)-1; i >= 0; i--)
			सचे (backup_state[i].गणनम्(k) != 0) {
				सचे (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				अञ्ञथा
					current_state[k] = *backup_state[i].at(k);
				पटिनिवत्त;
			}
		current_state.erase(k);
	}

	ध्रुव dict<कुञ्चिका_2, T> &stdmap()
	{
		पटिनिवत्त current_state;
	}

	शून्य save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	शून्य restore()
	{
		log_assert(!backup_state.empty());
		अत्थाय (auto &it : backup_state.back())
			सचे (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} अञ्ञथा
				current_state.erase(it.first);
		backup_state.निष्कासय_back();
	}

	~stackmap()
	{
		याव (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::मानम् && std::is_same<C, std::less<T>>::मानम्),
			"std::less is run-to-run unstable for pointers");
सार्वजनिक:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	संरचना IndirectCmp {
		IndirectCmp(ध्रुव std::vector<T> &nodes, ध्रुव C &ग्रन्थिः_cmp) : ग्रन्थिः_cmp_2(ग्रन्थिः_cmp), nodes_(nodes) {}
		bool operator()(पूर्णाङ्क a, पूर्णाङ्क b) ध्रुव
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			पटिनिवत्त ग्रन्थिः_cmp_2(nodes_[a], nodes_[b]);
		}
		ध्रुव C ग्रन्थिः_cmp_2;
		ध्रुव std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, पूर्णाङ्क, C> ग्रन्थिः_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<पूर्णाङ्क, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(ध्रुव C &cmp) : ग्रन्थिः_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	पूर्णाङ्क ग्रन्थिः(T n)
	{
		auto rv = ग्रन्थिः_to_index.emplace(n, static_cast<पूर्णाङ्क>(nodes.size()));
		सचे (rv.second) {
			nodes.प्रक्षिप_back(n);
			edges.प्रक्षिप_back(std::set<पूर्णाङ्क, IndirectCmp>(indirect_cmp));
		}
		पटिनिवत्त rv.first->second;
	}

	शून्य edge(पूर्णाङ्क l_index, पूर्णाङ्क r_index) { edges[r_index].निवेशय(l_index); }

	शून्य edge(T वामः_2, T दक्षिणः_2) { edge(ग्रन्थिः(वामः_2), ग्रन्थिः(दक्षिणः_2)); }

	bool has_ग्रन्थिः(ध्रुव T &ग्रन्थिः) { पटिनिवत्त ग्रन्थिः_to_index.find(ग्रन्थिः) != ग्रन्थिः_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(ग्रन्थिः_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> ग्रन्थिः_is_sorted(edges.size(), false);
		std::vector<bool> ग्रन्थिः_is_on_राशिः(edges.size(), false);
		// Only used with analyze_loops
		std::vector<पूर्णाङ्क> राशिः;
		sorted.reserve(edges.size());

		अत्थाय (ध्रुव auto &it : ग्रन्थिः_to_index)
			sort_worker(it.second, ग्रन्थिः_is_sorted, ग्रन्थिः_is_on_राशिः, राशिः);

		log_assert(GetSize(sorted) == GetSize(nodes));

		पटिनिवत्त !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(ग्रन्थिः_to_index.कुञ्चिका_comp());
		अत्थाय (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(ग्रन्थिः_to_index.कुञ्चिका_comp());
			अत्थाय (पूर्णाङ्क other_ग्रन्थिः : edges[i]) {
				converted_edge_set.निवेशय(nodes[other_ग्रन्थिः]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		पटिनिवत्त database;
	}

गुप्त:
	bool found_loops;
	std::vector<T> nodes;
	ध्रुव IndirectCmp indirect_cmp;

	शून्य sort_worker(ध्रुव पूर्णाङ्क मूलम्_index, std::vector<bool> &ग्रन्थिः_is_sorted, std::vector<bool> &ग्रन्थिः_is_on_राशिः, std::vector<पूर्णाङ्क> &राशिः)
	{
		सचे (ग्रन्थिः_is_on_राशिः[मूलम्_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			सचे (analyze_loops) {
				std::vector<T> loop;
				अत्थाय (पूर्णाङ्क i = GetSize(राशिः) - 1; i >= 0; i--) {
					ध्रुव पूर्णाङ्क index = राशिः[i];
					loop.प्रक्षिप_back(nodes[index]);
					सचे (index == मूलम्_index)
						भङ्ग;
				}
				loops.निवेशय(loop);
			}
			पटिनिवत्त;
		}

		// We're done if we've already sorted this subgraph
		सचे (ग्रन्थिः_is_sorted[मूलम्_index])
			पटिनिवत्त;

		सचे (!edges[मूलम्_index].empty()) {
			सचे (analyze_loops)
				राशिः.प्रक्षिप_back(मूलम्_index);
			ग्रन्थिः_is_on_राशिः[मूलम्_index] = true;

			अत्थाय (पूर्णाङ्क वामः_n : edges[मूलम्_index])
				sort_worker(वामः_n, ग्रन्थिः_is_sorted, ग्रन्थिः_is_on_राशिः, राशिः);

			सचे (analyze_loops)
				राशिः.निष्कासय_back();
			ग्रन्थिः_is_on_राशिः[मूलम्_index] = false;
		}

		ग्रन्थिः_is_sorted[मूलम्_index] = true;
		sorted.प्रक्षिप_back(nodes[मूलम्_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
संरचना arrow_proxy {
	T v;
	explicit arrow_proxy(T ध्रुव & v) : v(v) {}
	T* operator->() { पटिनिवत्त &v; }
};

अन्तःस्थ पूर्णाङ्क ceil_log2(पूर्णाङ्क x)
{
#सचे defined(__GNUC__)
        पटिनिवत्त x > 1 ? (8*परिमाण(पूर्णाङ्क)) - __builtin_clz(x-1) : 0;
#अञ्ञथा
	सचे (x <= 0)
		पटिनिवत्त 0;
	अत्थाय (पूर्णाङ्क i = 0; i < 32; i++)
		सचे (((x-1) >> i) == 0)
			पटिनिवत्त i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	संरचना reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() ध्रुव { पटिनिवत्त container.rbegin(); }
		auto end() ध्रुव { पटिनिवत्त container.rend(); }
		T& container;
	};
	पटिनिवत्त reverse_view{container};
}

template <typename T>
auto reversed(ध्रुव T& container) {
	संरचना reverse_view {
		reverse_view(ध्रुव T& container) : container(container) {}
		auto begin() ध्रुव { पटिनिवत्त container.rbegin(); }
		auto end() ध्रुव { पटिनिवत्त container.rend(); }
		ध्रुव T& container;
	};
	पटिनिवत्त reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
संरचना IntRange {
	पूर्णाङ्क start_;
	पूर्णाङ्क end_;
	संरचना Int {
		पूर्णाङ्क v;
		पूर्णाङ्क operator*() ध्रुव { पटिनिवत्त v; }
		Int &operator++() { ++v; पटिनिवत्त *this; }
		bool operator!=(ध्रुव Int &other) ध्रुव { पटिनिवत्त v != other.v; }
	};
	Int begin() ध्रुव { पटिनिवत्त {start_}; }
	Int end() ध्रुव { पटिनिवत्त {end_}; }

	bool operator==(ध्रुव IntRange &other) ध्रुव { पटिनिवत्त start_ == other.start_ && end_ == other.end_; }
	bool operator!=(ध्रुव IntRange &other) ध्रुव { पटिनिवत्त !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
