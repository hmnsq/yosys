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

ढाँचा<typename किल्ली_2, typename T>
संरचना stackmap
{
खाजगी:
	std::vector<dict<किल्ली_2, T*>> backup_state;
	dict<किल्ली_2, T> current_state;
	स्थिर T empty_tuple;

सार्वजनिक:
	stackmap() { }
	stackmap(स्थिरांक dict<किल्ली_2, T> &other) : current_state(other) { }

	ढाँचा<typename Other>
	शून्य चालक=(स्थिरांक Other &other)
	{
		साठी (स्वतः &it : current_state)
			जर (!backup_state.empty() && backup_state.back().मोजणी(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		साठी (स्वतः &it : other)
			set(it.first, it.second);
	}

	bool has(स्थिरांक किल्ली_2 &k)
	{
		परत current_state.मोजणी(k) != 0;
	}

	शून्य set(स्थिरांक किल्ली_2 &k, स्थिरांक T &v)
	{
		जर (!backup_state.empty() && backup_state.back().मोजणी(k) == 0)
			backup_state.back()[k] = current_state.मोजणी(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	शून्य unset(स्थिरांक किल्ली_2 &k)
	{
		जर (!backup_state.empty() && backup_state.back().मोजणी(k) == 0)
			backup_state.back()[k] = current_state.मोजणी(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	स्थिरांक T &get(स्थिरांक किल्ली_2 &k)
	{
		जर (current_state.मोजणी(k) == 0)
			परत empty_tuple;
		परत current_state.at(k);
	}

	शून्य reset(स्थिरांक किल्ली_2 &k)
	{
		साठी (पूर्णांक i = GetSize(backup_state)-1; i >= 0; i--)
			जर (backup_state[i].मोजणी(k) != 0) {
				जर (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				अन्यथा
					current_state[k] = *backup_state[i].at(k);
				परत;
			}
		current_state.erase(k);
	}

	स्थिरांक dict<किल्ली_2, T> &stdmap()
	{
		परत current_state;
	}

	शून्य save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	शून्य restore()
	{
		log_assert(!backup_state.empty());
		साठी (स्वतः &it : backup_state.back())
			जर (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} अन्यथा
				current_state.erase(it.first);
		backup_state.काढा_back();
	}

	~stackmap()
	{
		पर्यंत (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

ढाँचा <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::मूल्य && std::is_same<C, std::less<T>>::मूल्य),
			"std::less is run-to-run unstable for pointers");
सार्वजनिक:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	संरचना IndirectCmp {
		IndirectCmp(स्थिरांक std::vector<T> &nodes, स्थिरांक C &गाठ_cmp) : गाठ_cmp_2(गाठ_cmp), nodes_(nodes) {}
		bool चालक()(पूर्णांक a, पूर्णांक b) स्थिरांक
		{
			log_assert(static_cast<माप_प्रकार>(a) < nodes_.size());
			log_assert(static_cast<माप_प्रकार>(b) < nodes_.size());
			परत गाठ_cmp_2(nodes_[a], nodes_[b]);
		}
		स्थिरांक C गाठ_cmp_2;
		स्थिरांक std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, पूर्णांक, C> गाठ_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<पूर्णांक, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(स्थिरांक C &cmp) : गाठ_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	पूर्णांक गाठ(T n)
	{
		स्वतः rv = गाठ_to_index.emplace(n, static_cast<पूर्णांक>(nodes.size()));
		जर (rv.second) {
			nodes.ढकला_back(n);
			edges.ढकला_back(std::set<पूर्णांक, IndirectCmp>(indirect_cmp));
		}
		परत rv.first->second;
	}

	शून्य edge(पूर्णांक l_index, पूर्णांक r_index) { edges[r_index].घाला(l_index); }

	शून्य edge(T डावा_2, T उजवा_2) { edge(गाठ(डावा_2), गाठ(उजवा_2)); }

	bool has_गाठ(स्थिरांक T &गाठ) { परत गाठ_to_index.find(गाठ) != गाठ_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(गाठ_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> गाठ_is_sorted(edges.size(), false);
		std::vector<bool> गाठ_is_on_चळत(edges.size(), false);
		// Only used with analyze_loops
		std::vector<पूर्णांक> चळत;
		sorted.reserve(edges.size());

		साठी (स्थिरांक स्वतः &it : गाठ_to_index)
			sort_worker(it.second, गाठ_is_sorted, गाठ_is_on_चळत, चळत);

		log_assert(GetSize(sorted) == GetSize(nodes));

		परत !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(गाठ_to_index.किल्ली_comp());
		साठी (माप_प्रकार i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(गाठ_to_index.किल्ली_comp());
			साठी (पूर्णांक other_गाठ : edges[i]) {
				converted_edge_set.घाला(nodes[other_गाठ]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		परत database;
	}

खाजगी:
	bool found_loops;
	std::vector<T> nodes;
	स्थिरांक IndirectCmp indirect_cmp;

	शून्य sort_worker(स्थिरांक पूर्णांक मूळ_index, std::vector<bool> &गाठ_is_sorted, std::vector<bool> &गाठ_is_on_चळत, std::vector<पूर्णांक> &चळत)
	{
		जर (गाठ_is_on_चळत[मूळ_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			जर (analyze_loops) {
				std::vector<T> loop;
				साठी (पूर्णांक i = GetSize(चळत) - 1; i >= 0; i--) {
					स्थिरांक पूर्णांक index = चळत[i];
					loop.ढकला_back(nodes[index]);
					जर (index == मूळ_index)
						थांबा;
				}
				loops.घाला(loop);
			}
			परत;
		}

		// We're done if we've already sorted this subgraph
		जर (गाठ_is_sorted[मूळ_index])
			परत;

		जर (!edges[मूळ_index].empty()) {
			जर (analyze_loops)
				चळत.ढकला_back(मूळ_index);
			गाठ_is_on_चळत[मूळ_index] = true;

			साठी (पूर्णांक डावा_n : edges[मूळ_index])
				sort_worker(डावा_n, गाठ_is_sorted, गाठ_is_on_चळत, चळत);

			जर (analyze_loops)
				चळत.काढा_back();
			गाठ_is_on_चळत[मूळ_index] = false;
		}

		गाठ_is_sorted[मूळ_index] = true;
		sorted.ढकला_back(nodes[मूळ_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
ढाँचा<class T>
संरचना arrow_proxy {
	T v;
	explicit arrow_proxy(T स्थिरांक & v) : v(v) {}
	T* चालक->() { परत &v; }
};

ओळीत पूर्णांक ceil_log2(पूर्णांक x)
{
#जर defined(__GNUC__)
        परत x > 1 ? (8*आकार(पूर्णांक)) - __builtin_clz(x-1) : 0;
#अन्यथा
	जर (x <= 0)
		परत 0;
	साठी (पूर्णांक i = 0; i < 32; i++)
		जर (((x-1) >> i) == 0)
			परत i;
	log_abort();
#endif
}

ढाँचा <typename T>
स्वतः reversed(T& container) {
	संरचना reverse_view {
		reverse_view(T& container) : container(container) {}
		स्वतः begin() स्थिरांक { परत container.rbegin(); }
		स्वतः end() स्थिरांक { परत container.rend(); }
		T& container;
	};
	परत reverse_view{container};
}

ढाँचा <typename T>
स्वतः reversed(स्थिरांक T& container) {
	संरचना reverse_view {
		reverse_view(स्थिरांक T& container) : container(container) {}
		स्वतः begin() स्थिरांक { परत container.rbegin(); }
		स्वतः end() स्थिरांक { परत container.rend(); }
		स्थिरांक T& container;
	};
	परत reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
संरचना IntRange {
	पूर्णांक start_;
	पूर्णांक end_;
	संरचना Int {
		पूर्णांक v;
		पूर्णांक चालक*() स्थिरांक { परत v; }
		Int &चालक++() { ++v; परत *this; }
		bool चालक!=(स्थिरांक Int &other) स्थिरांक { परत v != other.v; }
	};
	Int begin() स्थिरांक { परत {start_}; }
	Int end() स्थिरांक { परत {end_}; }

	bool चालक==(स्थिरांक IntRange &other) स्थिरांक { परत start_ == other.start_ && end_ == other.end_; }
	bool चालक!=(स्थिरांक IntRange &other) स्थिरांक { परत !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
