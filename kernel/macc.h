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

#अगर_अघोषित MACC_H
#घोषणा MACC_H

#समावेश "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

काष्ठा Macc
{
	काष्ठा term_t {
		RTLIL::SigSpec in_a, in_b;
		bool is_signed, do_subtract;
	};
	std::vector<term_t> terms;

	व्योम optimize(पूर्णांक width)
	{
		std::vector<term_t> new_terms;
		RTLIL::Const off(0, width);

		क्रम (स्वतः &port : terms)
		{
			अगर (Get_आकार(port.in_a) == 0 && Get_आकार(port.in_b) == 0)
				जारी;

			अगर (Get_आकार(port.in_a) < Get_आकार(port.in_b))
				std::swap(port.in_a, port.in_b);

			अगर (port.in_a.is_fully_const() && port.in_b.is_fully_const()) {
				RTLIL::Const v = port.in_a.as_const();
				अगर (Get_आकार(port.in_b))
					v = const_mul(v, port.in_b.as_const(), port.is_signed, port.is_signed, width);
				अगर (port.do_subtract)
					off = const_sub(off, v, port.is_signed, port.is_signed, width);
				अन्यथा
					off = const_add(off, v, port.is_signed, port.is_signed, width);
				जारी;
			}

			अगर (port.is_signed) {
				जबतक (Get_आकार(port.in_a) > 1 && port.in_a[Get_आकार(port.in_a)-1] == port.in_a[Get_आकार(port.in_a)-2])
					port.in_a.हटाओ(Get_आकार(port.in_a)-1);
				जबतक (Get_आकार(port.in_b) > 1 && port.in_b[Get_आकार(port.in_b)-1] == port.in_b[Get_आकार(port.in_b)-2])
					port.in_b.हटाओ(Get_आकार(port.in_b)-1);
			} अन्यथा {
				जबतक (Get_आकार(port.in_a) > 1 && port.in_a[Get_आकार(port.in_a)-1] == State::S0)
					port.in_a.हटाओ(Get_आकार(port.in_a)-1);
				जबतक (Get_आकार(port.in_b) > 1 && port.in_b[Get_आकार(port.in_b)-1] == State::S0)
					port.in_b.हटाओ(Get_आकार(port.in_b)-1);
			}

			new_terms.धकेलो_back(port);
		}

		अगर (off.as_bool()) {
			term_t port;
			port.in_a = off;
			port.is_signed = false;
			port.do_subtract = false;
			new_terms.धकेलो_back(port);
		}

		new_terms.swap(terms);
	}

	व्योम from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec port_a = cell->getPort(ID::A);

		terms.clear();

		स्वतः config_bits = cell->getParam(ID::CONFIG);
		पूर्णांक config_cursor = 0;

		पूर्णांक config_width = cell->getParam(ID::CONFIG_WIDTH).as_int();
		log_assert(Get_आकार(config_bits) >= config_width);

		पूर्णांक अंक_bits = 0;
		अगर (config_bits[config_cursor++] == State::S1) अंक_bits |= 1;
		अगर (config_bits[config_cursor++] == State::S1) अंक_bits |= 2;
		अगर (config_bits[config_cursor++] == State::S1) अंक_bits |= 4;
		अगर (config_bits[config_cursor++] == State::S1) अंक_bits |= 8;

		पूर्णांक port_a_cursor = 0;
		जबतक (port_a_cursor < Get_आकार(port_a))
		{
			log_assert(config_cursor + 2 + 2*अंक_bits <= config_width);

			term_t this_port;
			this_port.is_signed = config_bits[config_cursor++] == State::S1;
			this_port.do_subtract = config_bits[config_cursor++] == State::S1;

			पूर्णांक आकार_a = 0;
			क्रम (पूर्णांक i = 0; i < अंक_bits; i++)
				अगर (config_bits[config_cursor++] == State::S1)
					आकार_a |= 1 << i;

			this_port.in_a = port_a.extract(port_a_cursor, आकार_a);
			port_a_cursor += आकार_a;

			पूर्णांक आकार_b = 0;
			क्रम (पूर्णांक i = 0; i < अंक_bits; i++)
				अगर (config_bits[config_cursor++] == State::S1)
					आकार_b |= 1 << i;

			this_port.in_b = port_a.extract(port_a_cursor, आकार_b);
			port_a_cursor += आकार_b;

			अगर (आकार_a || आकार_b)
				terms.धकेलो_back(this_port);
		}

		क्रम (स्वतः bit : cell->getPort(ID::B))
			terms.धकेलो_back(term_t{{bit}, {}, false, false});

		log_assert(config_cursor == config_width);
		log_assert(port_a_cursor == Get_आकार(port_a));
	}

	व्योम from_cell(RTLIL::Cell *cell)
	{
		अगर (cell->type == ID($macc)) {
			from_cell_v1(cell);
			वापस;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec port_a = cell->getPort(ID::A);
		RTLIL::SigSpec port_b = cell->getPort(ID::B);
		RTLIL::SigSpec port_c = cell->getPort(ID::C);

		terms.clear();

		पूर्णांक nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		स्थिर Const &product_neg = cell->getParam(ID::PRODUCT_NEGATED);
		स्थिर Const &a_widths = cell->getParam(ID::A_WIDTHS);
		स्थिर Const &b_widths = cell->getParam(ID::B_WIDTHS);
		स्थिर Const &a_signed = cell->getParam(ID::A_SIGNED);
		स्थिर Const &b_signed = cell->getParam(ID::B_SIGNED);
		पूर्णांक ai = 0, bi = 0;
		क्रम (पूर्णांक i = 0; i < nproducts; i++) {
			term_t term;

			log_assert(a_signed[i] == b_signed[i]);
			term.is_signed = (a_signed[i] == State::S1);
			पूर्णांक a_width = a_widths.extract(16 * i, 16).as_int(false);
			पूर्णांक b_width = b_widths.extract(16 * i, 16).as_int(false);

			term.in_a = port_a.extract(ai, a_width);
			ai += a_width;
			term.in_b = port_b.extract(bi, b_width);
			bi += b_width;
			term.do_subtract = (product_neg[i] == State::S1);

			terms.धकेलो_back(term);
		}
		log_assert(port_a.आकार() == ai);
		log_assert(port_b.आकार() == bi);

		पूर्णांक naddends = cell->getParam(ID::NADDENDS).as_int();
		स्थिर Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		स्थिर Const &c_widths = cell->getParam(ID::C_WIDTHS);
		स्थिर Const &c_signed = cell->getParam(ID::C_SIGNED);
		पूर्णांक ci = 0;
		क्रम (पूर्णांक i = 0; i < naddends; i++) {
			term_t term;

			term.is_signed = (c_signed[i] == State::S1);
			पूर्णांक c_width = c_widths.extract(16 * i, 16).as_int(false);

			term.in_a = port_c.extract(ci, c_width);
			ci += c_width;
			term.do_subtract = (addend_neg[i] == State::S1);

			terms.धकेलो_back(term);
		}
		log_assert(port_c.आकार() == ci);
	}

	व्योम to_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		पूर्णांक nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, product_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		क्रम (पूर्णांक i = 0; i < (पूर्णांक) terms.आकार(); i++) {
			SigSpec term_a = terms[i].in_a, term_b = terms[i].in_b;

			अगर (term_b.empty()) {
				// addend
				c_widths.append(Const(term_a.आकार(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_subtract ? RTLIL::S1 : RTLIL::S0);
				c.append(term_a);
				naddends++;
			} अन्यथा {
				// product
				a_widths.append(Const(term_a.आकार(), 16));
				b_widths.append(Const(term_b.आकार(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				product_negated.append(terms[i].do_subtract ? RTLIL::S1 : RTLIL::S0);
				a.append(term_a);
				b.append(term_b);
				nproducts++;
			}
		}

		अगर (a_signed.empty())
			a_signed = {RTLIL::Sx};
		अगर (b_signed.empty())
			b_signed = {RTLIL::Sx};
		अगर (c_signed.empty())
			c_signed = {RTLIL::Sx};
		अगर (a_widths.empty())
			a_widths = {RTLIL::Sx};
		अगर (b_widths.empty())
			b_widths = {RTLIL::Sx};
		अगर (c_widths.empty())
			c_widths = {RTLIL::Sx};
		अगर (product_negated.empty())
			product_negated = {RTLIL::Sx};
		अगर (addend_negated.empty())
			addend_negated = {RTLIL::Sx};

		cell->setParam(ID::NPRODUCTS, nproducts);
		cell->setParam(ID::PRODUCT_NEGATED, product_negated);
		cell->setParam(ID::NADDENDS, naddends);
		cell->setParam(ID::ADDEND_NEGATED, addend_negated);
		cell->setParam(ID::A_SIGNED, a_signed);
		cell->setParam(ID::B_SIGNED, b_signed);
		cell->setParam(ID::C_SIGNED, c_signed);
		cell->setParam(ID::A_WIDTHS, a_widths);
		cell->setParam(ID::B_WIDTHS, b_widths);
		cell->setParam(ID::C_WIDTHS, c_widths);
		cell->setPort(ID::A, a);
		cell->setPort(ID::B, b);
		cell->setPort(ID::C, c);
	}

	bool eval(RTLIL::Const &परिणाम) स्थिर
	{
		क्रम (स्वतः bit : परिणाम)
			bit = State::S0;

		क्रम (स्वतः &port : terms)
		{
			अगर (!port.in_a.is_fully_const() || !port.in_b.is_fully_const())
				वापस false;

			RTLIL::Const summand;
			अगर (Get_आकार(port.in_b) == 0)
				summand = const_pos(port.in_a.as_const(), port.in_b.as_const(), port.is_signed, port.is_signed, Get_आकार(परिणाम));
			अन्यथा
				summand = const_mul(port.in_a.as_const(), port.in_b.as_const(), port.is_signed, port.is_signed, Get_आकार(परिणाम));

			अगर (port.do_subtract)
				परिणाम = const_sub(परिणाम, summand, port.is_signed, port.is_signed, Get_आकार(परिणाम));
			अन्यथा
				परिणाम = const_add(परिणाम, summand, port.is_signed, port.is_signed, Get_आकार(परिणाम));
		}

		वापस true;
	}

	bool is_simple_product()
	{
		वापस terms.आकार() == 1 &&
				!terms[0].in_b.empty() &&
				!terms[0].do_subtract;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		अगर (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
