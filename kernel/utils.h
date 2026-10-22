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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ <iterator>

#ifndef UTILS_H
#define UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

template<typename താക്കോൽ_2, typename T>
ഘടന stackmap
{
സ്വകാര്യ:
	std::vector<dict<താക്കോൽ_2, T*>> backup_state;
	dict<താക്കോൽ_2, T> current_state;
	സ്ഥിതം T empty_tuple;

പൊതു:
	stackmap() { }
	stackmap(സ്ഥിരം dict<താക്കോൽ_2, T> &other) : current_state(other) { }

	template<typename Other>
	ശൂന്യം operator=(സ്ഥിരം Other &other)
	{
		വേണ്ടി (auto &it : current_state)
			എങ്കിൽ (!backup_state.empty() && backup_state.back().എണ്ണം(it.first) == 0)
				backup_state.back()[it.first] = new T(it.second);
		current_state.clear();

		വേണ്ടി (auto &it : other)
			set(it.first, it.second);
	}

	bool has(സ്ഥിരം താക്കോൽ_2 &k)
	{
		തിരികെ current_state.എണ്ണം(k) != 0;
	}

	ശൂന്യം set(സ്ഥിരം താക്കോൽ_2 &k, സ്ഥിരം T &v)
	{
		എങ്കിൽ (!backup_state.empty() && backup_state.back().എണ്ണം(k) == 0)
			backup_state.back()[k] = current_state.എണ്ണം(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	ശൂന്യം unset(സ്ഥിരം താക്കോൽ_2 &k)
	{
		എങ്കിൽ (!backup_state.empty() && backup_state.back().എണ്ണം(k) == 0)
			backup_state.back()[k] = current_state.എണ്ണം(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	സ്ഥിരം T &get(സ്ഥിരം താക്കോൽ_2 &k)
	{
		എങ്കിൽ (current_state.എണ്ണം(k) == 0)
			തിരികെ empty_tuple;
		തിരികെ current_state.at(k);
	}

	ശൂന്യം reset(സ്ഥിരം താക്കോൽ_2 &k)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(backup_state)-1; i >= 0; i--)
			എങ്കിൽ (backup_state[i].എണ്ണം(k) != 0) {
				എങ്കിൽ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				അല്ലെങ്കിൽ
					current_state[k] = *backup_state[i].at(k);
				തിരികെ;
			}
		current_state.erase(k);
	}

	സ്ഥിരം dict<താക്കോൽ_2, T> &stdmap()
	{
		തിരികെ current_state;
	}

	ശൂന്യം save()
	{
		backup_state.resize(backup_state.size()+1);
	}

	ശൂന്യം restore()
	{
		log_assert(!backup_state.empty());
		വേണ്ടി (auto &it : backup_state.back())
			എങ്കിൽ (it.second != nullptr) {
				current_state[it.first] = *it.second;
				delete it.second;
			} അല്ലെങ്കിൽ
				current_state.erase(it.first);
		backup_state.എടുക്കുക_back();
	}

	~stackmap()
	{
		ആകുമ്പോൾ (!backup_state.empty())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

template <typename T, typename C = std::less<T>> class TopoSort
{
	static_assert(!(std::is_pointer<T>::മൂല്യം && std::is_same<C, std::less<T>>::മൂല്യം),
			"std::less is run-to-run unstable for pointers");
പൊതു:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	ഘടന IndirectCmp {
		IndirectCmp(സ്ഥിരം std::vector<T> &nodes, സ്ഥിരം C &കണ്ണി_cmp) : കണ്ണി_cmp_2(കണ്ണി_cmp), nodes_(nodes) {}
		bool operator()(പൂർണ്ണസംഖ്യ a, പൂർണ്ണസംഖ്യ b) സ്ഥിരം
		{
			log_assert(static_cast<size_t>(a) < nodes_.size());
			log_assert(static_cast<size_t>(b) < nodes_.size());
			തിരികെ കണ്ണി_cmp_2(nodes_[a], nodes_[b]);
		}
		സ്ഥിരം C കണ്ണി_cmp_2;
		സ്ഥിരം std::vector<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, പൂർണ്ണസംഖ്യ, C> കണ്ണി_to_index;
	// edges[i] is the set of nodes with an edge into node i
	std::vector<std::set<പൂർണ്ണസംഖ്യ, IndirectCmp>> edges;
	std::vector<T> sorted;
	std::set<std::vector<T>> loops;

	TopoSort() requires std::is_default_constructible_v<C> : TopoSort(C()) {}
	explicit TopoSort(സ്ഥിരം C &cmp) : കണ്ണി_to_index(cmp), indirect_cmp(nodes, cmp)
	{
		analyze_loops = true;
		found_loops = false;
	}

	പൂർണ്ണസംഖ്യ കണ്ണി(T n)
	{
		auto rv = കണ്ണി_to_index.emplace(n, static_cast<പൂർണ്ണസംഖ്യ>(nodes.size()));
		എങ്കിൽ (rv.second) {
			nodes.തള്ളുക_back(n);
			edges.തള്ളുക_back(std::set<പൂർണ്ണസംഖ്യ, IndirectCmp>(indirect_cmp));
		}
		തിരികെ rv.first->second;
	}

	ശൂന്യം edge(പൂർണ്ണസംഖ്യ l_index, പൂർണ്ണസംഖ്യ r_index) { edges[r_index].തിരുകുക(l_index); }

	ശൂന്യം edge(T ഇടത്_2, T വലത്_2) { edge(കണ്ണി(ഇടത്_2), കണ്ണി(വലത്_2)); }

	bool has_കണ്ണി(സ്ഥിരം T &കണ്ണി) { തിരികെ കണ്ണി_to_index.find(കണ്ണി) != കണ്ണി_to_index.end(); }

	bool sort()
	{
		log_assert(GetSize(കണ്ണി_to_index) == GetSize(edges));
		log_assert(GetSize(nodes) == GetSize(edges));

		loops.clear();
		sorted.clear();
		found_loops = false;

		std::vector<bool> കണ്ണി_is_sorted(edges.size(), false);
		std::vector<bool> കണ്ണി_is_on_അട്ടി(edges.size(), false);
		// Only used with analyze_loops
		std::vector<പൂർണ്ണസംഖ്യ> അട്ടി;
		sorted.reserve(edges.size());

		വേണ്ടി (സ്ഥിരം auto &it : കണ്ണി_to_index)
			sort_worker(it.second, കണ്ണി_is_sorted, കണ്ണി_is_on_അട്ടി, അട്ടി);

		log_assert(GetSize(sorted) == GetSize(nodes));

		തിരികെ !found_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::set<T, C>, C> get_database()
	{
		std::map<T, std::set<T, C>, C> database(കണ്ണി_to_index.താക്കോൽ_comp());
		വേണ്ടി (size_t i = 0; i < nodes.size(); ++i) {
			std::set<T, C> converted_edge_set(കണ്ണി_to_index.താക്കോൽ_comp());
			വേണ്ടി (പൂർണ്ണസംഖ്യ other_കണ്ണി : edges[i]) {
				converted_edge_set.തിരുകുക(nodes[other_കണ്ണി]);
			}
			database.emplace(nodes[i], converted_edge_set);
		}
		തിരികെ database;
	}

സ്വകാര്യ:
	bool found_loops;
	std::vector<T> nodes;
	സ്ഥിരം IndirectCmp indirect_cmp;

	ശൂന്യം sort_worker(സ്ഥിരം പൂർണ്ണസംഖ്യ വേര്_index, std::vector<bool> &കണ്ണി_is_sorted, std::vector<bool> &കണ്ണി_is_on_അട്ടി, std::vector<പൂർണ്ണസംഖ്യ> &അട്ടി)
	{
		എങ്കിൽ (കണ്ണി_is_on_അട്ടി[വേര്_index]) {
			// We've been here before, meaning we have a loop
			found_loops = true;
			എങ്കിൽ (analyze_loops) {
				std::vector<T> loop;
				വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(അട്ടി) - 1; i >= 0; i--) {
					സ്ഥിരം പൂർണ്ണസംഖ്യ index = അട്ടി[i];
					loop.തള്ളുക_back(nodes[index]);
					എങ്കിൽ (index == വേര്_index)
						നിർത്തുക;
				}
				loops.തിരുകുക(loop);
			}
			തിരികെ;
		}

		// We're done if we've already sorted this subgraph
		എങ്കിൽ (കണ്ണി_is_sorted[വേര്_index])
			തിരികെ;

		എങ്കിൽ (!edges[വേര്_index].empty()) {
			എങ്കിൽ (analyze_loops)
				അട്ടി.തള്ളുക_back(വേര്_index);
			കണ്ണി_is_on_അട്ടി[വേര്_index] = true;

			വേണ്ടി (പൂർണ്ണസംഖ്യ ഇടത്_n : edges[വേര്_index])
				sort_worker(ഇടത്_n, കണ്ണി_is_sorted, കണ്ണി_is_on_അട്ടി, അട്ടി);

			എങ്കിൽ (analyze_loops)
				അട്ടി.എടുക്കുക_back();
			കണ്ണി_is_on_അട്ടി[വേര്_index] = false;
		}

		കണ്ണി_is_sorted[വേര്_index] = true;
		sorted.തള്ളുക_back(nodes[വേര്_index]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
template<class T>
ഘടന arrow_proxy {
	T v;
	explicit arrow_proxy(T സ്ഥിരം & v) : v(v) {}
	T* operator->() { തിരികെ &v; }
};

വരിയിൽ പൂർണ്ണസംഖ്യ ceil_log2(പൂർണ്ണസംഖ്യ x)
{
#എങ്കിൽ defined(__GNUC__)
        തിരികെ x > 1 ? (8*വലുപ്പം(പൂർണ്ണസംഖ്യ)) - __builtin_clz(x-1) : 0;
#അല്ലെങ്കിൽ
	എങ്കിൽ (x <= 0)
		തിരികെ 0;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 32; i++)
		എങ്കിൽ (((x-1) >> i) == 0)
			തിരികെ i;
	log_abort();
#endif
}

template <typename T>
auto reversed(T& container) {
	ഘടന reverse_view {
		reverse_view(T& container) : container(container) {}
		auto begin() സ്ഥിരം { തിരികെ container.rbegin(); }
		auto end() സ്ഥിരം { തിരികെ container.rend(); }
		T& container;
	};
	തിരികെ reverse_view{container};
}

template <typename T>
auto reversed(സ്ഥിരം T& container) {
	ഘടന reverse_view {
		reverse_view(സ്ഥിരം T& container) : container(container) {}
		auto begin() സ്ഥിരം { തിരികെ container.rbegin(); }
		auto end() സ്ഥിരം { തിരികെ container.rend(); }
		സ്ഥിരം T& container;
	};
	തിരികെ reverse_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
ഘടന IntRange {
	പൂർണ്ണസംഖ്യ start_;
	പൂർണ്ണസംഖ്യ end_;
	ഘടന Int {
		പൂർണ്ണസംഖ്യ v;
		പൂർണ്ണസംഖ്യ operator*() സ്ഥിരം { തിരികെ v; }
		Int &operator++() { ++v; തിരികെ *this; }
		bool operator!=(സ്ഥിരം Int &other) സ്ഥിരം { തിരികെ v != other.v; }
	};
	Int begin() സ്ഥിരം { തിരികെ {start_}; }
	Int end() സ്ഥിരം { തിരികെ {end_}; }

	bool operator==(സ്ഥിരം IntRange &other) സ്ഥിരം { തിരികെ start_ == other.start_ && end_ == other.end_; }
	bool operator!=(സ്ഥിരം IntRange &other) സ്ഥിരം { തിരികെ !(*this == other); }
};

YOSYS_NAMESPACE_END

#endif
