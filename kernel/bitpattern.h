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

#include "kernel/log.h"
#include "kernel/rtlil.h"

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
sa_nracanaa BitPatternPool
{
	จำนวนเต็ม width;
	sa_nracanaa bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(จำนวนเต็ม width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](จำนวนเต็ม index) {
			คืน bitdata[index];
		}
		const RTLIL::State &operator[](จำนวนเต็ม index) const {
			คืน bitdata[index];
		}
		bool operator==(const bits_t &other) const {
			ถ้า (run_hash(*this) != run_hash(other))
				คืน false;
			คืน bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) const {
			ถ้า (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			คืน h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		ถ้า (width > 0) {
			bits_t pattern(width);
			สำหรับ (จำนวนเต็ม i = 0; i < width; i++) {
				ถ้า (sig[i].wire == NULL && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				มิฉะนั้น
					pattern[i] = RTLIL::State::Sa;
			}
			database.insert(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(จำนวนเต็ม width)
	{
		this->width = width;
		ถ้า (width > 0) {
			bits_t pattern(width);
			สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.insert(pattern);
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
		สำหรับ (auto &b : bits.bitdata)
			ถ้า (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		คืน bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	static bool covers_nothing(RTLIL::SigSpec sig)
	{
		สำหรับ (auto bit : sig)
			ถ้า (bit.wire == NULL && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				คืน true;
		คืน false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(จำนวนเต็ม(a.bitdata.size()) == width);
		log_assert(จำนวนเต็ม(b.bitdata.size()) == width);
		สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
			ถ้า (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				คืน false;
		คืน true;
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
		ถ้า (covers_nothing(sig))
			คืน false;
		bits_t bits = sig2bits(sig);
		สำหรับ (auto &it : database)
			ถ้า (match(it, bits))
				คืน true;
		คืน false;
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
		ถ้า (covers_nothing(sig))
			คืน true;
		bits_t bits = sig2bits(sig);
		สำหรับ (auto &it : database)
			ถ้า (match(it, bits)) {
				สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
					ถ้า (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						goto next_database_entry;
				คืน true;
	next_database_entry:;
			}
		คืน false;
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
		ถ้า (covers_nothing(sig))
			คืน false;
		bits_t bits = sig2bits(sig);
		สำหรับ (auto it = database.begin(); it != database.end();)
			ถ้า (match(*it, bits)) {
				สำหรับ (จำนวนเต็ม i = 0; i < width; i++) {
					ถ้า ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						continue;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.insert(new_pattern);
				}
				it = database.erase(it);
				status = true;
				continue;
			} มิฉะนั้น
				++it;
		คืน status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		ถ้า (database.empty())
			คืน false;
		database.clear();
		คืน true;
	}

	bool empty()
	{
		คืน database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
