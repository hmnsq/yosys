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

// [[CITE]] Power-Modulus Algorithm
// Schneier, Bruce (1996). Applied Cryptography: Protocols, Algorithms, and Source Code in C,
// Second Edition (2nd ed.). Wiley. ISBN 978-0-471-11709-4, page 244

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

स्थिर शून्य extend_u0(RTLIL::Const &arg, पूर्णाङ्क विस्तारः, तार्किक is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	यदि (arg.आकार() > 0 && is_signed)
		padding = arg.पृष्ठभागः();

	arg.resize(विस्तारः, padding);
}

स्थिर BigInteger const2big(ध्रुव RTLIL::Const &val, तार्किक as_signed, पूर्णाङ्क &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::धनात्मकम्;
	State inv_sign_bit = RTLIL::State::S1;
	स्वचालित अङ्कः_द्वयङ्काः = val.आकार();

	यदि (as_signed && अङ्कः_द्वयङ्काः && val[अङ्कः_द्वयङ्काः-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		अङ्कः_द्वयङ्काः--;
	}

	प्रति (स्वचालित i = 0; i < अङ्कः_द्वयङ्काः; i++)
		यदि (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.समुच्चयः_Bit(i, val[i] == inv_sign_bit);
		अन्यथा यदि (undef_bit_pos < 0)
			undef_bit_pos = i;

	यदि (sign == BigInteger::negative)
		mag += 1;

	प्रत्यागम BigInteger(mag, sign);
}

स्थिर RTLIL::Const big2const(ध्रुव BigInteger &val, पूर्णाङ्क परिणाम_len, पूर्णाङ्क undef_bit_pos)
{
	यदि (undef_bit_pos >= 0)
		प्रत्यागम RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const परिणाम(0, परिणाम_len);

	यदि (!mag.isZero())
	{
		यदि (val.getSign() < 0)
		{
			mag--;
			प्रति (स्वचालित i = 0; i < परिणाम_len; i++)
				परिणाम.समुच्चयः(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		अन्यथा
		{
			प्रति (स्वचालित i = 0; i < परिणाम_len; i++)
				परिणाम.समुच्चयः(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#यदि 0
	यदि (undef_bit_pos >= 0)
		प्रति (पूर्णाङ्क i = undef_bit_pos; i < परिणाम_len; i++)
			परिणाम[i] = RTLIL::State::Sx;
#यदि_अन्तः

	प्रत्यागम परिणाम;
}

स्थिर RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	यदि (a == RTLIL::State::S0) प्रत्यागम RTLIL::State::S0;
	यदि (b == RTLIL::State::S0) प्रत्यागम RTLIL::State::S0;
	यदि (a != RTLIL::State::S1) प्रत्यागम RTLIL::State::Sx;
	यदि (b != RTLIL::State::S1) प्रत्यागम RTLIL::State::Sx;
	प्रत्यागम RTLIL::State::S1;
}

स्थिर RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	यदि (a == RTLIL::State::S1) प्रत्यागम RTLIL::State::S1;
	यदि (b == RTLIL::State::S1) प्रत्यागम RTLIL::State::S1;
	यदि (a != RTLIL::State::S0) प्रत्यागम RTLIL::State::Sx;
	यदि (b != RTLIL::State::S0) प्रत्यागम RTLIL::State::Sx;
	प्रत्यागम RTLIL::State::S0;
}

स्थिर RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	यदि (a != RTLIL::State::S0 && a != RTLIL::State::S1) प्रत्यागम RTLIL::State::Sx;
	यदि (b != RTLIL::State::S0 && b != RTLIL::State::S1) प्रत्यागम RTLIL::State::Sx;
	प्रत्यागम a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

स्थिर RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	यदि (a != RTLIL::State::S0 && a != RTLIL::State::S1) प्रत्यागम RTLIL::State::Sx;
	यदि (b != RTLIL::State::S0 && b != RTLIL::State::S1) प्रत्यागम RTLIL::State::Sx;
	प्रत्यागम a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	यदि (परिणाम_len < 0)
		परिणाम_len = Get_आकार(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);

	RTLIL::Const परिणाम(RTLIL::State::Sx, परिणाम_len);
	प्रति (स्वचालित i = 0; i < परिणाम_len; i++) {
		यदि (i >= Get_आकार(arg1_ext))
			परिणाम.समुच्चयः(i, RTLIL::State::S0);
		अन्यथा यदि (arg1_ext[i] == RTLIL::State::S0)
			परिणाम.समुच्चयः(i, RTLIL::State::S1);
		अन्यथा यदि (arg1_ext[i] == RTLIL::State::S1)
			परिणाम.समुच्चयः(i, RTLIL::State::S0);
	}

	प्रत्यागम परिणाम;
}

स्थिर RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len = -1)
{
	यदि (परिणाम_len < 0)
		परिणाम_len = max(Get_आकार(arg1), Get_आकार(arg2));

	extend_u0(arg1, परिणाम_len, signed1);
	extend_u0(arg2, परिणाम_len, signed2);

	RTLIL::Const परिणाम(RTLIL::State::Sx, परिणाम_len);
	प्रति (स्वचालित i = 0; i < परिणाम_len; i++) {
		RTLIL::State a = i < Get_आकार(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_आकार(arg2) ? arg2[i] : RTLIL::State::S0;
		परिणाम.समुच्चयः(i, logic_func(a, b));
	}

	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_and(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_wrapper(logic_and, arg1, arg2, signed1, signed2, परिणाम_len);
}

RTLIL::Const RTLIL::const_or(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_wrapper(logic_or, arg1, arg2, signed1, signed2, परिणाम_len);
}

RTLIL::Const RTLIL::const_xor(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, परिणाम_len);
}

RTLIL::Const RTLIL::const_xnor(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, परिणाम_len);
}

स्थिर RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ध्रुव RTLIL::Const &arg1, पूर्णाङ्क परिणाम_len)
{
	RTLIL::State अस्थायी = initial;

	प्रति (स्वचालित i = 0; i < arg1.आकार(); i++)
		अस्थायी = logic_func(अस्थायी, arg1[i]);

	RTLIL::Const परिणाम(अस्थायी);
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_reduce_and(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक, तार्किक, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_reduce_or(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक, तार्किक, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक, तार्किक, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक, तार्किक, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const बफर = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, परिणाम_len);
	यदि (!बफर.रिक्तानि()) {
		यदि (बफर.front() == RTLIL::State::S0)
			बफर.समुच्चयः(0, RTLIL::State::S1);
		अन्यथा यदि (बफर.front() == RTLIL::State::S1)
			बफर.समुच्चयः(0, RTLIL::State::S0);
	}
	प्रत्यागम बफर;
}

RTLIL::Const RTLIL::const_reduce_bool(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक, तार्किक, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_logic_not(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const परिणाम(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_logic_and(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const परिणाम(logic_and(bit_a, bit_b));
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_logic_or(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const परिणाम(logic_or(bit_a, bit_b));
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
स्थिर RTLIL::Const const_सरणम्_कार्यकर्ता(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक sign_ext, तार्किक signed2, पूर्णाङ्क direction, पूर्णाङ्क परिणाम_len, RTLIL::State vacant_द्वयङ्काः = RTLIL::State::S0)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger विस्थापनम् = const2big(arg2, signed2, undef_bit_pos) * direction;

	यदि (परिणाम_len < 0)
		परिणाम_len = Get_आकार(arg1);

	RTLIL::Const परिणाम(RTLIL::State::Sx, परिणाम_len);
	यदि (undef_bit_pos >= 0)
		प्रत्यागम परिणाम;

	प्रति (पूर्णाङ्क i = 0; i < परिणाम_len; i++) {
		BigInteger pos = BigInteger(i) + विस्थापनम्;
		यदि (pos < 0)
			परिणाम.समुच्चयः(i, vacant_द्वयङ्काः);
		अन्यथा यदि (pos >= BigInteger(Get_आकार(arg1)))
			परिणाम.समुच्चयः(i, sign_ext && !arg1.रिक्तानि() ? arg1.पृष्ठभागः() : vacant_द्वयङ्काः);
		अन्यथा
			परिणाम.समुच्चयः(i, arg1[pos.गन्तव्यम्_Int()]);
	}

	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_shl(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);
	प्रत्यागम const_सरणम्_कार्यकर्ता(arg1_ext, arg2, असत्य, असत्य, -1, परिणाम_len);
}

RTLIL::Const RTLIL::const_shr(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(परिणाम_len, Get_आकार(arg1)), signed1);
	प्रत्यागम const_सरणम्_कार्यकर्ता(arg1_ext, arg2, असत्य, असत्य, +1, परिणाम_len);
}

RTLIL::Const RTLIL::const_sshl(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम const_सरणम्_कार्यकर्ता(arg1, arg2, signed1, असत्य, -1, परिणाम_len);
}

RTLIL::Const RTLIL::const_sshr(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम const_सरणम्_कार्यकर्ता(arg1, arg2, signed1, असत्य, +1, परिणाम_len);
}

RTLIL::Const RTLIL::const_सरणम्(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(परिणाम_len, Get_आकार(arg1)), signed1);
	प्रत्यागम const_सरणम्_कार्यकर्ता(arg1_ext, arg2, असत्य, signed2, +1, परिणाम_len);
}

RTLIL::Const RTLIL::const_shiftx(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	प्रत्यागम const_सरणम्_कार्यकर्ता(arg1, arg2, असत्य, signed2, +1, परिणाम_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_le(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_eq(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const परिणाम(RTLIL::State::S0, परिणाम_len);

	पूर्णाङ्क विस्तारः = max(Get_आकार(arg1_ext), Get_आकार(arg2_ext));
	extend_u0(arg1_ext, विस्तारः, signed1 && signed2);
	extend_u0(arg2_ext, विस्तारः, signed1 && signed2);

	RTLIL::State matched_स्थितिः = RTLIL::State::S1;
	प्रति (स्वचालित i = 0; i < arg1_ext.आकार(); i++) {
		यदि (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			प्रत्यागम परिणाम;
		यदि (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			प्रत्यागम परिणाम;
		यदि (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_स्थितिः = RTLIL::State::Sx;
	}

	परिणाम.समुच्चयः(0, matched_स्थितिः);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_ne(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const परिणाम = RTLIL::const_eq(arg1, arg2, signed1, signed2, परिणाम_len);
	यदि (परिणाम.front() == RTLIL::State::S0)
		परिणाम.समुच्चयः(0, RTLIL::State::S1);
	अन्यथा यदि (परिणाम.front() == RTLIL::State::S1)
		परिणाम.समुच्चयः(0, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_eqx(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const परिणाम(RTLIL::State::S0, परिणाम_len);

	पूर्णाङ्क विस्तारः = max(Get_आकार(arg1_ext), Get_आकार(arg2_ext));
	extend_u0(arg1_ext, विस्तारः, signed1 && signed2);
	extend_u0(arg2_ext, विस्तारः, signed1 && signed2);

	प्रति (स्वचालित i = 0; i < arg1_ext.आकार(); i++) {
		यदि (arg1_ext.at(i) != arg2_ext.at(i))
			प्रत्यागम परिणाम;
	}

	परिणाम.समुच्चयः(0, RTLIL::State::S1);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_nex(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const परिणाम = RTLIL::const_eqx(arg1, arg2, signed1, signed2, परिणाम_len);
	यदि (परिणाम.front() == RTLIL::State::S0)
		परिणाम.समुच्चयः(0, RTLIL::State::S1);
	अन्यथा यदि (परिणाम.front() == RTLIL::State::S1)
		परिणाम.समुच्चयः(0, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_ge(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_gt(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	यदि (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_सङ्कलय(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	प्रत्यागम big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	प्रत्यागम big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	प्रत्यागम big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	यदि (b.isZero())
		प्रत्यागम RTLIL::Const(RTLIL::State::Sx, परिणाम_len);
	तार्किक परिणाम_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	प्रत्यागम big2const(परिणाम_neg ? -(a / b) : (a / b), परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	यदि (b.isZero())
		प्रत्यागम RTLIL::Const(RTLIL::State::Sx, परिणाम_len);
	तार्किक परिणाम_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	प्रत्यागम big2const(परिणाम_neg ? -(a % b) : (a % b), परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	यदि (b.isZero())
		प्रत्यागम RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	तार्किक परिणाम_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger परिणाम;

	यदि (परिणाम_pos || a == 0) {
		परिणाम = a / b;
	} अन्यथा {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		परिणाम = -((a + b - 1) / b);
	}
	प्रत्यागम big2const(परिणाम, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	यदि (b.isZero())
		प्रत्यागम RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	यदि (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} अन्यथा {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	प्रत्यागम big2const(modulo, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णाङ्क परिणाम_len)
{
	पूर्णाङ्क undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	यदि (a == 0 && b < 0)
		प्रत्यागम RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	यदि (a == 0 && b > 0)
		प्रत्यागम RTLIL::Const(RTLIL::State::S0, परिणाम_len);

	यदि (b < 0)
	{
		यदि (a < -1 || a > 1)
			y = 0;
		यदि (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	यदि (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		पूर्णाङ्क modulus_द्वयङ्काः = (परिणाम_len >= 0 ? परिणाम_len : 1024);
		प्रति (पूर्णाङ्क i = 0; i < modulus_द्वयङ्काः; i++)
			modulus *= 2;

		तार्किक flip_परिणाम_sign = असत्य;
		यदि (a < 0) {
			a *= -1;
			यदि (b % 2 == 1)
				flip_परिणाम_sign = सत्य;
		}

		यावत् (b > 0) {
			यदि (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		यदि (flip_परिणाम_sign)
			y *= -1;
	}

	प्रत्यागम big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);

	प्रत्यागम arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);

	प्रत्यागम arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णाङ्क परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	प्रत्यागम RTLIL::const_sub(zero, arg1_ext, सत्य, signed1, परिणाम_len);
}

RTLIL::Const RTLIL::const_mux(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3)
{
	log_assert(arg2.आकार() == arg1.आकार());
	यदि (arg3[0] == State::S0)
		प्रत्यागम arg1;
	अन्यथा यदि (arg3[0] == State::S1)
		प्रत्यागम arg2;

	RTLIL::Const ret = arg1;
	प्रति (स्वचालित i = 0; i < ret.आकार(); i++)
		यदि (ret[i] != arg2[i])
			ret.समुच्चयः(i, State::Sx);
	प्रत्यागम ret;
}

RTLIL::Const RTLIL::const_pmux(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3)
{
	यदि (arg3.is_fully_zero())
		प्रत्यागम arg1;

	यदि (!arg3.is_onehot())
		प्रत्यागम RTLIL::Const(State::Sx, arg1.आकार());

	प्रति (स्वचालित i = 0; i < arg3.आकार(); i++)
		यदि (arg3[i] == State::S1)
			प्रत्यागम RTLIL::Const(std::सदिशम्<RTLIL::State>(arg2.begin() + i*arg1.आकार(), arg2.begin() + (i+1)*arg1.आकार()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2)
{
	std::सदिशम्<State> t = arg1.गन्तव्यम्_द्वयङ्काः();

	प्रति (पूर्णाङ्क i = Get_आकार(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::सदिशम्<RTLIL::State> new_t;
		यदि (sel == State::S0)
			new_t = std::सदिशम्<RTLIL::State>(t.begin(), t.begin() + Get_आकार(t)/2);
		अन्यथा यदि (sel == State::S1)
			new_t = std::सदिशम्<RTLIL::State>(t.begin() + Get_आकार(t)/2, t.अन्तः());
		अन्यथा
			प्रति (पूर्णाङ्क j = 0; j < Get_आकार(t)/2; j++)
				new_t.प्रक्षिप_पृष्ठभागः(t[j] == t[j + Get_आकार(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	प्रत्यागम t;
}

RTLIL::Const RTLIL::const_demux(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2)
{
	पूर्णाङ्क विस्तारः = Get_आकार(arg1);
	पूर्णाङ्क s_विस्तारः = Get_आकार(arg2);
	std::सदिशम्<RTLIL::State> res;
	प्रति (पूर्णाङ्क i = 0; i < (1 << s_विस्तारः); i++)
	{
		तार्किक ne = असत्य;
		तार्किक x = असत्य;
		प्रति (पूर्णाङ्क j = 0; j < s_विस्तारः; j++) {
			तार्किक bit = i & 1 << j;
			यदि (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = सत्य;
			अन्यथा यदि (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = सत्य;
		}
		यदि (ne) {
			प्रति (पूर्णाङ्क j = 0; j < विस्तारः; j++)
				res.प्रक्षिप_पृष्ठभागः(State::S0);
		} अन्यथा यदि (x) {
			प्रति (पूर्णाङ्क j = 0; j < विस्तारः; j++)
				res.प्रक्षिप_पृष्ठभागः(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} अन्यथा {
			प्रति (पूर्णाङ्क j = 0; j < विस्तारः; j++)
				res.प्रक्षिप_पृष्ठभागः(arg1[j]);
		}
	}
	प्रत्यागम res;
}

RTLIL::Const RTLIL::const_bweqx(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2)
{
	log_assert(arg2.आकार() == arg1.आकार());
	RTLIL::Const परिणाम(RTLIL::State::S0, arg1.आकार());
	प्रति (स्वचालित i = 0; i < arg1.आकार(); i++)
		परिणाम.समुच्चयः(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	प्रत्यागम परिणाम;
}

RTLIL::Const RTLIL::const_bwmux(ध्रुव RTLIL::Const &arg1, ध्रुव RTLIL::Const &arg2, ध्रुव RTLIL::Const &arg3)
{
	log_assert(arg2.आकार() == arg1.आकार());
	log_assert(arg3.आकार() == arg1.आकार());
	RTLIL::Const परिणाम(RTLIL::State::Sx, arg1.आकार());
	प्रति (स्वचालित i = 0; i < arg1.आकार(); i++) {
		यदि (arg3[i] != State::Sx || arg1[i] == arg2[i])
			परिणाम.समुच्चयः(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	प्रत्यागम परिणाम;
}

YOSYS_NAMESPACE_अन्तः
