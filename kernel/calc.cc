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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

நிலையான வெறுமை extend_u0(RTLIL::Const &arg, முழு width, தருக்க is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	எனில் (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

நிலையான BigInteger const2big(மாறிலி RTLIL::Const &val, தருக்க as_signed, முழு &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto இலக்கம்_bits = val.size();

	எனில் (as_signed && இலக்கம்_bits && val[இலக்கம்_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		இலக்கம்_bits--;
	}

	க்கு (auto i = 0; i < இலக்கம்_bits; i++)
		எனில் (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		இல்லையெனில் எனில் (undef_bit_pos < 0)
			undef_bit_pos = i;

	எனில் (sign == BigInteger::negative)
		mag += 1;

	திரும்பு BigInteger(mag, sign);
}

நிலையான RTLIL::Const big2const(மாறிலி BigInteger &val, முழு result_len, முழு undef_bit_pos)
{
	எனில் (undef_bit_pos >= 0)
		திரும்பு RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	எனில் (!mag.isZero())
	{
		எனில் (val.getSign() < 0)
		{
			mag--;
			க்கு (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		இல்லையெனில்
		{
			க்கு (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#எனில் 0
	எனில் (undef_bit_pos >= 0)
		க்கு (முழு i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	திரும்பு result;
}

நிலையான RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	எனில் (a == RTLIL::State::S0) திரும்பு RTLIL::State::S0;
	எனில் (b == RTLIL::State::S0) திரும்பு RTLIL::State::S0;
	எனில் (a != RTLIL::State::S1) திரும்பு RTLIL::State::Sx;
	எனில் (b != RTLIL::State::S1) திரும்பு RTLIL::State::Sx;
	திரும்பு RTLIL::State::S1;
}

நிலையான RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	எனில் (a == RTLIL::State::S1) திரும்பு RTLIL::State::S1;
	எனில் (b == RTLIL::State::S1) திரும்பு RTLIL::State::S1;
	எனில் (a != RTLIL::State::S0) திரும்பு RTLIL::State::Sx;
	எனில் (b != RTLIL::State::S0) திரும்பு RTLIL::State::Sx;
	திரும்பு RTLIL::State::S0;
}

நிலையான RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	எனில் (a != RTLIL::State::S0 && a != RTLIL::State::S1) திரும்பு RTLIL::State::Sx;
	எனில் (b != RTLIL::State::S0 && b != RTLIL::State::S1) திரும்பு RTLIL::State::Sx;
	திரும்பு a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

நிலையான RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	எனில் (a != RTLIL::State::S0 && a != RTLIL::State::S1) திரும்பு RTLIL::State::Sx;
	எனில் (b != RTLIL::State::S0 && b != RTLIL::State::S1) திரும்பு RTLIL::State::Sx;
	திரும்பு a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க signed1, தருக்க, முழு result_len)
{
	எனில் (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	க்கு (auto i = 0; i < result_len; i++) {
		எனில் (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		இல்லையெனில் எனில் (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		இல்லையெனில் எனில் (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	திரும்பு result;
}

நிலையான RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, தருக்க signed1, தருக்க signed2, முழு result_len = -1)
{
	எனில் (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	க்கு (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	திரும்பு result;
}

RTLIL::Const RTLIL::const_and(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	திரும்பு logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	திரும்பு logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	திரும்பு logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	திரும்பு logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

நிலையான RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), மாறிலி RTLIL::Const &arg1, முழு result_len)
{
	RTLIL::State temp = initial;

	க்கு (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_reduce_and(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க, தருக்க, முழு result_len)
{
	திரும்பு logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க, தருக்க, முழு result_len)
{
	திரும்பு logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க, தருக்க, முழு result_len)
{
	திரும்பு logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க, தருக்க, முழு result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	எனில் (!buffer.empty()) {
		எனில் (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		இல்லையெனில் எனில் (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	திரும்பு buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க, தருக்க, முழு result_len)
{
	திரும்பு logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க signed1, தருக்க, முழு result_len)
{
	முழு undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_logic_and(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_logic_or(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
நிலையான RTLIL::Const const_shift_worker(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க sign_ext, தருக்க signed2, முழு direction, முழு result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	முழு undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	எனில் (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	எனில் (undef_bit_pos >= 0)
		திரும்பு result;

	க்கு (முழு i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		எனில் (pos < 0)
			result.set(i, vacant_bits);
		இல்லையெனில் எனில் (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		இல்லையெனில்
			result.set(i, arg1[pos.toInt()]);
	}

	திரும்பு result;
}

RTLIL::Const RTLIL::const_shl(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	திரும்பு const_shift_worker(arg1_ext, arg2, பொய், பொய், -1, result_len);
}

RTLIL::Const RTLIL::const_shr(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	திரும்பு const_shift_worker(arg1_ext, arg2, பொய், பொய், +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க, முழு result_len)
{
	திரும்பு const_shift_worker(arg1, arg2, signed1, பொய், -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க, முழு result_len)
{
	திரும்பு const_shift_worker(arg1, arg2, signed1, பொய், +1, result_len);
}

RTLIL::Const RTLIL::const_shift(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	திரும்பு const_shift_worker(arg1_ext, arg2, பொய், signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க, தருக்க signed2, முழு result_len)
{
	திரும்பு const_shift_worker(arg1, arg2, பொய், signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	தருக்க y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_le(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	தருக்க y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_eq(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	முழு width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	க்கு (auto i = 0; i < arg1_ext.size(); i++) {
		எனில் (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			திரும்பு result;
		எனில் (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			திரும்பு result;
		எனில் (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_ne(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	எனில் (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	இல்லையெனில் எனில் (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_eqx(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	முழு width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	க்கு (auto i = 0; i < arg1_ext.size(); i++) {
		எனில் (arg1_ext.at(i) != arg2_ext.at(i))
			திரும்பு result;
	}

	result.set(0, RTLIL::State::S1);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_nex(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	எனில் (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	இல்லையெனில் எனில் (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_ge(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	தருக்க y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_gt(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	தருக்க y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	எனில் (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	திரும்பு result;
}

RTLIL::Const RTLIL::const_add(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	திரும்பு big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	திரும்பு big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	திரும்பு big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	எனில் (b.isZero())
		திரும்பு RTLIL::Const(RTLIL::State::Sx, result_len);
	தருக்க result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	திரும்பு big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	எனில் (b.isZero())
		திரும்பு RTLIL::Const(RTLIL::State::Sx, result_len);
	தருக்க result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	திரும்பு big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	எனில் (b.isZero())
		திரும்பு RTLIL::Const(RTLIL::State::Sx, result_len);

	தருக்க result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	எனில் (result_pos || a == 0) {
		result = a / b;
	} இல்லையெனில் {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	திரும்பு big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	எனில் (b.isZero())
		திரும்பு RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	எனில் (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} இல்லையெனில் {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	திரும்பு big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, தருக்க signed1, தருக்க signed2, முழு result_len)
{
	முழு undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	எனில் (a == 0 && b < 0)
		திரும்பு RTLIL::Const(RTLIL::State::Sx, result_len);

	எனில் (a == 0 && b > 0)
		திரும்பு RTLIL::Const(RTLIL::State::S0, result_len);

	எனில் (b < 0)
	{
		எனில் (a < -1 || a > 1)
			y = 0;
		எனில் (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	எனில் (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		முழு modulus_bits = (result_len >= 0 ? result_len : 1024);
		க்கு (முழு i = 0; i < modulus_bits; i++)
			modulus *= 2;

		தருக்க flip_result_sign = பொய்;
		எனில் (a < 0) {
			a *= -1;
			எனில் (b % 2 == 1)
				flip_result_sign = உண்மை;
		}

		போது (b > 0) {
			எனில் (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		எனில் (flip_result_sign)
			y *= -1;
	}

	திரும்பு big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க signed1, தருக்க, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	திரும்பு arg1_ext;
}

RTLIL::Const RTLIL::const_buf(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க signed1, தருக்க, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	திரும்பு arg1_ext;
}

RTLIL::Const RTLIL::const_neg(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const&, தருக்க signed1, தருக்க, முழு result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	திரும்பு RTLIL::const_sub(zero, arg1_ext, உண்மை, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, மாறிலி RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	எனில் (arg3[0] == State::S0)
		திரும்பு arg1;
	இல்லையெனில் எனில் (arg3[0] == State::S1)
		திரும்பு arg2;

	RTLIL::Const ret = arg1;
	க்கு (auto i = 0; i < ret.size(); i++)
		எனில் (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	திரும்பு ret;
}

RTLIL::Const RTLIL::const_pmux(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, மாறிலி RTLIL::Const &arg3)
{
	எனில் (arg3.is_fully_zero())
		திரும்பு arg1;

	எனில் (!arg3.is_onehot())
		திரும்பு RTLIL::Const(State::Sx, arg1.size());

	க்கு (auto i = 0; i < arg3.size(); i++)
		எனில் (arg3[i] == State::S1)
			திரும்பு RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	க்கு (முழு i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		எனில் (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		இல்லையெனில் எனில் (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		இல்லையெனில்
			க்கு (முழு j = 0; j < GetSize(t)/2; j++)
				new_t.தள்ளு_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	திரும்பு t;
}

RTLIL::Const RTLIL::const_demux(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2)
{
	முழு width = GetSize(arg1);
	முழு s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	க்கு (முழு i = 0; i < (1 << s_width); i++)
	{
		தருக்க ne = பொய்;
		தருக்க x = பொய்;
		க்கு (முழு j = 0; j < s_width; j++) {
			தருக்க bit = i & 1 << j;
			எனில் (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = உண்மை;
			இல்லையெனில் எனில் (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = உண்மை;
		}
		எனில் (ne) {
			க்கு (முழு j = 0; j < width; j++)
				res.தள்ளு_back(State::S0);
		} இல்லையெனில் எனில் (x) {
			க்கு (முழு j = 0; j < width; j++)
				res.தள்ளு_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} இல்லையெனில் {
			க்கு (முழு j = 0; j < width; j++)
				res.தள்ளு_back(arg1[j]);
		}
	}
	திரும்பு res;
}

RTLIL::Const RTLIL::const_bweqx(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	க்கு (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	திரும்பு result;
}

RTLIL::Const RTLIL::const_bwmux(மாறிலி RTLIL::Const &arg1, மாறிலி RTLIL::Const &arg2, மாறிலி RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	க்கு (auto i = 0; i < arg1.size(); i++) {
		எனில் (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	திரும்பு result;
}

YOSYS_NAMESPACE_END
