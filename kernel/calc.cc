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

ಸ್ಥಿರ ಶೂನ್ಯ extend_u0(RTLIL::Const &arg, ಪೂರ್ಣಾಂಕ ಅಗಲ, ತಾರ್ಕಿಕ is_signed)
{
	RTLIL::State padding = RTLIL::State::S0;

	ಆದರೆ (arg.ಗಾತ್ರ_2() > 0 && is_signed)
		padding = arg.ಹಿಂಭಾಗ();

	arg.resize(ಅಗಲ, padding);
}

ಸ್ಥಿರ BigInteger const2big(ಸ್ಥಿರಾಂಕ RTLIL::Const &val, ತಾರ್ಕಿಕ as_signed, ಪೂರ್ಣಾಂಕ &undef_ಬಿಟ್_pos)
{
	BigUnsigned mag;

	BigInteger::Sign sign = BigInteger::ಧನಾತ್ಮಕ;
	State inv_sign_ಬಿಟ್ = RTLIL::State::S1;
	ಸ್ವಯಂಚಾಲಿತ ಅಂಕೆ_ಬಿಟ್ಟುಗಳು = val.ಗಾತ್ರ_2();

	ಆದರೆ (as_signed && ಅಂಕೆ_ಬಿಟ್ಟುಗಳು && val[ಅಂಕೆ_ಬಿಟ್ಟುಗಳು-1] == RTLIL::State::S1) {
		inv_sign_ಬಿಟ್ = RTLIL::State::S0;
		sign = BigInteger::negative;
		ಅಂಕೆ_ಬಿಟ್ಟುಗಳು--;
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < ಅಂಕೆ_ಬಿಟ್ಟುಗಳು; i++)
		ಆದರೆ (val[i] == RTLIL::State::S0 || val[i] == RTLIL::State::S1)
			mag.ಗಣ_ಬಿಟ್(i, val[i] == inv_sign_ಬಿಟ್);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (undef_ಬಿಟ್_pos < 0)
			undef_ಬಿಟ್_pos = i;

	ಆದರೆ (sign == BigInteger::negative)
		mag += 1;

	ಹಿಂತಿರುಗಿ BigInteger(mag, sign);
}

ಸ್ಥಿರ RTLIL::Const big2const(ಸ್ಥಿರಾಂಕ BigInteger &val, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len, ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos)
{
	ಆದರೆ (undef_ಬಿಟ್_pos >= 0)
		ಹಿಂತಿರುಗಿ RTLIL::Const(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);

	BigUnsigned mag = val.getMagnitude();
	RTLIL::Const ಫಲಿತಾಂಶ(0, ಫಲಿತಾಂಶ_len);

	ಆದರೆ (!mag.isZero())
	{
		ಆದರೆ (val.getSign() < 0)
		{
			mag--;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < ಫಲಿತಾಂಶ_len; i++)
				ಫಲಿತಾಂಶ.ಗಣ(i, mag.get_ಬಿಟ್(i) ? RTLIL::State::S0 : RTLIL::State::S1);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < ಫಲಿತಾಂಶ_len; i++)
				ಫಲಿತಾಂಶ.ಗಣ(i, mag.get_ಬಿಟ್(i) ? RTLIL::State::S1 : RTLIL::State::S0);
		}
	}

#ಆದರೆ 0
	ಆದರೆ (undef_ಬಿಟ್_pos >= 0)
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = undef_ಬಿಟ್_pos; i < ಫಲಿತಾಂಶ_len; i++)
			ಫಲಿತಾಂಶ[i] = RTLIL::State::Sx;
