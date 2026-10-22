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

ساكن فراغ extend_u0(RTLIL::Const &arg, صحيح ويڪر, منطقي is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	إذا (arg.سائيز() > 0 && is_signed)
		padding = arg.پٺ();

	arg.resize(ويڪر, padding);
}

ساكن BigInteger const2big(ثابت RTLIL::Const &val, منطقي as_signed, صحيح &undef_ٻٽو_انگ_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::مثبت;
	State inv_sign_ٻٽو_انگ = RTLIL::State::S1;
	خودڪار انگ_بٽ = val.سائيز();

	إذا (as_signed && انگ_بٽ && val[انگ_بٽ-1] == RTLIL::State::S1) {
		inv_sign_ٻٽو_انگ = RTLIL::State::S0;
		sign = BigInteger::negative;
		انگ_بٽ--;
	}

	لكل (خودڪار i = 0; i < انگ_بٽ; i++)
		إذا (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.سيٽ_ٻٽو_انگ(i, val[i] == inv_sign_ٻٽو_انگ);
		وإلا إذا (undef_ٻٽو_انگ_pos < 0)
			undef_ٻٽو_انگ_pos = i;

	إذا (sign == BigInteger::negative)
		mag += 1;

	أرجع BigInteger(mag, sign);
}

ساكن RTLIL::Const big2const(ثابت BigInteger &val, صحيح نتيجو_len, صحيح undef_ٻٽو_انگ_pos)
{
	إذا (undef_ٻٽو_انگ_pos >= 0)
		أرجع RTLIL::Const(RTLIL::State::Sx, نتيجو_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const نتيجو(0, نتيجو_len);

	إذا (!mag.isZero())
	{
		إذا (val.getSign() < 0)
		{
			mag--;
			لكل (خودڪار i = 0; i < نتيجو_len; i++)
				نتيجو.سيٽ(i, mag.get_ٻٽو_انگ(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		وإلا
		{
			لكل (خودڪار i = 0; i < نتيجو_len; i++)
				نتيجو.سيٽ(i, mag.get_ٻٽو_انگ(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#إذا 0
	إذا (undef_ٻٽو_انگ_pos >= 0)
		لكل (صحيح i = undef_ٻٽو_انگ_pos; i < نتيجو_len; i++)
			نتيجو[i] = RTLIL::State::Sx;
#جيڪڏهن_پورو

	أرجع نتيجو;
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

RTLIL::Const RTLIL::const_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	إذا (نتيجو_len < 0)
		نتيجو_len = Get_سائيز(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتيجو_len, signed1);

	RTLIL::Const نتيجو(RTLIL::State::Sx, نتيجو_len);
	لكل (خودڪار i = 0; i < نتيجو_len; i++) {
		إذا (i >= Get_سائيز(arg1_ext))
			نتيجو.سيٽ(i, RTLIL::State::S0);
		وإلا إذا (arg1_ext[i] == RTLIL::State::S0)
			نتيجو.سيٽ(i, RTLIL::State::S1);
		وإلا إذا (arg1_ext[i] == RTLIL::State::S1)
			نتيجو.سيٽ(i, RTLIL::State::S0);
	}

	أرجع نتيجو;
}

ساكن RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len = -1)
{
	إذا (نتيجو_len < 0)
		نتيجو_len = max(Get_سائيز(arg1), Get_سائيز(arg2));

	extend_u0(arg1, نتيجو_len, signed1);
	extend_u0(arg2, نتيجو_len, signed2);

	RTLIL::Const نتيجو(RTLIL::State::Sx, نتيجو_len);
	لكل (خودڪار i = 0; i < نتيجو_len; i++) {
		RTLIL::State a = i < Get_سائيز(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_سائيز(arg2) ? arg2[i] : RTLIL::State::S0;
		نتيجو.سيٽ(i, logic_func(a, b));
	}

	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	أرجع logic_wrapper(logic_and, arg1, arg2, signed1, signed2, نتيجو_len);
}

RTLIL::Const RTLIL::const_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	أرجع logic_wrapper(logic_or, arg1, arg2, signed1, signed2, نتيجو_len);
}

RTLIL::Const RTLIL::const_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	أرجع logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, نتيجو_len);
}

RTLIL::Const RTLIL::const_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	أرجع logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, نتيجو_len);
}

ساكن RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ثابت RTLIL::Const &arg1, صحيح نتيجو_len)
{
	RTLIL::State عارضي = initial;

	لكل (خودڪار i = 0; i < arg1.سائيز(); i++)
		عارضي = logic_func(عارضي, arg1[i]);

	RTLIL::Const نتيجو(عارضي);
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_reduce_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتيجو_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, نتيجو_len);
}

RTLIL::Const RTLIL::const_reduce_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتيجو_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, نتيجو_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتيجو_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, نتيجو_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتيجو_len)
{
	RTLIL::Const بفر = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, نتيجو_len);
	إذا (!بفر.خالي_جايون()) {
		إذا (بفر.front() == RTLIL::State::S0)
			بفر.سيٽ(0, RTLIL::State::S1);
		وإلا إذا (بفر.front() == RTLIL::State::S1)
			بفر.سيٽ(0, RTLIL::State::S0);
	}
	أرجع بفر;
}

