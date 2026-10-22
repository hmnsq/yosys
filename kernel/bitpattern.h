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

#اگر_تعریف_نشده BITPATTERN_H
#تعریف_کن BITPATTERN_H

#ضمن "kernel/log.h"
#ضمن "kernel/rtlil.h"

YOSYS_NAMESPACE_BEGIN

/**
 * This file implements BitPatternPool for efficiently storing and querying
 * sets of fixed-width 2-valued logic constants compressed as "bit patterns".
 * A bit pattern can have don't cares on one or more bit positions (State::Sa).
 *
 * In terms of logic synthesis:
 * A BitPatternPool is a sum of products (SOP).
 * BitPatternPool::bits_t is a cube.
 *
 * BitPatternPool does not permit adding new patterns, only removing.
 * Its intended use case is in analysing cases in case/match constructs in HDL.
 */
بنية بیت_الگو_Pool
{
	صحيح عرض;
	بنية بیتها_t {
		std::وکتور<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		بیتها_t(صحيح عرض = 0) : bitdata(عرض), cached_hash(0) { }
		RTLIL::State &عملگر[](صحيح شاخص) {
			أرجع bitdata[شاخص];
		}
		ثابت RTLIL::State &عملگر[](صحيح شاخص) ثابت {
			أرجع bitdata[شاخص];
		}
		bool عملگر==(ثابت بیتها_t &other) ثابت {
			إذا (پیاپی_hash(*this) != پیاپی_hash(other))
				أرجع false;
			أرجع bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			إذا (!cached_hash)
				cached_hash = پیاپی_hash(bitdata);
			h.eat(cached_hash);
			أرجع h;
		}
	};
	pool<بیتها_t> database;

	بیت_الگو_Pool(RTLIL::SigSpec sig)
	{
		عرض = sig.اندازه();
		إذا (عرض > 0) {
			بیتها_t الگو(عرض);
			لكل (صحيح i = 0; i < عرض; i++) {
				إذا (sig[i].wire == عدم && sig[i].داده <= RTLIL::State::S1)
					الگو[i] = sig[i].داده;
				وإلا
					الگو[i] = RTLIL::State::Sa;
			}
			database.درج(الگو);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	بیت_الگو_Pool(صحيح عرض)
	{
		this->عرض = عرض;
		إذا (عرض > 0) {
			بیتها_t الگو(عرض);
			لكل (صحيح i = 0; i < عرض; i++)
				الگو[i] = RTLIL::State::Sa;
			database.درج(الگو);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	بیتها_t sig2bits(RTLIL::SigSpec sig)
	{
		بیتها_t بیتها;
		بیتها.bitdata = sig.as_const().مقصد_بیتها();
		لكل (خودکار &b : بیتها.bitdata)
			إذا (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		أرجع بیتها;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ساكن bool covers_nothing(RTLIL::SigSpec sig)
	{
		لكل (خودکار بیت : sig)
			إذا (بیت.wire == عدم && (بیت.داده == RTLIL::State::Sx || بیت.داده == RTLIL::State::Sz))
				أرجع true;
		أرجع false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool تطابق(بیتها_t a, بیتها_t b)
	{
		log_assert(صحيح(a.bitdata.اندازه()) == عرض);
		log_assert(صحيح(b.bitdata.اندازه()) == عرض);
		لكل (صحيح i = 0; i < عرض; i++)
			إذا (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				أرجع false;
		أرجع true;
	}

	/**
	 * Does cube sig overlap any cube in the pool?
	 * For example:
	 * pool({aaa}).has_any(01a) == true
	 * pool({01a}).has_any(01a) == true
	 * pool({011}).has_any(01a) == true
	 * pool({01a}).has_any(011) == true
	 * pool({111}).has_any(01a) == false
	 */
	bool has_any(RTLIL::SigSpec sig)
	{
		إذا (covers_nothing(sig))
			أرجع false;
		بیتها_t بیتها = sig2bits(sig);
		لكل (خودکار &it : database)
			إذا (تطابق(it, بیتها))
				أرجع true;
		أرجع false;
	}

	/**
	 * Is cube sig covered by a cube in the pool?
	 * For example:
	 * pool({aaa}).has_all(01a) == true
	 * pool({01a}).has_any(01a) == true
	 * pool({01a}).has_any(011) == true
	 * pool({011}).has_all(01a) == false
	 * pool({111}).has_all(01a) == false
	 */
	bool has_all(RTLIL::SigSpec sig)
	{
		إذا (covers_nothing(sig))
			أرجع true;
		بیتها_t بیتها = sig2bits(sig);
		لكل (خودکار &it : database)
			إذا (تطابق(it, بیتها)) {
				لكل (صحيح i = 0; i < عرض; i++)
					إذا (بیتها[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						اذهب بعدی_database_مدخل;
				أرجع true;
	بعدی_database_مدخل:;
			}
		أرجع false;
	}

	/**
	 * Remove cube sig from the pool, splitting the remaining cubes. True if success.
	 * For example:
	 * Taking 011 out of pool({01a}) -> pool({010}), returns true.
	 * Taking 011 out of pool({010}) does nothing, returns false.
	 */
	bool take(RTLIL::SigSpec sig)
	{
		bool وضعیت = false;
		إذا (covers_nothing(sig))
			أرجع false;
		بیتها_t بیتها = sig2bits(sig);
		لكل (خودکار it = database.begin(); it != database.انتها();)
			إذا (تطابق(*it, بیتها)) {
				لكل (صحيح i = 0; i < عرض; i++) {
					إذا ((*it)[i] != RTLIL::State::Sa || بیتها[i] == RTLIL::State::Sa)
						استمر;
					بیتها_t new_الگو;
					new_الگو.bitdata = it->bitdata;
					new_الگو[i] = بیتها[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.درج(new_الگو);
				}
				it = database.erase(it);
				وضعیت = true;
				استمر;
			} وإلا
				++it;
		أرجع وضعیت;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		إذا (database.خالی())
			أرجع false;
		database.clear();
		أرجع true;
	}

	bool خالی()
	{
		أرجع database.خالی();
	}
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
