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

#اگر_تعریف_نشده SIGTOOLS_H
#تعریف_کن SIGTOOLS_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية SigPool
{
	بنية بیت_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		بیت_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		بیت_Def_t(ثابت RTLIL::Sig_بیت &بیت) : std::pair<RTLIL::Wire*, صحيح>(بیت.wire, بیت.جابه_جایی) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(اولی->name);
			h.eat(دومی);
			أرجع h;
		}
	};

	pool<بیت_Def_t> بیتها;

	فراغ clear()
	{
		بیتها.clear();
	}

	فراغ جمع_کن(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار &بیت : sig)
			إذا (بیت.wire != عدم)
				بیتها.درج(بیت);
	}

	فراغ جمع_کن(ثابت SigPool &other)
	{
		لكل (خودکار &بیت : other.بیتها)
			بیتها.درج(بیت);
	}

	فراغ del(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار &بیت : sig)
			إذا (بیت.wire != عدم)
				بیتها.erase(بیت);
	}

	فراغ del(ثابت SigPool &other)
	{
		لكل (خودکار &بیت : other.بیتها)
			بیتها.erase(بیت);
	}

	فراغ expand(ثابت RTLIL::SigSpec &from, ثابت RTLIL::SigSpec &مقصد)
	{
		log_assert(Get_اندازه(from) == Get_اندازه(مقصد));
		لكل (صحيح i = 0; i < Get_اندازه(from); i++) {
			بیت_Def_t بیت_from(from[i]), بیت_مقصد(مقصد[i]);
			إذا (بیت_from.اولی != عدم && بیت_مقصد.اولی != عدم && بیتها.شمار(بیت_from) > 0)
				بیتها.درج(بیت_مقصد);
		}
	}

	RTLIL::SigSpec استخراج(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتیجه;
		لكل (خودکار &بیت : sig)
			إذا (بیت.wire != عدم && بیتها.شمار(بیت))
				نتیجه.append(بیت);
		أرجع نتیجه;
	}

	RTLIL::SigSpec remove(ثابت RTLIL::SigSpec &sig) ثابت
	{
		RTLIL::SigSpec نتیجه;
		لكل (خودکار &بیت : sig)
			إذا (بیت.wire != عدم && بیتها.شمار(بیت) == 0)
				نتیجه.append(بیت);
		أرجع نتیجه;
	}

	bool check(ثابت RTLIL::Sig_بیت &بیت) ثابت
	{
		أرجع بیت.wire != عدم && بیتها.شمار(بیت);
	}

	bool check_any(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودکار &بیت : sig)
			إذا (بیت.wire != عدم && بیتها.شمار(بیت))
				أرجع true;
		أرجع false;
	}

	bool check_all(ثابت RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودکار &بیت : sig)
			إذا (بیت.wire != عدم && بیتها.شمار(بیت) == 0)
				أرجع false;
		أرجع true;
	}

	RTLIL::SigSpec export_one() ثابت
	{
		لكل (خودکار &بیت : بیتها)
			أرجع RTLIL::SigSpec(بیت.اولی, بیت.دومی);
		أرجع RTLIL::SigSpec();
	}

	RTLIL::SigSpec export_all() ثابت
	{
		pool<RTLIL::Sig_بیت> sig;
		لكل (خودکار &بیت : بیتها)
			sig.درج(RTLIL::Sig_بیت(بیت.اولی, بیت.دومی));
		أرجع sig;
	}

	اندازه_t اندازه() ثابت
	{
		أرجع بیتها.اندازه();
	}
};

