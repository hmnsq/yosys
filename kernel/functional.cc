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

#شامل "kernel/functional.h"
#شامل "kernel/topo_scc.h"
#شامل "ff.h"
#شامل "ffinit.h"
#شامل <deque>

YOSYS_NAMESPACE_BEGIN
فضاینام Functional {

ثابت نویسه *fn_to_string(Fn fn) {
	انتخاب(fn) {
	حالت Fn::invalid: بازگشت "invalid";
	حالت Fn::buf: بازگشت "buf";
	حالت Fn::slice: بازگشت "slice";
	حالت Fn::zero_extend: بازگشت "zero_extend";
	حالت Fn::sign_extend: بازگشت "sign_extend";
	حالت Fn::concat: بازگشت "concat";
	حالت Fn::add: بازگشت "add";
	حالت Fn::sub: بازگشت "sub";
	حالت Fn::mul: بازگشت "mul";
	حالت Fn::unsigned_div: بازگشت "unsigned_div";
	حالت Fn::unsigned_mod: بازگشت "unsigned_mod";
	حالت Fn::bitwise_and: بازگشت "bitwise_and";
	حالت Fn::bitwise_or: بازگشت "bitwise_or";
	حالت Fn::bitwise_xor: بازگشت "bitwise_xor";
	حالت Fn::bitwise_not: بازگشت "bitwise_not";
	حالت Fn::reduce_and: بازگشت "reduce_and";
	حالت Fn::reduce_or: بازگشت "reduce_or";
	حالت Fn::reduce_xor: بازگشت "reduce_xor";
	حالت Fn::unary_minus: بازگشت "unary_minus";
	حالت Fn::equal: بازگشت "equal";
	حالت Fn::not_equal: بازگشت "not_equal";
	حالت Fn::signed_greater_than: بازگشت "signed_greater_than";
	حالت Fn::signed_greater_equal: بازگشت "signed_greater_equal";
	حالت Fn::unsigned_greater_than: بازگشت "unsigned_greater_than";
	حالت Fn::unsigned_greater_equal: بازگشت "unsigned_greater_equal";
	حالت Fn::logical_shift_چپ: بازگشت "logical_shift_left";
	حالت Fn::logical_shift_راست: بازگشت "logical_shift_right";
	حالت Fn::arithmetic_shift_راست: بازگشت "arithmetic_shift_right";
	حالت Fn::mux: بازگشت "mux";
	حالت Fn::constant: بازگشت "constant";
	حالت Fn::درون_داد: بازگشت "input";
	حالت Fn::state: بازگشت "state";
	حالت Fn::memory_read: بازگشت "memory_read";
	حالت Fn::memory_write: بازگشت "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحیح)fn);
}

vector<IRInput ثابت*> IR::inputs(IdString kind) ثابت {
	vector<IRInput ثابت*> ret;
	برای (ثابت auto &[name, درون_داد] : _inputs)
		اگر(درون_داد.kind == kind)
			ret.افزودن_back(&درون_داد);
	بازگشت ret;
}

vector<IROutput ثابت*> IR::outputs(IdString kind) ثابت {
	vector<IROutput ثابت*> ret;
	برای (ثابت auto &[name, output] : _outputs)
		اگر(output.kind == kind)
			ret.افزودن_back(&output);
	بازگشت ret;
}

vector<IRState ثابت*> IR::states(IdString kind) ثابت {
	vector<IRState ثابت*> ret;
	برای (ثابت auto &[name, state] : _states)
		اگر(state.kind == kind)
			ret.افزودن_back(&state);
	بازگشت ret;
}

vector<IRInput ثابت*> IR::all_inputs() ثابت {
	vector<IRInput ثابت*> ret;
	برای (ثابت auto &[name, درون_داد] : _inputs)
		ret.افزودن_back(&درون_داد);
	بازگشت ret;
}

vector<IROutput ثابت*> IR::all_outputs() ثابت {
	vector<IROutput ثابت*> ret;
	برای (ثابت auto &[name, output] : _outputs)
		ret.افزودن_back(&output);
	بازگشت ret;
}

vector<IRState ثابت*> IR::all_states() ثابت {
	vector<IRState ثابت*> ret;
	برای (ثابت auto &[name, state] : _states)
		ret.افزودن_back(&state);
	بازگشت ret;
}

ساختار PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(گره_2)> np;
	PrintVisitor(std::function<std::string(گره_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(گره_2, گره_2 a, صحیح offset, صحیح out_width) override { بازگشت "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(گره_2, گره_2 a, صحیح out_width) override { بازگشت "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(گره_2, گره_2 a, صحیح out_width) override { بازگشت "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(گره_2, RTLIL::Const ثابت& مقدار) override { بازگشت "constant(" + مقدار.as_string() + ")"; }
	std::string درون_داد(گره_2 self, IdString name, IdString kind) override { بازگشت "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(گره_2 self, IdString name, IdString kind) override { بازگشت "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(گره_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		برای(size_t i = 0; i < self.arg_شمار(); i++) {
			اگر(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		بازگشت ret;
	}
};

std::string گره_2::to_string()
{
	بازگشت to_string([](گره_2 n) { بازگشت n.name().unescape(); });
}

std::string گره_2::to_string(std::function<std::string(گره_2)> np)
{
	بازگشت visit(PrintVisitor(np));
}

رده CellSimplifier {
	Factory &factory;
	گره_2 sign(گره_2 a) {
		بازگشت factory.slice(a, a.width() - 1, 1);
	}
	گره_2 neg_if(گره_2 a, گره_2 s) {
		بازگشت factory.mux(a, factory.unary_minus(a), s);
	}
	گره_2 abs(گره_2 a) {
		بازگشت neg_if(a, sign(a));
	}
	گره_2 handle_shift(گره_2 a, گره_2 b, منطقی is_راست, منطقی is_signed) {
		// to prevent new_width == 0, we handle this case separately
		اگر(a.width() == 1) {
			اگر(!is_signed)
				بازگشت factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			وگرنه
				بازگشت a;
		}
		صحیح new_width = ceil_log2(a.width());
		گره_2 b_truncated = factory.extend(b, new_width, نادرست);
		گره_2 y =
			!is_راست ? factory.logical_shift_چپ(a, b_truncated) :
			!is_signed ? factory.logical_shift_راست(a, b_truncated) :
			factory.arithmetic_shift_راست(a, b_truncated);
		اگر(b.width() <= new_width)
			بازگشت y;
		گره_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		گره_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), درست) : factory.constant(RTLIL::Const(State::S0, a.width()));
		بازگشت factory.mux(y, y_if_overflow, overflow);
	}
عمومی:
	گره_2 logical_shift_چپ(گره_2 a, گره_2 b) { بازگشت handle_shift(a, b, نادرست, نادرست); }
	گره_2 logical_shift_راست(گره_2 a, گره_2 b) { بازگشت handle_shift(a, b, درست, نادرست); }
	گره_2 arithmetic_shift_راست(گره_2 a, گره_2 b) { بازگشت handle_shift(a, b, درست, درست); }
	گره_2 bitwise_mux(گره_2 a, گره_2 b, گره_2 s) {
		گره_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		گره_2 bb = factory.bitwise_and(b, s);
		بازگشت factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
خصوصی:
	گره_2 handle_pow(گره_2 a0, گره_2 b, صحیح y_width, منطقی is_signed) {
		گره_2 a = factory.extend(a0, y_width, is_signed);
		گره_2 r = factory.constant(Const(1, y_width));
		برای(صحیح i = 0; i < b.width(); i++) {
			گره_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		اگر (is_signed) {
			گره_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			گره_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		بازگشت r;
	}
	گره_2 handle_bmux(گره_2 a, گره_2 s, صحیح a_offset, صحیح width, صحیح sn) {
		اگر(sn < 1)
			بازگشت factory.slice(a, a_offset, width);
		وگرنه {
			گره_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			گره_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			بازگشت factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	گره_2 handle_pmux(گره_2 a, گره_2 b, گره_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		گره_2 y = a;
		برای(صحیح i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		بازگشت y;
	}
	dict<IdString, گره_2> handle_fa(گره_2 a, گره_2 b, گره_2 c) {
		گره_2 t1 = factory.bitwise_xor(a, b);
		گره_2 t2 = factory.bitwise_and(a, b);
		گره_2 t3 = factory.bitwise_and(c, t1);
		گره_2 y = factory.bitwise_xor(c, t1);
		گره_2 x = factory.bitwise_or(t2, t3);
		بازگشت {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, گره_2> handle_alu(گره_2 a_in, گره_2 b_in, صحیح y_width, منطقی is_signed, گره_2 ci, گره_2 bi) {
		گره_2 a = factory.extend(a_in, y_width, is_signed);
		گره_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		گره_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		گره_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		گره_2 a_extra = factory.extend(a, y_width + 1, نادرست);
		گره_2 b_extra = factory.extend(b, y_width + 1, نادرست);
		گره_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, نادرست));
		گره_2 y = factory.slice(y_extra, 0, y_width);
		گره_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		گره_2 co = factory.slice(carries, 1, y_width);
		بازگشت {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	گره_2 handle_lcu(گره_2 p, گره_2 g, گره_2 ci) {
		بازگشت handle_alu(g, factory.bitwise_or(p, g), g.width(), نادرست, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عمومی:
	std::variant<dict<IdString, گره_2>, گره_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, گره_2> inputs)
	{
		صحیح a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		صحیح b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		صحیح y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		منطقی a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقی b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		اگر(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقی is_signed = a_signed && b_signed;
			گره_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			گره_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			اگر(cellType == ID($add))
				بازگشت factory.add(a, b);
			وگرنه اگر(cellType == ID($sub))
				بازگشت factory.sub(a, b);
			وگرنه اگر(cellType == ID($mul))
				بازگشت factory.mul(a, b);
			وگرنه اگر(cellType == ID($and))
				بازگشت factory.bitwise_and(a, b);
			وگرنه اگر(cellType == ID($or))
				بازگشت factory.bitwise_or(a, b);
			وگرنه اگر(cellType == ID($xor))
				بازگشت factory.bitwise_xor(a, b);
			وگرنه اگر(cellType == ID($xnor))
				بازگشت factory.bitwise_not(factory.bitwise_xor(a, b));
			وگرنه
				log_abort();
		}وگرنه اگر(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقی is_signed = a_signed && b_signed;
			صحیح width = max(a_width, b_width);
			گره_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			گره_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			اگر(cellType.in(ID($eq), ID($eqx)))
				بازگشت factory.extend(factory.equal(a, b), y_width, نادرست);
			وگرنه اگر(cellType.in(ID($ne), ID($nex)))
				بازگشت factory.extend(factory.not_equal(a, b), y_width, نادرست);
			وگرنه اگر(cellType == ID($lt))
				بازگشت factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, نادرست);
			وگرنه اگر(cellType == ID($le))
				بازگشت factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, نادرست);
			وگرنه اگر(cellType == ID($gt))
				بازگشت factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, نادرست);
			وگرنه اگر(cellType == ID($ge))
				بازگشت factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, نادرست);
			وگرنه
				log_abort();
		}وگرنه اگر(cellType.in(ID($logic_or), ID($logic_and))){
			گره_2 a = factory.reduce_or(inputs.at(ID(A)));
			گره_2 b = factory.reduce_or(inputs.at(ID(B)));
			گره_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			بازگشت factory.extend(y, y_width, نادرست);
		}وگرنه اگر(cellType == ID($not)){
			گره_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			بازگشت factory.bitwise_not(a);
		}وگرنه اگر(cellType == ID($pos)){
			بازگشت factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}وگرنه اگر(cellType == ID($barrier)){
			بازگشت inputs.at(ID(A));
		}وگرنه اگر(cellType == ID($neg)){
			گره_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			بازگشت factory.unary_minus(a);
		}وگرنه اگر(cellType == ID($logic_not)){
			گره_2 a = factory.reduce_or(inputs.at(ID(A)));
			گره_2 y = factory.bitwise_not(a);
			بازگشت factory.extend(y, y_width, نادرست);
		}وگرنه اگر(cellType.in(ID($reduce_or), ID($reduce_bool))){
			گره_2 a = factory.reduce_or(inputs.at(ID(A)));
			بازگشت factory.extend(a, y_width, نادرست);
		}وگرنه اگر(cellType == ID($reduce_and)){
			گره_2 a = factory.reduce_and(inputs.at(ID(A)));
			بازگشت factory.extend(a, y_width, نادرست);
		}وگرنه اگر(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			گره_2 a = factory.reduce_xor(inputs.at(ID(A)));
			گره_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			بازگشت factory.extend(y, y_width, نادرست);
		}وگرنه اگر(cellType == ID($shl) || cellType == ID($sshl)){
			گره_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			گره_2 b = inputs.at(ID(B));
			بازگشت logical_shift_چپ(a, b);
		}وگرنه اگر(cellType == ID($shr) || cellType == ID($sshr)){
			صحیح width = max(a_width, y_width);
			گره_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			گره_2 b = inputs.at(ID(B));
			گره_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_راست(a, b) :
				logical_shift_راست(a, b);
			بازگشت factory.extend(y, y_width, a_signed);
		}وگرنه اگر(cellType == ID($shiftx) || cellType == ID($shift)){
			صحیح width = max(a_width, y_width);
			گره_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			گره_2 b = inputs.at(ID(B));
			گره_2 shr = logical_shift_راست(a, b);
			اگر(b_signed) {
				گره_2 shl = logical_shift_چپ(a, factory.unary_minus(b));
				گره_2 y = factory.mux(shr, shl, sign(b));
				بازگشت factory.extend(y, y_width, نادرست);
			} وگرنه {
				بازگشت factory.extend(shr, y_width, نادرست);
			}
		}وگرنه اگر(cellType == ID($mux)){
			بازگشت factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وگرنه اگر(cellType == ID($pmux)){
			بازگشت handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وگرنه اگر(cellType == ID($concat)){
			گره_2 a = inputs.at(ID(A));
			گره_2 b = inputs.at(ID(B));
			بازگشت factory.concat(a, b);
		}وگرنه اگر(cellType == ID($slice)){
			صحیح offset = parameters.at(ID(OFFSET)).as_int();
			گره_2 a = inputs.at(ID(A));
			بازگشت factory.slice(a, offset, y_width);
		}وگرنه اگر(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحیح width = max(a_width, b_width);
			منطقی is_signed = a_signed && b_signed;
			گره_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			گره_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			اگر(is_signed) {
				اگر(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					گره_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					گره_2 out_sign = factory.not_equal(sign(a), sign(b));
					بازگشت neg_if(factory.extend(abs_y, y_width, نادرست), out_sign);
				} وگرنه اگر(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					گره_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					بازگشت neg_if(factory.extend(abs_y, y_width, نادرست), sign(a));
				} وگرنه اگر(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					گره_2 b_sign = sign(b);
					گره_2 a1 = neg_if(a, b_sign);
					گره_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					گره_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					گره_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					گره_2 y1 = factory.unsigned_div(a2, b1);
					گره_2 y2 = factory.extend(y1, y_width, نادرست);
					بازگشت factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} وگرنه اگر(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					گره_2 abs_b = abs(b);
					گره_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					گره_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					گره_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					گره_2 y = neg_if(y_flipped, sign(b));
					بازگشت factory.extend(y, y_width, درست);
				} وگرنه
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} وگرنه {
				اگر(cellType.in(ID($mod), ID($modfloor)))
					بازگشت factory.extend(factory.unsigned_mod(a, b), y_width, نادرست);
				وگرنه
					بازگشت factory.extend(factory.unsigned_div(a, b), y_width, نادرست);
			}
		} وگرنه اگر(cellType == ID($pow)) {
			بازگشت handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} وگرنه اگر (cellType == ID($lut)) {
			صحیح width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			بازگشت handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} وگرنه اگر (cellType == ID($bwmux)) {
			گره_2 a = inputs.at(ID(A));
			گره_2 b = inputs.at(ID(B));
			گره_2 s = inputs.at(ID(S));
			بازگشت factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} وگرنه اگر (cellType == ID($bweqx)) {
			گره_2 a = inputs.at(ID(A));
			گره_2 b = inputs.at(ID(B));
			بازگشت factory.bitwise_not(factory.bitwise_xor(a, b));
		} وگرنه اگر(cellType == ID($bmux)) {
			صحیح width = parameters.at(ID(WIDTH)).as_int();
			صحیح s_width = parameters.at(ID(S_WIDTH)).as_int();
			بازگشت handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} وگرنه اگر(cellType == ID($demux)) {
			صحیح width = parameters.at(ID(WIDTH)).as_int();
			صحیح s_width = parameters.at(ID(S_WIDTH)).as_int();
			صحیح y_width = width << s_width;
			صحیح b_width = ceil_log2(y_width);
			گره_2 a = factory.extend(inputs.at(ID(A)), y_width, نادرست);
			گره_2 s = factory.extend(inputs.at(ID(S)), b_width, نادرست);
			گره_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			بازگشت factory.logical_shift_چپ(a, b);
		} وگرنه اگر(cellType == ID($fa)) {
			بازگشت handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} وگرنه اگر(cellType == ID($lcu)) {
			بازگشت handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} وگرنه اگر(cellType == ID($alu)) {
			بازگشت handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} وگرنه اگر(cellType.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover))) {
			گره_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_مقدار(a);
			بازگشت {};
		} وگرنه اگر(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحیح width = parameters.at(ID(WIDTH)).as_int();
			auto &درون_داد = factory.add_درون_داد(cellName, cellType, Sort(width));
			بازگشت factory.مقدار(درون_داد);
		} وگرنه اگر(cellType == ID($initstate)) {
			اگر(factory.ir().has_state(ID($initstate), ID($state)))
				بازگشت factory.مقدار(factory.ir().state(ID($initstate)));
			وگرنه {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_مقدار(RTLIL::Const(State::S1, 1));
				state.set_next_مقدار(factory.constant(RTLIL::Const(State::S0, 1)));
				بازگشت factory.مقدار(state);
			}
		} وگرنه اگر(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} وگرنه {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

رده FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, گره_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, گره_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	گره_2 enqueue(DriveSpec ثابت &spec)
	{
		auto it = graph_nodes.find(spec);
		اگر(it == graph_nodes.end()){
			auto گره = factory.create_pending(spec.size());
			graph_nodes.درج({spec, گره});
			queue.emplace_back(spec);
			بازگشت گره;
		}وگرنه
			بازگشت it->second;
	}
	گره_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		اگر(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<گره_2> rv;
			برای(auto ثابت &[name, sigspec] : cell->connections())
				اگر(driver_map.celltypes.cell_output(cell->type, name)) {
					auto گره = factory.create_pending(sigspec.size());
					factory.suggest_name(گره, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, گره);
					اگر(name == port_name)
						rv = گره;
				}
			بازگشت *rv;
		} وگرنه
			بازگشت it->second;
	}
عمومی:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		برای (auto cell : module->cells()) {
			اگر (cell->type.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		برای (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			اگر (wire && wire->port_درون_داد) {
				factory.add_درون_داد(wire->name, ID($درون_داد), Sort(wire->width));
			}
			اگر (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_مقدار(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		برای (auto &mem : memories_vector) {
			اگر (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
خصوصی:
	گره_2 concatenate_read_results(Mem *mem, vector<گره_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		صحیح current = 0;
		برای(size_t i = 0; i < mem->rd_ports.size(); i++) {
			صحیح width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		گره_2 گره = results[0];
		برای(size_t i = 1; i < results.size(); i++)
			گره = factory.concat(گره, results[i]);
		بازگشت گره;
	}
	گره_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<گره_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_مقدار(MemContents(mem));
		گره_2 گره = factory.مقدار(state);
		برای (size_t i = 0; i < mem->wr_ports.size(); i++) {
			ثابت auto &wr = mem->wr_ports[i];
			اگر (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گره_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			گره_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			گره_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			گره_2 old_data = factory.memory_read(گره, addr);
			گره_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			گره = factory.memory_write(گره, addr, wr_data);
		}
		اگر (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		برای (size_t i = 0; i < mem->rd_ports.size(); i++) {
			ثابت auto &rd = mem->rd_ports[i];
			اگر (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گره_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.افزودن_back(factory.memory_read(گره, addr));
		}
		state.set_next_مقدار(گره);
		بازگشت concatenate_read_results(mem, read_results);
	}
	تهی process_cell(Cell *cell)
	{
		اگر (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			اگر (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			گره_2 گره = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), گره);
		} وگرنه اگر (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			اگر (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			گره_2 q_مقدار = factory.مقدار(state);
			factory.suggest_name(q_مقدار, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_مقدار);
			state.set_next_مقدار(enqueue(ff.sig_d));
			state.set_initial_مقدار(ff.val_init);
		} وگرنه {
			dict<IdString, گره_2> connections;
			IdString output_name; // for the single output case
			صحیح n_outputs = 0;
			برای(auto ثابت &[name, sigspec] : cell->connections()) {
				اگر(driver_map.celltypes.cell_درون_داد(cell->type, name) && sigspec.size() > 0)
					connections.درج({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				اگر(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, گره_2>, گره_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			اگر(auto *nodep = std::get_if<گره_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} وگرنه {
				برای(auto [name, گره] : std::get<dict<IdString, گره_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), گره);
			}
		}
	}
	تهی undriven(ثابت std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	تهی check_undriven(DriveSpec ثابت& spec, std::string ثابت& name) {
		برای(auto ثابت &chunk : spec.chunks())
			اگر(chunk.is_none())
				undriven(name);
	}
عمومی:
	تهی process_queue()
	{
		برای (; !queue.empty(); queue.برداشت_front()) {
			اگر(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				ادامه;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			گره_2 pending = graph_nodes.at(spec);

			اگر (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				گره_2 گره = enqueue(chunks[0]);
				برای(size_t i = 1; i < chunks.size(); i++)
					گره = factory.concat(گره, enqueue(chunks[i]));
				factory.update_pending(pending, گره);
			} وگرنه اگر (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				اگر (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					اگر (wire_chunk.is_whole()) {
						اگر (wire_chunk.wire->port_درون_داد) {
							گره_2 گره = factory.مقدار(factory.ir().درون_داد(wire_chunk.wire->name));
							factory.suggest_name(گره, wire_chunk.wire->name);
							factory.update_pending(pending, گره);
						} وگرنه {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							گره_2 گره = enqueue(driver);
							factory.suggest_name(گره, wire_chunk.wire->name);
							factory.update_pending(pending, گره);
						}
					} وگرنه {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						گره_2 گره = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, گره);
					}
				} وگرنه اگر (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					اگر (port_chunk.is_whole()) {
						اگر (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							گره_2 گره = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, گره);
						} وگرنه {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} وگرنه {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						گره_2 گره = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, گره);
					}
				} وگرنه اگر (chunk.is_constant()) {
					گره_2 گره = factory.constant(chunk.constant());
					factory.suggest_name(گره, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, گره);
				} وگرنه اگر (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} وگرنه اگر (chunk.is_none()) {
					undriven(log_signal(chunk));
				} وگرنه {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} وگرنه {
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
    بازگشت ir;
}

تهی IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    منطقی scc = نادرست;
    std::vector<صحیح> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](صحیح *begin, صحیح *end) {
        perm.درج(perm.end(), begin, end);
        اگر (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            برای (صحیح *i = begin; i != end; ++i) {
				گره_2 گره(_graph[*i], design);
                log("- %s = %s\n", گره.name().unescape(), گره.to_string());
			}
            log("\n");
            scc = درست;
        }
    });
	برای(ثابت auto &[name, state]: _states)
		اگر(state.has_next_مقدار())
			toposort.process(state.next_مقدار().id());
	برای(ثابت auto &[name, output]: _outputs)
		اگر(output.has_مقدار())
			toposort.process(output.مقدار().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    اگر(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ایستا IdString merge_name(IdString a, IdString b) {
	اگر(!a.isPublic() && b.isPublic())
		بازگشت b;
	وگرنه
		بازگشت a;
}

تهی IR::forward_buf() {
    std::vector<صحیح> perm, alias;
    perm.clear();

    برای (صحیح i = 0; i < _graph.size(); ++i)
    {
        auto گره = _graph[i];
        اگر (گره.function().fn() == Fn::buf && گره.arg(0).index() < i)
        {
            صحیح target_index = alias[گره.arg(0).index()];
            auto target_گره = _graph[perm[target_index]];
			اگر(گره.has_sparse_attr()) {
				اگر(target_گره.has_sparse_attr()) {
					IdString id = merge_name(گره.sparse_attr(), target_گره.sparse_attr());
					target_گره.sparse_attr() = id;
				} وگرنه {
					IdString id = گره.sparse_attr();
					target_گره.sparse_attr() = id;
				}
			}
            alias.افزودن_back(target_index);
        }
        وگرنه
        {
            alias.افزودن_back(GetSize(perm));
            perm.افزودن_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
ایستا std::string quote_fmt(ثابت نویسه *fmt)
{
	std::string r;
	برای(ثابت نویسه *p = fmt; *p != 0; p++) {
		انتخاب(*p) {
		حالت '\n': r += "\\n"; بشکن;
		حالت '\t': r += "\\t"; بشکن;
		حالت '"': r += "\\\""; بشکن;
		حالت '\\': r += "\\\\"; بشکن;
		پیشفرض: r += *p; بشکن;
		}
	}
	بازگشت r;
}

تهی Writer::print_impl(ثابت نویسه *fmt, vector<std::function<تهی()>> &fns)
{
	size_t next_index = 0;
	برای(ثابت نویسه *p = fmt; *p != 0; p++)
		انتخاب(*p) {
		حالت '{':
			اگر(*++p == '{') {
				*os << '{';
			} وگرنه {
				نویسه *pe;
				size_t index = strtoul(p, &pe, 10);
				اگر(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				اگر(p == pe)
					index = next_index;
				وگرنه
					p = pe;
				اگر(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			بشکن;
		حالت '}':
			p++;
			اگر(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			بشکن;
		پیشفرض:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
