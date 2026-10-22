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

ಧ್ರುವ ವರ್ಣ *fn_to_string(Fn fn) {
	ವಿಕಲ್ಪ(fn) {
	ಪಕ್ಷ Fn::invalid: ಪ್ರತ್ಯಾಗಮ "invalid";
	ಪಕ್ಷ Fn::buf: ಪ್ರತ್ಯಾಗಮ "buf";
	ಪಕ್ಷ Fn::slice: ಪ್ರತ್ಯಾಗಮ "slice";
	ಪಕ್ಷ Fn::zero_extend: ಪ್ರತ್ಯಾಗಮ "zero_extend";
	ಪಕ್ಷ Fn::sign_extend: ಪ್ರತ್ಯಾಗಮ "sign_extend";
	ಪಕ್ಷ Fn::concat: ಪ್ರತ್ಯಾಗಮ "concat";
	ಪಕ್ಷ Fn::add: ಪ್ರತ್ಯಾಗಮ "add";
	ಪಕ್ಷ Fn::sub: ಪ್ರತ್ಯಾಗಮ "sub";
	ಪಕ್ಷ Fn::mul: ಪ್ರತ್ಯಾಗಮ "mul";
	ಪಕ್ಷ Fn::unsigned_div: ಪ್ರತ್ಯಾಗಮ "unsigned_div";
	ಪಕ್ಷ Fn::unsigned_mod: ಪ್ರತ್ಯಾಗಮ "unsigned_mod";
	ಪಕ್ಷ Fn::bitwise_and: ಪ್ರತ್ಯಾಗಮ "bitwise_and";
	ಪಕ್ಷ Fn::bitwise_or: ಪ್ರತ್ಯಾಗಮ "bitwise_or";
	ಪಕ್ಷ Fn::bitwise_xor: ಪ್ರತ್ಯಾಗಮ "bitwise_xor";
	ಪಕ್ಷ Fn::bitwise_not: ಪ್ರತ್ಯಾಗಮ "bitwise_not";
	ಪಕ್ಷ Fn::reduce_and: ಪ್ರತ್ಯಾಗಮ "reduce_and";
	ಪಕ್ಷ Fn::reduce_or: ಪ್ರತ್ಯಾಗಮ "reduce_or";
	ಪಕ್ಷ Fn::reduce_xor: ಪ್ರತ್ಯಾಗಮ "reduce_xor";
	ಪಕ್ಷ Fn::unary_minus: ಪ್ರತ್ಯಾಗಮ "unary_minus";
	ಪಕ್ಷ Fn::equal: ಪ್ರತ್ಯಾಗಮ "equal";
	ಪಕ್ಷ Fn::not_equal: ಪ್ರತ್ಯಾಗಮ "not_equal";
	ಪಕ್ಷ Fn::signed_greater_than: ಪ್ರತ್ಯಾಗಮ "signed_greater_than";
	ಪಕ್ಷ Fn::signed_greater_equal: ಪ್ರತ್ಯಾಗಮ "signed_greater_equal";
	ಪಕ್ಷ Fn::unsigned_greater_than: ಪ್ರತ್ಯಾಗಮ "unsigned_greater_than";
	ಪಕ್ಷ Fn::unsigned_greater_equal: ಪ್ರತ್ಯಾಗಮ "unsigned_greater_equal";
	ಪಕ್ಷ Fn::logical_shift_ಎಡ: ಪ್ರತ್ಯಾಗಮ "logical_shift_left";
	ಪಕ್ಷ Fn::logical_shift_ಬಲ: ಪ್ರತ್ಯಾಗಮ "logical_shift_right";
	ಪಕ್ಷ Fn::arithmetic_shift_ಬಲ: ಪ್ರತ್ಯಾಗಮ "arithmetic_shift_right";
	ಪಕ್ಷ Fn::mux: ಪ್ರತ್ಯಾಗಮ "mux";
	ಪಕ್ಷ Fn::constant: ಪ್ರತ್ಯಾಗಮ "constant";
	ಪಕ್ಷ Fn::ಒಳಹರಿವು: ಪ್ರತ್ಯಾಗಮ "input";
	ಪಕ್ಷ Fn::state: ಪ್ರತ್ಯಾಗಮ "state";
	ಪಕ್ಷ Fn::memory_read: ಪ್ರತ್ಯಾಗಮ "memory_read";
	ಪಕ್ಷ Fn::memory_write: ಪ್ರತ್ಯಾಗಮ "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (ಪೂರ್ಣಾಙ್ಕ)fn);
}

vector<IRInput ಧ್ರುವ*> IR::inputs(IdString kind) ಧ್ರುವ {
	vector<IRInput ಧ್ರುವ*> ret;
	ಪ್ರತಿ (ಧ್ರುವ auto &[name, ಒಳಹರಿವು] : _inputs)
		ಯದಿ(ಒಳಹರಿವು.kind == kind)
			ret.ತಳ್ಳು_back(&ಒಳಹರಿವು);
	ಪ್ರತ್ಯಾಗಮ ret;
}

vector<IROutput ಧ್ರುವ*> IR::outputs(IdString kind) ಧ್ರುವ {
	vector<IROutput ಧ್ರುವ*> ret;
	ಪ್ರತಿ (ಧ್ರುವ auto &[name, output] : _outputs)
		ಯದಿ(output.kind == kind)
			ret.ತಳ್ಳು_back(&output);
	ಪ್ರತ್ಯಾಗಮ ret;
}

vector<IRState ಧ್ರುವ*> IR::states(IdString kind) ಧ್ರುವ {
	vector<IRState ಧ್ರುವ*> ret;
	ಪ್ರತಿ (ಧ್ರುವ auto &[name, state] : _states)
		ಯದಿ(state.kind == kind)
			ret.ತಳ್ಳು_back(&state);
	ಪ್ರತ್ಯಾಗಮ ret;
}

vector<IRInput ಧ್ರುವ*> IR::all_inputs() ಧ್ರುವ {
	vector<IRInput ಧ್ರುವ*> ret;
	ಪ್ರತಿ (ಧ್ರುವ auto &[name, ಒಳಹರಿವು] : _inputs)
		ret.ತಳ್ಳು_back(&ಒಳಹರಿವು);
	ಪ್ರತ್ಯಾಗಮ ret;
}

vector<IROutput ಧ್ರುವ*> IR::all_outputs() ಧ್ರುವ {
	vector<IROutput ಧ್ರುವ*> ret;
	ಪ್ರತಿ (ಧ್ರುವ auto &[name, output] : _outputs)
		ret.ತಳ್ಳು_back(&output);
	ಪ್ರತ್ಯಾಗಮ ret;
}

vector<IRState ಧ್ರುವ*> IR::all_states() ಧ್ರುವ {
	vector<IRState ಧ್ರುವ*> ret;
	ಪ್ರತಿ (ಧ್ರುವ auto &[name, state] : _states)
		ret.ತಳ್ಳು_back(&state);
	ಪ್ರತ್ಯಾಗಮ ret;
}

ಸಂರಚನಾ PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(ಗಂಟು_2)> np;
	PrintVisitor(std::function<std::string(ಗಂಟು_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(ಗಂಟು_2, ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ out_width) override { ಪ್ರತ್ಯಾಗಮ "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(ಗಂಟು_2, ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ out_width) override { ಪ್ರತ್ಯಾಗಮ "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(ಗಂಟು_2, ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ out_width) override { ಪ್ರತ್ಯಾಗಮ "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(ಗಂಟು_2, RTLIL::Const ಧ್ರುವ& ಮೌಲ್ಯ) override { ಪ್ರತ್ಯಾಗಮ "constant(" + ಮೌಲ್ಯ.as_string() + ")"; }
	std::string ಒಳಹರಿವು(ಗಂಟು_2 self, IdString name, IdString kind) override { ಪ್ರತ್ಯಾಗಮ "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(ಗಂಟು_2 self, IdString name, IdString kind) override { ಪ್ರತ್ಯಾಗಮ "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(ಗಂಟು_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		ಪ್ರತಿ(size_t i = 0; i < self.arg_ಎಣಿಕೆ(); i++) {
			ಯದಿ(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		ಪ್ರತ್ಯಾಗಮ ret;
	}
};

std::string ಗಂಟು_2::to_string()
{
	ಪ್ರತ್ಯಾಗಮ to_string([](ಗಂಟು_2 n) { ಪ್ರತ್ಯಾಗಮ n.name().unescape(); });
}

std::string ಗಂಟು_2::to_string(std::function<std::string(ಗಂಟು_2)> np)
{
	ಪ್ರತ್ಯಾಗಮ visit(PrintVisitor(np));
}

ವರ್ಗ CellSimplifier {
	Factory &factory;
	ಗಂಟು_2 sign(ಗಂಟು_2 a) {
		ಪ್ರತ್ಯಾಗಮ factory.slice(a, a.width() - 1, 1);
	}
	ಗಂಟು_2 neg_if(ಗಂಟು_2 a, ಗಂಟು_2 s) {
		ಪ್ರತ್ಯಾಗಮ factory.mux(a, factory.unary_minus(a), s);
	}
	ಗಂಟು_2 abs(ಗಂಟು_2 a) {
		ಪ್ರತ್ಯಾಗಮ neg_if(a, sign(a));
	}
	ಗಂಟು_2 handle_shift(ಗಂಟು_2 a, ಗಂಟು_2 b, ತಾರ್ಕಿಕ is_ಬಲ, ತಾರ್ಕಿಕ is_signed) {
		// to prevent new_width == 0, we handle this case separately
		ಯದಿ(a.width() == 1) {
			ಯದಿ(!is_signed)
				ಪ್ರತ್ಯಾಗಮ factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ a;
		}
		ಪೂರ್ಣಾಙ್ಕ new_width = ceil_log2(a.width());
		ಗಂಟು_2 b_truncated = factory.extend(b, new_width, ಅಸತ್ಯ);
		ಗಂಟು_2 y =
			!is_ಬಲ ? factory.logical_shift_ಎಡ(a, b_truncated) :
			!is_signed ? factory.logical_shift_ಬಲ(a, b_truncated) :
			factory.arithmetic_shift_ಬಲ(a, b_truncated);
		ಯದಿ(b.width() <= new_width)
			ಪ್ರತ್ಯಾಗಮ y;
		ಗಂಟು_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		ಗಂಟು_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), ಸತ್ಯ) : factory.constant(RTLIL::Const(State::S0, a.width()));
		ಪ್ರತ್ಯಾಗಮ factory.mux(y, y_if_overflow, overflow);
	}
ಸಾರ್ವಜನಿಕ:
	ಗಂಟು_2 logical_shift_ಎಡ(ಗಂಟು_2 a, ಗಂಟು_2 b) { ಪ್ರತ್ಯಾಗಮ handle_shift(a, b, ಅಸತ್ಯ, ಅಸತ್ಯ); }
	ಗಂಟು_2 logical_shift_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { ಪ್ರತ್ಯಾಗಮ handle_shift(a, b, ಸತ್ಯ, ಅಸತ್ಯ); }
	ಗಂಟು_2 arithmetic_shift_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { ಪ್ರತ್ಯಾಗಮ handle_shift(a, b, ಸತ್ಯ, ಸತ್ಯ); }
	ಗಂಟು_2 bitwise_mux(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) {
		ಗಂಟು_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		ಗಂಟು_2 bb = factory.bitwise_and(b, s);
		ಪ್ರತ್ಯಾಗಮ factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
ಗುಪ್ತ:
	ಗಂಟು_2 handle_pow(ಗಂಟು_2 a0, ಗಂಟು_2 b, ಪೂರ್ಣಾಙ್ಕ y_width, ತಾರ್ಕಿಕ is_signed) {
		ಗಂಟು_2 a = factory.extend(a0, y_width, is_signed);
		ಗಂಟು_2 r = factory.constant(Const(1, y_width));
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < b.width(); i++) {
			ಗಂಟು_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		ಯದಿ (is_signed) {
			ಗಂಟು_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			ಗಂಟು_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		ಪ್ರತ್ಯಾಗಮ r;
	}
	ಗಂಟು_2 handle_bmux(ಗಂಟು_2 a, ಗಂಟು_2 s, ಪೂರ್ಣಾಙ್ಕ a_offset, ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ sn) {
		ಯದಿ(sn < 1)
			ಪ್ರತ್ಯಾಗಮ factory.slice(a, a_offset, width);
		ಅನ್ಯಥಾ {
			ಗಂಟು_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			ಗಂಟು_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			ಪ್ರತ್ಯಾಗಮ factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	ಗಂಟು_2 handle_pmux(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		ಗಂಟು_2 y = a;
		ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		ಪ್ರತ್ಯಾಗಮ y;
	}
	dict<IdString, ಗಂಟು_2> handle_fa(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 c) {
		ಗಂಟು_2 t1 = factory.bitwise_xor(a, b);
		ಗಂಟು_2 t2 = factory.bitwise_and(a, b);
		ಗಂಟು_2 t3 = factory.bitwise_and(c, t1);
		ಗಂಟು_2 y = factory.bitwise_xor(c, t1);
		ಗಂಟು_2 x = factory.bitwise_or(t2, t3);
		ಪ್ರತ್ಯಾಗಮ {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, ಗಂಟು_2> handle_alu(ಗಂಟು_2 a_in, ಗಂಟು_2 b_in, ಪೂರ್ಣಾಙ್ಕ y_width, ತಾರ್ಕಿಕ is_signed, ಗಂಟು_2 ci, ಗಂಟು_2 bi) {
		ಗಂಟು_2 a = factory.extend(a_in, y_width, is_signed);
		ಗಂಟು_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		ಗಂಟು_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		ಗಂಟು_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		ಗಂಟು_2 a_extra = factory.extend(a, y_width + 1, ಅಸತ್ಯ);
		ಗಂಟು_2 b_extra = factory.extend(b, y_width + 1, ಅಸತ್ಯ);
		ಗಂಟು_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, ಅಸತ್ಯ));
		ಗಂಟು_2 y = factory.slice(y_extra, 0, y_width);
		ಗಂಟು_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		ಗಂಟು_2 co = factory.slice(carries, 1, y_width);
		ಪ್ರತ್ಯಾಗಮ {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	ಗಂಟು_2 handle_lcu(ಗಂಟು_2 p, ಗಂಟು_2 g, ಗಂಟು_2 ci) {
		ಪ್ರತ್ಯಾಗಮ handle_alu(g, factory.bitwise_or(p, g), g.width(), ಅಸತ್ಯ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
ಸಾರ್ವಜನಿಕ:
	std::variant<dict<IdString, ಗಂಟು_2>, ಗಂಟು_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, ಗಂಟು_2> inputs)
	{
		ಪೂರ್ಣಾಙ್ಕ a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		ಪೂರ್ಣಾಙ್ಕ b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		ಪೂರ್ಣಾಙ್ಕ y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		ತಾರ್ಕಿಕ a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		ತಾರ್ಕಿಕ b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		ಯದಿ(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			ತಾರ್ಕಿಕ is_signed = a_signed && b_signed;
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			ಗಂಟು_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			ಯದಿ(cellType == ID($add))
				ಪ್ರತ್ಯಾಗಮ factory.add(a, b);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($sub))
				ಪ್ರತ್ಯಾಗಮ factory.sub(a, b);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($mul))
				ಪ್ರತ್ಯಾಗಮ factory.mul(a, b);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($and))
				ಪ್ರತ್ಯಾಗಮ factory.bitwise_and(a, b);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($or))
				ಪ್ರತ್ಯಾಗಮ factory.bitwise_or(a, b);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($xor))
				ಪ್ರತ್ಯಾಗಮ factory.bitwise_xor(a, b);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($xnor))
				ಪ್ರತ್ಯಾಗಮ factory.bitwise_not(factory.bitwise_xor(a, b));
			ಅನ್ಯಥಾ
				log_abort();
		}ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			ತಾರ್ಕಿಕ is_signed = a_signed && b_signed;
			ಪೂರ್ಣಾಙ್ಕ width = max(a_width, b_width);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			ಗಂಟು_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ಯದಿ(cellType.in(ID($eq), ID($eqx)))
				ಪ್ರತ್ಯಾಗಮ factory.extend(factory.equal(a, b), y_width, ಅಸತ್ಯ);
			ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($ne), ID($nex)))
				ಪ್ರತ್ಯಾಗಮ factory.extend(factory.not_equal(a, b), y_width, ಅಸತ್ಯ);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($lt))
				ಪ್ರತ್ಯಾಗಮ factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, ಅಸತ್ಯ);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($le))
				ಪ್ರತ್ಯಾಗಮ factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, ಅಸತ್ಯ);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($gt))
				ಪ್ರತ್ಯಾಗಮ factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, ಅಸತ್ಯ);
			ಅನ್ಯಥಾ ಯದಿ(cellType == ID($ge))
				ಪ್ರತ್ಯಾಗಮ factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, ಅಸತ್ಯ);
			ಅನ್ಯಥಾ
				log_abort();
		}ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($logic_or), ID($logic_and))){
			ಗಂಟು_2 a = factory.reduce_or(inputs.at(ID(A)));
			ಗಂಟು_2 b = factory.reduce_or(inputs.at(ID(B)));
			ಗಂಟು_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			ಪ್ರತ್ಯಾಗಮ factory.extend(y, y_width, ಅಸತ್ಯ);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($not)){
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ಪ್ರತ್ಯಾಗಮ factory.bitwise_not(a);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($pos)){
			ಪ್ರತ್ಯಾಗಮ factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($barrier)){
			ಪ್ರತ್ಯಾಗಮ inputs.at(ID(A));
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($neg)){
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ಪ್ರತ್ಯಾಗಮ factory.unary_minus(a);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($logic_not)){
			ಗಂಟು_2 a = factory.reduce_or(inputs.at(ID(A)));
			ಗಂಟು_2 y = factory.bitwise_not(a);
			ಪ್ರತ್ಯಾಗಮ factory.extend(y, y_width, ಅಸತ್ಯ);
		}ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($reduce_or), ID($reduce_bool))){
			ಗಂಟು_2 a = factory.reduce_or(inputs.at(ID(A)));
			ಪ್ರತ್ಯಾಗಮ factory.extend(a, y_width, ಅಸತ್ಯ);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($reduce_and)){
			ಗಂಟು_2 a = factory.reduce_and(inputs.at(ID(A)));
			ಪ್ರತ್ಯಾಗಮ factory.extend(a, y_width, ಅಸತ್ಯ);
		}ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			ಗಂಟು_2 a = factory.reduce_xor(inputs.at(ID(A)));
			ಗಂಟು_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			ಪ್ರತ್ಯಾಗಮ factory.extend(y, y_width, ಅಸತ್ಯ);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($shl) || cellType == ID($sshl)){
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಪ್ರತ್ಯಾಗಮ logical_shift_ಎಡ(a, b);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($shr) || cellType == ID($sshr)){
			ಪೂರ್ಣಾಙ್ಕ width = max(a_width, y_width);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಗಂಟು_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_ಬಲ(a, b) :
				logical_shift_ಬಲ(a, b);
			ಪ್ರತ್ಯಾಗಮ factory.extend(y, y_width, a_signed);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($shiftx) || cellType == ID($shift)){
			ಪೂರ್ಣಾಙ್ಕ width = max(a_width, y_width);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಗಂಟು_2 shr = logical_shift_ಬಲ(a, b);
			ಯದಿ(b_signed) {
				ಗಂಟು_2 shl = logical_shift_ಎಡ(a, factory.unary_minus(b));
				ಗಂಟು_2 y = factory.mux(shr, shl, sign(b));
				ಪ್ರತ್ಯಾಗಮ factory.extend(y, y_width, ಅಸತ್ಯ);
			} ಅನ್ಯಥಾ {
				ಪ್ರತ್ಯಾಗಮ factory.extend(shr, y_width, ಅಸತ್ಯ);
			}
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($mux)){
			ಪ್ರತ್ಯಾಗಮ factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($pmux)){
			ಪ್ರತ್ಯಾಗಮ handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($concat)){
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಪ್ರತ್ಯಾಗಮ factory.concat(a, b);
		}ಅನ್ಯಥಾ ಯದಿ(cellType == ID($slice)){
			ಪೂರ್ಣಾಙ್ಕ offset = parameters.at(ID(OFFSET)).as_int();
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಪ್ರತ್ಯಾಗಮ factory.slice(a, offset, y_width);
		}ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			ಪೂರ್ಣಾಙ್ಕ width = max(a_width, b_width);
			ತಾರ್ಕಿಕ is_signed = a_signed && b_signed;
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			ಗಂಟು_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ಯದಿ(is_signed) {
				ಯದಿ(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					ಗಂಟು_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					ಗಂಟು_2 out_sign = factory.not_equal(sign(a), sign(b));
					ಪ್ರತ್ಯಾಗಮ neg_if(factory.extend(abs_y, y_width, ಅಸತ್ಯ), out_sign);
				} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					ಗಂಟು_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					ಪ್ರತ್ಯಾಗಮ neg_if(factory.extend(abs_y, y_width, ಅಸತ್ಯ), sign(a));
				} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($divfloor)) {
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
					ಗಂಟು_2 y2 = factory.extend(y1, y_width, ಅಸತ್ಯ);
					ಪ್ರತ್ಯಾಗಮ factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					ಗಂಟು_2 abs_b = abs(b);
					ಗಂಟು_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					ಗಂಟು_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					ಗಂಟು_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					ಗಂಟು_2 y = neg_if(y_flipped, sign(b));
					ಪ್ರತ್ಯಾಗಮ factory.extend(y, y_width, ಸತ್ಯ);
				} ಅನ್ಯಥಾ
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} ಅನ್ಯಥಾ {
				ಯದಿ(cellType.in(ID($mod), ID($modfloor)))
					ಪ್ರತ್ಯಾಗಮ factory.extend(factory.unsigned_mod(a, b), y_width, ಅಸತ್ಯ);
				ಅನ್ಯಥಾ
					ಪ್ರತ್ಯಾಗಮ factory.extend(factory.unsigned_div(a, b), y_width, ಅಸತ್ಯ);
			}
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($pow)) {
			ಪ್ರತ್ಯಾಗಮ handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} ಅನ್ಯಥಾ ಯದಿ (cellType == ID($lut)) {
			ಪೂರ್ಣಾಙ್ಕ width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			ಪ್ರತ್ಯಾಗಮ handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} ಅನ್ಯಥಾ ಯದಿ (cellType == ID($bwmux)) {
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಗಂಟು_2 s = inputs.at(ID(S));
			ಪ್ರತ್ಯಾಗಮ factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} ಅನ್ಯಥಾ ಯದಿ (cellType == ID($bweqx)) {
			ಗಂಟು_2 a = inputs.at(ID(A));
			ಗಂಟು_2 b = inputs.at(ID(B));
			ಪ್ರತ್ಯಾಗಮ factory.bitwise_not(factory.bitwise_xor(a, b));
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($bmux)) {
			ಪೂರ್ಣಾಙ್ಕ width = parameters.at(ID(WIDTH)).as_int();
			ಪೂರ್ಣಾಙ್ಕ s_width = parameters.at(ID(S_WIDTH)).as_int();
			ಪ್ರತ್ಯಾಗಮ handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($demux)) {
			ಪೂರ್ಣಾಙ್ಕ width = parameters.at(ID(WIDTH)).as_int();
			ಪೂರ್ಣಾಙ್ಕ s_width = parameters.at(ID(S_WIDTH)).as_int();
			ಪೂರ್ಣಾಙ್ಕ y_width = width << s_width;
			ಪೂರ್ಣಾಙ್ಕ b_width = ceil_log2(y_width);
			ಗಂಟು_2 a = factory.extend(inputs.at(ID(A)), y_width, ಅಸತ್ಯ);
			ಗಂಟು_2 s = factory.extend(inputs.at(ID(S)), b_width, ಅಸತ್ಯ);
			ಗಂಟು_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			ಪ್ರತ್ಯಾಗಮ factory.logical_shift_ಎಡ(a, b);
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($fa)) {
			ಪ್ರತ್ಯಾಗಮ handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($lcu)) {
			ಪ್ರತ್ಯಾಗಮ handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($alu)) {
			ಪ್ರತ್ಯಾಗಮ handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($live), ID($fair), ID($cover))) {
			ಗಂಟು_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_ಮೌಲ್ಯ(a);
			ಪ್ರತ್ಯಾಗಮ {};
		} ಅನ್ಯಥಾ ಯದಿ(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			ಪೂರ್ಣಾಙ್ಕ width = parameters.at(ID(WIDTH)).as_int();
			auto &ಒಳಹರಿವು = factory.add_ಒಳಹರಿವು(cellName, cellType, Sort(width));
			ಪ್ರತ್ಯಾಗಮ factory.ಮೌಲ್ಯ(ಒಳಹರಿವು);
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($initstate)) {
			ಯದಿ(factory.ir().has_state(ID($initstate), ID($state)))
				ಪ್ರತ್ಯಾಗಮ factory.ಮೌಲ್ಯ(factory.ir().state(ID($initstate)));
			ಅನ್ಯಥಾ {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_ಮೌಲ್ಯ(RTLIL::Const(State::S1, 1));
				state.set_next_ಮೌಲ್ಯ(factory.constant(RTLIL::Const(State::S0, 1)));
				ಪ್ರತ್ಯಾಗಮ factory.ಮೌಲ್ಯ(state);
			}
		} ಅನ್ಯಥಾ ಯದಿ(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} ಅನ್ಯಥಾ {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

ವರ್ಗ FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, ಗಂಟು_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, ಗಂಟು_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	ಗಂಟು_2 enqueue(DriveSpec ಧ್ರುವ &spec)
	{
		auto it = graph_nodes.find(spec);
		ಯದಿ(it == graph_nodes.end()){
			auto ಗಂಟು = factory.create_pending(spec.size());
			graph_nodes.ಸೇರಿಸು({spec, ಗಂಟು});
			queue.emplace_back(spec);
			ಪ್ರತ್ಯಾಗಮ ಗಂಟು;
		}ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ it->second;
	}
	ಗಂಟು_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		ಯದಿ(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<ಗಂಟು_2> rv;
			ಪ್ರತಿ(auto ಧ್ರುವ &[name, sigspec] : cell->connections())
				ಯದಿ(driver_map.celltypes.cell_output(cell->type, name)) {
					auto ಗಂಟು = factory.create_pending(sigspec.size());
					factory.suggest_name(ಗಂಟು, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, ಗಂಟು);
					ಯದಿ(name == port_name)
						rv = ಗಂಟು;
				}
			ಪ್ರತ್ಯಾಗಮ *rv;
		} ಅನ್ಯಥಾ
			ಪ್ರತ್ಯಾಗಮ it->second;
	}
