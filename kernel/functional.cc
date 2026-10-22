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

#અન્તર્ભાવ "kernel/functional.h"
#અન્તર્ભાવ "kernel/topo_scc.h"
#અન્તર્ભાવ "ff.h"
#અન્તર્ભાવ "ffinit.h"
#અન્તર્ભાવ <deque>

YOSYS_NAMESPACE_BEGIN
નામાકાશ Functional {

અચલ અક્ષર *fn_to_string(Fn fn) {
	પસંદગી(fn) {
	સ્થિતિ Fn::invalid: પાછા "invalid";
	સ્થિતિ Fn::buf: પાછા "buf";
	સ્થિતિ Fn::slice: પાછા "slice";
	સ્થિતિ Fn::zero_extend: પાછા "zero_extend";
	સ્થિતિ Fn::sign_extend: પાછા "sign_extend";
	સ્થિતિ Fn::concat: પાછા "concat";
	સ્થિતિ Fn::add: પાછા "add";
	સ્થિતિ Fn::sub: પાછા "sub";
	સ્થિતિ Fn::mul: પાછા "mul";
	સ્થિતિ Fn::unsigned_div: પાછા "unsigned_div";
	સ્થિતિ Fn::unsigned_mod: પાછા "unsigned_mod";
	સ્થિતિ Fn::bitwise_and: પાછા "bitwise_and";
	સ્થિતિ Fn::bitwise_or: પાછા "bitwise_or";
	સ્થિતિ Fn::bitwise_xor: પાછા "bitwise_xor";
	સ્થિતિ Fn::bitwise_not: પાછા "bitwise_not";
	સ્થિતિ Fn::reduce_and: પાછા "reduce_and";
	સ્થિતિ Fn::reduce_or: પાછા "reduce_or";
	સ્થિતિ Fn::reduce_xor: પાછા "reduce_xor";
	સ્થિતિ Fn::unary_minus: પાછા "unary_minus";
	સ્થિતિ Fn::equal: પાછા "equal";
	સ્થિતિ Fn::not_equal: પાછા "not_equal";
	સ્થિતિ Fn::signed_greater_than: પાછા "signed_greater_than";
	સ્થિતિ Fn::signed_greater_equal: પાછા "signed_greater_equal";
	સ્થિતિ Fn::unsigned_greater_than: પાછા "unsigned_greater_than";
	સ્થિતિ Fn::unsigned_greater_equal: પાછા "unsigned_greater_equal";
	સ્થિતિ Fn::logical_shift_ડાબું: પાછા "logical_shift_left";
	સ્થિતિ Fn::logical_shift_જમણું: પાછા "logical_shift_right";
	સ્થિતિ Fn::arithmetic_shift_જમણું: પાછા "arithmetic_shift_right";
	સ્થિતિ Fn::mux: પાછા "mux";
	સ્થિતિ Fn::constant: પાછા "constant";
	સ્થિતિ Fn::આગત: પાછા "input";
	સ્થિતિ Fn::state: પાછા "state";
	સ્થિતિ Fn::memory_read: પાછા "memory_read";
	સ્થિતિ Fn::memory_write: પાછા "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (પૂર્ણાંક)fn);
}

vector<IRInput અચલ*> IR::inputs(IdString kind) અચલ {
	vector<IRInput અચલ*> ret;
	માટે (અચલ auto &[name, આગત] : _inputs)
		જો(આગત.kind == kind)
			ret.ધકેલો_back(&આગત);
	પાછા ret;
}

vector<IROutput અચલ*> IR::outputs(IdString kind) અચલ {
	vector<IROutput અચલ*> ret;
	માટે (અચલ auto &[name, output] : _outputs)
		જો(output.kind == kind)
			ret.ધકેલો_back(&output);
	પાછા ret;
}

vector<IRState અચલ*> IR::states(IdString kind) અચલ {
	vector<IRState અચલ*> ret;
	માટે (અચલ auto &[name, state] : _states)
		જો(state.kind == kind)
			ret.ધકેલો_back(&state);
	પાછા ret;
}

vector<IRInput અચલ*> IR::all_inputs() અચલ {
	vector<IRInput અચલ*> ret;
	માટે (અચલ auto &[name, આગત] : _inputs)
		ret.ધકેલો_back(&આગત);
	પાછા ret;
}

vector<IROutput અચલ*> IR::all_outputs() અચલ {
	vector<IROutput અચલ*> ret;
	માટે (અચલ auto &[name, output] : _outputs)
		ret.ધકેલો_back(&output);
	પાછા ret;
}

vector<IRState અચલ*> IR::all_states() અચલ {
	vector<IRState અચલ*> ret;
	માટે (અચલ auto &[name, state] : _states)
		ret.ધકેલો_back(&state);
	પાછા ret;
}

સંરચના PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(ગાંઠ_2)> np;
	PrintVisitor(std::function<std::string(ગાંઠ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(ગાંઠ_2, ગાંઠ_2 a, પૂર્ણાંક offset, પૂર્ણાંક out_width) override { પાછા "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(ગાંઠ_2, ગાંઠ_2 a, પૂર્ણાંક out_width) override { પાછા "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(ગાંઠ_2, ગાંઠ_2 a, પૂર્ણાંક out_width) override { પાછા "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(ગાંઠ_2, RTLIL::Const અચલ& કિંમત) override { પાછા "constant(" + કિંમત.as_string() + ")"; }
	std::string આગત(ગાંઠ_2 self, IdString name, IdString kind) override { પાછા "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(ગાંઠ_2 self, IdString name, IdString kind) override { પાછા "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(ગાંઠ_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		માટે(size_t i = 0; i < self.arg_ગણતરી(); i++) {
			જો(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		પાછા ret;
	}
};

std::string ગાંઠ_2::to_string()
{
	પાછા to_string([](ગાંઠ_2 n) { પાછા n.name().unescape(); });
}

std::string ગાંઠ_2::to_string(std::function<std::string(ગાંઠ_2)> np)
{
	પાછા visit(PrintVisitor(np));
}

વર્ગ CellSimplifier {
	Factory &factory;
	ગાંઠ_2 sign(ગાંઠ_2 a) {
		પાછા factory.slice(a, a.width() - 1, 1);
	}
	ગાંઠ_2 neg_if(ગાંઠ_2 a, ગાંઠ_2 s) {
		પાછા factory.mux(a, factory.unary_minus(a), s);
	}
	ગાંઠ_2 abs(ગાંઠ_2 a) {
		પાછા neg_if(a, sign(a));
	}
	ગાંઠ_2 handle_shift(ગાંઠ_2 a, ગાંઠ_2 b, તાર્કિક is_જમણું, તાર્કિક is_signed) {
		// to prevent new_width == 0, we handle this case separately
		જો(a.width() == 1) {
			જો(!is_signed)
				પાછા factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			નહીંતર
				પાછા a;
		}
		પૂર્ણાંક new_width = ceil_log2(a.width());
		ગાંઠ_2 b_truncated = factory.extend(b, new_width, ખોટું);
		ગાંઠ_2 y =
			!is_જમણું ? factory.logical_shift_ડાબું(a, b_truncated) :
			!is_signed ? factory.logical_shift_જમણું(a, b_truncated) :
			factory.arithmetic_shift_જમણું(a, b_truncated);
		જો(b.width() <= new_width)
			પાછા y;
		ગાંઠ_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		ગાંઠ_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), સાચું) : factory.constant(RTLIL::Const(State::S0, a.width()));
		પાછા factory.mux(y, y_if_overflow, overflow);
	}
જાહેર:
	ગાંઠ_2 logical_shift_ડાબું(ગાંઠ_2 a, ગાંઠ_2 b) { પાછા handle_shift(a, b, ખોટું, ખોટું); }
	ગાંઠ_2 logical_shift_જમણું(ગાંઠ_2 a, ગાંઠ_2 b) { પાછા handle_shift(a, b, સાચું, ખોટું); }
	ગાંઠ_2 arithmetic_shift_જમણું(ગાંઠ_2 a, ગાંઠ_2 b) { પાછા handle_shift(a, b, સાચું, સાચું); }
	ગાંઠ_2 bitwise_mux(ગાંઠ_2 a, ગાંઠ_2 b, ગાંઠ_2 s) {
		ગાંઠ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		ગાંઠ_2 bb = factory.bitwise_and(b, s);
		પાછા factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
ખાનગી:
	ગાંઠ_2 handle_pow(ગાંઠ_2 a0, ગાંઠ_2 b, પૂર્ણાંક y_width, તાર્કિક is_signed) {
		ગાંઠ_2 a = factory.extend(a0, y_width, is_signed);
		ગાંઠ_2 r = factory.constant(Const(1, y_width));
		માટે(પૂર્ણાંક i = 0; i < b.width(); i++) {
			ગાંઠ_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		જો (is_signed) {
			ગાંઠ_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			ગાંઠ_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		પાછા r;
	}
	ગાંઠ_2 handle_bmux(ગાંઠ_2 a, ગાંઠ_2 s, પૂર્ણાંક a_offset, પૂર્ણાંક width, પૂર્ણાંક sn) {
		જો(sn < 1)
			પાછા factory.slice(a, a_offset, width);
		નહીંતર {
			ગાંઠ_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			ગાંઠ_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			પાછા factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	ગાંઠ_2 handle_pmux(ગાંઠ_2 a, ગાંઠ_2 b, ગાંઠ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		ગાંઠ_2 y = a;
		માટે(પૂર્ણાંક i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		પાછા y;
	}
	dict<IdString, ગાંઠ_2> handle_fa(ગાંઠ_2 a, ગાંઠ_2 b, ગાંઠ_2 c) {
		ગાંઠ_2 t1 = factory.bitwise_xor(a, b);
		ગાંઠ_2 t2 = factory.bitwise_and(a, b);
		ગાંઠ_2 t3 = factory.bitwise_and(c, t1);
		ગાંઠ_2 y = factory.bitwise_xor(c, t1);
		ગાંઠ_2 x = factory.bitwise_or(t2, t3);
		પાછા {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, ગાંઠ_2> handle_alu(ગાંઠ_2 a_in, ગાંઠ_2 b_in, પૂર્ણાંક y_width, તાર્કિક is_signed, ગાંઠ_2 ci, ગાંઠ_2 bi) {
		ગાંઠ_2 a = factory.extend(a_in, y_width, is_signed);
		ગાંઠ_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		ગાંઠ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		ગાંઠ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		ગાંઠ_2 a_extra = factory.extend(a, y_width + 1, ખોટું);
		ગાંઠ_2 b_extra = factory.extend(b, y_width + 1, ખોટું);
		ગાંઠ_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, ખોટું));
		ગાંઠ_2 y = factory.slice(y_extra, 0, y_width);
		ગાંઠ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		ગાંઠ_2 co = factory.slice(carries, 1, y_width);
		પાછા {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	ગાંઠ_2 handle_lcu(ગાંઠ_2 p, ગાંઠ_2 g, ગાંઠ_2 ci) {
		પાછા handle_alu(g, factory.bitwise_or(p, g), g.width(), ખોટું, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
જાહેર:
	std::variant<dict<IdString, ગાંઠ_2>, ગાંઠ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, ગાંઠ_2> inputs)
	{
		પૂર્ણાંક a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		પૂર્ણાંક b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		પૂર્ણાંક y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		તાર્કિક a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		તાર્કિક b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		જો(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			તાર્કિક is_signed = a_signed && b_signed;
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			ગાંઠ_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			જો(cellType == ID($add))
				પાછા factory.add(a, b);
			નહીંતર જો(cellType == ID($sub))
				પાછા factory.sub(a, b);
			નહીંતર જો(cellType == ID($mul))
				પાછા factory.mul(a, b);
			નહીંતર જો(cellType == ID($and))
				પાછા factory.bitwise_and(a, b);
			નહીંતર જો(cellType == ID($or))
				પાછા factory.bitwise_or(a, b);
			નહીંતર જો(cellType == ID($xor))
				પાછા factory.bitwise_xor(a, b);
			નહીંતર જો(cellType == ID($xnor))
				પાછા factory.bitwise_not(factory.bitwise_xor(a, b));
			નહીંતર
				log_abort();
		}નહીંતર જો(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			તાર્કિક is_signed = a_signed && b_signed;
			પૂર્ણાંક width = max(a_width, b_width);
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			ગાંઠ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			જો(cellType.in(ID($eq), ID($eqx)))
				પાછા factory.extend(factory.equal(a, b), y_width, ખોટું);
			નહીંતર જો(cellType.in(ID($ne), ID($nex)))
				પાછા factory.extend(factory.not_equal(a, b), y_width, ખોટું);
			નહીંતર જો(cellType == ID($lt))
				પાછા factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, ખોટું);
			નહીંતર જો(cellType == ID($le))
				પાછા factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, ખોટું);
			નહીંતર જો(cellType == ID($gt))
				પાછા factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, ખોટું);
			નહીંતર જો(cellType == ID($ge))
				પાછા factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, ખોટું);
			નહીંતર
				log_abort();
		}નહીંતર જો(cellType.in(ID($logic_or), ID($logic_and))){
			ગાંઠ_2 a = factory.reduce_or(inputs.at(ID(A)));
			ગાંઠ_2 b = factory.reduce_or(inputs.at(ID(B)));
			ગાંઠ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			પાછા factory.extend(y, y_width, ખોટું);
		}નહીંતર જો(cellType == ID($not)){
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			પાછા factory.bitwise_not(a);
		}નહીંતર જો(cellType == ID($pos)){
			પાછા factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}નહીંતર જો(cellType == ID($barrier)){
			પાછા inputs.at(ID(A));
		}નહીંતર જો(cellType == ID($neg)){
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			પાછા factory.unary_minus(a);
		}નહીંતર જો(cellType == ID($logic_not)){
			ગાંઠ_2 a = factory.reduce_or(inputs.at(ID(A)));
			ગાંઠ_2 y = factory.bitwise_not(a);
			પાછા factory.extend(y, y_width, ખોટું);
		}નહીંતર જો(cellType.in(ID($reduce_or), ID($reduce_bool))){
			ગાંઠ_2 a = factory.reduce_or(inputs.at(ID(A)));
			પાછા factory.extend(a, y_width, ખોટું);
		}નહીંતર જો(cellType == ID($reduce_and)){
			ગાંઠ_2 a = factory.reduce_and(inputs.at(ID(A)));
			પાછા factory.extend(a, y_width, ખોટું);
		}નહીંતર જો(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			ગાંઠ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			ગાંઠ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			પાછા factory.extend(y, y_width, ખોટું);
		}નહીંતર જો(cellType == ID($shl) || cellType == ID($sshl)){
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ગાંઠ_2 b = inputs.at(ID(B));
			પાછા logical_shift_ડાબું(a, b);
		}નહીંતર જો(cellType == ID($shr) || cellType == ID($sshr)){
			પૂર્ણાંક width = max(a_width, y_width);
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			ગાંઠ_2 b = inputs.at(ID(B));
			ગાંઠ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_જમણું(a, b) :
				logical_shift_જમણું(a, b);
			પાછા factory.extend(y, y_width, a_signed);
		}નહીંતર જો(cellType == ID($shiftx) || cellType == ID($shift)){
			પૂર્ણાંક width = max(a_width, y_width);
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			ગાંઠ_2 b = inputs.at(ID(B));
			ગાંઠ_2 shr = logical_shift_જમણું(a, b);
			જો(b_signed) {
				ગાંઠ_2 shl = logical_shift_ડાબું(a, factory.unary_minus(b));
				ગાંઠ_2 y = factory.mux(shr, shl, sign(b));
				પાછા factory.extend(y, y_width, ખોટું);
			} નહીંતર {
				પાછા factory.extend(shr, y_width, ખોટું);
			}
		}નહીંતર જો(cellType == ID($mux)){
			પાછા factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}નહીંતર જો(cellType == ID($pmux)){
			પાછા handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}નહીંતર જો(cellType == ID($concat)){
			ગાંઠ_2 a = inputs.at(ID(A));
			ગાંઠ_2 b = inputs.at(ID(B));
			પાછા factory.concat(a, b);
		}નહીંતર જો(cellType == ID($slice)){
			પૂર્ણાંક offset = parameters.at(ID(OFFSET)).as_int();
			ગાંઠ_2 a = inputs.at(ID(A));
			પાછા factory.slice(a, offset, y_width);
		}નહીંતર જો(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			પૂર્ણાંક width = max(a_width, b_width);
			તાર્કિક is_signed = a_signed && b_signed;
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			ગાંઠ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			જો(is_signed) {
				જો(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					ગાંઠ_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					ગાંઠ_2 out_sign = factory.not_equal(sign(a), sign(b));
					પાછા neg_if(factory.extend(abs_y, y_width, ખોટું), out_sign);
				} નહીંતર જો(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					ગાંઠ_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					પાછા neg_if(factory.extend(abs_y, y_width, ખોટું), sign(a));
				} નહીંતર જો(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					ગાંઠ_2 b_sign = sign(b);
					ગાંઠ_2 a1 = neg_if(a, b_sign);
					ગાંઠ_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					ગાંઠ_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					ગાંઠ_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					ગાંઠ_2 y1 = factory.unsigned_div(a2, b1);
					ગાંઠ_2 y2 = factory.extend(y1, y_width, ખોટું);
					પાછા factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} નહીંતર જો(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					ગાંઠ_2 abs_b = abs(b);
					ગાંઠ_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					ગાંઠ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					ગાંઠ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					ગાંઠ_2 y = neg_if(y_flipped, sign(b));
					પાછા factory.extend(y, y_width, સાચું);
				} નહીંતર
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} નહીંતર {
				જો(cellType.in(ID($mod), ID($modfloor)))
					પાછા factory.extend(factory.unsigned_mod(a, b), y_width, ખોટું);
				નહીંતર
					પાછા factory.extend(factory.unsigned_div(a, b), y_width, ખોટું);
			}
		} નહીંતર જો(cellType == ID($pow)) {
			પાછા handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} નહીંતર જો (cellType == ID($lut)) {
			પૂર્ણાંક width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			પાછા handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} નહીંતર જો (cellType == ID($bwmux)) {
			ગાંઠ_2 a = inputs.at(ID(A));
			ગાંઠ_2 b = inputs.at(ID(B));
			ગાંઠ_2 s = inputs.at(ID(S));
			પાછા factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} નહીંતર જો (cellType == ID($bweqx)) {
			ગાંઠ_2 a = inputs.at(ID(A));
			ગાંઠ_2 b = inputs.at(ID(B));
			પાછા factory.bitwise_not(factory.bitwise_xor(a, b));
		} નહીંતર જો(cellType == ID($bmux)) {
			પૂર્ણાંક width = parameters.at(ID(WIDTH)).as_int();
			પૂર્ણાંક s_width = parameters.at(ID(S_WIDTH)).as_int();
			પાછા handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} નહીંતર જો(cellType == ID($demux)) {
			પૂર્ણાંક width = parameters.at(ID(WIDTH)).as_int();
			પૂર્ણાંક s_width = parameters.at(ID(S_WIDTH)).as_int();
			પૂર્ણાંક y_width = width << s_width;
			પૂર્ણાંક b_width = ceil_log2(y_width);
			ગાંઠ_2 a = factory.extend(inputs.at(ID(A)), y_width, ખોટું);
			ગાંઠ_2 s = factory.extend(inputs.at(ID(S)), b_width, ખોટું);
			ગાંઠ_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			પાછા factory.logical_shift_ડાબું(a, b);
		} નહીંતર જો(cellType == ID($fa)) {
			પાછા handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} નહીંતર જો(cellType == ID($lcu)) {
			પાછા handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} નહીંતર જો(cellType == ID($alu)) {
			પાછા handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} નહીંતર જો(cellType.in(ID($પ્રતિજ્ઞા), ID($assume), ID($live), ID($fair), ID($cover))) {
			ગાંઠ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_કિંમત(a);
			પાછા {};
		} નહીંતર જો(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			પૂર્ણાંક width = parameters.at(ID(WIDTH)).as_int();
			auto &આગત = factory.add_આગત(cellName, cellType, Sort(width));
			પાછા factory.કિંમત(આગત);
		} નહીંતર જો(cellType == ID($initstate)) {
			જો(factory.ir().has_state(ID($initstate), ID($state)))
				પાછા factory.કિંમત(factory.ir().state(ID($initstate)));
			નહીંતર {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_કિંમત(RTLIL::Const(State::S1, 1));
				state.set_next_કિંમત(factory.constant(RTLIL::Const(State::S0, 1)));
				પાછા factory.કિંમત(state);
			}
		} નહીંતર જો(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} નહીંતર {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

વર્ગ FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, ગાંઠ_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, ગાંઠ_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	ગાંઠ_2 enqueue(DriveSpec અચલ &spec)
	{
		auto it = graph_nodes.find(spec);
		જો(it == graph_nodes.end()){
			auto ગાંઠ = factory.create_pending(spec.size());
			graph_nodes.ઉમેરો({spec, ગાંઠ});
			queue.emplace_back(spec);
			પાછા ગાંઠ;
		}નહીંતર
			પાછા it->second;
	}
	ગાંઠ_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		જો(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<ગાંઠ_2> rv;
			માટે(auto અચલ &[name, sigspec] : cell->connections())
				જો(driver_map.celltypes.cell_output(cell->type, name)) {
					auto ગાંઠ = factory.create_pending(sigspec.size());
					factory.suggest_name(ગાંઠ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, ગાંઠ);
					જો(name == port_name)
						rv = ગાંઠ;
				}
			પાછા *rv;
		} નહીંતર
			પાછા it->second;
	}
જાહેર:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		માટે (auto cell : module->cells()) {
			જો (cell->type.in(ID($પ્રતિજ્ઞા), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		માટે (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			જો (wire && wire->port_આગત) {
				factory.add_આગત(wire->name, ID($આગત), Sort(wire->width));
			}
			જો (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_કિંમત(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		માટે (auto &mem : memories_vector) {
			જો (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
ખાનગી:
	ગાંઠ_2 concatenate_read_results(Mem *mem, vector<ગાંઠ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		અચલ SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		પૂર્ણાંક current = 0;
		માટે(size_t i = 0; i < mem->rd_ports.size(); i++) {
			પૂર્ણાંક width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		ગાંઠ_2 ગાંઠ = results[0];
		માટે(size_t i = 1; i < results.size(); i++)
			ગાંઠ = factory.concat(ગાંઠ, results[i]);
		પાછા ગાંઠ;
	}
	ગાંઠ_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<ગાંઠ_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_કિંમત(MemContents(mem));
		ગાંઠ_2 ગાંઠ = factory.કિંમત(state);
		માટે (size_t i = 0; i < mem->wr_ports.size(); i++) {
			અચલ auto &wr = mem->wr_ports[i];
			જો (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ગાંઠ_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			ગાંઠ_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			ગાંઠ_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			ગાંઠ_2 old_data = factory.memory_read(ગાંઠ, addr);
			ગાંઠ_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			ગાંઠ = factory.memory_write(ગાંઠ, addr, wr_data);
		}
		જો (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		માટે (size_t i = 0; i < mem->rd_ports.size(); i++) {
			અચલ auto &rd = mem->rd_ports[i];
			જો (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ગાંઠ_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.ધકેલો_back(factory.memory_read(ગાંઠ, addr));
		}
		state.set_next_કિંમત(ગાંઠ);
		પાછા concatenate_read_results(mem, read_results);
	}
	શૂન્ય process_cell(Cell *cell)
	{
		જો (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			જો (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			ગાંઠ_2 ગાંઠ = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), ગાંઠ);
		} નહીંતર જો (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			જો (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			ગાંઠ_2 q_કિંમત = factory.કિંમત(state);
			factory.suggest_name(q_કિંમત, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_કિંમત);
			state.set_next_કિંમત(enqueue(ff.sig_d));
			state.set_initial_કિંમત(ff.val_init);
		} નહીંતર {
			dict<IdString, ગાંઠ_2> connections;
			IdString output_name; // for the single output case
			પૂર્ણાંક n_outputs = 0;
			માટે(auto અચલ &[name, sigspec] : cell->connections()) {
				જો(driver_map.celltypes.cell_આગત(cell->type, name) && sigspec.size() > 0)
					connections.ઉમેરો({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				જો(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, ગાંઠ_2>, ગાંઠ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			જો(auto *nodep = std::get_if<ગાંઠ_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} નહીંતર {
				માટે(auto [name, ગાંઠ] : std::get<dict<IdString, ગાંઠ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), ગાંઠ);
			}
		}
	}
	શૂન્ય undriven(અચલ std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	શૂન્ય check_undriven(DriveSpec અચલ& spec, std::string અચલ& name) {
		માટે(auto અચલ &chunk : spec.chunks())
			જો(chunk.is_none())
				undriven(name);
	}
જાહેર:
	શૂન્ય process_queue()
	{
		માટે (; !queue.empty(); queue.કાઢો_front()) {
			જો(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				ચાલુ;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			ગાંઠ_2 pending = graph_nodes.at(spec);

			જો (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				ગાંઠ_2 ગાંઠ = enqueue(chunks[0]);
				માટે(size_t i = 1; i < chunks.size(); i++)
					ગાંઠ = factory.concat(ગાંઠ, enqueue(chunks[i]));
				factory.update_pending(pending, ગાંઠ);
			} નહીંતર જો (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				જો (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					જો (wire_chunk.is_whole()) {
						જો (wire_chunk.wire->port_આગત) {
							ગાંઠ_2 ગાંઠ = factory.કિંમત(factory.ir().આગત(wire_chunk.wire->name));
							factory.suggest_name(ગાંઠ, wire_chunk.wire->name);
							factory.update_pending(pending, ગાંઠ);
						} નહીંતર {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							ગાંઠ_2 ગાંઠ = enqueue(driver);
							factory.suggest_name(ગાંઠ, wire_chunk.wire->name);
							factory.update_pending(pending, ગાંઠ);
						}
					} નહીંતર {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						ગાંઠ_2 ગાંઠ = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, ગાંઠ);
					}
				} નહીંતર જો (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					જો (port_chunk.is_whole()) {
						જો (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							ગાંઠ_2 ગાંઠ = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, ગાંઠ);
						} નહીંતર {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} નહીંતર {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						ગાંઠ_2 ગાંઠ = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, ગાંઠ);
					}
				} નહીંતર જો (chunk.is_constant()) {
					ગાંઠ_2 ગાંઠ = factory.constant(chunk.constant());
					factory.suggest_name(ગાંઠ, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, ગાંઠ);
				} નહીંતર જો (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} નહીંતર જો (chunk.is_none()) {
					undriven(log_signal(chunk));
				} નહીંતર {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} નહીંતર {
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
    પાછા ir;
}

શૂન્ય IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    તાર્કિક scc = ખોટું;
    std::vector<પૂર્ણાંક> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](પૂર્ણાંક *begin, પૂર્ણાંક *end) {
        perm.ઉમેરો(perm.end(), begin, end);
        જો (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            માટે (પૂર્ણાંક *i = begin; i != end; ++i) {
				ગાંઠ_2 ગાંઠ(_graph[*i], design);
                log("- %s = %s\n", ગાંઠ.name().unescape(), ગાંઠ.to_string());
			}
            log("\n");
            scc = સાચું;
        }
    });
	માટે(અચલ auto &[name, state]: _states)
		જો(state.has_next_કિંમત())
			toposort.process(state.next_કિંમત().id());
	માટે(અચલ auto &[name, output]: _outputs)
		જો(output.has_કિંમત())
			toposort.process(output.કિંમત().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    જો(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

સ્થિર IdString merge_name(IdString a, IdString b) {
	જો(!a.isPublic() && b.isPublic())
		પાછા b;
	નહીંતર
		પાછા a;
}

શૂન્ય IR::forward_buf() {
    std::vector<પૂર્ણાંક> perm, alias;
    perm.clear();

    માટે (પૂર્ણાંક i = 0; i < _graph.size(); ++i)
    {
        auto ગાંઠ = _graph[i];
        જો (ગાંઠ.function().fn() == Fn::buf && ગાંઠ.arg(0).index() < i)
        {
            પૂર્ણાંક target_index = alias[ગાંઠ.arg(0).index()];
            auto target_ગાંઠ = _graph[perm[target_index]];
			જો(ગાંઠ.has_sparse_attr()) {
				જો(target_ગાંઠ.has_sparse_attr()) {
					IdString id = merge_name(ગાંઠ.sparse_attr(), target_ગાંઠ.sparse_attr());
					target_ગાંઠ.sparse_attr() = id;
				} નહીંતર {
					IdString id = ગાંઠ.sparse_attr();
					target_ગાંઠ.sparse_attr() = id;
				}
			}
            alias.ધકેલો_back(target_index);
        }
        નહીંતર
        {
            alias.ધકેલો_back(GetSize(perm));
            perm.ધકેલો_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
સ્થિર std::string quote_fmt(અચલ અક્ષર *fmt)
{
	std::string r;
	માટે(અચલ અક્ષર *p = fmt; *p != 0; p++) {
		પસંદગી(*p) {
		સ્થિતિ '\n': r += "\\n"; તોડો;
		સ્થિતિ '\t': r += "\\t"; તોડો;
		સ્થિતિ '"': r += "\\\""; તોડો;
		સ્થિતિ '\\': r += "\\\\"; તોડો;
		મૂળભૂત: r += *p; તોડો;
		}
	}
	પાછા r;
}

શૂન્ય Writer::print_impl(અચલ અક્ષર *fmt, vector<std::function<શૂન્ય()>> &fns)
{
	size_t next_index = 0;
	માટે(અચલ અક્ષર *p = fmt; *p != 0; p++)
		પસંદગી(*p) {
		સ્થિતિ '{':
			જો(*++p == '{') {
				*os << '{';
			} નહીંતર {
				અક્ષર *pe;
				size_t index = strtoul(p, &pe, 10);
				જો(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				જો(p == pe)
					index = next_index;
				નહીંતર
					p = pe;
				જો(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			તોડો;
		સ્થિતિ '}':
			p++;
			જો(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			તોડો;
		મૂળભૂત:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
