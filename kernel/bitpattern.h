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

#ଅନ୍ତର୍ଭାଵ "kernel/log.h"
#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"

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
ସଂରଚନା BitPatternPool
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width;
	ସଂରଚନା bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) {
			ଫେରନ୍ତୁ bitdata[index];
		}
		ଅଚଳ RTLIL::State &operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) ଅଚଳ {
			ଫେରନ୍ତୁ bitdata[index];
		}
		bool operator==(ଅଚଳ bits_t &other) ଅଚଳ {
			ଯଦି (run_hash(*this) != run_hash(other))
				ଫେରନ୍ତୁ false;
			ଫେରନ୍ତୁ bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
			ଯଦି (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			ଫେରନ୍ତୁ h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		ଯଦି (width > 0) {
			bits_t pattern(width);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++) {
				ଯଦି (sig[i].wire == ଖାଲି && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				ଅନ୍ୟଥା
					pattern[i] = RTLIL::State::Sa;
			}
			database.ଭର୍ତ୍ତି(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
	{
		this->width = width;
		ଯଦି (width > 0) {
			bits_t pattern(width);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.ଭର୍ତ୍ତି(pattern);
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
		ପାଇଁ (auto &b : bits.bitdata)
			ଯଦି (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		ଫେରନ୍ତୁ bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ସ୍ଥିର bool covers_nothing(RTLIL::SigSpec sig)
	{
		ପାଇଁ (auto bit : sig)
			ଯଦି (bit.wire == ଖାଲି && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				ଫେରନ୍ତୁ true;
		ଫେରନ୍ତୁ false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(ପୂର୍ଣ୍ଣସଂଖ୍ୟା(a.bitdata.size()) == width);
		log_assert(ପୂର୍ଣ୍ଣସଂଖ୍ୟା(b.bitdata.size()) == width);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
			ଯଦି (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				ଫେରନ୍ତୁ false;
		ଫେରନ୍ତୁ true;
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
		ଯଦି (covers_nothing(sig))
			ଫେରନ୍ତୁ false;
		bits_t bits = sig2bits(sig);
		ପାଇଁ (auto &it : database)
			ଯଦି (match(it, bits))
				ଫେରନ୍ତୁ true;
		ଫେରନ୍ତୁ false;
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
		ଯଦି (covers_nothing(sig))
			ଫେରନ୍ତୁ true;
		bits_t bits = sig2bits(sig);
		ପାଇଁ (auto &it : database)
			ଯଦି (match(it, bits)) {
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
					ଯଦି (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						ଯାଅ next_database_entry;
				ଫେରନ୍ତୁ true;
	next_database_entry:;
			}
		ଫେରନ୍ତୁ false;
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
		ଯଦି (covers_nothing(sig))
			ଫେରନ୍ତୁ false;
		bits_t bits = sig2bits(sig);
		ପାଇଁ (auto it = database.begin(); it != database.end();)
			ଯଦି (match(*it, bits)) {
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++) {
					ଯଦି ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						ଜାରି;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ଭର୍ତ୍ତି(new_pattern);
				}
				it = database.erase(it);
				status = true;
				ଜାରି;
			} ଅନ୍ୟଥା
				++it;
		ଫେରନ୍ତୁ status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		ଯଦି (database.empty())
			ଫେରନ୍ତୁ false;
		database.clear();
		ଫେରନ୍ତୁ true;
	}

	bool empty()
	{
		ଫେରନ୍ତୁ database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
