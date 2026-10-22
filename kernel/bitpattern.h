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

#अगर_अघोषित BITPATTERN_H
#घोषणा BITPATTERN_H

#समावेश "kernel/log.h"
#समावेश "kernel/rtlil.h"

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
काष्ठा BitPatternPool
{
	पूर्णांक width;
	काष्ठा bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(पूर्णांक width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &चालक[](पूर्णांक सूचकांक) {
			वापस bitdata[सूचकांक];
		}
		स्थिर RTLIL::State &चालक[](पूर्णांक सूचकांक) स्थिर {
			वापस bitdata[सूचकांक];
		}
		bool चालक==(स्थिर bits_t &other) स्थिर {
			अगर (run_hash(*this) != run_hash(other))
				वापस false;
			वापस bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
			अगर (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			वापस h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.आकार();
		अगर (width > 0) {
			bits_t pattern(width);
			क्रम (पूर्णांक i = 0; i < width; i++) {
				अगर (sig[i].wire == शून्य && sig[i].तथ्य <= RTLIL::State::S1)
					pattern[i] = sig[i].तथ्य;
				अन्यथा
					pattern[i] = RTLIL::State::Sa;
			}
			database.जोड़ो(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(पूर्णांक width)
	{
		this->width = width;
		अगर (width > 0) {
			bits_t pattern(width);
			क्रम (पूर्णांक i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.जोड़ो(pattern);
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
		क्रम (स्वतः &b : bits.bitdata)
			अगर (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		वापस bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	अटल bool covers_nothing(RTLIL::SigSpec sig)
	{
		क्रम (स्वतः bit : sig)
			अगर (bit.wire == शून्य && (bit.तथ्य == RTLIL::State::Sx || bit.तथ्य == RTLIL::State::Sz))
				वापस true;
		वापस false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(पूर्णांक(a.bitdata.आकार()) == width);
		log_assert(पूर्णांक(b.bitdata.आकार()) == width);
		क्रम (पूर्णांक i = 0; i < width; i++)
			अगर (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				वापस false;
		वापस true;
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
		अगर (covers_nothing(sig))
			वापस false;
		bits_t bits = sig2bits(sig);
		क्रम (स्वतः &it : database)
			अगर (match(it, bits))
				वापस true;
		वापस false;
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
		अगर (covers_nothing(sig))
			वापस true;
		bits_t bits = sig2bits(sig);
		क्रम (स्वतः &it : database)
			अगर (match(it, bits)) {
				क्रम (पूर्णांक i = 0; i < width; i++)
					अगर (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						जाओ अग्र_database_entry;
				वापस true;
	अग्र_database_entry:;
			}
		वापस false;
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
		अगर (covers_nothing(sig))
			वापस false;
		bits_t bits = sig2bits(sig);
		क्रम (स्वतः it = database.begin(); it != database.end();)
			अगर (match(*it, bits)) {
				क्रम (पूर्णांक i = 0; i < width; i++) {
					अगर ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						जारी;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.जोड़ो(new_pattern);
				}
				it = database.erase(it);
				status = true;
				जारी;
			} अन्यथा
				++it;
		वापस status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		अगर (database.empty())
			वापस false;
		database.clear();
		वापस true;
	}

	bool empty()
	{
		वापस database.empty();
	}
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
