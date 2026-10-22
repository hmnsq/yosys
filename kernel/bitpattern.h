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

#که_نه_تعریف_شوی BITPATTERN_H
#تعریف_کړه BITPATTERN_H

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
بنية بټ_نمونه_Pool
{
	صحيح پلنوالی;
	بنية بټونه_t {
		std::ویکټور<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		بټونه_t(صحيح پلنوالی = 0) : bitdata(پلنوالی), cached_hash(0) { }
		RTLIL::State &چلوونکی[](صحيح شاخص) {
			أرجع bitdata[شاخص];
		}
		ثابت RTLIL::State &چلوونکی[](صحيح شاخص) ثابت {
			أرجع bitdata[شاخص];
		}
		bool چلوونکی==(ثابت بټونه_t &other) ثابت {
			إذا (پرلپسې_hash(*this) != پرلپسې_hash(other))
				أرجع false;
			أرجع bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			إذا (!cached_hash)
				cached_hash = پرلپسې_hash(bitdata);
			h.eat(cached_hash);
			أرجع h;
		}
	};
	pool<بټونه_t> database;

	بټ_نمونه_Pool(RTLIL::SigSpec sig)
	{
		پلنوالی = sig.اندازه();
		إذا (پلنوالی > 0) {
			بټونه_t نمونه(پلنوالی);
			لكل (صحيح i = 0; i < پلنوالی; i++) {
				إذا (sig[i].wire == عدم && sig[i].معلومات <= RTLIL::State::S1)
					نمونه[i] = sig[i].معلومات;
				وإلا
					نمونه[i] = RTLIL::State::Sa;
			}
			database.ورننباسه(نمونه);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	بټ_نمونه_Pool(صحيح پلنوالی)
	{
		this->پلنوالی = پلنوالی;
		إذا (پلنوالی > 0) {
			بټونه_t نمونه(پلنوالی);
			لكل (صحيح i = 0; i < پلنوالی; i++)
				نمونه[i] = RTLIL::State::Sa;
			database.ورننباسه(نمونه);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	بټونه_t sig2bits(RTLIL::SigSpec sig)
	{
		بټونه_t بټونه;
		بټونه.bitdata = sig.as_const().منزل_بټونه();
		لكل (خپلکاری &b : بټونه.bitdata)
			إذا (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		أرجع بټونه;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ساكن bool covers_nothing(RTLIL::SigSpec sig)
	{
		لكل (خپلکاری بټ : sig)
			إذا (بټ.wire == عدم && (بټ.معلومات == RTLIL::State::Sx || بټ.معلومات == RTLIL::State::Sz))
				أرجع true;
		أرجع false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool سمون(بټونه_t a, بټونه_t b)
	{
		log_assert(صحيح(a.bitdata.اندازه()) == پلنوالی);
		log_assert(صحيح(b.bitdata.اندازه()) == پلنوالی);
		لكل (صحيح i = 0; i < پلنوالی; i++)
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
		بټونه_t بټونه = sig2bits(sig);
		لكل (خپلکاری &it : database)
			إذا (سمون(it, بټونه))
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
		بټونه_t بټونه = sig2bits(sig);
		لكل (خپلکاری &it : database)
			إذا (سمون(it, بټونه)) {
				لكل (صحيح i = 0; i < پلنوالی; i++)
					إذا (بټونه[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						اذهب بل_database_ثبت;
				أرجع true;
	بل_database_ثبت:;
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
		بټونه_t بټونه = sig2bits(sig);
		لكل (خپلکاری it = database.begin(); it != database.پای_ټکی();)
			إذا (سمون(*it, بټونه)) {
				لكل (صحيح i = 0; i < پلنوالی; i++) {
					إذا ((*it)[i] != RTLIL::State::Sa || بټونه[i] == RTLIL::State::Sa)
						استمر;
					بټونه_t new_نمونه;
					new_نمونه.bitdata = it->bitdata;
					new_نمونه[i] = بټونه[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ورننباسه(new_نمونه);
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
		إذا (database.تشه())
			أرجع false;
		database.clear();
		أرجع true;
	}

	bool تشه()
	{
		أرجع database.تشه();
	}
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
