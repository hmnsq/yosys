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

#অন্তর্ভাব "kernel/satgen.h"
#অন্তর্ভাব "kernel/ff.h"
#অন্তর্ভাব "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

যৌক্তিক SatGen::importCell(RTLIL::Cell *cell, পূর্ণসংখ্যা timestep)
{
	যৌক্তিক arith_undef_handled = মিথ্যা;
	যৌক্তিক is_arith_compare = cell->type.in(ID($lt), ID($le), ID($ge), ID($gt));

	যদি (model_undef && (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		যদি (is_arith_compare)
			extendSignalWidth(undef_a, undef_b, cell, সত্য);
		অন্যথা
			extendSignalWidth(undef_a, undef_b, undef_y, cell, সত্য);

		পূর্ণসংখ্যা undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
		পূর্ণসংখ্যা undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
		পূর্ণসংখ্যা undef_y_bit = ez->OR(undef_any_a, undef_any_b);

		যদি (cell->type.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::vector<পূর্ণসংখ্যা> b = importSigSpec(cell->getPort(ID::B), timestep);
			undef_y_bit = ez->OR(undef_y_bit, ez->NOT(ez->expression(ezSAT::OpOr, b)));
		}

		যদি (is_arith_compare) {
			জন্য (আকার_t i = 1; i < undef_y.আকার_2(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));
		} অন্যথা {
			std::vector<পূর্ণসংখ্যা> undef_y_bits(undef_y.আকার_2(), undef_y_bit);
			ez->assume(ez->vec_eq(undef_y_bits, undef_y));
		}

		arith_undef_handled = সত্য;
	}

	যদি (cell->type.in(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($add), ID($sub)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		যদি (cell->type.in(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		যদি (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		যদি (cell->type.in(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		যদি (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		যদি (cell->type.in(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		যদি (cell->type.in(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		যদি (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		যদি (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		যদি (cell->type == ID($add))
			ez->assume(ez->vec_eq(ez->vec_add(a, b), yy));
		যদি (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		যদি (model_undef && !arith_undef_handled)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, undef_y, cell, মিথ্যা);

			যদি (cell->type.in(ID($and), ID($_AND_), ID($_NAND_))) {
				std::vector<পূর্ণসংখ্যা> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<পূর্ণসংখ্যা> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<পূর্ণসংখ্যা> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			অন্যথা যদি (cell->type.in(ID($or), ID($_OR_), ID($_NOR_))) {
				std::vector<পূর্ণসংখ্যা> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<পূর্ণসংখ্যা> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<পূর্ণসংখ্যা> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			অন্যথা যদি (cell->type.in(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::vector<পূর্ণসংখ্যা> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			অন্যথা যদি (cell->type == ID($_ANDNOT_)) {
				std::vector<পূর্ণসংখ্যা> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<পূর্ণসংখ্যা> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<পূর্ণসংখ্যা> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			অন্যথা যদি (cell->type == ID($_ORNOT_)) {
				std::vector<পূর্ণসংখ্যা> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<পূর্ণসংখ্যা> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<পূর্ণসংখ্যা> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			অন্যথা
				log_abort();

			undefGating(y, yy, undef_y);
		}
		অন্যথা যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		যৌক্তিক aoi_mode = cell->type.in(ID($_AOI3_), ID($_AOI4_));
		যৌক্তিক three_mode = cell->type.in(ID($_AOI3_), ID($_OAI3_));

		পূর্ণসংখ্যা a = importDefSigSpec(cell->getPort(ID::A), timestep).at(0);
		পূর্ণসংখ্যা b = importDefSigSpec(cell->getPort(ID::B), timestep).at(0);
		পূর্ণসংখ্যা c = importDefSigSpec(cell->getPort(ID::C), timestep).at(0);
		পূর্ণসংখ্যা d = three_mode ? (aoi_mode ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->getPort(ID::D), timestep).at(0);
		পূর্ণসংখ্যা y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);
		পূর্ণসংখ্যা yy = model_undef ? ez->literal() : y;

		যদি (cell->type.in(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		অন্যথা
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		যদি (model_undef)
		{
			পূর্ণসংখ্যা undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep).at(0);
			পূর্ণসংখ্যা undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep).at(0);
			পূর্ণসংখ্যা undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep).at(0);
			পূর্ণসংখ্যা undef_d = three_mode ? ez->CONST_FALSE : importUndefSigSpec(cell->getPort(ID::D), timestep).at(0);
			পূর্ণসংখ্যা undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			যদি (aoi_mode)
			{
				পূর্ণসংখ্যা a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				পূর্ণসংখ্যা b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				পূর্ণসংখ্যা c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				পূর্ণসংখ্যা d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				পূর্ণসংখ্যা ab = ez->AND(a, b), cd = ez->AND(c, d);
				পূর্ণসংখ্যা undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				পূর্ণসংখ্যা undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				পূর্ণসংখ্যা ab1 = ez->AND(ab, ez->NOT(undef_ab));
				পূর্ণসংখ্যা cd1 = ez->AND(cd, ez->NOT(undef_cd));
				পূর্ণসংখ্যা yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			অন্যথা
			{
				পূর্ণসংখ্যা a1 = ez->AND(a, ez->NOT(undef_a));
				পূর্ণসংখ্যা b1 = ez->AND(b, ez->NOT(undef_b));
				পূর্ণসংখ্যা c1 = ez->AND(c, ez->NOT(undef_c));
				পূর্ণসংখ্যা d1 = ez->AND(d, ez->NOT(undef_d));

				পূর্ণসংখ্যা ab = ez->OR(a, b), cd = ez->OR(c, d);
				পূর্ণসংখ্যা undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				পূর্ণসংখ্যা undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				পূর্ণসংখ্যা ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				পূর্ণসংখ্যা cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				পূর্ণসংখ্যা yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($_NOT_), ID($not)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		যদি (model_undef) {
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, মিথ্যা);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($bweqx))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<পূর্ণসংখ্যা> bweqx = ez->vec_not(ez->vec_xor(a, b));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<পূর্ণসংখ্যা> both_undef = ez->vec_and(undef_a, undef_b);
			std::vector<পূর্ণসংখ্যা> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			জন্য (পূর্ণসংখ্যা yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;
		যদি (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		অন্যথা যদি (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		অন্যথা
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<পূর্ণসংখ্যা> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<পূর্ণসংখ্যা> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::vector<পূর্ণসংখ্যা> yX;
			যদি (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			অন্যথা
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($bmux))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<পূর্ণসংখ্যা> undef_a, undef_s, undef_y;

		যদি (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		যদি (Get_আকার(s) == 0) {
			ez->vec_set(a, y);
			যদি (model_undef)
				ez->vec_set(undef_a, undef_y);
		} অন্যথা {
			জন্য (পূর্ণসংখ্যা i = Get_আকার(s)-1; i >= 0; i--)
			{
				std::vector<পূর্ণসংখ্যা> out = (i == 0) ? y : ez->vec_var(a.আকার_2() / 2);
				std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(out.আকার_2()) : out;

				std::vector<পূর্ণসংখ্যা> a0(a.begin(), a.begin() + a.আকার_2() / 2);
				std::vector<পূর্ণসংখ্যা> a1(a.begin() + a.আকার_2() / 2, a.end());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				যদি (model_undef)
				{
					std::vector<পূর্ণসংখ্যা> undef_out = (i == 0) ? undef_y : ez->vec_var(a.আকার_2() / 2);
					std::vector<পূর্ণসংখ্যা> undef_a0(undef_a.begin(), undef_a.begin() + a.আকার_2() / 2);
					std::vector<পূর্ণসংখ্যা> undef_a1(undef_a.begin() + a.আকার_2() / 2, undef_a.end());
					std::vector<পূর্ণসংখ্যা> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::vector<পূর্ণসংখ্যা> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::vector<পূর্ণসংখ্যা> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_out));
					undefGating(out, yy, undef_out);

					undef_a = undef_out;
				}

				a = out;
			}
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($demux))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;
		std::vector<পূর্ণসংখ্যা> undef_a, undef_s, undef_y;

		যদি (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		যদি (Get_আকার(s) == 0) {
			ez->vec_set(a, y);
			যদি (model_undef)
				ez->vec_set(undef_a, undef_y);
		} অন্যথা {
			জন্য (পূর্ণসংখ্যা i = 0; i < (1 << Get_আকার(s)); i++)
			{
				std::vector<পূর্ণসংখ্যা> ss;
				জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(s); j++) {
					যদি (i & 1 << j)
						ss.ঠেলো_back(s[j]);
					অন্যথা
						ss.ঠেলো_back(ez->NOT(s[j]));
				}
				পূর্ণসংখ্যা sss = ez->expression(ezSAT::OpAnd, ss);

				জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(a); j++) {
					ez->SET(ez->AND(sss, a[j]), yy.at(i * Get_আকার(a) + j));
				}

				যদি (model_undef)
				{
					পূর্ণসংখ্যা s0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					পূর্ণসংখ্যা us = ez->AND(ez->NOT(s0), ez->expression(ezSAT::OpOr, undef_s));
					জন্য (পূর্ণসংখ্যা j = 0; j < Get_আকার(a); j++) {
						পূর্ণসংখ্যা a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						পূর্ণসংখ্যা yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->SET(yX, undef_y.at(i * Get_আকার(a) + j));
					}
				}
			}
			যদি (model_undef)
				undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($pmux))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		std::vector<পূর্ণসংখ্যা> tmp = a;
		জন্য (আকার_t i = 0; i < s.আকার_2(); i++) {
			std::vector<পূর্ণসংখ্যা> part_of_b(b.begin()+i*a.আকার_2(), b.begin()+(i+1)*a.আকার_2());
			tmp = ez->vec_ite(s.at(i), part_of_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			পূর্ণসংখ্যা all_undef = ez->CONST_FALSE;
			পূর্ণসংখ্যা found_active = ez->CONST_FALSE;

			std::vector<পূর্ণসংখ্যা> undef_tmp = undef_a;

			জন্য (আকার_t i = 0; i < s.আকার_2(); i++)
			{
				std::vector<পূর্ণসংখ্যা> part_of_undef_b(undef_b.begin()+i*a.আকার_2(), undef_b.begin()+(i+1)*a.আকার_2());

				undef_tmp = ez->vec_ite(s.at(i), part_of_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), found_active));
				found_active = ez->OR(found_active, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::vector<পূর্ণসংখ্যা>(a.আকার_2(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($pos), ID($buf), ID($neg)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		যদি (cell->type.in(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} অন্যথা {
			std::vector<পূর্ণসংখ্যা> zero(a.আকার_2(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell);

			যদি (cell->type.in(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} অন্যথা {
				পূর্ণসংখ্যা undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
				std::vector<পূর্ণসংখ্যা> undef_y_bits(undef_y.আকার_2(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_bits, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		যদি (cell->type == ID($reduce_and))
			ez->SET(ez->expression(ez->OpAnd, a), yy.at(0));
		যদি (cell->type.in(ID($reduce_or), ID($reduce_bool)))
			ez->SET(ez->expression(ez->OpOr, a), yy.at(0));
		যদি (cell->type == ID($reduce_xor))
			ez->SET(ez->expression(ez->OpXor, a), yy.at(0));
		যদি (cell->type == ID($reduce_xnor))
			ez->SET(ez->NOT(ez->expression(ez->OpXor, a)), yy.at(0));
		যদি (cell->type == ID($logic_not))
			ez->SET(ez->NOT(ez->expression(ez->OpOr, a)), yy.at(0));
		জন্য (আকার_t i = 1; i < y.আকার_2(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			পূর্ণসংখ্যা aX = ez->expression(ezSAT::OpOr, undef_a);

			যদি (cell->type == ID($reduce_and)) {
				পূর্ণসংখ্যা a0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			অন্যথা যদি (cell->type.in(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				পূর্ণসংখ্যা a1 = ez->expression(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			অন্যথা যদি (cell->type.in(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} অন্যথা
				log_abort();

			জন্য (আকার_t i = 1; i < undef_y.আকার_2(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($logic_and), ID($logic_or)))
	{
		std::vector<পূর্ণসংখ্যা> vec_a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> vec_b = importDefSigSpec(cell->getPort(ID::B), timestep);

		পূর্ণসংখ্যা a = ez->expression(ez->OpOr, vec_a);
		পূর্ণসংখ্যা b = ez->expression(ez->OpOr, vec_b);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		যদি (cell->type == ID($logic_and))
			ez->SET(ez->expression(ez->OpAnd, a, b), yy.at(0));
		অন্যথা
			ez->SET(ez->expression(ez->OpOr, a, b), yy.at(0));
		জন্য (আকার_t i = 1; i < y.আকার_2(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			পূর্ণসংখ্যা a0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_a), ez->expression(ezSAT::OpOr, undef_a)));
			পূর্ণসংখ্যা b0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_b), ez->expression(ezSAT::OpOr, undef_b)));
			পূর্ণসংখ্যা a1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			পূর্ণসংখ্যা b1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			পূর্ণসংখ্যা aX = ez->expression(ezSAT::OpOr, undef_a);
			পূর্ণসংখ্যা bX = ez->expression(ezSAT::OpOr, undef_b);

			যদি (cell->type == ID($logic_and))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			অন্যথা যদি (cell->type == ID($logic_or))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			অন্যথা
				log_abort();

			জন্য (আকার_t i = 1; i < undef_y.আকার_2(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		যৌক্তিক is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, cell);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		যদি (model_undef && cell->type.in(ID($eqx), ID($nex))) {
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			extendSignalWidth(undef_a, undef_b, cell, সত্য);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		যদি (cell->type == ID($lt))
			ez->SET(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		যদি (cell->type == ID($le))
			ez->SET(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		যদি (cell->type.in(ID($eq), ID($eqx)))
			ez->SET(ez->vec_eq(a, b), yy.at(0));
		যদি (cell->type.in(ID($ne), ID($nex)))
			ez->SET(ez->vec_ne(a, b), yy.at(0));
		যদি (cell->type == ID($ge))
			ez->SET(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		যদি (cell->type == ID($gt))
			ez->SET(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		জন্য (আকার_t i = 1; i < y.আকার_2(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		যদি (model_undef && cell->type.in(ID($eqx), ID($nex)))
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, সত্য);

			যদি (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			অন্যথা
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			জন্য (আকার_t i = 0; i < y.আকার_2(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		অন্যথা যদি (model_undef && cell->type.in(ID($eq), ID($ne)))
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, সত্য);

			পূর্ণসংখ্যা undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			পূর্ণসংখ্যা undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			পূর্ণসংখ্যা undef_any = ez->OR(undef_any_a, undef_any_b);

			std::vector<পূর্ণসংখ্যা> masked_a_bits = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::vector<পূর্ণসংখ্যা> masked_b_bits = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			পূর্ণসংখ্যা masked_ne = ez->vec_ne(masked_a_bits, masked_b_bits);
			পূর্ণসংখ্যা undef_y_bit = ez->AND(undef_any, ez->NOT(masked_ne));

			জন্য (আকার_t i = 1; i < undef_y.আকার_2(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		অন্যথা
		{
			যদি (model_undef) {
				std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		পূর্ণসংখ্যা extend_bit = ez->CONST_FALSE;

		যদি (cell->parameters[ID::A_SIGNED].as_bool())
			extend_bit = a.back();

		যখন (y.আকার_2() < a.আকার_2())
			y.ঠেলো_back(ez->literal());
		যখন (y.আকার_2() > a.আকার_2())
			a.ঠেলো_back(extend_bit);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;
		std::vector<পূর্ণসংখ্যা> shifted_a;

		যদি (cell->type.in( ID($shl), ID($sshl)))
			shifted_a = ez->vec_shift_বাম(a, b, মিথ্যা, ez->CONST_FALSE, ez->CONST_FALSE);

		যদি (cell->type == ID($shr))
			shifted_a = ez->vec_shift_ডান(a, b, মিথ্যা, ez->CONST_FALSE, ez->CONST_FALSE);

		যদি (cell->type == ID($sshr))
			shifted_a = ez->vec_shift_ডান(a, b, মিথ্যা, cell->parameters[ID::A_SIGNED].as_bool() ? a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

		যদি (cell->type.in(ID($shift), ID($shiftx)))
			shifted_a = ez->vec_shift_ডান(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<পূর্ণসংখ্যা> undef_a_shifted;

			extend_bit = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			যদি (cell->parameters[ID::A_SIGNED].as_bool())
				extend_bit = undef_a.back();

			যখন (undef_y.আকার_2() < undef_a.আকার_2())
				undef_y.ঠেলো_back(ez->literal());
			যখন (undef_y.আকার_2() > undef_a.আকার_2())
				undef_a.ঠেলো_back(extend_bit);

			যদি (cell->type.in(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_shift_বাম(undef_a, b, মিথ্যা, ez->CONST_FALSE, ez->CONST_FALSE);

			যদি (cell->type == ID($shr))
				undef_a_shifted = ez->vec_shift_ডান(undef_a, b, মিথ্যা, ez->CONST_FALSE, ez->CONST_FALSE);

			যদি (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_shift_ডান(undef_a, b, মিথ্যা, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

			যদি (cell->type == ID($shift))
				undef_a_shifted = ez->vec_shift_ডান(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			যদি (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_shift_ডান(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			পূর্ণসংখ্যা undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			std::vector<পূর্ণসংখ্যা> undef_all_y_bits(undef_y.আকার_2(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_bits), undef_y));
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($mul))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		std::vector<পূর্ণসংখ্যা> tmp(a.আকার_2(), ez->CONST_FALSE);
		জন্য (পূর্ণসংখ্যা i = 0; i < পূর্ণসংখ্যা(a.আকার_2()); i++)
		{
			std::vector<পূর্ণসংখ্যা> shifted_a(a.আকার_2(), ez->CONST_FALSE);
			জন্য (পূর্ণসংখ্যা j = i; j < পূর্ণসংখ্যা(a.আকার_2()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_add(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		যদি (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($macc), ID($macc_v2)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::vector<পূর্ণসংখ্যা> tmp(Get_আকার(y), ez->CONST_FALSE);

		জন্য (auto &port : macc.terms)
		{
			std::vector<পূর্ণসংখ্যা> in_a = importDefSigSpec(port.in_a, timestep);
			std::vector<পূর্ণসংখ্যা> in_b = importDefSigSpec(port.in_b, timestep);

			যখন (Get_আকার(in_a) < Get_আকার(y))
				in_a.ঠেলো_back(port.is_signed && !in_a.empty() ? in_a.back() : ez->CONST_FALSE);
			in_a.resize(Get_আকার(y));

			যদি (Get_আকার(in_b))
			{
				যখন (Get_আকার(in_b) < Get_আকার(y))
					in_b.ঠেলো_back(port.is_signed && !in_b.empty() ? in_b.back() : ez->CONST_FALSE);
				in_b.resize(Get_আকার(y));

				জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(in_b); i++) {
					std::vector<পূর্ণসংখ্যা> shifted_a(in_a.আকার_2(), ez->CONST_FALSE);
					জন্য (পূর্ণসংখ্যা j = i; j < পূর্ণসংখ্যা(in_a.আকার_2()); j++)
						shifted_a.at(j) = in_a.at(j-i);
					যদি (port.do_subtract)
						tmp = ez->vec_ite(in_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					অন্যথা
						tmp = ez->vec_ite(in_b.at(i), ez->vec_add(tmp, shifted_a), tmp);
				}
			}
			অন্যথা
			{
				যদি (port.do_subtract)
					tmp = ez->vec_sub(tmp, in_a);
				অন্যথা
					tmp = ez->vec_add(tmp, in_a);
			}
		}

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_c;

			যদি (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			পূর্ণসংখ্যা undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			পূর্ণসংখ্যা undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			পূর্ণসংখ্যা undef_any_c = ez->expression(ezSAT::OpOr, undef_c);
			পূর্ণসংখ্যা undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::vector<পূর্ণসংখ্যা>(Get_আকার(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		অন্যথা
			ez->assume(ez->vec_eq(y, tmp));

		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;

		std::vector<পূর্ণসংখ্যা> a_u, b_u;
		যদি (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.back(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.back(), ez->vec_neg(b), b);
		} অন্যথা {
			a_u = a;
			b_u = b;
		}

		std::vector<পূর্ণসংখ্যা> chain_buf = a_u;
		std::vector<পূর্ণসংখ্যা> y_u(a_u.আকার_2(), ez->CONST_FALSE);
		জন্য (পূর্ণসংখ্যা i = পূর্ণসংখ্যা(a.আকার_2())-1; i >= 0; i--)
		{
			chain_buf.ঢোকাও(chain_buf.end(), chain_buf.আকার_2(), ez->CONST_FALSE);

			std::vector<পূর্ণসংখ্যা> b_shl(i, ez->CONST_FALSE);
			b_shl.ঢোকাও(b_shl.end(), b_u.begin(), b_u.end());
			b_shl.ঢোকাও(b_shl.end(), chain_buf.আকার_2()-b_shl.আকার_2(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.আকার_2(), chain_buf.end());
		}

		std::vector<পূর্ণসংখ্যা> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.আকার_2());

		// modulo calculation
		std::vector<পূর্ণসংখ্যা> modulo_trunc;
		পূর্ণসংখ্যা floored_eq_trunc;
		যদি (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.back(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.back(), b.back()), ez->NOT(ez->expression(ezSAT::OpOr, modulo_trunc)));
		} অন্যথা {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		যদি (cell->type == ID($div)) {
			যদি (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.back(), b.back()), ez->vec_neg(y_u), y_u)));
			অন্যথা
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} অন্যথা যদি (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} অন্যথা যদি (cell->type == ID($divfloor)) {
			যদি (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.back(), b.back()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_add(y_u, ez->vec_const_unsigned(1, y_u.আকার_2())),
						y_u
					)),
					y_u
				)));
			অন্যথা
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} অন্যথা যদি (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_add(modulo_trunc, b))));
		}

		যদি (ignore_div_by_zero) {
			ez->assume(ez->expression(ezSAT::OpOr, b));
		} অন্যথা {
			std::vector<পূর্ণসংখ্যা> div_zero_ফলাফল;
			যদি (cell->type.in(ID($div), ID($divfloor))) {
				যদি (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::vector<পূর্ণসংখ্যা> all_ones(y.আকার_2(), ez->CONST_TRUE);
					std::vector<পূর্ণসংখ্যা> only_first_one(y.আকার_2(), ez->CONST_FALSE);
					only_first_one.at(0) = ez->CONST_TRUE;
					div_zero_ফলাফল = ez->vec_ite(a.back(), only_first_one, all_ones);
				} অন্যথা {
					div_zero_ফলাফল.ঢোকাও(div_zero_ফলাফল.end(), cell->getPort(ID::A).আকার_2(), ez->CONST_TRUE);
					div_zero_ফলাফল.ঢোকাও(div_zero_ফলাফল.end(), y.আকার_2() - div_zero_ফলাফল.আকার_2(), ez->CONST_FALSE);
				}
			} অন্যথা যদি (cell->type.in(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				পূর্ণসংখ্যা copy_a_bits = min(cell->getPort(ID::A).আকার_2(), cell->getPort(ID::B).আকার_2());
				div_zero_ফলাফল.ঢোকাও(div_zero_ফলাফল.end(), a.begin(), a.begin() + copy_a_bits);
				যদি (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_ফলাফল.ঢোকাও(div_zero_ফলাফল.end(), y.আকার_2() - div_zero_ফলাফল.আকার_2(), div_zero_ফলাফল.back());
				অন্যথা
					div_zero_ফলাফল.ঢোকাও(div_zero_ফলাফল.end(), y.আকার_2() - div_zero_ফলাফল.আকার_2(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->expression(ezSAT::OpOr, b), y_tmp, div_zero_ফলাফল)));
		}

		যদি (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($lut))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<পূর্ণসংখ্যা> lut;
		জন্য (auto bit : cell->getParam(ID::LUT))
			lut.ঠেলো_back(bit == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		যখন (Get_আকার(lut) < (1 << Get_আকার(a)))
			lut.ঠেলো_back(ez->CONST_FALSE);
		lut.resize(1 << Get_আকার(a));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> t(lut), u(Get_আকার(t), ez->CONST_FALSE);

			জন্য (পূর্ণসংখ্যা i = Get_আকার(a)-1; i >= 0; i--)
			{
				std::vector<পূর্ণসংখ্যা> t0(t.begin(), t.begin() + Get_আকার(t)/2);
				std::vector<পূর্ণসংখ্যা> t1(t.begin() + Get_আকার(t)/2, t.end());

				std::vector<পূর্ণসংখ্যা> u0(u.begin(), u.begin() + Get_আকার(u)/2);
				std::vector<পূর্ণসংখ্যা> u1(u.begin() + Get_আকার(u)/2, u.end());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_আকার(t) == 1);
			log_assert(Get_আকার(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->getPort(ID::Y), timestep), u));
		}
		অন্যথা
		{
			std::vector<পূর্ণসংখ্যা> t = lut;
			জন্য (পূর্ণসংখ্যা i = Get_আকার(a)-1; i >= 0; i--)
			{
				std::vector<পূর্ণসংখ্যা> t0(t.begin(), t.begin() + Get_আকার(t)/2);
				std::vector<পূর্ণসংখ্যা> t1(t.begin() + Get_আকার(t)/2, t.end());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_আকার(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($sop))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		পূর্ণসংখ্যা y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);

		পূর্ণসংখ্যা width = cell->getParam(ID::WIDTH).as_int();
		পূর্ণসংখ্যা depth = cell->getParam(ID::DEPTH).as_int();

		vector<State> table_raw = cell->getParam(ID::TABLE).to_bits();
		যখন (Get_আকার(table_raw) < 2*width*depth)
			table_raw.ঠেলো_back(State::S0);

		vector<vector<পূর্ণসংখ্যা>> table(depth);

		জন্য (পূর্ণসংখ্যা i = 0; i < depth; i++)
		জন্য (পূর্ণসংখ্যা j = 0; j < width; j++)
		{
			যৌক্তিক pat0 = (table_raw[2*width*i + 2*j + 0] == State::S1);
			যৌক্তিক pat1 = (table_raw[2*width*i + 2*j + 1] == State::S1);

			যদি (pat0 && !pat1)
				table.at(i).ঠেলো_back(0);
			অন্যথা যদি (!pat0 && pat1)
				table.at(i).ঠেলো_back(1);
			অন্যথা
				table.at(i).ঠেলো_back(-1);
		}

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> products, undef_products;
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			পূর্ণসংখ্যা undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			জন্য (পূর্ণসংখ্যা i = 0; i < depth; i++)
			{
				std::vector<পূর্ণসংখ্যা> cmp_a, cmp_ua, cmp_b;

				জন্য (পূর্ণসংখ্যা j = 0; j < width; j++)
					যদি (table.at(i).at(j) >= 0) {
						cmp_a.ঠেলো_back(a.at(j));
						cmp_ua.ঠেলো_back(undef_a.at(j));
						cmp_b.ঠেলো_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::vector<পূর্ণসংখ্যা> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::vector<পূর্ণসংখ্যা> masked_b = ez->vec_or(cmp_b, cmp_ua);

				পূর্ণসংখ্যা masked_eq = ez->vec_eq(masked_a, masked_b);
				পূর্ণসংখ্যা any_undef = ez->expression(ezSAT::OpOr, cmp_ua);

				undef_products.ঠেলো_back(ez->AND(any_undef, masked_eq));
				products.ঠেলো_back(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			পূর্ণসংখ্যা yy = ez->expression(ezSAT::OpOr, products);
			ez->SET(undef_y, ez->AND(ez->NOT(yy), ez->expression(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		অন্যথা
		{
			std::vector<পূর্ণসংখ্যা> products;

			জন্য (পূর্ণসংখ্যা i = 0; i < depth; i++)
			{
				std::vector<পূর্ণসংখ্যা> cmp_a, cmp_b;

				জন্য (পূর্ণসংখ্যা j = 0; j < width; j++)
					যদি (table.at(i).at(j) >= 0) {
						cmp_a.ঠেলো_back(a.at(j));
						cmp_b.ঠেলো_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.ঠেলো_back(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->SET(y, ez->expression(ezSAT::OpOr, products));
		}

		ফেরত সত্য;
	}

	যদি (cell->type == ID($fa))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> c = importDefSigSpec(cell->getPort(ID::C), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<পূর্ণসংখ্যা> x = importDefSigSpec(cell->getPort(ID::X), timestep);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;
		std::vector<পূর্ণসংখ্যা> xx = model_undef ? ez->vec_var(x.আকার_2()) : x;

		std::vector<পূর্ণসংখ্যা> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::vector<পূর্ণসংখ্যা> t2 = ez->vec_and(a, b);
		std::vector<পূর্ণসংখ্যা> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<পূর্ণসংখ্যা> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($lcu))
	{
		std::vector<পূর্ণসংখ্যা> p = importDefSigSpec(cell->getPort(ID::P), timestep);
		std::vector<পূর্ণসংখ্যা> g = importDefSigSpec(cell->getPort(ID::G), timestep);
		std::vector<পূর্ণসংখ্যা> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<পূর্ণসংখ্যা> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(co.আকার_2()) : co;

		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(co); i++)
			ez->SET(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_p = importUndefSigSpec(cell->getPort(ID::P), timestep);
			std::vector<পূর্ণসংখ্যা> undef_g = importUndefSigSpec(cell->getPort(ID::G), timestep);
			std::vector<পূর্ণসংখ্যা> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<পূর্ণসংখ্যা> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			পূর্ণসংখ্যা undef_any_p = ez->expression(ezSAT::OpOr, undef_p);
			পূর্ণসংখ্যা undef_any_g = ez->expression(ezSAT::OpOr, undef_g);
			পূর্ণসংখ্যা undef_any_ci = ez->expression(ezSAT::OpOr, undef_ci);
			পূর্ণসংখ্যা undef_co_bit = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::vector<পূর্ণসংখ্যা> undef_co_bits(undef_co.আকার_2(), undef_co_bit);
			ez->assume(ez->vec_eq(undef_co_bits, undef_co));

			undefGating(co, yy, undef_co);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($alu))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<পূর্ণসংখ্যা> x = importDefSigSpec(cell->getPort(ID::X), timestep);
		std::vector<পূর্ণসংখ্যা> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<পূর্ণসংখ্যা> bi = importDefSigSpec(cell->getPort(ID::BI), timestep);
		std::vector<পূর্ণসংখ্যা> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		extendSignalWidth(a, b, y, cell);
		extendSignalWidth(a, b, x, cell);
		extendSignalWidth(a, b, co, cell);

		std::vector<পূর্ণসংখ্যা> def_y = model_undef ? ez->vec_var(y.আকার_2()) : y;
		std::vector<পূর্ণসংখ্যা> def_x = model_undef ? ez->vec_var(x.আকার_2()) : x;
		std::vector<পূর্ণসংখ্যা> def_co = model_undef ? ez->vec_var(co.আকার_2()) : co;

		log_assert(Get_আকার(y) == Get_আকার(x));
		log_assert(Get_আকার(y) == Get_আকার(co));
		log_assert(Get_আকার(ci) == 1);
		log_assert(Get_আকার(bi) == 1);

		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(y); i++)
		{
			পূর্ণসংখ্যা s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->SET(def_x.at(i), ez->XOR(s1, s2));
			ez->SET(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->SET(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<পূর্ণসংখ্যা> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<পূর্ণসংখ্যা> undef_bi = importUndefSigSpec(cell->getPort(ID::BI), timestep);

			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<পূর্ণসংখ্যা> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);
			std::vector<পূর্ণসংখ্যা> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			extendSignalWidth(undef_a, undef_b, undef_y, cell);
			extendSignalWidth(undef_a, undef_b, undef_x, cell);
			extendSignalWidth(undef_a, undef_b, undef_co, cell);

			std::vector<পূর্ণসংখ্যা> all_inputs_undef;
			all_inputs_undef.ঢোকাও(all_inputs_undef.end(), undef_a.begin(), undef_a.end());
			all_inputs_undef.ঢোকাও(all_inputs_undef.end(), undef_b.begin(), undef_b.end());
			all_inputs_undef.ঢোকাও(all_inputs_undef.end(), undef_ci.begin(), undef_ci.end());
			all_inputs_undef.ঢোকাও(all_inputs_undef.end(), undef_bi.begin(), undef_bi.end());
			পূর্ণসংখ্যা undef_any = ez->expression(ezSAT::OpOr, all_inputs_undef);

			জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(undef_y); i++) {
				ez->SET(undef_y.at(i), undef_any);
				ez->SET(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->SET(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($slice))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec y = cell->getPort(ID::Y);
		ez->assume(signals_eq(a.extract(cell->parameters.at(ID::OFFSET).as_int(), y.আকার_2()), y, timestep));
		ফেরত সত্য;
	}

	যদি (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec b = cell->getPort(ID::B);
		RTLIL::SigSpec y = cell->getPort(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(signals_eq(ab, y, timestep));
		ফেরত সত্য;
	}

	যদি (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_তথ্য ff(nullptr, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		যদি (ff.has_aload || ff.has_arst || ff.has_sr)
			ফেরত মিথ্যা;

		যদি (timestep == 1)
		{
			initial_state.add((*sigmap)(cell->getPort(ID::Q)));
			যদি (model_undef && def_formal) {
				std::vector<পূর্ণসংখ্যা> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		অন্যথা
		{
			std::vector<পূর্ণসংখ্যা> d = importDefSigSpec(cell->getPort(ID::D), timestep-1);
			std::vector<পূর্ণসংখ্যা> undef_d;
			যদি (model_undef)
				undef_d = importUndefSigSpec(cell->getPort(ID::D), timestep-1);
			যদি (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				পূর্ণসংখ্যা srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<পূর্ণসংখ্যা> rval = importDefSigSpec(ff.val_srst, timestep-1);
				পূর্ণসংখ্যা undef_srst = -1;
				std::vector<পূর্ণসংখ্যা> undef_rval;
				যদি (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				যদি (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				অন্যথা
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			যদি (ff.has_ce) {
				পূর্ণসংখ্যা ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::vector<পূর্ণসংখ্যা> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				পূর্ণসংখ্যা undef_ce = -1;
				std::vector<পূর্ণসংখ্যা> undef_old_q;
				যদি (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				যদি (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				অন্যথা
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			যদি (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				পূর্ণসংখ্যা srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<পূর্ণসংখ্যা> rval = importDefSigSpec(ff.val_srst, timestep-1);
				পূর্ণসংখ্যা undef_srst = -1;
				std::vector<পূর্ণসংখ্যা> undef_rval;
				যদি (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				যদি (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				অন্যথা
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::vector<পূর্ণসংখ্যা> q = importDefSigSpec(cell->getPort(ID::Q), timestep);

			std::vector<পূর্ণসংখ্যা> qq = model_undef ? ez->vec_var(q.আকার_2()) : q;
			ez->assume(ez->vec_eq(d, qq));

			যদি (model_undef)
			{
				std::vector<পূর্ণসংখ্যা> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($anyconst))
	{
		যদি (timestep < 2) {
			যদি (model_undef && def_formal) {
				std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			ফেরত সত্য;
		}

		std::vector<পূর্ণসংখ্যা> d = importDefSigSpec(cell->getPort(ID::Y), timestep-1);
		std::vector<পূর্ণসংখ্যা> q = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<পূর্ণসংখ্যা> qq = (model_undef && !def_formal) ? ez->vec_var(q.আকার_2()) : q;
		ez->assume(ez->vec_eq(d, qq));

		যদি (model_undef)
		{
			std::vector<পূর্ণসংখ্যা> undef_d = importUndefSigSpec(cell->getPort(ID::Y), timestep-1);
			std::vector<পূর্ণসংখ্যা> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			যদি (def_formal) {
				জন্য (auto &undef_q_bit : undef_q)
					ez->SET(ez->CONST_FALSE, undef_q_bit);
			} অন্যথা {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($anyseq))
	{
		যদি (model_undef && def_formal) {
			std::vector<পূর্ণসংখ্যা> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			জন্য (auto &undef_q_bit : undef_q)
				ez->SET(ez->CONST_FALSE, undef_q_bit);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($barrier) && !model_barriers)
		ফেরত সত্য;

	যদি (cell->type.in(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::vector<পূর্ণসংখ্যা> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<পূর্ণসংখ্যা> yy = model_undef ? ez->vec_var(y.আকার_2()) : y;
		ez->assume(ez->vec_eq(a, yy));

		যদি (model_undef) {
			std::vector<পূর্ণসংখ্যা> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, মিথ্যা);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ফেরত সত্য;
	}

	যদি (cell->type == ID($initstate))
	{
		auto চাবি = make_pair(prefix, timestep);
		যদি (initstates.গুনতি(চাবি) == 0)
			initstates[চাবি] = মিথ্যা;

		std::vector<পূর্ণসংখ্যা> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		log_assert(Get_আকার(y) == 1);
		ez->SET(y[0], initstates[চাবি] ? ez->CONST_TRUE : ez->CONST_FALSE);

		যদি (model_undef) {
			std::vector<পূর্ণসংখ্যা> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			log_assert(Get_আকার(undef_y) == 1);
			ez->SET(undef_y[0], ez->CONST_FALSE);
		}

		ফেরত সত্য;
	}

	যদি (cell->type == ID($প্রতিজ্ঞা))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		ফেরত সত্য;
	}

	যদি (cell->type == ID($assume))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		ফেরত সত্য;
	}

	যদি (cell->type == ID($scopeinfo) || cell->type == ID($প্রবেশ_port))
	{
		ফেরত সত্য;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	ফেরত মিথ্যা;
}

নামাকাশ Yosys {

শূন্য report_missing_model(যৌক্তিক warn_only, RTLIL::Cell* cell)
{
	std::string s;
	যদি (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	অন্যথা
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	যদি (warn_only) {
		log_warning_noprefix("%s\n", s);
	} অন্যথা {
		log_error("%s\n", s);
	}
}

}
