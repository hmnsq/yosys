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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

ಸ್ಥಿರ ಶೂನ್ಯ extend_u0(RTLIL::Const &arg, ಪೂರ್ಣಾಙ್ಕ width, ತಾರ್ಕಿಕ is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	ಯದಿ (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

ಸ್ಥಿರ BigInteger const2big(ಧ್ರುವ RTLIL::Const &val, ತಾರ್ಕಿಕ as_signed, ಪೂರ್ಣಾಙ್ಕ &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto ಅಂಕೆ_bits = val.size();

	ಯದಿ (as_signed && ಅಂಕೆ_bits && val[ಅಂಕೆ_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		ಅಂಕೆ_bits--;
	}

	ಪ್ರತಿ (auto i = 0; i < ಅಂಕೆ_bits; i++)
		ಯದಿ (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		ಅನ್ಯಥಾ ಯದಿ (undef_bit_pos < 0)
			undef_bit_pos = i;

	ಯದಿ (sign == BigInteger::negative)
		mag += 1;

	ಪ್ರತ್ಯಾಗಮ BigInteger(mag, sign);
}

ಸ್ಥಿರ RTLIL::Const big2const(ಧ್ರುವ BigInteger &val, ಪೂರ್ಣಾಙ್ಕ result_len, ಪೂರ್ಣಾಙ್ಕ undef_bit_pos)
{
	ಯದಿ (undef_bit_pos >= 0)
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	ಯದಿ (!mag.isZero())
	{
		ಯದಿ (val.getSign() < 0)
		{
			mag--;
			ಪ್ರತಿ (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		ಅನ್ಯಥಾ
		{
			ಪ್ರತಿ (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#ಯದಿ 0
	ಯದಿ (undef_bit_pos >= 0)
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	ಯದಿ (a == RTLIL::State::S0) ಪ್ರತ್ಯಾಗಮ RTLIL::State::S0;
	ಯದಿ (b == RTLIL::State::S0) ಪ್ರತ್ಯಾಗಮ RTLIL::State::S0;
	ಯದಿ (a != RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಯದಿ (b != RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಪ್ರತ್ಯಾಗಮ RTLIL::State::S1;
}

ಸ್ಥಿರ RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	ಯದಿ (a == RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::S1;
	ಯದಿ (b == RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::S1;
	ಯದಿ (a != RTLIL::State::S0) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಯದಿ (b != RTLIL::State::S0) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಪ್ರತ್ಯಾಗಮ RTLIL::State::S0;
}

ಸ್ಥಿರ RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	ಯದಿ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಯದಿ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಪ್ರತ್ಯಾಗಮ a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

ಸ್ಥಿರ RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	ಯದಿ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಯದಿ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ಪ್ರತ್ಯಾಗಮ RTLIL::State::Sx;
	ಪ್ರತ್ಯಾಗಮ a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಯದಿ (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ಪ್ರತಿ (auto i = 0; i < result_len; i++) {
		ಯದಿ (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		ಅನ್ಯಥಾ ಯದಿ (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		ಅನ್ಯಥಾ ಯದಿ (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len = -1)
{
	ಯದಿ (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ಪ್ರತಿ (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_and(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

ಸ್ಥಿರ RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ಧ್ರುವ RTLIL::Const &arg1, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::State temp = initial;

	ಪ್ರತಿ (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_reduce_and(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	ಯದಿ (!buffer.empty()) {
		ಯದಿ (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		ಅನ್ಯಥಾ ಯದಿ (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	ಪ್ರತ್ಯಾಗಮ buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_logic_and(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_logic_or(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ಸ್ಥಿರ RTLIL::Const const_shift_worker(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ sign_ext, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ direction, ಪೂರ್ಣಾಙ್ಕ result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	ಯದಿ (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ಯದಿ (undef_bit_pos >= 0)
		ಪ್ರತ್ಯಾಗಮ result;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		ಯದಿ (pos < 0)
			result.set(i, vacant_bits);
		ಅನ್ಯಥಾ ಯದಿ (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		ಅನ್ಯಥಾ
			result.set(i, arg1[pos.toInt()]);
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_shl(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	ಪ್ರತ್ಯಾಗಮ const_shift_worker(arg1_ext, arg2, ಅಸತ್ಯ, ಅಸತ್ಯ, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ಪ್ರತ್ಯಾಗಮ const_shift_worker(arg1_ext, arg2, ಅಸತ್ಯ, ಅಸತ್ಯ, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ const_shift_worker(arg1, arg2, signed1, ಅಸತ್ಯ, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ const_shift_worker(arg1, arg2, signed1, ಅಸತ್ಯ, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ಪ್ರತ್ಯಾಗಮ const_shift_worker(arg1_ext, arg2, ಅಸತ್ಯ, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪ್ರತ್ಯಾಗಮ const_shift_worker(arg1, arg2, ಅಸತ್ಯ, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_le(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_eq(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	ಪೂರ್ಣಾಙ್ಕ width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	ಪ್ರತಿ (auto i = 0; i < arg1_ext.size(); i++) {
		ಯದಿ (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			ಪ್ರತ್ಯಾಗಮ result;
		ಯದಿ (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			ಪ್ರತ್ಯಾಗಮ result;
		ಯದಿ (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_ne(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	ಯದಿ (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	ಅನ್ಯಥಾ ಯದಿ (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_eqx(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	ಪೂರ್ಣಾಙ್ಕ width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	ಪ್ರತಿ (auto i = 0; i < arg1_ext.size(); i++) {
		ಯದಿ (arg1_ext.at(i) != arg2_ext.at(i))
			ಪ್ರತ್ಯಾಗಮ result;
	}

	result.set(0, RTLIL::State::S1);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_nex(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	ಯದಿ (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	ಅನ್ಯಥಾ ಯದಿ (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_ge(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_gt(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಯದಿ (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_add(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	ಪ್ರತ್ಯಾಗಮ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	ಪ್ರತ್ಯಾಗಮ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	ಪ್ರತ್ಯಾಗಮ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ಯದಿ (b.isZero())
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(RTLIL::State::Sx, result_len);
	ತಾರ್ಕಿಕ result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ಪ್ರತ್ಯಾಗಮ big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ಯದಿ (b.isZero())
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(RTLIL::State::Sx, result_len);
	ತಾರ್ಕಿಕ result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ಪ್ರತ್ಯಾಗಮ big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ಯದಿ (b.isZero())
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(RTLIL::State::Sx, result_len);

	ತಾರ್ಕಿಕ result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	ಯದಿ (result_pos || a == 0) {
		result = a / b;
	} ಅನ್ಯಥಾ {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	ಪ್ರತ್ಯಾಗಮ big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ಯದಿ (b.isZero())
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	ಯದಿ (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} ಅನ್ಯಥಾ {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	ಪ್ರತ್ಯಾಗಮ big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	ಪೂರ್ಣಾಙ್ಕ undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	ಯದಿ (a == 0 && b < 0)
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(RTLIL::State::Sx, result_len);

	ಯದಿ (a == 0 && b > 0)
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(RTLIL::State::S0, result_len);

	ಯದಿ (b < 0)
	{
		ಯದಿ (a < -1 || a > 1)
			y = 0;
		ಯದಿ (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	ಯದಿ (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		ಪೂರ್ಣಾಙ್ಕ modulus_bits = (result_len >= 0 ? result_len : 1024);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < modulus_bits; i++)
			modulus *= 2;

		ತಾರ್ಕಿಕ flip_result_sign = ಅಸತ್ಯ;
		ಯದಿ (a < 0) {
			a *= -1;
			ಯದಿ (b % 2 == 1)
				flip_result_sign = ಸತ್ಯ;
		}

		ಯಾವತ್ (b > 0) {
			ಯದಿ (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		ಯದಿ (flip_result_sign)
			y *= -1;
	}

	ಪ್ರತ್ಯಾಗಮ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ಪ್ರತ್ಯಾಗಮ arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ಪ್ರತ್ಯಾಗಮ arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಙ್ಕ result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	ಪ್ರತ್ಯಾಗಮ RTLIL::const_sub(zero, arg1_ext, ಸತ್ಯ, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	ಯದಿ (arg3[0] == State::S0)
		ಪ್ರತ್ಯಾಗಮ arg1;
	ಅನ್ಯಥಾ ಯದಿ (arg3[0] == State::S1)
		ಪ್ರತ್ಯಾಗಮ arg2;

	RTLIL::Const ret = arg1;
	ಪ್ರತಿ (auto i = 0; i < ret.size(); i++)
		ಯದಿ (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	ಪ್ರತ್ಯಾಗಮ ret;
}

RTLIL::Const RTLIL::const_pmux(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3)
{
	ಯದಿ (arg3.is_fully_zero())
		ಪ್ರತ್ಯಾಗಮ arg1;

	ಯದಿ (!arg3.is_onehot())
		ಪ್ರತ್ಯಾಗಮ RTLIL::Const(State::Sx, arg1.size());

	ಪ್ರತಿ (auto i = 0; i < arg3.size(); i++)
		ಯದಿ (arg3[i] == State::S1)
			ಪ್ರತ್ಯಾಗಮ RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		ಯದಿ (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		ಅನ್ಯಥಾ ಯದಿ (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		ಅನ್ಯಥಾ
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < GetSize(t)/2; j++)
				new_t.ತಳ್ಳು_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	ಪ್ರತ್ಯಾಗಮ t;
}

RTLIL::Const RTLIL::const_demux(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2)
{
	ಪೂರ್ಣಾಙ್ಕ width = GetSize(arg1);
	ಪೂರ್ಣಾಙ್ಕ s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (1 << s_width); i++)
	{
		ತಾರ್ಕಿಕ ne = ಅಸತ್ಯ;
		ತಾರ್ಕಿಕ x = ಅಸತ್ಯ;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < s_width; j++) {
			ತಾರ್ಕಿಕ bit = i & 1 << j;
			ಯದಿ (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = ಸತ್ಯ;
			ಅನ್ಯಥಾ ಯದಿ (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = ಸತ್ಯ;
		}
		ಯದಿ (ne) {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < width; j++)
				res.ತಳ್ಳು_back(State::S0);
		} ಅನ್ಯಥಾ ಯದಿ (x) {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < width; j++)
				res.ತಳ್ಳು_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} ಅನ್ಯಥಾ {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < width; j++)
				res.ತಳ್ಳು_back(arg1[j]);
		}
	}
	ಪ್ರತ್ಯಾಗಮ res;
}

RTLIL::Const RTLIL::const_bweqx(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	ಪ್ರತಿ (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	ಪ್ರತ್ಯಾಗಮ result;
}

RTLIL::Const RTLIL::const_bwmux(ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	ಪ್ರತಿ (auto i = 0; i < arg1.size(); i++) {
		ಯದಿ (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	ಪ್ರತ್ಯಾಗಮ result;
}

YOSYS_NAMESPACE_END
