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

const var_na *fn_to_string(Fn fn) {
	switch(fn) {
	pak_sa Fn::invalid: คืน "invalid";
	pak_sa Fn::buf: คืน "buf";
	pak_sa Fn::slice: คืน "slice";
	pak_sa Fn::zero_extend: คืน "zero_extend";
	pak_sa Fn::sign_extend: คืน "sign_extend";
	pak_sa Fn::concat: คืน "concat";
	pak_sa Fn::add: คืน "add";
	pak_sa Fn::sub: คืน "sub";
	pak_sa Fn::mul: คืน "mul";
	pak_sa Fn::unsigned_div: คืน "unsigned_div";
	pak_sa Fn::unsigned_mod: คืน "unsigned_mod";
	pak_sa Fn::bitwise_and: คืน "bitwise_and";
	pak_sa Fn::bitwise_or: คืน "bitwise_or";
	pak_sa Fn::bitwise_xor: คืน "bitwise_xor";
	pak_sa Fn::bitwise_not: คืน "bitwise_not";
	pak_sa Fn::reduce_and: คืน "reduce_and";
	pak_sa Fn::reduce_or: คืน "reduce_or";
	pak_sa Fn::reduce_xor: คืน "reduce_xor";
	pak_sa Fn::unary_minus: คืน "unary_minus";
	pak_sa Fn::equal: คืน "equal";
	pak_sa Fn::not_equal: คืน "not_equal";
	pak_sa Fn::signed_greater_than: คืน "signed_greater_than";
	pak_sa Fn::signed_greater_equal: คืน "signed_greater_equal";
	pak_sa Fn::unsigned_greater_than: คืน "unsigned_greater_than";
	pak_sa Fn::unsigned_greater_equal: คืน "unsigned_greater_equal";
	pak_sa Fn::logical_shift_left: คืน "logical_shift_left";
	pak_sa Fn::logical_shift_right: คืน "logical_shift_right";
	pak_sa Fn::arithmetic_shift_right: คืน "arithmetic_shift_right";
	pak_sa Fn::mux: คืน "mux";
	pak_sa Fn::constant: คืน "constant";
	pak_sa Fn::input: คืน "input";
	pak_sa Fn::state: คืน "state";
	pak_sa Fn::memory_read: คืน "memory_read";
	pak_sa Fn::memory_write: คืน "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (จำนวนเต็ม)fn);
}

vector<IRInput const*> IR::inputs(IdString kind) const {
	vector<IRInput const*> ret;
	สำหรับ (const auto &[name, input] : _inputs)
		ถ้า(input.kind == kind)
			ret.push_back(&input);
	คืน ret;
}

vector<IROutput const*> IR::outputs(IdString kind) const {
	vector<IROutput const*> ret;
	สำหรับ (const auto &[name, output] : _outputs)
		ถ้า(output.kind == kind)
			ret.push_back(&output);
	คืน ret;
}

vector<IRState const*> IR::states(IdString kind) const {
	vector<IRState const*> ret;
	สำหรับ (const auto &[name, state] : _states)
		ถ้า(state.kind == kind)
			ret.push_back(&state);
	คืน ret;
}

vector<IRInput const*> IR::all_inputs() const {
	vector<IRInput const*> ret;
	สำหรับ (const auto &[name, input] : _inputs)
		ret.push_back(&input);
	คืน ret;
}

vector<IROutput const*> IR::all_outputs() const {
	vector<IROutput const*> ret;
	สำหรับ (const auto &[name, output] : _outputs)
		ret.push_back(&output);
	คืน ret;
}

vector<IRState const*> IR::all_states() const {
	vector<IRState const*> ret;
	สำหรับ (const auto &[name, state] : _states)
		ret.push_back(&state);
	คืน ret;
}

sa_nracanaa PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(Node)> np;
	PrintVisitor(std::function<std::string(Node)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(Node, Node a, จำนวนเต็ม offset, จำนวนเต็ม out_width) override { คืน "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(Node, Node a, จำนวนเต็ม out_width) override { คืน "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(Node, Node a, จำนวนเต็ม out_width) override { คืน "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(Node, RTLIL::Const const& value) override { คืน "constant(" + value.as_string() + ")"; }
	std::string input(Node self, IdString name, IdString kind) override { คืน "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(Node self, IdString name, IdString kind) override { คืน "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(Node self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		สำหรับ(size_t i = 0; i < self.arg_count(); i++) {
			ถ้า(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		คืน ret;
	}
};

std::string Node::to_string()
{
	คืน to_string([](Node n) { คืน n.name().unescape(); });
}

std::string Node::to_string(std::function<std::string(Node)> np)
{
	คืน visit(PrintVisitor(np));
}

class CellSimplifier {
	Factory &factory;
	Node sign(Node a) {
		คืน factory.slice(a, a.width() - 1, 1);
	}
	Node neg_if(Node a, Node s) {
		คืน factory.mux(a, factory.unary_minus(a), s);
	}
	Node abs(Node a) {
		คืน neg_if(a, sign(a));
	}
	Node handle_shift(Node a, Node b, bool is_right, bool is_signed) {
		// to prevent new_width == 0, we handle this case separately
		ถ้า(a.width() == 1) {
			ถ้า(!is_signed)
				คืน factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			มิฉะนั้น
				คืน a;
		}
		จำนวนเต็ม new_width = ceil_log2(a.width());
		Node b_truncated = factory.extend(b, new_width, false);
		Node y =
			!is_right ? factory.logical_shift_left(a, b_truncated) :
			!is_signed ? factory.logical_shift_right(a, b_truncated) :
			factory.arithmetic_shift_right(a, b_truncated);
		ถ้า(b.width() <= new_width)
			คืน y;
		Node overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		Node y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), true) : factory.constant(RTLIL::Const(State::S0, a.width()));
		คืน factory.mux(y, y_if_overflow, overflow);
	}
public:
	Node logical_shift_left(Node a, Node b) { คืน handle_shift(a, b, false, false); }
	Node logical_shift_right(Node a, Node b) { คืน handle_shift(a, b, true, false); }
	Node arithmetic_shift_right(Node a, Node b) { คืน handle_shift(a, b, true, true); }
	Node bitwise_mux(Node a, Node b, Node s) {
		Node aa = factory.bitwise_and(a, factory.bitwise_not(s));
		Node bb = factory.bitwise_and(b, s);
		คืน factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
private:
	Node handle_pow(Node a0, Node b, จำนวนเต็ม y_width, bool is_signed) {
		Node a = factory.extend(a0, y_width, is_signed);
		Node r = factory.constant(Const(1, y_width));
		สำหรับ(จำนวนเต็ม i = 0; i < b.width(); i++) {
			Node b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		ถ้า (is_signed) {
			Node a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			Node zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		คืน r;
	}
	Node handle_bmux(Node a, Node s, จำนวนเต็ม a_offset, จำนวนเต็ม width, จำนวนเต็ม sn) {
		ถ้า(sn < 1)
			คืน factory.slice(a, a_offset, width);
		มิฉะนั้น {
			Node y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			Node y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			คืน factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	Node handle_pmux(Node a, Node b, Node s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		Node y = a;
		สำหรับ(จำนวนเต็ม i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		คืน y;
	}
	dict<IdString, Node> handle_fa(Node a, Node b, Node c) {
		Node t1 = factory.bitwise_xor(a, b);
		Node t2 = factory.bitwise_and(a, b);
		Node t3 = factory.bitwise_and(c, t1);
		Node y = factory.bitwise_xor(c, t1);
		Node x = factory.bitwise_or(t2, t3);
		คืน {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, Node> handle_alu(Node a_in, Node b_in, จำนวนเต็ม y_width, bool is_signed, Node ci, Node bi) {
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
		คืน {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	Node handle_lcu(Node p, Node g, Node ci) {
		คืน handle_alu(g, factory.bitwise_or(p, g), g.width(), false, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
public:
	std::variant<dict<IdString, Node>, Node> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, Node> inputs)
	{
		จำนวนเต็ม a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		จำนวนเต็ม b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		จำนวนเต็ม y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		bool a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		bool b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		ถ้า(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			ถ้า(cellType == ID($add))
				คืน factory.add(a, b);
			มิฉะนั้น ถ้า(cellType == ID($sub))
				คืน factory.sub(a, b);
			มิฉะนั้น ถ้า(cellType == ID($mul))
				คืน factory.mul(a, b);
			มิฉะนั้น ถ้า(cellType == ID($and))
				คืน factory.bitwise_and(a, b);
			มิฉะนั้น ถ้า(cellType == ID($or))
				คืน factory.bitwise_or(a, b);
			มิฉะนั้น ถ้า(cellType == ID($xor))
				คืน factory.bitwise_xor(a, b);
			มิฉะนั้น ถ้า(cellType == ID($xnor))
				คืน factory.bitwise_not(factory.bitwise_xor(a, b));
			มิฉะนั้น
				log_abort();
		}มิฉะนั้น ถ้า(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			bool is_signed = a_signed && b_signed;
			จำนวนเต็ม width = max(a_width, b_width);
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ถ้า(cellType.in(ID($eq), ID($eqx)))
				คืน factory.extend(factory.equal(a, b), y_width, false);
			มิฉะนั้น ถ้า(cellType.in(ID($ne), ID($nex)))
				คืน factory.extend(factory.not_equal(a, b), y_width, false);
			มิฉะนั้น ถ้า(cellType == ID($lt))
				คืน factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, false);
			มิฉะนั้น ถ้า(cellType == ID($le))
				คืน factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, false);
			มิฉะนั้น ถ้า(cellType == ID($gt))
				คืน factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, false);
			มิฉะนั้น ถ้า(cellType == ID($ge))
				คืน factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, false);
			มิฉะนั้น
				log_abort();
		}มิฉะนั้น ถ้า(cellType.in(ID($logic_or), ID($logic_and))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node b = factory.reduce_or(inputs.at(ID(B)));
			Node y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			คืน factory.extend(y, y_width, false);
		}มิฉะนั้น ถ้า(cellType == ID($not)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			คืน factory.bitwise_not(a);
		}มิฉะนั้น ถ้า(cellType == ID($pos)){
			คืน factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}มิฉะนั้น ถ้า(cellType == ID($barrier)){
			คืน inputs.at(ID(A));
		}มิฉะนั้น ถ้า(cellType == ID($neg)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			คืน factory.unary_minus(a);
		}มิฉะนั้น ถ้า(cellType == ID($logic_not)){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node y = factory.bitwise_not(a);
			คืน factory.extend(y, y_width, false);
		}มิฉะนั้น ถ้า(cellType.in(ID($reduce_or), ID($reduce_bool))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			คืน factory.extend(a, y_width, false);
		}มิฉะนั้น ถ้า(cellType == ID($reduce_and)){
			Node a = factory.reduce_and(inputs.at(ID(A)));
			คืน factory.extend(a, y_width, false);
		}มิฉะนั้น ถ้า(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			Node a = factory.reduce_xor(inputs.at(ID(A)));
			Node y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			คืน factory.extend(y, y_width, false);
		}มิฉะนั้น ถ้า(cellType == ID($shl) || cellType == ID($sshl)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			Node b = inputs.at(ID(B));
			คืน logical_shift_left(a, b);
		}มิฉะนั้น ถ้า(cellType == ID($shr) || cellType == ID($sshr)){
			จำนวนเต็ม width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, a_signed);
			Node b = inputs.at(ID(B));
			Node y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_right(a, b) :
				logical_shift_right(a, b);
			คืน factory.extend(y, y_width, a_signed);
		}มิฉะนั้น ถ้า(cellType == ID($shiftx) || cellType == ID($shift)){
			จำนวนเต็ม width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			Node b = inputs.at(ID(B));
			Node shr = logical_shift_right(a, b);
			ถ้า(b_signed) {
				Node shl = logical_shift_left(a, factory.unary_minus(b));
				Node y = factory.mux(shr, shl, sign(b));
				คืน factory.extend(y, y_width, false);
			} มิฉะนั้น {
				คืน factory.extend(shr, y_width, false);
			}
		}มิฉะนั้น ถ้า(cellType == ID($mux)){
			คืน factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}มิฉะนั้น ถ้า(cellType == ID($pmux)){
			คืน handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}มิฉะนั้น ถ้า(cellType == ID($concat)){
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			คืน factory.concat(a, b);
		}มิฉะนั้น ถ้า(cellType == ID($slice)){
			จำนวนเต็ม offset = parameters.at(ID(OFFSET)).as_int();
			Node a = inputs.at(ID(A));
			คืน factory.slice(a, offset, y_width);
		}มิฉะนั้น ถ้า(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			จำนวนเต็ม width = max(a_width, b_width);
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ถ้า(is_signed) {
				ถ้า(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					Node abs_y = factory.unsigned_div(abs(a), abs(b));
					Node out_sign = factory.not_equal(sign(a), sign(b));
					คืน neg_if(factory.extend(abs_y, y_width, false), out_sign);
				} มิฉะนั้น ถ้า(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					Node abs_y = factory.unsigned_mod(abs(a), abs(b));
					คืน neg_if(factory.extend(abs_y, y_width, false), sign(a));
				} มิฉะนั้น ถ้า(cellType == ID($divfloor)) {
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
					คืน factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} มิฉะนั้น ถ้า(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					Node abs_b = abs(b);
					Node abs_y = factory.unsigned_mod(abs(a), abs_b);
					Node flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					Node y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					Node y = neg_if(y_flipped, sign(b));
					คืน factory.extend(y, y_width, true);
				} มิฉะนั้น
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} มิฉะนั้น {
				ถ้า(cellType.in(ID($mod), ID($modfloor)))
					คืน factory.extend(factory.unsigned_mod(a, b), y_width, false);
				มิฉะนั้น
					คืน factory.extend(factory.unsigned_div(a, b), y_width, false);
			}
		} มิฉะนั้น ถ้า(cellType == ID($pow)) {
			คืน handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} มิฉะนั้น ถ้า (cellType == ID($lut)) {
			จำนวนเต็ม width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			คืน handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} มิฉะนั้น ถ้า (cellType == ID($bwmux)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			Node s = inputs.at(ID(S));
			คืน factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} มิฉะนั้น ถ้า (cellType == ID($bweqx)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			คืน factory.bitwise_not(factory.bitwise_xor(a, b));
		} มิฉะนั้น ถ้า(cellType == ID($bmux)) {
			จำนวนเต็ม width = parameters.at(ID(WIDTH)).as_int();
			จำนวนเต็ม s_width = parameters.at(ID(S_WIDTH)).as_int();
			คืน handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} มิฉะนั้น ถ้า(cellType == ID($demux)) {
			จำนวนเต็ม width = parameters.at(ID(WIDTH)).as_int();
			จำนวนเต็ม s_width = parameters.at(ID(S_WIDTH)).as_int();
			จำนวนเต็ม y_width = width << s_width;
			จำนวนเต็ม b_width = ceil_log2(y_width);
			Node a = factory.extend(inputs.at(ID(A)), y_width, false);
			Node s = factory.extend(inputs.at(ID(S)), b_width, false);
			Node b = factory.mul(s, factory.constant(Const(width, b_width)));
			คืน factory.logical_shift_left(a, b);
		} มิฉะนั้น ถ้า(cellType == ID($fa)) {
			คืน handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} มิฉะนั้น ถ้า(cellType == ID($lcu)) {
			คืน handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} มิฉะนั้น ถ้า(cellType == ID($alu)) {
			คืน handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} มิฉะนั้น ถ้า(cellType.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover))) {
			Node a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_value(a);
			คืน {};
		} มิฉะนั้น ถ้า(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			จำนวนเต็ม width = parameters.at(ID(WIDTH)).as_int();
			auto &input = factory.add_input(cellName, cellType, Sort(width));
			คืน factory.value(input);
		} มิฉะนั้น ถ้า(cellType == ID($initstate)) {
			ถ้า(factory.ir().has_state(ID($initstate), ID($state)))
				คืน factory.value(factory.ir().state(ID($initstate)));
			มิฉะนั้น {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_value(RTLIL::Const(State::S1, 1));
				state.set_next_value(factory.constant(RTLIL::Const(State::S0, 1)));
				คืน factory.value(state);
			}
		} มิฉะนั้น ถ้า(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} มิฉะนั้น {
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
		ถ้า(it == graph_nodes.end()){
			auto node = factory.create_pending(spec.size());
			graph_nodes.insert({spec, node});
			queue.emplace_back(spec);
			คืน node;
		}มิฉะนั้น
			คืน it->second;
	}
	Node enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		ถ้า(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<Node> rv;
			สำหรับ(auto const &[name, sigspec] : cell->connections())
				ถ้า(driver_map.celltypes.cell_output(cell->type, name)) {
					auto node = factory.create_pending(sigspec.size());
					factory.suggest_name(node, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, node);
					ถ้า(name == port_name)
						rv = node;
				}
			คืน *rv;
		} มิฉะนั้น
			คืน it->second;
	}
public:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		สำหรับ (auto cell : module->cells()) {
			ถ้า (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		สำหรับ (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			ถ้า (wire && wire->port_input) {
				factory.add_input(wire->name, ID($input), Sort(wire->width));
			}
			ถ้า (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_value(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		สำหรับ (auto &mem : memories_vector) {
			ถ้า (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
private:
	Node concatenate_read_results(Mem *mem, vector<Node> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		const SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		จำนวนเต็ม current = 0;
		สำหรับ(size_t i = 0; i < mem->rd_ports.size(); i++) {
			จำนวนเต็ม width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		Node node = results[0];
		สำหรับ(size_t i = 1; i < results.size(); i++)
			node = factory.concat(node, results[i]);
		คืน node;
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
		สำหรับ (size_t i = 0; i < mem->wr_ports.size(); i++) {
			const auto &wr = mem->wr_ports[i];
			ถ้า (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node en = enqueue(driver_map(DriveSpec(wr.en)));
			Node addr = enqueue(driver_map(DriveSpec(wr.addr)));
			Node new_data = enqueue(driver_map(DriveSpec(wr.data)));
			Node old_data = factory.memory_read(node, addr);
			Node wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			node = factory.memory_write(node, addr, wr_data);
		}
		ถ้า (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		สำหรับ (size_t i = 0; i < mem->rd_ports.size(); i++) {
			const auto &rd = mem->rd_ports[i];
			ถ้า (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.push_back(factory.memory_read(node, addr));
		}
		state.set_next_value(node);
		คืน concatenate_read_results(mem, read_results);
	}
	void process_cell(Cell *cell)
	{
		ถ้า (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			ถ้า (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			Node node = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), node);
		} มิฉะนั้น ถ้า (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			ถ้า (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			Node q_value = factory.value(state);
			factory.suggest_name(q_value, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_value);
			state.set_next_value(enqueue(ff.sig_d));
			state.set_initial_value(ff.val_init);
		} มิฉะนั้น {
			dict<IdString, Node> connections;
			IdString output_name; // for the single output case
			จำนวนเต็ม n_outputs = 0;
			สำหรับ(auto const &[name, sigspec] : cell->connections()) {
				ถ้า(driver_map.celltypes.cell_input(cell->type, name) && sigspec.size() > 0)
					connections.insert({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				ถ้า(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, Node>, Node> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			ถ้า(auto *nodep = std::get_if<Node>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} มิฉะนั้น {
				สำหรับ(auto [name, node] : std::get<dict<IdString, Node>>(outputs))
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
		สำหรับ(auto const &chunk : spec.chunks())
			ถ้า(chunk.is_none())
				undriven(name);
	}
public:
	void process_queue()
	{
		สำหรับ (; !queue.empty(); queue.pop_front()) {
			ถ้า(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				continue;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			Node pending = graph_nodes.at(spec);

			ถ้า (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				Node node = enqueue(chunks[0]);
				สำหรับ(size_t i = 1; i < chunks.size(); i++)
					node = factory.concat(node, enqueue(chunks[i]));
				factory.update_pending(pending, node);
			} มิฉะนั้น ถ้า (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				ถ้า (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					ถ้า (wire_chunk.is_whole()) {
						ถ้า (wire_chunk.wire->port_input) {
							Node node = factory.value(factory.ir().input(wire_chunk.wire->name));
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						} มิฉะนั้น {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							Node node = enqueue(driver);
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						}
					} มิฉะนั้น {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						Node node = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, node);
					}
				} มิฉะนั้น ถ้า (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					ถ้า (port_chunk.is_whole()) {
						ถ้า (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							Node node = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, node);
						} มิฉะนั้น {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} มิฉะนั้น {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						Node node = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, node);
					}
				} มิฉะนั้น ถ้า (chunk.is_constant()) {
					Node node = factory.constant(chunk.constant());
					factory.suggest_name(node, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, node);
				} มิฉะนั้น ถ้า (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} มิฉะนั้น ถ้า (chunk.is_none()) {
					undriven(log_signal(chunk));
				} มิฉะนั้น {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} มิฉะนั้น {
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
    คืน ir;
}

void IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    bool scc = false;
    std::vector<จำนวนเต็ม> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](จำนวนเต็ม *begin, จำนวนเต็ม *end) {
        perm.insert(perm.end(), begin, end);
        ถ้า (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            สำหรับ (จำนวนเต็ม *i = begin; i != end; ++i) {
				Node node(_graph[*i], design);
                log("- %s = %s\n", node.name().unescape(), node.to_string());
			}
            log("\n");
            scc = true;
        }
    });
	สำหรับ(const auto &[name, state]: _states)
		ถ้า(state.has_next_value())
			toposort.process(state.next_value().id());
	สำหรับ(const auto &[name, output]: _outputs)
		ถ้า(output.has_value())
			toposort.process(output.value().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    ถ้า(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

static IdString merge_name(IdString a, IdString b) {
	ถ้า(!a.isPublic() && b.isPublic())
		คืน b;
	มิฉะนั้น
		คืน a;
}

void IR::forward_buf() {
    std::vector<จำนวนเต็ม> perm, alias;
    perm.clear();

    สำหรับ (จำนวนเต็ม i = 0; i < _graph.size(); ++i)
    {
        auto node = _graph[i];
        ถ้า (node.function().fn() == Fn::buf && node.arg(0).index() < i)
        {
            จำนวนเต็ม target_index = alias[node.arg(0).index()];
            auto target_node = _graph[perm[target_index]];
			ถ้า(node.has_sparse_attr()) {
				ถ้า(target_node.has_sparse_attr()) {
					IdString id = merge_name(node.sparse_attr(), target_node.sparse_attr());
					target_node.sparse_attr() = id;
				} มิฉะนั้น {
					IdString id = node.sparse_attr();
					target_node.sparse_attr() = id;
				}
			}
            alias.push_back(target_index);
        }
        มิฉะนั้น
        {
            alias.push_back(GetSize(perm));
            perm.push_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
static std::string quote_fmt(const var_na *fmt)
{
	std::string r;
	สำหรับ(const var_na *p = fmt; *p != 0; p++) {
		switch(*p) {
		pak_sa '\n': r += "\\n"; bha_nga;
		pak_sa '\t': r += "\\t"; bha_nga;
		pak_sa '"': r += "\\\""; bha_nga;
		pak_sa '\\': r += "\\\\"; bha_nga;
		default: r += *p; bha_nga;
		}
	}
	คืน r;
}

void Writer::print_impl(const var_na *fmt, vector<std::function<void()>> &fns)
{
	size_t next_index = 0;
	สำหรับ(const var_na *p = fmt; *p != 0; p++)
		switch(*p) {
		pak_sa '{':
			ถ้า(*++p == '{') {
				*os << '{';
			} มิฉะนั้น {
				var_na *pe;
				size_t index = strtoul(p, &pe, 10);
				ถ้า(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				ถ้า(p == pe)
					index = next_index;
				มิฉะนั้น
					p = pe;
				ถ้า(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			bha_nga;
		pak_sa '}':
			p++;
			ถ้า(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			bha_nga;
		default:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
