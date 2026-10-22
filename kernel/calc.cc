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

#ضمن "kernel/yosys.h"
#ضمن "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

ساكن فراغ extend_u0(RTLIL::Const &arg, صحيح عرض, منطقي is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	إذا (arg.اندازه() > 0 && is_signed)
		padding = arg.پشت();

	arg.resize(عرض, padding);
}

ساكن BigInteger const2big(ثابت RTLIL::Const &val, منطقي as_signed, صحيح &undef_بیت_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::مثبت;
	State inv_sign_بیت = RTLIL::State::S1;
	خودکار رقم_بیتها = val.اندازه();

	إذا (as_signed && رقم_بیتها && val[رقم_بیتها-1] == RTLIL::State::S1) {
		inv_sign_بیت = RTLIL::State::S0;
		sign = BigInteger::negative;
		رقم_بیتها--;
	}

	لكل (خودکار i = 0; i < رقم_بیتها; i++)
		إذا (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.مجموعه_بیت(i, val[i] == inv_sign_بیت);
		وإلا إذا (undef_بیت_pos < 0)
			undef_بیت_pos = i;

	إذا (sign == BigInteger::negative)
		mag += 1;

	أرجع BigInteger(mag, sign);
}

ساكن RTLIL::Const big2const(ثابت BigInteger &val, صحيح نتیجه_len, صحيح undef_بیت_pos)
{
	إذا (undef_بیت_pos >= 0)
		أرجع RTLIL::Const(RTLIL::State::Sx, نتیجه_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const نتیجه(0, نتیجه_len);

	إذا (!mag.isZero())
	{
		إذا (val.getSign() < 0)
		{
			mag--;
			لكل (خودکار i = 0; i < نتیجه_len; i++)
				نتیجه.مجموعه(i, mag.get_بیت(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		وإلا
		{
			لكل (خودکار i = 0; i < نتیجه_len; i++)
				نتیجه.مجموعه(i, mag.get_بیت(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#إذا 0
	إذا (undef_بیت_pos >= 0)
		لكل (صحيح i = undef_بیت_pos; i < نتیجه_len; i++)
			نتیجه[i] = RTLIL::State::Sx;
#پایان_اگر

	أرجع نتیجه;
}

ساكن RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	إذا (a == RTLIL::State::S0) أرجع RTLIL::State::S0;
	إذا (b == RTLIL::State::S0) أرجع RTLIL::State::S0;
	إذا (a != RTLIL::State::S1) أرجع RTLIL::State::Sx;
	إذا (b != RTLIL::State::S1) أرجع RTLIL::State::Sx;
	أرجع RTLIL::State::S1;
}

ساكن RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	إذا (a == RTLIL::State::S1) أرجع RTLIL::State::S1;
	إذا (b == RTLIL::State::S1) أرجع RTLIL::State::S1;
	إذا (a != RTLIL::State::S0) أرجع RTLIL::State::Sx;
	إذا (b != RTLIL::State::S0) أرجع RTLIL::State::Sx;
	أرجع RTLIL::State::S0;
}

ساكن RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	إذا (a != RTLIL::State::S0 && a != RTLIL::State::S1) أرجع RTLIL::State::Sx;
	إذا (b != RTLIL::State::S0 && b != RTLIL::State::S1) أرجع RTLIL::State::Sx;
	أرجع a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

ساكن RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	إذا (a != RTLIL::State::S0 && a != RTLIL::State::S1) أرجع RTLIL::State::Sx;
	إذا (b != RTLIL::State::S0 && b != RTLIL::State::S1) أرجع RTLIL::State::Sx;
	أرجع a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	إذا (نتیجه_len < 0)
		نتیجه_len = Get_اندازه(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجه_len, signed1);

	RTLIL::Const نتیجه(RTLIL::State::Sx, نتیجه_len);
	لكل (خودکار i = 0; i < نتیجه_len; i++) {
		إذا (i >= Get_اندازه(arg1_ext))
			نتیجه.مجموعه(i, RTLIL::State::S0);
		وإلا إذا (arg1_ext[i] == RTLIL::State::S0)
			نتیجه.مجموعه(i, RTLIL::State::S1);
		وإلا إذا (arg1_ext[i] == RTLIL::State::S1)
			نتیجه.مجموعه(i, RTLIL::State::S0);
	}

	أرجع نتیجه;
}

ساكن RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len = -1)
{
	إذا (نتیجه_len < 0)
		نتیجه_len = max(Get_اندازه(arg1), Get_اندازه(arg2));

	extend_u0(arg1, نتیجه_len, signed1);
	extend_u0(arg2, نتیجه_len, signed2);

	RTLIL::Const نتیجه(RTLIL::State::Sx, نتیجه_len);
	لكل (خودکار i = 0; i < نتیجه_len; i++) {
		RTLIL::State a = i < Get_اندازه(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_اندازه(arg2) ? arg2[i] : RTLIL::State::S0;
		نتیجه.مجموعه(i, logic_func(a, b));
	}

	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	أرجع logic_wrapper(logic_and, arg1, arg2, signed1, signed2, نتیجه_len);
}

RTLIL::Const RTLIL::const_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	أرجع logic_wrapper(logic_or, arg1, arg2, signed1, signed2, نتیجه_len);
}

RTLIL::Const RTLIL::const_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	أرجع logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, نتیجه_len);
}

RTLIL::Const RTLIL::const_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	أرجع logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, نتیجه_len);
}

ساكن RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ثابت RTLIL::Const &arg1, صحيح نتیجه_len)
{
	RTLIL::State موقت = initial;

	لكل (خودکار i = 0; i < arg1.اندازه(); i++)
		موقت = logic_func(موقت, arg1[i]);

	RTLIL::Const نتیجه(موقت);
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_reduce_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتیجه_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, نتیجه_len);
}

