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

#که_نه_تعریف_شوی SIGTOOLS_H
#تعریف_کړه SIGTOOLS_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية SigPool
{
	بنية بټ_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		بټ_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		بټ_Def_t(ثابت RTLIL::Sig_بټ &بټ) : std::pair<RTLIL::Wire*, صحيح>(بټ.wire, بټ.دننه_واټن) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(لومړی->name);
			h.eat(دوهم);
			أرجع h;
		}
	};

	pool<بټ_Def_t> بټونه;

	فراغ clear()
	{
		بټونه.clear();
	}

	فراغ جمع_کړه(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم)
				بټونه.ورننباسه(بټ);
	}

	فراغ جمع_کړه(ثابت SigPool &other)
	{
		لكل (خپلکاری &بټ : other.بټونه)
			بټونه.ورننباسه(بټ);
	}

	فراغ del(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم)
				بټونه.erase(بټ);
	}

	فراغ del(ثابت SigPool &other)
	{
		لكل (خپلکاری &بټ : other.بټونه)
			بټونه.erase(بټ);
	}

	فراغ expand(ثابت RTLIL::SigSpec &from, ثابت RTLIL::SigSpec &منزل)
	{
		log_assert(Get_اندازه(from) == Get_اندازه(منزل));
		لكل (صحيح i = 0; i < Get_اندازه(from); i++) {
			بټ_Def_t بټ_from(from[i]), بټ_منزل(منزل[i]);
			إذا (بټ_from.لومړی != عدم && بټ_منزل.لومړی != عدم && بټونه.شمېر(بټ_from) > 0)
				بټونه.ورننباسه(بټ_منزل);
		}
	}

	RTLIL::SigSpec ایستل(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec پایله;
		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم && بټونه.شمېر(بټ))
				پایله.append(بټ);
		أرجع پایله;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec پایله;
		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم && بټونه.شمېر(بټ) == 0)
				پایله.append(بټ);
		أرجع پایله;
	}

	bool check(ثابت RTLIL::Sig_بټ &بټ) ثابت
	{
		أرجع بټ.wire != عدم && بټونه.شمېر(بټ);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم && بټونه.شمېر(بټ))
				أرجع true;
		أرجع false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم && بټونه.شمېر(بټ) == 0)
				أرجع false;
		أرجع true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		لكل (خپلکاری &بټ : بټونه)
			أرجع RTLIL::SigSpec(بټ.لومړی, بټ.دوهم);
		أرجع RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::Sig_بټ> sig;
		لكل (خپلکاری &بټ : بټونه)
			sig.ورننباسه(RTLIL::Sig_بټ(بټ.لومړی, بټ.دوهم));
		أرجع sig;
	}

	اندازه_t اندازه() ثابت
	{
		أرجع بټونه.اندازه();
	}
};

کينډۍ <typename T, class Compare = فراغ>
بنية Sig_سېټ
{
	static_assert(!std::is_same<Compare,فراغ>::ارزښت, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	بنية بټ_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		بټ_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		بټ_Def_t(ثابت RTLIL::Sig_بټ &بټ) : std::pair<RTLIL::Wire*, صحيح>(بټ.wire, بټ.دننه_واټن) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(لومړی->name);
			h.eat(دوهم);
			أرجع h;
		}
	};

	dict<بټ_Def_t, std::سېټ<T, Compare>> بټونه;

	فراغ clear()
	{
		بټونه.clear();
	}

	فراغ ورننباسه(ثابت RTLIL::SigSpec &sig, T معلومات)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم)
				بټونه[بټ].ورننباسه(معلومات);
	}

	فراغ ورننباسه(ثابت RTLIL::SigSpec& sig, ثابت std::سېټ<T> &معلومات)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم)
				بټونه[بټ].ورننباسه(معلومات.begin(), معلومات.پای_ټکی());
	}

	فراغ erase(ثابت RTLIL::SigSpec& sig)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم)
				بټونه[بټ].clear();
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, T معلومات)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم)
				بټونه[بټ].erase(معلومات);
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, ثابت std::سېټ<T> &معلومات)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم)
				بټونه[بټ].erase(معلومات.begin(), معلومات.پای_ټکی());
	}

	فراغ پیدا_کړه(ثابت RTLIL::SigSpec &sig, std::سېټ<T> &پایله)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم) {
				خپلکاری &معلومات = بټونه[بټ];
				پایله.ورننباسه(معلومات.begin(), معلومات.پای_ټکی());
			}
	}

	فراغ پیدا_کړه(ثابت RTLIL::SigSpec &sig, pool<T> &پایله)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم) {
				خپلکاری &معلومات = بټونه[بټ];
				پایله.ورننباسه(معلومات.begin(), معلومات.پای_ټکی());
			}
	}

	std::سېټ<T> پیدا_کړه(ثابت RTLIL::SigSpec &sig)
	{
		std::سېټ<T> پایله;
		پیدا_کړه(sig, پایله);
		أرجع پایله;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire != عدم && بټونه.شمېر(بټ))
				أرجع true;
		أرجع false;
	}
};

