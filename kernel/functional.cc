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
	pak_sa Fn::invalid: ལོག "invalid";
	pak_sa Fn::buf: ལོག "buf";
	pak_sa Fn::slice: ལོག "slice";
	pak_sa Fn::zero_extend: ལོག "zero_extend";
	pak_sa Fn::sign_extend: ལོག "sign_extend";
	pak_sa Fn::concat: ལོག "concat";
	pak_sa Fn::add: ལོག "add";
	pak_sa Fn::sub: ལོག "sub";
	pak_sa Fn::mul: ལོག "mul";
	pak_sa Fn::unsigned_div: ལོག "unsigned_div";
	pak_sa Fn::unsigned_mod: ལོག "unsigned_mod";
	pak_sa Fn::bitwise_and: ལོག "bitwise_and";
	pak_sa Fn::bitwise_or: ལོག "bitwise_or";
	pak_sa Fn::bitwise_xor: ལོག "bitwise_xor";
	pak_sa Fn::bitwise_not: ལོག "bitwise_not";
	pak_sa Fn::reduce_and: ལོག "reduce_and";
	pak_sa Fn::reduce_or: ལོག "reduce_or";
	pak_sa Fn::reduce_xor: ལོག "reduce_xor";
	pak_sa Fn::unary_minus: ལོག "unary_minus";
	pak_sa Fn::equal: ལོག "equal";
	pak_sa Fn::not_equal: ལོག "not_equal";
	pak_sa Fn::signed_greater_than: ལོག "signed_greater_than";
	pak_sa Fn::signed_greater_equal: ལོག "signed_greater_equal";
	pak_sa Fn::unsigned_greater_than: ལོག "unsigned_greater_than";
	pak_sa Fn::unsigned_greater_equal: ལོག "unsigned_greater_equal";
	pak_sa Fn::logical_shift_left: ལོག "logical_shift_left";
	pak_sa Fn::logical_shift_right: ལོག "logical_shift_right";
	pak_sa Fn::arithmetic_shift_right: ལོག "arithmetic_shift_right";
	pak_sa Fn::mux: ལོག "mux";
	pak_sa Fn::constant: ལོག "constant";
	pak_sa Fn::input: ལོག "input";
	pak_sa Fn::state: ལོག "state";
	pak_sa Fn::memory_read: ལོག "memory_read";
	pak_sa Fn::memory_write: ལོག "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (ཧྲིལ)fn);
}

vector<IRInput const*> IR::inputs(IdString kind) const {
	vector<IRInput const*> ret;
	དོན་ལུ (const auto &[name, input] : _inputs)
		གལ་སྲིད(input.kind == kind)
			ret.push_back(&input);
	ལོག ret;
}

vector<IROutput const*> IR::outputs(IdString kind) const {
	vector<IROutput const*> ret;
	དོན་ལུ (const auto &[name, output] : _outputs)
		གལ་སྲིད(output.kind == kind)
			ret.push_back(&output);
	ལོག ret;
}

vector<IRState const*> IR::states(IdString kind) const {
	vector<IRState const*> ret;
	དོན་ལུ (const auto &[name, state] : _states)
		གལ་སྲིད(state.kind == kind)
			ret.push_back(&state);
	ལོག ret;
}

vector<IRInput const*> IR::all_inputs() const {
	vector<IRInput const*> ret;
	དོན་ལུ (const auto &[name, input] : _inputs)
		ret.push_back(&input);
	ལོག ret;
}

vector<IROutput const*> IR::all_outputs() const {
	vector<IROutput const*> ret;
	དོན་ལུ (const auto &[name, output] : _outputs)
		ret.push_back(&output);
	ལོག ret;
}

vector<IRState const*> IR::all_states() const {
	vector<IRState const*> ret;
	དོན་ལུ (const auto &[name, state] : _states)
		ret.push_back(&state);
	ལོག ret;
}

