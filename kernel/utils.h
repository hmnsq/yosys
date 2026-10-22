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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <iterator>

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ UTILS_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

ਸਾਂਚਾ<typename ਕੁੰਜੀ_2, typename T>
ਬਣਤਰ stackmap
{
ਨਿਜੀ:
	std::ਸਦਿਸ਼<dict<ਕੁੰਜੀ_2, T*>> backup_state;
	dict<ਕੁੰਜੀ_2, T> current_state;
	ਸਥਿਰ T ਖਾਲੀ_ਥਾਵਾਂ_tuple;

ਜਨਤਕ:
	stackmap() { }
	stackmap(ਅਚਲ dict<ਕੁੰਜੀ_2, T> &other) : current_state(other) { }

	ਸਾਂਚਾ<typename Other>
	ਖਾਲੀ ਸੰਚਾਲਕ=(ਅਚਲ Other &other)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : current_state)
			ਜੇ (!backup_state.ਖਾਲੀ_ਥਾਵਾਂ() && backup_state.ਪਿੱਠ().ਗਿਣਤੀ_2(it.ਪਹਿਲਾ) == 0)
				backup_state.ਪਿੱਠ()[it.ਪਹਿਲਾ] = new T(it.ਦੂਜਾ);
		current_state.clear();

		ਲਈ (ਸਵੈਚਾਲਿਤ &it : other)
			ਸੈੱਟ(it.ਪਹਿਲਾ, it.ਦੂਜਾ);
	}

	bool has(ਅਚਲ ਕੁੰਜੀ_2 &k)
	{
		ਵਾਪਸ current_state.ਗਿਣਤੀ_2(k) != 0;
	}

	ਖਾਲੀ ਸੈੱਟ(ਅਚਲ ਕੁੰਜੀ_2 &k, ਅਚਲ T &v)
	{
		ਜੇ (!backup_state.ਖਾਲੀ_ਥਾਵਾਂ() && backup_state.ਪਿੱਠ().ਗਿਣਤੀ_2(k) == 0)
			backup_state.ਪਿੱਠ()[k] = current_state.ਗਿਣਤੀ_2(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	ਖਾਲੀ unset(ਅਚਲ ਕੁੰਜੀ_2 &k)
	{
		ਜੇ (!backup_state.ਖਾਲੀ_ਥਾਵਾਂ() && backup_state.ਪਿੱਠ().ਗਿਣਤੀ_2(k) == 0)
			backup_state.ਪਿੱਠ()[k] = current_state.ਗਿਣਤੀ_2(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	ਅਚਲ T &get(ਅਚਲ ਕੁੰਜੀ_2 &k)
	{
		ਜੇ (current_state.ਗਿਣਤੀ_2(k) == 0)
			ਵਾਪਸ ਖਾਲੀ_ਥਾਵਾਂ_tuple;
		ਵਾਪਸ current_state.at(k);
	}

	ਖਾਲੀ reset(ਅਚਲ ਕੁੰਜੀ_2 &k)
	{
		ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(backup_state)-1; i >= 0; i--)
			ਜੇ (backup_state[i].ਗਿਣਤੀ_2(k) != 0) {
				ਜੇ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				ਨਹੀਂਤਾਂ
					current_state[k] = *backup_state[i].at(k);
				ਵਾਪਸ;
			}
		current_state.erase(k);
	}

	ਅਚਲ dict<ਕੁੰਜੀ_2, T> &stdmap()
	{
		ਵਾਪਸ current_state;
	}

	ਖਾਲੀ save()
	{
		backup_state.resize(backup_state.ਮਾਪ()+1);
	}

	ਖਾਲੀ restore()
	{
		log_assert(!backup_state.ਖਾਲੀ_ਥਾਵਾਂ());
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : backup_state.ਪਿੱਠ())
			ਜੇ (it.ਦੂਜਾ != nullptr) {
				current_state[it.ਪਹਿਲਾ] = *it.ਦੂਜਾ;
				delete it.ਦੂਜਾ;
			} ਨਹੀਂਤਾਂ
				current_state.erase(it.ਪਹਿਲਾ);
		backup_state.ਕੱਢੋ_ਪਿੱਠ();
	}

	~stackmap()
	{
		ਜਦੋਂ (!backup_state.ਖਾਲੀ_ਥਾਵਾਂ())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

ਸਾਂਚਾ <typename T, typename C = std::less<T>> class Topo_ਛਾਂਟੋ
{
	static_assert(!(std::is_ਸੂਚਕ<T>::ਮੁੱਲ && std::is_same<C, std::less<T>>::ਮੁੱਲ),
			"std::less is run-to-run unstable for pointers");
ਜਨਤਕ:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	ਬਣਤਰ ਅਸਿੱਧਾ_Cmp {
		ਅਸਿੱਧਾ_Cmp(ਅਚਲ std::ਸਦਿਸ਼<T> &nodes, ਅਚਲ C &ਗੰਢ_cmp) : ਗੰਢ_cmp_2(ਗੰਢ_cmp), nodes_(nodes) {}
		bool ਸੰਚਾਲਕ()(ਪੂਰਨ a, ਪੂਰਨ b) ਅਚਲ
		{
			log_assert(static_cast<ਮਾਪ_t>(a) < nodes_.ਮਾਪ());
			log_assert(static_cast<ਮਾਪ_t>(b) < nodes_.ਮਾਪ());
			ਵਾਪਸ ਗੰਢ_cmp_2(nodes_[a], nodes_[b]);
		}
		ਅਚਲ C ਗੰਢ_cmp_2;
		ਅਚਲ std::ਸਦਿਸ਼<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, ਪੂਰਨ, C> ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ;
	// edges[i] is the set of nodes with an edge into node i
	std::ਸਦਿਸ਼<std::ਸੈੱਟ<ਪੂਰਨ, ਅਸਿੱਧਾ_Cmp>> ਕਿਨਾਰੇ;
	std::ਸਦਿਸ਼<T> sorted;
	std::ਸੈੱਟ<std::ਸਦਿਸ਼<T>> loops;

	Topo_ਛਾਂਟੋ() requires std::is_default_constructible_v<C> : Topo_ਛਾਂਟੋ(C()) {}
	explicit Topo_ਛਾਂਟੋ(ਅਚਲ C &cmp) : ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ(cmp), ਅਸਿੱਧਾ_cmp(nodes, cmp)
	{
		analyze_loops = true;
		ਮਿਲਿਆ_loops = false;
	}

	ਪੂਰਨ ਗੰਢ(T n)
	{
		ਸਵੈਚਾਲਿਤ rv = ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ.emplace(n, static_cast<ਪੂਰਨ>(nodes.ਮਾਪ()));
		ਜੇ (rv.ਦੂਜਾ) {
			nodes.ਧੱਕੋ_ਪਿੱਠ(n);
			ਕਿਨਾਰੇ.ਧੱਕੋ_ਪਿੱਠ(std::ਸੈੱਟ<ਪੂਰਨ, ਅਸਿੱਧਾ_Cmp>(ਅਸਿੱਧਾ_cmp));
		}
		ਵਾਪਸ rv.ਪਹਿਲਾ->ਦੂਜਾ;
	}

	ਖਾਲੀ ਕਿਨਾਰਾ(ਪੂਰਨ l_ਸੂਚਕਾਂਕ, ਪੂਰਨ r_ਸੂਚਕਾਂਕ) { ਕਿਨਾਰੇ[r_ਸੂਚਕਾਂਕ].ਪਾਓ(l_ਸੂਚਕਾਂਕ); }

	ਖਾਲੀ ਕਿਨਾਰਾ(T ਖੱਬਾ_2, T ਸੱਜਾ_2) { ਕਿਨਾਰਾ(ਗੰਢ(ਖੱਬਾ_2), ਗੰਢ(ਸੱਜਾ_2)); }

	bool has_ਗੰਢ(ਅਚਲ T &ਗੰਢ) { ਵਾਪਸ ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ.ਲੱਭੋ(ਗੰਢ) != ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ.ਸਿਰਾ(); }

	bool ਛਾਂਟੋ()
	{
		log_assert(Get_ਮਾਪ(ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ) == Get_ਮਾਪ(ਕਿਨਾਰੇ));
		log_assert(Get_ਮਾਪ(nodes) == Get_ਮਾਪ(ਕਿਨਾਰੇ));

		loops.clear();
		sorted.clear();
		ਮਿਲਿਆ_loops = false;

		std::ਸਦਿਸ਼<bool> ਗੰਢ_is_sorted(ਕਿਨਾਰੇ.ਮਾਪ(), false);
		std::ਸਦਿਸ਼<bool> ਗੰਢ_is_on_ਢੇਰ(ਕਿਨਾਰੇ.ਮਾਪ(), false);
		// Only used with analyze_loops
		std::ਸਦਿਸ਼<ਪੂਰਨ> ਢੇਰ;
		sorted.reserve(ਕਿਨਾਰੇ.ਮਾਪ());

		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &it : ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ)
			ਛਾਂਟੋ_ਕਾਮਾ(it.ਦੂਜਾ, ਗੰਢ_is_sorted, ਗੰਢ_is_on_ਢੇਰ, ਢੇਰ);

		log_assert(Get_ਮਾਪ(sorted) == Get_ਮਾਪ(nodes));

		ਵਾਪਸ !ਮਿਲਿਆ_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::ਸੈੱਟ<T, C>, C> get_database()
	{
		std::map<T, std::ਸੈੱਟ<T, C>, C> database(ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ.ਕੁੰਜੀ_comp());
		ਲਈ (ਮਾਪ_t i = 0; i < nodes.ਮਾਪ(); ++i) {
			std::ਸੈੱਟ<T, C> converted_ਕਿਨਾਰਾ_ਸੈੱਟ(ਗੰਢ_ਮੰਜ਼ਿਲ_ਸੂਚਕਾਂਕ.ਕੁੰਜੀ_comp());
			ਲਈ (ਪੂਰਨ other_ਗੰਢ : ਕਿਨਾਰੇ[i]) {
				converted_ਕਿਨਾਰਾ_ਸੈੱਟ.ਪਾਓ(nodes[other_ਗੰਢ]);
			}
			database.emplace(nodes[i], converted_ਕਿਨਾਰਾ_ਸੈੱਟ);
		}
		ਵਾਪਸ database;
	}

ਨਿਜੀ:
	bool ਮਿਲਿਆ_loops;
	std::ਸਦਿਸ਼<T> nodes;
	ਅਚਲ ਅਸਿੱਧਾ_Cmp ਅਸਿੱਧਾ_cmp;

	ਖਾਲੀ ਛਾਂਟੋ_ਕਾਮਾ(ਅਚਲ ਪੂਰਨ ਜੜ੍ਹ_ਸੂਚਕਾਂਕ, std::ਸਦਿਸ਼<bool> &ਗੰਢ_is_sorted, std::ਸਦਿਸ਼<bool> &ਗੰਢ_is_on_ਢੇਰ, std::ਸਦਿਸ਼<ਪੂਰਨ> &ਢੇਰ)
	{
		ਜੇ (ਗੰਢ_is_on_ਢੇਰ[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ]) {
			// We've been here before, meaning we have a loop
			ਮਿਲਿਆ_loops = true;
			ਜੇ (analyze_loops) {
				std::ਸਦਿਸ਼<T> loop;
				ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(ਢੇਰ) - 1; i >= 0; i--) {
					ਅਚਲ ਪੂਰਨ ਸੂਚਕਾਂਕ = ਢੇਰ[i];
					loop.ਧੱਕੋ_ਪਿੱਠ(nodes[ਸੂਚਕਾਂਕ]);
					ਜੇ (ਸੂਚਕਾਂਕ == ਜੜ੍ਹ_ਸੂਚਕਾਂਕ)
						ਰੋਕੋ;
				}
				loops.ਪਾਓ(loop);
			}
			ਵਾਪਸ;
		}

		// We're done if we've already sorted this subgraph
		ਜੇ (ਗੰਢ_is_sorted[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ])
			ਵਾਪਸ;

		ਜੇ (!ਕਿਨਾਰੇ[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ].ਖਾਲੀ_ਥਾਵਾਂ()) {
			ਜੇ (analyze_loops)
				ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(ਜੜ੍ਹ_ਸੂਚਕਾਂਕ);
			ਗੰਢ_is_on_ਢੇਰ[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ] = true;

			ਲਈ (ਪੂਰਨ ਖੱਬਾ_n : ਕਿਨਾਰੇ[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ])
				ਛਾਂਟੋ_ਕਾਮਾ(ਖੱਬਾ_n, ਗੰਢ_is_sorted, ਗੰਢ_is_on_ਢੇਰ, ਢੇਰ);

			ਜੇ (analyze_loops)
				ਢੇਰ.ਕੱਢੋ_ਪਿੱਠ();
			ਗੰਢ_is_on_ਢੇਰ[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ] = false;
		}

		ਗੰਢ_is_sorted[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ] = true;
		sorted.ਧੱਕੋ_ਪਿੱਠ(nodes[ਜੜ੍ਹ_ਸੂਚਕਾਂਕ]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
ਸਾਂਚਾ<class T>
ਬਣਤਰ arrow_proxy {
	T v;
	explicit arrow_proxy(T ਅਚਲ & v) : v(v) {}
	T* ਸੰਚਾਲਕ->() { ਵਾਪਸ &v; }
};

ਸਤਰਅੰਦਰ ਪੂਰਨ ceil_log2(ਪੂਰਨ x)
{
#ਜੇ defined(__GNUC__)
        ਵਾਪਸ x > 1 ? (8*ਆਕਾਰ(ਪੂਰਨ)) - __builtin_clz(x-1) : 0;
#ਨਹੀਂਤਾਂ
	ਜੇ (x <= 0)
		ਵਾਪਸ 0;
	ਲਈ (ਪੂਰਨ i = 0; i < 32; i++)
		ਜੇ (((x-1) >> i) == 0)
			ਵਾਪਸ i;
	log_abort();
#ਜੇ_ਅੰਤ
}

ਸਾਂਚਾ <typename T>
ਸਵੈਚਾਲਿਤ reversed(T& container) {
	ਬਣਤਰ ਉਲਟ_view {
		ਉਲਟ_view(T& container) : container(container) {}
		ਸਵੈਚਾਲਿਤ begin() ਅਚਲ { ਵਾਪਸ container.rbegin(); }
		ਸਵੈਚਾਲਿਤ ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ container.rend(); }
		T& container;
	};
	ਵਾਪਸ ਉਲਟ_view{container};
}

ਸਾਂਚਾ <typename T>
ਸਵੈਚਾਲਿਤ reversed(ਅਚਲ T& container) {
	ਬਣਤਰ ਉਲਟ_view {
		ਉਲਟ_view(ਅਚਲ T& container) : container(container) {}
		ਸਵੈਚਾਲਿਤ begin() ਅਚਲ { ਵਾਪਸ container.rbegin(); }
		ਸਵੈਚਾਲਿਤ ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ container.rend(); }
		ਅਚਲ T& container;
	};
	ਵਾਪਸ ਉਲਟ_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
ਬਣਤਰ IntRange {
	ਪੂਰਨ ਸ਼ੁਰੂ_2;
	ਪੂਰਨ ਸਿਰਾ_4;
	ਬਣਤਰ Int {
		ਪੂਰਨ v;
		ਪੂਰਨ ਸੰਚਾਲਕ*() ਅਚਲ { ਵਾਪਸ v; }
		Int &ਸੰਚਾਲਕ++() { ++v; ਵਾਪਸ *this; }
		bool ਸੰਚਾਲਕ!=(ਅਚਲ Int &other) ਅਚਲ { ਵਾਪਸ v != other.v; }
	};
	Int begin() ਅਚਲ { ਵਾਪਸ {ਸ਼ੁਰੂ_2}; }
	Int ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ {ਸਿਰਾ_4}; }

	bool ਸੰਚਾਲਕ==(ਅਚਲ IntRange &other) ਅਚਲ { ਵਾਪਸ ਸ਼ੁਰੂ_2 == other.ਸ਼ੁਰੂ_2 && ਸਿਰਾ_4 == other.ਸਿਰਾ_4; }
	bool ਸੰਚਾਲਕ!=(ਅਚਲ IntRange &other) ਅਚਲ { ਵਾਪਸ !(*this == other); }
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
