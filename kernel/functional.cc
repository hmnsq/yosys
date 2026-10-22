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
	pak_sa Fn::invalid: ສົ່ງຄືນ "invalid";
	pak_sa Fn::buf: ສົ່ງຄືນ "buf";
	pak_sa Fn::slice: ສົ່ງຄືນ "slice";
	pak_sa Fn::zero_extend: ສົ່ງຄືນ "zero_extend";
	pak_sa Fn::sign_extend: ສົ່ງຄືນ "sign_extend";
	pak_sa Fn::concat: ສົ່ງຄືນ "concat";
	pak_sa Fn::add: ສົ່ງຄືນ "add";
	pak_sa Fn::sub: ສົ່ງຄືນ "sub";
	pak_sa Fn::mul: ສົ່ງຄືນ "mul";
	pak_sa Fn::unsigned_div: ສົ່ງຄືນ "unsigned_div";
	pak_sa Fn::unsigned_mod: ສົ່ງຄືນ "unsigned_mod";
	pak_sa Fn::bitwise_and: ສົ່ງຄືນ "bitwise_and";
	pak_sa Fn::bitwise_or: ສົ່ງຄືນ "bitwise_or";
	pak_sa Fn::bitwise_xor: ສົ່ງຄືນ "bitwise_xor";
	pak_sa Fn::bitwise_not: ສົ່ງຄືນ "bitwise_not";
	pak_sa Fn::reduce_and: ສົ່ງຄືນ "reduce_and";
	pak_sa Fn::reduce_or: ສົ່ງຄືນ "reduce_or";
	pak_sa Fn::reduce_xor: ສົ່ງຄືນ "reduce_xor";
	pak_sa Fn::unary_minus: ສົ່ງຄືນ "unary_minus";
	pak_sa Fn::equal: ສົ່ງຄືນ "equal";
	pak_sa Fn::not_equal: ສົ່ງຄືນ "not_equal";
	pak_sa Fn::signed_greater_than: ສົ່ງຄືນ "signed_greater_than";
	pak_sa Fn::signed_greater_equal: ສົ່ງຄືນ "signed_greater_equal";
	pak_sa Fn::unsigned_greater_than: ສົ່ງຄືນ "unsigned_greater_than";
	pak_sa Fn::unsigned_greater_equal: ສົ່ງຄືນ "unsigned_greater_equal";
	pak_sa Fn::logical_shift_left: ສົ່ງຄືນ "logical_shift_left";
	pak_sa Fn::logical_shift_right: ສົ່ງຄືນ "logical_shift_right";
	pak_sa Fn::arithmetic_shift_right: ສົ່ງຄືນ "arithmetic_shift_right";
	pak_sa Fn::mux: ສົ່ງຄືນ "mux";
	pak_sa Fn::constant: ສົ່ງຄືນ "constant";
	pak_sa Fn::input: ສົ່ງຄືນ "input";
	pak_sa Fn::state: ສົ່ງຄືນ "state";
	pak_sa Fn::memory_read: ສົ່ງຄືນ "memory_read";
	pak_sa Fn::memory_write: ສົ່ງຄືນ "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (ຈຳນວນເຕັມ)fn);
}

vector<IRInput const*> IR::inputs(IdString kind) const {
	vector<IRInput const*> ret;
	ສຳລັບ (const auto &[name, input] : _inputs)
		ຖ້າ(input.kind == kind)
			ret.push_back(&input);
	ສົ່ງຄືນ ret;
}

vector<IROutput const*> IR::outputs(IdString kind) const {
	vector<IROutput const*> ret;
	ສຳລັບ (const auto &[name, output] : _outputs)
		ຖ້າ(output.kind == kind)
			ret.push_back(&output);
	ສົ່ງຄືນ ret;
}

vector<IRState const*> IR::states(IdString kind) const {
	vector<IRState const*> ret;
	ສຳລັບ (const auto &[name, state] : _states)
		ຖ້າ(state.kind == kind)
			ret.push_back(&state);
	ສົ່ງຄືນ ret;
}

vector<IRInput const*> IR::all_inputs() const {
	vector<IRInput const*> ret;
	ສຳລັບ (const auto &[name, input] : _inputs)
		ret.push_back(&input);
	ສົ່ງຄືນ ret;
}

vector<IROutput const*> IR::all_outputs() const {
	vector<IROutput const*> ret;
	ສຳລັບ (const auto &[name, output] : _outputs)
		ret.push_back(&output);
	ສົ່ງຄືນ ret;
}

