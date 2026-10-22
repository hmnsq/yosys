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

#समावेश "kernel/yosys.h"
#समावेश "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

अटल व्योम extend_u0(RTLIL::Const &arg, पूर्णांक width, bool is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	अगर (arg.आकार() > 0 && is_signed)
		padding = arg.back();

	arg.resize(width, padding);
}

अटल BigInteger const2big(स्थिर RTLIL::Const &val, bool as_signed, पूर्णांक &undef_bit_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::positive;
	State inv_sign_bit = RTLIL::State::S1;
	स्वतः अंक_bits = val.आकार();

	अगर (as_signed && अंक_bits && val[अंक_bits-1] == RTLIL::State::S1) {
		inv_sign_bit = RTLIL::State::S0;
		sign = BigInteger::negative;
		अंक_bits--;
	}

	क्रम (स्वतः i = 0; i < अंक_bits; i++)
		अगर (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.setBit(i, val[i] == inv_sign_bit);
		अन्यथा अगर (undef_bit_pos < 0)
			undef_bit_pos = i;

	अगर (sign == BigInteger::negative)
		mag += 1;

	वापस BigInteger(mag, sign);
}

अटल RTLIL::Const big2const(स्थिर BigInteger &val, पूर्णांक परिणाम_len, पूर्णांक undef_bit_pos)
{
	अगर (undef_bit_pos >= 0)
		वापस RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const परिणाम(0, परिणाम_len);

	अगर (!mag.isZero())
	{
		अगर (val.getSign() < 0)
		{
			mag--;
			क्रम (स्वतः i = 0; i < परिणाम_len; i++)
				परिणाम.set(i, mag.getBit(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		अन्यथा
		{
			क्रम (स्वतः i = 0; i < परिणाम_len; i++)
				परिणाम.set(i, mag.getBit(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#अगर 0
	अगर (undef_bit_pos >= 0)
		क्रम (पूर्णांक i = undef_bit_pos; i < परिणाम_len; i++)
			परिणाम[i] = RTLIL::State::Sx;
#पूर्ण_अगर

	वापस परिणाम;
}

अटल RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	अगर (a == RTLIL::State::S0) वापस RTLIL::State::S0;
	अगर (b == RTLIL::State::S0) वापस RTLIL::State::S0;
	अगर (a != RTLIL::State::S1) वापस RTLIL::State::Sx;
	अगर (b != RTLIL::State::S1) वापस RTLIL::State::Sx;
	वापस RTLIL::State::S1;
}

अटल RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	अगर (a == RTLIL::State::S1) वापस RTLIL::State::S1;
	अगर (b == RTLIL::State::S1) वापस RTLIL::State::S1;
	अगर (a != RTLIL::State::S0) वापस RTLIL::State::Sx;
	अगर (b != RTLIL::State::S0) वापस RTLIL::State::Sx;
	वापस RTLIL::State::S0;
}

अटल RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	अगर (a != RTLIL::State::S0 && a != RTLIL::State::S1) वापस RTLIL::State::Sx;
	अगर (b != RTLIL::State::S0 && b != RTLIL::State::S1) वापस RTLIL::State::Sx;
	वापस a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

अटल RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	अगर (a != RTLIL::State::S0 && a != RTLIL::State::S1) वापस RTLIL::State::Sx;
	अगर (b != RTLIL::State::S0 && b != RTLIL::State::S1) वापस RTLIL::State::Sx;
	वापस a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool signed1, bool, पूर्णांक परिणाम_len)
{
	अगर (परिणाम_len < 0)
		परिणाम_len = Get_आकार(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);

	RTLIL::Const परिणाम(RTLIL::State::Sx, परिणाम_len);
	क्रम (स्वतः i = 0; i < परिणाम_len; i++) {
		अगर (i >= Get_आकार(arg1_ext))
			परिणाम.set(i, RTLIL::State::S0);
		अन्यथा अगर (arg1_ext[i] == RTLIL::State::S0)
			परिणाम.set(i, RTLIL::State::S1);
		अन्यथा अगर (arg1_ext[i] == RTLIL::State::S1)
			परिणाम.set(i, RTLIL::State::S0);
	}

	वापस परिणाम;
}

अटल RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len = -1)
{
	अगर (परिणाम_len < 0)
		परिणाम_len = max(Get_आकार(arg1), Get_आकार(arg2));

	extend_u0(arg1, परिणाम_len, signed1);
	extend_u0(arg2, परिणाम_len, signed2);

	RTLIL::Const परिणाम(RTLIL::State::Sx, परिणाम_len);
	क्रम (स्वतः i = 0; i < परिणाम_len; i++) {
		RTLIL::State a = i < Get_आकार(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_आकार(arg2) ? arg2[i] : RTLIL::State::S0;
		परिणाम.set(i, logic_func(a, b));
	}

	वापस परिणाम;
}

RTLIL::Const RTLIL::const_and(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	वापस logic_wrapper(logic_and, arg1, arg2, signed1, signed2, परिणाम_len);
}

RTLIL::Const RTLIL::const_or(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	वापस logic_wrapper(logic_or, arg1, arg2, signed1, signed2, परिणाम_len);
}

RTLIL::Const RTLIL::const_xor(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	वापस logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, परिणाम_len);
}

RTLIL::Const RTLIL::const_xnor(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	वापस logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, परिणाम_len);
}

अटल RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), स्थिर RTLIL::Const &arg1, पूर्णांक परिणाम_len)
{
	RTLIL::State अस्थायी = initial;

	क्रम (स्वतः i = 0; i < arg1.आकार(); i++)
		अस्थायी = logic_func(अस्थायी, arg1[i]);

	RTLIL::Const परिणाम(अस्थायी);
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_reduce_and(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool, bool, पूर्णांक परिणाम_len)
{
	वापस logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_reduce_or(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool, bool, पूर्णांक परिणाम_len)
{
	वापस logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_reduce_xor(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool, bool, पूर्णांक परिणाम_len)
{
	वापस logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool, bool, पूर्णांक परिणाम_len)
{
	RTLIL::Const बफर = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, परिणाम_len);
	अगर (!बफर.empty()) {
		अगर (बफर.front() == RTLIL::State::S0)
			बफर.set(0, RTLIL::State::S1);
		अन्यथा अगर (बफर.front() == RTLIL::State::S1)
			बफर.set(0, RTLIL::State::S0);
	}
	वापस बफर;
}

RTLIL::Const RTLIL::const_reduce_bool(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool, bool, पूर्णांक परिणाम_len)
{
	वापस logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, परिणाम_len);
}

RTLIL::Const RTLIL::const_logic_not(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool signed1, bool, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	RTLIL::Const परिणाम(a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_logic_and(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const परिणाम(logic_and(bit_a, bit_b));
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_logic_or(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos_a = -1, undef_bit_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos_b);

	RTLIL::State bit_a = a.isZero() ? undef_bit_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State bit_b = b.isZero() ? undef_bit_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const परिणाम(logic_or(bit_a, bit_b));
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
अटल RTLIL::Const const_shift_worker(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool sign_ext, bool signed2, पूर्णांक direction, पूर्णांक परिणाम_len, RTLIL::State vacant_bits = RTLIL::State::S0)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger offset = const2big(arg2, signed2, undef_bit_pos) * direction;

	अगर (परिणाम_len < 0)
		परिणाम_len = Get_आकार(arg1);

	RTLIL::Const परिणाम(RTLIL::State::Sx, परिणाम_len);
	अगर (undef_bit_pos >= 0)
		वापस परिणाम;

	क्रम (पूर्णांक i = 0; i < परिणाम_len; i++) {
		BigInteger pos = BigInteger(i) + offset;
		अगर (pos < 0)
			परिणाम.set(i, vacant_bits);
		अन्यथा अगर (pos >= BigInteger(Get_आकार(arg1)))
			परिणाम.set(i, sign_ext && !arg1.empty() ? arg1.back() : vacant_bits);
		अन्यथा
			परिणाम.set(i, arg1[pos.toInt()]);
	}

	वापस परिणाम;
}

RTLIL::Const RTLIL::const_shl(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);
	वापस const_shift_worker(arg1_ext, arg2, false, false, -1, परिणाम_len);
}

RTLIL::Const RTLIL::const_shr(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(परिणाम_len, Get_आकार(arg1)), signed1);
	वापस const_shift_worker(arg1_ext, arg2, false, false, +1, परिणाम_len);
}

RTLIL::Const RTLIL::const_sshl(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool, पूर्णांक परिणाम_len)
{
	वापस const_shift_worker(arg1, arg2, signed1, false, -1, परिणाम_len);
}

RTLIL::Const RTLIL::const_sshr(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool, पूर्णांक परिणाम_len)
{
	वापस const_shift_worker(arg1, arg2, signed1, false, +1, परिणाम_len);
}

RTLIL::Const RTLIL::const_shift(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(परिणाम_len, Get_आकार(arg1)), signed1);
	वापस const_shift_worker(arg1_ext, arg2, false, signed2, +1, परिणाम_len);
}

RTLIL::Const RTLIL::const_shiftx(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool, bool signed2, पूर्णांक परिणाम_len)
{
	वापस const_shift_worker(arg1, arg2, false, signed2, +1, परिणाम_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) < const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_le(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) <= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_eq(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const परिणाम(RTLIL::State::S0, परिणाम_len);

	पूर्णांक width = max(Get_आकार(arg1_ext), Get_आकार(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	RTLIL::State matched_status = RTLIL::State::S1;
	क्रम (स्वतः i = 0; i < arg1_ext.आकार(); i++) {
		अगर (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			वापस परिणाम;
		अगर (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			वापस परिणाम;
		अगर (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_status = RTLIL::State::Sx;
	}

	परिणाम.set(0, matched_status);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_ne(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	RTLIL::Const परिणाम = RTLIL::const_eq(arg1, arg2, signed1, signed2, परिणाम_len);
	अगर (परिणाम.front() == RTLIL::State::S0)
		परिणाम.set(0, RTLIL::State::S1);
	अन्यथा अगर (परिणाम.front() == RTLIL::State::S1)
		परिणाम.set(0, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_eqx(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const परिणाम(RTLIL::State::S0, परिणाम_len);

	पूर्णांक width = max(Get_आकार(arg1_ext), Get_आकार(arg2_ext));
	extend_u0(arg1_ext, width, signed1 && signed2);
	extend_u0(arg2_ext, width, signed1 && signed2);

	क्रम (स्वतः i = 0; i < arg1_ext.आकार(); i++) {
		अगर (arg1_ext.at(i) != arg2_ext.at(i))
			वापस परिणाम;
	}

	परिणाम.set(0, RTLIL::State::S1);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_nex(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	RTLIL::Const परिणाम = RTLIL::const_eqx(arg1, arg2, signed1, signed2, परिणाम_len);
	अगर (परिणाम.front() == RTLIL::State::S0)
		परिणाम.set(0, RTLIL::State::S1);
	अन्यथा अगर (परिणाम.front() == RTLIL::State::S1)
		परिणाम.set(0, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_ge(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) >= const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_gt(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	bool y = const2big(arg1, signed1, undef_bit_pos) > const2big(arg2, signed2, undef_bit_pos);
	RTLIL::Const परिणाम(undef_bit_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	अगर (Get_आकार(परिणाम) < परिणाम_len)
		परिणाम.resize(परिणाम_len, RTLIL::State::S0);
	वापस परिणाम;
}

RTLIL::Const RTLIL::const_add(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) + const2big(arg2, signed2, undef_bit_pos);
	वापस big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_sub(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) - const2big(arg2, signed2, undef_bit_pos);
	वापस big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), undef_bit_pos);
}

RTLIL::Const RTLIL::const_mul(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_bit_pos) * const2big(arg2, signed2, undef_bit_pos);
	वापस big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	अगर (b.isZero())
		वापस RTLIL::Const(RTLIL::State::Sx, परिणाम_len);
	bool परिणाम_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	वापस big2const(परिणाम_neg ? -(a / b) : (a / b), परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	अगर (b.isZero())
		वापस RTLIL::Const(RTLIL::State::Sx, परिणाम_len);
	bool परिणाम_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	वापस big2const(परिणाम_neg ? -(a % b) : (a % b), परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	अगर (b.isZero())
		वापस RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	bool परिणाम_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger परिणाम;

	अगर (परिणाम_pos || a == 0) {
		परिणाम = a / b;
	} अन्यथा {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		परिणाम = -((a + b - 1) / b);
	}
	वापस big2const(परिणाम, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	अगर (b.isZero())
		वापस RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	अगर (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} अन्यथा {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	वापस big2const(modulo, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pow(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, bool signed1, bool signed2, पूर्णांक परिणाम_len)
{
	पूर्णांक undef_bit_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_bit_pos);
	BigInteger b = const2big(arg2, signed2, undef_bit_pos);
	BigInteger y = 1;

	अगर (a == 0 && b < 0)
		वापस RTLIL::Const(RTLIL::State::Sx, परिणाम_len);

	अगर (a == 0 && b > 0)
		वापस RTLIL::Const(RTLIL::State::S0, परिणाम_len);

	अगर (b < 0)
	{
		अगर (a < -1 || a > 1)
			y = 0;
		अगर (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	अगर (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		पूर्णांक modulus_bits = (परिणाम_len >= 0 ? परिणाम_len : 1024);
		क्रम (पूर्णांक i = 0; i < modulus_bits; i++)
			modulus *= 2;

		bool flip_परिणाम_sign = false;
		अगर (a < 0) {
			a *= -1;
			अगर (b % 2 == 1)
				flip_परिणाम_sign = true;
		}

		जबतक (b > 0) {
			अगर (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		अगर (flip_परिणाम_sign)
			y *= -1;
	}

	वापस big2const(y, परिणाम_len >= 0 ? परिणाम_len : max(Get_आकार(arg1), Get_आकार(arg2)), min(undef_bit_pos, 0));
}

RTLIL::Const RTLIL::const_pos(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool signed1, bool, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);

	वापस arg1_ext;
}

RTLIL::Const RTLIL::const_buf(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool signed1, bool, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, परिणाम_len, signed1);

	वापस arg1_ext;
}

RTLIL::Const RTLIL::const_neg(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const&, bool signed1, bool, पूर्णांक परिणाम_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	वापस RTLIL::const_sub(zero, arg1_ext, true, signed1, परिणाम_len);
}

RTLIL::Const RTLIL::const_mux(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, स्थिर RTLIL::Const &arg3)
{
	log_assert(arg2.आकार() == arg1.आकार());
	अगर (arg3[0] == State::S0)
		वापस arg1;
	अन्यथा अगर (arg3[0] == State::S1)
		वापस arg2;

	RTLIL::Const ret = arg1;
	क्रम (स्वतः i = 0; i < ret.आकार(); i++)
		अगर (ret[i] != arg2[i])
			ret.set(i, State::Sx);
	वापस ret;
}

RTLIL::Const RTLIL::const_pmux(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, स्थिर RTLIL::Const &arg3)
{
	अगर (arg3.is_fully_zero())
		वापस arg1;

	अगर (!arg3.is_onehot())
		वापस RTLIL::Const(State::Sx, arg1.आकार());

	क्रम (स्वतः i = 0; i < arg3.आकार(); i++)
		अगर (arg3[i] == State::S1)
			वापस RTLIL::Const(std::vector<RTLIL::State>(arg2.begin() + i*arg1.आकार(), arg2.begin() + (i+1)*arg1.आकार()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2)
{
	std::vector<State> t = arg1.to_bits();

	क्रम (पूर्णांक i = Get_आकार(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::vector<RTLIL::State> new_t;
		अगर (sel == State::S0)
			new_t = std::vector<RTLIL::State>(t.begin(), t.begin() + Get_आकार(t)/2);
		अन्यथा अगर (sel == State::S1)
			new_t = std::vector<RTLIL::State>(t.begin() + Get_आकार(t)/2, t.end());
		अन्यथा
			क्रम (पूर्णांक j = 0; j < Get_आकार(t)/2; j++)
				new_t.धकेलो_back(t[j] == t[j + Get_आकार(t)/2] ? t[j] : RTLIL::Sx);
		t.swap(new_t);
	}

	वापस t;
}

RTLIL::Const RTLIL::const_demux(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2)
{
	पूर्णांक width = Get_आकार(arg1);
	पूर्णांक s_width = Get_आकार(arg2);
	std::vector<RTLIL::State> res;
	क्रम (पूर्णांक i = 0; i < (1 << s_width); i++)
	{
		bool ne = false;
		bool x = false;
		क्रम (पूर्णांक j = 0; j < s_width; j++) {
			bool bit = i & 1 << j;
			अगर (arg2[j] == (bit ? RTLIL::S0 : RTLIL::S1))
				ne = true;
			अन्यथा अगर (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = true;
		}
		अगर (ne) {
			क्रम (पूर्णांक j = 0; j < width; j++)
				res.धकेलो_back(State::S0);
		} अन्यथा अगर (x) {
			क्रम (पूर्णांक j = 0; j < width; j++)
				res.धकेलो_back(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} अन्यथा {
			क्रम (पूर्णांक j = 0; j < width; j++)
				res.धकेलो_back(arg1[j]);
		}
	}
	वापस res;
}

RTLIL::Const RTLIL::const_bweqx(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2)
{
	log_assert(arg2.आकार() == arg1.आकार());
	RTLIL::Const परिणाम(RTLIL::State::S0, arg1.आकार());
	क्रम (स्वतः i = 0; i < arg1.आकार(); i++)
		परिणाम.set(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	वापस परिणाम;
}

RTLIL::Const RTLIL::const_bwmux(स्थिर RTLIL::Const &arg1, स्थिर RTLIL::Const &arg2, स्थिर RTLIL::Const &arg3)
{
	log_assert(arg2.आकार() == arg1.आकार());
	log_assert(arg3.आकार() == arg1.आकार());
	RTLIL::Const परिणाम(RTLIL::State::Sx, arg1.आकार());
	क्रम (स्वतः i = 0; i < arg1.आकार(); i++) {
		अगर (arg3[i] != State::Sx || arg1[i] == arg2[i])
			परिणाम.set(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	वापस परिणाम;
}

YOSYS_NAMESPACE_END
