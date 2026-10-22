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

#यदि_अपरिभाषितम् MACC_H
#परिभाषय MACC_H

#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

संरचना Macc
{
	संरचना पदांशः_t {
		RTLIL::SigSpec अन्तर्_a, अन्तर्_b;
		bool is_signed, do_व्यवकलय;
	};
	std::सदिशम्<पदांशः_t> terms;

	शून्य optimize(पूर्णाङ्क विस्तारः)
	{
		std::सदिशम्<पदांशः_t> new_terms;
		RTLIL::Const off(0, विस्तारः);

		प्रति (स्वचालित &पत्तनम् : terms)
		{
			यदि (Get_आकार(पत्तनम्.अन्तर्_a) == 0 && Get_आकार(पत्तनम्.अन्तर्_b) == 0)
				अनुवर्त;

			यदि (Get_आकार(पत्तनम्.अन्तर्_a) < Get_आकार(पत्तनम्.अन्तर्_b))
				std::swap(पत्तनम्.अन्तर्_a, पत्तनम्.अन्तर्_b);

			यदि (पत्तनम्.अन्तर्_a.is_fully_const() && पत्तनम्.अन्तर्_b.is_fully_const()) {
				RTLIL::Const v = पत्तनम्.अन्तर्_a.as_const();
				यदि (Get_आकार(पत्तनम्.अन्तर्_b))
					v = const_mul(v, पत्तनम्.अन्तर्_b.as_const(), पत्तनम्.is_signed, पत्तनम्.is_signed, विस्तारः);
				यदि (पत्तनम्.do_व्यवकलय)
					off = const_sub(off, v, पत्तनम्.is_signed, पत्तनम्.is_signed, विस्तारः);
				अन्यथा
					off = const_सङ्कलय(off, v, पत्तनम्.is_signed, पत्तनम्.is_signed, विस्तारः);
				अनुवर्त;
			}

			यदि (पत्तनम्.is_signed) {
				यावत् (Get_आकार(पत्तनम्.अन्तर्_a) > 1 && पत्तनम्.अन्तर्_a[Get_आकार(पत्तनम्.अन्तर्_a)-1] == पत्तनम्.अन्तर्_a[Get_आकार(पत्तनम्.अन्तर्_a)-2])
					पत्तनम्.अन्तर्_a.remove(Get_आकार(पत्तनम्.अन्तर्_a)-1);
				यावत् (Get_आकार(पत्तनम्.अन्तर्_b) > 1 && पत्तनम्.अन्तर्_b[Get_आकार(पत्तनम्.अन्तर्_b)-1] == पत्तनम्.अन्तर्_b[Get_आकार(पत्तनम्.अन्तर्_b)-2])
					पत्तनम्.अन्तर्_b.remove(Get_आकार(पत्तनम्.अन्तर्_b)-1);
			} अन्यथा {
				यावत् (Get_आकार(पत्तनम्.अन्तर्_a) > 1 && पत्तनम्.अन्तर्_a[Get_आकार(पत्तनम्.अन्तर्_a)-1] == State::S0)
					पत्तनम्.अन्तर्_a.remove(Get_आकार(पत्तनम्.अन्तर्_a)-1);
				यावत् (Get_आकार(पत्तनम्.अन्तर्_b) > 1 && पत्तनम्.अन्तर्_b[Get_आकार(पत्तनम्.अन्तर्_b)-1] == State::S0)
					पत्तनम्.अन्तर्_b.remove(Get_आकार(पत्तनम्.अन्तर्_b)-1);
			}

			new_terms.प्रक्षिप_पृष्ठभागः(पत्तनम्);
		}

		यदि (off.as_bool()) {
			पदांशः_t पत्तनम्;
			पत्तनम्.अन्तर्_a = off;
			पत्तनम्.is_signed = false;
			पत्तनम्.do_व्यवकलय = false;
			new_terms.प्रक्षिप_पृष्ठभागः(पत्तनम्);
		}

		new_terms.swap(terms);
	}

	शून्य from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec पत्तनम्_a = cell->get_पत्तनम्(ID::A);

		terms.clear();

		स्वचालित config_द्वयङ्काः = cell->getParam(ID::CONFIG);
		पूर्णाङ्क config_cursor = 0;

		पूर्णाङ्क config_विस्तारः = cell->getParam(ID::CONFIG_विस्तारः).as_int();
		log_assert(Get_आकार(config_द्वयङ्काः) >= config_विस्तारः);

		पूर्णाङ्क अङ्कः_द्वयङ्काः = 0;
		यदि (config_द्वयङ्काः[config_cursor++] == State::S1) अङ्कः_द्वयङ्काः |= 1;
		यदि (config_द्वयङ्काः[config_cursor++] == State::S1) अङ्कः_द्वयङ्काः |= 2;
		यदि (config_द्वयङ्काः[config_cursor++] == State::S1) अङ्कः_द्वयङ्काः |= 4;
		यदि (config_द्वयङ्काः[config_cursor++] == State::S1) अङ्कः_द्वयङ्काः |= 8;

		पूर्णाङ्क पत्तनम्_a_cursor = 0;
		यावत् (पत्तनम्_a_cursor < Get_आकार(पत्तनम्_a))
		{
			log_assert(config_cursor + 2 + 2*अङ्कः_द्वयङ्काः <= config_विस्तारः);

			पदांशः_t this_पत्तनम्;
			this_पत्तनम्.is_signed = config_द्वयङ्काः[config_cursor++] == State::S1;
			this_पत्तनम्.do_व्यवकलय = config_द्वयङ्काः[config_cursor++] == State::S1;

			पूर्णाङ्क आकार_a = 0;
			प्रति (पूर्णाङ्क i = 0; i < अङ्कः_द्वयङ्काः; i++)
				यदि (config_द्वयङ्काः[config_cursor++] == State::S1)
					आकार_a |= 1 << i;

			this_पत्तनम्.अन्तर्_a = पत्तनम्_a.extract(पत्तनम्_a_cursor, आकार_a);
			पत्तनम्_a_cursor += आकार_a;

			पूर्णाङ्क आकार_b = 0;
			प्रति (पूर्णाङ्क i = 0; i < अङ्कः_द्वयङ्काः; i++)
				यदि (config_द्वयङ्काः[config_cursor++] == State::S1)
					आकार_b |= 1 << i;

			this_पत्तनम्.अन्तर्_b = पत्तनम्_a.extract(पत्तनम्_a_cursor, आकार_b);
			पत्तनम्_a_cursor += आकार_b;

			यदि (आकार_a || आकार_b)
				terms.प्रक्षिप_पृष्ठभागः(this_पत्तनम्);
		}

		प्रति (स्वचालित bit : cell->get_पत्तनम्(ID::B))
			terms.प्रक्षिप_पृष्ठभागः(पदांशः_t{{bit}, {}, false, false});

		log_assert(config_cursor == config_विस्तारः);
		log_assert(पत्तनम्_a_cursor == Get_आकार(पत्तनम्_a));
	}

	शून्य from_cell(RTLIL::Cell *cell)
	{
		यदि (cell->type == ID($macc)) {
			from_cell_v1(cell);
			प्रत्यागम;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec पत्तनम्_a = cell->get_पत्तनम्(ID::A);
		RTLIL::SigSpec पत्तनम्_b = cell->get_पत्तनम्(ID::B);
		RTLIL::SigSpec पत्तनम्_c = cell->get_पत्तनम्(ID::C);

		terms.clear();

		पूर्णाङ्क nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ध्रुव Const &गुणनफलम्_neg = cell->getParam(ID::गुणनफलम्_NEGATED);
		ध्रुव Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ध्रुव Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ध्रुव Const &a_signed = cell->getParam(ID::A_SIGNED);
		ध्रुव Const &b_signed = cell->getParam(ID::B_SIGNED);
		पूर्णाङ्क ai = 0, bi = 0;
		प्रति (पूर्णाङ्क i = 0; i < nproducts; i++) {
			पदांशः_t पदांशः;

			log_assert(a_signed[i] == b_signed[i]);
			पदांशः.is_signed = (a_signed[i] == State::S1);
			पूर्णाङ्क a_विस्तारः = a_widths.extract(16 * i, 16).as_int(false);
			पूर्णाङ्क b_विस्तारः = b_widths.extract(16 * i, 16).as_int(false);

			पदांशः.अन्तर्_a = पत्तनम्_a.extract(ai, a_विस्तारः);
			ai += a_विस्तारः;
			पदांशः.अन्तर्_b = पत्तनम्_b.extract(bi, b_विस्तारः);
			bi += b_विस्तारः;
			पदांशः.do_व्यवकलय = (गुणनफलम्_neg[i] == State::S1);

			terms.प्रक्षिप_पृष्ठभागः(पदांशः);
		}
		log_assert(पत्तनम्_a.आकार() == ai);
		log_assert(पत्तनम्_b.आकार() == bi);

		पूर्णाङ्क naddends = cell->getParam(ID::NADDENDS).as_int();
		ध्रुव Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ध्रुव Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ध्रुव Const &c_signed = cell->getParam(ID::C_SIGNED);
		पूर्णाङ्क ci = 0;
		प्रति (पूर्णाङ्क i = 0; i < naddends; i++) {
			पदांशः_t पदांशः;

			पदांशः.is_signed = (c_signed[i] == State::S1);
			पूर्णाङ्क c_विस्तारः = c_widths.extract(16 * i, 16).as_int(false);

			पदांशः.अन्तर्_a = पत्तनम्_c.extract(ci, c_विस्तारः);
			ci += c_विस्तारः;
			पदांशः.do_व्यवकलय = (addend_neg[i] == State::S1);

			terms.प्रक्षिप_पृष्ठभागः(पदांशः);
		}
		log_assert(पत्तनम्_c.आकार() == ci);
	}

	शून्य गन्तव्यम्_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		पूर्णाङ्क nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, गुणनफलम्_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क) terms.आकार(); i++) {
			SigSpec पदांशः_a = terms[i].अन्तर्_a, पदांशः_b = terms[i].अन्तर्_b;

			यदि (पदांशः_b.रिक्तानि()) {
				// addend
				c_widths.append(Const(पदांशः_a.आकार(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_व्यवकलय ? RTLIL::S1 : RTLIL::S0);
				c.append(पदांशः_a);
				naddends++;
			} अन्यथा {
				//गुणनफलम्
				a_widths.append(Const(पदांशः_a.आकार(), 16));
				b_widths.append(Const(पदांशः_b.आकार(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				गुणनफलम्_negated.append(terms[i].do_व्यवकलय ? RTLIL::S1 : RTLIL::S0);
				a.append(पदांशः_a);
				b.append(पदांशः_b);
				nproducts++;
			}
		}

		यदि (a_signed.रिक्तानि())
			a_signed = {RTLIL::Sx};
		यदि (b_signed.रिक्तानि())
			b_signed = {RTLIL::Sx};
		यदि (c_signed.रिक्तानि())
			c_signed = {RTLIL::Sx};
		यदि (a_widths.रिक्तानि())
			a_widths = {RTLIL::Sx};
		यदि (b_widths.रिक्तानि())
			b_widths = {RTLIL::Sx};
		यदि (c_widths.रिक्तानि())
			c_widths = {RTLIL::Sx};
		यदि (गुणनफलम्_negated.रिक्तानि())
			गुणनफलम्_negated = {RTLIL::Sx};
		यदि (addend_negated.रिक्तानि())
			addend_negated = {RTLIL::Sx};

		cell->समुच्चयः_Param(ID::NPRODUCTS, nproducts);
		cell->समुच्चयः_Param(ID::गुणनफलम्_NEGATED, गुणनफलम्_negated);
		cell->समुच्चयः_Param(ID::NADDENDS, naddends);
		cell->समुच्चयः_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->समुच्चयः_Param(ID::A_SIGNED, a_signed);
		cell->समुच्चयः_Param(ID::B_SIGNED, b_signed);
		cell->समुच्चयः_Param(ID::C_SIGNED, c_signed);
		cell->समुच्चयः_Param(ID::A_WIDTHS, a_widths);
		cell->समुच्चयः_Param(ID::B_WIDTHS, b_widths);
		cell->समुच्चयः_Param(ID::C_WIDTHS, c_widths);
		cell->समुच्चयः_पत्तनम्(ID::A, a);
		cell->समुच्चयः_पत्तनम्(ID::B, b);
		cell->समुच्चयः_पत्तनम्(ID::C, c);
	}

	bool eval(RTLIL::Const &परिणाम) ध्रुव
	{
		प्रति (स्वचालित bit : परिणाम)
			bit = State::S0;

		प्रति (स्वचालित &पत्तनम् : terms)
		{
			यदि (!पत्तनम्.अन्तर्_a.is_fully_const() || !पत्तनम्.अन्तर्_b.is_fully_const())
				प्रत्यागम false;

			RTLIL::Const summand;
			यदि (Get_आकार(पत्तनम्.अन्तर्_b) == 0)
				summand = const_pos(पत्तनम्.अन्तर्_a.as_const(), पत्तनम्.अन्तर्_b.as_const(), पत्तनम्.is_signed, पत्तनम्.is_signed, Get_आकार(परिणाम));
			अन्यथा
				summand = const_mul(पत्तनम्.अन्तर्_a.as_const(), पत्तनम्.अन्तर्_b.as_const(), पत्तनम्.is_signed, पत्तनम्.is_signed, Get_आकार(परिणाम));

			यदि (पत्तनम्.do_व्यवकलय)
				परिणाम = const_sub(परिणाम, summand, पत्तनम्.is_signed, पत्तनम्.is_signed, Get_आकार(परिणाम));
			अन्यथा
				परिणाम = const_सङ्कलय(परिणाम, summand, पत्तनम्.is_signed, पत्तनम्.is_signed, Get_आकार(परिणाम));
		}

		प्रत्यागम true;
	}

	bool is_simple_गुणनफलम्()
	{
		प्रत्यागम terms.आकार() == 1 &&
				!terms[0].अन्तर्_b.रिक्तानि() &&
				!terms[0].do_व्यवकलय;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		यदि (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