RTLIL::Const RTLIL::const_reduce_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتیجه_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, نتیجه_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتیجه_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, نتیجه_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتیجه_len)
{
	RTLIL::Const میانگیر = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, نتیجه_len);
	إذا (!میانگیر.خالی()) {
		إذا (میانگیر.front() == RTLIL::State::S0)
			میانگیر.مجموعه(0, RTLIL::State::S1);
		وإلا إذا (میانگیر.front() == RTLIL::State::S1)
			میانگیر.مجموعه(0, RTLIL::State::S0);
	}
	أرجع میانگیر;
}

RTLIL::Const RTLIL::const_reduce_bool(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتیجه_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, نتیجه_len);
}

RTLIL::Const RTLIL::const_logic_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_بیت_pos_a);
	RTLIL::Const نتیجه(a.isZero() ? undef_بیت_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_logic_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos_a = -1, undef_بیت_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_بیت_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_بیت_pos_b);

	RTLIL::State بیت_a = a.isZero() ? undef_بیت_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State بیت_b = b.isZero() ? undef_بیت_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const نتیجه(logic_and(بیت_a, بیت_b));
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_logic_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos_a = -1, undef_بیت_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_بیت_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_بیت_pos_b);

	RTLIL::State بیت_a = a.isZero() ? undef_بیت_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State بیت_b = b.isZero() ? undef_بیت_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const نتیجه(logic_or(بیت_a, بیت_b));
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ساكن RTLIL::Const const_لغزش_کارگر(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي sign_ext, منطقي signed2, صحيح direction, صحيح نتیجه_len, RTLIL::State vacant_بیتها = RTLIL::State::S0)
{
	صحيح undef_بیت_pos = -1;
	BigInteger جابه_جایی = const2big(arg2, signed2, undef_بیت_pos) * direction;

	إذا (نتیجه_len < 0)
		نتیجه_len = Get_اندازه(arg1);

	RTLIL::Const نتیجه(RTLIL::State::Sx, نتیجه_len);
	إذا (undef_بیت_pos >= 0)
		أرجع نتیجه;

	لكل (صحيح i = 0; i < نتیجه_len; i++) {
		BigInteger pos = BigInteger(i) + جابه_جایی;
		إذا (pos < 0)
			نتیجه.مجموعه(i, vacant_بیتها);
		وإلا إذا (pos >= BigInteger(Get_اندازه(arg1)))
			نتیجه.مجموعه(i, sign_ext && !arg1.خالی() ? arg1.پشت() : vacant_بیتها);
		وإلا
			نتیجه.مجموعه(i, arg1[pos.مقصد_Int()]);
	}

	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_shl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجه_len, signed1);
	أرجع const_لغزش_کارگر(arg1_ext, arg2, خطأ, خطأ, -1, نتیجه_len);
}

