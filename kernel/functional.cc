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

#ضمن "kernel/functional.h"
#ضمن "kernel/topo_scc.h"
#ضمن "ff.h"
#ضمن "ffinit.h"
#ضمن <deque>

YOSYS_NAMESPACE_BEGIN
نطاق Functional {

ثابت حرف *fn_to_string(Fn fn) {
	بدل(fn) {
	حالة Fn::invalid: أرجع "invalid";
	حالة Fn::buf: أرجع "buf";
	حالة Fn::slice: أرجع "slice";
	حالة Fn::zero_extend: أرجع "zero_extend";
	حالة Fn::sign_extend: أرجع "sign_extend";
	حالة Fn::concat: أرجع "concat";
	حالة Fn::add: أرجع "add";
	حالة Fn::sub: أرجع "sub";
	حالة Fn::mul: أرجع "mul";
	حالة Fn::unsigned_div: أرجع "unsigned_div";
	حالة Fn::unsigned_mod: أرجع "unsigned_mod";
	حالة Fn::bitwise_and: أرجع "bitwise_and";
	حالة Fn::bitwise_or: أرجع "bitwise_or";
	حالة Fn::bitwise_xor: أرجع "bitwise_xor";
	حالة Fn::bitwise_not: أرجع "bitwise_not";
	حالة Fn::reduce_and: أرجع "reduce_and";
	حالة Fn::reduce_or: أرجع "reduce_or";
	حالة Fn::reduce_xor: أرجع "reduce_xor";
	حالة Fn::unary_minus: أرجع "unary_minus";
	حالة Fn::equal: أرجع "equal";
	حالة Fn::not_equal: أرجع "not_equal";
	حالة Fn::signed_greater_than: أرجع "signed_greater_than";
	حالة Fn::signed_greater_equal: أرجع "signed_greater_equal";
	حالة Fn::unsigned_greater_than: أرجع "unsigned_greater_than";
	حالة Fn::unsigned_greater_equal: أرجع "unsigned_greater_equal";
	حالة Fn::logical_shift_هغو: أرجع "logical_shift_left";
	حالة Fn::logical_shift_دام: أرجع "logical_shift_right";
	حالة Fn::arithmetic_shift_دام: أرجع "arithmetic_shift_right";
	حالة Fn::mux: أرجع "mux";
	حالة Fn::constant: أرجع "constant";
	حالة Fn::شغرو: أرجع "input";
	حالة Fn::state: أرجع "state";
	حالة Fn::memory_read: أرجع "memory_read";
	حالة Fn::memory_write: أرجع "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحيح)fn);
}

vector<IRInput ثابت*> IR::inputs(IdString kind) ثابت {
	vector<IRInput ثابت*> ret;
	لكل (ثابت auto &[name, شغرو] : _inputs)
		إذا(شغرو.kind == kind)
			ret.تور_back(&شغرو);
	أرجع ret;
}

vector<IROutput ثابت*> IR::outputs(IdString kind) ثابت {
	vector<IROutput ثابت*> ret;
	لكل (ثابت auto &[name, output] : _outputs)
		إذا(output.kind == kind)
			ret.تور_back(&output);
	أرجع ret;
}

vector<IRState ثابت*> IR::states(IdString kind) ثابت {
	vector<IRState ثابت*> ret;
	لكل (ثابت auto &[name, state] : _states)
		إذا(state.kind == kind)
			ret.تور_back(&state);
	أرجع ret;
}

vector<IRInput ثابت*> IR::all_inputs() ثابت {
	vector<IRInput ثابت*> ret;
	لكل (ثابت auto &[name, شغرو] : _inputs)
		ret.تور_back(&شغرو);
	أرجع ret;
}

vector<IROutput ثابت*> IR::all_outputs() ثابت {
	vector<IROutput ثابت*> ret;
	لكل (ثابت auto &[name, output] : _outputs)
		ret.تور_back(&output);
	أرجع ret;
}

vector<IRState ثابت*> IR::all_states() ثابت {
	vector<IRState ثابت*> ret;
	لكل (ثابت auto &[name, state] : _states)
		ret.تور_back(&state);
	أرجع ret;
}

بنية بغ_Visitor : DefaultVisitor<std::string> {
	std::function<std::string(كلي_2)> np;
	بغ_Visitor(std::function<std::string(كلي_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(كلي_2, كلي_2 a, صحيح offset, صحيح out_width) override { أرجع "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(كلي_2, كلي_2 a, صحيح out_width) override { أرجع "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(كلي_2, كلي_2 a, صحيح out_width) override { أرجع "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(كلي_2, RTLIL::Const ثابت& درج) override { أرجع "constant(" + درج.as_string() + ")"; }
	std::string شغرو(كلي_2 self, IdString name, IdString kind) override { أرجع "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(كلي_2 self, IdString name, IdString kind) override { أرجع "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(كلي_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		لكل(size_t i = 0; i < self.arg_كدي(); i++) {
			إذا(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		أرجع ret;
	}
};

std::string كلي_2::to_string()
{
	أرجع to_string([](كلي_2 n) { أرجع n.name().unescape(); });
}

std::string كلي_2::to_string(std::function<std::string(كلي_2)> np)
{
	أرجع visit(بغ_Visitor(np));
}

صنف CellSimplifier {
	Factory &factory;
	كلي_2 sign(كلي_2 a) {
		أرجع factory.slice(a, a.width() - 1, 1);
	}
	كلي_2 neg_if(كلي_2 a, كلي_2 s) {
		أرجع factory.mux(a, factory.unary_minus(a), s);
	}
	كلي_2 abs(كلي_2 a) {
		أرجع neg_if(a, sign(a));
	}
	كلي_2 handle_shift(كلي_2 a, كلي_2 b, منطقي is_دام, منطقي is_signed) {
		// to prevent new_width == 0, we handle this case separately
		إذا(a.width() == 1) {
			إذا(!is_signed)
				أرجع factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			وإلا
				أرجع a;
		}
		صحيح new_width = ceil_log2(a.width());
		كلي_2 b_truncated = factory.extend(b, new_width, خطأ);
		كلي_2 y =
			!is_دام ? factory.logical_shift_هغو(a, b_truncated) :
			!is_signed ? factory.logical_shift_دام(a, b_truncated) :
			factory.arithmetic_shift_دام(a, b_truncated);
		إذا(b.width() <= new_width)
			أرجع y;
		كلي_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		كلي_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), true) : factory.constant(RTLIL::Const(State::S0, a.width()));
		أرجع factory.mux(y, y_if_overflow, overflow);
	}
عام:
	كلي_2 logical_shift_هغو(كلي_2 a, كلي_2 b) { أرجع handle_shift(a, b, خطأ, خطأ); }
	كلي_2 logical_shift_دام(كلي_2 a, كلي_2 b) { أرجع handle_shift(a, b, true, خطأ); }
	كلي_2 arithmetic_shift_دام(كلي_2 a, كلي_2 b) { أرجع handle_shift(a, b, true, true); }
	كلي_2 bitwise_mux(كلي_2 a, كلي_2 b, كلي_2 s) {
		كلي_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		كلي_2 bb = factory.bitwise_and(b, s);
		أرجع factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
خاص:
	كلي_2 handle_pow(كلي_2 a0, كلي_2 b, صحيح y_width, منطقي is_signed) {
		كلي_2 a = factory.extend(a0, y_width, is_signed);
		كلي_2 r = factory.constant(Const(1, y_width));
		لكل(صحيح i = 0; i < b.width(); i++) {
			كلي_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		إذا (is_signed) {
			كلي_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			كلي_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		أرجع r;
	}
	كلي_2 handle_bmux(كلي_2 a, كلي_2 s, صحيح a_offset, صحيح width, صحيح sn) {
		إذا(sn < 1)
			أرجع factory.slice(a, a_offset, width);
		وإلا {
			كلي_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			كلي_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			أرجع factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	كلي_2 handle_pmux(كلي_2 a, كلي_2 b, كلي_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		كلي_2 y = a;
		لكل(صحيح i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		أرجع y;
	}
	dict<IdString, كلي_2> handle_fa(كلي_2 a, كلي_2 b, كلي_2 c) {
		كلي_2 t1 = factory.bitwise_xor(a, b);
		كلي_2 t2 = factory.bitwise_and(a, b);
		كلي_2 t3 = factory.bitwise_and(c, t1);
		كلي_2 y = factory.bitwise_xor(c, t1);
		كلي_2 x = factory.bitwise_or(t2, t3);
		أرجع {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, كلي_2> handle_alu(كلي_2 a_in, كلي_2 b_in, صحيح y_width, منطقي is_signed, كلي_2 ci, كلي_2 bi) {
		كلي_2 a = factory.extend(a_in, y_width, is_signed);
		كلي_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		كلي_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		كلي_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		كلي_2 a_extra = factory.extend(a, y_width + 1, خطأ);
		كلي_2 b_extra = factory.extend(b, y_width + 1, خطأ);
		كلي_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, خطأ));
		كلي_2 y = factory.slice(y_extra, 0, y_width);
		كلي_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		كلي_2 co = factory.slice(carries, 1, y_width);
		أرجع {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	كلي_2 handle_lcu(كلي_2 p, كلي_2 g, كلي_2 ci) {
		أرجع handle_alu(g, factory.bitwise_or(p, g), g.width(), خطأ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عام:
	std::variant<dict<IdString, كلي_2>, كلي_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, كلي_2> inputs)
	{
		صحيح a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		صحيح b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		صحيح y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		منطقي a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقي b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		إذا(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقي is_signed = a_signed && b_signed;
			كلي_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			كلي_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			إذا(cellType == ID($add))
				أرجع factory.add(a, b);
			وإلا إذا(cellType == ID($sub))
				أرجع factory.sub(a, b);
			وإلا إذا(cellType == ID($mul))
				أرجع factory.mul(a, b);
			وإلا إذا(cellType == ID($and))
				أرجع factory.bitwise_and(a, b);
			وإلا إذا(cellType == ID($or))
				أرجع factory.bitwise_or(a, b);
			وإلا إذا(cellType == ID($xor))
				أرجع factory.bitwise_xor(a, b);
			وإلا إذا(cellType == ID($xnor))
				أرجع factory.bitwise_not(factory.bitwise_xor(a, b));
			وإلا
				log_abort();
		}وإلا إذا(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقي is_signed = a_signed && b_signed;
			صحيح width = max(a_width, b_width);
			كلي_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			كلي_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			إذا(cellType.in(ID($eq), ID($eqx)))
				أرجع factory.extend(factory.equal(a, b), y_width, خطأ);
			وإلا إذا(cellType.in(ID($ne), ID($nex)))
				أرجع factory.extend(factory.not_equal(a, b), y_width, خطأ);
			وإلا إذا(cellType == ID($lt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, خطأ);
			وإلا إذا(cellType == ID($le))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, خطأ);
			وإلا إذا(cellType == ID($gt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, خطأ);
			وإلا إذا(cellType == ID($ge))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, خطأ);
			وإلا
				log_abort();
		}وإلا إذا(cellType.in(ID($logic_or), ID($logic_and))){
			كلي_2 a = factory.reduce_or(inputs.at(ID(A)));
			كلي_2 b = factory.reduce_or(inputs.at(ID(B)));
			كلي_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			أرجع factory.extend(y, y_width, خطأ);
		}وإلا إذا(cellType == ID($not)){
			كلي_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			أرجع factory.bitwise_not(a);
		}وإلا إذا(cellType == ID($pos)){
			أرجع factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}وإلا إذا(cellType == ID($barrier)){
			أرجع inputs.at(ID(A));
		}وإلا إذا(cellType == ID($neg)){
			كلي_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			أرجع factory.unary_minus(a);
		}وإلا إذا(cellType == ID($logic_not)){
			كلي_2 a = factory.reduce_or(inputs.at(ID(A)));
			كلي_2 y = factory.bitwise_not(a);
			أرجع factory.extend(y, y_width, خطأ);
		}وإلا إذا(cellType.in(ID($reduce_or), ID($reduce_bool))){
			كلي_2 a = factory.reduce_or(inputs.at(ID(A)));
			أرجع factory.extend(a, y_width, خطأ);
		}وإلا إذا(cellType == ID($reduce_and)){
			كلي_2 a = factory.reduce_and(inputs.at(ID(A)));
			أرجع factory.extend(a, y_width, خطأ);
		}وإلا إذا(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			كلي_2 a = factory.reduce_xor(inputs.at(ID(A)));
			كلي_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			أرجع factory.extend(y, y_width, خطأ);
		}وإلا إذا(cellType == ID($shl) || cellType == ID($sshl)){
			كلي_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			كلي_2 b = inputs.at(ID(B));
			أرجع logical_shift_هغو(a, b);
		}وإلا إذا(cellType == ID($shr) || cellType == ID($sshr)){
			صحيح width = max(a_width, y_width);
			كلي_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			كلي_2 b = inputs.at(ID(B));
			كلي_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_دام(a, b) :
				logical_shift_دام(a, b);
			أرجع factory.extend(y, y_width, a_signed);
		}وإلا إذا(cellType == ID($shiftx) || cellType == ID($shift)){
			صحيح width = max(a_width, y_width);
			كلي_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			كلي_2 b = inputs.at(ID(B));
			كلي_2 shr = logical_shift_دام(a, b);
			إذا(b_signed) {
				كلي_2 shl = logical_shift_هغو(a, factory.unary_minus(b));
				كلي_2 y = factory.mux(shr, shl, sign(b));
				أرجع factory.extend(y, y_width, خطأ);
			} وإلا {
				أرجع factory.extend(shr, y_width, خطأ);
			}
		}وإلا إذا(cellType == ID($mux)){
			أرجع factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($pmux)){
			أرجع handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($concat)){
			كلي_2 a = inputs.at(ID(A));
			كلي_2 b = inputs.at(ID(B));
			أرجع factory.concat(a, b);
		}وإلا إذا(cellType == ID($slice)){
			صحيح offset = parameters.at(ID(OFFSET)).as_int();
			كلي_2 a = inputs.at(ID(A));
			أرجع factory.slice(a, offset, y_width);
		}وإلا إذا(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحيح width = max(a_width, b_width);
			منطقي is_signed = a_signed && b_signed;
			كلي_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			كلي_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			إذا(is_signed) {
				إذا(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					كلي_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					كلي_2 out_sign = factory.not_equal(sign(a), sign(b));
					أرجع neg_if(factory.extend(abs_y, y_width, خطأ), out_sign);
				} وإلا إذا(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					كلي_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					أرجع neg_if(factory.extend(abs_y, y_width, خطأ), sign(a));
				} وإلا إذا(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					كلي_2 b_sign = sign(b);
					كلي_2 a1 = neg_if(a, b_sign);
					كلي_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					كلي_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					كلي_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					كلي_2 y1 = factory.unsigned_div(a2, b1);
					كلي_2 y2 = factory.extend(y1, y_width, خطأ);
					أرجع factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} وإلا إذا(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					كلي_2 abs_b = abs(b);
					كلي_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					كلي_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					كلي_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					كلي_2 y = neg_if(y_flipped, sign(b));
					أرجع factory.extend(y, y_width, true);
				} وإلا
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} وإلا {
				إذا(cellType.in(ID($mod), ID($modfloor)))
					أرجع factory.extend(factory.unsigned_mod(a, b), y_width, خطأ);
				وإلا
					أرجع factory.extend(factory.unsigned_div(a, b), y_width, خطأ);
			}
		} وإلا إذا(cellType == ID($pow)) {
			أرجع handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} وإلا إذا (cellType == ID($lut)) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			أرجع handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} وإلا إذا (cellType == ID($bwmux)) {
			كلي_2 a = inputs.at(ID(A));
			كلي_2 b = inputs.at(ID(B));
			كلي_2 s = inputs.at(ID(S));
			أرجع factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} وإلا إذا (cellType == ID($bweqx)) {
			كلي_2 a = inputs.at(ID(A));
			كلي_2 b = inputs.at(ID(B));
			أرجع factory.bitwise_not(factory.bitwise_xor(a, b));
		} وإلا إذا(cellType == ID($bmux)) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			صحيح s_width = parameters.at(ID(S_WIDTH)).as_int();
			أرجع handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} وإلا إذا(cellType == ID($demux)) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			صحيح s_width = parameters.at(ID(S_WIDTH)).as_int();
			صحيح y_width = width << s_width;
			صحيح b_width = ceil_log2(y_width);
			كلي_2 a = factory.extend(inputs.at(ID(A)), y_width, خطأ);
			كلي_2 s = factory.extend(inputs.at(ID(S)), b_width, خطأ);
			كلي_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			أرجع factory.logical_shift_هغو(a, b);
		} وإلا إذا(cellType == ID($fa)) {
			أرجع handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} وإلا إذا(cellType == ID($lcu)) {
			أرجع handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} وإلا إذا(cellType == ID($alu)) {
			أرجع handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} وإلا إذا(cellType.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover))) {
			كلي_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_درج(a);
			أرجع {};
		} وإلا إذا(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			auto &شغرو = factory.add_شغرو(cellName, cellType, Sort(width));
			أرجع factory.درج(شغرو);
		} وإلا إذا(cellType == ID($initstate)) {
			إذا(factory.ir().has_state(ID($initstate), ID($state)))
				أرجع factory.درج(factory.ir().state(ID($initstate)));
			وإلا {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_درج(RTLIL::Const(State::S1, 1));
				state.set_next_درج(factory.constant(RTLIL::Const(State::S0, 1)));
				أرجع factory.درج(state);
			}
		} وإلا إذا(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} وإلا {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

صنف FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, كلي_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, كلي_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	كلي_2 enqueue(DriveSpec ثابت &spec)
	{
		auto it = graph_nodes.find(spec);
		إذا(it == graph_nodes.end()){
			auto كلي = factory.create_pending(spec.size());
			graph_nodes.سك({spec, كلي});
			queue.emplace_back(spec);
			أرجع كلي;
		}وإلا
			أرجع it->second;
	}
	كلي_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		إذا(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<كلي_2> rv;
			لكل(auto ثابت &[name, sigspec] : cell->connections())
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					auto كلي = factory.create_pending(sigspec.size());
					factory.suggest_name(كلي, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, كلي);
					إذا(name == port_name)
						rv = كلي;
				}
			أرجع *rv;
		} وإلا
			أرجع it->second;
	}
عام:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		لكل (auto cell : module->cells()) {
			إذا (cell->type.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		لكل (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			إذا (wire && wire->port_شغرو) {
				factory.add_شغرو(wire->name, ID($شغرو), Sort(wire->width));
			}
			إذا (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_درج(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		لكل (auto &mem : memories_vector) {
			إذا (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
خاص:
	كلي_2 concatenate_read_results(Mem *mem, vector<كلي_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		صحيح current = 0;
		لكل(size_t i = 0; i < mem->rd_ports.size(); i++) {
			صحيح width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		كلي_2 كلي = results[0];
		لكل(size_t i = 1; i < results.size(); i++)
			كلي = factory.concat(كلي, results[i]);
		أرجع كلي;
	}
	كلي_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<كلي_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_درج(MemContents(mem));
		كلي_2 كلي = factory.درج(state);
		لكل (size_t i = 0; i < mem->wr_ports.size(); i++) {
			ثابت auto &wr = mem->wr_ports[i];
			إذا (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			كلي_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			كلي_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			كلي_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			كلي_2 old_data = factory.memory_read(كلي, addr);
			كلي_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			كلي = factory.memory_write(كلي, addr, wr_data);
		}
		إذا (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		لكل (size_t i = 0; i < mem->rd_ports.size(); i++) {
			ثابت auto &rd = mem->rd_ports[i];
			إذا (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			كلي_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.تور_back(factory.memory_read(كلي, addr));
		}
		state.set_next_درج(كلي);
		أرجع concatenate_read_results(mem, read_results);
	}
	فراغ process_cell(Cell *cell)
	{
		إذا (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			إذا (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			كلي_2 كلي = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), كلي);
		} وإلا إذا (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			إذا (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			كلي_2 q_درج = factory.درج(state);
			factory.suggest_name(q_درج, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_درج);
			state.set_next_درج(enqueue(ff.sig_d));
			state.set_initial_درج(ff.val_init);
		} وإلا {
			dict<IdString, كلي_2> connections;
			IdString output_name; // for the single output case
			صحيح n_outputs = 0;
			لكل(auto ثابت &[name, sigspec] : cell->connections()) {
				إذا(driver_map.celltypes.cell_شغرو(cell->type, name) && sigspec.size() > 0)
					connections.سك({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, كلي_2>, كلي_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			إذا(auto *nodep = std::get_if<كلي_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} وإلا {
				لكل(auto [name, كلي] : std::get<dict<IdString, كلي_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), كلي);
			}
		}
	}
	فراغ undriven(ثابت std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	فراغ check_undriven(DriveSpec ثابت& spec, std::string ثابت& name) {
		لكل(auto ثابت &chunk : spec.chunks())
			إذا(chunk.is_none())
				undriven(name);
	}
عام:
	فراغ process_queue()
	{
		لكل (; !queue.empty(); queue.چرو_front()) {
			إذا(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				استمر;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			كلي_2 pending = graph_nodes.at(spec);

			إذا (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				كلي_2 كلي = enqueue(chunks[0]);
				لكل(size_t i = 1; i < chunks.size(); i++)
					كلي = factory.concat(كلي, enqueue(chunks[i]));
				factory.update_pending(pending, كلي);
			} وإلا إذا (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				إذا (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					إذا (wire_chunk.is_whole()) {
						إذا (wire_chunk.wire->port_شغرو) {
							كلي_2 كلي = factory.درج(factory.ir().شغرو(wire_chunk.wire->name));
							factory.suggest_name(كلي, wire_chunk.wire->name);
							factory.update_pending(pending, كلي);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							كلي_2 كلي = enqueue(driver);
							factory.suggest_name(كلي, wire_chunk.wire->name);
							factory.update_pending(pending, كلي);
						}
					} وإلا {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						كلي_2 كلي = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, كلي);
					}
				} وإلا إذا (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					إذا (port_chunk.is_whole()) {
						إذا (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							كلي_2 كلي = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, كلي);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} وإلا {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						كلي_2 كلي = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, كلي);
					}
				} وإلا إذا (chunk.is_constant()) {
					كلي_2 كلي = factory.constant(chunk.constant());
					factory.suggest_name(كلي, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, كلي);
				} وإلا إذا (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} وإلا إذا (chunk.is_none()) {
					undriven(log_signal(chunk));
				} وإلا {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} وإلا {
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
    أرجع ir;
}

فراغ IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    منطقي scc = خطأ;
    std::vector<صحيح> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](صحيح *begin, صحيح *end) {
        perm.سك(perm.end(), begin, end);
        إذا (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            لكل (صحيح *i = begin; i != end; ++i) {
				كلي_2 كلي(_graph[*i], design);
                log("- %s = %s\n", كلي.name().unescape(), كلي.to_string());
			}
            log("\n");
            scc = true;
        }
    });
	لكل(ثابت auto &[name, state]: _states)
		إذا(state.has_next_درج())
			toposort.process(state.next_درج().id());
	لكل(ثابت auto &[name, output]: _outputs)
		إذا(output.has_درج())
			toposort.process(output.درج().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    إذا(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ساكن IdString merge_name(IdString a, IdString b) {
	إذا(!a.isPublic() && b.isPublic())
		أرجع b;
	وإلا
		أرجع a;
}

فراغ IR::forward_buf() {
    std::vector<صحيح> perm, alias;
    perm.clear();

    لكل (صحيح i = 0; i < _graph.size(); ++i)
    {
        auto كلي = _graph[i];
        إذا (كلي.function().fn() == Fn::buf && كلي.arg(0).index() < i)
        {
            صحيح target_index = alias[كلي.arg(0).index()];
            auto target_كلي = _graph[perm[target_index]];
			إذا(كلي.has_sparse_attr()) {
				إذا(target_كلي.has_sparse_attr()) {
					IdString id = merge_name(كلي.sparse_attr(), target_كلي.sparse_attr());
					target_كلي.sparse_attr() = id;
				} وإلا {
					IdString id = كلي.sparse_attr();
					target_كلي.sparse_attr() = id;
				}
			}
            alias.تور_back(target_index);
        }
        وإلا
        {
            alias.تور_back(GetSize(perm));
            perm.تور_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
ساكن std::string quote_fmt(ثابت حرف *fmt)
{
	std::string r;
	لكل(ثابت حرف *p = fmt; *p != 0; p++) {
		بدل(*p) {
		حالة '\n': r += "\\n"; اكسر;
		حالة '\t': r += "\\t"; اكسر;
		حالة '"': r += "\\\""; اكسر;
		حالة '\\': r += "\\\\"; اكسر;
		افتراضي: r += *p; اكسر;
		}
	}
	أرجع r;
}

فراغ Writer::بغ_impl(ثابت حرف *fmt, vector<std::function<فراغ()>> &fns)
{
	size_t next_index = 0;
	لكل(ثابت حرف *p = fmt; *p != 0; p++)
		بدل(*p) {
		حالة '{':
			إذا(*++p == '{') {
				*os << '{';
			} وإلا {
				حرف *pe;
				size_t index = strtoul(p, &pe, 10);
				إذا(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				إذا(p == pe)
					index = next_index;
				وإلا
					p = pe;
				إذا(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			اكسر;
		حالة '}':
			p++;
			إذا(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			اكسر;
		افتراضي:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
