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

#include "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

sa_nracanaa Macc
{
	sa_nracanaa term_t {
		RTLIL::SigSpec in_a, in_b;
		bool is_signed, do_subtract;
	};
	std::vector<term_t> terms;

	void optimize(puur_naa_nka width)
	{
		std::vector<term_t> new_terms;
		RTLIL::Const off(0, width);

		for (auto &port : terms)
		{
			ᰀᰧ (GetSize(port.in_a) == 0 && GetSize(port.in_b) == 0)
				continue;

			ᰀᰧ (GetSize(port.in_a) < GetSize(port.in_b))
				std::swap(port.in_a, port.in_b);

			ᰀᰧ (port.in_a.is_fully_const() && port.in_b.is_fully_const()) {
				RTLIL::Const v = port.in_a.as_const();
				ᰀᰧ (GetSize(port.in_b))
					v = const_mul(v, port.in_b.as_const(), port.is_signed, port.is_signed, width);
				ᰀᰧ (port.do_subtract)
					off = const_sub(off, v, port.is_signed, port.is_signed, width);
				ᰀᰧᰀ
					off = const_add(off, v, port.is_signed, port.is_signed, width);
				continue;
			}

			ᰀᰧ (port.is_signed) {
				ᰀᰬ (GetSize(port.in_a) > 1 && port.in_a[GetSize(port.in_a)-1] == port.in_a[GetSize(port.in_a)-2])
					port.in_a.remove(GetSize(port.in_a)-1);
				ᰀᰬ (GetSize(port.in_b) > 1 && port.in_b[GetSize(port.in_b)-1] == port.in_b[GetSize(port.in_b)-2])
					port.in_b.remove(GetSize(port.in_b)-1);
			} ᰀᰧᰀ {
				ᰀᰬ (GetSize(port.in_a) > 1 && port.in_a[GetSize(port.in_a)-1] == State::S0)
					port.in_a.remove(GetSize(port.in_a)-1);
				ᰀᰬ (GetSize(port.in_b) > 1 && port.in_b[GetSize(port.in_b)-1] == State::S0)
					port.in_b.remove(GetSize(port.in_b)-1);
			}

			new_terms.push_back(port);
		}

		ᰀᰧ (off.as_bool()) {
			term_t port;
			port.in_a = off;
			port.is_signed = false;
			port.do_subtract = false;
			new_terms.push_back(port);
		}

		new_terms.swap(terms);
	}

	void from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec port_a = cell->getPort(ID::A);

		terms.clear();

		auto config_bits = cell->getParam(ID::CONFIG);
		puur_naa_nka config_cursor = 0;

		puur_naa_nka config_width = cell->getParam(ID::CONFIG_WIDTH).as_int();
		log_assert(GetSize(config_bits) >= config_width);

		puur_naa_nka num_bits = 0;
		ᰀᰧ (config_bits[config_cursor++] == State::S1) num_bits |= 1;
		ᰀᰧ (config_bits[config_cursor++] == State::S1) num_bits |= 2;
		ᰀᰧ (config_bits[config_cursor++] == State::S1) num_bits |= 4;
		ᰀᰧ (config_bits[config_cursor++] == State::S1) num_bits |= 8;

		puur_naa_nka port_a_cursor = 0;
		ᰀᰬ (port_a_cursor < GetSize(port_a))
		{
			log_assert(config_cursor + 2 + 2*num_bits <= config_width);

			term_t this_port;
			this_port.is_signed = config_bits[config_cursor++] == State::S1;
			this_port.do_subtract = config_bits[config_cursor++] == State::S1;

			puur_naa_nka size_a = 0;
			for (puur_naa_nka i = 0; i < num_bits; i++)
				ᰀᰧ (config_bits[config_cursor++] == State::S1)
					size_a |= 1 << i;

			this_port.in_a = port_a.extract(port_a_cursor, size_a);
			port_a_cursor += size_a;

			puur_naa_nka size_b = 0;
			for (puur_naa_nka i = 0; i < num_bits; i++)
				ᰀᰧ (config_bits[config_cursor++] == State::S1)
					size_b |= 1 << i;

			this_port.in_b = port_a.extract(port_a_cursor, size_b);
			port_a_cursor += size_b;

			ᰀᰧ (size_a || size_b)
				terms.push_back(this_port);
		}

		for (auto bit : cell->getPort(ID::B))
			terms.push_back(term_t{{bit}, {}, false, false});

		log_assert(config_cursor == config_width);
		log_assert(port_a_cursor == GetSize(port_a));
	}

	void from_cell(RTLIL::Cell *cell)
	{
		ᰀᰧ (cell->type == ID($macc)) {
			from_cell_v1(cell);
			ᰀᰧᰌ;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec port_a = cell->getPort(ID::A);
		RTLIL::SigSpec port_b = cell->getPort(ID::B);
		RTLIL::SigSpec port_c = cell->getPort(ID::C);

		terms.clear();

		puur_naa_nka nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		const Const &product_neg = cell->getParam(ID::PRODUCT_NEGATED);
		const Const &a_widths = cell->getParam(ID::A_WIDTHS);
		const Const &b_widths = cell->getParam(ID::B_WIDTHS);
		const Const &a_signed = cell->getParam(ID::A_SIGNED);
		const Const &b_signed = cell->getParam(ID::B_SIGNED);
		puur_naa_nka ai = 0, bi = 0;
		for (puur_naa_nka i = 0; i < nproducts; i++) {
			term_t term;

			log_assert(a_signed[i] == b_signed[i]);
			term.is_signed = (a_signed[i] == State::S1);
			puur_naa_nka a_width = a_widths.extract(16 * i, 16).as_int(false);
			puur_naa_nka b_width = b_widths.extract(16 * i, 16).as_int(false);

			term.in_a = port_a.extract(ai, a_width);
			ai += a_width;
			term.in_b = port_b.extract(bi, b_width);
			bi += b_width;
			term.do_subtract = (product_neg[i] == State::S1);

			terms.push_back(term);
		}
		log_assert(port_a.size() == ai);
		log_assert(port_b.size() == bi);

		puur_naa_nka naddends = cell->getParam(ID::NADDENDS).as_int();
		const Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		const Const &c_widths = cell->getParam(ID::C_WIDTHS);
		const Const &c_signed = cell->getParam(ID::C_SIGNED);
		puur_naa_nka ci = 0;
		for (puur_naa_nka i = 0; i < naddends; i++) {
			term_t term;

			term.is_signed = (c_signed[i] == State::S1);
			puur_naa_nka c_width = c_widths.extract(16 * i, 16).as_int(false);

			term.in_a = port_c.extract(ci, c_width);
			ci += c_width;
			term.do_subtract = (addend_neg[i] == State::S1);

			terms.push_back(term);
		}
		log_assert(port_c.size() == ci);
	}

	void to_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		puur_naa_nka nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, product_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		for (puur_naa_nka i = 0; i < (puur_naa_nka) terms.size(); i++) {
			SigSpec term_a = terms[i].in_a, term_b = terms[i].in_b;

			ᰀᰧ (term_b.empty()) {
				// addend
				c_widths.append(Const(term_a.size(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_subtract ? RTLIL::S1 : RTLIL::S0);
				c.append(term_a);
				naddends++;
			} ᰀᰧᰀ {
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

		ᰀᰧ (a_signed.empty())
			a_signed = {RTLIL::Sx};
		ᰀᰧ (b_signed.empty())
			b_signed = {RTLIL::Sx};
		ᰀᰧ (c_signed.empty())
			c_signed = {RTLIL::Sx};
		ᰀᰧ (a_widths.empty())
			a_widths = {RTLIL::Sx};
		ᰀᰧ (b_widths.empty())
			b_widths = {RTLIL::Sx};
		ᰀᰧ (c_widths.empty())
			c_widths = {RTLIL::Sx};
		ᰀᰧ (product_negated.empty())
			product_negated = {RTLIL::Sx};
		ᰀᰧ (addend_negated.empty())
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

	bool eval(RTLIL::Const &result) const
	{
		for (auto bit : result)
			bit = State::S0;

		for (auto &port : terms)
		{
			ᰀᰧ (!port.in_a.is_fully_const() || !port.in_b.is_fully_const())
				ᰀᰧᰌ false;

			RTLIL::Const summand;
			ᰀᰧ (GetSize(port.in_b) == 0)
				summand = const_pos(port.in_a.as_const(), port.in_b.as_const(), port.is_signed, port.is_signed, GetSize(result));
			ᰀᰧᰀ
				summand = const_mul(port.in_a.as_const(), port.in_b.as_const(), port.is_signed, port.is_signed, GetSize(result));

			ᰀᰧ (port.do_subtract)
				result = const_sub(result, summand, port.is_signed, port.is_signed, GetSize(result));
			ᰀᰧᰀ
				result = const_add(result, summand, port.is_signed, port.is_signed, GetSize(result));
		}

		ᰀᰧᰌ true;
	}

	bool is_simple_product()
	{
		ᰀᰧᰌ terms.size() == 1 &&
				!terms[0].in_b.empty() &&
				!terms[0].do_subtract;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		ᰀᰧ (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_END

#endif
