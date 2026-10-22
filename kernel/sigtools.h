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

#నిర్వచితం_కాకపోతే SIGTOOLS_H
#నిర్వచించు SIGTOOLS_H

#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం SigPool
{
	నిర్మాణం బిట్టు_Def_t : బహిరంగ std::pair<RTLIL::Wire*, పూర్ణాంకం> {
		బిట్టు_Def_t() : std::pair<RTLIL::Wire*, పూర్ణాంకం>(ఖాళీ, 0) { }
		బిట్టు_Def_t(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు) : std::pair<RTLIL::Wire*, పూర్ణాంకం>(బిట్టు.wire, బిట్టు.స్థానభ్రంశం) { }
		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
			h.eat(మొదటి->name);
			h.eat(రెండవ);
			తిరిగి h;
		}
	};

	pool<బిట్టు_Def_t> బిట్లు;

	శూన్య clear()
	{
		బిట్లు.clear();
	}

	శూన్య కూడు(స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				బిట్లు.చొప్పించు(బిట్టు);
	}

	శూన్య కూడు(స్థిరరాశి SigPool &other)
	{
		కొరకు (స్వయంచాలక &బిట్టు : other.బిట్లు)
			బిట్లు.చొప్పించు(బిట్టు);
	}

	శూన్య del(స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				బిట్లు.erase(బిట్టు);
	}

	శూన్య del(స్థిరరాశి SigPool &other)
	{
		కొరకు (స్వయంచాలక &బిట్టు : other.బిట్లు)
			బిట్లు.erase(బిట్టు);
	}

	శూన్య expand(స్థిరరాశి RTLIL::SigSpec &from, స్థిరరాశి RTLIL::SigSpec &గమ్యం)
	{
		log_assert(Get_కొలత(from) == Get_కొలత(గమ్యం));
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(from); i++) {
			బిట్టు_Def_t బిట్టు_from(from[i]), బిట్టు_గమ్యం(గమ్యం[i]);
			ఒకవేళ (బిట్టు_from.మొదటి != ఖాళీ && బిట్టు_గమ్యం.మొదటి != ఖాళీ && బిట్లు.లెక్క(బిట్టు_from) > 0)
				బిట్లు.చొప్పించు(బిట్టు_గమ్యం);
		}
	}

	RTLIL::SigSpec వెలికితీయి(స్థిరరాశి RTLIL::SigSpec &sig) స్థిరరాశి
	{
		RTLIL::SigSpec ఫలితం;
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ && బిట్లు.లెక్క(బిట్టు))
				ఫలితం.append(బిట్టు);
		తిరిగి ఫలితం;
	}

	RTLIL::SigSpec remove(స్థిరరాశి RTLIL::SigSpec &sig) స్థిరరాశి
	{
		RTLIL::SigSpec ఫలితం;
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ && బిట్లు.లెక్క(బిట్టు) == 0)
				ఫలితం.append(బిట్టు);
		తిరిగి ఫలితం;
	}

	bool check(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు) స్థిరరాశి
	{
		తిరిగి బిట్టు.wire != ఖాళీ && బిట్లు.లెక్క(బిట్టు);
	}

	bool check_any(స్థిరరాశి RTLIL::SigSpec &sig) స్థిరరాశి
	{
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ && బిట్లు.లెక్క(బిట్టు))
				తిరిగి true;
		తిరిగి false;
	}

	bool check_all(స్థిరరాశి RTLIL::SigSpec &sig) స్థిరరాశి
	{
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ && బిట్లు.లెక్క(బిట్టు) == 0)
				తిరిగి false;
		తిరిగి true;
	}

	RTLIL::SigSpec export_one() స్థిరరాశి
	{
		కొరకు (స్వయంచాలక &బిట్టు : బిట్లు)
			తిరిగి RTLIL::SigSpec(బిట్టు.మొదటి, బిట్టు.రెండవ);
		తిరిగి RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() స్థిరరాశి
	{
		pool<RTLIL::Sig_బిట్టు> sig;
		కొరకు (స్వయంచాలక &బిట్టు : బిట్లు)
			sig.చొప్పించు(RTLIL::Sig_బిట్టు(బిట్టు.మొదటి, బిట్టు.రెండవ));
		తిరిగి sig;
	}

	కొలత_t కొలత() స్థిరరాశి
	{
		తిరిగి బిట్లు.కొలత();
	}
};

