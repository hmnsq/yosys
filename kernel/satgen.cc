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
	منطقي is_arith_compare = cell->type.درون(ID($lt), ID($le), ID($ge), ID($gt));

	إذا (model_undef && (cell->type.درون(ID($جمع_کن), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
		إذا (is_arith_compare)
			extend_Signal_عرض(undef_a, undef_b, cell, درست);
		وإلا
			extend_Signal_عرض(undef_a, undef_b, undef_y, cell, درست);

		صحيح undef_any_a = ez->عبارت_2(ezSAT::OpOr, undef_a);
		صحيح undef_any_b = ez->عبارت_2(ezSAT::OpOr, undef_b);
		صحيح undef_y_بیت = ez->OR(undef_any_a, undef_any_b);

		إذا (cell->type.درون(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::وکتور<صحيح> b = importSigSpec(cell->get_پورت(ID::B), timestep);
			undef_y_بیت = ez->OR(undef_y_بیت, ez->NOT(ez->عبارت_2(ezSAT::OpOr, b)));
		}

		إذا (is_arith_compare) {
			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->مجموعه_2(ez->CONST_FALSE, undef_y.at(i));
			ez->مجموعه_2(undef_y_بیت, undef_y.at(0));
		} وإلا {
			std::وکتور<صحيح> undef_y_بیتها(undef_y.اندازه(), undef_y_بیت);
			ez->assume(ez->vec_eq(undef_y_بیتها, undef_y));
		}

		arith_undef_handled = درست;
	}

	إذا (cell->type.درون(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($جمع_کن), ID($sub)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		extend_Signal_عرض(a, b, y, cell);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type.درون(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		إذا (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		إذا (cell->type.درون(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		إذا (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		إذا (cell->type.درون(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		إذا (cell->type.درون(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		إذا (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($جمع_کن))
			ez->assume(ez->vec_eq(ez->vec_جمع_کن(a, b), yy));
		إذا (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		إذا (model_undef && !arith_undef_handled)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			extend_Signal_عرض(undef_a, undef_b, undef_y, cell, خطأ);

			إذا (cell->type.درون(ID($and), ID($_AND_), ID($_NAND_))) {
				std::وکتور<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::وکتور<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::وکتور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.درون(ID($or), ID($_OR_), ID($_NOR_))) {
				std::وکتور<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::وکتور<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::وکتور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.درون(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::وکتور<صحيح> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type == ID($_ANDNOT_)) {
				std::وکتور<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::وکتور<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::وکتور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			وإلا إذا (cell->type == ID($_ORNOT_)) {
				std::وکتور<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::وکتور<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::وکتور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا
				log_abort();

			undefGating(y, yy, undef_y);
		}
		وإلا إذا (model_undef)
		{
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type.درون(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		منطقي aoi_نما = cell->type.درون(ID($_AOI3_), ID($_AOI4_));
		منطقي three_نما = cell->type.درون(ID($_AOI3_), ID($_OAI3_));

		صحيح a = importDefSigSpec(cell->get_پورت(ID::A), timestep).at(0);
		صحيح b = importDefSigSpec(cell->get_پورت(ID::B), timestep).at(0);
		صحيح c = importDefSigSpec(cell->get_پورت(ID::C), timestep).at(0);
		صحيح d = three_نما ? (aoi_نما ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_پورت(ID::D), timestep).at(0);
		صحيح y = importDefSigSpec(cell->get_پورت(ID::Y), timestep).at(0);
		صحيح yy = model_undef ? ez->literal() : y;

		إذا (cell->type.درون(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		وإلا
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		إذا (model_undef)
		{
			صحيح undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep).at(0);
			صحيح undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep).at(0);
			صحيح undef_c = importUndefSigSpec(cell->get_پورت(ID::C), timestep).at(0);
			صحيح undef_d = three_نما ? ez->CONST_FALSE : importUndefSigSpec(cell->get_پورت(ID::D), timestep).at(0);
			صحيح undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep).at(0);

			إذا (aoi_نما)
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

		أرجع درست;
	}

	إذا (cell->type.درون(ID($_NOT_), ID($not)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		extend_Signal_عرض_Unary(a, y, cell);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		إذا (model_undef) {
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			extend_Signal_عرض_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($bweqx))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		std::وکتور<صحيح> bweqx = ez->vec_not(ez->vec_xor(a, b));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);

			std::وکتور<صحيح> both_undef = ez->vec_and(undef_a, undef_b);
			std::وکتور<صحيح> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			لكل (صحيح yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		أرجع درست;
	}

	إذا (cell->type.درون(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> s = importDefSigSpec(cell->get_پورت(ID::S), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		إذا (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		وإلا إذا (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		وإلا
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_s = importUndefSigSpec(cell->get_پورت(ID::S), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);

			std::وکتور<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::وکتور<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::وکتور<صحيح> yX;
			إذا (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			وإلا
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($bmux))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> s = importDefSigSpec(cell->get_پورت(ID::S), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		std::وکتور<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورت(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
		}

		إذا (Get_اندازه(s) == 0) {
			ez->vec_مجموعه(a, y);
			إذا (model_undef)
				ez->vec_مجموعه(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = Get_اندازه(s)-1; i >= 0; i--)
			{
				std::وکتور<صحيح> بیرون = (i == 0) ? y : ez->vec_var(a.اندازه() / 2);
				std::وکتور<صحيح> yy = model_undef ? ez->vec_var(بیرون.اندازه()) : بیرون;

				std::وکتور<صحيح> a0(a.begin(), a.begin() + a.اندازه() / 2);
				std::وکتور<صحيح> a1(a.begin() + a.اندازه() / 2, a.انتها());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				إذا (model_undef)
				{
					std::وکتور<صحيح> undef_بیرون = (i == 0) ? undef_y : ez->vec_var(a.اندازه() / 2);
					std::وکتور<صحيح> undef_a0(undef_a.begin(), undef_a.begin() + a.اندازه() / 2);
					std::وکتور<صحيح> undef_a1(undef_a.begin() + a.اندازه() / 2, undef_a.انتها());
					std::وکتور<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::وکتور<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::وکتور<صحيح> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_بیرون));
					undefGating(بیرون, yy, undef_بیرون);

					undef_a = undef_بیرون;
				}

				a = بیرون;
			}
		}
		أرجع درست;
	}

	إذا (cell->type == ID($demux))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> s = importDefSigSpec(cell->get_پورت(ID::S), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::وکتور<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورت(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
		}

		إذا (Get_اندازه(s) == 0) {
			ez->vec_مجموعه(a, y);
			إذا (model_undef)
				ez->vec_مجموعه(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = 0; i < (1 << Get_اندازه(s)); i++)
			{
				std::وکتور<صحيح> ss;
				لكل (صحيح j = 0; j < Get_اندازه(s); j++) {
					إذا (i & 1 << j)
						ss.افزودن_پشت(s[j]);
					وإلا
						ss.افزودن_پشت(ez->NOT(s[j]));
				}
				صحيح sss = ez->عبارت_2(ezSAT::OpAnd, ss);

				لكل (صحيح j = 0; j < Get_اندازه(a); j++) {
					ez->مجموعه_2(ez->AND(sss, a[j]), yy.at(i * Get_اندازه(a) + j));
				}

				إذا (model_undef)
				{
					صحيح s0 = ez->عبارت_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					صحيح us = ez->AND(ez->NOT(s0), ez->عبارت_2(ezSAT::OpOr, undef_s));
					لكل (صحيح j = 0; j < Get_اندازه(a); j++) {
						صحيح a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						صحيح yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->مجموعه_2(yX, undef_y.at(i * Get_اندازه(a) + j));
					}
				}
			}
			إذا (model_undef)
				undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($pmux))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> s = importDefSigSpec(cell->get_پورت(ID::S), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		std::وکتور<صحيح> tmp = a;
		لكل (اندازه_t i = 0; i < s.اندازه(); i++) {
			std::وکتور<صحيح> part_از_b(b.begin()+i*a.اندازه(), b.begin()+(i+1)*a.اندازه());
			tmp = ez->vec_ite(s.at(i), part_از_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_s = importUndefSigSpec(cell->get_پورت(ID::S), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);

			صحيح all_undef = ez->CONST_FALSE;
			صحيح یافت_فعال = ez->CONST_FALSE;

			std::وکتور<صحيح> undef_tmp = undef_a;

			لكل (اندازه_t i = 0; i < s.اندازه(); i++)
			{
				std::وکتور<صحيح> part_از_undef_b(undef_b.begin()+i*a.اندازه(), undef_b.begin()+(i+1)*a.اندازه());

				undef_tmp = ez->vec_ite(s.at(i), part_از_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), یافت_فعال));
				یافت_فعال = ez->OR(یافت_فعال, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::وکتور<صحيح>(a.اندازه(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type.درون(ID($pos), ID($buf), ID($neg)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		extend_Signal_عرض_Unary(a, y, cell);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type.درون(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} وإلا {
			std::وکتور<صحيح> zero(a.اندازه(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			extend_Signal_عرض_Unary(undef_a, undef_y, cell);

			إذا (cell->type.درون(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} وإلا {
				صحيح undef_any_a = ez->عبارت_2(ezSAT::OpOr, undef_a);
				std::وکتور<صحيح> undef_y_بیتها(undef_y.اندازه(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_بیتها, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type.درون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type == ID($reduce_and))
			ez->مجموعه_2(ez->عبارت_2(ez->OpAnd, a), yy.at(0));
		إذا (cell->type.درون(ID($reduce_or), ID($reduce_bool)))
			ez->مجموعه_2(ez->عبارت_2(ez->OpOr, a), yy.at(0));
		إذا (cell->type == ID($reduce_xor))
			ez->مجموعه_2(ez->عبارت_2(ez->OpXor, a), yy.at(0));
		إذا (cell->type == ID($reduce_xnor))
			ez->مجموعه_2(ez->NOT(ez->عبارت_2(ez->OpXor, a)), yy.at(0));
		إذا (cell->type == ID($logic_not))
			ez->مجموعه_2(ez->NOT(ez->عبارت_2(ez->OpOr, a)), yy.at(0));
		لكل (اندازه_t i = 1; i < y.اندازه(); i++)
			ez->مجموعه_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			صحيح aX = ez->عبارت_2(ezSAT::OpOr, undef_a);

			إذا (cell->type == ID($reduce_and)) {
				صحيح a0 = ez->عبارت_2(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.درون(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				صحيح a1 = ez->عبارت_2(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.درون(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} وإلا
				log_abort();

			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->مجموعه_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type.درون(ID($logic_and), ID($logic_or)))
	{
		std::وکتور<صحيح> vec_a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> vec_b = importDefSigSpec(cell->get_پورت(ID::B), timestep);

		صحيح a = ez->عبارت_2(ez->OpOr, vec_a);
		صحيح b = ez->عبارت_2(ez->OpOr, vec_b);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type == ID($logic_and))
			ez->مجموعه_2(ez->عبارت_2(ez->OpAnd, a, b), yy.at(0));
		وإلا
			ez->مجموعه_2(ez->عبارت_2(ez->OpOr, a, b), yy.at(0));
		لكل (اندازه_t i = 1; i < y.اندازه(); i++)
			ez->مجموعه_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);

			صحيح a0 = ez->NOT(ez->OR(ez->عبارت_2(ezSAT::OpOr, vec_a), ez->عبارت_2(ezSAT::OpOr, undef_a)));
			صحيح b0 = ez->NOT(ez->OR(ez->عبارت_2(ezSAT::OpOr, vec_b), ez->عبارت_2(ezSAT::OpOr, undef_b)));
			صحيح a1 = ez->عبارت_2(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			صحيح b1 = ez->عبارت_2(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			صحيح aX = ez->عبارت_2(ezSAT::OpOr, undef_a);
			صحيح bX = ez->عبارت_2(ezSAT::OpOr, undef_b);

			إذا (cell->type == ID($logic_and))
				ez->مجموعه_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			وإلا إذا (cell->type == ID($logic_or))
				ez->مجموعه_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			وإلا
				log_abort();

			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->مجموعه_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type.درون(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		منطقي is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		extend_Signal_عرض(a, b, cell);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (model_undef && cell->type.درون(ID($eqx), ID($nex))) {
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			extend_Signal_عرض(undef_a, undef_b, cell, درست);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		إذا (cell->type == ID($lt))
			ez->مجموعه_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($le))
			ez->مجموعه_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		إذا (cell->type.درون(ID($eq), ID($eqx)))
			ez->مجموعه_2(ez->vec_eq(a, b), yy.at(0));
		إذا (cell->type.درون(ID($ne), ID($nex)))
			ez->مجموعه_2(ez->vec_ne(a, b), yy.at(0));
		إذا (cell->type == ID($ge))
			ez->مجموعه_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($gt))
			ez->مجموعه_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		لكل (اندازه_t i = 1; i < y.اندازه(); i++)
			ez->مجموعه_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef && cell->type.درون(ID($eqx), ID($nex)))
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			extend_Signal_عرض(undef_a, undef_b, cell, درست);

			إذا (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			وإلا
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			لكل (اندازه_t i = 0; i < y.اندازه(); i++)
				ez->مجموعه_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		وإلا إذا (model_undef && cell->type.درون(ID($eq), ID($ne)))
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			extend_Signal_عرض(undef_a, undef_b, cell, درست);

			صحيح undef_any_a = ez->عبارت_2(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->عبارت_2(ezSAT::OpOr, undef_b);
			صحيح undef_any = ez->OR(undef_any_a, undef_any_b);

			std::وکتور<صحيح> masked_a_بیتها = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::وکتور<صحيح> masked_b_بیتها = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			صحيح masked_ne = ez->vec_ne(masked_a_بیتها, masked_b_بیتها);
			صحيح undef_y_بیت = ez->AND(undef_any, ez->NOT(masked_ne));

			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->مجموعه_2(ez->CONST_FALSE, undef_y.at(i));
			ez->مجموعه_2(undef_y_بیت, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			إذا (model_undef) {
				std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		أرجع درست;
	}

	إذا (cell->type.درون(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($لغزش), ID($shiftx)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		صحيح extend_بیت = ez->CONST_FALSE;

		إذا (cell->parameters[ID::A_SIGNED].as_bool())
			extend_بیت = a.پشت();

		طالما (y.اندازه() < a.اندازه())
			y.افزودن_پشت(ez->literal());
		طالما (y.اندازه() > a.اندازه())
			a.افزودن_پشت(extend_بیت);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::وکتور<صحيح> shifted_a;

		إذا (cell->type.درون( ID($shl), ID($sshl)))
			shifted_a = ez->vec_لغزش_چپ(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($shr))
			shifted_a = ez->vec_لغزش_راست(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($sshr))
			shifted_a = ez->vec_لغزش_راست(a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? a.پشت() : ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type.درون(ID($لغزش), ID($shiftx)))
			shifted_a = ez->vec_لغزش_راست(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			std::وکتور<صحيح> undef_a_shifted;

			extend_بیت = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			إذا (cell->parameters[ID::A_SIGNED].as_bool())
				extend_بیت = undef_a.پشت();

			طالما (undef_y.اندازه() < undef_a.اندازه())
				undef_y.افزودن_پشت(ez->literal());
			طالما (undef_y.اندازه() > undef_a.اندازه())
				undef_a.افزودن_پشت(extend_بیت);

			إذا (cell->type.درون(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_لغزش_چپ(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shr))
				undef_a_shifted = ez->vec_لغزش_راست(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_لغزش_راست(undef_a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.پشت() : ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($لغزش))
				undef_a_shifted = ez->vec_لغزش_راست(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_لغزش_راست(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			صحيح undef_any_b = ez->عبارت_2(ezSAT::OpOr, undef_b);
			std::وکتور<صحيح> undef_all_y_بیتها(undef_y.اندازه(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_بیتها), undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($mul))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		extend_Signal_عرض(a, b, y, cell);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		std::وکتور<صحيح> tmp(a.اندازه(), ez->CONST_FALSE);
		لكل (صحيح i = 0; i < صحيح(a.اندازه()); i++)
		{
			std::وکتور<صحيح> shifted_a(a.اندازه(), ez->CONST_FALSE);
			لكل (صحيح j = i; j < صحيح(a.اندازه()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_جمع_کن(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type.درون(ID($macc), ID($macc_v2)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::وکتور<صحيح> tmp(Get_اندازه(y), ez->CONST_FALSE);

		لكل (خودکار &پورت : macc.terms)
		{
			std::وکتور<صحيح> درون_a = importDefSigSpec(پورت.درون_a, timestep);
			std::وکتور<صحيح> درون_b = importDefSigSpec(پورت.درون_b, timestep);

			طالما (Get_اندازه(درون_a) < Get_اندازه(y))
				درون_a.افزودن_پشت(پورت.is_signed && !درون_a.خالی() ? درون_a.پشت() : ez->CONST_FALSE);
			درون_a.resize(Get_اندازه(y));

			إذا (Get_اندازه(درون_b))
			{
				طالما (Get_اندازه(درون_b) < Get_اندازه(y))
					درون_b.افزودن_پشت(پورت.is_signed && !درون_b.خالی() ? درون_b.پشت() : ez->CONST_FALSE);
				درون_b.resize(Get_اندازه(y));

				لكل (صحيح i = 0; i < Get_اندازه(درون_b); i++) {
					std::وکتور<صحيح> shifted_a(درون_a.اندازه(), ez->CONST_FALSE);
					لكل (صحيح j = i; j < صحيح(درون_a.اندازه()); j++)
						shifted_a.at(j) = درون_a.at(j-i);
					إذا (پورت.do_تفریق)
						tmp = ez->vec_ite(درون_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					وإلا
						tmp = ez->vec_ite(درون_b.at(i), ez->vec_جمع_کن(tmp, shifted_a), tmp);
				}
			}
			وإلا
			{
				إذا (پورت.do_تفریق)
					tmp = ez->vec_sub(tmp, درون_a);
				وإلا
					tmp = ez->vec_جمع_کن(tmp, درون_a);
			}
		}

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_c;

			إذا (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_پورت(ID::C), timestep);

			صحيح undef_any_a = ez->عبارت_2(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->عبارت_2(ezSAT::OpOr, undef_b);
			صحيح undef_any_c = ez->عبارت_2(ezSAT::OpOr, undef_c);
			صحيح undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::وکتور<صحيح>(Get_اندازه(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		وإلا
			ez->assume(ez->vec_eq(y, tmp));

		أرجع درست;
	}

	إذا (cell->type.درون(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		extend_Signal_عرض(a, b, y, cell);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		std::وکتور<صحيح> a_u, b_u;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.پشت(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.پشت(), ez->vec_neg(b), b);
		} وإلا {
			a_u = a;
			b_u = b;
		}

		std::وکتور<صحيح> chain_buf = a_u;
		std::وکتور<صحيح> y_u(a_u.اندازه(), ez->CONST_FALSE);
		لكل (صحيح i = صحيح(a.اندازه())-1; i >= 0; i--)
		{
			chain_buf.درج(chain_buf.انتها(), chain_buf.اندازه(), ez->CONST_FALSE);

			std::وکتور<صحيح> b_shl(i, ez->CONST_FALSE);
			b_shl.درج(b_shl.انتها(), b_u.begin(), b_u.انتها());
			b_shl.درج(b_shl.انتها(), chain_buf.اندازه()-b_shl.اندازه(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.اندازه(), chain_buf.انتها());
		}

		std::وکتور<صحيح> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.اندازه());

		// modulo calculation
		std::وکتور<صحيح> modulo_trunc;
		صحيح floored_eq_trunc;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.پشت(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.پشت(), b.پشت()), ez->NOT(ez->عبارت_2(ezSAT::OpOr, modulo_trunc)));
		} وإلا {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		إذا (cell->type == ID($div)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.پشت(), b.پشت()), ez->vec_neg(y_u), y_u)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} وإلا إذا (cell->type == ID($divfloor)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.پشت(), b.پشت()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_جمع_کن(y_u, ez->vec_const_unsigned(1, y_u.اندازه())),
						y_u
					)),
					y_u
				)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_جمع_کن(modulo_trunc, b))));
		}

		إذا (ignore_div_by_zero) {
			ez->assume(ez->عبارت_2(ezSAT::OpOr, b));
		} وإلا {
			std::وکتور<صحيح> div_zero_نتیجه;
			إذا (cell->type.درون(ID($div), ID($divfloor))) {
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::وکتور<صحيح> all_ones(y.اندازه(), ez->CONST_TRUE);
					std::وکتور<صحيح> only_اولی_one(y.اندازه(), ez->CONST_FALSE);
					only_اولی_one.at(0) = ez->CONST_TRUE;
					div_zero_نتیجه = ez->vec_ite(a.پشت(), only_اولی_one, all_ones);
				} وإلا {
					div_zero_نتیجه.درج(div_zero_نتیجه.انتها(), cell->get_پورت(ID::A).اندازه(), ez->CONST_TRUE);
					div_zero_نتیجه.درج(div_zero_نتیجه.انتها(), y.اندازه() - div_zero_نتیجه.اندازه(), ez->CONST_FALSE);
				}
			} وإلا إذا (cell->type.درون(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				صحيح copy_a_بیتها = min(cell->get_پورت(ID::A).اندازه(), cell->get_پورت(ID::B).اندازه());
				div_zero_نتیجه.درج(div_zero_نتیجه.انتها(), a.begin(), a.begin() + copy_a_بیتها);
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_نتیجه.درج(div_zero_نتیجه.انتها(), y.اندازه() - div_zero_نتیجه.اندازه(), div_zero_نتیجه.پشت());
				وإلا
					div_zero_نتیجه.درج(div_zero_نتیجه.انتها(), y.اندازه() - div_zero_نتیجه.اندازه(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->عبارت_2(ezSAT::OpOr, b), y_tmp, div_zero_نتیجه)));
		}

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($lut))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		std::وکتور<صحيح> lut;
		لكل (خودکار بیت : cell->getParam(ID::LUT))
			lut.افزودن_پشت(بیت == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		طالما (Get_اندازه(lut) < (1 << Get_اندازه(a)))
			lut.افزودن_پشت(ez->CONST_FALSE);
		lut.resize(1 << Get_اندازه(a));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> t(lut), u(Get_اندازه(t), ez->CONST_FALSE);

			لكل (صحيح i = Get_اندازه(a)-1; i >= 0; i--)
			{
				std::وکتور<صحيح> t0(t.begin(), t.begin() + Get_اندازه(t)/2);
				std::وکتور<صحيح> t1(t.begin() + Get_اندازه(t)/2, t.انتها());

				std::وکتور<صحيح> u0(u.begin(), u.begin() + Get_اندازه(u)/2);
				std::وکتور<صحيح> u1(u.begin() + Get_اندازه(u)/2, u.انتها());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_اندازه(t) == 1);
			log_assert(Get_اندازه(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_پورت(ID::Y), timestep), u));
		}
		وإلا
		{
			std::وکتور<صحيح> t = lut;
			لكل (صحيح i = Get_اندازه(a)-1; i >= 0; i--)
			{
				std::وکتور<صحيح> t0(t.begin(), t.begin() + Get_اندازه(t)/2);
				std::وکتور<صحيح> t1(t.begin() + Get_اندازه(t)/2, t.انتها());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_اندازه(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		أرجع درست;
	}

	إذا (cell->type == ID($sop))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		صحيح y = importDefSigSpec(cell->get_پورت(ID::Y), timestep).at(0);

		صحيح عرض = cell->getParam(ID::عرض_2).as_int();
		صحيح عمق = cell->getParam(ID::عمق_2).as_int();

		وکتور<State> جدول_raw = cell->getParam(ID::جدول).مقصد_بیتها();
		طالما (Get_اندازه(جدول_raw) < 2*عرض*عمق)
			جدول_raw.افزودن_پشت(State::S0);

		وکتور<وکتور<صحيح>> جدول_2(عمق);

		لكل (صحيح i = 0; i < عمق; i++)
		لكل (صحيح j = 0; j < عرض; j++)
		{
			منطقي pat0 = (جدول_raw[2*عرض*i + 2*j + 0] == State::S1);
			منطقي pat1 = (جدول_raw[2*عرض*i + 2*j + 1] == State::S1);

			إذا (pat0 && !pat1)
				جدول_2.at(i).افزودن_پشت(0);
			وإلا إذا (!pat0 && pat1)
				جدول_2.at(i).افزودن_پشت(1);
			وإلا
				جدول_2.at(i).افزودن_پشت(-1);
		}

		إذا (model_undef)
		{
			std::وکتور<صحيح> products, undef_products;
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			صحيح undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep).at(0);

			لكل (صحيح i = 0; i < عمق; i++)
			{
				std::وکتور<صحيح> cmp_a, cmp_ua, cmp_b;

				لكل (صحيح j = 0; j < عرض; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.افزودن_پشت(a.at(j));
						cmp_ua.افزودن_پشت(undef_a.at(j));
						cmp_b.افزودن_پشت(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::وکتور<صحيح> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::وکتور<صحيح> masked_b = ez->vec_or(cmp_b, cmp_ua);

				صحيح masked_eq = ez->vec_eq(masked_a, masked_b);
				صحيح any_undef = ez->عبارت_2(ezSAT::OpOr, cmp_ua);

				undef_products.افزودن_پشت(ez->AND(any_undef, masked_eq));
				products.افزودن_پشت(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			صحيح yy = ez->عبارت_2(ezSAT::OpOr, products);
			ez->مجموعه_2(undef_y, ez->AND(ez->NOT(yy), ez->عبارت_2(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			std::وکتور<صحيح> products;

			لكل (صحيح i = 0; i < عمق; i++)
			{
				std::وکتور<صحيح> cmp_a, cmp_b;

				لكل (صحيح j = 0; j < عرض; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.افزودن_پشت(a.at(j));
						cmp_b.افزودن_پشت(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.افزودن_پشت(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->مجموعه_2(y, ez->عبارت_2(ezSAT::OpOr, products));
		}

		أرجع درست;
	}

	إذا (cell->type == ID($fa))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> c = importDefSigSpec(cell->get_پورت(ID::C), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		std::وکتور<صحيح> x = importDefSigSpec(cell->get_پورت(ID::X), timestep);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::وکتور<صحيح> xx = model_undef ? ez->vec_var(x.اندازه()) : x;

		std::وکتور<صحيح> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::وکتور<صحيح> t2 = ez->vec_and(a, b);
		std::وکتور<صحيح> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_c = importUndefSigSpec(cell->get_پورت(ID::C), timestep);

			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			std::وکتور<صحيح> undef_x = importUndefSigSpec(cell->get_پورت(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($lcu))
	{
		std::وکتور<صحيح> p = importDefSigSpec(cell->get_پورت(ID::P), timestep);
		std::وکتور<صحيح> g = importDefSigSpec(cell->get_پورت(ID::G), timestep);
		std::وکتور<صحيح> ci = importDefSigSpec(cell->get_پورت(ID::CI), timestep);
		std::وکتور<صحيح> co = importDefSigSpec(cell->get_پورت(ID::CO), timestep);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(co.اندازه()) : co;

		لكل (صحيح i = 0; i < Get_اندازه(co); i++)
			ez->مجموعه_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_p = importUndefSigSpec(cell->get_پورت(ID::P), timestep);
			std::وکتور<صحيح> undef_g = importUndefSigSpec(cell->get_پورت(ID::G), timestep);
			std::وکتور<صحيح> undef_ci = importUndefSigSpec(cell->get_پورت(ID::CI), timestep);
			std::وکتور<صحيح> undef_co = importUndefSigSpec(cell->get_پورت(ID::CO), timestep);

			صحيح undef_any_p = ez->عبارت_2(ezSAT::OpOr, undef_p);
			صحيح undef_any_g = ez->عبارت_2(ezSAT::OpOr, undef_g);
			صحيح undef_any_ci = ez->عبارت_2(ezSAT::OpOr, undef_ci);
			صحيح undef_co_بیت = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::وکتور<صحيح> undef_co_بیتها(undef_co.اندازه(), undef_co_بیت);
			ez->assume(ez->vec_eq(undef_co_بیتها, undef_co));

			undefGating(co, yy, undef_co);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($alu))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> b = importDefSigSpec(cell->get_پورت(ID::B), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		std::وکتور<صحيح> x = importDefSigSpec(cell->get_پورت(ID::X), timestep);
		std::وکتور<صحيح> ci = importDefSigSpec(cell->get_پورت(ID::CI), timestep);
		std::وکتور<صحيح> bi = importDefSigSpec(cell->get_پورت(ID::BI), timestep);
		std::وکتور<صحيح> co = importDefSigSpec(cell->get_پورت(ID::CO), timestep);

		extend_Signal_عرض(a, b, y, cell);
		extend_Signal_عرض(a, b, x, cell);
		extend_Signal_عرض(a, b, co, cell);

		std::وکتور<صحيح> def_y = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::وکتور<صحيح> def_x = model_undef ? ez->vec_var(x.اندازه()) : x;
		std::وکتور<صحيح> def_co = model_undef ? ez->vec_var(co.اندازه()) : co;

		log_assert(Get_اندازه(y) == Get_اندازه(x));
		log_assert(Get_اندازه(y) == Get_اندازه(co));
		log_assert(Get_اندازه(ci) == 1);
		log_assert(Get_اندازه(bi) == 1);

		لكل (صحيح i = 0; i < Get_اندازه(y); i++)
		{
			صحيح s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->مجموعه_2(def_x.at(i), ez->XOR(s1, s2));
			ez->مجموعه_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->مجموعه_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_b = importUndefSigSpec(cell->get_پورت(ID::B), timestep);
			std::وکتور<صحيح> undef_ci = importUndefSigSpec(cell->get_پورت(ID::CI), timestep);
			std::وکتور<صحيح> undef_bi = importUndefSigSpec(cell->get_پورت(ID::BI), timestep);

			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			std::وکتور<صحيح> undef_x = importUndefSigSpec(cell->get_پورت(ID::X), timestep);
			std::وکتور<صحيح> undef_co = importUndefSigSpec(cell->get_پورت(ID::CO), timestep);

			extend_Signal_عرض(undef_a, undef_b, undef_y, cell);
			extend_Signal_عرض(undef_a, undef_b, undef_x, cell);
			extend_Signal_عرض(undef_a, undef_b, undef_co, cell);

			std::وکتور<صحيح> all_inputs_undef;
			all_inputs_undef.درج(all_inputs_undef.انتها(), undef_a.begin(), undef_a.انتها());
			all_inputs_undef.درج(all_inputs_undef.انتها(), undef_b.begin(), undef_b.انتها());
			all_inputs_undef.درج(all_inputs_undef.انتها(), undef_ci.begin(), undef_ci.انتها());
			all_inputs_undef.درج(all_inputs_undef.انتها(), undef_bi.begin(), undef_bi.انتها());
			صحيح undef_any = ez->عبارت_2(ezSAT::OpOr, all_inputs_undef);

			لكل (صحيح i = 0; i < Get_اندازه(undef_y); i++) {
				ez->مجموعه_2(undef_y.at(i), undef_any);
				ez->مجموعه_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->مجموعه_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($برش))
	{
		RTLIL::SigSpec a = cell->get_پورت(ID::A);
		RTLIL::SigSpec y = cell->get_پورت(ID::Y);
		ez->assume(سیگنالها_eq(a.استخراج(cell->parameters.at(ID::جابه_جایی_2).as_int(), y.اندازه()), y, timestep));
		أرجع درست;
	}

	إذا (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_پورت(ID::A);
		RTLIL::SigSpec b = cell->get_پورت(ID::B);
		RTLIL::SigSpec y = cell->get_پورت(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(سیگنالها_eq(ab, y, timestep));
		أرجع درست;
	}

	إذا (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_داده ff(اشاره_تهی, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		إذا (ff.has_aload || ff.has_arst || ff.has_sr)
			أرجع خطأ;

		إذا (timestep == 1)
		{
			initial_state.جمع_کن((*sigmap)(cell->get_پورت(ID::Q)));
			إذا (model_undef && def_formal) {
				std::وکتور<صحيح> undef_q = importUndefSigSpec(cell->get_پورت(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		وإلا
		{
			std::وکتور<صحيح> d = importDefSigSpec(cell->get_پورت(ID::D), timestep-1);
			std::وکتور<صحيح> undef_d;
			إذا (model_undef)
				undef_d = importUndefSigSpec(cell->get_پورت(ID::D), timestep-1);
			إذا (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				صحيح srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::وکتور<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::وکتور<صحيح> undef_rval;
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
				std::وکتور<صحيح> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				صحيح undef_ce = -1;
				std::وکتور<صحيح> undef_old_q;
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
				std::وکتور<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::وکتور<صحيح> undef_rval;
				إذا (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				إذا (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				وإلا
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::وکتور<صحيح> q = importDefSigSpec(cell->get_پورت(ID::Q), timestep);

			std::وکتور<صحيح> qq = model_undef ? ez->vec_var(q.اندازه()) : q;
			ez->assume(ez->vec_eq(d, qq));

			إذا (model_undef)
			{
				std::وکتور<صحيح> undef_q = importUndefSigSpec(cell->get_پورت(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		أرجع درست;
	}

	إذا (cell->type == ID($anyconst))
	{
		إذا (timestep < 2) {
			إذا (model_undef && def_formal) {
				std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			أرجع درست;
		}

		std::وکتور<صحيح> d = importDefSigSpec(cell->get_پورت(ID::Y), timestep-1);
		std::وکتور<صحيح> q = importDefSigSpec(cell->get_پورت(ID::Y), timestep);

		std::وکتور<صحيح> qq = (model_undef && !def_formal) ? ez->vec_var(q.اندازه()) : q;
		ez->assume(ez->vec_eq(d, qq));

		إذا (model_undef)
		{
			std::وکتور<صحيح> undef_d = importUndefSigSpec(cell->get_پورت(ID::Y), timestep-1);
			std::وکتور<صحيح> undef_q = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);

			إذا (def_formal) {
				لكل (خودکار &undef_q_بیت : undef_q)
					ez->مجموعه_2(ez->CONST_FALSE, undef_q_بیت);
			} وإلا {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		أرجع درست;
	}

	إذا (cell->type == ID($anyseq))
	{
		إذا (model_undef && def_formal) {
			std::وکتور<صحيح> undef_q = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			لكل (خودکار &undef_q_بیت : undef_q)
				ez->مجموعه_2(ez->CONST_FALSE, undef_q_بیت);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($barrier) && !model_barriers)
		أرجع درست;

	إذا (cell->type.درون(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::وکتور<صحيح> a = importDefSigSpec(cell->get_پورت(ID::A), timestep);
		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		extend_Signal_عرض_Unary(a, y, cell);

		std::وکتور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		ez->assume(ez->vec_eq(a, yy));

		إذا (model_undef) {
			std::وکتور<صحيح> undef_a = importUndefSigSpec(cell->get_پورت(ID::A), timestep);
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			extend_Signal_عرض_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع درست;
	}

	إذا (cell->type == ID($initstate))
	{
		خودکار کلید = make_pair(پیشوند_2, timestep);
		إذا (initstates.شمار(کلید) == 0)
			initstates[کلید] = خطأ;

		std::وکتور<صحيح> y = importDefSigSpec(cell->get_پورت(ID::Y), timestep);
		log_assert(Get_اندازه(y) == 1);
		ez->مجموعه_2(y[0], initstates[کلید] ? ez->CONST_TRUE : ez->CONST_FALSE);

		إذا (model_undef) {
			std::وکتور<صحيح> undef_y = importUndefSigSpec(cell->get_پورت(ID::Y), timestep);
			log_assert(Get_اندازه(undef_y) == 1);
			ez->مجموعه_2(undef_y[0], ez->CONST_FALSE);
		}

		أرجع درست;
	}

	إذا (cell->type == ID($أكد))
	{
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_پورت(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_پورت(ID::EN)));
		أرجع درست;
	}

	إذا (cell->type == ID($assume))
	{
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_پورت(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_پورت(ID::EN)));
		أرجع درست;
	}

	إذا (cell->type == ID($scopeinfo) || cell->type == ID($درون_داد_پورت))
	{
		أرجع درست;
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
