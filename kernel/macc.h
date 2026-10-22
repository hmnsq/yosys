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

#که_نه_تعریف_شوی MACC_H
#تعریف_کړه MACC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية Macc
{
	بنية غړی_t {
		RTLIL::SigSpec دننه_a, دننه_b;
		bool is_signed, do_کمول;
	};
	std::ویکټور<غړی_t> terms;

	فراغ optimize(صحيح پلنوالی)
	{
		std::ویکټور<غړی_t> new_terms;
		RTLIL::Const off(0, پلنوالی);

		لكل (خپلکاری &پورټ : terms)
		{
			إذا (Get_اندازه(پورټ.دننه_a) == 0 && Get_اندازه(پورټ.دننه_b) == 0)
				استمر;

			إذا (Get_اندازه(پورټ.دننه_a) < Get_اندازه(پورټ.دننه_b))
				std::بدلون(پورټ.دننه_a, پورټ.دننه_b);

			إذا (پورټ.دننه_a.is_fully_const() && پورټ.دننه_b.is_fully_const()) {
				RTLIL::Const v = پورټ.دننه_a.as_const();
				إذا (Get_اندازه(پورټ.دننه_b))
					v = const_mul(v, پورټ.دننه_b.as_const(), پورټ.is_signed, پورټ.is_signed, پلنوالی);
				إذا (پورټ.do_کمول)
					off = const_sub(off, v, پورټ.is_signed, پورټ.is_signed, پلنوالی);
				وإلا
					off = const_جمع_کړه(off, v, پورټ.is_signed, پورټ.is_signed, پلنوالی);
				استمر;
			}

			إذا (پورټ.is_signed) {
				طالما (Get_اندازه(پورټ.دننه_a) > 1 && پورټ.دننه_a[Get_اندازه(پورټ.دننه_a)-1] == پورټ.دننه_a[Get_اندازه(پورټ.دننه_a)-2])
					پورټ.دننه_a.remove(Get_اندازه(پورټ.دننه_a)-1);
				طالما (Get_اندازه(پورټ.دننه_b) > 1 && پورټ.دننه_b[Get_اندازه(پورټ.دننه_b)-1] == پورټ.دننه_b[Get_اندازه(پورټ.دننه_b)-2])
					پورټ.دننه_b.remove(Get_اندازه(پورټ.دننه_b)-1);
			} وإلا {
				طالما (Get_اندازه(پورټ.دننه_a) > 1 && پورټ.دننه_a[Get_اندازه(پورټ.دننه_a)-1] == State::S0)
					پورټ.دننه_a.remove(Get_اندازه(پورټ.دننه_a)-1);
				طالما (Get_اندازه(پورټ.دننه_b) > 1 && پورټ.دننه_b[Get_اندازه(پورټ.دننه_b)-1] == State::S0)
					پورټ.دننه_b.remove(Get_اندازه(پورټ.دننه_b)-1);
			}

			new_terms.ټېل_شا(پورټ);
		}

		إذا (off.as_bool()) {
			غړی_t پورټ;
			پورټ.دننه_a = off;
			پورټ.is_signed = false;
			پورټ.do_کمول = false;
			new_terms.ټېل_شا(پورټ);
		}

		new_terms.بدلون(terms);
	}

	فراغ from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec پورټ_a = cell->get_پورټ(ID::A);

		terms.clear();

		خپلکاری config_بټونه = cell->getParam(ID::CONFIG);
		صحيح config_cursor = 0;

		صحيح config_پلنوالی = cell->getParam(ID::CONFIG_پلنوالی).as_int();
		log_assert(Get_اندازه(config_بټونه) >= config_پلنوالی);

		صحيح عدد_بټونه = 0;
		إذا (config_بټونه[config_cursor++] == State::S1) عدد_بټونه |= 1;
		إذا (config_بټونه[config_cursor++] == State::S1) عدد_بټونه |= 2;
		إذا (config_بټونه[config_cursor++] == State::S1) عدد_بټونه |= 4;
		إذا (config_بټونه[config_cursor++] == State::S1) عدد_بټونه |= 8;

		صحيح پورټ_a_cursor = 0;
		طالما (پورټ_a_cursor < Get_اندازه(پورټ_a))
		{
			log_assert(config_cursor + 2 + 2*عدد_بټونه <= config_پلنوالی);

			غړی_t this_پورټ;
			this_پورټ.is_signed = config_بټونه[config_cursor++] == State::S1;
			this_پورټ.do_کمول = config_بټونه[config_cursor++] == State::S1;

			صحيح اندازه_a = 0;
			لكل (صحيح i = 0; i < عدد_بټونه; i++)
				إذا (config_بټونه[config_cursor++] == State::S1)
					اندازه_a |= 1 << i;

			this_پورټ.دننه_a = پورټ_a.ایستل(پورټ_a_cursor, اندازه_a);
			پورټ_a_cursor += اندازه_a;

			صحيح اندازه_b = 0;
			لكل (صحيح i = 0; i < عدد_بټونه; i++)
				إذا (config_بټونه[config_cursor++] == State::S1)
					اندازه_b |= 1 << i;

			this_پورټ.دننه_b = پورټ_a.ایستل(پورټ_a_cursor, اندازه_b);
			پورټ_a_cursor += اندازه_b;

			إذا (اندازه_a || اندازه_b)
				terms.ټېل_شا(this_پورټ);
		}

		لكل (خپلکاری بټ : cell->get_پورټ(ID::B))
			terms.ټېل_شا(غړی_t{{بټ}, {}, false, false});

		log_assert(config_cursor == config_پلنوالی);
		log_assert(پورټ_a_cursor == Get_اندازه(پورټ_a));
	}

	فراغ from_cell(RTLIL::Cell *cell)
	{
		إذا (cell->type == ID($macc)) {
			from_cell_v1(cell);
			أرجع;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec پورټ_a = cell->get_پورټ(ID::A);
		RTLIL::SigSpec پورټ_b = cell->get_پورټ(ID::B);
		RTLIL::SigSpec پورټ_c = cell->get_پورټ(ID::C);

		terms.clear();

		صحيح nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ثابت Const &حاصل_ضرب_neg = cell->getParam(ID::حاصل_ضرب_NEGATED);
		ثابت Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ثابت Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ثابت Const &a_signed = cell->getParam(ID::A_SIGNED);
		ثابت Const &b_signed = cell->getParam(ID::B_SIGNED);
		صحيح ai = 0, bi = 0;
		لكل (صحيح i = 0; i < nproducts; i++) {
			غړی_t غړی;

			log_assert(a_signed[i] == b_signed[i]);
			غړی.is_signed = (a_signed[i] == State::S1);
			صحيح a_پلنوالی = a_widths.ایستل(16 * i, 16).as_int(false);
			صحيح b_پلنوالی = b_widths.ایستل(16 * i, 16).as_int(false);

			غړی.دننه_a = پورټ_a.ایستل(ai, a_پلنوالی);
			ai += a_پلنوالی;
			غړی.دننه_b = پورټ_b.ایستل(bi, b_پلنوالی);
			bi += b_پلنوالی;
			غړی.do_کمول = (حاصل_ضرب_neg[i] == State::S1);

			terms.ټېل_شا(غړی);
		}
		log_assert(پورټ_a.اندازه() == ai);
		log_assert(پورټ_b.اندازه() == bi);

		صحيح naddends = cell->getParam(ID::NADDENDS).as_int();
		ثابت Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ثابت Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ثابت Const &c_signed = cell->getParam(ID::C_SIGNED);
		صحيح ci = 0;
		لكل (صحيح i = 0; i < naddends; i++) {
			غړی_t غړی;

			غړی.is_signed = (c_signed[i] == State::S1);
			صحيح c_پلنوالی = c_widths.ایستل(16 * i, 16).as_int(false);

			غړی.دننه_a = پورټ_c.ایستل(ci, c_پلنوالی);
			ci += c_پلنوالی;
			غړی.do_کمول = (addend_neg[i] == State::S1);

			terms.ټېل_شا(غړی);
		}
		log_assert(پورټ_c.اندازه() == ci);
	}

	فراغ منزل_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		صحيح nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, حاصل_ضرب_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		لكل (صحيح i = 0; i < (صحيح) terms.اندازه(); i++) {
			SigSpec غړی_a = terms[i].دننه_a, غړی_b = terms[i].دننه_b;

			إذا (غړی_b.تشه()) {
				// addend
				c_widths.append(Const(غړی_a.اندازه(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_کمول ? RTLIL::S1 : RTLIL::S0);
				c.append(غړی_a);
				naddends++;
			} وإلا {
				//حاصل_ضرب
				a_widths.append(Const(غړی_a.اندازه(), 16));
				b_widths.append(Const(غړی_b.اندازه(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				حاصل_ضرب_negated.append(terms[i].do_کمول ? RTLIL::S1 : RTLIL::S0);
				a.append(غړی_a);
				b.append(غړی_b);
				nproducts++;
			}
		}

		إذا (a_signed.تشه())
			a_signed = {RTLIL::Sx};
		إذا (b_signed.تشه())
			b_signed = {RTLIL::Sx};
		إذا (c_signed.تشه())
			c_signed = {RTLIL::Sx};
		إذا (a_widths.تشه())
			a_widths = {RTLIL::Sx};
		إذا (b_widths.تشه())
			b_widths = {RTLIL::Sx};
		إذا (c_widths.تشه())
			c_widths = {RTLIL::Sx};
		إذا (حاصل_ضرب_negated.تشه())
			حاصل_ضرب_negated = {RTLIL::Sx};
		إذا (addend_negated.تشه())
			addend_negated = {RTLIL::Sx};

		cell->سېټ_Param(ID::NPRODUCTS, nproducts);
		cell->سېټ_Param(ID::حاصل_ضرب_NEGATED, حاصل_ضرب_negated);
		cell->سېټ_Param(ID::NADDENDS, naddends);
		cell->سېټ_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->سېټ_Param(ID::A_SIGNED, a_signed);
		cell->سېټ_Param(ID::B_SIGNED, b_signed);
		cell->سېټ_Param(ID::C_SIGNED, c_signed);
		cell->سېټ_Param(ID::A_WIDTHS, a_widths);
		cell->سېټ_Param(ID::B_WIDTHS, b_widths);
		cell->سېټ_Param(ID::C_WIDTHS, c_widths);
		cell->سېټ_پورټ(ID::A, a);
		cell->سېټ_پورټ(ID::B, b);
		cell->سېټ_پورټ(ID::C, c);
	}

	bool eval(RTLIL::Const &پایله) ثابت
	{
		لكل (خپلکاری بټ : پایله)
			بټ = State::S0;

		لكل (خپلکاری &پورټ : terms)
		{
			إذا (!پورټ.دننه_a.is_fully_const() || !پورټ.دننه_b.is_fully_const())
				أرجع false;

			RTLIL::Const summand;
			إذا (Get_اندازه(پورټ.دننه_b) == 0)
				summand = const_pos(پورټ.دننه_a.as_const(), پورټ.دننه_b.as_const(), پورټ.is_signed, پورټ.is_signed, Get_اندازه(پایله));
			وإلا
				summand = const_mul(پورټ.دننه_a.as_const(), پورټ.دننه_b.as_const(), پورټ.is_signed, پورټ.is_signed, Get_اندازه(پایله));

			إذا (پورټ.do_کمول)
				پایله = const_sub(پایله, summand, پورټ.is_signed, پورټ.is_signed, Get_اندازه(پایله));
			وإلا
				پایله = const_جمع_کړه(پایله, summand, پورټ.is_signed, پورټ.is_signed, Get_اندازه(پایله));
		}

		أرجع true;
	}

	bool is_simple_حاصل_ضرب()
	{
		أرجع terms.اندازه() == 1 &&
				!terms[0].دننه_b.تشه() &&
				!terms[0].do_کمول;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		إذا (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
