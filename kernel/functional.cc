/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2024  Emily Schmidt <emily@yosyshq.com>
 *  Copyright (C) 2024 National Technology and Engineering Solutions of Sandia, LLC
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

#ಅನ್ತರ್ಭಾವ "kernel/functional.h"
#ಅನ್ತರ್ಭಾವ "kernel/topo_scc.h"
#ಅನ್ತರ್ಭಾವ "ff.h"
#ಅನ್ತರ್ಭಾವ "ffinit.h"
#ಅನ್ತರ್ಭಾವ <deque>

YOSYS_NAMESPACE_BEGIN
ನಾಮಾಕಾಶ Functional {

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn_ಗಮ್ಯ_string(Fn fn) {
	ಆಯ್ಕೆ(fn) {
	ಸ್ಥಿತಿ Fn::invalid: ಹಿಂತಿರುಗಿ "invalid";
	ಸ್ಥಿತಿ Fn::buf: ಹಿಂತಿರುಗಿ "buf";
	ಸ್ಥಿತಿ Fn::ತುಂಡು: ಹಿಂತಿರುಗಿ "slice";
	ಸ್ಥಿತಿ Fn::zero_extend: ಹಿಂತಿರುಗಿ "zero_extend";
	ಸ್ಥಿತಿ Fn::sign_extend: ಹಿಂತಿರುಗಿ "sign_extend";
	ಸ್ಥಿತಿ Fn::concat: ಹಿಂತಿರುಗಿ "concat";
	ಸ್ಥಿತಿ Fn::ಕೂಡಿಸು: ಹಿಂತಿರುಗಿ "add";
	ಸ್ಥಿತಿ Fn::sub: ಹಿಂತಿರುಗಿ "sub";
	ಸ್ಥಿತಿ Fn::mul: ಹಿಂತಿರುಗಿ "mul";
	ಸ್ಥಿತಿ Fn::unsigned_div: ಹಿಂತಿರುಗಿ "unsigned_div";
	ಸ್ಥಿತಿ Fn::unsigned_mod: ಹಿಂತಿರುಗಿ "unsigned_mod";
	ಸ್ಥಿತಿ Fn::bitwise_and: ಹಿಂತಿರುಗಿ "bitwise_and";
	ಸ್ಥಿತಿ Fn::bitwise_or: ಹಿಂತಿರುಗಿ "bitwise_or";
	ಸ್ಥಿತಿ Fn::bitwise_xor: ಹಿಂತಿರುಗಿ "bitwise_xor";
	ಸ್ಥಿತಿ Fn::bitwise_not: ಹಿಂತಿರುಗಿ "bitwise_not";
	ಸ್ಥಿತಿ Fn::reduce_and: ಹಿಂತಿರುಗಿ "reduce_and";
	ಸ್ಥಿತಿ Fn::reduce_or: ಹಿಂತಿರುಗಿ "reduce_or";
	ಸ್ಥಿತಿ Fn::reduce_xor: ಹಿಂತಿರುಗಿ "reduce_xor";
	ಸ್ಥಿತಿ Fn::unary_minus: ಹಿಂತಿರುಗಿ "unary_minus";
	ಸ್ಥಿತಿ Fn::equal: ಹಿಂತಿರುಗಿ "equal";
	ಸ್ಥಿತಿ Fn::not_equal: ಹಿಂತಿರುಗಿ "not_equal";
	ಸ್ಥಿತಿ Fn::signed_greater_than: ಹಿಂತಿರುಗಿ "signed_greater_than";
	ಸ್ಥಿತಿ Fn::signed_greater_equal: ಹಿಂತಿರುಗಿ "signed_greater_equal";
	ಸ್ಥಿತಿ Fn::unsigned_greater_than: ಹಿಂತಿರುಗಿ "unsigned_greater_than";
	ಸ್ಥಿತಿ Fn::unsigned_greater_equal: ಹಿಂತಿರುಗಿ "unsigned_greater_equal";
	ಸ್ಥಿತಿ Fn::logical_ಸರಿಸು_ಎಡ: ಹಿಂತಿರುಗಿ "logical_shift_left";
	ಸ್ಥಿತಿ Fn::logical_ಸರಿಸು_ಬಲ: ಹಿಂತಿರುಗಿ "logical_shift_right";
	ಸ್ಥಿತಿ Fn::arithmetic_ಸರಿಸು_ಬಲ: ಹಿಂತಿರುಗಿ "arithmetic_shift_right";
	ಸ್ಥಿತಿ Fn::mux: ಹಿಂತಿರುಗಿ "mux";
	ಸ್ಥಿತಿ Fn::constant: ಹಿಂತಿರುಗಿ "constant";
	ಸ್ಥಿತಿ Fn::ಒಳಹರಿವು: ಹಿಂತಿರುಗಿ "input";
	ಸ್ಥಿತಿ Fn::state: ಹಿಂತಿರುಗಿ "state";
	ಸ್ಥಿತಿ Fn::ಸ್ಮರಣೆ_read: ಹಿಂತಿರುಗಿ "memory_read";
	ಸ್ಥಿತಿ Fn::ಸ್ಮರಣೆ_write: ಹಿಂತಿರುಗಿ "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (ಪೂರ್ಣಾಂಕ)fn);
}

ಸದಿಶ<IRInput ಸ್ಥಿರಾಂಕ*> IR::inputs(IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ {
	ಸದಿಶ<IRInput ಸ್ಥಿರಾಂಕ*> ret;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, ಒಳಹರಿವು] : _inputs)
		ಆದರೆ(ಒಳಹರಿವು.ಬಗೆ == ಬಗೆ)
			ret.ತಳ್ಳು_ಹಿಂಭಾಗ(&ಒಳಹರಿವು);
	ಹಿಂತಿರುಗಿ ret;
}

ಸದಿಶ<IROutput ಸ್ಥಿರಾಂಕ*> IR::outputs(IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ {
	ಸದಿಶ<IROutput ಸ್ಥಿರಾಂಕ*> ret;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, output] : _outputs)
		ಆದರೆ(output.ಬಗೆ == ಬಗೆ)
			ret.ತಳ್ಳು_ಹಿಂಭಾಗ(&output);
	ಹಿಂತಿರುಗಿ ret;
}

ಸದಿಶ<IRState ಸ್ಥಿರಾಂಕ*> IR::ಸ್ಥಿತಿಗಳು(IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ {
	ಸದಿಶ<IRState ಸ್ಥಿರಾಂಕ*> ret;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, state] : ಸ್ಥಿತಿಗಳು_2)
		ಆದರೆ(state.ಬಗೆ == ಬಗೆ)
			ret.ತಳ್ಳು_ಹಿಂಭಾಗ(&state);
	ಹಿಂತಿರುಗಿ ret;
}

ಸದಿಶ<IRInput ಸ್ಥಿರಾಂಕ*> IR::all_inputs() ಸ್ಥಿರಾಂಕ {
	ಸದಿಶ<IRInput ಸ್ಥಿರಾಂಕ*> ret;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, ಒಳಹರಿವು] : _inputs)
		ret.ತಳ್ಳು_ಹಿಂಭಾಗ(&ಒಳಹರಿವು);
	ಹಿಂತಿರುಗಿ ret;
}

