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

ढाँचा<typename साँचो_2, typename T>
संरचना stackmap
{
गुप्त:
	std::vector<dict<साँचो_2, T*>> backup_state;
	dict<साँचो_2, T> current_state;
	स्थिर T empty_tuple;

सार्वजनिक:
	stackmap() { }
	stackmap(अचर dict<साँचो_2, T> &other) : current_state(other) { }

	ढाँचा<typename Other>
	शून्य चालक=(अचर Other &other)
	{
		लागि (स्वतः &it : current_state)
			यदि (!backup_state.empty() && backup_state.back().गन्ती(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		लागि (स्वतः &it : other)
			set(it.first, it.second);
	}

	bool has(अचर साँचो_2 &k)
	{
		फर्काऊ current_state.गन्ती(k) != 0;
	}

	शून्य set(अचर साँचो_2 &k, अचर T &v)
	{
		यदि (!backup_state.empty() && backup_state.back().गन्ती(k) == 0)
			backup_state.back()[k] = current_state.गन्ती(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	शून्य unset(अचर साँचो_2 &k)
	{
		यदि (!backup_state.empty() && backup_state.back().गन्ती(k) == 0)
			backup_state.back()[k] = current_state.गन्ती(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	अचर T &get(अचर साँचो_2 &k)
	{
		यदि (current_state.गन्ती(k) == 0)
			फर्काऊ empty_tuple;
		फर्काऊ current_state.at(k);
	}

	शून्य reset(अचर साँचो_2 &k)
	{
		लागि (पूर्णाङ्क i = GetSize(backup_state)-1; i >= 0; i--)
			यदि (backup_state[i].गन्ती(k) != 0) {
				यदि (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				अन्यथा
					current_state[k] = *backup_state[i].at(k);
				फर्काऊ;
			}
		current_state.erase(k);
	}

	अचर dict<साँचो_2, T> &stdmap()
	{
		फर्काऊ current_state;
	}

	शून्य save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	शून्य restore()
	{
		log_assert(!backup_state.empty());
		लागि (स्वतः &it : backup_state.back())
			यदि (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} अन्यथा
				current_state.erase(it.first);
		backup_state.झिक_back();
	}

	~stackmap()
	{
		जबसम्म (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

ढाँचा <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::मान && std::is_same<C, std::less<T>>::मान),
			"std::less is run-to-run unstable for pointers");
सार्वजनिक:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	संरचना IndirectCmp {
		IndirectCmp(अचर std::vector<T> &nodes, अचर C &गाँठो_cmp) : गाँठो_cmp_2(गाँठो_cmp), nodes_(nodes) {}
		bool चालक()(पूर्णाङ्क a, पूर्णाङ्क b) अचर
		{
			log_assert(static_cast<माप_प्रकार>(a) < nodes_.size());
			log_assert(static_cast<माप_प्रकार>(b) < nodes_.size());
			फर्काऊ गाँठो_cmp_2(nodes_[a], nodes_[b]);
		}
		अचर C गाँठो_cmp_2;
		अचर std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, पूर्णाङ्क, C> गाँठो_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<पूर्णाङ्क, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(अचर C &cmp) : गाँठो_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	पूर्णाङ्क गाँठो(T n)
	{
		स्वतः rv = गाँठो_to_index.emplace(n, static_cast<पूर्णाङ्क>(nodes.size()));
		यदि (rv.second) {
			nodes.थप_back(n);
			edges.थप_back(std::set<पूर्णाङ्क, IndirectCmp>(indirect_cmp));
		}
		फर्काऊ rv.first->second;
	}

	शून्य edge(पूर्णाङ्क l_index, पूर्णाङ्क r_index) { edges[r_index].घुसाउ(l_index); }

	शून्य edge(T बायाँ_2, T दायाँ_2) { edge(गाँठो(बायाँ_2), गाँठो(दायाँ_2)); }

	bool has_गाँठो(अचर T &गाँठो) { फर्काऊ गाँठो_to_index.find(गाँठो) != गाँठो_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(गाँठो_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> गाँठो_is_sorted(edges.size(), false);
		std::vector<bool> गाँठो_is_on_थाक(edges.size(), false);
		// Only used with analyze_loops
		std::vector<पूर्णाङ्क> थाक;
		sorted.reserve(edges.size());

		लागि (अचर स्वतः &it : गाँठो_to_index)
			sort_worker(it.second, गाँठो_is_sorted, गाँठो_is_on_थाक, थाक);

		log_assert(GetSize(sorted) == GetSize(nodes));

		फर्काऊ !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(गाँठो_to_index.साँचो_comp());
		लागि (माप_प्रकार i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(गाँठो_to_index.साँचो_comp());
			लागि (पूर्णाङ्क other_गाँठो : edges[i]) {
				converted_edge_set.घुसाउ(nodes[other_गाँठो]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		फर्काऊ database;
	}

गुप्त:
	bool found_loops;
	std::vector<T> nodes;
	अचर IndirectCmp indirect_cmp;

	शून्य sort_worker(अचर पूर्णाङ्क जरा_index, std::vector<bool> &गाँठो_is_sorted, std::vector<bool> &गाँठो_is_on_थाक, std::vector<पूर्णाङ्क> &थाक)
	{
		यदि (गाँठो_is_on_थाक[जरा_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			यदि (analyze_loops) {
				std::vector<T> loop;
				लागि (पूर्णाङ्क i = GetSize(थाक) - 1; i >= 0; i--) {
					अचर पूर्णाङ्क index = थाक[i];
					loop.थप_back(nodes[index]);
					यदि (index == जरा_index)
						रोक;
				}
				loops.घुसाउ(loop);
			}
			फर्काऊ;
		}

		// We're done if we've already sorted this subgraph
		यदि (गाँठो_is_sorted[जरा_index])
			फर्काऊ;

		यदि (!edges[जरा_index].empty()) {
			यदि (analyze_loops)
				थाक.थप_back(जरा_index);
			गाँठो_is_on_थाक[जरा_index] = true;

			लागि (पूर्णाङ्क बायाँ_n : edges[जरा_index])
				sort_worker(बायाँ_n, गाँठो_is_sorted, गाँठो_is_on_थाक, थाक);

			यदि (analyze_loops)
				थाक.झिक_back();
			गाँठो_is_on_थाक[जरा_index] = false;
		}

		गाँठो_is_sorted[जरा_index] = true;
		sorted.थप_back(nodes[जरा_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
ढाँचा<class T>
संरचना arrow_proxy {
	T v;
	explicit arrow_proxy(T अचर & v) : v(v) {}
	T* चालक->() { फर्काऊ &v; }
};

अन्तःस्थ पूर्णाङ्क ceil_log2(पूर्णाङ्क x)
{
#यदि defined(__GNUC__)
        फर्काऊ x > 1 ? (8*परिमाण(पूर्णाङ्क)) - __builtin_clz(x-1) : 0;
#अन्यथा
	यदि (x <= 0)
		फर्काऊ 0;
	लागि (पूर्णाङ्क i = 0; i < 32; i++)
		यदि (((x-1) >> i) == 0)
			फर्काऊ i;
	log_abort();
#endif
}

ढाँचा <typename T>
स्वतः reversed(T& container) {
	संरचना reverse_view {
		reverse_view(T& container) : container(container) {}
		स्वतः begin() अचर { फर्काऊ container.rbegin(); }
		स्वतः end() अचर { फर्काऊ container.rend(); }
		T& container;
	};
	फर्काऊ reverse_view{container};
}

ढाँचा <typename T>
स्वतः reversed(अचर T& container) {
	संरचना reverse_view {
		reverse_view(अचर T& container) : container(container) {}
		स्वतः begin() अचर { फर्काऊ container.rbegin(); }
		स्वतः end() अचर { फर्काऊ container.rend(); }
		अचर T& container;
	};
	फर्काऊ reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
संरचना IntRange {
	पूर्णाङ्क start_;
	पूर्णाङ्क end_;
	संरचना Int {
		पूर्णाङ्क v;
		पूर्णाङ्क चालक*() अचर { फर्काऊ v; }
		Int &चालक++() { ++v; फर्काऊ *this; }
		bool चालक!=(अचर Int &other) अचर { फर्काऊ v != other.v; }
	};
	Int begin() अचर { फर्काऊ {start_}; }
	Int end() अचर { फर्काऊ {end_}; }

	bool चालक==(अचर IntRange &other) अचर { फर्काऊ start_ == other.start_ && end_ == other.end_; }
	bool चालक!=(अचर IntRange &other) अचर { फर्काऊ !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
