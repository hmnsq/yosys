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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

സ്ഥിതം ശൂന്യം extend_u0(RTLIL::Const &arg, പൂർണ്ണസംഖ്യ width, തർക്കം is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	എങ്കിൽ (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

സ്ഥിതം BigInteger const2big(സ്ഥിരം RTLIL::Const &val, തർക്കം as_signed, പൂർണ്ണസംഖ്യ &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto അക്കം_bits = val.size();

	എങ്കിൽ (as_signed && അക്കം_bits && val[അക്കം_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		അക്കം_bits--;
	}

	വേണ്ടി (auto i = 0; i < അക്കം_bits; i++)
		എങ്കിൽ (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		അല്ലെങ്കിൽ എങ്കിൽ (undef_bit_pos < 0)
			undef_bit_pos = i;

	എങ്കിൽ (sign == BigInteger::negative)
		mag += 1;

	തിരികെ BigInteger(mag, sign);
}

സ്ഥിതം RTLIL::Const big2const(സ്ഥിരം BigInteger &val, പൂർണ്ണസംഖ്യ result_len, പൂർണ്ണസംഖ്യ undef_bit_pos)
{
	എങ്കിൽ (undef_bit_pos >= 0)
		തിരികെ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	എങ്കിൽ (!mag.isZero())
	{
		എങ്കിൽ (val.getSign() < 0)
		{
			mag--;
			വേണ്ടി (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		അല്ലെങ്കിൽ
		{
			വേണ്ടി (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#എങ്കിൽ 0
	എങ്കിൽ (undef_bit_pos >= 0)
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	തിരികെ result;
}

സ്ഥിതം RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	എങ്കിൽ (a == RTLIL::State::S0) തിരികെ RTLIL::State::S0;
	എങ്കിൽ (b == RTLIL::State::S0) തിരികെ RTLIL::State::S0;
	എങ്കിൽ (a != RTLIL::State::S1) തിരികെ RTLIL::State::Sx;
	എങ്കിൽ (b != RTLIL::State::S1) തിരികെ RTLIL::State::Sx;
	തിരികെ RTLIL::State::S1;
}

സ്ഥിതം RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	എങ്കിൽ (a == RTLIL::State::S1) തിരികെ RTLIL::State::S1;
	എങ്കിൽ (b == RTLIL::State::S1) തിരികെ RTLIL::State::S1;
	എങ്കിൽ (a != RTLIL::State::S0) തിരികെ RTLIL::State::Sx;
	എങ്കിൽ (b != RTLIL::State::S0) തിരികെ RTLIL::State::Sx;
	തിരികെ RTLIL::State::S0;
}

സ്ഥിതം RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	എങ്കിൽ (a != RTLIL::State::S0 && a != RTLIL::State::S1) തിരികെ RTLIL::State::Sx;
	എങ്കിൽ (b != RTLIL::State::S0 && b != RTLIL::State::S1) തിരികെ RTLIL::State::Sx;
	തിരികെ a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

സ്ഥിതം RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	എങ്കിൽ (a != RTLIL::State::S0 && a != RTLIL::State::S1) തിരികെ RTLIL::State::Sx;
	എങ്കിൽ (b != RTLIL::State::S0 && b != RTLIL::State::S1) തിരികെ RTLIL::State::Sx;
	തിരികെ a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	എങ്കിൽ (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	വേണ്ടി (auto i = 0; i < result_len; i++) {
		എങ്കിൽ (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		അല്ലെങ്കിൽ എങ്കിൽ (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		അല്ലെങ്കിൽ എങ്കിൽ (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	തിരികെ result;
}

സ്ഥിതം RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len = -1)
{
	എങ്കിൽ (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	വേണ്ടി (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	തിരികെ result;
}

RTLIL::Const RTLIL::const_and(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

സ്ഥിതം RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), സ്ഥിരം RTLIL::Const &arg1, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::State temp = initial;

	വേണ്ടി (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_reduce_and(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	എങ്കിൽ (!buffer.empty()) {
		എങ്കിൽ (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		അല്ലെങ്കിൽ എങ്കിൽ (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	തിരികെ buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_logic_and(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_logic_or(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
സ്ഥിതം RTLIL::Const const_shift_worker(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം sign_ext, തർക്കം signed2, പൂർണ്ണസംഖ്യ direction, പൂർണ്ണസംഖ്യ result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	എങ്കിൽ (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	എങ്കിൽ (undef_bit_pos >= 0)
		തിരികെ result;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		എങ്കിൽ (pos < 0)
			result.set(i, vacant_bits);
		അല്ലെങ്കിൽ എങ്കിൽ (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		അല്ലെങ്കിൽ
			result.set(i, arg1[pos.toInt()]);
	}

	തിരികെ result;
}

RTLIL::Const RTLIL::const_shl(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	തിരികെ const_shift_worker(arg1_ext, arg2, അസത്യം, അസത്യം, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	തിരികെ const_shift_worker(arg1_ext, arg2, അസത്യം, അസത്യം, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ const_shift_worker(arg1, arg2, signed1, അസത്യം, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ const_shift_worker(arg1, arg2, signed1, അസത്യം, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	തിരികെ const_shift_worker(arg1_ext, arg2, അസത്യം, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	തിരികെ const_shift_worker(arg1, arg2, അസത്യം, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	തർക്കം y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_le(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	തർക്കം y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_eq(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	പൂർണ്ണസംഖ്യ width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	വേണ്ടി (auto i = 0; i < arg1_ext.size(); i++) {
		എങ്കിൽ (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			തിരികെ result;
		എങ്കിൽ (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			തിരികെ result;
		എങ്കിൽ (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_ne(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	എങ്കിൽ (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	അല്ലെങ്കിൽ എങ്കിൽ (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_eqx(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	പൂർണ്ണസംഖ്യ width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	വേണ്ടി (auto i = 0; i < arg1_ext.size(); i++) {
		എങ്കിൽ (arg1_ext.at(i) != arg2_ext.at(i))
			തിരികെ result;
	}

	result.set(0, RTLIL::State::S1);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_nex(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	എങ്കിൽ (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	അല്ലെങ്കിൽ എങ്കിൽ (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_ge(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	തർക്കം y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_gt(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	തർക്കം y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	എങ്കിൽ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	തിരികെ result;
}

RTLIL::Const RTLIL::const_add(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	തിരികെ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	തിരികെ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	തിരികെ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	എങ്കിൽ (b.isZero())
		തിരികെ RTLIL::Const(RTLIL::State::Sx, result_len);
	തർക്കം result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	തിരികെ big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	എങ്കിൽ (b.isZero())
		തിരികെ RTLIL::Const(RTLIL::State::Sx, result_len);
	തർക്കം result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	തിരികെ big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	എങ്കിൽ (b.isZero())
		തിരികെ RTLIL::Const(RTLIL::State::Sx, result_len);

	തർക്കം result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	എങ്കിൽ (result_pos || a == 0) {
		result = a / b;
	} അല്ലെങ്കിൽ {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	തിരികെ big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	എങ്കിൽ (b.isZero())
		തിരികെ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	എങ്കിൽ (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} അല്ലെങ്കിൽ {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	തിരികെ big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, തർക്കം signed1, തർക്കം signed2, പൂർണ്ണസംഖ്യ result_len)
{
	പൂർണ്ണസംഖ്യ undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	എങ്കിൽ (a == 0 && b < 0)
		തിരികെ RTLIL::Const(RTLIL::State::Sx, result_len);

	എങ്കിൽ (a == 0 && b > 0)
		തിരികെ RTLIL::Const(RTLIL::State::S0, result_len);

	എങ്കിൽ (b < 0)
	{
		എങ്കിൽ (a < -1 || a > 1)
			y = 0;
		എങ്കിൽ (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	എങ്കിൽ (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		പൂർണ്ണസംഖ്യ modulus_bits = (result_len >= 0 ? result_len : 1024);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < modulus_bits; i++)
			modulus *= 2;

		തർക്കം flip_result_sign = അസത്യം;
		എങ്കിൽ (a < 0) {
			a *= -1;
			എങ്കിൽ (b % 2 == 1)
				flip_result_sign = സത്യം;
		}

		ആകുമ്പോൾ (b > 0) {
			എങ്കിൽ (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		എങ്കിൽ (flip_result_sign)
			y *= -1;
	}

	തിരികെ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	തിരികെ arg1_ext;
}

RTLIL::Const RTLIL::const_buf(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	തിരികെ arg1_ext;
}

RTLIL::Const RTLIL::const_neg(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const&, തർക്കം signed1, തർക്കം, പൂർണ്ണസംഖ്യ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	തിരികെ RTLIL::const_sub(zero, arg1_ext, സത്യം, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, സ്ഥിരം RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	എങ്കിൽ (arg3[0] == State::S0)
		തിരികെ arg1;
	അല്ലെങ്കിൽ എങ്കിൽ (arg3[0] == State::S1)
		തിരികെ arg2;

	RTLIL::Const ret = arg1;
	വേണ്ടി (auto i = 0; i < ret.size(); i++)
		എങ്കിൽ (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	തിരികെ ret;
}

RTLIL::Const RTLIL::const_pmux(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, സ്ഥിരം RTLIL::Const &arg3)
{
	എങ്കിൽ (arg3.is_fully_zero())
		തിരികെ arg1;

	എങ്കിൽ (!arg3.is_onehot())
		തിരികെ RTLIL::Const(State::Sx, arg1.size());

	വേണ്ടി (auto i = 0; i < arg3.size(); i++)
		എങ്കിൽ (arg3[i] == State::S1)
			തിരികെ RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		എങ്കിൽ (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		അല്ലെങ്കിൽ എങ്കിൽ (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		അല്ലെങ്കിൽ
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < GetSize(t)/2; j++)
				new_t.തള്ളുക_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	തിരികെ t;
}

RTLIL::Const RTLIL::const_demux(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2)
{
	പൂർണ്ണസംഖ്യ width = GetSize(arg1);
	പൂർണ്ണസംഖ്യ s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < (1 << s_width); i++)
	{
		തർക്കം ne = അസത്യം;
		തർക്കം x = അസത്യം;
		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < s_width; j++) {
			തർക്കം bit = i & 1 << j;
			എങ്കിൽ (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = സത്യം;
			അല്ലെങ്കിൽ എങ്കിൽ (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = സത്യം;
		}
		എങ്കിൽ (ne) {
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < width; j++)
				res.തള്ളുക_back(State::S0);
		} അല്ലെങ്കിൽ എങ്കിൽ (x) {
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < width; j++)
				res.തള്ളുക_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} അല്ലെങ്കിൽ {
			വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < width; j++)
				res.തള്ളുക_back(arg1[j]);
		}
	}
	തിരികെ res;
}

RTLIL::Const RTLIL::const_bweqx(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	വേണ്ടി (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	തിരികെ result;
}

RTLIL::Const RTLIL::const_bwmux(സ്ഥിരം RTLIL::Const &arg1, സ്ഥിരം RTLIL::Const &arg2, സ്ഥിരം RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	വേണ്ടി (auto i = 0; i < arg1.size(); i++) {
		എങ്കിൽ (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	തിരികെ result;
}

YOSYS_NAMESPACE_END
