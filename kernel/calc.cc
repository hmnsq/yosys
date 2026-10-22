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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "libs/bigint/BigIntegerLibrary.hh"

YOSYS_NAMESPACE_BEGIN

ਸਥਿਰ ਖਾਲੀ extend_u0(RTLIL::Const &arg, ਪੂਰਨ ਚੌੜਾਈ, ਤਾਰਕਿਕ is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	ਜੇ (arg.ਮਾਪ() > 0 && is_signed)
		padding = arg.ਪਿੱਠ();

	arg.resize(ਚੌੜਾਈ, padding);
}

ਸਥਿਰ BigInteger const2big(ਅਚਲ RTLIL::Const &val, ਤਾਰਕਿਕ as_signed, ਪੂਰਨ &undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::ਸਕਾਰਾਤਮਕ;
	State inv_sign_ਦੋਆਧਾਰੀ_ਅੰਕ = RTLIL::State::S1;
	ਸਵੈਚਾਲਿਤ ਅੰਕ_ਬਿੱਟ = val.ਮਾਪ();

	ਜੇ (as_signed && ਅੰਕ_ਬਿੱਟ && val[ਅੰਕ_ਬਿੱਟ-1] == RTLIL::State::S1) {
		inv_sign_ਦੋਆਧਾਰੀ_ਅੰਕ = RTLIL::State::S0;
		sign = BigInteger::negative;
		ਅੰਕ_ਬਿੱਟ--;
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < ਅੰਕ_ਬਿੱਟ; i++)
		ਜੇ (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.ਸੈੱਟ_ਦੋਆਧਾਰੀ_ਅੰਕ(i, val[i] == inv_sign_ਦੋਆਧਾਰੀ_ਅੰਕ);
		ਨਹੀਂਤਾਂ ਜੇ (undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos < 0)
			undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = i;

	ਜੇ (sign == BigInteger::negative)
		mag += 1;

	ਵਾਪਸ BigInteger(mag, sign);
}

ਸਥਿਰ RTLIL::Const big2const(ਅਚਲ BigInteger &val, ਪੂਰਨ ਨਤੀਜਾ_len, ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos)
{
	ਜੇ (undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos >= 0)
		ਵਾਪਸ RTLIL::Const(RTLIL::State::Sx, ਨਤੀਜਾ_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const ਨਤੀਜਾ(0, ਨਤੀਜਾ_len);

	ਜੇ (!mag.isZero())
	{
		ਜੇ (val.getSign() < 0)
		{
			mag--;
			ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < ਨਤੀਜਾ_len; i++)
				ਨਤੀਜਾ.ਸੈੱਟ(i, mag.get_ਦੋਆਧਾਰੀ_ਅੰਕ(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		ਨਹੀਂਤਾਂ
		{
			ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < ਨਤੀਜਾ_len; i++)
				ਨਤੀਜਾ.ਸੈੱਟ(i, mag.get_ਦੋਆਧਾਰੀ_ਅੰਕ(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#ਜੇ 0
	ਜੇ (undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos >= 0)
		ਲਈ (ਪੂਰਨ i = undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos; i < ਨਤੀਜਾ_len; i++)
			ਨਤੀਜਾ[i] = RTLIL::State::Sx;
#ਜੇ_ਅੰਤ

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	ਜੇ (a == RTLIL::State::S0) ਵਾਪਸ RTLIL::State::S0;
	ਜੇ (b == RTLIL::State::S0) ਵਾਪਸ RTLIL::State::S0;
	ਜੇ (a != RTLIL::State::S1) ਵਾਪਸ RTLIL::State::Sx;
	ਜੇ (b != RTLIL::State::S1) ਵਾਪਸ RTLIL::State::Sx;
	ਵਾਪਸ RTLIL::State::S1;
}

ਸਥਿਰ RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	ਜੇ (a == RTLIL::State::S1) ਵਾਪਸ RTLIL::State::S1;
	ਜੇ (b == RTLIL::State::S1) ਵਾਪਸ RTLIL::State::S1;
	ਜੇ (a != RTLIL::State::S0) ਵਾਪਸ RTLIL::State::Sx;
	ਜੇ (b != RTLIL::State::S0) ਵਾਪਸ RTLIL::State::Sx;
	ਵਾਪਸ RTLIL::State::S0;
}

ਸਥਿਰ RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	ਜੇ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ਵਾਪਸ RTLIL::State::Sx;
	ਜੇ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ਵਾਪਸ RTLIL::State::Sx;
	ਵਾਪਸ a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

ਸਥਿਰ RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	ਜੇ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ਵਾਪਸ RTLIL::State::Sx;
	ਜੇ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ਵਾਪਸ RTLIL::State::Sx;
	ਵਾਪਸ a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਜੇ (ਨਤੀਜਾ_len < 0)
		ਨਤੀਜਾ_len = Get_ਮਾਪ(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ਨਤੀਜਾ_len, signed1);

	RTLIL::Const ਨਤੀਜਾ(RTLIL::State::Sx, ਨਤੀਜਾ_len);
	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < ਨਤੀਜਾ_len; i++) {
		ਜੇ (i >= Get_ਮਾਪ(arg1_ext))
			ਨਤੀਜਾ.ਸੈੱਟ(i, RTLIL::State::S0);
		ਨਹੀਂਤਾਂ ਜੇ (arg1_ext[i] == RTLIL::State::S0)
			ਨਤੀਜਾ.ਸੈੱਟ(i, RTLIL::State::S1);
		ਨਹੀਂਤਾਂ ਜੇ (arg1_ext[i] == RTLIL::State::S1)
			ਨਤੀਜਾ.ਸੈੱਟ(i, RTLIL::State::S0);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len = -1)
{
	ਜੇ (ਨਤੀਜਾ_len < 0)
		ਨਤੀਜਾ_len = max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2));

	extend_u0(arg1, ਨਤੀਜਾ_len, signed1);
	extend_u0(arg2, ਨਤੀਜਾ_len, signed2);

	RTLIL::Const ਨਤੀਜਾ(RTLIL::State::Sx, ਨਤੀਜਾ_len);
	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < ਨਤੀਜਾ_len; i++) {
		RTLIL::State a = i < Get_ਮਾਪ(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_ਮਾਪ(arg2) ? arg2[i] : RTLIL::State::S0;
		ਨਤੀਜਾ.ਸੈੱਟ(i, logic_func(a, b));
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_and(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_wrapper(logic_and, arg1, arg2, signed1, signed2, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_or(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_wrapper(logic_or, arg1, arg2, signed1, signed2, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_xor(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_xnor(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, ਨਤੀਜਾ_len);
}

ਸਥਿਰ RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ਅਚਲ RTLIL::Const &arg1, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::State ਆਰਜ਼ੀ = initial;

	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < arg1.ਮਾਪ(); i++)
		ਆਰਜ਼ੀ = logic_func(ਆਰਜ਼ੀ, arg1[i]);

	RTLIL::Const ਨਤੀਜਾ(ਆਰਜ਼ੀ);
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_reduce_and(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_reduce_or(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const ਬਫਰ = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, ਨਤੀਜਾ_len);
	ਜੇ (!ਬਫਰ.ਖਾਲੀ_ਥਾਵਾਂ()) {
		ਜੇ (ਬਫਰ.front() == RTLIL::State::S0)
			ਬਫਰ.ਸੈੱਟ(0, RTLIL::State::S1);
		ਨਹੀਂਤਾਂ ਜੇ (ਬਫਰ.front() == RTLIL::State::S1)
			ਬਫਰ.ਸੈੱਟ(0, RTLIL::State::S0);
	}
	ਵਾਪਸ ਬਫਰ;
}

RTLIL::Const RTLIL::const_reduce_bool(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_logic_not(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a);
	RTLIL::Const ਨਤੀਜਾ(a.isZero() ? undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_logic_and(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a = -1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_b);

	RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ_a = a.isZero() ? undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ_b = b.isZero() ? undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const ਨਤੀਜਾ(logic_and(ਦੋਆਧਾਰੀ_ਅੰਕ_a, ਦੋਆਧਾਰੀ_ਅੰਕ_b));
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_logic_or(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a = -1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_b);

	RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ_a = a.isZero() ? undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ਦੋਆਧਾਰੀ_ਅੰਕ_b = b.isZero() ? undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const ਨਤੀਜਾ(logic_or(ਦੋਆਧਾਰੀ_ਅੰਕ_a, ਦੋਆਧਾਰੀ_ਅੰਕ_b));
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ਸਥਿਰ RTLIL::Const const_ਖਿਸਕਾਓ_ਕਾਮਾ(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ sign_ext, ਤਾਰਕਿਕ signed2, ਪੂਰਨ direction, ਪੂਰਨ ਨਤੀਜਾ_len, RTLIL::State vacant_ਬਿੱਟ = RTLIL::State::S0)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger ਵਿੱਥ = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) * direction;

	ਜੇ (ਨਤੀਜਾ_len < 0)
		ਨਤੀਜਾ_len = Get_ਮਾਪ(arg1);

	RTLIL::Const ਨਤੀਜਾ(RTLIL::State::Sx, ਨਤੀਜਾ_len);
	ਜੇ (undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos >= 0)
		ਵਾਪਸ ਨਤੀਜਾ;

	ਲਈ (ਪੂਰਨ i = 0; i < ਨਤੀਜਾ_len; i++) {
		BigInteger pos = BigInteger(i) + ਵਿੱਥ;
		ਜੇ (pos < 0)
			ਨਤੀਜਾ.ਸੈੱਟ(i, vacant_ਬਿੱਟ);
		ਨਹੀਂਤਾਂ ਜੇ (pos >= BigInteger(Get_ਮਾਪ(arg1)))
			ਨਤੀਜਾ.ਸੈੱਟ(i, sign_ext && !arg1.ਖਾਲੀ_ਥਾਵਾਂ() ? arg1.ਪਿੱਠ() : vacant_ਬਿੱਟ);
		ਨਹੀਂਤਾਂ
			ਨਤੀਜਾ.ਸੈੱਟ(i, arg1[pos.ਮੰਜ਼ਿਲ_Int()]);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_shl(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ਨਤੀਜਾ_len, signed1);
	ਵਾਪਸ const_ਖਿਸਕਾਓ_ਕਾਮਾ(arg1_ext, arg2, ਝੂਠ, ਝੂਠ, -1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_shr(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(ਨਤੀਜਾ_len, Get_ਮਾਪ(arg1)), signed1);
	ਵਾਪਸ const_ਖਿਸਕਾਓ_ਕਾਮਾ(arg1_ext, arg2, ਝੂਠ, ਝੂਠ, +1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_sshl(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ const_ਖਿਸਕਾਓ_ਕਾਮਾ(arg1, arg2, signed1, ਝੂਠ, -1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_sshr(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ const_ਖਿਸਕਾਓ_ਕਾਮਾ(arg1, arg2, signed1, ਝੂਠ, +1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_ਖਿਸਕਾਓ(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(ਨਤੀਜਾ_len, Get_ਮਾਪ(arg1)), signed1);
	ਵਾਪਸ const_ਖਿਸਕਾਓ_ਕਾਮਾ(arg1_ext, arg2, ਝੂਠ, signed2, +1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_shiftx(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਵਾਪਸ const_ਖਿਸਕਾਓ_ਕਾਮਾ(arg1, arg2, ਝੂਠ, signed2, +1, ਨਤੀਜਾ_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	ਤਾਰਕਿਕ y = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) < const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	RTLIL::Const ਨਤੀਜਾ(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_le(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	ਤਾਰਕਿਕ y = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) <= const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	RTLIL::Const ਨਤੀਜਾ(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_eq(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const ਨਤੀਜਾ(RTLIL::State::S0, ਨਤੀਜਾ_len);

	ਪੂਰਨ ਚੌੜਾਈ = max(Get_ਮਾਪ(arg1_ext), Get_ਮਾਪ(arg2_ext));
	extend_u0(arg1_ext, ਚੌੜਾਈ, signed1 && signed2);
	extend_u0(arg2_ext, ਚੌੜਾਈ, signed1 && signed2);

	RTLIL::State matched_ਸਥਿਤੀ = RTLIL::State::S1;
	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < arg1_ext.ਮਾਪ(); i++) {
		ਜੇ (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			ਵਾਪਸ ਨਤੀਜਾ;
		ਜੇ (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			ਵਾਪਸ ਨਤੀਜਾ;
		ਜੇ (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_ਸਥਿਤੀ = RTLIL::State::Sx;
	}

	ਨਤੀਜਾ.ਸੈੱਟ(0, matched_ਸਥਿਤੀ);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_ne(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const ਨਤੀਜਾ = RTLIL::const_eq(arg1, arg2, signed1, signed2, ਨਤੀਜਾ_len);
	ਜੇ (ਨਤੀਜਾ.front() == RTLIL::State::S0)
		ਨਤੀਜਾ.ਸੈੱਟ(0, RTLIL::State::S1);
	ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ.front() == RTLIL::State::S1)
		ਨਤੀਜਾ.ਸੈੱਟ(0, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_eqx(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const ਨਤੀਜਾ(RTLIL::State::S0, ਨਤੀਜਾ_len);

	ਪੂਰਨ ਚੌੜਾਈ = max(Get_ਮਾਪ(arg1_ext), Get_ਮਾਪ(arg2_ext));
	extend_u0(arg1_ext, ਚੌੜਾਈ, signed1 && signed2);
	extend_u0(arg2_ext, ਚੌੜਾਈ, signed1 && signed2);

	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < arg1_ext.ਮਾਪ(); i++) {
		ਜੇ (arg1_ext.at(i) != arg2_ext.at(i))
			ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਨਤੀਜਾ.ਸੈੱਟ(0, RTLIL::State::S1);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_nex(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const ਨਤੀਜਾ = RTLIL::const_eqx(arg1, arg2, signed1, signed2, ਨਤੀਜਾ_len);
	ਜੇ (ਨਤੀਜਾ.front() == RTLIL::State::S0)
		ਨਤੀਜਾ.ਸੈੱਟ(0, RTLIL::State::S1);
	ਨਹੀਂਤਾਂ ਜੇ (ਨਤੀਜਾ.front() == RTLIL::State::S1)
		ਨਤੀਜਾ.ਸੈੱਟ(0, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_ge(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	ਤਾਰਕਿਕ y = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) >= const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	RTLIL::Const ਨਤੀਜਾ(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_gt(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	ਤਾਰਕਿਕ y = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) > const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	RTLIL::Const ਨਤੀਜਾ(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ਜੇ (Get_ਮਾਪ(ਨਤੀਜਾ) < ਨਤੀਜਾ_len)
		ਨਤੀਜਾ.resize(ਨਤੀਜਾ_len, RTLIL::State::S0);
	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_ਜੋੜੋ(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) + const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	ਵਾਪਸ big2const(y, ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
}

RTLIL::Const RTLIL::const_sub(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) - const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	ਵਾਪਸ big2const(y, ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
}

RTLIL::Const RTLIL::const_mul(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos) * const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	ਵਾਪਸ big2const(y, ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), min(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	ਜੇ (b.isZero())
		ਵਾਪਸ RTLIL::Const(RTLIL::State::Sx, ਨਤੀਜਾ_len);
	ਤਾਰਕਿਕ ਨਤੀਜਾ_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ਵਾਪਸ big2const(ਨਤੀਜਾ_neg ? -(a / b) : (a / b), ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), min(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	ਜੇ (b.isZero())
		ਵਾਪਸ RTLIL::Const(RTLIL::State::Sx, ਨਤੀਜਾ_len);
	ਤਾਰਕਿਕ ਨਤੀਜਾ_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ਵਾਪਸ big2const(ਨਤੀਜਾ_neg ? -(a % b) : (a % b), ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), min(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	ਜੇ (b.isZero())
		ਵਾਪਸ RTLIL::Const(RTLIL::State::Sx, ਨਤੀਜਾ_len);

	ਤਾਰਕਿਕ ਨਤੀਜਾ_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger ਨਤੀਜਾ;

	ਜੇ (ਨਤੀਜਾ_pos || a == 0) {
		ਨਤੀਜਾ = a / b;
	} ਨਹੀਂਤਾਂ {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		ਨਤੀਜਾ = -((a + b - 1) / b);
	}
	ਵਾਪਸ big2const(ਨਤੀਜਾ, ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), min(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	ਜੇ (b.isZero())
		ਵਾਪਸ RTLIL::Const(RTLIL::State::Sx, ਨਤੀਜਾ_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	ਜੇ (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} ਨਹੀਂਤਾਂ {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	ਵਾਪਸ big2const(modulo, ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), min(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ signed2, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	ਪੂਰਨ undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	BigInteger b = const2big(arg2, signed2, undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos);
	BigInteger y = 1;

	ਜੇ (a == 0 && b < 0)
		ਵਾਪਸ RTLIL::Const(RTLIL::State::Sx, ਨਤੀਜਾ_len);

	ਜੇ (a == 0 && b > 0)
		ਵਾਪਸ RTLIL::Const(RTLIL::State::S0, ਨਤੀਜਾ_len);

	ਜੇ (b < 0)
	{
		ਜੇ (a < -1 || a > 1)
			y = 0;
		ਜੇ (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	ਜੇ (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		ਪੂਰਨ modulus_ਬਿੱਟ = (ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : 1024);
		ਲਈ (ਪੂਰਨ i = 0; i < modulus_ਬਿੱਟ; i++)
			modulus *= 2;

		ਤਾਰਕਿਕ flip_ਨਤੀਜਾ_sign = ਝੂਠ;
		ਜੇ (a < 0) {
			a *= -1;
			ਜੇ (b % 2 == 1)
				flip_ਨਤੀਜਾ_sign = ਸੱਚ;
		}

		ਜਦੋਂ (b > 0) {
			ਜੇ (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		ਜੇ (flip_ਨਤੀਜਾ_sign)
			y *= -1;
	}

	ਵਾਪਸ big2const(y, ਨਤੀਜਾ_len >= 0 ? ਨਤੀਜਾ_len : max(Get_ਮਾਪ(arg1), Get_ਮਾਪ(arg2)), min(undef_ਦੋਆਧਾਰੀ_ਅੰਕ_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ਨਤੀਜਾ_len, signed1);

	ਵਾਪਸ arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ਨਤੀਜਾ_len, signed1);

	ਵਾਪਸ arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const&, ਤਾਰਕਿਕ signed1, ਤਾਰਕਿਕ, ਪੂਰਨ ਨਤੀਜਾ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	ਵਾਪਸ RTLIL::const_sub(zero, arg1_ext, ਸੱਚ, signed1, ਨਤੀਜਾ_len);
}

RTLIL::Const RTLIL::const_mux(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3)
{
	log_assert(arg2.ਮਾਪ() == arg1.ਮਾਪ());
	ਜੇ (arg3[0] == State::S0)
		ਵਾਪਸ arg1;
	ਨਹੀਂਤਾਂ ਜੇ (arg3[0] == State::S1)
		ਵਾਪਸ arg2;

	RTLIL::Const ret = arg1;
	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < ret.ਮਾਪ(); i++)
		ਜੇ (ret[i] != arg2[i])
			ret.ਸੈੱਟ(i, State::Sx);
	ਵਾਪਸ ret;
}

RTLIL::Const RTLIL::const_pmux(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3)
{
	ਜੇ (arg3.is_fully_zero())
		ਵਾਪਸ arg1;

	ਜੇ (!arg3.is_onehot())
		ਵਾਪਸ RTLIL::Const(State::Sx, arg1.ਮਾਪ());

	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < arg3.ਮਾਪ(); i++)
		ਜੇ (arg3[i] == State::S1)
			ਵਾਪਸ RTLIL::Const(std::ਸਦਿਸ਼<RTLIL::State>(arg2.begin() + i*arg1.ਮਾਪ(), arg2.begin() + (i+1)*arg1.ਮਾਪ()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2)
{
	std::ਸਦਿਸ਼<State> t = arg1.ਮੰਜ਼ਿਲ_ਬਿੱਟ();

	ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::ਸਦਿਸ਼<RTLIL::State> new_t;
		ਜੇ (sel == State::S0)
			new_t = std::ਸਦਿਸ਼<RTLIL::State>(t.begin(), t.begin() + Get_ਮਾਪ(t)/2);
		ਨਹੀਂਤਾਂ ਜੇ (sel == State::S1)
			new_t = std::ਸਦਿਸ਼<RTLIL::State>(t.begin() + Get_ਮਾਪ(t)/2, t.ਸਿਰਾ());
		ਨਹੀਂਤਾਂ
			ਲਈ (ਪੂਰਨ j = 0; j < Get_ਮਾਪ(t)/2; j++)
				new_t.ਧੱਕੋ_ਪਿੱਠ(t[j] == t[j + Get_ਮਾਪ(t)/2] ? t[j] : RTLIL::Sx);
		t.ਅਦਲਾ_ਬਦਲੀ(new_t);
	}

	ਵਾਪਸ t;
}

RTLIL::Const RTLIL::const_demux(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2)
{
	ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(arg1);
	ਪੂਰਨ s_ਚੌੜਾਈ = Get_ਮਾਪ(arg2);
	std::ਸਦਿਸ਼<RTLIL::State> res;
	ਲਈ (ਪੂਰਨ i = 0; i < (1 << s_ਚੌੜਾਈ); i++)
	{
		ਤਾਰਕਿਕ ne = ਝੂਠ;
		ਤਾਰਕਿਕ x = ਝੂਠ;
		ਲਈ (ਪੂਰਨ j = 0; j < s_ਚੌੜਾਈ; j++) {
			ਤਾਰਕਿਕ ਦੋਆਧਾਰੀ_ਅੰਕ = i & 1 << j;
			ਜੇ (arg2[j] == (ਦੋਆਧਾਰੀ_ਅੰਕ ? RTLIL::S0 : RTLIL::S1))
				ne = ਸੱਚ;
			ਨਹੀਂਤਾਂ ਜੇ (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = ਸੱਚ;
		}
		ਜੇ (ne) {
			ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ; j++)
				res.ਧੱਕੋ_ਪਿੱਠ(State::S0);
		} ਨਹੀਂਤਾਂ ਜੇ (x) {
			ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ; j++)
				res.ਧੱਕੋ_ਪਿੱਠ(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} ਨਹੀਂਤਾਂ {
			ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ; j++)
				res.ਧੱਕੋ_ਪਿੱਠ(arg1[j]);
		}
	}
	ਵਾਪਸ res;
}

RTLIL::Const RTLIL::const_bweqx(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2)
{
	log_assert(arg2.ਮਾਪ() == arg1.ਮਾਪ());
	RTLIL::Const ਨਤੀਜਾ(RTLIL::State::S0, arg1.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < arg1.ਮਾਪ(); i++)
		ਨਤੀਜਾ.ਸੈੱਟ(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	ਵਾਪਸ ਨਤੀਜਾ;
}

RTLIL::Const RTLIL::const_bwmux(ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3)
{
	log_assert(arg2.ਮਾਪ() == arg1.ਮਾਪ());
	log_assert(arg3.ਮਾਪ() == arg1.ਮਾਪ());
	RTLIL::Const ਨਤੀਜਾ(RTLIL::State::Sx, arg1.ਮਾਪ());
	ਲਈ (ਸਵੈਚਾਲਿਤ i = 0; i < arg1.ਮਾਪ(); i++) {
		ਜੇ (arg3[i] != State::Sx || arg1[i] == arg2[i])
			ਨਤੀਜਾ.ਸੈੱਟ(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	ਵਾਪਸ ਨਤੀਜਾ;
}

YOSYS_NAMESPACE_ਸਿਰਾ
