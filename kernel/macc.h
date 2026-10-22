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

#ifndef MACC_H
#define MACC_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساختار Macc
{
	ساختار term_t {
		RTLIL::SigSpec in_a, in_b;
		bool is_signed, do_subtract;
	};
	std::vector<term_t> terms;

	تهی optimize(صحیح width)
	{
		std::vector<term_t> new_terms;
		RTLIL::Const off(0, width);

		برای (auto &port : terms)
		{
			اگر (GetSize(port.in_a) == 0 && GetSize(port.in_b) == 0)
				ادامه;

			اگر (GetSize(port.in_a) < GetSize(port.in_b))
				std::swap(port.in_a, port.in_b);

			اگر (port.in_a.is_fully_const() && port.in_b.is_fully_const()) {
				RTLIL::Const v = port.in_a.as_const();
				اگر (GetSize(port.in_b))
					v = const_mul(v, port.in_b.as_const(), port.is_signed, port.is_signed, width);
				اگر (port.do_subtract)
					off = const_sub(off, v, port.is_signed, port.is_signed, width);
				وگرنه
					off = const_add(off, v, port.is_signed, port.is_signed, width);
				ادامه;
			}

			اگر (port.is_signed) {
				وقتی (GetSize(port.in_a) > 1 && port.in_a[GetSize(port.in_a)-1] == port.in_a[GetSize(port.in_a)-2])
					port.in_a.remove(GetSize(port.in_a)-1);
				وقتی (GetSize(port.in_b) > 1 && port.in_b[GetSize(port.in_b)-1] == port.in_b[GetSize(port.in_b)-2])
					port.in_b.remove(GetSize(port.in_b)-1);
			} وگرنه {
				وقتی (GetSize(port.in_a) > 1 && port.in_a[GetSize(port.in_a)-1] == State::S0)
					port.in_a.remove(GetSize(port.in_a)-1);
				وقتی (GetSize(port.in_b) > 1 && port.in_b[GetSize(port.in_b)-1] == State::S0)
					port.in_b.remove(GetSize(port.in_b)-1);
			}

			new_terms.افزودن_back(port);
		}

		اگر (off.as_bool()) {
			term_t port;
			port.in_a = off;
			port.is_signed = false;
			port.do_subtract = false;
			new_terms.افزودن_back(port);
		}

		new_terms.swap(terms);
	}

	تهی from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec port_a = cell->getPort(ID::A);

		terms.clear();

		auto config_bits = cell->getParam(ID::CONFIG);
		صحیح config_cursor = 0;

		صحیح config_width = cell->getParam(ID::CONFIG_WIDTH).as_int();
		log_assert(GetSize(config_bits) >= config_width);

		صحیح رقم_bits = 0;
		اگر (config_bits[config_cursor++] == State::S1) رقم_bits |= 1;
		اگر (config_bits[config_cursor++] == State::S1) رقم_bits |= 2;
		اگر (config_bits[config_cursor++] == State::S1) رقم_bits |= 4;
		اگر (config_bits[config_cursor++] == State::S1) رقم_bits |= 8;

		صحیح port_a_cursor = 0;
		وقتی (port_a_cursor < GetSize(port_a))
		{
			log_assert(config_cursor + 2 + 2*رقم_bits <= config_width);

			term_t this_port;
			this_port.is_signed = config_bits[config_cursor++] == State::S1;
			this_port.do_subtract = config_bits[config_cursor++] == State::S1;

			صحیح size_a = 0;
			برای (صحیح i = 0; i < رقم_bits; i++)
				اگر (config_bits[config_cursor++] == State::S1)
					size_a |= 1 << i;

			this_port.in_a = port_a.extract(port_a_cursor, size_a);
			port_a_cursor += size_a;

			صحیح size_b = 0;
			برای (صحیح i = 0; i < رقم_bits; i++)
				اگر (config_bits[config_cursor++] == State::S1)
					size_b |= 1 << i;

			this_port.in_b = port_a.extract(port_a_cursor, size_b);
			port_a_cursor += size_b;

			اگر (size_a || size_b)
				terms.افزودن_back(this_port);
		}

		برای (auto bit : cell->getPort(ID::B))
			terms.افزودن_back(term_t{{bit}, {}, false, false});

		log_assert(config_cursor == config_width);
		log_assert(port_a_cursor == GetSize(port_a));
	}

	تهی from_cell(RTLIL::Cell *cell)
	{
		اگر (cell->type == ID($macc)) {
			from_cell_v1(cell);
			بازگشت;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec port_a = cell->getPort(ID::A);
		RTLIL::SigSpec port_b = cell->getPort(ID::B);
		RTLIL::SigSpec port_c = cell->getPort(ID::C);

		terms.clear();

		صحیح nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ثابت Const &product_neg = cell->getParam(ID::PRODUCT_NEGATED);
		ثابت Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ثابت Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ثابت Const &a_signed = cell->getParam(ID::A_SIGNED);
		ثابت Const &b_signed = cell->getParam(ID::B_SIGNED);
		صحیح ai = 0, bi = 0;
		برای (صحیح i = 0; i < nproducts; i++) {
			term_t term;

			log_assert(a_signed[i] == b_signed[i]);
			term.is_signed = (a_signed[i] == State::S1);
			صحیح a_width = a_widths.extract(16 * i, 16).as_int(false);
			صحیح b_width = b_widths.extract(16 * i, 16).as_int(false);

			term.in_a = port_a.extract(ai, a_width);
			ai += a_width;
			term.in_b = port_b.extract(bi, b_width);
			bi += b_width;
			term.do_subtract = (product_neg[i] == State::S1);

			terms.افزودن_back(term);
		}
		log_assert(port_a.size() == ai);
		log_assert(port_b.size() == bi);

		صحیح naddends = cell->getParam(ID::NADDENDS).as_int();
		ثابت Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ثابت Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ثابت Const &c_signed = cell->getParam(ID::C_SIGNED);
		صحیح ci = 0;
		برای (صحیح i = 0; i < naddends; i++) {
			term_t term;

			term.is_signed = (c_signed[i] == State::S1);
			صحیح c_width = c_widths.extract(16 * i, 16).as_int(false);

			term.in_a = port_c.extract(ci, c_width);
			ci += c_width;
			term.do_subtract = (addend_neg[i] == State::S1);

			terms.افزودن_back(term);
		}
		log_assert(port_c.size() == ci);
	}

	تهی to_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		صحیح nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, product_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		برای (صحیح i = 0; i < (صحیح) terms.size(); i++) {
			SigSpec term_a = terms[i].in_a, term_b = terms[i].in_b;

			اگر (term_b.empty()) {
				// addend
				c_widths.append(Const(term_a.size(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_subtract ? RTLIL::S1 : RTLIL::S0);
				c.append(term_a);
				naddends++;
			} وگرنه {
				// product
				a_widths.append(Const(term_a.size(), 16));
				b_widths.append(Const(term_b.size(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				product_negated.append(terms[i].do_subtract ? RTLIL::S1 : RTLIL::S0);
				a.append(term_a);
				b.append(term_b);
				nproducts++;
			}
		}

		اگر (a_signed.empty())
			a_signed = {RTLIL::Sx};
		اگر (b_signed.empty())
			b_signed = {RTLIL::Sx};
		اگر (c_signed.empty())
			c_signed = {RTLIL::Sx};
		اگر (a_widths.empty())
			a_widths = {RTLIL::Sx};
		اگر (b_widths.empty())
			b_widths = {RTLIL::Sx};
		اگر (c_widths.empty())
			c_widths = {RTLIL::Sx};
		اگر (product_negated.empty())
			product_negated = {RTLIL::Sx};
		اگر (addend_negated.empty())
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

	bool eval(RTLIL::Const &result) ثابت
	{
		برای (auto bit : result)
			bit = State::S0;

		برای (auto &port : terms)
		{
			اگر (!port.in_a.is_fully_const() || !port.in_b.is_fully_const())
				بازگشت false;

			RTLIL::Const summand;
			اگر (GetSize(port.in_b) == 0)
				summand = const_pos(port.in_a.as_const(), port.in_b.as_const(), port.is_signed, port.is_signed, GetSize(result));
			وگرنه
				summand = const_mul(port.in_a.as_const(), port.in_b.as_const(), port.is_signed, port.is_signed, GetSize(result));

			اگر (port.do_subtract)
				result = const_sub(result, summand, port.is_signed, port.is_signed, GetSize(result));
			وگرنه
				result = const_add(result, summand, port.is_signed, port.is_signed, GetSize(result));
		}

		بازگشت true;
	}

	bool is_simple_product()
	{
		بازگشت terms.size() == 1 &&
				!terms[0].in_b.empty() &&
				!terms[0].do_subtract;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		اگر (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_END

#endif
