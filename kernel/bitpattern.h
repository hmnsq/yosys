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

#അന്തര്ഭാവ "kernel/log.h"
#അന്തര്ഭാവ "kernel/rtlil.h"

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
ഘടന BitPatternPool
{
	പൂർണ്ണസംഖ്യ width;
	ഘടന bits_t {
		std::vector<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		bits_t(പൂർണ്ണസംഖ്യ width = 0) : bitdata(width), cached_hash(0) { }
		RTLIL::State &operator[](പൂർണ്ണസംഖ്യ index) {
			തിരികെ bitdata[index];
		}
		സ്ഥിരം RTLIL::State &operator[](പൂർണ്ണസംഖ്യ index) സ്ഥിരം {
			തിരികെ bitdata[index];
		}
		bool operator==(സ്ഥിരം bits_t &other) സ്ഥിരം {
			എങ്കിൽ (run_hash(*this) != run_hash(other))
				തിരികെ false;
			തിരികെ bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
			എങ്കിൽ (!cached_hash)
				cached_hash = run_hash(bitdata);
			h.eat(cached_hash);
			തിരികെ h;
		}
	};
	pool<bits_t> database;

	BitPatternPool(RTLIL::SigSpec sig)
	{
		width = sig.size();
		എങ്കിൽ (width > 0) {
			bits_t pattern(width);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++) {
				എങ്കിൽ (sig[i].wire == ശൂന്യ && sig[i].data <= RTLIL::State::S1)
					pattern[i] = sig[i].data;
				അല്ലെങ്കിൽ
					pattern[i] = RTLIL::State::Sa;
			}
			database.തിരുകുക(pattern);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	BitPatternPool(പൂർണ്ണസംഖ്യ width)
	{
		this->width = width;
		എങ്കിൽ (width > 0) {
			bits_t pattern(width);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
				pattern[i] = RTLIL::State::Sa;
			database.തിരുകുക(pattern);
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
		വേണ്ടി (auto &b : bits.bitdata)
			എങ്കിൽ (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		തിരികെ bits;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	സ്ഥിതം bool covers_nothing(RTLIL::SigSpec sig)
	{
		വേണ്ടി (auto bit : sig)
			എങ്കിൽ (bit.wire == ശൂന്യ && (bit.data == RTLIL::State::Sx || bit.data == RTLIL::State::Sz))
				തിരികെ true;
		തിരികെ false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool match(bits_t a, bits_t b)
	{
		log_assert(പൂർണ്ണസംഖ്യ(a.bitdata.size()) == width);
		log_assert(പൂർണ്ണസംഖ്യ(b.bitdata.size()) == width);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
			എങ്കിൽ (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				തിരികെ false;
		തിരികെ true;
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
		എങ്കിൽ (covers_nothing(sig))
			തിരികെ false;
		bits_t bits = sig2bits(sig);
		വേണ്ടി (auto &it : database)
			എങ്കിൽ (match(it, bits))
				തിരികെ true;
		തിരികെ false;
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
		എങ്കിൽ (covers_nothing(sig))
			തിരികെ true;
		bits_t bits = sig2bits(sig);
		വേണ്ടി (auto &it : database)
			എങ്കിൽ (match(it, bits)) {
				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
					എങ്കിൽ (bits[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						പോകുക next_database_entry;
				തിരികെ true;
	next_database_entry:;
			}
		തിരികെ false;
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
		എങ്കിൽ (covers_nothing(sig))
			തിരികെ false;
		bits_t bits = sig2bits(sig);
		വേണ്ടി (auto it = database.begin(); it != database.end();)
			എങ്കിൽ (match(*it, bits)) {
				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++) {
					എങ്കിൽ ((*it)[i] != RTLIL::State::Sa || bits[i] == RTLIL::State::Sa)
						തുടരുക;
					bits_t new_pattern;
					new_pattern.bitdata = it->bitdata;
					new_pattern[i] = bits[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.തിരുകുക(new_pattern);
				}
				it = database.erase(it);
				status = true;
				തുടരുക;
			} അല്ലെങ്കിൽ
				++it;
		തിരികെ status;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		എങ്കിൽ (database.empty())
			തിരികെ false;
		database.clear();
		തിരികെ true;
	}

	bool empty()
	{
		തിരികെ database.empty();
	}
};

YOSYS_NAMESPACE_END

#endif
