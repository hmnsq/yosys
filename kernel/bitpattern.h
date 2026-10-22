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

#నిర్వచితం_కాకపోతే BITPATTERN_H
#నిర్వచించు BITPATTERN_H

#అన్తర్భావ "kernel/log.h"
#అన్తర్భావ "kernel/rtlil.h"

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
నిర్మాణం బిట్టు_నమూనా_Pool
{
	పూర్ణాంకం వెడల్పు;
	నిర్మాణం బిట్లు_t {
		std::సదిశ<RTLIL::State> bitdata;
		mutable Hasher::hash_t cached_hash;
		బిట్లు_t(పూర్ణాంకం వెడల్పు = 0) : bitdata(వెడల్పు), cached_hash(0) { }
		RTLIL::State &క్రియాకారకం[](పూర్ణాంకం సూచీ) {
			తిరిగి bitdata[సూచీ];
		}
		స్థిరరాశి RTLIL::State &క్రియాకారకం[](పూర్ణాంకం సూచీ) స్థిరరాశి {
			తిరిగి bitdata[సూచీ];
		}
		bool క్రియాకారకం==(స్థిరరాశి బిట్లు_t &other) స్థిరరాశి {
			ఒకవేళ (నిరంతరం_hash(*this) != నిరంతరం_hash(other))
				తిరిగి false;
			తిరిగి bitdata == other.bitdata;
		}
		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
			ఒకవేళ (!cached_hash)
				cached_hash = నిరంతరం_hash(bitdata);
			h.eat(cached_hash);
			తిరిగి h;
		}
	};
	pool<బిట్లు_t> database;

	బిట్టు_నమూనా_Pool(RTLIL::SigSpec sig)
	{
		వెడల్పు = sig.కొలత();
		ఒకవేళ (వెడల్పు > 0) {
			బిట్లు_t నమూనా(వెడల్పు);
			కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++) {
				ఒకవేళ (sig[i].wire == ఖాళీ && sig[i].దత్తాంశం <= RTLIL::State::S1)
					నమూనా[i] = sig[i].దత్తాంశం;
				లేకపోతే
					నమూనా[i] = RTLIL::State::Sa;
			}
			database.చొప్పించు(నమూనా);
		}
	}

	/**
	 * Constructs a pool of all possible patterns (all don't-care bits)
	 */
	బిట్టు_నమూనా_Pool(పూర్ణాంకం వెడల్పు)
	{
		this->వెడల్పు = వెడల్పు;
		ఒకవేళ (వెడల్పు > 0) {
			బిట్లు_t నమూనా(వెడల్పు);
			కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
				నమూనా[i] = RTLIL::State::Sa;
			database.చొప్పించు(నమూనా);
		}
	}

	/**
	 * Convert a constant SigSpec to a pattern. Normalize Yosys many-valued
	 * to three-valued logic.
	 */
	బిట్లు_t sig2bits(RTLIL::SigSpec sig)
	{
		బిట్లు_t బిట్లు;
		బిట్లు.bitdata = sig.as_const().గమ్యం_బిట్లు();
		కొరకు (స్వయంచాలక &b : బిట్లు.bitdata)
			ఒకవేళ (b > RTLIL::State::S1)
				b = RTLIL::State::Sa;
		తిరిగి బిట్లు;
	}

	/**
	 * A literal x/z bit can never match a 2-valued selector, so a pattern containing
	 * one covers nothing.
	 */
	స్థిర bool covers_nothing(RTLIL::SigSpec sig)
	{
		కొరకు (స్వయంచాలక బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire == ఖాళీ && (బిట్టు.దత్తాంశం == RTLIL::State::Sx || బిట్టు.దత్తాంశం == RTLIL::State::Sz))
				తిరిగి true;
		తిరిగి false;
	}

	/**
	 * Two cubes match if their intersection is non-empty.
	 */
	bool సరిపోలిక(బిట్లు_t a, బిట్లు_t b)
	{
		log_assert(పూర్ణాంకం(a.bitdata.కొలత()) == వెడల్పు);
		log_assert(పూర్ణాంకం(b.bitdata.కొలత()) == వెడల్పు);
		కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
			ఒకవేళ (a[i] <= RTLIL::State::S1 && b[i] <= RTLIL::State::S1 && a[i] != b[i])
				తిరిగి false;
		తిరిగి true;
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
		ఒకవేళ (covers_nothing(sig))
			తిరిగి false;
		బిట్లు_t బిట్లు = sig2bits(sig);
		కొరకు (స్వయంచాలక &it : database)
			ఒకవేళ (సరిపోలిక(it, బిట్లు))
				తిరిగి true;
		తిరిగి false;
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
		ఒకవేళ (covers_nothing(sig))
			తిరిగి true;
		బిట్లు_t బిట్లు = sig2bits(sig);
		కొరకు (స్వయంచాలక &it : database)
			ఒకవేళ (సరిపోలిక(it, బిట్లు)) {
				కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
					ఒకవేళ (బిట్లు[i] > RTLIL::State::S1 && it[i] <= RTLIL::State::S1)
						వెళ్ళు తదుపరి_database_నమోదు;
				తిరిగి true;
	తదుపరి_database_నమోదు:;
			}
		తిరిగి false;
	}

	/**
	 * Remove cube sig from the pool, splitting the remaining cubes. True if success.
	 * For example:
	 * Taking 011 out of pool({01a}) -> pool({010}), returns true.
	 * Taking 011 out of pool({010}) does nothing, returns false.
	 */
	bool take(RTLIL::SigSpec sig)
	{
		bool స్థితి_2 = false;
		ఒకవేళ (covers_nothing(sig))
			తిరిగి false;
		బిట్లు_t బిట్లు = sig2bits(sig);
		కొరకు (స్వయంచాలక it = database.begin(); it != database.చివర();)
			ఒకవేళ (సరిపోలిక(*it, బిట్లు)) {
				కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++) {
					ఒకవేళ ((*it)[i] != RTLIL::State::Sa || బిట్లు[i] == RTLIL::State::Sa)
						కొనసాగు;
					బిట్లు_t new_నమూనా;
					new_నమూనా.bitdata = it->bitdata;
					new_నమూనా[i] = బిట్లు[i] == RTLIL::State::S1 ? RTLIL::State::S0 : RTLIL::State::S1;
					database.చొప్పించు(new_నమూనా);
				}
				it = database.erase(it);
				స్థితి_2 = true;
				కొనసాగు;
			} లేకపోతే
				++it;
		తిరిగి స్థితి_2;
	}

	/**
	 * Remove all patterns. Returns false if already empty.
	 */
	bool take_all()
	{
		ఒకవేళ (database.ఖాళీలు())
			తిరిగి false;
		database.clear();
		తిరిగి true;
	}

	bool ఖాళీలు()
	{
		తిరిగి database.ఖాళీలు();
	}
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
