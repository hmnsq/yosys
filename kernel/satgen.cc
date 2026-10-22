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

#include "kernel/satgen.h"
#include "kernel/ff.h"
#include "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

bool SatGen::importCell(RTLIL::Cell *cell, ចំនួនគត់ timestep)
{
	bool arith_undef_handled = false;
	bool is_arith_compare = cell->type.in(ID($lt), ID($le), ID($ge), ID($gt));

	ប្រសិនបើ (model_undef && (cell->type.in(ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		ប្រសិនបើ (is_arith_compare)
			extendSignalWidth(undef_a, undef_b, cell, true);
		បើមិនដូច្នេះ
			extendSignalWidth(undef_a, undef_b, undef_y, cell, true);

		ចំនួនគត់ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
		ចំនួនគត់ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
		ចំនួនគត់ undef_y_bit = ez->OR(undef_any_a, undef_any_b);

		ប្រសិនបើ (cell->type.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			std::vector<ចំនួនគត់> b = importSigSpec(cell->getPort(ID::B), timestep);
			undef_y_bit = ez->OR(undef_y_bit, ez->NOT(ez->expression(ezSAT::OpOr, b)));
		}

		ប្រសិនបើ (is_arith_compare) {
			សម្រាប់ (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));
		} បើមិនដូច្នេះ {
			std::vector<ចំនួនគត់> undef_y_bits(undef_y.size(), undef_y_bit);
			ez->assume(ez->vec_eq(undef_y_bits, undef_y));
		}

		arith_undef_handled = true;
	}

	ប្រសិនបើ (cell->type.in(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($add), ID($sub)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		ប្រសិនបើ (cell->type.in(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		ប្រសិនបើ (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		ប្រសិនបើ (cell->type.in(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		ប្រសិនបើ (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		ប្រសិនបើ (cell->type.in(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		ប្រសិនបើ (cell->type.in(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		ប្រសិនបើ (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		ប្រសិនបើ (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		ប្រសិនបើ (cell->type == ID($add))
			ez->assume(ez->vec_eq(ez->vec_add(a, b), yy));
		ប្រសិនបើ (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		ប្រសិនបើ (model_undef && !arith_undef_handled)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, undef_y, cell, false);

			ប្រសិនបើ (cell->type.in(ID($and), ID($_AND_), ID($_NAND_))) {
				std::vector<ចំនួនគត់> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<ចំនួនគត់> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<ចំនួនគត់> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			បើមិនដូច្នេះ ប្រសិនបើ (cell->type.in(ID($or), ID($_OR_), ID($_NOR_))) {
				std::vector<ចំនួនគត់> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<ចំនួនគត់> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<ចំនួនគត់> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			បើមិនដូច្នេះ ប្រសិនបើ (cell->type.in(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::vector<ចំនួនគត់> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($_ANDNOT_)) {
				std::vector<ចំនួនគត់> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::vector<ចំនួនគត់> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::vector<ចំនួនគត់> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($_ORNOT_)) {
				std::vector<ចំនួនគត់> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::vector<ចំនួនគត់> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::vector<ចំនួនគត់> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			បើមិនដូច្នេះ
				log_abort();

			undefGating(y, yy, undef_y);
		}
		បើមិនដូច្នេះ ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		bool aoi_mode = cell->type.in(ID($_AOI3_), ID($_AOI4_));
		bool three_mode = cell->type.in(ID($_AOI3_), ID($_OAI3_));

		ចំនួនគត់ a = importDefSigSpec(cell->getPort(ID::A), timestep).at(0);
		ចំនួនគត់ b = importDefSigSpec(cell->getPort(ID::B), timestep).at(0);
		ចំនួនគត់ c = importDefSigSpec(cell->getPort(ID::C), timestep).at(0);
		ចំនួនគត់ d = three_mode ? (aoi_mode ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->getPort(ID::D), timestep).at(0);
		ចំនួនគត់ y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);
		ចំនួនគត់ yy = model_undef ? ez->literal() : y;

		ប្រសិនបើ (cell->type.in(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		បើមិនដូច្នេះ
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		ប្រសិនបើ (model_undef)
		{
			ចំនួនគត់ undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep).at(0);
			ចំនួនគត់ undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep).at(0);
			ចំនួនគត់ undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep).at(0);
			ចំនួនគត់ undef_d = three_mode ? ez->CONST_FALSE : importUndefSigSpec(cell->getPort(ID::D), timestep).at(0);
			ចំនួនគត់ undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			ប្រសិនបើ (aoi_mode)
			{
				ចំនួនគត់ a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				ចំនួនគត់ b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				ចំនួនគត់ c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				ចំនួនគត់ d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				ចំនួនគត់ ab = ez->AND(a, b), cd = ez->AND(c, d);
				ចំនួនគត់ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				ចំនួនគត់ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				ចំនួនគត់ ab1 = ez->AND(ab, ez->NOT(undef_ab));
				ចំនួនគត់ cd1 = ez->AND(cd, ez->NOT(undef_cd));
				ចំនួនគត់ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			បើមិនដូច្នេះ
			{
				ចំនួនគត់ a1 = ez->AND(a, ez->NOT(undef_a));
				ចំនួនគត់ b1 = ez->AND(b, ez->NOT(undef_b));
				ចំនួនគត់ c1 = ez->AND(c, ez->NOT(undef_c));
				ចំនួនគត់ d1 = ez->AND(d, ez->NOT(undef_d));

				ចំនួនគត់ ab = ez->OR(a, b), cd = ez->OR(c, d);
				ចំនួនគត់ undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				ចំនួនគត់ undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				ចំនួនគត់ ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				ចំនួនគត់ cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				ចំនួនគត់ yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($_NOT_), ID($not)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		ប្រសិនបើ (model_undef) {
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, false);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($bweqx))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<ចំនួនគត់> bweqx = ez->vec_not(ez->vec_xor(a, b));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<ចំនួនគត់> both_undef = ez->vec_and(undef_a, undef_b);
			std::vector<ចំនួនគត់> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			សម្រាប់ (ចំនួនគត់ yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;
		ប្រសិនបើ (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		បើមិនដូច្នេះ
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			std::vector<ចំនួនគត់> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<ចំនួនគត់> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::vector<ចំនួនគត់> yX;
			ប្រសិនបើ (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			បើមិនដូច្នេះ
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($bmux))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<ចំនួនគត់> undef_a, undef_s, undef_y;

		ប្រសិនបើ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		ប្រសិនបើ (GetSize(s) == 0) {
			ez->vec_set(a, y);
			ប្រសិនបើ (model_undef)
				ez->vec_set(undef_a, undef_y);
		} បើមិនដូច្នេះ {
			សម្រាប់ (ចំនួនគត់ i = GetSize(s)-1; i >= 0; i--)
			{
				std::vector<ចំនួនគត់> out = (i == 0) ? y : ez->vec_var(a.size() / 2);
				std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(out.size()) : out;

				std::vector<ចំនួនគត់> a0(a.begin(), a.begin() + a.size() / 2);
				std::vector<ចំនួនគត់> a1(a.begin() + a.size() / 2, a.end());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				ប្រសិនបើ (model_undef)
				{
					std::vector<ចំនួនគត់> undef_out = (i == 0) ? undef_y : ez->vec_var(a.size() / 2);
					std::vector<ចំនួនគត់> undef_a0(undef_a.begin(), undef_a.begin() + a.size() / 2);
					std::vector<ចំនួនគត់> undef_a1(undef_a.begin() + a.size() / 2, undef_a.end());
					std::vector<ចំនួនគត់> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::vector<ចំនួនគត់> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::vector<ចំនួនគត់> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_out));
					undefGating(out, yy, undef_out);

					undef_a = undef_out;
				}

				a = out;
			}
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($demux))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<ចំនួនគត់> undef_a, undef_s, undef_y;

		ប្រសិនបើ (model_undef)
		{
			undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
		}

		ប្រសិនបើ (GetSize(s) == 0) {
			ez->vec_set(a, y);
			ប្រសិនបើ (model_undef)
				ez->vec_set(undef_a, undef_y);
		} បើមិនដូច្នេះ {
			សម្រាប់ (ចំនួនគត់ i = 0; i < (1 << GetSize(s)); i++)
			{
				std::vector<ចំនួនគត់> ss;
				សម្រាប់ (ចំនួនគត់ j = 0; j < GetSize(s); j++) {
					ប្រសិនបើ (i & 1 << j)
						ss.push_back(s[j]);
					បើមិនដូច្នេះ
						ss.push_back(ez->NOT(s[j]));
				}
				ចំនួនគត់ sss = ez->expression(ezSAT::OpAnd, ss);

				សម្រាប់ (ចំនួនគត់ j = 0; j < GetSize(a); j++) {
					ez->SET(ez->AND(sss, a[j]), yy.at(i * GetSize(a) + j));
				}

				ប្រសិនបើ (model_undef)
				{
					ចំនួនគត់ s0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					ចំនួនគត់ us = ez->AND(ez->NOT(s0), ez->expression(ezSAT::OpOr, undef_s));
					សម្រាប់ (ចំនួនគត់ j = 0; j < GetSize(a); j++) {
						ចំនួនគត់ a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						ចំនួនគត់ yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->SET(yX, undef_y.at(i * GetSize(a) + j));
					}
				}
			}
			ប្រសិនបើ (model_undef)
				undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($pmux))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> s = importDefSigSpec(cell->getPort(ID::S), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		std::vector<ចំនួនគត់> tmp = a;
		សម្រាប់ (size_t i = 0; i < s.size(); i++) {
			std::vector<ចំនួនគត់> part_of_b(b.begin()+i*a.size(), b.begin()+(i+1)*a.size());
			tmp = ez->vec_ite(s.at(i), part_of_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_s = importUndefSigSpec(cell->getPort(ID::S), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			ចំនួនគត់ all_undef = ez->CONST_FALSE;
			ចំនួនគត់ found_active = ez->CONST_FALSE;

			std::vector<ចំនួនគត់> undef_tmp = undef_a;

			សម្រាប់ (size_t i = 0; i < s.size(); i++)
			{
				std::vector<ចំនួនគត់> part_of_undef_b(undef_b.begin()+i*a.size(), undef_b.begin()+(i+1)*a.size());

				undef_tmp = ez->vec_ite(s.at(i), part_of_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), found_active));
				found_active = ez->OR(found_active, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::vector<ចំនួនគត់>(a.size(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($pos), ID($buf), ID($neg)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		ប្រសិនបើ (cell->type.in(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} បើមិនដូច្នេះ {
			std::vector<ចំនួនគត់> zero(a.size(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell);

			ប្រសិនបើ (cell->type.in(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} បើមិនដូច្នេះ {
				ចំនួនគត់ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
				std::vector<ចំនួនគត់> undef_y_bits(undef_y.size(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_bits, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		ប្រសិនបើ (cell->type == ID($reduce_and))
			ez->SET(ez->expression(ez->OpAnd, a), yy.at(0));
		ប្រសិនបើ (cell->type.in(ID($reduce_or), ID($reduce_bool)))
			ez->SET(ez->expression(ez->OpOr, a), yy.at(0));
		ប្រសិនបើ (cell->type == ID($reduce_xor))
			ez->SET(ez->expression(ez->OpXor, a), yy.at(0));
		ប្រសិនបើ (cell->type == ID($reduce_xnor))
			ez->SET(ez->NOT(ez->expression(ez->OpXor, a)), yy.at(0));
		ប្រសិនបើ (cell->type == ID($logic_not))
			ez->SET(ez->NOT(ez->expression(ez->OpOr, a)), yy.at(0));
		សម្រាប់ (size_t i = 1; i < y.size(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			ចំនួនគត់ aX = ez->expression(ezSAT::OpOr, undef_a);

			ប្រសិនបើ (cell->type == ID($reduce_and)) {
				ចំនួនគត់ a0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			បើមិនដូច្នេះ ប្រសិនបើ (cell->type.in(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				ចំនួនគត់ a1 = ez->expression(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			បើមិនដូច្នេះ ប្រសិនបើ (cell->type.in(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} បើមិនដូច្នេះ
				log_abort();

			សម្រាប់ (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($logic_and), ID($logic_or)))
	{
		std::vector<ចំនួនគត់> vec_a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> vec_b = importDefSigSpec(cell->getPort(ID::B), timestep);

		ចំនួនគត់ a = ez->expression(ez->OpOr, vec_a);
		ចំនួនគត់ b = ez->expression(ez->OpOr, vec_b);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		ប្រសិនបើ (cell->type == ID($logic_and))
			ez->SET(ez->expression(ez->OpAnd, a, b), yy.at(0));
		បើមិនដូច្នេះ
			ez->SET(ez->expression(ez->OpOr, a, b), yy.at(0));
		សម្រាប់ (size_t i = 1; i < y.size(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			ចំនួនគត់ a0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_a), ez->expression(ezSAT::OpOr, undef_a)));
			ចំនួនគត់ b0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_b), ez->expression(ezSAT::OpOr, undef_b)));
			ចំនួនគត់ a1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			ចំនួនគត់ b1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			ចំនួនគត់ aX = ez->expression(ezSAT::OpOr, undef_a);
			ចំនួនគត់ bX = ez->expression(ezSAT::OpOr, undef_b);

			ប្រសិនបើ (cell->type == ID($logic_and))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($logic_or))
				ez->SET(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			បើមិនដូច្នេះ
				log_abort();

			សម្រាប់ (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		bool is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, cell);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		ប្រសិនបើ (model_undef && cell->type.in(ID($eqx), ID($nex))) {
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			extendSignalWidth(undef_a, undef_b, cell, true);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		ប្រសិនបើ (cell->type == ID($lt))
			ez->SET(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		ប្រសិនបើ (cell->type == ID($le))
			ez->SET(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		ប្រសិនបើ (cell->type.in(ID($eq), ID($eqx)))
			ez->SET(ez->vec_eq(a, b), yy.at(0));
		ប្រសិនបើ (cell->type.in(ID($ne), ID($nex)))
			ez->SET(ez->vec_ne(a, b), yy.at(0));
		ប្រសិនបើ (cell->type == ID($ge))
			ez->SET(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		ប្រសិនបើ (cell->type == ID($gt))
			ez->SET(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		សម្រាប់ (size_t i = 1; i < y.size(); i++)
			ez->SET(ez->CONST_FALSE, yy.at(i));

		ប្រសិនបើ (model_undef && cell->type.in(ID($eqx), ID($nex)))
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, true);

			ប្រសិនបើ (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			បើមិនដូច្នេះ
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			សម្រាប់ (size_t i = 0; i < y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		បើមិនដូច្នេះ ប្រសិនបើ (model_undef && cell->type.in(ID($eq), ID($ne)))
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidth(undef_a, undef_b, cell, true);

			ចំនួនគត់ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			ចំនួនគត់ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			ចំនួនគត់ undef_any = ez->OR(undef_any_a, undef_any_b);

			std::vector<ចំនួនគត់> masked_a_bits = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::vector<ចំនួនគត់> masked_b_bits = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			ចំនួនគត់ masked_ne = ez->vec_ne(masked_a_bits, masked_b_bits);
			ចំនួនគត់ undef_y_bit = ez->AND(undef_any, ez->NOT(masked_ne));

			សម្រាប់ (size_t i = 1; i < undef_y.size(); i++)
				ez->SET(ez->CONST_FALSE, undef_y.at(i));
			ez->SET(undef_y_bit, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		បើមិនដូច្នេះ
		{
			ប្រសិនបើ (model_undef) {
				std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		ចំនួនគត់ extend_bit = ez->CONST_FALSE;

		ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool())
			extend_bit = a.back();

		ខណៈពេល (y.size() < a.size())
			y.push_back(ez->literal());
		ខណៈពេល (y.size() > a.size())
			a.push_back(extend_bit);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<ចំនួនគត់> shifted_a;

		ប្រសិនបើ (cell->type.in( ID($shl), ID($sshl)))
			shifted_a = ez->vec_shift_left(a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

		ប្រសិនបើ (cell->type == ID($shr))
			shifted_a = ez->vec_shift_right(a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

		ប្រសិនបើ (cell->type == ID($sshr))
			shifted_a = ez->vec_shift_right(a, b, false, cell->parameters[ID::A_SIGNED].as_bool() ? a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

		ប្រសិនបើ (cell->type.in(ID($shift), ID($shiftx)))
			shifted_a = ez->vec_shift_right(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<ចំនួនគត់> undef_a_shifted;

			extend_bit = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool())
				extend_bit = undef_a.back();

			ខណៈពេល (undef_y.size() < undef_a.size())
				undef_y.push_back(ez->literal());
			ខណៈពេល (undef_y.size() > undef_a.size())
				undef_a.push_back(extend_bit);

			ប្រសិនបើ (cell->type.in(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_shift_left(undef_a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

			ប្រសិនបើ (cell->type == ID($shr))
				undef_a_shifted = ez->vec_shift_right(undef_a, b, false, ez->CONST_FALSE, ez->CONST_FALSE);

			ប្រសិនបើ (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_shift_right(undef_a, b, false, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.back() : ez->CONST_FALSE, ez->CONST_FALSE);

			ប្រសិនបើ (cell->type == ID($shift))
				undef_a_shifted = ez->vec_shift_right(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			ប្រសិនបើ (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_shift_right(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			ចំនួនគត់ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			std::vector<ចំនួនគត់> undef_all_y_bits(undef_y.size(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_bits), undef_y));
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($mul))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		std::vector<ចំនួនគត់> tmp(a.size(), ez->CONST_FALSE);
		សម្រាប់ (ចំនួនគត់ i = 0; i < ចំនួនគត់(a.size()); i++)
		{
			std::vector<ចំនួនគត់> shifted_a(a.size(), ez->CONST_FALSE);
			សម្រាប់ (ចំនួនគត់ j = i; j < ចំនួនគត់(a.size()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_add(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		ប្រសិនបើ (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($macc), ID($macc_v2)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::vector<ចំនួនគត់> tmp(GetSize(y), ez->CONST_FALSE);

		សម្រាប់ (auto &port : macc.terms)
		{
			std::vector<ចំនួនគត់> in_a = importDefSigSpec(port.in_a, timestep);
			std::vector<ចំនួនគត់> in_b = importDefSigSpec(port.in_b, timestep);

			ខណៈពេល (GetSize(in_a) < GetSize(y))
				in_a.push_back(port.is_signed && !in_a.empty() ? in_a.back() : ez->CONST_FALSE);
			in_a.resize(GetSize(y));

			ប្រសិនបើ (GetSize(in_b))
			{
				ខណៈពេល (GetSize(in_b) < GetSize(y))
					in_b.push_back(port.is_signed && !in_b.empty() ? in_b.back() : ez->CONST_FALSE);
				in_b.resize(GetSize(y));

				សម្រាប់ (ចំនួនគត់ i = 0; i < GetSize(in_b); i++) {
					std::vector<ចំនួនគត់> shifted_a(in_a.size(), ez->CONST_FALSE);
					សម្រាប់ (ចំនួនគត់ j = i; j < ចំនួនគត់(in_a.size()); j++)
						shifted_a.at(j) = in_a.at(j-i);
					ប្រសិនបើ (port.do_subtract)
						tmp = ez->vec_ite(in_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					បើមិនដូច្នេះ
						tmp = ez->vec_ite(in_b.at(i), ez->vec_add(tmp, shifted_a), tmp);
				}
			}
			បើមិនដូច្នេះ
			{
				ប្រសិនបើ (port.do_subtract)
					tmp = ez->vec_sub(tmp, in_a);
				បើមិនដូច្នេះ
					tmp = ez->vec_add(tmp, in_a);
			}
		}

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_c;

			ប្រសិនបើ (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			ចំនួនគត់ undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			ចំនួនគត់ undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			ចំនួនគត់ undef_any_c = ez->expression(ezSAT::OpOr, undef_c);
			ចំនួនគត់ undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::vector<ចំនួនគត់>(GetSize(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		បើមិនដូច្នេះ
			ez->assume(ez->vec_eq(y, tmp));

		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type.in(ID($div), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidth(a, b, y, cell);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;

		std::vector<ចំនួនគត់> a_u, b_u;
		ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.back(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.back(), ez->vec_neg(b), b);
		} បើមិនដូច្នេះ {
			a_u = a;
			b_u = b;
		}

		std::vector<ចំនួនគត់> chain_buf = a_u;
		std::vector<ចំនួនគត់> y_u(a_u.size(), ez->CONST_FALSE);
		សម្រាប់ (ចំនួនគត់ i = ចំនួនគត់(a.size())-1; i >= 0; i--)
		{
			chain_buf.insert(chain_buf.end(), chain_buf.size(), ez->CONST_FALSE);

			std::vector<ចំនួនគត់> b_shl(i, ez->CONST_FALSE);
			b_shl.insert(b_shl.end(), b_u.begin(), b_u.end());
			b_shl.insert(b_shl.end(), chain_buf.size()-b_shl.size(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.size(), chain_buf.end());
		}

		std::vector<ចំនួនគត់> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.size());

		// modulo calculation
		std::vector<ចំនួនគត់> modulo_trunc;
		ចំនួនគត់ floored_eq_trunc;
		ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.back(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.back(), b.back()), ez->NOT(ez->expression(ezSAT::OpOr, modulo_trunc)));
		} បើមិនដូច្នេះ {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		ប្រសិនបើ (cell->type == ID($div)) {
			ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.back(), b.back()), ez->vec_neg(y_u), y_u)));
			បើមិនដូច្នេះ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($divfloor)) {
			ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.back(), b.back()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_add(y_u, ez->vec_const_unsigned(1, y_u.size())),
						y_u
					)),
					y_u
				)));
			បើមិនដូច្នេះ
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} បើមិនដូច្នេះ ប្រសិនបើ (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_add(modulo_trunc, b))));
		}

		ប្រសិនបើ (ignore_div_by_zero) {
			ez->assume(ez->expression(ezSAT::OpOr, b));
		} បើមិនដូច្នេះ {
			std::vector<ចំនួនគត់> div_zero_result;
			ប្រសិនបើ (cell->type.in(ID($div), ID($divfloor))) {
				ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::vector<ចំនួនគត់> all_ones(y.size(), ez->CONST_TRUE);
					std::vector<ចំនួនគត់> only_first_one(y.size(), ez->CONST_FALSE);
					only_first_one.at(0) = ez->CONST_TRUE;
					div_zero_result = ez->vec_ite(a.back(), only_first_one, all_ones);
				} បើមិនដូច្នេះ {
					div_zero_result.insert(div_zero_result.end(), cell->getPort(ID::A).size(), ez->CONST_TRUE);
					div_zero_result.insert(div_zero_result.end(), y.size() - div_zero_result.size(), ez->CONST_FALSE);
				}
			} បើមិនដូច្នេះ ប្រសិនបើ (cell->type.in(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				ចំនួនគត់ copy_a_bits = min(cell->getPort(ID::A).size(), cell->getPort(ID::B).size());
				div_zero_result.insert(div_zero_result.end(), a.begin(), a.begin() + copy_a_bits);
				ប្រសិនបើ (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_result.insert(div_zero_result.end(), y.size() - div_zero_result.size(), div_zero_result.back());
				បើមិនដូច្នេះ
					div_zero_result.insert(div_zero_result.end(), y.size() - div_zero_result.size(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->expression(ezSAT::OpOr, b), y_tmp, div_zero_result)));
		}

		ប្រសិនបើ (model_undef) {
			log_assert(arith_undef_handled);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($lut))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<ចំនួនគត់> lut;
		សម្រាប់ (auto bit : cell->getParam(ID::LUT))
			lut.push_back(bit == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		ខណៈពេល (GetSize(lut) < (1 << GetSize(a)))
			lut.push_back(ez->CONST_FALSE);
		lut.resize(1 << GetSize(a));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> t(lut), u(GetSize(t), ez->CONST_FALSE);

			សម្រាប់ (ចំនួនគត់ i = GetSize(a)-1; i >= 0; i--)
			{
				std::vector<ចំនួនគត់> t0(t.begin(), t.begin() + GetSize(t)/2);
				std::vector<ចំនួនគត់> t1(t.begin() + GetSize(t)/2, t.end());

				std::vector<ចំនួនគត់> u0(u.begin(), u.begin() + GetSize(u)/2);
				std::vector<ចំនួនគត់> u1(u.begin() + GetSize(u)/2, u.end());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(GetSize(t) == 1);
			log_assert(GetSize(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->getPort(ID::Y), timestep), u));
		}
		បើមិនដូច្នេះ
		{
			std::vector<ចំនួនគត់> t = lut;
			សម្រាប់ (ចំនួនគត់ i = GetSize(a)-1; i >= 0; i--)
			{
				std::vector<ចំនួនគត់> t0(t.begin(), t.begin() + GetSize(t)/2);
				std::vector<ចំនួនគត់> t1(t.begin() + GetSize(t)/2, t.end());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(GetSize(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($sop))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		ចំនួនគត់ y = importDefSigSpec(cell->getPort(ID::Y), timestep).at(0);

		ចំនួនគត់ width = cell->getParam(ID::WIDTH).as_int();
		ចំនួនគត់ depth = cell->getParam(ID::DEPTH).as_int();

		vector<State> table_raw = cell->getParam(ID::TABLE).to_bits();
		ខណៈពេល (GetSize(table_raw) < 2*width*depth)
			table_raw.push_back(State::S0);

		vector<vector<ចំនួនគត់>> table(depth);

		សម្រាប់ (ចំនួនគត់ i = 0; i < depth; i++)
		សម្រាប់ (ចំនួនគត់ j = 0; j < width; j++)
		{
			bool pat0 = (table_raw[2*width*i + 2*j + 0] == State::S1);
			bool pat1 = (table_raw[2*width*i + 2*j + 1] == State::S1);

			ប្រសិនបើ (pat0 && !pat1)
				table.at(i).push_back(0);
			បើមិនដូច្នេះ ប្រសិនបើ (!pat0 && pat1)
				table.at(i).push_back(1);
			បើមិនដូច្នេះ
				table.at(i).push_back(-1);
		}

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> products, undef_products;
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			ចំនួនគត់ undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep).at(0);

			សម្រាប់ (ចំនួនគត់ i = 0; i < depth; i++)
			{
				std::vector<ចំនួនគត់> cmp_a, cmp_ua, cmp_b;

				សម្រាប់ (ចំនួនគត់ j = 0; j < width; j++)
					ប្រសិនបើ (table.at(i).at(j) >= 0) {
						cmp_a.push_back(a.at(j));
						cmp_ua.push_back(undef_a.at(j));
						cmp_b.push_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::vector<ចំនួនគត់> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::vector<ចំនួនគត់> masked_b = ez->vec_or(cmp_b, cmp_ua);

				ចំនួនគត់ masked_eq = ez->vec_eq(masked_a, masked_b);
				ចំនួនគត់ any_undef = ez->expression(ezSAT::OpOr, cmp_ua);

				undef_products.push_back(ez->AND(any_undef, masked_eq));
				products.push_back(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			ចំនួនគត់ yy = ez->expression(ezSAT::OpOr, products);
			ez->SET(undef_y, ez->AND(ez->NOT(yy), ez->expression(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		បើមិនដូច្នេះ
		{
			std::vector<ចំនួនគត់> products;

			សម្រាប់ (ចំនួនគត់ i = 0; i < depth; i++)
			{
				std::vector<ចំនួនគត់> cmp_a, cmp_b;

				សម្រាប់ (ចំនួនគត់ j = 0; j < width; j++)
					ប្រសិនបើ (table.at(i).at(j) >= 0) {
						cmp_a.push_back(a.at(j));
						cmp_b.push_back(table.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.push_back(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->SET(y, ez->expression(ezSAT::OpOr, products));
		}

		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($fa))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> c = importDefSigSpec(cell->getPort(ID::C), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<ចំនួនគត់> x = importDefSigSpec(cell->getPort(ID::X), timestep);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<ចំនួនគត់> xx = model_undef ? ez->vec_var(x.size()) : x;

		std::vector<ចំនួនគត់> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::vector<ចំនួនគត់> t2 = ez->vec_and(a, b);
		std::vector<ចំនួនគត់> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_c = importUndefSigSpec(cell->getPort(ID::C), timestep);

			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<ចំនួនគត់> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($lcu))
	{
		std::vector<ចំនួនគត់> p = importDefSigSpec(cell->getPort(ID::P), timestep);
		std::vector<ចំនួនគត់> g = importDefSigSpec(cell->getPort(ID::G), timestep);
		std::vector<ចំនួនគត់> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<ចំនួនគត់> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(co.size()) : co;

		សម្រាប់ (ចំនួនគត់ i = 0; i < GetSize(co); i++)
			ez->SET(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_p = importUndefSigSpec(cell->getPort(ID::P), timestep);
			std::vector<ចំនួនគត់> undef_g = importUndefSigSpec(cell->getPort(ID::G), timestep);
			std::vector<ចំនួនគត់> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<ចំនួនគត់> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			ចំនួនគត់ undef_any_p = ez->expression(ezSAT::OpOr, undef_p);
			ចំនួនគត់ undef_any_g = ez->expression(ezSAT::OpOr, undef_g);
			ចំនួនគត់ undef_any_ci = ez->expression(ezSAT::OpOr, undef_ci);
			ចំនួនគត់ undef_co_bit = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::vector<ចំនួនគត់> undef_co_bits(undef_co.size(), undef_co_bit);
			ez->assume(ez->vec_eq(undef_co_bits, undef_co));

			undefGating(co, yy, undef_co);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($alu))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> b = importDefSigSpec(cell->getPort(ID::B), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		std::vector<ចំនួនគត់> x = importDefSigSpec(cell->getPort(ID::X), timestep);
		std::vector<ចំនួនគត់> ci = importDefSigSpec(cell->getPort(ID::CI), timestep);
		std::vector<ចំនួនគត់> bi = importDefSigSpec(cell->getPort(ID::BI), timestep);
		std::vector<ចំនួនគត់> co = importDefSigSpec(cell->getPort(ID::CO), timestep);

		extendSignalWidth(a, b, y, cell);
		extendSignalWidth(a, b, x, cell);
		extendSignalWidth(a, b, co, cell);

		std::vector<ចំនួនគត់> def_y = model_undef ? ez->vec_var(y.size()) : y;
		std::vector<ចំនួនគត់> def_x = model_undef ? ez->vec_var(x.size()) : x;
		std::vector<ចំនួនគត់> def_co = model_undef ? ez->vec_var(co.size()) : co;

		log_assert(GetSize(y) == GetSize(x));
		log_assert(GetSize(y) == GetSize(co));
		log_assert(GetSize(ci) == 1);
		log_assert(GetSize(bi) == 1);

		សម្រាប់ (ចំនួនគត់ i = 0; i < GetSize(y); i++)
		{
			ចំនួនគត់ s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->SET(def_x.at(i), ez->XOR(s1, s2));
			ez->SET(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->SET(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_b = importUndefSigSpec(cell->getPort(ID::B), timestep);
			std::vector<ចំនួនគត់> undef_ci = importUndefSigSpec(cell->getPort(ID::CI), timestep);
			std::vector<ចំនួនគត់> undef_bi = importUndefSigSpec(cell->getPort(ID::BI), timestep);

			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			std::vector<ចំនួនគត់> undef_x = importUndefSigSpec(cell->getPort(ID::X), timestep);
			std::vector<ចំនួនគត់> undef_co = importUndefSigSpec(cell->getPort(ID::CO), timestep);

			extendSignalWidth(undef_a, undef_b, undef_y, cell);
			extendSignalWidth(undef_a, undef_b, undef_x, cell);
			extendSignalWidth(undef_a, undef_b, undef_co, cell);

			std::vector<ចំនួនគត់> all_inputs_undef;
			all_inputs_undef.insert(all_inputs_undef.end(), undef_a.begin(), undef_a.end());
			all_inputs_undef.insert(all_inputs_undef.end(), undef_b.begin(), undef_b.end());
			all_inputs_undef.insert(all_inputs_undef.end(), undef_ci.begin(), undef_ci.end());
			all_inputs_undef.insert(all_inputs_undef.end(), undef_bi.begin(), undef_bi.end());
			ចំនួនគត់ undef_any = ez->expression(ezSAT::OpOr, all_inputs_undef);

			សម្រាប់ (ចំនួនគត់ i = 0; i < GetSize(undef_y); i++) {
				ez->SET(undef_y.at(i), undef_any);
				ez->SET(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->SET(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($slice))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec y = cell->getPort(ID::Y);
		ez->assume(signals_eq(a.extract(cell->parameters.at(ID::OFFSET).as_int(), y.size()), y, timestep));
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->getPort(ID::A);
		RTLIL::SigSpec b = cell->getPort(ID::B);
		RTLIL::SigSpec y = cell->getPort(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(signals_eq(ab, y, timestep));
		ត្រឡប់ true;
	}

	ប្រសិនបើ (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		FfData ff(nullptr, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		ប្រសិនបើ (ff.has_aload || ff.has_arst || ff.has_sr)
			ត្រឡប់ false;

		ប្រសិនបើ (timestep == 1)
		{
			initial_state.add((*sigmap)(cell->getPort(ID::Q)));
			ប្រសិនបើ (model_undef && def_formal) {
				std::vector<ចំនួនគត់> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		បើមិនដូច្នេះ
		{
			std::vector<ចំនួនគត់> d = importDefSigSpec(cell->getPort(ID::D), timestep-1);
			std::vector<ចំនួនគត់> undef_d;
			ប្រសិនបើ (model_undef)
				undef_d = importUndefSigSpec(cell->getPort(ID::D), timestep-1);
			ប្រសិនបើ (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				ចំនួនគត់ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<ចំនួនគត់> rval = importDefSigSpec(ff.val_srst, timestep-1);
				ចំនួនគត់ undef_srst = -1;
				std::vector<ចំនួនគត់> undef_rval;
				ប្រសិនបើ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ប្រសិនបើ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				បើមិនដូច្នេះ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			ប្រសិនបើ (ff.has_ce) {
				ចំនួនគត់ ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::vector<ចំនួនគត់> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				ចំនួនគត់ undef_ce = -1;
				std::vector<ចំនួនគត់> undef_old_q;
				ប្រសិនបើ (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				ប្រសិនបើ (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				បើមិនដូច្នេះ
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			ប្រសិនបើ (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				ចំនួនគត់ srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::vector<ចំនួនគត់> rval = importDefSigSpec(ff.val_srst, timestep-1);
				ចំនួនគត់ undef_srst = -1;
				std::vector<ចំនួនគត់> undef_rval;
				ប្រសិនបើ (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				ប្រសិនបើ (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				បើមិនដូច្នេះ
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::vector<ចំនួនគត់> q = importDefSigSpec(cell->getPort(ID::Q), timestep);

			std::vector<ចំនួនគត់> qq = model_undef ? ez->vec_var(q.size()) : q;
			ez->assume(ez->vec_eq(d, qq));

			ប្រសិនបើ (model_undef)
			{
				std::vector<ចំនួនគត់> undef_q = importUndefSigSpec(cell->getPort(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($anyconst))
	{
		ប្រសិនបើ (timestep < 2) {
			ប្រសិនបើ (model_undef && def_formal) {
				std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			ត្រឡប់ true;
		}

		std::vector<ចំនួនគត់> d = importDefSigSpec(cell->getPort(ID::Y), timestep-1);
		std::vector<ចំនួនគត់> q = importDefSigSpec(cell->getPort(ID::Y), timestep);

		std::vector<ចំនួនគត់> qq = (model_undef && !def_formal) ? ez->vec_var(q.size()) : q;
		ez->assume(ez->vec_eq(d, qq));

		ប្រសិនបើ (model_undef)
		{
			std::vector<ចំនួនគត់> undef_d = importUndefSigSpec(cell->getPort(ID::Y), timestep-1);
			std::vector<ចំនួនគត់> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);

			ប្រសិនបើ (def_formal) {
				សម្រាប់ (auto &undef_q_bit : undef_q)
					ez->SET(ez->CONST_FALSE, undef_q_bit);
			} បើមិនដូច្នេះ {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($anyseq))
	{
		ប្រសិនបើ (model_undef && def_formal) {
			std::vector<ចំនួនគត់> undef_q = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			សម្រាប់ (auto &undef_q_bit : undef_q)
				ez->SET(ez->CONST_FALSE, undef_q_bit);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($barrier) && !model_barriers)
		ត្រឡប់ true;

	ប្រសិនបើ (cell->type.in(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::vector<ចំនួនគត់> a = importDefSigSpec(cell->getPort(ID::A), timestep);
		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		extendSignalWidthUnary(a, y, cell);

		std::vector<ចំនួនគត់> yy = model_undef ? ez->vec_var(y.size()) : y;
		ez->assume(ez->vec_eq(a, yy));

		ប្រសិនបើ (model_undef) {
			std::vector<ចំនួនគត់> undef_a = importUndefSigSpec(cell->getPort(ID::A), timestep);
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			extendSignalWidthUnary(undef_a, undef_y, cell, false);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($initstate))
	{
		auto key = make_pair(prefix, timestep);
		ប្រសិនបើ (initstates.count(key) == 0)
			initstates[key] = false;

		std::vector<ចំនួនគត់> y = importDefSigSpec(cell->getPort(ID::Y), timestep);
		log_assert(GetSize(y) == 1);
		ez->SET(y[0], initstates[key] ? ez->CONST_TRUE : ez->CONST_FALSE);

		ប្រសិនបើ (model_undef) {
			std::vector<ចំនួនគត់> undef_y = importUndefSigSpec(cell->getPort(ID::Y), timestep);
			log_assert(GetSize(undef_y) == 1);
			ez->SET(undef_y[0], ez->CONST_FALSE);
		}

		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($assert))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($assume))
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->getPort(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->getPort(ID::EN)));
		ត្រឡប់ true;
	}

	ប្រសិនបើ (cell->type == ID($scopeinfo) || cell->type == ID($input_port))
	{
		ត្រឡប់ true;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	ត្រឡប់ false;
}

namespace Yosys {

void report_missing_model(bool warn_only, RTLIL::Cell* cell)
{
	std::string s;
	ប្រសិនបើ (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	បើមិនដូច្នេះ
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	ប្រសិនបើ (warn_only) {
		log_warning_noprefix("%s\n", s);
	} បើមិនដូច្នេះ {
		log_error("%s\n", s);
	}
}

}
