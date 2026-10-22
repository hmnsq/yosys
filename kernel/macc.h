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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MACC_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MACC_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ Macc
{
	ਬਣਤਰ ਪਦ_t {
		RTLIL::SigSpec ਵਿੱਚ_a, ਵਿੱਚ_b;
		bool is_signed, do_ਘਟਾਓ;
	};
	std::ਸਦਿਸ਼<ਪਦ_t> terms;

	ਖਾਲੀ optimize(ਪੂਰਨ ਚੌੜਾਈ)
	{
		std::ਸਦਿਸ਼<ਪਦ_t> new_terms;
		RTLIL::Const off(0, ਚੌੜਾਈ);

		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : terms)
		{
			ਜੇ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a) == 0 && Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b) == 0)
				ਜਾਰੀ;

			ਜੇ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a) < Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b))
				std::ਅਦਲਾ_ਬਦਲੀ(ਪੋਰਟ.ਵਿੱਚ_a, ਪੋਰਟ.ਵਿੱਚ_b);

			ਜੇ (ਪੋਰਟ.ਵਿੱਚ_a.is_fully_const() && ਪੋਰਟ.ਵਿੱਚ_b.is_fully_const()) {
				RTLIL::Const v = ਪੋਰਟ.ਵਿੱਚ_a.as_const();
				ਜੇ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b))
					v = const_mul(v, ਪੋਰਟ.ਵਿੱਚ_b.as_const(), ਪੋਰਟ.is_signed, ਪੋਰਟ.is_signed, ਚੌੜਾਈ);
				ਜੇ (ਪੋਰਟ.do_ਘਟਾਓ)
					off = const_sub(off, v, ਪੋਰਟ.is_signed, ਪੋਰਟ.is_signed, ਚੌੜਾਈ);
				ਨਹੀਂਤਾਂ
					off = const_ਜੋੜੋ(off, v, ਪੋਰਟ.is_signed, ਪੋਰਟ.is_signed, ਚੌੜਾਈ);
				ਜਾਰੀ;
			}

			ਜੇ (ਪੋਰਟ.is_signed) {
				ਜਦੋਂ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a) > 1 && ਪੋਰਟ.ਵਿੱਚ_a[Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a)-1] == ਪੋਰਟ.ਵਿੱਚ_a[Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a)-2])
					ਪੋਰਟ.ਵਿੱਚ_a.remove(Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a)-1);
				ਜਦੋਂ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b) > 1 && ਪੋਰਟ.ਵਿੱਚ_b[Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b)-1] == ਪੋਰਟ.ਵਿੱਚ_b[Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b)-2])
					ਪੋਰਟ.ਵਿੱਚ_b.remove(Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b)-1);
			} ਨਹੀਂਤਾਂ {
				ਜਦੋਂ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a) > 1 && ਪੋਰਟ.ਵਿੱਚ_a[Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a)-1] == State::S0)
					ਪੋਰਟ.ਵਿੱਚ_a.remove(Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_a)-1);
				ਜਦੋਂ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b) > 1 && ਪੋਰਟ.ਵਿੱਚ_b[Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b)-1] == State::S0)
					ਪੋਰਟ.ਵਿੱਚ_b.remove(Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b)-1);
			}

			new_terms.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ);
		}

		ਜੇ (off.as_bool()) {
			ਪਦ_t ਪੋਰਟ;
			ਪੋਰਟ.ਵਿੱਚ_a = off;
			ਪੋਰਟ.is_signed = false;
			ਪੋਰਟ.do_ਘਟਾਓ = false;
			new_terms.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ);
		}

		new_terms.ਅਦਲਾ_ਬਦਲੀ(terms);
	}

	ਖਾਲੀ from_cell_v1(RTLIL::Cell *cell)
	{
		RTLIL::SigSpec ਪੋਰਟ_a = cell->get_ਪੋਰਟ(ID::A);

		terms.clear();

		ਸਵੈਚਾਲਿਤ config_ਬਿੱਟ = cell->getParam(ID::CONFIG);
		ਪੂਰਨ config_cursor = 0;

		ਪੂਰਨ config_ਚੌੜਾਈ = cell->getParam(ID::CONFIG_ਚੌੜਾਈ).as_int();
		log_assert(Get_ਮਾਪ(config_ਬਿੱਟ) >= config_ਚੌੜਾਈ);

		ਪੂਰਨ ਅੰਕ_ਬਿੱਟ = 0;
		ਜੇ (config_ਬਿੱਟ[config_cursor++] == State::S1) ਅੰਕ_ਬਿੱਟ |= 1;
		ਜੇ (config_ਬਿੱਟ[config_cursor++] == State::S1) ਅੰਕ_ਬਿੱਟ |= 2;
		ਜੇ (config_ਬਿੱਟ[config_cursor++] == State::S1) ਅੰਕ_ਬਿੱਟ |= 4;
		ਜੇ (config_ਬਿੱਟ[config_cursor++] == State::S1) ਅੰਕ_ਬਿੱਟ |= 8;

		ਪੂਰਨ ਪੋਰਟ_a_cursor = 0;
		ਜਦੋਂ (ਪੋਰਟ_a_cursor < Get_ਮਾਪ(ਪੋਰਟ_a))
		{
			log_assert(config_cursor + 2 + 2*ਅੰਕ_ਬਿੱਟ <= config_ਚੌੜਾਈ);

			ਪਦ_t this_ਪੋਰਟ;
			this_ਪੋਰਟ.is_signed = config_ਬਿੱਟ[config_cursor++] == State::S1;
			this_ਪੋਰਟ.do_ਘਟਾਓ = config_ਬਿੱਟ[config_cursor++] == State::S1;

			ਪੂਰਨ ਮਾਪ_a = 0;
			ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਬਿੱਟ; i++)
				ਜੇ (config_ਬਿੱਟ[config_cursor++] == State::S1)
					ਮਾਪ_a |= 1 << i;

			this_ਪੋਰਟ.ਵਿੱਚ_a = ਪੋਰਟ_a.ਕੱਢੋ_2(ਪੋਰਟ_a_cursor, ਮਾਪ_a);
			ਪੋਰਟ_a_cursor += ਮਾਪ_a;

			ਪੂਰਨ ਮਾਪ_b = 0;
			ਲਈ (ਪੂਰਨ i = 0; i < ਅੰਕ_ਬਿੱਟ; i++)
				ਜੇ (config_ਬਿੱਟ[config_cursor++] == State::S1)
					ਮਾਪ_b |= 1 << i;

			this_ਪੋਰਟ.ਵਿੱਚ_b = ਪੋਰਟ_a.ਕੱਢੋ_2(ਪੋਰਟ_a_cursor, ਮਾਪ_b);
			ਪੋਰਟ_a_cursor += ਮਾਪ_b;

			ਜੇ (ਮਾਪ_a || ਮਾਪ_b)
				terms.ਧੱਕੋ_ਪਿੱਠ(this_ਪੋਰਟ);
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : cell->get_ਪੋਰਟ(ID::B))
			terms.ਧੱਕੋ_ਪਿੱਠ(ਪਦ_t{{ਦੋਆਧਾਰੀ_ਅੰਕ}, {}, false, false});

		log_assert(config_cursor == config_ਚੌੜਾਈ);
		log_assert(ਪੋਰਟ_a_cursor == Get_ਮਾਪ(ਪੋਰਟ_a));
	}

	ਖਾਲੀ from_cell(RTLIL::Cell *cell)
	{
		ਜੇ (cell->type == ID($macc)) {
			from_cell_v1(cell);
			ਵਾਪਸ;
		}
		log_assert(cell->type == ID($macc_v2));

		RTLIL::SigSpec ਪੋਰਟ_a = cell->get_ਪੋਰਟ(ID::A);
		RTLIL::SigSpec ਪੋਰਟ_b = cell->get_ਪੋਰਟ(ID::B);
		RTLIL::SigSpec ਪੋਰਟ_c = cell->get_ਪੋਰਟ(ID::C);

		terms.clear();

		ਪੂਰਨ nproducts = cell->getParam(ID::NPRODUCTS).as_int();
		ਅਚਲ Const &ਗੁਣਨਫਲ_neg = cell->getParam(ID::ਗੁਣਨਫਲ_NEGATED);
		ਅਚਲ Const &a_widths = cell->getParam(ID::A_WIDTHS);
		ਅਚਲ Const &b_widths = cell->getParam(ID::B_WIDTHS);
		ਅਚਲ Const &a_signed = cell->getParam(ID::A_SIGNED);
		ਅਚਲ Const &b_signed = cell->getParam(ID::B_SIGNED);
		ਪੂਰਨ ai = 0, bi = 0;
		ਲਈ (ਪੂਰਨ i = 0; i < nproducts; i++) {
			ਪਦ_t ਪਦ;

			log_assert(a_signed[i] == b_signed[i]);
			ਪਦ.is_signed = (a_signed[i] == State::S1);
			ਪੂਰਨ a_ਚੌੜਾਈ = a_widths.ਕੱਢੋ_2(16 * i, 16).as_int(false);
			ਪੂਰਨ b_ਚੌੜਾਈ = b_widths.ਕੱਢੋ_2(16 * i, 16).as_int(false);

			ਪਦ.ਵਿੱਚ_a = ਪੋਰਟ_a.ਕੱਢੋ_2(ai, a_ਚੌੜਾਈ);
			ai += a_ਚੌੜਾਈ;
			ਪਦ.ਵਿੱਚ_b = ਪੋਰਟ_b.ਕੱਢੋ_2(bi, b_ਚੌੜਾਈ);
			bi += b_ਚੌੜਾਈ;
			ਪਦ.do_ਘਟਾਓ = (ਗੁਣਨਫਲ_neg[i] == State::S1);

			terms.ਧੱਕੋ_ਪਿੱਠ(ਪਦ);
		}
		log_assert(ਪੋਰਟ_a.ਮਾਪ() == ai);
		log_assert(ਪੋਰਟ_b.ਮਾਪ() == bi);

		ਪੂਰਨ naddends = cell->getParam(ID::NADDENDS).as_int();
		ਅਚਲ Const &addend_neg = cell->getParam(ID::ADDEND_NEGATED);
		ਅਚਲ Const &c_widths = cell->getParam(ID::C_WIDTHS);
		ਅਚਲ Const &c_signed = cell->getParam(ID::C_SIGNED);
		ਪੂਰਨ ci = 0;
		ਲਈ (ਪੂਰਨ i = 0; i < naddends; i++) {
			ਪਦ_t ਪਦ;

			ਪਦ.is_signed = (c_signed[i] == State::S1);
			ਪੂਰਨ c_ਚੌੜਾਈ = c_widths.ਕੱਢੋ_2(16 * i, 16).as_int(false);

			ਪਦ.ਵਿੱਚ_a = ਪੋਰਟ_c.ਕੱਢੋ_2(ci, c_ਚੌੜਾਈ);
			ci += c_ਚੌੜਾਈ;
			ਪਦ.do_ਘਟਾਓ = (addend_neg[i] == State::S1);

			terms.ਧੱਕੋ_ਪਿੱਠ(ਪਦ);
		}
		log_assert(ਪੋਰਟ_c.ਮਾਪ() == ci);
	}

	ਖਾਲੀ ਮੰਜ਼ਿਲ_cell(RTLIL::Cell *cell)
	{
		cell->type = ID($macc_v2);

		ਪੂਰਨ nproducts = 0, naddends = 0;
		Const a_signed, b_signed, a_widths, b_widths, ਗੁਣਨਫਲ_negated;
		Const c_signed, c_widths, addend_negated;
		SigSpec a, b, c;

		ਲਈ (ਪੂਰਨ i = 0; i < (ਪੂਰਨ) terms.ਮਾਪ(); i++) {
			SigSpec ਪਦ_a = terms[i].ਵਿੱਚ_a, ਪਦ_b = terms[i].ਵਿੱਚ_b;

			ਜੇ (ਪਦ_b.ਖਾਲੀ_ਥਾਵਾਂ()) {
				// addend
				c_widths.append(Const(ਪਦ_a.ਮਾਪ(), 16));
				c_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				addend_negated.append(terms[i].do_ਘਟਾਓ ? RTLIL::S1 : RTLIL::S0);
				c.append(ਪਦ_a);
				naddends++;
			} ਨਹੀਂਤਾਂ {
				//ਗੁਣਨਫਲ
				a_widths.append(Const(ਪਦ_a.ਮਾਪ(), 16));
				b_widths.append(Const(ਪਦ_b.ਮਾਪ(), 16));
				a_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				b_signed.append(terms[i].is_signed ? RTLIL::S1 : RTLIL::S0);
				ਗੁਣਨਫਲ_negated.append(terms[i].do_ਘਟਾਓ ? RTLIL::S1 : RTLIL::S0);
				a.append(ਪਦ_a);
				b.append(ਪਦ_b);
				nproducts++;
			}
		}

		ਜੇ (a_signed.ਖਾਲੀ_ਥਾਵਾਂ())
			a_signed = {RTLIL::Sx};
		ਜੇ (b_signed.ਖਾਲੀ_ਥਾਵਾਂ())
			b_signed = {RTLIL::Sx};
		ਜੇ (c_signed.ਖਾਲੀ_ਥਾਵਾਂ())
			c_signed = {RTLIL::Sx};
		ਜੇ (a_widths.ਖਾਲੀ_ਥਾਵਾਂ())
			a_widths = {RTLIL::Sx};
		ਜੇ (b_widths.ਖਾਲੀ_ਥਾਵਾਂ())
			b_widths = {RTLIL::Sx};
		ਜੇ (c_widths.ਖਾਲੀ_ਥਾਵਾਂ())
			c_widths = {RTLIL::Sx};
		ਜੇ (ਗੁਣਨਫਲ_negated.ਖਾਲੀ_ਥਾਵਾਂ())
			ਗੁਣਨਫਲ_negated = {RTLIL::Sx};
		ਜੇ (addend_negated.ਖਾਲੀ_ਥਾਵਾਂ())
			addend_negated = {RTLIL::Sx};

		cell->ਸੈੱਟ_Param(ID::NPRODUCTS, nproducts);
		cell->ਸੈੱਟ_Param(ID::ਗੁਣਨਫਲ_NEGATED, ਗੁਣਨਫਲ_negated);
		cell->ਸੈੱਟ_Param(ID::NADDENDS, naddends);
		cell->ਸੈੱਟ_Param(ID::ADDEND_NEGATED, addend_negated);
		cell->ਸੈੱਟ_Param(ID::A_SIGNED, a_signed);
		cell->ਸੈੱਟ_Param(ID::B_SIGNED, b_signed);
		cell->ਸੈੱਟ_Param(ID::C_SIGNED, c_signed);
		cell->ਸੈੱਟ_Param(ID::A_WIDTHS, a_widths);
		cell->ਸੈੱਟ_Param(ID::B_WIDTHS, b_widths);
		cell->ਸੈੱਟ_Param(ID::C_WIDTHS, c_widths);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::A, a);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::B, b);
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::C, c);
	}

	bool eval(RTLIL::Const &ਨਤੀਜਾ) ਅਚਲ
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : ਨਤੀਜਾ)
			ਦੋਆਧਾਰੀ_ਅੰਕ = State::S0;

		ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : terms)
		{
			ਜੇ (!ਪੋਰਟ.ਵਿੱਚ_a.is_fully_const() || !ਪੋਰਟ.ਵਿੱਚ_b.is_fully_const())
				ਵਾਪਸ false;

			RTLIL::Const summand;
			ਜੇ (Get_ਮਾਪ(ਪੋਰਟ.ਵਿੱਚ_b) == 0)
				summand = const_pos(ਪੋਰਟ.ਵਿੱਚ_a.as_const(), ਪੋਰਟ.ਵਿੱਚ_b.as_const(), ਪੋਰਟ.is_signed, ਪੋਰਟ.is_signed, Get_ਮਾਪ(ਨਤੀਜਾ));
			ਨਹੀਂਤਾਂ
				summand = const_mul(ਪੋਰਟ.ਵਿੱਚ_a.as_const(), ਪੋਰਟ.ਵਿੱਚ_b.as_const(), ਪੋਰਟ.is_signed, ਪੋਰਟ.is_signed, Get_ਮਾਪ(ਨਤੀਜਾ));

			ਜੇ (ਪੋਰਟ.do_ਘਟਾਓ)
				ਨਤੀਜਾ = const_sub(ਨਤੀਜਾ, summand, ਪੋਰਟ.is_signed, ਪੋਰਟ.is_signed, Get_ਮਾਪ(ਨਤੀਜਾ));
			ਨਹੀਂਤਾਂ
				ਨਤੀਜਾ = const_ਜੋੜੋ(ਨਤੀਜਾ, summand, ਪੋਰਟ.is_signed, ਪੋਰਟ.is_signed, Get_ਮਾਪ(ਨਤੀਜਾ));
		}

		ਵਾਪਸ true;
	}

	bool is_simple_ਗੁਣਨਫਲ()
	{
		ਵਾਪਸ terms.ਮਾਪ() == 1 &&
				!terms[0].ਵਿੱਚ_b.ਖਾਲੀ_ਥਾਵਾਂ() &&
				!terms[0].do_ਘਟਾਓ;
	}

	Macc(RTLIL::Cell *cell = nullptr)
	{
		ਜੇ (cell != nullptr)
			from_cell(cell);
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
