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

ساكن فراغ extend_u0(RTLIL::Const &arg, صحيح width, منطقي is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	إذا (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

ساكن BigInteger const2big(ثابت RTLIL::Const &val, منطقي as_signed, صحيح &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto لمب_bits = val.size();

	إذا (as_signed && لمب_bits && val[لمب_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		لمب_bits--;
	}

	لكل (auto i = 0; i < لمب_bits; i++)
		إذا (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		وإلا إذا (undef_bit_pos < 0)
			undef_bit_pos = i;

	إذا (sign == BigInteger::negative)
		mag += 1;

	أرجع BigInteger(mag, sign);
}

ساكن RTLIL::Const big2const(ثابت BigInteger &val, صحيح result_len, صحيح undef_bit_pos)
{
	إذا (undef_bit_pos >= 0)
		أرجع RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	إذا (!mag.isZero())
	{
		إذا (val.getSign() < 0)
		{
			mag--;
			لكل (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		وإلا
		{
			لكل (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#إذا 0
	إذا (undef_bit_pos >= 0)
		لكل (صحيح i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	أرجع result;
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

RTLIL::Const RTLIL::const_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	إذا (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	لكل (auto i = 0; i < result_len; i++) {
		إذا (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		وإلا إذا (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		وإلا إذا (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	أرجع result;
}

ساكن RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, منطقي signed1, منطقي signed2, صحيح result_len = -1)
{
	إذا (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	لكل (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	أرجع result;
}

RTLIL::Const RTLIL::const_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	أرجع logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	أرجع logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	أرجع logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	أرجع logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

ساكن RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ثابت RTLIL::Const &arg1, صحيح result_len)
{
	RTLIL::State temp = initial;

	لكل (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_reduce_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	إذا (!buffer.empty()) {
		إذا (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		وإلا إذا (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	أرجع buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	أرجع logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	صحيح undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_logic_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_logic_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ساكن RTLIL::Const const_shift_worker(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي sign_ext, منطقي signed2, صحيح direction, صحيح result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	صحيح undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	إذا (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	إذا (undef_bit_pos >= 0)
		أرجع result;

	لكل (صحيح i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		إذا (pos < 0)
			result.set(i, vacant_bits);
		وإلا إذا (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		وإلا
			result.set(i, arg1[pos.toInt()]);
	}

	أرجع result;
}

RTLIL::Const RTLIL::const_shl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	أرجع const_shift_worker(arg1_ext, arg2, خطأ, خطأ, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	أرجع const_shift_worker(arg1_ext, arg2, خطأ, خطأ, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	أرجع const_shift_worker(arg1, arg2, signed1, خطأ, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	أرجع const_shift_worker(arg1, arg2, signed1, خطأ, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	أرجع const_shift_worker(arg1_ext, arg2, خطأ, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي, منطقي signed2, صحيح result_len)
{
	أرجع const_shift_worker(arg1, arg2, خطأ, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_le(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_eq(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	صحيح width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	لكل (auto i = 0; i < arg1_ext.size(); i++) {
		إذا (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			أرجع result;
		إذا (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			أرجع result;
		إذا (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	أرجع result;
}

RTLIL::Const RTLIL::const_ne(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	إذا (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	وإلا إذا (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_eqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	صحيح width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	لكل (auto i = 0; i < arg1_ext.size(); i++) {
		إذا (arg1_ext.at(i) != arg2_ext.at(i))
			أرجع result;
	}

	result.set(0, RTLIL::State::S1);
	أرجع result;
}

RTLIL::Const RTLIL::const_nex(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	إذا (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	وإلا إذا (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_ge(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_gt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	إذا (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	أرجع result;
}

RTLIL::Const RTLIL::const_add(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	أرجع big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	أرجع big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	أرجع big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, result_len);
	منطقي result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	أرجع big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, result_len);
	منطقي result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	أرجع big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, result_len);

	منطقي result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	إذا (result_pos || a == 0) {
		result = a / b;
	} وإلا {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	أرجع big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	إذا (b.isZero())
		أرجع RTLIL::Const(RTLIL::State::Sx, result_len);

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
	أرجع big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	إذا (a == 0 && b < 0)
		أرجع RTLIL::Const(RTLIL::State::Sx, result_len);

	إذا (a == 0 && b > 0)
		أرجع RTLIL::Const(RTLIL::State::S0, result_len);

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
		صحيح modulus_bits = (result_len >= 0 ? result_len : 1024);
		لكل (صحيح i = 0; i < modulus_bits; i++)
			modulus *= 2;

		منطقي flip_result_sign = خطأ;
		إذا (a < 0) {
			a *= -1;
			إذا (b % 2 == 1)
				flip_result_sign = true;
		}

		طالما (b > 0) {
			إذا (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		إذا (flip_result_sign)
			y *= -1;
	}

	أرجع big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	أرجع arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	أرجع arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	أرجع RTLIL::const_sub(zero, arg1_ext, true, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	إذا (arg3[0] == State::S0)
		أرجع arg1;
	وإلا إذا (arg3[0] == State::S1)
		أرجع arg2;

	RTLIL::Const ret = arg1;
	لكل (auto i = 0; i < ret.size(); i++)
		إذا (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	أرجع ret;
}

RTLIL::Const RTLIL::const_pmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	إذا (arg3.is_fully_zero())
		أرجع arg1;

	إذا (!arg3.is_onehot())
		أرجع RTLIL::Const(State::Sx, arg1.size());

	لكل (auto i = 0; i < arg3.size(); i++)
		إذا (arg3[i] == State::S1)
			أرجع RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	لكل (صحيح i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		إذا (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		وإلا إذا (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		وإلا
			لكل (صحيح j = 0; j < GetSize(t)/2; j++)
				new_t.تور_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	أرجع t;
}

RTLIL::Const RTLIL::const_demux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	صحيح width = GetSize(arg1);
	صحيح s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	لكل (صحيح i = 0; i < (1 << s_width); i++)
	{
		منطقي ne = خطأ;
		منطقي x = خطأ;
		لكل (صحيح j = 0; j < s_width; j++) {
			منطقي bit = i & 1 << j;
			إذا (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = true;
			وإلا إذا (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = true;
		}
		إذا (ne) {
			لكل (صحيح j = 0; j < width; j++)
				res.تور_back(State::S0);
		} وإلا إذا (x) {
			لكل (صحيح j = 0; j < width; j++)
				res.تور_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} وإلا {
			لكل (صحيح j = 0; j < width; j++)
				res.تور_back(arg1[j]);
		}
	}
	أرجع res;
}

RTLIL::Const RTLIL::const_bweqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	لكل (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	أرجع result;
}

RTLIL::Const RTLIL::const_bwmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	لكل (auto i = 0; i < arg1.size(); i++) {
		إذا (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	أرجع result;
}

YOSYS_NAMESPACE_END
