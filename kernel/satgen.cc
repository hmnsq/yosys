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

#అన్తర్భావ "kernel/satgen.h"
#అన్తర్భావ "kernel/ff.h"
#అన్తర్భావ "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

తార్కిక SatGen::importCell(RTLIL::Cell *cell, పూర్ణాంకం timestep)
{
	తార్కిక arith_undef_handled = అబద్ధం;
	తార్కిక is_arith_compare = cell->type.లోపల(ID($lt), ID($le), ID($ge), ID($gt));

	ఒకవేళ (model_undef && (cell->type.లోపల(ID($కూడు), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		ఒకవేళ (is_arith_compare)
			extend_Signal_వెడల్పు(undef_a, undef_b, cell, నిజం);
		లేకపోతే
			extend_Signal_వెడల్పు(undef_a, undef_b, undef_y, cell, నిజం);

		పూర్ణాంకం undef_any_a = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_a);
		పూర్ణాంకం undef_any_b = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_b);
		పూర్ణాంకం undef_y_బిట్టు = ez->OR(undef_any_a, undef_any_b);

		ఒకవేళ (cell->type.లోపల(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::సదిశ<పూర్ణాంకం> b = importSigSpec(cell->get_పోర్టు(ID::B), timestep);
			undef_y_బిట్టు = ez->OR(undef_y_బిట్టు, ez->NOT(ez->వ్యక్తీకరణ(ezSAT::OpOr, b)));
		}

		ఒకవేళ (is_arith_compare) {
			కొరకు (కొలత_t i = 1; i < undef_y.కొలత(); i++)
				ez->సమితి_2(ez->CONST_FALSE, undef_y.at(i));
			ez->సమితి_2(undef_y_బిట్టు, undef_y.at(0));
		} లేకపోతే {
			std::సదిశ<పూర్ణాంకం> undef_y_బిట్లు(undef_y.కొలత(), undef_y_బిట్టు);
			ez->assume(ez->vec_eq(undef_y_బిట్లు, undef_y));
		}

		arith_undef_handled = నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($కూడు), ID($sub)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		extend_Signal_వెడల్పు(a, b, y, cell);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		ఒకవేళ (cell->type.లోపల(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		ఒకవేళ (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		ఒకవేళ (cell->type.లోపల(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		ఒకవేళ (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		ఒకవేళ (cell->type.లోపల(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		ఒకవేళ (cell->type.లోపల(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		ఒకవేళ (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		ఒకవేళ (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		ఒకవేళ (cell->type == ID($కూడు))
			ez->assume(ez->vec_eq(ez->vec_కూడు(a, b), yy));
		ఒకవేళ (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		ఒకవేళ (model_undef && !arith_undef_handled)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			extend_Signal_వెడల్పు(undef_a, undef_b, undef_y, cell, అబద్ధం);

			ఒకవేళ (cell->type.లోపల(ID($and), ID($_AND_), ID($_NAND_))) {
				std::సదిశ<పూర్ణాంకం> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::సదిశ<పూర్ణాంకం> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::సదిశ<పూర్ణాంకం> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			లేకపోతే ఒకవేళ (cell->type.లోపల(ID($or), ID($_OR_), ID($_NOR_))) {
				std::సదిశ<పూర్ణాంకం> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::సదిశ<పూర్ణాంకం> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::సదిశ<పూర్ణాంకం> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			లేకపోతే ఒకవేళ (cell->type.లోపల(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::సదిశ<పూర్ణాంకం> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			లేకపోతే ఒకవేళ (cell->type == ID($_ANDNOT_)) {
				std::సదిశ<పూర్ణాంకం> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::సదిశ<పూర్ణాంకం> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::సదిశ<పూర్ణాంకం> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			లేకపోతే ఒకవేళ (cell->type == ID($_ORNOT_)) {
				std::సదిశ<పూర్ణాంకం> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::సదిశ<పూర్ణాంకం> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::సదిశ<పూర్ణాంకం> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			లేకపోతే
				log_abort();

			undefGating(y, yy, undef_y);
		}
		లేకపోతే ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		తార్కిక aoi_బహుళకం = cell->type.లోపల(ID($_AOI3_), ID($_AOI4_));
		తార్కిక three_బహుళకం = cell->type.లోపల(ID($_AOI3_), ID($_OAI3_));

		పూర్ణాంకం a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep).at(0);
		పూర్ణాంకం b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep).at(0);
		పూర్ణాంకం c = importDefSigSpec(cell->get_పోర్టు(ID::C), timestep).at(0);
		పూర్ణాంకం d = three_బహుళకం ? (aoi_బహుళకం ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_పోర్టు(ID::D), timestep).at(0);
		పూర్ణాంకం y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep).at(0);
		పూర్ణాంకం yy = model_undef ? ez->literal() : y;

		ఒకవేళ (cell->type.లోపల(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		లేకపోతే
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		ఒకవేళ (model_undef)
		{
			పూర్ణాంకం undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep).at(0);
			పూర్ణాంకం undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep).at(0);
			పూర్ణాంకం undef_c = importUndefSigSpec(cell->get_పోర్టు(ID::C), timestep).at(0);
			పూర్ణాంకం undef_d = three_బహుళకం ? ez->CONST_FALSE : importUndefSigSpec(cell->get_పోర్టు(ID::D), timestep).at(0);
			పూర్ణాంకం undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep).at(0);

			ఒకవేళ (aoi_బహుళకం)
			{
				పూర్ణాంకం a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				పూర్ణాంకం b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				పూర్ణాంకం c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				పూర్ణాంకం d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				పూర్ణాంకం ab = ez->AND(a, b), cd = ez->AND(c, d);
				పూర్ణాంకం undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				పూర్ణాంకం undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				పూర్ణాంకం ab1 = ez->AND(ab, ez->NOT(undef_ab));
				పూర్ణాంకం cd1 = ez->AND(cd, ez->NOT(undef_cd));
				పూర్ణాంకం yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			లేకపోతే
			{
				పూర్ణాంకం a1 = ez->AND(a, ez->NOT(undef_a));
				పూర్ణాంకం b1 = ez->AND(b, ez->NOT(undef_b));
				పూర్ణాంకం c1 = ez->AND(c, ez->NOT(undef_c));
				పూర్ణాంకం d1 = ez->AND(d, ez->NOT(undef_d));

				పూర్ణాంకం ab = ez->OR(a, b), cd = ez->OR(c, d);
				పూర్ణాంకం undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				పూర్ణాంకం undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				పూర్ణాంకం ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				పూర్ణాంకం cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				పూర్ణాంకం yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($_NOT_), ID($not)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		extend_Signal_వెడల్పు_Unary(a, y, cell);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		ఒకవేళ (model_undef) {
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			extend_Signal_వెడల్పు_Unary(undef_a, undef_y, cell, అబద్ధం);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($bweqx))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		std::సదిశ<పూర్ణాంకం> bweqx = ez->vec_not(ez->vec_xor(a, b));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

			std::సదిశ<పూర్ణాంకం> both_undef = ez->vec_and(undef_a, undef_b);
			std::సదిశ<పూర్ణాంకం> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			కొరకు (పూర్ణాంకం yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> s = importDefSigSpec(cell->get_పోర్టు(ID::S), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;
		ఒకవేళ (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		లేకపోతే ఒకవేళ (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		లేకపోతే
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_s = importUndefSigSpec(cell->get_పోర్టు(ID::S), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

			std::సదిశ<పూర్ణాంకం> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::సదిశ<పూర్ణాంకం> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::సదిశ<పూర్ణాంకం> yX;
			ఒకవేళ (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			లేకపోతే
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($bmux))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> s = importDefSigSpec(cell->get_పోర్టు(ID::S), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		std::సదిశ<పూర్ణాంకం> undef_a, undef_s, undef_y;

		ఒకవేళ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_పోర్టు(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		}

		ఒకవేళ (Get_కొలత(s) == 0) {
			ez->vec_సమితి(a, y);
			ఒకవేళ (model_undef)
				ez->vec_సమితి(undef_a, undef_y);
		} లేకపోతే {
			కొరకు (పూర్ణాంకం i = Get_కొలత(s)-1; i >= 0; i--)
			{
				std::సదిశ<పూర్ణాంకం> బహిర్గతం = (i == 0) ? y : ez->vec_var(a.కొలత() / 2);
				std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(బహిర్గతం.కొలత()) : బహిర్గతం;

				std::సదిశ<పూర్ణాంకం> a0(a.begin(), a.begin() + a.కొలత() / 2);
				std::సదిశ<పూర్ణాంకం> a1(a.begin() + a.కొలత() / 2, a.చివర());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				ఒకవేళ (model_undef)
				{
					std::సదిశ<పూర్ణాంకం> undef_బహిర్గతం = (i == 0) ? undef_y : ez->vec_var(a.కొలత() / 2);
					std::సదిశ<పూర్ణాంకం> undef_a0(undef_a.begin(), undef_a.begin() + a.కొలత() / 2);
					std::సదిశ<పూర్ణాంకం> undef_a1(undef_a.begin() + a.కొలత() / 2, undef_a.చివర());
					std::సదిశ<పూర్ణాంకం> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::సదిశ<పూర్ణాంకం> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::సదిశ<పూర్ణాంకం> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_బహిర్గతం));
					undefGating(బహిర్గతం, yy, undef_బహిర్గతం);

					undef_a = undef_బహిర్గతం;
				}

				a = బహిర్గతం;
			}
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($demux))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> s = importDefSigSpec(cell->get_పోర్టు(ID::S), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;
		std::సదిశ<పూర్ణాంకం> undef_a, undef_s, undef_y;

		ఒకవేళ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_పోర్టు(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		}

		ఒకవేళ (Get_కొలత(s) == 0) {
			ez->vec_సమితి(a, y);
			ఒకవేళ (model_undef)
				ez->vec_సమితి(undef_a, undef_y);
		} లేకపోతే {
			కొరకు (పూర్ణాంకం i = 0; i < (1 << Get_కొలత(s)); i++)
			{
				std::సదిశ<పూర్ణాంకం> ss;
				కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(s); j++) {
					ఒకవేళ (i & 1 << j)
						ss.నెట్టు_వెనుక(s[j]);
					లేకపోతే
						ss.నెట్టు_వెనుక(ez->NOT(s[j]));
				}
				పూర్ణాంకం sss = ez->వ్యక్తీకరణ(ezSAT::OpAnd, ss);

				కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(a); j++) {
					ez->సమితి_2(ez->AND(sss, a[j]), yy.at(i * Get_కొలత(a) + j));
				}

				ఒకవేళ (model_undef)
				{
					పూర్ణాంకం s0 = ez->వ్యక్తీకరణ(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					పూర్ణాంకం us = ez->AND(ez->NOT(s0), ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_s));
					కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(a); j++) {
						పూర్ణాంకం a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						పూర్ణాంకం yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->సమితి_2(yX, undef_y.at(i * Get_కొలత(a) + j));
					}
				}
			}
			ఒకవేళ (model_undef)
				undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($pmux))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> s = importDefSigSpec(cell->get_పోర్టు(ID::S), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		std::సదిశ<పూర్ణాంకం> tmp = a;
		కొరకు (కొలత_t i = 0; i < s.కొలత(); i++) {
			std::సదిశ<పూర్ణాంకం> part_యొక్క_b(b.begin()+i*a.కొలత(), b.begin()+(i+1)*a.కొలత());
			tmp = ez->vec_ite(s.at(i), part_యొక్క_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_s = importUndefSigSpec(cell->get_పోర్టు(ID::S), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

			పూర్ణాంకం all_undef = ez->CONST_FALSE;
			పూర్ణాంకం దొరికింది_చురుకైన = ez->CONST_FALSE;

			std::సదిశ<పూర్ణాంకం> undef_tmp = undef_a;

			కొరకు (కొలత_t i = 0; i < s.కొలత(); i++)
			{
				std::సదిశ<పూర్ణాంకం> part_యొక్క_undef_b(undef_b.begin()+i*a.కొలత(), undef_b.begin()+(i+1)*a.కొలత());

				undef_tmp = ez->vec_ite(s.at(i), part_యొక్క_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), దొరికింది_చురుకైన));
				దొరికింది_చురుకైన = ez->OR(దొరికింది_చురుకైన, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::సదిశ<పూర్ణాంకం>(a.కొలత(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($pos), ID($buf), ID($neg)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		extend_Signal_వెడల్పు_Unary(a, y, cell);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		ఒకవేళ (cell->type.లోపల(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} లేకపోతే {
			std::సదిశ<పూర్ణాంకం> zero(a.కొలత(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			extend_Signal_వెడల్పు_Unary(undef_a, undef_y, cell);

			ఒకవేళ (cell->type.లోపల(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} లేకపోతే {
				పూర్ణాంకం undef_any_a = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_a);
				std::సదిశ<పూర్ణాంకం> undef_y_బిట్లు(undef_y.కొలత(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_బిట్లు, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		ఒకవేళ (cell->type == ID($reduce_and))
			ez->సమితి_2(ez->వ్యక్తీకరణ(ez->OpAnd, a), yy.at(0));
		ఒకవేళ (cell->type.లోపల(ID($reduce_or), ID($reduce_bool)))
			ez->సమితి_2(ez->వ్యక్తీకరణ(ez->OpOr, a), yy.at(0));
		ఒకవేళ (cell->type == ID($reduce_xor))
			ez->సమితి_2(ez->వ్యక్తీకరణ(ez->OpXor, a), yy.at(0));
		ఒకవేళ (cell->type == ID($reduce_xnor))
			ez->సమితి_2(ez->NOT(ez->వ్యక్తీకరణ(ez->OpXor, a)), yy.at(0));
		ఒకవేళ (cell->type == ID($logic_not))
			ez->సమితి_2(ez->NOT(ez->వ్యక్తీకరణ(ez->OpOr, a)), yy.at(0));
		కొరకు (కొలత_t i = 1; i < y.కొలత(); i++)
			ez->సమితి_2(ez->CONST_FALSE, yy.at(i));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			పూర్ణాంకం aX = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_a);

			ఒకవేళ (cell->type == ID($reduce_and)) {
				పూర్ణాంకం a0 = ez->వ్యక్తీకరణ(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			లేకపోతే ఒకవేళ (cell->type.లోపల(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				పూర్ణాంకం a1 = ez->వ్యక్తీకరణ(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			లేకపోతే ఒకవేళ (cell->type.లోపల(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} లేకపోతే
				log_abort();

			కొరకు (కొలత_t i = 1; i < undef_y.కొలత(); i++)
				ez->సమితి_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($logic_and), ID($logic_or)))
	{
		std::సదిశ<పూర్ణాంకం> vec_a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> vec_b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);

		పూర్ణాంకం a = ez->వ్యక్తీకరణ(ez->OpOr, vec_a);
		పూర్ణాంకం b = ez->వ్యక్తీకరణ(ez->OpOr, vec_b);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		ఒకవేళ (cell->type == ID($logic_and))
			ez->సమితి_2(ez->వ్యక్తీకరణ(ez->OpAnd, a, b), yy.at(0));
		లేకపోతే
			ez->సమితి_2(ez->వ్యక్తీకరణ(ez->OpOr, a, b), yy.at(0));
		కొరకు (కొలత_t i = 1; i < y.కొలత(); i++)
			ez->సమితి_2(ez->CONST_FALSE, yy.at(i));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

			పూర్ణాంకం a0 = ez->NOT(ez->OR(ez->వ్యక్తీకరణ(ezSAT::OpOr, vec_a), ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_a)));
			పూర్ణాంకం b0 = ez->NOT(ez->OR(ez->వ్యక్తీకరణ(ezSAT::OpOr, vec_b), ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_b)));
			పూర్ణాంకం a1 = ez->వ్యక్తీకరణ(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			పూర్ణాంకం b1 = ez->వ్యక్తీకరణ(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			పూర్ణాంకం aX = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_a);
			పూర్ణాంకం bX = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_b);

			ఒకవేళ (cell->type == ID($logic_and))
				ez->సమితి_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			లేకపోతే ఒకవేళ (cell->type == ID($logic_or))
				ez->సమితి_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			లేకపోతే
				log_abort();

			కొరకు (కొలత_t i = 1; i < undef_y.కొలత(); i++)
				ez->సమితి_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		తార్కిక is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		extend_Signal_వెడల్పు(a, b, cell);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		ఒకవేళ (model_undef && cell->type.లోపల(ID($eqx), ID($nex))) {
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			extend_Signal_వెడల్పు(undef_a, undef_b, cell, నిజం);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		ఒకవేళ (cell->type == ID($lt))
			ez->సమితి_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		ఒకవేళ (cell->type == ID($le))
			ez->సమితి_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		ఒకవేళ (cell->type.లోపల(ID($eq), ID($eqx)))
			ez->సమితి_2(ez->vec_eq(a, b), yy.at(0));
		ఒకవేళ (cell->type.లోపల(ID($ne), ID($nex)))
			ez->సమితి_2(ez->vec_ne(a, b), yy.at(0));
		ఒకవేళ (cell->type == ID($ge))
			ez->సమితి_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		ఒకవేళ (cell->type == ID($gt))
			ez->సమితి_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		కొరకు (కొలత_t i = 1; i < y.కొలత(); i++)
			ez->సమితి_2(ez->CONST_FALSE, yy.at(i));

		ఒకవేళ (model_undef && cell->type.లోపల(ID($eqx), ID($nex)))
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			extend_Signal_వెడల్పు(undef_a, undef_b, cell, నిజం);

			ఒకవేళ (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			లేకపోతే
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			కొరకు (కొలత_t i = 0; i < y.కొలత(); i++)
				ez->సమితి_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		లేకపోతే ఒకవేళ (model_undef && cell->type.లోపల(ID($eq), ID($ne)))
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			extend_Signal_వెడల్పు(undef_a, undef_b, cell, నిజం);

			పూర్ణాంకం undef_any_a = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_a);
			పూర్ణాంకం undef_any_b = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_b);
			పూర్ణాంకం undef_any = ez->OR(undef_any_a, undef_any_b);

			std::సదిశ<పూర్ణాంకం> masked_a_బిట్లు = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::సదిశ<పూర్ణాంకం> masked_b_బిట్లు = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			పూర్ణాంకం masked_ne = ez->vec_ne(masked_a_బిట్లు, masked_b_బిట్లు);
			పూర్ణాంకం undef_y_బిట్టు = ez->AND(undef_any, ez->NOT(masked_ne));

			కొరకు (కొలత_t i = 1; i < undef_y.కొలత(); i++)
				ez->సమితి_2(ez->CONST_FALSE, undef_y.at(i));
			ez->సమితి_2(undef_y_బిట్టు, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		లేకపోతే
		{
			ఒకవేళ (model_undef) {
				std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($జరుపు), ID($shiftx)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		పూర్ణాంకం extend_బిట్టు = ez->CONST_FALSE;

		ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool())
			extend_బిట్టు = a.వెనుక();

		అయితే (y.కొలత() < a.కొలత())
			y.నెట్టు_వెనుక(ez->literal());
		అయితే (y.కొలత() > a.కొలత())
			a.నెట్టు_వెనుక(extend_బిట్టు);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;
		std::సదిశ<పూర్ణాంకం> shifted_a;

		ఒకవేళ (cell->type.లోపల( ID($shl), ID($sshl)))
			shifted_a = ez->vec_జరుపు_ఎడమ(a, b, అబద్ధం, ez->CONST_FALSE, ez->CONST_FALSE);

		ఒకవేళ (cell->type == ID($shr))
			shifted_a = ez->vec_జరుపు_కుడి(a, b, అబద్ధం, ez->CONST_FALSE, ez->CONST_FALSE);

		ఒకవేళ (cell->type == ID($sshr))
			shifted_a = ez->vec_జరుపు_కుడి(a, b, అబద్ధం, cell->parameters[ID::A_SIGNED].as_bool() ? a.వెనుక() : ez->CONST_FALSE, ez->CONST_FALSE);

		ఒకవేళ (cell->type.లోపల(ID($జరుపు), ID($shiftx)))
			shifted_a = ez->vec_జరుపు_కుడి(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			std::సదిశ<పూర్ణాంకం> undef_a_shifted;

			extend_బిట్టు = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool())
				extend_బిట్టు = undef_a.వెనుక();

			అయితే (undef_y.కొలత() < undef_a.కొలత())
				undef_y.నెట్టు_వెనుక(ez->literal());
			అయితే (undef_y.కొలత() > undef_a.కొలత())
				undef_a.నెట్టు_వెనుక(extend_బిట్టు);

			ఒకవేళ (cell->type.లోపల(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_జరుపు_ఎడమ(undef_a, b, అబద్ధం, ez->CONST_FALSE, ez->CONST_FALSE);

			ఒకవేళ (cell->type == ID($shr))
				undef_a_shifted = ez->vec_జరుపు_కుడి(undef_a, b, అబద్ధం, ez->CONST_FALSE, ez->CONST_FALSE);

			ఒకవేళ (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_జరుపు_కుడి(undef_a, b, అబద్ధం, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.వెనుక() : ez->CONST_FALSE, ez->CONST_FALSE);

			ఒకవేళ (cell->type == ID($జరుపు))
				undef_a_shifted = ez->vec_జరుపు_కుడి(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			ఒకవేళ (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_జరుపు_కుడి(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			పూర్ణాంకం undef_any_b = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_b);
			std::సదిశ<పూర్ణాంకం> undef_all_y_బిట్లు(undef_y.కొలత(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_బిట్లు), undef_y));
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($mul))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		extend_Signal_వెడల్పు(a, b, y, cell);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		std::సదిశ<పూర్ణాంకం> tmp(a.కొలత(), ez->CONST_FALSE);
		కొరకు (పూర్ణాంకం i = 0; i < పూర్ణాంకం(a.కొలత()); i++)
		{
			std::సదిశ<పూర్ణాంకం> shifted_a(a.కొలత(), ez->CONST_FALSE);
			కొరకు (పూర్ణాంకం j = i; j < పూర్ణాంకం(a.కొలత()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_కూడు(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ఒకవేళ (model_undef) {
			log_assert(arith_undef_handled);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($macc), ID($macc_v2)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::సదిశ<పూర్ణాంకం> tmp(Get_కొలత(y), ez->CONST_FALSE);

		కొరకు (స్వయంచాలక &పోర్టు : macc.terms)
		{
			std::సదిశ<పూర్ణాంకం> లోపల_a = importDefSigSpec(పోర్టు.లోపల_a, timestep);
			std::సదిశ<పూర్ణాంకం> లోపల_b = importDefSigSpec(పోర్టు.లోపల_b, timestep);

			అయితే (Get_కొలత(లోపల_a) < Get_కొలత(y))
				లోపల_a.నెట్టు_వెనుక(పోర్టు.is_signed && !లోపల_a.ఖాళీలు() ? లోపల_a.వెనుక() : ez->CONST_FALSE);
			లోపల_a.resize(Get_కొలత(y));

			ఒకవేళ (Get_కొలత(లోపల_b))
			{
				అయితే (Get_కొలత(లోపల_b) < Get_కొలత(y))
					లోపల_b.నెట్టు_వెనుక(పోర్టు.is_signed && !లోపల_b.ఖాళీలు() ? లోపల_b.వెనుక() : ez->CONST_FALSE);
				లోపల_b.resize(Get_కొలత(y));

				కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(లోపల_b); i++) {
					std::సదిశ<పూర్ణాంకం> shifted_a(లోపల_a.కొలత(), ez->CONST_FALSE);
					కొరకు (పూర్ణాంకం j = i; j < పూర్ణాంకం(లోపల_a.కొలత()); j++)
						shifted_a.at(j) = లోపల_a.at(j-i);
					ఒకవేళ (పోర్టు.do_వ్యవకలనం)
						tmp = ez->vec_ite(లోపల_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					లేకపోతే
						tmp = ez->vec_ite(లోపల_b.at(i), ez->vec_కూడు(tmp, shifted_a), tmp);
				}
			}
			లేకపోతే
			{
				ఒకవేళ (పోర్టు.do_వ్యవకలనం)
					tmp = ez->vec_sub(tmp, లోపల_a);
				లేకపోతే
					tmp = ez->vec_కూడు(tmp, లోపల_a);
			}
		}

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_c;

			ఒకవేళ (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_పోర్టు(ID::C), timestep);

			పూర్ణాంకం undef_any_a = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_a);
			పూర్ణాంకం undef_any_b = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_b);
			పూర్ణాంకం undef_any_c = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_c);
			పూర్ణాంకం undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::సదిశ<పూర్ణాంకం>(Get_కొలత(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		లేకపోతే
			ez->assume(ez->vec_eq(y, tmp));

		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type.లోపల(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		extend_Signal_వెడల్పు(a, b, y, cell);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;

		std::సదిశ<పూర్ణాంకం> a_u, b_u;
		ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.వెనుక(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.వెనుక(), ez->vec_neg(b), b);
		} లేకపోతే {
			a_u = a;
			b_u = b;
		}

		std::సదిశ<పూర్ణాంకం> chain_buf = a_u;
		std::సదిశ<పూర్ణాంకం> y_u(a_u.కొలత(), ez->CONST_FALSE);
		కొరకు (పూర్ణాంకం i = పూర్ణాంకం(a.కొలత())-1; i >= 0; i--)
		{
			chain_buf.చొప్పించు(chain_buf.చివర(), chain_buf.కొలత(), ez->CONST_FALSE);

			std::సదిశ<పూర్ణాంకం> b_shl(i, ez->CONST_FALSE);
			b_shl.చొప్పించు(b_shl.చివర(), b_u.begin(), b_u.చివర());
			b_shl.చొప్పించు(b_shl.చివర(), chain_buf.కొలత()-b_shl.కొలత(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.కొలత(), chain_buf.చివర());
		}

		std::సదిశ<పూర్ణాంకం> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.కొలత());

		// modulo calculation
		std::సదిశ<పూర్ణాంకం> modulo_trunc;
		పూర్ణాంకం floored_eq_trunc;
		ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.వెనుక(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.వెనుక(), b.వెనుక()), ez->NOT(ez->వ్యక్తీకరణ(ezSAT::OpOr, modulo_trunc)));
		} లేకపోతే {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		ఒకవేళ (cell->type == ID($div)) {
			ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.వెనుక(), b.వెనుక()), ez->vec_neg(y_u), y_u)));
			లేకపోతే
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} లేకపోతే ఒకవేళ (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} లేకపోతే ఒకవేళ (cell->type == ID($divfloor)) {
			ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.వెనుక(), b.వెనుక()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_కూడు(y_u, ez->vec_const_unsigned(1, y_u.కొలత())),
						y_u
					)),
					y_u
				)));
			లేకపోతే
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} లేకపోతే ఒకవేళ (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_కూడు(modulo_trunc, b))));
		}

		ఒకవేళ (ignore_div_by_zero) {
			ez->assume(ez->వ్యక్తీకరణ(ezSAT::OpOr, b));
		} లేకపోతే {
			std::సదిశ<పూర్ణాంకం> div_zero_ఫలితం;
			ఒకవేళ (cell->type.లోపల(ID($div), ID($divfloor))) {
				ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::సదిశ<పూర్ణాంకం> all_ones(y.కొలత(), ez->CONST_TRUE);
					std::సదిశ<పూర్ణాంకం> only_మొదటి_one(y.కొలత(), ez->CONST_FALSE);
					only_మొదటి_one.at(0) = ez->CONST_TRUE;
					div_zero_ఫలితం = ez->vec_ite(a.వెనుక(), only_మొదటి_one, all_ones);
				} లేకపోతే {
					div_zero_ఫలితం.చొప్పించు(div_zero_ఫలితం.చివర(), cell->get_పోర్టు(ID::A).కొలత(), ez->CONST_TRUE);
					div_zero_ఫలితం.చొప్పించు(div_zero_ఫలితం.చివర(), y.కొలత() - div_zero_ఫలితం.కొలత(), ez->CONST_FALSE);
				}
			} లేకపోతే ఒకవేళ (cell->type.లోపల(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				పూర్ణాంకం copy_a_బిట్లు = min(cell->get_పోర్టు(ID::A).కొలత(), cell->get_పోర్టు(ID::B).కొలత());
				div_zero_ఫలితం.చొప్పించు(div_zero_ఫలితం.చివర(), a.begin(), a.begin() + copy_a_బిట్లు);
				ఒకవేళ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_ఫలితం.చొప్పించు(div_zero_ఫలితం.చివర(), y.కొలత() - div_zero_ఫలితం.కొలత(), div_zero_ఫలితం.వెనుక());
				లేకపోతే
					div_zero_ఫలితం.చొప్పించు(div_zero_ఫలితం.చివర(), y.కొలత() - div_zero_ఫలితం.కొలత(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->వ్యక్తీకరణ(ezSAT::OpOr, b), y_tmp, div_zero_ఫలితం)));
		}

		ఒకవేళ (model_undef) {
			log_assert(arith_undef_handled);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($lut))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		std::సదిశ<పూర్ణాంకం> lut;
		కొరకు (స్వయంచాలక బిట్టు : cell->getParam(ID::LUT))
			lut.నెట్టు_వెనుక(బిట్టు == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		అయితే (Get_కొలత(lut) < (1 << Get_కొలత(a)))
			lut.నెట్టు_వెనుక(ez->CONST_FALSE);
		lut.resize(1 << Get_కొలత(a));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> t(lut), u(Get_కొలత(t), ez->CONST_FALSE);

			కొరకు (పూర్ణాంకం i = Get_కొలత(a)-1; i >= 0; i--)
			{
				std::సదిశ<పూర్ణాంకం> t0(t.begin(), t.begin() + Get_కొలత(t)/2);
				std::సదిశ<పూర్ణాంకం> t1(t.begin() + Get_కొలత(t)/2, t.చివర());

				std::సదిశ<పూర్ణాంకం> u0(u.begin(), u.begin() + Get_కొలత(u)/2);
				std::సదిశ<పూర్ణాంకం> u1(u.begin() + Get_కొలత(u)/2, u.చివర());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_కొలత(t) == 1);
			log_assert(Get_కొలత(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep), u));
		}
		లేకపోతే
		{
			std::సదిశ<పూర్ణాంకం> t = lut;
			కొరకు (పూర్ణాంకం i = Get_కొలత(a)-1; i >= 0; i--)
			{
				std::సదిశ<పూర్ణాంకం> t0(t.begin(), t.begin() + Get_కొలత(t)/2);
				std::సదిశ<పూర్ణాంకం> t1(t.begin() + Get_కొలత(t)/2, t.చివర());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_కొలత(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($sop))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		పూర్ణాంకం y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep).at(0);

		పూర్ణాంకం వెడల్పు = cell->getParam(ID::వెడల్పు_2).as_int();
		పూర్ణాంకం లోతు = cell->getParam(ID::లోతు_2).as_int();

		సదిశ<State> పట్టిక_raw = cell->getParam(ID::పట్టిక).గమ్యం_బిట్లు();
		అయితే (Get_కొలత(పట్టిక_raw) < 2*వెడల్పు*లోతు)
			పట్టిక_raw.నెట్టు_వెనుక(State::S0);

		సదిశ<సదిశ<పూర్ణాంకం>> పట్టిక_2(లోతు);

		కొరకు (పూర్ణాంకం i = 0; i < లోతు; i++)
		కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు; j++)
		{
			తార్కిక pat0 = (పట్టిక_raw[2*వెడల్పు*i + 2*j + 0] == State::S1);
			తార్కిక pat1 = (పట్టిక_raw[2*వెడల్పు*i + 2*j + 1] == State::S1);

			ఒకవేళ (pat0 && !pat1)
				పట్టిక_2.at(i).నెట్టు_వెనుక(0);
			లేకపోతే ఒకవేళ (!pat0 && pat1)
				పట్టిక_2.at(i).నెట్టు_వెనుక(1);
			లేకపోతే
				పట్టిక_2.at(i).నెట్టు_వెనుక(-1);
		}

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> products, undef_products;
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			పూర్ణాంకం undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep).at(0);

			కొరకు (పూర్ణాంకం i = 0; i < లోతు; i++)
			{
				std::సదిశ<పూర్ణాంకం> cmp_a, cmp_ua, cmp_b;

				కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు; j++)
					ఒకవేళ (పట్టిక_2.at(i).at(j) >= 0) {
						cmp_a.నెట్టు_వెనుక(a.at(j));
						cmp_ua.నెట్టు_వెనుక(undef_a.at(j));
						cmp_b.నెట్టు_వెనుక(పట్టిక_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::సదిశ<పూర్ణాంకం> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::సదిశ<పూర్ణాంకం> masked_b = ez->vec_or(cmp_b, cmp_ua);

				పూర్ణాంకం masked_eq = ez->vec_eq(masked_a, masked_b);
				పూర్ణాంకం any_undef = ez->వ్యక్తీకరణ(ezSAT::OpOr, cmp_ua);

				undef_products.నెట్టు_వెనుక(ez->AND(any_undef, masked_eq));
				products.నెట్టు_వెనుక(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			పూర్ణాంకం yy = ez->వ్యక్తీకరణ(ezSAT::OpOr, products);
			ez->సమితి_2(undef_y, ez->AND(ez->NOT(yy), ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		లేకపోతే
		{
			std::సదిశ<పూర్ణాంకం> products;

			కొరకు (పూర్ణాంకం i = 0; i < లోతు; i++)
			{
				std::సదిశ<పూర్ణాంకం> cmp_a, cmp_b;

				కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు; j++)
					ఒకవేళ (పట్టిక_2.at(i).at(j) >= 0) {
						cmp_a.నెట్టు_వెనుక(a.at(j));
						cmp_b.నెట్టు_వెనుక(పట్టిక_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.నెట్టు_వెనుక(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->సమితి_2(y, ez->వ్యక్తీకరణ(ezSAT::OpOr, products));
		}

		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($fa))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> c = importDefSigSpec(cell->get_పోర్టు(ID::C), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		std::సదిశ<పూర్ణాంకం> x = importDefSigSpec(cell->get_పోర్టు(ID::X), timestep);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;
		std::సదిశ<పూర్ణాంకం> xx = model_undef ? ez->vec_var(x.కొలత()) : x;

		std::సదిశ<పూర్ణాంకం> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::సదిశ<పూర్ణాంకం> t2 = ez->vec_and(a, b);
		std::సదిశ<పూర్ణాంకం> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_c = importUndefSigSpec(cell->get_పోర్టు(ID::C), timestep);

			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			std::సదిశ<పూర్ణాంకం> undef_x = importUndefSigSpec(cell->get_పోర్టు(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($lcu))
	{
		std::సదిశ<పూర్ణాంకం> p = importDefSigSpec(cell->get_పోర్టు(ID::P), timestep);
		std::సదిశ<పూర్ణాంకం> g = importDefSigSpec(cell->get_పోర్టు(ID::G), timestep);
		std::సదిశ<పూర్ణాంకం> ci = importDefSigSpec(cell->get_పోర్టు(ID::CI), timestep);
		std::సదిశ<పూర్ణాంకం> co = importDefSigSpec(cell->get_పోర్టు(ID::CO), timestep);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(co.కొలత()) : co;

		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(co); i++)
			ez->సమితి_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_p = importUndefSigSpec(cell->get_పోర్టు(ID::P), timestep);
			std::సదిశ<పూర్ణాంకం> undef_g = importUndefSigSpec(cell->get_పోర్టు(ID::G), timestep);
			std::సదిశ<పూర్ణాంకం> undef_ci = importUndefSigSpec(cell->get_పోర్టు(ID::CI), timestep);
			std::సదిశ<పూర్ణాంకం> undef_co = importUndefSigSpec(cell->get_పోర్టు(ID::CO), timestep);

			పూర్ణాంకం undef_any_p = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_p);
			పూర్ణాంకం undef_any_g = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_g);
			పూర్ణాంకం undef_any_ci = ez->వ్యక్తీకరణ(ezSAT::OpOr, undef_ci);
			పూర్ణాంకం undef_co_బిట్టు = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::సదిశ<పూర్ణాంకం> undef_co_బిట్లు(undef_co.కొలత(), undef_co_బిట్టు);
			ez->assume(ez->vec_eq(undef_co_బిట్లు, undef_co));

			undefGating(co, yy, undef_co);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($alu))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> b = importDefSigSpec(cell->get_పోర్టు(ID::B), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		std::సదిశ<పూర్ణాంకం> x = importDefSigSpec(cell->get_పోర్టు(ID::X), timestep);
		std::సదిశ<పూర్ణాంకం> ci = importDefSigSpec(cell->get_పోర్టు(ID::CI), timestep);
		std::సదిశ<పూర్ణాంకం> bi = importDefSigSpec(cell->get_పోర్టు(ID::BI), timestep);
		std::సదిశ<పూర్ణాంకం> co = importDefSigSpec(cell->get_పోర్టు(ID::CO), timestep);

		extend_Signal_వెడల్పు(a, b, y, cell);
		extend_Signal_వెడల్పు(a, b, x, cell);
		extend_Signal_వెడల్పు(a, b, co, cell);

		std::సదిశ<పూర్ణాంకం> def_y = model_undef ? ez->vec_var(y.కొలత()) : y;
		std::సదిశ<పూర్ణాంకం> def_x = model_undef ? ez->vec_var(x.కొలత()) : x;
		std::సదిశ<పూర్ణాంకం> def_co = model_undef ? ez->vec_var(co.కొలత()) : co;

		log_assert(Get_కొలత(y) == Get_కొలత(x));
		log_assert(Get_కొలత(y) == Get_కొలత(co));
		log_assert(Get_కొలత(ci) == 1);
		log_assert(Get_కొలత(bi) == 1);

		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(y); i++)
		{
			పూర్ణాంకం s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->సమితి_2(def_x.at(i), ez->XOR(s1, s2));
			ez->సమితి_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->సమితి_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_b = importUndefSigSpec(cell->get_పోర్టు(ID::B), timestep);
			std::సదిశ<పూర్ణాంకం> undef_ci = importUndefSigSpec(cell->get_పోర్టు(ID::CI), timestep);
			std::సదిశ<పూర్ణాంకం> undef_bi = importUndefSigSpec(cell->get_పోర్టు(ID::BI), timestep);

			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			std::సదిశ<పూర్ణాంకం> undef_x = importUndefSigSpec(cell->get_పోర్టు(ID::X), timestep);
			std::సదిశ<పూర్ణాంకం> undef_co = importUndefSigSpec(cell->get_పోర్టు(ID::CO), timestep);

			extend_Signal_వెడల్పు(undef_a, undef_b, undef_y, cell);
			extend_Signal_వెడల్పు(undef_a, undef_b, undef_x, cell);
			extend_Signal_వెడల్పు(undef_a, undef_b, undef_co, cell);

			std::సదిశ<పూర్ణాంకం> all_inputs_undef;
			all_inputs_undef.చొప్పించు(all_inputs_undef.చివర(), undef_a.begin(), undef_a.చివర());
			all_inputs_undef.చొప్పించు(all_inputs_undef.చివర(), undef_b.begin(), undef_b.చివర());
			all_inputs_undef.చొప్పించు(all_inputs_undef.చివర(), undef_ci.begin(), undef_ci.చివర());
			all_inputs_undef.చొప్పించు(all_inputs_undef.చివర(), undef_bi.begin(), undef_bi.చివర());
			పూర్ణాంకం undef_any = ez->వ్యక్తీకరణ(ezSAT::OpOr, all_inputs_undef);

			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(undef_y); i++) {
				ez->సమితి_2(undef_y.at(i), undef_any);
				ez->సమితి_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->సమితి_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($ముక్క))
	{
		RTLIL::SigSpec a = cell->get_పోర్టు(ID::A);
		RTLIL::SigSpec y = cell->get_పోర్టు(ID::Y);
		ez->assume(సంకేతాలు_eq(a.వెలికితీయి(cell->parameters.at(ID::స్థానభ్రంశం_2).as_int(), y.కొలత()), y, timestep));
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_పోర్టు(ID::A);
		RTLIL::SigSpec b = cell->get_పోర్టు(ID::B);
		RTLIL::SigSpec y = cell->get_పోర్టు(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(సంకేతాలు_eq(ab, y, timestep));
		తిరిగి నిజం;
	}

	ఒకవేళ (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_దత్తాంశం ff(శూన్యసూచిక, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		ఒకవేళ (ff.has_aload || ff.has_arst || ff.has_sr)
			తిరిగి అబద్ధం;

		ఒకవేళ (timestep == 1)
		{
			initial_state.కూడు((*sigmap)(cell->get_పోర్టు(ID::Q)));
			ఒకవేళ (model_undef && def_formal) {
				std::సదిశ<పూర్ణాంకం> undef_q = importUndefSigSpec(cell->get_పోర్టు(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		లేకపోతే
		{
			std::సదిశ<పూర్ణాంకం> d = importDefSigSpec(cell->get_పోర్టు(ID::D), timestep-1);
			std::సదిశ<పూర్ణాంకం> undef_d;
			ఒకవేళ (model_undef)
				undef_d = importUndefSigSpec(cell->get_పోర్టు(ID::D), timestep-1);
			ఒకవేళ (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				పూర్ణాంకం srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::సదిశ<పూర్ణాంకం> rval = importDefSigSpec(ff.val_srst, timestep-1);
				పూర్ణాంకం undef_srst = -1;
				std::సదిశ<పూర్ణాంకం> undef_rval;
				ఒకవేళ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ఒకవేళ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				లేకపోతే
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			ఒకవేళ (ff.has_ce) {
				పూర్ణాంకం ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::సదిశ<పూర్ణాంకం> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				పూర్ణాంకం undef_ce = -1;
				std::సదిశ<పూర్ణాంకం> undef_old_q;
				ఒకవేళ (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				ఒకవేళ (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				లేకపోతే
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			ఒకవేళ (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				పూర్ణాంకం srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::సదిశ<పూర్ణాంకం> rval = importDefSigSpec(ff.val_srst, timestep-1);
				పూర్ణాంకం undef_srst = -1;
				std::సదిశ<పూర్ణాంకం> undef_rval;
				ఒకవేళ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ఒకవేళ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				లేకపోతే
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::సదిశ<పూర్ణాంకం> q = importDefSigSpec(cell->get_పోర్టు(ID::Q), timestep);

			std::సదిశ<పూర్ణాంకం> qq = model_undef ? ez->vec_var(q.కొలత()) : q;
			ez->assume(ez->vec_eq(d, qq));

			ఒకవేళ (model_undef)
			{
				std::సదిశ<పూర్ణాంకం> undef_q = importUndefSigSpec(cell->get_పోర్టు(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($anyconst))
	{
		ఒకవేళ (timestep < 2) {
			ఒకవేళ (model_undef && def_formal) {
				std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			తిరిగి నిజం;
		}

		std::సదిశ<పూర్ణాంకం> d = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep-1);
		std::సదిశ<పూర్ణాంకం> q = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

		std::సదిశ<పూర్ణాంకం> qq = (model_undef && !def_formal) ? ez->vec_var(q.కొలత()) : q;
		ez->assume(ez->vec_eq(d, qq));

		ఒకవేళ (model_undef)
		{
			std::సదిశ<పూర్ణాంకం> undef_d = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep-1);
			std::సదిశ<పూర్ణాంకం> undef_q = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);

			ఒకవేళ (def_formal) {
				కొరకు (స్వయంచాలక &undef_q_బిట్టు : undef_q)
					ez->సమితి_2(ez->CONST_FALSE, undef_q_బిట్టు);
			} లేకపోతే {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($anyseq))
	{
		ఒకవేళ (model_undef && def_formal) {
			std::సదిశ<పూర్ణాంకం> undef_q = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			కొరకు (స్వయంచాలక &undef_q_బిట్టు : undef_q)
				ez->సమితి_2(ez->CONST_FALSE, undef_q_బిట్టు);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($barrier) && !model_barriers)
		తిరిగి నిజం;

	ఒకవేళ (cell->type.లోపల(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::సదిశ<పూర్ణాంకం> a = importDefSigSpec(cell->get_పోర్టు(ID::A), timestep);
		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		extend_Signal_వెడల్పు_Unary(a, y, cell);

		std::సదిశ<పూర్ణాంకం> yy = model_undef ? ez->vec_var(y.కొలత()) : y;
		ez->assume(ez->vec_eq(a, yy));

		ఒకవేళ (model_undef) {
			std::సదిశ<పూర్ణాంకం> undef_a = importUndefSigSpec(cell->get_పోర్టు(ID::A), timestep);
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			extend_Signal_వెడల్పు_Unary(undef_a, undef_y, cell, అబద్ధం);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($initstate))
	{
		స్వయంచాలక కీలకం = make_pair(ఉపసర్గ_2, timestep);
		ఒకవేళ (initstates.లెక్క(కీలకం) == 0)
			initstates[కీలకం] = అబద్ధం;

		std::సదిశ<పూర్ణాంకం> y = importDefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
		log_assert(Get_కొలత(y) == 1);
		ez->సమితి_2(y[0], initstates[కీలకం] ? ez->CONST_TRUE : ez->CONST_FALSE);

		ఒకవేళ (model_undef) {
			std::సదిశ<పూర్ణాంకం> undef_y = importUndefSigSpec(cell->get_పోర్టు(ID::Y), timestep);
			log_assert(Get_కొలత(undef_y) == 1);
			ez->సమితి_2(undef_y[0], ez->CONST_FALSE);
		}

		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($ప్రతిజ్ఞా))
	{
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_పోర్టు(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_పోర్టు(ID::EN)));
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($assume))
	{
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_పోర్టు(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_పోర్టు(ID::EN)));
		తిరిగి నిజం;
	}

	ఒకవేళ (cell->type == ID($scopeinfo) || cell->type == ID($ఆదానం_పోర్టు))
	{
		తిరిగి నిజం;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	తిరిగి అబద్ధం;
}

నామాకాశ Yosys {

శూన్య report_missing_model(తార్కిక warn_only, RTLIL::Cell* cell)
{
	std::string s;
	ఒకవేళ (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	లేకపోతే
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	ఒకవేళ (warn_only) {
		log_warning_noprefix("%s\n", s);
	} లేకపోతే {
		log_error("%s\n", s);
	}
}

}
