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

#અન્તર્ભાવ "kernel/log.h"
#અન્તર્ભાવ "kernel/rtlil.h"

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
સંરચના BitPatternPool
{
	પૂર્ણાંક width;
	સંરચના bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(પૂર્ણાંક width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](પૂર્ણાંક index) {
			પાછા bitdata[index];
		}
		અચલ RTLIL::State &operator[](પૂર્ણાંક index) અચલ {
			પાછા bitdata[index];
		}
		bool operator==(અચલ bits_t &other) અચલ {
			જો (run_hash(*this) != run_hash(other))
				પાછા false;
			પાછા bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
			જો (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			પાછા h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		જો (width > 0) {
			bits_t pattern(width);
			માટે (પૂર્ણાંક i = 0; i < width; i++) {
				જો (sig[i].wire == ખાલી && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				નહીંતર
					pattern[i] = RTLIL::State::Sa;
			}
			database.ઉમેરો(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(પૂર્ણાંક width)
	{
		this->width = width;
		જો (width > 0) {
			bits_t pattern(width);
			માટે (પૂર્ણાંક i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.ઉમેરો(pattern);
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
		માટે (auto &b : bits.bitdata)
			જો (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		પાછા bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	સ્થિર bool covers_nothing(RTLIL::SigSpec sig)
	{
		માટે (auto bit : sig)
			જો (bit.wire == ખાલી && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				પાછા true;
		પાછા false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(પૂર્ણાંક(a.bitdata.size()) == width);
		log_assert(પૂર્ણાંક(b.bitdata.size()) == width);
		માટે (પૂર્ણાંક i = 0; i < width; i++)
			જો (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				પાછા false;
		પાછા true;
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
		જો (covers_nothing(sig))
			પાછા false;
		bits_t bits = sig2bits(sig);
		માટે (auto &it : database)
			જો (match(it, bits))
				પાછા true;
		પાછા false;
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
		જો (covers_nothing(sig))
			પાછા true;
		bits_t bits = sig2bits(sig);
		માટે (auto &it : database)
			જો (match(it, bits)) {
				માટે (પૂર્ણાંક i = 0; i < width; i++)
					જો (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						જાઓ next_database_entry;
				પાછા true;
	next_database_entry:;
			}
		પાછા false;
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
		જો (covers_nothing(sig))
			પાછા false;
		bits_t bits = sig2bits(sig);
		માટે (auto it = database.begin(); it != database.end();)
			જો (match(*it, bits)) {
				માટે (પૂર્ણાંક i = 0; i < width; i++) {
					જો ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						ચાલુ;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ઉમેરો(new_pattern);
				}
				it = database.erase(it);
				status = true;
				ચાલુ;
			} નહીંતર
				++it;
		પાછા status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		જો (database.empty())
			પાછા false;
		database.clear();
		પાછા true;
	}

	bool empty()
	{
		પાછા database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
