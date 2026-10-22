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

#جے_بنا_تعریف MACC_H
#تعریف_کرو MACC_H

#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

بنية Macc
{
	بنية رکن_t {
		RTLIL::SigSpec وچ_a, وچ_b;
		bool is_signed, do_گھٹاؤ;
	};
	std::سمتیہ<رکن_t> terms;

	فراغ optimize(صحيح چوڑائی)
	{
		std::سمتیہ<رکن_t> new_terms;
		RTLIL::Const off(0, چوڑائی);

		لكل (خودکار &پورٹ : terms)
		{
			إذا (Get_ناپ(پورٹ.وچ_a) == 0 && Get_ناپ(پورٹ.وچ_b) == 0)
				استمر;

			إذا (Get_ناپ(پورٹ.وچ_a) < Get_ناپ(پورٹ.وچ_b))
				std::وٹاندرا(پورٹ.وچ_a, پورٹ.وچ_b);

			إذا (پورٹ.وچ_a.is_fully_const() && پورٹ.وچ_b.is_fully_const()) {
				RTLIL::Const v = پورٹ.وچ_a.as_const();
				إذا (Get_ناپ(پورٹ.وچ_b))
					v = const_mul(v, پورٹ.وچ_b.as_const(), پورٹ.is_signed, پورٹ.is_signed, چوڑائی);
				إذا (پورٹ.do_گھٹاؤ)
					off = const_sub(off, v, پورٹ.is_signed, پورٹ.is_signed, چوڑائی);
				وإلا
					off = const_جوڑو(off, v, پورٹ.is_signed, پورٹ.is_signed, چوڑائی);
				استمر;
			}

			إذا (پورٹ.is_signed) {
				طالما (Get_ناپ(پورٹ.وچ_a) > 1 && پورٹ.وچ_a[Get_ناپ(پورٹ.وچ_a)-1] == پورٹ.وچ_a[Get_ناپ(پورٹ.وچ_a)-2])
					پورٹ.وچ_a.remove(Get_ناپ(پورٹ.وچ_a)-1);
				طالما (Get_ناپ(پورٹ.وچ_b) > 1 && پورٹ.وچ_b[Get_ناپ(پورٹ.وچ_b)-1] == پورٹ.وچ_b[Get_ناپ(پورٹ.وچ_b)-2])
					پورٹ.وچ_b.remove(Get_ناپ(پورٹ.وچ_b)-1);
			} وإلا {
				طالما (Get_ناپ(پورٹ.وچ_a) > 1 && پورٹ.وچ_a[Get_ناپ(پورٹ.وچ_a)-1] == State::S0)
					پورٹ.وچ_a.remove(Get_ناپ(پورٹ.وچ_a)-1);
				طالما (Get_ناپ(پورٹ.وچ_b) > 1 && پورٹ.وچ_b[Get_ناپ(پورٹ.وچ_b)-1] == State::S0)
					پورٹ.وچ_b.remove(Get_ناپ(پورٹ.وچ_b)-1);
			}

			new_terms.ڈالو_پٹھ(پورٹ);
		}

		إذا (off.as_bool()) {
			رکن_t پورٹ;
			پورٹ.وچ_a = off;
			پورٹ.is_signed = false;
			پورٹ.do_گھٹاؤ = false;
			new_terms.ڈالو_پٹھ(پورٹ);
		}

		new_terms.وٹاندرا(terms);
	}

	فراغ from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec پورٹ_a = cell->get_پورٹ(ID::A);

		terms.clear();

		خودکار config_بٹ = cell->getParam(ID::CONFIG);
		صحيح config_cursor = 0;

		صحيح config_چوڑائی = cell->getParam(ID::CONFIG_چوڑائی).as_int();
		log_assert(Get_ناپ(config_بٹ) >= config_چوڑائی);

		صحيح عدد_بٹ = 0;
		إذا (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 1;
		إذا (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 2;
		إذا (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 4;
		إذا (config_بٹ[config_cursor++] == State::S1) عدد_بٹ |= 8;

		صحيح پورٹ_a_cursor = 0;
		طالما (پورٹ_a_cursor < Get_ناپ(پورٹ_a))
		{
			log_assert(config_cursor + 2 + 2*عدد_بٹ <= config_چوڑائی);

			رکن_t this_پورٹ;
			this_پورٹ.is_signed = config_بٹ[config_cursor++] == State::S1;
			this_پورٹ.do_گھٹاؤ = config_بٹ[config_cursor++] == State::S1;

			صحيح ناپ_a = 0;
			لكل (صحيح i = 0; i < عدد_بٹ; i++)
				إذا (config_بٹ[config_cursor++] == State::S1)
					ناپ_a |= 1 << i;

			this_پورٹ.وچ_a = پورٹ_a.کڈھو(پورٹ_a_cursor, ناپ_a);
			پورٹ_a_cursor += ناپ_a;

			صحيح ناپ_b = 0;
			لكل (صحيح i = 0; i < عدد_بٹ; i++)
				إذا (config_بٹ[config_cursor++] == State::S1)
					ناپ_b |= 1 << i;

			this_پورٹ.وچ_b = پورٹ_a.کڈھو(پورٹ_a_cursor, ناپ_b);
			پورٹ_a_cursor += ناپ_b;

			إذا (ناپ_a || ناپ_b)
				terms.ڈالو_پٹھ(this_پورٹ);
		}

		لكل (خودکار ثنائی_ہندسہ : cell->get_پورٹ(ID::B))
			terms.ڈالو_پٹھ(رکن_t{{ثنائی_ہندسہ}, {}, false, false});

		log_assert(config_cursor == config_چوڑائی);
		log_assert(پورٹ_a_cursor == Get_ناپ(پورٹ_a));
	}

	فراغ from_cell(RTLIL::Cell *cell)
	{
		إذا (cell->type == ID($macc)) {
			from_cell_v1(cell);
			أرجع;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec پورٹ_a = cell->get_پورٹ(ID::A);
		RTLIL::SigSpec پورٹ_b = cell->get_پورٹ(ID::B);
		RTLIL::SigSpec پورٹ_c = cell->get_پورٹ(ID::C);

		terms.clear();

		صحيح nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ثابت Const &ضرب_حاصل_neg = cell->getParam(ID::ضرب_حاصل_NEGATED);
		ثابت Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ثابت Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ثابت Const &a_signed = cell->getParam(ID::A_SIGNED);
		ثابت Const &b_signed = cell->getParam(ID::B_SIGNED);
		صحيح ai = 0, bi = 0;
		لكل (صحيح i = 0; i < nproducts; i++) {
			رکن_t رکن;

			log_assert(a_signed[i] == b_signed[i]);
			رکن.is_signed = (a_signed[i] == State::S1);
			صحيح a_چوڑائی = a_widths.کڈھو(16 * i, 16).as_int(false);
			صحيح b_چوڑائی = b_widths.کڈھو(16 * i, 16).as_int(false);

			رکن.وچ_a = پورٹ_a.کڈھو(ai, a_چوڑائی);
			ai += a_چوڑائی;
			رکن.وچ_b = پورٹ_b.کڈھو(bi, b_چوڑائی);
			bi += b_چوڑائی;
			رکن.do_گھٹاؤ = (ضرب_حاصل_neg[i] == State::S1);

			terms.ڈالو_پٹھ(رکن);
		}
		log_assert(پورٹ_a.ناپ() == ai);
		log_assert(پورٹ_b.ناپ() == bi);

		صحيح naddends = cell->getParam(ID::NADDENDS).as_int();
		ثابت Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ثابت Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ثابت Const &c_signed = cell->getParam(ID::C_SIGNED);
		صحيح ci = 0;
		لكل (صحيح i = 0; i < naddends; i++) {
			رکن_t رکن;

			رکن.is_signed = (c_signed[i] == State::S1);
			صحيح c_چوڑائی = c_widths.کڈھو(16 * i, 16).as_int(false);

			رکن.وچ_a = پورٹ_c.کڈھو(ci, c_چوڑائی);
			ci += c_چوڑائی;
			رکن.do_گھٹاؤ = (addend_neg[i] == State::S1);

			terms.ڈالو_پٹھ(رکن);
		}
		log_assert(پورٹ_c.ناپ() == ci);
	}

	فراغ منزل_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		صحيح nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, ضرب_حاصل_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		لكل (صحيح i = 0; i < (صحيح) terms.ناپ(); i++) {
			SigSpec رکن_a = terms[i].وچ_a, رکن_b = terms[i].وچ_b;

			إذا (رکن_b.خالی_تھاواں()) {
				// addend
				c_widths.append(Const(رکن_a.ناپ(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_گھٹاؤ ? RTLIL::S1 : RTLIL::S0);
				c.append(رکن_a);
				naddends++;
			} وإلا {
				//ضرب_حاصل
				a_widths.append(Const(رکن_a.ناپ(), 16));
				b_widths.append(Const(رکن_b.ناپ(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				ضرب_حاصل_negated.append(terms[i].do_گھٹاؤ ? RTLIL::S1 : RTLIL::S0);
				a.append(رکن_a);
				b.append(رکن_b);
				nproducts++;
			}
		}

		إذا (a_signed.خالی_تھاواں())
			a_signed = {RTLIL::Sx};
		إذا (b_signed.خالی_تھاواں())
			b_signed = {RTLIL::Sx};
		إذا (c_signed.خالی_تھاواں())
			c_signed = {RTLIL::Sx};
		إذا (a_widths.خالی_تھاواں())
			a_widths = {RTLIL::Sx};
		إذا (b_widths.خالی_تھاواں())
			b_widths = {RTLIL::Sx};
		إذا (c_widths.خالی_تھاواں())
			c_widths = {RTLIL::Sx};
		إذا (ضرب_حاصل_negated.خالی_تھاواں())
			ضرب_حاصل_negated = {RTLIL::Sx};
		إذا (addend_negated.خالی_تھاواں())
			addend_negated = {RTLIL::Sx};

		cell->سیٹ_Param(ID::NPRODUCTS, nproducts);
		cell->سیٹ_Param(ID::ضرب_حاصل_NEGATED, ضرب_حاصل_negated);
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
		لكل (خودکار ثنائی_ہندسہ : نتیجہ)
			ثنائی_ہندسہ = State::S0;

		لكل (خودکار &پورٹ : terms)
		{
			إذا (!پورٹ.وچ_a.is_fully_const() || !پورٹ.وچ_b.is_fully_const())
				أرجع false;

			RTLIL::Const summand;
			إذا (Get_ناپ(پورٹ.وچ_b) == 0)
				summand = const_pos(پورٹ.وچ_a.as_const(), پورٹ.وچ_b.as_const(), پورٹ.is_signed, پورٹ.is_signed, Get_ناپ(نتیجہ));
			وإلا
				summand = const_mul(پورٹ.وچ_a.as_const(), پورٹ.وچ_b.as_const(), پورٹ.is_signed, پورٹ.is_signed, Get_ناپ(نتیجہ));

			إذا (پورٹ.do_گھٹاؤ)
				نتیجہ = const_sub(نتیجہ, summand, پورٹ.is_signed, پورٹ.is_signed, Get_ناپ(نتیجہ));
			وإلا
				نتیجہ = const_جوڑو(نتیجہ, summand, پورٹ.is_signed, پورٹ.is_signed, Get_ناپ(نتیجہ));
		}

		أرجع true;
	}

	bool is_simple_ضرب_حاصل()
	{
		أرجع terms.ناپ() == 1 &&
				!terms[0].وچ_b.خالی_تھاواں() &&
				!terms[0].do_گھٹاؤ;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		إذا (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_سرا

#جے_انت
