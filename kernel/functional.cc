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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/functional.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/topo_scc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ff.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ffinit.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <deque>

YOSYS_NAMESPACE_BEGIN
ਨਾਮਾਕਾਸ਼ Functional {

ਅਚਲ ਅੱਖਰ *fn_ਮੰਜ਼ਿਲ_string(Fn fn) {
	ਚੋਣ(fn) {
	ਹਾਲਤ Fn::invalid: ਵਾਪਸ "invalid";
	ਹਾਲਤ Fn::buf: ਵਾਪਸ "buf";
	ਹਾਲਤ Fn::ਟੁਕੜਾ: ਵਾਪਸ "slice";
	ਹਾਲਤ Fn::zero_extend: ਵਾਪਸ "zero_extend";
	ਹਾਲਤ Fn::sign_extend: ਵਾਪਸ "sign_extend";
	ਹਾਲਤ Fn::concat: ਵਾਪਸ "concat";
	ਹਾਲਤ Fn::ਜੋੜੋ: ਵਾਪਸ "add";
	ਹਾਲਤ Fn::sub: ਵਾਪਸ "sub";
	ਹਾਲਤ Fn::mul: ਵਾਪਸ "mul";
	ਹਾਲਤ Fn::unsigned_div: ਵਾਪਸ "unsigned_div";
	ਹਾਲਤ Fn::unsigned_mod: ਵਾਪਸ "unsigned_mod";
	ਹਾਲਤ Fn::bitwise_and: ਵਾਪਸ "bitwise_and";
	ਹਾਲਤ Fn::bitwise_or: ਵਾਪਸ "bitwise_or";
	ਹਾਲਤ Fn::bitwise_xor: ਵਾਪਸ "bitwise_xor";
	ਹਾਲਤ Fn::bitwise_not: ਵਾਪਸ "bitwise_not";
	ਹਾਲਤ Fn::reduce_and: ਵਾਪਸ "reduce_and";
	ਹਾਲਤ Fn::reduce_or: ਵਾਪਸ "reduce_or";
	ਹਾਲਤ Fn::reduce_xor: ਵਾਪਸ "reduce_xor";
	ਹਾਲਤ Fn::unary_minus: ਵਾਪਸ "unary_minus";
	ਹਾਲਤ Fn::equal: ਵਾਪਸ "equal";
	ਹਾਲਤ Fn::not_equal: ਵਾਪਸ "not_equal";
	ਹਾਲਤ Fn::signed_greater_than: ਵਾਪਸ "signed_greater_than";
	ਹਾਲਤ Fn::signed_greater_equal: ਵਾਪਸ "signed_greater_equal";
	ਹਾਲਤ Fn::unsigned_greater_than: ਵਾਪਸ "unsigned_greater_than";
	ਹਾਲਤ Fn::unsigned_greater_equal: ਵਾਪਸ "unsigned_greater_equal";
	ਹਾਲਤ Fn::logical_ਖਿਸਕਾਓ_ਖੱਬਾ: ਵਾਪਸ "logical_shift_left";
	ਹਾਲਤ Fn::logical_ਖਿਸਕਾਓ_ਸੱਜਾ: ਵਾਪਸ "logical_shift_right";
	ਹਾਲਤ Fn::arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ: ਵਾਪਸ "arithmetic_shift_right";
	ਹਾਲਤ Fn::mux: ਵਾਪਸ "mux";
	ਹਾਲਤ Fn::constant: ਵਾਪਸ "constant";
	ਹਾਲਤ Fn::ਨਿਵੇਸ਼: ਵਾਪਸ "input";
	ਹਾਲਤ Fn::state: ਵਾਪਸ "state";
	ਹਾਲਤ Fn::ਸਮ੍ਰਿਤੀ_read: ਵਾਪਸ "memory_read";
	ਹਾਲਤ Fn::ਸਮ੍ਰਿਤੀ_write: ਵਾਪਸ "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (ਪੂਰਨ)fn);
}

ਸਦਿਸ਼<IRInput ਅਚਲ*> IR::inputs(IdString ਕਿਸਮ) ਅਚਲ {
	ਸਦਿਸ਼<IRInput ਅਚਲ*> ret;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, ਨਿਵੇਸ਼] : _inputs)
		ਜੇ(ਨਿਵੇਸ਼.ਕਿਸਮ == ਕਿਸਮ)
			ret.ਧੱਕੋ_ਪਿੱਠ(&ਨਿਵੇਸ਼);
	ਵਾਪਸ ret;
}

ਸਦਿਸ਼<IROutput ਅਚਲ*> IR::outputs(IdString ਕਿਸਮ) ਅਚਲ {
	ਸਦਿਸ਼<IROutput ਅਚਲ*> ret;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, output] : _outputs)
		ਜੇ(output.ਕਿਸਮ == ਕਿਸਮ)
			ret.ਧੱਕੋ_ਪਿੱਠ(&output);
	ਵਾਪਸ ret;
}

ਸਦਿਸ਼<IRState ਅਚਲ*> IR::ਹਾਲਤਾਂ(IdString ਕਿਸਮ) ਅਚਲ {
	ਸਦਿਸ਼<IRState ਅਚਲ*> ret;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, state] : ਹਾਲਤਾਂ_2)
		ਜੇ(state.ਕਿਸਮ == ਕਿਸਮ)
			ret.ਧੱਕੋ_ਪਿੱਠ(&state);
	ਵਾਪਸ ret;
}

ਸਦਿਸ਼<IRInput ਅਚਲ*> IR::all_inputs() ਅਚਲ {
	ਸਦਿਸ਼<IRInput ਅਚਲ*> ret;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, ਨਿਵੇਸ਼] : _inputs)
		ret.ਧੱਕੋ_ਪਿੱਠ(&ਨਿਵੇਸ਼);
	ਵਾਪਸ ret;
}

ਸਦਿਸ਼<IROutput ਅਚਲ*> IR::all_outputs() ਅਚਲ {
	ਸਦਿਸ਼<IROutput ਅਚਲ*> ret;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, output] : _outputs)
		ret.ਧੱਕੋ_ਪਿੱਠ(&output);
	ਵਾਪਸ ret;
}

ਸਦਿਸ਼<IRState ਅਚਲ*> IR::all_ਹਾਲਤਾਂ() ਅਚਲ {
	ਸਦਿਸ਼<IRState ਅਚਲ*> ret;
	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, state] : ਹਾਲਤਾਂ_2)
		ret.ਧੱਕੋ_ਪਿੱਠ(&state);
	ਵਾਪਸ ret;
}

