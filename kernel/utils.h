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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ <iterator>

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ UTILS_H
#ವ್ಯಾಖ್ಯಾನಿಸು UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

ಮಾದರಿ<typename ಕೀಲಿ_2, typename T>
ರಚನೆ stackmap
{
ಖಾಸಗಿ:
	std::ಸದಿಶ<dict<ಕೀಲಿ_2, T*>> backup_state;
	dict<ಕೀಲಿ_2, T> current_state;
	ಸ್ಥಿರ T ಖಾಲಿಗಳು_tuple;

ಸಾರ್ವಜನಿಕ:
	stackmap() { }
	stackmap(ಸ್ಥಿರಾಂಕ dict<ಕೀಲಿ_2, T> &other) : current_state(other) { }

	ಮಾದರಿ<typename Other>
	ಶೂನ್ಯ ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ Other &other)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : current_state)
			ಆದರೆ (!backup_state.ಖಾಲಿಗಳು() && backup_state.ಹಿಂಭಾಗ().ಎಣಿಕೆ_2(it.ಮೊದಲ) == 0)
				backup_state.ಹಿಂಭಾಗ()[it.ಮೊದಲ] = new T(it.ಎರಡನೇ);
		current_state.clear();

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : other)
			ಗಣ(it.ಮೊದಲ, it.ಎರಡನೇ);
	}

	bool has(ಸ್ಥಿರಾಂಕ ಕೀಲಿ_2 &k)
	{
		ಹಿಂತಿರುಗಿ current_state.ಎಣಿಕೆ_2(k) != 0;
	}

	ಶೂನ್ಯ ಗಣ(ಸ್ಥಿರಾಂಕ ಕೀಲಿ_2 &k, ಸ್ಥಿರಾಂಕ T &v)
	{
		ಆದರೆ (!backup_state.ಖಾಲಿಗಳು() && backup_state.ಹಿಂಭಾಗ().ಎಣಿಕೆ_2(k) == 0)
			backup_state.ಹಿಂಭಾಗ()[k] = current_state.ಎಣಿಕೆ_2(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	ಶೂನ್ಯ unset(ಸ್ಥಿರಾಂಕ ಕೀಲಿ_2 &k)
	{
		ಆದರೆ (!backup_state.ಖಾಲಿಗಳು() && backup_state.ಹಿಂಭಾಗ().ಎಣಿಕೆ_2(k) == 0)
			backup_state.ಹಿಂಭಾಗ()[k] = current_state.ಎಣಿಕೆ_2(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ಸ್ಥಿರಾಂಕ T &get(ಸ್ಥಿರಾಂಕ ಕೀಲಿ_2 &k)
	{
		ಆದರೆ (current_state.ಎಣಿಕೆ_2(k) == 0)
			ಹಿಂತಿರುಗಿ ಖಾಲಿಗಳು_tuple;
		ಹಿಂತಿರುಗಿ current_state.at(k);
	}

	ಶೂನ್ಯ reset(ಸ್ಥಿರಾಂಕ ಕೀಲಿ_2 &k)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(backup_state)-1; i >= 0; i--)
			ಆದರೆ (backup_state[i].ಎಣಿಕೆ_2(k) != 0) {
				ಆದರೆ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				ಇಲ್ಲದಿದ್ದರೆ
					current_state[k] = *backup_state[i].at(k);
				ಹಿಂತಿರುಗಿ;
			}
		current_state.erase(k);
	}

	ಸ್ಥಿರಾಂಕ dict<ಕೀಲಿ_2, T> &stdmap()
	{
		ಹಿಂತಿರುಗಿ current_state;
	}

	ಶೂನ್ಯ save()
	{
		backup_state.resize(backup_state.ಗಾತ್ರ_2()+1);
	}

	ಶೂನ್ಯ restore()
	{
		log_assert(!backup_state.ಖಾಲಿಗಳು());
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : backup_state.ಹಿಂಭಾಗ())
			ಆದರೆ (it.ಎರಡನೇ != nullptr) {
				current_state[it.ಮೊದಲ] = *it.ಎರಡನೇ;
				delete it.ಎರಡನೇ;
			} ಇಲ್ಲದಿದ್ದರೆ
				current_state.erase(it.ಮೊದಲ);
		backup_state.ತೆಗೆ_ಹಿಂಭಾಗ();
	}

	~stackmap()
	{
		ಆಗ (!backup_state.ಖಾಲಿಗಳು())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

ಮಾದರಿ <typename T, typename C = std::less<T>> class Topo_ವಿಂಗಡಿಸು
{
	static_assert(!(std::is_ಸೂಚಕ<T>::ಮೌಲ್ಯ && std::is_same<C, std::less<T>>::ಮೌಲ್ಯ),
			"std::less is run-to-run unstable for pointers");
ಸಾರ್ವಜನಿಕ:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	ರಚನೆ ಪರೋಕ್ಷ_Cmp {
		ಪರೋಕ್ಷ_Cmp(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<T> &nodes, ಸ್ಥಿರಾಂಕ C &ಗಂಟು_cmp) : ಗಂಟು_cmp_2(ಗಂಟು_cmp), nodes_(nodes) {}
		bool ಕ್ರಿಯಾಕಾರಕ()(ಪೂರ್ಣಾಂಕ a, ಪೂರ್ಣಾಂಕ b) ಸ್ಥಿರಾಂಕ
		{
			log_assert(static_cast<ಗಾತ್ರ_t>(a) < nodes_.ಗಾತ್ರ_2());
			log_assert(static_cast<ಗಾತ್ರ_t>(b) < nodes_.ಗಾತ್ರ_2());
			ಹಿಂತಿರುಗಿ ಗಂಟು_cmp_2(nodes_[a], nodes_[b]);
		}
		ಸ್ಥಿರಾಂಕ C ಗಂಟು_cmp_2;
		ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ಪೂರ್ಣಾಂಕ, C> ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ;
	// edges[i] is the set of nodes with an edge into node i
	std::ಸದಿಶ<std::ಗಣ<ಪೂರ್ಣಾಂಕ, ಪರೋಕ್ಷ_Cmp>> ಅಂಚುಗಳು;
	std::ಸದಿಶ<T> sorted;
	std::ಗಣ<std::ಸದಿಶ<T>> loops;

	Topo_ವಿಂಗಡಿಸು() requires std::is_default_constructible_v<C> : Topo_ವಿಂಗಡಿಸು(C()) {}
	explicit Topo_ವಿಂಗಡಿಸು(ಸ್ಥಿರಾಂಕ C &cmp) : ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ(cmp), ಪರೋಕ್ಷ_cmp(nodes, cmp)
	{
		analyze_loops = true;
		ಸಿಕ್ಕಿತು_loops = false;
	}

	ಪೂರ್ಣಾಂಕ ಗಂಟು(T n)
	{
		ಸ್ವಯಂಚಾಲಿತ rv = ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ.emplace(n, static_cast<ಪೂರ್ಣಾಂಕ>(nodes.ಗಾತ್ರ_2()));
		ಆದರೆ (rv.ಎರಡನೇ) {
			nodes.ತಳ್ಳು_ಹಿಂಭಾಗ(n);
			ಅಂಚುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(std::ಗಣ<ಪೂರ್ಣಾಂಕ, ಪರೋಕ್ಷ_Cmp>(ಪರೋಕ್ಷ_cmp));
		}
		ಹಿಂತಿರುಗಿ rv.ಮೊದಲ->ಎರಡನೇ;
	}

	ಶೂನ್ಯ ಅಂಚು(ಪೂರ್ಣಾಂಕ l_ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ r_ಸೂಚ್ಯಂಕ) { ಅಂಚುಗಳು[r_ಸೂಚ್ಯಂಕ].ಸೇರಿಸು(l_ಸೂಚ್ಯಂಕ); }

	ಶೂನ್ಯ ಅಂಚು(T ಎಡ_3, T ಬಲ_3) { ಅಂಚು(ಗಂಟು(ಎಡ_3), ಗಂಟು(ಬಲ_3)); }

	bool has_ಗಂಟು(ಸ್ಥಿರಾಂಕ T &ಗಂಟು) { ಹಿಂತಿರುಗಿ ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ.ಕಂಡುಹಿಡಿ(ಗಂಟು) != ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ.ಕೊನೆ(); }

	bool ವಿಂಗಡಿಸು()
	{
		log_assert(Get_ಗಾತ್ರ(ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ) == Get_ಗಾತ್ರ(ಅಂಚುಗಳು));
		log_assert(Get_ಗಾತ್ರ(nodes) == Get_ಗಾತ್ರ(ಅಂಚುಗಳು));

		loops.clear();
		sorted.clear();
		ಸಿಕ್ಕಿತು_loops = false;

		std::ಸದಿಶ<bool> ಗಂಟು_is_sorted(ಅಂಚುಗಳು.ಗಾತ್ರ_2(), false);
		std::ಸದಿಶ<bool> ಗಂಟು_is_on_ರಾಶಿ(ಅಂಚುಗಳು.ಗಾತ್ರ_2(), false);
		// Only used with analyze_loops
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ರಾಶಿ;
		sorted.reserve(ಅಂಚುಗಳು.ಗಾತ್ರ_2());

		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &it : ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ)
			ವಿಂಗಡಿಸು_ಕೆಲಸಗಾರ(it.ಎರಡನೇ, ಗಂಟು_is_sorted, ಗಂಟು_is_on_ರಾಶಿ, ರಾಶಿ);

		log_assert(Get_ಗಾತ್ರ(sorted) == Get_ಗಾತ್ರ(nodes));

		ಹಿಂತಿರುಗಿ !ಸಿಕ್ಕಿತು_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::ಗಣ<T, C>, C> get_database()
	{
		std::map<T, std::ಗಣ<T, C>, C> database(ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ.ಕೀಲಿ_comp());
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < nodes.ಗಾತ್ರ_2(); ++i) {
			std::ಗಣ<T, C> converted_ಅಂಚು_ಗಣ(ಗಂಟು_ಗಮ್ಯ_ಸೂಚ್ಯಂಕ.ಕೀಲಿ_comp());
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ other_ಗಂಟು : ಅಂಚುಗಳು[i]) {
				converted_ಅಂಚು_ಗಣ.ಸೇರಿಸು(nodes[other_ಗಂಟು]);
			}
			database.emplace(nodes[i], converted_ಅಂಚು_ಗಣ);
		}
		ಹಿಂತಿರುಗಿ database;
	}

ಖಾಸಗಿ:
	bool ಸಿಕ್ಕಿತು_loops;
	std::ಸದಿಶ<T> nodes;
	ಸ್ಥಿರಾಂಕ ಪರೋಕ್ಷ_Cmp ಪರೋಕ್ಷ_cmp;

	ಶೂನ್ಯ ವಿಂಗಡಿಸು_ಕೆಲಸಗಾರ(ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಬೇರು_ಸೂಚ್ಯಂಕ, std::ಸದಿಶ<bool> &ಗಂಟು_is_sorted, std::ಸದಿಶ<bool> &ಗಂಟು_is_on_ರಾಶಿ, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &ರಾಶಿ)
	{
		ಆದರೆ (ಗಂಟು_is_on_ರಾಶಿ[ಬೇರು_ಸೂಚ್ಯಂಕ]) {
			// We've been here before, meaning we have a loop
			ಸಿಕ್ಕಿತು_loops = true;
			ಆದರೆ (analyze_loops) {
				std::ಸದಿಶ<T> loop;
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(ರಾಶಿ) - 1; i >= 0; i--) {
					ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = ರಾಶಿ[i];
					loop.ತಳ್ಳು_ಹಿಂಭಾಗ(nodes[ಸೂಚ್ಯಂಕ]);
					ಆದರೆ (ಸೂಚ್ಯಂಕ == ಬೇರು_ಸೂಚ್ಯಂಕ)
						ನಿಲ್ಲಿಸು;
				}
				loops.ಸೇರಿಸು(loop);
			}
			ಹಿಂತಿರುಗಿ;
		}

		// We're done if we've already sorted this subgraph
		ಆದರೆ (ಗಂಟು_is_sorted[ಬೇರು_ಸೂಚ್ಯಂಕ])
			ಹಿಂತಿರುಗಿ;

		ಆದರೆ (!ಅಂಚುಗಳು[ಬೇರು_ಸೂಚ್ಯಂಕ].ಖಾಲಿಗಳು()) {
			ಆದರೆ (analyze_loops)
				ರಾಶಿ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬೇರು_ಸೂಚ್ಯಂಕ);
			ಗಂಟು_is_on_ರಾಶಿ[ಬೇರು_ಸೂಚ್ಯಂಕ] = true;

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಎಡ_n : ಅಂಚುಗಳು[ಬೇರು_ಸೂಚ್ಯಂಕ])
				ವಿಂಗಡಿಸು_ಕೆಲಸಗಾರ(ಎಡ_n, ಗಂಟು_is_sorted, ಗಂಟು_is_on_ರಾಶಿ, ರಾಶಿ);

			ಆದರೆ (analyze_loops)
				ರಾಶಿ.ತೆಗೆ_ಹಿಂಭಾಗ();
			ಗಂಟು_is_on_ರಾಶಿ[ಬೇರು_ಸೂಚ್ಯಂಕ] = false;
		}

		ಗಂಟು_is_sorted[ಬೇರು_ಸೂಚ್ಯಂಕ] = true;
		sorted.ತಳ್ಳು_ಹಿಂಭಾಗ(nodes[ಬೇರು_ಸೂಚ್ಯಂಕ]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
ಮಾದರಿ<class T>
ರಚನೆ arrow_proxy {
	T v;
	explicit arrow_proxy(T ಸ್ಥಿರಾಂಕ & v) : v(v) {}
	T* ಕ್ರಿಯಾಕಾರಕ->() { ಹಿಂತಿರುಗಿ &v; }
};

ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ceil_log2(ಪೂರ್ಣಾಂಕ x)
{
#ಆದರೆ defined(__GNUC__)
        ಹಿಂತಿರುಗಿ x > 1 ? (8*ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)) - __builtin_clz(x-1) : 0;
#ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (x <= 0)
		ಹಿಂತಿರುಗಿ 0;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 32; i++)
		ಆದರೆ (((x-1) >> i) == 0)
			ಹಿಂತಿರುಗಿ i;
	log_abort();
