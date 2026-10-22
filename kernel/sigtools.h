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

#جيڪڏهن_وصف_نه_ٿيل SIGTOOLS_H
#وصف_ڪريو SIGTOOLS_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية SigPool
{
	بنية ٻٽو_انگ_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		ٻٽو_انگ_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		ٻٽو_انگ_Def_t(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ) : std::pair<RTLIL::Wire*, صحيح>(ٻٽو_انگ.wire, ٻٽو_انگ.وٿي) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پهريون->name);
			h.eat(ٻيو);
			أرجع h;
		}
	};

	pool<ٻٽو_انگ_Def_t> بٽ;

	فراغ clear()
	{
		بٽ.clear();
	}

	فراغ جوڙيو(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم)
				بٽ.وجهو(ٻٽو_انگ);
	}

	فراغ جوڙيو(ثابت SigPool &other)
	{
		لكل (خودڪار &ٻٽو_انگ : other.بٽ)
			بٽ.وجهو(ٻٽو_انگ);
	}

	فراغ del(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم)
				بٽ.erase(ٻٽو_انگ);
	}

	فراغ del(ثابت SigPool &other)
	{
		لكل (خودڪار &ٻٽو_انگ : other.بٽ)
			بٽ.erase(ٻٽو_انگ);
	}

	فراغ expand(ثابت RTLIL::SigSpec &from, ثابت RTLIL::SigSpec &منزل)
	{
		log_assert(Get_سائيز(from) == Get_سائيز(منزل));
		لكل (صحيح i = 0; i < Get_سائيز(from); i++) {
			ٻٽو_انگ_Def_t ٻٽو_انگ_from(from[i]), ٻٽو_انگ_منزل(منزل[i]);
			إذا (ٻٽو_انگ_from.پهريون != عدم && ٻٽو_انگ_منزل.پهريون != عدم && بٽ.ڳڻپ(ٻٽو_انگ_from) > 0)
				بٽ.وجهو(ٻٽو_انگ_منزل);
		}
	}

	RTLIL::SigSpec ڪڍو_2(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتيجو;
		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم && بٽ.ڳڻپ(ٻٽو_انگ))
				نتيجو.append(ٻٽو_انگ);
		أرجع نتيجو;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتيجو;
		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم && بٽ.ڳڻپ(ٻٽو_انگ) == 0)
				نتيجو.append(ٻٽو_انگ);
		أرجع نتيجو;
	}

	bool check(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ) ثابت
	{
		أرجع ٻٽو_انگ.wire != عدم && بٽ.ڳڻپ(ٻٽو_انگ);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم && بٽ.ڳڻپ(ٻٽو_انگ))
				أرجع true;
		أرجع false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم && بٽ.ڳڻپ(ٻٽو_انگ) == 0)
				أرجع false;
		أرجع true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		لكل (خودڪار &ٻٽو_انگ : بٽ)
			أرجع RTLIL::SigSpec(ٻٽو_انگ.پهريون, ٻٽو_انگ.ٻيو);
		أرجع RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::Sig_ٻٽو_انگ> sig;
		لكل (خودڪار &ٻٽو_انگ : بٽ)
			sig.وجهو(RTLIL::Sig_ٻٽو_انگ(ٻٽو_انگ.پهريون, ٻٽو_انگ.ٻيو));
		أرجع sig;
	}

	سائيز_t سائيز() ثابت
	{
		أرجع بٽ.سائيز();
	}
};

سانچو <typename T, class Compare = فراغ>
بنية Sig_سيٽ
{
	static_assert(!std::is_same<Compare,فراغ>::قدر, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	بنية ٻٽو_انگ_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		ٻٽو_انگ_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		ٻٽو_انگ_Def_t(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ) : std::pair<RTLIL::Wire*, صحيح>(ٻٽو_انگ.wire, ٻٽو_انگ.وٿي) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(پهريون->name);
			h.eat(ٻيو);
			أرجع h;
		}
	};

	dict<ٻٽو_انگ_Def_t, std::سيٽ<T, Compare>> بٽ;

	فراغ clear()
	{
		بٽ.clear();
	}

	فراغ وجهو(ثابت RTLIL::SigSpec &sig, T معلومات)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم)
				بٽ[ٻٽو_انگ].وجهو(معلومات);
	}

	فراغ وجهو(ثابت RTLIL::SigSpec& sig, ثابت std::سيٽ<T> &معلومات)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم)
				بٽ[ٻٽو_انگ].وجهو(معلومات.begin(), معلومات.پڇاڙي());
	}

	فراغ erase(ثابت RTLIL::SigSpec& sig)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم)
				بٽ[ٻٽو_انگ].clear();
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, T معلومات)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم)
				بٽ[ٻٽو_انگ].erase(معلومات);
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, ثابت std::سيٽ<T> &معلومات)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم)
				بٽ[ٻٽو_انگ].erase(معلومات.begin(), معلومات.پڇاڙي());
	}

	فراغ ڳوليو(ثابت RTLIL::SigSpec &sig, std::سيٽ<T> &نتيجو)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم) {
				خودڪار &معلومات = بٽ[ٻٽو_انگ];
				نتيجو.وجهو(معلومات.begin(), معلومات.پڇاڙي());
			}
	}

	فراغ ڳوليو(ثابت RTLIL::SigSpec &sig, pool<T> &نتيجو)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم) {
				خودڪار &معلومات = بٽ[ٻٽو_انگ];
				نتيجو.وجهو(معلومات.begin(), معلومات.پڇاڙي());
			}
	}

	std::سيٽ<T> ڳوليو(ثابت RTLIL::SigSpec &sig)
	{
		std::سيٽ<T> نتيجو;
		ڳوليو(sig, نتيجو);
		أرجع نتيجو;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire != عدم && بٽ.ڳڻپ(ٻٽو_انگ))
				أرجع true;
		أرجع false;
	}
};

