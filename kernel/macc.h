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

#నిర్వచితం_కాకపోతే MACC_H
#నిర్వచించు MACC_H

#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం Macc
{
	నిర్మాణం పదం_t {
		RTLIL::SigSpec లోపల_a, లోపల_b;
		bool is_signed, do_వ్యవకలనం;
	};
	std::సదిశ<పదం_t> terms;

	శూన్య optimize(పూర్ణాంకం వెడల్పు)
	{
		std::సదిశ<పదం_t> new_terms;
		RTLIL::Const off(0, వెడల్పు);

		కొరకు (స్వయంచాలక &పోర్టు : terms)
		{
			ఒకవేళ (Get_కొలత(పోర్టు.లోపల_a) == 0 && Get_కొలత(పోర్టు.లోపల_b) == 0)
				కొనసాగు;

			ఒకవేళ (Get_కొలత(పోర్టు.లోపల_a) < Get_కొలత(పోర్టు.లోపల_b))
				std::మార్పిడి(పోర్టు.లోపల_a, పోర్టు.లోపల_b);

			ఒకవేళ (పోర్టు.లోపల_a.is_fully_const() && పోర్టు.లోపల_b.is_fully_const()) {
				RTLIL::Const v = పోర్టు.లోపల_a.as_const();
				ఒకవేళ (Get_కొలత(పోర్టు.లోపల_b))
					v = const_mul(v, పోర్టు.లోపల_b.as_const(), పోర్టు.is_signed, పోర్టు.is_signed, వెడల్పు);
				ఒకవేళ (పోర్టు.do_వ్యవకలనం)
					off = const_sub(off, v, పోర్టు.is_signed, పోర్టు.is_signed, వెడల్పు);
				లేకపోతే
					off = const_కూడు(off, v, పోర్టు.is_signed, పోర్టు.is_signed, వెడల్పు);
				కొనసాగు;
			}

			ఒకవేళ (పోర్టు.is_signed) {
				అయితే (Get_కొలత(పోర్టు.లోపల_a) > 1 && పోర్టు.లోపల_a[Get_కొలత(పోర్టు.లోపల_a)-1] == పోర్టు.లోపల_a[Get_కొలత(పోర్టు.లోపల_a)-2])
					పోర్టు.లోపల_a.remove(Get_కొలత(పోర్టు.లోపల_a)-1);
				అయితే (Get_కొలత(పోర్టు.లోపల_b) > 1 && పోర్టు.లోపల_b[Get_కొలత(పోర్టు.లోపల_b)-1] == పోర్టు.లోపల_b[Get_కొలత(పోర్టు.లోపల_b)-2])
					పోర్టు.లోపల_b.remove(Get_కొలత(పోర్టు.లోపల_b)-1);
			} లేకపోతే {
				అయితే (Get_కొలత(పోర్టు.లోపల_a) > 1 && పోర్టు.లోపల_a[Get_కొలత(పోర్టు.లోపల_a)-1] == State::S0)
					పోర్టు.లోపల_a.remove(Get_కొలత(పోర్టు.లోపల_a)-1);
				అయితే (Get_కొలత(పోర్టు.లోపల_b) > 1 && పోర్టు.లోపల_b[Get_కొలత(పోర్టు.లోపల_b)-1] == State::S0)
					పోర్టు.లోపల_b.remove(Get_కొలత(పోర్టు.లోపల_b)-1);
			}

			new_terms.నెట్టు_వెనుక(పోర్టు);
		}

		ఒకవేళ (off.as_bool()) {
			పదం_t పోర్టు;
			పోర్టు.లోపల_a = off;
			పోర్టు.is_signed = false;
			పోర్టు.do_వ్యవకలనం = false;
			new_terms.నెట్టు_వెనుక(పోర్టు);
		}

		new_terms.మార్పిడి(terms);
	}

	శూన్య from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec పోర్టు_a = cell->get_పోర్టు(ID::A);

		terms.clear();

		స్వయంచాలక config_బిట్లు = cell->getParam(ID::CONFIG);
		పూర్ణాంకం config_cursor = 0;

		పూర్ణాంకం config_వెడల్పు = cell->getParam(ID::CONFIG_వెడల్పు).as_int();
		log_assert(Get_కొలత(config_బిట్లు) >= config_వెడల్పు);

		పూర్ణాంకం అంకె_బిట్లు = 0;
		ఒకవేళ (config_బిట్లు[config_cursor++] == State::S1) అంకె_బిట్లు |= 1;
		ఒకవేళ (config_బిట్లు[config_cursor++] == State::S1) అంకె_బిట్లు |= 2;
		ఒకవేళ (config_బిట్లు[config_cursor++] == State::S1) అంకె_బిట్లు |= 4;
		ఒకవేళ (config_బిట్లు[config_cursor++] == State::S1) అంకె_బిట్లు |= 8;

		పూర్ణాంకం పోర్టు_a_cursor = 0;
		అయితే (పోర్టు_a_cursor < Get_కొలత(పోర్టు_a))
		{
			log_assert(config_cursor + 2 + 2*అంకె_బిట్లు <= config_వెడల్పు);

			పదం_t this_పోర్టు;
			this_పోర్టు.is_signed = config_బిట్లు[config_cursor++] == State::S1;
			this_పోర్టు.do_వ్యవకలనం = config_బిట్లు[config_cursor++] == State::S1;

			పూర్ణాంకం కొలత_a = 0;
			కొరకు (పూర్ణాంకం i = 0; i < అంకె_బిట్లు; i++)
				ఒకవేళ (config_బిట్లు[config_cursor++] == State::S1)
					కొలత_a |= 1 << i;

			this_పోర్టు.లోపల_a = పోర్టు_a.వెలికితీయి(పోర్టు_a_cursor, కొలత_a);
			పోర్టు_a_cursor += కొలత_a;

			పూర్ణాంకం కొలత_b = 0;
			కొరకు (పూర్ణాంకం i = 0; i < అంకె_బిట్లు; i++)
				ఒకవేళ (config_బిట్లు[config_cursor++] == State::S1)
					కొలత_b |= 1 << i;

			this_పోర్టు.లోపల_b = పోర్టు_a.వెలికితీయి(పోర్టు_a_cursor, కొలత_b);
			పోర్టు_a_cursor += కొలత_b;

			ఒకవేళ (కొలత_a || కొలత_b)
				terms.నెట్టు_వెనుక(this_పోర్టు);
		}

		కొరకు (స్వయంచాలక బిట్టు : cell->get_పోర్టు(ID::B))
			terms.నెట్టు_వెనుక(పదం_t{{బిట్టు}, {}, false, false});

		log_assert(config_cursor == config_వెడల్పు);
		log_assert(పోర్టు_a_cursor == Get_కొలత(పోర్టు_a));
	}

	శూన్య from_cell(RTLIL::Cell *cell)
	{
		ఒకవేళ (cell->type == ID($macc)) {
			from_cell_v1(cell);
			తిరిగి;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec పోర్టు_a = cell->get_పోర్టు(ID::A);
		RTLIL::SigSpec పోర్టు_b = cell->get_పోర్టు(ID::B);
		RTLIL::SigSpec పోర్టు_c = cell->get_పోర్టు(ID::C);

		terms.clear();

		పూర్ణాంకం nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		స్థిరరాశి Const &లబ్ధం_neg = cell->getParam(ID::లబ్ధం_NEGATED);
		స్థిరరాశి Const &a_widths = cell->getParam(ID::A_WIDTHS);
		స్థిరరాశి Const &b_widths = cell->getParam(ID::B_WIDTHS);
		స్థిరరాశి Const &a_signed = cell->getParam(ID::A_SIGNED);
		స్థిరరాశి Const &b_signed = cell->getParam(ID::B_SIGNED);
		పూర్ణాంకం ai = 0, bi = 0;
		కొరకు (పూర్ణాంకం i = 0; i < nproducts; i++) {
			పదం_t పదం;

			log_assert(a_signed[i] == b_signed[i]);
			పదం.is_signed = (a_signed[i] == State::S1);
			పూర్ణాంకం a_వెడల్పు = a_widths.వెలికితీయి(16 * i, 16).as_int(false);
			పూర్ణాంకం b_వెడల్పు = b_widths.వెలికితీయి(16 * i, 16).as_int(false);

			పదం.లోపల_a = పోర్టు_a.వెలికితీయి(ai, a_వెడల్పు);
			ai += a_వెడల్పు;
			పదం.లోపల_b = పోర్టు_b.వెలికితీయి(bi, b_వెడల్పు);
			bi += b_వెడల్పు;
			పదం.do_వ్యవకలనం = (లబ్ధం_neg[i] == State::S1);

			terms.నెట్టు_వెనుక(పదం);
		}
		log_assert(పోర్టు_a.కొలత() == ai);
		log_assert(పోర్టు_b.కొలత() == bi);

		పూర్ణాంకం naddends = cell->getParam(ID::NADDENDS).as_int();
		స్థిరరాశి Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		స్థిరరాశి Const &c_widths = cell->getParam(ID::C_WIDTHS);
		స్థిరరాశి Const &c_signed = cell->getParam(ID::C_SIGNED);
		పూర్ణాంకం ci = 0;
		కొరకు (పూర్ణాంకం i = 0; i < naddends; i++) {
			పదం_t పదం;

			పదం.is_signed = (c_signed[i] == State::S1);
			పూర్ణాంకం c_వెడల్పు = c_widths.వెలికితీయి(16 * i, 16).as_int(false);

			పదం.లోపల_a = పోర్టు_c.వెలికితీయి(ci, c_వెడల్పు);
			ci += c_వెడల్పు;
			పదం.do_వ్యవకలనం = (addend_neg[i] == State::S1);

			terms.నెట్టు_వెనుక(పదం);
		}
		log_assert(పోర్టు_c.కొలత() == ci);
	}

	శూన్య గమ్యం_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		పూర్ణాంకం nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, లబ్ధం_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం) terms.కొలత(); i++) {
			SigSpec పదం_a = terms[i].లోపల_a, పదం_b = terms[i].లోపల_b;

			ఒకవేళ (పదం_b.ఖాళీలు()) {
				// addend
				c_widths.append(Const(పదం_a.కొలత(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_వ్యవకలనం ? RTLIL::S1 : RTLIL::S0);
				c.append(పదం_a);
				naddends++;
			} లేకపోతే {
				//లబ్ధం
				a_widths.append(Const(పదం_a.కొలత(), 16));
				b_widths.append(Const(పదం_b.కొలత(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				లబ్ధం_negated.append(terms[i].do_వ్యవకలనం ? RTLIL::S1 : RTLIL::S0);
				a.append(పదం_a);
				b.append(పదం_b);
				nproducts++;
			}
		}

		ఒకవేళ (a_signed.ఖాళీలు())
			a_signed = {RTLIL::Sx};
		ఒకవేళ (b_signed.ఖాళీలు())
			b_signed = {RTLIL::Sx};
		ఒకవేళ (c_signed.ఖాళీలు())
			c_signed = {RTLIL::Sx};
		ఒకవేళ (a_widths.ఖాళీలు())
			a_widths = {RTLIL::Sx};
		ఒకవేళ (b_widths.ఖాళీలు())
			b_widths = {RTLIL::Sx};
		ఒకవేళ (c_widths.ఖాళీలు())
			c_widths = {RTLIL::Sx};
		ఒకవేళ (లబ్ధం_negated.ఖాళీలు())
			లబ్ధం_negated = {RTLIL::Sx};
		ఒకవేళ (addend_negated.ఖాళీలు())
			addend_negated = {RTLIL::Sx};

		cell->సమితి_Param(ID::NPRODUCTS, nproducts);
		cell->సమితి_Param(ID::లబ్ధం_NEGATED, లబ్ధం_negated);
		cell->సమితి_Param(ID::NADDENDS, naddends);
		cell->సమితి_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->సమితి_Param(ID::A_SIGNED, a_signed);
		cell->సమితి_Param(ID::B_SIGNED, b_signed);
		cell->సమితి_Param(ID::C_SIGNED, c_signed);
		cell->సమితి_Param(ID::A_WIDTHS, a_widths);
		cell->సమితి_Param(ID::B_WIDTHS, b_widths);
		cell->సమితి_Param(ID::C_WIDTHS, c_widths);
		cell->సమితి_పోర్టు(ID::A, a);
		cell->సమితి_పోర్టు(ID::B, b);
		cell->సమితి_పోర్టు(ID::C, c);
	}

	bool eval(RTLIL::Const &ఫలితం) స్థిరరాశి
	{
		కొరకు (స్వయంచాలక బిట్టు : ఫలితం)
			బిట్టు = State::S0;

		కొరకు (స్వయంచాలక &పోర్టు : terms)
		{
			ఒకవేళ (!పోర్టు.లోపల_a.is_fully_const() || !పోర్టు.లోపల_b.is_fully_const())
				తిరిగి false;

			RTLIL::Const summand;
			ఒకవేళ (Get_కొలత(పోర్టు.లోపల_b) == 0)
				summand = const_pos(పోర్టు.లోపల_a.as_const(), పోర్టు.లోపల_b.as_const(), పోర్టు.is_signed, పోర్టు.is_signed, Get_కొలత(ఫలితం));
			లేకపోతే
				summand = const_mul(పోర్టు.లోపల_a.as_const(), పోర్టు.లోపల_b.as_const(), పోర్టు.is_signed, పోర్టు.is_signed, Get_కొలత(ఫలితం));

			ఒకవేళ (పోర్టు.do_వ్యవకలనం)
				ఫలితం = const_sub(ఫలితం, summand, పోర్టు.is_signed, పోర్టు.is_signed, Get_కొలత(ఫలితం));
			లేకపోతే
				ఫలితం = const_కూడు(ఫలితం, summand, పోర్టు.is_signed, పోర్టు.is_signed, Get_కొలత(ఫలితం));
		}

		తిరిగి true;
	}

	bool is_simple_లబ్ధం()
	{
		తిరిగి terms.కొలత() == 1 &&
				!terms[0].లోపల_b.ఖాళీలు() &&
				!terms[0].do_వ్యవకలనం;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		ఒకవేళ (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