ਬਣਤਰ PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(ਗੰਢ_2)> np;
	PrintVisitor(std::function<std::string(ਗੰਢ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string ਟੁਕੜਾ(ਗੰਢ_2, ਗੰਢ_2 a, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ) override { ਵਾਪਸ "slice(" + np(a) + ", " + std::ਮੰਜ਼ਿਲ_string(ਵਿੱਥ) + ", " + std::ਮੰਜ਼ਿਲ_string(ਬਾਹਰਲਾ_ਚੌੜਾਈ) + ")"; }
	std::string zero_extend(ਗੰਢ_2, ਗੰਢ_2 a, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ) override { ਵਾਪਸ "zero_extend(" + np(a) + ", " + std::ਮੰਜ਼ਿਲ_string(ਬਾਹਰਲਾ_ਚੌੜਾਈ) + ")"; }
	std::string sign_extend(ਗੰਢ_2, ਗੰਢ_2 a, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ) override { ਵਾਪਸ "sign_extend(" + np(a) + ", " + std::ਮੰਜ਼ਿਲ_string(ਬਾਹਰਲਾ_ਚੌੜਾਈ) + ")"; }
	std::string constant(ਗੰਢ_2, RTLIL::Const ਅਚਲ& ਮੁੱਲ) override { ਵਾਪਸ "constant(" + ਮੁੱਲ.as_string() + ")"; }
	std::string ਨਿਵੇਸ਼(ਗੰਢ_2 self, IdString name, IdString ਕਿਸਮ) override { ਵਾਪਸ "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(ਕਿਸਮ) + ")"; }
	std::string state(ਗੰਢ_2 self, IdString name, IdString ਕਿਸਮ) override { ਵਾਪਸ "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(ਕਿਸਮ) + ")"; }
	std::string default_handler(ਗੰਢ_2 self) override {
		std::string ret = fn_ਮੰਜ਼ਿਲ_string(self.fn());
		ret += "(";
		ਲਈ(ਮਾਪ_t i = 0; i < self.arg_ਗਿਣਤੀ(); i++) {
			ਜੇ(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		ਵਾਪਸ ret;
	}
};

std::string ਗੰਢ_2::ਮੰਜ਼ਿਲ_string()
{
	ਵਾਪਸ ਮੰਜ਼ਿਲ_string([](ਗੰਢ_2 n) { ਵਾਪਸ n.name().unescape(); });
}

std::string ਗੰਢ_2::ਮੰਜ਼ਿਲ_string(std::function<std::string(ਗੰਢ_2)> np)
{
	ਵਾਪਸ ਦੇਖੋ(PrintVisitor(np));
}

ਵਰਗ CellSimplifier {
	Factory &factory;
	ਗੰਢ_2 sign(ਗੰਢ_2 a) {
		ਵਾਪਸ factory.ਟੁਕੜਾ(a, a.ਚੌੜਾਈ() - 1, 1);
	}
	ਗੰਢ_2 neg_if(ਗੰਢ_2 a, ਗੰਢ_2 s) {
		ਵਾਪਸ factory.mux(a, factory.unary_minus(a), s);
	}
	ਗੰਢ_2 ਨਿਰਪੇਖ(ਗੰਢ_2 a) {
		ਵਾਪਸ neg_if(a, sign(a));
	}
	ਗੰਢ_2 handle_ਖਿਸਕਾਓ(ਗੰਢ_2 a, ਗੰਢ_2 b, ਤਾਰਕਿਕ is_ਸੱਜਾ, ਤਾਰਕਿਕ is_signed) {
		// to prevent new_width == 0, we handle this case separately
		ਜੇ(a.ਚੌੜਾਈ() == 1) {
			ਜੇ(!is_signed)
				ਵਾਪਸ factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ a;
		}
		ਪੂਰਨ new_ਚੌੜਾਈ = ceil_log2(a.ਚੌੜਾਈ());
		ਗੰਢ_2 b_truncated = factory.extend(b, new_ਚੌੜਾਈ, ਝੂਠ);
		ਗੰਢ_2 y =
			!is_ਸੱਜਾ ? factory.logical_ਖਿਸਕਾਓ_ਖੱਬਾ(a, b_truncated) :
			!is_signed ? factory.logical_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b_truncated) :
			factory.arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b_truncated);
		ਜੇ(b.ਚੌੜਾਈ() <= new_ਚੌੜਾਈ)
			ਵਾਪਸ y;
		ਗੰਢ_2 ਉਛਾਲ = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.ਚੌੜਾਈ(), b.ਚੌੜਾਈ())));
		ਗੰਢ_2 y_if_ਉਛਾਲ = is_signed ? factory.extend(sign(a), a.ਚੌੜਾਈ(), ਸੱਚ) : factory.constant(RTLIL::Const(State::S0, a.ਚੌੜਾਈ()));
		ਵਾਪਸ factory.mux(y, y_if_ਉਛਾਲ, ਉਛਾਲ);
	}
