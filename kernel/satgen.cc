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

#അന്തര്ഭാവ "kernel/satgen.h"
#അന്തര്ഭാവ "kernel/ff.h"
#അന്തര്ഭാവ "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

തർക്കം SatGen::importCell(RTLIL::Cell *cell, പൂർണ്ണസംഖ്യ timestep)
{
	തർക്കം arith_undef_handled = അസത്യം;
	തർക്കം is_arith_compare = cell->type.in(ID($lt), ID($le), ID($ge), ID($gt));

	എങ്കിൽ (model_undef && (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		എങ്കിൽ (is_arith_compare)
			extendSignalWidth(undef_a, undef_b, cell, സത്യം);
		അല്ലെങ്കിൽ
			extendSignalWidth(undef_a, undef_b, undef_y, cell, സത്യം);

		പൂർണ്ണസംഖ്യ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
		പൂർണ്ണസംഖ്യ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
		പൂർണ്ണസംഖ്യ undef_y_bit = ez->OR(undef_any_a, undef_any_b);

		എങ്കിൽ (cell->type.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::vector<പൂർണ്ണസംഖ്യ> b = importSigSpec(cell->getPort(ID::B), timestep);
			undef_y_bit = ez->OR(undef_y_bit, ez->NOT(ez->expression(ezSAT::OpOr, b)));
		}

		എങ്കിൽ (is_arith_compare) {
			വേണ്ടി (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));
		} അല്ലെങ്കിൽ {
			std::vector<പൂർണ്ണസംഖ്യ> undef_y_bits(undef_y.size(), undef_y_bit);
			ez->assume(ez->vec_eq(undef_y_bits, undef_y));
		}

		arith_undef_handled = സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($add), ID($sub)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		എങ്കിൽ (cell->type.in(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		എങ്കിൽ (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		എങ്കിൽ (cell->type.in(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		എങ്കിൽ (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		എങ്കിൽ (cell->type.in(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		എങ്കിൽ (cell->type.in(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		എങ്കിൽ (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		എങ്കിൽ (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		എങ്കിൽ (cell->type == ID($add))
			ez->assume(ez->vec_eq(ez->vec_add(a, b), yy));
		എങ്കിൽ (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		എങ്കിൽ (model_undef && !arith_undef_handled)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, undef_y, cell, അസത്യം);

			എങ്കിൽ (cell->type.in(ID($and), ID($_AND_), ID($_NAND_))) {
				std::vector<പൂർണ്ണസംഖ്യ> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<പൂർണ്ണസംഖ്യ> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<പൂർണ്ണസംഖ്യ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($or), ID($_OR_), ID($_NOR_))) {
				std::vector<പൂർണ്ണസംഖ്യ> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<പൂർണ്ണസംഖ്യ> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<പൂർണ്ണസംഖ്യ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::vector<പൂർണ്ണസംഖ്യ> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($_ANDNOT_)) {
				std::vector<പൂർണ്ണസംഖ്യ> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<പൂർണ്ണസംഖ്യ> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<പൂർണ്ണസംഖ്യ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($_ORNOT_)) {
				std::vector<പൂർണ്ണസംഖ്യ> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<പൂർണ്ണസംഖ്യ> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<പൂർണ്ണസംഖ്യ> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			അല്ലെങ്കിൽ
				log_abort();

			undefGating(y, yy, undef_y);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		തർക്കം aoi_mode = cell->type.in(ID($_AOI3_), ID($_AOI4_));
		തർക്കം three_mode = cell->type.in(ID($_AOI3_), ID($_OAI3_));

		പൂർണ്ണസംഖ്യ a = importDefSigSpec(cell->getPort(ID::A), timestep).at(0);
		പൂർണ്ണസംഖ്യ b = importDefSigSpec(cell->getPort(ID::B), timestep).at(0);
		പൂർണ്ണസംഖ്യ c = importDefSigSpec(cell->getPort(ID::C), timestep).at(0);
		പൂർണ്ണസംഖ്യ d = three_mode ? (aoi_mode ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->getPort(ID::D), timestep).at(0);
		പൂർണ്ണസംഖ്യ y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);
		പൂർണ്ണസംഖ്യ yy = model_undef ? ez->literal() : y;

		എങ്കിൽ (cell->type.in(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		അല്ലെങ്കിൽ
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		എങ്കിൽ (model_undef)
		{
			പൂർണ്ണസംഖ്യ undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep).at(0);
			പൂർണ്ണസംഖ്യ undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep).at(0);
			പൂർണ്ണസംഖ്യ undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep).at(0);
			പൂർണ്ണസംഖ്യ undef_d = three_mode ? ez->CONST_FALSE : importUndefSigSpec(cell->getPort(ID::D), timestep).at(0);
			പൂർണ്ണസംഖ്യ undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			എങ്കിൽ (aoi_mode)
			{
				പൂർണ്ണസംഖ്യ a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				പൂർണ്ണസംഖ്യ b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				പൂർണ്ണസംഖ്യ c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				പൂർണ്ണസംഖ്യ d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				പൂർണ്ണസംഖ്യ ab = ez->AND(a, b), cd = ez->AND(c, d);
				പൂർണ്ണസംഖ്യ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				പൂർണ്ണസംഖ്യ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				പൂർണ്ണസംഖ്യ ab1 = ez->AND(ab, ez->NOT(undef_ab));
				പൂർണ്ണസംഖ്യ cd1 = ez->AND(cd, ez->NOT(undef_cd));
				പൂർണ്ണസംഖ്യ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			അല്ലെങ്കിൽ
			{
				പൂർണ്ണസംഖ്യ a1 = ez->AND(a, ez->NOT(undef_a));
				പൂർണ്ണസംഖ്യ b1 = ez->AND(b, ez->NOT(undef_b));
				പൂർണ്ണസംഖ്യ c1 = ez->AND(c, ez->NOT(undef_c));
				പൂർണ്ണസംഖ്യ d1 = ez->AND(d, ez->NOT(undef_d));

				പൂർണ്ണസംഖ്യ ab = ez->OR(a, b), cd = ez->OR(c, d);
				പൂർണ്ണസംഖ്യ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				പൂർണ്ണസംഖ്യ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				പൂർണ്ണസംഖ്യ ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				പൂർണ്ണസംഖ്യ cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				പൂർണ്ണസംഖ്യ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($_NOT_), ID($not)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		എങ്കിൽ (model_undef) {
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, അസത്യം);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($bweqx))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> bweqx = ez->vec_not(ez->vec_xor(a, b));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<പൂർണ്ണസംഖ്യ> both_undef = ez->vec_and(undef_a, undef_b);
			std::vector<പൂർണ്ണസംഖ്യ> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			വേണ്ടി (പൂർണ്ണസംഖ്യ yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;
		എങ്കിൽ (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		അല്ലെങ്കിൽ
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<പൂർണ്ണസംഖ്യ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<പൂർണ്ണസംഖ്യ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::vector<പൂർണ്ണസംഖ്യ> yX;
			എങ്കിൽ (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			അല്ലെങ്കിൽ
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($bmux))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> undef_a, undef_s, undef_y;

		എങ്കിൽ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		എങ്കിൽ (GetSize(s) == 0) {
			ez->vec_set(a, y);
			എങ്കിൽ (model_undef)
				ez->vec_set(undef_a, undef_y);
		} അല്ലെങ്കിൽ {
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(s)-1; i >= 0; i--)
			{
				std::vector<പൂർണ്ണസംഖ്യ> out = (i == 0) ? y : ez->vec_var(a.size() / 2);
				std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(out.size()) : out;

				std::vector<പൂർണ്ണസംഖ്യ> a0(a.begin(), a.begin() + a.size() / 2);
				std::vector<പൂർണ്ണസംഖ്യ> a1(a.begin() + a.size() / 2, a.end());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				എങ്കിൽ (model_undef)
				{
					std::vector<പൂർണ്ണസംഖ്യ> undef_out = (i == 0) ? undef_y : ez->vec_var(a.size() / 2);
					std::vector<പൂർണ്ണസംഖ്യ> undef_a0(undef_a.begin(), undef_a.begin() + a.size() / 2);
					std::vector<പൂർണ്ണസംഖ്യ> undef_a1(undef_a.begin() + a.size() / 2, undef_a.end());
					std::vector<പൂർണ്ണസംഖ്യ> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::vector<പൂർണ്ണസംഖ്യ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::vector<പൂർണ്ണസംഖ്യ> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_out));
					undefGating(out, yy, undef_out);

					undef_a = undef_out;
				}

				a = out;
			}
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($demux))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<പൂർണ്ണസംഖ്യ> undef_a, undef_s, undef_y;

		എങ്കിൽ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		എങ്കിൽ (GetSize(s) == 0) {
			ez->vec_set(a, y);
			എങ്കിൽ (model_undef)
				ez->vec_set(undef_a, undef_y);
		} അല്ലെങ്കിൽ {
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (1 << GetSize(s)); i++)
			{
				std::vector<പൂർണ്ണസംഖ്യ> ss;
				വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < GetSize(s); j++) {
					എങ്കിൽ (i & 1 << j)
						ss.തള്ളുക_back(s[j]);
					അല്ലെങ്കിൽ
						ss.തള്ളുക_back(ez->NOT(s[j]));
				}
				പൂർണ്ണസംഖ്യ sss = ez->expression(ezSAT::OpAnd, ss);

				വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < GetSize(a); j++) {
					ez->SET(ez->AND(sss, a[j]), yy.at(i * GetSize(a) + j));
				}

				എങ്കിൽ (model_undef)
				{
					പൂർണ്ണസംഖ്യ s0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					പൂർണ്ണസംഖ്യ us = ez->AND(ez->NOT(s0), ez->expression(ezSAT::OpOr, undef_s));
					വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < GetSize(a); j++) {
						പൂർണ്ണസംഖ്യ a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						പൂർണ്ണസംഖ്യ yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->SET(yX, undef_y.at(i * GetSize(a) + j));
					}
				}
			}
			എങ്കിൽ (model_undef)
				undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($pmux))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		std::vector<പൂർണ്ണസംഖ്യ> tmp = a;
		വേണ്ടി (size_t i = 0; i < s.size(); i++) {
			std::vector<പൂർണ്ണസംഖ്യ> part_of_b(b.begin()+i*a.size(), b.begin()+(i+1)*a.size());
			tmp = ez->vec_ite(s.at(i), part_of_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			പൂർണ്ണസംഖ്യ all_undef = ez->CONST_FALSE;
			പൂർണ്ണസംഖ്യ found_active = ez->CONST_FALSE;

			std::vector<പൂർണ്ണസംഖ്യ> undef_tmp = undef_a;

			വേണ്ടി (size_t i = 0; i < s.size(); i++)
			{
				std::vector<പൂർണ്ണസംഖ്യ> part_of_undef_b(undef_b.begin()+i*a.size(), undef_b.begin()+(i+1)*a.size());

				undef_tmp = ez->vec_ite(s.at(i), part_of_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), found_active));
				found_active = ez->OR(found_active, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::vector<പൂർണ്ണസംഖ്യ>(a.size(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($pos), ID($buf), ID($neg)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		എങ്കിൽ (cell->type.in(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} അല്ലെങ്കിൽ {
			std::vector<പൂർണ്ണസംഖ്യ> zero(a.size(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell);

			എങ്കിൽ (cell->type.in(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} അല്ലെങ്കിൽ {
				പൂർണ്ണസംഖ്യ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
				std::vector<പൂർണ്ണസംഖ്യ> undef_y_bits(undef_y.size(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_bits, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		എങ്കിൽ (cell->type == ID($reduce_and))
			ez->SET(ez->expression(ez->OpAnd, a), yy.at(0));
		എങ്കിൽ (cell->type.in(ID($reduce_or), ID($reduce_bool)))
			ez->SET(ez->expression(ez->OpOr, a), yy.at(0));
		എങ്കിൽ (cell->type == ID($reduce_xor))
			ez->SET(ez->expression(ez->OpXor, a), yy.at(0));
		എങ്കിൽ (cell->type == ID($reduce_xnor))
			ez->SET(ez->NOT(ez->expression(ez->OpXor, a)), yy.at(0));
		എങ്കിൽ (cell->type == ID($logic_not))
			ez->SET(ez->NOT(ez->expression(ez->OpOr, a)), yy.at(0));
		വേണ്ടി (size_t i = 1; i < y.size(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			പൂർണ്ണസംഖ്യ aX = ez->expression(ezSAT::OpOr, undef_a);

			എങ്കിൽ (cell->type == ID($reduce_and)) {
				പൂർണ്ണസംഖ്യ a0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				പൂർണ്ണസംഖ്യ a1 = ez->expression(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} അല്ലെങ്കിൽ
				log_abort();

			വേണ്ടി (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($logic_and), ID($logic_or)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> vec_a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> vec_b = importDefSigSpec(cell->getPort(ID::B), timestep);

		പൂർണ്ണസംഖ്യ a = ez->expression(ez->OpOr, vec_a);
		പൂർണ്ണസംഖ്യ b = ez->expression(ez->OpOr, vec_b);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		എങ്കിൽ (cell->type == ID($logic_and))
			ez->SET(ez->expression(ez->OpAnd, a, b), yy.at(0));
		അല്ലെങ്കിൽ
			ez->SET(ez->expression(ez->OpOr, a, b), yy.at(0));
		വേണ്ടി (size_t i = 1; i < y.size(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			പൂർണ്ണസംഖ്യ a0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_a), ez->expression(ezSAT::OpOr, undef_a)));
			പൂർണ്ണസംഖ്യ b0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_b), ez->expression(ezSAT::OpOr, undef_b)));
			പൂർണ്ണസംഖ്യ a1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			പൂർണ്ണസംഖ്യ b1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			പൂർണ്ണസംഖ്യ aX = ez->expression(ezSAT::OpOr, undef_a);
			പൂർണ്ണസംഖ്യ bX = ez->expression(ezSAT::OpOr, undef_b);

			എങ്കിൽ (cell->type == ID($logic_and))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($logic_or))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			അല്ലെങ്കിൽ
				log_abort();

			വേണ്ടി (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		തർക്കം is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, cell);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		എങ്കിൽ (model_undef && cell->type.in(ID($eqx), ID($nex))) {
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			extendSignalWidth(undef_a, undef_b, cell, സത്യം);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		എങ്കിൽ (cell->type == ID($lt))
			ez->SET(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		എങ്കിൽ (cell->type == ID($le))
			ez->SET(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		എങ്കിൽ (cell->type.in(ID($eq), ID($eqx)))
			ez->SET(ez->vec_eq(a, b), yy.at(0));
		എങ്കിൽ (cell->type.in(ID($ne), ID($nex)))
			ez->SET(ez->vec_ne(a, b), yy.at(0));
		എങ്കിൽ (cell->type == ID($ge))
			ez->SET(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		എങ്കിൽ (cell->type == ID($gt))
			ez->SET(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		വേണ്ടി (size_t i = 1; i < y.size(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		എങ്കിൽ (model_undef && cell->type.in(ID($eqx), ID($nex)))
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, സത്യം);

			എങ്കിൽ (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			അല്ലെങ്കിൽ
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			വേണ്ടി (size_t i = 0; i < y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		അല്ലെങ്കിൽ എങ്കിൽ (model_undef && cell->type.in(ID($eq), ID($ne)))
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, സത്യം);

			പൂർണ്ണസംഖ്യ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			പൂർണ്ണസംഖ്യ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			പൂർണ്ണസംഖ്യ undef_any = ez->OR(undef_any_a, undef_any_b);

			std::vector<പൂർണ്ണസംഖ്യ> masked_a_bits = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::vector<പൂർണ്ണസംഖ്യ> masked_b_bits = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			പൂർണ്ണസംഖ്യ masked_ne = ez->vec_ne(masked_a_bits, masked_b_bits);
			പൂർണ്ണസംഖ്യ undef_y_bit = ez->AND(undef_any, ez->NOT(masked_ne));

			വേണ്ടി (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		അല്ലെങ്കിൽ
		{
			എങ്കിൽ (model_undef) {
				std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		പൂർണ്ണസംഖ്യ extend_bit = ez->CONST_FALSE;

		എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool())
			extend_bit = a.back();

		ആകുമ്പോൾ (y.size() < a.size())
			y.തള്ളുക_back(ez->literal());
		ആകുമ്പോൾ (y.size() > a.size())
			a.തള്ളുക_back(extend_bit);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<പൂർണ്ണസംഖ്യ> shifted_a;

		എങ്കിൽ (cell->type.in( ID($shl), ID($sshl)))
			shifted_a = ez->vec_shift_ഇടത്(a, b, അസത്യം, ez->CONST_FALSE, ez->CONST_FALSE);

		എങ്കിൽ (cell->type == ID($shr))
			shifted_a = ez->vec_shift_വലത്(a, b, അസത്യം, ez->CONST_FALSE, ez->CONST_FALSE);

		എങ്കിൽ (cell->type == ID($sshr))
			shifted_a = ez->vec_shift_വലത്(a, b, അസത്യം, cell->parameters[ID::A_SIGNED].as_bool() ? a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

		എങ്കിൽ (cell->type.in(ID($shift), ID($shiftx)))
			shifted_a = ez->vec_shift_വലത്(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_a_shifted;

			extend_bit = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool())
				extend_bit = undef_a.back();

			ആകുമ്പോൾ (undef_y.size() < undef_a.size())
				undef_y.തള്ളുക_back(ez->literal());
			ആകുമ്പോൾ (undef_y.size() > undef_a.size())
				undef_a.തള്ളുക_back(extend_bit);

			എങ്കിൽ (cell->type.in(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_shift_ഇടത്(undef_a, b, അസത്യം, ez->CONST_FALSE, ez->CONST_FALSE);

			എങ്കിൽ (cell->type == ID($shr))
				undef_a_shifted = ez->vec_shift_വലത്(undef_a, b, അസത്യം, ez->CONST_FALSE, ez->CONST_FALSE);

			എങ്കിൽ (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_shift_വലത്(undef_a, b, അസത്യം, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

			എങ്കിൽ (cell->type == ID($shift))
				undef_a_shifted = ez->vec_shift_വലത്(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			എങ്കിൽ (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_shift_വലത്(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			പൂർണ്ണസംഖ്യ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			std::vector<പൂർണ്ണസംഖ്യ> undef_all_y_bits(undef_y.size(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_bits), undef_y));
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($mul))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		std::vector<പൂർണ്ണസംഖ്യ> tmp(a.size(), ez->CONST_FALSE);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < പൂർണ്ണസംഖ്യ(a.size()); i++)
		{
			std::vector<പൂർണ്ണസംഖ്യ> shifted_a(a.size(), ez->CONST_FALSE);
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = i; j < പൂർണ്ണസംഖ്യ(a.size()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_add(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		എങ്കിൽ (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($macc), ID($macc_v2)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::vector<പൂർണ്ണസംഖ്യ> tmp(GetSize(y), ez->CONST_FALSE);

		വേണ്ടി (auto &port : macc.terms)
		{
			std::vector<പൂർണ്ണസംഖ്യ> in_a = importDefSigSpec(port.in_a, timestep);
			std::vector<പൂർണ്ണസംഖ്യ> in_b = importDefSigSpec(port.in_b, timestep);

			ആകുമ്പോൾ (GetSize(in_a) < GetSize(y))
				in_a.തള്ളുക_back(port.is_signed && !in_a.empty() ? in_a.back() : ez->CONST_FALSE);
			in_a.resize(GetSize(y));

			എങ്കിൽ (GetSize(in_b))
			{
				ആകുമ്പോൾ (GetSize(in_b) < GetSize(y))
					in_b.തള്ളുക_back(port.is_signed && !in_b.empty() ? in_b.back() : ez->CONST_FALSE);
				in_b.resize(GetSize(y));

				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(in_b); i++) {
					std::vector<പൂർണ്ണസംഖ്യ> shifted_a(in_a.size(), ez->CONST_FALSE);
					വേണ്ടി (പൂർണ്ണസംഖ്യ j = i; j < പൂർണ്ണസംഖ്യ(in_a.size()); j++)
						shifted_a.at(j) = in_a.at(j-i);
					എങ്കിൽ (port.do_subtract)
						tmp = ez->vec_ite(in_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					അല്ലെങ്കിൽ
						tmp = ez->vec_ite(in_b.at(i), ez->vec_add(tmp, shifted_a), tmp);
				}
			}
			അല്ലെങ്കിൽ
			{
				എങ്കിൽ (port.do_subtract)
					tmp = ez->vec_sub(tmp, in_a);
				അല്ലെങ്കിൽ
					tmp = ez->vec_add(tmp, in_a);
			}
		}

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_c;

			എങ്കിൽ (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			പൂർണ്ണസംഖ്യ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			പൂർണ്ണസംഖ്യ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			പൂർണ്ണസംഖ്യ undef_any_c = ez->expression(ezSAT::OpOr, undef_c);
			പൂർണ്ണസംഖ്യ undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::vector<പൂർണ്ണസംഖ്യ>(GetSize(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		അല്ലെങ്കിൽ
			ez->assume(ez->vec_eq(y, tmp));

		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;

		std::vector<പൂർണ്ണസംഖ്യ> a_u, b_u;
		എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.back(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.back(), ez->vec_neg(b), b);
		} അല്ലെങ്കിൽ {
			a_u = a;
			b_u = b;
		}

		std::vector<പൂർണ്ണസംഖ്യ> chain_buf = a_u;
		std::vector<പൂർണ്ണസംഖ്യ> y_u(a_u.size(), ez->CONST_FALSE);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = പൂർണ്ണസംഖ്യ(a.size())-1; i >= 0; i--)
		{
			chain_buf.തിരുകുക(chain_buf.end(), chain_buf.size(), ez->CONST_FALSE);

			std::vector<പൂർണ്ണസംഖ്യ> b_shl(i, ez->CONST_FALSE);
			b_shl.തിരുകുക(b_shl.end(), b_u.begin(), b_u.end());
			b_shl.തിരുകുക(b_shl.end(), chain_buf.size()-b_shl.size(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.size(), chain_buf.end());
		}

		std::vector<പൂർണ്ണസംഖ്യ> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.size());

		// modulo calculation
		std::vector<പൂർണ്ണസംഖ്യ> modulo_trunc;
		പൂർണ്ണസംഖ്യ floored_eq_trunc;
		എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.back(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.back(), b.back()), ez->NOT(ez->expression(ezSAT::OpOr, modulo_trunc)));
		} അല്ലെങ്കിൽ {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		എങ്കിൽ (cell->type == ID($div)) {
			എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.back(), b.back()), ez->vec_neg(y_u), y_u)));
			അല്ലെങ്കിൽ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($divfloor)) {
			എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.back(), b.back()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_add(y_u, ez->vec_const_unsigned(1, y_u.size())),
						y_u
					)),
					y_u
				)));
			അല്ലെങ്കിൽ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_add(modulo_trunc, b))));
		}

		എങ്കിൽ (ignore_div_by_zero) {
			ez->assume(ez->expression(ezSAT::OpOr, b));
		} അല്ലെങ്കിൽ {
			std::vector<പൂർണ്ണസംഖ്യ> div_zero_result;
			എങ്കിൽ (cell->type.in(ID($div), ID($divfloor))) {
				എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::vector<പൂർണ്ണസംഖ്യ> all_ones(y.size(), ez->CONST_TRUE);
					std::vector<പൂർണ്ണസംഖ്യ> only_first_one(y.size(), ez->CONST_FALSE);
					only_first_one.at(0) = ez->CONST_TRUE;
					div_zero_result = ez->vec_ite(a.back(), only_first_one, all_ones);
				} അല്ലെങ്കിൽ {
					div_zero_result.തിരുകുക(div_zero_result.end(), cell->getPort(ID::A).size(), ez->CONST_TRUE);
					div_zero_result.തിരുകുക(div_zero_result.end(), y.size() - div_zero_result.size(), ez->CONST_FALSE);
				}
			} അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				പൂർണ്ണസംഖ്യ copy_a_bits = min(cell->getPort(ID::A).size(), cell->getPort(ID::B).size());
				div_zero_result.തിരുകുക(div_zero_result.end(), a.begin(), a.begin() + copy_a_bits);
				എങ്കിൽ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_result.തിരുകുക(div_zero_result.end(), y.size() - div_zero_result.size(), div_zero_result.back());
				അല്ലെങ്കിൽ
					div_zero_result.തിരുകുക(div_zero_result.end(), y.size() - div_zero_result.size(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->expression(ezSAT::OpOr, b), y_tmp, div_zero_result)));
		}

		എങ്കിൽ (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($lut))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> lut;
		വേണ്ടി (auto bit : cell->getParam(ID::LUT))
			lut.തള്ളുക_back(bit == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		ആകുമ്പോൾ (GetSize(lut) < (1 << GetSize(a)))
			lut.തള്ളുക_back(ez->CONST_FALSE);
		lut.resize(1 << GetSize(a));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> t(lut), u(GetSize(t), ez->CONST_FALSE);

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(a)-1; i >= 0; i--)
			{
				std::vector<പൂർണ്ണസംഖ്യ> t0(t.begin(), t.begin() + GetSize(t)/2);
				std::vector<പൂർണ്ണസംഖ്യ> t1(t.begin() + GetSize(t)/2, t.end());

				std::vector<പൂർണ്ണസംഖ്യ> u0(u.begin(), u.begin() + GetSize(u)/2);
				std::vector<പൂർണ്ണസംഖ്യ> u1(u.begin() + GetSize(u)/2, u.end());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(GetSize(t) == 1);
			log_assert(GetSize(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->getPort(ID::Y), timestep), u));
		}
		അല്ലെങ്കിൽ
		{
			std::vector<പൂർണ്ണസംഖ്യ> t = lut;
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(a)-1; i >= 0; i--)
			{
				std::vector<പൂർണ്ണസംഖ്യ> t0(t.begin(), t.begin() + GetSize(t)/2);
				std::vector<പൂർണ്ണസംഖ്യ> t1(t.begin() + GetSize(t)/2, t.end());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(GetSize(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($sop))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		പൂർണ്ണസംഖ്യ y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);

		പൂർണ്ണസംഖ്യ width = cell->getParam(ID::WIDTH).as_int();
		പൂർണ്ണസംഖ്യ depth = cell->getParam(ID::DEPTH).as_int();

		vector<State> table_raw = cell->getParam(ID::TABLE).to_bits();
		ആകുമ്പോൾ (GetSize(table_raw) < 2*width*depth)
			table_raw.തള്ളുക_back(State::S0);

		vector<vector<പൂർണ്ണസംഖ്യ>> table(depth);

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < depth; i++)
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < width; j++)
		{
			തർക്കം pat0 = (table_raw[2*width*i + 2*j + 0] == State::S1);
			തർക്കം pat1 = (table_raw[2*width*i + 2*j + 1] == State::S1);

			എങ്കിൽ (pat0 && !pat1)
				table.at(i).തള്ളുക_back(0);
			അല്ലെങ്കിൽ എങ്കിൽ (!pat0 && pat1)
				table.at(i).തള്ളുക_back(1);
			അല്ലെങ്കിൽ
				table.at(i).തള്ളുക_back(-1);
		}

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> products, undef_products;
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			പൂർണ്ണസംഖ്യ undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < depth; i++)
			{
				std::vector<പൂർണ്ണസംഖ്യ> cmp_a, cmp_ua, cmp_b;

				വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < width; j++)
					എങ്കിൽ (table.at(i).at(j) >= 0) {
						cmp_a.തള്ളുക_back(a.at(j));
						cmp_ua.തള്ളുക_back(undef_a.at(j));
						cmp_b.തള്ളുക_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::vector<പൂർണ്ണസംഖ്യ> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::vector<പൂർണ്ണസംഖ്യ> masked_b = ez->vec_or(cmp_b, cmp_ua);

				പൂർണ്ണസംഖ്യ masked_eq = ez->vec_eq(masked_a, masked_b);
				പൂർണ്ണസംഖ്യ any_undef = ez->expression(ezSAT::OpOr, cmp_ua);

				undef_products.തള്ളുക_back(ez->AND(any_undef, masked_eq));
				products.തള്ളുക_back(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			പൂർണ്ണസംഖ്യ yy = ez->expression(ezSAT::OpOr, products);
			ez->SET(undef_y, ez->AND(ez->NOT(yy), ez->expression(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		അല്ലെങ്കിൽ
		{
			std::vector<പൂർണ്ണസംഖ്യ> products;

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < depth; i++)
			{
				std::vector<പൂർണ്ണസംഖ്യ> cmp_a, cmp_b;

				വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < width; j++)
					എങ്കിൽ (table.at(i).at(j) >= 0) {
						cmp_a.തള്ളുക_back(a.at(j));
						cmp_b.തള്ളുക_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.തള്ളുക_back(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->SET(y, ez->expression(ezSAT::OpOr, products));
		}

		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($fa))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> c = importDefSigSpec(cell->getPort(ID::C), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> x = importDefSigSpec(cell->getPort(ID::X), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<പൂർണ്ണസംഖ്യ> xx = model_undef ? ez->vec_var(x.size()) : x;

		std::vector<പൂർണ്ണസംഖ്യ> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::vector<പൂർണ്ണസംഖ്യ> t2 = ez->vec_and(a, b);
		std::vector<പൂർണ്ണസംഖ്യ> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($lcu))
	{
		std::vector<പൂർണ്ണസംഖ്യ> p = importDefSigSpec(cell->getPort(ID::P), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> g = importDefSigSpec(cell->getPort(ID::G), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(co.size()) : co;

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(co); i++)
			ez->SET(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_p = importUndefSigSpec(cell->getPort(ID::P), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_g = importUndefSigSpec(cell->getPort(ID::G), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			പൂർണ്ണസംഖ്യ undef_any_p = ez->expression(ezSAT::OpOr, undef_p);
			പൂർണ്ണസംഖ്യ undef_any_g = ez->expression(ezSAT::OpOr, undef_g);
			പൂർണ്ണസംഖ്യ undef_any_ci = ez->expression(ezSAT::OpOr, undef_ci);
			പൂർണ്ണസംഖ്യ undef_co_bit = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::vector<പൂർണ്ണസംഖ്യ> undef_co_bits(undef_co.size(), undef_co_bit);
			ez->assume(ez->vec_eq(undef_co_bits, undef_co));

			undefGating(co, yy, undef_co);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($alu))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> x = importDefSigSpec(cell->getPort(ID::X), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> bi = importDefSigSpec(cell->getPort(ID::BI), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		extendSignalWidth(a, b, y, cell);
		extendSignalWidth(a, b, x, cell);
		extendSignalWidth(a, b, co, cell);

		std::vector<പൂർണ്ണസംഖ്യ> def_y = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<പൂർണ്ണസംഖ്യ> def_x = model_undef ? ez->vec_var(x.size()) : x;
		std::vector<പൂർണ്ണസംഖ്യ> def_co = model_undef ? ez->vec_var(co.size()) : co;

		log_assert(GetSize(y) == GetSize(x));
		log_assert(GetSize(y) == GetSize(co));
		log_assert(GetSize(ci) == 1);
		log_assert(GetSize(bi) == 1);

		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(y); i++)
		{
			പൂർണ്ണസംഖ്യ s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->SET(def_x.at(i), ez->XOR(s1, s2));
			ez->SET(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->SET(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_bi = importUndefSigSpec(cell->getPort(ID::BI), timestep);

			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			extendSignalWidth(undef_a, undef_b, undef_y, cell);
			extendSignalWidth(undef_a, undef_b, undef_x, cell);
			extendSignalWidth(undef_a, undef_b, undef_co, cell);

			std::vector<പൂർണ്ണസംഖ്യ> all_inputs_undef;
			all_inputs_undef.തിരുകുക(all_inputs_undef.end(), undef_a.begin(), undef_a.end());
			all_inputs_undef.തിരുകുക(all_inputs_undef.end(), undef_b.begin(), undef_b.end());
			all_inputs_undef.തിരുകുക(all_inputs_undef.end(), undef_ci.begin(), undef_ci.end());
			all_inputs_undef.തിരുകുക(all_inputs_undef.end(), undef_bi.begin(), undef_bi.end());
			പൂർണ്ണസംഖ്യ undef_any = ez->expression(ezSAT::OpOr, all_inputs_undef);

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(undef_y); i++) {
				ez->SET(undef_y.at(i), undef_any);
				ez->SET(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->SET(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($slice))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec y = cell->getPort(ID::Y);
		ez->assume(signals_eq(a.extract(cell->parameters.at(ID::OFFSET).as_int(), y.size()), y, timestep));
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec b = cell->getPort(ID::B);
		RTLIL::SigSpec y = cell->getPort(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(signals_eq(ab, y, timestep));
		തിരികെ സത്യം;
	}

	എങ്കിൽ (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		FfData ff(nullptr, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		എങ്കിൽ (ff.has_aload || ff.has_arst || ff.has_sr)
			തിരികെ അസത്യം;

		എങ്കിൽ (timestep == 1)
		{
			initial_state.add((*sigmap)(cell->getPort(ID::Q)));
			എങ്കിൽ (model_undef && def_formal) {
				std::vector<പൂർണ്ണസംഖ്യ> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		അല്ലെങ്കിൽ
		{
			std::vector<പൂർണ്ണസംഖ്യ> d = importDefSigSpec(cell->getPort(ID::D), timestep-1);
			std::vector<പൂർണ്ണസംഖ്യ> undef_d;
			എങ്കിൽ (model_undef)
				undef_d = importUndefSigSpec(cell->getPort(ID::D), timestep-1);
			എങ്കിൽ (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				പൂർണ്ണസംഖ്യ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<പൂർണ്ണസംഖ്യ> rval = importDefSigSpec(ff.val_srst, timestep-1);
				പൂർണ്ണസംഖ്യ undef_srst = -1;
				std::vector<പൂർണ്ണസംഖ്യ> undef_rval;
				എങ്കിൽ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				എങ്കിൽ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				അല്ലെങ്കിൽ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			എങ്കിൽ (ff.has_ce) {
				പൂർണ്ണസംഖ്യ ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::vector<പൂർണ്ണസംഖ്യ> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				പൂർണ്ണസംഖ്യ undef_ce = -1;
				std::vector<പൂർണ്ണസംഖ്യ> undef_old_q;
				എങ്കിൽ (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				എങ്കിൽ (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				അല്ലെങ്കിൽ
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			എങ്കിൽ (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				പൂർണ്ണസംഖ്യ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<പൂർണ്ണസംഖ്യ> rval = importDefSigSpec(ff.val_srst, timestep-1);
				പൂർണ്ണസംഖ്യ undef_srst = -1;
				std::vector<പൂർണ്ണസംഖ്യ> undef_rval;
				എങ്കിൽ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				എങ്കിൽ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				അല്ലെങ്കിൽ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::vector<പൂർണ്ണസംഖ്യ> q = importDefSigSpec(cell->getPort(ID::Q), timestep);

			std::vector<പൂർണ്ണസംഖ്യ> qq = model_undef ? ez->vec_var(q.size()) : q;
			ez->assume(ez->vec_eq(d, qq));

			എങ്കിൽ (model_undef)
			{
				std::vector<പൂർണ്ണസംഖ്യ> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($anyconst))
	{
		എങ്കിൽ (timestep < 2) {
			എങ്കിൽ (model_undef && def_formal) {
				std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			തിരികെ സത്യം;
		}

		std::vector<പൂർണ്ണസംഖ്യ> d = importDefSigSpec(cell->getPort(ID::Y), timestep-1);
		std::vector<പൂർണ്ണസംഖ്യ> q = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<പൂർണ്ണസംഖ്യ> qq = (model_undef && !def_formal) ? ez->vec_var(q.size()) : q;
		ez->assume(ez->vec_eq(d, qq));

		എങ്കിൽ (model_undef)
		{
			std::vector<പൂർണ്ണസംഖ്യ> undef_d = importUndefSigSpec(cell->getPort(ID::Y), timestep-1);
			std::vector<പൂർണ്ണസംഖ്യ> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			എങ്കിൽ (def_formal) {
				വേണ്ടി (auto &undef_q_bit : undef_q)
					ez->SET(ez->CONST_FALSE, undef_q_bit);
			} അല്ലെങ്കിൽ {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($anyseq))
	{
		എങ്കിൽ (model_undef && def_formal) {
			std::vector<പൂർണ്ണസംഖ്യ> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			വേണ്ടി (auto &undef_q_bit : undef_q)
				ez->SET(ez->CONST_FALSE, undef_q_bit);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($barrier) && !model_barriers)
		തിരികെ സത്യം;

	എങ്കിൽ (cell->type.in(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::vector<പൂർണ്ണസംഖ്യ> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<പൂർണ്ണസംഖ്യ> yy = model_undef ? ez->vec_var(y.size()) : y;
		ez->assume(ez->vec_eq(a, yy));

		എങ്കിൽ (model_undef) {
			std::vector<പൂർണ്ണസംഖ്യ> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, അസത്യം);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($initstate))
	{
		auto താക്കോൽ = make_pair(prefix, timestep);
		എങ്കിൽ (initstates.എണ്ണം(താക്കോൽ) == 0)
			initstates[താക്കോൽ] = അസത്യം;

		std::vector<പൂർണ്ണസംഖ്യ> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		log_assert(GetSize(y) == 1);
		ez->SET(y[0], initstates[താക്കോൽ] ? ez->CONST_TRUE : ez->CONST_FALSE);

		എങ്കിൽ (model_undef) {
			std::vector<പൂർണ്ണസംഖ്യ> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			log_assert(GetSize(undef_y) == 1);
			ez->SET(undef_y[0], ez->CONST_FALSE);
		}

		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($പ്രതിജ്ഞാ))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($assume))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($scopeinfo) || cell->type == ID($നിവേശം_port))
	{
		തിരികെ സത്യം;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	തിരികെ അസത്യം;
}

നാമാകാശ Yosys {

ശൂന്യം report_missing_model(തർക്കം warn_only, RTLIL::Cell* cell)
{
	std::string s;
	എങ്കിൽ (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	അല്ലെങ്കിൽ
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	എങ്കിൽ (warn_only) {
		log_warning_noprefix("%s\n", s);
	} അല്ലെങ്കിൽ {
		log_error("%s\n", s);
	}
}

}