sa_nracanaa PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(Node)> np;
	PrintVisitor(std::function<std::string(Node)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(Node, Node a, ཧྲིལ offset, ཧྲིལ out_width) override { ལོག "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(Node, Node a, ཧྲིལ out_width) override { ལོག "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(Node, Node a, ཧྲིལ out_width) override { ལོག "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(Node, RTLIL::Const const& value) override { ལོག "constant(" + value.as_string() + ")"; }
	std::string input(Node self, IdString name, IdString kind) override { ལོག "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(Node self, IdString name, IdString kind) override { ལོག "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(Node self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		དོན་ལུ(size_t i = 0; i < self.arg_count(); i++) {
			གལ་སྲིད(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		ལོག ret;
	}
};

std::string Node::to_string()
{
	ལོག to_string([](Node n) { ལོག n.name().unescape(); });
}

std::string Node::to_string(std::function<std::string(Node)> np)
{
	ལོག visit(PrintVisitor(np));
}

class CellSimplifier {
	Factory &factory;
	Node sign(Node a) {
		ལོག factory.slice(a, a.width() - 1, 1);
	}
	Node neg_if(Node a, Node s) {
		ལོག factory.mux(a, factory.unary_minus(a), s);
	}
	Node abs(Node a) {
		ལོག neg_if(a, sign(a));
	}
	Node handle_shift(Node a, Node b, bool is_right, bool is_signed) {
		// to prevent new_width == 0, we handle this case separately
		གལ་སྲིད(a.width() == 1) {
			གལ་སྲིད(!is_signed)
				ལོག factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			དེ་མིན
				ལོག a;
		}
		ཧྲིལ new_width = ceil_log2(a.width());
		Node b_truncated = factory.extend(b, new_width, false);
		Node y =
			!is_right ? factory.logical_shift_left(a, b_truncated) :
			!is_signed ? factory.logical_shift_right(a, b_truncated) :
			factory.arithmetic_shift_right(a, b_truncated);
		གལ་སྲིད(b.width() <= new_width)
			ལོག y;
		Node overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		Node y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), true) : factory.constant(RTLIL::Const(State::S0, a.width()));
		ལོག factory.mux(y, y_if_overflow, overflow);
	}
public:
	Node logical_shift_left(Node a, Node b) { ལོག handle_shift(a, b, false, false); }
	Node logical_shift_right(Node a, Node b) { ལོག handle_shift(a, b, true, false); }
	Node arithmetic_shift_right(Node a, Node b) { ལོག handle_shift(a, b, true, true); }
	Node bitwise_mux(Node a, Node b, Node s) {
		Node aa = factory.bitwise_and(a, factory.bitwise_not(s));
		Node bb = factory.bitwise_and(b, s);
		ལོག factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
private:
	Node handle_pow(Node a0, Node b, ཧྲིལ y_width, bool is_signed) {
		Node a = factory.extend(a0, y_width, is_signed);
		Node r = factory.constant(Const(1, y_width));
		དོན་ལུ(ཧྲིལ i = 0; i < b.width(); i++) {
			Node b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		གལ་སྲིད (is_signed) {
			Node a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			Node zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		ལོག r;
	}
	Node handle_bmux(Node a, Node s, ཧྲིལ a_offset, ཧྲིལ width, ཧྲིལ sn) {
		གལ་སྲིད(sn < 1)
			ལོག factory.slice(a, a_offset, width);
		དེ་མིན {
			Node y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			Node y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			ལོག factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	Node handle_pmux(Node a, Node b, Node s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		Node y = a;
		དོན་ལུ(ཧྲིལ i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		ལོག y;
	}
	dict<IdString, Node> handle_fa(Node a, Node b, Node c) {
		Node t1 = factory.bitwise_xor(a, b);
		Node t2 = factory.bitwise_and(a, b);
		Node t3 = factory.bitwise_and(c, t1);
		Node y = factory.bitwise_xor(c, t1);
		Node x = factory.bitwise_or(t2, t3);
		ལོག {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, Node> handle_alu(Node a_in, Node b_in, ཧྲིལ y_width, bool is_signed, Node ci, Node bi) {
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
		ལོག {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	Node handle_lcu(Node p, Node g, Node ci) {
		ལོག handle_alu(g, factory.bitwise_or(p, g), g.width(), false, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
public:
	std::variant<dict<IdString, Node>, Node> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, Node> inputs)
	{
		ཧྲིལ a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		ཧྲིལ b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		ཧྲིལ y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		bool a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		bool b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		གལ་སྲིད(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			གལ་སྲིད(cellType == ID($add))
				ལོག factory.add(a, b);
			དེ་མིན གལ་སྲིད(cellType == ID($sub))
				ལོག factory.sub(a, b);
			དེ་མིན གལ་སྲིད(cellType == ID($mul))
				ལོག factory.mul(a, b);
			དེ་མིན གལ་སྲིད(cellType == ID($and))
				ལོག factory.bitwise_and(a, b);
			དེ་མིན གལ་སྲིད(cellType == ID($or))
				ལོག factory.bitwise_or(a, b);
			དེ་མིན གལ་སྲིད(cellType == ID($xor))
				ལོག factory.bitwise_xor(a, b);
			དེ་མིན གལ་སྲིད(cellType == ID($xnor))
				ལོག factory.bitwise_not(factory.bitwise_xor(a, b));
			དེ་མིན
				log_abort();
		}དེ་མིན གལ་སྲིད(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			bool is_signed = a_signed && b_signed;
			ཧྲིལ width = max(a_width, b_width);
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			གལ་སྲིད(cellType.in(ID($eq), ID($eqx)))
				ལོག factory.extend(factory.equal(a, b), y_width, false);
			དེ་མིན གལ་སྲིད(cellType.in(ID($ne), ID($nex)))
				ལོག factory.extend(factory.not_equal(a, b), y_width, false);
			དེ་མིན གལ་སྲིད(cellType == ID($lt))
				ལོག factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, false);
			དེ་མིན གལ་སྲིད(cellType == ID($le))
				ལོག factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, false);
			དེ་མིན གལ་སྲིད(cellType == ID($gt))
				ལོག factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, false);
			དེ་མིན གལ་སྲིད(cellType == ID($ge))
				ལོག factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, false);
			དེ་མིན
				log_abort();
		}དེ་མིན གལ་སྲིད(cellType.in(ID($logic_or), ID($logic_and))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node b = factory.reduce_or(inputs.at(ID(B)));
			Node y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			ལོག factory.extend(y, y_width, false);
		}དེ་མིན གལ་སྲིད(cellType == ID($not)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ལོག factory.bitwise_not(a);
		}དེ་མིན གལ་སྲིད(cellType == ID($pos)){
			ལོག factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}དེ་མིན གལ་སྲིད(cellType == ID($barrier)){
			ལོག inputs.at(ID(A));
		}དེ་མིན གལ་སྲིད(cellType == ID($neg)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ལོག factory.unary_minus(a);
		}དེ་མིན གལ་སྲིད(cellType == ID($logic_not)){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node y = factory.bitwise_not(a);
			ལོག factory.extend(y, y_width, false);
		}དེ་མིན གལ་སྲིད(cellType.in(ID($reduce_or), ID($reduce_bool))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			ལོག factory.extend(a, y_width, false);
		}དེ་མིན གལ་སྲིད(cellType == ID($reduce_and)){
			Node a = factory.reduce_and(inputs.at(ID(A)));
			ལོག factory.extend(a, y_width, false);
		}དེ་མིན གལ་སྲིད(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			Node a = factory.reduce_xor(inputs.at(ID(A)));
			Node y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			ལོག factory.extend(y, y_width, false);
		}དེ་མིན གལ་སྲིད(cellType == ID($shl) || cellType == ID($sshl)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			Node b = inputs.at(ID(B));
			ལོག logical_shift_left(a, b);
		}དེ་མིན གལ་སྲིད(cellType == ID($shr) || cellType == ID($sshr)){
			ཧྲིལ width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, a_signed);
			Node b = inputs.at(ID(B));
			Node y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_right(a, b) :
				logical_shift_right(a, b);
			ལོག factory.extend(y, y_width, a_signed);
		}དེ་མིན གལ་སྲིད(cellType == ID($shiftx) || cellType == ID($shift)){
			ཧྲིལ width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			Node b = inputs.at(ID(B));
			Node shr = logical_shift_right(a, b);
			གལ་སྲིད(b_signed) {
				Node shl = logical_shift_left(a, factory.unary_minus(b));
				Node y = factory.mux(shr, shl, sign(b));
				ལོག factory.extend(y, y_width, false);
			} དེ་མིན {
				ལོག factory.extend(shr, y_width, false);
			}
		}དེ་མིན གལ་སྲིད(cellType == ID($mux)){
			ལོག factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}དེ་མིན གལ་སྲིད(cellType == ID($pmux)){
			ལོག handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}དེ་མིན གལ་སྲིད(cellType == ID($concat)){
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			ལོག factory.concat(a, b);
		}དེ་མིན གལ་སྲིད(cellType == ID($slice)){
			ཧྲིལ offset = parameters.at(ID(OFFSET)).as_int();
			Node a = inputs.at(ID(A));
			ལོག factory.slice(a, offset, y_width);
		}དེ་མིན གལ་སྲིད(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			ཧྲིལ width = max(a_width, b_width);
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			གལ་སྲིད(is_signed) {
				གལ་སྲིད(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					Node abs_y = factory.unsigned_div(abs(a), abs(b));
					Node out_sign = factory.not_equal(sign(a), sign(b));
					ལོག neg_if(factory.extend(abs_y, y_width, false), out_sign);
				} དེ་མིན གལ་སྲིད(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					Node abs_y = factory.unsigned_mod(abs(a), abs(b));
					ལོག neg_if(factory.extend(abs_y, y_width, false), sign(a));
				} དེ་མིན གལ་སྲིད(cellType == ID($divfloor)) {
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
					ལོག factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} དེ་མིན གལ་སྲིད(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					Node abs_b = abs(b);
					Node abs_y = factory.unsigned_mod(abs(a), abs_b);
					Node flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					Node y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					Node y = neg_if(y_flipped, sign(b));
					ལོག factory.extend(y, y_width, true);
				} དེ་མིན
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} དེ་མིན {
				གལ་སྲིད(cellType.in(ID($mod), ID($modfloor)))
					ལོག factory.extend(factory.unsigned_mod(a, b), y_width, false);
				དེ་མིན
					ལོག factory.extend(factory.unsigned_div(a, b), y_width, false);
			}
		} དེ་མིན གལ་སྲིད(cellType == ID($pow)) {
			ལོག handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} དེ་མིན གལ་སྲིད (cellType == ID($lut)) {
			ཧྲིལ width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			ལོག handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} དེ་མིན གལ་སྲིད (cellType == ID($bwmux)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			Node s = inputs.at(ID(S));
			ལོག factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} དེ་མིན གལ་སྲིད (cellType == ID($bweqx)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			ལོག factory.bitwise_not(factory.bitwise_xor(a, b));
		} དེ་མིན གལ་སྲིད(cellType == ID($bmux)) {
			ཧྲིལ width = parameters.at(ID(WIDTH)).as_int();
			ཧྲིལ s_width = parameters.at(ID(S_WIDTH)).as_int();
			ལོག handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} དེ་མིན གལ་སྲིད(cellType == ID($demux)) {
			ཧྲིལ width = parameters.at(ID(WIDTH)).as_int();
			ཧྲིལ s_width = parameters.at(ID(S_WIDTH)).as_int();
			ཧྲིལ y_width = width << s_width;
			ཧྲིལ b_width = ceil_log2(y_width);
			Node a = factory.extend(inputs.at(ID(A)), y_width, false);
			Node s = factory.extend(inputs.at(ID(S)), b_width, false);
			Node b = factory.mul(s, factory.constant(Const(width, b_width)));
			ལོག factory.logical_shift_left(a, b);
		} དེ་མིན གལ་སྲིད(cellType == ID($fa)) {
			ལོག handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} དེ་མིན གལ་སྲིད(cellType == ID($lcu)) {
			ལོག handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} དེ་མིན གལ་སྲིད(cellType == ID($alu)) {
			ལོག handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} དེ་མིན གལ་སྲིད(cellType.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover))) {
			Node a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_value(a);
			ལོག {};
		} དེ་མིན གལ་སྲིད(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			ཧྲིལ width = parameters.at(ID(WIDTH)).as_int();
			auto &input = factory.add_input(cellName, cellType, Sort(width));
			ལོག factory.value(input);
		} དེ་མིན གལ་སྲིད(cellType == ID($initstate)) {
			གལ་སྲིད(factory.ir().has_state(ID($initstate), ID($state)))
				ལོག factory.value(factory.ir().state(ID($initstate)));
			དེ་མིན {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_value(RTLIL::Const(State::S1, 1));
				state.set_next_value(factory.constant(RTLIL::Const(State::S0, 1)));
				ལོག factory.value(state);
			}
		} དེ་མིན གལ་སྲིད(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} དེ་མིན {
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
		གལ་སྲིད(it == graph_nodes.end()){
			auto node = factory.create_pending(spec.size());
			graph_nodes.insert({spec, node});
			queue.emplace_back(spec);
			ལོག node;
		}དེ་མིན
			ལོག it->second;
	}
	Node enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		གལ་སྲིད(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<Node> rv;
			དོན་ལུ(auto const &[name, sigspec] : cell->connections())
				གལ་སྲིད(driver_map.celltypes.cell_output(cell->type, name)) {
					auto node = factory.create_pending(sigspec.size());
					factory.suggest_name(node, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, node);
					གལ་སྲིད(name == port_name)
						rv = node;
				}
			ལོག *rv;
		} དེ་མིན
			ལོག it->second;
	}
public:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		དོན་ལུ (auto cell : module->cells()) {
			གལ་སྲིད (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		དོན་ལུ (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			གལ་སྲིད (wire && wire->port_input) {
				factory.add_input(wire->name, ID($input), Sort(wire->width));
			}
			གལ་སྲིད (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_value(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		དོན་ལུ (auto &mem : memories_vector) {
			གལ་སྲིད (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
private:
	Node concatenate_read_results(Mem *mem, vector<Node> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		const SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		ཧྲིལ current = 0;
		དོན་ལུ(size_t i = 0; i < mem->rd_ports.size(); i++) {
			ཧྲིལ width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		Node node = results[0];
		དོན་ལུ(size_t i = 1; i < results.size(); i++)
			node = factory.concat(node, results[i]);
		ལོག node;
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
		དོན་ལུ (size_t i = 0; i < mem->wr_ports.size(); i++) {
			const auto &wr = mem->wr_ports[i];
			གལ་སྲིད (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node en = enqueue(driver_map(DriveSpec(wr.en)));
			Node addr = enqueue(driver_map(DriveSpec(wr.addr)));
			Node new_data = enqueue(driver_map(DriveSpec(wr.data)));
			Node old_data = factory.memory_read(node, addr);
			Node wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			node = factory.memory_write(node, addr, wr_data);
		}
		གལ་སྲིད (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		དོན་ལུ (size_t i = 0; i < mem->rd_ports.size(); i++) {
			const auto &rd = mem->rd_ports[i];
			གལ་སྲིད (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.push_back(factory.memory_read(node, addr));
		}
		state.set_next_value(node);
		ལོག concatenate_read_results(mem, read_results);
	}
	void process_cell(Cell *cell)
	{
		གལ་སྲིད (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			གལ་སྲིད (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			Node node = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), node);
		} དེ་མིན གལ་སྲིད (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			གལ་སྲིད (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			Node q_value = factory.value(state);
			factory.suggest_name(q_value, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_value);
			state.set_next_value(enqueue(ff.sig_d));
			state.set_initial_value(ff.val_init);
		} དེ་མིན {
			dict<IdString, Node> connections;
			IdString output_name; // for the single output case
			ཧྲིལ n_outputs = 0;
			དོན་ལུ(auto const &[name, sigspec] : cell->connections()) {
				གལ་སྲིད(driver_map.celltypes.cell_input(cell->type, name) && sigspec.size() > 0)
					connections.insert({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				གལ་སྲིད(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, Node>, Node> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			གལ་སྲིད(auto *nodep = std::get_if<Node>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} དེ་མིན {
				དོན་ལུ(auto [name, node] : std::get<dict<IdString, Node>>(outputs))
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
		དོན་ལུ(auto const &chunk : spec.chunks())
			གལ་སྲིད(chunk.is_none())
				undriven(name);
	}
public:
	void process_queue()
	{
		དོན་ལུ (; !queue.empty(); queue.pop_front()) {
			གལ་སྲིད(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				continue;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			Node pending = graph_nodes.at(spec);

			གལ་སྲིད (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				Node node = enqueue(chunks[0]);
				དོན་ལུ(size_t i = 1; i < chunks.size(); i++)
					node = factory.concat(node, enqueue(chunks[i]));
				factory.update_pending(pending, node);
			} དེ་མིན གལ་སྲིད (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				གལ་སྲིད (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					གལ་སྲིད (wire_chunk.is_whole()) {
						གལ་སྲིད (wire_chunk.wire->port_input) {
							Node node = factory.value(factory.ir().input(wire_chunk.wire->name));
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						} དེ་མིན {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							Node node = enqueue(driver);
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						}
					} དེ་མིན {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						Node node = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, node);
					}
				} དེ་མིན གལ་སྲིད (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					གལ་སྲིད (port_chunk.is_whole()) {
						གལ་སྲིད (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							Node node = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, node);
						} དེ་མིན {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} དེ་མིན {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						Node node = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, node);
					}
				} དེ་མིན གལ་སྲིད (chunk.is_constant()) {
					Node node = factory.constant(chunk.constant());
					factory.suggest_name(node, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, node);
				} དེ་མིན གལ་སྲིད (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} དེ་མིན གལ་སྲིད (chunk.is_none()) {
					undriven(log_signal(chunk));
				} དེ་མིན {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} དེ་མིན {
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
    ལོག ir;
}

void IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    bool scc = false;
    std::vector<ཧྲིལ> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](ཧྲིལ *begin, ཧྲིལ *end) {
        perm.insert(perm.end(), begin, end);
        གལ་སྲིད (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            དོན་ལུ (ཧྲིལ *i = begin; i != end; ++i) {
				Node node(_graph[*i], design);
                log("- %s = %s\n", node.name().unescape(), node.to_string());
			}
            log("\n");
            scc = true;
        }
    });
	དོན་ལུ(const auto &[name, state]: _states)
		གལ་སྲིད(state.has_next_value())
			toposort.process(state.next_value().id());
	དོན་ལུ(const auto &[name, output]: _outputs)
		གལ་སྲིད(output.has_value())
			toposort.process(output.value().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    གལ་སྲིད(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

static IdString merge_name(IdString a, IdString b) {
	གལ་སྲིད(!a.isPublic() && b.isPublic())
		ལོག b;
	དེ་མིན
		ལོག a;
}

void IR::forward_buf() {
    std::vector<ཧྲིལ> perm, alias;
    perm.clear();

    དོན་ལུ (ཧྲིལ i = 0; i < _graph.size(); ++i)
    {
        auto node = _graph[i];
        གལ་སྲིད (node.function().fn() == Fn::buf && node.arg(0).index() < i)
        {
            ཧྲིལ target_index = alias[node.arg(0).index()];
            auto target_node = _graph[perm[target_index]];
			གལ་སྲིད(node.has_sparse_attr()) {
				གལ་སྲིད(target_node.has_sparse_attr()) {
					IdString id = merge_name(node.sparse_attr(), target_node.sparse_attr());
					target_node.sparse_attr() = id;
				} དེ་མིན {
					IdString id = node.sparse_attr();
					target_node.sparse_attr() = id;
				}
			}
            alias.push_back(target_index);
        }
        དེ་མིན
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
	དོན་ལུ(const var_na *p = fmt; *p != 0; p++) {
		switch(*p) {
		pak_sa '\n': r += "\\n"; bha_nga;
		pak_sa '\t': r += "\\t"; bha_nga;
		pak_sa '"': r += "\\\""; bha_nga;
		pak_sa '\\': r += "\\\\"; bha_nga;
		default: r += *p; bha_nga;
		}
	}
	ལོག r;
}

void Writer::print_impl(const var_na *fmt, vector<std::function<void()>> &fns)
{
	size_t next_index = 0;
	དོན་ལུ(const var_na *p = fmt; *p != 0; p++)
		switch(*p) {
		pak_sa '{':
			གལ་སྲིད(*++p == '{') {
				*os << '{';
			} དེ་མིན {
				var_na *pe;
				size_t index = strtoul(p, &pe, 10);
				གལ་སྲིད(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				གལ་སྲིད(p == pe)
					index = next_index;
				དེ་མིན
					p = pe;
				གལ་སྲིད(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			bha_nga;
		pak_sa '}':
			p++;
			གལ་སྲིད(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			bha_nga;
		default:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
