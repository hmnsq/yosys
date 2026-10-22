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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MACC_H
#ವ್ಯಾಖ್ಯಾನಿಸು MACC_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ Macc
{
	ರಚನೆ ಪದ_t {
		RTLIL::SigSpec ಒಳ_a, ಒಳ_b;
		bool is_signed, do_ಕಳೆ;
	};
	std::ಸದಿಶ<ಪದ_t> terms;

	ಶೂನ್ಯ optimize(ಪೂರ್ಣಾಂಕ ಅಗಲ)
	{
		std::ಸದಿಶ<ಪದ_t> new_terms;
		RTLIL::Const off(0, ಅಗಲ);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : terms)
		{
			ಆದರೆ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a) == 0 && Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b) == 0)
				ಮುಂದುವರಿ;

			ಆದರೆ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a) < Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b))
				std::ಅದಲುಬದಲು(ಪೋರ್ಟ್.ಒಳ_a, ಪೋರ್ಟ್.ಒಳ_b);

			ಆದರೆ (ಪೋರ್ಟ್.ಒಳ_a.is_fully_const() && ಪೋರ್ಟ್.ಒಳ_b.is_fully_const()) {
				RTLIL::Const v = ಪೋರ್ಟ್.ಒಳ_a.as_const();
				ಆದರೆ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b))
					v = const_mul(v, ಪೋರ್ಟ್.ಒಳ_b.as_const(), ಪೋರ್ಟ್.is_signed, ಪೋರ್ಟ್.is_signed, ಅಗಲ);
				ಆದರೆ (ಪೋರ್ಟ್.do_ಕಳೆ)
					off = const_sub(off, v, ಪೋರ್ಟ್.is_signed, ಪೋರ್ಟ್.is_signed, ಅಗಲ);
				ಇಲ್ಲದಿದ್ದರೆ
					off = const_ಕೂಡಿಸು(off, v, ಪೋರ್ಟ್.is_signed, ಪೋರ್ಟ್.is_signed, ಅಗಲ);
				ಮುಂದುವರಿ;
			}

			ಆದರೆ (ಪೋರ್ಟ್.is_signed) {
				ಆಗ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a) > 1 && ಪೋರ್ಟ್.ಒಳ_a[Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a)-1] == ಪೋರ್ಟ್.ಒಳ_a[Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a)-2])
					ಪೋರ್ಟ್.ಒಳ_a.remove(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a)-1);
				ಆಗ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b) > 1 && ಪೋರ್ಟ್.ಒಳ_b[Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b)-1] == ಪೋರ್ಟ್.ಒಳ_b[Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b)-2])
					ಪೋರ್ಟ್.ಒಳ_b.remove(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b)-1);
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆಗ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a) > 1 && ಪೋರ್ಟ್.ಒಳ_a[Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a)-1] == State::S0)
					ಪೋರ್ಟ್.ಒಳ_a.remove(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_a)-1);
				ಆಗ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b) > 1 && ಪೋರ್ಟ್.ಒಳ_b[Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b)-1] == State::S0)
					ಪೋರ್ಟ್.ಒಳ_b.remove(Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b)-1);
			}

			new_terms.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್);
		}

		ಆದರೆ (off.as_bool()) {
			ಪದ_t ಪೋರ್ಟ್;
			ಪೋರ್ಟ್.ಒಳ_a = off;
			ಪೋರ್ಟ್.is_signed = false;
			ಪೋರ್ಟ್.do_ಕಳೆ = false;
			new_terms.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್);
		}

		new_terms.ಅದಲುಬದಲು(terms);
	}

	ಶೂನ್ಯ from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec ಪೋರ್ಟ್_a = cell->get_ಪೋರ್ಟ್(ID::A);

		terms.clear();

		ಸ್ವಯಂಚಾಲಿತ config_ಬಿಟ್ಟುಗಳು = cell->getParam(ID::CONFIG);
		ಪೂರ್ಣಾಂಕ config_cursor = 0;

		ಪೂರ್ಣಾಂಕ config_ಅಗಲ = cell->getParam(ID::CONFIG_ಅಗಲ).as_int();
		log_assert(Get_ಗಾತ್ರ(config_ಬಿಟ್ಟುಗಳು) >= config_ಅಗಲ);

		ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಬಿಟ್ಟುಗಳು = 0;
		ಆದರೆ (config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1) ಅಂಕೆ_ಬಿಟ್ಟುಗಳು |= 1;
		ಆದರೆ (config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1) ಅಂಕೆ_ಬಿಟ್ಟುಗಳು |= 2;
		ಆದರೆ (config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1) ಅಂಕೆ_ಬಿಟ್ಟುಗಳು |= 4;
		ಆದರೆ (config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1) ಅಂಕೆ_ಬಿಟ್ಟುಗಳು |= 8;

		ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್_a_cursor = 0;
		ಆಗ (ಪೋರ್ಟ್_a_cursor < Get_ಗಾತ್ರ(ಪೋರ್ಟ್_a))
		{
			log_assert(config_cursor + 2 + 2*ಅಂಕೆ_ಬಿಟ್ಟುಗಳು <= config_ಅಗಲ);

			ಪದ_t this_ಪೋರ್ಟ್;
			this_ಪೋರ್ಟ್.is_signed = config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1;
			this_ಪೋರ್ಟ್.do_ಕಳೆ = config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1;

			ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_a = 0;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ಬಿಟ್ಟುಗಳು; i++)
				ಆದರೆ (config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1)
					ಗಾತ್ರ_a |= 1 << i;

			this_ಪೋರ್ಟ್.ಒಳ_a = ಪೋರ್ಟ್_a.ಹೊರತೆಗೆ(ಪೋರ್ಟ್_a_cursor, ಗಾತ್ರ_a);
			ಪೋರ್ಟ್_a_cursor += ಗಾತ್ರ_a;

			ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_b = 0;
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_ಬಿಟ್ಟುಗಳು; i++)
				ಆದರೆ (config_ಬಿಟ್ಟುಗಳು[config_cursor++] == State::S1)
					ಗಾತ್ರ_b |= 1 << i;

			this_ಪೋರ್ಟ್.ಒಳ_b = ಪೋರ್ಟ್_a.ಹೊರತೆಗೆ(ಪೋರ್ಟ್_a_cursor, ಗಾತ್ರ_b);
			ಪೋರ್ಟ್_a_cursor += ಗಾತ್ರ_b;

			ಆದರೆ (ಗಾತ್ರ_a || ಗಾತ್ರ_b)
				terms.ತಳ್ಳು_ಹಿಂಭಾಗ(this_ಪೋರ್ಟ್);
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : cell->get_ಪೋರ್ಟ್(ID::B))
			terms.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪದ_t{{ಬಿಟ್}, {}, false, false});

		log_assert(config_cursor == config_ಅಗಲ);
		log_assert(ಪೋರ್ಟ್_a_cursor == Get_ಗಾತ್ರ(ಪೋರ್ಟ್_a));
	}

	ಶೂನ್ಯ from_cell(RTLIL::Cell *cell)
	{
		ಆದರೆ (cell->type == ID($macc)) {
			from_cell_v1(cell);
			ಹಿಂತಿರುಗಿ;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec ಪೋರ್ಟ್_a = cell->get_ಪೋರ್ಟ್(ID::A);
		RTLIL::SigSpec ಪೋರ್ಟ್_b = cell->get_ಪೋರ್ಟ್(ID::B);
		RTLIL::SigSpec ಪೋರ್ಟ್_c = cell->get_ಪೋರ್ಟ್(ID::C);

		terms.clear();

		ಪೂರ್ಣಾಂಕ nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ಸ್ಥಿರಾಂಕ Const &ಗುಣಲಬ್ಧ_neg = cell->getParam(ID::ಗುಣಲಬ್ಧ_NEGATED);
		ಸ್ಥಿರಾಂಕ Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ಸ್ಥಿರಾಂಕ Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ಸ್ಥಿರಾಂಕ Const &a_signed = cell->getParam(ID::A_SIGNED);
		ಸ್ಥಿರಾಂಕ Const &b_signed = cell->getParam(ID::B_SIGNED);
		ಪೂರ್ಣಾಂಕ ai = 0, bi = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nproducts; i++) {
			ಪದ_t ಪದ;

			log_assert(a_signed[i] == b_signed[i]);
			ಪದ.is_signed = (a_signed[i] == State::S1);
			ಪೂರ್ಣಾಂಕ a_ಅಗಲ = a_widths.ಹೊರತೆಗೆ(16 * i, 16).as_int(false);
			ಪೂರ್ಣಾಂಕ b_ಅಗಲ = b_widths.ಹೊರತೆಗೆ(16 * i, 16).as_int(false);

			ಪದ.ಒಳ_a = ಪೋರ್ಟ್_a.ಹೊರತೆಗೆ(ai, a_ಅಗಲ);
			ai += a_ಅಗಲ;
			ಪದ.ಒಳ_b = ಪೋರ್ಟ್_b.ಹೊರತೆಗೆ(bi, b_ಅಗಲ);
			bi += b_ಅಗಲ;
			ಪದ.do_ಕಳೆ = (ಗುಣಲಬ್ಧ_neg[i] == State::S1);

			terms.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪದ);
		}
		log_assert(ಪೋರ್ಟ್_a.ಗಾತ್ರ_2() == ai);
		log_assert(ಪೋರ್ಟ್_b.ಗಾತ್ರ_2() == bi);

		ಪೂರ್ಣಾಂಕ naddends = cell->getParam(ID::NADDENDS).as_int();
		ಸ್ಥಿರಾಂಕ Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ಸ್ಥಿರಾಂಕ Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ಸ್ಥಿರಾಂಕ Const &c_signed = cell->getParam(ID::C_SIGNED);
		ಪೂರ್ಣಾಂಕ ci = 0;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < naddends; i++) {
			ಪದ_t ಪದ;

			ಪದ.is_signed = (c_signed[i] == State::S1);
			ಪೂರ್ಣಾಂಕ c_ಅಗಲ = c_widths.ಹೊರತೆಗೆ(16 * i, 16).as_int(false);

			ಪದ.ಒಳ_a = ಪೋರ್ಟ್_c.ಹೊರತೆಗೆ(ci, c_ಅಗಲ);
			ci += c_ಅಗಲ;
			ಪದ.do_ಕಳೆ = (addend_neg[i] == State::S1);

			terms.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪದ);
		}
		log_assert(ಪೋರ್ಟ್_c.ಗಾತ್ರ_2() == ci);
	}

	ಶೂನ್ಯ ಗಮ್ಯ_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		ಪೂರ್ಣಾಂಕ nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, ಗುಣಲಬ್ಧ_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (ಪೂರ್ಣಾಂಕ) terms.ಗಾತ್ರ_2(); i++) {
			SigSpec ಪದ_a = terms[i].ಒಳ_a, ಪದ_b = terms[i].ಒಳ_b;

			ಆದರೆ (ಪದ_b.ಖಾಲಿಗಳು()) {
				// addend
				c_widths.append(Const(ಪದ_a.ಗಾತ್ರ_2(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_ಕಳೆ ? RTLIL::S1 : RTLIL::S0);
				c.append(ಪದ_a);
				naddends++;
			} ಇಲ್ಲದಿದ್ದರೆ {
				//ಗುಣಲಬ್ಧ
				a_widths.append(Const(ಪದ_a.ಗಾತ್ರ_2(), 16));
				b_widths.append(Const(ಪದ_b.ಗಾತ್ರ_2(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				ಗುಣಲಬ್ಧ_negated.append(terms[i].do_ಕಳೆ ? RTLIL::S1 : RTLIL::S0);
				a.append(ಪದ_a);
				b.append(ಪದ_b);
				nproducts++;
			}
		}

		ಆದರೆ (a_signed.ಖಾಲಿಗಳು())
			a_signed = {RTLIL::Sx};
		ಆದರೆ (b_signed.ಖಾಲಿಗಳು())
			b_signed = {RTLIL::Sx};
		ಆದರೆ (c_signed.ಖಾಲಿಗಳು())
			c_signed = {RTLIL::Sx};
		ಆದರೆ (a_widths.ಖಾಲಿಗಳು())
			a_widths = {RTLIL::Sx};
		ಆದರೆ (b_widths.ಖಾಲಿಗಳು())
			b_widths = {RTLIL::Sx};
		ಆದರೆ (c_widths.ಖಾಲಿಗಳು())
			c_widths = {RTLIL::Sx};
		ಆದರೆ (ಗುಣಲಬ್ಧ_negated.ಖಾಲಿಗಳು())
			ಗುಣಲಬ್ಧ_negated = {RTLIL::Sx};
		ಆದರೆ (addend_negated.ಖಾಲಿಗಳು())
			addend_negated = {RTLIL::Sx};

		cell->ಗಣ_Param(ID::NPRODUCTS, nproducts);
		cell->ಗಣ_Param(ID::ಗುಣಲಬ್ಧ_NEGATED, ಗುಣಲಬ್ಧ_negated);
		cell->ಗಣ_Param(ID::NADDENDS, naddends);
		cell->ಗಣ_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->ಗಣ_Param(ID::A_SIGNED, a_signed);
		cell->ಗಣ_Param(ID::B_SIGNED, b_signed);
		cell->ಗಣ_Param(ID::C_SIGNED, c_signed);
		cell->ಗಣ_Param(ID::A_WIDTHS, a_widths);
		cell->ಗಣ_Param(ID::B_WIDTHS, b_widths);
		cell->ಗಣ_Param(ID::C_WIDTHS, c_widths);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::A, a);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::B, b);
		cell->ಗಣ_ಪೋರ್ಟ್(ID::C, c);
	}

	bool eval(RTLIL::Const &ಫಲಿತಾಂಶ) ಸ್ಥಿರಾಂಕ
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : ಫಲಿತಾಂಶ)
			ಬಿಟ್ = State::S0;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : terms)
		{
			ಆದರೆ (!ಪೋರ್ಟ್.ಒಳ_a.is_fully_const() || !ಪೋರ್ಟ್.ಒಳ_b.is_fully_const())
				ಹಿಂತಿರುಗಿ false;

			RTLIL::Const summand;
			ಆದರೆ (Get_ಗಾತ್ರ(ಪೋರ್ಟ್.ಒಳ_b) == 0)
				summand = const_pos(ಪೋರ್ಟ್.ಒಳ_a.as_const(), ಪೋರ್ಟ್.ಒಳ_b.as_const(), ಪೋರ್ಟ್.is_signed, ಪೋರ್ಟ್.is_signed, Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ));
			ಇಲ್ಲದಿದ್ದರೆ
				summand = const_mul(ಪೋರ್ಟ್.ಒಳ_a.as_const(), ಪೋರ್ಟ್.ಒಳ_b.as_const(), ಪೋರ್ಟ್.is_signed, ಪೋರ್ಟ್.is_signed, Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ));

			ಆದರೆ (ಪೋರ್ಟ್.do_ಕಳೆ)
				ಫಲಿತಾಂಶ = const_sub(ಫಲಿತಾಂಶ, summand, ಪೋರ್ಟ್.is_signed, ಪೋರ್ಟ್.is_signed, Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ));
			ಇಲ್ಲದಿದ್ದರೆ
				ಫಲಿತಾಂಶ = const_ಕೂಡಿಸು(ಫಲಿತಾಂಶ, summand, ಪೋರ್ಟ್.is_signed, ಪೋರ್ಟ್.is_signed, Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ));
		}

		ಹಿಂತಿರುಗಿ true;
	}

	bool is_simple_ಗುಣಲಬ್ಧ()
	{
		ಹಿಂತಿರುಗಿ terms.ಗಾತ್ರ_2() == 1 &&
				!terms[0].ಒಳ_b.ಖಾಲಿಗಳು() &&
				!terms[0].do_ಕಳೆ;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		ಆದರೆ (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
