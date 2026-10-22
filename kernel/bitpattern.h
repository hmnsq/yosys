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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ BITPATTERN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BITPATTERN_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"

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
ਬਣਤਰ ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Pool
{
	ਪੂਰਨ ਚੌੜਾਈ;
	ਬਣਤਰ ਬਿੱਟ_t {
		std::ਸਦਿਸ਼<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		ਬਿੱਟ_t(ਪੂਰਨ ਚੌੜਾਈ = 0) : bitdata(ਚੌੜਾਈ), cached_hash(0) { }
		RTLIL::State &ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) {
			ਵਾਪਸ bitdata[ਸੂਚਕਾਂਕ];
		}
		ਅਚਲ RTLIL::State &ਸੰਚਾਲਕ[](ਪੂਰਨ ਸੂਚਕਾਂਕ) ਅਚਲ {
			ਵਾਪਸ bitdata[ਸੂਚਕਾਂਕ];
		}
		bool ਸੰਚਾਲਕ==(ਅਚਲ ਬਿੱਟ_t &other) ਅਚਲ {
			ਜੇ (ਲੜੀਵਾਰ_hash(*this) != ਲੜੀਵਾਰ_hash(other))
				ਵਾਪਸ false;
			ਵਾਪਸ bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
			ਜੇ (!cached_hash)
				cached_hash = ਲੜੀਵਾਰ_hash(bitdata);
			h.eat(cached_hash);
			ਵਾਪਸ h;
		}
	};
	pool<ਬਿੱਟ_t> database;

	ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Pool(RTLIL::SigSpec sig)
	{
		ਚੌੜਾਈ = sig.ਮਾਪ();
		ਜੇ (ਚੌੜਾਈ > 0) {
			ਬਿੱਟ_t ਨਮੂਨਾ(ਚੌੜਾਈ);
			ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++) {
				ਜੇ (sig[i].wire == ਨਿਰਾ && sig[i].ਅੰਕੜੇ <= RTLIL::State::S1)
					ਨਮੂਨਾ[i] = sig[i].ਅੰਕੜੇ;
				ਨਹੀਂਤਾਂ
					ਨਮੂਨਾ[i] = RTLIL::State::Sa;
			}
			database.ਪਾਓ(ਨਮੂਨਾ);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਮੂਨਾ_Pool(ਪੂਰਨ ਚੌੜਾਈ)
	{
		this->ਚੌੜਾਈ = ਚੌੜਾਈ;
		ਜੇ (ਚੌੜਾਈ > 0) {
			ਬਿੱਟ_t ਨਮੂਨਾ(ਚੌੜਾਈ);
			ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
				ਨਮੂਨਾ[i] = RTLIL::State::Sa;
			database.ਪਾਓ(ਨਮੂਨਾ);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	ਬਿੱਟ_t sig2bits(RTLIL::SigSpec sig)
	{
		ਬਿੱਟ_t ਬਿੱਟ;
		ਬਿੱਟ.bitdata = sig.as_const().ਮੰਜ਼ਿਲ_ਬਿੱਟ();
		ਲਈ (ਸਵੈਚਾਲਿਤ &b : ਬਿੱਟ.bitdata)
			ਜੇ (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		ਵਾਪਸ ਬਿੱਟ;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ਸਥਿਰ bool covers_nothing(RTLIL::SigSpec sig)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire == ਨਿਰਾ && (ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ == RTLIL::State::Sx || ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ == RTLIL::State::Sz))
				ਵਾਪਸ true;
		ਵਾਪਸ false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool ਮੇਲ(ਬਿੱਟ_t a, ਬਿੱਟ_t b)
	{
		log_assert(ਪੂਰਨ(a.bitdata.ਮਾਪ()) == ਚੌੜਾਈ);
		log_assert(ਪੂਰਨ(b.bitdata.ਮਾਪ()) == ਚੌੜਾਈ);
		ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
			ਜੇ (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				ਵਾਪਸ false;
		ਵਾਪਸ true;
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
		ਜੇ (covers_nothing(sig))
			ਵਾਪਸ false;
		ਬਿੱਟ_t ਬਿੱਟ = sig2bits(sig);
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : database)
			ਜੇ (ਮੇਲ(it, ਬਿੱਟ))
				ਵਾਪਸ true;
		ਵਾਪਸ false;
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
		ਜੇ (covers_nothing(sig))
			ਵਾਪਸ true;
		ਬਿੱਟ_t ਬਿੱਟ = sig2bits(sig);
		ਲਈ (ਸਵੈਚਾਲਿਤ &it : database)
			ਜੇ (ਮੇਲ(it, ਬਿੱਟ)) {
				ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
					ਜੇ (ਬਿੱਟ[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						ਜਾਓ ਅਗਲਾ_database_ਇੰਦਰਾਜ;
				ਵਾਪਸ true;
	ਅਗਲਾ_database_ਇੰਦਰਾਜ:;
			}
		ਵਾਪਸ false;
	}

	/**
	 * Remove cube sig from the pool, splitting the remaining cubes. True if success.
	 * For example:
	 * Taking 011 out of pool({01a}) -> pool({010}), returns true.
	 * Taking 011 out of pool({010}) does nothing, returns false.
	 */
	bool take(RTLIL::SigSpec sig)
	{
		bool ਸਥਿਤੀ = false;
		ਜੇ (covers_nothing(sig))
			ਵਾਪਸ false;
		ਬਿੱਟ_t ਬਿੱਟ = sig2bits(sig);
		ਲਈ (ਸਵੈਚਾਲਿਤ it = database.begin(); it != database.ਸਿਰਾ();)
			ਜੇ (ਮੇਲ(*it, ਬਿੱਟ)) {
				ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++) {
					ਜੇ ((*it)[i] != RTLIL::State::Sa || ਬਿੱਟ[i] == RTLIL::State::Sa)
						ਜਾਰੀ;
					ਬਿੱਟ_t new_ਨਮੂਨਾ;
					new_ਨਮੂਨਾ.bitdata = it->bitdata;
					new_ਨਮੂਨਾ[i] = ਬਿੱਟ[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ਪਾਓ(new_ਨਮੂਨਾ);
				}
				it = database.erase(it);
				ਸਥਿਤੀ = true;
				ਜਾਰੀ;
			} ਨਹੀਂਤਾਂ
				++it;
		ਵਾਪਸ ਸਥਿਤੀ;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		ਜੇ (database.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਾਪਸ false;
		database.clear();
		ਵਾਪਸ true;
	}

	bool ਖਾਲੀ_ਥਾਵਾਂ()
	{
		ਵਾਪਸ database.ਖਾਲੀ_ਥਾਵਾਂ();
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
