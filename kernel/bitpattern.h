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

#جيڪڏهن_وصف_نه_ٿيل BITPATTERN_H
#وصف_ڪريو BITPATTERN_H

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
بنية ٻٽو_انگ_نمونو_Pool
{
	صحيح ويڪر;
	بنية بٽ_t {
		std::ويڪٽر<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		بٽ_t(صحيح ويڪر = 0) : bitdata(ويڪر), cached_hash(0) { }
		RTLIL::State &عامل[](صحيح اشاريو) {
			أرجع bitdata[اشاريو];
		}
		ثابت RTLIL::State &عامل[](صحيح اشاريو) ثابت {
			أرجع bitdata[اشاريو];
		}
		bool عامل==(ثابت بٽ_t &other) ثابت {
			إذا (لاڳيتو_hash(*this) != لاڳيتو_hash(other))
				أرجع false;
			أرجع bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
			إذا (!cached_hash)
				cached_hash = لاڳيتو_hash(bitdata);
			h.eat(cached_hash);
			أرجع h;
		}
	};
	pool<بٽ_t> database;

	ٻٽو_انگ_نمونو_Pool(RTLIL::SigSpec sig)
	{
		ويڪر = sig.سائيز();
		إذا (ويڪر > 0) {
			بٽ_t نمونو(ويڪر);
			لكل (صحيح i = 0; i < ويڪر; i++) {
				إذا (sig[i].wire == عدم && sig[i].معلومات <= RTLIL::State::S1)
					نمونو[i] = sig[i].معلومات;
				وإلا
					نمونو[i] = RTLIL::State::Sa;
			}
			database.وجهو(نمونو);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	ٻٽو_انگ_نمونو_Pool(صحيح ويڪر)
	{
		this->ويڪر = ويڪر;
		إذا (ويڪر > 0) {
			بٽ_t نمونو(ويڪر);
			لكل (صحيح i = 0; i < ويڪر; i++)
				نمونو[i] = RTLIL::State::Sa;
			database.وجهو(نمونو);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	بٽ_t sig2bits(RTLIL::SigSpec sig)
	{
		بٽ_t بٽ;
		بٽ.bitdata = sig.as_const().منزل_بٽ();
		لكل (خودڪار &b : بٽ.bitdata)
			إذا (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		أرجع بٽ;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ساكن bool covers_nothing(RTLIL::SigSpec sig)
	{
		لكل (خودڪار ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire == عدم && (ٻٽو_انگ.معلومات == RTLIL::State::Sx || ٻٽو_انگ.معلومات == RTLIL::State::Sz))
				أرجع true;
		أرجع false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool ميلاپ(بٽ_t a, بٽ_t b)
	{
		log_assert(صحيح(a.bitdata.سائيز()) == ويڪر);
		log_assert(صحيح(b.bitdata.سائيز()) == ويڪر);
		لكل (صحيح i = 0; i < ويڪر; i++)
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
		بٽ_t بٽ = sig2bits(sig);
		لكل (خودڪار &it : database)
			إذا (ميلاپ(it, بٽ))
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
		بٽ_t بٽ = sig2bits(sig);
		لكل (خودڪار &it : database)
			إذا (ميلاپ(it, بٽ)) {
				لكل (صحيح i = 0; i < ويڪر; i++)
					إذا (بٽ[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						اذهب ايندڙ_database_داخلا;
				أرجع true;
	ايندڙ_database_داخلا:;
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
		بٽ_t بٽ = sig2bits(sig);
		لكل (خودڪار it = database.begin(); it != database.پڇاڙي();)
			إذا (ميلاپ(*it, بٽ)) {
				لكل (صحيح i = 0; i < ويڪر; i++) {
					إذا ((*it)[i] != RTLIL::State::Sa || بٽ[i] == RTLIL::State::Sa)
						استمر;
					بٽ_t new_نمونو;
					new_نمونو.bitdata = it->bitdata;
					new_نمونو[i] = بٽ[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.وجهو(new_نمونو);
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
		إذا (database.خالي_جايون())
			أرجع false;
		database.clear();
		أرجع true;
	}

	bool خالي_جايون()
	{
		أرجع database.خالي_جايون();
	}
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