سانچو<typename T>
class Sig_سيٽ<T, typename std::enable_if<!std::is_اشارو<T>::قدر>::type> : عام Sig_سيٽ<T, std::less<T>> {};
سانچو<typename T>
using ترتيب_ڏيو_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::قدر>::type;
سانچو<typename T>
class Sig_سيٽ<T, ترتيب_ڏيو_by_name_id_guard<T>> : عام Sig_سيٽ<T, RTLIL::ترتيب_ڏيو_by_name_id<typename std::remove_اشارو<T>::type>> {};

بنية SigMapView
{
	mfp<Sig_ٻٽو_انگ> database;

	// Modify bit to its representative
	فراغ apply(RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ) ثابت
	{
		ٻٽو_انگ = database.ڳوليو(ٻٽو_انگ);
	}

	فراغ apply(RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودڪار &ٻٽو_انگ : sig)
			apply(ٻٽو_انگ);
	}

	RTLIL::Sig_ٻٽو_انگ عامل()(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ) ثابت
	{
		apply(ٻٽو_انگ);
		أرجع ٻٽو_انگ;
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
			سيٽ(module);
	}

	فراغ مٽا_سٽا(SigMap &other)
	{
		database.مٽا_سٽا(other.database);
	}

	فراغ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ سيٽ(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خودڪار &it : module->connections())
			bitcount += it.پهريون.سائيز();

		database.clear();
		database.reserve(bitcount);

		لكل (خودڪار &it : module->connections())
			جوڙيو(it.پهريون, it.ٻيو);
	}

	// Add connections from "from" to "to", bit-by-bit
	فراغ جوڙيو(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& منزل)
	{
		log_assert(Get_سائيز(from) == Get_سائيز(منزل));

		لكل (صحيح i = 0; i < Get_سائيز(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(منزل[i]);

			ثابت RTLIL::Sig_ٻٽو_انگ &bf = database[bfi];
			ثابت RTLIL::Sig_ٻٽو_انگ &bt = database[bti];

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
	فراغ جوڙيو(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ)
	{
		ثابت خودڪار &b = database.ڳوليو(ٻٽو_انگ);
		إذا (b.wire != nullptr)
			database.promote(ٻٽو_انگ);
	}

	فراغ جوڙيو(ثابت RTLIL::SigSpec &sig)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			جوڙيو(ٻٽو_انگ);
	}

	مضمن فراغ جوڙيو(Wire *wire) { أرجع جوڙيو(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		لكل (ثابت خودڪار &ٻٽو_انگ : database)
			إذا (ٻٽو_انگ.wire != nullptr)
				sig.append(ٻٽو_انگ);
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
سانچو <class Val>
بنية SigValMap final : عام SigMapView
{
	dict<Sig_ٻٽو_انگ, Val> قدرون;

	فراغ مٽا_سٽا(SigValMap<Val> &other)
	{
		database.مٽا_سٽا(other.database);
		قدرون.مٽا_سٽا(other.قدرون);
	}

	فراغ clear()
	{
		database.clear();
		قدرون.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ سيٽ(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خودڪار &it : module->connections())
			bitcount += it.پهريون.سائيز();

		database.clear();
		قدرون.clear();
		database.reserve(bitcount);

		لكل (خودڪار &it : module->connections())
			جوڙيو(it.پهريون, it.ٻيو);
	}

	// Add connections from "from" to "to", bit-by-bit.
	فراغ جوڙيو(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& منزل)
	{
		log_assert(Get_سائيز(from) == Get_سائيز(منزل));

		لكل (صحيح i = 0; i < Get_سائيز(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(منزل[i]);
			إذا (bfi == bti) {
				استمر;
			}

			ثابت RTLIL::Sig_ٻٽو_انگ &bf = database[bfi];
			ثابت RTLIL::Sig_ٻٽو_انگ &bt = database[bti];
			إذا (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				ملايو_قدر(bt, bf);
			} وإلا {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				ملايو_قدر(bf, bt);
			}
		}
	}

	فراغ جوڙيو_Val(ثابت RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ, ثابت Val &val)
	{
		قدرون[database.ڳوليو(ٻٽو_انگ)] |= val;
	}

	فراغ جوڙيو_Val(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		لكل (ثابت خودڪار &ٻٽو_انگ : sig)
			جوڙيو_Val(ٻٽو_انگ, val);
	}

	Val apply_and_get_قدر(RTLIL::Sig_ٻٽو_انگ &ٻٽو_انگ) ثابت
	{
		ٻٽو_انگ = database.ڳوليو(ٻٽو_انگ);
		خودڪار it = قدرون.ڳوليو(ٻٽو_انگ);
		أرجع it == قدرون.پڇاڙي() ? Val() : it->ٻيو;
	}

خاص:
	فراغ ملايو_قدر(ثابت RTLIL::Sig_ٻٽو_انگ &from, ثابت RTLIL::Sig_ٻٽو_انگ &منزل)
	{
		خودڪار it = قدرون.ڳوليو(from);
		إذا (it == قدرون.پڇاڙي()) {
			أرجع;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->ٻيو;
		قدرون.erase(it);
		قدرون[منزل] |= v;
	}
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو /* SIGTOOLS_H */