#ಆದರೆ_ಅಂತ್ಯ

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ RTLIL::State logic_and(RTLIL::State a, RTLIL::State b)
{
	ಆದರೆ (a == RTLIL::State::S0) ಹಿಂತಿರುಗಿ RTLIL::State::S0;
	ಆದರೆ (b == RTLIL::State::S0) ಹಿಂತಿರುಗಿ RTLIL::State::S0;
	ಆದರೆ (a != RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಆದರೆ (b != RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಹಿಂತಿರುಗಿ RTLIL::State::S1;
}

ಸ್ಥಿರ RTLIL::State logic_or(RTLIL::State a, RTLIL::State b)
{
	ಆದರೆ (a == RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::S1;
	ಆದರೆ (b == RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::S1;
	ಆದರೆ (a != RTLIL::State::S0) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಆದರೆ (b != RTLIL::State::S0) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಹಿಂತಿರುಗಿ RTLIL::State::S0;
}

ಸ್ಥಿರ RTLIL::State logic_xor(RTLIL::State a, RTLIL::State b)
{
	ಆದರೆ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಆದರೆ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಹಿಂತಿರುಗಿ a != b ? RTLIL::State::S1 : RTLIL::State::S0;
}

ಸ್ಥಿರ RTLIL::State logic_xnor(RTLIL::State a, RTLIL::State b)
{
	ಆದರೆ (a != RTLIL::State::S0 && a != RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಆದರೆ (b != RTLIL::State::S0 && b != RTLIL::State::S1) ಹಿಂತಿರುಗಿ RTLIL::State::Sx;
	ಹಿಂತಿರುಗಿ a == b ? RTLIL::State::S1 : RTLIL::State::S0;
}

RTLIL::Const RTLIL::const_not(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಆದರೆ (ಫಲಿತಾಂಶ_len < 0)
		ಫಲಿತಾಂಶ_len = Get_ಗಾತ್ರ(arg1);

	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ಫಲಿತಾಂಶ_len, signed1);

	RTLIL::Const ಫಲಿತಾಂಶ(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < ಫಲಿತಾಂಶ_len; i++) {
		ಆದರೆ (i >= Get_ಗಾತ್ರ(arg1_ext))
			ಫಲಿತಾಂಶ.ಗಣ(i, RTLIL::State::S0);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg1_ext[i] == RTLIL::State::S0)
			ಫಲಿತಾಂಶ.ಗಣ(i, RTLIL::State::S1);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg1_ext[i] == RTLIL::State::S1)
			ಫಲಿತಾಂಶ.ಗಣ(i, RTLIL::State::S0);
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ RTLIL::Const logic_wrapper(RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State),
		RTLIL::Const arg1, RTLIL::Const arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len = -1)
{
	ಆದರೆ (ಫಲಿತಾಂಶ_len < 0)
		ಫಲಿತಾಂಶ_len = max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2));

	extend_u0(arg1, ಫಲಿತಾಂಶ_len, signed1);
	extend_u0(arg2, ಫಲಿತಾಂಶ_len, signed2);

	RTLIL::Const ಫಲಿತಾಂಶ(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < ಫಲಿತಾಂಶ_len; i++) {
		RTLIL::State a = i < Get_ಗಾತ್ರ(arg1) ? arg1[i] : RTLIL::State::S0;
		RTLIL::State b = i < Get_ಗಾತ್ರ(arg2) ? arg2[i] : RTLIL::State::S0;
		ಫಲಿತಾಂಶ.ಗಣ(i, logic_func(a, b));
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_and(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_wrapper(logic_and, arg1, arg2, signed1, signed2, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_or(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_wrapper(logic_or, arg1, arg2, signed1, signed2, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_xor(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_wrapper(logic_xor, arg1, arg2, signed1, signed2, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_xnor(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_wrapper(logic_xnor, arg1, arg2, signed1, signed2, ಫಲಿತಾಂಶ_len);
}

ಸ್ಥಿರ RTLIL::Const logic_reduce_wrapper(RTLIL::State initial, RTLIL::State(*logic_func)(RTLIL::State, RTLIL::State), ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::State ತಾತ್ಕಾಲಿಕ = initial;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < arg1.ಗಾತ್ರ_2(); i++)
		ತಾತ್ಕಾಲಿಕ = logic_func(ತಾತ್ಕಾಲಿಕ, arg1[i]);

	RTLIL::Const ಫಲಿತಾಂಶ(ತಾತ್ಕಾಲಿಕ);
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_reduce_and(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_reduce_wrapper(RTLIL::State::S1, logic_and, arg1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_reduce_or(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_reduce_xor(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_reduce_xnor(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const ಬಫರ್ = logic_reduce_wrapper(RTLIL::State::S0, logic_xor, arg1, ಫಲಿತಾಂಶ_len);
	ಆದರೆ (!ಬಫರ್.ಖಾಲಿಗಳು()) {
		ಆದರೆ (ಬಫರ್.front() == RTLIL::State::S0)
			ಬಫರ್.ಗಣ(0, RTLIL::State::S1);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಫರ್.front() == RTLIL::State::S1)
			ಬಫರ್.ಗಣ(0, RTLIL::State::S0);
	}
	ಹಿಂತಿರುಗಿ ಬಫರ್;
}

RTLIL::Const RTLIL::const_reduce_bool(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ logic_reduce_wrapper(RTLIL::State::S0, logic_or, arg1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_logic_not(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos_a = -1;
	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos_a);
	RTLIL::Const ಫಲಿತಾಂಶ(a.isZero() ? undef_ಬಿಟ್_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S1 : RTLIL::State::S0);
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_logic_and(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos_a = -1, undef_ಬಿಟ್_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ಬಿಟ್_pos_b);

	RTLIL::State ಬಿಟ್_a = a.isZero() ? undef_ಬಿಟ್_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ಬಿಟ್_b = b.isZero() ? undef_ಬಿಟ್_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const ಫಲಿತಾಂಶ(logic_and(ಬಿಟ್_a, ಬಿಟ್_b));
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_logic_or(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos_a = -1, undef_ಬಿಟ್_pos_b = -1;
	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos_a);
	BigInteger b = const2big(arg2, signed2, undef_ಬಿಟ್_pos_b);

	RTLIL::State ಬಿಟ್_a = a.isZero() ? undef_ಬಿಟ್_pos_a >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::State ಬಿಟ್_b = b.isZero() ? undef_ಬಿಟ್_pos_b >= 0 ? RTLIL::State::Sx : RTLIL::State::S0 : RTLIL::State::S1;
	RTLIL::Const ಫಲಿತಾಂಶ(logic_or(ಬಿಟ್_a, ಬಿಟ್_b));
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

// Shift `arg1` by `arg2` bits.
// If `direction` is +1, `arg1` is shifted right by `arg2` bits; if `direction` is -1, `arg1` is shifted left by `arg2` bits.
// If `signed2` is true, `arg2` is interpreted as a signed integer; a negative `arg2` will cause a shift in the opposite direction.
// Any required bits outside the bounds of `arg1` are padded with `vacant_bits` unless `sign_ext` is true, in which case any bits outside the left
// bounds are filled with the leftmost bit of `arg1` (arithmetic shift).
ಸ್ಥಿರ RTLIL::Const const_ಸರಿಸು_ಕೆಲಸಗಾರ(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ sign_ext, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ direction, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len, RTLIL::State vacant_ಬಿಟ್ಟುಗಳು = RTLIL::State::S0)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger ಸ್ಥಾನಾಂತರ = const2big(arg2, signed2, undef_ಬಿಟ್_pos) * direction;

	ಆದರೆ (ಫಲಿತಾಂಶ_len < 0)
		ಫಲಿತಾಂಶ_len = Get_ಗಾತ್ರ(arg1);

	RTLIL::Const ಫಲಿತಾಂಶ(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);
	ಆದರೆ (undef_ಬಿಟ್_pos >= 0)
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಫಲಿತಾಂಶ_len; i++) {
		BigInteger pos = BigInteger(i) + ಸ್ಥಾನಾಂತರ;
		ಆದರೆ (pos < 0)
			ಫಲಿತಾಂಶ.ಗಣ(i, vacant_ಬಿಟ್ಟುಗಳು);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pos >= BigInteger(Get_ಗಾತ್ರ(arg1)))
			ಫಲಿತಾಂಶ.ಗಣ(i, sign_ext && !arg1.ಖಾಲಿಗಳು() ? arg1.ಹಿಂಭಾಗ() : vacant_ಬಿಟ್ಟುಗಳು);
		ಇಲ್ಲದಿದ್ದರೆ
			ಫಲಿತಾಂಶ.ಗಣ(i, arg1[pos.ಗಮ್ಯ_Int()]);
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_shl(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ಫಲಿತಾಂಶ_len, signed1);
	ಹಿಂತಿರುಗಿ const_ಸರಿಸು_ಕೆಲಸಗಾರ(arg1_ext, arg2, ಸುಳ್ಳು, ಸುಳ್ಳು, -1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_shr(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(ಫಲಿತಾಂಶ_len, Get_ಗಾತ್ರ(arg1)), signed1);
	ಹಿಂತಿರುಗಿ const_ಸರಿಸು_ಕೆಲಸಗಾರ(arg1_ext, arg2, ಸುಳ್ಳು, ಸುಳ್ಳು, +1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_sshl(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ const_ಸರಿಸು_ಕೆಲಸಗಾರ(arg1, arg2, signed1, ಸುಳ್ಳು, -1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_sshr(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ const_ಸರಿಸು_ಕೆಲಸಗಾರ(arg1, arg2, signed1, ಸುಳ್ಳು, +1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_ಸರಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, max(ಫಲಿತಾಂಶ_len, Get_ಗಾತ್ರ(arg1)), signed1);
	ಹಿಂತಿರುಗಿ const_ಸರಿಸು_ಕೆಲಸಗಾರ(arg1_ext, arg2, ಸುಳ್ಳು, signed2, +1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_shiftx(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಹಿಂತಿರುಗಿ const_ಸರಿಸು_ಕೆಲಸಗಾರ(arg1, arg2, ಸುಳ್ಳು, signed2, +1, ಫಲಿತಾಂಶ_len, RTLIL::State::Sx);
}

RTLIL::Const RTLIL::const_lt(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_ಬಿಟ್_pos) < const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	RTLIL::Const ಫಲಿತಾಂಶ(undef_ಬಿಟ್_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_le(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_ಬಿಟ್_pos) <= const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	RTLIL::Const ಫಲಿತಾಂಶ(undef_ಬಿಟ್_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_eq(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const ಫಲಿತಾಂಶ(RTLIL::State::S0, ಫಲಿತಾಂಶ_len);

	ಪೂರ್ಣಾಂಕ ಅಗಲ = max(Get_ಗಾತ್ರ(arg1_ext), Get_ಗಾತ್ರ(arg2_ext));
	extend_u0(arg1_ext, ಅಗಲ, signed1 && signed2);
	extend_u0(arg2_ext, ಅಗಲ, signed1 && signed2);

	RTLIL::State matched_ಸ್ಥಿತಿ = RTLIL::State::S1;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < arg1_ext.ಗಾತ್ರ_2(); i++) {
		ಆದರೆ (arg1_ext.at(i) == RTLIL::State::S0 && arg2_ext.at(i) == RTLIL::State::S1)
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		ಆದರೆ (arg1_ext.at(i) == RTLIL::State::S1 && arg2_ext.at(i) == RTLIL::State::S0)
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
		ಆದರೆ (arg1_ext.at(i) > RTLIL::State::S1 || arg2_ext.at(i) > RTLIL::State::S1)
			matched_ಸ್ಥಿತಿ = RTLIL::State::Sx;
	}

	ಫಲಿತಾಂಶ.ಗಣ(0, matched_ಸ್ಥಿತಿ);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_ne(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const ಫಲಿತಾಂಶ = RTLIL::const_eq(arg1, arg2, signed1, signed2, ಫಲಿತಾಂಶ_len);
	ಆದರೆ (ಫಲಿತಾಂಶ.front() == RTLIL::State::S0)
		ಫಲಿತಾಂಶ.ಗಣ(0, RTLIL::State::S1);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ.front() == RTLIL::State::S1)
		ಫಲಿತಾಂಶ.ಗಣ(0, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_eqx(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const arg2_ext = arg2;
	RTLIL::Const ಫಲಿತಾಂಶ(RTLIL::State::S0, ಫಲಿತಾಂಶ_len);

	ಪೂರ್ಣಾಂಕ ಅಗಲ = max(Get_ಗಾತ್ರ(arg1_ext), Get_ಗಾತ್ರ(arg2_ext));
	extend_u0(arg1_ext, ಅಗಲ, signed1 && signed2);
	extend_u0(arg2_ext, ಅಗಲ, signed1 && signed2);

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < arg1_ext.ಗಾತ್ರ_2(); i++) {
		ಆದರೆ (arg1_ext.at(i) != arg2_ext.at(i))
			ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಫಲಿತಾಂಶ.ಗಣ(0, RTLIL::State::S1);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_nex(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const ಫಲಿತಾಂಶ = RTLIL::const_eqx(arg1, arg2, signed1, signed2, ಫಲಿತಾಂಶ_len);
	ಆದರೆ (ಫಲಿತಾಂಶ.front() == RTLIL::State::S0)
		ಫಲಿತಾಂಶ.ಗಣ(0, RTLIL::State::S1);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಫಲಿತಾಂಶ.front() == RTLIL::State::S1)
		ಫಲಿತಾಂಶ.ಗಣ(0, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_ge(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_ಬಿಟ್_pos) >= const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	RTLIL::Const ಫಲಿತಾಂಶ(undef_ಬಿಟ್_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_gt(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	ತಾರ್ಕಿಕ y = const2big(arg1, signed1, undef_ಬಿಟ್_pos) > const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	RTLIL::Const ಫಲಿತಾಂಶ(undef_ಬಿಟ್_pos >= 0 ? RTLIL::State::Sx : y ? RTLIL::State::S1 : RTLIL::State::S0);
	ಆದರೆ (Get_ಗಾತ್ರ(ಫಲಿತಾಂಶ) < ಫಲಿತಾಂಶ_len)
		ಫಲಿತಾಂಶ.resize(ಫಲಿತಾಂಶ_len, RTLIL::State::S0);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_ಕೂಡಿಸು(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ಬಿಟ್_pos) + const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	ಹಿಂತಿರುಗಿ big2const(y, ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), undef_ಬಿಟ್_pos);
}

RTLIL::Const RTLIL::const_sub(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ಬಿಟ್_pos) - const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	ಹಿಂತಿರುಗಿ big2const(y, ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), undef_ಬಿಟ್_pos);
}

RTLIL::Const RTLIL::const_mul(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger y = const2big(arg1, signed1, undef_ಬಿಟ್_pos) * const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	ಹಿಂತಿರುಗಿ big2const(y, ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), min(undef_ಬಿಟ್_pos, 0));
}

// truncating division
RTLIL::Const RTLIL::const_div(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos);
	BigInteger b = const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	ಆದರೆ (b.isZero())
		ಹಿಂತಿರುಗಿ RTLIL::Const(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);
	ತಾರ್ಕಿಕ ಫಲಿತಾಂಶ_neg = (a.getSign() == BigInteger::negative) != (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ಹಿಂತಿರುಗಿ big2const(ಫಲಿತಾಂಶ_neg ? -(a / b) : (a / b), ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), min(undef_ಬಿಟ್_pos, 0));
}

// truncating modulo
RTLIL::Const RTLIL::const_mod(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos);
	BigInteger b = const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	ಆದರೆ (b.isZero())
		ಹಿಂತಿರುಗಿ RTLIL::Const(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);
	ತಾರ್ಕಿಕ ಫಲಿತಾಂಶ_neg = a.getSign() == BigInteger::negative;
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	ಹಿಂತಿರುಗಿ big2const(ಫಲಿತಾಂಶ_neg ? -(a % b) : (a % b), ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), min(undef_ಬಿಟ್_pos, 0));
}

RTLIL::Const RTLIL::const_divfloor(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos);
	BigInteger b = const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	ಆದರೆ (b.isZero())
		ಹಿಂತಿರುಗಿ RTLIL::Const(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);

	ತಾರ್ಕಿಕ ಫಲಿತಾಂಶ_pos = (a.getSign() == BigInteger::negative) == (b.getSign() == BigInteger::negative);
	a = a.getSign() == BigInteger::negative ? -a : a;
	b = b.getSign() == BigInteger::negative ? -b : b;
	BigInteger ಫಲಿತಾಂಶ;

	ಆದರೆ (ಫಲಿತಾಂಶ_pos || a == 0) {
		ಫಲಿತಾಂಶ = a / b;
	} ಇಲ್ಲದಿದ್ದರೆ {
		// bigint division with negative numbers is wonky, make sure we only negate at the very end
		ಫಲಿತಾಂಶ = -((a + b - 1) / b);
	}
	ಹಿಂತಿರುಗಿ big2const(ಫಲಿತಾಂಶ, ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), min(undef_ಬಿಟ್_pos, 0));
}

RTLIL::Const RTLIL::const_modfloor(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;
	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos);
	BigInteger b = const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	ಆದರೆ (b.isZero())
		ಹಿಂತಿರುಗಿ RTLIL::Const(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);

	BigInteger::Sign a_sign = a.getSign();
	BigInteger::Sign b_sign = b.getSign();
	a = a_sign == BigInteger::negative ? -a : a;
	b = b_sign == BigInteger::negative ? -b : b;
	BigInteger truncated = a_sign == BigInteger::negative ? -(a % b) : (a % b);
	BigInteger modulo;

	ಆದರೆ (truncated == 0 || (a_sign == b_sign)) {
		modulo = truncated;
	} ಇಲ್ಲದಿದ್ದರೆ {
		modulo = b_sign == BigInteger::negative ? truncated - b : truncated + b;
	}
	ಹಿಂತಿರುಗಿ big2const(modulo, ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), min(undef_ಬಿಟ್_pos, 0));
}

RTLIL::Const RTLIL::const_pow(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	ಪೂರ್ಣಾಂಕ undef_ಬಿಟ್_pos = -1;

	BigInteger a = const2big(arg1, signed1, undef_ಬಿಟ್_pos);
	BigInteger b = const2big(arg2, signed2, undef_ಬಿಟ್_pos);
	BigInteger y = 1;

	ಆದರೆ (a == 0 && b < 0)
		ಹಿಂತಿರುಗಿ RTLIL::Const(RTLIL::State::Sx, ಫಲಿತಾಂಶ_len);

	ಆದರೆ (a == 0 && b > 0)
		ಹಿಂತಿರುಗಿ RTLIL::Const(RTLIL::State::S0, ಫಲಿತಾಂಶ_len);

	ಆದರೆ (b < 0)
	{
		ಆದರೆ (a < -1 || a > 1)
			y = 0;
		ಆದರೆ (a == -1)
			y = (-b % 2) == 0 ? 1 : -1;
	}

	ಆದರೆ (b > 0)
	{
		// Power-modulo with 2^result_len as modulus
		BigInteger modulus = 1;
		ಪೂರ್ಣಾಂಕ modulus_ಬಿಟ್ಟುಗಳು = (ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : 1024);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < modulus_ಬಿಟ್ಟುಗಳು; i++)
			modulus *= 2;

		ತಾರ್ಕಿಕ flip_ಫಲಿತಾಂಶ_sign = ಸುಳ್ಳು;
		ಆದರೆ (a < 0) {
			a *= -1;
			ಆದರೆ (b % 2 == 1)
				flip_ಫಲಿತಾಂಶ_sign = ನಿಜ;
		}

		ಆಗ (b > 0) {
			ಆದರೆ (b % 2 == 1)
				y = (y * a) % modulus;
			b = b / 2;
			a = (a * a) % modulus;
		}

		ಆದರೆ (flip_ಫಲಿತಾಂಶ_sign)
			y *= -1;
	}

	ಹಿಂತಿರುಗಿ big2const(y, ಫಲಿತಾಂಶ_len >= 0 ? ಫಲಿತಾಂಶ_len : max(Get_ಗಾತ್ರ(arg1), Get_ಗಾತ್ರ(arg2)), min(undef_ಬಿಟ್_pos, 0));
}

RTLIL::Const RTLIL::const_pos(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ಫಲಿತಾಂಶ_len, signed1);

	ಹಿಂತಿರುಗಿ arg1_ext;
}

RTLIL::Const RTLIL::const_buf(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	extend_u0(arg1_ext, ಫಲಿತಾಂಶ_len, signed1);

	ಹಿಂತಿರುಗಿ arg1_ext;
}

RTLIL::Const RTLIL::const_neg(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const&, ತಾರ್ಕಿಕ signed1, ತಾರ್ಕಿಕ, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len)
{
	RTLIL::Const arg1_ext = arg1;
	RTLIL::Const zero(RTLIL::State::S0, 1);

	ಹಿಂತಿರುಗಿ RTLIL::const_sub(zero, arg1_ext, ನಿಜ, signed1, ಫಲಿತಾಂಶ_len);
}

RTLIL::Const RTLIL::const_mux(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3)
{
	log_assert(arg2.ಗಾತ್ರ_2() == arg1.ಗಾತ್ರ_2());
	ಆದರೆ (arg3[0] == State::S0)
		ಹಿಂತಿರುಗಿ arg1;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg3[0] == State::S1)
		ಹಿಂತಿರುಗಿ arg2;

	RTLIL::Const ret = arg1;
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < ret.ಗಾತ್ರ_2(); i++)
		ಆದರೆ (ret[i] != arg2[i])
			ret.ಗಣ(i, State::Sx);
	ಹಿಂತಿರುಗಿ ret;
}

RTLIL::Const RTLIL::const_pmux(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3)
{
	ಆದರೆ (arg3.is_fully_zero())
		ಹಿಂತಿರುಗಿ arg1;

	ಆದರೆ (!arg3.is_onehot())
		ಹಿಂತಿರುಗಿ RTLIL::Const(State::Sx, arg1.ಗಾತ್ರ_2());

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < arg3.ಗಾತ್ರ_2(); i++)
		ಆದರೆ (arg3[i] == State::S1)
			ಹಿಂತಿರುಗಿ RTLIL::Const(std::ಸದಿಶ<RTLIL::State>(arg2.begin() + i*arg1.ಗಾತ್ರ_2(), arg2.begin() + (i+1)*arg1.ಗಾತ್ರ_2()));

	log_abort(); // unreachable
}

RTLIL::Const RTLIL::const_bmux(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2)
{
	std::ಸದಿಶ<State> t = arg1.ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(arg2)-1; i >= 0; i--)
	{
		RTLIL::State sel = arg2.at(i);
		std::ಸದಿಶ<RTLIL::State> new_t;
		ಆದರೆ (sel == State::S0)
			new_t = std::ಸದಿಶ<RTLIL::State>(t.begin(), t.begin() + Get_ಗಾತ್ರ(t)/2);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (sel == State::S1)
			new_t = std::ಸದಿಶ<RTLIL::State>(t.begin() + Get_ಗಾತ್ರ(t)/2, t.ಕೊನೆ());
		ಇಲ್ಲದಿದ್ದರೆ
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < Get_ಗಾತ್ರ(t)/2; j++)
				new_t.ತಳ್ಳು_ಹಿಂಭಾಗ(t[j] == t[j + Get_ಗಾತ್ರ(t)/2] ? t[j] : RTLIL::Sx);
		t.ಅದಲುಬದಲು(new_t);
	}

	ಹಿಂತಿರುಗಿ t;
}

RTLIL::Const RTLIL::const_demux(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(arg1);
	ಪೂರ್ಣಾಂಕ s_ಅಗಲ = Get_ಗಾತ್ರ(arg2);
	std::ಸದಿಶ<RTLIL::State> res;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < (1 << s_ಅಗಲ); i++)
	{
		ತಾರ್ಕಿಕ ne = ಸುಳ್ಳು;
		ತಾರ್ಕಿಕ x = ಸುಳ್ಳು;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < s_ಅಗಲ; j++) {
			ತಾರ್ಕಿಕ ಬಿಟ್ = i & 1 << j;
			ಆದರೆ (arg2[j] == (ಬಿಟ್ ? RTLIL::S0 : RTLIL::S1))
				ne = ನಿಜ;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg2[j] != RTLIL::S0 && arg2[j] != RTLIL::S1)
				x = ನಿಜ;
		}
		ಆದರೆ (ne) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ; j++)
				res.ತಳ್ಳು_ಹಿಂಭಾಗ(State::S0);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (x) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ; j++)
				res.ತಳ್ಳು_ಹಿಂಭಾಗ(arg1[j] == State::S0 ? State::S0 : State::Sx);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ; j++)
				res.ತಳ್ಳು_ಹಿಂಭಾಗ(arg1[j]);
		}
	}
	ಹಿಂತಿರುಗಿ res;
}

