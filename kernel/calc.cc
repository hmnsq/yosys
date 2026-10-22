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

	אם (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

ساكن BigInteger const2big(ثابت RTLIL::Const &val, منطقي as_signed, صحيح &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto num_bits = val.size();

	אם (as_signed && num_bits && val[num_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		num_bits--;
	}

	עבור (auto i = 0; i < num_bits; i++)
		אם (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		אחרת אם (undef_bit_pos < 0)
			undef_bit_pos = i;

	אם (sign == BigInteger::negative)
		mag += 1;

	החזר BigInteger(mag, sign);
}

ساكن RTLIL::Const big2const(ثابت BigInteger &val, صحيح result_len, صحيح undef_bit_pos)
{
	אם (undef_bit_pos >= 0)
		החזר RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	אם (!mag.isZero())
	{
		אם (val.getSign() < 0)
		{
			mag--;
			עבור (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		אחרת
		{
			עבור (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#אם 0
	אם (undef_bit_pos >= 0)
		עבור (صحيح i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	החזר result;
}

ساكن RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	אם (a == RTLIL::State::S0) החזר RTLIL::State::S0;
	אם (b == RTLIL::State::S0) החזר RTLIL::State::S0;
	אם (a != RTLIL::State::S1) החזר RTLIL::State::Sx;
	אם (b != RTLIL::State::S1) החזר RTLIL::State::Sx;
	החזר RTLIL::State::S1;
}

ساكن RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	אם (a == RTLIL::State::S1) החזר RTLIL::State::S1;
	אם (b == RTLIL::State::S1) החזר RTLIL::State::S1;
	אם (a != RTLIL::State::S0) החזר RTLIL::State::Sx;
	אם (b != RTLIL::State::S0) החזר RTLIL::State::Sx;
	החזר RTLIL::State::S0;
}

ساكن RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	אם (a != RTLIL::State::S0 && a != RTLIL::State::S1) החזר RTLIL::State::Sx;
	אם (b != RTLIL::State::S0 && b != RTLIL::State::S1) החזר RTLIL::State::Sx;
	החזר a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

ساكن RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	אם (a != RTLIL::State::S0 && a != RTLIL::State::S1) החזר RTLIL::State::Sx;
	אם (b != RTLIL::State::S0 && b != RTLIL::State::S1) החזר RTLIL::State::Sx;
	החזר a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	אם (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	עבור (auto i = 0; i < result_len; i++) {
		אם (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		אחרת אם (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		אחרת אם (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	החזר result;
}

ساكن RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, منطقي signed1, منطقي signed2, صحيح result_len = -1)
{
	אם (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	עבור (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	החזר result;
}

RTLIL::Const RTLIL::const_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	החזר logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	החזר logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	החזר logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	החזר logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

ساكن RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ثابت RTLIL::Const &arg1, صحيح result_len)
{
	RTLIL::State temp = initial;

	עבור (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_reduce_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	החזר logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	החזר logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	החזר logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	אם (!buffer.empty()) {
		אם (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		אחרת אם (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	החזר buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي, منطقي, صحيح result_len)
{
	החזר logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	صحيح undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_logic_and(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_logic_or(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
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

	אם (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	אם (undef_bit_pos >= 0)
		החזר result;

	עבור (صحيح i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		אם (pos < 0)
			result.set(i, vacant_bits);
		אחרת אם (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		אחרת
			result.set(i, arg1[pos.toInt()]);
	}

	החזר result;
}

RTLIL::Const RTLIL::const_shl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	החזר const_shift_worker(arg1_ext, arg2, שקר, שקר, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	החזר const_shift_worker(arg1_ext, arg2, שקר, שקר, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	החזר const_shift_worker(arg1, arg2, signed1, שקר, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي, صحيح result_len)
{
	החזר const_shift_worker(arg1, arg2, signed1, שקר, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	החזר const_shift_worker(arg1_ext, arg2, שקר, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي, منطقي signed2, صحيح result_len)
{
	החזר const_shift_worker(arg1, arg2, שקר, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_le(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
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
	עבור (auto i = 0; i < arg1_ext.size(); i++) {
		אם (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			החזר result;
		אם (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			החזר result;
		אם (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	החזר result;
}

RTLIL::Const RTLIL::const_ne(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	אם (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	אחרת אם (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_eqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	صحيح width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	עבור (auto i = 0; i < arg1_ext.size(); i++) {
		אם (arg1_ext.at(i) != arg2_ext.at(i))
			החזר result;
	}

	result.set(0, RTLIL::State::S1);
	החזר result;
}

RTLIL::Const RTLIL::const_nex(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	אם (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	אחרת אם (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_ge(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_gt(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	منطقي y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	אם (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	החזר result;
}

RTLIL::Const RTLIL::const_add(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	החזר big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	החזר big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	החזר big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	אם (b.isZero())
		החזר RTLIL::Const(RTLIL::State::Sx, result_len);
	منطقي result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	החזר big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	אם (b.isZero())
		החזר RTLIL::Const(RTLIL::State::Sx, result_len);
	منطقي result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	החזר big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	אם (b.isZero())
		החזר RTLIL::Const(RTLIL::State::Sx, result_len);

	منطقي result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	אם (result_pos || a == 0) {
		result = a / b;
	} אחרת {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	החזר big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	אם (b.isZero())
		החזר RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	אם (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} אחרת {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	החזר big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, منطقي signed1, منطقي signed2, صحيح result_len)
{
	صحيح undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	אם (a == 0 && b < 0)
		החזר RTLIL::Const(RTLIL::State::Sx, result_len);

	אם (a == 0 && b > 0)
		החזר RTLIL::Const(RTLIL::State::S0, result_len);

	אם (b < 0)
	{
		אם (a < -1 || a > 1)
			y = 0;
		אם (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	אם (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		صحيح modulus_bits = (result_len >= 0 ? result_len : 1024);
		עבור (صحيح i = 0; i < modulus_bits; i++)
			modulus *= 2;

		منطقي flip_result_sign = שקר;
		אם (a < 0) {
			a *= -1;
			אם (b % 2 == 1)
				flip_result_sign = אמת;
		}

		כלעוד (b > 0) {
			אם (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		אם (flip_result_sign)
			y *= -1;
	}

	החזר big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	החזר arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	החזר arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const&, منطقي signed1, منطقي, صحيح result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	החזר RTLIL::const_sub(zero, arg1_ext, אמת, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	אם (arg3[0] == State::S0)
		החזר arg1;
	אחרת אם (arg3[0] == State::S1)
		החזר arg2;

	RTLIL::Const ret = arg1;
	עבור (auto i = 0; i < ret.size(); i++)
		אם (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	החזר ret;
}

RTLIL::Const RTLIL::const_pmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	אם (arg3.is_fully_zero())
		החזר arg1;

	אם (!arg3.is_onehot())
		החזר RTLIL::Const(State::Sx, arg1.size());

	עבור (auto i = 0; i < arg3.size(); i++)
		אם (arg3[i] == State::S1)
			החזר RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	עבור (صحيح i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		אם (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		אחרת אם (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		אחרת
			עבור (صحيح j = 0; j < GetSize(t)/2; j++)
				new_t.push_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	החזר t;
}

RTLIL::Const RTLIL::const_demux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	صحيح width = GetSize(arg1);
	صحيح s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	עבור (صحيح i = 0; i < (1 << s_width); i++)
	{
		منطقي ne = שקר;
		منطقي x = שקר;
		עבור (صحيح j = 0; j < s_width; j++) {
			منطقي bit = i & 1 << j;
			אם (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = אמת;
			אחרת אם (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = אמת;
		}
		אם (ne) {
			עבור (صحيح j = 0; j < width; j++)
				res.push_back(State::S0);
		} אחרת אם (x) {
			עבור (صحيح j = 0; j < width; j++)
				res.push_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} אחרת {
			עבור (صحيح j = 0; j < width; j++)
				res.push_back(arg1[j]);
		}
	}
	החזר res;
}

RTLIL::Const RTLIL::const_bweqx(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	עבור (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	החזר result;
}

RTLIL::Const RTLIL::const_bwmux(ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	עבור (auto i = 0; i < arg1.size(); i++) {
		אם (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	החזר result;
}

YOSYS_NAMESPACE_END
