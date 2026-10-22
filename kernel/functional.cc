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
	حالة Fn::invalid: החזר "invalid";
	حالة Fn::buf: החזר "buf";
	حالة Fn::slice: החזר "slice";
	حالة Fn::zero_extend: החזר "zero_extend";
	حالة Fn::sign_extend: החזר "sign_extend";
	حالة Fn::concat: החזר "concat";
	حالة Fn::add: החזר "add";
	حالة Fn::sub: החזר "sub";
	حالة Fn::mul: החזר "mul";
	حالة Fn::unsigned_div: החזר "unsigned_div";
	حالة Fn::unsigned_mod: החזר "unsigned_mod";
	حالة Fn::bitwise_and: החזר "bitwise_and";
	حالة Fn::bitwise_or: החזר "bitwise_or";
	حالة Fn::bitwise_xor: החזר "bitwise_xor";
	حالة Fn::bitwise_not: החזר "bitwise_not";
	حالة Fn::reduce_and: החזר "reduce_and";
	حالة Fn::reduce_or: החזר "reduce_or";
	حالة Fn::reduce_xor: החזר "reduce_xor";
	حالة Fn::unary_minus: החזר "unary_minus";
	حالة Fn::equal: החזר "equal";
	حالة Fn::not_equal: החזר "not_equal";
	حالة Fn::signed_greater_than: החזר "signed_greater_than";
	حالة Fn::signed_greater_equal: החזר "signed_greater_equal";
	حالة Fn::unsigned_greater_than: החזר "unsigned_greater_than";
	حالة Fn::unsigned_greater_equal: החזר "unsigned_greater_equal";
	حالة Fn::logical_shift_left: החזר "logical_shift_left";
	حالة Fn::logical_shift_right: החזר "logical_shift_right";
	حالة Fn::arithmetic_shift_right: החזר "arithmetic_shift_right";
	حالة Fn::mux: החזר "mux";
	حالة Fn::constant: החזר "constant";
	حالة Fn::input: החזר "input";
	حالة Fn::state: החזר "state";
	حالة Fn::memory_read: החזר "memory_read";
	حالة Fn::memory_write: החזר "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحيح)fn);
}

vector<IRInput ثابت*> IR::inputs(IdString kind) ثابت {
	vector<IRInput ثابت*> ret;
	עבור (ثابت auto &[name, input] : _inputs)
		אם(input.kind == kind)
			ret.push_back(&input);
	החזר ret;
}

vector<IROutput ثابت*> IR::outputs(IdString kind) ثابت {
	vector<IROutput ثابت*> ret;
	עבור (ثابت auto &[name, output] : _outputs)
		אם(output.kind == kind)
			ret.push_back(&output);
	החזר ret;
}

vector<IRState ثابت*> IR::states(IdString kind) ثابت {
	vector<IRState ثابت*> ret;
	עבור (ثابت auto &[name, state] : _states)
		אם(state.kind == kind)
			ret.push_back(&state);
	החזר ret;
}

vector<IRInput ثابت*> IR::all_inputs() ثابت {
	vector<IRInput ثابت*> ret;
	עבור (ثابت auto &[name, input] : _inputs)
		ret.push_back(&input);
	החזר ret;
}

vector<IROutput ثابت*> IR::all_outputs() ثابت {
	vector<IROutput ثابت*> ret;
	עבור (ثابت auto &[name, output] : _outputs)
		ret.push_back(&output);
	החזר ret;
}

vector<IRState ثابت*> IR::all_states() ثابت {
	vector<IRState ثابت*> ret;
	עבור (ثابت auto &[name, state] : _states)
		ret.push_back(&state);
	החזר ret;
}

