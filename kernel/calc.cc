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

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

స్థిర శూన్య extend_u0(RTLIL::Const &arg, పూర్ణాంకం వెడల్పు, తార్కిక is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	ఒకవేళ (arg.కొలత() > 0 && is_signed)
		padding = arg.వెనుక();

	arg.resize(వెడల్పు, padding);
}

స్థిర BigInteger const2big(స్థిరరాశి RTLIL::Const &val, తార్కిక as_signed, పూర్ణాంకం &undef_బిట్టు_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::ధనాత్మక;
	State inv_sign_బిట్టు = RTLIL::State::S1;
	స్వయంచాలక అంకె_బిట్లు = val.కొలత();

	ఒకవేళ (as_signed && అంకె_బిట్లు && val[అంకె_బిట్లు-1] == RTLIL::State::S1) {
		inv_sign_బిట్టు = RTLIL::State::S0;
		sign = BigInteger::negative;
		అంకె_బిట్లు--;
	}

	కొరకు (స్వయంచాలక i = 0; i < అంకె_బిట్లు; i++)
		ఒకవేళ (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.సమితి_బిట్టు(i, val[i] == inv_sign_బిట్టు);
		లేకపోతే ఒకవేళ (undef_బిట్టు_pos < 0)
			undef_బిట్టు_pos = i;

	ఒకవేళ (sign == BigInteger::negative)
		mag += 1;

	తిరిగి BigInteger(mag, sign);
}

స్థిర RTLIL::Const big2const(స్థిరరాశి BigInteger &val, పూర్ణాంకం ఫలితం_len, పూర్ణాంకం undef_బిట్టు_pos)
{
	ఒకవేళ (undef_బిట్టు_pos >= 0)
		తిరిగి RTLIL::Const(RTLIL::State::Sx, ఫలితం_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const ఫలితం(0, ఫలితం_len);

	ఒకవేళ (!mag.isZero())
	{
		ఒకవేళ (val.getSign() < 0)
		{
			mag--;
			కొరకు (స్వయంచాలక i = 0; i < ఫలితం_len; i++)
				ఫలితం.సమితి(i, mag.get_బిట్టు(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		లేకపోతే
		{
			కొరకు (స్వయంచాలక i = 0; i < ఫలితం_len; i++)
				ఫలితం.సమితి(i, mag.get_బిట్టు(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#ఒకవేళ 0
	ఒకవేళ (undef_బిట్టు_pos >= 0)
		కొరకు (పూర్ణాంకం i = undef_బిట్టు_pos; i < ఫలితం_len; i++)
			ఫలితం[i] = RTLIL::State::Sx;
#అయితే_ముగింపు

	తిరిగి ఫలితం;
}

స్థిర RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	ఒకవేళ (a == RTLIL::State::S0) తిరిగి RTLIL::State::S0;
	ఒకవేళ (b == RTLIL::State::S0) తిరిగి RTLIL::State::S0;
	ఒకవేళ (a != RTLIL::State::S1) తిరిగి RTLIL::State::Sx;
	ఒకవేళ (b != RTLIL::State::S1) తిరిగి RTLIL::State::Sx;
	తిరిగి RTLIL::State::S1;
}

స్థిర RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	ఒకవేళ (a == RTLIL::State::S1) తిరిగి RTLIL::State::S1;
	ఒకవేళ (b == RTLIL::State::S1) తిరిగి RTLIL::State::S1;
	ఒకవేళ (a != RTLIL::State::S0) తిరిగి RTLIL::State::Sx;
	ఒకవేళ (b != RTLIL::State::S0) తిరిగి RTLIL::State::Sx;
	తిరిగి RTLIL::State::S0;
}

స్థిర RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	ఒకవేళ (a != RTLIL::State::S0 && a != RTLIL::State::S1) తిరిగి RTLIL::State::Sx;
	ఒకవేళ (b != RTLIL::State::S0 && b != RTLIL::State::S1) తిరిగి RTLIL::State::Sx;
	తిరిగి a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

స్థిర RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	ఒకవేళ (a != RTLIL::State::S0 && a != RTLIL::State::S1) తిరిగి RTLIL::State::Sx;
	ఒకవేళ (b != RTLIL::State::S0 && b != RTLIL::State::S1) తిరిగి RTLIL::State::Sx;
	తిరిగి a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	ఒకవేళ (ఫలితం_len < 0)
		ఫలితం_len = Get_కొలత(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ఫలితం_len, signed1);

	RTLIL::Const ఫలితం(RTLIL::State::Sx, ఫలితం_len);
	కొరకు (స్వయంచాలక i = 0; i < ఫలితం_len; i++) {
		ఒకవేళ (i >= Get_కొలత(arg1_ext))
			ఫలితం.సమితి(i, RTLIL::State::S0);
		లేకపోతే ఒకవేళ (arg1_ext[i] == RTLIL::State::S0)
			ఫలితం.సమితి(i, RTLIL::State::S1);
		లేకపోతే ఒకవేళ (arg1_ext[i] == RTLIL::State::S1)
			ఫలితం.సమితి(i, RTLIL::State::S0);
	}

	తిరిగి ఫలితం;
}

స్థిర RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len = -1)
{
	ఒకవేళ (ఫలితం_len < 0)
		ఫలితం_len = max(Get_కొలత(arg1), Get_కొలత(arg2));

	extend_u0(arg1, ఫలితం_len, signed1);
	extend_u0(arg2, ఫలితం_len, signed2);

	RTLIL::Const ఫలితం(RTLIL::State::Sx, ఫలితం_len);
	కొరకు (స్వయంచాలక i = 0; i < ఫలితం_len; i++) {
		RTLIL::State a = i < Get_కొలత(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_కొలత(arg2) ? arg2[i] : RTLIL::State::S0;
		ఫలితం.సమితి(i, logic_func(a, b));
	}

	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_and(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_wrapper(logic_and, arg1, arg2, signed1, signed2, ఫలితం_len);
}

RTLIL::Const RTLIL::const_or(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_wrapper(logic_or, arg1, arg2, signed1, signed2, ఫలితం_len);
}

RTLIL::Const RTLIL::const_xor(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, ఫలితం_len);
}

RTLIL::Const RTLIL::const_xnor(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, ఫలితం_len);
}

స్థిర RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), స్థిరరాశి RTLIL::Const &arg1, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::State తాత్కాలిక = initial;

	కొరకు (స్వయంచాలక i = 0; i < arg1.కొలత(); i++)
		తాత్కాలిక = logic_func(తాత్కాలిక, arg1[i]);

	RTLIL::Const ఫలితం(తాత్కాలిక);
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_reduce_and(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_reduce_or(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_reduce_xor(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const బఫరు = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, ఫలితం_len);
	ఒకవేళ (!బఫరు.ఖాళీలు()) {
		ఒకవేళ (బఫరు.front() == RTLIL::State::S0)
			బఫరు.సమితి(0, RTLIL::State::S1);
		లేకపోతే ఒకవేళ (బఫరు.front() == RTLIL::State::S1)
			బఫరు.సమితి(0, RTLIL::State::S0);
	}
	తిరిగి బఫరు;
}

RTLIL::Const RTLIL::const_reduce_bool(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_logic_not(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos_a);
	RTLIL::Const ఫలితం(a.isZero() ? undef_బిట్టు_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_logic_and(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos_a = -1, undef_బిట్టు_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_బిట్టు_pos_b);

	RTLIL::State బిట్టు_a = a.isZero() ? undef_బిట్టు_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State బిట్టు_b = b.isZero() ? undef_బిట్టు_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const ఫలితం(logic_and(బిట్టు_a, బిట్టు_b));
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_logic_or(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos_a = -1, undef_బిట్టు_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_బిట్టు_pos_b);

	RTLIL::State బిట్టు_a = a.isZero() ? undef_బిట్టు_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State బిట్టు_b = b.isZero() ? undef_బిట్టు_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const ఫలితం(logic_or(బిట్టు_a, బిట్టు_b));
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
స్థిర RTLIL::Const const_జరుపు_కార్మికుడు(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక sign_ext, తార్కిక signed2, పూర్ణాంకం direction, పూర్ణాంకం ఫలితం_len, RTLIL::State vacant_బిట్లు = RTLIL::State::S0)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger స్థానభ్రంశం = const2big(arg2, signed2, undef_బిట్టు_pos) * direction;

	ఒకవేళ (ఫలితం_len < 0)
		ఫలితం_len = Get_కొలత(arg1);

	RTLIL::Const ఫలితం(RTLIL::State::Sx, ఫలితం_len);
	ఒకవేళ (undef_బిట్టు_pos >= 0)
		తిరిగి ఫలితం;

	కొరకు (పూర్ణాంకం i = 0; i < ఫలితం_len; i++) {
		BigInteger pos = BigInteger(i) + స్థానభ్రంశం;
		ఒకవేళ (pos < 0)
			ఫలితం.సమితి(i, vacant_బిట్లు);
		లేకపోతే ఒకవేళ (pos >= BigInteger(Get_కొలత(arg1)))
			ఫలితం.సమితి(i, sign_ext && !arg1.ఖాళీలు() ? arg1.వెనుక() : vacant_బిట్లు);
		లేకపోతే
			ఫలితం.సమితి(i, arg1[pos.గమ్యం_Int()]);
	}

	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_shl(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ఫలితం_len, signed1);
	తిరిగి const_జరుపు_కార్మికుడు(arg1_ext, arg2, అబద్ధం, అబద్ధం, -1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_shr(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(ఫలితం_len, Get_కొలత(arg1)), signed1);
	తిరిగి const_జరుపు_కార్మికుడు(arg1_ext, arg2, అబద్ధం, అబద్ధం, +1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_sshl(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి const_జరుపు_కార్మికుడు(arg1, arg2, signed1, అబద్ధం, -1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_sshr(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి const_జరుపు_కార్మికుడు(arg1, arg2, signed1, అబద్ధం, +1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_జరుపు(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(ఫలితం_len, Get_కొలత(arg1)), signed1);
	తిరిగి const_జరుపు_కార్మికుడు(arg1_ext, arg2, అబద్ధం, signed2, +1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_shiftx(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	తిరిగి const_జరుపు_కార్మికుడు(arg1, arg2, అబద్ధం, signed2, +1, ఫలితం_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	తార్కిక y = const2big(arg1, signed1, undef_బిట్టు_pos) < const2big(arg2, signed2, undef_బిట్టు_pos);
	RTLIL::Const ఫలితం(undef_బిట్టు_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_le(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	తార్కిక y = const2big(arg1, signed1, undef_బిట్టు_pos) <= const2big(arg2, signed2, undef_బిట్టు_pos);
	RTLIL::Const ఫలితం(undef_బిట్టు_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_eq(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const ఫలితం(RTLIL::State::S0, ఫలితం_len);

	పూర్ణాంకం వెడల్పు = max(Get_కొలత(arg1_ext), Get_కొలత(arg2_ext));
	extend_u0(arg1_ext, వెడల్పు, signed1 && signed2);
	extend_u0(arg2_ext, వెడల్పు, signed1 && signed2);

	RTLIL::State matched_స్థితి = RTLIL::State::S1;
	కొరకు (స్వయంచాలక i = 0; i < arg1_ext.కొలత(); i++) {
		ఒకవేళ (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			తిరిగి ఫలితం;
		ఒకవేళ (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			తిరిగి ఫలితం;
		ఒకవేళ (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_స్థితి = RTLIL::State::Sx;
	}

	ఫలితం.సమితి(0, matched_స్థితి);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_ne(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const ఫలితం = RTLIL::const_eq(arg1, arg2, signed1, signed2, ఫలితం_len);
	ఒకవేళ (ఫలితం.front() == RTLIL::State::S0)
		ఫలితం.సమితి(0, RTLIL::State::S1);
	లేకపోతే ఒకవేళ (ఫలితం.front() == RTLIL::State::S1)
		ఫలితం.సమితి(0, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_eqx(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const ఫలితం(RTLIL::State::S0, ఫలితం_len);

	పూర్ణాంకం వెడల్పు = max(Get_కొలత(arg1_ext), Get_కొలత(arg2_ext));
	extend_u0(arg1_ext, వెడల్పు, signed1 && signed2);
	extend_u0(arg2_ext, వెడల్పు, signed1 && signed2);

	కొరకు (స్వయంచాలక i = 0; i < arg1_ext.కొలత(); i++) {
		ఒకవేళ (arg1_ext.at(i) != arg2_ext.at(i))
			తిరిగి ఫలితం;
	}

	ఫలితం.సమితి(0, RTLIL::State::S1);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_nex(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const ఫలితం = RTLIL::const_eqx(arg1, arg2, signed1, signed2, ఫలితం_len);
	ఒకవేళ (ఫలితం.front() == RTLIL::State::S0)
		ఫలితం.సమితి(0, RTLIL::State::S1);
	లేకపోతే ఒకవేళ (ఫలితం.front() == RTLIL::State::S1)
		ఫలితం.సమితి(0, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_ge(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	తార్కిక y = const2big(arg1, signed1, undef_బిట్టు_pos) >= const2big(arg2, signed2, undef_బిట్టు_pos);
	RTLIL::Const ఫలితం(undef_బిట్టు_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_gt(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	తార్కిక y = const2big(arg1, signed1, undef_బిట్టు_pos) > const2big(arg2, signed2, undef_బిట్టు_pos);
	RTLIL::Const ఫలితం(undef_బిట్టు_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ఒకవేళ (Get_కొలత(ఫలితం) < ఫలితం_len)
		ఫలితం.resize(ఫలితం_len, RTLIL::State::S0);
	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_కూడు(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_బిట్టు_pos) + const2big(arg2, signed2, undef_బిట్టు_pos);
	తిరిగి big2const(y, ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), undef_బిట్టు_pos);
}

RTLIL::Const RTLIL::const_sub(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_బిట్టు_pos) - const2big(arg2, signed2, undef_బిట్టు_pos);
	తిరిగి big2const(y, ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), undef_బిట్టు_pos);
}

RTLIL::Const RTLIL::const_mul(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_బిట్టు_pos) * const2big(arg2, signed2, undef_బిట్టు_pos);
	తిరిగి big2const(y, ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), min(undef_బిట్టు_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos);
	BigInteger b = const2big(arg2, signed2, undef_బిట్టు_pos);
	ఒకవేళ (b.isZero())
		తిరిగి RTLIL::Const(RTLIL::State::Sx, ఫలితం_len);
	తార్కిక ఫలితం_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	తిరిగి big2const(ఫలితం_neg ? -(a / b) : (a / b), ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), min(undef_బిట్టు_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos);
	BigInteger b = const2big(arg2, signed2, undef_బిట్టు_pos);
	ఒకవేళ (b.isZero())
		తిరిగి RTLIL::Const(RTLIL::State::Sx, ఫలితం_len);
	తార్కిక ఫలితం_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	తిరిగి big2const(ఫలితం_neg ? -(a % b) : (a % b), ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), min(undef_బిట్టు_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos);
	BigInteger b = const2big(arg2, signed2, undef_బిట్టు_pos);
	ఒకవేళ (b.isZero())
		తిరిగి RTLIL::Const(RTLIL::State::Sx, ఫలితం_len);

	తార్కిక ఫలితం_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger ఫలితం;

	ఒకవేళ (ఫలితం_pos || a == 0) {
		ఫలితం = a / b;
	} లేకపోతే {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		ఫలితం = -((a + b - 1) / b);
	}
	తిరిగి big2const(ఫలితం, ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), min(undef_బిట్టు_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos);
	BigInteger b = const2big(arg2, signed2, undef_బిట్టు_pos);
	ఒకవేళ (b.isZero())
		తిరిగి RTLIL::Const(RTLIL::State::Sx, ఫలితం_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	ఒకవేళ (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} లేకపోతే {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	తిరిగి big2const(modulo, ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), min(undef_బిట్టు_pos, 0));
}

RTLIL::Const RTLIL::const_pow(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, తార్కిక signed1, తార్కిక signed2, పూర్ణాంకం ఫలితం_len)
{
	పూర్ణాంకం undef_బిట్టు_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_బిట్టు_pos);
	BigInteger b = const2big(arg2, signed2, undef_బిట్టు_pos);
	BigInteger y = 1;

	ఒకవేళ (a == 0 && b < 0)
		తిరిగి RTLIL::Const(RTLIL::State::Sx, ఫలితం_len);

	ఒకవేళ (a == 0 && b > 0)
		తిరిగి RTLIL::Const(RTLIL::State::S0, ఫలితం_len);

	ఒకవేళ (b < 0)
	{
		ఒకవేళ (a < -1 || a > 1)
			y = 0;
		ఒకవేళ (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	ఒకవేళ (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		పూర్ణాంకం modulus_బిట్లు = (ఫలితం_len >= 0 ? ఫలితం_len : 1024);
		కొరకు (పూర్ణాంకం i = 0; i < modulus_బిట్లు; i++)
			modulus *= 2;

		తార్కిక flip_ఫలితం_sign = అబద్ధం;
		ఒకవేళ (a < 0) {
			a *= -1;
			ఒకవేళ (b % 2 == 1)
				flip_ఫలితం_sign = నిజం;
		}

		అయితే (b > 0) {
			ఒకవేళ (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		ఒకవేళ (flip_ఫలితం_sign)
			y *= -1;
	}

	తిరిగి big2const(y, ఫలితం_len >= 0 ? ఫలితం_len : max(Get_కొలత(arg1), Get_కొలత(arg2)), min(undef_బిట్టు_pos, 0));
}

RTLIL::Const RTLIL::const_pos(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ఫలితం_len, signed1);

	తిరిగి arg1_ext;
}

RTLIL::Const RTLIL::const_buf(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ఫలితం_len, signed1);

	తిరిగి arg1_ext;
}

RTLIL::Const RTLIL::const_neg(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const&, తార్కిక signed1, తార్కిక, పూర్ణాంకం ఫలితం_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	తిరిగి RTLIL::const_sub(zero, arg1_ext, నిజం, signed1, ఫలితం_len);
}

RTLIL::Const RTLIL::const_mux(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3)
{
	log_assert(arg2.కొలత() == arg1.కొలత());
	ఒకవేళ (arg3[0] == State::S0)
		తిరిగి arg1;
	లేకపోతే ఒకవేళ (arg3[0] == State::S1)
		తిరిగి arg2;

	RTLIL::Const ret = arg1;
	కొరకు (స్వయంచాలక i = 0; i < ret.కొలత(); i++)
		ఒకవేళ (ret[i] != arg2[i])
			ret.సమితి(i, State::Sx);
	తిరిగి ret;
}

RTLIL::Const RTLIL::const_pmux(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3)
{
	ఒకవేళ (arg3.is_fully_zero())
		తిరిగి arg1;

	ఒకవేళ (!arg3.is_onehot())
		తిరిగి RTLIL::Const(State::Sx, arg1.కొలత());

	కొరకు (స్వయంచాలక i = 0; i < arg3.కొలత(); i++)
		ఒకవేళ (arg3[i] == State::S1)
			తిరిగి RTLIL::Const(std::సదిశ<RTLIL::State>(arg2.begin() + i*arg1.కొలత(), arg2.begin() + (i+1)*arg1.కొలత()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2)
{
	std::సదిశ<State> t = arg1.గమ్యం_బిట్లు();

	కొరకు (పూర్ణాంకం i = Get_కొలత(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::సదిశ<RTLIL::State> new_t;
		ఒకవేళ (sel == State::S0)
			new_t = std::సదిశ<RTLIL::State>(t.begin(), t.begin() + Get_కొలత(t)/2);
		లేకపోతే ఒకవేళ (sel == State::S1)
			new_t = std::సదిశ<RTLIL::State>(t.begin() + Get_కొలత(t)/2, t.చివర());
		లేకపోతే
			కొరకు (పూర్ణాంకం j = 0; j < Get_కొలత(t)/2; j++)
				new_t.నెట్టు_వెనుక(t[j] == t[j + Get_కొలత(t)/2] ? t[j] : RTLIL::Sx);
		t.మార్పిడి(new_t);
	}

	తిరిగి t;
}

RTLIL::Const RTLIL::const_demux(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2)
{
	పూర్ణాంకం వెడల్పు = Get_కొలత(arg1);
	పూర్ణాంకం s_వెడల్పు = Get_కొలత(arg2);
	std::సదిశ<RTLIL::State> res;
	కొరకు (పూర్ణాంకం i = 0; i < (1 << s_వెడల్పు); i++)
	{
		తార్కిక ne = అబద్ధం;
		తార్కిక x = అబద్ధం;
		కొరకు (పూర్ణాంకం j = 0; j < s_వెడల్పు; j++) {
			తార్కిక బిట్టు = i & 1 << j;
			ఒకవేళ (arg2[j] == (బిట్టు ? RTLIL::S0 : RTLIL::S1))
				ne = నిజం;
			లేకపోతే ఒకవేళ (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = నిజం;
		}
		ఒకవేళ (ne) {
			కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు; j++)
				res.నెట్టు_వెనుక(State::S0);
		} లేకపోతే ఒకవేళ (x) {
			కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు; j++)
				res.నెట్టు_వెనుక(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} లేకపోతే {
			కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు; j++)
				res.నెట్టు_వెనుక(arg1[j]);
		}
	}
	తిరిగి res;
}

RTLIL::Const RTLIL::const_bweqx(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2)
{
	log_assert(arg2.కొలత() == arg1.కొలత());
	RTLIL::Const ఫలితం(RTLIL::State::S0, arg1.కొలత());
	కొరకు (స్వయంచాలక i = 0; i < arg1.కొలత(); i++)
		ఫలితం.సమితి(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	తిరిగి ఫలితం;
}

RTLIL::Const RTLIL::const_bwmux(స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3)
{
	log_assert(arg2.కొలత() == arg1.కొలత());
	log_assert(arg3.కొలత() == arg1.కొలత());
	RTLIL::Const ఫలితం(RTLIL::State::Sx, arg1.కొలత());
	కొరకు (స్వయంచాలక i = 0; i < arg1.కొలత(); i++) {
		ఒకవేళ (arg3[i] != State::Sx || arg1[i] == arg2[i])
			ఫలితం.సమితి(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	తిరిగి ఫలితం;
}

YOSYS_NAMESPACE_చివర
