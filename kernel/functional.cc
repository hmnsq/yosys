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

#include "kernel/functional.h"
#include "kernel/topo_scc.h"
#include "ff.h"
#include "ffinit.h"
#include <deque>

YOSYS_NAMESPACE_BEGIN
namespace Functional {

const char *fn_to_string(Fn fn) {
	switch(fn) {
	case Fn::invalid: повернути "invalid";
	case Fn::buf: повернути "buf";
	case Fn::slice: повернути "slice";
	case Fn::zero_extend: повернути "zero_extend";
	case Fn::sign_extend: повернути "sign_extend";
	case Fn::concat: повернути "concat";
	case Fn::add: повернути "add";
	case Fn::sub: повернути "sub";
	case Fn::mul: повернути "mul";
	case Fn::unsigned_div: повернути "unsigned_div";
	case Fn::unsigned_mod: повернути "unsigned_mod";
	case Fn::bitwise_and: повернути "bitwise_and";
	case Fn::bitwise_or: повернути "bitwise_or";
	case Fn::bitwise_xor: повернути "bitwise_xor";
	case Fn::bitwise_not: повернути "bitwise_not";
	case Fn::reduce_and: повернути "reduce_and";
	case Fn::reduce_or: повернути "reduce_or";
	case Fn::reduce_xor: повернути "reduce_xor";
	case Fn::unary_minus: повернути "unary_minus";
	case Fn::equal: повернути "equal";
	case Fn::not_equal: повернути "not_equal";
	case Fn::signed_greater_than: повернути "signed_greater_than";
	case Fn::signed_greater_equal: повернути "signed_greater_equal";
	case Fn::unsigned_greater_than: повернути "unsigned_greater_than";
	case Fn::unsigned_greater_equal: повернути "unsigned_greater_equal";
	case Fn::logical_shift_left: повернути "logical_shift_left";
	case Fn::logical_shift_right: повернути "logical_shift_right";
	case Fn::arithmetic_shift_right: повернути "arithmetic_shift_right";
	case Fn::mux: повернути "mux";
	case Fn::constant: повернути "constant";
	case Fn::input: повернути "input";
	case Fn::state: повернути "state";
	case Fn::memory_read: повернути "memory_read";
	case Fn::memory_write: повернути "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (ціле)fn);
}

vector<IRInput const*> IR::inputs(IdString kind) const {
	vector<IRInput const*> ret;
	для (const auto &[name, input] : _inputs)
		якщо(input.kind == kind)
			ret.push_back(&input);
	повернути ret;
}

vector<IROutput const*> IR::outputs(IdString kind) const {
	vector<IROutput const*> ret;
	для (const auto &[name, output] : _outputs)
		якщо(output.kind == kind)
			ret.push_back(&output);
	повернути ret;
}

vector<IRState const*> IR::states(IdString kind) const {
	vector<IRState const*> ret;
	для (const auto &[name, state] : _states)
		якщо(state.kind == kind)
			ret.push_back(&state);
	повернути ret;
}

vector<IRInput const*> IR::all_inputs() const {
	vector<IRInput const*> ret;
	для (const auto &[name, input] : _inputs)
		ret.push_back(&input);
	повернути ret;
}

vector<IROutput const*> IR::all_outputs() const {
	vector<IROutput const*> ret;
	для (const auto &[name, output] : _outputs)
		ret.push_back(&output);
	повернути ret;
}

vector<IRState const*> IR::all_states() const {
	vector<IRState const*> ret;
	для (const auto &[name, state] : _states)
		ret.push_back(&state);
	повернути ret;
}

struct PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(Node)> np;
	PrintVisitor(std::function<std::string(Node)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(Node, Node a, ціле offset, ціле out_width) override { повернути "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(Node, Node a, ціле out_width) override { повернути "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(Node, Node a, ціле out_width) override { повернути "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(Node, RTLIL::Const const& value) override { повернути "constant(" + value.as_string() + ")"; }
	std::string input(Node self, IdString name, IdString kind) override { повернути "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(Node self, IdString name, IdString kind) override { повернути "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(Node self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		для(size_t i = 0; i < self.arg_count(); i++) {
			якщо(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		повернути ret;
	}
};

std::string Node::to_string()
{
	повернути to_string([](Node n) { повернути n.name().unescape(); });
}

std::string Node::to_string(std::function<std::string(Node)> np)
{
	повернути visit(PrintVisitor(np));
}

class CellSimplifier {
	Factory &factory;
	Node sign(Node a) {
		повернути factory.slice(a, a.width() - 1, 1);
	}
	Node neg_if(Node a, Node s) {
		повернути factory.mux(a, factory.unary_minus(a), s);
	}
	Node abs(Node a) {
		повернути neg_if(a, sign(a));
	}
	Node handle_shift(Node a, Node b, bool is_right, bool is_signed) {
		// to prevent new_width == 0, we handle this case separately
		якщо(a.width() == 1) {
			якщо(!is_signed)
				повернути factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			інакше
				повернути a;
		}
		ціле new_width = ceil_log2(a.width());
		Node b_truncated = factory.extend(b, new_width, false);
		Node y =
			!is_right ? factory.logical_shift_left(a, b_truncated) :
			!is_signed ? factory.logical_shift_right(a, b_truncated) :
			factory.arithmetic_shift_right(a, b_truncated);
		якщо(b.width() <= new_width)
			повернути y;
		Node overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		Node y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), true) : factory.constant(RTLIL::Const(State::S0, a.width()));
		повернути factory.mux(y, y_if_overflow, overflow);
	}
public:
	Node logical_shift_left(Node a, Node b) { повернути handle_shift(a, b, false, false); }
	Node logical_shift_right(Node a, Node b) { повернути handle_shift(a, b, true, false); }
	Node arithmetic_shift_right(Node a, Node b) { повернути handle_shift(a, b, true, true); }
	Node bitwise_mux(Node a, Node b, Node s) {
		Node aa = factory.bitwise_and(a, factory.bitwise_not(s));
		Node bb = factory.bitwise_and(b, s);
		повернути factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
private:
	Node handle_pow(Node a0, Node b, ціле y_width, bool is_signed) {
		Node a = factory.extend(a0, y_width, is_signed);
		Node r = factory.constant(Const(1, y_width));
		для(ціле i = 0; i < b.width(); i++) {
			Node b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		якщо (is_signed) {
			Node a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			Node zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		повернути r;
	}
	Node handle_bmux(Node a, Node s, ціле a_offset, ціле width, ціле sn) {
		якщо(sn < 1)
			повернути factory.slice(a, a_offset, width);
		інакше {
			Node y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			Node y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			повернути factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	Node handle_pmux(Node a, Node b, Node s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		Node y = a;
		для(ціле i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		повернути y;
	}
	dict<IdString, Node> handle_fa(Node a, Node b, Node c) {
		Node t1 = factory.bitwise_xor(a, b);
		Node t2 = factory.bitwise_and(a, b);
		Node t3 = factory.bitwise_and(c, t1);
		Node y = factory.bitwise_xor(c, t1);
		Node x = factory.bitwise_or(t2, t3);
		повернути {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, Node> handle_alu(Node a_in, Node b_in, ціле y_width, bool is_signed, Node ci, Node bi) {
		Node a = factory.extend(a_in, y_width, is_signed);
		Node b_uninverted = factory.extend(b_in, y_width, is_signed);
		Node b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		Node x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		Node a_extra = factory.extend(a, y_width + 1, false);
		Node b_extra = factory.extend(b, y_width + 1, false);
		Node y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, false));
		Node y = factory.slice(y_extra, 0, y_width);
		Node carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		Node co = factory.slice(carries, 1, y_width);
		повернути {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	Node handle_lcu(Node p, Node g, Node ci) {
		повернути handle_alu(g, factory.bitwise_or(p, g), g.width(), false, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
public:
	std::variant<dict<IdString, Node>, Node> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, Node> inputs)
	{
		ціле a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		ціле b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		ціле y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		bool a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		bool b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		якщо(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			якщо(cellType == ID($add))
				повернути factory.add(a, b);
			інакше якщо(cellType == ID($sub))
				повернути factory.sub(a, b);
			інакше якщо(cellType == ID($mul))
				повернути factory.mul(a, b);
			інакше якщо(cellType == ID($and))
				повернути factory.bitwise_and(a, b);
			інакше якщо(cellType == ID($or))
				повернути factory.bitwise_or(a, b);
			інакше якщо(cellType == ID($xor))
				повернути factory.bitwise_xor(a, b);
			інакше якщо(cellType == ID($xnor))
				повернути factory.bitwise_not(factory.bitwise_xor(a, b));
			інакше
				log_abort();
		}інакше якщо(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			bool is_signed = a_signed && b_signed;
			ціле width = max(a_width, b_width);
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			якщо(cellType.in(ID($eq), ID($eqx)))
				повернути factory.extend(factory.equal(a, b), y_width, false);
			інакше якщо(cellType.in(ID($ne), ID($nex)))
				повернути factory.extend(factory.not_equal(a, b), y_width, false);
			інакше якщо(cellType == ID($lt))
				повернути factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, false);
			інакше якщо(cellType == ID($le))
				повернути factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, false);
			інакше якщо(cellType == ID($gt))
				повернути factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, false);
			інакше якщо(cellType == ID($ge))
				повернути factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, false);
			інакше
				log_abort();
		}інакше якщо(cellType.in(ID($logic_or), ID($logic_and))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node b = factory.reduce_or(inputs.at(ID(B)));
			Node y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			повернути factory.extend(y, y_width, false);
		}інакше якщо(cellType == ID($not)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			повернути factory.bitwise_not(a);
		}інакше якщо(cellType == ID($pos)){
			повернути factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}інакше якщо(cellType == ID($barrier)){
			повернути inputs.at(ID(A));
		}інакше якщо(cellType == ID($neg)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			повернути factory.unary_minus(a);
		}інакше якщо(cellType == ID($logic_not)){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node y = factory.bitwise_not(a);
			повернути factory.extend(y, y_width, false);
		}інакше якщо(cellType.in(ID($reduce_or), ID($reduce_bool))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			повернути factory.extend(a, y_width, false);
		}інакше якщо(cellType == ID($reduce_and)){
			Node a = factory.reduce_and(inputs.at(ID(A)));
			повернути factory.extend(a, y_width, false);
		}інакше якщо(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			Node a = factory.reduce_xor(inputs.at(ID(A)));
			Node y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			повернути factory.extend(y, y_width, false);
		}інакше якщо(cellType == ID($shl) || cellType == ID($sshl)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			Node b = inputs.at(ID(B));
			повернути logical_shift_left(a, b);
		}інакше якщо(cellType == ID($shr) || cellType == ID($sshr)){
			ціле width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, a_signed);
			Node b = inputs.at(ID(B));
			Node y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_right(a, b) :
				logical_shift_right(a, b);
			повернути factory.extend(y, y_width, a_signed);
		}інакше якщо(cellType == ID($shiftx) || cellType == ID($shift)){
			ціле width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			Node b = inputs.at(ID(B));
			Node shr = logical_shift_right(a, b);
			якщо(b_signed) {
				Node shl = logical_shift_left(a, factory.unary_minus(b));
				Node y = factory.mux(shr, shl, sign(b));
				повернути factory.extend(y, y_width, false);
			} інакше {
				повернути factory.extend(shr, y_width, false);
			}
		}інакше якщо(cellType == ID($mux)){
			повернути factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}інакше якщо(cellType == ID($pmux)){
			повернути handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}інакше якщо(cellType == ID($concat)){
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			повернути factory.concat(a, b);
		}інакше якщо(cellType == ID($slice)){
			ціле offset = parameters.at(ID(OFFSET)).as_int();
			Node a = inputs.at(ID(A));
			повернути factory.slice(a, offset, y_width);
		}інакше якщо(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			ціле width = max(a_width, b_width);
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			якщо(is_signed) {
				якщо(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					Node abs_y = factory.unsigned_div(abs(a), abs(b));
					Node out_sign = factory.not_equal(sign(a), sign(b));
					повернути neg_if(factory.extend(abs_y, y_width, false), out_sign);
				} інакше якщо(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					Node abs_y = factory.unsigned_mod(abs(a), abs(b));
					повернути neg_if(factory.extend(abs_y, y_width, false), sign(a));
				} інакше якщо(cellType == ID($divfloor)) {
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
					Node y2 = factory.extend(y1, y_width, false);
					повернути factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} інакше якщо(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					Node abs_b = abs(b);
					Node abs_y = factory.unsigned_mod(abs(a), abs_b);
					Node flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					Node y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					Node y = neg_if(y_flipped, sign(b));
					повернути factory.extend(y, y_width, true);
				} інакше
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} інакше {
				якщо(cellType.in(ID($mod), ID($modfloor)))
					повернути factory.extend(factory.unsigned_mod(a, b), y_width, false);
				інакше
					повернути factory.extend(factory.unsigned_div(a, b), y_width, false);
			}
		} інакше якщо(cellType == ID($pow)) {
			повернути handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} інакше якщо (cellType == ID($lut)) {
			ціле width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			повернути handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} інакше якщо (cellType == ID($bwmux)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			Node s = inputs.at(ID(S));
			повернути factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} інакше якщо (cellType == ID($bweqx)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			повернути factory.bitwise_not(factory.bitwise_xor(a, b));
		} інакше якщо(cellType == ID($bmux)) {
			ціле width = parameters.at(ID(WIDTH)).as_int();
			ціле s_width = parameters.at(ID(S_WIDTH)).as_int();
			повернути handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} інакше якщо(cellType == ID($demux)) {
			ціле width = parameters.at(ID(WIDTH)).as_int();
			ціле s_width = parameters.at(ID(S_WIDTH)).as_int();
			ціле y_width = width << s_width;
			ціле b_width = ceil_log2(y_width);
			Node a = factory.extend(inputs.at(ID(A)), y_width, false);
			Node s = factory.extend(inputs.at(ID(S)), b_width, false);
			Node b = factory.mul(s, factory.constant(Const(width, b_width)));
			повернути factory.logical_shift_left(a, b);
		} інакше якщо(cellType == ID($fa)) {
			повернути handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} інакше якщо(cellType == ID($lcu)) {
			повернути handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} інакше якщо(cellType == ID($alu)) {
			повернути handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} інакше якщо(cellType.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover))) {
			Node a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_value(a);
			повернути {};
		} інакше якщо(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			ціле width = parameters.at(ID(WIDTH)).as_int();
			auto &input = factory.add_input(cellName, cellType, Sort(width));
			повернути factory.value(input);
		} інакше якщо(cellType == ID($initstate)) {
			якщо(factory.ir().has_state(ID($initstate), ID($state)))
				повернути factory.value(factory.ir().state(ID($initstate)));
			інакше {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_value(RTLIL::Const(State::S1, 1));
				state.set_next_value(factory.constant(RTLIL::Const(State::S0, 1)));
				повернути factory.value(state);
			}
		} інакше якщо(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} інакше {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

class FunctionalIRConstruction {
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

	Node enqueue(DriveSpec const &spec)
	{
		auto it = graph_nodes.find(spec);
		якщо(it == graph_nodes.end()){
			auto node = factory.create_pending(spec.size());
			graph_nodes.insert({spec, node});
			queue.emplace_back(spec);
			повернути node;
		}інакше
			повернути it->second;
	}
	Node enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		якщо(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<Node> rv;
			для(auto const &[name, sigspec] : cell->connections())
				якщо(driver_map.celltypes.cell_output(cell->type, name)) {
					auto node = factory.create_pending(sigspec.size());
					factory.suggest_name(node, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, node);
					якщо(name == port_name)
						rv = node;
				}
			повернути *rv;
		} інакше
			повернути it->second;
	}
public:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		для (auto cell : module->cells()) {
			якщо (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		для (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			якщо (wire && wire->port_input) {
				factory.add_input(wire->name, ID($input), Sort(wire->width));
			}
			якщо (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_value(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		для (auto &mem : memories_vector) {
			якщо (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
private:
	Node concatenate_read_results(Mem *mem, vector<Node> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		const SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		ціле current = 0;
		для(size_t i = 0; i < mem->rd_ports.size(); i++) {
			ціле width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		Node node = results[0];
		для(size_t i = 1; i < results.size(); i++)
			node = factory.concat(node, results[i]);
		повернути node;
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
		для (size_t i = 0; i < mem->wr_ports.size(); i++) {
			const auto &wr = mem->wr_ports[i];
			якщо (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node en = enqueue(driver_map(DriveSpec(wr.en)));
			Node addr = enqueue(driver_map(DriveSpec(wr.addr)));
			Node new_data = enqueue(driver_map(DriveSpec(wr.data)));
			Node old_data = factory.memory_read(node, addr);
			Node wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			node = factory.memory_write(node, addr, wr_data);
		}
		якщо (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		для (size_t i = 0; i < mem->rd_ports.size(); i++) {
			const auto &rd = mem->rd_ports[i];
			якщо (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.push_back(factory.memory_read(node, addr));
		}
		state.set_next_value(node);
		повернути concatenate_read_results(mem, read_results);
	}
	void process_cell(Cell *cell)
	{
		якщо (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			якщо (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			Node node = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), node);
		} інакше якщо (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			якщо (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			Node q_value = factory.value(state);
			factory.suggest_name(q_value, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_value);
			state.set_next_value(enqueue(ff.sig_d));
			state.set_initial_value(ff.val_init);
		} інакше {
			dict<IdString, Node> connections;
			IdString output_name; // for the single output case
			ціле n_outputs = 0;
			для(auto const &[name, sigspec] : cell->connections()) {
				якщо(driver_map.celltypes.cell_input(cell->type, name) && sigspec.size() > 0)
					connections.insert({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				якщо(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, Node>, Node> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			якщо(auto *nodep = std::get_if<Node>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} інакше {
				для(auto [name, node] : std::get<dict<IdString, Node>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), node);
			}
		}
	}
	void undriven(const std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	void check_undriven(DriveSpec const& spec, std::string const& name) {
		для(auto const &chunk : spec.chunks())
			якщо(chunk.is_none())
				undriven(name);
	}
public:
	void process_queue()
	{
		для (; !queue.empty(); queue.pop_front()) {
			якщо(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				continue;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			Node pending = graph_nodes.at(spec);

			якщо (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				Node node = enqueue(chunks[0]);
				для(size_t i = 1; i < chunks.size(); i++)
					node = factory.concat(node, enqueue(chunks[i]));
				factory.update_pending(pending, node);
			} інакше якщо (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				якщо (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					якщо (wire_chunk.is_whole()) {
						якщо (wire_chunk.wire->port_input) {
							Node node = factory.value(factory.ir().input(wire_chunk.wire->name));
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						} інакше {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							Node node = enqueue(driver);
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						}
					} інакше {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						Node node = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, node);
					}
				} інакше якщо (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					якщо (port_chunk.is_whole()) {
						якщо (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							Node node = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, node);
						} інакше {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} інакше {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						Node node = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, node);
					}
				} інакше якщо (chunk.is_constant()) {
					Node node = factory.constant(chunk.constant());
					factory.suggest_name(node, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, node);
				} інакше якщо (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} інакше якщо (chunk.is_none()) {
					undriven(log_signal(chunk));
				} інакше {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} інакше {
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
    повернути ir;
}

void IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    bool scc = false;
    std::vector<ціле> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](ціле *begin, ціле *end) {
        perm.insert(perm.end(), begin, end);
        якщо (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            для (ціле *i = begin; i != end; ++i) {
				Node node(_graph[*i], design);
                log("- %s = %s\n", node.name().unescape(), node.to_string());
			}
            log("\n");
            scc = true;
        }
    });
	для(const auto &[name, state]: _states)
		якщо(state.has_next_value())
			toposort.process(state.next_value().id());
	для(const auto &[name, output]: _outputs)
		якщо(output.has_value())
			toposort.process(output.value().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    якщо(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

static IdString merge_name(IdString a, IdString b) {
	якщо(!a.isPublic() && b.isPublic())
		повернути b;
	інакше
		повернути a;
}

void IR::forward_buf() {
    std::vector<ціле> perm, alias;
    perm.clear();

    для (ціле i = 0; i < _graph.size(); ++i)
    {
        auto node = _graph[i];
        якщо (node.function().fn() == Fn::buf && node.arg(0).index() < i)
        {
            ціле target_index = alias[node.arg(0).index()];
            auto target_node = _graph[perm[target_index]];
			якщо(node.has_sparse_attr()) {
				якщо(target_node.has_sparse_attr()) {
					IdString id = merge_name(node.sparse_attr(), target_node.sparse_attr());
					target_node.sparse_attr() = id;
				} інакше {
					IdString id = node.sparse_attr();
					target_node.sparse_attr() = id;
				}
			}
            alias.push_back(target_index);
        }
        інакше
        {
            alias.push_back(GetSize(perm));
            perm.push_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
static std::string quote_fmt(const char *fmt)
{
	std::string r;
	для(const char *p = fmt; *p != 0; p++) {
		switch(*p) {
		case '\n': r += "\\n"; break;
		case '\t': r += "\\t"; break;
		case '"': r += "\\\""; break;
		case '\\': r += "\\\\"; break;
		default: r += *p; break;
		}
	}
	повернути r;
}

void Writer::print_impl(const char *fmt, vector<std::function<void()>> &fns)
{
	size_t next_index = 0;
	для(const char *p = fmt; *p != 0; p++)
		switch(*p) {
		case '{':
			якщо(*++p == '{') {
				*os << '{';
			} інакше {
				char *pe;
				size_t index = strtoul(p, &pe, 10);
				якщо(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				якщо(p == pe)
					index = next_index;
				інакше
					p = pe;
				якщо(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			break;
		case '}':
			p++;
			якщо(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			break;
		default:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
