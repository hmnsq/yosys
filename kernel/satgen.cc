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

#अन्तर्भाव "kernel/satgen.h"
#अन्तर्भाव "kernel/ff.h"
#अन्तर्भाव "kernel/yosys_common.h"

USING_YOSYS_NAMESPACE

तार्किक SatGen::importCell(RTLIL::Cell *cell, पूर्णाङ्क timestep)
{
	तार्किक arith_undef_handled = असत्य;
	तार्किक is_arith_compare = cell->type.अन्तर्(ID($lt), ID($le), ID($ge), ID($gt));

	यदि (model_undef && (cell->type.अन्तर्(ID($सङ्कलय), ID($sub), ID($mul), ID($भाजय), ID($mod), ID($divfloor), ID($modfloor)) || is_arith_compare))
	{
		std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		यदि (is_arith_compare)
			extend_Signal_विस्तारः(undef_a, undef_b, cell, सत्य);
		अन्यथा
			extend_Signal_विस्तारः(undef_a, undef_b, undef_y, cell, सत्य);

		पूर्णाङ्क undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
		पूर्णाङ्क undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
		पूर्णाङ्क undef_y_bit = ez->OR(undef_any_a, undef_any_b);

		यदि (cell->type.अन्तर्(ID($भाजय), ID($mod), ID($divfloor), ID($modfloor))) {
			std::सदिशम्<पूर्णाङ्क> b = importSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			undef_y_bit = ez->OR(undef_y_bit, ez->NOT(ez->expression(ezSAT::OpOr, b)));
		}

		यदि (is_arith_compare) {
			प्रति (आकार_t i = 1; i < undef_y.आकार(); i++)
				ez->समुच्चयः_2(ez->CONST_FALSE, undef_y.at(i));
			ez->समुच्चयः_2(undef_y_bit, undef_y.at(0));
		} अन्यथा {
			std::सदिशम्<पूर्णाङ्क> undef_y_द्वयङ्काः(undef_y.आकार(), undef_y_bit);
			ez->assume(ez->vec_eq(undef_y_द्वयङ्काः, undef_y));
		}

		arith_undef_handled = सत्य;
	}

	यदि (cell->type.अन्तर्(ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($and), ID($or), ID($xor), ID($xnor), ID($सङ्कलय), ID($sub)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		extend_Signal_विस्तारः(a, b, y, cell);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		यदि (cell->type.अन्तर्(ID($and), ID($_AND_)))
			ez->assume(ez->vec_eq(ez->vec_and(a, b), yy));
		यदि (cell->type == ID($_NAND_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_and(a, b)), yy));
		यदि (cell->type.अन्तर्(ID($or), ID($_OR_)))
			ez->assume(ez->vec_eq(ez->vec_or(a, b), yy));
		यदि (cell->type == ID($_NOR_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_or(a, b)), yy));
		यदि (cell->type.अन्तर्(ID($xor), ID($_XOR_)))
			ez->assume(ez->vec_eq(ez->vec_xor(a, b), yy));
		यदि (cell->type.अन्तर्(ID($xnor), ID($_XNOR_)))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_xor(a, b)), yy));
		यदि (cell->type == ID($_ANDNOT_))
			ez->assume(ez->vec_eq(ez->vec_and(a, ez->vec_not(b)), yy));
		यदि (cell->type == ID($_ORNOT_))
			ez->assume(ez->vec_eq(ez->vec_or(a, ez->vec_not(b)), yy));
		यदि (cell->type == ID($सङ्कलय))
			ez->assume(ez->vec_eq(ez->vec_सङ्कलय(a, b), yy));
		यदि (cell->type == ID($sub))
			ez->assume(ez->vec_eq(ez->vec_sub(a, b), yy));

		यदि (model_undef && !arith_undef_handled)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			extend_Signal_विस्तारः(undef_a, undef_b, undef_y, cell, असत्य);

			यदि (cell->type.अन्तर्(ID($and), ID($_AND_), ID($_NAND_))) {
				std::सदिशम्<पूर्णाङ्क> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::सदिशम्<पूर्णाङ्क> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::सदिशम्<पूर्णाङ्क> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा यदि (cell->type.अन्तर्(ID($or), ID($_OR_), ID($_NOR_))) {
				std::सदिशम्<पूर्णाङ्क> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::सदिशम्<पूर्णाङ्क> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::सदिशम्<पूर्णाङ्क> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा यदि (cell->type.अन्तर्(ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_))) {
				std::सदिशम्<पूर्णाङ्क> yX = ez->vec_or(undef_a, undef_b);
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा यदि (cell->type == ID($_ANDNOT_)) {
				std::सदिशम्<पूर्णाङ्क> a0 = ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a));
				std::सदिशम्<पूर्णाङ्क> b1 = ez->vec_and(b, ez->vec_not(undef_b));
				std::सदिशम्<पूर्णाङ्क> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a0, b1)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}

			अन्यथा यदि (cell->type == ID($_ORNOT_)) {
				std::सदिशम्<पूर्णाङ्क> a1 = ez->vec_and(a, ez->vec_not(undef_a));
				std::सदिशम्<पूर्णाङ्क> b0 = ez->vec_and(ez->vec_not(b), ez->vec_not(undef_b));
				std::सदिशम्<पूर्णाङ्क> yX = ez->vec_and(ez->vec_or(undef_a, undef_b), ez->vec_not(ez->vec_or(a1, b0)));
				ez->assume(ez->vec_eq(yX, undef_y));
			}
			अन्यथा
				log_abort();

			undefGating(y, yy, undef_y);
		}
		अन्यथा यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
	{
		तार्किक aoi_बहुलकम् = cell->type.अन्तर्(ID($_AOI3_), ID($_AOI4_));
		तार्किक three_बहुलकम् = cell->type.अन्तर्(ID($_AOI3_), ID($_OAI3_));

		पूर्णाङ्क a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep).at(0);
		पूर्णाङ्क b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep).at(0);
		पूर्णाङ्क c = importDefSigSpec(cell->get_पत्तनम्(ID::C), timestep).at(0);
		पूर्णाङ्क d = three_बहुलकम् ? (aoi_बहुलकम् ? ez->CONST_TRUE : ez->CONST_FALSE) : importDefSigSpec(cell->get_पत्तनम्(ID::D), timestep).at(0);
		पूर्णाङ्क y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep).at(0);
		पूर्णाङ्क yy = model_undef ? ez->literal() : y;

		यदि (cell->type.अन्तर्(ID($_AOI3_), ID($_AOI4_)))
			ez->assume(ez->IFF(ez->NOT(ez->OR(ez->AND(a, b), ez->AND(c, d))), yy));
		अन्यथा
			ez->assume(ez->IFF(ez->NOT(ez->AND(ez->OR(a, b), ez->OR(c, d))), yy));

		यदि (model_undef)
		{
			पूर्णाङ्क undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep).at(0);
			पूर्णाङ्क undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep).at(0);
			पूर्णाङ्क undef_c = importUndefSigSpec(cell->get_पत्तनम्(ID::C), timestep).at(0);
			पूर्णाङ्क undef_d = three_बहुलकम् ? ez->CONST_FALSE : importUndefSigSpec(cell->get_पत्तनम्(ID::D), timestep).at(0);
			पूर्णाङ्क undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep).at(0);

			यदि (aoi_बहुलकम्)
			{
				पूर्णाङ्क a0 = ez->AND(ez->NOT(a), ez->NOT(undef_a));
				पूर्णाङ्क b0 = ez->AND(ez->NOT(b), ez->NOT(undef_b));
				पूर्णाङ्क c0 = ez->AND(ez->NOT(c), ez->NOT(undef_c));
				पूर्णाङ्क d0 = ez->AND(ez->NOT(d), ez->NOT(undef_d));

				पूर्णाङ्क ab = ez->AND(a, b), cd = ez->AND(c, d);
				पूर्णाङ्क undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a0, b0)));
				पूर्णाङ्क undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c0, d0)));

				पूर्णाङ्क ab1 = ez->AND(ab, ez->NOT(undef_ab));
				पूर्णाङ्क cd1 = ez->AND(cd, ez->NOT(undef_cd));
				पूर्णाङ्क yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab1, cd1)));

				ez->assume(ez->IFF(yX, undef_y));
			}
			अन्यथा
			{
				पूर्णाङ्क a1 = ez->AND(a, ez->NOT(undef_a));
				पूर्णाङ्क b1 = ez->AND(b, ez->NOT(undef_b));
				पूर्णाङ्क c1 = ez->AND(c, ez->NOT(undef_c));
				पूर्णाङ्क d1 = ez->AND(d, ez->NOT(undef_d));

				पूर्णाङ्क ab = ez->OR(a, b), cd = ez->OR(c, d);
				पूर्णाङ्क undef_ab = ez->AND(ez->OR(undef_a, undef_b), ez->NOT(ez->OR(a1, b1)));
				पूर्णाङ्क undef_cd = ez->AND(ez->OR(undef_c, undef_d), ez->NOT(ez->OR(c1, d1)));

				पूर्णाङ्क ab0 = ez->AND(ez->NOT(ab), ez->NOT(undef_ab));
				पूर्णाङ्क cd0 = ez->AND(ez->NOT(cd), ez->NOT(undef_cd));
				पूर्णाङ्क yX = ez->AND(ez->OR(undef_ab, undef_cd), ez->NOT(ez->OR(ab0, cd0)));

				ez->assume(ez->IFF(yX, undef_y));
			}

			undefGating(y, yy, undef_y);
		}

		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($_NOT_), ID($not)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		extend_Signal_विस्तारः_Unary(a, y, cell);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		ez->assume(ez->vec_eq(ez->vec_not(a), yy));

		यदि (model_undef) {
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			extend_Signal_विस्तारः_Unary(undef_a, undef_y, cell, असत्य);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($bweqx))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		std::सदिशम्<पूर्णाङ्क> bweqx = ez->vec_not(ez->vec_xor(a, b));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

			std::सदिशम्<पूर्णाङ्क> both_undef = ez->vec_and(undef_a, undef_b);
			std::सदिशम्<पूर्णाङ्क> both_def = ez->vec_and(ez->vec_not(undef_a), ez->vec_not(undef_b));

			bweqx = ez->vec_or(both_undef, ez->vec_and(both_def, bweqx));

			प्रति (पूर्णाङ्क yx : undef_y)
				ez->assume(ez->NOT(yx));
		}
		ez->assume(ez->vec_eq(bweqx, y));
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($_MUX_), ID($mux), ID($_NMUX_), ID($bwmux)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> s = importDefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		यदि (cell->type == ID($_NMUX_))
			ez->assume(ez->vec_eq(ez->vec_not(ez->vec_ite(s.at(0), b, a)), yy));
		अन्यथा यदि (cell->type == ID($bwmux))
			ez->assume(ez->vec_eq(ez->vec_ite(s, b, a), yy));
		अन्यथा
			ez->assume(ez->vec_eq(ez->vec_ite(s.at(0), b, a), yy));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_s = importUndefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

			std::सदिशम्<पूर्णाङ्क> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::सदिशम्<पूर्णाङ्क> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			std::सदिशम्<पूर्णाङ्क> yX;
			यदि (cell->type == ID($bwmux))
				yX = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
			अन्यथा
				yX = ez->vec_ite(undef_s.at(0), undef_ab, ez->vec_ite(s.at(0), undef_b, undef_a));
			ez->assume(ez->vec_eq(yX, undef_y));
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($bmux))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> s = importDefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		std::सदिशम्<पूर्णाङ्क> undef_a, undef_s, undef_y;

		यदि (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		}

		यदि (Get_आकार(s) == 0) {
			ez->vec_समुच्चयः(a, y);
			यदि (model_undef)
				ez->vec_समुच्चयः(undef_a, undef_y);
		} अन्यथा {
			प्रति (पूर्णाङ्क i = Get_आकार(s)-1; i >= 0; i--)
			{
				std::सदिशम्<पूर्णाङ्क> बहिर्गमनम् = (i == 0) ? y : ez->vec_var(a.आकार() / 2);
				std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(बहिर्गमनम्.आकार()) : बहिर्गमनम्;

				std::सदिशम्<पूर्णाङ्क> a0(a.begin(), a.begin() + a.आकार() / 2);
				std::सदिशम्<पूर्णाङ्क> a1(a.begin() + a.आकार() / 2, a.अन्तः());
				ez->assume(ez->vec_eq(ez->vec_ite(s.at(i), a1, a0), yy));

				यदि (model_undef)
				{
					std::सदिशम्<पूर्णाङ्क> undef_बहिर्गमनम् = (i == 0) ? undef_y : ez->vec_var(a.आकार() / 2);
					std::सदिशम्<पूर्णाङ्क> undef_a0(undef_a.begin(), undef_a.begin() + a.आकार() / 2);
					std::सदिशम्<पूर्णाङ्क> undef_a1(undef_a.begin() + a.आकार() / 2, undef_a.अन्तः());
					std::सदिशम्<पूर्णाङ्क> unequal_ab = ez->vec_not(ez->vec_iff(a0, a1));
					std::सदिशम्<पूर्णाङ्क> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a0, undef_a1));
					std::सदिशम्<पूर्णाङ्क> yX = ez->vec_ite(undef_s.at(i), undef_ab, ez->vec_ite(s.at(i), undef_a1, undef_a0));
					ez->assume(ez->vec_eq(yX, undef_बहिर्गमनम्));
					undefGating(बहिर्गमनम्, yy, undef_बहिर्गमनम्);

					undef_a = undef_बहिर्गमनम्;
				}

				a = बहिर्गमनम्;
			}
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($demux))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> s = importDefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		std::सदिशम्<पूर्णाङ्क> undef_a, undef_s, undef_y;

		यदि (model_undef)
		{
			undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			undef_s = importUndefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
			undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		}

		यदि (Get_आकार(s) == 0) {
			ez->vec_समुच्चयः(a, y);
			यदि (model_undef)
				ez->vec_समुच्चयः(undef_a, undef_y);
		} अन्यथा {
			प्रति (पूर्णाङ्क i = 0; i < (1 << Get_आकार(s)); i++)
			{
				std::सदिशम्<पूर्णाङ्क> ss;
				प्रति (पूर्णाङ्क j = 0; j < Get_आकार(s); j++) {
					यदि (i & 1 << j)
						ss.प्रक्षिप_पृष्ठभागः(s[j]);
					अन्यथा
						ss.प्रक्षिप_पृष्ठभागः(ez->NOT(s[j]));
				}
				पूर्णाङ्क sss = ez->expression(ezSAT::OpAnd, ss);

				प्रति (पूर्णाङ्क j = 0; j < Get_आकार(a); j++) {
					ez->समुच्चयः_2(ez->AND(sss, a[j]), yy.at(i * Get_आकार(a) + j));
				}

				यदि (model_undef)
				{
					पूर्णाङ्क s0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(ss), ez->vec_not(undef_s)));
					पूर्णाङ्क us = ez->AND(ez->NOT(s0), ez->expression(ezSAT::OpOr, undef_s));
					प्रति (पूर्णाङ्क j = 0; j < Get_आकार(a); j++) {
						पूर्णाङ्क a0 = ez->AND(ez->NOT(a[j]), ez->NOT(undef_a[j]));
						पूर्णाङ्क yX = ez->AND(ez->OR(us, undef_a[j]), ez->NOT(ez->OR(s0, a0)));
						ez->समुच्चयः_2(yX, undef_y.at(i * Get_आकार(a) + j));
					}
				}
			}
			यदि (model_undef)
				undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($pmux))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> s = importDefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		std::सदिशम्<पूर्णाङ्क> tmp = a;
		प्रति (आकार_t i = 0; i < s.आकार(); i++) {
			std::सदिशम्<पूर्णाङ्क> part_सम्बन्धि_b(b.begin()+i*a.आकार(), b.begin()+(i+1)*a.आकार());
			tmp = ez->vec_ite(s.at(i), part_सम्बन्धि_b, tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_s = importUndefSigSpec(cell->get_पत्तनम्(ID::S), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

			पूर्णाङ्क all_undef = ez->CONST_FALSE;
			पूर्णाङ्क प्राप्तम्_सक्रिय = ez->CONST_FALSE;

			std::सदिशम्<पूर्णाङ्क> undef_tmp = undef_a;

			प्रति (आकार_t i = 0; i < s.आकार(); i++)
			{
				std::सदिशम्<पूर्णाङ्क> part_सम्बन्धि_undef_b(undef_b.begin()+i*a.आकार(), undef_b.begin()+(i+1)*a.आकार());

				undef_tmp = ez->vec_ite(s.at(i), part_सम्बन्धि_undef_b, undef_tmp);
				all_undef = ez->OR(all_undef, undef_s.at(i));
				all_undef = ez->OR(all_undef, ez->AND(s.at(i), प्राप्तम्_सक्रिय));
				प्राप्तम्_सक्रिय = ez->OR(प्राप्तम्_सक्रिय, s.at(i));
			}

			undef_tmp = ez->vec_or(undef_tmp, std::सदिशम्<पूर्णाङ्क>(a.आकार(), all_undef));

			ez->assume(ez->vec_eq(undef_tmp, undef_y));
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($pos), ID($buf), ID($neg)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		extend_Signal_विस्तारः_Unary(a, y, cell);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		यदि (cell->type.अन्तर्(ID($pos), ID($buf))) {
			ez->assume(ez->vec_eq(a, yy));
		} अन्यथा {
			std::सदिशम्<पूर्णाङ्क> zero(a.आकार(), ez->CONST_FALSE);
			ez->assume(ez->vec_eq(ez->vec_sub(zero, a), yy));
		}

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			extend_Signal_विस्तारः_Unary(undef_a, undef_y, cell);

			यदि (cell->type.अन्तर्(ID($pos), ID($buf))) {
				ez->assume(ez->vec_eq(undef_a, undef_y));
			} अन्यथा {
				पूर्णाङ्क undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
				std::सदिशम्<पूर्णाङ्क> undef_y_द्वयङ्काः(undef_y.आकार(), undef_any_a);
				ez->assume(ez->vec_eq(undef_y_द्वयङ्काः, undef_y));
			}

			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		यदि (cell->type == ID($reduce_and))
			ez->समुच्चयः_2(ez->expression(ez->OpAnd, a), yy.at(0));
		यदि (cell->type.अन्तर्(ID($reduce_or), ID($reduce_bool)))
			ez->समुच्चयः_2(ez->expression(ez->OpOr, a), yy.at(0));
		यदि (cell->type == ID($reduce_xor))
			ez->समुच्चयः_2(ez->expression(ez->OpXor, a), yy.at(0));
		यदि (cell->type == ID($reduce_xnor))
			ez->समुच्चयः_2(ez->NOT(ez->expression(ez->OpXor, a)), yy.at(0));
		यदि (cell->type == ID($logic_not))
			ez->समुच्चयः_2(ez->NOT(ez->expression(ez->OpOr, a)), yy.at(0));
		प्रति (आकार_t i = 1; i < y.आकार(); i++)
			ez->समुच्चयः_2(ez->CONST_FALSE, yy.at(i));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			पूर्णाङ्क aX = ez->expression(ezSAT::OpOr, undef_a);

			यदि (cell->type == ID($reduce_and)) {
				पूर्णाङ्क a0 = ez->expression(ezSAT::OpOr, ez->vec_and(ez->vec_not(a), ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a0), aX), undef_y.at(0)));
			}
			अन्यथा यदि (cell->type.अन्तर्(ID($reduce_or), ID($reduce_bool), ID($logic_not))) {
				पूर्णाङ्क a1 = ez->expression(ezSAT::OpOr, ez->vec_and(a, ez->vec_not(undef_a)));
				ez->assume(ez->IFF(ez->AND(ez->NOT(a1), aX), undef_y.at(0)));
			}
			अन्यथा यदि (cell->type.अन्तर्(ID($reduce_xor), ID($reduce_xnor))) {
				ez->assume(ez->IFF(aX, undef_y.at(0)));
			} अन्यथा
				log_abort();

			प्रति (आकार_t i = 1; i < undef_y.आकार(); i++)
				ez->समुच्चयः_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($logic_and), ID($logic_or)))
	{
		std::सदिशम्<पूर्णाङ्क> vec_a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> vec_b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);

		पूर्णाङ्क a = ez->expression(ez->OpOr, vec_a);
		पूर्णाङ्क b = ez->expression(ez->OpOr, vec_b);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		यदि (cell->type == ID($logic_and))
			ez->समुच्चयः_2(ez->expression(ez->OpAnd, a, b), yy.at(0));
		अन्यथा
			ez->समुच्चयः_2(ez->expression(ez->OpOr, a, b), yy.at(0));
		प्रति (आकार_t i = 1; i < y.आकार(); i++)
			ez->समुच्चयः_2(ez->CONST_FALSE, yy.at(i));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

			पूर्णाङ्क a0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_a), ez->expression(ezSAT::OpOr, undef_a)));
			पूर्णाङ्क b0 = ez->NOT(ez->OR(ez->expression(ezSAT::OpOr, vec_b), ez->expression(ezSAT::OpOr, undef_b)));
			पूर्णाङ्क a1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_a, ez->vec_not(undef_a)));
			पूर्णाङ्क b1 = ez->expression(ezSAT::OpOr, ez->vec_and(vec_b, ez->vec_not(undef_b)));
			पूर्णाङ्क aX = ez->expression(ezSAT::OpOr, undef_a);
			पूर्णाङ्क bX = ez->expression(ezSAT::OpOr, undef_b);

			यदि (cell->type == ID($logic_and))
				ez->समुच्चयः_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a1, b1)), ez->NOT(a0), ez->NOT(b0)), undef_y.at(0));
			अन्यथा यदि (cell->type == ID($logic_or))
				ez->समुच्चयः_2(ez->AND(ez->OR(aX, bX), ez->NOT(ez->AND(a0, b0)), ez->NOT(a1), ez->NOT(b1)), undef_y.at(0));
			अन्यथा
				log_abort();

			प्रति (आकार_t i = 1; i < undef_y.आकार(); i++)
				ez->समुच्चयः_2(ez->CONST_FALSE, undef_y.at(i));

			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt)))
	{
		तार्किक is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		extend_Signal_विस्तारः(a, b, cell);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		यदि (model_undef && cell->type.अन्तर्(ID($eqx), ID($nex))) {
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			extend_Signal_विस्तारः(undef_a, undef_b, cell, सत्य);
			a = ez->vec_or(a, undef_a);
			b = ez->vec_or(b, undef_b);
		}

		यदि (cell->type == ID($lt))
			ez->समुच्चयः_2(is_signed ? ez->vec_lt_signed(a, b) : ez->vec_lt_unsigned(a, b), yy.at(0));
		यदि (cell->type == ID($le))
			ez->समुच्चयः_2(is_signed ? ez->vec_le_signed(a, b) : ez->vec_le_unsigned(a, b), yy.at(0));
		यदि (cell->type.अन्तर्(ID($eq), ID($eqx)))
			ez->समुच्चयः_2(ez->vec_eq(a, b), yy.at(0));
		यदि (cell->type.अन्तर्(ID($ne), ID($nex)))
			ez->समुच्चयः_2(ez->vec_ne(a, b), yy.at(0));
		यदि (cell->type == ID($ge))
			ez->समुच्चयः_2(is_signed ? ez->vec_ge_signed(a, b) : ez->vec_ge_unsigned(a, b), yy.at(0));
		यदि (cell->type == ID($gt))
			ez->समुच्चयः_2(is_signed ? ez->vec_gt_signed(a, b) : ez->vec_gt_unsigned(a, b), yy.at(0));
		प्रति (आकार_t i = 1; i < y.आकार(); i++)
			ez->समुच्चयः_2(ez->CONST_FALSE, yy.at(i));

		यदि (model_undef && cell->type.अन्तर्(ID($eqx), ID($nex)))
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			extend_Signal_विस्तारः(undef_a, undef_b, cell, सत्य);

			यदि (cell->type == ID($eqx))
				yy.at(0) = ez->AND(yy.at(0), ez->vec_eq(undef_a, undef_b));
			अन्यथा
				yy.at(0) = ez->OR(yy.at(0), ez->vec_ne(undef_a, undef_b));

			प्रति (आकार_t i = 0; i < y.आकार(); i++)
				ez->समुच्चयः_2(ez->CONST_FALSE, undef_y.at(i));

			ez->assume(ez->vec_eq(y, yy));
		}
		अन्यथा यदि (model_undef && cell->type.अन्तर्(ID($eq), ID($ne)))
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			extend_Signal_विस्तारः(undef_a, undef_b, cell, सत्य);

			पूर्णाङ्क undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			पूर्णाङ्क undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			पूर्णाङ्क undef_any = ez->OR(undef_any_a, undef_any_b);

			std::सदिशम्<पूर्णाङ्क> masked_a_द्वयङ्काः = ez->vec_or(a, ez->vec_or(undef_a, undef_b));
			std::सदिशम्<पूर्णाङ्क> masked_b_द्वयङ्काः = ez->vec_or(b, ez->vec_or(undef_a, undef_b));

			पूर्णाङ्क masked_ne = ez->vec_ne(masked_a_द्वयङ्काः, masked_b_द्वयङ्काः);
			पूर्णाङ्क undef_y_bit = ez->AND(undef_any, ez->NOT(masked_ne));

			प्रति (आकार_t i = 1; i < undef_y.आकार(); i++)
				ez->समुच्चयः_2(ez->CONST_FALSE, undef_y.at(i));
			ez->समुच्चयः_2(undef_y_bit, undef_y.at(0));

			undefGating(y, yy, undef_y);
		}
		अन्यथा
		{
			यदि (model_undef) {
				std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
				undefGating(y, yy, undef_y);
			}
			log_assert(!model_undef || arith_undef_handled);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($सरणम्), ID($shiftx)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		पूर्णाङ्क extend_bit = ez->CONST_FALSE;

		यदि (cell->parameters[ID::A_SIGNED].as_bool())
			extend_bit = a.पृष्ठभागः();

		यावत् (y.आकार() < a.आकार())
			y.प्रक्षिप_पृष्ठभागः(ez->literal());
		यावत् (y.आकार() > a.आकार())
			a.प्रक्षिप_पृष्ठभागः(extend_bit);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		std::सदिशम्<पूर्णाङ्क> shifted_a;

		यदि (cell->type.अन्तर्( ID($shl), ID($sshl)))
			shifted_a = ez->vec_सरणम्_वामः(a, b, असत्य, ez->CONST_FALSE, ez->CONST_FALSE);

		यदि (cell->type == ID($shr))
			shifted_a = ez->vec_सरणम्_दक्षिणः(a, b, असत्य, ez->CONST_FALSE, ez->CONST_FALSE);

		यदि (cell->type == ID($sshr))
			shifted_a = ez->vec_सरणम्_दक्षिणः(a, b, असत्य, cell->parameters[ID::A_SIGNED].as_bool() ? a.पृष्ठभागः() : ez->CONST_FALSE, ez->CONST_FALSE);

		यदि (cell->type.अन्तर्(ID($सरणम्), ID($shiftx)))
			shifted_a = ez->vec_सरणम्_दक्षिणः(a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

		ez->assume(ez->vec_eq(shifted_a, yy));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_a_shifted;

			extend_bit = cell->type == ID($shiftx) ? ez->CONST_TRUE : ez->CONST_FALSE;
			यदि (cell->parameters[ID::A_SIGNED].as_bool())
				extend_bit = undef_a.पृष्ठभागः();

			यावत् (undef_y.आकार() < undef_a.आकार())
				undef_y.प्रक्षिप_पृष्ठभागः(ez->literal());
			यावत् (undef_y.आकार() > undef_a.आकार())
				undef_a.प्रक्षिप_पृष्ठभागः(extend_bit);

			यदि (cell->type.अन्तर्(ID($shl), ID($sshl)))
				undef_a_shifted = ez->vec_सरणम्_वामः(undef_a, b, असत्य, ez->CONST_FALSE, ez->CONST_FALSE);

			यदि (cell->type == ID($shr))
				undef_a_shifted = ez->vec_सरणम्_दक्षिणः(undef_a, b, असत्य, ez->CONST_FALSE, ez->CONST_FALSE);

			यदि (cell->type == ID($sshr))
				undef_a_shifted = ez->vec_सरणम्_दक्षिणः(undef_a, b, असत्य, cell->parameters[ID::A_SIGNED].as_bool() ? undef_a.पृष्ठभागः() : ez->CONST_FALSE, ez->CONST_FALSE);

			यदि (cell->type == ID($सरणम्))
				undef_a_shifted = ez->vec_सरणम्_दक्षिणः(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_FALSE, ez->CONST_FALSE);

			यदि (cell->type == ID($shiftx))
				undef_a_shifted = ez->vec_सरणम्_दक्षिणः(undef_a, b, cell->parameters[ID::B_SIGNED].as_bool(), ez->CONST_TRUE, ez->CONST_TRUE);

			पूर्णाङ्क undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			std::सदिशम्<पूर्णाङ्क> undef_all_y_द्वयङ्काः(undef_y.आकार(), undef_any_b);
			ez->assume(ez->vec_eq(ez->vec_or(undef_a_shifted, undef_all_y_द्वयङ्काः), undef_y));
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($mul))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		extend_Signal_विस्तारः(a, b, y, cell);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		std::सदिशम्<पूर्णाङ्क> tmp(a.आकार(), ez->CONST_FALSE);
		प्रति (पूर्णाङ्क i = 0; i < पूर्णाङ्क(a.आकार()); i++)
		{
			std::सदिशम्<पूर्णाङ्क> shifted_a(a.आकार(), ez->CONST_FALSE);
			प्रति (पूर्णाङ्क j = i; j < पूर्णाङ्क(a.आकार()); j++)
				shifted_a.at(j) = a.at(j-i);
			tmp = ez->vec_ite(b.at(i), ez->vec_सङ्कलय(tmp, shifted_a), tmp);
		}
		ez->assume(ez->vec_eq(tmp, yy));

		यदि (model_undef) {
			log_assert(arith_undef_handled);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($macc), ID($macc_v2)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		Macc macc;
		macc.from_cell(cell);

		std::सदिशम्<पूर्णाङ्क> tmp(Get_आकार(y), ez->CONST_FALSE);

		प्रति (स्वचालित &पत्तनम् : macc.terms)
		{
			std::सदिशम्<पूर्णाङ्क> अन्तर्_a = importDefSigSpec(पत्तनम्.अन्तर्_a, timestep);
			std::सदिशम्<पूर्णाङ्क> अन्तर्_b = importDefSigSpec(पत्तनम्.अन्तर्_b, timestep);

			यावत् (Get_आकार(अन्तर्_a) < Get_आकार(y))
				अन्तर्_a.प्रक्षिप_पृष्ठभागः(पत्तनम्.is_signed && !अन्तर्_a.रिक्तानि() ? अन्तर्_a.पृष्ठभागः() : ez->CONST_FALSE);
			अन्तर्_a.resize(Get_आकार(y));

			यदि (Get_आकार(अन्तर्_b))
			{
				यावत् (Get_आकार(अन्तर्_b) < Get_आकार(y))
					अन्तर्_b.प्रक्षिप_पृष्ठभागः(पत्तनम्.is_signed && !अन्तर्_b.रिक्तानि() ? अन्तर्_b.पृष्ठभागः() : ez->CONST_FALSE);
				अन्तर्_b.resize(Get_आकार(y));

				प्रति (पूर्णाङ्क i = 0; i < Get_आकार(अन्तर्_b); i++) {
					std::सदिशम्<पूर्णाङ्क> shifted_a(अन्तर्_a.आकार(), ez->CONST_FALSE);
					प्रति (पूर्णाङ्क j = i; j < पूर्णाङ्क(अन्तर्_a.आकार()); j++)
						shifted_a.at(j) = अन्तर्_a.at(j-i);
					यदि (पत्तनम्.do_व्यवकलय)
						tmp = ez->vec_ite(अन्तर्_b.at(i), ez->vec_sub(tmp, shifted_a), tmp);
					अन्यथा
						tmp = ez->vec_ite(अन्तर्_b.at(i), ez->vec_सङ्कलय(tmp, shifted_a), tmp);
				}
			}
			अन्यथा
			{
				यदि (पत्तनम्.do_व्यवकलय)
					tmp = ez->vec_sub(tmp, अन्तर्_a);
				अन्यथा
					tmp = ez->vec_सङ्कलय(tmp, अन्तर्_a);
			}
		}

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_c;

			यदि (cell->type == ID($macc_v2))
				undef_c = importUndefSigSpec(cell->get_पत्तनम्(ID::C), timestep);

			पूर्णाङ्क undef_any_a = ez->expression(ezSAT::OpOr, undef_a);
			पूर्णाङ्क undef_any_b = ez->expression(ezSAT::OpOr, undef_b);
			पूर्णाङ्क undef_any_c = ez->expression(ezSAT::OpOr, undef_c);
			पूर्णाङ्क undef_any = ez->OR(undef_any_a, ez->OR(undef_any_b, undef_any_c));

			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			ez->assume(ez->vec_eq(undef_y, std::सदिशम्<पूर्णाङ्क>(Get_आकार(y), undef_any)));

			undefGating(y, tmp, undef_y);
		}
		अन्यथा
			ez->assume(ez->vec_eq(y, tmp));

		प्रत्यागम सत्य;
	}

	यदि (cell->type.अन्तर्(ID($भाजय), ID($mod), ID($divfloor), ID($modfloor)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		extend_Signal_विस्तारः(a, b, y, cell);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;

		std::सदिशम्<पूर्णाङ्क> a_u, b_u;
		यदि (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			a_u = ez->vec_ite(a.पृष्ठभागः(), ez->vec_neg(a), a);
			b_u = ez->vec_ite(b.पृष्ठभागः(), ez->vec_neg(b), b);
		} अन्यथा {
			a_u = a;
			b_u = b;
		}

		std::सदिशम्<पूर्णाङ्क> chain_buf = a_u;
		std::सदिशम्<पूर्णाङ्क> y_u(a_u.आकार(), ez->CONST_FALSE);
		प्रति (पूर्णाङ्क i = पूर्णाङ्क(a.आकार())-1; i >= 0; i--)
		{
			chain_buf.निवेशय(chain_buf.अन्तः(), chain_buf.आकार(), ez->CONST_FALSE);

			std::सदिशम्<पूर्णाङ्क> b_shl(i, ez->CONST_FALSE);
			b_shl.निवेशय(b_shl.अन्तः(), b_u.begin(), b_u.अन्तः());
			b_shl.निवेशय(b_shl.अन्तः(), chain_buf.आकार()-b_shl.आकार(), ez->CONST_FALSE);

			y_u.at(i) = ez->vec_ge_unsigned(chain_buf, b_shl);
			chain_buf = ez->vec_ite(y_u.at(i), ez->vec_sub(chain_buf, b_shl), chain_buf);

			chain_buf.erase(chain_buf.begin() + a_u.आकार(), chain_buf.अन्तः());
		}

		std::सदिशम्<पूर्णाङ्क> y_tmp = ignore_div_by_zero ? yy : ez->vec_var(y.आकार());

		// modulo calculation
		std::सदिशम्<पूर्णाङ्क> modulo_trunc;
		पूर्णाङ्क floored_eq_trunc;
		यदि (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
			modulo_trunc = ez->vec_ite(a.पृष्ठभागः(), ez->vec_neg(chain_buf), chain_buf);
			// floor == trunc when sgn(a) == sgn(b) or trunc == 0
			floored_eq_trunc = ez->OR(ez->IFF(a.पृष्ठभागः(), b.पृष्ठभागः()), ez->NOT(ez->expression(ezSAT::OpOr, modulo_trunc)));
		} अन्यथा {
			modulo_trunc = chain_buf;
			floored_eq_trunc = ez->CONST_TRUE;
		}

		यदि (cell->type == ID($भाजय)) {
			यदि (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(ez->XOR(a.पृष्ठभागः(), b.पृष्ठभागः()), ez->vec_neg(y_u), y_u)));
			अन्यथा
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} अन्यथा यदि (cell->type == ID($mod)) {
			ez->assume(ez->vec_eq(y_tmp, modulo_trunc));
		} अन्यथा यदि (cell->type == ID($divfloor)) {
			यदि (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
				ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(
					ez->XOR(a.पृष्ठभागः(), b.पृष्ठभागः()),
					ez->vec_neg(ez->vec_ite(
						ez->vec_reduce_or(modulo_trunc),
						ez->vec_सङ्कलय(y_u, ez->vec_const_unsigned(1, y_u.आकार())),
						y_u
					)),
					y_u
				)));
			अन्यथा
				ez->assume(ez->vec_eq(y_tmp, y_u));
		} अन्यथा यदि (cell->type == ID($modfloor)) {
			ez->assume(ez->vec_eq(y_tmp, ez->vec_ite(floored_eq_trunc, modulo_trunc, ez->vec_सङ्कलय(modulo_trunc, b))));
		}

		यदि (ignore_div_by_zero) {
			ez->assume(ez->expression(ezSAT::OpOr, b));
		} अन्यथा {
			std::सदिशम्<पूर्णाङ्क> div_zero_परिणाम;
			यदि (cell->type.अन्तर्(ID($भाजय), ID($divfloor))) {
				यदि (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool()) {
					std::सदिशम्<पूर्णाङ्क> all_ones(y.आकार(), ez->CONST_TRUE);
					std::सदिशम्<पूर्णाङ्क> only_first_one(y.आकार(), ez->CONST_FALSE);
					only_first_one.at(0) = ez->CONST_TRUE;
					div_zero_परिणाम = ez->vec_ite(a.पृष्ठभागः(), only_first_one, all_ones);
				} अन्यथा {
					div_zero_परिणाम.निवेशय(div_zero_परिणाम.अन्तः(), cell->get_पत्तनम्(ID::A).आकार(), ez->CONST_TRUE);
					div_zero_परिणाम.निवेशय(div_zero_परिणाम.अन्तः(), y.आकार() - div_zero_परिणाम.आकार(), ez->CONST_FALSE);
				}
			} अन्यथा यदि (cell->type.अन्तर्(ID($mod), ID($modfloor))) {
				// a mod 0 = a
				पूर्णाङ्क copy_a_द्वयङ्काः = min(cell->get_पत्तनम्(ID::A).आकार(), cell->get_पत्तनम्(ID::B).आकार());
				div_zero_परिणाम.निवेशय(div_zero_परिणाम.अन्तः(), a.begin(), a.begin() + copy_a_द्वयङ्काः);
				यदि (cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool())
					div_zero_परिणाम.निवेशय(div_zero_परिणाम.अन्तः(), y.आकार() - div_zero_परिणाम.आकार(), div_zero_परिणाम.पृष्ठभागः());
				अन्यथा
					div_zero_परिणाम.निवेशय(div_zero_परिणाम.अन्तः(), y.आकार() - div_zero_परिणाम.आकार(), ez->CONST_FALSE);
			}
			ez->assume(ez->vec_eq(yy, ez->vec_ite(ez->expression(ezSAT::OpOr, b), y_tmp, div_zero_परिणाम)));
		}

		यदि (model_undef) {
			log_assert(arith_undef_handled);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($lut))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		std::सदिशम्<पूर्णाङ्क> lut;
		प्रति (स्वचालित bit : cell->getParam(ID::LUT))
			lut.प्रक्षिप_पृष्ठभागः(bit == State::S1 ? ez->CONST_TRUE : ez->CONST_FALSE);
		यावत् (Get_आकार(lut) < (1 << Get_आकार(a)))
			lut.प्रक्षिप_पृष्ठभागः(ez->CONST_FALSE);
		lut.resize(1 << Get_आकार(a));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> t(lut), u(Get_आकार(t), ez->CONST_FALSE);

			प्रति (पूर्णाङ्क i = Get_आकार(a)-1; i >= 0; i--)
			{
				std::सदिशम्<पूर्णाङ्क> t0(t.begin(), t.begin() + Get_आकार(t)/2);
				std::सदिशम्<पूर्णाङ्क> t1(t.begin() + Get_आकार(t)/2, t.अन्तः());

				std::सदिशम्<पूर्णाङ्क> u0(u.begin(), u.begin() + Get_आकार(u)/2);
				std::सदिशम्<पूर्णाङ्क> u1(u.begin() + Get_आकार(u)/2, u.अन्तः());

				t = ez->vec_ite(a[i], t1, t0);
				u = ez->vec_ite(undef_a[i], ez->vec_or(ez->vec_xor(t0, t1), ez->vec_or(u0, u1)), ez->vec_ite(a[i], u1, u0));
			}

			log_assert(Get_आकार(t) == 1);
			log_assert(Get_आकार(u) == 1);
			undefGating(y, t, u);
			ez->assume(ez->vec_eq(importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep), u));
		}
		अन्यथा
		{
			std::सदिशम्<पूर्णाङ्क> t = lut;
			प्रति (पूर्णाङ्क i = Get_आकार(a)-1; i >= 0; i--)
			{
				std::सदिशम्<पूर्णाङ्क> t0(t.begin(), t.begin() + Get_आकार(t)/2);
				std::सदिशम्<पूर्णाङ्क> t1(t.begin() + Get_आकार(t)/2, t.अन्तः());
				t = ez->vec_ite(a[i], t1, t0);
			}

			log_assert(Get_आकार(t) == 1);
			ez->assume(ez->vec_eq(y, t));
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($sop))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		पूर्णाङ्क y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep).at(0);

		पूर्णाङ्क विस्तारः = cell->getParam(ID::विस्तारः_2).as_int();
		पूर्णाङ्क गभीरता = cell->getParam(ID::गभीरता_2).as_int();

		सदिशम्<State> सारणी_raw = cell->getParam(ID::सारणी).गन्तव्यम्_द्वयङ्काः();
		यावत् (Get_आकार(सारणी_raw) < 2*विस्तारः*गभीरता)
			सारणी_raw.प्रक्षिप_पृष्ठभागः(State::S0);

		सदिशम्<सदिशम्<पूर्णाङ्क>> सारणी_2(गभीरता);

		प्रति (पूर्णाङ्क i = 0; i < गभीरता; i++)
		प्रति (पूर्णाङ्क j = 0; j < विस्तारः; j++)
		{
			तार्किक pat0 = (सारणी_raw[2*विस्तारः*i + 2*j + 0] == State::S1);
			तार्किक pat1 = (सारणी_raw[2*विस्तारः*i + 2*j + 1] == State::S1);

			यदि (pat0 && !pat1)
				सारणी_2.at(i).प्रक्षिप_पृष्ठभागः(0);
			अन्यथा यदि (!pat0 && pat1)
				सारणी_2.at(i).प्रक्षिप_पृष्ठभागः(1);
			अन्यथा
				सारणी_2.at(i).प्रक्षिप_पृष्ठभागः(-1);
		}

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> products, undef_products;
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			पूर्णाङ्क undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep).at(0);

			प्रति (पूर्णाङ्क i = 0; i < गभीरता; i++)
			{
				std::सदिशम्<पूर्णाङ्क> cmp_a, cmp_ua, cmp_b;

				प्रति (पूर्णाङ्क j = 0; j < विस्तारः; j++)
					यदि (सारणी_2.at(i).at(j) >= 0) {
						cmp_a.प्रक्षिप_पृष्ठभागः(a.at(j));
						cmp_ua.प्रक्षिप_पृष्ठभागः(undef_a.at(j));
						cmp_b.प्रक्षिप_पृष्ठभागः(सारणी_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				std::सदिशम्<पूर्णाङ्क> masked_a = ez->vec_or(cmp_a, cmp_ua);
				std::सदिशम्<पूर्णाङ्क> masked_b = ez->vec_or(cmp_b, cmp_ua);

				पूर्णाङ्क masked_eq = ez->vec_eq(masked_a, masked_b);
				पूर्णाङ्क any_undef = ez->expression(ezSAT::OpOr, cmp_ua);

				undef_products.प्रक्षिप_पृष्ठभागः(ez->AND(any_undef, masked_eq));
				products.प्रक्षिप_पृष्ठभागः(ez->AND(ez->NOT(any_undef), masked_eq));
			}

			पूर्णाङ्क yy = ez->expression(ezSAT::OpOr, products);
			ez->समुच्चयः_2(undef_y, ez->AND(ez->NOT(yy), ez->expression(ezSAT::OpOr, undef_products)));
			undefGating(y, yy, undef_y);
		}
		अन्यथा
		{
			std::सदिशम्<पूर्णाङ्क> products;

			प्रति (पूर्णाङ्क i = 0; i < गभीरता; i++)
			{
				std::सदिशम्<पूर्णाङ्क> cmp_a, cmp_b;

				प्रति (पूर्णाङ्क j = 0; j < विस्तारः; j++)
					यदि (सारणी_2.at(i).at(j) >= 0) {
						cmp_a.प्रक्षिप_पृष्ठभागः(a.at(j));
						cmp_b.प्रक्षिप_पृष्ठभागः(सारणी_2.at(i).at(j) ? ez->CONST_TRUE : ez->CONST_FALSE);
					}

				products.प्रक्षिप_पृष्ठभागः(ez->vec_eq(cmp_a, cmp_b));
			}

			ez->समुच्चयः_2(y, ez->expression(ezSAT::OpOr, products));
		}

		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($fa))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> c = importDefSigSpec(cell->get_पत्तनम्(ID::C), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		std::सदिशम्<पूर्णाङ्क> x = importDefSigSpec(cell->get_पत्तनम्(ID::X), timestep);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		std::सदिशम्<पूर्णाङ्क> xx = model_undef ? ez->vec_var(x.आकार()) : x;

		std::सदिशम्<पूर्णाङ्क> t1 = ez->vec_xor(a, b);
		ez->assume(ez->vec_eq(yy, ez->vec_xor(t1, c)));

		std::सदिशम्<पूर्णाङ्क> t2 = ez->vec_and(a, b);
		std::सदिशम्<पूर्णाङ्क> t3 = ez->vec_and(c, t1);
		ez->assume(ez->vec_eq(xx, ez->vec_or(t2, t3)));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_c = importUndefSigSpec(cell->get_पत्तनम्(ID::C), timestep);

			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_x = importUndefSigSpec(cell->get_पत्तनम्(ID::X), timestep);

			ez->assume(ez->vec_eq(undef_y, ez->vec_or(ez->vec_or(undef_a, undef_b), undef_c)));
			ez->assume(ez->vec_eq(undef_x, undef_y));

			undefGating(y, yy, undef_y);
			undefGating(x, xx, undef_x);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($lcu))
	{
		std::सदिशम्<पूर्णाङ्क> p = importDefSigSpec(cell->get_पत्तनम्(ID::P), timestep);
		std::सदिशम्<पूर्णाङ्क> g = importDefSigSpec(cell->get_पत्तनम्(ID::G), timestep);
		std::सदिशम्<पूर्णाङ्क> ci = importDefSigSpec(cell->get_पत्तनम्(ID::CI), timestep);
		std::सदिशम्<पूर्णाङ्क> co = importDefSigSpec(cell->get_पत्तनम्(ID::CO), timestep);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(co.आकार()) : co;

		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(co); i++)
			ez->समुच्चयः_2(yy[i], ez->OR(g[i], ez->AND(p[i], i ? yy[i-1] : ci[0])));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_p = importUndefSigSpec(cell->get_पत्तनम्(ID::P), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_g = importUndefSigSpec(cell->get_पत्तनम्(ID::G), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_ci = importUndefSigSpec(cell->get_पत्तनम्(ID::CI), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_co = importUndefSigSpec(cell->get_पत्तनम्(ID::CO), timestep);

			पूर्णाङ्क undef_any_p = ez->expression(ezSAT::OpOr, undef_p);
			पूर्णाङ्क undef_any_g = ez->expression(ezSAT::OpOr, undef_g);
			पूर्णाङ्क undef_any_ci = ez->expression(ezSAT::OpOr, undef_ci);
			पूर्णाङ्क undef_co_bit = ez->OR(undef_any_p, undef_any_g, undef_any_ci);

			std::सदिशम्<पूर्णाङ्क> undef_co_द्वयङ्काः(undef_co.आकार(), undef_co_bit);
			ez->assume(ez->vec_eq(undef_co_द्वयङ्काः, undef_co));

			undefGating(co, yy, undef_co);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($alu))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> b = importDefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		std::सदिशम्<पूर्णाङ्क> x = importDefSigSpec(cell->get_पत्तनम्(ID::X), timestep);
		std::सदिशम्<पूर्णाङ्क> ci = importDefSigSpec(cell->get_पत्तनम्(ID::CI), timestep);
		std::सदिशम्<पूर्णाङ्क> bi = importDefSigSpec(cell->get_पत्तनम्(ID::BI), timestep);
		std::सदिशम्<पूर्णाङ्क> co = importDefSigSpec(cell->get_पत्तनम्(ID::CO), timestep);

		extend_Signal_विस्तारः(a, b, y, cell);
		extend_Signal_विस्तारः(a, b, x, cell);
		extend_Signal_विस्तारः(a, b, co, cell);

		std::सदिशम्<पूर्णाङ्क> def_y = model_undef ? ez->vec_var(y.आकार()) : y;
		std::सदिशम्<पूर्णाङ्क> def_x = model_undef ? ez->vec_var(x.आकार()) : x;
		std::सदिशम्<पूर्णाङ्क> def_co = model_undef ? ez->vec_var(co.आकार()) : co;

		log_assert(Get_आकार(y) == Get_आकार(x));
		log_assert(Get_आकार(y) == Get_आकार(co));
		log_assert(Get_आकार(ci) == 1);
		log_assert(Get_आकार(bi) == 1);

		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(y); i++)
		{
			पूर्णाङ्क s1 = a.at(i), s2 = ez->XOR(b.at(i), bi.at(0)), s3 = i ? co.at(i-1) : ci.at(0);
			ez->समुच्चयः_2(def_x.at(i), ez->XOR(s1, s2));
			ez->समुच्चयः_2(def_y.at(i), ez->XOR(def_x.at(i), s3));
			ez->समुच्चयः_2(def_co.at(i), ez->OR(ez->AND(s1, s2), ez->AND(s1, s3), ez->AND(s2, s3)));
		}

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_b = importUndefSigSpec(cell->get_पत्तनम्(ID::B), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_ci = importUndefSigSpec(cell->get_पत्तनम्(ID::CI), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_bi = importUndefSigSpec(cell->get_पत्तनम्(ID::BI), timestep);

			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_x = importUndefSigSpec(cell->get_पत्तनम्(ID::X), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_co = importUndefSigSpec(cell->get_पत्तनम्(ID::CO), timestep);

			extend_Signal_विस्तारः(undef_a, undef_b, undef_y, cell);
			extend_Signal_विस्तारः(undef_a, undef_b, undef_x, cell);
			extend_Signal_विस्तारः(undef_a, undef_b, undef_co, cell);

			std::सदिशम्<पूर्णाङ्क> all_inputs_undef;
			all_inputs_undef.निवेशय(all_inputs_undef.अन्तः(), undef_a.begin(), undef_a.अन्तः());
			all_inputs_undef.निवेशय(all_inputs_undef.अन्तः(), undef_b.begin(), undef_b.अन्तः());
			all_inputs_undef.निवेशय(all_inputs_undef.अन्तः(), undef_ci.begin(), undef_ci.अन्तः());
			all_inputs_undef.निवेशय(all_inputs_undef.अन्तः(), undef_bi.begin(), undef_bi.अन्तः());
			पूर्णाङ्क undef_any = ez->expression(ezSAT::OpOr, all_inputs_undef);

			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(undef_y); i++) {
				ez->समुच्चयः_2(undef_y.at(i), undef_any);
				ez->समुच्चयः_2(undef_x.at(i), ez->OR(undef_a.at(i), undef_b.at(i), undef_bi.at(0)));
				ez->समुच्चयः_2(undef_co.at(i), undef_any);
			}

			undefGating(y, def_y, undef_y);
			undefGating(x, def_x, undef_x);
			undefGating(co, def_co, undef_co);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($खण्डिका))
	{
		RTLIL::SigSpec a = cell->get_पत्तनम्(ID::A);
		RTLIL::SigSpec y = cell->get_पत्तनम्(ID::Y);
		ez->assume(सङ्केताः_eq(a.extract(cell->parameters.at(ID::विस्थापनम्_2).as_int(), y.आकार()), y, timestep));
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($concat))
	{
		RTLIL::SigSpec a = cell->get_पत्तनम्(ID::A);
		RTLIL::SigSpec b = cell->get_पत्तनम्(ID::B);
		RTLIL::SigSpec y = cell->get_पत्तनम्(ID::Y);

		RTLIL::SigSpec ab = a;
		ab.append(b);

		ez->assume(सङ्केताः_eq(ab, y, timestep));
		प्रत्यागम सत्य;
	}

	यदि (timestep > 0 && (cell->is_builtin_ff() || cell->type == ID($anyinit)))
	{
		Ff_दत्त ff(शून्यसूचकः, cell);

		// Latches and FFs with async inputs are not supported — use clk2fflogic or async2sync first.
		यदि (ff.has_aload || ff.has_arst || ff.has_sr)
			प्रत्यागम असत्य;

		यदि (timestep == 1)
		{
			initial_state.सङ्कलय((*sigmap)(cell->get_पत्तनम्(ID::Q)));
			यदि (model_undef && def_formal) {
				std::सदिशम्<पूर्णाङ्क> undef_q = importUndefSigSpec(cell->get_पत्तनम्(ID::Q), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_q)));
			}
		}
		अन्यथा
		{
			std::सदिशम्<पूर्णाङ्क> d = importDefSigSpec(cell->get_पत्तनम्(ID::D), timestep-1);
			std::सदिशम्<पूर्णाङ्क> undef_d;
			यदि (model_undef)
				undef_d = importUndefSigSpec(cell->get_पत्तनम्(ID::D), timestep-1);
			यदि (ff.has_srst && ff.has_ce && ff.ce_over_srst) {
				पूर्णाङ्क srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::सदिशम्<पूर्णाङ्क> rval = importDefSigSpec(ff.val_srst, timestep-1);
				पूर्णाङ्क undef_srst = -1;
				std::सदिशम्<पूर्णाङ्क> undef_rval;
				यदि (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				यदि (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				अन्यथा
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			यदि (ff.has_ce) {
				पूर्णाङ्क ce = importDefSigSpec(ff.sig_ce, timestep-1).at(0);
				std::सदिशम्<पूर्णाङ्क> old_q = importDefSigSpec(ff.sig_q, timestep-1);
				पूर्णाङ्क undef_ce = -1;
				std::सदिशम्<पूर्णाङ्क> undef_old_q;
				यदि (model_undef) {
					undef_ce = importUndefSigSpec(ff.sig_ce, timestep-1).at(0);
					undef_old_q = importUndefSigSpec(ff.sig_q, timestep-1);
				}
				यदि (ff.pol_ce)
					std::tie(d, undef_d) = mux(ce, undef_ce, old_q, undef_old_q, d, undef_d);
				अन्यथा
					std::tie(d, undef_d) = mux(ce, undef_ce, d, undef_d, old_q, undef_old_q);
			}
			यदि (ff.has_srst && !(ff.has_ce && ff.ce_over_srst)) {
				पूर्णाङ्क srst = importDefSigSpec(ff.sig_srst, timestep-1).at(0);
				std::सदिशम्<पूर्णाङ्क> rval = importDefSigSpec(ff.val_srst, timestep-1);
				पूर्णाङ्क undef_srst = -1;
				std::सदिशम्<पूर्णाङ्क> undef_rval;
				यदि (model_undef) {
					undef_srst = importUndefSigSpec(ff.sig_srst, timestep-1).at(0);
					undef_rval = importUndefSigSpec(ff.val_srst, timestep-1);
				}
				यदि (ff.pol_srst)
					std::tie(d, undef_d) = mux(srst, undef_srst, d, undef_d, rval, undef_rval);
				अन्यथा
					std::tie(d, undef_d) = mux(srst, undef_srst, rval, undef_rval, d, undef_d);
			}
			std::सदिशम्<पूर्णाङ्क> q = importDefSigSpec(cell->get_पत्तनम्(ID::Q), timestep);

			std::सदिशम्<पूर्णाङ्क> qq = model_undef ? ez->vec_var(q.आकार()) : q;
			ez->assume(ez->vec_eq(d, qq));

			यदि (model_undef)
			{
				std::सदिशम्<पूर्णाङ्क> undef_q = importUndefSigSpec(cell->get_पत्तनम्(ID::Q), timestep);

				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($anyconst))
	{
		यदि (timestep < 2) {
			यदि (model_undef && def_formal) {
				std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
				ez->assume(ez->NOT(ez->vec_reduce_or(undef_y)));
			}
			प्रत्यागम सत्य;
		}

		std::सदिशम्<पूर्णाङ्क> d = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep-1);
		std::सदिशम्<पूर्णाङ्क> q = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

		std::सदिशम्<पूर्णाङ्क> qq = (model_undef && !def_formal) ? ez->vec_var(q.आकार()) : q;
		ez->assume(ez->vec_eq(d, qq));

		यदि (model_undef)
		{
			std::सदिशम्<पूर्णाङ्क> undef_d = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep-1);
			std::सदिशम्<पूर्णाङ्क> undef_q = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);

			यदि (def_formal) {
				प्रति (स्वचालित &undef_q_bit : undef_q)
					ez->समुच्चयः_2(ez->CONST_FALSE, undef_q_bit);
			} अन्यथा {
				ez->assume(ez->vec_eq(undef_d, undef_q));
				undefGating(q, qq, undef_q);
			}
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($anyseq))
	{
		यदि (model_undef && def_formal) {
			std::सदिशम्<पूर्णाङ्क> undef_q = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			प्रति (स्वचालित &undef_q_bit : undef_q)
				ez->समुच्चयः_2(ez->CONST_FALSE, undef_q_bit);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($barrier) && !model_barriers)
		प्रत्यागम सत्य;

	यदि (cell->type.अन्तर्(ID($_BUF_), ID($equiv), ID($barrier)))
	{
		std::सदिशम्<पूर्णाङ्क> a = importDefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		extend_Signal_विस्तारः_Unary(a, y, cell);

		std::सदिशम्<पूर्णाङ्क> yy = model_undef ? ez->vec_var(y.आकार()) : y;
		ez->assume(ez->vec_eq(a, yy));

		यदि (model_undef) {
			std::सदिशम्<पूर्णाङ्क> undef_a = importUndefSigSpec(cell->get_पत्तनम्(ID::A), timestep);
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			extend_Signal_विस्तारः_Unary(undef_a, undef_y, cell, असत्य);
			ez->assume(ez->vec_eq(undef_a, undef_y));
			undefGating(y, yy, undef_y);
		}
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($initstate))
	{
		स्वचालित कुञ्चिका = make_pair(उपसर्गः_2, timestep);
		यदि (initstates.गणनम्(कुञ्चिका) == 0)
			initstates[कुञ्चिका] = असत्य;

		std::सदिशम्<पूर्णाङ्क> y = importDefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
		log_assert(Get_आकार(y) == 1);
		ez->समुच्चयः_2(y[0], initstates[कुञ्चिका] ? ez->CONST_TRUE : ez->CONST_FALSE);

		यदि (model_undef) {
			std::सदिशम्<पूर्णाङ्क> undef_y = importUndefSigSpec(cell->get_पत्तनम्(ID::Y), timestep);
			log_assert(Get_आकार(undef_y) == 1);
			ez->समुच्चयः_2(undef_y[0], ez->CONST_FALSE);
		}

		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($प्रतिज्ञा))
	{
		std::string pf = उपसर्गः_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		asserts_a[pf].append((*sigmap)(cell->get_पत्तनम्(ID::A)));
		asserts_en[pf].append((*sigmap)(cell->get_पत्तनम्(ID::EN)));
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($assume))
	{
		std::string pf = उपसर्गः_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		assumes_a[pf].append((*sigmap)(cell->get_पत्तनम्(ID::A)));
		assumes_en[pf].append((*sigmap)(cell->get_पत्तनम्(ID::EN)));
		प्रत्यागम सत्य;
	}

	यदि (cell->type == ID($scopeinfo) || cell->type == ID($निवेशः_पत्तनम्))
	{
		प्रत्यागम सत्य;
	}

	// Unsupported internal cell types: $pow $fsm $mem*
	// .. and all sequential cells with asynchronous inputs
	प्रत्यागम असत्य;
}

नामाकाश Yosys {

शून्य report_missing_model(तार्किक warn_only, RTLIL::Cell* cell)
{
	std::string s;
	यदि (cell->is_builtin_ff())
		s = stringf("No SAT model available for async FF cell %s (%s).  Consider running `async2sync` or `clk2fflogic` first.", cell, cell->type.unescape());
	अन्यथा
		s = stringf("No SAT model available for cell %s (%s).", cell, cell->type.unescape());

	यदि (warn_only) {
		log_warning_noprefix("%s\n", s);
	} अन्यथा {
		log_error("%s\n", s);
	}
}

}
