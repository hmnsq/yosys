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

#جے_بنا_تعریف BITPATTERN_H
#تعریف_کرو BITPATTERN_H

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
بنية ثنائی_ہندسہ_نمونہ_Pool
{
	صحيح چوڑائی;
	بنية بٹ_t {
		std::سمتیہ<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		بٹ_t(صحيح چوڑائی = 0) : bitdata(چوڑائی), cached_hash(0) { }
		RTLIL::State &عامل[](صحيح اشاریہ) {
			أرجع bitdata[اشاریہ];
		}
		ثابت RTLIL::State &عامل[](صحيح اشاریہ) ثابت {
			أرجع bitdata[اشاریہ];
		}
		bool عامل==(ثابت بٹ_t &other) ثابت {
			إذا (لگاتار_hash(*this) != لگاتار_hash(other))
				أرجع false;
			أرجع bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			إذا (!cached_hash)
				cached_hash = لگاتار_hash(bitdata);
			h.eat(cached_hash);
			أرجع h;
		}
	};
	pool<بٹ_t> database;

	ثنائی_ہندسہ_نمونہ_Pool(RTLIL::SigSpec sig)
	{
		چوڑائی = sig.ناپ();
		إذا (چوڑائی > 0) {
			بٹ_t نمونہ(چوڑائی);
			لكل (صحيح i = 0; i < چوڑائی; i++) {
				إذا (sig[i].wire == عدم && sig[i].انکڑے <= RTLIL::State::S1)
					نمونہ[i] = sig[i].انکڑے;
				وإلا
					نمونہ[i] = RTLIL::State::Sa;
			}
			database.داخل_کرو(نمونہ);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	ثنائی_ہندسہ_نمونہ_Pool(صحيح چوڑائی)
	{
		this->چوڑائی = چوڑائی;
		إذا (چوڑائی > 0) {
			بٹ_t نمونہ(چوڑائی);
			لكل (صحيح i = 0; i < چوڑائی; i++)
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
		بٹ.bitdata = sig.as_const().منزل_بٹ();
		لكل (خودکار &b : بٹ.bitdata)
			إذا (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		أرجع بٹ;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ساكن bool covers_nothing(RTLIL::SigSpec sig)
	{
		لكل (خودکار ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire == عدم && (ثنائی_ہندسہ.انکڑے == RTLIL::State::Sx || ثنائی_ہندسہ.انکڑے == RTLIL::State::Sz))
				أرجع true;
		أرجع false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool میل(بٹ_t a, بٹ_t b)
	{
		log_assert(صحيح(a.bitdata.ناپ()) == چوڑائی);
		log_assert(صحيح(b.bitdata.ناپ()) == چوڑائی);
		لكل (صحيح i = 0; i < چوڑائی; i++)
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
		بٹ_t بٹ = sig2bits(sig);
		لكل (خودکار &it : database)
			إذا (میل(it, بٹ))
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
		بٹ_t بٹ = sig2bits(sig);
		لكل (خودکار &it : database)
			إذا (میل(it, بٹ)) {
				لكل (صحيح i = 0; i < چوڑائی; i++)
					إذا (بٹ[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						اذهب اگلا_database_اندراج;
				أرجع true;
	اگلا_database_اندراج:;
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
		bool حالت = false;
		إذا (covers_nothing(sig))
			أرجع false;
		بٹ_t بٹ = sig2bits(sig);
		لكل (خودکار it = database.begin(); it != database.سرا();)
			إذا (میل(*it, بٹ)) {
				لكل (صحيح i = 0; i < چوڑائی; i++) {
					إذا ((*it)[i] != RTLIL::State::Sa || بٹ[i] == RTLIL::State::Sa)
						استمر;
					بٹ_t new_نمونہ;
					new_نمونہ.bitdata = it->bitdata;
					new_نمونہ[i] = بٹ[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.داخل_کرو(new_نمونہ);
				}
				it = database.erase(it);
				حالت = true;
				استمر;
			} وإلا
				++it;
		أرجع حالت;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		إذا (database.خالی_تھاواں())
			أرجع false;
		database.clear();
		أرجع true;
	}

	bool خالی_تھاواں()
	{
		أرجع database.خالی_تھاواں();
	}
};

YOSYS_NAMESPACE_سرا

#جے_انت
