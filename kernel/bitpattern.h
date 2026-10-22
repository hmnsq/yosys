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

#अन्तर्भाव "kernel/log.h"
#अन्तर्भाव "kernel/rtlil.h"

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
संरचना BitPatternPool
{
	पूर्णाङ्क width;
	संरचना bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(पूर्णाङ्क width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](पूर्णाङ्क index) {
			पटिनिवत्त bitdata[index];
		}
		ध्रुव RTLIL::State &operator[](पूर्णाङ्क index) ध्रुव {
			पटिनिवत्त bitdata[index];
		}
		bool operator==(ध्रुव bits_t &other) ध्रुव {
			सचे (run_hash(*this) != run_hash(other))
				पटिनिवत्त false;
			पटिनिवत्त bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			सचे (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			पटिनिवत्त h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		सचे (width > 0) {
			bits_t pattern(width);
			अत्थाय (पूर्णाङ्क i = 0; i < width; i++) {
				सचे (sig[i].wire == रिक्त && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				अञ्ञथा
					pattern[i] = RTLIL::State::Sa;
			}
			database.निवेशय(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(पूर्णाङ्क width)
	{
		this->width = width;
		सचे (width > 0) {
			bits_t pattern(width);
			अत्थाय (पूर्णाङ्क i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.निवेशय(pattern);
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
		अत्थाय (auto &b : bits.bitdata)
			सचे (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		पटिनिवत्त bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	स्थिर bool covers_nothing(RTLIL::SigSpec sig)
	{
		अत्थाय (auto bit : sig)
			सचे (bit.wire == रिक्त && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				पटिनिवत्त true;
		पटिनिवत्त false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(पूर्णाङ्क(a.bitdata.size()) == width);
		log_assert(पूर्णाङ्क(b.bitdata.size()) == width);
		अत्थाय (पूर्णाङ्क i = 0; i < width; i++)
			सचे (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				पटिनिवत्त false;
		पटिनिवत्त true;
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
		सचे (covers_nothing(sig))
			पटिनिवत्त false;
		bits_t bits = sig2bits(sig);
		अत्थाय (auto &it : database)
			सचे (match(it, bits))
				पटिनिवत्त true;
		पटिनिवत्त false;
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
		सचे (covers_nothing(sig))
			पटिनिवत्त true;
		bits_t bits = sig2bits(sig);
		अत्थाय (auto &it : database)
			सचे (match(it, bits)) {
				अत्थाय (पूर्णाङ्क i = 0; i < width; i++)
					सचे (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						गच्छ next_database_entry;
				पटिनिवत्त true;
	next_database_entry:;
			}
		पटिनिवत्त false;
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
		सचे (covers_nothing(sig))
			पटिनिवत्त false;
		bits_t bits = sig2bits(sig);
		अत्थाय (auto it = database.begin(); it != database.end();)
			सचे (match(*it, bits)) {
				अत्थाय (पूर्णाङ्क i = 0; i < width; i++) {
					सचे ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						अनुवर्त;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.निवेशय(new_pattern);
				}
				it = database.erase(it);
				status = true;
				अनुवर्त;
			} अञ्ञथा
				++it;
		पटिनिवत्त status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		सचे (database.empty())
			पटिनिवत्त false;
		database.clear();
		पटिनिवत्त true;
	}

	bool empty()
	{
		पटिनिवत्त database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
