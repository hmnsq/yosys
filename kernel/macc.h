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

#جيڪڏهن_وصف_نه_ٿيل MACC_H
#وصف_ڪريو MACC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية Macc
{
	بنية رڪن_t {
		RTLIL::SigSpec اندريون_a, اندريون_b;
		bool is_signed, do_ڪٽيو;
	};
	std::ويڪٽر<رڪن_t> terms;

	فراغ optimize(صحيح ويڪر)
	{
		std::ويڪٽر<رڪن_t> new_terms;
		RTLIL::Const off(0, ويڪر);

		لكل (خودڪار &پورٽ : terms)
		{
			إذا (Get_سائيز(پورٽ.اندريون_a) == 0 && Get_سائيز(پورٽ.اندريون_b) == 0)
				استمر;

			إذا (Get_سائيز(پورٽ.اندريون_a) < Get_سائيز(پورٽ.اندريون_b))
				std::مٽا_سٽا(پورٽ.اندريون_a, پورٽ.اندريون_b);

			إذا (پورٽ.اندريون_a.is_fully_const() && پورٽ.اندريون_b.is_fully_const()) {
				RTLIL::Const v = پورٽ.اندريون_a.as_const();
				إذا (Get_سائيز(پورٽ.اندريون_b))
					v = const_mul(v, پورٽ.اندريون_b.as_const(), پورٽ.is_signed, پورٽ.is_signed, ويڪر);
				إذا (پورٽ.do_ڪٽيو)
					off = const_sub(off, v, پورٽ.is_signed, پورٽ.is_signed, ويڪر);
				وإلا
					off = const_جوڙيو(off, v, پورٽ.is_signed, پورٽ.is_signed, ويڪر);
				استمر;
			}

			إذا (پورٽ.is_signed) {
				طالما (Get_سائيز(پورٽ.اندريون_a) > 1 && پورٽ.اندريون_a[Get_سائيز(پورٽ.اندريون_a)-1] == پورٽ.اندريون_a[Get_سائيز(پورٽ.اندريون_a)-2])
					پورٽ.اندريون_a.remove(Get_سائيز(پورٽ.اندريون_a)-1);
				طالما (Get_سائيز(پورٽ.اندريون_b) > 1 && پورٽ.اندريون_b[Get_سائيز(پورٽ.اندريون_b)-1] == پورٽ.اندريون_b[Get_سائيز(پورٽ.اندريون_b)-2])
					پورٽ.اندريون_b.remove(Get_سائيز(پورٽ.اندريون_b)-1);
			} وإلا {
				طالما (Get_سائيز(پورٽ.اندريون_a) > 1 && پورٽ.اندريون_a[Get_سائيز(پورٽ.اندريون_a)-1] == State::S0)
					پورٽ.اندريون_a.remove(Get_سائيز(پورٽ.اندريون_a)-1);
				طالما (Get_سائيز(پورٽ.اندريون_b) > 1 && پورٽ.اندريون_b[Get_سائيز(پورٽ.اندريون_b)-1] == State::S0)
					پورٽ.اندريون_b.remove(Get_سائيز(پورٽ.اندريون_b)-1);
			}

			new_terms.ڌڪو_پٺ(پورٽ);
		}

		إذا (off.as_bool()) {
			رڪن_t پورٽ;
			پورٽ.اندريون_a = off;
			پورٽ.is_signed = false;
			پورٽ.do_ڪٽيو = false;
			new_terms.ڌڪو_پٺ(پورٽ);
		}

		new_terms.مٽا_سٽا(terms);
	}

	فراغ from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec پورٽ_a = cell->get_پورٽ(ID::A);

		terms.clear();

		خودڪار config_بٽ = cell->getParam(ID::CONFIG);
		صحيح config_cursor = 0;

		صحيح config_ويڪر = cell->getParam(ID::CONFIG_ويڪر).as_int();
		log_assert(Get_سائيز(config_بٽ) >= config_ويڪر);

		صحيح انگ_بٽ = 0;
		إذا (config_بٽ[config_cursor++] == State::S1) انگ_بٽ |= 1;
		إذا (config_بٽ[config_cursor++] == State::S1) انگ_بٽ |= 2;
		إذا (config_بٽ[config_cursor++] == State::S1) انگ_بٽ |= 4;
		إذا (config_بٽ[config_cursor++] == State::S1) انگ_بٽ |= 8;

		صحيح پورٽ_a_cursor = 0;
		طالما (پورٽ_a_cursor < Get_سائيز(پورٽ_a))
		{
			log_assert(config_cursor + 2 + 2*انگ_بٽ <= config_ويڪر);

			رڪن_t this_پورٽ;
			this_پورٽ.is_signed = config_بٽ[config_cursor++] == State::S1;
			this_پورٽ.do_ڪٽيو = config_بٽ[config_cursor++] == State::S1;

			صحيح سائيز_a = 0;
			لكل (صحيح i = 0; i < انگ_بٽ; i++)
				إذا (config_بٽ[config_cursor++] == State::S1)
					سائيز_a |= 1 << i;

			this_پورٽ.اندريون_a = پورٽ_a.ڪڍو_2(پورٽ_a_cursor, سائيز_a);
			پورٽ_a_cursor += سائيز_a;

			صحيح سائيز_b = 0;
			لكل (صحيح i = 0; i < انگ_بٽ; i++)
				إذا (config_بٽ[config_cursor++] == State::S1)
					سائيز_b |= 1 << i;

			this_پورٽ.اندريون_b = پورٽ_a.ڪڍو_2(پورٽ_a_cursor, سائيز_b);
			پورٽ_a_cursor += سائيز_b;

			إذا (سائيز_a || سائيز_b)
				terms.ڌڪو_پٺ(this_پورٽ);
		}

		لكل (خودڪار ٻٽو_انگ : cell->get_پورٽ(ID::B))
			terms.ڌڪو_پٺ(رڪن_t{{ٻٽو_انگ}, {}, false, false});

		log_assert(config_cursor == config_ويڪر);
		log_assert(پورٽ_a_cursor == Get_سائيز(پورٽ_a));
	}

	فراغ from_cell(RTLIL::Cell *cell)
	{
		إذا (cell->type == ID($macc)) {
			from_cell_v1(cell);
			أرجع;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec پورٽ_a = cell->get_پورٽ(ID::A);
		RTLIL::SigSpec پورٽ_b = cell->get_پورٽ(ID::B);
		RTLIL::SigSpec پورٽ_c = cell->get_پورٽ(ID::C);

		terms.clear();

		صحيح nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ثابت Const &حاصل_ضرب_neg = cell->getParam(ID::حاصل_ضرب_NEGATED);
		ثابت Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ثابت Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ثابت Const &a_signed = cell->getParam(ID::A_SIGNED);
		ثابت Const &b_signed = cell->getParam(ID::B_SIGNED);
		صحيح ai = 0, bi = 0;
		لكل (صحيح i = 0; i < nproducts; i++) {
			رڪن_t رڪن;

			log_assert(a_signed[i] == b_signed[i]);
			رڪن.is_signed = (a_signed[i] == State::S1);
			صحيح a_ويڪر = a_widths.ڪڍو_2(16 * i, 16).as_int(false);
			صحيح b_ويڪر = b_widths.ڪڍو_2(16 * i, 16).as_int(false);

			رڪن.اندريون_a = پورٽ_a.ڪڍو_2(ai, a_ويڪر);
			ai += a_ويڪر;
			رڪن.اندريون_b = پورٽ_b.ڪڍو_2(bi, b_ويڪر);
			bi += b_ويڪر;
			رڪن.do_ڪٽيو = (حاصل_ضرب_neg[i] == State::S1);

			terms.ڌڪو_پٺ(رڪن);
		}
		log_assert(پورٽ_a.سائيز() == ai);
		log_assert(پورٽ_b.سائيز() == bi);

		صحيح naddends = cell->getParam(ID::NADDENDS).as_int();
		ثابت Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ثابت Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ثابت Const &c_signed = cell->getParam(ID::C_SIGNED);
		صحيح ci = 0;
		لكل (صحيح i = 0; i < naddends; i++) {
			رڪن_t رڪن;

			رڪن.is_signed = (c_signed[i] == State::S1);
			صحيح c_ويڪر = c_widths.ڪڍو_2(16 * i, 16).as_int(false);

			رڪن.اندريون_a = پورٽ_c.ڪڍو_2(ci, c_ويڪر);
			ci += c_ويڪر;
			رڪن.do_ڪٽيو = (addend_neg[i] == State::S1);

			terms.ڌڪو_پٺ(رڪن);
		}
		log_assert(پورٽ_c.سائيز() == ci);
	}

	فراغ منزل_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		صحيح nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, حاصل_ضرب_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		لكل (صحيح i = 0; i < (صحيح) terms.سائيز(); i++) {
			SigSpec رڪن_a = terms[i].اندريون_a, رڪن_b = terms[i].اندريون_b;

			إذا (رڪن_b.خالي_جايون()) {
				// addend
				c_widths.append(Const(رڪن_a.سائيز(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_ڪٽيو ? RTLIL::S1 : RTLIL::S0);
				c.append(رڪن_a);
				naddends++;
			} وإلا {
				//حاصل_ضرب
				a_widths.append(Const(رڪن_a.سائيز(), 16));
				b_widths.append(Const(رڪن_b.سائيز(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				حاصل_ضرب_negated.append(terms[i].do_ڪٽيو ? RTLIL::S1 : RTLIL::S0);
				a.append(رڪن_a);
				b.append(رڪن_b);
				nproducts++;
			}
		}

		إذا (a_signed.خالي_جايون())
			a_signed = {RTLIL::Sx};
		إذا (b_signed.خالي_جايون())
			b_signed = {RTLIL::Sx};
		إذا (c_signed.خالي_جايون())
			c_signed = {RTLIL::Sx};
		إذا (a_widths.خالي_جايون())
			a_widths = {RTLIL::Sx};
		إذا (b_widths.خالي_جايون())
			b_widths = {RTLIL::Sx};
		إذا (c_widths.خالي_جايون())
			c_widths = {RTLIL::Sx};
		إذا (حاصل_ضرب_negated.خالي_جايون())
			حاصل_ضرب_negated = {RTLIL::Sx};
		إذا (addend_negated.خالي_جايون())
			addend_negated = {RTLIL::Sx};

		cell->سيٽ_Param(ID::NPRODUCTS, nproducts);
		cell->سيٽ_Param(ID::حاصل_ضرب_NEGATED, حاصل_ضرب_negated);
		cell->سيٽ_Param(ID::NADDENDS, naddends);
		cell->سيٽ_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->سيٽ_Param(ID::A_SIGNED, a_signed);
		cell->سيٽ_Param(ID::B_SIGNED, b_signed);
		cell->سيٽ_Param(ID::C_SIGNED, c_signed);
		cell->سيٽ_Param(ID::A_WIDTHS, a_widths);
		cell->سيٽ_Param(ID::B_WIDTHS, b_widths);
		cell->سيٽ_Param(ID::C_WIDTHS, c_widths);
		cell->سيٽ_پورٽ(ID::A, a);
		cell->سيٽ_پورٽ(ID::B, b);
		cell->سيٽ_پورٽ(ID::C, c);
	}

	bool eval(RTLIL::Const &نتيجو) ثابت
	{
		لكل (خودڪار ٻٽو_انگ : نتيجو)
			ٻٽو_انگ = State::S0;

		لكل (خودڪار &پورٽ : terms)
		{
			إذا (!پورٽ.اندريون_a.is_fully_const() || !پورٽ.اندريون_b.is_fully_const())
				أرجع false;

			RTLIL::Const summand;
			إذا (Get_سائيز(پورٽ.اندريون_b) == 0)
				summand = const_pos(پورٽ.اندريون_a.as_const(), پورٽ.اندريون_b.as_const(), پورٽ.is_signed, پورٽ.is_signed, Get_سائيز(نتيجو));
			وإلا
				summand = const_mul(پورٽ.اندريون_a.as_const(), پورٽ.اندريون_b.as_const(), پورٽ.is_signed, پورٽ.is_signed, Get_سائيز(نتيجو));

			إذا (پورٽ.do_ڪٽيو)
				نتيجو = const_sub(نتيجو, summand, پورٽ.is_signed, پورٽ.is_signed, Get_سائيز(نتيجو));
			وإلا
				نتيجو = const_جوڙيو(نتيجو, summand, پورٽ.is_signed, پورٽ.is_signed, Get_سائيز(نتيجو));
		}

		أرجع true;
	}

	bool is_simple_حاصل_ضرب()
	{
		أرجع terms.سائيز() == 1 &&
				!terms[0].اندريون_b.خالي_جايون() &&
				!terms[0].do_ڪٽيو;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		إذا (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