మూస <typename T, class Compare = శూన్య>
నిర్మాణం Sig_సమితి
{
	static_assert(!std::is_same<Compare,శూన్య>::విలువ, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	నిర్మాణం బిట్టు_Def_t : బహిరంగ std::pair<RTLIL::Wire*, పూర్ణాంకం> {
		బిట్టు_Def_t() : std::pair<RTLIL::Wire*, పూర్ణాంకం>(ఖాళీ, 0) { }
		బిట్టు_Def_t(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు) : std::pair<RTLIL::Wire*, పూర్ణాంకం>(బిట్టు.wire, బిట్టు.స్థానభ్రంశం) { }
		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
			h.eat(మొదటి->name);
			h.eat(రెండవ);
			తిరిగి h;
		}
	};

	dict<బిట్టు_Def_t, std::సమితి<T, Compare>> బిట్లు;

	శూన్య clear()
	{
		బిట్లు.clear();
	}

	శూన్య చొప్పించు(స్థిరరాశి RTLIL::SigSpec &sig, T దత్తాంశం)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				బిట్లు[బిట్టు].చొప్పించు(దత్తాంశం);
	}

	శూన్య చొప్పించు(స్థిరరాశి RTLIL::SigSpec& sig, స్థిరరాశి std::సమితి<T> &దత్తాంశం)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				బిట్లు[బిట్టు].చొప్పించు(దత్తాంశం.begin(), దత్తాంశం.చివర());
	}

	శూన్య erase(స్థిరరాశి RTLIL::SigSpec& sig)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				బిట్లు[బిట్టు].clear();
	}

	శూన్య erase(స్థిరరాశి RTLIL::SigSpec &sig, T దత్తాంశం)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				బిట్లు[బిట్టు].erase(దత్తాంశం);
	}

	శూన్య erase(స్థిరరాశి RTLIL::SigSpec &sig, స్థిరరాశి std::సమితి<T> &దత్తాంశం)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ)
				బిట్లు[బిట్టు].erase(దత్తాంశం.begin(), దత్తాంశం.చివర());
	}

	శూన్య కనుగొను(స్థిరరాశి RTLIL::SigSpec &sig, std::సమితి<T> &ఫలితం)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ) {
				స్వయంచాలక &దత్తాంశం = బిట్లు[బిట్టు];
				ఫలితం.చొప్పించు(దత్తాంశం.begin(), దత్తాంశం.చివర());
			}
	}

	శూన్య కనుగొను(స్థిరరాశి RTLIL::SigSpec &sig, pool<T> &ఫలితం)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ) {
				స్వయంచాలక &దత్తాంశం = బిట్లు[బిట్టు];
				ఫలితం.చొప్పించు(దత్తాంశం.begin(), దత్తాంశం.చివర());
			}
	}

	std::సమితి<T> కనుగొను(స్థిరరాశి RTLIL::SigSpec &sig)
	{
		std::సమితి<T> ఫలితం;
		కనుగొను(sig, ఫలితం);
		తిరిగి ఫలితం;
	}

	bool has(స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire != ఖాళీ && బిట్లు.లెక్క(బిట్టు))
				తిరిగి true;
		తిరిగి false;
	}
};

మూస<typename T>
class Sig_సమితి<T, typename std::enable_if<!std::is_సూచిక<T>::విలువ>::type> : బహిరంగ Sig_సమితి<T, std::less<T>> {};
మూస<typename T>
using క్రమీకరించు_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::విలువ>::type;
మూస<typename T>
class Sig_సమితి<T, క్రమీకరించు_by_name_id_guard<T>> : బహిరంగ Sig_సమితి<T, RTLIL::క్రమీకరించు_by_name_id<typename std::remove_సూచిక<T>::type>> {};

