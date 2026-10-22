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

#جے_بنا_تعریف SIGTOOLS_H
#تعریف_کرو SIGTOOLS_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية SigPool
{
	بنية ثنائی_ہندسہ_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		ثنائی_ہندسہ_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		ثنائی_ہندسہ_Def_t(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) : std::pair<RTLIL::Wire*, صحيح>(ثنائی_ہندسہ.wire, ثنائی_ہندسہ.وتھ) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پہلا->name);
			h.eat(دوجا);
			أرجع h;
		}
	};

	pool<ثنائی_ہندسہ_Def_t> بٹ;

	فراغ clear()
	{
		بٹ.clear();
	}

	فراغ جوڑو(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم)
				بٹ.داخل_کرو(ثنائی_ہندسہ);
	}

	فراغ جوڑو(ثابت SigPool &other)
	{
		لكل (خودکار &ثنائی_ہندسہ : other.بٹ)
			بٹ.داخل_کرو(ثنائی_ہندسہ);
	}

	فراغ del(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم)
				بٹ.erase(ثنائی_ہندسہ);
	}

	فراغ del(ثابت SigPool &other)
	{
		لكل (خودکار &ثنائی_ہندسہ : other.بٹ)
			بٹ.erase(ثنائی_ہندسہ);
	}

	فراغ expand(ثابت RTLIL::SigSpec &from, ثابت RTLIL::SigSpec &منزل)
	{
		log_assert(Get_ناپ(from) == Get_ناپ(منزل));
		لكل (صحيح i = 0; i < Get_ناپ(from); i++) {
			ثنائی_ہندسہ_Def_t ثنائی_ہندسہ_from(from[i]), ثنائی_ہندسہ_منزل(منزل[i]);
			إذا (ثنائی_ہندسہ_from.پہلا != عدم && ثنائی_ہندسہ_منزل.پہلا != عدم && بٹ.گنتی(ثنائی_ہندسہ_from) > 0)
				بٹ.داخل_کرو(ثنائی_ہندسہ_منزل);
		}
	}

	RTLIL::SigSpec کڈھو(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتیجہ;
		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم && بٹ.گنتی(ثنائی_ہندسہ))
				نتیجہ.append(ثنائی_ہندسہ);
		أرجع نتیجہ;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتیجہ;
		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم && بٹ.گنتی(ثنائی_ہندسہ) == 0)
				نتیجہ.append(ثنائی_ہندسہ);
		أرجع نتیجہ;
	}

	bool check(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) ثابت
	{
		أرجع ثنائی_ہندسہ.wire != عدم && بٹ.گنتی(ثنائی_ہندسہ);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم && بٹ.گنتی(ثنائی_ہندسہ))
				أرجع true;
		أرجع false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم && بٹ.گنتی(ثنائی_ہندسہ) == 0)
				أرجع false;
		أرجع true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		لكل (خودکار &ثنائی_ہندسہ : بٹ)
			أرجع RTLIL::SigSpec(ثنائی_ہندسہ.پہلا, ثنائی_ہندسہ.دوجا);
		أرجع RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::Sig_ثنائی_ہندسہ> sig;
		لكل (خودکار &ثنائی_ہندسہ : بٹ)
			sig.داخل_کرو(RTLIL::Sig_ثنائی_ہندسہ(ثنائی_ہندسہ.پہلا, ثنائی_ہندسہ.دوجا));
		أرجع sig;
	}

	ناپ_t ناپ() ثابت
	{
		أرجع بٹ.ناپ();
	}
};

