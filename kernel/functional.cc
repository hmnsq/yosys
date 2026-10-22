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

#அந்தர்பாவ "kernel/functional.h"
#அந்தர்பாவ "kernel/topo_scc.h"
#அந்தர்பாவ "ff.h"
#அந்தர்பாவ "ffinit.h"
#அந்தர்பாவ <deque>

YOSYS_NAMESPACE_BEGIN
நாமாகாஶ Functional {

மாறிலி எழுத்து *fn_to_string(Fn fn) {
	தேர்வு(fn) {
	நிலை Fn::invalid: திரும்பு "invalid";
	நிலை Fn::buf: திரும்பு "buf";
	நிலை Fn::slice: திரும்பு "slice";
	நிலை Fn::zero_extend: திரும்பு "zero_extend";
	நிலை Fn::sign_extend: திரும்பு "sign_extend";
	நிலை Fn::concat: திரும்பு "concat";
	நிலை Fn::add: திரும்பு "add";
	நிலை Fn::sub: திரும்பு "sub";
	நிலை Fn::mul: திரும்பு "mul";
	நிலை Fn::unsigned_div: திரும்பு "unsigned_div";
	நிலை Fn::unsigned_mod: திரும்பு "unsigned_mod";
	நிலை Fn::bitwise_and: திரும்பு "bitwise_and";
	நிலை Fn::bitwise_or: திரும்பு "bitwise_or";
	நிலை Fn::bitwise_xor: திரும்பு "bitwise_xor";
	நிலை Fn::bitwise_not: திரும்பு "bitwise_not";
	நிலை Fn::reduce_and: திரும்பு "reduce_and";
	நிலை Fn::reduce_or: திரும்பு "reduce_or";
	நிலை Fn::reduce_xor: திரும்பு "reduce_xor";
	நிலை Fn::unary_minus: திரும்பு "unary_minus";
	நிலை Fn::equal: திரும்பு "equal";
	நிலை Fn::not_equal: திரும்பு "not_equal";
	நிலை Fn::signed_greater_than: திரும்பு "signed_greater_than";
	நிலை Fn::signed_greater_equal: திரும்பு "signed_greater_equal";
	நிலை Fn::unsigned_greater_than: திரும்பு "unsigned_greater_than";
	நிலை Fn::unsigned_greater_equal: திரும்பு "unsigned_greater_equal";
	நிலை Fn::logical_shift_இடது: திரும்பு "logical_shift_left";
	நிலை Fn::logical_shift_வலது: திரும்பு "logical_shift_right";
	நிலை Fn::arithmetic_shift_வலது: திரும்பு "arithmetic_shift_right";
	நிலை Fn::mux: திரும்பு "mux";
	நிலை Fn::constant: திரும்பு "constant";
	நிலை Fn::உள்ளீடு: திரும்பு "input";
	நிலை Fn::state: திரும்பு "state";
	நிலை Fn::memory_read: திரும்பு "memory_read";
	நிலை Fn::memory_write: திரும்பு "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (முழு)fn);
}

vector<IRInput மாறிலி*> IR::inputs(IdString kind) மாறிலி {
	vector<IRInput மாறிலி*> ret;
	க்கு (மாறிலி auto &[name, உள்ளீடு] : _inputs)
		எனில்(உள்ளீடு.kind == kind)
			ret.தள்ளு_back(&உள்ளீடு);
	திரும்பு ret;
}

vector<IROutput மாறிலி*> IR::outputs(IdString kind) மாறிலி {
	vector<IROutput மாறிலி*> ret;
	க்கு (மாறிலி auto &[name, output] : _outputs)
		எனில்(output.kind == kind)
			ret.தள்ளு_back(&output);
	திரும்பு ret;
}

vector<IRState மாறிலி*> IR::states(IdString kind) மாறிலி {
	vector<IRState மாறிலி*> ret;
	க்கு (மாறிலி auto &[name, state] : _states)
		எனில்(state.kind == kind)
			ret.தள்ளு_back(&state);
	திரும்பு ret;
}

vector<IRInput மாறிலி*> IR::all_inputs() மாறிலி {
	vector<IRInput மாறிலி*> ret;
	க்கு (மாறிலி auto &[name, உள்ளீடு] : _inputs)
		ret.தள்ளு_back(&உள்ளீடு);
	திரும்பு ret;
}

vector<IROutput மாறிலி*> IR::all_outputs() மாறிலி {
	vector<IROutput மாறிலி*> ret;
	க்கு (மாறிலி auto &[name, output] : _outputs)
		ret.தள்ளு_back(&output);
	திரும்பு ret;
}

vector<IRState மாறிலி*> IR::all_states() மாறிலி {
	vector<IRState மாறிலி*> ret;
	க்கு (மாறிலி auto &[name, state] : _states)
		ret.தள்ளு_back(&state);
	திரும்பு ret;
}

கட்டமைப்பு PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(முனை_2)> np;
	PrintVisitor(std::function<std::string(முனை_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(முனை_2, முனை_2 a, முழு offset, முழு out_width) override { திரும்பு "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(முனை_2, முனை_2 a, முழு out_width) override { திரும்பு "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(முனை_2, முனை_2 a, முழு out_width) override { திரும்பு "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(முனை_2, RTLIL::Const மாறிலி& மதிப்பு) override { திரும்பு "constant(" + மதிப்பு.as_string() + ")"; }
	std::string உள்ளீடு(முனை_2 self, IdString name, IdString kind) override { திரும்பு "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(முனை_2 self, IdString name, IdString kind) override { திரும்பு "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(முனை_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		க்கு(size_t i = 0; i < self.arg_மொத்தம்(); i++) {
			எனில்(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		திரும்பு ret;
	}
};

std::string முனை_2::to_string()
{
	திரும்பு to_string([](முனை_2 n) { திரும்பு n.name().unescape(); });
}

std::string முனை_2::to_string(std::function<std::string(முனை_2)> np)
{
	திரும்பு visit(PrintVisitor(np));
}

வகுப்பு CellSimplifier {
	Factory &factory;
	முனை_2 sign(முனை_2 a) {
		திரும்பு factory.slice(a, a.width() - 1, 1);
	}
	முனை_2 neg_if(முனை_2 a, முனை_2 s) {
		திரும்பு factory.mux(a, factory.unary_minus(a), s);
	}
	முனை_2 abs(முனை_2 a) {
		திரும்பு neg_if(a, sign(a));
	}
	முனை_2 handle_shift(முனை_2 a, முனை_2 b, தருக்க is_வலது, தருக்க is_signed) {
		// to prevent new_width == 0, we handle this case separately
		எனில்(a.width() == 1) {
			எனில்(!is_signed)
				திரும்பு factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			இல்லையெனில்
				திரும்பு a;
		}
		முழு new_width = ceil_log2(a.width());
		முனை_2 b_truncated = factory.extend(b, new_width, பொய்);
		முனை_2 y =
			!is_வலது ? factory.logical_shift_இடது(a, b_truncated) :
			!is_signed ? factory.logical_shift_வலது(a, b_truncated) :
			factory.arithmetic_shift_வலது(a, b_truncated);
		எனில்(b.width() <= new_width)
			திரும்பு y;
		முனை_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		முனை_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), உண்மை) : factory.constant(RTLIL::Const(State::S0, a.width()));
		திரும்பு factory.mux(y, y_if_overflow, overflow);
	}
பொது:
	முனை_2 logical_shift_இடது(முனை_2 a, முனை_2 b) { திரும்பு handle_shift(a, b, பொய், பொய்); }
	முனை_2 logical_shift_வலது(முனை_2 a, முனை_2 b) { திரும்பு handle_shift(a, b, உண்மை, பொய்); }
	முனை_2 arithmetic_shift_வலது(முனை_2 a, முனை_2 b) { திரும்பு handle_shift(a, b, உண்மை, உண்மை); }
	முனை_2 bitwise_mux(முனை_2 a, முனை_2 b, முனை_2 s) {
		முனை_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		முனை_2 bb = factory.bitwise_and(b, s);
		திரும்பு factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
தனிப்பட்ட:
	முனை_2 handle_pow(முனை_2 a0, முனை_2 b, முழு y_width, தருக்க is_signed) {
		முனை_2 a = factory.extend(a0, y_width, is_signed);
		முனை_2 r = factory.constant(Const(1, y_width));
		க்கு(முழு i = 0; i < b.width(); i++) {
			முனை_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		எனில் (is_signed) {
			முனை_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			முனை_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		திரும்பு r;
	}
	முனை_2 handle_bmux(முனை_2 a, முனை_2 s, முழு a_offset, முழு width, முழு sn) {
		எனில்(sn < 1)
			திரும்பு factory.slice(a, a_offset, width);
		இல்லையெனில் {
			முனை_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			முனை_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			திரும்பு factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	முனை_2 handle_pmux(முனை_2 a, முனை_2 b, முனை_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		முனை_2 y = a;
		க்கு(முழு i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		திரும்பு y;
	}
	dict<IdString, முனை_2> handle_fa(முனை_2 a, முனை_2 b, முனை_2 c) {
		முனை_2 t1 = factory.bitwise_xor(a, b);
		முனை_2 t2 = factory.bitwise_and(a, b);
		முனை_2 t3 = factory.bitwise_and(c, t1);
		முனை_2 y = factory.bitwise_xor(c, t1);
		முனை_2 x = factory.bitwise_or(t2, t3);
		திரும்பு {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, முனை_2> handle_alu(முனை_2 a_in, முனை_2 b_in, முழு y_width, தருக்க is_signed, முனை_2 ci, முனை_2 bi) {
		முனை_2 a = factory.extend(a_in, y_width, is_signed);
		முனை_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		முனை_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		முனை_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		முனை_2 a_extra = factory.extend(a, y_width + 1, பொய்);
		முனை_2 b_extra = factory.extend(b, y_width + 1, பொய்);
		முனை_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, பொய்));
		முனை_2 y = factory.slice(y_extra, 0, y_width);
		முனை_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		முனை_2 co = factory.slice(carries, 1, y_width);
		திரும்பு {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	முனை_2 handle_lcu(முனை_2 p, முனை_2 g, முனை_2 ci) {
		திரும்பு handle_alu(g, factory.bitwise_or(p, g), g.width(), பொய், ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
பொது:
	std::variant<dict<IdString, முனை_2>, முனை_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, முனை_2> inputs)
	{
		முழு a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		முழு b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		முழு y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		தருக்க a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		தருக்க b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		எனில்(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			தருக்க is_signed = a_signed && b_signed;
			முனை_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			முனை_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			எனில்(cellType == ID($add))
				திரும்பு factory.add(a, b);
			இல்லையெனில் எனில்(cellType == ID($sub))
				திரும்பு factory.sub(a, b);
			இல்லையெனில் எனில்(cellType == ID($mul))
				திரும்பு factory.mul(a, b);
			இல்லையெனில் எனில்(cellType == ID($and))
				திரும்பு factory.bitwise_and(a, b);
			இல்லையெனில் எனில்(cellType == ID($or))
				திரும்பு factory.bitwise_or(a, b);
			இல்லையெனில் எனில்(cellType == ID($xor))
				திரும்பு factory.bitwise_xor(a, b);
			இல்லையெனில் எனில்(cellType == ID($xnor))
				திரும்பு factory.bitwise_not(factory.bitwise_xor(a, b));
			இல்லையெனில்
				log_abort();
		}இல்லையெனில் எனில்(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			தருக்க is_signed = a_signed && b_signed;
			முழு width = max(a_width, b_width);
			முனை_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			முனை_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			எனில்(cellType.in(ID($eq), ID($eqx)))
				திரும்பு factory.extend(factory.equal(a, b), y_width, பொய்);
			இல்லையெனில் எனில்(cellType.in(ID($ne), ID($nex)))
				திரும்பு factory.extend(factory.not_equal(a, b), y_width, பொய்);
			இல்லையெனில் எனில்(cellType == ID($lt))
				திரும்பு factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, பொய்);
			இல்லையெனில் எனில்(cellType == ID($le))
				திரும்பு factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, பொய்);
			இல்லையெனில் எனில்(cellType == ID($gt))
				திரும்பு factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, பொய்);
			இல்லையெனில் எனில்(cellType == ID($ge))
				திரும்பு factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, பொய்);
			இல்லையெனில்
				log_abort();
		}இல்லையெனில் எனில்(cellType.in(ID($logic_or), ID($logic_and))){
			முனை_2 a = factory.reduce_or(inputs.at(ID(A)));
			முனை_2 b = factory.reduce_or(inputs.at(ID(B)));
			முனை_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			திரும்பு factory.extend(y, y_width, பொய்);
		}இல்லையெனில் எனில்(cellType == ID($not)){
			முனை_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			திரும்பு factory.bitwise_not(a);
		}இல்லையெனில் எனில்(cellType == ID($pos)){
			திரும்பு factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}இல்லையெனில் எனில்(cellType == ID($barrier)){
			திரும்பு inputs.at(ID(A));
		}இல்லையெனில் எனில்(cellType == ID($neg)){
			முனை_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			திரும்பு factory.unary_minus(a);
		}இல்லையெனில் எனில்(cellType == ID($logic_not)){
			முனை_2 a = factory.reduce_or(inputs.at(ID(A)));
			முனை_2 y = factory.bitwise_not(a);
			திரும்பு factory.extend(y, y_width, பொய்);
		}இல்லையெனில் எனில்(cellType.in(ID($reduce_or), ID($reduce_bool))){
			முனை_2 a = factory.reduce_or(inputs.at(ID(A)));
			திரும்பு factory.extend(a, y_width, பொய்);
		}இல்லையெனில் எனில்(cellType == ID($reduce_and)){
			முனை_2 a = factory.reduce_and(inputs.at(ID(A)));
			திரும்பு factory.extend(a, y_width, பொய்);
		}இல்லையெனில் எனில்(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			முனை_2 a = factory.reduce_xor(inputs.at(ID(A)));
			முனை_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			திரும்பு factory.extend(y, y_width, பொய்);
		}இல்லையெனில் எனில்(cellType == ID($shl) || cellType == ID($sshl)){
			முனை_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			முனை_2 b = inputs.at(ID(B));
			திரும்பு logical_shift_இடது(a, b);
		}இல்லையெனில் எனில்(cellType == ID($shr) || cellType == ID($sshr)){
			முழு width = max(a_width, y_width);
			முனை_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			முனை_2 b = inputs.at(ID(B));
			முனை_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_வலது(a, b) :
				logical_shift_வலது(a, b);
			திரும்பு factory.extend(y, y_width, a_signed);
		}இல்லையெனில் எனில்(cellType == ID($shiftx) || cellType == ID($shift)){
			முழு width = max(a_width, y_width);
			முனை_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			முனை_2 b = inputs.at(ID(B));
			முனை_2 shr = logical_shift_வலது(a, b);
			எனில்(b_signed) {
				முனை_2 shl = logical_shift_இடது(a, factory.unary_minus(b));
				முனை_2 y = factory.mux(shr, shl, sign(b));
				திரும்பு factory.extend(y, y_width, பொய்);
			} இல்லையெனில் {
				திரும்பு factory.extend(shr, y_width, பொய்);
			}
		}இல்லையெனில் எனில்(cellType == ID($mux)){
			திரும்பு factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}இல்லையெனில் எனில்(cellType == ID($pmux)){
			திரும்பு handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}இல்லையெனில் எனில்(cellType == ID($concat)){
			முனை_2 a = inputs.at(ID(A));
			முனை_2 b = inputs.at(ID(B));
			திரும்பு factory.concat(a, b);
		}இல்லையெனில் எனில்(cellType == ID($slice)){
			முழு offset = parameters.at(ID(OFFSET)).as_int();
			முனை_2 a = inputs.at(ID(A));
			திரும்பு factory.slice(a, offset, y_width);
		}இல்லையெனில் எனில்(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			முழு width = max(a_width, b_width);
			தருக்க is_signed = a_signed && b_signed;
			முனை_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			முனை_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			எனில்(is_signed) {
				எனில்(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					முனை_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					முனை_2 out_sign = factory.not_equal(sign(a), sign(b));
					திரும்பு neg_if(factory.extend(abs_y, y_width, பொய்), out_sign);
				} இல்லையெனில் எனில்(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					முனை_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					திரும்பு neg_if(factory.extend(abs_y, y_width, பொய்), sign(a));
				} இல்லையெனில் எனில்(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					முனை_2 b_sign = sign(b);
					முனை_2 a1 = neg_if(a, b_sign);
					முனை_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					முனை_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					முனை_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					முனை_2 y1 = factory.unsigned_div(a2, b1);
					முனை_2 y2 = factory.extend(y1, y_width, பொய்);
					திரும்பு factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} இல்லையெனில் எனில்(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					முனை_2 abs_b = abs(b);
					முனை_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					முனை_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					முனை_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					முனை_2 y = neg_if(y_flipped, sign(b));
					திரும்பு factory.extend(y, y_width, உண்மை);
				} இல்லையெனில்
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} இல்லையெனில் {
				எனில்(cellType.in(ID($mod), ID($modfloor)))
					திரும்பு factory.extend(factory.unsigned_mod(a, b), y_width, பொய்);
				இல்லையெனில்
					திரும்பு factory.extend(factory.unsigned_div(a, b), y_width, பொய்);
			}
		} இல்லையெனில் எனில்(cellType == ID($pow)) {
			திரும்பு handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} இல்லையெனில் எனில் (cellType == ID($lut)) {
			முழு width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			திரும்பு handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} இல்லையெனில் எனில் (cellType == ID($bwmux)) {
			முனை_2 a = inputs.at(ID(A));
			முனை_2 b = inputs.at(ID(B));
			முனை_2 s = inputs.at(ID(S));
			திரும்பு factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} இல்லையெனில் எனில் (cellType == ID($bweqx)) {
			முனை_2 a = inputs.at(ID(A));
			முனை_2 b = inputs.at(ID(B));
			திரும்பு factory.bitwise_not(factory.bitwise_xor(a, b));
		} இல்லையெனில் எனில்(cellType == ID($bmux)) {
			முழு width = parameters.at(ID(WIDTH)).as_int();
			முழு s_width = parameters.at(ID(S_WIDTH)).as_int();
			திரும்பு handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} இல்லையெனில் எனில்(cellType == ID($demux)) {
			முழு width = parameters.at(ID(WIDTH)).as_int();
			முழு s_width = parameters.at(ID(S_WIDTH)).as_int();
			முழு y_width = width << s_width;
			முழு b_width = ceil_log2(y_width);
			முனை_2 a = factory.extend(inputs.at(ID(A)), y_width, பொய்);
			முனை_2 s = factory.extend(inputs.at(ID(S)), b_width, பொய்);
			முனை_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			திரும்பு factory.logical_shift_இடது(a, b);
		} இல்லையெனில் எனில்(cellType == ID($fa)) {
			திரும்பு handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} இல்லையெனில் எனில்(cellType == ID($lcu)) {
			திரும்பு handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} இல்லையெனில் எனில்(cellType == ID($alu)) {
			திரும்பு handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} இல்லையெனில் எனில்(cellType.in(ID($ப்ரதிஜ்ஞா), ID($assume), ID($live), ID($fair), ID($cover))) {
			முனை_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_மதிப்பு(a);
			திரும்பு {};
		} இல்லையெனில் எனில்(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			முழு width = parameters.at(ID(WIDTH)).as_int();
			auto &உள்ளீடு = factory.add_உள்ளீடு(cellName, cellType, Sort(width));
			திரும்பு factory.மதிப்பு(உள்ளீடு);
		} இல்லையெனில் எனில்(cellType == ID($initstate)) {
			எனில்(factory.ir().has_state(ID($initstate), ID($state)))
				திரும்பு factory.மதிப்பு(factory.ir().state(ID($initstate)));
			இல்லையெனில் {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_மதிப்பு(RTLIL::Const(State::S1, 1));
				state.set_next_மதிப்பு(factory.constant(RTLIL::Const(State::S0, 1)));
				திரும்பு factory.மதிப்பு(state);
			}
		} இல்லையெனில் எனில்(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} இல்லையெனில் {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

வகுப்பு FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, முனை_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, முனை_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	முனை_2 enqueue(DriveSpec மாறிலி &spec)
	{
		auto it = graph_nodes.find(spec);
		எனில்(it == graph_nodes.end()){
			auto முனை = factory.create_pending(spec.size());
			graph_nodes.செருகு({spec, முனை});
			queue.emplace_back(spec);
			திரும்பு முனை;
		}இல்லையெனில்
			திரும்பு it->second;
	}
	முனை_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		எனில்(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<முனை_2> rv;
			க்கு(auto மாறிலி &[name, sigspec] : cell->connections())
				எனில்(driver_map.celltypes.cell_output(cell->type, name)) {
					auto முனை = factory.create_pending(sigspec.size());
					factory.suggest_name(முனை, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, முனை);
					எனில்(name == port_name)
						rv = முனை;
				}
			திரும்பு *rv;
		} இல்லையெனில்
			திரும்பு it->second;
	}
பொது:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		க்கு (auto cell : module->cells()) {
			எனில் (cell->type.in(ID($ப்ரதிஜ்ஞா), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		க்கு (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			எனில் (wire && wire->port_உள்ளீடு) {
				factory.add_உள்ளீடு(wire->name, ID($உள்ளீடு), Sort(wire->width));
			}
			எனில் (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_மதிப்பு(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		க்கு (auto &mem : memories_vector) {
			எனில் (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
தனிப்பட்ட:
	முனை_2 concatenate_read_results(Mem *mem, vector<முனை_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		மாறிலி SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		முழு current = 0;
		க்கு(size_t i = 0; i < mem->rd_ports.size(); i++) {
			முழு width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		முனை_2 முனை = results[0];
		க்கு(size_t i = 1; i < results.size(); i++)
			முனை = factory.concat(முனை, results[i]);
		திரும்பு முனை;
	}
	முனை_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<முனை_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_மதிப்பு(MemContents(mem));
		முனை_2 முனை = factory.மதிப்பு(state);
		க்கு (size_t i = 0; i < mem->wr_ports.size(); i++) {
			மாறிலி auto &wr = mem->wr_ports[i];
			எனில் (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			முனை_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			முனை_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			முனை_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			முனை_2 old_data = factory.memory_read(முனை, addr);
			முனை_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			முனை = factory.memory_write(முனை, addr, wr_data);
		}
		எனில் (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		க்கு (size_t i = 0; i < mem->rd_ports.size(); i++) {
			மாறிலி auto &rd = mem->rd_ports[i];
			எனில் (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			முனை_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.தள்ளு_back(factory.memory_read(முனை, addr));
		}
		state.set_next_மதிப்பு(முனை);
		திரும்பு concatenate_read_results(mem, read_results);
	}
	வெறுமை process_cell(Cell *cell)
	{
		எனில் (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			எனில் (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			முனை_2 முனை = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), முனை);
		} இல்லையெனில் எனில் (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			எனில் (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			முனை_2 q_மதிப்பு = factory.மதிப்பு(state);
			factory.suggest_name(q_மதிப்பு, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_மதிப்பு);
			state.set_next_மதிப்பு(enqueue(ff.sig_d));
			state.set_initial_மதிப்பு(ff.val_init);
		} இல்லையெனில் {
			dict<IdString, முனை_2> connections;
			IdString output_name; // for the single output case
			முழு n_outputs = 0;
			க்கு(auto மாறிலி &[name, sigspec] : cell->connections()) {
				எனில்(driver_map.celltypes.cell_உள்ளீடு(cell->type, name) && sigspec.size() > 0)
					connections.செருகு({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				எனில்(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, முனை_2>, முனை_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			எனில்(auto *nodep = std::get_if<முனை_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} இல்லையெனில் {
				க்கு(auto [name, முனை] : std::get<dict<IdString, முனை_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), முனை);
			}
		}
	}
	வெறுமை undriven(மாறிலி std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	வெறுமை check_undriven(DriveSpec மாறிலி& spec, std::string மாறிலி& name) {
		க்கு(auto மாறிலி &chunk : spec.chunks())
			எனில்(chunk.is_none())
				undriven(name);
	}
பொது:
	வெறுமை process_queue()
	{
		க்கு (; !queue.empty(); queue.எடு_front()) {
			எனில்(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				தொடர்;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			முனை_2 pending = graph_nodes.at(spec);

			எனில் (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				முனை_2 முனை = enqueue(chunks[0]);
				க்கு(size_t i = 1; i < chunks.size(); i++)
					முனை = factory.concat(முனை, enqueue(chunks[i]));
				factory.update_pending(pending, முனை);
			} இல்லையெனில் எனில் (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				எனில் (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					எனில் (wire_chunk.is_whole()) {
						எனில் (wire_chunk.wire->port_உள்ளீடு) {
							முனை_2 முனை = factory.மதிப்பு(factory.ir().உள்ளீடு(wire_chunk.wire->name));
							factory.suggest_name(முனை, wire_chunk.wire->name);
							factory.update_pending(pending, முனை);
						} இல்லையெனில் {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							முனை_2 முனை = enqueue(driver);
							factory.suggest_name(முனை, wire_chunk.wire->name);
							factory.update_pending(pending, முனை);
						}
					} இல்லையெனில் {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						முனை_2 முனை = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, முனை);
					}
				} இல்லையெனில் எனில் (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					எனில் (port_chunk.is_whole()) {
						எனில் (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							முனை_2 முனை = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, முனை);
						} இல்லையெனில் {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} இல்லையெனில் {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						முனை_2 முனை = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, முனை);
					}
				} இல்லையெனில் எனில் (chunk.is_constant()) {
					முனை_2 முனை = factory.constant(chunk.constant());
					factory.suggest_name(முனை, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, முனை);
				} இல்லையெனில் எனில் (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} இல்லையெனில் எனில் (chunk.is_none()) {
					undriven(log_signal(chunk));
				} இல்லையெனில் {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} இல்லையெனில் {
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
    திரும்பு ir;
}

வெறுமை IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    தருக்க scc = பொய்;
    std::vector<முழு> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](முழு *begin, முழு *end) {
        perm.செருகு(perm.end(), begin, end);
        எனில் (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            க்கு (முழு *i = begin; i != end; ++i) {
				முனை_2 முனை(_graph[*i], design);
                log("- %s = %s\n", முனை.name().unescape(), முனை.to_string());
			}
            log("\n");
            scc = உண்மை;
        }
    });
	க்கு(மாறிலி auto &[name, state]: _states)
		எனில்(state.has_next_மதிப்பு())
			toposort.process(state.next_மதிப்பு().id());
	க்கு(மாறிலி auto &[name, output]: _outputs)
		எனில்(output.has_மதிப்பு())
			toposort.process(output.மதிப்பு().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    எனில்(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

நிலையான IdString merge_name(IdString a, IdString b) {
	எனில்(!a.isPublic() && b.isPublic())
		திரும்பு b;
	இல்லையெனில்
		திரும்பு a;
}

வெறுமை IR::forward_buf() {
    std::vector<முழு> perm, alias;
    perm.clear();

    க்கு (முழு i = 0; i < _graph.size(); ++i)
    {
        auto முனை = _graph[i];
        எனில் (முனை.function().fn() == Fn::buf && முனை.arg(0).index() < i)
        {
            முழு target_index = alias[முனை.arg(0).index()];
            auto target_முனை = _graph[perm[target_index]];
			எனில்(முனை.has_sparse_attr()) {
				எனில்(target_முனை.has_sparse_attr()) {
					IdString id = merge_name(முனை.sparse_attr(), target_முனை.sparse_attr());
					target_முனை.sparse_attr() = id;
				} இல்லையெனில் {
					IdString id = முனை.sparse_attr();
					target_முனை.sparse_attr() = id;
				}
			}
            alias.தள்ளு_back(target_index);
        }
        இல்லையெனில்
        {
            alias.தள்ளு_back(GetSize(perm));
            perm.தள்ளு_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
நிலையான std::string quote_fmt(மாறிலி எழுத்து *fmt)
{
	std::string r;
	க்கு(மாறிலி எழுத்து *p = fmt; *p != 0; p++) {
		தேர்வு(*p) {
		நிலை '\n': r += "\\n"; நிறுத்து;
		நிலை '\t': r += "\\t"; நிறுத்து;
		நிலை '"': r += "\\\""; நிறுத்து;
		நிலை '\\': r += "\\\\"; நிறுத்து;
		இயல்பு: r += *p; நிறுத்து;
		}
	}
	திரும்பு r;
}

வெறுமை Writer::print_impl(மாறிலி எழுத்து *fmt, vector<std::function<வெறுமை()>> &fns)
{
	size_t next_index = 0;
	க்கு(மாறிலி எழுத்து *p = fmt; *p != 0; p++)
		தேர்வு(*p) {
		நிலை '{':
			எனில்(*++p == '{') {
				*os << '{';
			} இல்லையெனில் {
				எழுத்து *pe;
				size_t index = strtoul(p, &pe, 10);
				எனில்(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				எனில்(p == pe)
					index = next_index;
				இல்லையெனில்
					p = pe;
				எனில்(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			நிறுத்து;
		நிலை '}':
			p++;
			எனில்(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			நிறுத்து;
		இயல்பு:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
