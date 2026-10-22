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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/satgen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/ff.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

ਤਾਰਕਿਕ SatGen::importCell(RTLIL::Cell *cell, ਪੂਰਨ timestep)
{
	ਤਾਰਕਿਕ arith_undef_handled = ਝੂਠ;
	ਤਾਰਕਿਕ is_arith_compare = cell->type.ਵਿੱਚ(ID($lt), ID($le), ID($ge), ID($gt));

	ਜੇ (model_undef && (cell->type.ਵਿੱਚ(ID($ਜੋੜੋ), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		ਜੇ (is_arith_compare)
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, cell, ਸੱਚ);
		ਨਹੀਂਤਾਂ
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, undef_y, cell, ਸੱਚ);

		ਪੂਰਨ undef_any_a = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_a);
		ਪੂਰਨ undef_any_b = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_b);
		ਪੂਰਨ undef_y_ਦੋਆਧਾਰੀ_ਅੰਕ = ez->OR(undef_any_a, undef_any_b);

		ਜੇ (cell->type.ਵਿੱਚ(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> b = importSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			undef_y_ਦੋਆਧਾਰੀ_ਅੰਕ = ez->OR(undef_y_ਦੋਆਧਾਰੀ_ਅੰਕ, ez->NOT(ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, b)));
		}

		ਜੇ (is_arith_compare) {
			ਲਈ (ਮਾਪ_t i = 1; i < undef_y.ਮਾਪ(); i++)
				ez->ਸੈੱਟ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->ਸੈੱਟ_2(undef_y_ਦੋਆਧਾਰੀ_ਅੰਕ, undef_y.at(0));
		} ਨਹੀਂਤਾਂ {
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y_ਬਿੱਟ(undef_y.ਮਾਪ(), undef_y_ਦੋਆਧਾਰੀ_ਅੰਕ);
			ez->assume(ez->vec_eq(undef_y_ਬਿੱਟ, undef_y));
		}

		arith_undef_handled = ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($ਜੋੜੋ), ID($sub)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		extend_Signal_ਚੌੜਾਈ(a, b, y, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		ਜੇ (cell->type.ਵਿੱਚ(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		ਜੇ (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		ਜੇ (cell->type.ਵਿੱਚ(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		ਜੇ (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		ਜੇ (cell->type.ਵਿੱਚ(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		ਜੇ (cell->type.ਵਿੱਚ(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		ਜੇ (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		ਜੇ (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		ਜੇ (cell->type == ID($ਜੋੜੋ))
			ez->assume(ez->vec_eq(ez->vec_ਜੋੜੋ(a, b), yy));
		ਜੇ (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		ਜੇ (model_undef && !arith_undef_handled)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, undef_y, cell, ਝੂਠ);

			ਜੇ (cell->type.ਵਿੱਚ(ID($and), ID($_AND_), ID($_NAND_))) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ਸਦਿਸ਼<ਪੂਰਨ> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ਸਦਿਸ਼<ਪੂਰਨ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($or), ID($_OR_), ID($_NOR_))) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ਸਦਿਸ਼<ਪੂਰਨ> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ਸਦਿਸ਼<ਪੂਰਨ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($_ANDNOT_)) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ਸਦਿਸ਼<ਪੂਰਨ> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ਸਦਿਸ਼<ਪੂਰਨ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($_ORNOT_)) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ਸਦਿਸ਼<ਪੂਰਨ> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ਸਦਿਸ਼<ਪੂਰਨ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ਨਹੀਂਤਾਂ
				log_abort();

			undefGating(y, yy, undef_y);
		}
		ਨਹੀਂਤਾਂ ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		ਤਾਰਕਿਕ aoi_ਬਹੁਲਕ = cell->type.ਵਿੱਚ(ID($_AOI3_), ID($_AOI4_));
		ਤਾਰਕਿਕ three_ਬਹੁਲਕ = cell->type.ਵਿੱਚ(ID($_AOI3_), ID($_OAI3_));

		ਪੂਰਨ a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep).at(0);
		ਪੂਰਨ b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep).at(0);
		ਪੂਰਨ c = importDefSigSpec(cell->get_ਪੋਰਟ(ID::C), timestep).at(0);
		ਪੂਰਨ d = three_ਬਹੁਲਕ ? (aoi_ਬਹੁਲਕ ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_ਪੋਰਟ(ID::D), timestep).at(0);
		ਪੂਰਨ y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep).at(0);
		ਪੂਰਨ yy = model_undef ? ez->literal() : y;

		ਜੇ (cell->type.ਵਿੱਚ(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		ਨਹੀਂਤਾਂ
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		ਜੇ (model_undef)
		{
			ਪੂਰਨ undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep).at(0);
			ਪੂਰਨ undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep).at(0);
			ਪੂਰਨ undef_c = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::C), timestep).at(0);
			ਪੂਰਨ undef_d = three_ਬਹੁਲਕ ? ez->CONST_FALSE : importUndefSigSpec(cell->get_ਪੋਰਟ(ID::D), timestep).at(0);
			ਪੂਰਨ undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep).at(0);

			ਜੇ (aoi_ਬਹੁਲਕ)
			{
				ਪੂਰਨ a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				ਪੂਰਨ b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				ਪੂਰਨ c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				ਪੂਰਨ d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				ਪੂਰਨ ab = ez->AND(a, b), cd = ez->AND(c, d);
				ਪੂਰਨ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				ਪੂਰਨ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				ਪੂਰਨ ab1 = ez->AND(ab, ez->NOT(undef_ab));
				ਪੂਰਨ cd1 = ez->AND(cd, ez->NOT(undef_cd));
				ਪੂਰਨ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			ਨਹੀਂਤਾਂ
			{
				ਪੂਰਨ a1 = ez->AND(a, ez->NOT(undef_a));
				ਪੂਰਨ b1 = ez->AND(b, ez->NOT(undef_b));
				ਪੂਰਨ c1 = ez->AND(c, ez->NOT(undef_c));
				ਪੂਰਨ d1 = ez->AND(d, ez->NOT(undef_d));

				ਪੂਰਨ ab = ez->OR(a, b), cd = ez->OR(c, d);
				ਪੂਰਨ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				ਪੂਰਨ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				ਪੂਰਨ ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				ਪੂਰਨ cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				ਪੂਰਨ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($_NOT_), ID($not)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		extend_Signal_ਚੌੜਾਈ_Unary(a, y, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		ਜੇ (model_undef) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			extend_Signal_ਚੌੜਾਈ_Unary(undef_a, undef_y, cell, ਝੂਠ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($bweqx))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> bweqx = ez->vec_not(ez->vec_xor(a, b));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

			std::ਸਦਿਸ਼<ਪੂਰਨ> both_undef = ez->vec_and(undef_a, undef_b);
			std::ਸਦਿਸ਼<ਪੂਰਨ> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			ਲਈ (ਪੂਰਨ yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> s = importDefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;
		ਜੇ (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		ਨਹੀਂਤਾਂ
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_s = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

			std::ਸਦਿਸ਼<ਪੂਰਨ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::ਸਦਿਸ਼<ਪੂਰਨ> yX;
			ਜੇ (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			ਨਹੀਂਤਾਂ
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($bmux))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> s = importDefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a, undef_s, undef_y;

		ਜੇ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		}

		ਜੇ (Get_ਮਾਪ(s) == 0) {
			ez->vec_ਸੈੱਟ(a, y);
			ਜੇ (model_undef)
				ez->vec_ਸੈੱਟ(undef_a, undef_y);
		} ਨਹੀਂਤਾਂ {
			ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(s)-1; i >= 0; i--)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> ਬਾਹਰਲਾ = (i == 0) ? y : ez->vec_var(a.ਮਾਪ() / 2);
				std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(ਬਾਹਰਲਾ.ਮਾਪ()) : ਬਾਹਰਲਾ;

				std::ਸਦਿਸ਼<ਪੂਰਨ> a0(a.begin(), a.begin() + a.ਮਾਪ() / 2);
				std::ਸਦਿਸ਼<ਪੂਰਨ> a1(a.begin() + a.ਮਾਪ() / 2, a.ਸਿਰਾ());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				ਜੇ (model_undef)
				{
					std::ਸਦਿਸ਼<ਪੂਰਨ> undef_ਬਾਹਰਲਾ = (i == 0) ? undef_y : ez->vec_var(a.ਮਾਪ() / 2);
					std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a0(undef_a.begin(), undef_a.begin() + a.ਮਾਪ() / 2);
					std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a1(undef_a.begin() + a.ਮਾਪ() / 2, undef_a.ਸਿਰਾ());
					std::ਸਦਿਸ਼<ਪੂਰਨ> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::ਸਦਿਸ਼<ਪੂਰਨ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::ਸਦਿਸ਼<ਪੂਰਨ> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_ਬਾਹਰਲਾ));
					undefGating(ਬਾਹਰਲਾ, yy, undef_ਬਾਹਰਲਾ);

					undef_a = undef_ਬਾਹਰਲਾ;
				}

				a = ਬਾਹਰਲਾ;
			}
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($demux))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> s = importDefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;
		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a, undef_s, undef_y;

		ਜੇ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		}

		ਜੇ (Get_ਮਾਪ(s) == 0) {
			ez->vec_ਸੈੱਟ(a, y);
			ਜੇ (model_undef)
				ez->vec_ਸੈੱਟ(undef_a, undef_y);
		} ਨਹੀਂਤਾਂ {
			ਲਈ (ਪੂਰਨ i = 0; i < (1 << Get_ਮਾਪ(s)); i++)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> ss;
				ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(s); j++) {
					ਜੇ (i & 1 << j)
						ss.ਧੱਕੋ_ਪਿੱਠ(s[j]);
					ਨਹੀਂਤਾਂ
						ss.ਧੱਕੋ_ਪਿੱਠ(ez->NOT(s[j]));
				}
				ਪੂਰਨ sss = ez->ਵਿਅੰਜਕ_2(ezSAT::OpAnd, ss);

				ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(a); j++) {
					ez->ਸੈੱਟ_2(ez->AND(sss, a[j]), yy.at(i * Get_ਮਾਪ(a) + j));
				}

				ਜੇ (model_undef)
				{
					ਪੂਰਨ s0 = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					ਪੂਰਨ us = ez->AND(ez->NOT(s0), ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_s));
					ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(a); j++) {
						ਪੂਰਨ a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						ਪੂਰਨ yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->ਸੈੱਟ_2(yX, undef_y.at(i * Get_ਮਾਪ(a) + j));
					}
				}
			}
			ਜੇ (model_undef)
				undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($pmux))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> s = importDefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		std::ਸਦਿਸ਼<ਪੂਰਨ> tmp = a;
		ਲਈ (ਮਾਪ_t i = 0; i < s.ਮਾਪ(); i++) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> part_ਦਾ_b(b.begin()+i*a.ਮਾਪ(), b.begin()+(i+1)*a.ਮਾਪ());
			tmp = ez->vec_ite(s.at(i), part_ਦਾ_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_s = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::S), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

			ਪੂਰਨ all_undef = ez->CONST_FALSE;
			ਪੂਰਨ ਮਿਲਿਆ_ਸਰਗਰਮ = ez->CONST_FALSE;

			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_tmp = undef_a;

			ਲਈ (ਮਾਪ_t i = 0; i < s.ਮਾਪ(); i++)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> part_ਦਾ_undef_b(undef_b.begin()+i*a.ਮਾਪ(), undef_b.begin()+(i+1)*a.ਮਾਪ());

				undef_tmp = ez->vec_ite(s.at(i), part_ਦਾ_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), ਮਿਲਿਆ_ਸਰਗਰਮ));
				ਮਿਲਿਆ_ਸਰਗਰਮ = ez->OR(ਮਿਲਿਆ_ਸਰਗਰਮ, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::ਸਦਿਸ਼<ਪੂਰਨ>(a.ਮਾਪ(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($pos), ID($buf), ID($neg)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		extend_Signal_ਚੌੜਾਈ_Unary(a, y, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		ਜੇ (cell->type.ਵਿੱਚ(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} ਨਹੀਂਤਾਂ {
			std::ਸਦਿਸ਼<ਪੂਰਨ> zero(a.ਮਾਪ(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			extend_Signal_ਚੌੜਾਈ_Unary(undef_a, undef_y, cell);

			ਜੇ (cell->type.ਵਿੱਚ(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} ਨਹੀਂਤਾਂ {
				ਪੂਰਨ undef_any_a = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_a);
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y_ਬਿੱਟ(undef_y.ਮਾਪ(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_ਬਿੱਟ, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		ਜੇ (cell->type == ID($reduce_and))
			ez->ਸੈੱਟ_2(ez->ਵਿਅੰਜਕ_2(ez->OpAnd, a), yy.at(0));
		ਜੇ (cell->type.ਵਿੱਚ(ID($reduce_or), ID($reduce_bool)))
			ez->ਸੈੱਟ_2(ez->ਵਿਅੰਜਕ_2(ez->OpOr, a), yy.at(0));
		ਜੇ (cell->type == ID($reduce_xor))
			ez->ਸੈੱਟ_2(ez->ਵਿਅੰਜਕ_2(ez->OpXor, a), yy.at(0));
		ਜੇ (cell->type == ID($reduce_xnor))
			ez->ਸੈੱਟ_2(ez->NOT(ez->ਵਿਅੰਜਕ_2(ez->OpXor, a)), yy.at(0));
		ਜੇ (cell->type == ID($logic_not))
			ez->ਸੈੱਟ_2(ez->NOT(ez->ਵਿਅੰਜਕ_2(ez->OpOr, a)), yy.at(0));
		ਲਈ (ਮਾਪ_t i = 1; i < y.ਮਾਪ(); i++)
			ez->ਸੈੱਟ_2(ez->CONST_FALSE, yy.at(i));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			ਪੂਰਨ aX = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_a);

			ਜੇ (cell->type == ID($reduce_and)) {
				ਪੂਰਨ a0 = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				ਪੂਰਨ a1 = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} ਨਹੀਂਤਾਂ
				log_abort();

			ਲਈ (ਮਾਪ_t i = 1; i < undef_y.ਮਾਪ(); i++)
				ez->ਸੈੱਟ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($logic_and), ID($logic_or)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> vec_a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> vec_b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);

		ਪੂਰਨ a = ez->ਵਿਅੰਜਕ_2(ez->OpOr, vec_a);
		ਪੂਰਨ b = ez->ਵਿਅੰਜਕ_2(ez->OpOr, vec_b);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		ਜੇ (cell->type == ID($logic_and))
			ez->ਸੈੱਟ_2(ez->ਵਿਅੰਜਕ_2(ez->OpAnd, a, b), yy.at(0));
		ਨਹੀਂਤਾਂ
			ez->ਸੈੱਟ_2(ez->ਵਿਅੰਜਕ_2(ez->OpOr, a, b), yy.at(0));
		ਲਈ (ਮਾਪ_t i = 1; i < y.ਮਾਪ(); i++)
			ez->ਸੈੱਟ_2(ez->CONST_FALSE, yy.at(i));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

			ਪੂਰਨ a0 = ez->NOT(ez->OR(ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, vec_a), ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_a)));
			ਪੂਰਨ b0 = ez->NOT(ez->OR(ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, vec_b), ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_b)));
			ਪੂਰਨ a1 = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			ਪੂਰਨ b1 = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			ਪੂਰਨ aX = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_a);
			ਪੂਰਨ bX = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_b);

			ਜੇ (cell->type == ID($logic_and))
				ez->ਸੈੱਟ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($logic_or))
				ez->ਸੈੱਟ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			ਨਹੀਂਤਾਂ
				log_abort();

			ਲਈ (ਮਾਪ_t i = 1; i < undef_y.ਮਾਪ(); i++)
				ez->ਸੈੱਟ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		ਤਾਰਕਿਕ is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		extend_Signal_ਚੌੜਾਈ(a, b, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		ਜੇ (model_undef && cell->type.ਵਿੱਚ(ID($eqx), ID($nex))) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, cell, ਸੱਚ);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		ਜੇ (cell->type == ID($lt))
			ez->ਸੈੱਟ_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		ਜੇ (cell->type == ID($le))
			ez->ਸੈੱਟ_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		ਜੇ (cell->type.ਵਿੱਚ(ID($eq), ID($eqx)))
			ez->ਸੈੱਟ_2(ez->vec_eq(a, b), yy.at(0));
		ਜੇ (cell->type.ਵਿੱਚ(ID($ne), ID($nex)))
			ez->ਸੈੱਟ_2(ez->vec_ne(a, b), yy.at(0));
		ਜੇ (cell->type == ID($ge))
			ez->ਸੈੱਟ_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		ਜੇ (cell->type == ID($gt))
			ez->ਸੈੱਟ_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		ਲਈ (ਮਾਪ_t i = 1; i < y.ਮਾਪ(); i++)
			ez->ਸੈੱਟ_2(ez->CONST_FALSE, yy.at(i));

		ਜੇ (model_undef && cell->type.ਵਿੱਚ(ID($eqx), ID($nex)))
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, cell, ਸੱਚ);

			ਜੇ (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			ਨਹੀਂਤਾਂ
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			ਲਈ (ਮਾਪ_t i = 0; i < y.ਮਾਪ(); i++)
				ez->ਸੈੱਟ_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		ਨਹੀਂਤਾਂ ਜੇ (model_undef && cell->type.ਵਿੱਚ(ID($eq), ID($ne)))
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, cell, ਸੱਚ);

			ਪੂਰਨ undef_any_a = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_a);
			ਪੂਰਨ undef_any_b = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_b);
			ਪੂਰਨ undef_any = ez->OR(undef_any_a, undef_any_b);

			std::ਸਦਿਸ਼<ਪੂਰਨ> masked_a_ਬਿੱਟ = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::ਸਦਿਸ਼<ਪੂਰਨ> masked_b_ਬਿੱਟ = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			ਪੂਰਨ masked_ne = ez->vec_ne(masked_a_ਬਿੱਟ, masked_b_ਬਿੱਟ);
			ਪੂਰਨ undef_y_ਦੋਆਧਾਰੀ_ਅੰਕ = ez->AND(undef_any, ez->NOT(masked_ne));

			ਲਈ (ਮਾਪ_t i = 1; i < undef_y.ਮਾਪ(); i++)
				ez->ਸੈੱਟ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->ਸੈੱਟ_2(undef_y_ਦੋਆਧਾਰੀ_ਅੰਕ, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		ਨਹੀਂਤਾਂ
		{
			ਜੇ (model_undef) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ਖਿਸਕਾਓ), ID($shiftx)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		ਪੂਰਨ extend_ਦੋਆਧਾਰੀ_ਅੰਕ = ez->CONST_FALSE;

		ਜੇ (cell->parameters[ID::A_SIGNED].as_bool())
			extend_ਦੋਆਧਾਰੀ_ਅੰਕ = a.ਪਿੱਠ();

		ਜਦੋਂ (y.ਮਾਪ() < a.ਮਾਪ())
			y.ਧੱਕੋ_ਪਿੱਠ(ez->literal());
		ਜਦੋਂ (y.ਮਾਪ() > a.ਮਾਪ())
			a.ਧੱਕੋ_ਪਿੱਠ(extend_ਦੋਆਧਾਰੀ_ਅੰਕ);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;
		std::ਸਦਿਸ਼<ਪੂਰਨ> shifted_a;

		ਜੇ (cell->type.ਵਿੱਚ( ID($shl), ID($sshl)))
			shifted_a = ez->vec_ਖਿਸਕਾਓ_ਖੱਬਾ(a, b, ਝੂਠ, ez->CONST_FALSE, ez->CONST_FALSE);

		ਜੇ (cell->type == ID($shr))
			shifted_a = ez->vec_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b, ਝੂਠ, ez->CONST_FALSE, ez->CONST_FALSE);

		ਜੇ (cell->type == ID($sshr))
			shifted_a = ez->vec_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b, ਝੂਠ, cell->parameters[ID::A_SIGNED].as_bool() ? a.ਪਿੱਠ() : ez->CONST_FALSE, ez->CONST_FALSE);

		ਜੇ (cell->type.ਵਿੱਚ(ID($ਖਿਸਕਾਓ), ID($shiftx)))
			shifted_a = ez->vec_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a_shifted;

			extend_ਦੋਆਧਾਰੀ_ਅੰਕ = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			ਜੇ (cell->parameters[ID::A_SIGNED].as_bool())
				extend_ਦੋਆਧਾਰੀ_ਅੰਕ = undef_a.ਪਿੱਠ();

			ਜਦੋਂ (undef_y.ਮਾਪ() < undef_a.ਮਾਪ())
				undef_y.ਧੱਕੋ_ਪਿੱਠ(ez->literal());
			ਜਦੋਂ (undef_y.ਮਾਪ() > undef_a.ਮਾਪ())
				undef_a.ਧੱਕੋ_ਪਿੱਠ(extend_ਦੋਆਧਾਰੀ_ਅੰਕ);

			ਜੇ (cell->type.ਵਿੱਚ(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_ਖਿਸਕਾਓ_ਖੱਬਾ(undef_a, b, ਝੂਠ, ez->CONST_FALSE, ez->CONST_FALSE);

			ਜੇ (cell->type == ID($shr))
				undef_a_shifted = ez->vec_ਖਿਸਕਾਓ_ਸੱਜਾ(undef_a, b, ਝੂਠ, ez->CONST_FALSE, ez->CONST_FALSE);

			ਜੇ (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_ਖਿਸਕਾਓ_ਸੱਜਾ(undef_a, b, ਝੂਠ, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.ਪਿੱਠ() : ez->CONST_FALSE, ez->CONST_FALSE);

			ਜੇ (cell->type == ID($ਖਿਸਕਾਓ))
				undef_a_shifted = ez->vec_ਖਿਸਕਾਓ_ਸੱਜਾ(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			ਜੇ (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_ਖਿਸਕਾਓ_ਸੱਜਾ(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			ਪੂਰਨ undef_any_b = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_b);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_all_y_ਬਿੱਟ(undef_y.ਮਾਪ(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_ਬਿੱਟ), undef_y));
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($mul))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		extend_Signal_ਚੌੜਾਈ(a, b, y, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		std::ਸਦਿਸ਼<ਪੂਰਨ> tmp(a.ਮਾਪ(), ez->CONST_FALSE);
		ਲਈ (ਪੂਰਨ i = 0; i < ਪੂਰਨ(a.ਮਾਪ()); i++)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> shifted_a(a.ਮਾਪ(), ez->CONST_FALSE);
			ਲਈ (ਪੂਰਨ j = i; j < ਪੂਰਨ(a.ਮਾਪ()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_ਜੋੜੋ(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ਜੇ (model_undef) {
			log_assert(arith_undef_handled);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($macc), ID($macc_v2)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> tmp(Get_ਮਾਪ(y), ez->CONST_FALSE);

		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : macc.terms)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> ਵਿੱਚ_a = importDefSigSpec(ਪੋਰਟ.ਵਿੱਚ_a, timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> ਵਿੱਚ_b = importDefSigSpec(ਪੋਰਟ.ਵਿੱਚ_b, timestep);

			ਜਦੋਂ (Get_ਮਾਪ(ਵਿੱਚ_a) < Get_ਮਾਪ(y))
				ਵਿੱਚ_a.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ.is_signed && !ਵਿੱਚ_a.ਖਾਲੀ_ਥਾਵਾਂ() ? ਵਿੱਚ_a.ਪਿੱਠ() : ez->CONST_FALSE);
			ਵਿੱਚ_a.resize(Get_ਮਾਪ(y));

			ਜੇ (Get_ਮਾਪ(ਵਿੱਚ_b))
			{
				ਜਦੋਂ (Get_ਮਾਪ(ਵਿੱਚ_b) < Get_ਮਾਪ(y))
					ਵਿੱਚ_b.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ.is_signed && !ਵਿੱਚ_b.ਖਾਲੀ_ਥਾਵਾਂ() ? ਵਿੱਚ_b.ਪਿੱਠ() : ez->CONST_FALSE);
				ਵਿੱਚ_b.resize(Get_ਮਾਪ(y));

				ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(ਵਿੱਚ_b); i++) {
					std::ਸਦਿਸ਼<ਪੂਰਨ> shifted_a(ਵਿੱਚ_a.ਮਾਪ(), ez->CONST_FALSE);
					ਲਈ (ਪੂਰਨ j = i; j < ਪੂਰਨ(ਵਿੱਚ_a.ਮਾਪ()); j++)
						shifted_a.at(j) = ਵਿੱਚ_a.at(j-i);
					ਜੇ (ਪੋਰਟ.do_ਘਟਾਓ)
						tmp = ez->vec_ite(ਵਿੱਚ_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					ਨਹੀਂਤਾਂ
						tmp = ez->vec_ite(ਵਿੱਚ_b.at(i), ez->vec_ਜੋੜੋ(tmp, shifted_a), tmp);
				}
			}
			ਨਹੀਂਤਾਂ
			{
				ਜੇ (ਪੋਰਟ.do_ਘਟਾਓ)
					tmp = ez->vec_sub(tmp, ਵਿੱਚ_a);
				ਨਹੀਂਤਾਂ
					tmp = ez->vec_ਜੋੜੋ(tmp, ਵਿੱਚ_a);
			}
		}

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_c;

			ਜੇ (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::C), timestep);

			ਪੂਰਨ undef_any_a = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_a);
			ਪੂਰਨ undef_any_b = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_b);
			ਪੂਰਨ undef_any_c = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_c);
			ਪੂਰਨ undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::ਸਦਿਸ਼<ਪੂਰਨ>(Get_ਮਾਪ(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		ਨਹੀਂਤਾਂ
			ez->assume(ez->vec_eq(y, tmp));

		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		extend_Signal_ਚੌੜਾਈ(a, b, y, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;

		std::ਸਦਿਸ਼<ਪੂਰਨ> a_u, b_u;
		ਜੇ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.ਪਿੱਠ(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.ਪਿੱਠ(), ez->vec_neg(b), b);
		} ਨਹੀਂਤਾਂ {
			a_u = a;
			b_u = b;
		}

		std::ਸਦਿਸ਼<ਪੂਰਨ> chain_buf = a_u;
		std::ਸਦਿਸ਼<ਪੂਰਨ> y_u(a_u.ਮਾਪ(), ez->CONST_FALSE);
		ਲਈ (ਪੂਰਨ i = ਪੂਰਨ(a.ਮਾਪ())-1; i >= 0; i--)
		{
			chain_buf.ਪਾਓ(chain_buf.ਸਿਰਾ(), chain_buf.ਮਾਪ(), ez->CONST_FALSE);

			std::ਸਦਿਸ਼<ਪੂਰਨ> b_shl(i, ez->CONST_FALSE);
			b_shl.ਪਾਓ(b_shl.ਸਿਰਾ(), b_u.begin(), b_u.ਸਿਰਾ());
			b_shl.ਪਾਓ(b_shl.ਸਿਰਾ(), chain_buf.ਮਾਪ()-b_shl.ਮਾਪ(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.ਮਾਪ(), chain_buf.ਸਿਰਾ());
		}

		std::ਸਦਿਸ਼<ਪੂਰਨ> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.ਮਾਪ());

		// modulo calculation
		std::ਸਦਿਸ਼<ਪੂਰਨ> modulo_trunc;
		ਪੂਰਨ floored_eq_trunc;
		ਜੇ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.ਪਿੱਠ(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.ਪਿੱਠ(), b.ਪਿੱਠ()), ez->NOT(ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, modulo_trunc)));
		} ਨਹੀਂਤਾਂ {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		ਜੇ (cell->type == ID($div)) {
			ਜੇ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.ਪਿੱਠ(), b.ਪਿੱਠ()), ez->vec_neg(y_u), y_u)));
			ਨਹੀਂਤਾਂ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($divfloor)) {
			ਜੇ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.ਪਿੱਠ(), b.ਪਿੱਠ()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_ਜੋੜੋ(y_u, ez->vec_const_unsigned(1, y_u.ਮਾਪ())),
						y_u
					)),
					y_u
				)));
			ਨਹੀਂਤਾਂ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_ਜੋੜੋ(modulo_trunc, b))));
		}

		ਜੇ (ignore_div_by_zero) {
			ez->assume(ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, b));
		} ਨਹੀਂਤਾਂ {
			std::ਸਦਿਸ਼<ਪੂਰਨ> div_zero_ਨਤੀਜਾ;
			ਜੇ (cell->type.ਵਿੱਚ(ID($div), ID($divfloor))) {
				ਜੇ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::ਸਦਿਸ਼<ਪੂਰਨ> all_ones(y.ਮਾਪ(), ez->CONST_TRUE);
					std::ਸਦਿਸ਼<ਪੂਰਨ> only_ਪਹਿਲਾ_one(y.ਮਾਪ(), ez->CONST_FALSE);
					only_ਪਹਿਲਾ_one.at(0) = ez->CONST_TRUE;
					div_zero_ਨਤੀਜਾ = ez->vec_ite(a.ਪਿੱਠ(), only_ਪਹਿਲਾ_one, all_ones);
				} ਨਹੀਂਤਾਂ {
					div_zero_ਨਤੀਜਾ.ਪਾਓ(div_zero_ਨਤੀਜਾ.ਸਿਰਾ(), cell->get_ਪੋਰਟ(ID::A).ਮਾਪ(), ez->CONST_TRUE);
					div_zero_ਨਤੀਜਾ.ਪਾਓ(div_zero_ਨਤੀਜਾ.ਸਿਰਾ(), y.ਮਾਪ() - div_zero_ਨਤੀਜਾ.ਮਾਪ(), ez->CONST_FALSE);
				}
			} ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				ਪੂਰਨ copy_a_ਬਿੱਟ = min(cell->get_ਪੋਰਟ(ID::A).ਮਾਪ(), cell->get_ਪੋਰਟ(ID::B).ਮਾਪ());
				div_zero_ਨਤੀਜਾ.ਪਾਓ(div_zero_ਨਤੀਜਾ.ਸਿਰਾ(), a.begin(), a.begin() + copy_a_ਬਿੱਟ);
				ਜੇ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_ਨਤੀਜਾ.ਪਾਓ(div_zero_ਨਤੀਜਾ.ਸਿਰਾ(), y.ਮਾਪ() - div_zero_ਨਤੀਜਾ.ਮਾਪ(), div_zero_ਨਤੀਜਾ.ਪਿੱਠ());
				ਨਹੀਂਤਾਂ
					div_zero_ਨਤੀਜਾ.ਪਾਓ(div_zero_ਨਤੀਜਾ.ਸਿਰਾ(), y.ਮਾਪ() - div_zero_ਨਤੀਜਾ.ਮਾਪ(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, b), y_tmp, div_zero_ਨਤੀਜਾ)));
		}

		ਜੇ (model_undef) {
			log_assert(arith_undef_handled);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($lut))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> lut;
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : cell->getParam(ID::LUT))
			lut.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		ਜਦੋਂ (Get_ਮਾਪ(lut) < (1 << Get_ਮਾਪ(a)))
			lut.ਧੱਕੋ_ਪਿੱਠ(ez->CONST_FALSE);
		lut.resize(1 << Get_ਮਾਪ(a));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> t(lut), u(Get_ਮਾਪ(t), ez->CONST_FALSE);

			ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(a)-1; i >= 0; i--)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> t0(t.begin(), t.begin() + Get_ਮਾਪ(t)/2);
				std::ਸਦਿਸ਼<ਪੂਰਨ> t1(t.begin() + Get_ਮਾਪ(t)/2, t.ਸਿਰਾ());

				std::ਸਦਿਸ਼<ਪੂਰਨ> u0(u.begin(), u.begin() + Get_ਮਾਪ(u)/2);
				std::ਸਦਿਸ਼<ਪੂਰਨ> u1(u.begin() + Get_ਮਾਪ(u)/2, u.ਸਿਰਾ());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_ਮਾਪ(t) == 1);
			log_assert(Get_ਮਾਪ(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep), u));
		}
		ਨਹੀਂਤਾਂ
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> t = lut;
			ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(a)-1; i >= 0; i--)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> t0(t.begin(), t.begin() + Get_ਮਾਪ(t)/2);
				std::ਸਦਿਸ਼<ਪੂਰਨ> t1(t.begin() + Get_ਮਾਪ(t)/2, t.ਸਿਰਾ());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_ਮਾਪ(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($sop))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		ਪੂਰਨ y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep).at(0);

		ਪੂਰਨ ਚੌੜਾਈ = cell->getParam(ID::ਚੌੜਾਈ_2).as_int();
		ਪੂਰਨ ਡੂੰਘਾਈ = cell->getParam(ID::ਡੂੰਘਾਈ_2).as_int();

		ਸਦਿਸ਼<State> ਸਾਰਣੀ_raw = cell->getParam(ID::ਸਾਰਣੀ).ਮੰਜ਼ਿਲ_ਬਿੱਟ();
		ਜਦੋਂ (Get_ਮਾਪ(ਸਾਰਣੀ_raw) < 2*ਚੌੜਾਈ*ਡੂੰਘਾਈ)
			ਸਾਰਣੀ_raw.ਧੱਕੋ_ਪਿੱਠ(State::S0);

		ਸਦਿਸ਼<ਸਦਿਸ਼<ਪੂਰਨ>> ਸਾਰਣੀ_2(ਡੂੰਘਾਈ);

		ਲਈ (ਪੂਰਨ i = 0; i < ਡੂੰਘਾਈ; i++)
		ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ; j++)
		{
			ਤਾਰਕਿਕ pat0 = (ਸਾਰਣੀ_raw[2*ਚੌੜਾਈ*i + 2*j + 0] == State::S1);
			ਤਾਰਕਿਕ pat1 = (ਸਾਰਣੀ_raw[2*ਚੌੜਾਈ*i + 2*j + 1] == State::S1);

			ਜੇ (pat0 && !pat1)
				ਸਾਰਣੀ_2.at(i).ਧੱਕੋ_ਪਿੱਠ(0);
			ਨਹੀਂਤਾਂ ਜੇ (!pat0 && pat1)
				ਸਾਰਣੀ_2.at(i).ਧੱਕੋ_ਪਿੱਠ(1);
			ਨਹੀਂਤਾਂ
				ਸਾਰਣੀ_2.at(i).ਧੱਕੋ_ਪਿੱਠ(-1);
		}

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> products, undef_products;
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			ਪੂਰਨ undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep).at(0);

			ਲਈ (ਪੂਰਨ i = 0; i < ਡੂੰਘਾਈ; i++)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> cmp_a, cmp_ua, cmp_b;

				ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ; j++)
					ਜੇ (ਸਾਰਣੀ_2.at(i).at(j) >= 0) {
						cmp_a.ਧੱਕੋ_ਪਿੱਠ(a.at(j));
						cmp_ua.ਧੱਕੋ_ਪਿੱਠ(undef_a.at(j));
						cmp_b.ਧੱਕੋ_ਪਿੱਠ(ਸਾਰਣੀ_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::ਸਦਿਸ਼<ਪੂਰਨ> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::ਸਦਿਸ਼<ਪੂਰਨ> masked_b = ez->vec_or(cmp_b, cmp_ua);

				ਪੂਰਨ masked_eq = ez->vec_eq(masked_a, masked_b);
				ਪੂਰਨ any_undef = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, cmp_ua);

				undef_products.ਧੱਕੋ_ਪਿੱਠ(ez->AND(any_undef, masked_eq));
				products.ਧੱਕੋ_ਪਿੱਠ(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			ਪੂਰਨ yy = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, products);
			ez->ਸੈੱਟ_2(undef_y, ez->AND(ez->NOT(yy), ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		ਨਹੀਂਤਾਂ
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> products;

			ਲਈ (ਪੂਰਨ i = 0; i < ਡੂੰਘਾਈ; i++)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> cmp_a, cmp_b;

				ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ; j++)
					ਜੇ (ਸਾਰਣੀ_2.at(i).at(j) >= 0) {
						cmp_a.ਧੱਕੋ_ਪਿੱਠ(a.at(j));
						cmp_b.ਧੱਕੋ_ਪਿੱਠ(ਸਾਰਣੀ_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.ਧੱਕੋ_ਪਿੱਠ(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->ਸੈੱਟ_2(y, ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, products));
		}

		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($fa))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> c = importDefSigSpec(cell->get_ਪੋਰਟ(ID::C), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> x = importDefSigSpec(cell->get_ਪੋਰਟ(ID::X), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;
		std::ਸਦਿਸ਼<ਪੂਰਨ> xx = model_undef ? ez->vec_var(x.ਮਾਪ()) : x;

		std::ਸਦਿਸ਼<ਪੂਰਨ> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::ਸਦਿਸ਼<ਪੂਰਨ> t2 = ez->vec_and(a, b);
		std::ਸਦਿਸ਼<ਪੂਰਨ> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_c = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::C), timestep);

			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_x = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($lcu))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> p = importDefSigSpec(cell->get_ਪੋਰਟ(ID::P), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> g = importDefSigSpec(cell->get_ਪੋਰਟ(ID::G), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> ci = importDefSigSpec(cell->get_ਪੋਰਟ(ID::CI), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> co = importDefSigSpec(cell->get_ਪੋਰਟ(ID::CO), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(co.ਮਾਪ()) : co;

		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(co); i++)
			ez->ਸੈੱਟ_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_p = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::P), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_g = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::G), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_ci = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::CI), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_co = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::CO), timestep);

			ਪੂਰਨ undef_any_p = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_p);
			ਪੂਰਨ undef_any_g = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_g);
			ਪੂਰਨ undef_any_ci = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, undef_ci);
			ਪੂਰਨ undef_co_ਦੋਆਧਾਰੀ_ਅੰਕ = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_co_ਬਿੱਟ(undef_co.ਮਾਪ(), undef_co_ਦੋਆਧਾਰੀ_ਅੰਕ);
			ez->assume(ez->vec_eq(undef_co_ਬਿੱਟ, undef_co));

			undefGating(co, yy, undef_co);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($alu))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> b = importDefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> x = importDefSigSpec(cell->get_ਪੋਰਟ(ID::X), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> ci = importDefSigSpec(cell->get_ਪੋਰਟ(ID::CI), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> bi = importDefSigSpec(cell->get_ਪੋਰਟ(ID::BI), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> co = importDefSigSpec(cell->get_ਪੋਰਟ(ID::CO), timestep);

		extend_Signal_ਚੌੜਾਈ(a, b, y, cell);
		extend_Signal_ਚੌੜਾਈ(a, b, x, cell);
		extend_Signal_ਚੌੜਾਈ(a, b, co, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> def_y = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;
		std::ਸਦਿਸ਼<ਪੂਰਨ> def_x = model_undef ? ez->vec_var(x.ਮਾਪ()) : x;
		std::ਸਦਿਸ਼<ਪੂਰਨ> def_co = model_undef ? ez->vec_var(co.ਮਾਪ()) : co;

		log_assert(Get_ਮਾਪ(y) == Get_ਮਾਪ(x));
		log_assert(Get_ਮਾਪ(y) == Get_ਮਾਪ(co));
		log_assert(Get_ਮਾਪ(ci) == 1);
		log_assert(Get_ਮਾਪ(bi) == 1);

		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(y); i++)
		{
			ਪੂਰਨ s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->ਸੈੱਟ_2(def_x.at(i), ez->XOR(s1, s2));
			ez->ਸੈੱਟ_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->ਸੈੱਟ_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_b = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::B), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_ci = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::CI), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_bi = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::BI), timestep);

			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_x = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::X), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_co = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::CO), timestep);

			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, undef_y, cell);
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, undef_x, cell);
			extend_Signal_ਚੌੜਾਈ(undef_a, undef_b, undef_co, cell);

			std::ਸਦਿਸ਼<ਪੂਰਨ> all_inputs_undef;
			all_inputs_undef.ਪਾਓ(all_inputs_undef.ਸਿਰਾ(), undef_a.begin(), undef_a.ਸਿਰਾ());
			all_inputs_undef.ਪਾਓ(all_inputs_undef.ਸਿਰਾ(), undef_b.begin(), undef_b.ਸਿਰਾ());
			all_inputs_undef.ਪਾਓ(all_inputs_undef.ਸਿਰਾ(), undef_ci.begin(), undef_ci.ਸਿਰਾ());
			all_inputs_undef.ਪਾਓ(all_inputs_undef.ਸਿਰਾ(), undef_bi.begin(), undef_bi.ਸਿਰਾ());
			ਪੂਰਨ undef_any = ez->ਵਿਅੰਜਕ_2(ezSAT::OpOr, all_inputs_undef);

			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(undef_y); i++) {
				ez->ਸੈੱਟ_2(undef_y.at(i), undef_any);
				ez->ਸੈੱਟ_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->ਸੈੱਟ_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($ਟੁਕੜਾ))
	{
		RTLIL::SigSpec a = cell->get_ਪੋਰਟ(ID::A);
		RTLIL::SigSpec y = cell->get_ਪੋਰਟ(ID::Y);
		ez->assume(ਸੰਕੇਤ_eq(a.ਕੱਢੋ_2(cell->parameters.at(ID::ਵਿੱਥ_2).as_int(), y.ਮਾਪ()), y, timestep));
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_ਪੋਰਟ(ID::A);
		RTLIL::SigSpec b = cell->get_ਪੋਰਟ(ID::B);
		RTLIL::SigSpec y = cell->get_ਪੋਰਟ(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(ਸੰਕੇਤ_eq(ab, y, timestep));
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_ਅੰਕੜੇ ff(ਖਾਲੀ_ਸੂਚਕ, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		ਜੇ (ff.has_aload || ff.has_arst || ff.has_sr)
			ਵਾਪਸ ਝੂਠ;

		ਜੇ (timestep == 1)
		{
			initial_state.ਜੋੜੋ((*sigmap)(cell->get_ਪੋਰਟ(ID::Q)));
			ਜੇ (model_undef && def_formal) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_q = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		ਨਹੀਂਤਾਂ
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> d = importDefSigSpec(cell->get_ਪੋਰਟ(ID::D), timestep-1);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_d;
			ਜੇ (model_undef)
				undef_d = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::D), timestep-1);
			ਜੇ (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				ਪੂਰਨ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::ਸਦਿਸ਼<ਪੂਰਨ> rval = importDefSigSpec(ff.val_srst, timestep-1);
				ਪੂਰਨ undef_srst = -1;
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_rval;
				ਜੇ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ਜੇ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				ਨਹੀਂਤਾਂ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			ਜੇ (ff.has_ce) {
				ਪੂਰਨ ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::ਸਦਿਸ਼<ਪੂਰਨ> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				ਪੂਰਨ undef_ce = -1;
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_old_q;
				ਜੇ (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				ਜੇ (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				ਨਹੀਂਤਾਂ
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			ਜੇ (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				ਪੂਰਨ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::ਸਦਿਸ਼<ਪੂਰਨ> rval = importDefSigSpec(ff.val_srst, timestep-1);
				ਪੂਰਨ undef_srst = -1;
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_rval;
				ਜੇ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ਜੇ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				ਨਹੀਂਤਾਂ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::ਸਦਿਸ਼<ਪੂਰਨ> q = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Q), timestep);

			std::ਸਦਿਸ਼<ਪੂਰਨ> qq = model_undef ? ez->vec_var(q.ਮਾਪ()) : q;
			ez->assume(ez->vec_eq(d, qq));

			ਜੇ (model_undef)
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_q = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($anyconst))
	{
		ਜੇ (timestep < 2) {
			ਜੇ (model_undef && def_formal) {
				std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			ਵਾਪਸ ਸੱਚ;
		}

		std::ਸਦਿਸ਼<ਪੂਰਨ> d = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep-1);
		std::ਸਦਿਸ਼<ਪੂਰਨ> q = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

		std::ਸਦਿਸ਼<ਪੂਰਨ> qq = (model_undef && !def_formal) ? ez->vec_var(q.ਮਾਪ()) : q;
		ez->assume(ez->vec_eq(d, qq));

		ਜੇ (model_undef)
		{
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_d = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep-1);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_q = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);

			ਜੇ (def_formal) {
				ਲਈ (ਸਵੈਚਾਲਿਤ &undef_q_ਦੋਆਧਾਰੀ_ਅੰਕ : undef_q)
					ez->ਸੈੱਟ_2(ez->CONST_FALSE, undef_q_ਦੋਆਧਾਰੀ_ਅੰਕ);
			} ਨਹੀਂਤਾਂ {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($anyseq))
	{
		ਜੇ (model_undef && def_formal) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_q = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			ਲਈ (ਸਵੈਚਾਲਿਤ &undef_q_ਦੋਆਧਾਰੀ_ਅੰਕ : undef_q)
				ez->ਸੈੱਟ_2(ez->CONST_FALSE, undef_q_ਦੋਆਧਾਰੀ_ਅੰਕ);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($barrier) && !model_barriers)
		ਵਾਪਸ ਸੱਚ;

	ਜੇ (cell->type.ਵਿੱਚ(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> a = importDefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		extend_Signal_ਚੌੜਾਈ_Unary(a, y, cell);

		std::ਸਦਿਸ਼<ਪੂਰਨ> yy = model_undef ? ez->vec_var(y.ਮਾਪ()) : y;
		ez->assume(ez->vec_eq(a, yy));

		ਜੇ (model_undef) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_a = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::A), timestep);
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			extend_Signal_ਚੌੜਾਈ_Unary(undef_a, undef_y, cell, ਝੂਠ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($initstate))
	{
		ਸਵੈਚਾਲਿਤ ਕੁੰਜੀ = make_pair(ਅਗੇਤਰ_2, timestep);
		ਜੇ (initstates.ਗਿਣਤੀ_2(ਕੁੰਜੀ) == 0)
			initstates[ਕੁੰਜੀ] = ਝੂਠ;

		std::ਸਦਿਸ਼<ਪੂਰਨ> y = importDefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
		log_assert(Get_ਮਾਪ(y) == 1);
		ez->ਸੈੱਟ_2(y[0], initstates[ਕੁੰਜੀ] ? ez->CONST_TRUE : ez->CONST_FALSE);

		ਜੇ (model_undef) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_y = importUndefSigSpec(cell->get_ਪੋਰਟ(ID::Y), timestep);
			log_assert(Get_ਮਾਪ(undef_y) == 1);
			ez->ਸੈੱਟ_2(undef_y[0], ez->CONST_FALSE);
		}

		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($ਪ੍ਰਤਿਜ੍ਞਾ))
	{
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_ਪੋਰਟ(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_ਪੋਰਟ(ID::EN)));
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($assume))
	{
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_ਪੋਰਟ(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_ਪੋਰਟ(ID::EN)));
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($scopeinfo) || cell->type == ID($ਨਿਵੇਸ਼_ਪੋਰਟ))
	{
		ਵਾਪਸ ਸੱਚ;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	ਵਾਪਸ ਝੂਠ;
}

ਨਾਮਾਕਾਸ਼ Yosys {

ਖਾਲੀ report_missing_model(ਤਾਰਕਿਕ warn_only, RTLIL::Cell* cell)
{
	std::string s;
	ਜੇ (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	ਨਹੀਂਤਾਂ
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	ਜੇ (warn_only) {
		log_warning_noprefix("%s\n", s);
	} ਨਹੀਂਤਾਂ {
		log_error("%s\n", s);
	}
}

}