RTLIL::Const RTLIL::const_shr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(نتیجه_len, Get_اندازه(arg1)), signed1);
	أرجع const_لغزش_کارگر(arg1_ext, arg2, خطأ, خطأ, +1, نتیجه_len);
}

RTLIL::Const RTLIL::const_sshl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	أرجع const_لغزش_کارگر(arg1, arg2, signed1, خطأ, -1, نتیجه_len);
}

RTLIL::Const RTLIL::const_sshr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	أرجع const_لغزش_کارگر(arg1, arg2, signed1, خطأ, +1, نتیجه_len);
}

RTLIL::Const RTLIL::const_لغزش(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(نتیجه_len, Get_اندازه(arg1)), signed1);
	أرجع const_لغزش_کارگر(arg1_ext, arg2, خطأ, signed2, +1, نتیجه_len);
}

RTLIL::Const RTLIL::const_shiftx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي, منطقي signed2, صحيح نتیجه_len)
{
	أرجع const_لغزش_کارگر(arg1, arg2, خطأ, signed2, +1, نتیجه_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_بیت_pos) < const2big(arg2, signed2, undef_بیت_pos);
	RTLIL::Const نتیجه(undef_بیت_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_le(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_بیت_pos) <= const2big(arg2, signed2, undef_بیت_pos);
	RTLIL::Const نتیجه(undef_بیت_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_eq(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const نتیجه(RTLIL::State::S0, نتیجه_len);

	صحيح عرض = max(Get_اندازه(arg1_ext), Get_اندازه(arg2_ext));
	extend_u0(arg1_ext, عرض, signed1 && signed2);
	extend_u0(arg2_ext, عرض, signed1 && signed2);

	RTLIL::State matched_وضعیت = RTLIL::State::S1;
	لكل (خودکار i = 0; i < arg1_ext.اندازه(); i++) {
		إذا (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			أرجع نتیجه;
		إذا (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			أرجع نتیجه;
		إذا (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_وضعیت = RTLIL::State::Sx;
	}

	نتیجه.مجموعه(0, matched_وضعیت);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_ne(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	RTLIL::Const نتیجه = RTLIL::const_eq(arg1, arg2, signed1, signed2, نتیجه_len);
	إذا (نتیجه.front() == RTLIL::State::S0)
		نتیجه.مجموعه(0, RTLIL::State::S1);
	وإلا إذا (نتیجه.front() == RTLIL::State::S1)
		نتیجه.مجموعه(0, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_eqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const نتیجه(RTLIL::State::S0, نتیجه_len);

	صحيح عرض = max(Get_اندازه(arg1_ext), Get_اندازه(arg2_ext));
	extend_u0(arg1_ext, عرض, signed1 && signed2);
	extend_u0(arg2_ext, عرض, signed1 && signed2);

	لكل (خودکار i = 0; i < arg1_ext.اندازه(); i++) {
		إذا (arg1_ext.at(i) != arg2_ext.at(i))
			أرجع نتیجه;
	}

	نتیجه.مجموعه(0, RTLIL::State::S1);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_nex(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	RTLIL::Const نتیجه = RTLIL::const_eqx(arg1, arg2, signed1, signed2, نتیجه_len);
	إذا (نتیجه.front() == RTLIL::State::S0)
		نتیجه.مجموعه(0, RTLIL::State::S1);
	وإلا إذا (نتیجه.front() == RTLIL::State::S1)
		نتیجه.مجموعه(0, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_ge(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_بیت_pos) >= const2big(arg2, signed2, undef_بیت_pos);
	RTLIL::Const نتیجه(undef_بیت_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_gt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_بیت_pos) > const2big(arg2, signed2, undef_بیت_pos);
	RTLIL::Const نتیجه(undef_بیت_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_اندازه(نتیجه) < نتیجه_len)
		نتیجه.resize(نتیجه_len, RTLIL::State::S0);
	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_جمع_کن(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_بیت_pos) + const2big(arg2, signed2, undef_بیت_pos);
	أرجع big2const(y, نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), undef_بیت_pos);
}

RTLIL::Const RTLIL::const_sub(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_بیت_pos) - const2big(arg2, signed2, undef_بیت_pos);
	أرجع big2const(y, نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), undef_بیت_pos);
}

RTLIL::Const RTLIL::const_mul(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_بیت_pos) * const2big(arg2, signed2, undef_بیت_pos);
	أرجع big2const(y, نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), min(undef_بیت_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_بیت_pos);
	BigInteger b = const2big(arg2, signed2, undef_بیت_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتیجه_len);
	منطقي نتیجه_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	أرجع big2const(نتیجه_neg ? -(a / b) : (a / b), نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), min(undef_بیت_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_بیت_pos);
	BigInteger b = const2big(arg2, signed2, undef_بیت_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتیجه_len);
	منطقي نتیجه_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	أرجع big2const(نتیجه_neg ? -(a % b) : (a % b), نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), min(undef_بیت_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_بیت_pos);
	BigInteger b = const2big(arg2, signed2, undef_بیت_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتیجه_len);

	منطقي نتیجه_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger نتیجه;

	إذا (نتیجه_pos || a == 0) {
		نتیجه = a / b;
	} وإلا {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		نتیجه = -((a + b - 1) / b);
	}
	أرجع big2const(نتیجه, نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), min(undef_بیت_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_بیت_pos);
	BigInteger b = const2big(arg2, signed2, undef_بیت_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتیجه_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	إذا (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} وإلا {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	أرجع big2const(modulo, نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), min(undef_بیت_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتیجه_len)
{
	صحيح undef_بیت_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_بیت_pos);
	BigInteger b = const2big(arg2, signed2, undef_بیت_pos);
	BigInteger y = 1;

	إذا (a == 0 && b < 0)
		أرجع RTLIL::Const(RTLIL::State::Sx, نتیجه_len);

	إذا (a == 0 && b > 0)
		أرجع RTLIL::Const(RTLIL::State::S0, نتیجه_len);

	إذا (b < 0)
	{
		إذا (a < -1 || a > 1)
			y = 0;
		إذا (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	إذا (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		صحيح modulus_بیتها = (نتیجه_len >= 0 ? نتیجه_len : 1024);
		لكل (صحيح i = 0; i < modulus_بیتها; i++)
			modulus *= 2;

		منطقي flip_نتیجه_sign = خطأ;
		إذا (a < 0) {
			a *= -1;
			إذا (b % 2 == 1)
				flip_نتیجه_sign = درست;
		}

		طالما (b > 0) {
			إذا (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		إذا (flip_نتیجه_sign)
			y *= -1;
	}

	أرجع big2const(y, نتیجه_len >= 0 ? نتیجه_len : max(Get_اندازه(arg1), Get_اندازه(arg2)), min(undef_بیت_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجه_len, signed1);

	أرجع arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتیجه_len, signed1);

	أرجع arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتیجه_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	أرجع RTLIL::const_sub(zero, arg1_ext, درست, signed1, نتیجه_len);
}

RTLIL::Const RTLIL::const_mux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.اندازه() == arg1.اندازه());
	إذا (arg3[0] == State::S0)
		أرجع arg1;
	وإلا إذا (arg3[0] == State::S1)
		أرجع arg2;

	RTLIL::Const ret = arg1;
	لكل (خودکار i = 0; i < ret.اندازه(); i++)
		إذا (ret[i] != arg2[i])
			ret.مجموعه(i, State::Sx);
	أرجع ret;
}

RTLIL::Const RTLIL::const_pmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	إذا (arg3.is_fully_zero())
		أرجع arg1;

	إذا (!arg3.is_onehot())
		أرجع RTLIL::Const(State::Sx, arg1.اندازه());

	لكل (خودکار i = 0; i < arg3.اندازه(); i++)
		إذا (arg3[i] == State::S1)
			أرجع RTLIL::Const(std::وکتور<RTLIL::State>(arg2.begin() + i*arg1.اندازه(), arg2.begin() + (i+1)*arg1.اندازه()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	std::وکتور<State> t = arg1.مقصد_بیتها();

	لكل (صحيح i = Get_اندازه(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::وکتور<RTLIL::State> new_t;
		إذا (sel == State::S0)
			new_t = std::وکتور<RTLIL::State>(t.begin(), t.begin() + Get_اندازه(t)/2);
		وإلا إذا (sel == State::S1)
			new_t = std::وکتور<RTLIL::State>(t.begin() + Get_اندازه(t)/2, t.انتها());
		وإلا
			لكل (صحيح j = 0; j < Get_اندازه(t)/2; j++)
				new_t.افزودن_پشت(t[j] == t[j + Get_اندازه(t)/2] ? t[j] : RTLIL::Sx);
		t.جابجا(new_t);
	}

	أرجع t;
}

RTLIL::Const RTLIL::const_demux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	صحيح عرض = Get_اندازه(arg1);
	صحيح s_عرض = Get_اندازه(arg2);
	std::وکتور<RTLIL::State> res;
	لكل (صحيح i = 0; i < (1 << s_عرض); i++)
	{
		منطقي ne = خطأ;
		منطقي x = خطأ;
		لكل (صحيح j = 0; j < s_عرض; j++) {
			منطقي بیت = i & 1 << j;
			إذا (arg2[j] == (بیت ? RTLIL::S0 : RTLIL::S1))
				ne = درست;
			وإلا إذا (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = درست;
		}
		إذا (ne) {
			لكل (صحيح j = 0; j < عرض; j++)
				res.افزودن_پشت(State::S0);
		} وإلا إذا (x) {
			لكل (صحيح j = 0; j < عرض; j++)
				res.افزودن_پشت(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} وإلا {
			لكل (صحيح j = 0; j < عرض; j++)
				res.افزودن_پشت(arg1[j]);
		}
	}
	أرجع res;
}

RTLIL::Const RTLIL::const_bweqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	log_assert(arg2.اندازه() == arg1.اندازه());
	RTLIL::Const نتیجه(RTLIL::State::S0, arg1.اندازه());
	لكل (خودکار i = 0; i < arg1.اندازه(); i++)
		نتیجه.مجموعه(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	أرجع نتیجه;
}

RTLIL::Const RTLIL::const_bwmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.اندازه() == arg1.اندازه());
	log_assert(arg3.اندازه() == arg1.اندازه());
	RTLIL::Const نتیجه(RTLIL::State::Sx, arg1.اندازه());
	لكل (خودکار i = 0; i < arg1.اندازه(); i++) {
		إذا (arg3[i] != State::Sx || arg1[i] == arg2[i])
			نتیجه.مجموعه(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	أرجع نتیجه;
}

YOSYS_NAMESPACE_انتها
