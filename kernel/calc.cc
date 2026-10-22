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

#شامل "kernel/yosys.h"
#شامل "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

ساکن خالی extend_u0(RTLIL::Const &arg, صحیح چوڑائی, منطقی is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	اگر (arg.سائز() > 0 && is_signed)
		padding = arg.عقب();

	arg.resize(چوڑائی, padding);
}

ساکن BigInteger const2big(ثابت RTLIL::Const &val, منطقی as_signed, صحیح &undef_ثنائی_ہندسہ_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::مثبت;
	State inv_sign_ثنائی_ہندسہ = RTLIL::State::S1;
	خودکار عدد_بٹ = val.سائز();

	اگر (as_signed && عدد_بٹ && val[عدد_بٹ-1] == RTLIL::State::S1) {
		inv_sign_ثنائی_ہندسہ = RTLIL::State::S0;
		sign = BigInteger::negative;
		عدد_بٹ--;
	}

	برائے (خودکار i = 0; i < عدد_بٹ; i++)
		اگر (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.سیٹ_ثنائی_ہندسہ(i, val[i] == inv_sign_ثنائی_ہندسہ);
		ورنہ اگر (undef_ثنائی_ہندسہ_pos < 0)
			undef_ثنائی_ہندسہ_pos = i;

	اگر (sign == BigInteger::negative)
		mag += 1;

	واپس BigInteger(mag, sign);
}

ساکن RTLIL::Const big2const(ثابت BigInteger &val, صحیح نتیجہ_len, صحیح undef_ثنائی_ہندسہ_pos)
{
	اگر (undef_ثنائی_ہندسہ_pos >= 0)
		واپس RTLIL::Const(RTLIL::State::Sx, نتیجہ_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const نتیجہ(0, نتیجہ_len);

	اگر (!mag.isZero())
	{
		اگر (val.getSign() < 0)
		{
			mag--;
			برائے (خودکار i = 0; i < نتیجہ_len; i++)
				نتیجہ.سیٹ(i, mag.get_ثنائی_ہندسہ(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		ورنہ
		{
			برائے (خودکار i = 0; i < نتیجہ_len; i++)
				نتیجہ.سیٹ(i, mag.get_ثنائی_ہندسہ(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#اگر 0
	اگر (undef_ثنائی_ہندسہ_pos >= 0)
		برائے (صحیح i = undef_ثنائی_ہندسہ_pos; i < نتیجہ_len; i++)
			نتیجہ[i] = RTLIL::State::Sx;
#اگر_ختم

	واپس نتیجہ;
}

ساکن RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	اگر (a == RTLIL::State::S0) واپس RTLIL::State::S0;
	اگر (b == RTLIL::State::S0) واپس RTLIL::State::S0;
	اگر (a != RTLIL::State::S1) واپس RTLIL::State::Sx;
	اگر (b != RTLIL::State::S1) واپس RTLIL::State::Sx;
	واپس RTLIL::State::S1;
}

ساکن RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	اگر (a == RTLIL::State::S1) واپس RTLIL::State::S1;
	اگر (b == RTLIL::State::S1) واپس RTLIL::State::S1;
	اگر (a != RTLIL::State::S0) واپس RTLIL::State::Sx;
	اگر (b != RTLIL::State::S0) واپس RTLIL::State::Sx;
	واپس RTLIL::State::S0;
}

ساکن RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	اگر (a != RTLIL::State::S0 && a != RTLIL::State::S1) واپس RTLIL::State::Sx;
	اگر (b != RTLIL::State::S0 && b != RTLIL::State::S1) واپس RTLIL::State::Sx;
	واپس a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

ساکن RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	اگر (a != RTLIL::State::S0 && a != RTLIL::State::S1) واپس RTLIL::State::Sx;
	اگر (b != RTLIL::State::S0 && b != RTLIL::State::S1) واپس RTLIL::State::Sx;
	واپس a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	اگر (نتیجہ_len < 0)
		نتیجہ_len = Get_سائز(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجہ_len, signed1);

	RTLIL::Const نتیجہ(RTLIL::State::Sx, نتیجہ_len);
	برائے (خودکار i = 0; i < نتیجہ_len; i++) {
		اگر (i >= Get_سائز(arg1_ext))
			نتیجہ.سیٹ(i, RTLIL::State::S0);
		ورنہ اگر (arg1_ext[i] == RTLIL::State::S0)
			نتیجہ.سیٹ(i, RTLIL::State::S1);
		ورنہ اگر (arg1_ext[i] == RTLIL::State::S1)
			نتیجہ.سیٹ(i, RTLIL::State::S0);
	}

	واپس نتیجہ;
}

ساکن RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len = -1)
{
	اگر (نتیجہ_len < 0)
		نتیجہ_len = max(Get_سائز(arg1), Get_سائز(arg2));

	extend_u0(arg1, نتیجہ_len, signed1);
	extend_u0(arg2, نتیجہ_len, signed2);

	RTLIL::Const نتیجہ(RTLIL::State::Sx, نتیجہ_len);
	برائے (خودکار i = 0; i < نتیجہ_len; i++) {
		RTLIL::State a = i < Get_سائز(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_سائز(arg2) ? arg2[i] : RTLIL::State::S0;
		نتیجہ.سیٹ(i, logic_func(a, b));
	}

	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	واپس logic_wrapper(logic_and, arg1, arg2, signed1, signed2, نتیجہ_len);
}

RTLIL::Const RTLIL::const_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	واپس logic_wrapper(logic_or, arg1, arg2, signed1, signed2, نتیجہ_len);
}

RTLIL::Const RTLIL::const_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	واپس logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, نتیجہ_len);
}

RTLIL::Const RTLIL::const_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	واپس logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, نتیجہ_len);
}

ساکن RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ثابت RTLIL::Const &arg1, صحیح نتیجہ_len)
{
	RTLIL::State عارضی = initial;

	برائے (خودکار i = 0; i < arg1.سائز(); i++)
		عارضی = logic_func(عارضی, arg1[i]);

	RTLIL::Const نتیجہ(عارضی);
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_reduce_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی, منطقی, صحیح نتیجہ_len)
{
	واپس logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_reduce_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی, منطقی, صحیح نتیجہ_len)
{
	واپس logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی, منطقی, صحیح نتیجہ_len)
{
	واپس logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی, منطقی, صحیح نتیجہ_len)
{
	RTLIL::Const بفر = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, نتیجہ_len);
	اگر (!بفر.خالی_خانے()) {
		اگر (بفر.front() == RTLIL::State::S0)
			بفر.سیٹ(0, RTLIL::State::S1);
		ورنہ اگر (بفر.front() == RTLIL::State::S1)
			بفر.سیٹ(0, RTLIL::State::S0);
	}
	واپس بفر;
}

