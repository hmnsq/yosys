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

#समावेश "kernel/satgen.h"
#समावेश "kernel/ff.h"
#समावेश "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

bool SatGen::importCell(RTLIL::Cell *cell, पूर्णांक timestep)
{
	bool arith_undef_handled = false;
	bool is_arith_compare = cell->type.in(ID($lt), ID($le), ID($ge), ID($gt));

	अगर (model_undef && (cell->type.in(ID($add), ID($sub), ID($mul), ID($भाग), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		अगर (is_arith_compare)
			extendSignalWidth(undef_a, undef_b, cell, true);
		अन्यथा
			extendSignalWidth(undef_a, undef_b, undef_y, cell, true);

		पूर्णांक undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
		पूर्णांक undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
		पूर्णांक undef_y_bit = ez->OR(undef_any_a, undef_any_b);

		अगर (cell->type.in(ID($भाग), ID($mod), ID($divfloor), ID($modfloor))) {
			std::vector<पूर्णांक> b = importSigSpec(cell->getPort(ID::B), timestep);
			undef_y_bit = ez->OR(undef_y_bit, ez->NOT(ez->expression(ezSAT::OpOr, b)));
		}

		अगर (is_arith_compare) {
			क्रम (माप_प्रकार i = 1; i < undef_y.आकार(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));
		} अन्यथा {
			std::vector<पूर्णांक> undef_y_bits(undef_y.आकार(), undef_y_bit);
			ez->assume(ez->vec_eq(undef_y_bits, undef_y));
		}

		arith_undef_handled = true;
	}

	अगर (cell->type.in(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($add), ID($sub)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		अगर (cell->type.in(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		अगर (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		अगर (cell->type.in(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		अगर (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		अगर (cell->type.in(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		अगर (cell->type.in(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		अगर (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		अगर (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		अगर (cell->type == ID($add))
			ez->assume(ez->vec_eq(ez->vec_add(a, b), yy));
		अगर (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		अगर (model_undef && !arith_undef_handled)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, undef_y, cell, false);

			अगर (cell->type.in(ID($and), ID($_AND_), ID($_NAND_))) {
				std::vector<पूर्णांक> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<पूर्णांक> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<पूर्णांक> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा अगर (cell->type.in(ID($or), ID($_OR_), ID($_NOR_))) {
				std::vector<पूर्णांक> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<पूर्णांक> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<पूर्णांक> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा अगर (cell->type.in(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::vector<पूर्णांक> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा अगर (cell->type == ID($_ANDNOT_)) {
				std::vector<पूर्णांक> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<पूर्णांक> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<पूर्णांक> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			अन्यथा अगर (cell->type == ID($_ORNOT_)) {
				std::vector<पूर्णांक> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<पूर्णांक> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<पूर्णांक> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा
				log_abort();

			undefGating(y, yy, undef_y);
		}
		अन्यथा अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		bool aoi_mode = cell->type.in(ID($_AOI3_), ID($_AOI4_));
		bool three_mode = cell->type.in(ID($_AOI3_), ID($_OAI3_));

		पूर्णांक a = importDefSigSpec(cell->getPort(ID::A), timestep).at(0);
		पूर्णांक b = importDefSigSpec(cell->getPort(ID::B), timestep).at(0);
		पूर्णांक c = importDefSigSpec(cell->getPort(ID::C), timestep).at(0);
		पूर्णांक d = three_mode ? (aoi_mode ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->getPort(ID::D), timestep).at(0);
		पूर्णांक y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);
		पूर्णांक yy = model_undef ? ez->literal() : y;

		अगर (cell->type.in(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		अन्यथा
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		अगर (model_undef)
		{
			पूर्णांक undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep).at(0);
			पूर्णांक undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep).at(0);
			पूर्णांक undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep).at(0);
			पूर्णांक undef_d = three_mode ? ez->CONST_FALSE : importUndefSigSpec(cell->getPort(ID::D), timestep).at(0);
			पूर्णांक undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			अगर (aoi_mode)
			{
				पूर्णांक a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				पूर्णांक b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				पूर्णांक c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				पूर्णांक d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				पूर्णांक ab = ez->AND(a, b), cd = ez->AND(c, d);
				पूर्णांक undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				पूर्णांक undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				पूर्णांक ab1 = ez->AND(ab, ez->NOT(undef_ab));
				पूर्णांक cd1 = ez->AND(cd, ez->NOT(undef_cd));
				पूर्णांक yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			अन्यथा
			{
				पूर्णांक a1 = ez->AND(a, ez->NOT(undef_a));
				पूर्णांक b1 = ez->AND(b, ez->NOT(undef_b));
				पूर्णांक c1 = ez->AND(c, ez->NOT(undef_c));
				पूर्णांक d1 = ez->AND(d, ez->NOT(undef_d));

				पूर्णांक ab = ez->OR(a, b), cd = ez->OR(c, d);
				पूर्णांक undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				पूर्णांक undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				पूर्णांक ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				पूर्णांक cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				पूर्णांक yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		वापस true;
	}

	अगर (cell->type.in(ID($_NOT_), ID($not)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		अगर (model_undef) {
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, false);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type == ID($bweqx))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<पूर्णांक> bweqx = ez->vec_not(ez->vec_xor(a, b));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<पूर्णांक> both_undef = ez->vec_and(undef_a, undef_b);
			std::vector<पूर्णांक> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			क्रम (पूर्णांक yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		वापस true;
	}

	अगर (cell->type.in(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		अगर (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		अन्यथा अगर (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		अन्यथा
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<पूर्णांक> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<पूर्णांक> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::vector<पूर्णांक> yX;
			अगर (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			अन्यथा
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type == ID($bmux))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<पूर्णांक> undef_a, undef_s, undef_y;

		अगर (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		अगर (Get_आकार(s) == 0) {
			ez->vec_set(a, y);
			अगर (model_undef)
				ez->vec_set(undef_a, undef_y);
		} अन्यथा {
			क्रम (पूर्णांक i = Get_आकार(s)-1; i >= 0; i--)
			{
				std::vector<पूर्णांक> out = (i == 0) ? y : ez->vec_var(a.आकार() / 2);
				std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(out.आकार()) : out;

				std::vector<पूर्णांक> a0(a.begin(), a.begin() + a.आकार() / 2);
				std::vector<पूर्णांक> a1(a.begin() + a.आकार() / 2, a.end());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				अगर (model_undef)
				{
					std::vector<पूर्णांक> undef_out = (i == 0) ? undef_y : ez->vec_var(a.आकार() / 2);
					std::vector<पूर्णांक> undef_a0(undef_a.begin(), undef_a.begin() + a.आकार() / 2);
					std::vector<पूर्णांक> undef_a1(undef_a.begin() + a.आकार() / 2, undef_a.end());
					std::vector<पूर्णांक> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::vector<पूर्णांक> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::vector<पूर्णांक> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_out));
					undefGating(out, yy, undef_out);

					undef_a = undef_out;
				}

				a = out;
			}
		}
		वापस true;
	}

	अगर (cell->type == ID($demux))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		std::vector<पूर्णांक> undef_a, undef_s, undef_y;

		अगर (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		अगर (Get_आकार(s) == 0) {
			ez->vec_set(a, y);
			अगर (model_undef)
				ez->vec_set(undef_a, undef_y);
		} अन्यथा {
			क्रम (पूर्णांक i = 0; i < (1 << Get_आकार(s)); i++)
			{
				std::vector<पूर्णांक> ss;
				क्रम (पूर्णांक j = 0; j < Get_आकार(s); j++) {
					अगर (i & 1 << j)
						ss.धकेलो_back(s[j]);
					अन्यथा
						ss.धकेलो_back(ez->NOT(s[j]));
				}
				पूर्णांक sss = ez->expression(ezSAT::OpAnd, ss);

				क्रम (पूर्णांक j = 0; j < Get_आकार(a); j++) {
					ez->SET(ez->AND(sss, a[j]), yy.at(i * Get_आकार(a) + j));
				}

				अगर (model_undef)
				{
					पूर्णांक s0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					पूर्णांक us = ez->AND(ez->NOT(s0), ez->expression(ezSAT::OpOr, undef_s));
					क्रम (पूर्णांक j = 0; j < Get_आकार(a); j++) {
						पूर्णांक a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						पूर्णांक yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->SET(yX, undef_y.at(i * Get_आकार(a) + j));
					}
				}
			}
			अगर (model_undef)
				undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type == ID($pmux))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		std::vector<पूर्णांक> tmp = a;
		क्रम (माप_प्रकार i = 0; i < s.आकार(); i++) {
			std::vector<पूर्णांक> part_of_b(b.begin()+i*a.आकार(), b.begin()+(i+1)*a.आकार());
			tmp = ez->vec_ite(s.at(i), part_of_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			पूर्णांक all_undef = ez->CONST_FALSE;
			पूर्णांक found_active = ez->CONST_FALSE;

			std::vector<पूर्णांक> undef_tmp = undef_a;

			क्रम (माप_प्रकार i = 0; i < s.आकार(); i++)
			{
				std::vector<पूर्णांक> part_of_undef_b(undef_b.begin()+i*a.आकार(), undef_b.begin()+(i+1)*a.आकार());

				undef_tmp = ez->vec_ite(s.at(i), part_of_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), found_active));
				found_active = ez->OR(found_active, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::vector<पूर्णांक>(a.आकार(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type.in(ID($pos), ID($buf), ID($neg)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		अगर (cell->type.in(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} अन्यथा {
			std::vector<पूर्णांक> zero(a.आकार(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell);

			अगर (cell->type.in(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} अन्यथा {
				पूर्णांक undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
				std::vector<पूर्णांक> undef_y_bits(undef_y.आकार(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_bits, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		अगर (cell->type == ID($reduce_and))
			ez->SET(ez->expression(ez->OpAnd, a), yy.at(0));
		अगर (cell->type.in(ID($reduce_or), ID($reduce_bool)))
			ez->SET(ez->expression(ez->OpOr, a), yy.at(0));
		अगर (cell->type == ID($reduce_xor))
			ez->SET(ez->expression(ez->OpXor, a), yy.at(0));
		अगर (cell->type == ID($reduce_xnor))
			ez->SET(ez->NOT(ez->expression(ez->OpXor, a)), yy.at(0));
		अगर (cell->type == ID($logic_not))
			ez->SET(ez->NOT(ez->expression(ez->OpOr, a)), yy.at(0));
		क्रम (माप_प्रकार i = 1; i < y.आकार(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			पूर्णांक aX = ez->expression(ezSAT::OpOr, undef_a);

			अगर (cell->type == ID($reduce_and)) {
				पूर्णांक a0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			अन्यथा अगर (cell->type.in(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				पूर्णांक a1 = ez->expression(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			अन्यथा अगर (cell->type.in(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} अन्यथा
				log_abort();

			क्रम (माप_प्रकार i = 1; i < undef_y.आकार(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type.in(ID($logic_and), ID($logic_or)))
	{
		std::vector<पूर्णांक> vec_a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> vec_b = importDefSigSpec(cell->getPort(ID::B), timestep);

		पूर्णांक a = ez->expression(ez->OpOr, vec_a);
		पूर्णांक b = ez->expression(ez->OpOr, vec_b);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		अगर (cell->type == ID($logic_and))
			ez->SET(ez->expression(ez->OpAnd, a, b), yy.at(0));
		अन्यथा
			ez->SET(ez->expression(ez->OpOr, a, b), yy.at(0));
		क्रम (माप_प्रकार i = 1; i < y.आकार(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			पूर्णांक a0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_a), ez->expression(ezSAT::OpOr, undef_a)));
			पूर्णांक b0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_b), ez->expression(ezSAT::OpOr, undef_b)));
			पूर्णांक a1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			पूर्णांक b1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			पूर्णांक aX = ez->expression(ezSAT::OpOr, undef_a);
			पूर्णांक bX = ez->expression(ezSAT::OpOr, undef_b);

			अगर (cell->type == ID($logic_and))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			अन्यथा अगर (cell->type == ID($logic_or))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			अन्यथा
				log_abort();

			क्रम (माप_प्रकार i = 1; i < undef_y.आकार(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		bool is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, cell);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		अगर (model_undef && cell->type.in(ID($eqx), ID($nex))) {
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			extendSignalWidth(undef_a, undef_b, cell, true);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		अगर (cell->type == ID($lt))
			ez->SET(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		अगर (cell->type == ID($le))
			ez->SET(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		अगर (cell->type.in(ID($eq), ID($eqx)))
			ez->SET(ez->vec_eq(a, b), yy.at(0));
		अगर (cell->type.in(ID($ne), ID($nex)))
			ez->SET(ez->vec_ne(a, b), yy.at(0));
		अगर (cell->type == ID($ge))
			ez->SET(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		अगर (cell->type == ID($gt))
			ez->SET(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		क्रम (माप_प्रकार i = 1; i < y.आकार(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		अगर (model_undef && cell->type.in(ID($eqx), ID($nex)))
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, true);

			अगर (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			अन्यथा
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			क्रम (माप_प्रकार i = 0; i < y.आकार(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		अन्यथा अगर (model_undef && cell->type.in(ID($eq), ID($ne)))
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, true);

			पूर्णांक undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			पूर्णांक undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			पूर्णांक undef_any = ez->OR(undef_any_a, undef_any_b);

			std::vector<पूर्णांक> masked_a_bits = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::vector<पूर्णांक> masked_b_bits = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			पूर्णांक masked_ne = ez->vec_ne(masked_a_bits, masked_b_bits);
			पूर्णांक undef_y_bit = ez->AND(undef_any, ez->NOT(masked_ne));

			क्रम (माप_प्रकार i = 1; i < undef_y.आकार(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		अन्यथा
		{
			अगर (model_undef) {
				std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		वापस true;
	}

	अगर (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		पूर्णांक extend_bit = ez->CONST_FALSE;

		अगर (cell->parameters[ID::A_SIGNED].as_bool())
			extend_bit = a.back();

		जबतक (y.आकार() < a.आकार())
			y.धकेलो_back(ez->literal());
		जबतक (y.आकार() > a.आकार())
			a.धकेलो_back(extend_bit);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		std::vector<पूर्णांक> shifted_a;

		अगर (cell->type.in( ID($shl), ID($sshl)))
			shifted_a = ez->vec_shift_बायाँ(a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

		अगर (cell->type == ID($shr))
			shifted_a = ez->vec_shift_दायाँ(a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

		अगर (cell->type == ID($sshr))
			shifted_a = ez->vec_shift_दायाँ(a, b, false, cell->parameters[ID::A_SIGNED].as_bool() ? a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

		अगर (cell->type.in(ID($shift), ID($shiftx)))
			shifted_a = ez->vec_shift_दायाँ(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<पूर्णांक> undef_a_shifted;

			extend_bit = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			अगर (cell->parameters[ID::A_SIGNED].as_bool())
				extend_bit = undef_a.back();

			जबतक (undef_y.आकार() < undef_a.आकार())
				undef_y.धकेलो_back(ez->literal());
			जबतक (undef_y.आकार() > undef_a.आकार())
				undef_a.धकेलो_back(extend_bit);

			अगर (cell->type.in(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_shift_बायाँ(undef_a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

			अगर (cell->type == ID($shr))
				undef_a_shifted = ez->vec_shift_दायाँ(undef_a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

			अगर (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_shift_दायाँ(undef_a, b, false, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

			अगर (cell->type == ID($shift))
				undef_a_shifted = ez->vec_shift_दायाँ(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			अगर (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_shift_दायाँ(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			पूर्णांक undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			std::vector<पूर्णांक> undef_all_y_bits(undef_y.आकार(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_bits), undef_y));
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type == ID($mul))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		std::vector<पूर्णांक> tmp(a.आकार(), ez->CONST_FALSE);
		क्रम (पूर्णांक i = 0; i < पूर्णांक(a.आकार()); i++)
		{
			std::vector<पूर्णांक> shifted_a(a.आकार(), ez->CONST_FALSE);
			क्रम (पूर्णांक j = i; j < पूर्णांक(a.आकार()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_add(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		अगर (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type.in(ID($macc), ID($macc_v2)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::vector<पूर्णांक> tmp(Get_आकार(y), ez->CONST_FALSE);

		क्रम (स्वतः &port : macc.terms)
		{
			std::vector<पूर्णांक> in_a = importDefSigSpec(port.in_a, timestep);
			std::vector<पूर्णांक> in_b = importDefSigSpec(port.in_b, timestep);

			जबतक (Get_आकार(in_a) < Get_आकार(y))
				in_a.धकेलो_back(port.is_signed && !in_a.empty() ? in_a.back() : ez->CONST_FALSE);
			in_a.resize(Get_आकार(y));

			अगर (Get_आकार(in_b))
			{
				जबतक (Get_आकार(in_b) < Get_आकार(y))
					in_b.धकेलो_back(port.is_signed && !in_b.empty() ? in_b.back() : ez->CONST_FALSE);
				in_b.resize(Get_आकार(y));

				क्रम (पूर्णांक i = 0; i < Get_आकार(in_b); i++) {
					std::vector<पूर्णांक> shifted_a(in_a.आकार(), ez->CONST_FALSE);
					क्रम (पूर्णांक j = i; j < पूर्णांक(in_a.आकार()); j++)
						shifted_a.at(j) = in_a.at(j-i);
					अगर (port.do_subtract)
						tmp = ez->vec_ite(in_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					अन्यथा
						tmp = ez->vec_ite(in_b.at(i), ez->vec_add(tmp, shifted_a), tmp);
				}
			}
			अन्यथा
			{
				अगर (port.do_subtract)
					tmp = ez->vec_sub(tmp, in_a);
				अन्यथा
					tmp = ez->vec_add(tmp, in_a);
			}
		}

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_c;

			अगर (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			पूर्णांक undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			पूर्णांक undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			पूर्णांक undef_any_c = ez->expression(ezSAT::OpOr, undef_c);
			पूर्णांक undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::vector<पूर्णांक>(Get_आकार(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		अन्यथा
			ez->assume(ez->vec_eq(y, tmp));

		वापस true;
	}

	अगर (cell->type.in(ID($भाग), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		std::vector<पूर्णांक> a_u, b_u;
		अगर (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.back(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.back(), ez->vec_neg(b), b);
		} अन्यथा {
			a_u = a;
			b_u = b;
		}

		std::vector<पूर्णांक> chain_buf = a_u;
		std::vector<पूर्णांक> y_u(a_u.आकार(), ez->CONST_FALSE);
		क्रम (पूर्णांक i = पूर्णांक(a.आकार())-1; i >= 0; i--)
		{
			chain_buf.जोड़ो(chain_buf.end(), chain_buf.आकार(), ez->CONST_FALSE);

			std::vector<पूर्णांक> b_shl(i, ez->CONST_FALSE);
			b_shl.जोड़ो(b_shl.end(), b_u.begin(), b_u.end());
			b_shl.जोड़ो(b_shl.end(), chain_buf.आकार()-b_shl.आकार(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.आकार(), chain_buf.end());
		}

		std::vector<पूर्णांक> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.आकार());

		// modulo calculation
		std::vector<पूर्णांक> modulo_trunc;
		पूर्णांक floored_eq_trunc;
		अगर (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.back(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.back(), b.back()), ez->NOT(ez->expression(ezSAT::OpOr, modulo_trunc)));
		} अन्यथा {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		अगर (cell->type == ID($भाग)) {
			अगर (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.back(), b.back()), ez->vec_neg(y_u), y_u)));
			अन्यथा
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} अन्यथा अगर (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} अन्यथा अगर (cell->type == ID($divfloor)) {
			अगर (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.back(), b.back()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_add(y_u, ez->vec_const_unsigned(1, y_u.आकार())),
						y_u
					)),
					y_u
				)));
			अन्यथा
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} अन्यथा अगर (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_add(modulo_trunc, b))));
		}

		अगर (ignore_div_by_zero) {
			ez->assume(ez->expression(ezSAT::OpOr, b));
		} अन्यथा {
			std::vector<पूर्णांक> div_zero_परिणाम;
			अगर (cell->type.in(ID($भाग), ID($divfloor))) {
				अगर (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::vector<पूर्णांक> all_ones(y.आकार(), ez->CONST_TRUE);
					std::vector<पूर्णांक> only_first_one(y.आकार(), ez->CONST_FALSE);
					only_first_one.at(0) = ez->CONST_TRUE;
					div_zero_परिणाम = ez->vec_ite(a.back(), only_first_one, all_ones);
				} अन्यथा {
					div_zero_परिणाम.जोड़ो(div_zero_परिणाम.end(), cell->getPort(ID::A).आकार(), ez->CONST_TRUE);
					div_zero_परिणाम.जोड़ो(div_zero_परिणाम.end(), y.आकार() - div_zero_परिणाम.आकार(), ez->CONST_FALSE);
				}
			} अन्यथा अगर (cell->type.in(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				पूर्णांक copy_a_bits = min(cell->getPort(ID::A).आकार(), cell->getPort(ID::B).आकार());
				div_zero_परिणाम.जोड़ो(div_zero_परिणाम.end(), a.begin(), a.begin() + copy_a_bits);
				अगर (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_परिणाम.जोड़ो(div_zero_परिणाम.end(), y.आकार() - div_zero_परिणाम.आकार(), div_zero_परिणाम.back());
				अन्यथा
					div_zero_परिणाम.जोड़ो(div_zero_परिणाम.end(), y.आकार() - div_zero_परिणाम.आकार(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->expression(ezSAT::OpOr, b), y_tmp, div_zero_परिणाम)));
		}

		अगर (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type == ID($lut))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<पूर्णांक> lut;
		क्रम (स्वतः bit : cell->getParam(ID::LUT))
			lut.धकेलो_back(bit == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		जबतक (Get_आकार(lut) < (1 << Get_आकार(a)))
			lut.धकेलो_back(ez->CONST_FALSE);
		lut.resize(1 << Get_आकार(a));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> t(lut), u(Get_आकार(t), ez->CONST_FALSE);

			क्रम (पूर्णांक i = Get_आकार(a)-1; i >= 0; i--)
			{
				std::vector<पूर्णांक> t0(t.begin(), t.begin() + Get_आकार(t)/2);
				std::vector<पूर्णांक> t1(t.begin() + Get_आकार(t)/2, t.end());

				std::vector<पूर्णांक> u0(u.begin(), u.begin() + Get_आकार(u)/2);
				std::vector<पूर्णांक> u1(u.begin() + Get_आकार(u)/2, u.end());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_आकार(t) == 1);
			log_assert(Get_आकार(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->getPort(ID::Y), timestep), u));
		}
		अन्यथा
		{
			std::vector<पूर्णांक> t = lut;
			क्रम (पूर्णांक i = Get_आकार(a)-1; i >= 0; i--)
			{
				std::vector<पूर्णांक> t0(t.begin(), t.begin() + Get_आकार(t)/2);
				std::vector<पूर्णांक> t1(t.begin() + Get_आकार(t)/2, t.end());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_आकार(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		वापस true;
	}

	अगर (cell->type == ID($sop))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		पूर्णांक y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);

		पूर्णांक width = cell->getParam(ID::WIDTH).as_int();
		पूर्णांक depth = cell->getParam(ID::DEPTH).as_int();

		vector<State> table_raw = cell->getParam(ID::TABLE).to_bits();
		जबतक (Get_आकार(table_raw) < 2*width*depth)
			table_raw.धकेलो_back(State::S0);

		vector<vector<पूर्णांक>> table(depth);

		क्रम (पूर्णांक i = 0; i < depth; i++)
		क्रम (पूर्णांक j = 0; j < width; j++)
		{
			bool pat0 = (table_raw[2*width*i + 2*j + 0] == State::S1);
			bool pat1 = (table_raw[2*width*i + 2*j + 1] == State::S1);

			अगर (pat0 && !pat1)
				table.at(i).धकेलो_back(0);
			अन्यथा अगर (!pat0 && pat1)
				table.at(i).धकेलो_back(1);
			अन्यथा
				table.at(i).धकेलो_back(-1);
		}

		अगर (model_undef)
		{
			std::vector<पूर्णांक> products, undef_products;
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			पूर्णांक undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			क्रम (पूर्णांक i = 0; i < depth; i++)
			{
				std::vector<पूर्णांक> cmp_a, cmp_ua, cmp_b;

				क्रम (पूर्णांक j = 0; j < width; j++)
					अगर (table.at(i).at(j) >= 0) {
						cmp_a.धकेलो_back(a.at(j));
						cmp_ua.धकेलो_back(undef_a.at(j));
						cmp_b.धकेलो_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::vector<पूर्णांक> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::vector<पूर्णांक> masked_b = ez->vec_or(cmp_b, cmp_ua);

				पूर्णांक masked_eq = ez->vec_eq(masked_a, masked_b);
				पूर्णांक any_undef = ez->expression(ezSAT::OpOr, cmp_ua);

				undef_products.धकेलो_back(ez->AND(any_undef, masked_eq));
				products.धकेलो_back(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			पूर्णांक yy = ez->expression(ezSAT::OpOr, products);
			ez->SET(undef_y, ez->AND(ez->NOT(yy), ez->expression(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		अन्यथा
		{
			std::vector<पूर्णांक> products;

			क्रम (पूर्णांक i = 0; i < depth; i++)
			{
				std::vector<पूर्णांक> cmp_a, cmp_b;

				क्रम (पूर्णांक j = 0; j < width; j++)
					अगर (table.at(i).at(j) >= 0) {
						cmp_a.धकेलो_back(a.at(j));
						cmp_b.धकेलो_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.धकेलो_back(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->SET(y, ez->expression(ezSAT::OpOr, products));
		}

		वापस true;
	}

	अगर (cell->type == ID($fa))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> c = importDefSigSpec(cell->getPort(ID::C), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<पूर्णांक> x = importDefSigSpec(cell->getPort(ID::X), timestep);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		std::vector<पूर्णांक> xx = model_undef ? ez->vec_var(x.आकार()) : x;

		std::vector<पूर्णांक> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::vector<पूर्णांक> t2 = ez->vec_and(a, b);
		std::vector<पूर्णांक> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<पूर्णांक> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		वापस true;
	}

	अगर (cell->type == ID($lcu))
	{
		std::vector<पूर्णांक> p = importDefSigSpec(cell->getPort(ID::P), timestep);
		std::vector<पूर्णांक> g = importDefSigSpec(cell->getPort(ID::G), timestep);
		std::vector<पूर्णांक> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<पूर्णांक> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(co.आकार()) : co;

		क्रम (पूर्णांक i = 0; i < Get_आकार(co); i++)
			ez->SET(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_p = importUndefSigSpec(cell->getPort(ID::P), timestep);
			std::vector<पूर्णांक> undef_g = importUndefSigSpec(cell->getPort(ID::G), timestep);
			std::vector<पूर्णांक> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<पूर्णांक> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			पूर्णांक undef_any_p = ez->expression(ezSAT::OpOr, undef_p);
			पूर्णांक undef_any_g = ez->expression(ezSAT::OpOr, undef_g);
			पूर्णांक undef_any_ci = ez->expression(ezSAT::OpOr, undef_ci);
			पूर्णांक undef_co_bit = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::vector<पूर्णांक> undef_co_bits(undef_co.आकार(), undef_co_bit);
			ez->assume(ez->vec_eq(undef_co_bits, undef_co));

			undefGating(co, yy, undef_co);
		}
		वापस true;
	}

	अगर (cell->type == ID($alu))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<पूर्णांक> x = importDefSigSpec(cell->getPort(ID::X), timestep);
		std::vector<पूर्णांक> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<पूर्णांक> bi = importDefSigSpec(cell->getPort(ID::BI), timestep);
		std::vector<पूर्णांक> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		extendSignalWidth(a, b, y, cell);
		extendSignalWidth(a, b, x, cell);
		extendSignalWidth(a, b, co, cell);

		std::vector<पूर्णांक> def_y = model_undef ? ez->vec_var(y.आकार()) : y;
		std::vector<पूर्णांक> def_x = model_undef ? ez->vec_var(x.आकार()) : x;
		std::vector<पूर्णांक> def_co = model_undef ? ez->vec_var(co.आकार()) : co;

		log_assert(Get_आकार(y) == Get_आकार(x));
		log_assert(Get_आकार(y) == Get_आकार(co));
		log_assert(Get_आकार(ci) == 1);
		log_assert(Get_आकार(bi) == 1);

		क्रम (पूर्णांक i = 0; i < Get_आकार(y); i++)
		{
			पूर्णांक s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->SET(def_x.at(i), ez->XOR(s1, s2));
			ez->SET(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->SET(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<पूर्णांक> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<पूर्णांक> undef_bi = importUndefSigSpec(cell->getPort(ID::BI), timestep);

			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<पूर्णांक> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);
			std::vector<पूर्णांक> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			extendSignalWidth(undef_a, undef_b, undef_y, cell);
			extendSignalWidth(undef_a, undef_b, undef_x, cell);
			extendSignalWidth(undef_a, undef_b, undef_co, cell);

			std::vector<पूर्णांक> all_inputs_undef;
			all_inputs_undef.जोड़ो(all_inputs_undef.end(), undef_a.begin(), undef_a.end());
			all_inputs_undef.जोड़ो(all_inputs_undef.end(), undef_b.begin(), undef_b.end());
			all_inputs_undef.जोड़ो(all_inputs_undef.end(), undef_ci.begin(), undef_ci.end());
			all_inputs_undef.जोड़ो(all_inputs_undef.end(), undef_bi.begin(), undef_bi.end());
			पूर्णांक undef_any = ez->expression(ezSAT::OpOr, all_inputs_undef);

			क्रम (पूर्णांक i = 0; i < Get_आकार(undef_y); i++) {
				ez->SET(undef_y.at(i), undef_any);
				ez->SET(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->SET(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		वापस true;
	}

	अगर (cell->type == ID($slice))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec y = cell->getPort(ID::Y);
		ez->assume(signals_eq(a.extract(cell->parameters.at(ID::OFFSET).as_int(), y.आकार()), y, timestep));
		वापस true;
	}

	अगर (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec b = cell->getPort(ID::B);
		RTLIL::SigSpec y = cell->getPort(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(signals_eq(ab, y, timestep));
		वापस true;
	}

	अगर (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_तथ्य ff(nullptr, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		अगर (ff.has_aload || ff.has_arst || ff.has_sr)
			वापस false;

		अगर (timestep == 1)
		{
			initial_state.add((*sigmap)(cell->getPort(ID::Q)));
			अगर (model_undef && def_formal) {
				std::vector<पूर्णांक> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		अन्यथा
		{
			std::vector<पूर्णांक> d = importDefSigSpec(cell->getPort(ID::D), timestep-1);
			std::vector<पूर्णांक> undef_d;
			अगर (model_undef)
				undef_d = importUndefSigSpec(cell->getPort(ID::D), timestep-1);
			अगर (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				पूर्णांक srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<पूर्णांक> rval = importDefSigSpec(ff.val_srst, timestep-1);
				पूर्णांक undef_srst = -1;
				std::vector<पूर्णांक> undef_rval;
				अगर (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				अगर (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				अन्यथा
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			अगर (ff.has_ce) {
				पूर्णांक ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::vector<पूर्णांक> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				पूर्णांक undef_ce = -1;
				std::vector<पूर्णांक> undef_old_q;
				अगर (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				अगर (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				अन्यथा
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			अगर (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				पूर्णांक srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<पूर्णांक> rval = importDefSigSpec(ff.val_srst, timestep-1);
				पूर्णांक undef_srst = -1;
				std::vector<पूर्णांक> undef_rval;
				अगर (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				अगर (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				अन्यथा
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::vector<पूर्णांक> q = importDefSigSpec(cell->getPort(ID::Q), timestep);

			std::vector<पूर्णांक> qq = model_undef ? ez->vec_var(q.आकार()) : q;
			ez->assume(ez->vec_eq(d, qq));

			अगर (model_undef)
			{
				std::vector<पूर्णांक> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		वापस true;
	}

	अगर (cell->type == ID($anyconst))
	{
		अगर (timestep < 2) {
			अगर (model_undef && def_formal) {
				std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			वापस true;
		}

		std::vector<पूर्णांक> d = importDefSigSpec(cell->getPort(ID::Y), timestep-1);
		std::vector<पूर्णांक> q = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<पूर्णांक> qq = (model_undef && !def_formal) ? ez->vec_var(q.आकार()) : q;
		ez->assume(ez->vec_eq(d, qq));

		अगर (model_undef)
		{
			std::vector<पूर्णांक> undef_d = importUndefSigSpec(cell->getPort(ID::Y), timestep-1);
			std::vector<पूर्णांक> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			अगर (def_formal) {
				क्रम (स्वतः &undef_q_bit : undef_q)
					ez->SET(ez->CONST_FALSE, undef_q_bit);
			} अन्यथा {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		वापस true;
	}

	अगर (cell->type == ID($anyseq))
	{
		अगर (model_undef && def_formal) {
			std::vector<पूर्णांक> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			क्रम (स्वतः &undef_q_bit : undef_q)
				ez->SET(ez->CONST_FALSE, undef_q_bit);
		}
		वापस true;
	}

	अगर (cell->type == ID($barrier) && !model_barriers)
		वापस true;

	अगर (cell->type.in(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::vector<पूर्णांक> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<पूर्णांक> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		ez->assume(ez->vec_eq(a, yy));

		अगर (model_undef) {
			std::vector<पूर्णांक> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, false);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		वापस true;
	}

	अगर (cell->type == ID($initstate))
	{
		स्वतः कुंजी = make_pair(prefix, timestep);
		अगर (initstates.गिनती(कुंजी) == 0)
			initstates[कुंजी] = false;

		std::vector<पूर्णांक> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		log_assert(Get_आकार(y) == 1);
		ez->SET(y[0], initstates[कुंजी] ? ez->CONST_TRUE : ez->CONST_FALSE);

		अगर (model_undef) {
			std::vector<पूर्णांक> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			log_assert(Get_आकार(undef_y) == 1);
			ez->SET(undef_y[0], ez->CONST_FALSE);
		}

		वापस true;
	}

	अगर (cell->type == ID($निश्चित))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		वापस true;
	}

	अगर (cell->type == ID($assume))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		वापस true;
	}

	अगर (cell->type == ID($scopeinfo) || cell->type == ID($निवेश_port))
	{
		वापस true;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	वापस false;
}

namespace Yosys {

व्योम report_missing_model(bool warn_only, RTLIL::Cell* cell)
{
	std::string s;
	अगर (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	अन्यथा
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	अगर (warn_only) {
		log_warning_noprefix("%s\n", s);
	} अन्यथा {
		log_error("%s\n", s);
	}
}

}
