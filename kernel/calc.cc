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

#include "kernel/yosys.h"
#include "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

static void extend_u0(RTLIL::Const &arg, ຈຳນວນເຕັມ width, bool is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	ຖ້າ (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

static BigInteger const2big(const RTLIL::Const &val, bool as_signed, ຈຳນວນເຕັມ &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto num_bits = val.size();

	ຖ້າ (as_signed && num_bits && val[num_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		num_bits--;
	}

	ສຳລັບ (auto i = 0; i < num_bits; i++)
		ຖ້າ (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (undef_bit_pos < 0)
			undef_bit_pos = i;

	ຖ້າ (sign == BigInteger::negative)
		mag += 1;

	ສົ່ງຄືນ BigInteger(mag, sign);
}

static RTLIL::Const big2const(const BigInteger &val, ຈຳນວນເຕັມ result_len, ຈຳນວນເຕັມ undef_bit_pos)
{
	ຖ້າ (undef_bit_pos >= 0)
		ສົ່ງຄືນ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	ຖ້າ (!mag.isZero())
	{
		ຖ້າ (val.getSign() < 0)
		{
			mag--;
			ສຳລັບ (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		ບໍ່ດັ່ງນັ້ນ
		{
			ສຳລັບ (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#ຖ້າ 0
	ຖ້າ (undef_bit_pos >= 0)
		ສຳລັບ (ຈຳນວນເຕັມ i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	ສົ່ງຄືນ result;
}

static RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	ຖ້າ (a == RTLIL::State::S0) ສົ່ງຄືນ RTLIL::State::S0;
	ຖ້າ (b == RTLIL::State::S0) ສົ່ງຄືນ RTLIL::State::S0;
	ຖ້າ (a != RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::Sx;
	ຖ້າ (b != RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::Sx;
	ສົ່ງຄືນ RTLIL::State::S1;
}

static RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	ຖ້າ (a == RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::S1;
	ຖ້າ (b == RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::S1;
	ຖ້າ (a != RTLIL::State::S0) ສົ່ງຄືນ RTLIL::State::Sx;
	ຖ້າ (b != RTLIL::State::S0) ສົ່ງຄືນ RTLIL::State::Sx;
	ສົ່ງຄືນ RTLIL::State::S0;
}

static RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	ຖ້າ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::Sx;
	ຖ້າ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::Sx;
	ສົ່ງຄືນ a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

static RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	ຖ້າ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::Sx;
	ຖ້າ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ສົ່ງຄືນ RTLIL::State::Sx;
	ສົ່ງຄືນ a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(const RTLIL::Const &arg1, const RTLIL::Const&, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	ຖ້າ (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ສຳລັບ (auto i = 0; i < result_len; i++) {
		ຖ້າ (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	ສົ່ງຄືນ result;
}

static RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len = -1)
{
	ຖ້າ (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ສຳລັບ (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_and(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

static RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), const RTLIL::Const &arg1, ຈຳນວນເຕັມ result_len)
{
	RTLIL::State temp = initial;

	ສຳລັບ (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_reduce_and(const RTLIL::Const &arg1, const RTLIL::Const&, bool, bool, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(const RTLIL::Const &arg1, const RTLIL::Const&, bool, bool, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(const RTLIL::Const &arg1, const RTLIL::Const&, bool, bool, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(const RTLIL::Const &arg1, const RTLIL::Const&, bool, bool, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	ຖ້າ (!buffer.empty()) {
		ຖ້າ (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	ສົ່ງຄືນ buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(const RTLIL::Const &arg1, const RTLIL::Const&, bool, bool, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(const RTLIL::Const &arg1, const RTLIL::Const&, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_logic_and(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_logic_or(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
static RTLIL::Const const_shift_worker(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool sign_ext, bool signed2, ຈຳນວນເຕັມ direction, ຈຳນວນເຕັມ result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	ຖ້າ (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ຖ້າ (undef_bit_pos >= 0)
		ສົ່ງຄືນ result;

	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		ຖ້າ (pos < 0)
			result.set(i, vacant_bits);
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		ບໍ່ດັ່ງນັ້ນ
			result.set(i, arg1[pos.toInt()]);
	}

	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_shl(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	ສົ່ງຄືນ const_shift_worker(arg1_ext, arg2, false, false, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ສົ່ງຄືນ const_shift_worker(arg1_ext, arg2, false, false, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ const_shift_worker(arg1, arg2, signed1, false, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ const_shift_worker(arg1, arg2, signed1, false, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ສົ່ງຄືນ const_shift_worker(arg1_ext, arg2, false, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ສົ່ງຄືນ const_shift_worker(arg1, arg2, false, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_le(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_eq(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	ຈຳນວນເຕັມ width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	ສຳລັບ (auto i = 0; i < arg1_ext.size(); i++) {
		ຖ້າ (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			ສົ່ງຄືນ result;
		ຖ້າ (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			ສົ່ງຄືນ result;
		ຖ້າ (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_ne(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	ຖ້າ (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_eqx(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	ຈຳນວນເຕັມ width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	ສຳລັບ (auto i = 0; i < arg1_ext.size(); i++) {
		ຖ້າ (arg1_ext.at(i) != arg2_ext.at(i))
			ສົ່ງຄືນ result;
	}

	result.set(0, RTLIL::State::S1);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_nex(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	ຖ້າ (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_ge(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_gt(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ຖ້າ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_add(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	ສົ່ງຄືນ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	ສົ່ງຄືນ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	ສົ່ງຄືນ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ຖ້າ (b.isZero())
		ສົ່ງຄືນ RTLIL::Const(RTLIL::State::Sx, result_len);
	bool result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ສົ່ງຄືນ big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ຖ້າ (b.isZero())
		ສົ່ງຄືນ RTLIL::Const(RTLIL::State::Sx, result_len);
	bool result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ສົ່ງຄືນ big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ຖ້າ (b.isZero())
		ສົ່ງຄືນ RTLIL::Const(RTLIL::State::Sx, result_len);

	bool result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	ຖ້າ (result_pos || a == 0) {
		result = a / b;
	} ບໍ່ດັ່ງນັ້ນ {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	ສົ່ງຄືນ big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ຖ້າ (b.isZero())
		ສົ່ງຄືນ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	ຖ້າ (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} ບໍ່ດັ່ງນັ້ນ {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	ສົ່ງຄືນ big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(const RTLIL::Const &arg1, const RTLIL::Const &arg2, bool signed1, bool signed2, ຈຳນວນເຕັມ result_len)
{
	ຈຳນວນເຕັມ undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	ຖ້າ (a == 0 && b < 0)
		ສົ່ງຄືນ RTLIL::Const(RTLIL::State::Sx, result_len);

	ຖ້າ (a == 0 && b > 0)
		ສົ່ງຄືນ RTLIL::Const(RTLIL::State::S0, result_len);

	ຖ້າ (b < 0)
	{
		ຖ້າ (a < -1 || a > 1)
			y = 0;
		ຖ້າ (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	ຖ້າ (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		ຈຳນວນເຕັມ modulus_bits = (result_len >= 0 ? result_len : 1024);
		ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < modulus_bits; i++)
			modulus *= 2;

		bool flip_result_sign = false;
		ຖ້າ (a < 0) {
			a *= -1;
			ຖ້າ (b % 2 == 1)
				flip_result_sign = true;
		}

		ໃນຂະນະທີ່ (b > 0) {
			ຖ້າ (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		ຖ້າ (flip_result_sign)
			y *= -1;
	}

	ສົ່ງຄືນ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(const RTLIL::Const &arg1, const RTLIL::Const&, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ສົ່ງຄືນ arg1_ext;
}

RTLIL::Const RTLIL::const_buf(const RTLIL::Const &arg1, const RTLIL::Const&, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ສົ່ງຄືນ arg1_ext;
}

RTLIL::Const RTLIL::const_neg(const RTLIL::Const &arg1, const RTLIL::Const&, bool signed1, bool, ຈຳນວນເຕັມ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	ສົ່ງຄືນ RTLIL::const_sub(zero, arg1_ext, true, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(const RTLIL::Const &arg1, const RTLIL::Const &arg2, const RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	ຖ້າ (arg3[0] == State::S0)
		ສົ່ງຄືນ arg1;
	ບໍ່ດັ່ງນັ້ນ ຖ້າ (arg3[0] == State::S1)
		ສົ່ງຄືນ arg2;

	RTLIL::Const ret = arg1;
	ສຳລັບ (auto i = 0; i < ret.size(); i++)
		ຖ້າ (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	ສົ່ງຄືນ ret;
}

RTLIL::Const RTLIL::const_pmux(const RTLIL::Const &arg1, const RTLIL::Const &arg2, const RTLIL::Const &arg3)
{
	ຖ້າ (arg3.is_fully_zero())
		ສົ່ງຄືນ arg1;

	ຖ້າ (!arg3.is_onehot())
		ສົ່ງຄືນ RTLIL::Const(State::Sx, arg1.size());

	ສຳລັບ (auto i = 0; i < arg3.size(); i++)
		ຖ້າ (arg3[i] == State::S1)
			ສົ່ງຄືນ RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(const RTLIL::Const &arg1, const RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	ສຳລັບ (ຈຳນວນເຕັມ i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		ຖ້າ (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		ບໍ່ດັ່ງນັ້ນ
			ສຳລັບ (ຈຳນວນເຕັມ j = 0; j < GetSize(t)/2; j++)
				new_t.push_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	ສົ່ງຄືນ t;
}

RTLIL::Const RTLIL::const_demux(const RTLIL::Const &arg1, const RTLIL::Const &arg2)
{
	ຈຳນວນເຕັມ width = GetSize(arg1);
	ຈຳນວນເຕັມ s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < (1 << s_width); i++)
	{
		bool ne = false;
		bool x = false;
		ສຳລັບ (ຈຳນວນເຕັມ j = 0; j < s_width; j++) {
			bool bit = i & 1 << j;
			ຖ້າ (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = true;
			ບໍ່ດັ່ງນັ້ນ ຖ້າ (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = true;
		}
		ຖ້າ (ne) {
			ສຳລັບ (ຈຳນວນເຕັມ j = 0; j < width; j++)
				res.push_back(State::S0);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ (x) {
			ສຳລັບ (ຈຳນວນເຕັມ j = 0; j < width; j++)
				res.push_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} ບໍ່ດັ່ງນັ້ນ {
			ສຳລັບ (ຈຳນວນເຕັມ j = 0; j < width; j++)
				res.push_back(arg1[j]);
		}
	}
	ສົ່ງຄືນ res;
}

RTLIL::Const RTLIL::const_bweqx(const RTLIL::Const &arg1, const RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	ສຳລັບ (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	ສົ່ງຄືນ result;
}

RTLIL::Const RTLIL::const_bwmux(const RTLIL::Const &arg1, const RTLIL::Const &arg2, const RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	ສຳລັບ (auto i = 0; i < arg1.size(); i++) {
		ຖ້າ (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	ສົ່ງຄືນ result;
}

YOSYS_NAMESPACE_END
