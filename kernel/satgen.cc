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
	منطقي is_arith_compare = cell->type.دننه(ID($lt), ID($le), ID($ge), ID($gt));

	إذا (model_undef && (cell->type.دننه(ID($جمع_کړه), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
		إذا (is_arith_compare)
			extend_Signal_پلنوالی(undef_a, undef_b, cell, ریښتیا);
		وإلا
			extend_Signal_پلنوالی(undef_a, undef_b, undef_y, cell, ریښتیا);

		صحيح undef_any_a = ez->افاده(ezSAT::OpOr, undef_a);
		صحيح undef_any_b = ez->افاده(ezSAT::OpOr, undef_b);
		صحيح undef_y_بټ = ez->OR(undef_any_a, undef_any_b);

		إذا (cell->type.دننه(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::ویکټور<صحيح> b = importSigSpec(cell->get_پورټ(ID::B), timestep);
			undef_y_بټ = ez->OR(undef_y_بټ, ez->NOT(ez->افاده(ezSAT::OpOr, b)));
		}

		إذا (is_arith_compare) {
			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->سېټ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سېټ_2(undef_y_بټ, undef_y.at(0));
		} وإلا {
			std::ویکټور<صحيح> undef_y_بټونه(undef_y.اندازه(), undef_y_بټ);
			ez->assume(ez->vec_eq(undef_y_بټونه, undef_y));
		}

		arith_undef_handled = ریښتیا;
	}

	إذا (cell->type.دننه(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($جمع_کړه), ID($sub)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		extend_Signal_پلنوالی(a, b, y, cell);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type.دننه(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		إذا (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		إذا (cell->type.دننه(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		إذا (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		إذا (cell->type.دننه(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		إذا (cell->type.دننه(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		إذا (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		إذا (cell->type == ID($جمع_کړه))
			ez->assume(ez->vec_eq(ez->vec_جمع_کړه(a, b), yy));
		إذا (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		إذا (model_undef && !arith_undef_handled)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			extend_Signal_پلنوالی(undef_a, undef_b, undef_y, cell, خطأ);

			إذا (cell->type.دننه(ID($and), ID($_AND_), ID($_NAND_))) {
				std::ویکټور<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ویکټور<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ویکټور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.دننه(ID($or), ID($_OR_), ID($_NOR_))) {
				std::ویکټور<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ویکټور<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ویکټور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type.دننه(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::ویکټور<صحيح> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا إذا (cell->type == ID($_ANDNOT_)) {
				std::ویکټور<صحيح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::ویکټور<صحيح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::ویکټور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			وإلا إذا (cell->type == ID($_ORNOT_)) {
				std::ویکټور<صحيح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::ویکټور<صحيح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::ویکټور<صحيح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			وإلا
				log_abort();

			undefGating(y, yy, undef_y);
		}
		وإلا إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		منطقي aoi_ډېرپېښ = cell->type.دننه(ID($_AOI3_), ID($_AOI4_));
		منطقي three_ډېرپېښ = cell->type.دننه(ID($_AOI3_), ID($_OAI3_));

		صحيح a = importDefSigSpec(cell->get_پورټ(ID::A), timestep).at(0);
		صحيح b = importDefSigSpec(cell->get_پورټ(ID::B), timestep).at(0);
		صحيح c = importDefSigSpec(cell->get_پورټ(ID::C), timestep).at(0);
		صحيح d = three_ډېرپېښ ? (aoi_ډېرپېښ ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_پورټ(ID::D), timestep).at(0);
		صحيح y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep).at(0);
		صحيح yy = model_undef ? ez->literal() : y;

		إذا (cell->type.دننه(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		وإلا
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		إذا (model_undef)
		{
			صحيح undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep).at(0);
			صحيح undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep).at(0);
			صحيح undef_c = importUndefSigSpec(cell->get_پورټ(ID::C), timestep).at(0);
			صحيح undef_d = three_ډېرپېښ ? ez->CONST_FALSE : importUndefSigSpec(cell->get_پورټ(ID::D), timestep).at(0);
			صحيح undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep).at(0);

			إذا (aoi_ډېرپېښ)
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

		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($_NOT_), ID($not)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		extend_Signal_پلنوالی_Unary(a, y, cell);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		إذا (model_undef) {
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			extend_Signal_پلنوالی_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($bweqx))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		std::ویکټور<صحيح> bweqx = ez->vec_not(ez->vec_xor(a, b));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);

			std::ویکټور<صحيح> both_undef = ez->vec_and(undef_a, undef_b);
			std::ویکټور<صحيح> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			لكل (صحيح yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> s = importDefSigSpec(cell->get_پورټ(ID::S), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		إذا (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		وإلا إذا (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		وإلا
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_s = importUndefSigSpec(cell->get_پورټ(ID::S), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);

			std::ویکټور<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ویکټور<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::ویکټور<صحيح> yX;
			إذا (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			وإلا
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($bmux))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> s = importDefSigSpec(cell->get_پورټ(ID::S), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		std::ویکټور<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورټ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
		}

		إذا (Get_اندازه(s) == 0) {
			ez->vec_سېټ(a, y);
			إذا (model_undef)
				ez->vec_سېټ(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = Get_اندازه(s)-1; i >= 0; i--)
			{
				std::ویکټور<صحيح> بهر = (i == 0) ? y : ez->vec_var(a.اندازه() / 2);
				std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(بهر.اندازه()) : بهر;

				std::ویکټور<صحيح> a0(a.begin(), a.begin() + a.اندازه() / 2);
				std::ویکټور<صحيح> a1(a.begin() + a.اندازه() / 2, a.پای_ټکی());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				إذا (model_undef)
				{
					std::ویکټور<صحيح> undef_بهر = (i == 0) ? undef_y : ez->vec_var(a.اندازه() / 2);
					std::ویکټور<صحيح> undef_a0(undef_a.begin(), undef_a.begin() + a.اندازه() / 2);
					std::ویکټور<صحيح> undef_a1(undef_a.begin() + a.اندازه() / 2, undef_a.پای_ټکی());
					std::ویکټور<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::ویکټور<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::ویکټور<صحيح> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_بهر));
					undefGating(بهر, yy, undef_بهر);

					undef_a = undef_بهر;
				}

				a = بهر;
			}
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($demux))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> s = importDefSigSpec(cell->get_پورټ(ID::S), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::ویکټور<صحيح> undef_a, undef_s, undef_y;

		إذا (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورټ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
		}

		إذا (Get_اندازه(s) == 0) {
			ez->vec_سېټ(a, y);
			إذا (model_undef)
				ez->vec_سېټ(undef_a, undef_y);
		} وإلا {
			لكل (صحيح i = 0; i < (1 << Get_اندازه(s)); i++)
			{
				std::ویکټور<صحيح> ss;
				لكل (صحيح j = 0; j < Get_اندازه(s); j++) {
					إذا (i & 1 << j)
						ss.ټېل_شا(s[j]);
					وإلا
						ss.ټېل_شا(ez->NOT(s[j]));
				}
				صحيح sss = ez->افاده(ezSAT::OpAnd, ss);

				لكل (صحيح j = 0; j < Get_اندازه(a); j++) {
					ez->سېټ_2(ez->AND(sss, a[j]), yy.at(i * Get_اندازه(a) + j));
				}

				إذا (model_undef)
				{
					صحيح s0 = ez->افاده(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					صحيح us = ez->AND(ez->NOT(s0), ez->افاده(ezSAT::OpOr, undef_s));
					لكل (صحيح j = 0; j < Get_اندازه(a); j++) {
						صحيح a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						صحيح yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->سېټ_2(yX, undef_y.at(i * Get_اندازه(a) + j));
					}
				}
			}
			إذا (model_undef)
				undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($pmux))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> s = importDefSigSpec(cell->get_پورټ(ID::S), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		std::ویکټور<صحيح> tmp = a;
		لكل (اندازه_t i = 0; i < s.اندازه(); i++) {
			std::ویکټور<صحيح> part_د_b(b.begin()+i*a.اندازه(), b.begin()+(i+1)*a.اندازه());
			tmp = ez->vec_ite(s.at(i), part_د_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_s = importUndefSigSpec(cell->get_پورټ(ID::S), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);

			صحيح all_undef = ez->CONST_FALSE;
			صحيح وموندل_فعال = ez->CONST_FALSE;

			std::ویکټور<صحيح> undef_tmp = undef_a;

			لكل (اندازه_t i = 0; i < s.اندازه(); i++)
			{
				std::ویکټور<صحيح> part_د_undef_b(undef_b.begin()+i*a.اندازه(), undef_b.begin()+(i+1)*a.اندازه());

				undef_tmp = ez->vec_ite(s.at(i), part_د_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), وموندل_فعال));
				وموندل_فعال = ez->OR(وموندل_فعال, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::ویکټور<صحيح>(a.اندازه(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($pos), ID($buf), ID($neg)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		extend_Signal_پلنوالی_Unary(a, y, cell);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type.دننه(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} وإلا {
			std::ویکټور<صحيح> zero(a.اندازه(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			extend_Signal_پلنوالی_Unary(undef_a, undef_y, cell);

			إذا (cell->type.دننه(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} وإلا {
				صحيح undef_any_a = ez->افاده(ezSAT::OpOr, undef_a);
				std::ویکټور<صحيح> undef_y_بټونه(undef_y.اندازه(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_بټونه, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type == ID($reduce_and))
			ez->سېټ_2(ez->افاده(ez->OpAnd, a), yy.at(0));
		إذا (cell->type.دننه(ID($reduce_or), ID($reduce_bool)))
			ez->سېټ_2(ez->افاده(ez->OpOr, a), yy.at(0));
		إذا (cell->type == ID($reduce_xor))
			ez->سېټ_2(ez->افاده(ez->OpXor, a), yy.at(0));
		إذا (cell->type == ID($reduce_xnor))
			ez->سېټ_2(ez->NOT(ez->افاده(ez->OpXor, a)), yy.at(0));
		إذا (cell->type == ID($logic_not))
			ez->سېټ_2(ez->NOT(ez->افاده(ez->OpOr, a)), yy.at(0));
		لكل (اندازه_t i = 1; i < y.اندازه(); i++)
			ez->سېټ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			صحيح aX = ez->افاده(ezSAT::OpOr, undef_a);

			إذا (cell->type == ID($reduce_and)) {
				صحيح a0 = ez->افاده(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.دننه(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				صحيح a1 = ez->افاده(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			وإلا إذا (cell->type.دننه(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} وإلا
				log_abort();

			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->سېټ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($logic_and), ID($logic_or)))
	{
		std::ویکټور<صحيح> vec_a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> vec_b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);

		صحيح a = ez->افاده(ez->OpOr, vec_a);
		صحيح b = ez->افاده(ez->OpOr, vec_b);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (cell->type == ID($logic_and))
			ez->سېټ_2(ez->افاده(ez->OpAnd, a, b), yy.at(0));
		وإلا
			ez->سېټ_2(ez->افاده(ez->OpOr, a, b), yy.at(0));
		لكل (اندازه_t i = 1; i < y.اندازه(); i++)
			ez->سېټ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);

			صحيح a0 = ez->NOT(ez->OR(ez->افاده(ezSAT::OpOr, vec_a), ez->افاده(ezSAT::OpOr, undef_a)));
			صحيح b0 = ez->NOT(ez->OR(ez->افاده(ezSAT::OpOr, vec_b), ez->افاده(ezSAT::OpOr, undef_b)));
			صحيح a1 = ez->افاده(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			صحيح b1 = ez->افاده(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			صحيح aX = ez->افاده(ezSAT::OpOr, undef_a);
			صحيح bX = ez->افاده(ezSAT::OpOr, undef_b);

			إذا (cell->type == ID($logic_and))
				ez->سېټ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			وإلا إذا (cell->type == ID($logic_or))
				ez->سېټ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			وإلا
				log_abort();

			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->سېټ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		منطقي is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		extend_Signal_پلنوالی(a, b, cell);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		إذا (model_undef && cell->type.دننه(ID($eqx), ID($nex))) {
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			extend_Signal_پلنوالی(undef_a, undef_b, cell, ریښتیا);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		إذا (cell->type == ID($lt))
			ez->سېټ_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($le))
			ez->سېټ_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		إذا (cell->type.دننه(ID($eq), ID($eqx)))
			ez->سېټ_2(ez->vec_eq(a, b), yy.at(0));
		إذا (cell->type.دننه(ID($ne), ID($nex)))
			ez->سېټ_2(ez->vec_ne(a, b), yy.at(0));
		إذا (cell->type == ID($ge))
			ez->سېټ_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		إذا (cell->type == ID($gt))
			ez->سېټ_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		لكل (اندازه_t i = 1; i < y.اندازه(); i++)
			ez->سېټ_2(ez->CONST_FALSE, yy.at(i));

		إذا (model_undef && cell->type.دننه(ID($eqx), ID($nex)))
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			extend_Signal_پلنوالی(undef_a, undef_b, cell, ریښتیا);

			إذا (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			وإلا
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			لكل (اندازه_t i = 0; i < y.اندازه(); i++)
				ez->سېټ_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		وإلا إذا (model_undef && cell->type.دننه(ID($eq), ID($ne)))
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			extend_Signal_پلنوالی(undef_a, undef_b, cell, ریښتیا);

			صحيح undef_any_a = ez->افاده(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->افاده(ezSAT::OpOr, undef_b);
			صحيح undef_any = ez->OR(undef_any_a, undef_any_b);

			std::ویکټور<صحيح> masked_a_بټونه = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::ویکټور<صحيح> masked_b_بټونه = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			صحيح masked_ne = ez->vec_ne(masked_a_بټونه, masked_b_بټونه);
			صحيح undef_y_بټ = ez->AND(undef_any, ez->NOT(masked_ne));

			لكل (اندازه_t i = 1; i < undef_y.اندازه(); i++)
				ez->سېټ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سېټ_2(undef_y_بټ, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			إذا (model_undef) {
				std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ښويدنه), ID($shiftx)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		صحيح extend_بټ = ez->CONST_FALSE;

		إذا (cell->parameters[ID::A_SIGNED].as_bool())
			extend_بټ = a.شا();

		طالما (y.اندازه() < a.اندازه())
			y.ټېل_شا(ez->literal());
		طالما (y.اندازه() > a.اندازه())
			a.ټېل_شا(extend_بټ);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::ویکټور<صحيح> shifted_a;

		إذا (cell->type.دننه( ID($shl), ID($sshl)))
			shifted_a = ez->vec_ښويدنه_کیڼ(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($shr))
			shifted_a = ez->vec_ښويدنه_ښی(a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type == ID($sshr))
			shifted_a = ez->vec_ښويدنه_ښی(a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? a.شا() : ez->CONST_FALSE, ez->CONST_FALSE);

		إذا (cell->type.دننه(ID($ښويدنه), ID($shiftx)))
			shifted_a = ez->vec_ښويدنه_ښی(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			std::ویکټور<صحيح> undef_a_shifted;

			extend_بټ = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			إذا (cell->parameters[ID::A_SIGNED].as_bool())
				extend_بټ = undef_a.شا();

			طالما (undef_y.اندازه() < undef_a.اندازه())
				undef_y.ټېل_شا(ez->literal());
			طالما (undef_y.اندازه() > undef_a.اندازه())
				undef_a.ټېل_شا(extend_بټ);

			إذا (cell->type.دننه(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_ښويدنه_کیڼ(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shr))
				undef_a_shifted = ez->vec_ښويدنه_ښی(undef_a, b, خطأ, ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_ښويدنه_ښی(undef_a, b, خطأ, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.شا() : ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($ښويدنه))
				undef_a_shifted = ez->vec_ښويدنه_ښی(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			إذا (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_ښويدنه_ښی(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			صحيح undef_any_b = ez->افاده(ezSAT::OpOr, undef_b);
			std::ویکټور<صحيح> undef_all_y_بټونه(undef_y.اندازه(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_بټونه), undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($mul))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		extend_Signal_پلنوالی(a, b, y, cell);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		std::ویکټور<صحيح> tmp(a.اندازه(), ez->CONST_FALSE);
		لكل (صحيح i = 0; i < صحيح(a.اندازه()); i++)
		{
			std::ویکټور<صحيح> shifted_a(a.اندازه(), ez->CONST_FALSE);
			لكل (صحيح j = i; j < صحيح(a.اندازه()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_جمع_کړه(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($macc), ID($macc_v2)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::ویکټور<صحيح> tmp(Get_اندازه(y), ez->CONST_FALSE);

		لكل (خپلکاری &پورټ : macc.terms)
		{
			std::ویکټور<صحيح> دننه_a = importDefSigSpec(پورټ.دننه_a, timestep);
			std::ویکټور<صحيح> دننه_b = importDefSigSpec(پورټ.دننه_b, timestep);

			طالما (Get_اندازه(دننه_a) < Get_اندازه(y))
				دننه_a.ټېل_شا(پورټ.is_signed && !دننه_a.تشه() ? دننه_a.شا() : ez->CONST_FALSE);
			دننه_a.resize(Get_اندازه(y));

			إذا (Get_اندازه(دننه_b))
			{
				طالما (Get_اندازه(دننه_b) < Get_اندازه(y))
					دننه_b.ټېل_شا(پورټ.is_signed && !دننه_b.تشه() ? دننه_b.شا() : ez->CONST_FALSE);
				دننه_b.resize(Get_اندازه(y));

				لكل (صحيح i = 0; i < Get_اندازه(دننه_b); i++) {
					std::ویکټور<صحيح> shifted_a(دننه_a.اندازه(), ez->CONST_FALSE);
					لكل (صحيح j = i; j < صحيح(دننه_a.اندازه()); j++)
						shifted_a.at(j) = دننه_a.at(j-i);
					إذا (پورټ.do_کمول)
						tmp = ez->vec_ite(دننه_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					وإلا
						tmp = ez->vec_ite(دننه_b.at(i), ez->vec_جمع_کړه(tmp, shifted_a), tmp);
				}
			}
			وإلا
			{
				إذا (پورټ.do_کمول)
					tmp = ez->vec_sub(tmp, دننه_a);
				وإلا
					tmp = ez->vec_جمع_کړه(tmp, دننه_a);
			}
		}

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_c;

			إذا (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_پورټ(ID::C), timestep);

			صحيح undef_any_a = ez->افاده(ezSAT::OpOr, undef_a);
			صحيح undef_any_b = ez->افاده(ezSAT::OpOr, undef_b);
			صحيح undef_any_c = ez->افاده(ezSAT::OpOr, undef_c);
			صحيح undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::ویکټور<صحيح>(Get_اندازه(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		وإلا
			ez->assume(ez->vec_eq(y, tmp));

		أرجع ریښتیا;
	}

	إذا (cell->type.دننه(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		extend_Signal_پلنوالی(a, b, y, cell);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;

		std::ویکټور<صحيح> a_u, b_u;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.شا(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.شا(), ez->vec_neg(b), b);
		} وإلا {
			a_u = a;
			b_u = b;
		}

		std::ویکټور<صحيح> chain_buf = a_u;
		std::ویکټور<صحيح> y_u(a_u.اندازه(), ez->CONST_FALSE);
		لكل (صحيح i = صحيح(a.اندازه())-1; i >= 0; i--)
		{
			chain_buf.ورننباسه(chain_buf.پای_ټکی(), chain_buf.اندازه(), ez->CONST_FALSE);

			std::ویکټور<صحيح> b_shl(i, ez->CONST_FALSE);
			b_shl.ورننباسه(b_shl.پای_ټکی(), b_u.begin(), b_u.پای_ټکی());
			b_shl.ورننباسه(b_shl.پای_ټکی(), chain_buf.اندازه()-b_shl.اندازه(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.اندازه(), chain_buf.پای_ټکی());
		}

		std::ویکټور<صحيح> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.اندازه());

		// modulo calculation
		std::ویکټور<صحيح> modulo_trunc;
		صحيح floored_eq_trunc;
		إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.شا(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.شا(), b.شا()), ez->NOT(ez->افاده(ezSAT::OpOr, modulo_trunc)));
		} وإلا {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		إذا (cell->type == ID($div)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.شا(), b.شا()), ez->vec_neg(y_u), y_u)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} وإلا إذا (cell->type == ID($divfloor)) {
			إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.شا(), b.شا()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_جمع_کړه(y_u, ez->vec_const_unsigned(1, y_u.اندازه())),
						y_u
					)),
					y_u
				)));
			وإلا
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} وإلا إذا (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_جمع_کړه(modulo_trunc, b))));
		}

		إذا (ignore_div_by_zero) {
			ez->assume(ez->افاده(ezSAT::OpOr, b));
		} وإلا {
			std::ویکټور<صحيح> div_zero_پایله;
			إذا (cell->type.دننه(ID($div), ID($divfloor))) {
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::ویکټور<صحيح> all_ones(y.اندازه(), ez->CONST_TRUE);
					std::ویکټور<صحيح> only_لومړی_one(y.اندازه(), ez->CONST_FALSE);
					only_لومړی_one.at(0) = ez->CONST_TRUE;
					div_zero_پایله = ez->vec_ite(a.شا(), only_لومړی_one, all_ones);
				} وإلا {
					div_zero_پایله.ورننباسه(div_zero_پایله.پای_ټکی(), cell->get_پورټ(ID::A).اندازه(), ez->CONST_TRUE);
					div_zero_پایله.ورننباسه(div_zero_پایله.پای_ټکی(), y.اندازه() - div_zero_پایله.اندازه(), ez->CONST_FALSE);
				}
			} وإلا إذا (cell->type.دننه(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				صحيح copy_a_بټونه = min(cell->get_پورټ(ID::A).اندازه(), cell->get_پورټ(ID::B).اندازه());
				div_zero_پایله.ورننباسه(div_zero_پایله.پای_ټکی(), a.begin(), a.begin() + copy_a_بټونه);
				إذا (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_پایله.ورننباسه(div_zero_پایله.پای_ټکی(), y.اندازه() - div_zero_پایله.اندازه(), div_zero_پایله.شا());
				وإلا
					div_zero_پایله.ورننباسه(div_zero_پایله.پای_ټکی(), y.اندازه() - div_zero_پایله.اندازه(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->افاده(ezSAT::OpOr, b), y_tmp, div_zero_پایله)));
		}

		إذا (model_undef) {
			log_assert(arith_undef_handled);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($lut))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		std::ویکټور<صحيح> lut;
		لكل (خپلکاری بټ : cell->getParam(ID::LUT))
			lut.ټېل_شا(بټ == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		طالما (Get_اندازه(lut) < (1 << Get_اندازه(a)))
			lut.ټېل_شا(ez->CONST_FALSE);
		lut.resize(1 << Get_اندازه(a));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> t(lut), u(Get_اندازه(t), ez->CONST_FALSE);

			لكل (صحيح i = Get_اندازه(a)-1; i >= 0; i--)
			{
				std::ویکټور<صحيح> t0(t.begin(), t.begin() + Get_اندازه(t)/2);
				std::ویکټور<صحيح> t1(t.begin() + Get_اندازه(t)/2, t.پای_ټکی());

				std::ویکټور<صحيح> u0(u.begin(), u.begin() + Get_اندازه(u)/2);
				std::ویکټور<صحيح> u1(u.begin() + Get_اندازه(u)/2, u.پای_ټکی());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_اندازه(t) == 1);
			log_assert(Get_اندازه(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_پورټ(ID::Y), timestep), u));
		}
		وإلا
		{
			std::ویکټور<صحيح> t = lut;
			لكل (صحيح i = Get_اندازه(a)-1; i >= 0; i--)
			{
				std::ویکټور<صحيح> t0(t.begin(), t.begin() + Get_اندازه(t)/2);
				std::ویکټور<صحيح> t1(t.begin() + Get_اندازه(t)/2, t.پای_ټکی());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_اندازه(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($sop))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		صحيح y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep).at(0);

		صحيح پلنوالی = cell->getParam(ID::پلنوالی_2).as_int();
		صحيح ژوروالی = cell->getParam(ID::ژوروالی_2).as_int();

		ویکټور<State> جدول_raw = cell->getParam(ID::جدول).منزل_بټونه();
		طالما (Get_اندازه(جدول_raw) < 2*پلنوالی*ژوروالی)
			جدول_raw.ټېل_شا(State::S0);

		ویکټور<ویکټور<صحيح>> جدول_2(ژوروالی);

		لكل (صحيح i = 0; i < ژوروالی; i++)
		لكل (صحيح j = 0; j < پلنوالی; j++)
		{
			منطقي pat0 = (جدول_raw[2*پلنوالی*i + 2*j + 0] == State::S1);
			منطقي pat1 = (جدول_raw[2*پلنوالی*i + 2*j + 1] == State::S1);

			إذا (pat0 && !pat1)
				جدول_2.at(i).ټېل_شا(0);
			وإلا إذا (!pat0 && pat1)
				جدول_2.at(i).ټېل_شا(1);
			وإلا
				جدول_2.at(i).ټېل_شا(-1);
		}

		إذا (model_undef)
		{
			std::ویکټور<صحيح> products, undef_products;
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			صحيح undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep).at(0);

			لكل (صحيح i = 0; i < ژوروالی; i++)
			{
				std::ویکټور<صحيح> cmp_a, cmp_ua, cmp_b;

				لكل (صحيح j = 0; j < پلنوالی; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ټېل_شا(a.at(j));
						cmp_ua.ټېل_شا(undef_a.at(j));
						cmp_b.ټېل_شا(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::ویکټور<صحيح> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::ویکټور<صحيح> masked_b = ez->vec_or(cmp_b, cmp_ua);

				صحيح masked_eq = ez->vec_eq(masked_a, masked_b);
				صحيح any_undef = ez->افاده(ezSAT::OpOr, cmp_ua);

				undef_products.ټېل_شا(ez->AND(any_undef, masked_eq));
				products.ټېل_شا(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			صحيح yy = ez->افاده(ezSAT::OpOr, products);
			ez->سېټ_2(undef_y, ez->AND(ez->NOT(yy), ez->افاده(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		وإلا
		{
			std::ویکټور<صحيح> products;

			لكل (صحيح i = 0; i < ژوروالی; i++)
			{
				std::ویکټور<صحيح> cmp_a, cmp_b;

				لكل (صحيح j = 0; j < پلنوالی; j++)
					إذا (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ټېل_شا(a.at(j));
						cmp_b.ټېل_شا(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.ټېل_شا(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->سېټ_2(y, ez->افاده(ezSAT::OpOr, products));
		}

		أرجع ریښتیا;
	}

	إذا (cell->type == ID($fa))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> c = importDefSigSpec(cell->get_پورټ(ID::C), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		std::ویکټور<صحيح> x = importDefSigSpec(cell->get_پورټ(ID::X), timestep);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::ویکټور<صحيح> xx = model_undef ? ez->vec_var(x.اندازه()) : x;

		std::ویکټور<صحيح> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::ویکټور<صحيح> t2 = ez->vec_and(a, b);
		std::ویکټور<صحيح> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_c = importUndefSigSpec(cell->get_پورټ(ID::C), timestep);

			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			std::ویکټور<صحيح> undef_x = importUndefSigSpec(cell->get_پورټ(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($lcu))
	{
		std::ویکټور<صحيح> p = importDefSigSpec(cell->get_پورټ(ID::P), timestep);
		std::ویکټور<صحيح> g = importDefSigSpec(cell->get_پورټ(ID::G), timestep);
		std::ویکټور<صحيح> ci = importDefSigSpec(cell->get_پورټ(ID::CI), timestep);
		std::ویکټور<صحيح> co = importDefSigSpec(cell->get_پورټ(ID::CO), timestep);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(co.اندازه()) : co;

		لكل (صحيح i = 0; i < Get_اندازه(co); i++)
			ez->سېټ_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_p = importUndefSigSpec(cell->get_پورټ(ID::P), timestep);
			std::ویکټور<صحيح> undef_g = importUndefSigSpec(cell->get_پورټ(ID::G), timestep);
			std::ویکټور<صحيح> undef_ci = importUndefSigSpec(cell->get_پورټ(ID::CI), timestep);
			std::ویکټور<صحيح> undef_co = importUndefSigSpec(cell->get_پورټ(ID::CO), timestep);

			صحيح undef_any_p = ez->افاده(ezSAT::OpOr, undef_p);
			صحيح undef_any_g = ez->افاده(ezSAT::OpOr, undef_g);
			صحيح undef_any_ci = ez->افاده(ezSAT::OpOr, undef_ci);
			صحيح undef_co_بټ = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::ویکټور<صحيح> undef_co_بټونه(undef_co.اندازه(), undef_co_بټ);
			ez->assume(ez->vec_eq(undef_co_بټونه, undef_co));

			undefGating(co, yy, undef_co);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($alu))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> b = importDefSigSpec(cell->get_پورټ(ID::B), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		std::ویکټور<صحيح> x = importDefSigSpec(cell->get_پورټ(ID::X), timestep);
		std::ویکټور<صحيح> ci = importDefSigSpec(cell->get_پورټ(ID::CI), timestep);
		std::ویکټور<صحيح> bi = importDefSigSpec(cell->get_پورټ(ID::BI), timestep);
		std::ویکټور<صحيح> co = importDefSigSpec(cell->get_پورټ(ID::CO), timestep);

		extend_Signal_پلنوالی(a, b, y, cell);
		extend_Signal_پلنوالی(a, b, x, cell);
		extend_Signal_پلنوالی(a, b, co, cell);

		std::ویکټور<صحيح> def_y = model_undef ? ez->vec_var(y.اندازه()) : y;
		std::ویکټور<صحيح> def_x = model_undef ? ez->vec_var(x.اندازه()) : x;
		std::ویکټور<صحيح> def_co = model_undef ? ez->vec_var(co.اندازه()) : co;

		log_assert(Get_اندازه(y) == Get_اندازه(x));
		log_assert(Get_اندازه(y) == Get_اندازه(co));
		log_assert(Get_اندازه(ci) == 1);
		log_assert(Get_اندازه(bi) == 1);

		لكل (صحيح i = 0; i < Get_اندازه(y); i++)
		{
			صحيح s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->سېټ_2(def_x.at(i), ez->XOR(s1, s2));
			ez->سېټ_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->سېټ_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_b = importUndefSigSpec(cell->get_پورټ(ID::B), timestep);
			std::ویکټور<صحيح> undef_ci = importUndefSigSpec(cell->get_پورټ(ID::CI), timestep);
			std::ویکټور<صحيح> undef_bi = importUndefSigSpec(cell->get_پورټ(ID::BI), timestep);

			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			std::ویکټور<صحيح> undef_x = importUndefSigSpec(cell->get_پورټ(ID::X), timestep);
			std::ویکټور<صحيح> undef_co = importUndefSigSpec(cell->get_پورټ(ID::CO), timestep);

			extend_Signal_پلنوالی(undef_a, undef_b, undef_y, cell);
			extend_Signal_پلنوالی(undef_a, undef_b, undef_x, cell);
			extend_Signal_پلنوالی(undef_a, undef_b, undef_co, cell);

			std::ویکټور<صحيح> all_inputs_undef;
			all_inputs_undef.ورننباسه(all_inputs_undef.پای_ټکی(), undef_a.begin(), undef_a.پای_ټکی());
			all_inputs_undef.ورننباسه(all_inputs_undef.پای_ټکی(), undef_b.begin(), undef_b.پای_ټکی());
			all_inputs_undef.ورننباسه(all_inputs_undef.پای_ټکی(), undef_ci.begin(), undef_ci.پای_ټکی());
			all_inputs_undef.ورننباسه(all_inputs_undef.پای_ټکی(), undef_bi.begin(), undef_bi.پای_ټکی());
			صحيح undef_any = ez->افاده(ezSAT::OpOr, all_inputs_undef);

			لكل (صحيح i = 0; i < Get_اندازه(undef_y); i++) {
				ez->سېټ_2(undef_y.at(i), undef_any);
				ez->سېټ_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->سېټ_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($ټوټه))
	{
		RTLIL::SigSpec a = cell->get_پورټ(ID::A);
		RTLIL::SigSpec y = cell->get_پورټ(ID::Y);
		ez->assume(سیګنالونه_eq(a.ایستل(cell->parameters.at(ID::دننه_واټن_2).as_int(), y.اندازه()), y, timestep));
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_پورټ(ID::A);
		RTLIL::SigSpec b = cell->get_پورټ(ID::B);
		RTLIL::SigSpec y = cell->get_پورټ(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(سیګنالونه_eq(ab, y, timestep));
		أرجع ریښتیا;
	}

	إذا (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_معلومات ff(تش_اشاره, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		إذا (ff.has_aload || ff.has_arst || ff.has_sr)
			أرجع خطأ;

		إذا (timestep == 1)
		{
			initial_state.جمع_کړه((*sigmap)(cell->get_پورټ(ID::Q)));
			إذا (model_undef && def_formal) {
				std::ویکټور<صحيح> undef_q = importUndefSigSpec(cell->get_پورټ(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		وإلا
		{
			std::ویکټور<صحيح> d = importDefSigSpec(cell->get_پورټ(ID::D), timestep-1);
			std::ویکټور<صحيح> undef_d;
			إذا (model_undef)
				undef_d = importUndefSigSpec(cell->get_پورټ(ID::D), timestep-1);
			إذا (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				صحيح srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::ویکټور<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::ویکټور<صحيح> undef_rval;
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
				std::ویکټور<صحيح> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				صحيح undef_ce = -1;
				std::ویکټور<صحيح> undef_old_q;
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
				std::ویکټور<صحيح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحيح undef_srst = -1;
				std::ویکټور<صحيح> undef_rval;
				إذا (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				إذا (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				وإلا
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::ویکټور<صحيح> q = importDefSigSpec(cell->get_پورټ(ID::Q), timestep);

			std::ویکټور<صحيح> qq = model_undef ? ez->vec_var(q.اندازه()) : q;
			ez->assume(ez->vec_eq(d, qq));

			إذا (model_undef)
			{
				std::ویکټور<صحيح> undef_q = importUndefSigSpec(cell->get_پورټ(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($anyconst))
	{
		إذا (timestep < 2) {
			إذا (model_undef && def_formal) {
				std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			أرجع ریښتیا;
		}

		std::ویکټور<صحيح> d = importDefSigSpec(cell->get_پورټ(ID::Y), timestep-1);
		std::ویکټور<صحيح> q = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);

		std::ویکټور<صحيح> qq = (model_undef && !def_formal) ? ez->vec_var(q.اندازه()) : q;
		ez->assume(ez->vec_eq(d, qq));

		إذا (model_undef)
		{
			std::ویکټور<صحيح> undef_d = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep-1);
			std::ویکټور<صحيح> undef_q = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);

			إذا (def_formal) {
				لكل (خپلکاری &undef_q_بټ : undef_q)
					ez->سېټ_2(ez->CONST_FALSE, undef_q_بټ);
			} وإلا {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($anyseq))
	{
		إذا (model_undef && def_formal) {
			std::ویکټور<صحيح> undef_q = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			لكل (خپلکاری &undef_q_بټ : undef_q)
				ez->سېټ_2(ez->CONST_FALSE, undef_q_بټ);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($barrier) && !model_barriers)
		أرجع ریښتیا;

	إذا (cell->type.دننه(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::ویکټور<صحيح> a = importDefSigSpec(cell->get_پورټ(ID::A), timestep);
		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		extend_Signal_پلنوالی_Unary(a, y, cell);

		std::ویکټور<صحيح> yy = model_undef ? ez->vec_var(y.اندازه()) : y;
		ez->assume(ez->vec_eq(a, yy));

		إذا (model_undef) {
			std::ویکټور<صحيح> undef_a = importUndefSigSpec(cell->get_پورټ(ID::A), timestep);
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			extend_Signal_پلنوالی_Unary(undef_a, undef_y, cell, خطأ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($initstate))
	{
		خپلکاری کلی = make_pair(مختاړی_2, timestep);
		إذا (initstates.شمېر(کلی) == 0)
			initstates[کلی] = خطأ;

		std::ویکټور<صحيح> y = importDefSigSpec(cell->get_پورټ(ID::Y), timestep);
		log_assert(Get_اندازه(y) == 1);
		ez->سېټ_2(y[0], initstates[کلی] ? ez->CONST_TRUE : ez->CONST_FALSE);

		إذا (model_undef) {
			std::ویکټور<صحيح> undef_y = importUndefSigSpec(cell->get_پورټ(ID::Y), timestep);
			log_assert(Get_اندازه(undef_y) == 1);
			ez->سېټ_2(undef_y[0], ez->CONST_FALSE);
		}

		أرجع ریښتیا;
	}

	إذا (cell->type == ID($أكد))
	{
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_پورټ(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_پورټ(ID::EN)));
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($assume))
	{
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_پورټ(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_پورټ(ID::EN)));
		أرجع ریښتیا;
	}

	إذا (cell->type == ID($scopeinfo) || cell->type == ID($ننوت_پورټ))
	{
		أرجع ریښتیا;
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