RTLIL::Const RTLIL::const_reduce_bool(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی, منطقی, صحیح نتیجہ_len)
{
	واپس logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_logic_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos_a);
	RTLIL::Const نتیجہ(a.isZero() ? undef_ثنائی_ہندسہ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_logic_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos_a = -1, undef_ثنائی_ہندسہ_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos_b);

	RTLIL::State ثنائی_ہندسہ_a = a.isZero() ? undef_ثنائی_ہندسہ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ثنائی_ہندسہ_b = b.isZero() ? undef_ثنائی_ہندسہ_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const نتیجہ(logic_and(ثنائی_ہندسہ_a, ثنائی_ہندسہ_b));
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_logic_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos_a = -1, undef_ثنائی_ہندسہ_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos_b);

	RTLIL::State ثنائی_ہندسہ_a = a.isZero() ? undef_ثنائی_ہندسہ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ثنائی_ہندسہ_b = b.isZero() ? undef_ثنائی_ہندسہ_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const نتیجہ(logic_or(ثنائی_ہندسہ_a, ثنائی_ہندسہ_b));
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ساکن RTLIL::Const const_سرکاؤ_کارکن(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی sign_ext, منطقی signed2, صحیح direction, صحیح نتیجہ_len, RTLIL::State vacant_بٹ = RTLIL::State::S0)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger انحراف = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos) * direction;

	اگر (نتیجہ_len < 0)
		نتیجہ_len = Get_سائز(arg1);

	RTLIL::Const نتیجہ(RTLIL::State::Sx, نتیجہ_len);
	اگر (undef_ثنائی_ہندسہ_pos >= 0)
		واپس نتیجہ;

	برائے (صحیح i = 0; i < نتیجہ_len; i++) {
		BigInteger pos = BigInteger(i) + انحراف;
		اگر (pos < 0)
			نتیجہ.سیٹ(i, vacant_بٹ);
		ورنہ اگر (pos >= BigInteger(Get_سائز(arg1)))
			نتیجہ.سیٹ(i, sign_ext && !arg1.خالی_خانے() ? arg1.عقب() : vacant_بٹ);
		ورنہ
			نتیجہ.سیٹ(i, arg1[pos.مقصد_Int()]);
	}

	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_shl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجہ_len, signed1);
	واپس const_سرکاؤ_کارکن(arg1_ext, arg2, جھوٹ, جھوٹ, -1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_shr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(نتیجہ_len, Get_سائز(arg1)), signed1);
	واپس const_سرکاؤ_کارکن(arg1_ext, arg2, جھوٹ, جھوٹ, +1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_sshl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	واپس const_سرکاؤ_کارکن(arg1, arg2, signed1, جھوٹ, -1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_sshr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	واپس const_سرکاؤ_کارکن(arg1, arg2, signed1, جھوٹ, +1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_سرکاؤ(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(نتیجہ_len, Get_سائز(arg1)), signed1);
	واپس const_سرکاؤ_کارکن(arg1_ext, arg2, جھوٹ, signed2, +1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_shiftx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی, منطقی signed2, صحیح نتیجہ_len)
{
	واپس const_سرکاؤ_کارکن(arg1, arg2, جھوٹ, signed2, +1, نتیجہ_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	منطقی y = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos) < const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	RTLIL::Const نتیجہ(undef_ثنائی_ہندسہ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_le(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	منطقی y = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos) <= const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	RTLIL::Const نتیجہ(undef_ثنائی_ہندسہ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_eq(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const نتیجہ(RTLIL::State::S0, نتیجہ_len);

	صحیح چوڑائی = max(Get_سائز(arg1_ext), Get_سائز(arg2_ext));
	extend_u0(arg1_ext, چوڑائی, signed1 && signed2);
	extend_u0(arg2_ext, چوڑائی, signed1 && signed2);

	RTLIL::State matched_حالت = RTLIL::State::S1;
	برائے (خودکار i = 0; i < arg1_ext.سائز(); i++) {
		اگر (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			واپس نتیجہ;
		اگر (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			واپس نتیجہ;
		اگر (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_حالت = RTLIL::State::Sx;
	}

	نتیجہ.سیٹ(0, matched_حالت);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_ne(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	RTLIL::Const نتیجہ = RTLIL::const_eq(arg1, arg2, signed1, signed2, نتیجہ_len);
	اگر (نتیجہ.front() == RTLIL::State::S0)
		نتیجہ.سیٹ(0, RTLIL::State::S1);
	ورنہ اگر (نتیجہ.front() == RTLIL::State::S1)
		نتیجہ.سیٹ(0, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_eqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const نتیجہ(RTLIL::State::S0, نتیجہ_len);

	صحیح چوڑائی = max(Get_سائز(arg1_ext), Get_سائز(arg2_ext));
	extend_u0(arg1_ext, چوڑائی, signed1 && signed2);
	extend_u0(arg2_ext, چوڑائی, signed1 && signed2);

	برائے (خودکار i = 0; i < arg1_ext.سائز(); i++) {
		اگر (arg1_ext.at(i) != arg2_ext.at(i))
			واپس نتیجہ;
	}

	نتیجہ.سیٹ(0, RTLIL::State::S1);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_nex(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	RTLIL::Const نتیجہ = RTLIL::const_eqx(arg1, arg2, signed1, signed2, نتیجہ_len);
	اگر (نتیجہ.front() == RTLIL::State::S0)
		نتیجہ.سیٹ(0, RTLIL::State::S1);
	ورنہ اگر (نتیجہ.front() == RTLIL::State::S1)
		نتیجہ.سیٹ(0, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_ge(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	منطقی y = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos) >= const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	RTLIL::Const نتیجہ(undef_ثنائی_ہندسہ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_gt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	منطقی y = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos) > const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	RTLIL::Const نتیجہ(undef_ثنائی_ہندسہ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	اگر (Get_سائز(نتیجہ) < نتیجہ_len)
		نتیجہ.resize(نتیجہ_len, RTLIL::State::S0);
	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_جوڑو(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos) + const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	واپس big2const(y, نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), undef_ثنائی_ہندسہ_pos);
}

RTLIL::Const RTLIL::const_sub(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos) - const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	واپس big2const(y, نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), undef_ثنائی_ہندسہ_pos);
}

RTLIL::Const RTLIL::const_mul(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos) * const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	واپس big2const(y, نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), min(undef_ثنائی_ہندسہ_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	اگر (b.isZero())
		واپس RTLIL::Const(RTLIL::State::Sx, نتیجہ_len);
	منطقی نتیجہ_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	واپس big2const(نتیجہ_neg ? -(a / b) : (a / b), نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), min(undef_ثنائی_ہندسہ_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	اگر (b.isZero())
		واپس RTLIL::Const(RTLIL::State::Sx, نتیجہ_len);
	منطقی نتیجہ_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	واپس big2const(نتیجہ_neg ? -(a % b) : (a % b), نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), min(undef_ثنائی_ہندسہ_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	اگر (b.isZero())
		واپس RTLIL::Const(RTLIL::State::Sx, نتیجہ_len);

	منطقی نتیجہ_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger نتیجہ;

	اگر (نتیجہ_pos || a == 0) {
		نتیجہ = a / b;
	} ورنہ {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		نتیجہ = -((a + b - 1) / b);
	}
	واپس big2const(نتیجہ, نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), min(undef_ثنائی_ہندسہ_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	اگر (b.isZero())
		واپس RTLIL::Const(RTLIL::State::Sx, نتیجہ_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	اگر (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} ورنہ {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	واپس big2const(modulo, نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), min(undef_ثنائی_ہندسہ_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقی signed1, منطقی signed2, صحیح نتیجہ_len)
{
	صحیح undef_ثنائی_ہندسہ_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_ثنائی_ہندسہ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ثنائی_ہندسہ_pos);
	BigInteger y = 1;

	اگر (a == 0 && b < 0)
		واپس RTLIL::Const(RTLIL::State::Sx, نتیجہ_len);

	اگر (a == 0 && b > 0)
		واپس RTLIL::Const(RTLIL::State::S0, نتیجہ_len);

	اگر (b < 0)
	{
		اگر (a < -1 || a > 1)
			y = 0;
		اگر (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	اگر (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		صحیح modulus_بٹ = (نتیجہ_len >= 0 ? نتیجہ_len : 1024);
		برائے (صحیح i = 0; i < modulus_بٹ; i++)
			modulus *= 2;

		منطقی flip_نتیجہ_sign = جھوٹ;
		اگر (a < 0) {
			a *= -1;
			اگر (b % 2 == 1)
				flip_نتیجہ_sign = سچ;
		}

		جب (b > 0) {
			اگر (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		اگر (flip_نتیجہ_sign)
			y *= -1;
	}

	واپس big2const(y, نتیجہ_len >= 0 ? نتیجہ_len : max(Get_سائز(arg1), Get_سائز(arg2)), min(undef_ثنائی_ہندسہ_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجہ_len, signed1);

	واپس arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجہ_len, signed1);

	واپس arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقی signed1, منطقی, صحیح نتیجہ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	واپس RTLIL::const_sub(zero, arg1_ext, سچ, signed1, نتیجہ_len);
}

RTLIL::Const RTLIL::const_mux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.سائز() == arg1.سائز());
	اگر (arg3[0] == State::S0)
		واپس arg1;
	ورنہ اگر (arg3[0] == State::S1)
		واپس arg2;

	RTLIL::Const ret = arg1;
	برائے (خودکار i = 0; i < ret.سائز(); i++)
		اگر (ret[i] != arg2[i])
			ret.سیٹ(i, State::Sx);
	واپس ret;
}

RTLIL::Const RTLIL::const_pmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	اگر (arg3.is_fully_zero())
		واپس arg1;

	اگر (!arg3.is_onehot())
		واپس RTLIL::Const(State::Sx, arg1.سائز());

	برائے (خودکار i = 0; i < arg3.سائز(); i++)
		اگر (arg3[i] == State::S1)
			واپس RTLIL::Const(std::سمتیہ<RTLIL::State>(arg2.begin() + i*arg1.سائز(), arg2.begin() + (i+1)*arg1.سائز()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	std::سمتیہ<State> t = arg1.مقصد_بٹ();

	برائے (صحیح i = Get_سائز(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::سمتیہ<RTLIL::State> new_t;
		اگر (sel == State::S0)
			new_t = std::سمتیہ<RTLIL::State>(t.begin(), t.begin() + Get_سائز(t)/2);
		ورنہ اگر (sel == State::S1)
			new_t = std::سمتیہ<RTLIL::State>(t.begin() + Get_سائز(t)/2, t.انتہا());
		ورنہ
			برائے (صحیح j = 0; j < Get_سائز(t)/2; j++)
				new_t.ڈالو_عقب(t[j] == t[j + Get_سائز(t)/2] ? t[j] : RTLIL::Sx);
		t.تبادلہ(new_t);
	}

	واپس t;
}

RTLIL::Const RTLIL::const_demux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	صحیح چوڑائی = Get_سائز(arg1);
	صحیح s_چوڑائی = Get_سائز(arg2);
	std::سمتیہ<RTLIL::State> res;
	برائے (صحیح i = 0; i < (1 << s_چوڑائی); i++)
	{
		منطقی ne = جھوٹ;
		منطقی x = جھوٹ;
		برائے (صحیح j = 0; j < s_چوڑائی; j++) {
			منطقی ثنائی_ہندسہ = i & 1 << j;
			اگر (arg2[j] == (ثنائی_ہندسہ ? RTLIL::S0 : RTLIL::S1))
				ne = سچ;
			ورنہ اگر (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = سچ;
		}
		اگر (ne) {
			برائے (صحیح j = 0; j < چوڑائی; j++)
				res.ڈالو_عقب(State::S0);
		} ورنہ اگر (x) {
			برائے (صحیح j = 0; j < چوڑائی; j++)
				res.ڈالو_عقب(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} ورنہ {
			برائے (صحیح j = 0; j < چوڑائی; j++)
				res.ڈالو_عقب(arg1[j]);
		}
	}
	واپس res;
}

RTLIL::Const RTLIL::const_bweqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	log_assert(arg2.سائز() == arg1.سائز());
	RTLIL::Const نتیجہ(RTLIL::State::S0, arg1.سائز());
	برائے (خودکار i = 0; i < arg1.سائز(); i++)
		نتیجہ.سیٹ(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	واپس نتیجہ;
}

RTLIL::Const RTLIL::const_bwmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.سائز() == arg1.سائز());
	log_assert(arg3.سائز() == arg1.سائز());
	RTLIL::Const نتیجہ(RTLIL::State::Sx, arg1.سائز());
	برائے (خودکار i = 0; i < arg1.سائز(); i++) {
		اگر (arg3[i] != State::Sx || arg1[i] == arg2[i])
			نتیجہ.سیٹ(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	واپس نتیجہ;
}

YOSYS_NAMESPACE_انتہا
