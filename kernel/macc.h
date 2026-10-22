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

#اگر_تعریف_نشده MACC_H
#تعریف_کن MACC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية Macc
{
	بنية جمله_t {
		RTLIL::SigSpec درون_a, درون_b;
		bool is_signed, do_تفریق;
	};
	std::وکتور<جمله_t> terms;

	فراغ optimize(صحيح عرض)
	{
		std::وکتور<جمله_t> new_terms;
		RTLIL::Const off(0, عرض);

		لكل (خودکار &پورت : terms)
		{
			إذا (Get_اندازه(پورت.درون_a) == 0 && Get_اندازه(پورت.درون_b) == 0)
				استمر;

			إذا (Get_اندازه(پورت.درون_a) < Get_اندازه(پورت.درون_b))
				std::جابجا(پورت.درون_a, پورت.درون_b);

			إذا (پورت.درون_a.is_fully_const() && پورت.درون_b.is_fully_const()) {
				RTLIL::Const v = پورت.درون_a.as_const();
				إذا (Get_اندازه(پورت.درون_b))
					v = const_mul(v, پورت.درون_b.as_const(), پورت.is_signed, پورت.is_signed, عرض);
				إذا (پورت.do_تفریق)
					off = const_sub(off, v, پورت.is_signed, پورت.is_signed, عرض);
				وإلا
					off = const_جمع_کن(off, v, پورت.is_signed, پورت.is_signed, عرض);
				استمر;
			}

			إذا (پورت.is_signed) {
				طالما (Get_اندازه(پورت.درون_a) > 1 && پورت.درون_a[Get_اندازه(پورت.درون_a)-1] == پورت.درون_a[Get_اندازه(پورت.درون_a)-2])
					پورت.درون_a.remove(Get_اندازه(پورت.درون_a)-1);
				طالما (Get_اندازه(پورت.درون_b) > 1 && پورت.درون_b[Get_اندازه(پورت.درون_b)-1] == پورت.درون_b[Get_اندازه(پورت.درون_b)-2])
					پورت.درون_b.remove(Get_اندازه(پورت.درون_b)-1);
			} وإلا {
				طالما (Get_اندازه(پورت.درون_a) > 1 && پورت.درون_a[Get_اندازه(پورت.درون_a)-1] == State::S0)
					پورت.درون_a.remove(Get_اندازه(پورت.درون_a)-1);
				طالما (Get_اندازه(پورت.درون_b) > 1 && پورت.درون_b[Get_اندازه(پورت.درون_b)-1] == State::S0)
					پورت.درون_b.remove(Get_اندازه(پورت.درون_b)-1);
			}

			new_terms.افزودن_پشت(پورت);
		}

		إذا (off.as_bool()) {
			جمله_t پورت;
			پورت.درون_a = off;
			پورت.is_signed = false;
			پورت.do_تفریق = false;
			new_terms.افزودن_پشت(پورت);
		}

		new_terms.جابجا(terms);
	}

	فراغ from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec پورت_a = cell->get_پورت(ID::A);

		terms.clear();

		خودکار config_بیتها = cell->getParam(ID::CONFIG);
		صحيح config_cursor = 0;

		صحيح config_عرض = cell->getParam(ID::CONFIG_عرض).as_int();
		log_assert(Get_اندازه(config_بیتها) >= config_عرض);

		صحيح رقم_بیتها = 0;
		إذا (config_بیتها[config_cursor++] == State::S1) رقم_بیتها |= 1;
		إذا (config_بیتها[config_cursor++] == State::S1) رقم_بیتها |= 2;
		إذا (config_بیتها[config_cursor++] == State::S1) رقم_بیتها |= 4;
		إذا (config_بیتها[config_cursor++] == State::S1) رقم_بیتها |= 8;

		صحيح پورت_a_cursor = 0;
		طالما (پورت_a_cursor < Get_اندازه(پورت_a))
		{
			log_assert(config_cursor + 2 + 2*رقم_بیتها <= config_عرض);

			جمله_t this_پورت;
			this_پورت.is_signed = config_بیتها[config_cursor++] == State::S1;
			this_پورت.do_تفریق = config_بیتها[config_cursor++] == State::S1;

			صحيح اندازه_a = 0;
			لكل (صحيح i = 0; i < رقم_بیتها; i++)
				إذا (config_بیتها[config_cursor++] == State::S1)
					اندازه_a |= 1 << i;

			this_پورت.درون_a = پورت_a.استخراج(پورت_a_cursor, اندازه_a);
			پورت_a_cursor += اندازه_a;

			صحيح اندازه_b = 0;
			لكل (صحيح i = 0; i < رقم_بیتها; i++)
				إذا (config_بیتها[config_cursor++] == State::S1)
					اندازه_b |= 1 << i;

			this_پورت.درون_b = پورت_a.استخراج(پورت_a_cursor, اندازه_b);
			پورت_a_cursor += اندازه_b;

			إذا (اندازه_a || اندازه_b)
				terms.افزودن_پشت(this_پورت);
		}

		لكل (خودکار بیت : cell->get_پورت(ID::B))
			terms.افزودن_پشت(جمله_t{{بیت}, {}, false, false});

		log_assert(config_cursor == config_عرض);
		log_assert(پورت_a_cursor == Get_اندازه(پورت_a));
	}

	فراغ from_cell(RTLIL::Cell *cell)
	{
		إذا (cell->type == ID($macc)) {
			from_cell_v1(cell);
			أرجع;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec پورت_a = cell->get_پورت(ID::A);
		RTLIL::SigSpec پورت_b = cell->get_پورت(ID::B);
		RTLIL::SigSpec پورت_c = cell->get_پورت(ID::C);

		terms.clear();

		صحيح nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ثابت Const &حاصلضرب_neg = cell->getParam(ID::حاصلضرب_NEGATED);
		ثابت Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ثابت Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ثابت Const &a_signed = cell->getParam(ID::A_SIGNED);
		ثابت Const &b_signed = cell->getParam(ID::B_SIGNED);
		صحيح ai = 0, bi = 0;
		لكل (صحيح i = 0; i < nproducts; i++) {
			جمله_t جمله;

			log_assert(a_signed[i] == b_signed[i]);
			جمله.is_signed = (a_signed[i] == State::S1);
			صحيح a_عرض = a_widths.استخراج(16 * i, 16).as_int(false);
			صحيح b_عرض = b_widths.استخراج(16 * i, 16).as_int(false);

			جمله.درون_a = پورت_a.استخراج(ai, a_عرض);
			ai += a_عرض;
			جمله.درون_b = پورت_b.استخراج(bi, b_عرض);
			bi += b_عرض;
			جمله.do_تفریق = (حاصلضرب_neg[i] == State::S1);

			terms.افزودن_پشت(جمله);
		}
		log_assert(پورت_a.اندازه() == ai);
		log_assert(پورت_b.اندازه() == bi);

		صحيح naddends = cell->getParam(ID::NADDENDS).as_int();
		ثابت Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ثابت Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ثابت Const &c_signed = cell->getParam(ID::C_SIGNED);
		صحيح ci = 0;
		لكل (صحيح i = 0; i < naddends; i++) {
			جمله_t جمله;

			جمله.is_signed = (c_signed[i] == State::S1);
			صحيح c_عرض = c_widths.استخراج(16 * i, 16).as_int(false);

			جمله.درون_a = پورت_c.استخراج(ci, c_عرض);
			ci += c_عرض;
			جمله.do_تفریق = (addend_neg[i] == State::S1);

			terms.افزودن_پشت(جمله);
		}
		log_assert(پورت_c.اندازه() == ci);
	}

	فراغ مقصد_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		صحيح nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, حاصلضرب_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		لكل (صحيح i = 0; i < (صحيح) terms.اندازه(); i++) {
			SigSpec جمله_a = terms[i].درون_a, جمله_b = terms[i].درون_b;

			إذا (جمله_b.خالی()) {
				// addend
				c_widths.append(Const(جمله_a.اندازه(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_تفریق ? RTLIL::S1 : RTLIL::S0);
				c.append(جمله_a);
				naddends++;
			} وإلا {
				//حاصلضرب
				a_widths.append(Const(جمله_a.اندازه(), 16));
				b_widths.append(Const(جمله_b.اندازه(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				حاصلضرب_negated.append(terms[i].do_تفریق ? RTLIL::S1 : RTLIL::S0);
				a.append(جمله_a);
				b.append(جمله_b);
				nproducts++;
			}
		}

		إذا (a_signed.خالی())
			a_signed = {RTLIL::Sx};
		إذا (b_signed.خالی())
			b_signed = {RTLIL::Sx};
		إذا (c_signed.خالی())
			c_signed = {RTLIL::Sx};
		إذا (a_widths.خالی())
			a_widths = {RTLIL::Sx};
		إذا (b_widths.خالی())
			b_widths = {RTLIL::Sx};
		إذا (c_widths.خالی())
			c_widths = {RTLIL::Sx};
		إذا (حاصلضرب_negated.خالی())
			حاصلضرب_negated = {RTLIL::Sx};
		إذا (addend_negated.خالی())
			addend_negated = {RTLIL::Sx};

		cell->مجموعه_Param(ID::NPRODUCTS, nproducts);
		cell->مجموعه_Param(ID::حاصلضرب_NEGATED, حاصلضرب_negated);
		cell->مجموعه_Param(ID::NADDENDS, naddends);
		cell->مجموعه_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->مجموعه_Param(ID::A_SIGNED, a_signed);
		cell->مجموعه_Param(ID::B_SIGNED, b_signed);
		cell->مجموعه_Param(ID::C_SIGNED, c_signed);
		cell->مجموعه_Param(ID::A_WIDTHS, a_widths);
		cell->مجموعه_Param(ID::B_WIDTHS, b_widths);
		cell->مجموعه_Param(ID::C_WIDTHS, c_widths);
		cell->مجموعه_پورت(ID::A, a);
		cell->مجموعه_پورت(ID::B, b);
		cell->مجموعه_پورت(ID::C, c);
	}

	bool eval(RTLIL::Const &نتیجه) ثابت
	{
		لكل (خودکار بیت : نتیجه)
			بیت = State::S0;

		لكل (خودکار &پورت : terms)
		{
			إذا (!پورت.درون_a.is_fully_const() || !پورت.درون_b.is_fully_const())
				أرجع false;

			RTLIL::Const summand;
			إذا (Get_اندازه(پورت.درون_b) == 0)
				summand = const_pos(پورت.درون_a.as_const(), پورت.درون_b.as_const(), پورت.is_signed, پورت.is_signed, Get_اندازه(نتیجه));
			وإلا
				summand = const_mul(پورت.درون_a.as_const(), پورت.درون_b.as_const(), پورت.is_signed, پورت.is_signed, Get_اندازه(نتیجه));

			إذا (پورت.do_تفریق)
				نتیجه = const_sub(نتیجه, summand, پورت.is_signed, پورت.is_signed, Get_اندازه(نتیجه));
			وإلا
				نتیجه = const_جمع_کن(نتیجه, summand, پورت.is_signed, پورت.is_signed, Get_اندازه(نتیجه));
		}

		أرجع true;
	}

	bool is_simple_حاصلضرب()
	{
		أرجع terms.اندازه() == 1 &&
				!terms[0].درون_b.خالی() &&
				!terms[0].do_تفریق;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		إذا (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_انتها

#پایان_اگر
