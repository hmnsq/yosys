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

#অন্তর্ভাব "kernel/log.h"
#অন্তর্ভাব "kernel/rtlil.h"

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
কাঠামো BitPatternPool
{
	পূর্ণসংখ্যা width;
	কাঠামো bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(পূর্ণসংখ্যা width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](পূর্ণসংখ্যা সূচক) {
			ফেরত bitdata[সূচক];
		}
		ধ্রুবক RTLIL::State &operator[](পূর্ণসংখ্যা সূচক) ধ্রুবক {
			ফেরত bitdata[সূচক];
		}
		bool operator==(ধ্রুবক bits_t &other) ধ্রুবক {
			যদি (run_hash(*this) != run_hash(other))
				ফেরত false;
			ফেরত bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুবক {
			যদি (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			ফেরত h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.আকার_2();
		যদি (width > 0) {
			bits_t pattern(width);
			জন্য (পূর্ণসংখ্যা i = 0; i < width; i++) {
				যদি (sig[i].wire == ফাঁকা && sig[i].তথ্য <= RTLIL::State::S1)
					pattern[i] = sig[i].তথ্য;
				অন্যথা
					pattern[i] = RTLIL::State::Sa;
			}
			database.ঢোকাও(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(পূর্ণসংখ্যা width)
	{
		this->width = width;
		যদি (width > 0) {
			bits_t pattern(width);
			জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.ঢোকাও(pattern);
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
		জন্য (auto &b : bits.bitdata)
			যদি (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		ফেরত bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	স্থির bool covers_nothing(RTLIL::SigSpec sig)
	{
		জন্য (auto bit : sig)
			যদি (bit.wire == ফাঁকা && (bit.তথ্য == RTLIL::State::Sx || bit.তথ্য == RTLIL::State::Sz))
				ফেরত true;
		ফেরত false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(পূর্ণসংখ্যা(a.bitdata.আকার_2()) == width);
		log_assert(পূর্ণসংখ্যা(b.bitdata.আকার_2()) == width);
		জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
			যদি (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				ফেরত false;
		ফেরত true;
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
		যদি (covers_nothing(sig))
			ফেরত false;
		bits_t bits = sig2bits(sig);
		জন্য (auto &it : database)
			যদি (match(it, bits))
				ফেরত true;
		ফেরত false;
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
		যদি (covers_nothing(sig))
			ফেরত true;
		bits_t bits = sig2bits(sig);
		জন্য (auto &it : database)
			যদি (match(it, bits)) {
				জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
					যদি (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						যাও পরবর্তী_database_entry;
				ফেরত true;
	পরবর্তী_database_entry:;
			}
		ফেরত false;
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
		যদি (covers_nothing(sig))
			ফেরত false;
		bits_t bits = sig2bits(sig);
		জন্য (auto it = database.begin(); it != database.end();)
			যদি (match(*it, bits)) {
				জন্য (পূর্ণসংখ্যা i = 0; i < width; i++) {
					যদি ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						চালু;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ঢোকাও(new_pattern);
				}
				it = database.erase(it);
				status = true;
				চালু;
			} অন্যথা
				++it;
		ফেরত status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		যদি (database.empty())
			ফেরত false;
		database.clear();
		ফেরত true;
	}

	bool empty()
	{
		ফেরত database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
