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

#اگر_غیر_تعریف_شدہ BITPATTERN_H
#تعریف_کرو BITPATTERN_H

#شامل "kernel/log.h"
#شامل "kernel/rtlil.h"

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
ساخت ثنائی_ہندسہ_نمونہ_Pool
{
	صحیح چوڑائی;
	ساخت بٹ_t {
		std::سمتیہ<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		بٹ_t(صحیح چوڑائی = 0) : bitdata(چوڑائی), cached_hash(0) { }
		RTLIL::State &عامل[](صحیح اشاریہ) {
			واپس bitdata[اشاریہ];
		}
		ثابت RTLIL::State &عامل[](صحیح اشاریہ) ثابت {
			واپس bitdata[اشاریہ];
		}
		bool عامل==(ثابت بٹ_t &other) ثابت {
			اگر (تسلسل_hash(*this) != تسلسل_hash(other))
				واپس false;
			واپس bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			اگر (!cached_hash)
				cached_hash = تسلسل_hash(bitdata);
			h.eat(cached_hash);
			واپس h;
		}
	};
	pool<بٹ_t> database;

	ثنائی_ہندسہ_نمونہ_Pool(RTLIL::SigSpec sig)
	{
		چوڑائی = sig.سائز();
		اگر (چوڑائی > 0) {
			بٹ_t نمونہ(چوڑائی);
			برائے (صحیح i = 0; i < چوڑائی; i++) {
				اگر (sig[i].wire == تہی && sig[i].ڈیٹا <= RTLIL::State::S1)
					نمونہ[i] = sig[i].ڈیٹا;
				ورنہ
					نمونہ[i] = RTLIL::State::Sa;
			}
			database.داخل_کرو(نمونہ);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	ثنائی_ہندسہ_نمونہ_Pool(صحیح چوڑائی)
	{
		this->چوڑائی = چوڑائی;
		اگر (چوڑائی > 0) {
			بٹ_t نمونہ(چوڑائی);
			برائے (صحیح i = 0; i < چوڑائی; i++)
				نمونہ[i] = RTLIL::State::Sa;
			database.داخل_کرو(نمونہ);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	بٹ_t sig2bits(RTLIL::SigSpec sig)
	{
		بٹ_t بٹ;
		بٹ.bitdata = sig.as_const().مقصد_بٹ();
		برائے (خودکار &b : بٹ.bitdata)
			اگر (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		واپس بٹ;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ساکن bool covers_nothing(RTLIL::SigSpec sig)
	{
		برائے (خودکار ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire == تہی && (ثنائی_ہندسہ.ڈیٹا == RTLIL::State::Sx || ثنائی_ہندسہ.ڈیٹا == RTLIL::State::Sz))
				واپس true;
		واپس false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool مطابقت(بٹ_t a, بٹ_t b)
	{
		log_assert(صحیح(a.bitdata.سائز()) == چوڑائی);
		log_assert(صحیح(b.bitdata.سائز()) == چوڑائی);
		برائے (صحیح i = 0; i < چوڑائی; i++)
			اگر (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				واپس false;
		واپس true;
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
		اگر (covers_nothing(sig))
			واپس false;
		بٹ_t بٹ = sig2bits(sig);
		برائے (خودکار &it : database)
			اگر (مطابقت(it, بٹ))
				واپس true;
		واپس false;
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
		اگر (covers_nothing(sig))
			واپس true;
		بٹ_t بٹ = sig2bits(sig);
		برائے (خودکار &it : database)
			اگر (مطابقت(it, بٹ)) {
				برائے (صحیح i = 0; i < چوڑائی; i++)
					اگر (بٹ[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						جاؤ اگلا_database_اندراج;
				واپس true;
	اگلا_database_اندراج:;
			}
		واپس false;
	}

	/**
	 * Remove cube sig from the pool, splitting the remaining cubes. True if success.
	 * For example:
	 * Taking 011 out of pool({01a}) -> pool({010}), returns true.
	 * Taking 011 out of pool({010}) does nothing, returns false.
	 */
	bool take(RTLIL::SigSpec sig)
	{
		bool حالت_2 = false;
		اگر (covers_nothing(sig))
			واپس false;
		بٹ_t بٹ = sig2bits(sig);
		برائے (خودکار it = database.begin(); it != database.انتہا();)
			اگر (مطابقت(*it, بٹ)) {
				برائے (صحیح i = 0; i < چوڑائی; i++) {
					اگر ((*it)[i] != RTLIL::State::Sa || بٹ[i] == RTLIL::State::Sa)
						جاری;
					بٹ_t new_نمونہ;
					new_نمونہ.bitdata = it->bitdata;
					new_نمونہ[i] = بٹ[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.داخل_کرو(new_نمونہ);
				}
				it = database.erase(it);
				حالت_2 = true;
				جاری;
			} ورنہ
				++it;
		واپس حالت_2;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		اگر (database.خالی_خانے())
			واپس false;
		database.clear();
		واپس true;
	}

	bool خالی_خانے()
	{
		واپس database.خالی_خانے();
	}
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