سانچا <typename T, class Compare = فراغ>
بنية Sig_سیٹ
{
	static_assert(!std::is_same<Compare,فراغ>::قدر, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	بنية ثنائی_ہندسہ_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		ثنائی_ہندسہ_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		ثنائی_ہندسہ_Def_t(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) : std::pair<RTLIL::Wire*, صحيح>(ثنائی_ہندسہ.wire, ثنائی_ہندسہ.وتھ) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پہلا->name);
			h.eat(دوجا);
			أرجع h;
		}
	};

	dict<ثنائی_ہندسہ_Def_t, std::سیٹ<T, Compare>> بٹ;

	فراغ clear()
	{
		بٹ.clear();
	}

	فراغ داخل_کرو(ثابت RTLIL::SigSpec &sig, T انکڑے)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم)
				بٹ[ثنائی_ہندسہ].داخل_کرو(انکڑے);
	}

	فراغ داخل_کرو(ثابت RTLIL::SigSpec& sig, ثابت std::سیٹ<T> &انکڑے)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم)
				بٹ[ثنائی_ہندسہ].داخل_کرو(انکڑے.begin(), انکڑے.سرا());
	}

	فراغ erase(ثابت RTLIL::SigSpec& sig)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم)
				بٹ[ثنائی_ہندسہ].clear();
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, T انکڑے)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم)
				بٹ[ثنائی_ہندسہ].erase(انکڑے);
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, ثابت std::سیٹ<T> &انکڑے)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم)
				بٹ[ثنائی_ہندسہ].erase(انکڑے.begin(), انکڑے.سرا());
	}

	فراغ لبھو(ثابت RTLIL::SigSpec &sig, std::سیٹ<T> &نتیجہ)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم) {
				خودکار &انکڑے = بٹ[ثنائی_ہندسہ];
				نتیجہ.داخل_کرو(انکڑے.begin(), انکڑے.سرا());
			}
	}

	فراغ لبھو(ثابت RTLIL::SigSpec &sig, pool<T> &نتیجہ)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم) {
				خودکار &انکڑے = بٹ[ثنائی_ہندسہ];
				نتیجہ.داخل_کرو(انکڑے.begin(), انکڑے.سرا());
			}
	}

	std::سیٹ<T> لبھو(ثابت RTLIL::SigSpec &sig)
	{
		std::سیٹ<T> نتیجہ;
		لبھو(sig, نتیجہ);
		أرجع نتیجہ;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire != عدم && بٹ.گنتی(ثنائی_ہندسہ))
				أرجع true;
		أرجع false;
	}
};

سانچا<typename T>
class Sig_سیٹ<T, typename std::enable_if<!std::is_اشارہ_گر<T>::قدر>::type> : عام Sig_سیٹ<T, std::less<T>> {};
سانچا<typename T>
using ترتیب_دیو_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::قدر>::type;
سانچا<typename T>
class Sig_سیٹ<T, ترتیب_دیو_by_name_id_guard<T>> : عام Sig_سیٹ<T, RTLIL::ترتیب_دیو_by_name_id<typename std::remove_اشارہ_گر<T>::type>> {};

بنية SigMapView
{
	mfp<Sig_ثنائی_ہندسہ> database;

	// Modify bit to its representative
	فراغ apply(RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) ثابت
	{
		ثنائی_ہندسہ = database.لبھو(ثنائی_ہندسہ);
	}

	فراغ apply(RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودکار &ثنائی_ہندسہ : sig)
			apply(ثنائی_ہندسہ);
	}

	RTLIL::Sig_ثنائی_ہندسہ عامل()(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ) ثابت
	{
		apply(ثنائی_ہندسہ);
		أرجع ثنائی_ہندسہ;
	}

	RTLIL::SigSpec عامل()(RTLIL::SigSpec sig) ثابت
	{
		apply(sig);
		أرجع sig;
	}

	RTLIL::SigSpec عامل()(RTLIL::Wire *wire) ثابت
	{
		SigSpec sig(wire);
		apply(sig);
		أرجع sig;
	}
};

/**
 * SigMap wraps a union-find "database"
 * to map SigBits of a module to canonical representative SigBits.
 * SigBits that are connected share a set in the underlying database.
 * If a SigBit has a const state (impl: bit.wire is nullptr),
 * it's promoted to a representative.
 */