RTLIL::Const RTLIL::const_bweqx(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2)
{
	log_assert(arg2.ಗಾತ್ರ_2() == arg1.ಗಾತ್ರ_2());
	RTLIL::Const ಫಲಿತಾಂಶ(RTLIL::State::S0, arg1.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < arg1.ಗಾತ್ರ_2(); i++)
		ಫಲಿತಾಂಶ.ಗಣ(i, arg1[i] == arg2[i] ? State::S1 : State::S0);

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

RTLIL::Const RTLIL::const_bwmux(ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3)
{
	log_assert(arg2.ಗಾತ್ರ_2() == arg1.ಗಾತ್ರ_2());
	log_assert(arg3.ಗಾತ್ರ_2() == arg1.ಗಾತ್ರ_2());
	RTLIL::Const ಫಲಿತಾಂಶ(RTLIL::State::Sx, arg1.ಗಾತ್ರ_2());
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i = 0; i < arg1.ಗಾತ್ರ_2(); i++) {
		ಆದರೆ (arg3[i] != State::Sx || arg1[i] == arg2[i])
			ಫಲಿತಾಂಶ.ಗಣ(i, arg3[i] == State::S1 ? arg2[i] : arg1[i]);
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

YOSYS_NAMESPACE_ಕೊನೆ
