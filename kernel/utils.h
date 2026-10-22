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

#समावेश "kernel/yosys.h"
#समावेश <iterator>

#अगर_अघोषित UTILS_H
#घोषणा UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

ढाँचा<typename कुंजी_2, typename T>
काष्ठा stackmap
{
निजी:
	std::vector<dict<कुंजी_2, T*>> backup_state;
	dict<कुंजी_2, T> current_state;
	अटल T empty_tuple;

खुला:
	stackmap() { }
	stackmap(स्थिर dict<कुंजी_2, T> &other) : current_state(other) { }

	ढाँचा<typename Other>
	व्योम चालक=(स्थिर Other &other)
	{
		क्रम (स्वतः &it : current_state)
			अगर (!backup_state.empty() && backup_state.back().गिनती(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		क्रम (स्वतः &it : other)
			set(it.first, it.second);
	}

	bool has(स्थिर कुंजी_2 &k)
	{
		वापस current_state.गिनती(k) != 0;
	}

	व्योम set(स्थिर कुंजी_2 &k, स्थिर T &v)
	{
		अगर (!backup_state.empty() && backup_state.back().गिनती(k) == 0)
			backup_state.back()[k] = current_state.गिनती(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	व्योम unset(स्थिर कुंजी_2 &k)
	{
		अगर (!backup_state.empty() && backup_state.back().गिनती(k) == 0)
			backup_state.back()[k] = current_state.गिनती(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	स्थिर T &get(स्थिर कुंजी_2 &k)
	{
		अगर (current_state.गिनती(k) == 0)
			वापस empty_tuple;
		वापस current_state.at(k);
	}

	व्योम reset(स्थिर कुंजी_2 &k)
	{
		क्रम (पूर्णांक i = Get_आकार(backup_state)-1; i >= 0; i--)
			अगर (backup_state[i].गिनती(k) != 0) {
				अगर (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				अन्यथा
					current_state[k] = *backup_state[i].at(k);
				वापस;
			}
		current_state.erase(k);
	}

	स्थिर dict<कुंजी_2, T> &stdmap()
	{
		वापस current_state;
	}

	व्योम save()
	{
		backup_state.resize(backup_state.आकार()+1);
	}

	व्योम restore()
	{
		log_assert(!backup_state.empty());
		क्रम (स्वतः &it : backup_state.back())
			अगर (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} अन्यथा
				current_state.erase(it.first);
		backup_state.निकालो_back();
	}

	~stackmap()
	{
		जबतक (!backup_state.empty())
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
खुला:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	काष्ठा IndirectCmp {
		IndirectCmp(स्थिर std::vector<T> &nodes, स्थिर C &गाँठ_cmp) : गाँठ_cmp_2(गाँठ_cmp), nodes_(nodes) {}
		bool चालक()(पूर्णांक a, पूर्णांक b) स्थिर
		{
			log_assert(static_cast<माप_प्रकार>(a) < nodes_.आकार());
			log_assert(static_cast<माप_प्रकार>(b) < nodes_.आकार());
			वापस गाँठ_cmp_2(nodes_[a], nodes_[b]);
		}
		स्थिर C गाँठ_cmp_2;
		स्थिर std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, पूर्णांक, C> गाँठ_to_सूचकांक;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<पूर्णांक, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(स्थिर C &cmp) : गाँठ_to_सूचकांक(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	पूर्णांक गाँठ(T n)
	{
		स्वतः rv = गाँठ_to_सूचकांक.emplace(n, static_cast<पूर्णांक>(nodes.आकार()));
		अगर (rv.second) {
			nodes.धकेलो_back(n);
			edges.धकेलो_back(std::set<पूर्णांक, IndirectCmp>(indirect_cmp));
		}
		वापस rv.first->second;
	}

	व्योम edge(पूर्णांक l_सूचकांक, पूर्णांक r_सूचकांक) { edges[r_सूचकांक].जोड़ो(l_सूचकांक); }

	व्योम edge(T बायाँ_2, T दायाँ_2) { edge(गाँठ(बायाँ_2), गाँठ(दायाँ_2)); }

	bool has_गाँठ(स्थिर T &गाँठ) { वापस गाँठ_to_सूचकांक.find(गाँठ) != गाँठ_to_सूचकांक.end(); }

	bool sort()
	{
		log_assert(Get_आकार(गाँठ_to_सूचकांक) == Get_आकार(edges));
		log_assert(Get_आकार(nodes) == Get_आकार(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> गाँठ_is_sorted(edges.आकार(), false);
		std::vector<bool> गाँठ_is_on_ढेर(edges.आकार(), false);
		// Only used with analyze_loops
		std::vector<पूर्णांक> ढेर;
		sorted.reserve(edges.आकार());

		क्रम (स्थिर स्वतः &it : गाँठ_to_सूचकांक)
			sort_worker(it.second, गाँठ_is_sorted, गाँठ_is_on_ढेर, ढेर);

		log_assert(Get_आकार(sorted) == Get_आकार(nodes));

		वापस !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(गाँठ_to_सूचकांक.कुंजी_comp());
		क्रम (माप_प्रकार i = 0; i < nodes.आकार(); ++i) {
			std::set<T, C> converted_edge_set(गाँठ_to_सूचकांक.कुंजी_comp());
			क्रम (पूर्णांक other_गाँठ : edges[i]) {
				converted_edge_set.जोड़ो(nodes[other_गाँठ]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		वापस database;
	}

निजी:
	bool found_loops;
	std::vector<T> nodes;
	स्थिर IndirectCmp indirect_cmp;

	व्योम sort_worker(स्थिर पूर्णांक मूल_सूचकांक, std::vector<bool> &गाँठ_is_sorted, std::vector<bool> &गाँठ_is_on_ढेर, std::vector<पूर्णांक> &ढेर)
	{
		अगर (गाँठ_is_on_ढेर[मूल_सूचकांक]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			अगर (analyze_loops) {
				std::vector<T> loop;
				क्रम (पूर्णांक i = Get_आकार(ढेर) - 1; i >= 0; i--) {
					स्थिर पूर्णांक सूचकांक = ढेर[i];
					loop.धकेलो_back(nodes[सूचकांक]);
					अगर (सूचकांक == मूल_सूचकांक)
						अवरोध;
				}
				loops.जोड़ो(loop);
			}
			वापस;
		}

		// We're done if we've already sorted this subgraph
		अगर (गाँठ_is_sorted[मूल_सूचकांक])
			वापस;

		अगर (!edges[मूल_सूचकांक].empty()) {
			अगर (analyze_loops)
				ढेर.धकेलो_back(मूल_सूचकांक);
			गाँठ_is_on_ढेर[मूल_सूचकांक] = true;

			क्रम (पूर्णांक बायाँ_n : edges[मूल_सूचकांक])
				sort_worker(बायाँ_n, गाँठ_is_sorted, गाँठ_is_on_ढेर, ढेर);

			अगर (analyze_loops)
				ढेर.निकालो_back();
			गाँठ_is_on_ढेर[मूल_सूचकांक] = false;
		}

		गाँठ_is_sorted[मूल_सूचकांक] = true;
		sorted.धकेलो_back(nodes[मूल_सूचकांक]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
ढाँचा<class T>
काष्ठा arrow_proxy {
	T v;
	explicit arrow_proxy(T स्थिर & v) : v(v) {}
	T* चालक->() { वापस &v; }
};

अंतरभूत पूर्णांक ceil_log2(पूर्णांक x)
{
#अगर defined(__GNUC__)
        वापस x > 1 ? (8*माप(पूर्णांक)) - __builtin_clz(x-1) : 0;
#अन्यथा
	अगर (x <= 0)
		वापस 0;
	क्रम (पूर्णांक i = 0; i < 32; i++)
		अगर (((x-1) >> i) == 0)
			वापस i;
	log_abort();
#पूर्ण_अगर
}

ढाँचा <typename T>
स्वतः reversed(T& container) {
	काष्ठा reverse_view {
		reverse_view(T& container) : container(container) {}
		स्वतः begin() स्थिर { वापस container.rbegin(); }
		स्वतः end() स्थिर { वापस container.rend(); }
		T& container;
	};
	वापस reverse_view{container};
}

ढाँचा <typename T>
स्वतः reversed(स्थिर T& container) {
	काष्ठा reverse_view {
		reverse_view(स्थिर T& container) : container(container) {}
		स्वतः begin() स्थिर { वापस container.rbegin(); }
		स्वतः end() स्थिर { वापस container.rend(); }
		स्थिर T& container;
	};
	वापस reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
काष्ठा IntRange {
	पूर्णांक start_;
	पूर्णांक end_;
	काष्ठा Int {
		पूर्णांक v;
		पूर्णांक चालक*() स्थिर { वापस v; }
		Int &चालक++() { ++v; वापस *this; }
		bool चालक!=(स्थिर Int &other) स्थिर { वापस v != other.v; }
	};
	Int begin() स्थिर { वापस {start_}; }
	Int end() स्थिर { वापस {end_}; }

	bool चालक==(स्थिर IntRange &other) स्थिर { वापस start_ == other.start_ && end_ == other.end_; }
	bool चालक!=(स्थिर IntRange &other) स्थिर { वापस !(*this == other); }
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