ಸಾರ್ವಜನಿಕ:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		ಪ್ರತಿ (auto cell : module->cells()) {
			ಯದಿ (cell->type.in(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		ಪ್ರತಿ (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			ಯದಿ (wire && wire->port_ಒಳಹರಿವು) {
				factory.add_ಒಳಹರಿವು(wire->name, ID($ಒಳಹರಿವು), Sort(wire->width));
			}
			ಯದಿ (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_ಮೌಲ್ಯ(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		ಪ್ರತಿ (auto &mem : memories_vector) {
			ಯದಿ (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
ಗುಪ್ತ:
	ಗಂಟು_2 concatenate_read_results(Mem *mem, vector<ಗಂಟು_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ಧ್ರುವ SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		ಪೂರ್ಣಾಙ್ಕ current = 0;
		ಪ್ರತಿ(size_t i = 0; i < mem->rd_ports.size(); i++) {
			ಪೂರ್ಣಾಙ್ಕ width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		ಗಂಟು_2 ಗಂಟು = results[0];
		ಪ್ರತಿ(size_t i = 1; i < results.size(); i++)
			ಗಂಟು = factory.concat(ಗಂಟು, results[i]);
		ಪ್ರತ್ಯಾಗಮ ಗಂಟು;
	}
	ಗಂಟು_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<ಗಂಟು_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_ಮೌಲ್ಯ(MemContents(mem));
		ಗಂಟು_2 ಗಂಟು = factory.ಮೌಲ್ಯ(state);
		ಪ್ರತಿ (size_t i = 0; i < mem->wr_ports.size(); i++) {
			ಧ್ರುವ auto &wr = mem->wr_ports[i];
			ಯದಿ (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ಗಂಟು_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			ಗಂಟು_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			ಗಂಟು_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			ಗಂಟು_2 old_data = factory.memory_read(ಗಂಟು, addr);
			ಗಂಟು_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			ಗಂಟು = factory.memory_write(ಗಂಟು, addr, wr_data);
		}
		ಯದಿ (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		ಪ್ರತಿ (size_t i = 0; i < mem->rd_ports.size(); i++) {
			ಧ್ರುವ auto &rd = mem->rd_ports[i];
			ಯದಿ (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ಗಂಟು_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.ತಳ್ಳು_back(factory.memory_read(ಗಂಟು, addr));
		}
		state.set_next_ಮೌಲ್ಯ(ಗಂಟು);
		ಪ್ರತ್ಯಾಗಮ concatenate_read_results(mem, read_results);
	}
	ಶೂನ್ಯ process_cell(Cell *cell)
	{
		ಯದಿ (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			ಯದಿ (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			ಗಂಟು_2 ಗಂಟು = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), ಗಂಟು);
		} ಅನ್ಯಥಾ ಯದಿ (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			ಯದಿ (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			ಗಂಟು_2 q_ಮೌಲ್ಯ = factory.ಮೌಲ್ಯ(state);
			factory.suggest_name(q_ಮೌಲ್ಯ, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_ಮೌಲ್ಯ);
			state.set_next_ಮೌಲ್ಯ(enqueue(ff.sig_d));
			state.set_initial_ಮೌಲ್ಯ(ff.val_init);
		} ಅನ್ಯಥಾ {
			dict<IdString, ಗಂಟು_2> connections;
			IdString output_name; // for the single output case
			ಪೂರ್ಣಾಙ್ಕ n_outputs = 0;
			ಪ್ರತಿ(auto ಧ್ರುವ &[name, sigspec] : cell->connections()) {
				ಯದಿ(driver_map.celltypes.cell_ಒಳಹರಿವು(cell->type, name) && sigspec.size() > 0)
					connections.ಸೇರಿಸು({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				ಯದಿ(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, ಗಂಟು_2>, ಗಂಟು_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			ಯದಿ(auto *nodep = std::get_if<ಗಂಟು_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} ಅನ್ಯಥಾ {
				ಪ್ರತಿ(auto [name, ಗಂಟು] : std::get<dict<IdString, ಗಂಟು_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), ಗಂಟು);
			}
		}
	}
	ಶೂನ್ಯ undriven(ಧ್ರುವ std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	ಶೂನ್ಯ check_undriven(DriveSpec ಧ್ರುವ& spec, std::string ಧ್ರುವ& name) {
		ಪ್ರತಿ(auto ಧ್ರುವ &chunk : spec.chunks())
			ಯದಿ(chunk.is_none())
				undriven(name);
	}
ಸಾರ್ವಜನಿಕ:
	ಶೂನ್ಯ process_queue()
	{
		ಪ್ರತಿ (; !queue.empty(); queue.ತೆಗೆ_front()) {
			ಯದಿ(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				ಅನುವರ್ತ;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			ಗಂಟು_2 pending = graph_nodes.at(spec);

			ಯದಿ (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				ಗಂಟು_2 ಗಂಟು = enqueue(chunks[0]);
				ಪ್ರತಿ(size_t i = 1; i < chunks.size(); i++)
					ಗಂಟು = factory.concat(ಗಂಟು, enqueue(chunks[i]));
				factory.update_pending(pending, ಗಂಟು);
			} ಅನ್ಯಥಾ ಯದಿ (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				ಯದಿ (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					ಯದಿ (wire_chunk.is_whole()) {
						ಯದಿ (wire_chunk.wire->port_ಒಳಹರಿವು) {
							ಗಂಟು_2 ಗಂಟು = factory.ಮೌಲ್ಯ(factory.ir().ಒಳಹರಿವು(wire_chunk.wire->name));
							factory.suggest_name(ಗಂಟು, wire_chunk.wire->name);
							factory.update_pending(pending, ಗಂಟು);
						} ಅನ್ಯಥಾ {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							ಗಂಟು_2 ಗಂಟು = enqueue(driver);
							factory.suggest_name(ಗಂಟು, wire_chunk.wire->name);
							factory.update_pending(pending, ಗಂಟು);
						}
					} ಅನ್ಯಥಾ {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						ಗಂಟು_2 ಗಂಟು = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, ಗಂಟು);
					}
				} ಅನ್ಯಥಾ ಯದಿ (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					ಯದಿ (port_chunk.is_whole()) {
						ಯದಿ (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							ಗಂಟು_2 ಗಂಟು = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, ಗಂಟು);
						} ಅನ್ಯಥಾ {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} ಅನ್ಯಥಾ {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						ಗಂಟು_2 ಗಂಟು = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, ಗಂಟು);
					}
				} ಅನ್ಯಥಾ ಯದಿ (chunk.is_constant()) {
					ಗಂಟು_2 ಗಂಟು = factory.constant(chunk.constant());
					factory.suggest_name(ಗಂಟು, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, ಗಂಟು);
				} ಅನ್ಯಥಾ ಯದಿ (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} ಅನ್ಯಥಾ ಯದಿ (chunk.is_none()) {
					undriven(log_signal(chunk));
				} ಅನ್ಯಥಾ {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} ಅನ್ಯಥಾ {
				log_abort();
			}
		}
	}
};

IR IR::from_module(Module *module) {
	IR ir;
    ir.design = module->design;
    auto factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_queue();
    ir.topological_sort();
    ir.forward_buf();
    ಪ್ರತ್ಯಾಗಮ ir;
}

ಶೂನ್ಯ IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    ತಾರ್ಕಿಕ scc = ಅಸತ್ಯ;
    std::vector<ಪೂರ್ಣಾಙ್ಕ> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](ಪೂರ್ಣಾಙ್ಕ *begin, ಪೂರ್ಣಾಙ್ಕ *end) {
        perm.ಸೇರಿಸು(perm.end(), begin, end);
        ಯದಿ (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ *i = begin; i != end; ++i) {
				ಗಂಟು_2 ಗಂಟು(_graph[*i], design);
                log("- %s = %s\n", ಗಂಟು.name().unescape(), ಗಂಟು.to_string());
			}
            log("\n");
            scc = ಸತ್ಯ;
        }
    });
	ಪ್ರತಿ(ಧ್ರುವ auto &[name, state]: _states)
		ಯದಿ(state.has_next_ಮೌಲ್ಯ())
			toposort.process(state.next_ಮೌಲ್ಯ().id());
	ಪ್ರತಿ(ಧ್ರುವ auto &[name, output]: _outputs)
		ಯದಿ(output.has_ಮೌಲ್ಯ())
			toposort.process(output.ಮೌಲ್ಯ().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    ಯದಿ(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ಸ್ಥಿರ IdString merge_name(IdString a, IdString b) {
	ಯದಿ(!a.isPublic() && b.isPublic())
		ಪ್ರತ್ಯಾಗಮ b;
	ಅನ್ಯಥಾ
		ಪ್ರತ್ಯಾಗಮ a;
}

ಶೂನ್ಯ IR::forward_buf() {
    std::vector<ಪೂರ್ಣಾಙ್ಕ> perm, alias;
    perm.clear();

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < _graph.size(); ++i)
    {
        auto ಗಂಟು = _graph[i];
        ಯದಿ (ಗಂಟು.function().fn() == Fn::buf && ಗಂಟು.arg(0).index() < i)
        {
            ಪೂರ್ಣಾಙ್ಕ target_index = alias[ಗಂಟು.arg(0).index()];
            auto target_ಗಂಟು = _graph[perm[target_index]];
			ಯದಿ(ಗಂಟು.has_sparse_attr()) {
				ಯದಿ(target_ಗಂಟು.has_sparse_attr()) {
					IdString id = merge_name(ಗಂಟು.sparse_attr(), target_ಗಂಟು.sparse_attr());
					target_ಗಂಟು.sparse_attr() = id;
				} ಅನ್ಯಥಾ {
					IdString id = ಗಂಟು.sparse_attr();
					target_ಗಂಟು.sparse_attr() = id;
				}
			}
            alias.ತಳ್ಳು_back(target_index);
        }
        ಅನ್ಯಥಾ
        {
            alias.ತಳ್ಳು_back(GetSize(perm));
            perm.ತಳ್ಳು_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
ಸ್ಥಿರ std::string quote_fmt(ಧ್ರುವ ವರ್ಣ *fmt)
{
	std::string r;
	ಪ್ರತಿ(ಧ್ರುವ ವರ್ಣ *p = fmt; *p != 0; p++) {
		ವಿಕಲ್ಪ(*p) {
		ಪಕ್ಷ '\n': r += "\\n"; ಭಙ್ಗ;
		ಪಕ್ಷ '\t': r += "\\t"; ಭಙ್ಗ;
		ಪಕ್ಷ '"': r += "\\\""; ಭಙ್ಗ;
		ಪಕ್ಷ '\\': r += "\\\\"; ಭಙ್ಗ;
		ಸ್ವಭಾವ: r += *p; ಭಙ್ಗ;
		}
	}
	ಪ್ರತ್ಯಾಗಮ r;
}

ಶೂನ್ಯ Writer::print_impl(ಧ್ರುವ ವರ್ಣ *fmt, vector<std::function<ಶೂನ್ಯ()>> &fns)
{
	size_t next_index = 0;
	ಪ್ರತಿ(ಧ್ರುವ ವರ್ಣ *p = fmt; *p != 0; p++)
		ವಿಕಲ್ಪ(*p) {
		ಪಕ್ಷ '{':
			ಯದಿ(*++p == '{') {
				*os << '{';
			} ಅನ್ಯಥಾ {
				ವರ್ಣ *pe;
				size_t index = strtoul(p, &pe, 10);
				ಯದಿ(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				ಯದಿ(p == pe)
					index = next_index;
				ಅನ್ಯಥಾ
					p = pe;
				ಯದಿ(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			ಭಙ್ಗ;
		ಪಕ್ಷ '}':
			p++;
			ಯದಿ(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			ಭಙ್ಗ;
		ಸ್ವಭಾವ:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