RTLIL::Const RTLIL::const_reduce_bool(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح نتيجو_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, نتيجو_len);
}

RTLIL::Const RTLIL::const_logic_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos_a);
	RTLIL::Const نتيجو(a.isZero() ? undef_ٻٽو_انگ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_logic_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos_a = -1, undef_ٻٽو_انگ_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ٻٽو_انگ_pos_b);

	RTLIL::State ٻٽو_انگ_a = a.isZero() ? undef_ٻٽو_انگ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ٻٽو_انگ_b = b.isZero() ? undef_ٻٽو_انگ_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const نتيجو(logic_and(ٻٽو_انگ_a, ٻٽو_انگ_b));
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_logic_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos_a = -1, undef_ٻٽو_انگ_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ٻٽو_انگ_pos_b);

	RTLIL::State ٻٽو_انگ_a = a.isZero() ? undef_ٻٽو_انگ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ٻٽو_انگ_b = b.isZero() ? undef_ٻٽو_انگ_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const نتيجو(logic_or(ٻٽو_انگ_a, ٻٽو_انگ_b));
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ساكن RTLIL::Const const_سرڪايو_ڪم_ڪندڙ(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي sign_ext, منطقي signed2, صحيح direction, صحيح نتيجو_len, RTLIL::State vacant_بٽ = RTLIL::State::S0)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger وٿي = const2big(arg2, signed2, undef_ٻٽو_انگ_pos) * direction;

	إذا (نتيجو_len < 0)
		نتيجو_len = Get_سائيز(arg1);

	RTLIL::Const نتيجو(RTLIL::State::Sx, نتيجو_len);
	إذا (undef_ٻٽو_انگ_pos >= 0)
		أرجع نتيجو;

	لكل (صحيح i = 0; i < نتيجو_len; i++) {
		BigInteger pos = BigInteger(i) + وٿي;
		إذا (pos < 0)
			نتيجو.سيٽ(i, vacant_بٽ);
		وإلا إذا (pos >= BigInteger(Get_سائيز(arg1)))
			نتيجو.سيٽ(i, sign_ext && !arg1.خالي_جايون() ? arg1.پٺ() : vacant_بٽ);
		وإلا
			نتيجو.سيٽ(i, arg1[pos.منزل_Int()]);
	}

	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_shl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتيجو_len, signed1);
	أرجع const_سرڪايو_ڪم_ڪندڙ(arg1_ext, arg2, خطأ, خطأ, -1, نتيجو_len);
}

RTLIL::Const RTLIL::const_shr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(نتيجو_len, Get_سائيز(arg1)), signed1);
	أرجع const_سرڪايو_ڪم_ڪندڙ(arg1_ext, arg2, خطأ, خطأ, +1, نتيجو_len);
}

RTLIL::Const RTLIL::const_sshl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	أرجع const_سرڪايو_ڪم_ڪندڙ(arg1, arg2, signed1, خطأ, -1, نتيجو_len);
}

RTLIL::Const RTLIL::const_sshr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	أرجع const_سرڪايو_ڪم_ڪندڙ(arg1, arg2, signed1, خطأ, +1, نتيجو_len);
}

RTLIL::Const RTLIL::const_سرڪايو(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(نتيجو_len, Get_سائيز(arg1)), signed1);
	أرجع const_سرڪايو_ڪم_ڪندڙ(arg1_ext, arg2, خطأ, signed2, +1, نتيجو_len);
}

