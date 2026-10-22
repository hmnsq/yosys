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

#অন্তর্ভাব "kernel/functional.h"
#অন্তর্ভাব "kernel/topo_scc.h"
#অন্তর্ভাব "ff.h"
#অন্তর্ভাব "ffinit.h"
#অন্তর্ভাব <deque>

YOSYS_NAMESPACE_BEGIN
নামাকাশ Functional {

ধ্রুব বর্ণ *fn_to_string(Fn fn) {
	বিকল্প(fn) {
	পক্ষ Fn::invalid: প্রত্যাগম "invalid";
	পক্ষ Fn::buf: প্রত্যাগম "buf";
	পক্ষ Fn::slice: প্রত্যাগম "slice";
	পক্ষ Fn::zero_extend: প্রত্যাগম "zero_extend";
	পক্ষ Fn::sign_extend: প্রত্যাগম "sign_extend";
	পক্ষ Fn::concat: প্রত্যাগম "concat";
	পক্ষ Fn::add: প্রত্যাগম "add";
	পক্ষ Fn::sub: প্রত্যাগম "sub";
	পক্ষ Fn::mul: প্রত্যাগম "mul";
	পক্ষ Fn::unsigned_div: প্রত্যাগম "unsigned_div";
	পক্ষ Fn::unsigned_mod: প্রত্যাগম "unsigned_mod";
	পক্ষ Fn::bitwise_and: প্রত্যাগম "bitwise_and";
	পক্ষ Fn::bitwise_or: প্রত্যাগম "bitwise_or";
	পক্ষ Fn::bitwise_xor: প্রত্যাগম "bitwise_xor";
	পক্ষ Fn::bitwise_not: প্রত্যাগম "bitwise_not";
	পক্ষ Fn::reduce_and: প্রত্যাগম "reduce_and";
	পক্ষ Fn::reduce_or: প্রত্যাগম "reduce_or";
	পক্ষ Fn::reduce_xor: প্রত্যাগম "reduce_xor";
	পক্ষ Fn::unary_minus: প্রত্যাগম "unary_minus";
	পক্ষ Fn::equal: প্রত্যাগম "equal";
	পক্ষ Fn::not_equal: প্রত্যাগম "not_equal";
	পক্ষ Fn::signed_greater_than: প্রত্যাগম "signed_greater_than";
	পক্ষ Fn::signed_greater_equal: প্রত্যাগম "signed_greater_equal";
	পক্ষ Fn::unsigned_greater_than: প্রত্যাগম "unsigned_greater_than";
	পক্ষ Fn::unsigned_greater_equal: প্রত্যাগম "unsigned_greater_equal";
	পক্ষ Fn::logical_shift_বাম: প্রত্যাগম "logical_shift_left";
	পক্ষ Fn::logical_shift_ডান: প্রত্যাগম "logical_shift_right";
	পক্ষ Fn::arithmetic_shift_ডান: প্রত্যাগম "arithmetic_shift_right";
	পক্ষ Fn::mux: প্রত্যাগম "mux";
	পক্ষ Fn::constant: প্রত্যাগম "constant";
	পক্ষ Fn::প্রবেশ: প্রত্যাগম "input";
	পক্ষ Fn::state: প্রত্যাগম "state";
	পক্ষ Fn::memory_read: প্রত্যাগম "memory_read";
	পক্ষ Fn::memory_write: প্রত্যাগম "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (পূর্ণাঙ্ক)fn);
}

vector<IRInput ধ্রুব*> IR::inputs(IdString kind) ধ্রুব {
	vector<IRInput ধ্রুব*> ret;
	প্রতি (ধ্রুব auto &[name, প্রবেশ] : _inputs)
		যদি(প্রবেশ.kind == kind)
			ret.ঠেলো_back(&প্রবেশ);
	প্রত্যাগম ret;
}

vector<IROutput ধ্রুব*> IR::outputs(IdString kind) ধ্রুব {
	vector<IROutput ধ্রুব*> ret;
	প্রতি (ধ্রুব auto &[name, output] : _outputs)
		যদি(output.kind == kind)
			ret.ঠেলো_back(&output);
	প্রত্যাগম ret;
}

vector<IRState ধ্রুব*> IR::states(IdString kind) ধ্রুব {
	vector<IRState ধ্রুব*> ret;
	প্রতি (ধ্রুব auto &[name, state] : _states)
		যদি(state.kind == kind)
			ret.ঠেলো_back(&state);
	প্রত্যাগম ret;
}

vector<IRInput ধ্রুব*> IR::all_inputs() ধ্রুব {
	vector<IRInput ধ্রুব*> ret;
	প্রতি (ধ্রুব auto &[name, প্রবেশ] : _inputs)
		ret.ঠেলো_back(&প্রবেশ);
	প্রত্যাগম ret;
}

vector<IROutput ধ্রুব*> IR::all_outputs() ধ্রুব {
	vector<IROutput ধ্রুব*> ret;
	প্রতি (ধ্রুব auto &[name, output] : _outputs)
		ret.ঠেলো_back(&output);
	প্রত্যাগম ret;
}

vector<IRState ধ্রুব*> IR::all_states() ধ্রুব {
	vector<IRState ধ্রুব*> ret;
	প্রতি (ধ্রুব auto &[name, state] : _states)
		ret.ঠেলো_back(&state);
	প্রত্যাগম ret;
}

সংরচনা PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(গ্রন্থি_2)> np;
	PrintVisitor(std::function<std::string(গ্রন্থি_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(গ্রন্থি_2, গ্রন্থি_2 a, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক out_width) override { প্রত্যাগম "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(গ্রন্থি_2, গ্রন্থি_2 a, পূর্ণাঙ্ক out_width) override { প্রত্যাগম "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(গ্রন্থি_2, গ্রন্থি_2 a, পূর্ণাঙ্ক out_width) override { প্রত্যাগম "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(গ্রন্থি_2, RTLIL::Const ধ্রুব& মান) override { প্রত্যাগম "constant(" + মান.as_string() + ")"; }
	std::string প্রবেশ(গ্রন্থি_2 self, IdString name, IdString kind) override { প্রত্যাগম "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(গ্রন্থি_2 self, IdString name, IdString kind) override { প্রত্যাগম "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(গ্রন্থি_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		প্রতি(size_t i = 0; i < self.arg_গুনতি(); i++) {
			যদি(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		প্রত্যাগম ret;
	}
};

std::string গ্রন্থি_2::to_string()
{
	প্রত্যাগম to_string([](গ্রন্থি_2 n) { প্রত্যাগম n.name().unescape(); });
}

std::string গ্রন্থি_2::to_string(std::function<std::string(গ্রন্থি_2)> np)
{
	প্রত্যাগম visit(PrintVisitor(np));
}

বর্গ CellSimplifier {
	Factory &factory;
	গ্রন্থি_2 sign(গ্রন্থি_2 a) {
		প্রত্যাগম factory.slice(a, a.width() - 1, 1);
	}
	গ্রন্থি_2 neg_if(গ্রন্থি_2 a, গ্রন্থি_2 s) {
		প্রত্যাগম factory.mux(a, factory.unary_minus(a), s);
	}
	গ্রন্থি_2 abs(গ্রন্থি_2 a) {
		প্রত্যাগম neg_if(a, sign(a));
	}
	গ্রন্থি_2 handle_shift(গ্রন্থি_2 a, গ্রন্থি_2 b, তার্কিক is_ডান, তার্কিক is_signed) {
		// to prevent new_width == 0, we handle this case separately
		যদি(a.width() == 1) {
			যদি(!is_signed)
				প্রত্যাগম factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			অন্যথা
				প্রত্যাগম a;
		}
		পূর্ণাঙ্ক new_width = ceil_log2(a.width());
		গ্রন্থি_2 b_truncated = factory.extend(b, new_width, অসত্য);
		গ্রন্থি_2 y =
			!is_ডান ? factory.logical_shift_বাম(a, b_truncated) :
			!is_signed ? factory.logical_shift_ডান(a, b_truncated) :
			factory.arithmetic_shift_ডান(a, b_truncated);
		যদি(b.width() <= new_width)
			প্রত্যাগম y;
		গ্রন্থি_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		গ্রন্থি_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), সত্য) : factory.constant(RTLIL::Const(State::S0, a.width()));
		প্রত্যাগম factory.mux(y, y_if_overflow, overflow);
	}
সার্বজনিক:
	গ্রন্থি_2 logical_shift_বাম(গ্রন্থি_2 a, গ্রন্থি_2 b) { প্রত্যাগম handle_shift(a, b, অসত্য, অসত্য); }
	গ্রন্থি_2 logical_shift_ডান(গ্রন্থি_2 a, গ্রন্থি_2 b) { প্রত্যাগম handle_shift(a, b, সত্য, অসত্য); }
	গ্রন্থি_2 arithmetic_shift_ডান(গ্রন্থি_2 a, গ্রন্থি_2 b) { প্রত্যাগম handle_shift(a, b, সত্য, সত্য); }
	গ্রন্থি_2 bitwise_mux(গ্রন্থি_2 a, গ্রন্থি_2 b, গ্রন্থি_2 s) {
		গ্রন্থি_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		গ্রন্থি_2 bb = factory.bitwise_and(b, s);
		প্রত্যাগম factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
গুপ্ত:
	গ্রন্থি_2 handle_pow(গ্রন্থি_2 a0, গ্রন্থি_2 b, পূর্ণাঙ্ক y_width, তার্কিক is_signed) {
		গ্রন্থি_2 a = factory.extend(a0, y_width, is_signed);
		গ্রন্থি_2 r = factory.constant(Const(1, y_width));
		প্রতি(পূর্ণাঙ্ক i = 0; i < b.width(); i++) {
			গ্রন্থি_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		যদি (is_signed) {
			গ্রন্থি_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			গ্রন্থি_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		প্রত্যাগম r;
	}
	গ্রন্থি_2 handle_bmux(গ্রন্থি_2 a, গ্রন্থি_2 s, পূর্ণাঙ্ক a_offset, পূর্ণাঙ্ক width, পূর্ণাঙ্ক sn) {
		যদি(sn < 1)
			প্রত্যাগম factory.slice(a, a_offset, width);
		অন্যথা {
			গ্রন্থি_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			গ্রন্থি_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			প্রত্যাগম factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	গ্রন্থি_2 handle_pmux(গ্রন্থি_2 a, গ্রন্থি_2 b, গ্রন্থি_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		গ্রন্থি_2 y = a;
		প্রতি(পূর্ণাঙ্ক i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		প্রত্যাগম y;
	}
	dict<IdString, গ্রন্থি_2> handle_fa(গ্রন্থি_2 a, গ্রন্থি_2 b, গ্রন্থি_2 c) {
		গ্রন্থি_2 t1 = factory.bitwise_xor(a, b);
		গ্রন্থি_2 t2 = factory.bitwise_and(a, b);
		গ্রন্থি_2 t3 = factory.bitwise_and(c, t1);
		গ্রন্থি_2 y = factory.bitwise_xor(c, t1);
		গ্রন্থি_2 x = factory.bitwise_or(t2, t3);
		প্রত্যাগম {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, গ্রন্থি_2> handle_alu(গ্রন্থি_2 a_in, গ্রন্থি_2 b_in, পূর্ণাঙ্ক y_width, তার্কিক is_signed, গ্রন্থি_2 ci, গ্রন্থি_2 bi) {
		গ্রন্থি_2 a = factory.extend(a_in, y_width, is_signed);
		গ্রন্থি_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		গ্রন্থি_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		গ্রন্থি_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		গ্রন্থি_2 a_extra = factory.extend(a, y_width + 1, অসত্য);
		গ্রন্থি_2 b_extra = factory.extend(b, y_width + 1, অসত্য);
		গ্রন্থি_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, অসত্য));
		গ্রন্থি_2 y = factory.slice(y_extra, 0, y_width);
		গ্রন্থি_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		গ্রন্থি_2 co = factory.slice(carries, 1, y_width);
		প্রত্যাগম {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	গ্রন্থি_2 handle_lcu(গ্রন্থি_2 p, গ্রন্থি_2 g, গ্রন্থি_2 ci) {
		প্রত্যাগম handle_alu(g, factory.bitwise_or(p, g), g.width(), অসত্য, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
সার্বজনিক:
	std::variant<dict<IdString, গ্রন্থি_2>, গ্রন্থি_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, গ্রন্থি_2> inputs)
	{
		পূর্ণাঙ্ক a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		পূর্ণাঙ্ক b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		পূর্ণাঙ্ক y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		তার্কিক a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		তার্কিক b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		যদি(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			তার্কিক is_signed = a_signed && b_signed;
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			গ্রন্থি_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			যদি(cellType == ID($add))
				প্রত্যাগম factory.add(a, b);
			অন্যথা যদি(cellType == ID($sub))
				প্রত্যাগম factory.sub(a, b);
			অন্যথা যদি(cellType == ID($mul))
				প্রত্যাগম factory.mul(a, b);
			অন্যথা যদি(cellType == ID($and))
				প্রত্যাগম factory.bitwise_and(a, b);
			অন্যথা যদি(cellType == ID($or))
				প্রত্যাগম factory.bitwise_or(a, b);
			অন্যথা যদি(cellType == ID($xor))
				প্রত্যাগম factory.bitwise_xor(a, b);
			অন্যথা যদি(cellType == ID($xnor))
				প্রত্যাগম factory.bitwise_not(factory.bitwise_xor(a, b));
			অন্যথা
				log_abort();
		}অন্যথা যদি(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			তার্কিক is_signed = a_signed && b_signed;
			পূর্ণাঙ্ক width = max(a_width, b_width);
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			গ্রন্থি_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			যদি(cellType.in(ID($eq), ID($eqx)))
				প্রত্যাগম factory.extend(factory.equal(a, b), y_width, অসত্য);
			অন্যথা যদি(cellType.in(ID($ne), ID($nex)))
				প্রত্যাগম factory.extend(factory.not_equal(a, b), y_width, অসত্য);
			অন্যথা যদি(cellType == ID($lt))
				প্রত্যাগম factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, অসত্য);
			অন্যথা যদি(cellType == ID($le))
				প্রত্যাগম factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, অসত্য);
			অন্যথা যদি(cellType == ID($gt))
				প্রত্যাগম factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, অসত্য);
			অন্যথা যদি(cellType == ID($ge))
				প্রত্যাগম factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, অসত্য);
			অন্যথা
				log_abort();
		}অন্যথা যদি(cellType.in(ID($logic_or), ID($logic_and))){
			গ্রন্থি_2 a = factory.reduce_or(inputs.at(ID(A)));
			গ্রন্থি_2 b = factory.reduce_or(inputs.at(ID(B)));
			গ্রন্থি_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			প্রত্যাগম factory.extend(y, y_width, অসত্য);
		}অন্যথা যদি(cellType == ID($not)){
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			প্রত্যাগম factory.bitwise_not(a);
		}অন্যথা যদি(cellType == ID($pos)){
			প্রত্যাগম factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}অন্যথা যদি(cellType == ID($barrier)){
			প্রত্যাগম inputs.at(ID(A));
		}অন্যথা যদি(cellType == ID($neg)){
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			প্রত্যাগম factory.unary_minus(a);
		}অন্যথা যদি(cellType == ID($logic_not)){
			গ্রন্থি_2 a = factory.reduce_or(inputs.at(ID(A)));
			গ্রন্থি_2 y = factory.bitwise_not(a);
			প্রত্যাগম factory.extend(y, y_width, অসত্য);
		}অন্যথা যদি(cellType.in(ID($reduce_or), ID($reduce_bool))){
			গ্রন্থি_2 a = factory.reduce_or(inputs.at(ID(A)));
			প্রত্যাগম factory.extend(a, y_width, অসত্য);
		}অন্যথা যদি(cellType == ID($reduce_and)){
			গ্রন্থি_2 a = factory.reduce_and(inputs.at(ID(A)));
			প্রত্যাগম factory.extend(a, y_width, অসত্য);
		}অন্যথা যদি(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			গ্রন্থি_2 a = factory.reduce_xor(inputs.at(ID(A)));
			গ্রন্থি_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			প্রত্যাগম factory.extend(y, y_width, অসত্য);
		}অন্যথা যদি(cellType == ID($shl) || cellType == ID($sshl)){
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			গ্রন্থি_2 b = inputs.at(ID(B));
			প্রত্যাগম logical_shift_বাম(a, b);
		}অন্যথা যদি(cellType == ID($shr) || cellType == ID($sshr)){
			পূর্ণাঙ্ক width = max(a_width, y_width);
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			গ্রন্থি_2 b = inputs.at(ID(B));
			গ্রন্থি_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_ডান(a, b) :
				logical_shift_ডান(a, b);
			প্রত্যাগম factory.extend(y, y_width, a_signed);
		}অন্যথা যদি(cellType == ID($shiftx) || cellType == ID($shift)){
			পূর্ণাঙ্ক width = max(a_width, y_width);
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			গ্রন্থি_2 b = inputs.at(ID(B));
			গ্রন্থি_2 shr = logical_shift_ডান(a, b);
			যদি(b_signed) {
				গ্রন্থি_2 shl = logical_shift_বাম(a, factory.unary_minus(b));
				গ্রন্থি_2 y = factory.mux(shr, shl, sign(b));
				প্রত্যাগম factory.extend(y, y_width, অসত্য);
			} অন্যথা {
				প্রত্যাগম factory.extend(shr, y_width, অসত্য);
			}
		}অন্যথা যদি(cellType == ID($mux)){
			প্রত্যাগম factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}অন্যথা যদি(cellType == ID($pmux)){
			প্রত্যাগম handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}অন্যথা যদি(cellType == ID($concat)){
			গ্রন্থি_2 a = inputs.at(ID(A));
			গ্রন্থি_2 b = inputs.at(ID(B));
			প্রত্যাগম factory.concat(a, b);
		}অন্যথা যদি(cellType == ID($slice)){
			পূর্ণাঙ্ক offset = parameters.at(ID(OFFSET)).as_int();
			গ্রন্থি_2 a = inputs.at(ID(A));
			প্রত্যাগম factory.slice(a, offset, y_width);
		}অন্যথা যদি(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			পূর্ণাঙ্ক width = max(a_width, b_width);
			তার্কিক is_signed = a_signed && b_signed;
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			গ্রন্থি_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			যদি(is_signed) {
				যদি(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					গ্রন্থি_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					গ্রন্থি_2 out_sign = factory.not_equal(sign(a), sign(b));
					প্রত্যাগম neg_if(factory.extend(abs_y, y_width, অসত্য), out_sign);
				} অন্যথা যদি(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					গ্রন্থি_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					প্রত্যাগম neg_if(factory.extend(abs_y, y_width, অসত্য), sign(a));
				} অন্যথা যদি(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					গ্রন্থি_2 b_sign = sign(b);
					গ্রন্থি_2 a1 = neg_if(a, b_sign);
					গ্রন্থি_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					গ্রন্থি_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					গ্রন্থি_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					গ্রন্থি_2 y1 = factory.unsigned_div(a2, b1);
					গ্রন্থি_2 y2 = factory.extend(y1, y_width, অসত্য);
					প্রত্যাগম factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} অন্যথা যদি(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					গ্রন্থি_2 abs_b = abs(b);
					গ্রন্থি_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					গ্রন্থি_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					গ্রন্থি_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					গ্রন্থি_2 y = neg_if(y_flipped, sign(b));
					প্রত্যাগম factory.extend(y, y_width, সত্য);
				} অন্যথা
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} অন্যথা {
				যদি(cellType.in(ID($mod), ID($modfloor)))
					প্রত্যাগম factory.extend(factory.unsigned_mod(a, b), y_width, অসত্য);
				অন্যথা
					প্রত্যাগম factory.extend(factory.unsigned_div(a, b), y_width, অসত্য);
			}
		} অন্যথা যদি(cellType == ID($pow)) {
			প্রত্যাগম handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} অন্যথা যদি (cellType == ID($lut)) {
			পূর্ণাঙ্ক width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			প্রত্যাগম handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} অন্যথা যদি (cellType == ID($bwmux)) {
			গ্রন্থি_2 a = inputs.at(ID(A));
			গ্রন্থি_2 b = inputs.at(ID(B));
			গ্রন্থি_2 s = inputs.at(ID(S));
			প্রত্যাগম factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} অন্যথা যদি (cellType == ID($bweqx)) {
			গ্রন্থি_2 a = inputs.at(ID(A));
			গ্রন্থি_2 b = inputs.at(ID(B));
			প্রত্যাগম factory.bitwise_not(factory.bitwise_xor(a, b));
		} অন্যথা যদি(cellType == ID($bmux)) {
			পূর্ণাঙ্ক width = parameters.at(ID(WIDTH)).as_int();
			পূর্ণাঙ্ক s_width = parameters.at(ID(S_WIDTH)).as_int();
			প্রত্যাগম handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} অন্যথা যদি(cellType == ID($demux)) {
			পূর্ণাঙ্ক width = parameters.at(ID(WIDTH)).as_int();
			পূর্ণাঙ্ক s_width = parameters.at(ID(S_WIDTH)).as_int();
			পূর্ণাঙ্ক y_width = width << s_width;
			পূর্ণাঙ্ক b_width = ceil_log2(y_width);
			গ্রন্থি_2 a = factory.extend(inputs.at(ID(A)), y_width, অসত্য);
			গ্রন্থি_2 s = factory.extend(inputs.at(ID(S)), b_width, অসত্য);
			গ্রন্থি_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			প্রত্যাগম factory.logical_shift_বাম(a, b);
		} অন্যথা যদি(cellType == ID($fa)) {
			প্রত্যাগম handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} অন্যথা যদি(cellType == ID($lcu)) {
			প্রত্যাগম handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} অন্যথা যদি(cellType == ID($alu)) {
			প্রত্যাগম handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} অন্যথা যদি(cellType.in(ID($প্রতিজ্ঞা), ID($assume), ID($live), ID($fair), ID($cover))) {
			গ্রন্থি_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_মান(a);
			প্রত্যাগম {};
		} অন্যথা যদি(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			পূর্ণাঙ্ক width = parameters.at(ID(WIDTH)).as_int();
			auto &প্রবেশ = factory.add_প্রবেশ(cellName, cellType, Sort(width));
			প্রত্যাগম factory.মান(প্রবেশ);
		} অন্যথা যদি(cellType == ID($initstate)) {
			যদি(factory.ir().has_state(ID($initstate), ID($state)))
				প্রত্যাগম factory.মান(factory.ir().state(ID($initstate)));
			অন্যথা {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_মান(RTLIL::Const(State::S1, 1));
				state.set_next_মান(factory.constant(RTLIL::Const(State::S0, 1)));
				প্রত্যাগম factory.মান(state);
			}
		} অন্যথা যদি(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} অন্যথা {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

বর্গ FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, গ্রন্থি_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, গ্রন্থি_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	গ্রন্থি_2 enqueue(DriveSpec ধ্রুব &spec)
	{
		auto it = graph_nodes.find(spec);
		যদি(it == graph_nodes.end()){
			auto গ্রন্থি = factory.create_pending(spec.size());
			graph_nodes.ঢোকাও({spec, গ্রন্থি});
			queue.emplace_back(spec);
			প্রত্যাগম গ্রন্থি;
		}অন্যথা
			প্রত্যাগম it->second;
	}
	গ্রন্থি_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		যদি(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<গ্রন্থি_2> rv;
			প্রতি(auto ধ্রুব &[name, sigspec] : cell->connections())
				যদি(driver_map.celltypes.cell_output(cell->type, name)) {
					auto গ্রন্থি = factory.create_pending(sigspec.size());
					factory.suggest_name(গ্রন্থি, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, গ্রন্থি);
					যদি(name == port_name)
						rv = গ্রন্থি;
				}
			প্রত্যাগম *rv;
		} অন্যথা
			প্রত্যাগম it->second;
	}
সার্বজনিক:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		প্রতি (auto cell : module->cells()) {
			যদি (cell->type.in(ID($প্রতিজ্ঞা), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		প্রতি (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			যদি (wire && wire->port_প্রবেশ) {
				factory.add_প্রবেশ(wire->name, ID($প্রবেশ), Sort(wire->width));
			}
			যদি (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_মান(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		প্রতি (auto &mem : memories_vector) {
			যদি (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
গুপ্ত:
	গ্রন্থি_2 concatenate_read_results(Mem *mem, vector<গ্রন্থি_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ধ্রুব SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		পূর্ণাঙ্ক current = 0;
		প্রতি(size_t i = 0; i < mem->rd_ports.size(); i++) {
			পূর্ণাঙ্ক width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		গ্রন্থি_2 গ্রন্থি = results[0];
		প্রতি(size_t i = 1; i < results.size(); i++)
			গ্রন্থি = factory.concat(গ্রন্থি, results[i]);
		প্রত্যাগম গ্রন্থি;
	}
	গ্রন্থি_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<গ্রন্থি_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_মান(MemContents(mem));
		গ্রন্থি_2 গ্রন্থি = factory.মান(state);
		প্রতি (size_t i = 0; i < mem->wr_ports.size(); i++) {
			ধ্রুব auto &wr = mem->wr_ports[i];
			যদি (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			গ্রন্থি_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			গ্রন্থি_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			গ্রন্থি_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			গ্রন্থি_2 old_data = factory.memory_read(গ্রন্থি, addr);
			গ্রন্থি_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			গ্রন্থি = factory.memory_write(গ্রন্থি, addr, wr_data);
		}
		যদি (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		প্রতি (size_t i = 0; i < mem->rd_ports.size(); i++) {
			ধ্রুব auto &rd = mem->rd_ports[i];
			যদি (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			গ্রন্থি_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.ঠেলো_back(factory.memory_read(গ্রন্থি, addr));
		}
		state.set_next_মান(গ্রন্থি);
		প্রত্যাগম concatenate_read_results(mem, read_results);
	}
	শূন্য process_cell(Cell *cell)
	{
		যদি (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			যদি (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			গ্রন্থি_2 গ্রন্থি = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), গ্রন্থি);
		} অন্যথা যদি (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			যদি (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			গ্রন্থি_2 q_মান = factory.মান(state);
			factory.suggest_name(q_মান, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_মান);
			state.set_next_মান(enqueue(ff.sig_d));
			state.set_initial_মান(ff.val_init);
		} অন্যথা {
			dict<IdString, গ্রন্থি_2> connections;
			IdString output_name; // for the single output case
			পূর্ণাঙ্ক n_outputs = 0;
			প্রতি(auto ধ্রুব &[name, sigspec] : cell->connections()) {
				যদি(driver_map.celltypes.cell_প্রবেশ(cell->type, name) && sigspec.size() > 0)
					connections.ঢোকাও({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				যদি(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, গ্রন্থি_2>, গ্রন্থি_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			যদি(auto *nodep = std::get_if<গ্রন্থি_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} অন্যথা {
				প্রতি(auto [name, গ্রন্থি] : std::get<dict<IdString, গ্রন্থি_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), গ্রন্থি);
			}
		}
	}
	শূন্য undriven(ধ্রুব std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	শূন্য check_undriven(DriveSpec ধ্রুব& spec, std::string ধ্রুব& name) {
		প্রতি(auto ধ্রুব &chunk : spec.chunks())
			যদি(chunk.is_none())
				undriven(name);
	}
সার্বজনিক:
	শূন্য process_queue()
	{
		প্রতি (; !queue.empty(); queue.বের_করো_front()) {
			যদি(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				অনুবর্ত;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			গ্রন্থি_2 pending = graph_nodes.at(spec);

			যদি (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				গ্রন্থি_2 গ্রন্থি = enqueue(chunks[0]);
				প্রতি(size_t i = 1; i < chunks.size(); i++)
					গ্রন্থি = factory.concat(গ্রন্থি, enqueue(chunks[i]));
				factory.update_pending(pending, গ্রন্থি);
			} অন্যথা যদি (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				যদি (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					যদি (wire_chunk.is_whole()) {
						যদি (wire_chunk.wire->port_প্রবেশ) {
							গ্রন্থি_2 গ্রন্থি = factory.মান(factory.ir().প্রবেশ(wire_chunk.wire->name));
							factory.suggest_name(গ্রন্থি, wire_chunk.wire->name);
							factory.update_pending(pending, গ্রন্থি);
						} অন্যথা {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							গ্রন্থি_2 গ্রন্থি = enqueue(driver);
							factory.suggest_name(গ্রন্থি, wire_chunk.wire->name);
							factory.update_pending(pending, গ্রন্থি);
						}
					} অন্যথা {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						গ্রন্থি_2 গ্রন্থি = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, গ্রন্থি);
					}
				} অন্যথা যদি (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					যদি (port_chunk.is_whole()) {
						যদি (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							গ্রন্থি_2 গ্রন্থি = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, গ্রন্থি);
						} অন্যথা {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} অন্যথা {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						গ্রন্থি_2 গ্রন্থি = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, গ্রন্থি);
					}
				} অন্যথা যদি (chunk.is_constant()) {
					গ্রন্থি_2 গ্রন্থি = factory.constant(chunk.constant());
					factory.suggest_name(গ্রন্থি, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, গ্রন্থি);
				} অন্যথা যদি (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} অন্যথা যদি (chunk.is_none()) {
					undriven(log_signal(chunk));
				} অন্যথা {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} অন্যথা {
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
    প্রত্যাগম ir;
}

শূন্য IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    তার্কিক scc = অসত্য;
    std::vector<পূর্ণাঙ্ক> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](পূর্ণাঙ্ক *begin, পূর্ণাঙ্ক *end) {
        perm.ঢোকাও(perm.end(), begin, end);
        যদি (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            প্রতি (পূর্ণাঙ্ক *i = begin; i != end; ++i) {
				গ্রন্থি_2 গ্রন্থি(_graph[*i], design);
                log("- %s = %s\n", গ্রন্থি.name().unescape(), গ্রন্থি.to_string());
			}
            log("\n");
            scc = সত্য;
        }
    });
	প্রতি(ধ্রুব auto &[name, state]: _states)
		যদি(state.has_next_মান())
			toposort.process(state.next_মান().id());
	প্রতি(ধ্রুব auto &[name, output]: _outputs)
		যদি(output.has_মান())
			toposort.process(output.মান().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    যদি(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

স্থির IdString merge_name(IdString a, IdString b) {
	যদি(!a.isPublic() && b.isPublic())
		প্রত্যাগম b;
	অন্যথা
		প্রত্যাগম a;
}

শূন্য IR::forward_buf() {
    std::vector<পূর্ণাঙ্ক> perm, alias;
    perm.clear();

    প্রতি (পূর্ণাঙ্ক i = 0; i < _graph.size(); ++i)
    {
        auto গ্রন্থি = _graph[i];
        যদি (গ্রন্থি.function().fn() == Fn::buf && গ্রন্থি.arg(0).index() < i)
        {
            পূর্ণাঙ্ক target_index = alias[গ্রন্থি.arg(0).index()];
            auto target_গ্রন্থি = _graph[perm[target_index]];
			যদি(গ্রন্থি.has_sparse_attr()) {
				যদি(target_গ্রন্থি.has_sparse_attr()) {
					IdString id = merge_name(গ্রন্থি.sparse_attr(), target_গ্রন্থি.sparse_attr());
					target_গ্রন্থি.sparse_attr() = id;
				} অন্যথা {
					IdString id = গ্রন্থি.sparse_attr();
					target_গ্রন্থি.sparse_attr() = id;
				}
			}
            alias.ঠেলো_back(target_index);
        }
        অন্যথা
        {
            alias.ঠেলো_back(GetSize(perm));
            perm.ঠেলো_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
স্থির std::string quote_fmt(ধ্রুব বর্ণ *fmt)
{
	std::string r;
	প্রতি(ধ্রুব বর্ণ *p = fmt; *p != 0; p++) {
		বিকল্প(*p) {
		পক্ষ '\n': r += "\\n"; ভঙ্গ;
		পক্ষ '\t': r += "\\t"; ভঙ্গ;
		পক্ষ '"': r += "\\\""; ভঙ্গ;
		পক্ষ '\\': r += "\\\\"; ভঙ্গ;
		স্বভাব: r += *p; ভঙ্গ;
		}
	}
	প্রত্যাগম r;
}

শূন্য Writer::print_impl(ধ্রুব বর্ণ *fmt, vector<std::function<শূন্য()>> &fns)
{
	size_t next_index = 0;
	প্রতি(ধ্রুব বর্ণ *p = fmt; *p != 0; p++)
		বিকল্প(*p) {
		পক্ষ '{':
			যদি(*++p == '{') {
				*os << '{';
			} অন্যথা {
				বর্ণ *pe;
				size_t index = strtoul(p, &pe, 10);
				যদি(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				যদি(p == pe)
					index = next_index;
				অন্যথা
					p = pe;
				যদি(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			ভঙ্গ;
		পক্ষ '}':
			p++;
			যদি(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			ভঙ্গ;
		স্বভাব:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
