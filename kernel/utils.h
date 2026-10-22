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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ <iterator>

#నిర్వచితం_కాకపోతే UTILS_H
#నిర్వచించు UTILS_H

YOSYS_NAMESPACE_BEGIN

// ------------------------------------------------
// A map-like container, but you can save and restore the state
// ------------------------------------------------

మూస<typename కీలకం_2, typename T>
నిర్మాణం stackmap
{
అంతరంగ:
	std::సదిశ<dict<కీలకం_2, T*>> backup_state;
	dict<కీలకం_2, T> current_state;
	స్థిర T ఖాళీలు_tuple;

బహిరంగ:
	stackmap() { }
	stackmap(స్థిరరాశి dict<కీలకం_2, T> &other) : current_state(other) { }

	మూస<typename Other>
	శూన్య క్రియాకారకం=(స్థిరరాశి Other &other)
	{
		కొరకు (స్వయంచాలక &it : current_state)
			ఒకవేళ (!backup_state.ఖాళీలు() && backup_state.వెనుక().లెక్క(it.మొదటి) == 0)
				backup_state.వెనుక()[it.మొదటి] = new T(it.రెండవ);
		current_state.clear();

		కొరకు (స్వయంచాలక &it : other)
			సమితి(it.మొదటి, it.రెండవ);
	}

	bool has(స్థిరరాశి కీలకం_2 &k)
	{
		తిరిగి current_state.లెక్క(k) != 0;
	}

	శూన్య సమితి(స్థిరరాశి కీలకం_2 &k, స్థిరరాశి T &v)
	{
		ఒకవేళ (!backup_state.ఖాళీలు() && backup_state.వెనుక().లెక్క(k) == 0)
			backup_state.వెనుక()[k] = current_state.లెక్క(k) ? new T(current_state.at(k)) : nullptr;
		current_state[k] = v;
	}

	శూన్య unset(స్థిరరాశి కీలకం_2 &k)
	{
		ఒకవేళ (!backup_state.ఖాళీలు() && backup_state.వెనుక().లెక్క(k) == 0)
			backup_state.వెనుక()[k] = current_state.లెక్క(k) ? new T(current_state.at(k)) : nullptr;
		current_state.erase(k);
	}

	స్థిరరాశి T &get(స్థిరరాశి కీలకం_2 &k)
	{
		ఒకవేళ (current_state.లెక్క(k) == 0)
			తిరిగి ఖాళీలు_tuple;
		తిరిగి current_state.at(k);
	}

	శూన్య reset(స్థిరరాశి కీలకం_2 &k)
	{
		కొరకు (పూర్ణాంకం i = Get_కొలత(backup_state)-1; i >= 0; i--)
			ఒకవేళ (backup_state[i].లెక్క(k) != 0) {
				ఒకవేళ (backup_state[i].at(k) == nullptr)
					current_state.erase(k);
				లేకపోతే
					current_state[k] = *backup_state[i].at(k);
				తిరిగి;
			}
		current_state.erase(k);
	}

	స్థిరరాశి dict<కీలకం_2, T> &stdmap()
	{
		తిరిగి current_state;
	}

	శూన్య save()
	{
		backup_state.resize(backup_state.కొలత()+1);
	}

	శూన్య restore()
	{
		log_assert(!backup_state.ఖాళీలు());
		కొరకు (స్వయంచాలక &it : backup_state.వెనుక())
			ఒకవేళ (it.రెండవ != nullptr) {
				current_state[it.మొదటి] = *it.రెండవ;
				delete it.రెండవ;
			} లేకపోతే
				current_state.erase(it.మొదటి);
		backup_state.తీయి_వెనుక();
	}

	~stackmap()
	{
		అయితే (!backup_state.ఖాళీలు())
			restore();
	}
};


// ---------------------------------------------------
// Best-effort topological sorting with loop detection
// ---------------------------------------------------