#ಆದರೆ_ಅಂತ್ಯ
}

ಮಾದರಿ <typename T>
ಸ್ವಯಂಚಾಲಿತ reversed(T& container) {
	ರಚನೆ ಹಿಮ್ಮುಖ_view {
		ಹಿಮ್ಮುಖ_view(T& container) : container(container) {}
		ಸ್ವಯಂಚಾಲಿತ begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ container.rbegin(); }
		ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ container.rend(); }
		T& container;
	};
	ಹಿಂತಿರುಗಿ ಹಿಮ್ಮುಖ_view{container};
}

ಮಾದರಿ <typename T>
ಸ್ವಯಂಚಾಲಿತ reversed(ಸ್ಥಿರಾಂಕ T& container) {
	ರಚನೆ ಹಿಮ್ಮುಖ_view {
		ಹಿಮ್ಮುಖ_view(ಸ್ಥಿರಾಂಕ T& container) : container(container) {}
		ಸ್ವಯಂಚಾಲಿತ begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ container.rbegin(); }
		ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ container.rend(); }
		ಸ್ಥಿರಾಂಕ T& container;
	};
	ಹಿಂತಿರುಗಿ ಹಿಮ್ಮುಖ_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
ರಚನೆ IntRange {
	ಪೂರ್ಣಾಂಕ ಆರಂಭ_2;
	ಪೂರ್ಣಾಂಕ ಕೊನೆ_4;
	ರಚನೆ Int {
		ಪೂರ್ಣಾಂಕ v;
		ಪೂರ್ಣಾಂಕ ಕ್ರಿಯಾಕಾರಕ*() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ v; }
		Int &ಕ್ರಿಯಾಕಾರಕ++() { ++v; ಹಿಂತಿರುಗಿ *this; }
		bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ Int &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ v != other.v; }
	};
	Int begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ {ಆರಂಭ_2}; }
	Int ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ {ಕೊನೆ_4}; }

	bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ IntRange &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಆರಂಭ_2 == other.ಆರಂಭ_2 && ಕೊನೆ_4 == other.ಕೊನೆ_4; }
	bool ಕ್ರಿಯಾಕಾರಕ!=(ಸ್ಥಿರಾಂಕ IntRange &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !(*this == other); }
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