vector<IRState const*> IR::all_states() const {
	vector<IRState const*> ret;
	ສຳລັບ (const auto &[name, state] : _states)
		ret.push_back(&state);
	ສົ່ງຄືນ ret;
}

sa_nracanaa PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(Node)> np;
	PrintVisitor(std::function<std::string(Node)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(Node, Node a, ຈຳນວນເຕັມ offset, ຈຳນວນເຕັມ out_width) override { ສົ່ງຄືນ "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(Node, Node a, ຈຳນວນເຕັມ out_width) override { ສົ່ງຄືນ "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(Node, Node a, ຈຳນວນເຕັມ out_width) override { ສົ່ງຄືນ "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(Node, RTLIL::Const const& value) override { ສົ່ງຄືນ "constant(" + value.as_string() + ")"; }
	std::string input(Node self, IdString name, IdString kind) override { ສົ່ງຄືນ "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(Node self, IdString name, IdString kind) override { ສົ່ງຄືນ "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(Node self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		ສຳລັບ(size_t i = 0; i < self.arg_count(); i++) {
			ຖ້າ(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		ສົ່ງຄືນ ret;
	}
};

std::string Node::to_string()
{
	ສົ່ງຄືນ to_string([](Node n) { ສົ່ງຄືນ n.name().unescape(); });
}

std::string Node::to_string(std::function<std::string(Node)> np)
{
	ສົ່ງຄືນ visit(PrintVisitor(np));
}

class CellSimplifier {
	Factory &factory;
	Node sign(Node a) {
		ສົ່ງຄືນ factory.slice(a, a.width() - 1, 1);
	}
	Node neg_if(Node a, Node s) {
		ສົ່ງຄືນ factory.mux(a, factory.unary_minus(a), s);
	}
	Node abs(Node a) {
		ສົ່ງຄືນ neg_if(a, sign(a));
	}
	Node handle_shift(Node a, Node b, bool is_right, bool is_signed) {
		// to prevent new_width == 0, we handle this case separately
		ຖ້າ(a.width() == 1) {
			ຖ້າ(!is_signed)
				ສົ່ງຄືນ factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			ບໍ່ດັ່ງນັ້ນ
				ສົ່ງຄືນ a;
		}
		ຈຳນວນເຕັມ new_width = ceil_log2(a.width());
		Node b_truncated = factory.extend(b, new_width, false);
		Node y =
			!is_right ? factory.logical_shift_left(a, b_truncated) :
			!is_signed ? factory.logical_shift_right(a, b_truncated) :
			factory.arithmetic_shift_right(a, b_truncated);
		ຖ້າ(b.width() <= new_width)
			ສົ່ງຄືນ y;
		Node overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		Node y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), true) : factory.constant(RTLIL::Const(State::S0, a.width()));
		ສົ່ງຄືນ factory.mux(y, y_if_overflow, overflow);
	}
public:
	Node logical_shift_left(Node a, Node b) { ສົ່ງຄືນ handle_shift(a, b, false, false); }
	Node logical_shift_right(Node a, Node b) { ສົ່ງຄືນ handle_shift(a, b, true, false); }
	Node arithmetic_shift_right(Node a, Node b) { ສົ່ງຄືນ handle_shift(a, b, true, true); }
	Node bitwise_mux(Node a, Node b, Node s) {
		Node aa = factory.bitwise_and(a, factory.bitwise_not(s));
		Node bb = factory.bitwise_and(b, s);
		ສົ່ງຄືນ factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
private:
	Node handle_pow(Node a0, Node b, ຈຳນວນເຕັມ y_width, bool is_signed) {
		Node a = factory.extend(a0, y_width, is_signed);
		Node r = factory.constant(Const(1, y_width));
		ສຳລັບ(ຈຳນວນເຕັມ i = 0; i < b.width(); i++) {
			Node b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		ຖ້າ (is_signed) {
			Node a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			Node zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		ສົ່ງຄືນ r;
	}
	Node handle_bmux(Node a, Node s, ຈຳນວນເຕັມ a_offset, ຈຳນວນເຕັມ width, ຈຳນວນເຕັມ sn) {
		ຖ້າ(sn < 1)
			ສົ່ງຄືນ factory.slice(a, a_offset, width);
		ບໍ່ດັ່ງນັ້ນ {
			Node y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			Node y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			ສົ່ງຄືນ factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	Node handle_pmux(Node a, Node b, Node s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		Node y = a;
		ສຳລັບ(ຈຳນວນເຕັມ i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		ສົ່ງຄືນ y;
	}
	dict<IdString, Node> handle_fa(Node a, Node b, Node c) {
		Node t1 = factory.bitwise_xor(a, b);
		Node t2 = factory.bitwise_and(a, b);
		Node t3 = factory.bitwise_and(c, t1);
		Node y = factory.bitwise_xor(c, t1);
		Node x = factory.bitwise_or(t2, t3);
		ສົ່ງຄືນ {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, Node> handle_alu(Node a_in, Node b_in, ຈຳນວນເຕັມ y_width, bool is_signed, Node ci, Node bi) {
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
		ສົ່ງຄືນ {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	Node handle_lcu(Node p, Node g, Node ci) {
		ສົ່ງຄືນ handle_alu(g, factory.bitwise_or(p, g), g.width(), false, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
public:
	std::variant<dict<IdString, Node>, Node> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, Node> inputs)
	{
		ຈຳນວນເຕັມ a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		ຈຳນວນເຕັມ b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		ຈຳນວນເຕັມ y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		bool a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		bool b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		ຖ້າ(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			ຖ້າ(cellType == ID($add))
				ສົ່ງຄືນ factory.add(a, b);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($sub))
				ສົ່ງຄືນ factory.sub(a, b);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($mul))
				ສົ່ງຄືນ factory.mul(a, b);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($and))
				ສົ່ງຄືນ factory.bitwise_and(a, b);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($or))
				ສົ່ງຄືນ factory.bitwise_or(a, b);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($xor))
				ສົ່ງຄືນ factory.bitwise_xor(a, b);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($xnor))
				ສົ່ງຄືນ factory.bitwise_not(factory.bitwise_xor(a, b));
			ບໍ່ດັ່ງນັ້ນ
				log_abort();
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			bool is_signed = a_signed && b_signed;
			ຈຳນວນເຕັມ width = max(a_width, b_width);
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ຖ້າ(cellType.in(ID($eq), ID($eqx)))
				ສົ່ງຄືນ factory.extend(factory.equal(a, b), y_width, false);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($ne), ID($nex)))
				ສົ່ງຄືນ factory.extend(factory.not_equal(a, b), y_width, false);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($lt))
				ສົ່ງຄືນ factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, false);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($le))
				ສົ່ງຄືນ factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, false);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($gt))
				ສົ່ງຄືນ factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, false);
			ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($ge))
				ສົ່ງຄືນ factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, false);
			ບໍ່ດັ່ງນັ້ນ
				log_abort();
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($logic_or), ID($logic_and))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node b = factory.reduce_or(inputs.at(ID(B)));
			Node y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			ສົ່ງຄືນ factory.extend(y, y_width, false);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($not)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ສົ່ງຄືນ factory.bitwise_not(a);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($pos)){
			ສົ່ງຄືນ factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($barrier)){
			ສົ່ງຄືນ inputs.at(ID(A));
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($neg)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ສົ່ງຄືນ factory.unary_minus(a);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($logic_not)){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			Node y = factory.bitwise_not(a);
			ສົ່ງຄືນ factory.extend(y, y_width, false);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($reduce_or), ID($reduce_bool))){
			Node a = factory.reduce_or(inputs.at(ID(A)));
			ສົ່ງຄືນ factory.extend(a, y_width, false);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($reduce_and)){
			Node a = factory.reduce_and(inputs.at(ID(A)));
			ສົ່ງຄືນ factory.extend(a, y_width, false);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			Node a = factory.reduce_xor(inputs.at(ID(A)));
			Node y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			ສົ່ງຄືນ factory.extend(y, y_width, false);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($shl) || cellType == ID($sshl)){
			Node a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			Node b = inputs.at(ID(B));
			ສົ່ງຄືນ logical_shift_left(a, b);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($shr) || cellType == ID($sshr)){
			ຈຳນວນເຕັມ width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, a_signed);
			Node b = inputs.at(ID(B));
			Node y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_right(a, b) :
				logical_shift_right(a, b);
			ສົ່ງຄືນ factory.extend(y, y_width, a_signed);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($shiftx) || cellType == ID($shift)){
			ຈຳນວນເຕັມ width = max(a_width, y_width);
			Node a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			Node b = inputs.at(ID(B));
			Node shr = logical_shift_right(a, b);
			ຖ້າ(b_signed) {
				Node shl = logical_shift_left(a, factory.unary_minus(b));
				Node y = factory.mux(shr, shl, sign(b));
				ສົ່ງຄືນ factory.extend(y, y_width, false);
			} ບໍ່ດັ່ງນັ້ນ {
				ສົ່ງຄືນ factory.extend(shr, y_width, false);
			}
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($mux)){
			ສົ່ງຄືນ factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($pmux)){
			ສົ່ງຄືນ handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($concat)){
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			ສົ່ງຄືນ factory.concat(a, b);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($slice)){
			ຈຳນວນເຕັມ offset = parameters.at(ID(OFFSET)).as_int();
			Node a = inputs.at(ID(A));
			ສົ່ງຄືນ factory.slice(a, offset, y_width);
		}ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			ຈຳນວນເຕັມ width = max(a_width, b_width);
			bool is_signed = a_signed && b_signed;
			Node a = factory.extend(inputs.at(ID(A)), width, is_signed);
			Node b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ຖ້າ(is_signed) {
				ຖ້າ(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					Node abs_y = factory.unsigned_div(abs(a), abs(b));
					Node out_sign = factory.not_equal(sign(a), sign(b));
					ສົ່ງຄືນ neg_if(factory.extend(abs_y, y_width, false), out_sign);
				} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					Node abs_y = factory.unsigned_mod(abs(a), abs(b));
					ສົ່ງຄືນ neg_if(factory.extend(abs_y, y_width, false), sign(a));
				} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($divfloor)) {
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
					ສົ່ງຄືນ factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					Node abs_b = abs(b);
					Node abs_y = factory.unsigned_mod(abs(a), abs_b);
					Node flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					Node y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					Node y = neg_if(y_flipped, sign(b));
					ສົ່ງຄືນ factory.extend(y, y_width, true);
				} ບໍ່ດັ່ງນັ້ນ
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} ບໍ່ດັ່ງນັ້ນ {
				ຖ້າ(cellType.in(ID($mod), ID($modfloor)))
					ສົ່ງຄືນ factory.extend(factory.unsigned_mod(a, b), y_width, false);
				ບໍ່ດັ່ງນັ້ນ
					ສົ່ງຄືນ factory.extend(factory.unsigned_div(a, b), y_width, false);
			}
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($pow)) {
			ສົ່ງຄືນ handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cellType == ID($lut)) {
			ຈຳນວນເຕັມ width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			ສົ່ງຄືນ handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cellType == ID($bwmux)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			Node s = inputs.at(ID(S));
			ສົ່ງຄືນ factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cellType == ID($bweqx)) {
			Node a = inputs.at(ID(A));
			Node b = inputs.at(ID(B));
			ສົ່ງຄືນ factory.bitwise_not(factory.bitwise_xor(a, b));
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($bmux)) {
			ຈຳນວນເຕັມ width = parameters.at(ID(WIDTH)).as_int();
			ຈຳນວນເຕັມ s_width = parameters.at(ID(S_WIDTH)).as_int();
			ສົ່ງຄືນ handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($demux)) {
			ຈຳນວນເຕັມ width = parameters.at(ID(WIDTH)).as_int();
			ຈຳນວນເຕັມ s_width = parameters.at(ID(S_WIDTH)).as_int();
			ຈຳນວນເຕັມ y_width = width << s_width;
			ຈຳນວນເຕັມ b_width = ceil_log2(y_width);
			Node a = factory.extend(inputs.at(ID(A)), y_width, false);
			Node s = factory.extend(inputs.at(ID(S)), b_width, false);
			Node b = factory.mul(s, factory.constant(Const(width, b_width)));
			ສົ່ງຄືນ factory.logical_shift_left(a, b);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($fa)) {
			ສົ່ງຄືນ handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($lcu)) {
			ສົ່ງຄືນ handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($alu)) {
			ສົ່ງຄືນ handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover))) {
			Node a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_value(a);
			ສົ່ງຄືນ {};
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			ຈຳນວນເຕັມ width = parameters.at(ID(WIDTH)).as_int();
			auto &input = factory.add_input(cellName, cellType, Sort(width));
			ສົ່ງຄືນ factory.value(input);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($initstate)) {
			ຖ້າ(factory.ir().has_state(ID($initstate), ID($state)))
				ສົ່ງຄືນ factory.value(factory.ir().state(ID($initstate)));
			ບໍ່ດັ່ງນັ້ນ {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_value(RTLIL::Const(State::S1, 1));
				state.set_next_value(factory.constant(RTLIL::Const(State::S0, 1)));
				ສົ່ງຄືນ factory.value(state);
			}
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} ບໍ່ດັ່ງນັ້ນ {
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
		ຖ້າ(it == graph_nodes.end()){
			auto node = factory.create_pending(spec.size());
			graph_nodes.insert({spec, node});
			queue.emplace_back(spec);
			ສົ່ງຄືນ node;
		}ບໍ່ດັ່ງນັ້ນ
			ສົ່ງຄືນ it->second;
	}
	Node enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		ຖ້າ(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<Node> rv;
			ສຳລັບ(auto const &[name, sigspec] : cell->connections())
				ຖ້າ(driver_map.celltypes.cell_output(cell->type, name)) {
					auto node = factory.create_pending(sigspec.size());
					factory.suggest_name(node, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, node);
					ຖ້າ(name == port_name)
						rv = node;
				}
			ສົ່ງຄືນ *rv;
		} ບໍ່ດັ່ງນັ້ນ
			ສົ່ງຄືນ it->second;
	}
public:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		ສຳລັບ (auto cell : module->cells()) {
			ຖ້າ (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		ສຳລັບ (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			ຖ້າ (wire && wire->port_input) {
				factory.add_input(wire->name, ID($input), Sort(wire->width));
			}
			ຖ້າ (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_value(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		ສຳລັບ (auto &mem : memories_vector) {
			ຖ້າ (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
private:
	Node concatenate_read_results(Mem *mem, vector<Node> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		const SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		ຈຳນວນເຕັມ current = 0;
		ສຳລັບ(size_t i = 0; i < mem->rd_ports.size(); i++) {
			ຈຳນວນເຕັມ width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		Node node = results[0];
		ສຳລັບ(size_t i = 1; i < results.size(); i++)
			node = factory.concat(node, results[i]);
		ສົ່ງຄືນ node;
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
		ສຳລັບ (size_t i = 0; i < mem->wr_ports.size(); i++) {
			const auto &wr = mem->wr_ports[i];
			ຖ້າ (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node en = enqueue(driver_map(DriveSpec(wr.en)));
			Node addr = enqueue(driver_map(DriveSpec(wr.addr)));
			Node new_data = enqueue(driver_map(DriveSpec(wr.data)));
			Node old_data = factory.memory_read(node, addr);
			Node wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			node = factory.memory_write(node, addr, wr_data);
		}
		ຖ້າ (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		ສຳລັບ (size_t i = 0; i < mem->rd_ports.size(); i++) {
			const auto &rd = mem->rd_ports[i];
			ຖ້າ (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			Node addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.push_back(factory.memory_read(node, addr));
		}
		state.set_next_value(node);
		ສົ່ງຄືນ concatenate_read_results(mem, read_results);
	}
	void process_cell(Cell *cell)
	{
		ຖ້າ (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			ຖ້າ (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			Node node = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), node);
		} ບໍ່ດັ່ງນັ້ນ ຖ້າ (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			ຖ້າ (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			Node q_value = factory.value(state);
			factory.suggest_name(q_value, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_value);
			state.set_next_value(enqueue(ff.sig_d));
			state.set_initial_value(ff.val_init);
		} ບໍ່ດັ່ງນັ້ນ {
			dict<IdString, Node> connections;
			IdString output_name; // for the single output case
			ຈຳນວນເຕັມ n_outputs = 0;
			ສຳລັບ(auto const &[name, sigspec] : cell->connections()) {
				ຖ້າ(driver_map.celltypes.cell_input(cell->type, name) && sigspec.size() > 0)
					connections.insert({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				ຖ້າ(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, Node>, Node> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			ຖ້າ(auto *nodep = std::get_if<Node>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} ບໍ່ດັ່ງນັ້ນ {
				ສຳລັບ(auto [name, node] : std::get<dict<IdString, Node>>(outputs))
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
		ສຳລັບ(auto const &chunk : spec.chunks())
			ຖ້າ(chunk.is_none())
				undriven(name);
	}
public:
	void process_queue()
	{
		ສຳລັບ (; !queue.empty(); queue.pop_front()) {
			ຖ້າ(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				continue;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			Node pending = graph_nodes.at(spec);

			ຖ້າ (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				Node node = enqueue(chunks[0]);
				ສຳລັບ(size_t i = 1; i < chunks.size(); i++)
					node = factory.concat(node, enqueue(chunks[i]));
				factory.update_pending(pending, node);
			} ບໍ່ດັ່ງນັ້ນ ຖ້າ (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				ຖ້າ (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					ຖ້າ (wire_chunk.is_whole()) {
						ຖ້າ (wire_chunk.wire->port_input) {
							Node node = factory.value(factory.ir().input(wire_chunk.wire->name));
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						} ບໍ່ດັ່ງນັ້ນ {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							Node node = enqueue(driver);
							factory.suggest_name(node, wire_chunk.wire->name);
							factory.update_pending(pending, node);
						}
					} ບໍ່ດັ່ງນັ້ນ {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						Node node = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, node);
					}
				} ບໍ່ດັ່ງນັ້ນ ຖ້າ (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					ຖ້າ (port_chunk.is_whole()) {
						ຖ້າ (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							Node node = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, node);
						} ບໍ່ດັ່ງນັ້ນ {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} ບໍ່ດັ່ງນັ້ນ {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						Node node = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, node);
					}
				} ບໍ່ດັ່ງນັ້ນ ຖ້າ (chunk.is_constant()) {
					Node node = factory.constant(chunk.constant());
					factory.suggest_name(node, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, node);
				} ບໍ່ດັ່ງນັ້ນ ຖ້າ (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} ບໍ່ດັ່ງນັ້ນ ຖ້າ (chunk.is_none()) {
					undriven(log_signal(chunk));
				} ບໍ່ດັ່ງນັ້ນ {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} ບໍ່ດັ່ງນັ້ນ {
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
    ສົ່ງຄືນ ir;
}

void IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    bool scc = false;
    std::vector<ຈຳນວນເຕັມ> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](ຈຳນວນເຕັມ *begin, ຈຳນວນເຕັມ *end) {
        perm.insert(perm.end(), begin, end);
        ຖ້າ (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            ສຳລັບ (ຈຳນວນເຕັມ *i = begin; i != end; ++i) {
				Node node(_graph[*i], design);
                log("- %s = %s\n", node.name().unescape(), node.to_string());
			}
            log("\n");
            scc = true;
        }
    });
	ສຳລັບ(const auto &[name, state]: _states)
		ຖ້າ(state.has_next_value())
			toposort.process(state.next_value().id());
	ສຳລັບ(const auto &[name, output]: _outputs)
		ຖ້າ(output.has_value())
			toposort.process(output.value().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    ຖ້າ(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

static IdString merge_name(IdString a, IdString b) {
	ຖ້າ(!a.isPublic() && b.isPublic())
		ສົ່ງຄືນ b;
	ບໍ່ດັ່ງນັ້ນ
		ສົ່ງຄືນ a;
}

void IR::forward_buf() {
    std::vector<ຈຳນວນເຕັມ> perm, alias;
    perm.clear();

    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < _graph.size(); ++i)
    {
        auto node = _graph[i];
        ຖ້າ (node.function().fn() == Fn::buf && node.arg(0).index() < i)
        {
            ຈຳນວນເຕັມ target_index = alias[node.arg(0).index()];
            auto target_node = _graph[perm[target_index]];
			ຖ້າ(node.has_sparse_attr()) {
				ຖ້າ(target_node.has_sparse_attr()) {
					IdString id = merge_name(node.sparse_attr(), target_node.sparse_attr());
					target_node.sparse_attr() = id;
				} ບໍ່ດັ່ງນັ້ນ {
					IdString id = node.sparse_attr();
					target_node.sparse_attr() = id;
				}
			}
            alias.push_back(target_index);
        }
        ບໍ່ດັ່ງນັ້ນ
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
	ສຳລັບ(const var_na *p = fmt; *p != 0; p++) {
		switch(*p) {
		pak_sa '\n': r += "\\n"; bha_nga;
		pak_sa '\t': r += "\\t"; bha_nga;
		pak_sa '"': r += "\\\""; bha_nga;
		pak_sa '\\': r += "\\\\"; bha_nga;
		default: r += *p; bha_nga;
		}
	}
	ສົ່ງຄືນ r;
}

void Writer::print_impl(const var_na *fmt, vector<std::function<void()>> &fns)
{
	size_t next_index = 0;
	ສຳລັບ(const var_na *p = fmt; *p != 0; p++)
		switch(*p) {
		pak_sa '{':
			ຖ້າ(*++p == '{') {
				*os << '{';
			} ບໍ່ດັ່ງນັ້ນ {
				var_na *pe;
				size_t index = strtoul(p, &pe, 10);
				ຖ້າ(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				ຖ້າ(p == pe)
					index = next_index;
				ບໍ່ດັ່ງນັ້ນ
					p = pe;
				ຖ້າ(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			bha_nga;
		pak_sa '}':
			p++;
			ຖ້າ(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			bha_nga;
		default:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
