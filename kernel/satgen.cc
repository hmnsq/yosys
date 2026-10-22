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

#ضمن "kernel/satgen.h"
#ضمن "kernel/ff.h"
#ضمن "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

منطقي SatGen::importCell(RTLIL::Cell *cell, صحيح timestep)
{
	منطقي arith_undef_handled = خطأ;
	منطقي is_arith_compare = cell->type.اندريون(ID($lt), ID($le), ID($ge), ID($gt));

	إذا (model_undef && (cell->type.اندريون(ID($جوڙيو), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		إذا (is_arith_compare)
			extend_Signal_ويڪر(undef_a, undef_b, cell, سچ);
		وإلا
			extend_Signal_ويڪر(undef_a, undef_b, undef_y, cell, سچ);

		صحيح undef_any_a = ez->اظهار_2(ezSAT::OpOr, undef_a);
		صحيح undef_any_b = ez->اظهار_2(ezSAT::OpOr, undef_b);
		صحيح undef_y_ٻٽو_انگ = ez->OR(undef_any_a, undef_any_b);

		إذا (cell->type.اندريون(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::ويڪٽر<صحيح> b = importSigSpec(cell->get_پورٽ(ID::B), timestep);
			undef_y_ٻٽو_انگ = ez->OR(undef_y_ٻٽو_انگ, ez->NOT(ez->اظهار_2(ezSAT::OpOr, b)));
		}

		إذا (is_arith_compare) {
			لكل (سائيز_t i = 1; i < undef_y.سائيز(); i++)
				ez->سيٽ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سيٽ_2(undef_y_ٻٽو_انگ, undef_y.at(0));
		} وإلا {
			std::ويڪٽر<صحيح> undef_y_بٽ(undef_y.سائيز(), undef_y_ٻٽو_انگ);
			ez->assume(ez->vec_eq(undef_y_بٽ, undef_y));
		}

		arith_undef_handled = سچ;
	}

	إذا (cell->type.اندريون(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($جوڙيو), ID($sub)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		extend_Signal_ويڪر(a, b, y, cell);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		إذا (cell->type.اندريون(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		إذا (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		إذا (cell->type.اندريون(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		إذا (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		إذا (cell->type.اندريون(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		إذا (cell->type.اندريون(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		إذا (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($جوڙيو))
			ez->assume(ez->vec_eq(ez->vec_جوڙيو(a, b), yy));
		إذا (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		إذا (model_undef && !arith_undef_handled)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			extend_Signal_ويڪر(undef_a, undef_b, undef_y, cell, خطأ);

			إذا (cell->type.اندريون(ID($and), ID($_AND_), ID($_NAND_))) {
				std::ويڪٽر<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ويڪٽر<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ويڪٽر<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.اندريون(ID($or), ID($_OR_), ID($_NOR_))) {
				std::ويڪٽر<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ويڪٽر<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ويڪٽر<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.اندريون(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::ويڪٽر<صحيح> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type == ID($_ANDNOT_)) {
				std::ويڪٽر<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ويڪٽر<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ويڪٽر<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			وإلا إذا (cell->type == ID($_ORNOT_)) {
				std::ويڪٽر<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ويڪٽر<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ويڪٽر<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا
				log_abort();

			undefGating(y, yy, undef_y);
		}
		وإلا إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		منطقي aoi_گهڻائي = cell->type.اندريون(ID($_AOI3_), ID($_AOI4_));
		منطقي three_گهڻائي = cell->type.اندريون(ID($_AOI3_), ID($_OAI3_));

		صحيح a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep).at(0);
		صحيح b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep).at(0);
		صحيح c = importDefSigSpec(cell->get_پورٽ(ID::C), timestep).at(0);
		صحيح d = three_گهڻائي ? (aoi_گهڻائي ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_پورٽ(ID::D), timestep).at(0);
		صحيح y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep).at(0);
		صحيح yy = model_undef ? ez->literal() : y;

		إذا (cell->type.اندريون(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		وإلا
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		إذا (model_undef)
		{
			صحيح undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep).at(0);
			صحيح undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep).at(0);
			صحيح undef_c = importUndefSigSpec(cell->get_پورٽ(ID::C), timestep).at(0);
			صحيح undef_d = three_گهڻائي ? ez->CONST_FALSE : importUndefSigSpec(cell->get_پورٽ(ID::D), timestep).at(0);
			صحيح undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep).at(0);

			إذا (aoi_گهڻائي)
			{
				صحيح a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				صحيح b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				صحيح c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				صحيح d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				صحيح ab = ez->AND(a, b), cd = ez->AND(c, d);
				صحيح undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				صحيح undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				صحيح ab1 = ez->AND(ab, ez->NOT(undef_ab));
				صحيح cd1 = ez->AND(cd, ez->NOT(undef_cd));
				صحيح yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			وإلا
			{
				صحيح a1 = ez->AND(a, ez->NOT(undef_a));
				صحيح b1 = ez->AND(b, ez->NOT(undef_b));
				صحيح c1 = ez->AND(c, ez->NOT(undef_c));
				صحيح d1 = ez->AND(d, ez->NOT(undef_d));

				صحيح ab = ez->OR(a, b), cd = ez->OR(c, d);
				صحيح undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				صحيح undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				صحيح ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				صحيح cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				صحيح yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($_NOT_), ID($not)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		extend_Signal_ويڪر_Unary(a, y, cell);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		إذا (model_undef) {
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			extend_Signal_ويڪر_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($bweqx))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		std::ويڪٽر<صحيح> bweqx = ez->vec_not(ez->vec_xor(a, b));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);

			std::ويڪٽر<صحيح> both_undef = ez->vec_and(undef_a, undef_b);
			std::ويڪٽر<صحيح> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			لكل (صحيح yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> s = importDefSigSpec(cell->get_پورٽ(ID::S), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;
		إذا (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		وإلا إذا (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		وإلا
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_s = importUndefSigSpec(cell->get_پورٽ(ID::S), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);

			std::ويڪٽر<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ويڪٽر<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::ويڪٽر<صحيح> yX;
			إذا (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			وإلا
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($bmux))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> s = importDefSigSpec(cell->get_پورٽ(ID::S), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		std::ويڪٽر<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورٽ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		}

		إذا (Get_سائيز(s) == 0) {
			ez->vec_سيٽ(a, y);
			إذا (model_undef)
				ez->vec_سيٽ(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = Get_سائيز(s)-1; i >= 0; i--)
			{
				std::ويڪٽر<صحيح> ٻاهريون = (i == 0) ? y : ez->vec_var(a.سائيز() / 2);
				std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(ٻاهريون.سائيز()) : ٻاهريون;

				std::ويڪٽر<صحيح> a0(a.begin(), a.begin() + a.سائيز() / 2);
				std::ويڪٽر<صحيح> a1(a.begin() + a.سائيز() / 2, a.پڇاڙي());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				إذا (model_undef)
				{
					std::ويڪٽر<صحيح> undef_ٻاهريون = (i == 0) ? undef_y : ez->vec_var(a.سائيز() / 2);
					std::ويڪٽر<صحيح> undef_a0(undef_a.begin(), undef_a.begin() + a.سائيز() / 2);
					std::ويڪٽر<صحيح> undef_a1(undef_a.begin() + a.سائيز() / 2, undef_a.پڇاڙي());
					std::ويڪٽر<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::ويڪٽر<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::ويڪٽر<صحيح> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_ٻاهريون));
					undefGating(ٻاهريون, yy, undef_ٻاهريون);

					undef_a = undef_ٻاهريون;
				}

				a = ٻاهريون;
			}
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($demux))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> s = importDefSigSpec(cell->get_پورٽ(ID::S), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;
		std::ويڪٽر<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورٽ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		}

		إذا (Get_سائيز(s) == 0) {
			ez->vec_سيٽ(a, y);
			إذا (model_undef)
				ez->vec_سيٽ(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = 0; i < (1 << Get_سائيز(s)); i++)
			{
				std::ويڪٽر<صحيح> ss;
				لكل (صحيح j = 0; j < Get_سائيز(s); j++) {
					إذا (i & 1 << j)
						ss.ڌڪو_پٺ(s[j]);
					وإلا
						ss.ڌڪو_پٺ(ez->NOT(s[j]));
				}
				صحيح sss = ez->اظهار_2(ezSAT::OpAnd, ss);

				لكل (صحيح j = 0; j < Get_سائيز(a); j++) {
					ez->سيٽ_2(ez->AND(sss, a[j]), yy.at(i * Get_سائيز(a) + j));
				}

				إذا (model_undef)
				{
					صحيح s0 = ez->اظهار_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					صحيح us = ez->AND(ez->NOT(s0), ez->اظهار_2(ezSAT::OpOr, undef_s));
					لكل (صحيح j = 0; j < Get_سائيز(a); j++) {
						صحيح a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						صحيح yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->سيٽ_2(yX, undef_y.at(i * Get_سائيز(a) + j));
					}
				}
			}
			إذا (model_undef)
				undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($pmux))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> s = importDefSigSpec(cell->get_پورٽ(ID::S), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		std::ويڪٽر<صحيح> tmp = a;
		لكل (سائيز_t i = 0; i < s.سائيز(); i++) {
			std::ويڪٽر<صحيح> part_جو_b(b.begin()+i*a.سائيز(), b.begin()+(i+1)*a.سائيز());
			tmp = ez->vec_ite(s.at(i), part_جو_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_s = importUndefSigSpec(cell->get_پورٽ(ID::S), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);

			صحيح all_undef = ez->CONST_FALSE;
			صحيح مليو_سرگرم = ez->CONST_FALSE;

			std::ويڪٽر<صحيح> undef_tmp = undef_a;

			لكل (سائيز_t i = 0; i < s.سائيز(); i++)
			{
				std::ويڪٽر<صحيح> part_جو_undef_b(undef_b.begin()+i*a.سائيز(), undef_b.begin()+(i+1)*a.سائيز());

				undef_tmp = ez->vec_ite(s.at(i), part_جو_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), مليو_سرگرم));
				مليو_سرگرم = ez->OR(مليو_سرگرم, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::ويڪٽر<صحيح>(a.سائيز(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($pos), ID($buf), ID($neg)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		extend_Signal_ويڪر_Unary(a, y, cell);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		إذا (cell->type.اندريون(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} وإلا {
			std::ويڪٽر<صحيح> zero(a.سائيز(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			extend_Signal_ويڪر_Unary(undef_a, undef_y, cell);

			إذا (cell->type.اندريون(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} وإلا {
				صحيح undef_any_a = ez->اظهار_2(ezSAT::OpOr, undef_a);
				std::ويڪٽر<صحيح> undef_y_بٽ(undef_y.سائيز(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_بٽ, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		إذا (cell->type == ID($reduce_and))
			ez->سيٽ_2(ez->اظهار_2(ez->OpAnd, a), yy.at(0));
		إذا (cell->type.اندريون(ID($reduce_or), ID($reduce_bool)))
			ez->سيٽ_2(ez->اظهار_2(ez->OpOr, a), yy.at(0));
		إذا (cell->type == ID($reduce_xor))
			ez->سيٽ_2(ez->اظهار_2(ez->OpXor, a), yy.at(0));
		إذا (cell->type == ID($reduce_xnor))
			ez->سيٽ_2(ez->NOT(ez->اظهار_2(ez->OpXor, a)), yy.at(0));
		إذا (cell->type == ID($logic_not))
			ez->سيٽ_2(ez->NOT(ez->اظهار_2(ez->OpOr, a)), yy.at(0));
		لكل (سائيز_t i = 1; i < y.سائيز(); i++)
			ez->سيٽ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			صحيح aX = ez->اظهار_2(ezSAT::OpOr, undef_a);

			إذا (cell->type == ID($reduce_and)) {
				صحيح a0 = ez->اظهار_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.اندريون(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				صحيح a1 = ez->اظهار_2(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.اندريون(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} وإلا
				log_abort();

			لكل (سائيز_t i = 1; i < undef_y.سائيز(); i++)
				ez->سيٽ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($logic_and), ID($logic_or)))
	{
		std::ويڪٽر<صحيح> vec_a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> vec_b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);

		صحيح a = ez->اظهار_2(ez->OpOr, vec_a);
		صحيح b = ez->اظهار_2(ez->OpOr, vec_b);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		إذا (cell->type == ID($logic_and))
			ez->سيٽ_2(ez->اظهار_2(ez->OpAnd, a, b), yy.at(0));
		وإلا
			ez->سيٽ_2(ez->اظهار_2(ez->OpOr, a, b), yy.at(0));
		لكل (سائيز_t i = 1; i < y.سائيز(); i++)
			ez->سيٽ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);

			صحيح a0 = ez->NOT(ez->OR(ez->اظهار_2(ezSAT::OpOr, vec_a), ez->اظهار_2(ezSAT::OpOr, undef_a)));
			صحيح b0 = ez->NOT(ez->OR(ez->اظهار_2(ezSAT::OpOr, vec_b), ez->اظهار_2(ezSAT::OpOr, undef_b)));
			صحيح a1 = ez->اظهار_2(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			صحيح b1 = ez->اظهار_2(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			صحيح aX = ez->اظهار_2(ezSAT::OpOr, undef_a);
			صحيح bX = ez->اظهار_2(ezSAT::OpOr, undef_b);

			إذا (cell->type == ID($logic_and))
				ez->سيٽ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			وإلا إذا (cell->type == ID($logic_or))
				ez->سيٽ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			وإلا
				log_abort();

			لكل (سائيز_t i = 1; i < undef_y.سائيز(); i++)
				ez->سيٽ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		منطقي is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		extend_Signal_ويڪر(a, b, cell);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		إذا (model_undef && cell->type.اندريون(ID($eqx), ID($nex))) {
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			extend_Signal_ويڪر(undef_a, undef_b, cell, سچ);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		إذا (cell->type == ID($lt))
			ez->سيٽ_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($le))
			ez->سيٽ_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		إذا (cell->type.اندريون(ID($eq), ID($eqx)))
			ez->سيٽ_2(ez->vec_eq(a, b), yy.at(0));
		إذا (cell->type.اندريون(ID($ne), ID($nex)))
			ez->سيٽ_2(ez->vec_ne(a, b), yy.at(0));
		إذا (cell->type == ID($ge))
			ez->سيٽ_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($gt))
			ez->سيٽ_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		لكل (سائيز_t i = 1; i < y.سائيز(); i++)
			ez->سيٽ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef && cell->type.اندريون(ID($eqx), ID($nex)))
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			extend_Signal_ويڪر(undef_a, undef_b, cell, سچ);

			إذا (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			وإلا
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			لكل (سائيز_t i = 0; i < y.سائيز(); i++)
				ez->سيٽ_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		وإلا إذا (model_undef && cell->type.اندريون(ID($eq), ID($ne)))
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			extend_Signal_ويڪر(undef_a, undef_b, cell, سچ);

			صحيح undef_any_a = ez->اظهار_2(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->اظهار_2(ezSAT::OpOr, undef_b);
			صحيح undef_any = ez->OR(undef_any_a, undef_any_b);

			std::ويڪٽر<صحيح> masked_a_بٽ = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::ويڪٽر<صحيح> masked_b_بٽ = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			صحيح masked_ne = ez->vec_ne(masked_a_بٽ, masked_b_بٽ);
			صحيح undef_y_ٻٽو_انگ = ez->AND(undef_any, ez->NOT(masked_ne));

			لكل (سائيز_t i = 1; i < undef_y.سائيز(); i++)
				ez->سيٽ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سيٽ_2(undef_y_ٻٽو_انگ, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			إذا (model_undef) {
				std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($سرڪايو), ID($shiftx)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		صحيح extend_ٻٽو_انگ = ez->CONST_FALSE;

		إذا (cell->parameters[ID::A_SIGNED].as_bool())
			extend_ٻٽو_انگ = a.پٺ();

		طالما (y.سائيز() < a.سائيز())
			y.ڌڪو_پٺ(ez->literal());
		طالما (y.سائيز() > a.سائيز())
			a.ڌڪو_پٺ(extend_ٻٽو_انگ);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;
		std::ويڪٽر<صحيح> shifted_a;

		إذا (cell->type.اندريون( ID($shl), ID($sshl)))
			shifted_a = ez->vec_سرڪايو_کاٻو(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($shr))
			shifted_a = ez->vec_سرڪايو_ساڄو(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($sshr))
			shifted_a = ez->vec_سرڪايو_ساڄو(a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? a.پٺ() : ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type.اندريون(ID($سرڪايو), ID($shiftx)))
			shifted_a = ez->vec_سرڪايو_ساڄو(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			std::ويڪٽر<صحيح> undef_a_shifted;

			extend_ٻٽو_انگ = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			إذا (cell->parameters[ID::A_SIGNED].as_bool())
				extend_ٻٽو_انگ = undef_a.پٺ();

			طالما (undef_y.سائيز() < undef_a.سائيز())
				undef_y.ڌڪو_پٺ(ez->literal());
			طالما (undef_y.سائيز() > undef_a.سائيز())
				undef_a.ڌڪو_پٺ(extend_ٻٽو_انگ);

			إذا (cell->type.اندريون(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_سرڪايو_کاٻو(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shr))
				undef_a_shifted = ez->vec_سرڪايو_ساڄو(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_سرڪايو_ساڄو(undef_a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.پٺ() : ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($سرڪايو))
				undef_a_shifted = ez->vec_سرڪايو_ساڄو(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_سرڪايو_ساڄو(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			صحيح undef_any_b = ez->اظهار_2(ezSAT::OpOr, undef_b);
			std::ويڪٽر<صحيح> undef_all_y_بٽ(undef_y.سائيز(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_بٽ), undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($mul))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		extend_Signal_ويڪر(a, b, y, cell);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		std::ويڪٽر<صحيح> tmp(a.سائيز(), ez->CONST_FALSE);
		لكل (صحيح i = 0; i < صحيح(a.سائيز()); i++)
		{
			std::ويڪٽر<صحيح> shifted_a(a.سائيز(), ez->CONST_FALSE);
			لكل (صحيح j = i; j < صحيح(a.سائيز()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_جوڙيو(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($macc), ID($macc_v2)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::ويڪٽر<صحيح> tmp(Get_سائيز(y), ez->CONST_FALSE);

		لكل (خودڪار &پورٽ : macc.terms)
		{
			std::ويڪٽر<صحيح> اندريون_a = importDefSigSpec(پورٽ.اندريون_a, timestep);
			std::ويڪٽر<صحيح> اندريون_b = importDefSigSpec(پورٽ.اندريون_b, timestep);

			طالما (Get_سائيز(اندريون_a) < Get_سائيز(y))
				اندريون_a.ڌڪو_پٺ(پورٽ.is_signed && !اندريون_a.خالي_جايون() ? اندريون_a.پٺ() : ez->CONST_FALSE);
			اندريون_a.resize(Get_سائيز(y));

			إذا (Get_سائيز(اندريون_b))
			{
				طالما (Get_سائيز(اندريون_b) < Get_سائيز(y))
					اندريون_b.ڌڪو_پٺ(پورٽ.is_signed && !اندريون_b.خالي_جايون() ? اندريون_b.پٺ() : ez->CONST_FALSE);
				اندريون_b.resize(Get_سائيز(y));

				لكل (صحيح i = 0; i < Get_سائيز(اندريون_b); i++) {
					std::ويڪٽر<صحيح> shifted_a(اندريون_a.سائيز(), ez->CONST_FALSE);
					لكل (صحيح j = i; j < صحيح(اندريون_a.سائيز()); j++)
						shifted_a.at(j) = اندريون_a.at(j-i);
					إذا (پورٽ.do_ڪٽيو)
						tmp = ez->vec_ite(اندريون_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					وإلا
						tmp = ez->vec_ite(اندريون_b.at(i), ez->vec_جوڙيو(tmp, shifted_a), tmp);
				}
			}
			وإلا
			{
				إذا (پورٽ.do_ڪٽيو)
					tmp = ez->vec_sub(tmp, اندريون_a);
				وإلا
					tmp = ez->vec_جوڙيو(tmp, اندريون_a);
			}
		}

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_c;

			إذا (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_پورٽ(ID::C), timestep);

			صحيح undef_any_a = ez->اظهار_2(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->اظهار_2(ezSAT::OpOr, undef_b);
			صحيح undef_any_c = ez->اظهار_2(ezSAT::OpOr, undef_c);
			صحيح undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::ويڪٽر<صحيح>(Get_سائيز(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		وإلا
			ez->assume(ez->vec_eq(y, tmp));

		أرجع سچ;
	}

	إذا (cell->type.اندريون(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		extend_Signal_ويڪر(a, b, y, cell);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;

		std::ويڪٽر<صحيح> a_u, b_u;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.پٺ(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.پٺ(), ez->vec_neg(b), b);
		} وإلا {
			a_u = a;
			b_u = b;
		}

		std::ويڪٽر<صحيح> chain_buf = a_u;
		std::ويڪٽر<صحيح> y_u(a_u.سائيز(), ez->CONST_FALSE);
		لكل (صحيح i = صحيح(a.سائيز())-1; i >= 0; i--)
		{
			chain_buf.وجهو(chain_buf.پڇاڙي(), chain_buf.سائيز(), ez->CONST_FALSE);

			std::ويڪٽر<صحيح> b_shl(i, ez->CONST_FALSE);
			b_shl.وجهو(b_shl.پڇاڙي(), b_u.begin(), b_u.پڇاڙي());
			b_shl.وجهو(b_shl.پڇاڙي(), chain_buf.سائيز()-b_shl.سائيز(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.سائيز(), chain_buf.پڇاڙي());
		}

		std::ويڪٽر<صحيح> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.سائيز());

		// modulo calculation
		std::ويڪٽر<صحيح> modulo_trunc;
		صحيح floored_eq_trunc;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.پٺ(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.پٺ(), b.پٺ()), ez->NOT(ez->اظهار_2(ezSAT::OpOr, modulo_trunc)));
		} وإلا {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		إذا (cell->type == ID($div)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.پٺ(), b.پٺ()), ez->vec_neg(y_u), y_u)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} وإلا إذا (cell->type == ID($divfloor)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.پٺ(), b.پٺ()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_جوڙيو(y_u, ez->vec_const_unsigned(1, y_u.سائيز())),
						y_u
					)),
					y_u
				)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_جوڙيو(modulo_trunc, b))));
		}

		إذا (ignore_div_by_zero) {
			ez->assume(ez->اظهار_2(ezSAT::OpOr, b));
		} وإلا {
			std::ويڪٽر<صحيح> div_zero_نتيجو;
			إذا (cell->type.اندريون(ID($div), ID($divfloor))) {
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::ويڪٽر<صحيح> all_ones(y.سائيز(), ez->CONST_TRUE);
					std::ويڪٽر<صحيح> only_پهريون_one(y.سائيز(), ez->CONST_FALSE);
					only_پهريون_one.at(0) = ez->CONST_TRUE;
					div_zero_نتيجو = ez->vec_ite(a.پٺ(), only_پهريون_one, all_ones);
				} وإلا {
					div_zero_نتيجو.وجهو(div_zero_نتيجو.پڇاڙي(), cell->get_پورٽ(ID::A).سائيز(), ez->CONST_TRUE);
					div_zero_نتيجو.وجهو(div_zero_نتيجو.پڇاڙي(), y.سائيز() - div_zero_نتيجو.سائيز(), ez->CONST_FALSE);
				}
			} وإلا إذا (cell->type.اندريون(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				صحيح copy_a_بٽ = min(cell->get_پورٽ(ID::A).سائيز(), cell->get_پورٽ(ID::B).سائيز());
				div_zero_نتيجو.وجهو(div_zero_نتيجو.پڇاڙي(), a.begin(), a.begin() + copy_a_بٽ);
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_نتيجو.وجهو(div_zero_نتيجو.پڇاڙي(), y.سائيز() - div_zero_نتيجو.سائيز(), div_zero_نتيجو.پٺ());
				وإلا
					div_zero_نتيجو.وجهو(div_zero_نتيجو.پڇاڙي(), y.سائيز() - div_zero_نتيجو.سائيز(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->اظهار_2(ezSAT::OpOr, b), y_tmp, div_zero_نتيجو)));
		}

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($lut))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		std::ويڪٽر<صحيح> lut;
		لكل (خودڪار ٻٽو_انگ : cell->getParam(ID::LUT))
			lut.ڌڪو_پٺ(ٻٽو_انگ == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		طالما (Get_سائيز(lut) < (1 << Get_سائيز(a)))
			lut.ڌڪو_پٺ(ez->CONST_FALSE);
		lut.resize(1 << Get_سائيز(a));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> t(lut), u(Get_سائيز(t), ez->CONST_FALSE);

			لكل (صحيح i = Get_سائيز(a)-1; i >= 0; i--)
			{
				std::ويڪٽر<صحيح> t0(t.begin(), t.begin() + Get_سائيز(t)/2);
				std::ويڪٽر<صحيح> t1(t.begin() + Get_سائيز(t)/2, t.پڇاڙي());

				std::ويڪٽر<صحيح> u0(u.begin(), u.begin() + Get_سائيز(u)/2);
				std::ويڪٽر<صحيح> u1(u.begin() + Get_سائيز(u)/2, u.پڇاڙي());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_سائيز(t) == 1);
			log_assert(Get_سائيز(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep), u));
		}
		وإلا
		{
			std::ويڪٽر<صحيح> t = lut;
			لكل (صحيح i = Get_سائيز(a)-1; i >= 0; i--)
			{
				std::ويڪٽر<صحيح> t0(t.begin(), t.begin() + Get_سائيز(t)/2);
				std::ويڪٽر<صحيح> t1(t.begin() + Get_سائيز(t)/2, t.پڇاڙي());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_سائيز(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($sop))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		صحيح y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep).at(0);

		صحيح ويڪر = cell->getParam(ID::ويڪر_2).as_int();
		صحيح اونهائي = cell->getParam(ID::اونهائي_2).as_int();

		ويڪٽر<State> جدول_raw = cell->getParam(ID::جدول).منزل_بٽ();
		طالما (Get_سائيز(جدول_raw) < 2*ويڪر*اونهائي)
			جدول_raw.ڌڪو_پٺ(State::S0);

		ويڪٽر<ويڪٽر<صحيح>> جدول_2(اونهائي);

		لكل (صحيح i = 0; i < اونهائي; i++)
		لكل (صحيح j = 0; j < ويڪر; j++)
		{
			منطقي pat0 = (جدول_raw[2*ويڪر*i + 2*j + 0] == State::S1);
			منطقي pat1 = (جدول_raw[2*ويڪر*i + 2*j + 1] == State::S1);

			إذا (pat0 && !pat1)
				جدول_2.at(i).ڌڪو_پٺ(0);
			وإلا إذا (!pat0 && pat1)
				جدول_2.at(i).ڌڪو_پٺ(1);
			وإلا
				جدول_2.at(i).ڌڪو_پٺ(-1);
		}

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> products, undef_products;
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			صحيح undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep).at(0);

			لكل (صحيح i = 0; i < اونهائي; i++)
			{
				std::ويڪٽر<صحيح> cmp_a, cmp_ua, cmp_b;

				لكل (صحيح j = 0; j < ويڪر; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ڌڪو_پٺ(a.at(j));
						cmp_ua.ڌڪو_پٺ(undef_a.at(j));
						cmp_b.ڌڪو_پٺ(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::ويڪٽر<صحيح> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::ويڪٽر<صحيح> masked_b = ez->vec_or(cmp_b, cmp_ua);

				صحيح masked_eq = ez->vec_eq(masked_a, masked_b);
				صحيح any_undef = ez->اظهار_2(ezSAT::OpOr, cmp_ua);

				undef_products.ڌڪو_پٺ(ez->AND(any_undef, masked_eq));
				products.ڌڪو_پٺ(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			صحيح yy = ez->اظهار_2(ezSAT::OpOr, products);
			ez->سيٽ_2(undef_y, ez->AND(ez->NOT(yy), ez->اظهار_2(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			std::ويڪٽر<صحيح> products;

			لكل (صحيح i = 0; i < اونهائي; i++)
			{
				std::ويڪٽر<صحيح> cmp_a, cmp_b;

				لكل (صحيح j = 0; j < ويڪر; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ڌڪو_پٺ(a.at(j));
						cmp_b.ڌڪو_پٺ(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.ڌڪو_پٺ(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->سيٽ_2(y, ez->اظهار_2(ezSAT::OpOr, products));
		}

		أرجع سچ;
	}

	إذا (cell->type == ID($fa))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> c = importDefSigSpec(cell->get_پورٽ(ID::C), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		std::ويڪٽر<صحيح> x = importDefSigSpec(cell->get_پورٽ(ID::X), timestep);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;
		std::ويڪٽر<صحيح> xx = model_undef ? ez->vec_var(x.سائيز()) : x;

		std::ويڪٽر<صحيح> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::ويڪٽر<صحيح> t2 = ez->vec_and(a, b);
		std::ويڪٽر<صحيح> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_c = importUndefSigSpec(cell->get_پورٽ(ID::C), timestep);

			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			std::ويڪٽر<صحيح> undef_x = importUndefSigSpec(cell->get_پورٽ(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($lcu))
	{
		std::ويڪٽر<صحيح> p = importDefSigSpec(cell->get_پورٽ(ID::P), timestep);
		std::ويڪٽر<صحيح> g = importDefSigSpec(cell->get_پورٽ(ID::G), timestep);
		std::ويڪٽر<صحيح> ci = importDefSigSpec(cell->get_پورٽ(ID::CI), timestep);
		std::ويڪٽر<صحيح> co = importDefSigSpec(cell->get_پورٽ(ID::CO), timestep);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(co.سائيز()) : co;

		لكل (صحيح i = 0; i < Get_سائيز(co); i++)
			ez->سيٽ_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_p = importUndefSigSpec(cell->get_پورٽ(ID::P), timestep);
			std::ويڪٽر<صحيح> undef_g = importUndefSigSpec(cell->get_پورٽ(ID::G), timestep);
			std::ويڪٽر<صحيح> undef_ci = importUndefSigSpec(cell->get_پورٽ(ID::CI), timestep);
			std::ويڪٽر<صحيح> undef_co = importUndefSigSpec(cell->get_پورٽ(ID::CO), timestep);

			صحيح undef_any_p = ez->اظهار_2(ezSAT::OpOr, undef_p);
			صحيح undef_any_g = ez->اظهار_2(ezSAT::OpOr, undef_g);
			صحيح undef_any_ci = ez->اظهار_2(ezSAT::OpOr, undef_ci);
			صحيح undef_co_ٻٽو_انگ = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::ويڪٽر<صحيح> undef_co_بٽ(undef_co.سائيز(), undef_co_ٻٽو_انگ);
			ez->assume(ez->vec_eq(undef_co_بٽ, undef_co));

			undefGating(co, yy, undef_co);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($alu))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> b = importDefSigSpec(cell->get_پورٽ(ID::B), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		std::ويڪٽر<صحيح> x = importDefSigSpec(cell->get_پورٽ(ID::X), timestep);
		std::ويڪٽر<صحيح> ci = importDefSigSpec(cell->get_پورٽ(ID::CI), timestep);
		std::ويڪٽر<صحيح> bi = importDefSigSpec(cell->get_پورٽ(ID::BI), timestep);
		std::ويڪٽر<صحيح> co = importDefSigSpec(cell->get_پورٽ(ID::CO), timestep);

		extend_Signal_ويڪر(a, b, y, cell);
		extend_Signal_ويڪر(a, b, x, cell);
		extend_Signal_ويڪر(a, b, co, cell);

		std::ويڪٽر<صحيح> def_y = model_undef ? ez->vec_var(y.سائيز()) : y;
		std::ويڪٽر<صحيح> def_x = model_undef ? ez->vec_var(x.سائيز()) : x;
		std::ويڪٽر<صحيح> def_co = model_undef ? ez->vec_var(co.سائيز()) : co;

		log_assert(Get_سائيز(y) == Get_سائيز(x));
		log_assert(Get_سائيز(y) == Get_سائيز(co));
		log_assert(Get_سائيز(ci) == 1);
		log_assert(Get_سائيز(bi) == 1);

		لكل (صحيح i = 0; i < Get_سائيز(y); i++)
		{
			صحيح s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->سيٽ_2(def_x.at(i), ez->XOR(s1, s2));
			ez->سيٽ_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->سيٽ_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_b = importUndefSigSpec(cell->get_پورٽ(ID::B), timestep);
			std::ويڪٽر<صحيح> undef_ci = importUndefSigSpec(cell->get_پورٽ(ID::CI), timestep);
			std::ويڪٽر<صحيح> undef_bi = importUndefSigSpec(cell->get_پورٽ(ID::BI), timestep);

			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			std::ويڪٽر<صحيح> undef_x = importUndefSigSpec(cell->get_پورٽ(ID::X), timestep);
			std::ويڪٽر<صحيح> undef_co = importUndefSigSpec(cell->get_پورٽ(ID::CO), timestep);

			extend_Signal_ويڪر(undef_a, undef_b, undef_y, cell);
			extend_Signal_ويڪر(undef_a, undef_b, undef_x, cell);
			extend_Signal_ويڪر(undef_a, undef_b, undef_co, cell);

			std::ويڪٽر<صحيح> all_inputs_undef;
			all_inputs_undef.وجهو(all_inputs_undef.پڇاڙي(), undef_a.begin(), undef_a.پڇاڙي());
			all_inputs_undef.وجهو(all_inputs_undef.پڇاڙي(), undef_b.begin(), undef_b.پڇاڙي());
			all_inputs_undef.وجهو(all_inputs_undef.پڇاڙي(), undef_ci.begin(), undef_ci.پڇاڙي());
			all_inputs_undef.وجهو(all_inputs_undef.پڇاڙي(), undef_bi.begin(), undef_bi.پڇاڙي());
			صحيح undef_any = ez->اظهار_2(ezSAT::OpOr, all_inputs_undef);

			لكل (صحيح i = 0; i < Get_سائيز(undef_y); i++) {
				ez->سيٽ_2(undef_y.at(i), undef_any);
				ez->سيٽ_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->سيٽ_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($ٽڪرو))
	{
		RTLIL::SigSpec a = cell->get_پورٽ(ID::A);
		RTLIL::SigSpec y = cell->get_پورٽ(ID::Y);
		ez->assume(سگنل_eq(a.ڪڍو_2(cell->parameters.at(ID::وٿي_2).as_int(), y.سائيز()), y, timestep));
		أرجع سچ;
	}

	إذا (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_پورٽ(ID::A);
		RTLIL::SigSpec b = cell->get_پورٽ(ID::B);
		RTLIL::SigSpec y = cell->get_پورٽ(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(سگنل_eq(ab, y, timestep));
		أرجع سچ;
	}

	إذا (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_معلومات ff(خالي_اشارو, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		إذا (ff.has_aload || ff.has_arst || ff.has_sr)
			أرجع خطأ;

		إذا (timestep == 1)
		{
			initial_state.جوڙيو((*sigmap)(cell->get_پورٽ(ID::Q)));
			إذا (model_undef && def_formal) {
				std::ويڪٽر<صحيح> undef_q = importUndefSigSpec(cell->get_پورٽ(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		وإلا
		{
			std::ويڪٽر<صحيح> d = importDefSigSpec(cell->get_پورٽ(ID::D), timestep-1);
			std::ويڪٽر<صحيح> undef_d;
			إذا (model_undef)
				undef_d = importUndefSigSpec(cell->get_پورٽ(ID::D), timestep-1);
			إذا (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				صحيح srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::ويڪٽر<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::ويڪٽر<صحيح> undef_rval;
				إذا (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				إذا (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				وإلا
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			إذا (ff.has_ce) {
				صحيح ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::ويڪٽر<صحيح> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				صحيح undef_ce = -1;
				std::ويڪٽر<صحيح> undef_old_q;
				إذا (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				إذا (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				وإلا
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			إذا (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				صحيح srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::ويڪٽر<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::ويڪٽر<صحيح> undef_rval;
				إذا (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				إذا (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				وإلا
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::ويڪٽر<صحيح> q = importDefSigSpec(cell->get_پورٽ(ID::Q), timestep);

			std::ويڪٽر<صحيح> qq = model_undef ? ez->vec_var(q.سائيز()) : q;
			ez->assume(ez->vec_eq(d, qq));

			إذا (model_undef)
			{
				std::ويڪٽر<صحيح> undef_q = importUndefSigSpec(cell->get_پورٽ(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($anyconst))
	{
		إذا (timestep < 2) {
			إذا (model_undef && def_formal) {
				std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			أرجع سچ;
		}

		std::ويڪٽر<صحيح> d = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep-1);
		std::ويڪٽر<صحيح> q = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);

		std::ويڪٽر<صحيح> qq = (model_undef && !def_formal) ? ez->vec_var(q.سائيز()) : q;
		ez->assume(ez->vec_eq(d, qq));

		إذا (model_undef)
		{
			std::ويڪٽر<صحيح> undef_d = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep-1);
			std::ويڪٽر<صحيح> undef_q = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);

			إذا (def_formal) {
				لكل (خودڪار &undef_q_ٻٽو_انگ : undef_q)
					ez->سيٽ_2(ez->CONST_FALSE, undef_q_ٻٽو_انگ);
			} وإلا {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($anyseq))
	{
		إذا (model_undef && def_formal) {
			std::ويڪٽر<صحيح> undef_q = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			لكل (خودڪار &undef_q_ٻٽو_انگ : undef_q)
				ez->سيٽ_2(ez->CONST_FALSE, undef_q_ٻٽو_انگ);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($barrier) && !model_barriers)
		أرجع سچ;

	إذا (cell->type.اندريون(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::ويڪٽر<صحيح> a = importDefSigSpec(cell->get_پورٽ(ID::A), timestep);
		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		extend_Signal_ويڪر_Unary(a, y, cell);

		std::ويڪٽر<صحيح> yy = model_undef ? ez->vec_var(y.سائيز()) : y;
		ez->assume(ez->vec_eq(a, yy));

		إذا (model_undef) {
			std::ويڪٽر<صحيح> undef_a = importUndefSigSpec(cell->get_پورٽ(ID::A), timestep);
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			extend_Signal_ويڪر_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($initstate))
	{
		خودڪار ڪنجي = make_pair(اڳياڙي_2, timestep);
		إذا (initstates.ڳڻپ(ڪنجي) == 0)
			initstates[ڪنجي] = خطأ;

		std::ويڪٽر<صحيح> y = importDefSigSpec(cell->get_پورٽ(ID::Y), timestep);
		log_assert(Get_سائيز(y) == 1);
		ez->سيٽ_2(y[0], initstates[ڪنجي] ? ez->CONST_TRUE : ez->CONST_FALSE);

		إذا (model_undef) {
			std::ويڪٽر<صحيح> undef_y = importUndefSigSpec(cell->get_پورٽ(ID::Y), timestep);
			log_assert(Get_سائيز(undef_y) == 1);
			ez->سيٽ_2(undef_y[0], ez->CONST_FALSE);
		}

		أرجع سچ;
	}

	إذا (cell->type == ID($أكد))
	{
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_پورٽ(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_پورٽ(ID::EN)));
		أرجع سچ;
	}

	إذا (cell->type == ID($assume))
	{
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_پورٽ(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_پورٽ(ID::EN)));
		أرجع سچ;
	}

	إذا (cell->type == ID($scopeinfo) || cell->type == ID($داخلا_پورٽ))
	{
		أرجع سچ;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	أرجع خطأ;
}

نطاق Yosys {

فراغ report_missing_model(منطقي warn_only, RTLIL::Cell* cell)
{
	std::string s;
	إذا (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	وإلا
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	إذا (warn_only) {
		log_warning_noprefix("%s\n", s);
	} وإلا {
		log_error("%s\n", s);
	}
}

}