RTLIL::Const RTLIL::const_shiftx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي, منطقي signed2, صحيح نتيجو_len)
{
	أرجع const_سرڪايو_ڪم_ڪندڙ(arg1, arg2, خطأ, signed2, +1, نتيجو_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_ٻٽو_انگ_pos) < const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	RTLIL::Const نتيجو(undef_ٻٽو_انگ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_le(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_ٻٽو_انگ_pos) <= const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	RTLIL::Const نتيجو(undef_ٻٽو_انگ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_eq(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const نتيجو(RTLIL::State::S0, نتيجو_len);

	صحيح ويڪر = max(Get_سائيز(arg1_ext), Get_سائيز(arg2_ext));
	extend_u0(arg1_ext, ويڪر, signed1 && signed2);
	extend_u0(arg2_ext, ويڪر, signed1 && signed2);

	RTLIL::State matched_حالت = RTLIL::State::S1;
	لكل (خودڪار i = 0; i < arg1_ext.سائيز(); i++) {
		إذا (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			أرجع نتيجو;
		إذا (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			أرجع نتيجو;
		إذا (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_حالت = RTLIL::State::Sx;
	}

	نتيجو.سيٽ(0, matched_حالت);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_ne(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	RTLIL::Const نتيجو = RTLIL::const_eq(arg1, arg2, signed1, signed2, نتيجو_len);
	إذا (نتيجو.front() == RTLIL::State::S0)
		نتيجو.سيٽ(0, RTLIL::State::S1);
	وإلا إذا (نتيجو.front() == RTLIL::State::S1)
		نتيجو.سيٽ(0, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_eqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const نتيجو(RTLIL::State::S0, نتيجو_len);

	صحيح ويڪر = max(Get_سائيز(arg1_ext), Get_سائيز(arg2_ext));
	extend_u0(arg1_ext, ويڪر, signed1 && signed2);
	extend_u0(arg2_ext, ويڪر, signed1 && signed2);

	لكل (خودڪار i = 0; i < arg1_ext.سائيز(); i++) {
		إذا (arg1_ext.at(i) != arg2_ext.at(i))
			أرجع نتيجو;
	}

	نتيجو.سيٽ(0, RTLIL::State::S1);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_nex(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	RTLIL::Const نتيجو = RTLIL::const_eqx(arg1, arg2, signed1, signed2, نتيجو_len);
	إذا (نتيجو.front() == RTLIL::State::S0)
		نتيجو.سيٽ(0, RTLIL::State::S1);
	وإلا إذا (نتيجو.front() == RTLIL::State::S1)
		نتيجو.سيٽ(0, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_ge(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_ٻٽو_انگ_pos) >= const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	RTLIL::Const نتيجو(undef_ٻٽو_انگ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_gt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_ٻٽو_انگ_pos) > const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	RTLIL::Const نتيجو(undef_ٻٽو_انگ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (Get_سائيز(نتيجو) < نتيجو_len)
		نتيجو.resize(نتيجو_len, RTLIL::State::S0);
	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_جوڙيو(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ٻٽو_انگ_pos) + const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	أرجع big2const(y, نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), undef_ٻٽو_انگ_pos);
}

RTLIL::Const RTLIL::const_sub(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ٻٽو_انگ_pos) - const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	أرجع big2const(y, نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), undef_ٻٽو_انگ_pos);
}

RTLIL::Const RTLIL::const_mul(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ٻٽو_انگ_pos) * const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	أرجع big2const(y, نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), min(undef_ٻٽو_انگ_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتيجو_len);
	منطقي نتيجو_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	أرجع big2const(نتيجو_neg ? -(a / b) : (a / b), نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), min(undef_ٻٽو_انگ_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتيجو_len);
	منطقي نتيجو_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	أرجع big2const(نتيجو_neg ? -(a % b) : (a % b), نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), min(undef_ٻٽو_انگ_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتيجو_len);

	منطقي نتيجو_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger نتيجو;

	إذا (نتيجو_pos || a == 0) {
		نتيجو = a / b;
	} وإلا {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		نتيجو = -((a + b - 1) / b);
	}
	أرجع big2const(نتيجو, نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), min(undef_ٻٽو_انگ_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, نتيجو_len);

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
	أرجع big2const(modulo, نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), min(undef_ٻٽو_انگ_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح نتيجو_len)
{
	صحيح undef_ٻٽو_انگ_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_ٻٽو_انگ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ٻٽو_انگ_pos);
	BigInteger y = 1;

	إذا (a == 0 && b < 0)
		أرجع RTLIL::Const(RTLIL::State::Sx, نتيجو_len);

	إذا (a == 0 && b > 0)
		أرجع RTLIL::Const(RTLIL::State::S0, نتيجو_len);

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
		صحيح modulus_بٽ = (نتيجو_len >= 0 ? نتيجو_len : 1024);
		لكل (صحيح i = 0; i < modulus_بٽ; i++)
			modulus *= 2;

		منطقي flip_نتيجو_sign = خطأ;
		إذا (a < 0) {
			a *= -1;
			إذا (b % 2 == 1)
				flip_نتيجو_sign = سچ;
		}

		طالما (b > 0) {
			إذا (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		إذا (flip_نتيجو_sign)
			y *= -1;
	}

	أرجع big2const(y, نتيجو_len >= 0 ? نتيجو_len : max(Get_سائيز(arg1), Get_سائيز(arg2)), min(undef_ٻٽو_انگ_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتيجو_len, signed1);

	أرجع arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, نتيجو_len, signed1);

	أرجع arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح نتيجو_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	أرجع RTLIL::const_sub(zero, arg1_ext, سچ, signed1, نتيجو_len);
}

RTLIL::Const RTLIL::const_mux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.سائيز() == arg1.سائيز());
	إذا (arg3[0] == State::S0)
		أرجع arg1;
	وإلا إذا (arg3[0] == State::S1)
		أرجع arg2;

	RTLIL::Const ret = arg1;
	لكل (خودڪار i = 0; i < ret.سائيز(); i++)
		إذا (ret[i] != arg2[i])
			ret.سيٽ(i, State::Sx);
	أرجع ret;
}

RTLIL::Const RTLIL::const_pmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	إذا (arg3.is_fully_zero())
		أرجع arg1;

	إذا (!arg3.is_onehot())
		أرجع RTLIL::Const(State::Sx, arg1.سائيز());

	لكل (خودڪار i = 0; i < arg3.سائيز(); i++)
		إذا (arg3[i] == State::S1)
			أرجع RTLIL::Const(std::ويڪٽر<RTLIL::State>(arg2.begin() + i*arg1.سائيز(), arg2.begin() + (i+1)*arg1.سائيز()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	std::ويڪٽر<State> t = arg1.منزل_بٽ();

	لكل (صحيح i = Get_سائيز(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::ويڪٽر<RTLIL::State> new_t;
		إذا (sel == State::S0)
			new_t = std::ويڪٽر<RTLIL::State>(t.begin(), t.begin() + Get_سائيز(t)/2);
		وإلا إذا (sel == State::S1)
			new_t = std::ويڪٽر<RTLIL::State>(t.begin() + Get_سائيز(t)/2, t.پڇاڙي());
		وإلا
			لكل (صحيح j = 0; j < Get_سائيز(t)/2; j++)
				new_t.ڌڪو_پٺ(t[j] == t[j + Get_سائيز(t)/2] ? t[j] : RTLIL::Sx);
		t.مٽا_سٽا(new_t);
	}

	أرجع t;
}

RTLIL::Const RTLIL::const_demux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	صحيح ويڪر = Get_سائيز(arg1);
	صحيح s_ويڪر = Get_سائيز(arg2);
	std::ويڪٽر<RTLIL::State> res;
	لكل (صحيح i = 0; i < (1 << s_ويڪر); i++)
	{
		منطقي ne = خطأ;
		منطقي x = خطأ;
		لكل (صحيح j = 0; j < s_ويڪر; j++) {
			منطقي ٻٽو_انگ = i & 1 << j;
			إذا (arg2[j] == (ٻٽو_انگ ? RTLIL::S0 : RTLIL::S1))
				ne = سچ;
			وإلا إذا (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = سچ;
		}
		إذا (ne) {
			لكل (صحيح j = 0; j < ويڪر; j++)
				res.ڌڪو_پٺ(State::S0);
		} وإلا إذا (x) {
			لكل (صحيح j = 0; j < ويڪر; j++)
				res.ڌڪو_پٺ(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} وإلا {
			لكل (صحيح j = 0; j < ويڪر; j++)
				res.ڌڪو_پٺ(arg1[j]);
		}
	}
	أرجع res;
}

RTLIL::Const RTLIL::const_bweqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	log_assert(arg2.سائيز() == arg1.سائيز());
	RTLIL::Const نتيجو(RTLIL::State::S0, arg1.سائيز());
	لكل (خودڪار i = 0; i < arg1.سائيز(); i++)
		نتيجو.سيٽ(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	أرجع نتيجو;
}

RTLIL::Const RTLIL::const_bwmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.سائيز() == arg1.سائيز());
	log_assert(arg3.سائيز() == arg1.سائيز());
	RTLIL::Const نتيجو(RTLIL::State::Sx, arg1.سائيز());
	لكل (خودڪار i = 0; i < arg1.سائيز(); i++) {
		إذا (arg3[i] != State::Sx || arg1[i] == arg2[i])
			نتيجو.سيٽ(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	أرجع نتيجو;
}

YOSYS_NAMESPACE_پڇاڙي