మూస <typename T, typename C = std::less<T>> class Topo_క్రమీకరించు
{
	static_assert(!(std::is_సూచిక<T>::విలువ && std::is_same<C, std::less<T>>::విలువ),
			"std::less is run-to-run unstable for pointers");
బహిరంగ:
	// We use this ordering of the edges in the adjacency matrix for
	// exact compatibility with an older implementation.
	నిర్మాణం పరోక్ష_Cmp {
		పరోక్ష_Cmp(స్థిరరాశి std::సదిశ<T> &nodes, స్థిరరాశి C &కణుపు_cmp) : కణుపు_cmp_2(కణుపు_cmp), nodes_(nodes) {}
		bool క్రియాకారకం()(పూర్ణాంకం a, పూర్ణాంకం b) స్థిరరాశి
		{
			log_assert(static_cast<కొలత_t>(a) < nodes_.కొలత());
			log_assert(static_cast<కొలత_t>(b) < nodes_.కొలత());
			తిరిగి కణుపు_cmp_2(nodes_[a], nodes_[b]);
		}
		స్థిరరాశి C కణుపు_cmp_2;
		స్థిరరాశి std::సదిశ<T> &nodes_;
	};

	bool analyze_loops;
	// The stability doesn't rely on std::less of T, so pointers are safe
	std::map<T, పూర్ణాంకం, C> కణుపు_గమ్యం_సూచీ;
	// edges[i] is the set of nodes with an edge into node i
	std::సదిశ<std::సమితి<పూర్ణాంకం, పరోక్ష_Cmp>> అంచులు;
	std::సదిశ<T> sorted;
	std::సమితి<std::సదిశ<T>> loops;

	Topo_క్రమీకరించు() requires std::is_default_constructible_v<C> : Topo_క్రమీకరించు(C()) {}
	explicit Topo_క్రమీకరించు(స్థిరరాశి C &cmp) : కణుపు_గమ్యం_సూచీ(cmp), పరోక్ష_cmp(nodes, cmp)
	{
		analyze_loops = true;
		దొరికింది_loops = false;
	}

	పూర్ణాంకం కణుపు(T n)
	{
		స్వయంచాలక rv = కణుపు_గమ్యం_సూచీ.emplace(n, static_cast<పూర్ణాంకం>(nodes.కొలత()));
		ఒకవేళ (rv.రెండవ) {
			nodes.నెట్టు_వెనుక(n);
			అంచులు.నెట్టు_వెనుక(std::సమితి<పూర్ణాంకం, పరోక్ష_Cmp>(పరోక్ష_cmp));
		}
		తిరిగి rv.మొదటి->రెండవ;
	}

	శూన్య అంచు(పూర్ణాంకం l_సూచీ, పూర్ణాంకం r_సూచీ) { అంచులు[r_సూచీ].చొప్పించు(l_సూచీ); }

	శూన్య అంచు(T ఎడమ_3, T కుడి_3) { అంచు(కణుపు(ఎడమ_3), కణుపు(కుడి_3)); }

	bool has_కణుపు(స్థిరరాశి T &కణుపు) { తిరిగి కణుపు_గమ్యం_సూచీ.కనుగొను(కణుపు) != కణుపు_గమ్యం_సూచీ.చివర(); }

	bool క్రమీకరించు()
	{
		log_assert(Get_కొలత(కణుపు_గమ్యం_సూచీ) == Get_కొలత(అంచులు));
		log_assert(Get_కొలత(nodes) == Get_కొలత(అంచులు));

		loops.clear();
		sorted.clear();
		దొరికింది_loops = false;

		std::సదిశ<bool> కణుపు_is_sorted(అంచులు.కొలత(), false);
		std::సదిశ<bool> కణుపు_is_on_దొంతర(అంచులు.కొలత(), false);
		// Only used with analyze_loops
		std::సదిశ<పూర్ణాంకం> దొంతర;
		sorted.reserve(అంచులు.కొలత());

		కొరకు (స్థిరరాశి స్వయంచాలక &it : కణుపు_గమ్యం_సూచీ)
			క్రమీకరించు_కార్మికుడు(it.రెండవ, కణుపు_is_sorted, కణుపు_is_on_దొంతర, దొంతర);

		log_assert(Get_కొలత(sorted) == Get_కొలత(nodes));

		తిరిగి !దొరికింది_loops;
	}

	// Build the more expensive representation of edges for
	// a few passes that use it directly.
	std::map<T, std::సమితి<T, C>, C> get_database()
	{
		std::map<T, std::సమితి<T, C>, C> database(కణుపు_గమ్యం_సూచీ.కీలకం_comp());
		కొరకు (కొలత_t i = 0; i < nodes.కొలత(); ++i) {
			std::సమితి<T, C> converted_అంచు_సమితి(కణుపు_గమ్యం_సూచీ.కీలకం_comp());
			కొరకు (పూర్ణాంకం other_కణుపు : అంచులు[i]) {
				converted_అంచు_సమితి.చొప్పించు(nodes[other_కణుపు]);
			}
			database.emplace(nodes[i], converted_అంచు_సమితి);
		}
		తిరిగి database;
	}

అంతరంగ:
	bool దొరికింది_loops;
	std::సదిశ<T> nodes;
	స్థిరరాశి పరోక్ష_Cmp పరోక్ష_cmp;

	శూన్య క్రమీకరించు_కార్మికుడు(స్థిరరాశి పూర్ణాంకం మూలం_సూచీ, std::సదిశ<bool> &కణుపు_is_sorted, std::సదిశ<bool> &కణుపు_is_on_దొంతర, std::సదిశ<పూర్ణాంకం> &దొంతర)
	{
		ఒకవేళ (కణుపు_is_on_దొంతర[మూలం_సూచీ]) {
			// We've been here before, meaning we have a loop
			దొరికింది_loops = true;
			ఒకవేళ (analyze_loops) {
				std::సదిశ<T> loop;
				కొరకు (పూర్ణాంకం i = Get_కొలత(దొంతర) - 1; i >= 0; i--) {
					స్థిరరాశి పూర్ణాంకం సూచీ = దొంతర[i];
					loop.నెట్టు_వెనుక(nodes[సూచీ]);
					ఒకవేళ (సూచీ == మూలం_సూచీ)
						ఆపు;
				}
				loops.చొప్పించు(loop);
			}
			తిరిగి;
		}

		// We're done if we've already sorted this subgraph
		ఒకవేళ (కణుపు_is_sorted[మూలం_సూచీ])
			తిరిగి;

		ఒకవేళ (!అంచులు[మూలం_సూచీ].ఖాళీలు()) {
			ఒకవేళ (analyze_loops)
				దొంతర.నెట్టు_వెనుక(మూలం_సూచీ);
			కణుపు_is_on_దొంతర[మూలం_సూచీ] = true;

			కొరకు (పూర్ణాంకం ఎడమ_n : అంచులు[మూలం_సూచీ])
				క్రమీకరించు_కార్మికుడు(ఎడమ_n, కణుపు_is_sorted, కణుపు_is_on_దొంతర, దొంతర);

			ఒకవేళ (analyze_loops)
				దొంతర.తీయి_వెనుక();
			కణుపు_is_on_దొంతర[మూలం_సూచీ] = false;
		}

		కణుపు_is_sorted[మూలం_సూచీ] = true;
		sorted.నెట్టు_వెనుక(nodes[మూలం_సూచీ]);
	}
};

