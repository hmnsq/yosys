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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

ସ୍ଥିର ଶୂନ୍ୟ extend_u0(RTLIL::Const &arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ତାର୍କିକ is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	ଯଦି (arg.size() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

ସ୍ଥିର BigInteger const2big(ଅଚଳ RTLIL::Const &val, ତାର୍କିକ as_signed, ପୂର୍ଣ୍ଣସଂଖ୍ୟା &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	auto ଅଙ୍କ_bits = val.size();

	ଯଦି (as_signed && ଅଙ୍କ_bits && val[ଅଙ୍କ_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		ଅଙ୍କ_bits--;
	}

	ପାଇଁ (auto i = 0; i < ଅଙ୍କ_bits; i++)
		ଯଦି (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		ଅନ୍ୟଥା ଯଦି (undef_bit_pos < 0)
			undef_bit_pos = i;

	ଯଦି (sign == BigInteger::negative)
		mag += 1;

	ଫେରନ୍ତୁ BigInteger(mag, sign);
}

ସ୍ଥିର RTLIL::Const big2const(ଅଚଳ BigInteger &val, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos)
{
	ଯଦି (undef_bit_pos >= 0)
		ଫେରନ୍ତୁ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const result(0, result_len);

	ଯଦି (!mag.isZero())
	{
		ଯଦି (val.getSign() < 0)
		{
			mag--;
			ପାଇଁ (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		ଅନ୍ୟଥା
		{
			ପାଇଁ (auto i = 0; i < result_len; i++)
				result.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#ଯଦି 0
	ଯଦି (undef_bit_pos >= 0)
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = undef_bit_pos; i < result_len; i++)
			result[i] = RTLIL::State::Sx;
#endif

	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	ଯଦି (a == RTLIL::State::S0) ଫେରନ୍ତୁ RTLIL::State::S0;
	ଯଦି (b == RTLIL::State::S0) ଫେରନ୍ତୁ RTLIL::State::S0;
	ଯଦି (a != RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଯଦି (b != RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଫେରନ୍ତୁ RTLIL::State::S1;
}

ସ୍ଥିର RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	ଯଦି (a == RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::S1;
	ଯଦି (b == RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::S1;
	ଯଦି (a != RTLIL::State::S0) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଯଦି (b != RTLIL::State::S0) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଫେରନ୍ତୁ RTLIL::State::S0;
}

ସ୍ଥିର RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	ଯଦି (a != RTLIL::State::S0 && a != RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଯଦି (b != RTLIL::State::S0 && b != RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଫେରନ୍ତୁ a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

ସ୍ଥିର RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	ଯଦି (a != RTLIL::State::S0 && a != RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଯଦି (b != RTLIL::State::S0 && b != RTLIL::State::S1) ଫେରନ୍ତୁ RTLIL::State::Sx;
	ଫେରନ୍ତୁ a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଯଦି (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ପାଇଁ (auto i = 0; i < result_len; i++) {
		ଯଦି (i >= GetSize(arg1_ext))
			result.set(i, RTLIL::State::S0);
		ଅନ୍ୟଥା ଯଦି (arg1_ext[i] == RTLIL::State::S0)
			result.set(i, RTLIL::State::S1);
		ଅନ୍ୟଥା ଯଦି (arg1_ext[i] == RTLIL::State::S1)
			result.set(i, RTLIL::State::S0);
	}

	ଫେରନ୍ତୁ result;
}

ସ୍ଥିର RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len = -1)
{
	ଯଦି (result_len < 0)
		result_len = max(GetSize(arg1), GetSize(arg2));

	extend_u0(arg1, result_len, signed1);
	extend_u0(arg2, result_len, signed2);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ପାଇଁ (auto i = 0; i < result_len; i++) {
		RTLIL::State a = i < GetSize(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < GetSize(arg2) ? arg2[i] : RTLIL::State::S0;
		result.set(i, logic_func(a, b));
	}

	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_and(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_wrapper(logic_and, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_or(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_wrapper(logic_or, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xor(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, result_len);
}

RTLIL::Const RTLIL::const_xnor(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, result_len);
}

ସ୍ଥିର RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ଅଚଳ RTLIL::Const &arg1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::State temp = initial;

	ପାଇଁ (auto i = 0; i < arg1.size(); i++)
		temp = logic_func(temp, arg1[i]);

	RTLIL::Const result(temp);
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_reduce_and(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_or(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const buffer = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, result_len);
	ଯଦି (!buffer.empty()) {
		ଯଦି (buffer.front() == RTLIL::State::S0)
			buffer.set(0, RTLIL::State::S1);
		ଅନ୍ୟଥା ଯଦି (buffer.front() == RTLIL::State::S1)
			buffer.set(0, RTLIL::State::S0);
	}
	ଫେରନ୍ତୁ buffer;
}

RTLIL::Const RTLIL::const_reduce_bool(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, result_len);
}

RTLIL::Const RTLIL::const_logic_not(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const result(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_logic_and(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_and(bit_a, bit_b));
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_logic_or(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const result(logic_or(bit_a, bit_b));
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ସ୍ଥିର RTLIL::Const const_shift_worker(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ sign_ext, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	ଯଦି (result_len < 0)
		result_len = GetSize(arg1);

	RTLIL::Const result(RTLIL::State::Sx, result_len);
	ଯଦି (undef_bit_pos >= 0)
		ଫେରନ୍ତୁ result;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < result_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		ଯଦି (pos < 0)
			result.set(i, vacant_bits);
		ଅନ୍ୟଥା ଯଦି (pos >= BigInteger(GetSize(arg1)))
			result.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		ଅନ୍ୟଥା
			result.set(i, arg1[pos.toInt()]);
	}

	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_shl(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);
	ଫେରନ୍ତୁ const_shift_worker(arg1_ext, arg2, ମିଥ୍ୟା, ମିଥ୍ୟା, -1, result_len);
}

RTLIL::Const RTLIL::const_shr(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ଫେରନ୍ତୁ const_shift_worker(arg1_ext, arg2, ମିଥ୍ୟା, ମିଥ୍ୟା, +1, result_len);
}

RTLIL::Const RTLIL::const_sshl(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ const_shift_worker(arg1, arg2, signed1, ମିଥ୍ୟା, -1, result_len);
}

RTLIL::Const RTLIL::const_sshr(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ const_shift_worker(arg1, arg2, signed1, ମିଥ୍ୟା, +1, result_len);
}

RTLIL::Const RTLIL::const_shift(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(result_len, GetSize(arg1)), signed1);
	ଫେରନ୍ତୁ const_shift_worker(arg1_ext, arg2, ମିଥ୍ୟା, signed2, +1, result_len);
}

RTLIL::Const RTLIL::const_shiftx(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ଫେରନ୍ତୁ const_shift_worker(arg1, arg2, ମିଥ୍ୟା, signed2, +1, result_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	ତାର୍କିକ y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_le(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	ତାର୍କିକ y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_eq(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	ପାଇଁ (auto i = 0; i < arg1_ext.size(); i++) {
		ଯଦି (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			ଫେରନ୍ତୁ result;
		ଯଦି (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			ଫେରନ୍ତୁ result;
		ଯଦି (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	result.set(0, matched_status);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_ne(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const result = RTLIL::const_eq(arg1, arg2, signed1, signed2, result_len);
	ଯଦି (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	ଅନ୍ୟଥା ଯଦି (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_eqx(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const result(RTLIL::State::S0, result_len);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = max(GetSize(arg1_ext), GetSize(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	ପାଇଁ (auto i = 0; i < arg1_ext.size(); i++) {
		ଯଦି (arg1_ext.at(i) != arg2_ext.at(i))
			ଫେରନ୍ତୁ result;
	}

	result.set(0, RTLIL::State::S1);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_nex(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const result = RTLIL::const_eqx(arg1, arg2, signed1, signed2, result_len);
	ଯଦି (result.front() == RTLIL::State::S0)
		result.set(0, RTLIL::State::S1);
	ଅନ୍ୟଥା ଯଦି (result.front() == RTLIL::State::S1)
		result.set(0, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_ge(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	ତାର୍କିକ y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_gt(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	ତାର୍କିକ y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const result(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ଯଦି (GetSize(result) < result_len)
		result.resize(result_len, RTLIL::State::S0);
	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_add(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	ଫେରନ୍ତୁ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	ଫେରନ୍ତୁ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	ଫେରନ୍ତୁ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ଯଦି (b.isZero())
		ଫେରନ୍ତୁ RTLIL::Const(RTLIL::State::Sx, result_len);
	ତାର୍କିକ result_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ଫେରନ୍ତୁ big2const(result_neg ? -(a / b) : (a / b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ଯଦି (b.isZero())
		ଫେରନ୍ତୁ RTLIL::Const(RTLIL::State::Sx, result_len);
	ତାର୍କିକ result_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ଫେରନ୍ତୁ big2const(result_neg ? -(a % b) : (a % b), result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ଯଦି (b.isZero())
		ଫେରନ୍ତୁ RTLIL::Const(RTLIL::State::Sx, result_len);

	ତାର୍କିକ result_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger result;

	ଯଦି (result_pos || a == 0) {
		result = a / b;
	} ଅନ୍ୟଥା {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		result = -((a + b - 1) / b);
	}
	ଫେରନ୍ତୁ big2const(result, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	ଯଦି (b.isZero())
		ଫେରନ୍ତୁ RTLIL::Const(RTLIL::State::Sx, result_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	ଯଦି (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} ଅନ୍ୟଥା {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	ଫେରନ୍ତୁ big2const(modulo, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ତାର୍କିକ signed1, ତାର୍କିକ signed2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	ଯଦି (a == 0 && b < 0)
		ଫେରନ୍ତୁ RTLIL::Const(RTLIL::State::Sx, result_len);

	ଯଦି (a == 0 && b > 0)
		ଫେରନ୍ତୁ RTLIL::Const(RTLIL::State::S0, result_len);

	ଯଦି (b < 0)
	{
		ଯଦି (a < -1 || a > 1)
			y = 0;
		ଯଦି (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	ଯଦି (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା modulus_bits = (result_len >= 0 ? result_len : 1024);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < modulus_bits; i++)
			modulus *= 2;

		ତାର୍କିକ flip_result_sign = ମିଥ୍ୟା;
		ଯଦି (a < 0) {
			a *= -1;
			ଯଦି (b % 2 == 1)
				flip_result_sign = ସତ୍ୟ;
		}

		ଯେତେବେଳେ (b > 0) {
			ଯଦି (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		ଯଦି (flip_result_sign)
			y *= -1;
	}

	ଫେରନ୍ତୁ big2const(y, result_len >= 0 ? result_len : max(GetSize(arg1), GetSize(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ଫେରନ୍ତୁ arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, result_len, signed1);

	ଫେରନ୍ତୁ arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const&, ତାର୍କିକ signed1, ତାର୍କିକ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା result_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	ଫେରନ୍ତୁ RTLIL::const_sub(zero, arg1_ext, ସତ୍ୟ, signed1, result_len);
}

RTLIL::Const RTLIL::const_mux(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ଅଚଳ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	ଯଦି (arg3[0] == State::S0)
		ଫେରନ୍ତୁ arg1;
	ଅନ୍ୟଥା ଯଦି (arg3[0] == State::S1)
		ଫେରନ୍ତୁ arg2;

	RTLIL::Const ret = arg1;
	ପାଇଁ (auto i = 0; i < ret.size(); i++)
		ଯଦି (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	ଫେରନ୍ତୁ ret;
}

RTLIL::Const RTLIL::const_pmux(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ଅଚଳ RTLIL::Const &arg3)
{
	ଯଦି (arg3.is_fully_zero())
		ଫେରନ୍ତୁ arg1;

	ଯଦି (!arg3.is_onehot())
		ଫେରନ୍ତୁ RTLIL::Const(State::Sx, arg1.size());

	ପାଇଁ (auto i = 0; i < arg3.size(); i++)
		ଯଦି (arg3[i] == State::S1)
			ଫେରନ୍ତୁ RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.size(), arg2.begin() + (i+1)*arg1.size()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = GetSize(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		ଯଦି (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + GetSize(t)/2);
		ଅନ୍ୟଥା ଯଦି (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + GetSize(t)/2, t.end());
		ଅନ୍ୟଥା
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < GetSize(t)/2; j++)
				new_t.ଠେଲ_back(t[j] == t[j + GetSize(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	ଫେରନ୍ତୁ t;
}

RTLIL::Const RTLIL::const_demux(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(arg1);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_width = GetSize(arg2);
	std::vector<RTLIL::State> res;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (1 << s_width); i++)
	{
		ତାର୍କିକ ne = ମିଥ୍ୟା;
		ତାର୍କିକ x = ମିଥ୍ୟା;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < s_width; j++) {
			ତାର୍କିକ bit = i & 1 << j;
			ଯଦି (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = ସତ୍ୟ;
			ଅନ୍ୟଥା ଯଦି (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = ସତ୍ୟ;
		}
		ଯଦି (ne) {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < width; j++)
				res.ଠେଲ_back(State::S0);
		} ଅନ୍ୟଥା ଯଦି (x) {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < width; j++)
				res.ଠେଲ_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} ଅନ୍ୟଥା {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < width; j++)
				res.ଠେଲ_back(arg1[j]);
		}
	}
	ଫେରନ୍ତୁ res;
}

RTLIL::Const RTLIL::const_bweqx(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2)
{
	log_assert(arg2.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::S0, arg1.size());
	ପାଇଁ (auto i = 0; i < arg1.size(); i++)
		result.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	ଫେରନ୍ତୁ result;
}

RTLIL::Const RTLIL::const_bwmux(ଅଚଳ RTLIL::Const &arg1, ଅଚଳ RTLIL::Const &arg2, ଅଚଳ RTLIL::Const &arg3)
{
	log_assert(arg2.size() == arg1.size());
	log_assert(arg3.size() == arg1.size());
	RTLIL::Const result(RTLIL::State::Sx, arg1.size());
	ପାଇଁ (auto i = 0; i < arg1.size(); i++) {
		ଯଦି (arg3[i] != State::Sx || arg1[i] == arg2[i])
			result.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	ଫେରନ୍ତୁ result;
}

YOSYS_NAMESPACE_END