بنية PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(Node)> np;
	PrintVisitor(std::function<std::string(Node)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(Node, Node a, صحيح offset, صحيح out_width) override { החזר "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(Node, Node a, صحيح out_width) override { החזר "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(Node, Node a, صحيح out_width) override { החזר "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(Node, RTLIL::Const ثابت& value) override { החזר "constant(" + value.as_string() + ")"; }
	std::string input(Node self, IdString name, IdString kind) override { החזר "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(Node self, IdString name, IdString kind) override { החזר "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(Node self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		עבור(size_t i = 0; i < self.arg_count(); i++) {
			אם(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		החזר ret;
	}
};

std::string Node::to_string()
{
	החזר to_string([](Node n) { החזר n.name().unescape(); });
}

std::string Node::to_string(std::function<std::string(Node)> np)
{
	החזר visit(PrintVisitor(np));
}

מחלקה CellSimplifier {
	Factory &factory;
	Node sign(Node a) {
		החזר factory.slice(a, a.width() - 1, 1);
	}
	Node neg_if(Node a, Node s) {
		החזר factory.mux(a, factory.unary_minus(a), s);
	}
	Node abs(Node a) {
		החזר neg_if(a, sign(a));
	}
	Node handle_shift(Node a, Node b, منطقي is_right, منطقي is_signed) {
		// to prevent new_width == 0, we handle this case separately
		אם(a.width() == 1) {
			אם(!is_signed)
				החזר factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			אחרת
				החזר a;
		}
		صحيح new_width = ceil_log2(a.width());
		Node b_truncated = factory.extend(b, new_width, שקר);
		Node y =
			!is_right ? factory.logical_shift_left(a, b_truncated) :
			!is_signed ? factory.logical_shift_right(a, b_truncated) :
			factory.arithmetic_shift_right(a, b_truncated);
		אם(b.width() <= new_width)
			החזר y;
		Node overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		Node y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), אמת) : factory.constant(RTLIL::Const(State::S0, a.width()));
		החזר factory.mux(y, y_if_overflow, overflow);
	}
عام:
	Node logical_shift_left(Node a, Node b) { החזר handle_shift(a, b, שקר, שקר); }
	Node logical_shift_right(Node a, Node b) { החזר handle_shift(a, b, אמת, שקר); }
	Node arithmetic_shift_right(Node a, Node b) { החזר handle_shift(a, b, אמת, אמת); }
	Node bitwise_mux(Node a, Node b, Node s) {
		Node aa = factory.bitwise_and(a, factory.bitwise_not(s));
		Node bb = factory.bitwise_and(b, s);
		החזר factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
خاص:
	Node handle_pow(Node a0, Node b, صحيح y_width, منطقي is_signed) {
		Node a = factory.extend(a0, y_width, is_signed);
		Node r = factory.constant(Const(1, y_width));
		עבור(صحيح i = 0; i < b.width(); i++) {
			Node b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		אם (is_signed) {
			Node a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			Node zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		החזר r;
	}
	Node handle_bmux(Node a, Node s, صحيح a_offset, صحيح width, صحيح sn) {
		אם(sn < 1)
			החזר factory.slice(a, a_offset, width);
		אחרת {
			Node y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			Node y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			החזר factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	Node handle_pmux(Node a, Node b, Node s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		Node y = a;
		עבור(صحيح i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		החזר y;
	}
	dict<IdString, Node> handle_fa(Node a, Node b, Node c) {
		Node t1 = factory.bitwise_xor(a, b);
		Node t2 = factory.bitwise_and(a, b);
		Node t3 = factory.bitwise_and(c, t1);
		Node y = factory.bitwise_xor(c, t1);
		Node x = factory.bitwise_or(t2, t3);
		החזר {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, Node> handle_alu(Node a_in, Node b_in, صحيح y_width, منطقي is_signed, Node ci, Node bi) {
		Node a = factory.extend(a_in, y_width, is_signed);
		Node b_uninverted = factory.extend(b_in, y_width, is_signed);
		Node b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		Node x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		Node a_extra = factory.extend(a, y_width + 1, שקר);
		Node b_extra = factory.extend(b, y_width + 1, שקר);
		Node y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, שקר));
		Node y = factory.slice(y_extra, 0, y_width);
		Node carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		Node co = factory.slice(carries, 1, y_width);
		החזר {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	Node handle_lcu(Node p, Node g, Node ci) {
		החזר handle_alu(g, factory.bitwise_or(p, g), g.width(), שקר, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عام:
	std::variant<dict<IdString, Node>, Node> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, Node> inputs)
	{
		صحيح a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		صحيح b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		صحيح y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		منطقي a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقي b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		אם(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقي is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			אם(cellType == ID($add))
				החזר factory.add(a, b);
			אחרת אם(cellType == ID($sub))
				החזר factory.sub(a, b);
			אחרת אם(cellType == ID($mul))
				החזר factory.mul(a, b);
			אחרת אם(cellType == ID($and))
				החזר factory.bitwise_and(a, b);
			אחרת אם(cellType == ID($or))
				החזר factory.bitwise_or(a, b);
			אחרת אם(cellType == ID($xor))
				החזר factory.bitwise_xor(a, b);
			אחרת אם(cellType == ID($xnor))
				החזר factory.bitwise_not(factory.bitwise_xor(a, b));
			אחרת
				log_abort();
		}אחרת אם(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقي is_signed = a_signed && b_signed;
			صحيح width = max(a_width, b_width);
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			אם(cellType.in(ID($eq), ID($eqx)))
				החזר factory.extend(factory.equal(a, b), y_width, שקר);
			אחרת אם(cellType.in(ID($ne), ID($nex)))
				החזר factory.extend(factory.not_equal(a, b), y_width, שקר);
			אחרת אם(cellType == ID($lt))
				החזר factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, שקר);
			אחרת אם(cellType == ID($le))
				החזר factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, שקר);
			אחרת אם(cellType == ID($gt))
				החזר factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, שקר);
			אחרת אם(cellType == ID($ge))
				החזר factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, שקר);
			אחרת
				log_abort();
		}אחרת אם(cellType.in(ID($logic_or), ID($logic_and))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node b = factory.reduce_or(inputs.at(ID(B)));
			Node y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			החזר factory.extend(y, y_width, שקר);
		}אחרת אם(cellType == ID($not)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			החזר factory.bitwise_not(a);
		}אחרת אם(cellType == ID($pos)){
			החזר factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}אחרת אם(cellType == ID($barrier)){
			החזר inputs.at(ID(A));
		}אחרת אם(cellType == ID($neg)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			החזר factory.unary_minus(a);
		}אחרת אם(cellType == ID($logic_not)){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node y = factory.bitwise_not(a);
			החזר factory.extend(y, y_width, שקר);
		}אחרת אם(cellType.in(ID($reduce_or), ID($reduce_bool))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			החזר factory.extend(a, y_width, שקר);
		}אחרת אם(cellType == ID($reduce_and)){
			Node a = factory.reduce_and(inputs.at(ID(A)));
			החזר factory.extend(a, y_width, שקר);
		}אחרת אם(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			Node a = factory.reduce_xor(inputs.at(ID(A)));
			Node y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			החזר factory.extend(y, y_width, שקר);
		}אחרת אם(cellType == ID($shl) || cellType == ID($sshl)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			Node b = inputs.at(ID(B));
			החזר logical_shift_left(a, b);
		}אחרת אם(cellType == ID($shr) || cellType == ID($sshr)){
			صحيح width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, a_signed);
			Node b = inputs.at(ID(B));
			Node y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_right(a, b) :
				logical_shift_right(a, b);
			החזר factory.extend(y, y_width, a_signed);
		}אחרת אם(cellType == ID($shiftx) || cellType == ID($shift)){
			صحيح width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			Node b = inputs.at(ID(B));
			Node shr = logical_shift_right(a, b);
			אם(b_signed) {
				Node shl = logical_shift_left(a, factory.unary_minus(b));
				Node y = factory.mux(shr, shl, sign(b));
				החזר factory.extend(y, y_width, שקר);
			} אחרת {
				החזר factory.extend(shr, y_width, שקר);
			}
		}אחרת אם(cellType == ID($mux)){
			החזר factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}אחרת אם(cellType == ID($pmux)){
			החזר handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}אחרת אם(cellType == ID($concat)){
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			החזר factory.concat(a, b);
		}אחרת אם(cellType == ID($slice)){
			صحيح offset = parameters.at(ID(OFFSET)).as_int();
			Node a = inputs.at(ID(A));
			החזר factory.slice(a, offset, y_width);
		}אחרת אם(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحيح width = max(a_width, b_width);
			منطقي is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			אם(is_signed) {
				אם(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					Node abs_y = factory.unsigned_div(abs(a), abs(b));
					Node out_sign = factory.not_equal(sign(a), sign(b));
					החזר neg_if(factory.extend(abs_y, y_width, שקר), out_sign);
				} אחרת אם(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					Node abs_y = factory.unsigned_mod(abs(a), abs(b));
					החזר neg_if(factory.extend(abs_y, y_width, שקר), sign(a));
				} אחרת אם(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					Node b_sign = sign(b);
					Node a1 = neg_if(a, b_sign);
					Node b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					Node a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					Node a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					Node y1 = factory.unsigned_div(a2, b1);
					Node y2 = factory.extend(y1, y_width, שקר);
					החזר factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} אחרת אם(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					Node abs_b = abs(b);
					Node abs_y = factory.unsigned_mod(abs(a), abs_b);
					Node flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					Node y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					Node y = neg_if(y_flipped, sign(b));
					החזר factory.extend(y, y_width, אמת);
				} אחרת
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} אחרת {
				אם(cellType.in(ID($mod), ID($modfloor)))
					החזר factory.extend(factory.unsigned_mod(a, b), y_width, שקר);
				אחרת
					החזר factory.extend(factory.unsigned_div(a, b), y_width, שקר);
			}
		} אחרת אם(cellType == ID($pow)) {
			החזר handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} אחרת אם (cellType == ID($lut)) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			החזר handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} אחרת אם (cellType == ID($bwmux)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			Node s = inputs.at(ID(S));
			החזר factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} אחרת אם (cellType == ID($bweqx)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			החזר factory.bitwise_not(factory.bitwise_xor(a, b));
		} אחרת אם(cellType == ID($bmux)) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			صحيح s_width = parameters.at(ID(S_WIDTH)).as_int();
			החזר handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} אחרת אם(cellType == ID($demux)) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			صحيح s_width = parameters.at(ID(S_WIDTH)).as_int();
			صحيح y_width = width << s_width;
			صحيح b_width = ceil_log2(y_width);
			Node a = factory.extend(inputs.at(ID(A)), y_width, שקר);
			Node s = factory.extend(inputs.at(ID(S)), b_width, שקר);
			Node b = factory.mul(s, factory.constant(Const(width, b_width)));
			החזר factory.logical_shift_left(a, b);
		} אחרת אם(cellType == ID($fa)) {
			החזר handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} אחרת אם(cellType == ID($lcu)) {
			החזר handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} אחרת אם(cellType == ID($alu)) {
			החזר handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} אחרת אם(cellType.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover))) {
			Node a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_value(a);
			החזר {};
		} אחרת אם(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحيح width = parameters.at(ID(WIDTH)).as_int();
			auto &input = factory.add_input(cellName, cellType, Sort(width));
			החזר factory.value(input);
		} אחרת אם(cellType == ID($initstate)) {
			אם(factory.ir().has_state(ID($initstate), ID($state)))
				החזר factory.value(factory.ir().state(ID($initstate)));
			אחרת {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_value(RTLIL::Const(State::S1, 1));
				state.set_next_value(factory.constant(RTLIL::Const(State::S0, 1)));
				החזר factory.value(state);
			}
		} אחרת אם(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} אחרת {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

מחלקה FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, Node> graph_nodes;
	dict<std::pair<Cell *, IdString>, Node> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	Node enqueue(DriveSpec ثابت &spec)
	{
		auto it = graph_nodes.find(spec);
		אם(it == graph_nodes.end()){
			auto node = factory.create_pending(spec.size());
			graph_nodes.insert({spec, node});
			queue.emplace_back(spec);
			החזר node;
		}אחרת
			החזר it->second;
	}
	Node enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		אם(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<Node> rv;
			עבור(auto ثابت &[name, sigspec] : cell->connections())
				אם(driver_map.celltypes.cell_output(cell->type, name)) {
					auto node = factory.create_pending(sigspec.size());
					factory.suggest_name(node, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, node);
					אם(name == port_name)
						rv = node;
				}
			החזר *rv;
		} אחרת
			החזר it->second;
	}
عام:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		עבור (auto cell : module->cells()) {
			אם (cell->type.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		עבור (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			אם (wire && wire->port_input) {
				factory.add_input(wire->name, ID($input), Sort(wire->width));
			}
			אם (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_value(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		עבור (auto &mem : memories_vector) {
			אם (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
خاص:
	Node concatenate_read_results(Mem *mem, vector<Node> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		صحيح current = 0;
		עבור(size_t i = 0; i < mem->rd_ports.size(); i++) {
			صحيح width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		Node node = results[0];
		עבור(size_t i = 1; i < results.size(); i++)
			node = factory.concat(node, results[i]);
		החזר node;
	}
	Node handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<Node> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_value(MemContents(mem));
		Node node = factory.value(state);
		עבור (size_t i = 0; i < mem->wr_ports.size(); i++) {
			ثابت auto &wr = mem->wr_ports[i];
			אם (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node en = enqueue(driver_map(DriveSpec(wr.en)));
			Node addr = enqueue(driver_map(DriveSpec(wr.addr)));
			Node new_data = enqueue(driver_map(DriveSpec(wr.data)));
			Node old_data = factory.memory_read(node, addr);
			Node wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			node = factory.memory_write(node, addr, wr_data);
		}
		אם (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		עבור (size_t i = 0; i < mem->rd_ports.size(); i++) {
			ثابت auto &rd = mem->rd_ports[i];
			אם (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.push_back(factory.memory_read(node, addr));
		}
		state.set_next_value(node);
		החזר concatenate_read_results(mem, read_results);
	}
	فراغ process_cell(Cell *cell)
	{
		אם (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			אם (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			Node node = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), node);
		} אחרת אם (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			אם (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			Node q_value = factory.value(state);
			factory.suggest_name(q_value, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_value);
			state.set_next_value(enqueue(ff.sig_d));
			state.set_initial_value(ff.val_init);
		} אחרת {
			dict<IdString, Node> connections;
			IdString output_name; // for the single output case
			صحيح n_outputs = 0;
			עבור(auto ثابت &[name, sigspec] : cell->connections()) {
				אם(driver_map.celltypes.cell_input(cell->type, name) && sigspec.size() > 0)
					connections.insert({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				אם(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, Node>, Node> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			אם(auto *nodep = std::get_if<Node>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} אחרת {
				עבור(auto [name, node] : std::get<dict<IdString, Node>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), node);
			}
		}
	}
	فراغ undriven(ثابت std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	فراغ check_undriven(DriveSpec ثابت& spec, std::string ثابت& name) {
		עבור(auto ثابت &chunk : spec.chunks())
			אם(chunk.is_none())
				undriven(name);
	}
عام:
	فراغ process_queue()
	{
		עבור (; !queue.empty(); queue.pop_front()) {
			אם(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				استمر;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			Node pending = graph_nodes.at(spec);

			אם (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				Node node = enqueue(chunks[0]);
				עבור(size_t i = 1; i < chunks.size(); i++)
					node = factory.concat(node, enqueue(chunks[i]));
				factory.update_pending(pending, node);
			} אחרת אם (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				אם (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					אם (wire_chunk.is_whole()) {
						אם (wire_chunk.wire->port_input) {
							Node node = factory.value(factory.ir().input(wire_chunk.wire->name));
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						} אחרת {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							Node node = enqueue(driver);
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						}
					} אחרת {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						Node node = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, node);
					}
				} אחרת אם (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					אם (port_chunk.is_whole()) {
						אם (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							Node node = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, node);
						} אחרת {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} אחרת {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						Node node = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, node);
					}
				} אחרת אם (chunk.is_constant()) {
					Node node = factory.constant(chunk.constant());
					factory.suggest_name(node, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, node);
				} אחרת אם (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} אחרת אם (chunk.is_none()) {
					undriven(log_signal(chunk));
				} אחרת {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} אחרת {
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
    החזר ir;
}

فراغ IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    منطقي scc = שקר;
    std::vector<صحيح> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](صحيح *begin, صحيح *end) {
        perm.insert(perm.end(), begin, end);
        אם (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            עבור (صحيح *i = begin; i != end; ++i) {
				Node node(_graph[*i], design);
                log("- %s = %s\n", node.name().unescape(), node.to_string());
			}
            log("\n");
            scc = אמת;
        }
    });
	עבור(ثابت auto &[name, state]: _states)
		אם(state.has_next_value())
			toposort.process(state.next_value().id());
	עבור(ثابت auto &[name, output]: _outputs)
		אם(output.has_value())
			toposort.process(output.value().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    אם(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ساكن IdString merge_name(IdString a, IdString b) {
	אם(!a.isPublic() && b.isPublic())
		החזר b;
	אחרת
		החזר a;
}

فراغ IR::forward_buf() {
    std::vector<صحيح> perm, alias;
    perm.clear();

    עבור (صحيح i = 0; i < _graph.size(); ++i)
    {
        auto node = _graph[i];
        אם (node.function().fn() == Fn::buf && node.arg(0).index() < i)
        {
            صحيح target_index = alias[node.arg(0).index()];
            auto target_node = _graph[perm[target_index]];
			אם(node.has_sparse_attr()) {
				אם(target_node.has_sparse_attr()) {
					IdString id = merge_name(node.sparse_attr(), target_node.sparse_attr());
					target_node.sparse_attr() = id;
				} אחרת {
					IdString id = node.sparse_attr();
					target_node.sparse_attr() = id;
				}
			}
            alias.push_back(target_index);
        }
        אחרת
        {
            alias.push_back(GetSize(perm));
            perm.push_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
ساكن std::string quote_fmt(ثابت حرف *fmt)
{
	std::string r;
	עבור(ثابت حرف *p = fmt; *p != 0; p++) {
		بدل(*p) {
		حالة '\n': r += "\\n"; اكسر;
		حالة '\t': r += "\\t"; اكسر;
		حالة '"': r += "\\\""; اكسر;
		حالة '\\': r += "\\\\"; اكسر;
		افتراضي: r += *p; اكسر;
		}
	}
	החזר r;
}

فراغ Writer::print_impl(ثابت حرف *fmt, vector<std::function<فراغ()>> &fns)
{
	size_t next_index = 0;
	עבור(ثابت حرف *p = fmt; *p != 0; p++)
		بدل(*p) {
		حالة '{':
			אם(*++p == '{') {
				*os << '{';
			} אחרת {
				حرف *pe;
				size_t index = strtoul(p, &pe, 10);
				אם(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				אם(p == pe)
					index = next_index;
				אחרת
					p = pe;
				אם(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			اكسر;
		حالة '}':
			p++;
			אם(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			اكسر;
		افتراضي:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