کينډۍ<typename T>
class Sig_سېټ<T, typename std::enable_if<!std::is_اشاره_ګر<T>::ارزښت>::type> : عام Sig_سېټ<T, std::less<T>> {};
کينډۍ<typename T>
using ترتیبول_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::ارزښت>::type;
کينډۍ<typename T>
class Sig_سېټ<T, ترتیبول_by_name_id_guard<T>> : عام Sig_سېټ<T, RTLIL::ترتیبول_by_name_id<typename std::remove_اشاره_ګر<T>::type>> {};

بنية SigMapView
{
	mfp<Sig_بټ> database;

	// Modify bit to its representative
	فراغ apply(RTLIL::Sig_بټ &بټ) ثابت
	{
		بټ = database.پیدا_کړه(بټ);
	}

	فراغ apply(RTLIL::SigSpec &sig) ثابت
	{
		لكل (خپلکاری &بټ : sig)
			apply(بټ);
	}

	RTLIL::Sig_بټ چلوونکی()(RTLIL::Sig_بټ بټ) ثابت
	{
		apply(بټ);
		أرجع بټ;
	}

	RTLIL::SigSpec چلوونکی()(RTLIL::SigSpec sig) ثابت
	{
		apply(sig);
		أرجع sig;
	}

	RTLIL::SigSpec چلوونکی()(RTLIL::Wire *wire) ثابت
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
			سېټ(module);
	}

	فراغ بدلون(SigMap &other)
	{
		database.بدلون(other.database);
	}

	فراغ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ سېټ(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خپلکاری &it : module->connections())
			bitcount += it.لومړی.اندازه();

		database.clear();
		database.reserve(bitcount);

		لكل (خپلکاری &it : module->connections())
			جمع_کړه(it.لومړی, it.دوهم);
	}

	// Add connections from "from" to "to", bit-by-bit
	فراغ جمع_کړه(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& منزل)
	{
		log_assert(Get_اندازه(from) == Get_اندازه(منزل));

		لكل (صحيح i = 0; i < Get_اندازه(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(منزل[i]);

			ثابت RTLIL::Sig_بټ &bf = database[bfi];
			ثابت RTLIL::Sig_بټ &bt = database[bti];

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
	فراغ جمع_کړه(ثابت RTLIL::Sig_بټ &بټ)
	{
		ثابت خپلکاری &b = database.پیدا_کړه(بټ);
		إذا (b.wire != nullptr)
			database.promote(بټ);
	}

	فراغ جمع_کړه(ثابت RTLIL::SigSpec &sig)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			جمع_کړه(بټ);
	}

	مضمن فراغ جمع_کړه(Wire *wire) { أرجع جمع_کړه(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		لكل (ثابت خپلکاری &بټ : database)
			إذا (بټ.wire != nullptr)
				sig.append(بټ);
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
کينډۍ <class Val>
بنية SigValMap final : عام SigMapView
{
	dict<Sig_بټ, Val> ارزښتونه;

	فراغ بدلون(SigValMap<Val> &other)
	{
		database.بدلون(other.database);
		ارزښتونه.بدلون(other.ارزښتونه);
	}

	فراغ clear()
	{
		database.clear();
		ارزښتونه.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ سېټ(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خپلکاری &it : module->connections())
			bitcount += it.لومړی.اندازه();

		database.clear();
		ارزښتونه.clear();
		database.reserve(bitcount);

		لكل (خپلکاری &it : module->connections())
			جمع_کړه(it.لومړی, it.دوهم);
	}

	// Add connections from "from" to "to", bit-by-bit.
	فراغ جمع_کړه(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& منزل)
	{
		log_assert(Get_اندازه(from) == Get_اندازه(منزل));

		لكل (صحيح i = 0; i < Get_اندازه(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(منزل[i]);
			إذا (bfi == bti) {
				استمر;
			}

			ثابت RTLIL::Sig_بټ &bf = database[bfi];
			ثابت RTLIL::Sig_بټ &bt = database[bti];
			إذا (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				یوځای_ارزښت(bt, bf);
			} وإلا {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				یوځای_ارزښت(bf, bt);
			}
		}
	}

	فراغ جمع_کړه_Val(ثابت RTLIL::Sig_بټ &بټ, ثابت Val &val)
	{
		ارزښتونه[database.پیدا_کړه(بټ)] |= val;
	}

	فراغ جمع_کړه_Val(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		لكل (ثابت خپلکاری &بټ : sig)
			جمع_کړه_Val(بټ, val);
	}

	Val apply_and_get_ارزښت(RTLIL::Sig_بټ &بټ) ثابت
	{
		بټ = database.پیدا_کړه(بټ);
		خپلکاری it = ارزښتونه.پیدا_کړه(بټ);
		أرجع it == ارزښتونه.پای_ټکی() ? Val() : it->دوهم;
	}

خاص:
	فراغ یوځای_ارزښت(ثابت RTLIL::Sig_بټ &from, ثابت RTLIL::Sig_بټ &منزل)
	{
		خپلکاری it = ارزښتونه.پیدا_کړه(from);
		إذا (it == ارزښتونه.پای_ټکی()) {
			أرجع;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->دوهم;
		ارزښتونه.erase(it);
		ارزښتونه[منزل] |= v;
	}
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای /* SIGTOOLS_H */