بنية SigMap final : عام SigMapView
{
	SigMap(RTLIL::Module *module = عدم)
	{
		إذا (module != عدم)
			سیٹ(module);
	}

	فراغ وٹاندرا(SigMap &other)
	{
		database.وٹاندرا(other.database);
	}

	فراغ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ سیٹ(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خودکار &it : module->connections())
			bitcount += it.پہلا.ناپ();

		database.clear();
		database.reserve(bitcount);

		لكل (خودکار &it : module->connections())
			جوڑو(it.پہلا, it.دوجا);
	}

	// Add connections from "from" to "to", bit-by-bit
	فراغ جوڑو(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& منزل)
	{
		log_assert(Get_ناپ(from) == Get_ناپ(منزل));

		لكل (صحيح i = 0; i < Get_ناپ(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(منزل[i]);

			ثابت RTLIL::Sig_ثنائی_ہندسہ &bf = database[bfi];
			ثابت RTLIL::Sig_ثنائی_ہندسہ &bt = database[bti];

			إذا (bf.wire || bt.wire)
			{
				database.imerge(bfi, bti);

				إذا (bf.wire == nullptr)
					database.ipromote(bfi);

				إذا (bt.wire == nullptr)
					database.ipromote(bti);
			}
		}
	}

	// Add sig as disconnected from anything
	فراغ جوڑو(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ)
	{
		ثابت خودکار &b = database.لبھو(ثنائی_ہندسہ);
		إذا (b.wire != nullptr)
			database.promote(ثنائی_ہندسہ);
	}

	فراغ جوڑو(ثابت RTLIL::SigSpec &sig)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			جوڑو(ثنائی_ہندسہ);
	}

	مضمن فراغ جوڑو(Wire *wire) { أرجع جوڑو(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		لكل (ثابت خودکار &ثنائی_ہندسہ : database)
			إذا (ثنائی_ہندسہ.wire != nullptr)
				sig.append(ثنائی_ہندسہ);
		أرجع sig;
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
سانچا <class Val>
بنية SigValMap final : عام SigMapView
{
	dict<Sig_ثنائی_ہندسہ, Val> اقدار;

	فراغ وٹاندرا(SigValMap<Val> &other)
	{
		database.وٹاندرا(other.database);
		اقدار.وٹاندرا(other.اقدار);
	}

	فراغ clear()
	{
		database.clear();
		اقدار.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ سیٹ(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خودکار &it : module->connections())
			bitcount += it.پہلا.ناپ();

		database.clear();
		اقدار.clear();
		database.reserve(bitcount);

		لكل (خودکار &it : module->connections())
			جوڑو(it.پہلا, it.دوجا);
	}

	// Add connections from "from" to "to", bit-by-bit.
	فراغ جوڑو(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& منزل)
	{
		log_assert(Get_ناپ(from) == Get_ناپ(منزل));

		لكل (صحيح i = 0; i < Get_ناپ(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(منزل[i]);
			إذا (bfi == bti) {
				استمر;
			}

			ثابت RTLIL::Sig_ثنائی_ہندسہ &bf = database[bfi];
			ثابت RTLIL::Sig_ثنائی_ہندسہ &bt = database[bti];
			إذا (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				ملاؤ_قدر(bt, bf);
			} وإلا {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				ملاؤ_قدر(bf, bt);
			}
		}
	}

	فراغ جوڑو_Val(ثابت RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ, ثابت Val &val)
	{
		اقدار[database.لبھو(ثنائی_ہندسہ)] |= val;
	}

	فراغ جوڑو_Val(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		لكل (ثابت خودکار &ثنائی_ہندسہ : sig)
			جوڑو_Val(ثنائی_ہندسہ, val);
	}

	Val apply_and_get_قدر(RTLIL::Sig_ثنائی_ہندسہ &ثنائی_ہندسہ) ثابت
	{
		ثنائی_ہندسہ = database.لبھو(ثنائی_ہندسہ);
		خودکار it = اقدار.لبھو(ثنائی_ہندسہ);
		أرجع it == اقدار.سرا() ? Val() : it->دوجا;
	}

خاص:
	فراغ ملاؤ_قدر(ثابت RTLIL::Sig_ثنائی_ہندسہ &from, ثابت RTLIL::Sig_ثنائی_ہندسہ &منزل)
	{
		خودکار it = اقدار.لبھو(from);
		إذا (it == اقدار.سرا()) {
			أرجع;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->دوجا;
		اقدار.erase(it);
		اقدار[منزل] |= v;
	}
};

YOSYS_NAMESPACE_سرا

#جے_انت /* SIGTOOLS_H */
