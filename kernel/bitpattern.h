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

#यदि_अपरिभाषितम् BITPATTERN_H
#परिभाषय BITPATTERN_H

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
संरचना Bit_आदर्शः_Pool
{
	पूर्णाङ्क विस्तारः;
	संरचना द्वयङ्काः_t {
		std::सदिशम्<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		द्वयङ्काः_t(पूर्णाङ्क विस्तारः = 0) : bitdata(विस्तारः), cached_hash(0) { }
		RTLIL::State &संक्रियकः[](पूर्णाङ्क सूचकांक) {
			प्रत्यागम bitdata[सूचकांक];
		}
		ध्रुव RTLIL::State &संक्रियकः[](पूर्णाङ्क सूचकांक) ध्रुव {
			प्रत्यागम bitdata[सूचकांक];
		}
		bool संक्रियकः==(ध्रुव द्वयङ्काः_t &other) ध्रुव {
			यदि (सततम्_hash(*this) != सततम्_hash(other))
				प्रत्यागम false;
			प्रत्यागम bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
			यदि (!cached_hash)
				cached_hash = सततम्_hash(bitdata);
			h.eat(cached_hash);
			प्रत्यागम h;
		}
	};
	pool<द्वयङ्काः_t> database;

	Bit_आदर्शः_Pool(RTLIL::SigSpec sig)
	{
		विस्तारः = sig.आकार();
		यदि (विस्तारः > 0) {
			द्वयङ्काः_t आदर्शः(विस्तारः);
			प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++) {
				यदि (sig[i].wire == रिक्त && sig[i].दत्त <= RTLIL::State::S1)
					आदर्शः[i] = sig[i].दत्त;
				अन्यथा
					आदर्शः[i] = RTLIL::State::Sa;
			}
			database.निवेशय(आदर्शः);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	Bit_आदर्शः_Pool(पूर्णाङ्क विस्तारः)
	{
		this->विस्तारः = विस्तारः;
		यदि (विस्तारः > 0) {
			द्वयङ्काः_t आदर्शः(विस्तारः);
			प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
				आदर्शः[i] = RTLIL::State::Sa;
			database.निवेशय(आदर्शः);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	द्वयङ्काः_t sig2bits(RTLIL::SigSpec sig)
	{
		द्वयङ्काः_t द्वयङ्काः;
		द्वयङ्काः.bitdata = sig.as_const().गन्तव्यम्_द्वयङ्काः();
		प्रति (स्वचालित &b : द्वयङ्काः.bitdata)
			यदि (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		प्रत्यागम द्वयङ्काः;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	स्थिर bool covers_nothing(RTLIL::SigSpec sig)
	{
		प्रति (स्वचालित bit : sig)
			यदि (bit.wire == रिक्त && (bit.दत्त == RTLIL::State::Sx || bit.दत्त == RTLIL::State::Sz))
				प्रत्यागम true;
		प्रत्यागम false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool मेलनम्(द्वयङ्काः_t a, द्वयङ्काः_t b)
	{
		log_assert(पूर्णाङ्क(a.bitdata.आकार()) == विस्तारः);
		log_assert(पूर्णाङ्क(b.bitdata.आकार()) == विस्तारः);
		प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
			यदि (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				प्रत्यागम false;
		प्रत्यागम true;
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
		यदि (covers_nothing(sig))
			प्रत्यागम false;
		द्वयङ्काः_t द्वयङ्काः = sig2bits(sig);
		प्रति (स्वचालित &it : database)
			यदि (मेलनम्(it, द्वयङ्काः))
				प्रत्यागम true;
		प्रत्यागम false;
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
		यदि (covers_nothing(sig))
			प्रत्यागम true;
		द्वयङ्काः_t द्वयङ्काः = sig2bits(sig);
		प्रति (स्वचालित &it : database)
			यदि (मेलनम्(it, द्वयङ्काः)) {
				प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
					यदि (द्वयङ्काः[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						गच्छ अग्र_database_प्रविष्टिः;
				प्रत्यागम true;
	अग्र_database_प्रविष्टिः:;
			}
		प्रत्यागम false;
	}

	/**
	 * Remove cube sig from the pool, splitting the remaining cubes. True if success.
	 * For example:
	 * Taking 011 out of pool({01a}) -> pool({010}), returns true.
	 * Taking 011 out of pool({010}) does nothing, returns false.
	 */
	bool take(RTLIL::SigSpec sig)
	{
		bool स्थितिः = false;
		यदि (covers_nothing(sig))
			प्रत्यागम false;
		द्वयङ्काः_t द्वयङ्काः = sig2bits(sig);
		प्रति (स्वचालित it = database.begin(); it != database.अन्तः();)
			यदि (मेलनम्(*it, द्वयङ्काः)) {
				प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++) {
					यदि ((*it)[i] != RTLIL::State::Sa || द्वयङ्काः[i] == RTLIL::State::Sa)
						अनुवर्त;
					द्वयङ्काः_t new_आदर्शः;
					new_आदर्शः.bitdata = it->bitdata;
					new_आदर्शः[i] = द्वयङ्काः[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.निवेशय(new_आदर्शः);
				}
				it = database.erase(it);
				स्थितिः = true;
				अनुवर्त;
			} अन्यथा
				++it;
		प्रत्यागम स्थितिः;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		यदि (database.रिक्तानि())
			प्रत्यागम false;
		database.clear();
		प्रत्यागम true;
	}

	bool रिक्तानि()
	{
		प्रत्यागम database.रिक्तानि();
	}
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