ಸದಿಶ<IROutput ಸ್ಥಿರಾಂಕ*> IR::all_outputs() ಸ್ಥಿರಾಂಕ {
	ಸದಿಶ<IROutput ಸ್ಥಿರಾಂಕ*> ret;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, output] : _outputs)
		ret.ತಳ್ಳು_ಹಿಂಭಾಗ(&output);
	ಹಿಂತಿರುಗಿ ret;
}

ಸದಿಶ<IRState ಸ್ಥಿರಾಂಕ*> IR::all_ಸ್ಥಿತಿಗಳು() ಸ್ಥಿರಾಂಕ {
	ಸದಿಶ<IRState ಸ್ಥಿರಾಂಕ*> ret;
	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, state] : ಸ್ಥಿತಿಗಳು_2)
		ret.ತಳ್ಳು_ಹಿಂಭಾಗ(&state);
	ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(ಗಂಟು_2)> np;
	PrintVisitor(std::function<std::string(ಗಂಟು_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string ತುಂಡು(ಗಂಟು_2, ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ) override { ಹಿಂತಿರುಗಿ "slice(" + np(a) + ", " + std::ಗಮ್ಯ_string(ಸ್ಥಾನಾಂತರ) + ", " + std::ಗಮ್ಯ_string(ಹೊರಹೋಗು_ಅಗಲ) + ")"; }
	std::string zero_extend(ಗಂಟು_2, ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ) override { ಹಿಂತಿರುಗಿ "zero_extend(" + np(a) + ", " + std::ಗಮ್ಯ_string(ಹೊರಹೋಗು_ಅಗಲ) + ")"; }
	std::string sign_extend(ಗಂಟು_2, ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ) override { ಹಿಂತಿರುಗಿ "sign_extend(" + np(a) + ", " + std::ಗಮ್ಯ_string(ಹೊರಹೋಗು_ಅಗಲ) + ")"; }
	std::string constant(ಗಂಟು_2, RTLIL::Const ಸ್ಥಿರಾಂಕ& ಮೌಲ್ಯ) override { ಹಿಂತಿರುಗಿ "constant(" + ಮೌಲ್ಯ.as_string() + ")"; }
	std::string ಒಳಹರಿವು(ಗಂಟು_2 self, IdString name, IdString ಬಗೆ) override { ಹಿಂತಿರುಗಿ "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(ಬಗೆ) + ")"; }
	std::string state(ಗಂಟು_2 self, IdString name, IdString ಬಗೆ) override { ಹಿಂತಿರುಗಿ "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(ಬಗೆ) + ")"; }
	std::string default_handler(ಗಂಟು_2 self) override {
		std::string ret = fn_ಗಮ್ಯ_string(self.fn());
		ret += "(";
		ಗಾಗಿ(ಗಾತ್ರ_t i = 0; i < self.arg_ಎಣಿಕೆ(); i++) {
			ಆದರೆ(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		ಹಿಂತಿರುಗಿ ret;
	}
};

std::string ಗಂಟು_2::ಗಮ್ಯ_string()
{
	ಹಿಂತಿರುಗಿ ಗಮ್ಯ_string([](ಗಂಟು_2 n) { ಹಿಂತಿರುಗಿ n.name().unescape(); });
}

std::string ಗಂಟು_2::ಗಮ್ಯ_string(std::function<std::string(ಗಂಟು_2)> np)
{
	ಹಿಂತಿರುಗಿ ಭೇಟಿ(PrintVisitor(np));
}

ವರ್ಗ CellSimplifier {
	Factory &factory;
	ಗಂಟು_2 sign(ಗಂಟು_2 a) {
		ಹಿಂತಿರುಗಿ factory.ತುಂಡು(a, a.ಅಗಲ() - 1, 1);
	}
	ಗಂಟು_2 neg_if(ಗಂಟು_2 a, ಗಂಟು_2 s) {
		ಹಿಂತಿರುಗಿ factory.mux(a, factory.unary_minus(a), s);
	}
	ಗಂಟು_2 ನಿರಪೇಕ್ಷ(ಗಂಟು_2 a) {
		ಹಿಂತಿರುಗಿ neg_if(a, sign(a));
	}
	ಗಂಟು_2 handle_ಸರಿಸು(ಗಂಟು_2 a, ಗಂಟು_2 b, ತಾರ್ಕಿಕ is_ಬಲ, ತಾರ್ಕಿಕ is_signed) {
		// to prevent new_width == 0, we handle this case separately
		ಆದರೆ(a.ಅಗಲ() == 1) {
			ಆದರೆ(!is_signed)
				ಹಿಂತಿರುಗಿ factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ a;
		}
		ಪೂರ್ಣಾಂಕ new_ಅಗಲ = ceil_log2(a.ಅಗಲ());
		ಗಂಟು_2 b_truncated = factory.extend(b, new_ಅಗಲ, ಸುಳ್ಳು);
		ಗಂಟು_2 y =
			!is_ಬಲ ? factory.logical_ಸರಿಸು_ಎಡ(a, b_truncated) :
			!is_signed ? factory.logical_ಸರಿಸು_ಬಲ(a, b_truncated) :
			factory.arithmetic_ಸರಿಸು_ಬಲ(a, b_truncated);
		ಆದರೆ(b.ಅಗಲ() <= new_ಅಗಲ)
			ಹಿಂತಿರುಗಿ y;
		ಗಂಟು_2 ಅತಿಪ್ರವಾಹ = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.ಅಗಲ(), b.ಅಗಲ())));
		ಗಂಟು_2 y_if_ಅತಿಪ್ರವಾಹ = is_signed ? factory.extend(sign(a), a.ಅಗಲ(), ನಿಜ) : factory.constant(RTLIL::Const(State::S0, a.ಅಗಲ()));
		ಹಿಂತಿರುಗಿ factory.mux(y, y_if_ಅತಿಪ್ರವಾಹ, ಅತಿಪ್ರವಾಹ);
	}
ಸಾರ್ವಜನಿಕ:
	ಗಂಟು_2 logical_ಸರಿಸು_ಎಡ(ಗಂಟು_2 a, ಗಂಟು_2 b) { ಹಿಂತಿರುಗಿ handle_ಸರಿಸು(a, b, ಸುಳ್ಳು, ಸುಳ್ಳು); }
	ಗಂಟು_2 logical_ಸರಿಸು_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { ಹಿಂತಿರುಗಿ handle_ಸರಿಸು(a, b, ನಿಜ, ಸುಳ್ಳು); }
	ಗಂಟು_2 arithmetic_ಸರಿಸು_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { ಹಿಂತಿರುಗಿ handle_ಸರಿಸು(a, b, ನಿಜ, ನಿಜ); }
	ಗಂಟು_2 bitwise_mux(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) {
		ಗಂಟು_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		ಗಂಟು_2 bb = factory.bitwise_and(b, s);
		ಹಿಂತಿರುಗಿ factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
ಖಾಸಗಿ:
	ಗಂಟು_2 handle_pow(ಗಂಟು_2 a0, ಗಂಟು_2 b, ಪೂರ್ಣಾಂಕ y_ಅಗಲ, ತಾರ್ಕಿಕ is_signed) {
		ಗಂಟು_2 a = factory.extend(a0, y_ಅಗಲ, is_signed);
		ಗಂಟು_2 r = factory.constant(Const(1, y_ಅಗಲ));
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < b.ಅಗಲ(); i++) {
			ಗಂಟು_2 b_ಬಿಟ್ = factory.ತುಂಡು(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_ಬಿಟ್);
			a = factory.mul(a, a);
		}
		ಆದರೆ (is_signed) {
			ಗಂಟು_2 a_ge_1 = factory.unsigned_greater_than(ನಿರಪೇಕ್ಷ(a0), factory.constant(Const(1, a0.ಅಗಲ())));
			ಗಂಟು_2 zero_ಫಲಿತಾಂಶ = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_ಅಗಲ)), zero_ಫಲಿತಾಂಶ);
		}
		ಹಿಂತಿರುಗಿ r;
	}
	ಗಂಟು_2 handle_bmux(ಗಂಟು_2 a, ಗಂಟು_2 s, ಪೂರ್ಣಾಂಕ a_ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ sn) {
		ಆದರೆ(sn < 1)
			ಹಿಂತಿರುಗಿ factory.ತುಂಡು(a, a_ಸ್ಥಾನಾಂತರ, ಅಗಲ);
		ಇಲ್ಲದಿದ್ದರೆ {
			ಗಂಟು_2 y0 = handle_bmux(a, s, a_ಸ್ಥಾನಾಂತರ, ಅಗಲ, sn - 1);
			ಗಂಟು_2 y1 = handle_bmux(a, s, a_ಸ್ಥಾನಾಂತರ + (ಅಗಲ << (sn - 1)), ಅಗಲ, sn - 1);
			ಹಿಂತಿರುಗಿ factory.mux(y0, y1, factory.ತುಂಡು(s, sn - 1, 1));
		}
	}
	ಗಂಟು_2 handle_pmux(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.ಅಗಲ() == a.ಅಗಲ() * s.ಅಗಲ());
		ಗಂಟು_2 y = a;
		ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < s.ಅಗಲ(); i++)
			y = factory.mux(y, factory.ತುಂಡು(b, a.ಅಗಲ() * i, a.ಅಗಲ()), factory.ತುಂಡು(s, i, 1));
		ಹಿಂತಿರುಗಿ y;
	}
	dict<IdString, ಗಂಟು_2> handle_fa(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 c) {
		ಗಂಟು_2 t1 = factory.bitwise_xor(a, b);
		ಗಂಟು_2 t2 = factory.bitwise_and(a, b);
		ಗಂಟು_2 t3 = factory.bitwise_and(c, t1);
		ಗಂಟು_2 y = factory.bitwise_xor(c, t1);
		ಗಂಟು_2 x = factory.bitwise_or(t2, t3);
		ಹಿಂತಿರುಗಿ {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, ಗಂಟು_2> handle_alu(ಗಂಟು_2 a_ಒಳ, ಗಂಟು_2 b_ಒಳ, ಪೂರ್ಣಾಂಕ y_ಅಗಲ, ತಾರ್ಕಿಕ is_signed, ಗಂಟು_2 ci, ಗಂಟು_2 bi) {
		ಗಂಟು_2 a = factory.extend(a_ಒಳ, y_ಅಗಲ, is_signed);
		ಗಂಟು_2 b_uninverted = factory.extend(b_ಒಳ, y_ಅಗಲ, is_signed);
		ಗಂಟು_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		ಗಂಟು_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		ಗಂಟು_2 a_extra = factory.extend(a, y_ಅಗಲ + 1, ಸುಳ್ಳು);
		ಗಂಟು_2 b_extra = factory.extend(b, y_ಅಗಲ + 1, ಸುಳ್ಳು);
		ಗಂಟು_2 y_extra = factory.ಕೂಡಿಸು(factory.ಕೂಡಿಸು(a_extra, b_extra), factory.extend(ci, a.ಅಗಲ() + 1, ಸುಳ್ಳು));
		ಗಂಟು_2 y = factory.ತುಂಡು(y_extra, 0, y_ಅಗಲ);
		ಗಂಟು_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		ಗಂಟು_2 co = factory.ತುಂಡು(carries, 1, y_ಅಗಲ);
		ಹಿಂತಿರುಗಿ {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	ಗಂಟು_2 handle_lcu(ಗಂಟು_2 p, ಗಂಟು_2 g, ಗಂಟು_2 ci) {
		ಹಿಂತಿರುಗಿ handle_alu(g, factory.bitwise_or(p, g), g.ಅಗಲ(), ಸುಳ್ಳು, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
ಸಾರ್ವಜನಿಕ:
	std::variant<dict<IdString, ಗಂಟು_2>, ಗಂಟು_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, ಗಂಟು_2> inputs)
	{
		ಪೂರ್ಣಾಂಕ a_ಅಗಲ = parameters.at(ID(A_ಅಗಲ), Const(-1)).as_int();
		ಪೂರ್ಣಾಂಕ b_ಅಗಲ = parameters.at(ID(B_ಅಗಲ), Const(-1)).as_int();
		ಪೂರ್ಣಾಂಕ y_ಅಗಲ = parameters.at(ID(Y_ಅಗಲ), Const(-1)).as_int();
		ತಾರ್ಕಿಕ a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		ತಾರ್ಕಿಕ b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		ಆದರೆ(cellType.ಒಳ(ID($ಕೂಡಿಸು), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			ತಾರ್ಕಿಕ is_signed = a_signed && b_signed;
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_ಅಗಲ, is_signed);
			ಗಂಟು_2 b = factory.extend(inputs.at(ID(B)), y_ಅಗಲ, is_signed);
			ಆದರೆ(cellType == ID($ಕೂಡಿಸು))
				ಹಿಂತಿರುಗಿ factory.ಕೂಡಿಸು(a, b);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($sub))
				ಹಿಂತಿರುಗಿ factory.sub(a, b);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($mul))
				ಹಿಂತಿರುಗಿ factory.mul(a, b);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($and))
				ಹಿಂತಿರುಗಿ factory.bitwise_and(a, b);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($or))
				ಹಿಂತಿರುಗಿ factory.bitwise_or(a, b);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($xor))
				ಹಿಂತಿರುಗಿ factory.bitwise_xor(a, b);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($xnor))
				ಹಿಂತಿರುಗಿ factory.bitwise_not(factory.bitwise_xor(a, b));
			ಇಲ್ಲದಿದ್ದರೆ
				log_abort();
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			ತಾರ್ಕಿಕ is_signed = a_signed && b_signed;
			ಪೂರ್ಣಾಂಕ ಅಗಲ = max(a_ಅಗಲ, b_ಅಗಲ);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), ಅಗಲ, is_signed);
			ಗಂಟು_2 b = factory.extend(inputs.at(ID(B)), ಅಗಲ, is_signed);
			ಆದರೆ(cellType.ಒಳ(ID($eq), ID($eqx)))
				ಹಿಂತಿರುಗಿ factory.extend(factory.equal(a, b), y_ಅಗಲ, ಸುಳ್ಳು);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($ne), ID($nex)))
				ಹಿಂತಿರುಗಿ factory.extend(factory.not_equal(a, b), y_ಅಗಲ, ಸುಳ್ಳು);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($lt))
				ಹಿಂತಿರುಗಿ factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_ಅಗಲ, ಸುಳ್ಳು);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($le))
				ಹಿಂತಿರುಗಿ factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_ಅಗಲ, ಸುಳ್ಳು);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($gt))
				ಹಿಂತಿರುಗಿ factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_ಅಗಲ, ಸುಳ್ಳು);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($ge))
				ಹಿಂತಿರುಗಿ factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_ಅಗಲ, ಸುಳ್ಳು);
			ಇಲ್ಲದಿದ್ದರೆ
				log_abort();
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($logic_or), ID($logic_and))){
			ಗಂಟು_2 a = factory.reduce_or(inputs.at(ID(A)));
			ಗಂಟು_2 b = factory.reduce_or(inputs.at(ID(B)));
			ಗಂಟು_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			ಹಿಂತಿರುಗಿ factory.extend(y, y_ಅಗಲ, ಸುಳ್ಳು);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($not)){
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_ಅಗಲ, a_signed);
			ಹಿಂತಿರುಗಿ factory.bitwise_not(a);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($pos)){
			ಹಿಂತಿರುಗಿ factory.extend(inputs.at(ID(A)), y_ಅಗಲ, a_signed);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($barrier)){
			ಹಿಂತಿರುಗಿ inputs.at(ID(A));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($neg)){
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_ಅಗಲ, a_signed);
			ಹಿಂತಿರುಗಿ factory.unary_minus(a);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($logic_not)){
			ಗಂಟು_2 a = factory.reduce_or(inputs.at(ID(A)));
			ಗಂಟು_2 y = factory.bitwise_not(a);
			ಹಿಂತಿರುಗಿ factory.extend(y, y_ಅಗಲ, ಸುಳ್ಳು);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($reduce_or), ID($reduce_bool))){
			ಗಂಟು_2 a = factory.reduce_or(inputs.at(ID(A)));
			ಹಿಂತಿರುಗಿ factory.extend(a, y_ಅಗಲ, ಸುಳ್ಳು);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($reduce_and)){
			ಗಂಟು_2 a = factory.reduce_and(inputs.at(ID(A)));
			ಹಿಂತಿರುಗಿ factory.extend(a, y_ಅಗಲ, ಸುಳ್ಳು);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($reduce_xor), ID($reduce_xnor))){
			ಗಂಟು_2 a = factory.reduce_xor(inputs.at(ID(A)));
			ಗಂಟು_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			ಹಿಂತಿರುಗಿ factory.extend(y, y_ಅಗಲ, ಸುಳ್ಳು);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($shl) || cellType == ID($sshl)){
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_ಅಗಲ, a_signed);
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಹಿಂತಿರುಗಿ logical_ಸರಿಸು_ಎಡ(a, b);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($shr) || cellType == ID($sshr)){
			ಪೂರ್ಣಾಂಕ ಅಗಲ = max(a_ಅಗಲ, y_ಅಗಲ);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), ಅಗಲ, a_signed);
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಗಂಟು_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_ಸರಿಸು_ಬಲ(a, b) :
				logical_ಸರಿಸು_ಬಲ(a, b);
			ಹಿಂತಿರುಗಿ factory.extend(y, y_ಅಗಲ, a_signed);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($shiftx) || cellType == ID($ಸರಿಸು)){
			ಪೂರ್ಣಾಂಕ ಅಗಲ = max(a_ಅಗಲ, y_ಅಗಲ);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), ಅಗಲ, cellType == ID($ಸರಿಸು) && a_signed);
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಗಂಟು_2 shr = logical_ಸರಿಸು_ಬಲ(a, b);
			ಆದರೆ(b_signed) {
				ಗಂಟು_2 shl = logical_ಸರಿಸು_ಎಡ(a, factory.unary_minus(b));
				ಗಂಟು_2 y = factory.mux(shr, shl, sign(b));
				ಹಿಂತಿರುಗಿ factory.extend(y, y_ಅಗಲ, ಸುಳ್ಳು);
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಹಿಂತಿರುಗಿ factory.extend(shr, y_ಅಗಲ, ಸುಳ್ಳು);
			}
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($mux)){
			ಹಿಂತಿರುಗಿ factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($pmux)){
			ಹಿಂತಿರುಗಿ handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($concat)){
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಹಿಂತಿರುಗಿ factory.concat(a, b);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($ತುಂಡು)){
			ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = parameters.at(ID(ಸ್ಥಾನಾಂತರ_2)).as_int();
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಹಿಂತಿರುಗಿ factory.ತುಂಡು(a, ಸ್ಥಾನಾಂತರ, y_ಅಗಲ);
		}ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = max(a_ಅಗಲ, b_ಅಗಲ);
			ತಾರ್ಕಿಕ is_signed = a_signed && b_signed;
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), ಅಗಲ, is_signed);
			ಗಂಟು_2 b = factory.extend(inputs.at(ID(B)), ಅಗಲ, is_signed);
			ಆದರೆ(is_signed) {
				ಆದರೆ(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					ಗಂಟು_2 abs_y = factory.unsigned_div(ನಿರಪೇಕ್ಷ(a), ನಿರಪೇಕ್ಷ(b));
					ಗಂಟು_2 ಹೊರಹೋಗು_sign = factory.not_equal(sign(a), sign(b));
					ಹಿಂತಿರುಗಿ neg_if(factory.extend(abs_y, y_ಅಗಲ, ಸುಳ್ಳು), ಹೊರಹೋಗು_sign);
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					ಗಂಟು_2 abs_y = factory.unsigned_mod(ನಿರಪೇಕ್ಷ(a), ನಿರಪೇಕ್ಷ(b));
					ಹಿಂತಿರುಗಿ neg_if(factory.extend(abs_y, y_ಅಗಲ, ಸುಳ್ಳು), sign(a));
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					ಗಂಟು_2 b_sign = sign(b);
					ಗಂಟು_2 a1 = neg_if(a, b_sign);
					ಗಂಟು_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					ಗಂಟು_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					ಗಂಟು_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					ಗಂಟು_2 y1 = factory.unsigned_div(a2, b1);
					ಗಂಟು_2 y2 = factory.extend(y1, y_ಅಗಲ, ಸುಳ್ಳು);
					ಹಿಂತಿರುಗಿ factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					ಗಂಟು_2 abs_b = ನಿರಪೇಕ್ಷ(b);
					ಗಂಟು_2 abs_y = factory.unsigned_mod(ನಿರಪೇಕ್ಷ(a), abs_b);
					ಗಂಟು_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					ಗಂಟು_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					ಗಂಟು_2 y = neg_if(y_flipped, sign(b));
					ಹಿಂತಿರುಗಿ factory.extend(y, y_ಅಗಲ, ನಿಜ);
				} ಇಲ್ಲದಿದ್ದರೆ
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ(cellType.ಒಳ(ID($mod), ID($modfloor)))
					ಹಿಂತಿರುಗಿ factory.extend(factory.unsigned_mod(a, b), y_ಅಗಲ, ಸುಳ್ಳು);
				ಇಲ್ಲದಿದ್ದರೆ
					ಹಿಂತಿರುಗಿ factory.extend(factory.unsigned_div(a, b), y_ಅಗಲ, ಸುಳ್ಳು);
			}
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($ಘಾತ)) {
			ಹಿಂತಿರುಗಿ handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_ಅಗಲ, a_signed && b_signed);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cellType == ID($lut)) {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = parameters.at(ID(ಅಗಲ_2)).as_int();
			Const lut_ಕೋಷ್ಟಕ = parameters.at(ID(LUT));
			lut_ಕೋಷ್ಟಕ.extu(1 << ಅಗಲ);
			ಹಿಂತಿರುಗಿ handle_bmux(factory.constant(lut_ಕೋಷ್ಟಕ), inputs.at(ID(A)), 0, 1, ಅಗಲ);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cellType == ID($bwmux)) {
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಗಂಟು_2 s = inputs.at(ID(S));
			ಹಿಂತಿರುಗಿ factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cellType == ID($bweqx)) {
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಹಿಂತಿರುಗಿ factory.bitwise_not(factory.bitwise_xor(a, b));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($bmux)) {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = parameters.at(ID(ಅಗಲ_2)).as_int();
			ಪೂರ್ಣಾಂಕ s_ಅಗಲ = parameters.at(ID(S_ಅಗಲ)).as_int();
			ಹಿಂತಿರುಗಿ handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, ಅಗಲ, s_ಅಗಲ);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($demux)) {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = parameters.at(ID(ಅಗಲ_2)).as_int();
			ಪೂರ್ಣಾಂಕ s_ಅಗಲ = parameters.at(ID(S_ಅಗಲ)).as_int();
			ಪೂರ್ಣಾಂಕ y_ಅಗಲ = ಅಗಲ << s_ಅಗಲ;
			ಪೂರ್ಣಾಂಕ b_ಅಗಲ = ceil_log2(y_ಅಗಲ);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_ಅಗಲ, ಸುಳ್ಳು);
			ಗಂಟು_2 s = factory.extend(inputs.at(ID(S)), b_ಅಗಲ, ಸುಳ್ಳು);
			ಗಂಟು_2 b = factory.mul(s, factory.constant(Const(ಅಗಲ, b_ಅಗಲ)));
			ಹಿಂತಿರುಗಿ factory.logical_ಸರಿಸು_ಎಡ(a, b);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($fa)) {
			ಹಿಂತಿರುಗಿ handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($lcu)) {
			ಹಿಂತಿರುಗಿ handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($alu)) {
			ಹಿಂತಿರುಗಿ handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_ಅಗಲ, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($ಜೀವಂತ), ID($fair), ID($cover))) {
			ಗಂಟು_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			ಸ್ವಯಂಚಾಲಿತ &output = factory.ಕೂಡಿಸು_output(cellName, cellType, ವಿಂಗಡಿಸು_2(1));
			output.ಗಣ_ಮೌಲ್ಯ(a);
			ಹಿಂತಿರುಗಿ {};
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType.ಒಳ(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = parameters.at(ID(ಅಗಲ_2)).as_int();
			ಸ್ವಯಂಚಾಲಿತ &ಒಳಹರಿವು = factory.ಕೂಡಿಸು_ಒಳಹರಿವು(cellName, cellType, ವಿಂಗಡಿಸು_2(ಅಗಲ));
			ಹಿಂತಿರುಗಿ factory.ಮೌಲ್ಯ(ಒಳಹರಿವು);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($initstate)) {
			ಆದರೆ(factory.ir().has_state(ID($initstate), ID($state)))
				ಹಿಂತಿರುಗಿ factory.ಮೌಲ್ಯ(factory.ir().state(ID($initstate)));
			ಇಲ್ಲದಿದ್ದರೆ {
				ಸ್ವಯಂಚಾಲಿತ &state = factory.ಕೂಡಿಸು_state(ID($initstate), ID($state), ವಿಂಗಡಿಸು_2(1));
				state.ಗಣ_initial_ಮೌಲ್ಯ(RTLIL::Const(State::S1, 1));
				state.ಗಣ_ಮುಂದಿನ_ಮೌಲ್ಯ(factory.constant(RTLIL::Const(State::S0, 1)));
				ಹಿಂತಿರುಗಿ factory.ಮೌಲ್ಯ(state);
			}
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} ಇಲ್ಲದಿದ್ದರೆ {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

ವರ್ಗ FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> ಸರತಿ;
	dict<DriveSpec, ಗಂಟು_2> ನಕ್ಷೆ_nodes;
	dict<std::pair<Cell *, IdString>, ಗಂಟು_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	ಸದಿಶ<Mem> memories_ಸದಿಶ;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	ಗಂಟು_2 ಸರತಿಗೆ_ಸೇರಿಸು(DriveSpec ಸ್ಥಿರಾಂಕ &spec)
	{
		ಸ್ವಯಂಚಾಲಿತ it = ನಕ್ಷೆ_nodes.ಕಂಡುಹಿಡಿ(spec);
		ಆದರೆ(it == ನಕ್ಷೆ_nodes.ಕೊನೆ()){
			ಸ್ವಯಂಚಾಲಿತ ಗಂಟು = factory.create_pending(spec.ಗಾತ್ರ_2());
			ನಕ್ಷೆ_nodes.ಸೇರಿಸು({spec, ಗಂಟು});
			ಸರತಿ.emplace_ಹಿಂಭಾಗ(spec);
			ಹಿಂತಿರುಗಿ ಗಂಟು;
		}ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
	}
	ಗಂಟು_2 ಸರತಿಗೆ_ಸೇರಿಸು_cell(Cell *cell, IdString ಪೋರ್ಟ್_name)
	{
		ಸ್ವಯಂಚಾಲಿತ it = cell_outputs.ಕಂಡುಹಿಡಿ({cell, ಪೋರ್ಟ್_name});
		ಆದರೆ(it == cell_outputs.ಕೊನೆ()) {
			ಸರತಿ.emplace_ಹಿಂಭಾಗ(cell);
			std::optional<ಗಂಟು_2> rv;
			ಗಾಗಿ(ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[name, sigspec] : cell->connections())
				ಆದರೆ(driver_map.celltypes.cell_output(cell->type, name)) {
					ಸ್ವಯಂಚಾಲಿತ ಗಂಟು = factory.create_pending(sigspec.ಗಾತ್ರ_2());
					factory.suggest_name(ಗಂಟು, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, ಗಂಟು);
					ಆದರೆ(name == ಪೋರ್ಟ್_name)
						rv = ಗಂಟು;
				}
			ಹಿಂತಿರುಗಿ *rv;
		} ಇಲ್ಲದಿದ್ದರೆ
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
	}
ಸಾರ್ವಜನಿಕ:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.ಕೂಡಿಸು(module);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells()) {
			ಆದರೆ (cell->type.ಒಳ(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($ಜೀವಂತ), ID($fair), ID($cover), ID($check)))
				ಸರತಿ.emplace_ಹಿಂಭಾಗ(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			ಸ್ವಯಂಚಾಲಿತ *wire = module->wire(*riter);
			ಆದರೆ (wire && wire->ಪೋರ್ಟ್_ಒಳಹರಿವು) {
				factory.ಕೂಡಿಸು_ಒಳಹರಿವು(wire->name, ID($ಒಳಹರಿವು), ವಿಂಗಡಿಸು_2(wire->ಅಗಲ));
			}
			ಆದರೆ (wire && wire->ಪೋರ್ಟ್_output) {
				ಸ್ವಯಂಚಾಲಿತ &output = factory.ಕೂಡಿಸು_output(wire->name, ID($output), ವಿಂಗಡಿಸು_2(wire->ಅಗಲ));
				output.ಗಣ_ಮೌಲ್ಯ(ಸರತಿಗೆ_ಸೇರಿಸು(DriveChunk(DriveChunkWire(wire, 0, wire->ಅಗಲ))));
			}
		}
		memories_ಸದಿಶ = Mem::get_all_memories(module);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &mem : memories_ಸದಿಶ) {
			ಆದರೆ (mem.cell != ಶೂನ್ಯಸೂಚಕ)
				memories[mem.cell] = &mem;
		}
	}
