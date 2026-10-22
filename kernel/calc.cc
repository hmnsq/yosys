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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

સ્થિર શૂન્ય extend_u0(RTLIL::Const &arg, પૂર્ણાંક width, તાર્કિક is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	જો (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

સ્થિર BigInteger const2big(અચલ RTLIL::Const &val, તાર્કિક as_signed, પૂર્ણાંક &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto અંક_bits = val.size();

	જો (as_signed && અંક_bits && val[અંક_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		અંક_bits--;
	}

	માટે (auto i = 0; i < અંક_bits; i++)
		જો (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		નહીંતર જો (undef_bit_pos < 0)
			undef_bit_pos = i;

	જો (sign == BigInteger::negative)
		mag += 1;

	પાછા BigInteger(mag, sign);
}

સ્થિર RTLIL::Const big2const(અચલ BigInteger &val, પૂર્ણાંક result_len, પૂર્ણાંક undef_bit_pos)
{
	જો (undef_bit_pos >= 0)
		પાછા RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	જો (!mag.isZero())
	{
		જો (val.getSign() < 0)
		{
			mag--;
			માટે (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		નહીંતર
		{
			માટે (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#જો 0
	જો (undef_bit_pos >= 0)
		માટે (પૂર્ણાંક i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	પાછા result;
}

સ્થિર RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	જો (a == RTLIL::State::S0) પાછા RTLIL::State::S0;
	જો (b == RTLIL::State::S0) પાછા RTLIL::State::S0;
	જો (a != RTLIL::State::S1) પાછા RTLIL::State::Sx;
	જો (b != RTLIL::State::S1) પાછા RTLIL::State::Sx;
	પાછા RTLIL::State::S1;
}

સ્થિર RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	જો (a == RTLIL::State::S1) પાછા RTLIL::State::S1;
	જો (b == RTLIL::State::S1) પાછા RTLIL::State::S1;
	જો (a != RTLIL::State::S0) પાછા RTLIL::State::Sx;
	જો (b != RTLIL::State::S0) પાછા RTLIL::State::Sx;
	પાછા RTLIL::State::S0;
}

સ્થિર RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	જો (a != RTLIL::State::S0 && a != RTLIL::State::S1) પાછા RTLIL::State::Sx;
	જો (b != RTLIL::State::S0 && b != RTLIL::State::S1) પાછા RTLIL::State::Sx;
	પાછા a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

સ્થિર RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	જો (a != RTLIL::State::S0 && a != RTLIL::State::S1) પાછા RTLIL::State::Sx;
	જો (b != RTLIL::State::S0 && b != RTLIL::State::S1) પાછા RTLIL::State::Sx;
	પાછા a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	જો (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	માટે (auto i = 0; i < result_len; i++) {
		જો (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		નહીંતર જો (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		નહીંતર જો (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	પાછા result;
}

સ્થિર RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len = -1)
{
	જો (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	માટે (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	પાછા result;
}

RTLIL::Const RTLIL::const_and(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પાછા logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પાછા logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પાછા logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પાછા logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

સ્થિર RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), અચલ RTLIL::Const &arg1, પૂર્ણાંક result_len)
{
	RTLIL::State temp = initial;

	માટે (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_reduce_and(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક, તાર્કિક, પૂર્ણાંક result_len)
{
	પાછા logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક, તાર્કિક, પૂર્ણાંક result_len)
{
	પાછા logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક, તાર્કિક, પૂર્ણાંક result_len)
{
	પાછા logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક, તાર્કિક, પૂર્ણાંક result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	જો (!buffer.empty()) {
		જો (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		નહીંતર જો (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	પાછા buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક, તાર્કિક, પૂર્ણાંક result_len)
{
	પાછા logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_logic_and(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_logic_or(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
સ્થિર RTLIL::Const const_shift_worker(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક sign_ext, તાર્કિક signed2, પૂર્ણાંક direction, પૂર્ણાંક result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	જો (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	જો (undef_bit_pos >= 0)
		પાછા result;

	માટે (પૂર્ણાંક i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		જો (pos < 0)
			result.set(i, vacant_bits);
		નહીંતર જો (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		નહીંતર
			result.set(i, arg1[pos.toInt()]);
	}

	પાછા result;
}

RTLIL::Const RTLIL::const_shl(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	પાછા const_shift_worker(arg1_ext, arg2, ખોટું, ખોટું, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	પાછા const_shift_worker(arg1_ext, arg2, ખોટું, ખોટું, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	પાછા const_shift_worker(arg1, arg2, signed1, ખોટું, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	પાછા const_shift_worker(arg1, arg2, signed1, ખોટું, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	પાછા const_shift_worker(arg1_ext, arg2, ખોટું, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પાછા const_shift_worker(arg1, arg2, ખોટું, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	તાર્કિક y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_le(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	તાર્કિક y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_eq(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	પૂર્ણાંક width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	માટે (auto i = 0; i < arg1_ext.size(); i++) {
		જો (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			પાછા result;
		જો (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			પાછા result;
		જો (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	પાછા result;
}

RTLIL::Const RTLIL::const_ne(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	જો (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	નહીંતર જો (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_eqx(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	પૂર્ણાંક width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	માટે (auto i = 0; i < arg1_ext.size(); i++) {
		જો (arg1_ext.at(i) != arg2_ext.at(i))
			પાછા result;
	}

	result.set(0, RTLIL::State::S1);
	પાછા result;
}

RTLIL::Const RTLIL::const_nex(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	જો (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	નહીંતર જો (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_ge(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	તાર્કિક y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_gt(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	તાર્કિક y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	જો (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	પાછા result;
}

RTLIL::Const RTLIL::const_add(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	પાછા big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	પાછા big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	પાછા big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	જો (b.isZero())
		પાછા RTLIL::Const(RTLIL::State::Sx, result_len);
	તાર્કિક result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	પાછા big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	જો (b.isZero())
		પાછા RTLIL::Const(RTLIL::State::Sx, result_len);
	તાર્કિક result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	પાછા big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	જો (b.isZero())
		પાછા RTLIL::Const(RTLIL::State::Sx, result_len);

	તાર્કિક result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	જો (result_pos || a == 0) {
		result = a / b;
	} નહીંતર {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	પાછા big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	જો (b.isZero())
		પાછા RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	જો (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} નહીંતર {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	પાછા big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, તાર્કિક signed1, તાર્કિક signed2, પૂર્ણાંક result_len)
{
	પૂર્ણાંક undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	જો (a == 0 && b < 0)
		પાછા RTLIL::Const(RTLIL::State::Sx, result_len);

	જો (a == 0 && b > 0)
		પાછા RTLIL::Const(RTLIL::State::S0, result_len);

	જો (b < 0)
	{
		જો (a < -1 || a > 1)
			y = 0;
		જો (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	જો (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		પૂર્ણાંક modulus_bits = (result_len >= 0 ? result_len : 1024);
		માટે (પૂર્ણાંક i = 0; i < modulus_bits; i++)
			modulus *= 2;

		તાર્કિક flip_result_sign = ખોટું;
		જો (a < 0) {
			a *= -1;
			જો (b % 2 == 1)
				flip_result_sign = સાચું;
		}

		જ્યારે (b > 0) {
			જો (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		જો (flip_result_sign)
			y *= -1;
	}

	પાછા big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	પાછા arg1_ext;
}

RTLIL::Const RTLIL::const_buf(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	પાછા arg1_ext;
}

RTLIL::Const RTLIL::const_neg(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const&, તાર્કિક signed1, તાર્કિક, પૂર્ણાંક result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	પાછા RTLIL::const_sub(zero, arg1_ext, સાચું, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, અચલ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	જો (arg3[0] == State::S0)
		પાછા arg1;
	નહીંતર જો (arg3[0] == State::S1)
		પાછા arg2;

	RTLIL::Const ret = arg1;
	માટે (auto i = 0; i < ret.size(); i++)
		જો (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	પાછા ret;
}

RTLIL::Const RTLIL::const_pmux(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, અચલ RTLIL::Const &arg3)
{
	જો (arg3.is_fully_zero())
		પાછા arg1;

	જો (!arg3.is_onehot())
		પાછા RTLIL::Const(State::Sx, arg1.size());

	માટે (auto i = 0; i < arg3.size(); i++)
		જો (arg3[i] == State::S1)
			પાછા RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	માટે (પૂર્ણાંક i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		જો (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		નહીંતર જો (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		નહીંતર
			માટે (પૂર્ણાંક j = 0; j < GetSize(t)/2; j++)
				new_t.ધકેલો_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	પાછા t;
}

RTLIL::Const RTLIL::const_demux(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2)
{
	પૂર્ણાંક width = GetSize(arg1);
	પૂર્ણાંક s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	માટે (પૂર્ણાંક i = 0; i < (1 << s_width); i++)
	{
		તાર્કિક ne = ખોટું;
		તાર્કિક x = ખોટું;
		માટે (પૂર્ણાંક j = 0; j < s_width; j++) {
			તાર્કિક bit = i & 1 << j;
			જો (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = સાચું;
			નહીંતર જો (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = સાચું;
		}
		જો (ne) {
			માટે (પૂર્ણાંક j = 0; j < width; j++)
				res.ધકેલો_back(State::S0);
		} નહીંતર જો (x) {
			માટે (પૂર્ણાંક j = 0; j < width; j++)
				res.ધકેલો_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} નહીંતર {
			માટે (પૂર્ણાંક j = 0; j < width; j++)
				res.ધકેલો_back(arg1[j]);
		}
	}
	પાછા res;
}

RTLIL::Const RTLIL::const_bweqx(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	માટે (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	પાછા result;
}

RTLIL::Const RTLIL::const_bwmux(અચલ RTLIL::Const &arg1, અચલ RTLIL::Const &arg2, અચલ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	માટે (auto i = 0; i < arg1.size(); i++) {
		જો (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	પાછા result;
}

YOSYS_NAMESPACE_END