قالب <typename T, class Compare = فراغ>
بنية Sig_مجموعه
{
	static_assert(!std::is_same<Compare,فراغ>::مقدار, "Default value for `Compare' class not found for SigSet<T>. Please specify.");

	بنية بیت_Def_t : عام std::pair<RTLIL::Wire*, صحيح> {
		بیت_Def_t() : std::pair<RTLIL::Wire*, صحيح>(عدم, 0) { }
		بیت_Def_t(ثابت RTLIL::Sig_بیت &بیت) : std::pair<RTLIL::Wire*, صحيح>(بیت.wire, بیت.جابه_جایی) { }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			h.eat(اولی->name);
			h.eat(دومی);
			أرجع h;
		}
	};

	dict<بیت_Def_t, std::مجموعه<T, Compare>> بیتها;

	فراغ clear()
	{
		بیتها.clear();
	}

	فراغ درج(ثابت RTLIL::SigSpec &sig, T داده)
	{
		لكل (ثابت خودکار &بیت : sig)
			إذا (بیت.wire != عدم)
				بیتها[بیت].درج(داده);
	}

	فراغ درج(ثابت RTLIL::SigSpec& sig, ثابت std::مجموعه<T> &داده)
	{
		لكل (ثابت خودکار &بیت : sig)
			إذا (بیت.wire != عدم)
				بیتها[بیت].درج(داده.begin(), داده.انتها());
	}

	فراغ erase(ثابت RTLIL::SigSpec& sig)
	{
		لكل (ثابت خودکار &بیت : sig)
			إذا (بیت.wire != عدم)
				بیتها[بیت].clear();
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, T داده)
	{
		لكل (ثابت خودکار &بیت : sig)
			إذا (بیت.wire != عدم)
				بیتها[بیت].erase(داده);
	}

	فراغ erase(ثابت RTLIL::SigSpec &sig, ثابت std::مجموعه<T> &داده)
	{
		لكل (ثابت خودکار &بیت : sig)
			إذا (بیت.wire != عدم)
				بیتها[بیت].erase(داده.begin(), داده.انتها());
	}

	فراغ پیدا_کن(ثابت RTLIL::SigSpec &sig, std::مجموعه<T> &نتیجه)
	{
		لكل (ثابت خودکار &بیت : sig)
			إذا (بیت.wire != عدم) {
				خودکار &داده = بیتها[بیت];
				نتیجه.درج(داده.begin(), داده.انتها());
			}
	}

	فراغ پیدا_کن(ثابت RTLIL::SigSpec &sig, pool<T> &نتیجه)
	{
		لكل (ثابت خودکار &بیت : sig)
			إذا (بیت.wire != عدم) {
				خودکار &داده = بیتها[بیت];
				نتیجه.درج(داده.begin(), داده.انتها());
			}
	}

	std::مجموعه<T> پیدا_کن(ثابت RTLIL::SigSpec &sig)
	{
		std::مجموعه<T> نتیجه;
		پیدا_کن(sig, نتیجه);
		أرجع نتیجه;
	}

	bool has(ثابت RTLIL::SigSpec &sig)
	{
		لكل (خودکار &بیت : sig)
			إذا (بیت.wire != عدم && بیتها.شمار(بیت))
				أرجع true;
		أرجع false;
	}
};

قالب<typename T>
class Sig_مجموعه<T, typename std::enable_if<!std::is_اشاره_گر<T>::مقدار>::type> : عام Sig_مجموعه<T, std::less<T>> {};
قالب<typename T>
using مرتب_سازی_by_name_id_guard = typename std::enable_if<std::is_same<T,RTLIL::Cell*>::مقدار>::type;
قالب<typename T>
class Sig_مجموعه<T, مرتب_سازی_by_name_id_guard<T>> : عام Sig_مجموعه<T, RTLIL::مرتب_سازی_by_name_id<typename std::remove_اشاره_گر<T>::type>> {};

