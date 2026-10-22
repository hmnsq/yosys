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

#ಅನ್ತರ್ಭಾವ "kernel/satgen.h"
#ಅನ್ತರ್ಭಾವ "kernel/ff.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

ತಾರ್ಕಿಕ SatGen::importCell(RTLIL::Cell *cell, ಪೂರ್ಣಾಂಕ timestep)
{
	ತಾರ್ಕಿಕ arith_undef_handled = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ is_arith_compare = cell->type.ಒಳ(ID($lt), ID($le), ID($ge), ID($gt));

	ಆದರೆ (model_undef && (cell->type.ಒಳ(ID($ಕೂಡಿಸು), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		ಆದರೆ (is_arith_compare)
			extend_Signal_ಅಗಲ(undef_a, undef_b, cell, ನಿಜ);
		ಇಲ್ಲದಿದ್ದರೆ
			extend_Signal_ಅಗಲ(undef_a, undef_b, undef_y, cell, ನಿಜ);

		ಪೂರ್ಣಾಂಕ undef_any_a = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_a);
		ಪೂರ್ಣಾಂಕ undef_any_b = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_b);
		ಪೂರ್ಣಾಂಕ undef_y_ಬಿಟ್ = ez->OR(undef_any_a, undef_any_b);

		ಆದರೆ (cell->type.ಒಳ(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			undef_y_ಬಿಟ್ = ez->OR(undef_y_ಬಿಟ್, ez->NOT(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, b)));
		}

		ಆದರೆ (is_arith_compare) {
			ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < undef_y.ಗಾತ್ರ_2(); i++)
				ez->ಗಣ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->ಗಣ_2(undef_y_ಬಿಟ್, undef_y.at(0));
		} ಇಲ್ಲದಿದ್ದರೆ {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y_ಬಿಟ್ಟುಗಳು(undef_y.ಗಾತ್ರ_2(), undef_y_ಬಿಟ್);
			ez->assume(ez->vec_eq(undef_y_ಬಿಟ್ಟುಗಳು, undef_y));
		}

		arith_undef_handled = ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($ಕೂಡಿಸು), ID($sub)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		extend_Signal_ಅಗಲ(a, b, y, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		ಆದರೆ (cell->type.ಒಳ(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		ಆದರೆ (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		ಆದರೆ (cell->type.ಒಳ(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		ಆದರೆ (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		ಆದರೆ (cell->type.ಒಳ(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		ಆದರೆ (cell->type.ಒಳ(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		ಆದರೆ (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		ಆದರೆ (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		ಆದರೆ (cell->type == ID($ಕೂಡಿಸು))
			ez->assume(ez->vec_eq(ez->vec_ಕೂಡಿಸು(a, b), yy));
		ಆದರೆ (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		ಆದರೆ (model_undef && !arith_undef_handled)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			extend_Signal_ಅಗಲ(undef_a, undef_b, undef_y, cell, ಸುಳ್ಳು);

			ಆದರೆ (cell->type.ಒಳ(ID($and), ID($_AND_), ID($_NAND_))) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($or), ID($_OR_), ID($_NOR_))) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($_ANDNOT_)) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($_ORNOT_)) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ಇಲ್ಲದಿದ್ದರೆ
				log_abort();

			undefGating(y, yy, undef_y);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		ತಾರ್ಕಿಕ aoi_ಬಹುಲಕ = cell->type.ಒಳ(ID($_AOI3_), ID($_AOI4_));
		ತಾರ್ಕಿಕ three_ಬಹುಲಕ = cell->type.ಒಳ(ID($_AOI3_), ID($_OAI3_));

		ಪೂರ್ಣಾಂಕ a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep).at(0);
		ಪೂರ್ಣಾಂಕ b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep).at(0);
		ಪೂರ್ಣಾಂಕ c = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::C), timestep).at(0);
		ಪೂರ್ಣಾಂಕ d = three_ಬಹುಲಕ ? (aoi_ಬಹುಲಕ ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::D), timestep).at(0);
		ಪೂರ್ಣಾಂಕ y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep).at(0);
		ಪೂರ್ಣಾಂಕ yy = model_undef ? ez->literal() : y;

		ಆದರೆ (cell->type.ಒಳ(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		ಇಲ್ಲದಿದ್ದರೆ
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		ಆದರೆ (model_undef)
		{
			ಪೂರ್ಣಾಂಕ undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep).at(0);
			ಪೂರ್ಣಾಂಕ undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep).at(0);
			ಪೂರ್ಣಾಂಕ undef_c = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::C), timestep).at(0);
			ಪೂರ್ಣಾಂಕ undef_d = three_ಬಹುಲಕ ? ez->CONST_FALSE : importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::D), timestep).at(0);
			ಪೂರ್ಣಾಂಕ undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep).at(0);

			ಆದರೆ (aoi_ಬಹುಲಕ)
			{
				ಪೂರ್ಣಾಂಕ a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				ಪೂರ್ಣಾಂಕ b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				ಪೂರ್ಣಾಂಕ c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				ಪೂರ್ಣಾಂಕ d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				ಪೂರ್ಣಾಂಕ ab = ez->AND(a, b), cd = ez->AND(c, d);
				ಪೂರ್ಣಾಂಕ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				ಪೂರ್ಣಾಂಕ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				ಪೂರ್ಣಾಂಕ ab1 = ez->AND(ab, ez->NOT(undef_ab));
				ಪೂರ್ಣಾಂಕ cd1 = ez->AND(cd, ez->NOT(undef_cd));
				ಪೂರ್ಣಾಂಕ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
				ಪೂರ್ಣಾಂಕ a1 = ez->AND(a, ez->NOT(undef_a));
				ಪೂರ್ಣಾಂಕ b1 = ez->AND(b, ez->NOT(undef_b));
				ಪೂರ್ಣಾಂಕ c1 = ez->AND(c, ez->NOT(undef_c));
				ಪೂರ್ಣಾಂಕ d1 = ez->AND(d, ez->NOT(undef_d));

				ಪೂರ್ಣಾಂಕ ab = ez->OR(a, b), cd = ez->OR(c, d);
				ಪೂರ್ಣಾಂಕ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				ಪೂರ್ಣಾಂಕ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				ಪೂರ್ಣಾಂಕ ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				ಪೂರ್ಣಾಂಕ cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				ಪೂರ್ಣಾಂಕ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($_NOT_), ID($not)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		extend_Signal_ಅಗಲ_Unary(a, y, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		ಆದರೆ (model_undef) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			extend_Signal_ಅಗಲ_Unary(undef_a, undef_y, cell, ಸುಳ್ಳು);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($bweqx))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> bweqx = ez->vec_not(ez->vec_xor(a, b));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> both_undef = ez->vec_and(undef_a, undef_b);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> s = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;
		ಆದರೆ (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		ಇಲ್ಲದಿದ್ದರೆ
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_s = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yX;
			ಆದರೆ (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			ಇಲ್ಲದಿದ್ದರೆ
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($bmux))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> s = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a, undef_s, undef_y;

		ಆದರೆ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		}

		ಆದರೆ (Get_ಗಾತ್ರ(s) == 0) {
			ez->vec_ಗಣ(a, y);
			ಆದರೆ (model_undef)
				ez->vec_ಗಣ(undef_a, undef_y);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(s)-1; i >= 0; i--)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಹೊರಹೋಗು = (i == 0) ? y : ez->vec_var(a.ಗಾತ್ರ_2() / 2);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(ಹೊರಹೋಗು.ಗಾತ್ರ_2()) : ಹೊರಹೋಗು;

				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a0(a.begin(), a.begin() + a.ಗಾತ್ರ_2() / 2);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a1(a.begin() + a.ಗಾತ್ರ_2() / 2, a.ಕೊನೆ());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				ಆದರೆ (model_undef)
				{
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_ಹೊರಹೋಗು = (i == 0) ? undef_y : ez->vec_var(a.ಗಾತ್ರ_2() / 2);
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a0(undef_a.begin(), undef_a.begin() + a.ಗಾತ್ರ_2() / 2);
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a1(undef_a.begin() + a.ಗಾತ್ರ_2() / 2, undef_a.ಕೊನೆ());
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_ಹೊರಹೋಗು));
					undefGating(ಹೊರಹೋಗು, yy, undef_ಹೊರಹೋಗು);

					undef_a = undef_ಹೊರಹೋಗು;
				}

				a = ಹೊರಹೋಗು;
			}
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($demux))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> s = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a, undef_s, undef_y;

		ಆದರೆ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		}

		ಆದರೆ (Get_ಗಾತ್ರ(s) == 0) {
			ez->vec_ಗಣ(a, y);
			ಆದರೆ (model_undef)
				ez->vec_ಗಣ(undef_a, undef_y);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (1 << Get_ಗಾತ್ರ(s)); i++)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ss;
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(s); j++) {
					ಆದರೆ (i & 1 << j)
						ss.ತಳ್ಳು_ಹಿಂಭಾಗ(s[j]);
					ಇಲ್ಲದಿದ್ದರೆ
						ss.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->NOT(s[j]));
				}
				ಪೂರ್ಣಾಂಕ sss = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpAnd, ss);

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(a); j++) {
					ez->ಗಣ_2(ez->AND(sss, a[j]), yy.at(i * Get_ಗಾತ್ರ(a) + j));
				}

				ಆದರೆ (model_undef)
				{
					ಪೂರ್ಣಾಂಕ s0 = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					ಪೂರ್ಣಾಂಕ us = ez->AND(ez->NOT(s0), ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_s));
					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(a); j++) {
						ಪೂರ್ಣಾಂಕ a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						ಪೂರ್ಣಾಂಕ yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->ಗಣ_2(yX, undef_y.at(i * Get_ಗಾತ್ರ(a) + j));
					}
				}
			}
			ಆದರೆ (model_undef)
				undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($pmux))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> s = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> tmp = a;
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < s.ಗಾತ್ರ_2(); i++) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> part_ದ_b(b.begin()+i*a.ಗಾತ್ರ_2(), b.begin()+(i+1)*a.ಗಾತ್ರ_2());
			tmp = ez->vec_ite(s.at(i), part_ದ_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_s = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::S), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

			ಪೂರ್ಣಾಂಕ all_undef = ez->CONST_FALSE;
			ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_ಸಕ್ರಿಯ = ez->CONST_FALSE;

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_tmp = undef_a;

			ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < s.ಗಾತ್ರ_2(); i++)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> part_ದ_undef_b(undef_b.begin()+i*a.ಗಾತ್ರ_2(), undef_b.begin()+(i+1)*a.ಗಾತ್ರ_2());

				undef_tmp = ez->vec_ite(s.at(i), part_ದ_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), ಸಿಕ್ಕಿತು_ಸಕ್ರಿಯ));
				ಸಿಕ್ಕಿತು_ಸಕ್ರಿಯ = ez->OR(ಸಿಕ್ಕಿತು_ಸಕ್ರಿಯ, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>(a.ಗಾತ್ರ_2(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($pos), ID($buf), ID($neg)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		extend_Signal_ಅಗಲ_Unary(a, y, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		ಆದರೆ (cell->type.ಒಳ(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} ಇಲ್ಲದಿದ್ದರೆ {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> zero(a.ಗಾತ್ರ_2(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			extend_Signal_ಅಗಲ_Unary(undef_a, undef_y, cell);

			ಆದರೆ (cell->type.ಒಳ(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಪೂರ್ಣಾಂಕ undef_any_a = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_a);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y_ಬಿಟ್ಟುಗಳು(undef_y.ಗಾತ್ರ_2(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_ಬಿಟ್ಟುಗಳು, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		ಆದರೆ (cell->type == ID($reduce_and))
			ez->ಗಣ_2(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpAnd, a), yy.at(0));
		ಆದರೆ (cell->type.ಒಳ(ID($reduce_or), ID($reduce_bool)))
			ez->ಗಣ_2(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpOr, a), yy.at(0));
		ಆದರೆ (cell->type == ID($reduce_xor))
			ez->ಗಣ_2(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpXor, a), yy.at(0));
		ಆದರೆ (cell->type == ID($reduce_xnor))
			ez->ಗಣ_2(ez->NOT(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpXor, a)), yy.at(0));
		ಆದರೆ (cell->type == ID($logic_not))
			ez->ಗಣ_2(ez->NOT(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpOr, a)), yy.at(0));
		ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < y.ಗಾತ್ರ_2(); i++)
			ez->ಗಣ_2(ez->CONST_FALSE, yy.at(i));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			ಪೂರ್ಣಾಂಕ aX = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_a);

			ಆದರೆ (cell->type == ID($reduce_and)) {
				ಪೂರ್ಣಾಂಕ a0 = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				ಪೂರ್ಣಾಂಕ a1 = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} ಇಲ್ಲದಿದ್ದರೆ
				log_abort();

			ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < undef_y.ಗಾತ್ರ_2(); i++)
				ez->ಗಣ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($logic_and), ID($logic_or)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> vec_a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> vec_b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);

		ಪೂರ್ಣಾಂಕ a = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpOr, vec_a);
		ಪೂರ್ಣಾಂಕ b = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpOr, vec_b);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		ಆದರೆ (cell->type == ID($logic_and))
			ez->ಗಣ_2(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpAnd, a, b), yy.at(0));
		ಇಲ್ಲದಿದ್ದರೆ
			ez->ಗಣ_2(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ez->OpOr, a, b), yy.at(0));
		ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < y.ಗಾತ್ರ_2(); i++)
			ez->ಗಣ_2(ez->CONST_FALSE, yy.at(i));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

			ಪೂರ್ಣಾಂಕ a0 = ez->NOT(ez->OR(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, vec_a), ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_a)));
			ಪೂರ್ಣಾಂಕ b0 = ez->NOT(ez->OR(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, vec_b), ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_b)));
			ಪೂರ್ಣಾಂಕ a1 = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			ಪೂರ್ಣಾಂಕ b1 = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			ಪೂರ್ಣಾಂಕ aX = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_a);
			ಪೂರ್ಣಾಂಕ bX = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_b);

			ಆದರೆ (cell->type == ID($logic_and))
				ez->ಗಣ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($logic_or))
				ez->ಗಣ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			ಇಲ್ಲದಿದ್ದರೆ
				log_abort();

			ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < undef_y.ಗಾತ್ರ_2(); i++)
				ez->ಗಣ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		ತಾರ್ಕಿಕ is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		extend_Signal_ಅಗಲ(a, b, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		ಆದರೆ (model_undef && cell->type.ಒಳ(ID($eqx), ID($nex))) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			extend_Signal_ಅಗಲ(undef_a, undef_b, cell, ನಿಜ);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		ಆದರೆ (cell->type == ID($lt))
			ez->ಗಣ_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		ಆದರೆ (cell->type == ID($le))
			ez->ಗಣ_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		ಆದರೆ (cell->type.ಒಳ(ID($eq), ID($eqx)))
			ez->ಗಣ_2(ez->vec_eq(a, b), yy.at(0));
		ಆದರೆ (cell->type.ಒಳ(ID($ne), ID($nex)))
			ez->ಗಣ_2(ez->vec_ne(a, b), yy.at(0));
		ಆದರೆ (cell->type == ID($ge))
			ez->ಗಣ_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		ಆದರೆ (cell->type == ID($gt))
			ez->ಗಣ_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < y.ಗಾತ್ರ_2(); i++)
			ez->ಗಣ_2(ez->CONST_FALSE, yy.at(i));

		ಆದರೆ (model_undef && cell->type.ಒಳ(ID($eqx), ID($nex)))
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			extend_Signal_ಅಗಲ(undef_a, undef_b, cell, ನಿಜ);

			ಆದರೆ (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			ಇಲ್ಲದಿದ್ದರೆ
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < y.ಗಾತ್ರ_2(); i++)
				ez->ಗಣ_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (model_undef && cell->type.ಒಳ(ID($eq), ID($ne)))
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			extend_Signal_ಅಗಲ(undef_a, undef_b, cell, ನಿಜ);

			ಪೂರ್ಣಾಂಕ undef_any_a = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_a);
			ಪೂರ್ಣಾಂಕ undef_any_b = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_b);
			ಪೂರ್ಣಾಂಕ undef_any = ez->OR(undef_any_a, undef_any_b);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> masked_a_ಬಿಟ್ಟುಗಳು = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> masked_b_ಬಿಟ್ಟುಗಳು = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			ಪೂರ್ಣಾಂಕ masked_ne = ez->vec_ne(masked_a_ಬಿಟ್ಟುಗಳು, masked_b_ಬಿಟ್ಟುಗಳು);
			ಪೂರ್ಣಾಂಕ undef_y_ಬಿಟ್ = ez->AND(undef_any, ez->NOT(masked_ne));

			ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < undef_y.ಗಾತ್ರ_2(); i++)
				ez->ಗಣ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->ಗಣ_2(undef_y_ಬಿಟ್, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			ಆದರೆ (model_undef) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ಸರಿಸು), ID($shiftx)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		ಪೂರ್ಣಾಂಕ extend_ಬಿಟ್ = ez->CONST_FALSE;

		ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool())
			extend_ಬಿಟ್ = a.ಹಿಂಭಾಗ();

		ಆಗ (y.ಗಾತ್ರ_2() < a.ಗಾತ್ರ_2())
			y.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->literal());
		ಆಗ (y.ಗಾತ್ರ_2() > a.ಗಾತ್ರ_2())
			a.ತಳ್ಳು_ಹಿಂಭಾಗ(extend_ಬಿಟ್);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> shifted_a;

		ಆದರೆ (cell->type.ಒಳ( ID($shl), ID($sshl)))
			shifted_a = ez->vec_ಸರಿಸು_ಎಡ(a, b, ಸುಳ್ಳು, ez->CONST_FALSE, ez->CONST_FALSE);

		ಆದರೆ (cell->type == ID($shr))
			shifted_a = ez->vec_ಸರಿಸು_ಬಲ(a, b, ಸುಳ್ಳು, ez->CONST_FALSE, ez->CONST_FALSE);

		ಆದರೆ (cell->type == ID($sshr))
			shifted_a = ez->vec_ಸರಿಸು_ಬಲ(a, b, ಸುಳ್ಳು, cell->parameters[ID::A_SIGNED].as_bool() ? a.ಹಿಂಭಾಗ() : ez->CONST_FALSE, ez->CONST_FALSE);

		ಆದರೆ (cell->type.ಒಳ(ID($ಸರಿಸು), ID($shiftx)))
			shifted_a = ez->vec_ಸರಿಸು_ಬಲ(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a_shifted;

			extend_ಬಿಟ್ = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool())
				extend_ಬಿಟ್ = undef_a.ಹಿಂಭಾಗ();

			ಆಗ (undef_y.ಗಾತ್ರ_2() < undef_a.ಗಾತ್ರ_2())
				undef_y.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->literal());
			ಆಗ (undef_y.ಗಾತ್ರ_2() > undef_a.ಗಾತ್ರ_2())
				undef_a.ತಳ್ಳು_ಹಿಂಭಾಗ(extend_ಬಿಟ್);

			ಆದರೆ (cell->type.ಒಳ(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_ಸರಿಸು_ಎಡ(undef_a, b, ಸುಳ್ಳು, ez->CONST_FALSE, ez->CONST_FALSE);

			ಆದರೆ (cell->type == ID($shr))
				undef_a_shifted = ez->vec_ಸರಿಸು_ಬಲ(undef_a, b, ಸುಳ್ಳು, ez->CONST_FALSE, ez->CONST_FALSE);

			ಆದರೆ (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_ಸರಿಸು_ಬಲ(undef_a, b, ಸುಳ್ಳು, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.ಹಿಂಭಾಗ() : ez->CONST_FALSE, ez->CONST_FALSE);

			ಆದರೆ (cell->type == ID($ಸರಿಸು))
				undef_a_shifted = ez->vec_ಸರಿಸು_ಬಲ(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			ಆದರೆ (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_ಸರಿಸು_ಬಲ(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			ಪೂರ್ಣಾಂಕ undef_any_b = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_b);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_all_y_ಬಿಟ್ಟುಗಳು(undef_y.ಗಾತ್ರ_2(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_ಬಿಟ್ಟುಗಳು), undef_y));
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($mul))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		extend_Signal_ಅಗಲ(a, b, y, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> tmp(a.ಗಾತ್ರ_2(), ez->CONST_FALSE);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಪೂರ್ಣಾಂಕ(a.ಗಾತ್ರ_2()); i++)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> shifted_a(a.ಗಾತ್ರ_2(), ez->CONST_FALSE);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = i; j < ಪೂರ್ಣಾಂಕ(a.ಗಾತ್ರ_2()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_ಕೂಡಿಸು(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ಆದರೆ (model_undef) {
			log_assert(arith_undef_handled);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($macc), ID($macc_v2)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> tmp(Get_ಗಾತ್ರ(y), ez->CONST_FALSE);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : macc.terms)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಒಳ_a = importDefSigSpec(ಪೋರ್ಟ್.ಒಳ_a, timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಒಳ_b = importDefSigSpec(ಪೋರ್ಟ್.ಒಳ_b, timestep);

			ಆಗ (Get_ಗಾತ್ರ(ಒಳ_a) < Get_ಗಾತ್ರ(y))
				ಒಳ_a.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್.is_signed && !ಒಳ_a.ಖಾಲಿಗಳು() ? ಒಳ_a.ಹಿಂಭಾಗ() : ez->CONST_FALSE);
			ಒಳ_a.resize(Get_ಗಾತ್ರ(y));

			ಆದರೆ (Get_ಗಾತ್ರ(ಒಳ_b))
			{
				ಆಗ (Get_ಗಾತ್ರ(ಒಳ_b) < Get_ಗಾತ್ರ(y))
					ಒಳ_b.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್.is_signed && !ಒಳ_b.ಖಾಲಿಗಳು() ? ಒಳ_b.ಹಿಂಭಾಗ() : ez->CONST_FALSE);
				ಒಳ_b.resize(Get_ಗಾತ್ರ(y));

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(ಒಳ_b); i++) {
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> shifted_a(ಒಳ_a.ಗಾತ್ರ_2(), ez->CONST_FALSE);
					ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = i; j < ಪೂರ್ಣಾಂಕ(ಒಳ_a.ಗಾತ್ರ_2()); j++)
						shifted_a.at(j) = ಒಳ_a.at(j-i);
					ಆದರೆ (ಪೋರ್ಟ್.do_ಕಳೆ)
						tmp = ez->vec_ite(ಒಳ_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					ಇಲ್ಲದಿದ್ದರೆ
						tmp = ez->vec_ite(ಒಳ_b.at(i), ez->vec_ಕೂಡಿಸು(tmp, shifted_a), tmp);
				}
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
				ಆದರೆ (ಪೋರ್ಟ್.do_ಕಳೆ)
					tmp = ez->vec_sub(tmp, ಒಳ_a);
				ಇಲ್ಲದಿದ್ದರೆ
					tmp = ez->vec_ಕೂಡಿಸು(tmp, ಒಳ_a);
			}
		}

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_c;

			ಆದರೆ (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::C), timestep);

			ಪೂರ್ಣಾಂಕ undef_any_a = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_a);
			ಪೂರ್ಣಾಂಕ undef_any_b = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_b);
			ಪೂರ್ಣಾಂಕ undef_any_c = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_c);
			ಪೂರ್ಣಾಂಕ undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>(Get_ಗಾತ್ರ(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		ಇಲ್ಲದಿದ್ದರೆ
			ez->assume(ez->vec_eq(y, tmp));

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		extend_Signal_ಅಗಲ(a, b, y, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a_u, b_u;
		ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.ಹಿಂಭಾಗ(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.ಹಿಂಭಾಗ(), ez->vec_neg(b), b);
		} ಇಲ್ಲದಿದ್ದರೆ {
			a_u = a;
			b_u = b;
		}

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> chain_buf = a_u;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y_u(a_u.ಗಾತ್ರ_2(), ez->CONST_FALSE);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಪೂರ್ಣಾಂಕ(a.ಗಾತ್ರ_2())-1; i >= 0; i--)
		{
			chain_buf.ಸೇರಿಸು(chain_buf.ಕೊನೆ(), chain_buf.ಗಾತ್ರ_2(), ez->CONST_FALSE);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b_shl(i, ez->CONST_FALSE);
			b_shl.ಸೇರಿಸು(b_shl.ಕೊನೆ(), b_u.begin(), b_u.ಕೊನೆ());
			b_shl.ಸೇರಿಸು(b_shl.ಕೊನೆ(), chain_buf.ಗಾತ್ರ_2()-b_shl.ಗಾತ್ರ_2(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.ಗಾತ್ರ_2(), chain_buf.ಕೊನೆ());
		}

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.ಗಾತ್ರ_2());

		// modulo calculation
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> modulo_trunc;
		ಪೂರ್ಣಾಂಕ floored_eq_trunc;
		ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.ಹಿಂಭಾಗ(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.ಹಿಂಭಾಗ(), b.ಹಿಂಭಾಗ()), ez->NOT(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, modulo_trunc)));
		} ಇಲ್ಲದಿದ್ದರೆ {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		ಆದರೆ (cell->type == ID($div)) {
			ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.ಹಿಂಭಾಗ(), b.ಹಿಂಭಾಗ()), ez->vec_neg(y_u), y_u)));
			ಇಲ್ಲದಿದ್ದರೆ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($divfloor)) {
			ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.ಹಿಂಭಾಗ(), b.ಹಿಂಭಾಗ()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_ಕೂಡಿಸು(y_u, ez->vec_const_unsigned(1, y_u.ಗಾತ್ರ_2())),
						y_u
					)),
					y_u
				)));
			ಇಲ್ಲದಿದ್ದರೆ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_ಕೂಡಿಸು(modulo_trunc, b))));
		}

		ಆದರೆ (ignore_div_by_zero) {
			ez->assume(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, b));
		} ಇಲ್ಲದಿದ್ದರೆ {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> div_zero_ಫಲಿತಾಂಶ;
			ಆದರೆ (cell->type.ಒಳ(ID($div), ID($divfloor))) {
				ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> all_ones(y.ಗಾತ್ರ_2(), ez->CONST_TRUE);
					std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> only_ಮೊದಲ_one(y.ಗಾತ್ರ_2(), ez->CONST_FALSE);
					only_ಮೊದಲ_one.at(0) = ez->CONST_TRUE;
					div_zero_ಫಲಿತಾಂಶ = ez->vec_ite(a.ಹಿಂಭಾಗ(), only_ಮೊದಲ_one, all_ones);
				} ಇಲ್ಲದಿದ್ದರೆ {
					div_zero_ಫಲಿತಾಂಶ.ಸೇರಿಸು(div_zero_ಫಲಿತಾಂಶ.ಕೊನೆ(), cell->get_ಪೋರ್ಟ್(ID::A).ಗಾತ್ರ_2(), ez->CONST_TRUE);
					div_zero_ಫಲಿತಾಂಶ.ಸೇರಿಸು(div_zero_ಫಲಿತಾಂಶ.ಕೊನೆ(), y.ಗಾತ್ರ_2() - div_zero_ಫಲಿತಾಂಶ.ಗಾತ್ರ_2(), ez->CONST_FALSE);
				}
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				ಪೂರ್ಣಾಂಕ copy_a_ಬಿಟ್ಟುಗಳು = min(cell->get_ಪೋರ್ಟ್(ID::A).ಗಾತ್ರ_2(), cell->get_ಪೋರ್ಟ್(ID::B).ಗಾತ್ರ_2());
				div_zero_ಫಲಿತಾಂಶ.ಸೇರಿಸು(div_zero_ಫಲಿತಾಂಶ.ಕೊನೆ(), a.begin(), a.begin() + copy_a_ಬಿಟ್ಟುಗಳು);
				ಆದರೆ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_ಫಲಿತಾಂಶ.ಸೇರಿಸು(div_zero_ಫಲಿತಾಂಶ.ಕೊನೆ(), y.ಗಾತ್ರ_2() - div_zero_ಫಲಿತಾಂಶ.ಗಾತ್ರ_2(), div_zero_ಫಲಿತಾಂಶ.ಹಿಂಭಾಗ());
				ಇಲ್ಲದಿದ್ದರೆ
					div_zero_ಫಲಿತಾಂಶ.ಸೇರಿಸು(div_zero_ಫಲಿತಾಂಶ.ಕೊನೆ(), y.ಗಾತ್ರ_2() - div_zero_ಫಲಿತಾಂಶ.ಗಾತ್ರ_2(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, b), y_tmp, div_zero_ಫಲಿತಾಂಶ)));
		}

		ಆದರೆ (model_undef) {
			log_assert(arith_undef_handled);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($lut))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> lut;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : cell->getParam(ID::LUT))
			lut.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್ == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		ಆಗ (Get_ಗಾತ್ರ(lut) < (1 << Get_ಗಾತ್ರ(a)))
			lut.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->CONST_FALSE);
		lut.resize(1 << Get_ಗಾತ್ರ(a));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t(lut), u(Get_ಗಾತ್ರ(t), ez->CONST_FALSE);

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(a)-1; i >= 0; i--)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t0(t.begin(), t.begin() + Get_ಗಾತ್ರ(t)/2);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t1(t.begin() + Get_ಗಾತ್ರ(t)/2, t.ಕೊನೆ());

				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> u0(u.begin(), u.begin() + Get_ಗಾತ್ರ(u)/2);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> u1(u.begin() + Get_ಗಾತ್ರ(u)/2, u.ಕೊನೆ());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_ಗಾತ್ರ(t) == 1);
			log_assert(Get_ಗಾತ್ರ(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep), u));
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t = lut;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(a)-1; i >= 0; i--)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t0(t.begin(), t.begin() + Get_ಗಾತ್ರ(t)/2);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t1(t.begin() + Get_ಗಾತ್ರ(t)/2, t.ಕೊನೆ());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_ಗಾತ್ರ(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($sop))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		ಪೂರ್ಣಾಂಕ y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep).at(0);

		ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->getParam(ID::ಅಗಲ_2).as_int();
		ಪೂರ್ಣಾಂಕ ಆಳ = cell->getParam(ID::ಆಳ_2).as_int();

		ಸದಿಶ<State> ಕೋಷ್ಟಕ_raw = cell->getParam(ID::ಕೋಷ್ಟಕ).ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು();
		ಆಗ (Get_ಗಾತ್ರ(ಕೋಷ್ಟಕ_raw) < 2*ಅಗಲ*ಆಳ)
			ಕೋಷ್ಟಕ_raw.ತಳ್ಳು_ಹಿಂಭಾಗ(State::S0);

		ಸದಿಶ<ಸದಿಶ<ಪೂರ್ಣಾಂಕ>> ಕೋಷ್ಟಕ_2(ಆಳ);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಆಳ; i++)
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ; j++)
		{
			ತಾರ್ಕಿಕ pat0 = (ಕೋಷ್ಟಕ_raw[2*ಅಗಲ*i + 2*j + 0] == State::S1);
			ತಾರ್ಕಿಕ pat1 = (ಕೋಷ್ಟಕ_raw[2*ಅಗಲ*i + 2*j + 1] == State::S1);

			ಆದರೆ (pat0 && !pat1)
				ಕೋಷ್ಟಕ_2.at(i).ತಳ್ಳು_ಹಿಂಭಾಗ(0);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!pat0 && pat1)
				ಕೋಷ್ಟಕ_2.at(i).ತಳ್ಳು_ಹಿಂಭಾಗ(1);
			ಇಲ್ಲದಿದ್ದರೆ
				ಕೋಷ್ಟಕ_2.at(i).ತಳ್ಳು_ಹಿಂಭಾಗ(-1);
		}

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> products, undef_products;
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			ಪೂರ್ಣಾಂಕ undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep).at(0);

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಆಳ; i++)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> cmp_a, cmp_ua, cmp_b;

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ; j++)
					ಆದರೆ (ಕೋಷ್ಟಕ_2.at(i).at(j) >= 0) {
						cmp_a.ತಳ್ಳು_ಹಿಂಭಾಗ(a.at(j));
						cmp_ua.ತಳ್ಳು_ಹಿಂಭಾಗ(undef_a.at(j));
						cmp_b.ತಳ್ಳು_ಹಿಂಭಾಗ(ಕೋಷ್ಟಕ_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> masked_b = ez->vec_or(cmp_b, cmp_ua);

				ಪೂರ್ಣಾಂಕ masked_eq = ez->vec_eq(masked_a, masked_b);
				ಪೂರ್ಣಾಂಕ any_undef = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, cmp_ua);

				undef_products.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->AND(any_undef, masked_eq));
				products.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			ಪೂರ್ಣಾಂಕ yy = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, products);
			ez->ಗಣ_2(undef_y, ez->AND(ez->NOT(yy), ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> products;

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಆಳ; i++)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> cmp_a, cmp_b;

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ; j++)
					ಆದರೆ (ಕೋಷ್ಟಕ_2.at(i).at(j) >= 0) {
						cmp_a.ತಳ್ಳು_ಹಿಂಭಾಗ(a.at(j));
						cmp_b.ತಳ್ಳು_ಹಿಂಭಾಗ(ಕೋಷ್ಟಕ_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->ಗಣ_2(y, ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, products));
		}

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($fa))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> c = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::C), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> x = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::X), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> xx = model_undef ? ez->vec_var(x.ಗಾತ್ರ_2()) : x;

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t2 = ez->vec_and(a, b);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_c = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::C), timestep);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_x = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($lcu))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> p = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::P), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> g = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::G), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ci = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CI), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> co = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CO), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(co.ಗಾತ್ರ_2()) : co;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(co); i++)
			ez->ಗಣ_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_p = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::P), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_g = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::G), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_ci = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CI), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_co = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CO), timestep);

			ಪೂರ್ಣಾಂಕ undef_any_p = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_p);
			ಪೂರ್ಣಾಂಕ undef_any_g = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_g);
			ಪೂರ್ಣಾಂಕ undef_any_ci = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, undef_ci);
			ಪೂರ್ಣಾಂಕ undef_co_ಬಿಟ್ = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_co_ಬಿಟ್ಟುಗಳು(undef_co.ಗಾತ್ರ_2(), undef_co_ಬಿಟ್);
			ez->assume(ez->vec_eq(undef_co_ಬಿಟ್ಟುಗಳು, undef_co));

			undefGating(co, yy, undef_co);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($alu))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> b = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> x = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::X), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ci = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CI), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> bi = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::BI), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> co = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CO), timestep);

		extend_Signal_ಅಗಲ(a, b, y, cell);
		extend_Signal_ಅಗಲ(a, b, x, cell);
		extend_Signal_ಅಗಲ(a, b, co, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> def_y = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> def_x = model_undef ? ez->vec_var(x.ಗಾತ್ರ_2()) : x;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> def_co = model_undef ? ez->vec_var(co.ಗಾತ್ರ_2()) : co;

		log_assert(Get_ಗಾತ್ರ(y) == Get_ಗಾತ್ರ(x));
		log_assert(Get_ಗಾತ್ರ(y) == Get_ಗಾತ್ರ(co));
		log_assert(Get_ಗಾತ್ರ(ci) == 1);
		log_assert(Get_ಗಾತ್ರ(bi) == 1);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(y); i++)
		{
			ಪೂರ್ಣಾಂಕ s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->ಗಣ_2(def_x.at(i), ez->XOR(s1, s2));
			ez->ಗಣ_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->ಗಣ_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_b = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::B), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_ci = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CI), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_bi = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::BI), timestep);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_x = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::X), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_co = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::CO), timestep);

			extend_Signal_ಅಗಲ(undef_a, undef_b, undef_y, cell);
			extend_Signal_ಅಗಲ(undef_a, undef_b, undef_x, cell);
			extend_Signal_ಅಗಲ(undef_a, undef_b, undef_co, cell);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> all_inputs_undef;
			all_inputs_undef.ಸೇರಿಸು(all_inputs_undef.ಕೊನೆ(), undef_a.begin(), undef_a.ಕೊನೆ());
			all_inputs_undef.ಸೇರಿಸು(all_inputs_undef.ಕೊನೆ(), undef_b.begin(), undef_b.ಕೊನೆ());
			all_inputs_undef.ಸೇರಿಸು(all_inputs_undef.ಕೊನೆ(), undef_ci.begin(), undef_ci.ಕೊನೆ());
			all_inputs_undef.ಸೇರಿಸು(all_inputs_undef.ಕೊನೆ(), undef_bi.begin(), undef_bi.ಕೊನೆ());
			ಪೂರ್ಣಾಂಕ undef_any = ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpOr, all_inputs_undef);

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(undef_y); i++) {
				ez->ಗಣ_2(undef_y.at(i), undef_any);
				ez->ಗಣ_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->ಗಣ_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($ತುಂಡು))
	{
		RTLIL::SigSpec a = cell->get_ಪೋರ್ಟ್(ID::A);
		RTLIL::SigSpec y = cell->get_ಪೋರ್ಟ್(ID::Y);
		ez->assume(ಸಂಕೇತಗಳು_eq(a.ಹೊರತೆಗೆ(cell->parameters.at(ID::ಸ್ಥಾನಾಂತರ_2).as_int(), y.ಗಾತ್ರ_2()), y, timestep));
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_ಪೋರ್ಟ್(ID::A);
		RTLIL::SigSpec b = cell->get_ಪೋರ್ಟ್(ID::B);
		RTLIL::SigSpec y = cell->get_ಪೋರ್ಟ್(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(ಸಂಕೇತಗಳು_eq(ab, y, timestep));
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_ದತ್ತಾಂಶ ff(ಶೂನ್ಯಸೂಚಕ, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		ಆದರೆ (ff.has_aload || ff.has_arst || ff.has_sr)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		ಆದರೆ (timestep == 1)
		{
			initial_state.ಕೂಡಿಸು((*sigmap)(cell->get_ಪೋರ್ಟ್(ID::Q)));
			ಆದರೆ (model_undef && def_formal) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_q = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> d = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::D), timestep-1);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_d;
			ಆದರೆ (model_undef)
				undef_d = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::D), timestep-1);
			ಆದರೆ (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				ಪೂರ್ಣಾಂಕ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> rval = importDefSigSpec(ff.val_srst, timestep-1);
				ಪೂರ್ಣಾಂಕ undef_srst = -1;
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_rval;
				ಆದರೆ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ಆದರೆ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				ಇಲ್ಲದಿದ್ದರೆ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			ಆದರೆ (ff.has_ce) {
				ಪೂರ್ಣಾಂಕ ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				ಪೂರ್ಣಾಂಕ undef_ce = -1;
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_old_q;
				ಆದರೆ (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				ಆದರೆ (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				ಇಲ್ಲದಿದ್ದರೆ
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			ಆದರೆ (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				ಪೂರ್ಣಾಂಕ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> rval = importDefSigSpec(ff.val_srst, timestep-1);
				ಪೂರ್ಣಾಂಕ undef_srst = -1;
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_rval;
				ಆದರೆ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ಆದರೆ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				ಇಲ್ಲದಿದ್ದರೆ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> q = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Q), timestep);

			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> qq = model_undef ? ez->vec_var(q.ಗಾತ್ರ_2()) : q;
			ez->assume(ez->vec_eq(d, qq));

			ಆದರೆ (model_undef)
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_q = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($anyconst))
	{
		ಆದರೆ (timestep < 2) {
			ಆದರೆ (model_undef && def_formal) {
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			ಹಿಂತಿರುಗಿ ನಿಜ;
		}

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> d = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep-1);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> q = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> qq = (model_undef && !def_formal) ? ez->vec_var(q.ಗಾತ್ರ_2()) : q;
		ez->assume(ez->vec_eq(d, qq));

		ಆದರೆ (model_undef)
		{
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_d = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep-1);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_q = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);

			ಆದರೆ (def_formal) {
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &undef_q_ಬಿಟ್ : undef_q)
					ez->ಗಣ_2(ez->CONST_FALSE, undef_q_ಬಿಟ್);
			} ಇಲ್ಲದಿದ್ದರೆ {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($anyseq))
	{
		ಆದರೆ (model_undef && def_formal) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_q = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &undef_q_ಬಿಟ್ : undef_q)
				ez->ಗಣ_2(ez->CONST_FALSE, undef_q_ಬಿಟ್);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($barrier) && !model_barriers)
		ಹಿಂತಿರುಗಿ ನಿಜ;

	ಆದರೆ (cell->type.ಒಳ(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> a = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		extend_Signal_ಅಗಲ_Unary(a, y, cell);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> yy = model_undef ? ez->vec_var(y.ಗಾತ್ರ_2()) : y;
		ez->assume(ez->vec_eq(a, yy));

		ಆದರೆ (model_undef) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_a = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::A), timestep);
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			extend_Signal_ಅಗಲ_Unary(undef_a, undef_y, cell, ಸುಳ್ಳು);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($initstate))
	{
		ಸ್ವಯಂಚಾಲಿತ ಕೀಲಿ = make_pair(ಪೂರ್ವಪ್ರತ್ಯಯ_2, timestep);
		ಆದರೆ (initstates.ಎಣಿಕೆ_2(ಕೀಲಿ) == 0)
			initstates[ಕೀಲಿ] = ಸುಳ್ಳು;

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> y = importDefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
		log_assert(Get_ಗಾತ್ರ(y) == 1);
		ez->ಗಣ_2(y[0], initstates[ಕೀಲಿ] ? ez->CONST_TRUE : ez->CONST_FALSE);

		ಆದರೆ (model_undef) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_y = importUndefSigSpec(cell->get_ಪೋರ್ಟ್(ID::Y), timestep);
			log_assert(Get_ಗಾತ್ರ(undef_y) == 1);
			ez->ಗಣ_2(undef_y[0], ez->CONST_FALSE);
		}

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($ಪ್ರತಿಜ್ಞಾ))
	{
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_ಪೋರ್ಟ್(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_ಪೋರ್ಟ್(ID::EN)));
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($assume))
	{
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_ಪೋರ್ಟ್(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_ಪೋರ್ಟ್(ID::EN)));
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($scopeinfo) || cell->type == ID($ಒಳಹರಿವು_ಪೋರ್ಟ್))
	{
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}

ನಾಮಾಕಾಶ Yosys {

ಶೂನ್ಯ report_missing_model(ತಾರ್ಕಿಕ warn_only, RTLIL::Cell* cell)
{
	std::string s;
	ಆದರೆ (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	ಇಲ್ಲದಿದ್ದರೆ
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	ಆದರೆ (warn_only) {
		log_warning_noprefix("%s\n", s);
	} ಇಲ್ಲದಿದ್ದರೆ {
		log_error("%s\n", s);
	}
}

}