// this class is used for implementing operator-> on iterators that return values rather than references
// it's necessary because in C++ operator-> is called recursively until a raw pointer is obtained
మూస<class T>
నిర్మాణం arrow_proxy {
	T v;
	explicit arrow_proxy(T స్థిరరాశి & v) : v(v) {}
	T* క్రియాకారకం->() { తిరిగి &v; }
};

అంతఃవరుస పూర్ణాంకం ceil_log2(పూర్ణాంకం x)
{
#ఒకవేళ defined(__GNUC__)
        తిరిగి x > 1 ? (8*పరిమాణం(పూర్ణాంకం)) - __builtin_clz(x-1) : 0;
#లేకపోతే
	ఒకవేళ (x <= 0)
		తిరిగి 0;
	కొరకు (పూర్ణాంకం i = 0; i < 32; i++)
		ఒకవేళ (((x-1) >> i) == 0)
			తిరిగి i;
	log_abort();
#అయితే_ముగింపు
}

మూస <typename T>
స్వయంచాలక reversed(T& container) {
	నిర్మాణం తిరగేయి_view {
		తిరగేయి_view(T& container) : container(container) {}
		స్వయంచాలక begin() స్థిరరాశి { తిరిగి container.rbegin(); }
		స్వయంచాలక చివర() స్థిరరాశి { తిరిగి container.rend(); }
		T& container;
	};
	తిరిగి తిరగేయి_view{container};
}

మూస <typename T>
స్వయంచాలక reversed(స్థిరరాశి T& container) {
	నిర్మాణం తిరగేయి_view {
		తిరగేయి_view(స్థిరరాశి T& container) : container(container) {}
		స్వయంచాలక begin() స్థిరరాశి { తిరిగి container.rbegin(); }
		స్వయంచాలక చివర() స్థిరరాశి { తిరిగి container.rend(); }
		స్థిరరాశి T& container;
	};
	తిరిగి తిరగేయి_view{container};
}

// A range of integers [start_, end_) that can be iterated over with a
// C++ range-based for loop.
నిర్మాణం IntRange {
	పూర్ణాంకం ప్రారంభం_2;
	పూర్ణాంకం చివర_4;
	నిర్మాణం Int {
		పూర్ణాంకం v;
		పూర్ణాంకం క్రియాకారకం*() స్థిరరాశి { తిరిగి v; }
		Int &క్రియాకారకం++() { ++v; తిరిగి *this; }
		bool క్రియాకారకం!=(స్థిరరాశి Int &other) స్థిరరాశి { తిరిగి v != other.v; }
	};
	Int begin() స్థిరరాశి { తిరిగి {ప్రారంభం_2}; }
	Int చివర() స్థిరరాశి { తిరిగి {చివర_4}; }

	bool క్రియాకారకం==(స్థిరరాశి IntRange &other) స్థిరరాశి { తిరిగి ప్రారంభం_2 == other.ప్రారంభం_2 && చివర_4 == other.చివర_4; }
	bool క్రియాకారకం!=(స్థిరరాశి IntRange &other) స్థిరరాశి { తిరిగి !(*this == other); }
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