ਜਨਤਕ:
	ਗੰਢ_2 logical_ਖਿਸਕਾਓ_ਖੱਬਾ(ਗੰਢ_2 a, ਗੰਢ_2 b) { ਵਾਪਸ handle_ਖਿਸਕਾਓ(a, b, ਝੂਠ, ਝੂਠ); }
	ਗੰਢ_2 logical_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 a, ਗੰਢ_2 b) { ਵਾਪਸ handle_ਖਿਸਕਾਓ(a, b, ਸੱਚ, ਝੂਠ); }
	ਗੰਢ_2 arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 a, ਗੰਢ_2 b) { ਵਾਪਸ handle_ਖਿਸਕਾਓ(a, b, ਸੱਚ, ਸੱਚ); }
	ਗੰਢ_2 bitwise_mux(ਗੰਢ_2 a, ਗੰਢ_2 b, ਗੰਢ_2 s) {
		ਗੰਢ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		ਗੰਢ_2 bb = factory.bitwise_and(b, s);
		ਵਾਪਸ factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
ਨਿਜੀ:
	ਗੰਢ_2 handle_pow(ਗੰਢ_2 a0, ਗੰਢ_2 b, ਪੂਰਨ y_ਚੌੜਾਈ, ਤਾਰਕਿਕ is_signed) {
		ਗੰਢ_2 a = factory.extend(a0, y_ਚੌੜਾਈ, is_signed);
		ਗੰਢ_2 r = factory.constant(Const(1, y_ਚੌੜਾਈ));
		ਲਈ(ਪੂਰਨ i = 0; i < b.ਚੌੜਾਈ(); i++) {
			ਗੰਢ_2 b_ਦੋਆਧਾਰੀ_ਅੰਕ = factory.ਟੁਕੜਾ(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_ਦੋਆਧਾਰੀ_ਅੰਕ);
			a = factory.mul(a, a);
		}
		ਜੇ (is_signed) {
			ਗੰਢ_2 a_ge_1 = factory.unsigned_greater_than(ਨਿਰਪੇਖ(a0), factory.constant(Const(1, a0.ਚੌੜਾਈ())));
			ਗੰਢ_2 zero_ਨਤੀਜਾ = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_ਚੌੜਾਈ)), zero_ਨਤੀਜਾ);
		}
		ਵਾਪਸ r;
	}
	ਗੰਢ_2 handle_bmux(ਗੰਢ_2 a, ਗੰਢ_2 s, ਪੂਰਨ a_ਵਿੱਥ, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ sn) {
		ਜੇ(sn < 1)
			ਵਾਪਸ factory.ਟੁਕੜਾ(a, a_ਵਿੱਥ, ਚੌੜਾਈ);
		ਨਹੀਂਤਾਂ {
			ਗੰਢ_2 y0 = handle_bmux(a, s, a_ਵਿੱਥ, ਚੌੜਾਈ, sn - 1);
			ਗੰਢ_2 y1 = handle_bmux(a, s, a_ਵਿੱਥ + (ਚੌੜਾਈ << (sn - 1)), ਚੌੜਾਈ, sn - 1);
			ਵਾਪਸ factory.mux(y0, y1, factory.ਟੁਕੜਾ(s, sn - 1, 1));
		}
	}
	ਗੰਢ_2 handle_pmux(ਗੰਢ_2 a, ਗੰਢ_2 b, ਗੰਢ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.ਚੌੜਾਈ() == a.ਚੌੜਾਈ() * s.ਚੌੜਾਈ());
		ਗੰਢ_2 y = a;
		ਲਈ(ਪੂਰਨ i = 0; i < s.ਚੌੜਾਈ(); i++)
			y = factory.mux(y, factory.ਟੁਕੜਾ(b, a.ਚੌੜਾਈ() * i, a.ਚੌੜਾਈ()), factory.ਟੁਕੜਾ(s, i, 1));
		ਵਾਪਸ y;
	}
	dict<IdString, ਗੰਢ_2> handle_fa(ਗੰਢ_2 a, ਗੰਢ_2 b, ਗੰਢ_2 c) {
		ਗੰਢ_2 t1 = factory.bitwise_xor(a, b);
		ਗੰਢ_2 t2 = factory.bitwise_and(a, b);
		ਗੰਢ_2 t3 = factory.bitwise_and(c, t1);
		ਗੰਢ_2 y = factory.bitwise_xor(c, t1);
		ਗੰਢ_2 x = factory.bitwise_or(t2, t3);
		ਵਾਪਸ {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, ਗੰਢ_2> handle_alu(ਗੰਢ_2 a_ਵਿੱਚ, ਗੰਢ_2 b_ਵਿੱਚ, ਪੂਰਨ y_ਚੌੜਾਈ, ਤਾਰਕਿਕ is_signed, ਗੰਢ_2 ci, ਗੰਢ_2 bi) {
		ਗੰਢ_2 a = factory.extend(a_ਵਿੱਚ, y_ਚੌੜਾਈ, is_signed);
		ਗੰਢ_2 b_uninverted = factory.extend(b_ਵਿੱਚ, y_ਚੌੜਾਈ, is_signed);
		ਗੰਢ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		ਗੰਢ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		ਗੰਢ_2 a_extra = factory.extend(a, y_ਚੌੜਾਈ + 1, ਝੂਠ);
		ਗੰਢ_2 b_extra = factory.extend(b, y_ਚੌੜਾਈ + 1, ਝੂਠ);
		ਗੰਢ_2 y_extra = factory.ਜੋੜੋ(factory.ਜੋੜੋ(a_extra, b_extra), factory.extend(ci, a.ਚੌੜਾਈ() + 1, ਝੂਠ));
		ਗੰਢ_2 y = factory.ਟੁਕੜਾ(y_extra, 0, y_ਚੌੜਾਈ);
		ਗੰਢ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		ਗੰਢ_2 co = factory.ਟੁਕੜਾ(carries, 1, y_ਚੌੜਾਈ);
		ਵਾਪਸ {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	ਗੰਢ_2 handle_lcu(ਗੰਢ_2 p, ਗੰਢ_2 g, ਗੰਢ_2 ci) {
		ਵਾਪਸ handle_alu(g, factory.bitwise_or(p, g), g.ਚੌੜਾਈ(), ਝੂਠ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
ਜਨਤਕ:
	std::variant<dict<IdString, ਗੰਢ_2>, ਗੰਢ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, ਗੰਢ_2> inputs)
	{
		ਪੂਰਨ a_ਚੌੜਾਈ = parameters.at(ID(A_ਚੌੜਾਈ), Const(-1)).as_int();
		ਪੂਰਨ b_ਚੌੜਾਈ = parameters.at(ID(B_ਚੌੜਾਈ), Const(-1)).as_int();
		ਪੂਰਨ y_ਚੌੜਾਈ = parameters.at(ID(Y_ਚੌੜਾਈ), Const(-1)).as_int();
		ਤਾਰਕਿਕ a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		ਤਾਰਕਿਕ b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		ਜੇ(cellType.ਵਿੱਚ(ID($ਜੋੜੋ), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			ਤਾਰਕਿਕ is_signed = a_signed && b_signed;
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), y_ਚੌੜਾਈ, is_signed);
			ਗੰਢ_2 b = factory.extend(inputs.at(ID(B)), y_ਚੌੜਾਈ, is_signed);
			ਜੇ(cellType == ID($ਜੋੜੋ))
				ਵਾਪਸ factory.ਜੋੜੋ(a, b);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($sub))
				ਵਾਪਸ factory.sub(a, b);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($mul))
				ਵਾਪਸ factory.mul(a, b);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($and))
				ਵਾਪਸ factory.bitwise_and(a, b);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($or))
				ਵਾਪਸ factory.bitwise_or(a, b);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($xor))
				ਵਾਪਸ factory.bitwise_xor(a, b);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($xnor))
				ਵਾਪਸ factory.bitwise_not(factory.bitwise_xor(a, b));
			ਨਹੀਂਤਾਂ
				log_abort();
		}ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			ਤਾਰਕਿਕ is_signed = a_signed && b_signed;
			ਪੂਰਨ ਚੌੜਾਈ = max(a_ਚੌੜਾਈ, b_ਚੌੜਾਈ);
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), ਚੌੜਾਈ, is_signed);
			ਗੰਢ_2 b = factory.extend(inputs.at(ID(B)), ਚੌੜਾਈ, is_signed);
			ਜੇ(cellType.ਵਿੱਚ(ID($eq), ID($eqx)))
				ਵਾਪਸ factory.extend(factory.equal(a, b), y_ਚੌੜਾਈ, ਝੂਠ);
			ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($ne), ID($nex)))
				ਵਾਪਸ factory.extend(factory.not_equal(a, b), y_ਚੌੜਾਈ, ਝੂਠ);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($lt))
				ਵਾਪਸ factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_ਚੌੜਾਈ, ਝੂਠ);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($le))
				ਵਾਪਸ factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_ਚੌੜਾਈ, ਝੂਠ);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($gt))
				ਵਾਪਸ factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_ਚੌੜਾਈ, ਝੂਠ);
			ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($ge))
				ਵਾਪਸ factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_ਚੌੜਾਈ, ਝੂਠ);
			ਨਹੀਂਤਾਂ
				log_abort();
		}ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($logic_or), ID($logic_and))){
			ਗੰਢ_2 a = factory.reduce_or(inputs.at(ID(A)));
			ਗੰਢ_2 b = factory.reduce_or(inputs.at(ID(B)));
			ਗੰਢ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			ਵਾਪਸ factory.extend(y, y_ਚੌੜਾਈ, ਝੂਠ);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($not)){
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), y_ਚੌੜਾਈ, a_signed);
			ਵਾਪਸ factory.bitwise_not(a);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($pos)){
			ਵਾਪਸ factory.extend(inputs.at(ID(A)), y_ਚੌੜਾਈ, a_signed);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($barrier)){
			ਵਾਪਸ inputs.at(ID(A));
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($neg)){
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), y_ਚੌੜਾਈ, a_signed);
			ਵਾਪਸ factory.unary_minus(a);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($logic_not)){
			ਗੰਢ_2 a = factory.reduce_or(inputs.at(ID(A)));
			ਗੰਢ_2 y = factory.bitwise_not(a);
			ਵਾਪਸ factory.extend(y, y_ਚੌੜਾਈ, ਝੂਠ);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($reduce_or), ID($reduce_bool))){
			ਗੰਢ_2 a = factory.reduce_or(inputs.at(ID(A)));
			ਵਾਪਸ factory.extend(a, y_ਚੌੜਾਈ, ਝੂਠ);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($reduce_and)){
			ਗੰਢ_2 a = factory.reduce_and(inputs.at(ID(A)));
			ਵਾਪਸ factory.extend(a, y_ਚੌੜਾਈ, ਝੂਠ);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($reduce_xor), ID($reduce_xnor))){
			ਗੰਢ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			ਗੰਢ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			ਵਾਪਸ factory.extend(y, y_ਚੌੜਾਈ, ਝੂਠ);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($shl) || cellType == ID($sshl)){
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), y_ਚੌੜਾਈ, a_signed);
			ਗੰਢ_2 b = inputs.at(ID(B));
			ਵਾਪਸ logical_ਖਿਸਕਾਓ_ਖੱਬਾ(a, b);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($shr) || cellType == ID($sshr)){
			ਪੂਰਨ ਚੌੜਾਈ = max(a_ਚੌੜਾਈ, y_ਚੌੜਾਈ);
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), ਚੌੜਾਈ, a_signed);
			ਗੰਢ_2 b = inputs.at(ID(B));
			ਗੰਢ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b) :
				logical_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b);
			ਵਾਪਸ factory.extend(y, y_ਚੌੜਾਈ, a_signed);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($shiftx) || cellType == ID($ਖਿਸਕਾਓ)){
			ਪੂਰਨ ਚੌੜਾਈ = max(a_ਚੌੜਾਈ, y_ਚੌੜਾਈ);
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), ਚੌੜਾਈ, cellType == ID($ਖਿਸਕਾਓ) && a_signed);
			ਗੰਢ_2 b = inputs.at(ID(B));
			ਗੰਢ_2 shr = logical_ਖਿਸਕਾਓ_ਸੱਜਾ(a, b);
			ਜੇ(b_signed) {
				ਗੰਢ_2 shl = logical_ਖਿਸਕਾਓ_ਖੱਬਾ(a, factory.unary_minus(b));
				ਗੰਢ_2 y = factory.mux(shr, shl, sign(b));
				ਵਾਪਸ factory.extend(y, y_ਚੌੜਾਈ, ਝੂਠ);
			} ਨਹੀਂਤਾਂ {
				ਵਾਪਸ factory.extend(shr, y_ਚੌੜਾਈ, ਝੂਠ);
			}
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($mux)){
			ਵਾਪਸ factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($pmux)){
			ਵਾਪਸ handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($concat)){
			ਗੰਢ_2 a = inputs.at(ID(A));
			ਗੰਢ_2 b = inputs.at(ID(B));
			ਵਾਪਸ factory.concat(a, b);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($ਟੁਕੜਾ)){
			ਪੂਰਨ ਵਿੱਥ = parameters.at(ID(ਵਿੱਥ_2)).as_int();
			ਗੰਢ_2 a = inputs.at(ID(A));
			ਵਾਪਸ factory.ਟੁਕੜਾ(a, ਵਿੱਥ, y_ਚੌੜਾਈ);
		}ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			ਪੂਰਨ ਚੌੜਾਈ = max(a_ਚੌੜਾਈ, b_ਚੌੜਾਈ);
			ਤਾਰਕਿਕ is_signed = a_signed && b_signed;
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), ਚੌੜਾਈ, is_signed);
			ਗੰਢ_2 b = factory.extend(inputs.at(ID(B)), ਚੌੜਾਈ, is_signed);
			ਜੇ(is_signed) {
				ਜੇ(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					ਗੰਢ_2 abs_y = factory.unsigned_div(ਨਿਰਪੇਖ(a), ਨਿਰਪੇਖ(b));
					ਗੰਢ_2 ਬਾਹਰਲਾ_sign = factory.not_equal(sign(a), sign(b));
					ਵਾਪਸ neg_if(factory.extend(abs_y, y_ਚੌੜਾਈ, ਝੂਠ), ਬਾਹਰਲਾ_sign);
				} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					ਗੰਢ_2 abs_y = factory.unsigned_mod(ਨਿਰਪੇਖ(a), ਨਿਰਪੇਖ(b));
					ਵਾਪਸ neg_if(factory.extend(abs_y, y_ਚੌੜਾਈ, ਝੂਠ), sign(a));
				} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					ਗੰਢ_2 b_sign = sign(b);
					ਗੰਢ_2 a1 = neg_if(a, b_sign);
					ਗੰਢ_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					ਗੰਢ_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					ਗੰਢ_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					ਗੰਢ_2 y1 = factory.unsigned_div(a2, b1);
					ਗੰਢ_2 y2 = factory.extend(y1, y_ਚੌੜਾਈ, ਝੂਠ);
					ਵਾਪਸ factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					ਗੰਢ_2 abs_b = ਨਿਰਪੇਖ(b);
					ਗੰਢ_2 abs_y = factory.unsigned_mod(ਨਿਰਪੇਖ(a), abs_b);
					ਗੰਢ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					ਗੰਢ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					ਗੰਢ_2 y = neg_if(y_flipped, sign(b));
					ਵਾਪਸ factory.extend(y, y_ਚੌੜਾਈ, ਸੱਚ);
				} ਨਹੀਂਤਾਂ
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} ਨਹੀਂਤਾਂ {
				ਜੇ(cellType.ਵਿੱਚ(ID($mod), ID($modfloor)))
					ਵਾਪਸ factory.extend(factory.unsigned_mod(a, b), y_ਚੌੜਾਈ, ਝੂਠ);
				ਨਹੀਂਤਾਂ
					ਵਾਪਸ factory.extend(factory.unsigned_div(a, b), y_ਚੌੜਾਈ, ਝੂਠ);
			}
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($ਘਾਤ)) {
			ਵਾਪਸ handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_ਚੌੜਾਈ, a_signed && b_signed);
		} ਨਹੀਂਤਾਂ ਜੇ (cellType == ID($lut)) {
			ਪੂਰਨ ਚੌੜਾਈ = parameters.at(ID(ਚੌੜਾਈ_2)).as_int();
			Const lut_ਸਾਰਣੀ = parameters.at(ID(LUT));
			lut_ਸਾਰਣੀ.extu(1 << ਚੌੜਾਈ);
			ਵਾਪਸ handle_bmux(factory.constant(lut_ਸਾਰਣੀ), inputs.at(ID(A)), 0, 1, ਚੌੜਾਈ);
		} ਨਹੀਂਤਾਂ ਜੇ (cellType == ID($bwmux)) {
			ਗੰਢ_2 a = inputs.at(ID(A));
			ਗੰਢ_2 b = inputs.at(ID(B));
			ਗੰਢ_2 s = inputs.at(ID(S));
			ਵਾਪਸ factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} ਨਹੀਂਤਾਂ ਜੇ (cellType == ID($bweqx)) {
			ਗੰਢ_2 a = inputs.at(ID(A));
			ਗੰਢ_2 b = inputs.at(ID(B));
			ਵਾਪਸ factory.bitwise_not(factory.bitwise_xor(a, b));
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($bmux)) {
			ਪੂਰਨ ਚੌੜਾਈ = parameters.at(ID(ਚੌੜਾਈ_2)).as_int();
			ਪੂਰਨ s_ਚੌੜਾਈ = parameters.at(ID(S_ਚੌੜਾਈ)).as_int();
			ਵਾਪਸ handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, ਚੌੜਾਈ, s_ਚੌੜਾਈ);
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($demux)) {
			ਪੂਰਨ ਚੌੜਾਈ = parameters.at(ID(ਚੌੜਾਈ_2)).as_int();
			ਪੂਰਨ s_ਚੌੜਾਈ = parameters.at(ID(S_ਚੌੜਾਈ)).as_int();
			ਪੂਰਨ y_ਚੌੜਾਈ = ਚੌੜਾਈ << s_ਚੌੜਾਈ;
			ਪੂਰਨ b_ਚੌੜਾਈ = ceil_log2(y_ਚੌੜਾਈ);
			ਗੰਢ_2 a = factory.extend(inputs.at(ID(A)), y_ਚੌੜਾਈ, ਝੂਠ);
			ਗੰਢ_2 s = factory.extend(inputs.at(ID(S)), b_ਚੌੜਾਈ, ਝੂਠ);
			ਗੰਢ_2 b = factory.mul(s, factory.constant(Const(ਚੌੜਾਈ, b_ਚੌੜਾਈ)));
			ਵਾਪਸ factory.logical_ਖਿਸਕਾਓ_ਖੱਬਾ(a, b);
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($fa)) {
			ਵਾਪਸ handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($lcu)) {
			ਵਾਪਸ handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($alu)) {
			ਵਾਪਸ handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_ਚੌੜਾਈ, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($ਪ੍ਰਤਿਜ੍ਞਾ), ID($assume), ID($ਜੀਵਤ), ID($fair), ID($cover))) {
			ਗੰਢ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			ਸਵੈਚਾਲਿਤ &output = factory.ਜੋੜੋ_output(cellName, cellType, ਛਾਂਟੋ_2(1));
			output.ਸੈੱਟ_ਮੁੱਲ(a);
			ਵਾਪਸ {};
		} ਨਹੀਂਤਾਂ ਜੇ(cellType.ਵਿੱਚ(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			ਪੂਰਨ ਚੌੜਾਈ = parameters.at(ID(ਚੌੜਾਈ_2)).as_int();
			ਸਵੈਚਾਲਿਤ &ਨਿਵੇਸ਼ = factory.ਜੋੜੋ_ਨਿਵੇਸ਼(cellName, cellType, ਛਾਂਟੋ_2(ਚੌੜਾਈ));
			ਵਾਪਸ factory.ਮੁੱਲ(ਨਿਵੇਸ਼);
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($initstate)) {
			ਜੇ(factory.ir().has_state(ID($initstate), ID($state)))
				ਵਾਪਸ factory.ਮੁੱਲ(factory.ir().state(ID($initstate)));
			ਨਹੀਂਤਾਂ {
				ਸਵੈਚਾਲਿਤ &state = factory.ਜੋੜੋ_state(ID($initstate), ID($state), ਛਾਂਟੋ_2(1));
				state.ਸੈੱਟ_initial_ਮੁੱਲ(RTLIL::Const(State::S1, 1));
				state.ਸੈੱਟ_ਅਗਲਾ_ਮੁੱਲ(factory.constant(RTLIL::Const(State::S0, 1)));
				ਵਾਪਸ factory.ਮੁੱਲ(state);
			}
		} ਨਹੀਂਤਾਂ ਜੇ(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} ਨਹੀਂਤਾਂ {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

ਵਰਗ FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> ਕਤਾਰ;
	dict<DriveSpec, ਗੰਢ_2> ਗ੍ਰਾਫ_nodes;
	dict<std::pair<Cell *, IdString>, ਗੰਢ_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	ਸਦਿਸ਼<Mem> memories_ਸਦਿਸ਼;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	ਗੰਢ_2 ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(DriveSpec ਅਚਲ &spec)
	{
		ਸਵੈਚਾਲਿਤ it = ਗ੍ਰਾਫ_nodes.ਲੱਭੋ(spec);
		ਜੇ(it == ਗ੍ਰਾਫ_nodes.ਸਿਰਾ()){
			ਸਵੈਚਾਲਿਤ ਗੰਢ = factory.create_pending(spec.ਮਾਪ());
			ਗ੍ਰਾਫ_nodes.ਪਾਓ({spec, ਗੰਢ});
			ਕਤਾਰ.emplace_ਪਿੱਠ(spec);
			ਵਾਪਸ ਗੰਢ;
		}ਨਹੀਂਤਾਂ
			ਵਾਪਸ it->ਦੂਜਾ;
	}
	ਗੰਢ_2 ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_cell(Cell *cell, IdString ਪੋਰਟ_name)
	{
		ਸਵੈਚਾਲਿਤ it = cell_outputs.ਲੱਭੋ({cell, ਪੋਰਟ_name});
		ਜੇ(it == cell_outputs.ਸਿਰਾ()) {
			ਕਤਾਰ.emplace_ਪਿੱਠ(cell);
			std::optional<ਗੰਢ_2> rv;
			ਲਈ(ਸਵੈਚਾਲਿਤ ਅਚਲ &[name, sigspec] : cell->connections())
				ਜੇ(driver_map.celltypes.cell_output(cell->type, name)) {
					ਸਵੈਚਾਲਿਤ ਗੰਢ = factory.create_pending(sigspec.ਮਾਪ());
					factory.suggest_name(ਗੰਢ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, ਗੰਢ);
					ਜੇ(name == ਪੋਰਟ_name)
						rv = ਗੰਢ;
				}
			ਵਾਪਸ *rv;
		} ਨਹੀਂਤਾਂ
			ਵਾਪਸ it->ਦੂਜਾ;
	}
ਜਨਤਕ:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.ਜੋੜੋ(module);
		ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells()) {
			ਜੇ (cell->type.ਵਿੱਚ(ID($ਪ੍ਰਤਿਜ੍ਞਾ), ID($assume), ID($ਜੀਵਤ), ID($fair), ID($cover), ID($check)))
				ਕਤਾਰ.emplace_ਪਿੱਠ(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		ਲਈ (ਸਵੈਚਾਲਿਤ riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			ਸਵੈਚਾਲਿਤ *wire = module->wire(*riter);
			ਜੇ (wire && wire->ਪੋਰਟ_ਨਿਵੇਸ਼) {
				factory.ਜੋੜੋ_ਨਿਵੇਸ਼(wire->name, ID($ਨਿਵੇਸ਼), ਛਾਂਟੋ_2(wire->ਚੌੜਾਈ));
			}
			ਜੇ (wire && wire->ਪੋਰਟ_output) {
				ਸਵੈਚਾਲਿਤ &output = factory.ਜੋੜੋ_output(wire->name, ID($output), ਛਾਂਟੋ_2(wire->ਚੌੜਾਈ));
				output.ਸੈੱਟ_ਮੁੱਲ(ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(DriveChunk(DriveChunkWire(wire, 0, wire->ਚੌੜਾਈ))));
			}
		}
		memories_ਸਦਿਸ਼ = Mem::get_all_memories(module);
		ਲਈ (ਸਵੈਚਾਲਿਤ &mem : memories_ਸਦਿਸ਼) {
			ਜੇ (mem.cell != ਖਾਲੀ_ਸੂਚਕ)
				memories[mem.cell] = &mem;
		}
	}
ਨਿਜੀ:
	ਗੰਢ_2 concatenate_read_results(Mem *mem, ਸਦਿਸ਼<ਗੰਢ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ਅਚਲ SigSpec &rd_ਅੰਕੜੇ = mem->cell->connections().at(ID(RD_ਅੰਕੜੇ));
		ਪੂਰਨ current = 0;
		ਲਈ(ਮਾਪ_t i = 0; i < mem->rd_ports.ਮਾਪ(); i++) {
			ਪੂਰਨ ਚੌੜਾਈ = mem->ਚੌੜਾਈ << mem->rd_ports[i].wide_log2;
			log_assert (results[i].ਚੌੜਾਈ() == ਚੌੜਾਈ);
			log_assert (mem->rd_ports[i].ਅੰਕੜੇ == rd_ਅੰਕੜੇ.ਕੱਢੋ_2(current, ਚੌੜਾਈ));
			current += ਚੌੜਾਈ;
		}
		log_assert (current == rd_ਅੰਕੜੇ.ਮਾਪ());
		log_assert (!results.ਖਾਲੀ_ਥਾਵਾਂ());
		ਗੰਢ_2 ਗੰਢ = results[0];
		ਲਈ(ਮਾਪ_t i = 1; i < results.ਮਾਪ(); i++)
			ਗੰਢ = factory.concat(ਗੰਢ, results[i]);
		ਵਾਪਸ ਗੰਢ;
	}
	ਗੰਢ_2 handle_ਸਮ੍ਰਿਤੀ(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		ਸਦਿਸ਼<ਗੰਢ_2> read_results;
		ਸਵੈਚਾਲਿਤ &state = factory.ਜੋੜੋ_state(mem->cell->name, ID($state), ਛਾਂਟੋ_2(ceil_log2(mem->ਮਾਪ), mem->ਚੌੜਾਈ));
		state.ਸੈੱਟ_initial_ਮੁੱਲ(MemContents(mem));
		ਗੰਢ_2 ਗੰਢ = factory.ਮੁੱਲ(state);
		ਲਈ (ਮਾਪ_t i = 0; i < mem->wr_ports.ਮਾਪ(); i++) {
			ਅਚਲ ਸਵੈਚਾਲਿਤ &wr = mem->wr_ports[i];
			ਜੇ (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ਗੰਢ_2 en = ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(driver_map(DriveSpec(wr.en)));
			ਗੰਢ_2 addr = ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(driver_map(DriveSpec(wr.addr)));
			ਗੰਢ_2 new_ਅੰਕੜੇ = ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(driver_map(DriveSpec(wr.ਅੰਕੜੇ)));
			ਗੰਢ_2 old_ਅੰਕੜੇ = factory.ਸਮ੍ਰਿਤੀ_read(ਗੰਢ, addr);
			ਗੰਢ_2 wr_ਅੰਕੜੇ = simplifier.bitwise_mux(old_ਅੰਕੜੇ, new_ਅੰਕੜੇ, en);
			ਗੰਢ = factory.ਸਮ੍ਰਿਤੀ_write(ਗੰਢ, addr, wr_ਅੰਕੜੇ);
		}
		ਜੇ (mem->rd_ports.ਖਾਲੀ_ਥਾਵਾਂ())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		ਲਈ (ਮਾਪ_t i = 0; i < mem->rd_ports.ਮਾਪ(); i++) {
			ਅਚਲ ਸਵੈਚਾਲਿਤ &rd = mem->rd_ports[i];
			ਜੇ (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ਗੰਢ_2 addr = ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(driver_map(DriveSpec(rd.addr)));
			read_results.ਧੱਕੋ_ਪਿੱਠ(factory.ਸਮ੍ਰਿਤੀ_read(ਗੰਢ, addr));
		}
		state.ਸੈੱਟ_ਅਗਲਾ_ਮੁੱਲ(ਗੰਢ);
		ਵਾਪਸ concatenate_read_results(mem, read_results);
	}
	ਖਾਲੀ process_cell(Cell *cell)
	{
		ਜੇ (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, ਖਾਲੀ_ਸੂਚਕ);
			ਜੇ (mem == ਖਾਲੀ_ਸੂਚਕ) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			ਗੰਢ_2 ਗੰਢ = handle_ਸਮ੍ਰਿਤੀ(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_ਅੰਕੜੇ)}), ਗੰਢ);
		} ਨਹੀਂਤਾਂ ਜੇ (cell->is_builtin_ff()) {
			Ff_ਅੰਕੜੇ ff(&ff_initvals, cell);
			ਜੇ (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			ਸਵੈਚਾਲਿਤ &state = factory.ਜੋੜੋ_state(ff.name, ID($state), ਛਾਂਟੋ_2(ff.ਚੌੜਾਈ));
			ਗੰਢ_2 q_ਮੁੱਲ = factory.ਮੁੱਲ(state);
			factory.suggest_name(q_ਮੁੱਲ, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_ਮੁੱਲ);
			state.ਸੈੱਟ_ਅਗਲਾ_ਮੁੱਲ(ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ff.sig_d));
			state.ਸੈੱਟ_initial_ਮੁੱਲ(ff.val_init);
		} ਨਹੀਂਤਾਂ {
			dict<IdString, ਗੰਢ_2> connections;
			IdString output_name; // for the single output case
			ਪੂਰਨ n_outputs = 0;
			ਲਈ(ਸਵੈਚਾਲਿਤ ਅਚਲ &[name, sigspec] : cell->connections()) {
				ਜੇ(driver_map.celltypes.cell_ਨਿਵੇਸ਼(cell->type, name) && sigspec.ਮਾਪ() > 0)
					connections.ਪਾਓ({ name, ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(Drive_Chunk_ਪੋਰਟ(cell, {name, sigspec})) });
				ਜੇ(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, ਗੰਢ_2>, ਗੰਢ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			ਜੇ(ਸਵੈਚਾਲਿਤ *nodep = std::get_if<ਗੰਢ_2>(&outputs); nodep != ਖਾਲੀ_ਸੂਚਕ) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} ਨਹੀਂਤਾਂ {
				ਲਈ(ਸਵੈਚਾਲਿਤ [name, ਗੰਢ] : std::get<dict<IdString, ਗੰਢ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), ਗੰਢ);
			}
		}
	}
	ਖਾਲੀ undriven(ਅਚਲ std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	ਖਾਲੀ check_undriven(DriveSpec ਅਚਲ& spec, std::string ਅਚਲ& name) {
		ਲਈ(ਸਵੈਚਾਲਿਤ ਅਚਲ &chunk : spec.chunks())
			ਜੇ(chunk.is_none())
				undriven(name);
	}
ਜਨਤਕ:
	ਖਾਲੀ process_ਕਤਾਰ()
	{
		ਲਈ (; !ਕਤਾਰ.ਖਾਲੀ_ਥਾਵਾਂ(); ਕਤਾਰ.ਕੱਢੋ_front()) {
			ਜੇ(ਸਵੈਚਾਲਿਤ p = std::get_if<Cell *>(&ਕਤਾਰ.front()); p != ਖਾਲੀ_ਸੂਚਕ) {
				process_cell(*p);
				ਜਾਰੀ;
			}

			DriveSpec spec = std::get<DriveSpec>(ਕਤਾਰ.front());
			ਗੰਢ_2 pending = ਗ੍ਰਾਫ_nodes.at(spec);

			ਜੇ (spec.chunks().ਮਾਪ() > 1) {
				ਸਵੈਚਾਲਿਤ chunks = spec.chunks();
				ਗੰਢ_2 ਗੰਢ = ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(chunks[0]);
				ਲਈ(ਮਾਪ_t i = 1; i < chunks.ਮਾਪ(); i++)
					ਗੰਢ = factory.concat(ਗੰਢ, ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(chunks[i]));
				factory.update_pending(pending, ਗੰਢ);
			} ਨਹੀਂਤਾਂ ਜੇ (spec.chunks().ਮਾਪ() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				ਜੇ (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					ਜੇ (wire_chunk.is_ਸਾਰਾ()) {
						ਜੇ (wire_chunk.wire->ਪੋਰਟ_ਨਿਵੇਸ਼) {
							ਗੰਢ_2 ਗੰਢ = factory.ਮੁੱਲ(factory.ir().ਨਿਵੇਸ਼(wire_chunk.wire->name));
							factory.suggest_name(ਗੰਢ, wire_chunk.wire->name);
							factory.update_pending(pending, ਗੰਢ);
						} ਨਹੀਂਤਾਂ {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							ਗੰਢ_2 ਗੰਢ = ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(driver);
							factory.suggest_name(ਗੰਢ, wire_chunk.wire->name);
							factory.update_pending(pending, ਗੰਢ);
						}
					} ਨਹੀਂਤਾਂ {
						DriveChunkWire ਸਾਰਾ_wire(wire_chunk.wire, 0, wire_chunk.wire->ਚੌੜਾਈ);
						ਗੰਢ_2 ਗੰਢ = factory.ਟੁਕੜਾ(ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਸਾਰਾ_wire), wire_chunk.ਵਿੱਥ, wire_chunk.ਚੌੜਾਈ);
						factory.update_pending(pending, ਗੰਢ);
					}
				} ਨਹੀਂਤਾਂ ਜੇ (chunk.is_ਪੋਰਟ()) {
					Drive_Chunk_ਪੋਰਟ ਪੋਰਟ_chunk = chunk.ਪੋਰਟ();
					ਜੇ (ਪੋਰਟ_chunk.is_ਸਾਰਾ()) {
						ਜੇ (driver_map.celltypes.cell_output(ਪੋਰਟ_chunk.cell->type, ਪੋਰਟ_chunk.ਪੋਰਟ)) {
							ਗੰਢ_2 ਗੰਢ = ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ_cell(ਪੋਰਟ_chunk.cell, ਪੋਰਟ_chunk.ਪੋਰਟ);
							factory.update_pending(pending, ਗੰਢ);
						} ਨਹੀਂਤਾਂ {
							DriveSpec driver = driver_map(DriveSpec(ਪੋਰਟ_chunk));
							check_undriven(driver, ਪੋਰਟ_chunk.cell->name.unescape() + " port " + PooledName(ਪੋਰਟ_chunk.cell->module, ਪੋਰਟ_chunk.ਪੋਰਟ).unescape());
							factory.update_pending(pending, ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(driver));
						}
					} ਨਹੀਂਤਾਂ {
						Drive_Chunk_ਪੋਰਟ ਸਾਰਾ_ਪੋਰਟ(ਪੋਰਟ_chunk.cell, ਪੋਰਟ_chunk.ਪੋਰਟ, 0, Get_ਮਾਪ(ਪੋਰਟ_chunk.cell->connections().at(ਪੋਰਟ_chunk.ਪੋਰਟ)));
						ਗੰਢ_2 ਗੰਢ = factory.ਟੁਕੜਾ(ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(ਸਾਰਾ_ਪੋਰਟ), ਪੋਰਟ_chunk.ਵਿੱਥ, ਪੋਰਟ_chunk.ਚੌੜਾਈ);
						factory.update_pending(pending, ਗੰਢ);
					}
				} ਨਹੀਂਤਾਂ ਜੇ (chunk.is_constant()) {
					ਗੰਢ_2 ਗੰਢ = factory.constant(chunk.constant());
					factory.suggest_name(ਗੰਢ, "$const" + std::ਮੰਜ਼ਿਲ_string(chunk.ਮਾਪ()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, ਗੰਢ);
				} ਨਹੀਂਤਾਂ ਜੇ (chunk.is_ਗੁਣਜ()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} ਨਹੀਂਤਾਂ ਜੇ (chunk.is_none()) {
					undriven(log_signal(chunk));
				} ਨਹੀਂਤਾਂ {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} ਨਹੀਂਤਾਂ {
				log_abort();
			}
		}
	}
};

IR IR::from_module(Module *module) {
	IR ir;
    ir.design = module->design;
    ਸਵੈਚਾਲਿਤ factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_ਕਤਾਰ();
    ir.topological_ਛਾਂਟੋ();
    ir.forward_buf();
    ਵਾਪਸ ir;
}

ਖਾਲੀ IR::topological_ਛਾਂਟੋ() {
    ਗ੍ਰਾਫ::SccAdaptor compute_ਗ੍ਰਾਫ_scc(ਗ੍ਰਾਫ_4);
    ਤਾਰਕਿਕ scc = ਝੂਠ;
    std::ਸਦਿਸ਼<ਪੂਰਨ> perm;
    TopoSortedSccs toposort(compute_ਗ੍ਰਾਫ_scc, [&](ਪੂਰਨ *begin, ਪੂਰਨ *ਸਿਰਾ) {
        perm.ਪਾਓ(perm.ਸਿਰਾ(), begin, ਸਿਰਾ);
        ਜੇ (ਸਿਰਾ > begin + 1)
        {
            log_warning("Combinational loop:\n");
            ਲਈ (ਪੂਰਨ *i = begin; i != ਸਿਰਾ; ++i) {
				ਗੰਢ_2 ਗੰਢ(ਗ੍ਰਾਫ_4[*i], design);
                ਲਘੂਗਣਕ("- %s = %s\n", ਗੰਢ.name().unescape(), ਗੰਢ.ਮੰਜ਼ਿਲ_string());
			}
            ਲਘੂਗਣਕ("\n");
            scc = ਸੱਚ;
        }
    });
	ਲਈ(ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, state]: ਹਾਲਤਾਂ_2)
		ਜੇ(state.has_ਅਗਲਾ_ਮੁੱਲ())
			toposort.process(state.ਅਗਲਾ_ਮੁੱਲ().id());
	ਲਈ(ਅਚਲ ਸਵੈਚਾਲਿਤ &[name, output]: _outputs)
		ਜੇ(output.has_ਮੁੱਲ())
			toposort.process(output.ਮੁੱਲ().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    ਗ੍ਰਾਫ_4.ਕ੍ਰਮ_ਬਦਲੋ(perm);
    ਜੇ(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ਸਥਿਰ IdString ਮਿਲਾਓ_name(IdString a, IdString b) {
	ਜੇ(!a.is_ਜਨਤਕ() && b.is_ਜਨਤਕ())
		ਵਾਪਸ b;
	ਨਹੀਂਤਾਂ
		ਵਾਪਸ a;
}

ਖਾਲੀ IR::forward_buf() {
    std::ਸਦਿਸ਼<ਪੂਰਨ> perm, alias;
    perm.clear();

    ਲਈ (ਪੂਰਨ i = 0; i < ਗ੍ਰਾਫ_4.ਮਾਪ(); ++i)
    {
        ਸਵੈਚਾਲਿਤ ਗੰਢ = ਗ੍ਰਾਫ_4[i];
        ਜੇ (ਗੰਢ.function().fn() == Fn::buf && ਗੰਢ.arg(0).ਸੂਚਕਾਂਕ() < i)
        {
            ਪੂਰਨ ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ = alias[ਗੰਢ.arg(0).ਸੂਚਕਾਂਕ()];
            ਸਵੈਚਾਲਿਤ ਨਿਸ਼ਾਨਾ_ਗੰਢ = ਗ੍ਰਾਫ_4[perm[ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ]];
			ਜੇ(ਗੰਢ.has_sparse_attr()) {
				ਜੇ(ਨਿਸ਼ਾਨਾ_ਗੰਢ.has_sparse_attr()) {
					IdString id = ਮਿਲਾਓ_name(ਗੰਢ.sparse_attr(), ਨਿਸ਼ਾਨਾ_ਗੰਢ.sparse_attr());
					ਨਿਸ਼ਾਨਾ_ਗੰਢ.sparse_attr() = id;
				} ਨਹੀਂਤਾਂ {
					IdString id = ਗੰਢ.sparse_attr();
					ਨਿਸ਼ਾਨਾ_ਗੰਢ.sparse_attr() = id;
				}
			}
            alias.ਧੱਕੋ_ਪਿੱਠ(ਨਿਸ਼ਾਨਾ_ਸੂਚਕਾਂਕ);
        }
        ਨਹੀਂਤਾਂ
        {
            alias.ਧੱਕੋ_ਪਿੱਠ(Get_ਮਾਪ(perm));
            perm.ਧੱਕੋ_ਪਿੱਠ(i);
        }
    }
    ਗ੍ਰਾਫ_4.ਕ੍ਰਮ_ਬਦਲੋ(perm, alias);
}

// Quoting routine to make error messages nicer
ਸਥਿਰ std::string quote_fmt(ਅਚਲ ਅੱਖਰ *fmt)
{
	std::string r;
	ਲਈ(ਅਚਲ ਅੱਖਰ *p = fmt; *p != 0; p++) {
		ਚੋਣ(*p) {
		ਹਾਲਤ '\n': r += "\\n"; ਰੋਕੋ;
		ਹਾਲਤ '\t': r += "\\t"; ਰੋਕੋ;
		ਹਾਲਤ '"': r += "\\\""; ਰੋਕੋ;
		ਹਾਲਤ '\\': r += "\\\\"; ਰੋਕੋ;
		ਮੂਲ: r += *p; ਰੋਕੋ;
		}
	}
	ਵਾਪਸ r;
}

ਖਾਲੀ Writer::print_impl(ਅਚਲ ਅੱਖਰ *fmt, ਸਦਿਸ਼<std::function<ਖਾਲੀ()>> &fns)
{
	ਮਾਪ_t ਅਗਲਾ_ਸੂਚਕਾਂਕ = 0;
	ਲਈ(ਅਚਲ ਅੱਖਰ *p = fmt; *p != 0; p++)
		ਚੋਣ(*p) {
		ਹਾਲਤ '{':
			ਜੇ(*++p == '{') {
				*os << '{';
			} ਨਹੀਂਤਾਂ {
				ਅੱਖਰ *pe;
				ਮਾਪ_t ਸੂਚਕਾਂਕ = strtoul(p, &pe, 10);
				ਜੇ(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				ਜੇ(p == pe)
					ਸੂਚਕਾਂਕ = ਅਗਲਾ_ਸੂਚਕਾਂਕ;
				ਨਹੀਂਤਾਂ
					p = pe;
				ਜੇ(ਸੂਚਕਾਂਕ >= fns.ਮਾਪ())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", ਸੂਚਕਾਂਕ, fns.ਮਾਪ(), quote_fmt(fmt));
				fns[ਸੂਚਕਾਂਕ]();
				ਅਗਲਾ_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ + 1;
			}
			ਰੋਕੋ;
		ਹਾਲਤ '}':
			p++;
			ਜੇ(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			ਰੋਕੋ;
		ਮੂਲ:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_ਸਿਰਾ
