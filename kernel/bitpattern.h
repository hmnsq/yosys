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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ BITPATTERN_H
#ವ್ಯಾಖ್ಯಾನಿಸು BITPATTERN_H

#ಅನ್ತರ್ಭಾವ "kernel/log.h"
#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"

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
ರಚನೆ ಬಿಟ್_ನಮೂನೆ_Pool
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ;
	ರಚನೆ ಬಿಟ್ಟುಗಳು_t {
		std::ಸದಿಶ<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		ಬಿಟ್ಟುಗಳು_t(ಪೂರ್ಣಾಂಕ ಅಗಲ = 0) : bitdata(ಅಗಲ), cached_hash(0) { }
		RTLIL::State &ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) {
			ಹಿಂತಿರುಗಿ bitdata[ಸೂಚ್ಯಂಕ];
		}
		ಸ್ಥಿರಾಂಕ RTLIL::State &ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ bitdata[ಸೂಚ್ಯಂಕ];
		}
		bool ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ ಬಿಟ್ಟುಗಳು_t &other) ಸ್ಥಿರಾಂಕ {
			ಆದರೆ (ಸತತ_hash(*this) != ಸತತ_hash(other))
				ಹಿಂತಿರುಗಿ false;
			ಹಿಂತಿರುಗಿ bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
			ಆದರೆ (!cached_hash)
				cached_hash = ಸತತ_hash(bitdata);
			h.eat(cached_hash);
			ಹಿಂತಿರುಗಿ h;
		}
	};
	pool<ಬಿಟ್ಟುಗಳು_t> database;

	ಬಿಟ್_ನಮೂನೆ_Pool(RTLIL::SigSpec sig)
	{
		ಅಗಲ = sig.ಗಾತ್ರ_2();
		ಆದರೆ (ಅಗಲ > 0) {
			ಬಿಟ್ಟುಗಳು_t ನಮೂನೆ(ಅಗಲ);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++) {
				ಆದರೆ (sig[i].wire == ಖಾಲಿ && sig[i].ದತ್ತಾಂಶ <= RTLIL::State::S1)
					ನಮೂನೆ[i] = sig[i].ದತ್ತಾಂಶ;
				ಇಲ್ಲದಿದ್ದರೆ
					ನಮೂನೆ[i] = RTLIL::State::Sa;
			}
			database.ಸೇರಿಸು(ನಮೂನೆ);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	ಬಿಟ್_ನಮೂನೆ_Pool(ಪೂರ್ಣಾಂಕ ಅಗಲ)
	{
		this->ಅಗಲ = ಅಗಲ;
		ಆದರೆ (ಅಗಲ > 0) {
			ಬಿಟ್ಟುಗಳು_t ನಮೂನೆ(ಅಗಲ);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
				ನಮೂನೆ[i] = RTLIL::State::Sa;
			database.ಸೇರಿಸು(ನಮೂನೆ);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	ಬಿಟ್ಟುಗಳು_t sig2bits(RTLIL::SigSpec sig)
	{
		ಬಿಟ್ಟುಗಳು_t ಬಿಟ್ಟುಗಳು;
		ಬಿಟ್ಟುಗಳು.bitdata = sig.as_const().ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು();
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &b : ಬಿಟ್ಟುಗಳು.bitdata)
			ಆದರೆ (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		ಹಿಂತಿರುಗಿ ಬಿಟ್ಟುಗಳು;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	ಸ್ಥಿರ bool covers_nothing(RTLIL::SigSpec sig)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire == ಖಾಲಿ && (ಬಿಟ್.ದತ್ತಾಂಶ == RTLIL::State::Sx || ಬಿಟ್.ದತ್ತಾಂಶ == RTLIL::State::Sz))
				ಹಿಂತಿರುಗಿ true;
		ಹಿಂತಿರುಗಿ false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool ಹೊಂದಿಕೆ(ಬಿಟ್ಟುಗಳು_t a, ಬಿಟ್ಟುಗಳು_t b)
	{
		log_assert(ಪೂರ್ಣಾಂಕ(a.bitdata.ಗಾತ್ರ_2()) == ಅಗಲ);
		log_assert(ಪೂರ್ಣಾಂಕ(b.bitdata.ಗಾತ್ರ_2()) == ಅಗಲ);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
			ಆದರೆ (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				ಹಿಂತಿರುಗಿ false;
		ಹಿಂತಿರುಗಿ true;
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
		ಆದರೆ (covers_nothing(sig))
			ಹಿಂತಿರುಗಿ false;
		ಬಿಟ್ಟುಗಳು_t ಬಿಟ್ಟುಗಳು = sig2bits(sig);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : database)
			ಆದರೆ (ಹೊಂದಿಕೆ(it, ಬಿಟ್ಟುಗಳು))
				ಹಿಂತಿರುಗಿ true;
		ಹಿಂತಿರುಗಿ false;
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
		ಆದರೆ (covers_nothing(sig))
			ಹಿಂತಿರುಗಿ true;
		ಬಿಟ್ಟುಗಳು_t ಬಿಟ್ಟುಗಳು = sig2bits(sig);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : database)
			ಆದರೆ (ಹೊಂದಿಕೆ(it, ಬಿಟ್ಟುಗಳು)) {
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
					ಆದರೆ (ಬಿಟ್ಟುಗಳು[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						ಹೋಗು ಮುಂದಿನ_database_ನಮೂದು;
				ಹಿಂತಿರುಗಿ true;
	ಮುಂದಿನ_database_ನಮೂದು:;
			}
		ಹಿಂತಿರುಗಿ false;
	}

	/**
	 * Remove cube sig from the pool, splitting the remaining cubes. True if success.
	 * For example:
	 * Taking 011 out of pool({01a}) -> pool({010}), returns true.
	 * Taking 011 out of pool({010}) does nothing, returns false.
	 */
	bool take(RTLIL::SigSpec sig)
	{
		bool ಸ್ಥಿತಿ_2 = false;
		ಆದರೆ (covers_nothing(sig))
			ಹಿಂತಿರುಗಿ false;
		ಬಿಟ್ಟುಗಳು_t ಬಿಟ್ಟುಗಳು = sig2bits(sig);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ it = database.begin(); it != database.ಕೊನೆ();)
			ಆದರೆ (ಹೊಂದಿಕೆ(*it, ಬಿಟ್ಟುಗಳು)) {
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++) {
					ಆದರೆ ((*it)[i] != RTLIL::State::Sa || ಬಿಟ್ಟುಗಳು[i] == RTLIL::State::Sa)
						ಮುಂದುವರಿ;
					ಬಿಟ್ಟುಗಳು_t new_ನಮೂನೆ;
					new_ನಮೂನೆ.bitdata = it->bitdata;
					new_ನಮೂನೆ[i] = ಬಿಟ್ಟುಗಳು[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.ಸೇರಿಸು(new_ನಮೂನೆ);
				}
				it = database.erase(it);
				ಸ್ಥಿತಿ_2 = true;
				ಮುಂದುವರಿ;
			} ಇಲ್ಲದಿದ್ದರೆ
				++it;
		ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		ಆದರೆ (database.ಖಾಲಿಗಳು())
			ಹಿಂತಿರುಗಿ false;
		database.clear();
		ಹಿಂತಿರುಗಿ true;
	}

	bool ಖಾಲಿಗಳು()
	{
		ಹಿಂತಿರುಗಿ database.ಖಾಲಿಗಳು();
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