ಖಾಸಗಿ:
	ಗಂಟು_2 concatenate_read_results(Mem *mem, ಸದಿಶ<ಗಂಟು_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ಸ್ಥಿರಾಂಕ SigSpec &rd_ದತ್ತಾಂಶ = mem->cell->connections().at(ID(RD_ದತ್ತಾಂಶ));
		ಪೂರ್ಣಾಂಕ current = 0;
		ಗಾಗಿ(ಗಾತ್ರ_t i = 0; i < mem->rd_ports.ಗಾತ್ರ_2(); i++) {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = mem->ಅಗಲ << mem->rd_ports[i].wide_log2;
			log_assert (results[i].ಅಗಲ() == ಅಗಲ);
			log_assert (mem->rd_ports[i].ದತ್ತಾಂಶ == rd_ದತ್ತಾಂಶ.ಹೊರತೆಗೆ(current, ಅಗಲ));
			current += ಅಗಲ;
		}
		log_assert (current == rd_ದತ್ತಾಂಶ.ಗಾತ್ರ_2());
		log_assert (!results.ಖಾಲಿಗಳು());
		ಗಂಟು_2 ಗಂಟು = results[0];
		ಗಾಗಿ(ಗಾತ್ರ_t i = 1; i < results.ಗಾತ್ರ_2(); i++)
			ಗಂಟು = factory.concat(ಗಂಟು, results[i]);
		ಹಿಂತಿರುಗಿ ಗಂಟು;
	}
	ಗಂಟು_2 handle_ಸ್ಮರಣೆ(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		ಸದಿಶ<ಗಂಟು_2> read_results;
		ಸ್ವಯಂಚಾಲಿತ &state = factory.ಕೂಡಿಸು_state(mem->cell->name, ID($state), ವಿಂಗಡಿಸು_2(ceil_log2(mem->ಗಾತ್ರ_2), mem->ಅಗಲ));
		state.ಗಣ_initial_ಮೌಲ್ಯ(MemContents(mem));
		ಗಂಟು_2 ಗಂಟು = factory.ಮೌಲ್ಯ(state);
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < mem->wr_ports.ಗಾತ್ರ_2(); i++) {
			ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &wr = mem->wr_ports[i];
			ಆದರೆ (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ಗಂಟು_2 en = ಸರತಿಗೆ_ಸೇರಿಸು(driver_map(DriveSpec(wr.en)));
			ಗಂಟು_2 addr = ಸರತಿಗೆ_ಸೇರಿಸು(driver_map(DriveSpec(wr.addr)));
			ಗಂಟು_2 new_ದತ್ತಾಂಶ = ಸರತಿಗೆ_ಸೇರಿಸು(driver_map(DriveSpec(wr.ದತ್ತಾಂಶ)));
			ಗಂಟು_2 old_ದತ್ತಾಂಶ = factory.ಸ್ಮರಣೆ_read(ಗಂಟು, addr);
			ಗಂಟು_2 wr_ದತ್ತಾಂಶ = simplifier.bitwise_mux(old_ದತ್ತಾಂಶ, new_ದತ್ತಾಂಶ, en);
			ಗಂಟು = factory.ಸ್ಮರಣೆ_write(ಗಂಟು, addr, wr_ದತ್ತಾಂಶ);
		}
		ಆದರೆ (mem->rd_ports.ಖಾಲಿಗಳು())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < mem->rd_ports.ಗಾತ್ರ_2(); i++) {
			ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &rd = mem->rd_ports[i];
			ಆದರೆ (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ಗಂಟು_2 addr = ಸರತಿಗೆ_ಸೇರಿಸು(driver_map(DriveSpec(rd.addr)));
			read_results.ತಳ್ಳು_ಹಿಂಭಾಗ(factory.ಸ್ಮರಣೆ_read(ಗಂಟು, addr));
		}
		state.ಗಣ_ಮುಂದಿನ_ಮೌಲ್ಯ(ಗಂಟು);
		ಹಿಂತಿರುಗಿ concatenate_read_results(mem, read_results);
	}
	ಶೂನ್ಯ process_cell(Cell *cell)
	{
		ಆದರೆ (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, ಶೂನ್ಯಸೂಚಕ);
			ಆದರೆ (mem == ಶೂನ್ಯಸೂಚಕ) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			ಗಂಟು_2 ಗಂಟು = handle_ಸ್ಮರಣೆ(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_ದತ್ತಾಂಶ)}), ಗಂಟು);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->is_builtin_ff()) {
			Ff_ದತ್ತಾಂಶ ff(&ff_initvals, cell);
			ಆದರೆ (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			ಸ್ವಯಂಚಾಲಿತ &state = factory.ಕೂಡಿಸು_state(ff.name, ID($state), ವಿಂಗಡಿಸು_2(ff.ಅಗಲ));
			ಗಂಟು_2 q_ಮೌಲ್ಯ = factory.ಮೌಲ್ಯ(state);
			factory.suggest_name(q_ಮೌಲ್ಯ, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_ಮೌಲ್ಯ);
			state.ಗಣ_ಮುಂದಿನ_ಮೌಲ್ಯ(ಸರತಿಗೆ_ಸೇರಿಸು(ff.sig_d));
			state.ಗಣ_initial_ಮೌಲ್ಯ(ff.val_init);
		} ಇಲ್ಲದಿದ್ದರೆ {
			dict<IdString, ಗಂಟು_2> connections;
			IdString output_name; // for the single output case
			ಪೂರ್ಣಾಂಕ n_outputs = 0;
			ಗಾಗಿ(ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &[name, sigspec] : cell->connections()) {
				ಆದರೆ(driver_map.celltypes.cell_ಒಳಹರಿವು(cell->type, name) && sigspec.ಗಾತ್ರ_2() > 0)
					connections.ಸೇರಿಸು({ name, ಸರತಿಗೆ_ಸೇರಿಸು(Drive_Chunk_ಪೋರ್ಟ್(cell, {name, sigspec})) });
				ಆದರೆ(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, ಗಂಟು_2>, ಗಂಟು_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			ಆದರೆ(ಸ್ವಯಂಚಾಲಿತ *nodep = std::get_if<ಗಂಟು_2>(&outputs); nodep != ಶೂನ್ಯಸೂಚಕ) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಗಾಗಿ(ಸ್ವಯಂಚಾಲಿತ [name, ಗಂಟು] : std::get<dict<IdString, ಗಂಟು_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), ಗಂಟು);
			}
		}
	}
	ಶೂನ್ಯ undriven(ಸ್ಥಿರಾಂಕ std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	ಶೂನ್ಯ check_undriven(DriveSpec ಸ್ಥಿರಾಂಕ& spec, std::string ಸ್ಥಿರಾಂಕ& name) {
		ಗಾಗಿ(ಸ್ವಯಂಚಾಲಿತ ಸ್ಥಿರಾಂಕ &chunk : spec.chunks())
			ಆದರೆ(chunk.is_none())
				undriven(name);
	}
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ process_ಸರತಿ()
	{
		ಗಾಗಿ (; !ಸರತಿ.ಖಾಲಿಗಳು(); ಸರತಿ.ತೆಗೆ_front()) {
			ಆದರೆ(ಸ್ವಯಂಚಾಲಿತ p = std::get_if<Cell *>(&ಸರತಿ.front()); p != ಶೂನ್ಯಸೂಚಕ) {
				process_cell(*p);
				ಮುಂದುವರಿ;
			}

			DriveSpec spec = std::get<DriveSpec>(ಸರತಿ.front());
			ಗಂಟು_2 pending = ನಕ್ಷೆ_nodes.at(spec);

			ಆದರೆ (spec.chunks().ಗಾತ್ರ_2() > 1) {
				ಸ್ವಯಂಚಾಲಿತ chunks = spec.chunks();
				ಗಂಟು_2 ಗಂಟು = ಸರತಿಗೆ_ಸೇರಿಸು(chunks[0]);
				ಗಾಗಿ(ಗಾತ್ರ_t i = 1; i < chunks.ಗಾತ್ರ_2(); i++)
					ಗಂಟು = factory.concat(ಗಂಟು, ಸರತಿಗೆ_ಸೇರಿಸು(chunks[i]));
				factory.update_pending(pending, ಗಂಟು);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (spec.chunks().ಗಾತ್ರ_2() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				ಆದರೆ (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					ಆದರೆ (wire_chunk.is_ಸಂಪೂರ್ಣ()) {
						ಆದರೆ (wire_chunk.wire->ಪೋರ್ಟ್_ಒಳಹರಿವು) {
							ಗಂಟು_2 ಗಂಟು = factory.ಮೌಲ್ಯ(factory.ir().ಒಳಹರಿವು(wire_chunk.wire->name));
							factory.suggest_name(ಗಂಟು, wire_chunk.wire->name);
							factory.update_pending(pending, ಗಂಟು);
						} ಇಲ್ಲದಿದ್ದರೆ {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							ಗಂಟು_2 ಗಂಟು = ಸರತಿಗೆ_ಸೇರಿಸು(driver);
							factory.suggest_name(ಗಂಟು, wire_chunk.wire->name);
							factory.update_pending(pending, ಗಂಟು);
						}
					} ಇಲ್ಲದಿದ್ದರೆ {
						DriveChunkWire ಸಂಪೂರ್ಣ_wire(wire_chunk.wire, 0, wire_chunk.wire->ಅಗಲ);
						ಗಂಟು_2 ಗಂಟು = factory.ತುಂಡು(ಸರತಿಗೆ_ಸೇರಿಸು(ಸಂಪೂರ್ಣ_wire), wire_chunk.ಸ್ಥಾನಾಂತರ, wire_chunk.ಅಗಲ);
						factory.update_pending(pending, ಗಂಟು);
					}
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (chunk.is_ಪೋರ್ಟ್()) {
					Drive_Chunk_ಪೋರ್ಟ್ ಪೋರ್ಟ್_chunk = chunk.ಪೋರ್ಟ್();
					ಆದರೆ (ಪೋರ್ಟ್_chunk.is_ಸಂಪೂರ್ಣ()) {
						ಆದರೆ (driver_map.celltypes.cell_output(ಪೋರ್ಟ್_chunk.cell->type, ಪೋರ್ಟ್_chunk.ಪೋರ್ಟ್)) {
							ಗಂಟು_2 ಗಂಟು = ಸರತಿಗೆ_ಸೇರಿಸು_cell(ಪೋರ್ಟ್_chunk.cell, ಪೋರ್ಟ್_chunk.ಪೋರ್ಟ್);
							factory.update_pending(pending, ಗಂಟು);
						} ಇಲ್ಲದಿದ್ದರೆ {
							DriveSpec driver = driver_map(DriveSpec(ಪೋರ್ಟ್_chunk));
							check_undriven(driver, ಪೋರ್ಟ್_chunk.cell->name.unescape() + " port " + PooledName(ಪೋರ್ಟ್_chunk.cell->module, ಪೋರ್ಟ್_chunk.ಪೋರ್ಟ್).unescape());
							factory.update_pending(pending, ಸರತಿಗೆ_ಸೇರಿಸು(driver));
						}
					} ಇಲ್ಲದಿದ್ದರೆ {
						Drive_Chunk_ಪೋರ್ಟ್ ಸಂಪೂರ್ಣ_ಪೋರ್ಟ್(ಪೋರ್ಟ್_chunk.cell, ಪೋರ್ಟ್_chunk.ಪೋರ್ಟ್, 0, Get_ಗಾತ್ರ(ಪೋರ್ಟ್_chunk.cell->connections().at(ಪೋರ್ಟ್_chunk.ಪೋರ್ಟ್)));
						ಗಂಟು_2 ಗಂಟು = factory.ತುಂಡು(ಸರತಿಗೆ_ಸೇರಿಸು(ಸಂಪೂರ್ಣ_ಪೋರ್ಟ್), ಪೋರ್ಟ್_chunk.ಸ್ಥಾನಾಂತರ, ಪೋರ್ಟ್_chunk.ಅಗಲ);
						factory.update_pending(pending, ಗಂಟು);
					}
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (chunk.is_constant()) {
					ಗಂಟು_2 ಗಂಟು = factory.constant(chunk.constant());
					factory.suggest_name(ಗಂಟು, "$const" + std::ಗಮ್ಯ_string(chunk.ಗಾತ್ರ_2()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, ಗಂಟು);
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (chunk.is_ಅಪವರ್ತ್ಯ()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (chunk.is_none()) {
					undriven(log_signal(chunk));
				} ಇಲ್ಲದಿದ್ದರೆ {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				log_abort();
			}
		}
	}
};

IR IR::from_module(Module *module) {
	IR ir;
    ir.design = module->design;
    ಸ್ವಯಂಚಾಲಿತ factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_ಸರತಿ();
    ir.topological_ವಿಂಗಡಿಸು();
    ir.forward_buf();
    ಹಿಂತಿರುಗಿ ir;
}

ಶೂನ್ಯ IR::topological_ವಿಂಗಡಿಸು() {
    ನಕ್ಷೆ::SccAdaptor compute_ನಕ್ಷೆ_scc(ನಕ್ಷೆ_4);
    ತಾರ್ಕಿಕ scc = ಸುಳ್ಳು;
    std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> perm;
    TopoSortedSccs toposort(compute_ನಕ್ಷೆ_scc, [&](ಪೂರ್ಣಾಂಕ *begin, ಪೂರ್ಣಾಂಕ *ಕೊನೆ) {
        perm.ಸೇರಿಸು(perm.ಕೊನೆ(), begin, ಕೊನೆ);
        ಆದರೆ (ಕೊನೆ > begin + 1)
        {
            log_warning("Combinational loop:\n");
            ಗಾಗಿ (ಪೂರ್ಣಾಂಕ *i = begin; i != ಕೊನೆ; ++i) {
				ಗಂಟು_2 ಗಂಟು(ನಕ್ಷೆ_4[*i], design);
                ಲಘುಗಣಕ("- %s = %s\n", ಗಂಟು.name().unescape(), ಗಂಟು.ಗಮ್ಯ_string());
			}
            ಲಘುಗಣಕ("\n");
            scc = ನಿಜ;
        }
    });
	ಗಾಗಿ(ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, state]: ಸ್ಥಿತಿಗಳು_2)
		ಆದರೆ(state.has_ಮುಂದಿನ_ಮೌಲ್ಯ())
			toposort.process(state.ಮುಂದಿನ_ಮೌಲ್ಯ().id());
	ಗಾಗಿ(ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &[name, output]: _outputs)
		ಆದರೆ(output.has_ಮೌಲ್ಯ())
			toposort.process(output.ಮೌಲ್ಯ().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    ನಕ್ಷೆ_4.ಕ್ರಮಬದಲಾವಣೆ(perm);
    ಆದರೆ(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ಸ್ಥಿರ IdString ವಿಲೀನ_name(IdString a, IdString b) {
	ಆದರೆ(!a.is_ಬಹಿರಂಗ() && b.is_ಬಹಿರಂಗ())
		ಹಿಂತಿರುಗಿ b;
	ಇಲ್ಲದಿದ್ದರೆ
		ಹಿಂತಿರುಗಿ a;
}

ಶೂನ್ಯ IR::forward_buf() {
    std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> perm, alias;
    perm.clear();

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ನಕ್ಷೆ_4.ಗಾತ್ರ_2(); ++i)
    {
        ಸ್ವಯಂಚಾಲಿತ ಗಂಟು = ನಕ್ಷೆ_4[i];
        ಆದರೆ (ಗಂಟು.function().fn() == Fn::buf && ಗಂಟು.arg(0).ಸೂಚ್ಯಂಕ() < i)
        {
            ಪೂರ್ಣಾಂಕ ಗುರಿ_ಸೂಚ್ಯಂಕ = alias[ಗಂಟು.arg(0).ಸೂಚ್ಯಂಕ()];
            ಸ್ವಯಂಚಾಲಿತ ಗುರಿ_ಗಂಟು = ನಕ್ಷೆ_4[perm[ಗುರಿ_ಸೂಚ್ಯಂಕ]];
			ಆದರೆ(ಗಂಟು.has_sparse_attr()) {
				ಆದರೆ(ಗುರಿ_ಗಂಟು.has_sparse_attr()) {
					IdString id = ವಿಲೀನ_name(ಗಂಟು.sparse_attr(), ಗುರಿ_ಗಂಟು.sparse_attr());
					ಗುರಿ_ಗಂಟು.sparse_attr() = id;
				} ಇಲ್ಲದಿದ್ದರೆ {
					IdString id = ಗಂಟು.sparse_attr();
					ಗುರಿ_ಗಂಟು.sparse_attr() = id;
				}
			}
            alias.ತಳ್ಳು_ಹಿಂಭಾಗ(ಗುರಿ_ಸೂಚ್ಯಂಕ);
        }
        ಇಲ್ಲದಿದ್ದರೆ
        {
            alias.ತಳ್ಳು_ಹಿಂಭಾಗ(Get_ಗಾತ್ರ(perm));
            perm.ತಳ್ಳು_ಹಿಂಭಾಗ(i);
        }
    }
    ನಕ್ಷೆ_4.ಕ್ರಮಬದಲಾವಣೆ(perm, alias);
}

// Quoting routine to make error messages nicer
ಸ್ಥಿರ std::string quote_fmt(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt)
{
	std::string r;
	ಗಾಗಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = fmt; *p != 0; p++) {
		ಆಯ್ಕೆ(*p) {
		ಸ್ಥಿತಿ '\n': r += "\\n"; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '\t': r += "\\t"; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '"': r += "\\\""; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '\\': r += "\\\\"; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ: r += *p; ನಿಲ್ಲಿಸು;
		}
	}
	ಹಿಂತಿರುಗಿ r;
}

ಶೂನ್ಯ Writer::print_impl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ಸದಿಶ<std::function<ಶೂನ್ಯ()>> &fns)
{
	ಗಾತ್ರ_t ಮುಂದಿನ_ಸೂಚ್ಯಂಕ = 0;
	ಗಾಗಿ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = fmt; *p != 0; p++)
		ಆಯ್ಕೆ(*p) {
		ಸ್ಥಿತಿ '{':
			ಆದರೆ(*++p == '{') {
				*os << '{';
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಅಕ್ಷರ *pe;
				ಗಾತ್ರ_t ಸೂಚ್ಯಂಕ = strtoul(p, &pe, 10);
				ಆದರೆ(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				ಆದರೆ(p == pe)
					ಸೂಚ್ಯಂಕ = ಮುಂದಿನ_ಸೂಚ್ಯಂಕ;
				ಇಲ್ಲದಿದ್ದರೆ
					p = pe;
				ಆದರೆ(ಸೂಚ್ಯಂಕ >= fns.ಗಾತ್ರ_2())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", ಸೂಚ್ಯಂಕ, fns.ಗಾತ್ರ_2(), quote_fmt(fmt));
				fns[ಸೂಚ್ಯಂಕ]();
				ಮುಂದಿನ_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ + 1;
			}
			ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '}':
			p++;
			ಆದರೆ(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_ಕೊನೆ
