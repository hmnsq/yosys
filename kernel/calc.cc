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

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

স্থির শূন্য extend_u0(RTLIL::Const &arg, পূর্ণাঙ্ক width, তার্কিক is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	যদি (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

স্থির BigInteger const2big(ধ্রুৱ RTLIL::Const &val, তার্কিক as_signed, পূর্ণাঙ্ক &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto অঙ্ক_bits = val.size();

	যদি (as_signed && অঙ্ক_bits && val[অঙ্ক_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		অঙ্ক_bits--;
	}

	বাবে (auto i = 0; i < অঙ্ক_bits; i++)
		যদি (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		অন্যথা যদি (undef_bit_pos < 0)
			undef_bit_pos = i;

	যদি (sign == BigInteger::negative)
		mag += 1;

	ঘূৰাই BigInteger(mag, sign);
}

স্থির RTLIL::Const big2const(ধ্রুৱ BigInteger &val, পূর্ণাঙ্ক result_len, পূর্ণাঙ্ক undef_bit_pos)
{
	যদি (undef_bit_pos >= 0)
		ঘূৰাই RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	যদি (!mag.isZero())
	{
		যদি (val.getSign() < 0)
		{
			mag--;
			বাবে (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		অন্যথা
		{
			বাবে (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#যদি 0
	যদি (undef_bit_pos >= 0)
		বাবে (পূর্ণাঙ্ক i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	ঘূৰাই result;
}

স্থির RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	যদি (a == RTLIL::State::S0) ঘূৰাই RTLIL::State::S0;
	যদি (b == RTLIL::State::S0) ঘূৰাই RTLIL::State::S0;
	যদি (a != RTLIL::State::S1) ঘূৰাই RTLIL::State::Sx;
	যদি (b != RTLIL::State::S1) ঘূৰাই RTLIL::State::Sx;
	ঘূৰাই RTLIL::State::S1;
}

স্থির RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	যদি (a == RTLIL::State::S1) ঘূৰাই RTLIL::State::S1;
	যদি (b == RTLIL::State::S1) ঘূৰাই RTLIL::State::S1;
	যদি (a != RTLIL::State::S0) ঘূৰাই RTLIL::State::Sx;
	যদি (b != RTLIL::State::S0) ঘূৰাই RTLIL::State::Sx;
	ঘূৰাই RTLIL::State::S0;
}

স্থির RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	যদি (a != RTLIL::State::S0 && a != RTLIL::State::S1) ঘূৰাই RTLIL::State::Sx;
	যদি (b != RTLIL::State::S0 && b != RTLIL::State::S1) ঘূৰাই RTLIL::State::Sx;
	ঘূৰাই a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

স্থির RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	যদি (a != RTLIL::State::S0 && a != RTLIL::State::S1) ঘূৰাই RTLIL::State::Sx;
	যদি (b != RTLIL::State::S0 && b != RTLIL::State::S1) ঘূৰাই RTLIL::State::Sx;
	ঘূৰাই a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	যদি (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	বাবে (auto i = 0; i < result_len; i++) {
		যদি (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		অন্যথা যদি (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		অন্যথা যদি (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	ঘূৰাই result;
}

স্থির RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len = -1)
{
	যদি (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	বাবে (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_and(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

স্থির RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ধ্রুৱ RTLIL::Const &arg1, পূর্ণাঙ্ক result_len)
{
	RTLIL::State temp = initial;

	বাবে (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_reduce_and(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক, তার্কিক, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক, তার্কিক, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক, তার্কিক, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক, তার্কিক, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	যদি (!buffer.empty()) {
		যদি (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		অন্যথা যদি (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	ঘূৰাই buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক, তার্কিক, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_logic_and(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_logic_or(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
স্থির RTLIL::Const const_shift_worker(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক sign_ext, তার্কিক signed2, পূর্ণাঙ্ক direction, পূর্ণাঙ্ক result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	যদি (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	যদি (undef_bit_pos >= 0)
		ঘূৰাই result;

	বাবে (পূর্ণাঙ্ক i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		যদি (pos < 0)
			result.set(i, vacant_bits);
		অন্যথা যদি (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		অন্যথা
			result.set(i, arg1[pos.toInt()]);
	}

	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_shl(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	ঘূৰাই const_shift_worker(arg1_ext, arg2, মিছা, মিছা, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ঘূৰাই const_shift_worker(arg1_ext, arg2, মিছা, মিছা, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই const_shift_worker(arg1, arg2, signed1, মিছা, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই const_shift_worker(arg1, arg2, signed1, মিছা, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ঘূৰাই const_shift_worker(arg1_ext, arg2, মিছা, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	ঘূৰাই const_shift_worker(arg1, arg2, মিছা, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	তার্কিক y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_le(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	তার্কিক y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_eq(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	পূর্ণাঙ্ক width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	বাবে (auto i = 0; i < arg1_ext.size(); i++) {
		যদি (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			ঘূৰাই result;
		যদি (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			ঘূৰাই result;
		যদি (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_ne(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	যদি (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	অন্যথা যদি (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_eqx(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	পূর্ণাঙ্ক width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	বাবে (auto i = 0; i < arg1_ext.size(); i++) {
		যদি (arg1_ext.at(i) != arg2_ext.at(i))
			ঘূৰাই result;
	}

	result.set(0, RTLIL::State::S1);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_nex(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	যদি (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	অন্যথা যদি (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_ge(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	তার্কিক y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_gt(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	তার্কিক y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	যদি (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_add(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	ঘূৰাই big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	ঘূৰাই big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	ঘূৰাই big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	যদি (b.isZero())
		ঘূৰাই RTLIL::Const(RTLIL::State::Sx, result_len);
	তার্কিক result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ঘূৰাই big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	যদি (b.isZero())
		ঘূৰাই RTLIL::Const(RTLIL::State::Sx, result_len);
	তার্কিক result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ঘূৰাই big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	যদি (b.isZero())
		ঘূৰাই RTLIL::Const(RTLIL::State::Sx, result_len);

	তার্কিক result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	যদি (result_pos || a == 0) {
		result = a / b;
	} অন্যথা {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	ঘূৰাই big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	যদি (b.isZero())
		ঘূৰাই RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	যদি (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} অন্যথা {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	ঘূৰাই big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, তার্কিক signed1, তার্কিক signed2, পূর্ণাঙ্ক result_len)
{
	পূর্ণাঙ্ক undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	যদি (a == 0 && b < 0)
		ঘূৰাই RTLIL::Const(RTLIL::State::Sx, result_len);

	যদি (a == 0 && b > 0)
		ঘূৰাই RTLIL::Const(RTLIL::State::S0, result_len);

	যদি (b < 0)
	{
		যদি (a < -1 || a > 1)
			y = 0;
		যদি (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	যদি (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		পূর্ণাঙ্ক modulus_bits = (result_len >= 0 ? result_len : 1024);
		বাবে (পূর্ণাঙ্ক i = 0; i < modulus_bits; i++)
			modulus *= 2;

		তার্কিক flip_result_sign = মিছা;
		যদি (a < 0) {
			a *= -1;
			যদি (b % 2 == 1)
				flip_result_sign = সঁচা;
		}

		যেতিয়ালৈ (b > 0) {
			যদি (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		যদি (flip_result_sign)
			y *= -1;
	}

	ঘূৰাই big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ঘূৰাই arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ঘূৰাই arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const&, তার্কিক signed1, তার্কিক, পূর্ণাঙ্ক result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	ঘূৰাই RTLIL::const_sub(zero, arg1_ext, সঁচা, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, ধ্রুৱ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	যদি (arg3[0] == State::S0)
		ঘূৰাই arg1;
	অন্যথা যদি (arg3[0] == State::S1)
		ঘূৰাই arg2;

	RTLIL::Const ret = arg1;
	বাবে (auto i = 0; i < ret.size(); i++)
		যদি (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	ঘূৰাই ret;
}

RTLIL::Const RTLIL::const_pmux(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, ধ্রুৱ RTLIL::Const &arg3)
{
	যদি (arg3.is_fully_zero())
		ঘূৰাই arg1;

	যদি (!arg3.is_onehot())
		ঘূৰাই RTLIL::Const(State::Sx, arg1.size());

	বাবে (auto i = 0; i < arg3.size(); i++)
		যদি (arg3[i] == State::S1)
			ঘূৰাই RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	বাবে (পূর্ণাঙ্ক i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		যদি (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		অন্যথা যদি (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		অন্যথা
			বাবে (পূর্ণাঙ্ক j = 0; j < GetSize(t)/2; j++)
				new_t.ঠেলো_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	ঘূৰাই t;
}

RTLIL::Const RTLIL::const_demux(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2)
{
	পূর্ণাঙ্ক width = GetSize(arg1);
	পূর্ণাঙ্ক s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	বাবে (পূর্ণাঙ্ক i = 0; i < (1 << s_width); i++)
	{
		তার্কিক ne = মিছা;
		তার্কিক x = মিছা;
		বাবে (পূর্ণাঙ্ক j = 0; j < s_width; j++) {
			তার্কিক bit = i & 1 << j;
			যদি (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = সঁচা;
			অন্যথা যদি (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = সঁচা;
		}
		যদি (ne) {
			বাবে (পূর্ণাঙ্ক j = 0; j < width; j++)
				res.ঠেলো_back(State::S0);
		} অন্যথা যদি (x) {
			বাবে (পূর্ণাঙ্ক j = 0; j < width; j++)
				res.ঠেলো_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} অন্যথা {
			বাবে (পূর্ণাঙ্ক j = 0; j < width; j++)
				res.ঠেলো_back(arg1[j]);
		}
	}
	ঘূৰাই res;
}

RTLIL::Const RTLIL::const_bweqx(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	বাবে (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	ঘূৰাই result;
}

RTLIL::Const RTLIL::const_bwmux(ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, ধ্রুৱ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	বাবে (auto i = 0; i < arg1.size(); i++) {
		যদি (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	ঘূৰাই result;
}

YOSYS_NAMESPACE_END
