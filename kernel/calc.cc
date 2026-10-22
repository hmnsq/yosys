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

स्थिर शून्य extend_u0(RTLIL::Const &arg, पूर्णांक width, तार्किक is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	जर (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

स्थिर BigInteger const2big(स्थिरांक RTLIL::Const &val, तार्किक as_signed, पूर्णांक &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	स्वतः अंक_bits = val.size();

	जर (as_signed && अंक_bits && val[अंक_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		अंक_bits--;
	}

	साठी (स्वतः i = 0; i < अंक_bits; i++)
		जर (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		अन्यथा जर (undef_bit_pos < 0)
			undef_bit_pos = i;

	जर (sign == BigInteger::negative)
		mag += 1;

	परत BigInteger(mag, sign);
}

स्थिर RTLIL::Const big2const(स्थिरांक BigInteger &val, पूर्णांक result_len, पूर्णांक undef_bit_pos)
{
	जर (undef_bit_pos >= 0)
		परत RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	जर (!mag.isZero())
	{
		जर (val.getSign() < 0)
		{
			mag--;
			साठी (स्वतः i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		अन्यथा
		{
			साठी (स्वतः i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#जर 0
	जर (undef_bit_pos >= 0)
		साठी (पूर्णांक i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	परत result;
}

स्थिर RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	जर (a == RTLIL::State::S0) परत RTLIL::State::S0;
	जर (b == RTLIL::State::S0) परत RTLIL::State::S0;
	जर (a != RTLIL::State::S1) परत RTLIL::State::Sx;
	जर (b != RTLIL::State::S1) परत RTLIL::State::Sx;
	परत RTLIL::State::S1;
}

स्थिर RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	जर (a == RTLIL::State::S1) परत RTLIL::State::S1;
	जर (b == RTLIL::State::S1) परत RTLIL::State::S1;
	जर (a != RTLIL::State::S0) परत RTLIL::State::Sx;
	जर (b != RTLIL::State::S0) परत RTLIL::State::Sx;
	परत RTLIL::State::S0;
}

स्थिर RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	जर (a != RTLIL::State::S0 && a != RTLIL::State::S1) परत RTLIL::State::Sx;
	जर (b != RTLIL::State::S0 && b != RTLIL::State::S1) परत RTLIL::State::Sx;
	परत a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

स्थिर RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	जर (a != RTLIL::State::S0 && a != RTLIL::State::S1) परत RTLIL::State::Sx;
	जर (b != RTLIL::State::S0 && b != RTLIL::State::S1) परत RTLIL::State::Sx;
	परत a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	जर (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	साठी (स्वतः i = 0; i < result_len; i++) {
		जर (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		अन्यथा जर (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		अन्यथा जर (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	परत result;
}

स्थिर RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len = -1)
{
	जर (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	साठी (स्वतः i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	परत result;
}

RTLIL::Const RTLIL::const_and(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	परत logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	परत logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	परत logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	परत logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

स्थिर RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), स्थिरांक RTLIL::Const &arg1, पूर्णांक result_len)
{
	RTLIL::State temp = initial;

	साठी (स्वतः i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_reduce_and(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक, तार्किक, पूर्णांक result_len)
{
	परत logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक, तार्किक, पूर्णांक result_len)
{
	परत logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक, तार्किक, पूर्णांक result_len)
{
	परत logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक, तार्किक, पूर्णांक result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	जर (!buffer.empty()) {
		जर (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		अन्यथा जर (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	परत buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक, तार्किक, पूर्णांक result_len)
{
	परत logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_logic_and(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_logic_or(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
स्थिर RTLIL::Const const_shift_worker(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक sign_ext, तार्किक signed2, पूर्णांक direction, पूर्णांक result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	जर (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	जर (undef_bit_pos >= 0)
		परत result;

	साठी (पूर्णांक i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		जर (pos < 0)
			result.set(i, vacant_bits);
		अन्यथा जर (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		अन्यथा
			result.set(i, arg1[pos.toInt()]);
	}

	परत result;
}

RTLIL::Const RTLIL::const_shl(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	परत const_shift_worker(arg1_ext, arg2, असत्य, असत्य, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	परत const_shift_worker(arg1_ext, arg2, असत्य, असत्य, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	परत const_shift_worker(arg1, arg2, signed1, असत्य, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	परत const_shift_worker(arg1, arg2, signed1, असत्य, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	परत const_shift_worker(arg1_ext, arg2, असत्य, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक, तार्किक signed2, पूर्णांक result_len)
{
	परत const_shift_worker(arg1, arg2, असत्य, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_le(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_eq(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	पूर्णांक width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	साठी (स्वतः i = 0; i < arg1_ext.size(); i++) {
		जर (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			परत result;
		जर (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			परत result;
		जर (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	परत result;
}

RTLIL::Const RTLIL::const_ne(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	जर (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	अन्यथा जर (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_eqx(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	पूर्णांक width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	साठी (स्वतः i = 0; i < arg1_ext.size(); i++) {
		जर (arg1_ext.at(i) != arg2_ext.at(i))
			परत result;
	}

	result.set(0, RTLIL::State::S1);
	परत result;
}

RTLIL::Const RTLIL::const_nex(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	जर (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	अन्यथा जर (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_ge(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_gt(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	तार्किक y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	जर (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	परत result;
}

RTLIL::Const RTLIL::const_add(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	परत big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	परत big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	परत big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	जर (b.isZero())
		परत RTLIL::Const(RTLIL::State::Sx, result_len);
	तार्किक result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	परत big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	जर (b.isZero())
		परत RTLIL::Const(RTLIL::State::Sx, result_len);
	तार्किक result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	परत big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	जर (b.isZero())
		परत RTLIL::Const(RTLIL::State::Sx, result_len);

	तार्किक result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	जर (result_pos || a == 0) {
		result = a / b;
	} अन्यथा {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	परत big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	जर (b.isZero())
		परत RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	जर (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} अन्यथा {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	परत big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, तार्किक signed1, तार्किक signed2, पूर्णांक result_len)
{
	पूर्णांक undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	जर (a == 0 && b < 0)
		परत RTLIL::Const(RTLIL::State::Sx, result_len);

	जर (a == 0 && b > 0)
		परत RTLIL::Const(RTLIL::State::S0, result_len);

	जर (b < 0)
	{
		जर (a < -1 || a > 1)
			y = 0;
		जर (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	जर (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		पूर्णांक modulus_bits = (result_len >= 0 ? result_len : 1024);
		साठी (पूर्णांक i = 0; i < modulus_bits; i++)
			modulus *= 2;

		तार्किक flip_result_sign = असत्य;
		जर (a < 0) {
			a *= -1;
			जर (b % 2 == 1)
				flip_result_sign = सत्य;
		}

		पर्यंत (b > 0) {
			जर (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		जर (flip_result_sign)
			y *= -1;
	}

	परत big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	परत arg1_ext;
}

RTLIL::Const RTLIL::const_buf(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	परत arg1_ext;
}

RTLIL::Const RTLIL::const_neg(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const&, तार्किक signed1, तार्किक, पूर्णांक result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	परत RTLIL::const_sub(zero, arg1_ext, सत्य, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, स्थिरांक RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	जर (arg3[0] == State::S0)
		परत arg1;
	अन्यथा जर (arg3[0] == State::S1)
		परत arg2;

	RTLIL::Const ret = arg1;
	साठी (स्वतः i = 0; i < ret.size(); i++)
		जर (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	परत ret;
}

RTLIL::Const RTLIL::const_pmux(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, स्थिरांक RTLIL::Const &arg3)
{
	जर (arg3.is_fully_zero())
		परत arg1;

	जर (!arg3.is_onehot())
		परत RTLIL::Const(State::Sx, arg1.size());

	साठी (स्वतः i = 0; i < arg3.size(); i++)
		जर (arg3[i] == State::S1)
			परत RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	साठी (पूर्णांक i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		जर (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		अन्यथा जर (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		अन्यथा
			साठी (पूर्णांक j = 0; j < GetSize(t)/2; j++)
				new_t.ढकला_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	परत t;
}

RTLIL::Const RTLIL::const_demux(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2)
{
	पूर्णांक width = GetSize(arg1);
	पूर्णांक s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	साठी (पूर्णांक i = 0; i < (1 << s_width); i++)
	{
		तार्किक ne = असत्य;
		तार्किक x = असत्य;
		साठी (पूर्णांक j = 0; j < s_width; j++) {
			तार्किक bit = i & 1 << j;
			जर (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = सत्य;
			अन्यथा जर (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = सत्य;
		}
		जर (ne) {
			साठी (पूर्णांक j = 0; j < width; j++)
				res.ढकला_back(State::S0);
		} अन्यथा जर (x) {
			साठी (पूर्णांक j = 0; j < width; j++)
				res.ढकला_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} अन्यथा {
			साठी (पूर्णांक j = 0; j < width; j++)
				res.ढकला_back(arg1[j]);
		}
	}
	परत res;
}

RTLIL::Const RTLIL::const_bweqx(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	साठी (स्वतः i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	परत result;
}

RTLIL::Const RTLIL::const_bwmux(स्थिरांक RTLIL::Const &arg1, स्थिरांक RTLIL::Const &arg2, स्थिरांक RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	साठी (स्वतः i = 0; i < arg1.size(); i++) {
		जर (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	परत result;
}

YOSYS_NAMESPACE_END
