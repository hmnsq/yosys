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
	منطقي is_arith_compare = cell->type.وچ(ID($lt), ID($le), ID($ge), ID($gt));

	إذا (model_undef && (cell->type.وچ(ID($جوڑو), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		إذا (is_arith_compare)
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);
		وإلا
			extend_Signal_چوڑائی(undef_a, undef_b, undef_y, cell, سچ);

		صحيح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
		صحيح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
		صحيح undef_y_ثنائی_ہندسہ = ez->OR(undef_any_a, undef_any_b);

		إذا (cell->type.وچ(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::سمتیہ<صحيح> b = importSigSpec(cell->get_پورٹ(ID::B), timestep);
			undef_y_ثنائی_ہندسہ = ez->OR(undef_y_ثنائی_ہندسہ, ez->NOT(ez->عبارت(ezSAT::OpOr, b)));
		}

		إذا (is_arith_compare) {
			لكل (ناپ_t i = 1; i < undef_y.ناپ(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سیٹ_2(undef_y_ثنائی_ہندسہ, undef_y.at(0));
		} وإلا {
			std::سمتیہ<صحيح> undef_y_بٹ(undef_y.ناپ(), undef_y_ثنائی_ہندسہ);
			ez->assume(ez->vec_eq(undef_y_بٹ, undef_y));
		}

		arith_undef_handled = سچ;
	}

	إذا (cell->type.وچ(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($جوڑو), ID($sub)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, y, cell);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		إذا (cell->type.وچ(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		إذا (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		إذا (cell->type.وچ(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		إذا (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		إذا (cell->type.وچ(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		إذا (cell->type.وچ(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		إذا (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($جوڑو))
			ez->assume(ez->vec_eq(ez->vec_جوڑو(a, b), yy));
		إذا (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		إذا (model_undef && !arith_undef_handled)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, undef_y, cell, خطأ);

			إذا (cell->type.وچ(ID($and), ID($_AND_), ID($_NAND_))) {
				std::سمتیہ<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::سمتیہ<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::سمتیہ<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.وچ(ID($or), ID($_OR_), ID($_NOR_))) {
				std::سمتیہ<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::سمتیہ<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::سمتیہ<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.وچ(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::سمتیہ<صحيح> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type == ID($_ANDNOT_)) {
				std::سمتیہ<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::سمتیہ<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::سمتیہ<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			وإلا إذا (cell->type == ID($_ORNOT_)) {
				std::سمتیہ<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::سمتیہ<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::سمتیہ<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا
				log_abort();

			undefGating(y, yy, undef_y);
		}
		وإلا إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		منطقي aoi_منوال = cell->type.وچ(ID($_AOI3_), ID($_AOI4_));
		منطقي three_منوال = cell->type.وچ(ID($_AOI3_), ID($_OAI3_));

		صحيح a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep).at(0);
		صحيح b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep).at(0);
		صحيح c = importDefSigSpec(cell->get_پورٹ(ID::C), timestep).at(0);
		صحيح d = three_منوال ? (aoi_منوال ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_پورٹ(ID::D), timestep).at(0);
		صحيح y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);
		صحيح yy = model_undef ? ez->literal() : y;

		إذا (cell->type.وچ(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		وإلا
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		إذا (model_undef)
		{
			صحيح undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep).at(0);
			صحيح undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep).at(0);
			صحيح undef_c = importUndefSigSpec(cell->get_پورٹ(ID::C), timestep).at(0);
			صحيح undef_d = three_منوال ? ez->CONST_FALSE : importUndefSigSpec(cell->get_پورٹ(ID::D), timestep).at(0);
			صحيح undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);

			إذا (aoi_منوال)
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

	إذا (cell->type.وچ(ID($_NOT_), ID($not)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی_Unary(a, y, cell);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		إذا (model_undef) {
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($bweqx))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحيح> bweqx = ez->vec_not(ez->vec_xor(a, b));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			std::سمتیہ<صحيح> both_undef = ez->vec_and(undef_a, undef_b);
			std::سمتیہ<صحيح> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			لكل (صحيح yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;
		إذا (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		وإلا إذا (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		وإلا
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			std::سمتیہ<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::سمتیہ<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::سمتیہ<صحيح> yX;
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
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		}

		إذا (Get_ناپ(s) == 0) {
			ez->vec_سیٹ(a, y);
			إذا (model_undef)
				ez->vec_سیٹ(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = Get_ناپ(s)-1; i >= 0; i--)
			{
				std::سمتیہ<صحيح> باہرلا = (i == 0) ? y : ez->vec_var(a.ناپ() / 2);
				std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(باہرلا.ناپ()) : باہرلا;

				std::سمتیہ<صحيح> a0(a.begin(), a.begin() + a.ناپ() / 2);
				std::سمتیہ<صحيح> a1(a.begin() + a.ناپ() / 2, a.سرا());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				إذا (model_undef)
				{
					std::سمتیہ<صحيح> undef_باہرلا = (i == 0) ? undef_y : ez->vec_var(a.ناپ() / 2);
					std::سمتیہ<صحيح> undef_a0(undef_a.begin(), undef_a.begin() + a.ناپ() / 2);
					std::سمتیہ<صحيح> undef_a1(undef_a.begin() + a.ناپ() / 2, undef_a.سرا());
					std::سمتیہ<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::سمتیہ<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::سمتیہ<صحيح> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_باہرلا));
					undefGating(باہرلا, yy, undef_باہرلا);

					undef_a = undef_باہرلا;
				}

				a = باہرلا;
			}
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($demux))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;
		std::سمتیہ<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		}

		إذا (Get_ناپ(s) == 0) {
			ez->vec_سیٹ(a, y);
			إذا (model_undef)
				ez->vec_سیٹ(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = 0; i < (1 << Get_ناپ(s)); i++)
			{
				std::سمتیہ<صحيح> ss;
				لكل (صحيح j = 0; j < Get_ناپ(s); j++) {
					إذا (i & 1 << j)
						ss.ڈالو_پٹھ(s[j]);
					وإلا
						ss.ڈالو_پٹھ(ez->NOT(s[j]));
				}
				صحيح sss = ez->عبارت(ezSAT::OpAnd, ss);

				لكل (صحيح j = 0; j < Get_ناپ(a); j++) {
					ez->سیٹ_2(ez->AND(sss, a[j]), yy.at(i * Get_ناپ(a) + j));
				}

				إذا (model_undef)
				{
					صحيح s0 = ez->عبارت(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					صحيح us = ez->AND(ez->NOT(s0), ez->عبارت(ezSAT::OpOr, undef_s));
					لكل (صحيح j = 0; j < Get_ناپ(a); j++) {
						صحيح a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						صحيح yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->سیٹ_2(yX, undef_y.at(i * Get_ناپ(a) + j));
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
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		std::سمتیہ<صحيح> tmp = a;
		لكل (ناپ_t i = 0; i < s.ناپ(); i++) {
			std::سمتیہ<صحيح> part_دا_b(b.begin()+i*a.ناپ(), b.begin()+(i+1)*a.ناپ());
			tmp = ez->vec_ite(s.at(i), part_دا_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			صحيح all_undef = ez->CONST_FALSE;
			صحيح لبھیا_سرگرم = ez->CONST_FALSE;

			std::سمتیہ<صحيح> undef_tmp = undef_a;

			لكل (ناپ_t i = 0; i < s.ناپ(); i++)
			{
				std::سمتیہ<صحيح> part_دا_undef_b(undef_b.begin()+i*a.ناپ(), undef_b.begin()+(i+1)*a.ناپ());

				undef_tmp = ez->vec_ite(s.at(i), part_دا_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), لبھیا_سرگرم));
				لبھیا_سرگرم = ez->OR(لبھیا_سرگرم, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::سمتیہ<صحيح>(a.ناپ(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($pos), ID($buf), ID($neg)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی_Unary(a, y, cell);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		إذا (cell->type.وچ(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} وإلا {
			std::سمتیہ<صحيح> zero(a.ناپ(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی_Unary(undef_a, undef_y, cell);

			إذا (cell->type.وچ(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} وإلا {
				صحيح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
				std::سمتیہ<صحيح> undef_y_بٹ(undef_y.ناپ(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_بٹ, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		إذا (cell->type == ID($reduce_and))
			ez->سیٹ_2(ez->عبارت(ez->OpAnd, a), yy.at(0));
		إذا (cell->type.وچ(ID($reduce_or), ID($reduce_bool)))
			ez->سیٹ_2(ez->عبارت(ez->OpOr, a), yy.at(0));
		إذا (cell->type == ID($reduce_xor))
			ez->سیٹ_2(ez->عبارت(ez->OpXor, a), yy.at(0));
		إذا (cell->type == ID($reduce_xnor))
			ez->سیٹ_2(ez->NOT(ez->عبارت(ez->OpXor, a)), yy.at(0));
		إذا (cell->type == ID($logic_not))
			ez->سیٹ_2(ez->NOT(ez->عبارت(ez->OpOr, a)), yy.at(0));
		لكل (ناپ_t i = 1; i < y.ناپ(); i++)
			ez->سیٹ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			صحيح aX = ez->عبارت(ezSAT::OpOr, undef_a);

			إذا (cell->type == ID($reduce_and)) {
				صحيح a0 = ez->عبارت(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.وچ(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				صحيح a1 = ez->عبارت(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.وچ(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} وإلا
				log_abort();

			لكل (ناپ_t i = 1; i < undef_y.ناپ(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($logic_and), ID($logic_or)))
	{
		std::سمتیہ<صحيح> vec_a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> vec_b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);

		صحيح a = ez->عبارت(ez->OpOr, vec_a);
		صحيح b = ez->عبارت(ez->OpOr, vec_b);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		إذا (cell->type == ID($logic_and))
			ez->سیٹ_2(ez->عبارت(ez->OpAnd, a, b), yy.at(0));
		وإلا
			ez->سیٹ_2(ez->عبارت(ez->OpOr, a, b), yy.at(0));
		لكل (ناپ_t i = 1; i < y.ناپ(); i++)
			ez->سیٹ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			صحيح a0 = ez->NOT(ez->OR(ez->عبارت(ezSAT::OpOr, vec_a), ez->عبارت(ezSAT::OpOr, undef_a)));
			صحيح b0 = ez->NOT(ez->OR(ez->عبارت(ezSAT::OpOr, vec_b), ez->عبارت(ezSAT::OpOr, undef_b)));
			صحيح a1 = ez->عبارت(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			صحيح b1 = ez->عبارت(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			صحيح aX = ez->عبارت(ezSAT::OpOr, undef_a);
			صحيح bX = ez->عبارت(ezSAT::OpOr, undef_b);

			إذا (cell->type == ID($logic_and))
				ez->سیٹ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			وإلا إذا (cell->type == ID($logic_or))
				ez->سیٹ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			وإلا
				log_abort();

			لكل (ناپ_t i = 1; i < undef_y.ناپ(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		منطقي is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, cell);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		إذا (model_undef && cell->type.وچ(ID($eqx), ID($nex))) {
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		إذا (cell->type == ID($lt))
			ez->سیٹ_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($le))
			ez->سیٹ_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		إذا (cell->type.وچ(ID($eq), ID($eqx)))
			ez->سیٹ_2(ez->vec_eq(a, b), yy.at(0));
		إذا (cell->type.وچ(ID($ne), ID($nex)))
			ez->سیٹ_2(ez->vec_ne(a, b), yy.at(0));
		إذا (cell->type == ID($ge))
			ez->سیٹ_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($gt))
			ez->سیٹ_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		لكل (ناپ_t i = 1; i < y.ناپ(); i++)
			ez->سیٹ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef && cell->type.وچ(ID($eqx), ID($nex)))
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);

			إذا (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			وإلا
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			لكل (ناپ_t i = 0; i < y.ناپ(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		وإلا إذا (model_undef && cell->type.وچ(ID($eq), ID($ne)))
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);

			صحيح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
			صحيح undef_any = ez->OR(undef_any_a, undef_any_b);

			std::سمتیہ<صحيح> masked_a_بٹ = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::سمتیہ<صحيح> masked_b_بٹ = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			صحيح masked_ne = ez->vec_ne(masked_a_بٹ, masked_b_بٹ);
			صحيح undef_y_ثنائی_ہندسہ = ez->AND(undef_any, ez->NOT(masked_ne));

			لكل (ناپ_t i = 1; i < undef_y.ناپ(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سیٹ_2(undef_y_ثنائی_ہندسہ, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			إذا (model_undef) {
				std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($کھسکاؤ), ID($shiftx)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		صحيح extend_ثنائی_ہندسہ = ez->CONST_FALSE;

		إذا (cell->parameters[ID::A_SIGNED].as_bool())
			extend_ثنائی_ہندسہ = a.پٹھ();

		طالما (y.ناپ() < a.ناپ())
			y.ڈالو_پٹھ(ez->literal());
		طالما (y.ناپ() > a.ناپ())
			a.ڈالو_پٹھ(extend_ثنائی_ہندسہ);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;
		std::سمتیہ<صحيح> shifted_a;

		إذا (cell->type.وچ( ID($shl), ID($sshl)))
			shifted_a = ez->vec_کھسکاؤ_بایاں(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($shr))
			shifted_a = ez->vec_کھسکاؤ_دایاں(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($sshr))
			shifted_a = ez->vec_کھسکاؤ_دایاں(a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? a.پٹھ() : ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type.وچ(ID($کھسکاؤ), ID($shiftx)))
			shifted_a = ez->vec_کھسکاؤ_دایاں(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			std::سمتیہ<صحيح> undef_a_shifted;

			extend_ثنائی_ہندسہ = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			إذا (cell->parameters[ID::A_SIGNED].as_bool())
				extend_ثنائی_ہندسہ = undef_a.پٹھ();

			طالما (undef_y.ناپ() < undef_a.ناپ())
				undef_y.ڈالو_پٹھ(ez->literal());
			طالما (undef_y.ناپ() > undef_a.ناپ())
				undef_a.ڈالو_پٹھ(extend_ثنائی_ہندسہ);

			إذا (cell->type.وچ(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_کھسکاؤ_بایاں(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shr))
				undef_a_shifted = ez->vec_کھسکاؤ_دایاں(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_کھسکاؤ_دایاں(undef_a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.پٹھ() : ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($کھسکاؤ))
				undef_a_shifted = ez->vec_کھسکاؤ_دایاں(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_کھسکاؤ_دایاں(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			صحيح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
			std::سمتیہ<صحيح> undef_all_y_بٹ(undef_y.ناپ(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_بٹ), undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($mul))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, y, cell);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		std::سمتیہ<صحيح> tmp(a.ناپ(), ez->CONST_FALSE);
		لكل (صحيح i = 0; i < صحيح(a.ناپ()); i++)
		{
			std::سمتیہ<صحيح> shifted_a(a.ناپ(), ez->CONST_FALSE);
			لكل (صحيح j = i; j < صحيح(a.ناپ()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_جوڑو(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($macc), ID($macc_v2)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::سمتیہ<صحيح> tmp(Get_ناپ(y), ez->CONST_FALSE);

		لكل (خودکار &پورٹ : macc.terms)
		{
			std::سمتیہ<صحيح> وچ_a = importDefSigSpec(پورٹ.وچ_a, timestep);
			std::سمتیہ<صحيح> وچ_b = importDefSigSpec(پورٹ.وچ_b, timestep);

			طالما (Get_ناپ(وچ_a) < Get_ناپ(y))
				وچ_a.ڈالو_پٹھ(پورٹ.is_signed && !وچ_a.خالی_تھاواں() ? وچ_a.پٹھ() : ez->CONST_FALSE);
			وچ_a.resize(Get_ناپ(y));

			إذا (Get_ناپ(وچ_b))
			{
				طالما (Get_ناپ(وچ_b) < Get_ناپ(y))
					وچ_b.ڈالو_پٹھ(پورٹ.is_signed && !وچ_b.خالی_تھاواں() ? وچ_b.پٹھ() : ez->CONST_FALSE);
				وچ_b.resize(Get_ناپ(y));

				لكل (صحيح i = 0; i < Get_ناپ(وچ_b); i++) {
					std::سمتیہ<صحيح> shifted_a(وچ_a.ناپ(), ez->CONST_FALSE);
					لكل (صحيح j = i; j < صحيح(وچ_a.ناپ()); j++)
						shifted_a.at(j) = وچ_a.at(j-i);
					إذا (پورٹ.do_گھٹاؤ)
						tmp = ez->vec_ite(وچ_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					وإلا
						tmp = ez->vec_ite(وچ_b.at(i), ez->vec_جوڑو(tmp, shifted_a), tmp);
				}
			}
			وإلا
			{
				إذا (پورٹ.do_گھٹاؤ)
					tmp = ez->vec_sub(tmp, وچ_a);
				وإلا
					tmp = ez->vec_جوڑو(tmp, وچ_a);
			}
		}

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_c;

			إذا (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_پورٹ(ID::C), timestep);

			صحيح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
			صحيح undef_any_c = ez->عبارت(ezSAT::OpOr, undef_c);
			صحيح undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::سمتیہ<صحيح>(Get_ناپ(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		وإلا
			ez->assume(ez->vec_eq(y, tmp));

		أرجع سچ;
	}

	إذا (cell->type.وچ(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, y, cell);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;

		std::سمتیہ<صحيح> a_u, b_u;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.پٹھ(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.پٹھ(), ez->vec_neg(b), b);
		} وإلا {
			a_u = a;
			b_u = b;
		}

		std::سمتیہ<صحيح> chain_buf = a_u;
		std::سمتیہ<صحيح> y_u(a_u.ناپ(), ez->CONST_FALSE);
		لكل (صحيح i = صحيح(a.ناپ())-1; i >= 0; i--)
		{
			chain_buf.داخل_کرو(chain_buf.سرا(), chain_buf.ناپ(), ez->CONST_FALSE);

			std::سمتیہ<صحيح> b_shl(i, ez->CONST_FALSE);
			b_shl.داخل_کرو(b_shl.سرا(), b_u.begin(), b_u.سرا());
			b_shl.داخل_کرو(b_shl.سرا(), chain_buf.ناپ()-b_shl.ناپ(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.ناپ(), chain_buf.سرا());
		}

		std::سمتیہ<صحيح> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.ناپ());

		// modulo calculation
		std::سمتیہ<صحيح> modulo_trunc;
		صحيح floored_eq_trunc;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.پٹھ(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.پٹھ(), b.پٹھ()), ez->NOT(ez->عبارت(ezSAT::OpOr, modulo_trunc)));
		} وإلا {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		إذا (cell->type == ID($div)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.پٹھ(), b.پٹھ()), ez->vec_neg(y_u), y_u)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} وإلا إذا (cell->type == ID($divfloor)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.پٹھ(), b.پٹھ()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_جوڑو(y_u, ez->vec_const_unsigned(1, y_u.ناپ())),
						y_u
					)),
					y_u
				)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_جوڑو(modulo_trunc, b))));
		}

		إذا (ignore_div_by_zero) {
			ez->assume(ez->عبارت(ezSAT::OpOr, b));
		} وإلا {
			std::سمتیہ<صحيح> div_zero_نتیجہ;
			إذا (cell->type.وچ(ID($div), ID($divfloor))) {
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::سمتیہ<صحيح> all_ones(y.ناپ(), ez->CONST_TRUE);
					std::سمتیہ<صحيح> only_پہلا_one(y.ناپ(), ez->CONST_FALSE);
					only_پہلا_one.at(0) = ez->CONST_TRUE;
					div_zero_نتیجہ = ez->vec_ite(a.پٹھ(), only_پہلا_one, all_ones);
				} وإلا {
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.سرا(), cell->get_پورٹ(ID::A).ناپ(), ez->CONST_TRUE);
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.سرا(), y.ناپ() - div_zero_نتیجہ.ناپ(), ez->CONST_FALSE);
				}
			} وإلا إذا (cell->type.وچ(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				صحيح copy_a_بٹ = min(cell->get_پورٹ(ID::A).ناپ(), cell->get_پورٹ(ID::B).ناپ());
				div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.سرا(), a.begin(), a.begin() + copy_a_بٹ);
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.سرا(), y.ناپ() - div_zero_نتیجہ.ناپ(), div_zero_نتیجہ.پٹھ());
				وإلا
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.سرا(), y.ناپ() - div_zero_نتیجہ.ناپ(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->عبارت(ezSAT::OpOr, b), y_tmp, div_zero_نتیجہ)));
		}

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($lut))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحيح> lut;
		لكل (خودکار ثنائی_ہندسہ : cell->getParam(ID::LUT))
			lut.ڈالو_پٹھ(ثنائی_ہندسہ == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		طالما (Get_ناپ(lut) < (1 << Get_ناپ(a)))
			lut.ڈالو_پٹھ(ez->CONST_FALSE);
		lut.resize(1 << Get_ناپ(a));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> t(lut), u(Get_ناپ(t), ez->CONST_FALSE);

			لكل (صحيح i = Get_ناپ(a)-1; i >= 0; i--)
			{
				std::سمتیہ<صحيح> t0(t.begin(), t.begin() + Get_ناپ(t)/2);
				std::سمتیہ<صحيح> t1(t.begin() + Get_ناپ(t)/2, t.سرا());

				std::سمتیہ<صحيح> u0(u.begin(), u.begin() + Get_ناپ(u)/2);
				std::سمتیہ<صحيح> u1(u.begin() + Get_ناپ(u)/2, u.سرا());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_ناپ(t) == 1);
			log_assert(Get_ناپ(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep), u));
		}
		وإلا
		{
			std::سمتیہ<صحيح> t = lut;
			لكل (صحيح i = Get_ناپ(a)-1; i >= 0; i--)
			{
				std::سمتیہ<صحيح> t0(t.begin(), t.begin() + Get_ناپ(t)/2);
				std::سمتیہ<صحيح> t1(t.begin() + Get_ناپ(t)/2, t.سرا());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_ناپ(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($sop))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		صحيح y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);

		صحيح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();
		صحيح ڈونگھائی = cell->getParam(ID::ڈونگھائی_2).as_int();

		سمتیہ<State> جدول_raw = cell->getParam(ID::جدول).منزل_بٹ();
		طالما (Get_ناپ(جدول_raw) < 2*چوڑائی*ڈونگھائی)
			جدول_raw.ڈالو_پٹھ(State::S0);

		سمتیہ<سمتیہ<صحيح>> جدول_2(ڈونگھائی);

		لكل (صحيح i = 0; i < ڈونگھائی; i++)
		لكل (صحيح j = 0; j < چوڑائی; j++)
		{
			منطقي pat0 = (جدول_raw[2*چوڑائی*i + 2*j + 0] == State::S1);
			منطقي pat1 = (جدول_raw[2*چوڑائی*i + 2*j + 1] == State::S1);

			إذا (pat0 && !pat1)
				جدول_2.at(i).ڈالو_پٹھ(0);
			وإلا إذا (!pat0 && pat1)
				جدول_2.at(i).ڈالو_پٹھ(1);
			وإلا
				جدول_2.at(i).ڈالو_پٹھ(-1);
		}

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> products, undef_products;
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			صحيح undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);

			لكل (صحيح i = 0; i < ڈونگھائی; i++)
			{
				std::سمتیہ<صحيح> cmp_a, cmp_ua, cmp_b;

				لكل (صحيح j = 0; j < چوڑائی; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ڈالو_پٹھ(a.at(j));
						cmp_ua.ڈالو_پٹھ(undef_a.at(j));
						cmp_b.ڈالو_پٹھ(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::سمتیہ<صحيح> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::سمتیہ<صحيح> masked_b = ez->vec_or(cmp_b, cmp_ua);

				صحيح masked_eq = ez->vec_eq(masked_a, masked_b);
				صحيح any_undef = ez->عبارت(ezSAT::OpOr, cmp_ua);

				undef_products.ڈالو_پٹھ(ez->AND(any_undef, masked_eq));
				products.ڈالو_پٹھ(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			صحيح yy = ez->عبارت(ezSAT::OpOr, products);
			ez->سیٹ_2(undef_y, ez->AND(ez->NOT(yy), ez->عبارت(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			std::سمتیہ<صحيح> products;

			لكل (صحيح i = 0; i < ڈونگھائی; i++)
			{
				std::سمتیہ<صحيح> cmp_a, cmp_b;

				لكل (صحيح j = 0; j < چوڑائی; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ڈالو_پٹھ(a.at(j));
						cmp_b.ڈالو_پٹھ(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.ڈالو_پٹھ(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->سیٹ_2(y, ez->عبارت(ezSAT::OpOr, products));
		}

		أرجع سچ;
	}

	إذا (cell->type == ID($fa))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> c = importDefSigSpec(cell->get_پورٹ(ID::C), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحيح> x = importDefSigSpec(cell->get_پورٹ(ID::X), timestep);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;
		std::سمتیہ<صحيح> xx = model_undef ? ez->vec_var(x.ناپ()) : x;

		std::سمتیہ<صحيح> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::سمتیہ<صحيح> t2 = ez->vec_and(a, b);
		std::سمتیہ<صحيح> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_c = importUndefSigSpec(cell->get_پورٹ(ID::C), timestep);

			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			std::سمتیہ<صحيح> undef_x = importUndefSigSpec(cell->get_پورٹ(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($lcu))
	{
		std::سمتیہ<صحيح> p = importDefSigSpec(cell->get_پورٹ(ID::P), timestep);
		std::سمتیہ<صحيح> g = importDefSigSpec(cell->get_پورٹ(ID::G), timestep);
		std::سمتیہ<صحيح> ci = importDefSigSpec(cell->get_پورٹ(ID::CI), timestep);
		std::سمتیہ<صحيح> co = importDefSigSpec(cell->get_پورٹ(ID::CO), timestep);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(co.ناپ()) : co;

		لكل (صحيح i = 0; i < Get_ناپ(co); i++)
			ez->سیٹ_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_p = importUndefSigSpec(cell->get_پورٹ(ID::P), timestep);
			std::سمتیہ<صحيح> undef_g = importUndefSigSpec(cell->get_پورٹ(ID::G), timestep);
			std::سمتیہ<صحيح> undef_ci = importUndefSigSpec(cell->get_پورٹ(ID::CI), timestep);
			std::سمتیہ<صحيح> undef_co = importUndefSigSpec(cell->get_پورٹ(ID::CO), timestep);

			صحيح undef_any_p = ez->عبارت(ezSAT::OpOr, undef_p);
			صحيح undef_any_g = ez->عبارت(ezSAT::OpOr, undef_g);
			صحيح undef_any_ci = ez->عبارت(ezSAT::OpOr, undef_ci);
			صحيح undef_co_ثنائی_ہندسہ = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::سمتیہ<صحيح> undef_co_بٹ(undef_co.ناپ(), undef_co_ثنائی_ہندسہ);
			ez->assume(ez->vec_eq(undef_co_بٹ, undef_co));

			undefGating(co, yy, undef_co);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($alu))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحيح> x = importDefSigSpec(cell->get_پورٹ(ID::X), timestep);
		std::سمتیہ<صحيح> ci = importDefSigSpec(cell->get_پورٹ(ID::CI), timestep);
		std::سمتیہ<صحيح> bi = importDefSigSpec(cell->get_پورٹ(ID::BI), timestep);
		std::سمتیہ<صحيح> co = importDefSigSpec(cell->get_پورٹ(ID::CO), timestep);

		extend_Signal_چوڑائی(a, b, y, cell);
		extend_Signal_چوڑائی(a, b, x, cell);
		extend_Signal_چوڑائی(a, b, co, cell);

		std::سمتیہ<صحيح> def_y = model_undef ? ez->vec_var(y.ناپ()) : y;
		std::سمتیہ<صحيح> def_x = model_undef ? ez->vec_var(x.ناپ()) : x;
		std::سمتیہ<صحيح> def_co = model_undef ? ez->vec_var(co.ناپ()) : co;

		log_assert(Get_ناپ(y) == Get_ناپ(x));
		log_assert(Get_ناپ(y) == Get_ناپ(co));
		log_assert(Get_ناپ(ci) == 1);
		log_assert(Get_ناپ(bi) == 1);

		لكل (صحيح i = 0; i < Get_ناپ(y); i++)
		{
			صحيح s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->سیٹ_2(def_x.at(i), ez->XOR(s1, s2));
			ez->سیٹ_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->سیٹ_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحيح> undef_ci = importUndefSigSpec(cell->get_پورٹ(ID::CI), timestep);
			std::سمتیہ<صحيح> undef_bi = importUndefSigSpec(cell->get_پورٹ(ID::BI), timestep);

			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			std::سمتیہ<صحيح> undef_x = importUndefSigSpec(cell->get_پورٹ(ID::X), timestep);
			std::سمتیہ<صحيح> undef_co = importUndefSigSpec(cell->get_پورٹ(ID::CO), timestep);

			extend_Signal_چوڑائی(undef_a, undef_b, undef_y, cell);
			extend_Signal_چوڑائی(undef_a, undef_b, undef_x, cell);
			extend_Signal_چوڑائی(undef_a, undef_b, undef_co, cell);

			std::سمتیہ<صحيح> all_inputs_undef;
			all_inputs_undef.داخل_کرو(all_inputs_undef.سرا(), undef_a.begin(), undef_a.سرا());
			all_inputs_undef.داخل_کرو(all_inputs_undef.سرا(), undef_b.begin(), undef_b.سرا());
			all_inputs_undef.داخل_کرو(all_inputs_undef.سرا(), undef_ci.begin(), undef_ci.سرا());
			all_inputs_undef.داخل_کرو(all_inputs_undef.سرا(), undef_bi.begin(), undef_bi.سرا());
			صحيح undef_any = ez->عبارت(ezSAT::OpOr, all_inputs_undef);

			لكل (صحيح i = 0; i < Get_ناپ(undef_y); i++) {
				ez->سیٹ_2(undef_y.at(i), undef_any);
				ez->سیٹ_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->سیٹ_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($ٹکڑا))
	{
		RTLIL::SigSpec a = cell->get_پورٹ(ID::A);
		RTLIL::SigSpec y = cell->get_پورٹ(ID::Y);
		ez->assume(اشارے_eq(a.کڈھو(cell->parameters.at(ID::وتھ_2).as_int(), y.ناپ()), y, timestep));
		أرجع سچ;
	}

	إذا (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_پورٹ(ID::A);
		RTLIL::SigSpec b = cell->get_پورٹ(ID::B);
		RTLIL::SigSpec y = cell->get_پورٹ(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(اشارے_eq(ab, y, timestep));
		أرجع سچ;
	}

	إذا (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_انکڑے ff(خالی_سوچک, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		إذا (ff.has_aload || ff.has_arst || ff.has_sr)
			أرجع خطأ;

		إذا (timestep == 1)
		{
			initial_state.جوڑو((*sigmap)(cell->get_پورٹ(ID::Q)));
			إذا (model_undef && def_formal) {
				std::سمتیہ<صحيح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		وإلا
		{
			std::سمتیہ<صحيح> d = importDefSigSpec(cell->get_پورٹ(ID::D), timestep-1);
			std::سمتیہ<صحيح> undef_d;
			إذا (model_undef)
				undef_d = importUndefSigSpec(cell->get_پورٹ(ID::D), timestep-1);
			إذا (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				صحيح srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::سمتیہ<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::سمتیہ<صحيح> undef_rval;
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
				std::سمتیہ<صحيح> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				صحيح undef_ce = -1;
				std::سمتیہ<صحيح> undef_old_q;
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
				std::سمتیہ<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::سمتیہ<صحيح> undef_rval;
				إذا (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				إذا (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				وإلا
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::سمتیہ<صحيح> q = importDefSigSpec(cell->get_پورٹ(ID::Q), timestep);

			std::سمتیہ<صحيح> qq = model_undef ? ez->vec_var(q.ناپ()) : q;
			ez->assume(ez->vec_eq(d, qq));

			إذا (model_undef)
			{
				std::سمتیہ<صحيح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Q), timestep);

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
				std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			أرجع سچ;
		}

		std::سمتیہ<صحيح> d = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep-1);
		std::سمتیہ<صحيح> q = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحيح> qq = (model_undef && !def_formal) ? ez->vec_var(q.ناپ()) : q;
		ez->assume(ez->vec_eq(d, qq));

		إذا (model_undef)
		{
			std::سمتیہ<صحيح> undef_d = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep-1);
			std::سمتیہ<صحيح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			إذا (def_formal) {
				لكل (خودکار &undef_q_ثنائی_ہندسہ : undef_q)
					ez->سیٹ_2(ez->CONST_FALSE, undef_q_ثنائی_ہندسہ);
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
			std::سمتیہ<صحيح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			لكل (خودکار &undef_q_ثنائی_ہندسہ : undef_q)
				ez->سیٹ_2(ez->CONST_FALSE, undef_q_ثنائی_ہندسہ);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($barrier) && !model_barriers)
		أرجع سچ;

	إذا (cell->type.وچ(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::سمتیہ<صحيح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی_Unary(a, y, cell);

		std::سمتیہ<صحيح> yy = model_undef ? ez->vec_var(y.ناپ()) : y;
		ez->assume(ez->vec_eq(a, yy));

		إذا (model_undef) {
			std::سمتیہ<صحيح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع سچ;
	}

	إذا (cell->type == ID($initstate))
	{
		خودکار کلید = make_pair(اگیتر_2, timestep);
		إذا (initstates.گنتی(کلید) == 0)
			initstates[کلید] = خطأ;

		std::سمتیہ<صحيح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		log_assert(Get_ناپ(y) == 1);
		ez->سیٹ_2(y[0], initstates[کلید] ? ez->CONST_TRUE : ez->CONST_FALSE);

		إذا (model_undef) {
			std::سمتیہ<صحيح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			log_assert(Get_ناپ(undef_y) == 1);
			ez->سیٹ_2(undef_y[0], ez->CONST_FALSE);
		}

		أرجع سچ;
	}

	إذا (cell->type == ID($أكد))
	{
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_پورٹ(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_پورٹ(ID::EN)));
		أرجع سچ;
	}

	إذا (cell->type == ID($assume))
	{
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_پورٹ(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_پورٹ(ID::EN)));
		أرجع سچ;
	}

	إذا (cell->type == ID($scopeinfo) || cell->type == ID($داخلہ_پورٹ))
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
