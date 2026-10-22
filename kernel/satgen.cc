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

#شامل "kernel/satgen.h"
#شامل "kernel/ff.h"
#شامل "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

منطقی SatGen::importCell(RTLIL::Cell *cell, صحیح timestep)
{
	منطقی arith_undef_handled = جھوٹ;
	منطقی is_arith_compare = cell->type.اندر(ID($lt), ID($le), ID($ge), ID($gt));

	اگر (model_undef && (cell->type.اندر(ID($جوڑو), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		اگر (is_arith_compare)
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);
		ورنہ
			extend_Signal_چوڑائی(undef_a, undef_b, undef_y, cell, سچ);

		صحیح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
		صحیح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
		صحیح undef_y_ثنائی_ہندسہ = ez->OR(undef_any_a, undef_any_b);

		اگر (cell->type.اندر(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::سمتیہ<صحیح> b = importSigSpec(cell->get_پورٹ(ID::B), timestep);
			undef_y_ثنائی_ہندسہ = ez->OR(undef_y_ثنائی_ہندسہ, ez->NOT(ez->عبارت(ezSAT::OpOr, b)));
		}

		اگر (is_arith_compare) {
			برائے (سائز_t i = 1; i < undef_y.سائز(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سیٹ_2(undef_y_ثنائی_ہندسہ, undef_y.at(0));
		} ورنہ {
			std::سمتیہ<صحیح> undef_y_بٹ(undef_y.سائز(), undef_y_ثنائی_ہندسہ);
			ez->assume(ez->vec_eq(undef_y_بٹ, undef_y));
		}

		arith_undef_handled = سچ;
	}

	اگر (cell->type.اندر(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($جوڑو), ID($sub)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, y, cell);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		اگر (cell->type.اندر(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		اگر (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		اگر (cell->type.اندر(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		اگر (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		اگر (cell->type.اندر(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		اگر (cell->type.اندر(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		اگر (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		اگر (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		اگر (cell->type == ID($جوڑو))
			ez->assume(ez->vec_eq(ez->vec_جوڑو(a, b), yy));
		اگر (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		اگر (model_undef && !arith_undef_handled)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, undef_y, cell, جھوٹ);

			اگر (cell->type.اندر(ID($and), ID($_AND_), ID($_NAND_))) {
				std::سمتیہ<صحیح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::سمتیہ<صحیح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::سمتیہ<صحیح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ورنہ اگر (cell->type.اندر(ID($or), ID($_OR_), ID($_NOR_))) {
				std::سمتیہ<صحیح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::سمتیہ<صحیح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::سمتیہ<صحیح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ورنہ اگر (cell->type.اندر(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::سمتیہ<صحیح> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ورنہ اگر (cell->type == ID($_ANDNOT_)) {
				std::سمتیہ<صحیح> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::سمتیہ<صحیح> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::سمتیہ<صحیح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			ورنہ اگر (cell->type == ID($_ORNOT_)) {
				std::سمتیہ<صحیح> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::سمتیہ<صحیح> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::سمتیہ<صحیح> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			ورنہ
				log_abort();

			undefGating(y, yy, undef_y);
		}
		ورنہ اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		منطقی aoi_منوال = cell->type.اندر(ID($_AOI3_), ID($_AOI4_));
		منطقی three_منوال = cell->type.اندر(ID($_AOI3_), ID($_OAI3_));

		صحیح a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep).at(0);
		صحیح b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep).at(0);
		صحیح c = importDefSigSpec(cell->get_پورٹ(ID::C), timestep).at(0);
		صحیح d = three_منوال ? (aoi_منوال ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_پورٹ(ID::D), timestep).at(0);
		صحیح y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);
		صحیح yy = model_undef ? ez->literal() : y;

		اگر (cell->type.اندر(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		ورنہ
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		اگر (model_undef)
		{
			صحیح undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep).at(0);
			صحیح undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep).at(0);
			صحیح undef_c = importUndefSigSpec(cell->get_پورٹ(ID::C), timestep).at(0);
			صحیح undef_d = three_منوال ? ez->CONST_FALSE : importUndefSigSpec(cell->get_پورٹ(ID::D), timestep).at(0);
			صحیح undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);

			اگر (aoi_منوال)
			{
				صحیح a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				صحیح b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				صحیح c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				صحیح d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				صحیح ab = ez->AND(a, b), cd = ez->AND(c, d);
				صحیح undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				صحیح undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				صحیح ab1 = ez->AND(ab, ez->NOT(undef_ab));
				صحیح cd1 = ez->AND(cd, ez->NOT(undef_cd));
				صحیح yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			ورنہ
			{
				صحیح a1 = ez->AND(a, ez->NOT(undef_a));
				صحیح b1 = ez->AND(b, ez->NOT(undef_b));
				صحیح c1 = ez->AND(c, ez->NOT(undef_c));
				صحیح d1 = ez->AND(d, ez->NOT(undef_d));

				صحیح ab = ez->OR(a, b), cd = ez->OR(c, d);
				صحیح undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				صحیح undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				صحیح ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				صحیح cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				صحیح yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		واپس سچ;
	}

	اگر (cell->type.اندر(ID($_NOT_), ID($not)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی_Unary(a, y, cell);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		اگر (model_undef) {
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی_Unary(undef_a, undef_y, cell, جھوٹ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($bweqx))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحیح> bweqx = ez->vec_not(ez->vec_xor(a, b));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			std::سمتیہ<صحیح> both_undef = ez->vec_and(undef_a, undef_b);
			std::سمتیہ<صحیح> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			برائے (صحیح yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;
		اگر (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		ورنہ اگر (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		ورنہ
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			std::سمتیہ<صحیح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::سمتیہ<صحیح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::سمتیہ<صحیح> yX;
			اگر (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			ورنہ
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($bmux))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحیح> undef_a, undef_s, undef_y;

		اگر (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		}

		اگر (Get_سائز(s) == 0) {
			ez->vec_سیٹ(a, y);
			اگر (model_undef)
				ez->vec_سیٹ(undef_a, undef_y);
		} ورنہ {
			برائے (صحیح i = Get_سائز(s)-1; i >= 0; i--)
			{
				std::سمتیہ<صحیح> برآمد = (i == 0) ? y : ez->vec_var(a.سائز() / 2);
				std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(برآمد.سائز()) : برآمد;

				std::سمتیہ<صحیح> a0(a.begin(), a.begin() + a.سائز() / 2);
				std::سمتیہ<صحیح> a1(a.begin() + a.سائز() / 2, a.انتہا());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				اگر (model_undef)
				{
					std::سمتیہ<صحیح> undef_برآمد = (i == 0) ? undef_y : ez->vec_var(a.سائز() / 2);
					std::سمتیہ<صحیح> undef_a0(undef_a.begin(), undef_a.begin() + a.سائز() / 2);
					std::سمتیہ<صحیح> undef_a1(undef_a.begin() + a.سائز() / 2, undef_a.انتہا());
					std::سمتیہ<صحیح> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::سمتیہ<صحیح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::سمتیہ<صحیح> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_برآمد));
					undefGating(برآمد, yy, undef_برآمد);

					undef_a = undef_برآمد;
				}

				a = برآمد;
			}
		}
		واپس سچ;
	}

	اگر (cell->type == ID($demux))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;
		std::سمتیہ<صحیح> undef_a, undef_s, undef_y;

		اگر (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		}

		اگر (Get_سائز(s) == 0) {
			ez->vec_سیٹ(a, y);
			اگر (model_undef)
				ez->vec_سیٹ(undef_a, undef_y);
		} ورنہ {
			برائے (صحیح i = 0; i < (1 << Get_سائز(s)); i++)
			{
				std::سمتیہ<صحیح> ss;
				برائے (صحیح j = 0; j < Get_سائز(s); j++) {
					اگر (i & 1 << j)
						ss.ڈالو_عقب(s[j]);
					ورنہ
						ss.ڈالو_عقب(ez->NOT(s[j]));
				}
				صحیح sss = ez->عبارت(ezSAT::OpAnd, ss);

				برائے (صحیح j = 0; j < Get_سائز(a); j++) {
					ez->سیٹ_2(ez->AND(sss, a[j]), yy.at(i * Get_سائز(a) + j));
				}

				اگر (model_undef)
				{
					صحیح s0 = ez->عبارت(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					صحیح us = ez->AND(ez->NOT(s0), ez->عبارت(ezSAT::OpOr, undef_s));
					برائے (صحیح j = 0; j < Get_سائز(a); j++) {
						صحیح a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						صحیح yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->سیٹ_2(yX, undef_y.at(i * Get_سائز(a) + j));
					}
				}
			}
			اگر (model_undef)
				undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($pmux))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> s = importDefSigSpec(cell->get_پورٹ(ID::S), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		std::سمتیہ<صحیح> tmp = a;
		برائے (سائز_t i = 0; i < s.سائز(); i++) {
			std::سمتیہ<صحیح> part_کا_b(b.begin()+i*a.سائز(), b.begin()+(i+1)*a.سائز());
			tmp = ez->vec_ite(s.at(i), part_کا_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_s = importUndefSigSpec(cell->get_پورٹ(ID::S), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			صحیح all_undef = ez->CONST_FALSE;
			صحیح ملا_فعال = ez->CONST_FALSE;

			std::سمتیہ<صحیح> undef_tmp = undef_a;

			برائے (سائز_t i = 0; i < s.سائز(); i++)
			{
				std::سمتیہ<صحیح> part_کا_undef_b(undef_b.begin()+i*a.سائز(), undef_b.begin()+(i+1)*a.سائز());

				undef_tmp = ez->vec_ite(s.at(i), part_کا_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), ملا_فعال));
				ملا_فعال = ez->OR(ملا_فعال, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::سمتیہ<صحیح>(a.سائز(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($pos), ID($buf), ID($neg)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی_Unary(a, y, cell);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		اگر (cell->type.اندر(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} ورنہ {
			std::سمتیہ<صحیح> zero(a.سائز(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی_Unary(undef_a, undef_y, cell);

			اگر (cell->type.اندر(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} ورنہ {
				صحیح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
				std::سمتیہ<صحیح> undef_y_بٹ(undef_y.سائز(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_بٹ, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		اگر (cell->type == ID($reduce_and))
			ez->سیٹ_2(ez->عبارت(ez->OpAnd, a), yy.at(0));
		اگر (cell->type.اندر(ID($reduce_or), ID($reduce_bool)))
			ez->سیٹ_2(ez->عبارت(ez->OpOr, a), yy.at(0));
		اگر (cell->type == ID($reduce_xor))
			ez->سیٹ_2(ez->عبارت(ez->OpXor, a), yy.at(0));
		اگر (cell->type == ID($reduce_xnor))
			ez->سیٹ_2(ez->NOT(ez->عبارت(ez->OpXor, a)), yy.at(0));
		اگر (cell->type == ID($logic_not))
			ez->سیٹ_2(ez->NOT(ez->عبارت(ez->OpOr, a)), yy.at(0));
		برائے (سائز_t i = 1; i < y.سائز(); i++)
			ez->سیٹ_2(ez->CONST_FALSE, yy.at(i));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			صحیح aX = ez->عبارت(ezSAT::OpOr, undef_a);

			اگر (cell->type == ID($reduce_and)) {
				صحیح a0 = ez->عبارت(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			ورنہ اگر (cell->type.اندر(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				صحیح a1 = ez->عبارت(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			ورنہ اگر (cell->type.اندر(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} ورنہ
				log_abort();

			برائے (سائز_t i = 1; i < undef_y.سائز(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($logic_and), ID($logic_or)))
	{
		std::سمتیہ<صحیح> vec_a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> vec_b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);

		صحیح a = ez->عبارت(ez->OpOr, vec_a);
		صحیح b = ez->عبارت(ez->OpOr, vec_b);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		اگر (cell->type == ID($logic_and))
			ez->سیٹ_2(ez->عبارت(ez->OpAnd, a, b), yy.at(0));
		ورنہ
			ez->سیٹ_2(ez->عبارت(ez->OpOr, a, b), yy.at(0));
		برائے (سائز_t i = 1; i < y.سائز(); i++)
			ez->سیٹ_2(ez->CONST_FALSE, yy.at(i));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			صحیح a0 = ez->NOT(ez->OR(ez->عبارت(ezSAT::OpOr, vec_a), ez->عبارت(ezSAT::OpOr, undef_a)));
			صحیح b0 = ez->NOT(ez->OR(ez->عبارت(ezSAT::OpOr, vec_b), ez->عبارت(ezSAT::OpOr, undef_b)));
			صحیح a1 = ez->عبارت(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			صحیح b1 = ez->عبارت(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			صحیح aX = ez->عبارت(ezSAT::OpOr, undef_a);
			صحیح bX = ez->عبارت(ezSAT::OpOr, undef_b);

			اگر (cell->type == ID($logic_and))
				ez->سیٹ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			ورنہ اگر (cell->type == ID($logic_or))
				ez->سیٹ_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			ورنہ
				log_abort();

			برائے (سائز_t i = 1; i < undef_y.سائز(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		منطقی is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, cell);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		اگر (model_undef && cell->type.اندر(ID($eqx), ID($nex))) {
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		اگر (cell->type == ID($lt))
			ez->سیٹ_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		اگر (cell->type == ID($le))
			ez->سیٹ_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		اگر (cell->type.اندر(ID($eq), ID($eqx)))
			ez->سیٹ_2(ez->vec_eq(a, b), yy.at(0));
		اگر (cell->type.اندر(ID($ne), ID($nex)))
			ez->سیٹ_2(ez->vec_ne(a, b), yy.at(0));
		اگر (cell->type == ID($ge))
			ez->سیٹ_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		اگر (cell->type == ID($gt))
			ez->سیٹ_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		برائے (سائز_t i = 1; i < y.سائز(); i++)
			ez->سیٹ_2(ez->CONST_FALSE, yy.at(i));

		اگر (model_undef && cell->type.اندر(ID($eqx), ID($nex)))
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);

			اگر (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			ورنہ
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			برائے (سائز_t i = 0; i < y.سائز(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		ورنہ اگر (model_undef && cell->type.اندر(ID($eq), ID($ne)))
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی(undef_a, undef_b, cell, سچ);

			صحیح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
			صحیح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
			صحیح undef_any = ez->OR(undef_any_a, undef_any_b);

			std::سمتیہ<صحیح> masked_a_بٹ = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::سمتیہ<صحیح> masked_b_بٹ = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			صحیح masked_ne = ez->vec_ne(masked_a_بٹ, masked_b_بٹ);
			صحیح undef_y_ثنائی_ہندسہ = ez->AND(undef_any, ez->NOT(masked_ne));

			برائے (سائز_t i = 1; i < undef_y.سائز(); i++)
				ez->سیٹ_2(ez->CONST_FALSE, undef_y.at(i));
			ez->سیٹ_2(undef_y_ثنائی_ہندسہ, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		ورنہ
		{
			اگر (model_undef) {
				std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($سرکاؤ), ID($shiftx)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		صحیح extend_ثنائی_ہندسہ = ez->CONST_FALSE;

		اگر (cell->parameters[ID::A_SIGNED].as_bool())
			extend_ثنائی_ہندسہ = a.عقب();

		جب (y.سائز() < a.سائز())
			y.ڈالو_عقب(ez->literal());
		جب (y.سائز() > a.سائز())
			a.ڈالو_عقب(extend_ثنائی_ہندسہ);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;
		std::سمتیہ<صحیح> shifted_a;

		اگر (cell->type.اندر( ID($shl), ID($sshl)))
			shifted_a = ez->vec_سرکاؤ_بایاں(a, b, جھوٹ, ez->CONST_FALSE, ez->CONST_FALSE);

		اگر (cell->type == ID($shr))
			shifted_a = ez->vec_سرکاؤ_دایاں(a, b, جھوٹ, ez->CONST_FALSE, ez->CONST_FALSE);

		اگر (cell->type == ID($sshr))
			shifted_a = ez->vec_سرکاؤ_دایاں(a, b, جھوٹ, cell->parameters[ID::A_SIGNED].as_bool() ? a.عقب() : ez->CONST_FALSE, ez->CONST_FALSE);

		اگر (cell->type.اندر(ID($سرکاؤ), ID($shiftx)))
			shifted_a = ez->vec_سرکاؤ_دایاں(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			std::سمتیہ<صحیح> undef_a_shifted;

			extend_ثنائی_ہندسہ = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			اگر (cell->parameters[ID::A_SIGNED].as_bool())
				extend_ثنائی_ہندسہ = undef_a.عقب();

			جب (undef_y.سائز() < undef_a.سائز())
				undef_y.ڈالو_عقب(ez->literal());
			جب (undef_y.سائز() > undef_a.سائز())
				undef_a.ڈالو_عقب(extend_ثنائی_ہندسہ);

			اگر (cell->type.اندر(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_سرکاؤ_بایاں(undef_a, b, جھوٹ, ez->CONST_FALSE, ez->CONST_FALSE);

			اگر (cell->type == ID($shr))
				undef_a_shifted = ez->vec_سرکاؤ_دایاں(undef_a, b, جھوٹ, ez->CONST_FALSE, ez->CONST_FALSE);

			اگر (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_سرکاؤ_دایاں(undef_a, b, جھوٹ, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.عقب() : ez->CONST_FALSE, ez->CONST_FALSE);

			اگر (cell->type == ID($سرکاؤ))
				undef_a_shifted = ez->vec_سرکاؤ_دایاں(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			اگر (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_سرکاؤ_دایاں(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			صحیح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
			std::سمتیہ<صحیح> undef_all_y_بٹ(undef_y.سائز(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_بٹ), undef_y));
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($mul))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, y, cell);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		std::سمتیہ<صحیح> tmp(a.سائز(), ez->CONST_FALSE);
		برائے (صحیح i = 0; i < صحیح(a.سائز()); i++)
		{
			std::سمتیہ<صحیح> shifted_a(a.سائز(), ez->CONST_FALSE);
			برائے (صحیح j = i; j < صحیح(a.سائز()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_جوڑو(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		اگر (model_undef) {
			log_assert(arith_undef_handled);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type.اندر(ID($macc), ID($macc_v2)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::سمتیہ<صحیح> tmp(Get_سائز(y), ez->CONST_FALSE);

		برائے (خودکار &پورٹ : macc.terms)
		{
			std::سمتیہ<صحیح> اندر_a = importDefSigSpec(پورٹ.اندر_a, timestep);
			std::سمتیہ<صحیح> اندر_b = importDefSigSpec(پورٹ.اندر_b, timestep);

			جب (Get_سائز(اندر_a) < Get_سائز(y))
				اندر_a.ڈالو_عقب(پورٹ.is_signed && !اندر_a.خالی_خانے() ? اندر_a.عقب() : ez->CONST_FALSE);
			اندر_a.resize(Get_سائز(y));

			اگر (Get_سائز(اندر_b))
			{
				جب (Get_سائز(اندر_b) < Get_سائز(y))
					اندر_b.ڈالو_عقب(پورٹ.is_signed && !اندر_b.خالی_خانے() ? اندر_b.عقب() : ez->CONST_FALSE);
				اندر_b.resize(Get_سائز(y));

				برائے (صحیح i = 0; i < Get_سائز(اندر_b); i++) {
					std::سمتیہ<صحیح> shifted_a(اندر_a.سائز(), ez->CONST_FALSE);
					برائے (صحیح j = i; j < صحیح(اندر_a.سائز()); j++)
						shifted_a.at(j) = اندر_a.at(j-i);
					اگر (پورٹ.do_تفریق)
						tmp = ez->vec_ite(اندر_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					ورنہ
						tmp = ez->vec_ite(اندر_b.at(i), ez->vec_جوڑو(tmp, shifted_a), tmp);
				}
			}
			ورنہ
			{
				اگر (پورٹ.do_تفریق)
					tmp = ez->vec_sub(tmp, اندر_a);
				ورنہ
					tmp = ez->vec_جوڑو(tmp, اندر_a);
			}
		}

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_c;

			اگر (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_پورٹ(ID::C), timestep);

			صحیح undef_any_a = ez->عبارت(ezSAT::OpOr, undef_a);
			صحیح undef_any_b = ez->عبارت(ezSAT::OpOr, undef_b);
			صحیح undef_any_c = ez->عبارت(ezSAT::OpOr, undef_c);
			صحیح undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::سمتیہ<صحیح>(Get_سائز(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		ورنہ
			ez->assume(ez->vec_eq(y, tmp));

		واپس سچ;
	}

	اگر (cell->type.اندر(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی(a, b, y, cell);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;

		std::سمتیہ<صحیح> a_u, b_u;
		اگر (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.عقب(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.عقب(), ez->vec_neg(b), b);
		} ورنہ {
			a_u = a;
			b_u = b;
		}

		std::سمتیہ<صحیح> chain_buf = a_u;
		std::سمتیہ<صحیح> y_u(a_u.سائز(), ez->CONST_FALSE);
		برائے (صحیح i = صحیح(a.سائز())-1; i >= 0; i--)
		{
			chain_buf.داخل_کرو(chain_buf.انتہا(), chain_buf.سائز(), ez->CONST_FALSE);

			std::سمتیہ<صحیح> b_shl(i, ez->CONST_FALSE);
			b_shl.داخل_کرو(b_shl.انتہا(), b_u.begin(), b_u.انتہا());
			b_shl.داخل_کرو(b_shl.انتہا(), chain_buf.سائز()-b_shl.سائز(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.سائز(), chain_buf.انتہا());
		}

		std::سمتیہ<صحیح> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.سائز());

		// modulo calculation
		std::سمتیہ<صحیح> modulo_trunc;
		صحیح floored_eq_trunc;
		اگر (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.عقب(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.عقب(), b.عقب()), ez->NOT(ez->عبارت(ezSAT::OpOr, modulo_trunc)));
		} ورنہ {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		اگر (cell->type == ID($div)) {
			اگر (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.عقب(), b.عقب()), ez->vec_neg(y_u), y_u)));
			ورنہ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} ورنہ اگر (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} ورنہ اگر (cell->type == ID($divfloor)) {
			اگر (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.عقب(), b.عقب()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_جوڑو(y_u, ez->vec_const_unsigned(1, y_u.سائز())),
						y_u
					)),
					y_u
				)));
			ورنہ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} ورنہ اگر (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_جوڑو(modulo_trunc, b))));
		}

		اگر (ignore_div_by_zero) {
			ez->assume(ez->عبارت(ezSAT::OpOr, b));
		} ورنہ {
			std::سمتیہ<صحیح> div_zero_نتیجہ;
			اگر (cell->type.اندر(ID($div), ID($divfloor))) {
				اگر (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::سمتیہ<صحیح> all_ones(y.سائز(), ez->CONST_TRUE);
					std::سمتیہ<صحیح> only_پہلا_one(y.سائز(), ez->CONST_FALSE);
					only_پہلا_one.at(0) = ez->CONST_TRUE;
					div_zero_نتیجہ = ez->vec_ite(a.عقب(), only_پہلا_one, all_ones);
				} ورنہ {
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.انتہا(), cell->get_پورٹ(ID::A).سائز(), ez->CONST_TRUE);
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.انتہا(), y.سائز() - div_zero_نتیجہ.سائز(), ez->CONST_FALSE);
				}
			} ورنہ اگر (cell->type.اندر(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				صحیح copy_a_بٹ = min(cell->get_پورٹ(ID::A).سائز(), cell->get_پورٹ(ID::B).سائز());
				div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.انتہا(), a.begin(), a.begin() + copy_a_بٹ);
				اگر (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.انتہا(), y.سائز() - div_zero_نتیجہ.سائز(), div_zero_نتیجہ.عقب());
				ورنہ
					div_zero_نتیجہ.داخل_کرو(div_zero_نتیجہ.انتہا(), y.سائز() - div_zero_نتیجہ.سائز(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->عبارت(ezSAT::OpOr, b), y_tmp, div_zero_نتیجہ)));
		}

		اگر (model_undef) {
			log_assert(arith_undef_handled);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($lut))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحیح> lut;
		برائے (خودکار ثنائی_ہندسہ : cell->getParam(ID::LUT))
			lut.ڈالو_عقب(ثنائی_ہندسہ == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		جب (Get_سائز(lut) < (1 << Get_سائز(a)))
			lut.ڈالو_عقب(ez->CONST_FALSE);
		lut.resize(1 << Get_سائز(a));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> t(lut), u(Get_سائز(t), ez->CONST_FALSE);

			برائے (صحیح i = Get_سائز(a)-1; i >= 0; i--)
			{
				std::سمتیہ<صحیح> t0(t.begin(), t.begin() + Get_سائز(t)/2);
				std::سمتیہ<صحیح> t1(t.begin() + Get_سائز(t)/2, t.انتہا());

				std::سمتیہ<صحیح> u0(u.begin(), u.begin() + Get_سائز(u)/2);
				std::سمتیہ<صحیح> u1(u.begin() + Get_سائز(u)/2, u.انتہا());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_سائز(t) == 1);
			log_assert(Get_سائز(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep), u));
		}
		ورنہ
		{
			std::سمتیہ<صحیح> t = lut;
			برائے (صحیح i = Get_سائز(a)-1; i >= 0; i--)
			{
				std::سمتیہ<صحیح> t0(t.begin(), t.begin() + Get_سائز(t)/2);
				std::سمتیہ<صحیح> t1(t.begin() + Get_سائز(t)/2, t.انتہا());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_سائز(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		واپس سچ;
	}

	اگر (cell->type == ID($sop))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		صحیح y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);

		صحیح چوڑائی = cell->getParam(ID::چوڑائی_2).as_int();
		صحیح گہرائی = cell->getParam(ID::گہرائی_2).as_int();

		سمتیہ<State> جدول_raw = cell->getParam(ID::جدول).مقصد_بٹ();
		جب (Get_سائز(جدول_raw) < 2*چوڑائی*گہرائی)
			جدول_raw.ڈالو_عقب(State::S0);

		سمتیہ<سمتیہ<صحیح>> جدول_2(گہرائی);

		برائے (صحیح i = 0; i < گہرائی; i++)
		برائے (صحیح j = 0; j < چوڑائی; j++)
		{
			منطقی pat0 = (جدول_raw[2*چوڑائی*i + 2*j + 0] == State::S1);
			منطقی pat1 = (جدول_raw[2*چوڑائی*i + 2*j + 1] == State::S1);

			اگر (pat0 && !pat1)
				جدول_2.at(i).ڈالو_عقب(0);
			ورنہ اگر (!pat0 && pat1)
				جدول_2.at(i).ڈالو_عقب(1);
			ورنہ
				جدول_2.at(i).ڈالو_عقب(-1);
		}

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> products, undef_products;
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			صحیح undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep).at(0);

			برائے (صحیح i = 0; i < گہرائی; i++)
			{
				std::سمتیہ<صحیح> cmp_a, cmp_ua, cmp_b;

				برائے (صحیح j = 0; j < چوڑائی; j++)
					اگر (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ڈالو_عقب(a.at(j));
						cmp_ua.ڈالو_عقب(undef_a.at(j));
						cmp_b.ڈالو_عقب(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::سمتیہ<صحیح> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::سمتیہ<صحیح> masked_b = ez->vec_or(cmp_b, cmp_ua);

				صحیح masked_eq = ez->vec_eq(masked_a, masked_b);
				صحیح any_undef = ez->عبارت(ezSAT::OpOr, cmp_ua);

				undef_products.ڈالو_عقب(ez->AND(any_undef, masked_eq));
				products.ڈالو_عقب(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			صحیح yy = ez->عبارت(ezSAT::OpOr, products);
			ez->سیٹ_2(undef_y, ez->AND(ez->NOT(yy), ez->عبارت(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		ورنہ
		{
			std::سمتیہ<صحیح> products;

			برائے (صحیح i = 0; i < گہرائی; i++)
			{
				std::سمتیہ<صحیح> cmp_a, cmp_b;

				برائے (صحیح j = 0; j < چوڑائی; j++)
					اگر (جدول_2.at(i).at(j) >= 0) {
						cmp_a.ڈالو_عقب(a.at(j));
						cmp_b.ڈالو_عقب(جدول_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.ڈالو_عقب(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->سیٹ_2(y, ez->عبارت(ezSAT::OpOr, products));
		}

		واپس سچ;
	}

	اگر (cell->type == ID($fa))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> c = importDefSigSpec(cell->get_پورٹ(ID::C), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحیح> x = importDefSigSpec(cell->get_پورٹ(ID::X), timestep);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;
		std::سمتیہ<صحیح> xx = model_undef ? ez->vec_var(x.سائز()) : x;

		std::سمتیہ<صحیح> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::سمتیہ<صحیح> t2 = ez->vec_and(a, b);
		std::سمتیہ<صحیح> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_c = importUndefSigSpec(cell->get_پورٹ(ID::C), timestep);

			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			std::سمتیہ<صحیح> undef_x = importUndefSigSpec(cell->get_پورٹ(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($lcu))
	{
		std::سمتیہ<صحیح> p = importDefSigSpec(cell->get_پورٹ(ID::P), timestep);
		std::سمتیہ<صحیح> g = importDefSigSpec(cell->get_پورٹ(ID::G), timestep);
		std::سمتیہ<صحیح> ci = importDefSigSpec(cell->get_پورٹ(ID::CI), timestep);
		std::سمتیہ<صحیح> co = importDefSigSpec(cell->get_پورٹ(ID::CO), timestep);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(co.سائز()) : co;

		برائے (صحیح i = 0; i < Get_سائز(co); i++)
			ez->سیٹ_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_p = importUndefSigSpec(cell->get_پورٹ(ID::P), timestep);
			std::سمتیہ<صحیح> undef_g = importUndefSigSpec(cell->get_پورٹ(ID::G), timestep);
			std::سمتیہ<صحیح> undef_ci = importUndefSigSpec(cell->get_پورٹ(ID::CI), timestep);
			std::سمتیہ<صحیح> undef_co = importUndefSigSpec(cell->get_پورٹ(ID::CO), timestep);

			صحیح undef_any_p = ez->عبارت(ezSAT::OpOr, undef_p);
			صحیح undef_any_g = ez->عبارت(ezSAT::OpOr, undef_g);
			صحیح undef_any_ci = ez->عبارت(ezSAT::OpOr, undef_ci);
			صحیح undef_co_ثنائی_ہندسہ = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::سمتیہ<صحیح> undef_co_بٹ(undef_co.سائز(), undef_co_ثنائی_ہندسہ);
			ez->assume(ez->vec_eq(undef_co_بٹ, undef_co));

			undefGating(co, yy, undef_co);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($alu))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> b = importDefSigSpec(cell->get_پورٹ(ID::B), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		std::سمتیہ<صحیح> x = importDefSigSpec(cell->get_پورٹ(ID::X), timestep);
		std::سمتیہ<صحیح> ci = importDefSigSpec(cell->get_پورٹ(ID::CI), timestep);
		std::سمتیہ<صحیح> bi = importDefSigSpec(cell->get_پورٹ(ID::BI), timestep);
		std::سمتیہ<صحیح> co = importDefSigSpec(cell->get_پورٹ(ID::CO), timestep);

		extend_Signal_چوڑائی(a, b, y, cell);
		extend_Signal_چوڑائی(a, b, x, cell);
		extend_Signal_چوڑائی(a, b, co, cell);

		std::سمتیہ<صحیح> def_y = model_undef ? ez->vec_var(y.سائز()) : y;
		std::سمتیہ<صحیح> def_x = model_undef ? ez->vec_var(x.سائز()) : x;
		std::سمتیہ<صحیح> def_co = model_undef ? ez->vec_var(co.سائز()) : co;

		log_assert(Get_سائز(y) == Get_سائز(x));
		log_assert(Get_سائز(y) == Get_سائز(co));
		log_assert(Get_سائز(ci) == 1);
		log_assert(Get_سائز(bi) == 1);

		برائے (صحیح i = 0; i < Get_سائز(y); i++)
		{
			صحیح s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->سیٹ_2(def_x.at(i), ez->XOR(s1, s2));
			ez->سیٹ_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->سیٹ_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_b = importUndefSigSpec(cell->get_پورٹ(ID::B), timestep);
			std::سمتیہ<صحیح> undef_ci = importUndefSigSpec(cell->get_پورٹ(ID::CI), timestep);
			std::سمتیہ<صحیح> undef_bi = importUndefSigSpec(cell->get_پورٹ(ID::BI), timestep);

			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			std::سمتیہ<صحیح> undef_x = importUndefSigSpec(cell->get_پورٹ(ID::X), timestep);
			std::سمتیہ<صحیح> undef_co = importUndefSigSpec(cell->get_پورٹ(ID::CO), timestep);

			extend_Signal_چوڑائی(undef_a, undef_b, undef_y, cell);
			extend_Signal_چوڑائی(undef_a, undef_b, undef_x, cell);
			extend_Signal_چوڑائی(undef_a, undef_b, undef_co, cell);

			std::سمتیہ<صحیح> all_inputs_undef;
			all_inputs_undef.داخل_کرو(all_inputs_undef.انتہا(), undef_a.begin(), undef_a.انتہا());
			all_inputs_undef.داخل_کرو(all_inputs_undef.انتہا(), undef_b.begin(), undef_b.انتہا());
			all_inputs_undef.داخل_کرو(all_inputs_undef.انتہا(), undef_ci.begin(), undef_ci.انتہا());
			all_inputs_undef.داخل_کرو(all_inputs_undef.انتہا(), undef_bi.begin(), undef_bi.انتہا());
			صحیح undef_any = ez->عبارت(ezSAT::OpOr, all_inputs_undef);

			برائے (صحیح i = 0; i < Get_سائز(undef_y); i++) {
				ez->سیٹ_2(undef_y.at(i), undef_any);
				ez->سیٹ_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->سیٹ_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($قاش))
	{
		RTLIL::SigSpec a = cell->get_پورٹ(ID::A);
		RTLIL::SigSpec y = cell->get_پورٹ(ID::Y);
		ez->assume(اشارات_eq(a.نکالو_2(cell->parameters.at(ID::انحراف_2).as_int(), y.سائز()), y, timestep));
		واپس سچ;
	}

	اگر (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_پورٹ(ID::A);
		RTLIL::SigSpec b = cell->get_پورٹ(ID::B);
		RTLIL::SigSpec y = cell->get_پورٹ(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(اشارات_eq(ab, y, timestep));
		واپس سچ;
	}

	اگر (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_ڈیٹا ff(خالی_اشاریہ, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		اگر (ff.has_aload || ff.has_arst || ff.has_sr)
			واپس جھوٹ;

		اگر (timestep == 1)
		{
			initial_state.جوڑو((*sigmap)(cell->get_پورٹ(ID::Q)));
			اگر (model_undef && def_formal) {
				std::سمتیہ<صحیح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		ورنہ
		{
			std::سمتیہ<صحیح> d = importDefSigSpec(cell->get_پورٹ(ID::D), timestep-1);
			std::سمتیہ<صحیح> undef_d;
			اگر (model_undef)
				undef_d = importUndefSigSpec(cell->get_پورٹ(ID::D), timestep-1);
			اگر (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				صحیح srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::سمتیہ<صحیح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحیح undef_srst = -1;
				std::سمتیہ<صحیح> undef_rval;
				اگر (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				اگر (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				ورنہ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			اگر (ff.has_ce) {
				صحیح ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::سمتیہ<صحیح> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				صحیح undef_ce = -1;
				std::سمتیہ<صحیح> undef_old_q;
				اگر (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				اگر (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				ورنہ
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			اگر (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				صحیح srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::سمتیہ<صحیح> rval = importDefSigSpec(ff.val_srst, timestep-1);
				صحیح undef_srst = -1;
				std::سمتیہ<صحیح> undef_rval;
				اگر (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				اگر (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				ورنہ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::سمتیہ<صحیح> q = importDefSigSpec(cell->get_پورٹ(ID::Q), timestep);

			std::سمتیہ<صحیح> qq = model_undef ? ez->vec_var(q.سائز()) : q;
			ez->assume(ez->vec_eq(d, qq));

			اگر (model_undef)
			{
				std::سمتیہ<صحیح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		واپس سچ;
	}

	اگر (cell->type == ID($anyconst))
	{
		اگر (timestep < 2) {
			اگر (model_undef && def_formal) {
				std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			واپس سچ;
		}

		std::سمتیہ<صحیح> d = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep-1);
		std::سمتیہ<صحیح> q = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);

		std::سمتیہ<صحیح> qq = (model_undef && !def_formal) ? ez->vec_var(q.سائز()) : q;
		ez->assume(ez->vec_eq(d, qq));

		اگر (model_undef)
		{
			std::سمتیہ<صحیح> undef_d = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep-1);
			std::سمتیہ<صحیح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);

			اگر (def_formal) {
				برائے (خودکار &undef_q_ثنائی_ہندسہ : undef_q)
					ez->سیٹ_2(ez->CONST_FALSE, undef_q_ثنائی_ہندسہ);
			} ورنہ {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		واپس سچ;
	}

	اگر (cell->type == ID($anyseq))
	{
		اگر (model_undef && def_formal) {
			std::سمتیہ<صحیح> undef_q = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			برائے (خودکار &undef_q_ثنائی_ہندسہ : undef_q)
				ez->سیٹ_2(ez->CONST_FALSE, undef_q_ثنائی_ہندسہ);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($barrier) && !model_barriers)
		واپس سچ;

	اگر (cell->type.اندر(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::سمتیہ<صحیح> a = importDefSigSpec(cell->get_پورٹ(ID::A), timestep);
		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		extend_Signal_چوڑائی_Unary(a, y, cell);

		std::سمتیہ<صحیح> yy = model_undef ? ez->vec_var(y.سائز()) : y;
		ez->assume(ez->vec_eq(a, yy));

		اگر (model_undef) {
			std::سمتیہ<صحیح> undef_a = importUndefSigSpec(cell->get_پورٹ(ID::A), timestep);
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			extend_Signal_چوڑائی_Unary(undef_a, undef_y, cell, جھوٹ);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		واپس سچ;
	}

	اگر (cell->type == ID($initstate))
	{
		خودکار کلید = make_pair(سابقہ_2, timestep);
		اگر (initstates.گنتی(کلید) == 0)
			initstates[کلید] = جھوٹ;

		std::سمتیہ<صحیح> y = importDefSigSpec(cell->get_پورٹ(ID::Y), timestep);
		log_assert(Get_سائز(y) == 1);
		ez->سیٹ_2(y[0], initstates[کلید] ? ez->CONST_TRUE : ez->CONST_FALSE);

		اگر (model_undef) {
			std::سمتیہ<صحیح> undef_y = importUndefSigSpec(cell->get_پورٹ(ID::Y), timestep);
			log_assert(Get_سائز(undef_y) == 1);
			ez->سیٹ_2(undef_y[0], ez->CONST_FALSE);
		}

		واپس سچ;
	}

	اگر (cell->type == ID($أكد))
	{
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_پورٹ(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_پورٹ(ID::EN)));
		واپس سچ;
	}

	اگر (cell->type == ID($assume))
	{
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_پورٹ(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_پورٹ(ID::EN)));
		واپس سچ;
	}

	اگر (cell->type == ID($scopeinfo) || cell->type == ID($داخلہ_پورٹ))
	{
		واپس سچ;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	واپس جھوٹ;
}

نامخلا Yosys {

خالی report_missing_model(منطقی warn_only, RTLIL::Cell* cell)
{
	std::string s;
	اگر (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	ورنہ
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	اگر (warn_only) {
		log_warning_noprefix("%s\n", s);
	} ورنہ {
		log_error("%s\n", s);
	}
}

}
