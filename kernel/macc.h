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

#اگر_غیر_تعریف_شدہ MACC_H
#تعریف_کرو MACC_H

#شامل "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ساخت Macc
{
	ساخت رکن_t {
		RTLIL::SigSpec اندر_a, اندر_b;
		bool is_signed, do_تفریق;
	};
	std::سمتیہ<رکن_t> terms;

	خالی optimize(صحیح چوڑائی)
	{
		std::سمتیہ<رکن_t> new_terms;
		RTLIL::Const off(0, چوڑائی);

		برائے (خودکار &پورٹ : terms)
		{
			اگر (Get_سائز(پورٹ.اندر_a) == 0 && Get_سائز(پورٹ.اندر_b) == 0)
				جاری;

			اگر (Get_سائز(پورٹ.اندر_a) < Get_سائز(پورٹ.اندر_b))
				std::تبادلہ(پورٹ.اندر_a, پورٹ.اندر_b);

			اگر (پورٹ.اندر_a.is_fully_const() && پورٹ.اندر_b.is_fully_const()) {
				RTLIL::Const v = پورٹ.اندر_a.as_const();
				اگر (Get_سائز(پورٹ.اندر_b))
					v = const_mul(v, پورٹ.اندر_b.as_const(), پورٹ.is_signed, پورٹ.is_signed, چوڑائی);
				اگر (پورٹ.do_تفریق)
					off = const_sub(off, v, پورٹ.is_signed, پورٹ.is_signed, چوڑائی);
				ورنہ
					off = const_جوڑو(off, v, پورٹ.is_signed, پورٹ.is_signed, چوڑائی);
				جاری;
			}

			اگر (پورٹ.is_signed) {
				جب (Get_سائز(پورٹ.اندر_a) > 1 && پورٹ.اندر_a[Get_سائز(پورٹ.اندر_a)-1] == پورٹ.اندر_a[Get_سائز(پورٹ.اندر_a)-2])
					پورٹ.اندر_a.remove(Get_سائز(پورٹ.اندر_a)-1);
				جب (Get_سائز(پورٹ.اندر_b) > 1 && پورٹ.اندر_b[Get_سائز(پورٹ.اندر_b)-1] == پورٹ.اندر_b[Get_سائز(پورٹ.اندر_b)-2])
					پورٹ.اندر_b.remove(Get_سائز(پورٹ.اندر_b)-1);
			} ورنہ {
				جب (Get_سائز(پورٹ.اندر_a) > 1 && پورٹ.اندر_a[Get_سائز(پورٹ.اندر_a)-1] == State::S0)
					پورٹ.اندر_a.remove(Get_سائز(پورٹ.اندر_a)-1);
				جب (Get_سائز(پورٹ.اندر_b) > 1 && پورٹ.اندر_b[Get_سائز(پورٹ.اندر_b)-1] == State::S0)
					پورٹ.اندر_b.remove(Get_سائز(پورٹ.اندر_b)-1);
			}

			new_terms.ڈالو_عقب(پورٹ);
		}

		اگر (off.as_bool()) {
			رکن_t پورٹ;
			پورٹ.اندر_a = off;
			پورٹ.is_signed = false;
			پورٹ.do_تفریق = false;
			new_terms.ڈالو_عقب(پورٹ);
		}

		new_terms.تبادلہ(terms);
	}

	خالی from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec پورٹ_a = cell->get_پورٹ(ID::A);

		terms.clear();

		خودکار config_بٹ = cell->getParam(ID::CONFIG);
		صحیح config_cursor = 0;

		صحیح config_چوڑائی = cell->getParam(ID::CONFIG_چوڑائی).as_int();
		log_assert(Get_سائز(config_بٹ) >= config_چوڑائی);

		صحیح عدد_بٹ = 0;
		اگر (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 1;
		اگر (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 2;
		اگر (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 4;
		اگر (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 8;

		صحیح پورٹ_a_cursor = 0;
		جب (پورٹ_a_cursor < Get_سائز(پورٹ_a))
		{
			log_assert(config_cursor + 2 + 2*عدد_بٹ <= config_چوڑائی);

			رکن_t this_پورٹ;
			this_پورٹ.is_signed = config_بٹ[config_cursor++] == State::S1;
			this_پورٹ.do_تفریق = config_بٹ[config_cursor++] == State::S1;

			صحیح سائز_a = 0;
			برائے (صحیح i = 0; i < عدد_بٹ; i++)
				اگر (config_بٹ[config_cursor++] == State::S1)
					سائز_a |= 1 << i;

			this_پورٹ.اندر_a = پورٹ_a.نکالو_2(پورٹ_a_cursor, سائز_a);
			پورٹ_a_cursor += سائز_a;

			صحیح سائز_b = 0;
			برائے (صحیح i = 0; i < عدد_بٹ; i++)
				اگر (config_بٹ[config_cursor++] == State::S1)
					سائز_b |= 1 << i;

			this_پورٹ.اندر_b = پورٹ_a.نکالو_2(پورٹ_a_cursor, سائز_b);
			پورٹ_a_cursor += سائز_b;

			اگر (سائز_a || سائز_b)
				terms.ڈالو_عقب(this_پورٹ);
		}

		برائے (خودکار ثنائی_ہندسہ : cell->get_پورٹ(ID::B))
			terms.ڈالو_عقب(رکن_t{{ثنائی_ہندسہ}, {}, false, false});

		log_assert(config_cursor == config_چوڑائی);
		log_assert(پورٹ_a_cursor == Get_سائز(پورٹ_a));
	}

	خالی from_cell(RTLIL::Cell *cell)
	{
		اگر (cell->type == ID($macc)) {
			from_cell_v1(cell);
			واپس;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec پورٹ_a = cell->get_پورٹ(ID::A);
		RTLIL::SigSpec پورٹ_b = cell->get_پورٹ(ID::B);
		RTLIL::SigSpec پورٹ_c = cell->get_پورٹ(ID::C);

		terms.clear();

		صحیح nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ثابت Const &حاصل_ضرب_neg = cell->getParam(ID::حاصل_ضرب_NEGATED);
		ثابت Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ثابت Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ثابت Const &a_signed = cell->getParam(ID::A_SIGNED);
		ثابت Const &b_signed = cell->getParam(ID::B_SIGNED);
		صحیح ai = 0, bi = 0;
		برائے (صحیح i = 0; i < nproducts; i++) {
			رکن_t رکن;

			log_assert(a_signed[i] == b_signed[i]);
			رکن.is_signed = (a_signed[i] == State::S1);
			صحیح a_چوڑائی = a_widths.نکالو_2(16 * i, 16).as_int(false);
			صحیح b_چوڑائی = b_widths.نکالو_2(16 * i, 16).as_int(false);

			رکن.اندر_a = پورٹ_a.نکالو_2(ai, a_چوڑائی);
			ai += a_چوڑائی;
			رکن.اندر_b = پورٹ_b.نکالو_2(bi, b_چوڑائی);
			bi += b_چوڑائی;
			رکن.do_تفریق = (حاصل_ضرب_neg[i] == State::S1);

			terms.ڈالو_عقب(رکن);
		}
		log_assert(پورٹ_a.سائز() == ai);
		log_assert(پورٹ_b.سائز() == bi);

		صحیح naddends = cell->getParam(ID::NADDENDS).as_int();
		ثابت Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ثابت Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ثابت Const &c_signed = cell->getParam(ID::C_SIGNED);
		صحیح ci = 0;
		برائے (صحیح i = 0; i < naddends; i++) {
			رکن_t رکن;

			رکن.is_signed = (c_signed[i] == State::S1);
			صحیح c_چوڑائی = c_widths.نکالو_2(16 * i, 16).as_int(false);

			رکن.اندر_a = پورٹ_c.نکالو_2(ci, c_چوڑائی);
			ci += c_چوڑائی;
			رکن.do_تفریق = (addend_neg[i] == State::S1);

			terms.ڈالو_عقب(رکن);
		}
		log_assert(پورٹ_c.سائز() == ci);
	}

	خالی مقصد_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		صحیح nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, حاصل_ضرب_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		برائے (صحیح i = 0; i < (صحیح) terms.سائز(); i++) {
			SigSpec رکن_a = terms[i].اندر_a, رکن_b = terms[i].اندر_b;

			اگر (رکن_b.خالی_خانے()) {
				// addend
				c_widths.append(Const(رکن_a.سائز(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_تفریق ? RTLIL::S1 : RTLIL::S0);
				c.append(رکن_a);
				naddends++;
			} ورنہ {
				//حاصل_ضرب
				a_widths.append(Const(رکن_a.سائز(), 16));
				b_widths.append(Const(رکن_b.سائز(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				حاصل_ضرب_negated.append(terms[i].do_تفریق ? RTLIL::S1 : RTLIL::S0);
				a.append(رکن_a);
				b.append(رکن_b);
				nproducts++;
			}
		}

		اگر (a_signed.خالی_خانے())
			a_signed = {RTLIL::Sx};
		اگر (b_signed.خالی_خانے())
			b_signed = {RTLIL::Sx};
		اگر (c_signed.خالی_خانے())
			c_signed = {RTLIL::Sx};
		اگر (a_widths.خالی_خانے())
			a_widths = {RTLIL::Sx};
		اگر (b_widths.خالی_خانے())
			b_widths = {RTLIL::Sx};
		اگر (c_widths.خالی_خانے())
			c_widths = {RTLIL::Sx};
		اگر (حاصل_ضرب_negated.خالی_خانے())
			حاصل_ضرب_negated = {RTLIL::Sx};
		اگر (addend_negated.خالی_خانے())
			addend_negated = {RTLIL::Sx};

		cell->سیٹ_Param(ID::NPRODUCTS, nproducts);
		cell->سیٹ_Param(ID::حاصل_ضرب_NEGATED, حاصل_ضرب_negated);
		cell->سیٹ_Param(ID::NADDENDS, naddends);
		cell->سیٹ_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->سیٹ_Param(ID::A_SIGNED, a_signed);
		cell->سیٹ_Param(ID::B_SIGNED, b_signed);
		cell->سیٹ_Param(ID::C_SIGNED, c_signed);
		cell->سیٹ_Param(ID::A_WIDTHS, a_widths);
		cell->سیٹ_Param(ID::B_WIDTHS, b_widths);
		cell->سیٹ_Param(ID::C_WIDTHS, c_widths);
		cell->سیٹ_پورٹ(ID::A, a);
		cell->سیٹ_پورٹ(ID::B, b);
		cell->سیٹ_پورٹ(ID::C, c);
	}

	bool eval(RTLIL::Const &نتیجہ) ثابت
	{
		برائے (خودکار ثنائی_ہندسہ : نتیجہ)
			ثنائی_ہندسہ = State::S0;

		برائے (خودکار &پورٹ : terms)
		{
			اگر (!پورٹ.اندر_a.is_fully_const() || !پورٹ.اندر_b.is_fully_const())
				واپس false;

			RTLIL::Const summand;
			اگر (Get_سائز(پورٹ.اندر_b) == 0)
				summand = const_pos(پورٹ.اندر_a.as_const(), پورٹ.اندر_b.as_const(), پورٹ.is_signed, پورٹ.is_signed, Get_سائز(نتیجہ));
			ورنہ
				summand = const_mul(پورٹ.اندر_a.as_const(), پورٹ.اندر_b.as_const(), پورٹ.is_signed, پورٹ.is_signed, Get_سائز(نتیجہ));

			اگر (پورٹ.do_تفریق)
				نتیجہ = const_sub(نتیجہ, summand, پورٹ.is_signed, پورٹ.is_signed, Get_سائز(نتیجہ));
			ورنہ
				نتیجہ = const_جوڑو(نتیجہ, summand, پورٹ.is_signed, پورٹ.is_signed, Get_سائز(نتیجہ));
		}

		واپس true;
	}

	bool is_simple_حاصل_ضرب()
	{
		واپس terms.سائز() == 1 &&
				!terms[0].اندر_b.خالی_خانے() &&
				!terms[0].do_تفریق;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		اگر (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
