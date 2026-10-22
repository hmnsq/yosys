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

#அந்தர்பாவ "kernel/log.h"
#அந்தர்பாவ "kernel/rtlil.h"

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
கட்டமைப்பு BitPatternPool
{
	முழு width;
	கட்டமைப்பு bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(முழு width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](முழு index) {
			திரும்பு bitdata[index];
		}
		மாறிலி RTLIL::State &operator[](முழு index) மாறிலி {
			திரும்பு bitdata[index];
		}
		bool operator==(மாறிலி bits_t &other) மாறிலி {
			எனில் (run_hash(*this) != run_hash(other))
				திரும்பு false;
			திரும்பு bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
			எனில் (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			திரும்பு h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		எனில் (width > 0) {
			bits_t pattern(width);
			க்கு (முழு i = 0; i < width; i++) {
				எனில் (sig[i].wire == வெற்று && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				இல்லையெனில்
					pattern[i] = RTLIL::State::Sa;
			}
			database.செருகு(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(முழு width)
	{
		this->width = width;
		எனில் (width > 0) {
			bits_t pattern(width);
			க்கு (முழு i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.செருகு(pattern);
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
		க்கு (auto &b : bits.bitdata)
			எனில் (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		திரும்பு bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	நிலையான bool covers_nothing(RTLIL::SigSpec sig)
	{
		க்கு (auto bit : sig)
			எனில் (bit.wire == வெற்று && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				திரும்பு true;
		திரும்பு false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(முழு(a.bitdata.size()) == width);
		log_assert(முழு(b.bitdata.size()) == width);
		க்கு (முழு i = 0; i < width; i++)
			எனில் (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				திரும்பு false;
		திரும்பு true;
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
		எனில் (covers_nothing(sig))
			திரும்பு false;
		bits_t bits = sig2bits(sig);
		க்கு (auto &it : database)
			எனில் (match(it, bits))
				திரும்பு true;
		திரும்பு false;
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
		எனில் (covers_nothing(sig))
			திரும்பு true;
		bits_t bits = sig2bits(sig);
		க்கு (auto &it : database)
			எனில் (match(it, bits)) {
				க்கு (முழு i = 0; i < width; i++)
					எனில் (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						செல் next_database_entry;
				திரும்பு true;
	next_database_entry:;
			}
		திரும்பு false;
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
		எனில் (covers_nothing(sig))
			திரும்பு false;
		bits_t bits = sig2bits(sig);
		க்கு (auto it = database.begin(); it != database.end();)
			எனில் (match(*it, bits)) {
				க்கு (முழு i = 0; i < width; i++) {
					எனில் ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						தொடர்;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.செருகு(new_pattern);
				}
				it = database.erase(it);
				status = true;
				தொடர்;
			} இல்லையெனில்
				++it;
		திரும்பு status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		எனில் (database.empty())
			திரும்பு false;
		database.clear();
		திரும்பு true;
	}

	bool empty()
	{
		திரும்பு database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
