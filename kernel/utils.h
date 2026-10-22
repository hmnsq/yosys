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

#यदि_अपरिभाषितम् UTILS_H
#परिभाषय UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

प्रतिरूपम्<typename कुञ्चिका_2, typename T>
संरचना stackmap
{
गुप्त:
	std::सदिशम्<dict<कुञ्चिका_2, T*>> backup_state;
	dict<कुञ्चिका_2, T> current_state;
	स्थिर T रिक्तानि_tuple;

सार्वजनिक:
	stackmap() { }
	stackmap(ध्रुव dict<कुञ्चिका_2, T> &other) : current_state(other) { }

	प्रतिरूपम्<typename Other>
	शून्य संक्रियकः=(ध्रुव Other &other)
	{
		प्रति (स्वचालित &it : current_state)
			यदि (!backup_state.रिक्तानि() && backup_state.पृष्ठभागः().गणनम्(it.first) == 0)
				backup_state.पृष्ठभागः()[it.first] = new T(it.second);
		current_state.clear();

		प्रति (स्वचालित &it : other)
			समुच्चयः(it.first, it.second);
	}

	bool has(ध्रुव कुञ्चिका_2 &k)
	{
		प्रत्यागम current_state.गणनम्(k) != 0;
	}

	शून्य समुच्चयः(ध्रुव कुञ्चिका_2 &k, ध्रुव T &v)
	{
		यदि (!backup_state.रिक्तानि() && backup_state.पृष्ठभागः().गणनम्(k) == 0)
			backup_state.पृष्ठभागः()[k] = current_state.गणनम्(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	शून्य unset(ध्रुव कुञ्चिका_2 &k)
	{
		यदि (!backup_state.रिक्तानि() && backup_state.पृष्ठभागः().गणनम्(k) == 0)
			backup_state.पृष्ठभागः()[k] = current_state.गणनम्(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ध्रुव T &get(ध्रुव कुञ्चिका_2 &k)
	{
		यदि (current_state.गणनम्(k) == 0)
			प्रत्यागम रिक्तानि_tuple;
		प्रत्यागम current_state.at(k);
	}

	शून्य reset(ध्रुव कुञ्चिका_2 &k)
	{
		प्रति (पूर्णाङ्क i = Get_आकार(backup_state)-1; i >= 0; i--)
			यदि (backup_state[i].गणनम्(k) != 0) {
				यदि (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				अन्यथा
					current_state[k] = *backup_state[i].at(k);
				प्रत्यागम;
			}
		current_state.erase(k);
	}

	ध्रुव dict<कुञ्चिका_2, T> &stdmap()
	{
		प्रत्यागम current_state;
	}

	शून्य save()
	{
		backup_state.resize(backup_state.आकार()+1);
	}

	शून्य restore()
	{
		log_assert(!backup_state.रिक्तानि());
		प्रति (स्वचालित &it : backup_state.पृष्ठभागः())
			यदि (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} अन्यथा
				current_state.erase(it.first);
		backup_state.निष्कासय_पृष्ठभागः();
	}

	~stackmap()
	{
		यावत् (!backup_state.रिक्तानि())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

प्रतिरूपम् <typename T, typename C = std::less<T>> class Topo_क्रमय
{
	static_assert(!(std::is_pointer<T>::मानम् && std::is_same<C, std::less<T>>::मानम्),
			"std::less is run-to-run unstable for pointers");
सार्वजनिक:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	संरचना परोक्षम्_Cmp {
		परोक्षम्_Cmp(ध्रुव std::सदिशम्<T> &nodes, ध्रुव C &ग्रन्थिः_cmp) : ग्रन्थिः_cmp_2(ग्रन्थिः_cmp), nodes_(nodes) {}
		bool संक्रियकः()(पूर्णाङ्क a, पूर्णाङ्क b) ध्रुव
		{
			log_assert(static_cast<आकार_t>(a) < nodes_.आकार());
			log_assert(static_cast<आकार_t>(b) < nodes_.आकार());
			प्रत्यागम ग्रन्थिः_cmp_2(nodes_[a], nodes_[b]);
		}
		ध्रुव C ग्रन्थिः_cmp_2;
		ध्रुव std::सदिशम्<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, पूर्णाङ्क, C> ग्रन्थिः_गन्तव्यम्_सूचकांक;
	// edges[i] is the set of nodes with an edge into node i
	std::सदिशम्<std::समुच्चयः<पूर्णाङ्क, परोक्षम्_Cmp>> धाराः;
	std::सदिशम्<T> sorted;
	std::समुच्चयः<std::सदिशम्<T>> loops;

	Topo_क्रमय() requires std::is_default_constructible_v<C> : Topo_क्रमय(C()) {}
	explicit Topo_क्रमय(ध्रुव C &cmp) : ग्रन्थिः_गन्तव्यम्_सूचकांक(cmp), परोक्षम्_cmp(nodes, cmp)
	{
		analyze_loops = true;
		प्राप्तम्_loops = false;
	}

	पूर्णाङ्क ग्रन्थिः(T n)
	{
		स्वचालित rv = ग्रन्थिः_गन्तव्यम्_सूचकांक.emplace(n, static_cast<पूर्णाङ्क>(nodes.आकार()));
		यदि (rv.second) {
			nodes.प्रक्षिप_पृष्ठभागः(n);
			धाराः.प्रक्षिप_पृष्ठभागः(std::समुच्चयः<पूर्णाङ्क, परोक्षम्_Cmp>(परोक्षम्_cmp));
		}
		प्रत्यागम rv.first->second;
	}

	शून्य edge(पूर्णाङ्क l_सूचकांक, पूर्णाङ्क r_सूचकांक) { धाराः[r_सूचकांक].निवेशय(l_सूचकांक); }

	शून्य edge(T वामः_2, T दक्षिणः_2) { edge(ग्रन्थिः(वामः_2), ग्रन्थिः(दक्षिणः_2)); }

	bool has_ग्रन्थिः(ध्रुव T &ग्रन्थिः) { प्रत्यागम ग्रन्थिः_गन्तव्यम्_सूचकांक.find(ग्रन्थिः) != ग्रन्थिः_गन्तव्यम्_सूचकांक.अन्तः(); }

	bool क्रमय()
	{
		log_assert(Get_आकार(ग्रन्थिः_गन्तव्यम्_सूचकांक) == Get_आकार(धाराः));
		log_assert(Get_आकार(nodes) == Get_आकार(धाराः));

		loops.clear();
		sorted.clear();
		प्राप्तम्_loops = false;

		std::सदिशम्<bool> ग्रन्थिः_is_sorted(धाराः.आकार(), false);
		std::सदिशम्<bool> ग्रन्थिः_is_on_राशिः(धाराः.आकार(), false);
		// Only used with analyze_loops
		std::सदिशम्<पूर्णाङ्क> राशिः;
		sorted.reserve(धाराः.आकार());

		प्रति (ध्रुव स्वचालित &it : ग्रन्थिः_गन्तव्यम्_सूचकांक)
			क्रमय_कार्यकर्ता(it.second, ग्रन्थिः_is_sorted, ग्रन्थिः_is_on_राशिः, राशिः);

		log_assert(Get_आकार(sorted) == Get_आकार(nodes));

		प्रत्यागम !प्राप्तम्_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::समुच्चयः<T, C>, C> get_database()
	{
		std::map<T, std::समुच्चयः<T, C>, C> database(ग्रन्थिः_गन्तव्यम्_सूचकांक.कुञ्चिका_comp());
		प्रति (आकार_t i = 0; i < nodes.आकार(); ++i) {
			std::समुच्चयः<T, C> converted_edge_समुच्चयः(ग्रन्थिः_गन्तव्यम्_सूचकांक.कुञ्चिका_comp());
			प्रति (पूर्णाङ्क other_ग्रन्थिः : धाराः[i]) {
				converted_edge_समुच्चयः.निवेशय(nodes[other_ग्रन्थिः]);
			}
			database.emplace(nodes[i], converted_edge_समुच्चयः);
		}
		प्रत्यागम database;
	}

गुप्त:
	bool प्राप्तम्_loops;
	std::सदिशम्<T> nodes;
	ध्रुव परोक्षम्_Cmp परोक्षम्_cmp;

	शून्य क्रमय_कार्यकर्ता(ध्रुव पूर्णाङ्क मूलम्_सूचकांक, std::सदिशम्<bool> &ग्रन्थिः_is_sorted, std::सदिशम्<bool> &ग्रन्थिः_is_on_राशिः, std::सदिशम्<पूर्णाङ्क> &राशिः)
	{
		यदि (ग्रन्थिः_is_on_राशिः[मूलम्_सूचकांक]) {
			// We've been here before, meaning we have a loop
			प्राप्तम्_loops = true;
			यदि (analyze_loops) {
				std::सदिशम्<T> loop;
				प्रति (पूर्णाङ्क i = Get_आकार(राशिः) - 1; i >= 0; i--) {
					ध्रुव पूर्णाङ्क सूचकांक = राशिः[i];
					loop.प्रक्षिप_पृष्ठभागः(nodes[सूचकांक]);
					यदि (सूचकांक == मूलम्_सूचकांक)
						भङ्ग;
				}
				loops.निवेशय(loop);
			}
			प्रत्यागम;
		}

		// We're done if we've already sorted this subgraph
		यदि (ग्रन्थिः_is_sorted[मूलम्_सूचकांक])
			प्रत्यागम;

		यदि (!धाराः[मूलम्_सूचकांक].रिक्तानि()) {
			यदि (analyze_loops)
				राशिः.प्रक्षिप_पृष्ठभागः(मूलम्_सूचकांक);
			ग्रन्थिः_is_on_राशिः[मूलम्_सूचकांक] = true;

			प्रति (पूर्णाङ्क वामः_n : धाराः[मूलम्_सूचकांक])
				क्रमय_कार्यकर्ता(वामः_n, ग्रन्थिः_is_sorted, ग्रन्थिः_is_on_राशिः, राशिः);

			यदि (analyze_loops)
				राशिः.निष्कासय_पृष्ठभागः();
			ग्रन्थिः_is_on_राशिः[मूलम्_सूचकांक] = false;
		}

		ग्रन्थिः_is_sorted[मूलम्_सूचकांक] = true;
		sorted.प्रक्षिप_पृष्ठभागः(nodes[मूलम्_सूचकांक]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
प्रतिरूपम्<class T>
संरचना arrow_proxy {
	T v;
	explicit arrow_proxy(T ध्रुव & v) : v(v) {}
	T* संक्रियकः->() { प्रत्यागम &v; }
};

अन्तःस्थ पूर्णाङ्क ceil_log2(पूर्णाङ्क x)
{
#यदि defined(__GNUC__)
        प्रत्यागम x > 1 ? (8*परिमाण(पूर्णाङ्क)) - __builtin_clz(x-1) : 0;
#अन्यथा
	यदि (x <= 0)
		प्रत्यागम 0;
	प्रति (पूर्णाङ्क i = 0; i < 32; i++)
		यदि (((x-1) >> i) == 0)
			प्रत्यागम i;
	log_abort();
#यदि_अन्तः
}

प्रतिरूपम् <typename T>
स्वचालित reversed(T& container) {
	संरचना reverse_view {
		reverse_view(T& container) : container(container) {}
		स्वचालित begin() ध्रुव { प्रत्यागम container.rbegin(); }
		स्वचालित अन्तः() ध्रुव { प्रत्यागम container.rend(); }
		T& container;
	};
	प्रत्यागम reverse_view{container};
}

प्रतिरूपम् <typename T>
स्वचालित reversed(ध्रुव T& container) {
	संरचना reverse_view {
		reverse_view(ध्रुव T& container) : container(container) {}
		स्वचालित begin() ध्रुव { प्रत्यागम container.rbegin(); }
		स्वचालित अन्तः() ध्रुव { प्रत्यागम container.rend(); }
		ध्रुव T& container;
	};
	प्रत्यागम reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
संरचना IntRange {
	पूर्णाङ्क start_;
	पूर्णाङ्क अन्तः_4;
	संरचना Int {
		पूर्णाङ्क v;
		पूर्णाङ्क संक्रियकः*() ध्रुव { प्रत्यागम v; }
		Int &संक्रियकः++() { ++v; प्रत्यागम *this; }
		bool संक्रियकः!=(ध्रुव Int &other) ध्रुव { प्रत्यागम v != other.v; }
	};
	Int begin() ध्रुव { प्रत्यागम {start_}; }
	Int अन्तः() ध्रुव { प्रत्यागम {अन्तः_4}; }

	bool संक्रियकः==(ध्रुव IntRange &other) ध्रुव { प्रत्यागम start_ == other.start_ && अन्तः_4 == other.अन्तः_4; }
	bool संक्रियकः!=(ध्रुव IntRange &other) ध्रुव { प्रत्यागम !(*this == other); }
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
