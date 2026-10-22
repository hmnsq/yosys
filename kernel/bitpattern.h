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

#অন্তর্ভাৱ "kernel/log.h"
#অন্তর্ভাৱ "kernel/rtlil.h"

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
সংরচনা BitPatternPool
{
	পূর্ণাঙ্ক width;
	সংরচনা bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(পূর্ণাঙ্ক width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](পূর্ণাঙ্ক index) {
			ঘূৰাই bitdata[index];
		}
		ধ্রুৱ RTLIL::State &operator[](পূর্ণাঙ্ক index) ধ্রুৱ {
			ঘূৰাই bitdata[index];
		}
		bool operator==(ধ্রুৱ bits_t &other) ধ্রুৱ {
			যদি (run_hash(*this) != run_hash(other))
				ঘূৰাই false;
			ঘূৰাই bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
			যদি (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			ঘূৰাই h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		যদি (width > 0) {
			bits_t pattern(width);
			বাবে (পূর্ণাঙ্ক i = 0; i < width; i++) {
				যদি (sig[i].wire == রিক্ত && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				অন্যথা
					pattern[i] = RTLIL::State::Sa;
			}
			database.ঢোকাও(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(পূর্ণাঙ্ক width)
	{
		this->width = width;
		যদি (width > 0) {
			bits_t pattern(width);
			বাবে (পূর্ণাঙ্ক i = 0; i < width; i++)
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
		বাবে (auto &b : bits.bitdata)
			যদি (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		ঘূৰাই bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	স্থির bool covers_nothing(RTLIL::SigSpec sig)
	{
		বাবে (auto bit : sig)
			যদি (bit.wire == রিক্ত && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				ঘূৰাই true;
		ঘূৰাই false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(পূর্ণাঙ্ক(a.bitdata.size()) == width);
		log_assert(পূর্ণাঙ্ক(b.bitdata.size()) == width);
		বাবে (পূর্ণাঙ্ক i = 0; i < width; i++)
			যদি (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				ঘূৰাই false;
		ঘূৰাই true;
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
			ঘূৰাই false;
		bits_t bits = sig2bits(sig);
		বাবে (auto &it : database)
			যদি (match(it, bits))
				ঘূৰাই true;
		ঘূৰাই false;
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
			ঘূৰাই true;
		bits_t bits = sig2bits(sig);
		বাবে (auto &it : database)
			যদি (match(it, bits)) {
				বাবে (পূর্ণাঙ্ক i = 0; i < width; i++)
					যদি (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						গচ্ছ next_database_entry;
				ঘূৰাই true;
	next_database_entry:;
			}
		ঘূৰাই false;
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
			ঘূৰাই false;
		bits_t bits = sig2bits(sig);
		বাবে (auto it = database.begin(); it != database.end();)
			যদি (match(*it, bits)) {
				বাবে (পূর্ণাঙ্ক i = 0; i < width; i++) {
					যদি ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						অনুৱর্ত;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ঢোকাও(new_pattern);
				}
				it = database.erase(it);
				status = true;
				অনুৱর্ত;
			} অন্যথা
				++it;
		ঘূৰাই status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		যদি (database.empty())
			ঘূৰাই false;
		database.clear();
		ঘূৰাই true;
	}

	bool empty()
	{
		ঘূৰাই database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