بنية SigMapView
{
	mfp<Sig_بیت> database;

	// Modify bit to its representative
	فراغ apply(RTLIL::Sig_بیت &بیت) ثابت
	{
		بیت = database.پیدا_کن(بیت);
	}

	فراغ apply(RTLIL::SigSpec &sig) ثابت
	{
		لكل (خودکار &بیت : sig)
			apply(بیت);
	}

	RTLIL::Sig_بیت عملگر()(RTLIL::Sig_بیت بیت) ثابت
	{
		apply(بیت);
		أرجع بیت;
	}

	RTLIL::SigSpec عملگر()(RTLIL::SigSpec sig) ثابت
	{
		apply(sig);
		أرجع sig;
	}

	RTLIL::SigSpec عملگر()(RTLIL::Wire *wire) ثابت
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
			مجموعه(module);
	}

	فراغ جابجا(SigMap &other)
	{
		database.جابجا(other.database);
	}

	فراغ clear()
	{
		database.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ مجموعه(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خودکار &it : module->connections())
			bitcount += it.اولی.اندازه();

		database.clear();
		database.reserve(bitcount);

		لكل (خودکار &it : module->connections())
			جمع_کن(it.اولی, it.دومی);
	}

	// Add connections from "from" to "to", bit-by-bit
	فراغ جمع_کن(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& مقصد)
	{
		log_assert(Get_اندازه(from) == Get_اندازه(مقصد));

		لكل (صحيح i = 0; i < Get_اندازه(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(مقصد[i]);

			ثابت RTLIL::Sig_بیت &bf = database[bfi];
			ثابت RTLIL::Sig_بیت &bt = database[bti];

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
	فراغ جمع_کن(ثابت RTLIL::Sig_بیت &بیت)
	{
		ثابت خودکار &b = database.پیدا_کن(بیت);
		إذا (b.wire != nullptr)
			database.promote(بیت);
	}

	فراغ جمع_کن(ثابت RTLIL::SigSpec &sig)
	{
		لكل (ثابت خودکار &بیت : sig)
			جمع_کن(بیت);
	}

	مضمن فراغ جمع_کن(Wire *wire) { أرجع جمع_کن(RTLIL::SigSpec(wire)); }

	// All non-const bits
	RTLIL::SigSpec allbits() ثابت
	{
		RTLIL::SigSpec sig;
		لكل (ثابت خودکار &بیت : database)
			إذا (بیت.wire != nullptr)
				sig.append(بیت);
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
قالب <class Val>
بنية SigValMap final : عام SigMapView
{
	dict<Sig_بیت, Val> مقادیر;

	فراغ جابجا(SigValMap<Val> &other)
	{
		database.جابجا(other.database);
		مقادیر.جابجا(other.مقادیر);
	}

	فراغ clear()
	{
		database.clear();
		مقادیر.clear();
	}

	// Rebuild SigMap for all connections in module
	فراغ مجموعه(RTLIL::Module *module)
	{
		صحيح bitcount = 0;
		لكل (خودکار &it : module->connections())
			bitcount += it.اولی.اندازه();

		database.clear();
		مقادیر.clear();
		database.reserve(bitcount);

		لكل (خودکار &it : module->connections())
			جمع_کن(it.اولی, it.دومی);
	}

	// Add connections from "from" to "to", bit-by-bit.
	فراغ جمع_کن(ثابت RTLIL::SigSpec& from, ثابت RTLIL::SigSpec& مقصد)
	{
		log_assert(Get_اندازه(from) == Get_اندازه(مقصد));

		لكل (صحيح i = 0; i < Get_اندازه(from); i++)
		{
			صحيح bfi = database.lookup(from[i]);
			صحيح bti = database.lookup(مقصد[i]);
			إذا (bfi == bti) {
				استمر;
			}

			ثابت RTLIL::Sig_بیت &bf = database[bfi];
			ثابت RTLIL::Sig_بیت &bt = database[bti];
			إذا (bf.wire == nullptr) {
				// bf is constant so make it the canonical representative.
				database.imerge(bti, bfi);
				ادغام_مقدار(bt, bf);
			} وإلا {
				// Make bt the canonical representative.
				database.imerge(bfi, bti);
				ادغام_مقدار(bf, bt);
			}
		}
	}

	فراغ جمع_کن_Val(ثابت RTLIL::Sig_بیت &بیت, ثابت Val &val)
	{
		مقادیر[database.پیدا_کن(بیت)] |= val;
	}

	فراغ جمع_کن_Val(ثابت RTLIL::SigSpec &sig, ثابت Val &val)
	{
		لكل (ثابت خودکار &بیت : sig)
			جمع_کن_Val(بیت, val);
	}

	Val apply_and_get_مقدار(RTLIL::Sig_بیت &بیت) ثابت
	{
		بیت = database.پیدا_کن(بیت);
		خودکار it = مقادیر.پیدا_کن(بیت);
		أرجع it == مقادیر.انتها() ? Val() : it->دومی;
	}

خاص:
	فراغ ادغام_مقدار(ثابت RTLIL::Sig_بیت &from, ثابت RTLIL::Sig_بیت &مقصد)
	{
		خودکار it = مقادیر.پیدا_کن(from);
		إذا (it == مقادیر.انتها()) {
			أرجع;
		}
		// values[to] could resize the underlying `entries` so
		// finish using `it` first.
		Val v = it->دومی;
		مقادیر.erase(it);
		مقادیر[مقصد] |= v;
	}
};

YOSYS_NAMESPACE_انتها

#پایان_اگر /* SIGTOOLS_H */