నిర్మాణం SigMapView
{
	mfp<Sig_బిట్టు> database;

	// Modify bit to its representative
	శూన్య apply(RTLIL::Sig_బిట్టు &బిట్టు) స్థిరరాశి
	{
		బిట్టు = database.కనుగొను(బిట్టు);
	}

	శూన్య apply(RTLIL::SigSpec &sig) స్థిరరాశి
	{
		కొరకు (స్వయంచాలక &బిట్టు : sig)
			apply(బిట్టు);
	}

	RTLIL::Sig_బిట్టు క్రియాకారకం()(RTLIL::Sig_బిట్టు బిట్టు) స్థిరరాశి
	{
		apply(బిట్టు);
		తిరిగి బిట్టు;
	}

	RTLIL::SigSpec క్రియాకారకం()(RTLIL::SigSpec sig) స్థిరరాశి
	{
		apply(sig);
		తిరిగి sig;
	}

	RTLIL::SigSpec క్రియాకారకం()(RTLIL::Wire *wire) స్థిరరాశి
	{
		SigSpec sig(wire);
		apply(sig);
		తిరిగి sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
నిర్మాణం SigMap final : బహిరంగ SigMapView
{
	SigMap(RTLIL::Module *module = ఖాళీ)
	{
		ఒకవేళ (module != ఖాళీ)
			సమితి(module);
	}

	శూన్య మార్పిడి(SigMap &other)
	{
		database.మార్పిడి(other.database);
	}

	శూన్య clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	శూన్య సమితి(RTLIL::Module *module)
	{
		పూర్ణాంకం bitcount = 0;
		కొరకు (స్వయంచాలక &it : module->connections())
			bitcount += it.మొదటి.కొలత();

		database.clear();
		database.reserve(bitcount);

		కొరకు (స్వయంచాలక &it : module->connections())
			కూడు(it.మొదటి, it.రెండవ);
	}

	// Add connections from "from" to "to", bit-by-bit
	శూన్య కూడు(స్థిరరాశి RTLIL::SigSpec& from, స్థిరరాశి RTLIL::SigSpec& గమ్యం)
	{
		log_assert(Get_కొలత(from) == Get_కొలత(గమ్యం));

		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(from); i++)
		{
			పూర్ణాంకం bfi = database.lookup(from[i]);
			పూర్ణాంకం bti = database.lookup(గమ్యం[i]);

			స్థిరరాశి RTLIL::Sig_బిట్టు &bf = database[bfi];
			స్థిరరాశి RTLIL::Sig_బిట్టు &bt = database[bti];

			ఒకవేళ (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				ఒకవేళ (bf.wire == nullptr)
					database.ipromote(bfi);

				ఒకవేళ (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	శూన్య కూడు(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు)
	{
		స్థిరరాశి స్వయంచాలక &b = database.కనుగొను(బిట్టు);
		ఒకవేళ (b.wire != nullptr)
			database.promote(బిట్టు);
	}

	శూన్య కూడు(స్థిరరాశి RTLIL::SigSpec &sig)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			కూడు(బిట్టు);
	}

	అంతఃవరుస శూన్య కూడు(Wire *wire) { తిరిగి కూడు(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() స్థిరరాశి
	{
		RTLIL::SigSpec sig;
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : database)
			ఒకవేళ (బిట్టు.wire != nullptr)
				sig.append(బిట్టు);
		తిరిగి sig;
	}
};

/**
 * SiValgMap wraps a union-find "database" to map SigBits of a module to
 * canonical representative SigBits plus some optional Val value associated with the bits.
 * Val has a commutative, associative, idempotent operator|=, a default constructor
 * which constructs an identity element, and a copy constructor.
 * SigBits that are connected share a set in the underlying database;
 * the associated value is the "sum" of all the values associated with the contributing bits.
 * If any of the SigBits in a set are a constant, the canonical SigBit is a constant.
 */
మూస <class Val>
నిర్మాణం SigValMap final : బహిరంగ SigMapView
{
	dict<Sig_బిట్టు, Val> విలువలు;

	శూన్య మార్పిడి(SigValMap<Val> &other)
	{
		database.మార్పిడి(other.database);
		విలువలు.మార్పిడి(other.విలువలు);
	}

	శూన్య clear()
	{
		database.clear();
		విలువలు.clear();
	}

	// Rebuild SigMap for all connections in module
	శూన్య సమితి(RTLIL::Module *module)
	{
		పూర్ణాంకం bitcount = 0;
		కొరకు (స్వయంచాలక &it : module->connections())
			bitcount += it.మొదటి.కొలత();

		database.clear();
		విలువలు.clear();
		database.reserve(bitcount);

		కొరకు (స్వయంచాలక &it : module->connections())
			కూడు(it.మొదటి, it.రెండవ);
	}

	// Add connections from "from" to "to", bit-by-bit.
	శూన్య కూడు(స్థిరరాశి RTLIL::SigSpec& from, స్థిరరాశి RTLIL::SigSpec& గమ్యం)
	{
		log_assert(Get_కొలత(from) == Get_కొలత(గమ్యం));

		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(from); i++)
		{
			పూర్ణాంకం bfi = database.lookup(from[i]);
			పూర్ణాంకం bti = database.lookup(గమ్యం[i]);
			ఒకవేళ (bfi == bti) {
				కొనసాగు;
			}

			స్థిరరాశి RTLIL::Sig_బిట్టు &bf = database[bfi];
			స్థిరరాశి RTLIL::Sig_బిట్టు &bt = database[bti];
			ఒకవేళ (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				విలీనం_విలువ(bt, bf);
			} లేకపోతే {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				విలీనం_విలువ(bf, bt);
			}
		}
	}

	శూన్య కూడు_Val(స్థిరరాశి RTLIL::Sig_బిట్టు &బిట్టు, స్థిరరాశి Val &val)
	{
		విలువలు[database.కనుగొను(బిట్టు)] |= val;
	}

	శూన్య కూడు_Val(స్థిరరాశి RTLIL::SigSpec &sig, స్థిరరాశి Val &val)
	{
		కొరకు (స్థిరరాశి స్వయంచాలక &బిట్టు : sig)
			కూడు_Val(బిట్టు, val);
	}

	Val apply_and_get_విలువ(RTLIL::Sig_బిట్టు &బిట్టు) స్థిరరాశి
	{
		బిట్టు = database.కనుగొను(బిట్టు);
		స్వయంచాలక it = విలువలు.కనుగొను(బిట్టు);
		తిరిగి it == విలువలు.చివర() ? Val() : it->రెండవ;
	}

అంతరంగ:
	శూన్య విలీనం_విలువ(స్థిరరాశి RTLIL::Sig_బిట్టు &from, స్థిరరాశి RTLIL::Sig_బిట్టు &గమ్యం)
	{
		స్వయంచాలక it = విలువలు.కనుగొను(from);
		ఒకవేళ (it == విలువలు.చివర()) {
			తిరిగి;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->రెండవ;
		విలువలు.erase(it);
		విలువలు[గమ్యం] |= v;
	}
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు /* SIGTOOLS_H */
