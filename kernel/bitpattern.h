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

#ifndef BITPATTERN_H
#define BITPATTERN_H

#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"

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
ಸಂರಚನಾ BitPatternPool
{
	ಪೂರ್ಣಾಙ್ಕ width;
	ಸಂರಚನಾ bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(ಪೂರ್ಣಾಙ್ಕ width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](ಪೂರ್ಣಾಙ್ಕ index) {
			ಪ್ರತ್ಯಾಗಮ bitdata[index];
		}
		ಧ್ರುವ RTLIL::State &operator[](ಪೂರ್ಣಾಙ್ಕ index) ಧ್ರುವ {
			ಪ್ರತ್ಯಾಗಮ bitdata[index];
		}
		bool operator==(ಧ್ರುವ bits_t &other) ಧ್ರುವ {
			ಯದಿ (run_hash(*this) != run_hash(other))
				ಪ್ರತ್ಯಾಗಮ false;
			ಪ್ರತ್ಯಾಗಮ bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
			ಯದಿ (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			ಪ್ರತ್ಯಾಗಮ h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		ಯದಿ (width > 0) {
			bits_t pattern(width);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++) {
				ಯದಿ (sig[i].wire == ರಿಕ್ತ && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				ಅನ್ಯಥಾ
					pattern[i] = RTLIL::State::Sa;
			}
			database.ಸೇರಿಸು(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(ಪೂರ್ಣಾಙ್ಕ width)
	{
		this->width = width;
		ಯದಿ (width > 0) {
			bits_t pattern(width);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.ಸೇರಿಸು(pattern);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	bits_t sig2bits(RTLIL::SigSpec sig)
	{
		bits_t bits;
		bits.bitdata = sig.as_const().to_bits();
		ಪ್ರತಿ (auto &b : bits.bitdata)
			ಯದಿ (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		ಪ್ರತ್ಯಾಗಮ bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ಸ್ಥಿರ bool covers_nothing(RTLIL::SigSpec sig)
	{
		ಪ್ರತಿ (auto bit : sig)
			ಯದಿ (bit.wire == ರಿಕ್ತ && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				ಪ್ರತ್ಯಾಗಮ true;
		ಪ್ರತ್ಯಾಗಮ false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(ಪೂರ್ಣಾಙ್ಕ(a.bitdata.size()) == width);
		log_assert(ಪೂರ್ಣಾಙ್ಕ(b.bitdata.size()) == width);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
			ಯದಿ (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				ಪ್ರತ್ಯಾಗಮ false;
		ಪ್ರತ್ಯಾಗಮ true;
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
		ಯದಿ (covers_nothing(sig))
			ಪ್ರತ್ಯಾಗಮ false;
		bits_t bits = sig2bits(sig);
		ಪ್ರತಿ (auto &it : database)
			ಯದಿ (match(it, bits))
				ಪ್ರತ್ಯಾಗಮ true;
		ಪ್ರತ್ಯಾಗಮ false;
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
		ಯದಿ (covers_nothing(sig))
			ಪ್ರತ್ಯಾಗಮ true;
		bits_t bits = sig2bits(sig);
		ಪ್ರತಿ (auto &it : database)
			ಯದಿ (match(it, bits)) {
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
					ಯದಿ (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						ಗಚ್ಛ next_database_entry;
				ಪ್ರತ್ಯಾಗಮ true;
	next_database_entry:;
			}
		ಪ್ರತ್ಯಾಗಮ false;
	}

	/**
	 * Remove cube sig from the pool, splitting the remaining cubes. True if success.
	 * For example:
	 * Taking 011 out of pool({01a}) -> pool({010}), returns true.
	 * Taking 011 out of pool({010}) does nothing, returns false.
	 */
	bool take(RTLIL::SigSpec sig)
	{
		bool status = false;
		ಯದಿ (covers_nothing(sig))
			ಪ್ರತ್ಯಾಗಮ false;
		bits_t bits = sig2bits(sig);
		ಪ್ರತಿ (auto it = database.begin(); it != database.end();)
			ಯದಿ (match(*it, bits)) {
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++) {
					ಯದಿ ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						ಅನುವರ್ತ;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ಸೇರಿಸು(new_pattern);
				}
				it = database.erase(it);
				status = true;
				ಅನುವರ್ತ;
			} ಅನ್ಯಥಾ
				++it;
		ಪ್ರತ್ಯಾಗಮ status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		ಯದಿ (database.empty())
			ಪ್ರತ್ಯಾಗಮ false;
		database.clear();
		ಪ್ರತ್ಯಾಗಮ true;
	}

	bool empty()
	{
		ಪ್ರತ್ಯಾಗಮ database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
