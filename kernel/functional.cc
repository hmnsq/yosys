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

#समावेश "kernel/functional.h"
#समावेश "kernel/topo_scc.h"
#समावेश "ff.h"
#समावेश "ffinit.h"
#समावेश <deque>

YOSYS_NAMESPACE_BEGIN
namespace Functional {

स्थिर अक्षर *fn_to_string(Fn fn) {
	चयन(fn) {
	हाल Fn::invalid: वापस "invalid";
	हाल Fn::buf: वापस "buf";
	हाल Fn::slice: वापस "slice";
	हाल Fn::zero_extend: वापस "zero_extend";
	हाल Fn::sign_extend: वापस "sign_extend";
	हाल Fn::concat: वापस "concat";
	हाल Fn::add: वापस "add";
	हाल Fn::sub: वापस "sub";
	हाल Fn::mul: वापस "mul";
	हाल Fn::unsigned_div: वापस "unsigned_div";
	हाल Fn::unsigned_mod: वापस "unsigned_mod";
	हाल Fn::bitwise_and: वापस "bitwise_and";
	हाल Fn::bitwise_or: वापस "bitwise_or";
	हाल Fn::bitwise_xor: वापस "bitwise_xor";
	हाल Fn::bitwise_not: वापस "bitwise_not";
	हाल Fn::reduce_and: वापस "reduce_and";
	हाल Fn::reduce_or: वापस "reduce_or";
	हाल Fn::reduce_xor: वापस "reduce_xor";
	हाल Fn::unary_minus: वापस "unary_minus";
	हाल Fn::equal: वापस "equal";
	हाल Fn::not_equal: वापस "not_equal";
	हाल Fn::signed_greater_than: वापस "signed_greater_than";
	हाल Fn::signed_greater_equal: वापस "signed_greater_equal";
	हाल Fn::unsigned_greater_than: वापस "unsigned_greater_than";
	हाल Fn::unsigned_greater_equal: वापस "unsigned_greater_equal";
	हाल Fn::logical_shift_बायाँ: वापस "logical_shift_left";
	हाल Fn::logical_shift_दायाँ: वापस "logical_shift_right";
	हाल Fn::arithmetic_shift_दायाँ: वापस "arithmetic_shift_right";
	हाल Fn::mux: वापस "mux";
	हाल Fn::constant: वापस "constant";
	हाल Fn::निवेश: वापस "input";
	हाल Fn::state: वापस "state";
	हाल Fn::memory_read: वापस "memory_read";
	हाल Fn::memory_write: वापस "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (पूर्णांक)fn);
}

vector<IRInput स्थिर*> IR::inputs(IdString kind) स्थिर {
	vector<IRInput स्थिर*> ret;
	क्रम (स्थिर स्वतः &[name, निवेश] : _inputs)
		अगर(निवेश.kind == kind)
			ret.धकेलो_back(&निवेश);
	वापस ret;
}

vector<IROutput स्थिर*> IR::outputs(IdString kind) स्थिर {
	vector<IROutput स्थिर*> ret;
	क्रम (स्थिर स्वतः &[name, output] : _outputs)
		अगर(output.kind == kind)
			ret.धकेलो_back(&output);
	वापस ret;
}

vector<IRState स्थिर*> IR::states(IdString kind) स्थिर {
	vector<IRState स्थिर*> ret;
	क्रम (स्थिर स्वतः &[name, state] : _states)
		अगर(state.kind == kind)
			ret.धकेलो_back(&state);
	वापस ret;
}

vector<IRInput स्थिर*> IR::all_inputs() स्थिर {
	vector<IRInput स्थिर*> ret;
	क्रम (स्थिर स्वतः &[name, निवेश] : _inputs)
		ret.धकेलो_back(&निवेश);
	वापस ret;
}

vector<IROutput स्थिर*> IR::all_outputs() स्थिर {
	vector<IROutput स्थिर*> ret;
	क्रम (स्थिर स्वतः &[name, output] : _outputs)
		ret.धकेलो_back(&output);
	वापस ret;
}

vector<IRState स्थिर*> IR::all_states() स्थिर {
	vector<IRState स्थिर*> ret;
	क्रम (स्थिर स्वतः &[name, state] : _states)
		ret.धकेलो_back(&state);
	वापस ret;
}

काष्ठा PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(गाँठ_2)> np;
	PrintVisitor(std::function<std::string(गाँठ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(गाँठ_2, गाँठ_2 a, पूर्णांक offset, पूर्णांक out_width) override { वापस "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(गाँठ_2, गाँठ_2 a, पूर्णांक out_width) override { वापस "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(गाँठ_2, गाँठ_2 a, पूर्णांक out_width) override { वापस "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(गाँठ_2, RTLIL::Const स्थिर& मूल्य) override { वापस "constant(" + मूल्य.as_string() + ")"; }
	std::string निवेश(गाँठ_2 self, IdString name, IdString kind) override { वापस "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(गाँठ_2 self, IdString name, IdString kind) override { वापस "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(गाँठ_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		क्रम(माप_प्रकार i = 0; i < self.arg_गिनती(); i++) {
			अगर(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		वापस ret;
	}
};

std::string गाँठ_2::to_string()
{
	वापस to_string([](गाँठ_2 n) { वापस n.name().unescape(); });
}

std::string गाँठ_2::to_string(std::function<std::string(गाँठ_2)> np)
{
	वापस visit(PrintVisitor(np));
}

श्रेणी CellSimplifier {
	Factory &factory;
	गाँठ_2 sign(गाँठ_2 a) {
		वापस factory.slice(a, a.width() - 1, 1);
	}
	गाँठ_2 neg_if(गाँठ_2 a, गाँठ_2 s) {
		वापस factory.mux(a, factory.unary_minus(a), s);
	}
	गाँठ_2 असल(गाँठ_2 a) {
		वापस neg_if(a, sign(a));
	}
	गाँठ_2 handle_shift(गाँठ_2 a, गाँठ_2 b, bool is_दायाँ, bool is_signed) {
		// to prevent new_width == 0, we handle this case separately
		अगर(a.width() == 1) {
			अगर(!is_signed)
				वापस factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			अन्यथा
				वापस a;
		}
		पूर्णांक new_width = ceil_log2(a.width());
		गाँठ_2 b_truncated = factory.extend(b, new_width, false);
		गाँठ_2 y =
			!is_दायाँ ? factory.logical_shift_बायाँ(a, b_truncated) :
			!is_signed ? factory.logical_shift_दायाँ(a, b_truncated) :
			factory.arithmetic_shift_दायाँ(a, b_truncated);
		अगर(b.width() <= new_width)
			वापस y;
		गाँठ_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		गाँठ_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), true) : factory.constant(RTLIL::Const(State::S0, a.width()));
		वापस factory.mux(y, y_if_overflow, overflow);
	}
खुला:
	गाँठ_2 logical_shift_बायाँ(गाँठ_2 a, गाँठ_2 b) { वापस handle_shift(a, b, false, false); }
	गाँठ_2 logical_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { वापस handle_shift(a, b, true, false); }
	गाँठ_2 arithmetic_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { वापस handle_shift(a, b, true, true); }
	गाँठ_2 bitwise_mux(गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) {
		गाँठ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		गाँठ_2 bb = factory.bitwise_and(b, s);
		वापस factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
निजी:
	गाँठ_2 handle_pow(गाँठ_2 a0, गाँठ_2 b, पूर्णांक y_width, bool is_signed) {
		गाँठ_2 a = factory.extend(a0, y_width, is_signed);
		गाँठ_2 r = factory.constant(Const(1, y_width));
		क्रम(पूर्णांक i = 0; i < b.width(); i++) {
			गाँठ_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		अगर (is_signed) {
			गाँठ_2 a_ge_1 = factory.unsigned_greater_than(असल(a0), factory.constant(Const(1, a0.width())));
			गाँठ_2 zero_परिणाम = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_परिणाम);
		}
		वापस r;
	}
	गाँठ_2 handle_bmux(गाँठ_2 a, गाँठ_2 s, पूर्णांक a_offset, पूर्णांक width, पूर्णांक sn) {
		अगर(sn < 1)
			वापस factory.slice(a, a_offset, width);
		अन्यथा {
			गाँठ_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			गाँठ_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			वापस factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	गाँठ_2 handle_pmux(गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		गाँठ_2 y = a;
		क्रम(पूर्णांक i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		वापस y;
	}
	dict<IdString, गाँठ_2> handle_fa(गाँठ_2 a, गाँठ_2 b, गाँठ_2 c) {
		गाँठ_2 t1 = factory.bitwise_xor(a, b);
		गाँठ_2 t2 = factory.bitwise_and(a, b);
		गाँठ_2 t3 = factory.bitwise_and(c, t1);
		गाँठ_2 y = factory.bitwise_xor(c, t1);
		गाँठ_2 x = factory.bitwise_or(t2, t3);
		वापस {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, गाँठ_2> handle_alu(गाँठ_2 a_in, गाँठ_2 b_in, पूर्णांक y_width, bool is_signed, गाँठ_2 ci, गाँठ_2 bi) {
		गाँठ_2 a = factory.extend(a_in, y_width, is_signed);
		गाँठ_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		गाँठ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		गाँठ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		गाँठ_2 a_extra = factory.extend(a, y_width + 1, false);
		गाँठ_2 b_extra = factory.extend(b, y_width + 1, false);
		गाँठ_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, false));
		गाँठ_2 y = factory.slice(y_extra, 0, y_width);
		गाँठ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		गाँठ_2 co = factory.slice(carries, 1, y_width);
		वापस {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	गाँठ_2 handle_lcu(गाँठ_2 p, गाँठ_2 g, गाँठ_2 ci) {
		वापस handle_alu(g, factory.bitwise_or(p, g), g.width(), false, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
खुला:
	std::variant<dict<IdString, गाँठ_2>, गाँठ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, गाँठ_2> inputs)
	{
		पूर्णांक a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		पूर्णांक b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		पूर्णांक y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		bool a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		bool b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		अगर(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			bool is_signed = a_signed && b_signed;
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			गाँठ_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			अगर(cellType == ID($add))
				वापस factory.add(a, b);
			अन्यथा अगर(cellType == ID($sub))
				वापस factory.sub(a, b);
			अन्यथा अगर(cellType == ID($mul))
				वापस factory.mul(a, b);
			अन्यथा अगर(cellType == ID($and))
				वापस factory.bitwise_and(a, b);
			अन्यथा अगर(cellType == ID($or))
				वापस factory.bitwise_or(a, b);
			अन्यथा अगर(cellType == ID($xor))
				वापस factory.bitwise_xor(a, b);
			अन्यथा अगर(cellType == ID($xnor))
				वापस factory.bitwise_not(factory.bitwise_xor(a, b));
			अन्यथा
				log_abort();
		}अन्यथा अगर(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			bool is_signed = a_signed && b_signed;
			पूर्णांक width = max(a_width, b_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाँठ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			अगर(cellType.in(ID($eq), ID($eqx)))
				वापस factory.extend(factory.equal(a, b), y_width, false);
			अन्यथा अगर(cellType.in(ID($ne), ID($nex)))
				वापस factory.extend(factory.not_equal(a, b), y_width, false);
			अन्यथा अगर(cellType == ID($lt))
				वापस factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, false);
			अन्यथा अगर(cellType == ID($le))
				वापस factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, false);
			अन्यथा अगर(cellType == ID($gt))
				वापस factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, false);
			अन्यथा अगर(cellType == ID($ge))
				वापस factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, false);
			अन्यथा
				log_abort();
		}अन्यथा अगर(cellType.in(ID($logic_or), ID($logic_and))){
			गाँठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाँठ_2 b = factory.reduce_or(inputs.at(ID(B)));
			गाँठ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			वापस factory.extend(y, y_width, false);
		}अन्यथा अगर(cellType == ID($not)){
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			वापस factory.bitwise_not(a);
		}अन्यथा अगर(cellType == ID($pos)){
			वापस factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}अन्यथा अगर(cellType == ID($barrier)){
			वापस inputs.at(ID(A));
		}अन्यथा अगर(cellType == ID($neg)){
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			वापस factory.unary_minus(a);
		}अन्यथा अगर(cellType == ID($logic_not)){
			गाँठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाँठ_2 y = factory.bitwise_not(a);
			वापस factory.extend(y, y_width, false);
		}अन्यथा अगर(cellType.in(ID($reduce_or), ID($reduce_bool))){
			गाँठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			वापस factory.extend(a, y_width, false);
		}अन्यथा अगर(cellType == ID($reduce_and)){
			गाँठ_2 a = factory.reduce_and(inputs.at(ID(A)));
			वापस factory.extend(a, y_width, false);
		}अन्यथा अगर(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			गाँठ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			गाँठ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			वापस factory.extend(y, y_width, false);
		}अन्यथा अगर(cellType == ID($shl) || cellType == ID($sshl)){
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			गाँठ_2 b = inputs.at(ID(B));
			वापस logical_shift_बायाँ(a, b);
		}अन्यथा अगर(cellType == ID($shr) || cellType == ID($sshr)){
			पूर्णांक width = max(a_width, y_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			गाँठ_2 b = inputs.at(ID(B));
			गाँठ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_दायाँ(a, b) :
				logical_shift_दायाँ(a, b);
			वापस factory.extend(y, y_width, a_signed);
		}अन्यथा अगर(cellType == ID($shiftx) || cellType == ID($shift)){
			पूर्णांक width = max(a_width, y_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			गाँठ_2 b = inputs.at(ID(B));
			गाँठ_2 shr = logical_shift_दायाँ(a, b);
			अगर(b_signed) {
				गाँठ_2 shl = logical_shift_बायाँ(a, factory.unary_minus(b));
				गाँठ_2 y = factory.mux(shr, shl, sign(b));
				वापस factory.extend(y, y_width, false);
			} अन्यथा {
				वापस factory.extend(shr, y_width, false);
			}
		}अन्यथा अगर(cellType == ID($mux)){
			वापस factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा अगर(cellType == ID($pmux)){
			वापस handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा अगर(cellType == ID($concat)){
			गाँठ_2 a = inputs.at(ID(A));
			गाँठ_2 b = inputs.at(ID(B));
			वापस factory.concat(a, b);
		}अन्यथा अगर(cellType == ID($slice)){
			पूर्णांक offset = parameters.at(ID(OFFSET)).as_int();
			गाँठ_2 a = inputs.at(ID(A));
			वापस factory.slice(a, offset, y_width);
		}अन्यथा अगर(cellType.in(ID($भाग), ID($mod), ID($divfloor), ID($modfloor))) {
			पूर्णांक width = max(a_width, b_width);
			bool is_signed = a_signed && b_signed;
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाँठ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			अगर(is_signed) {
				अगर(cellType == ID($भाग)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					गाँठ_2 abs_y = factory.unsigned_div(असल(a), असल(b));
					गाँठ_2 out_sign = factory.not_equal(sign(a), sign(b));
					वापस neg_if(factory.extend(abs_y, y_width, false), out_sign);
				} अन्यथा अगर(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					गाँठ_2 abs_y = factory.unsigned_mod(असल(a), असल(b));
					वापस neg_if(factory.extend(abs_y, y_width, false), sign(a));
				} अन्यथा अगर(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					गाँठ_2 b_sign = sign(b);
					गाँठ_2 a1 = neg_if(a, b_sign);
					गाँठ_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					गाँठ_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					गाँठ_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					गाँठ_2 y1 = factory.unsigned_div(a2, b1);
					गाँठ_2 y2 = factory.extend(y1, y_width, false);
					वापस factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} अन्यथा अगर(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					गाँठ_2 abs_b = असल(b);
					गाँठ_2 abs_y = factory.unsigned_mod(असल(a), abs_b);
					गाँठ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					गाँठ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					गाँठ_2 y = neg_if(y_flipped, sign(b));
					वापस factory.extend(y, y_width, true);
				} अन्यथा
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} अन्यथा {
				अगर(cellType.in(ID($mod), ID($modfloor)))
					वापस factory.extend(factory.unsigned_mod(a, b), y_width, false);
				अन्यथा
					वापस factory.extend(factory.unsigned_div(a, b), y_width, false);
			}
		} अन्यथा अगर(cellType == ID($घात)) {
			वापस handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} अन्यथा अगर (cellType == ID($lut)) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			वापस handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} अन्यथा अगर (cellType == ID($bwmux)) {
			गाँठ_2 a = inputs.at(ID(A));
			गाँठ_2 b = inputs.at(ID(B));
			गाँठ_2 s = inputs.at(ID(S));
			वापस factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} अन्यथा अगर (cellType == ID($bweqx)) {
			गाँठ_2 a = inputs.at(ID(A));
			गाँठ_2 b = inputs.at(ID(B));
			वापस factory.bitwise_not(factory.bitwise_xor(a, b));
		} अन्यथा अगर(cellType == ID($bmux)) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			पूर्णांक s_width = parameters.at(ID(S_WIDTH)).as_int();
			वापस handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} अन्यथा अगर(cellType == ID($demux)) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			पूर्णांक s_width = parameters.at(ID(S_WIDTH)).as_int();
			पूर्णांक y_width = width << s_width;
			पूर्णांक b_width = ceil_log2(y_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, false);
			गाँठ_2 s = factory.extend(inputs.at(ID(S)), b_width, false);
			गाँठ_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			वापस factory.logical_shift_बायाँ(a, b);
		} अन्यथा अगर(cellType == ID($fa)) {
			वापस handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} अन्यथा अगर(cellType == ID($lcu)) {
			वापस handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} अन्यथा अगर(cellType == ID($alu)) {
			वापस handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} अन्यथा अगर(cellType.in(ID($निश्चित), ID($assume), ID($live), ID($fair), ID($cover))) {
			गाँठ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			स्वतः &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_मूल्य(a);
			वापस {};
		} अन्यथा अगर(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			स्वतः &निवेश = factory.add_निवेश(cellName, cellType, Sort(width));
			वापस factory.मूल्य(निवेश);
		} अन्यथा अगर(cellType == ID($initstate)) {
			अगर(factory.ir().has_state(ID($initstate), ID($state)))
				वापस factory.मूल्य(factory.ir().state(ID($initstate)));
			अन्यथा {
				स्वतः &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_मूल्य(RTLIL::Const(State::S1, 1));
				state.set_अग्र_मूल्य(factory.constant(RTLIL::Const(State::S0, 1)));
				वापस factory.मूल्य(state);
			}
		} अन्यथा अगर(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} अन्यथा {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

श्रेणी FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, गाँठ_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, गाँठ_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	गाँठ_2 enqueue(DriveSpec स्थिर &spec)
	{
		स्वतः it = graph_nodes.find(spec);
		अगर(it == graph_nodes.end()){
			स्वतः गाँठ = factory.create_pending(spec.आकार());
			graph_nodes.जोड़ो({spec, गाँठ});
			queue.emplace_back(spec);
			वापस गाँठ;
		}अन्यथा
			वापस it->second;
	}
	गाँठ_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		स्वतः it = cell_outputs.find({cell, port_name});
		अगर(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<गाँठ_2> rv;
			क्रम(स्वतः स्थिर &[name, sigspec] : cell->connections())
				अगर(driver_map.celltypes.cell_output(cell->type, name)) {
					स्वतः गाँठ = factory.create_pending(sigspec.आकार());
					factory.suggest_name(गाँठ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, गाँठ);
					अगर(name == port_name)
						rv = गाँठ;
				}
			वापस *rv;
		} अन्यथा
			वापस it->second;
	}
खुला:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		क्रम (स्वतः cell : module->cells()) {
			अगर (cell->type.in(ID($निश्चित), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		क्रम (स्वतः riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			स्वतः *wire = module->wire(*riter);
			अगर (wire && wire->port_निवेश) {
				factory.add_निवेश(wire->name, ID($निवेश), Sort(wire->width));
			}
			अगर (wire && wire->port_output) {
				स्वतः &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_मूल्य(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		क्रम (स्वतः &mem : memories_vector) {
			अगर (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
निजी:
	गाँठ_2 concatenate_read_results(Mem *mem, vector<गाँठ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		स्थिर SigSpec &rd_तथ्य = mem->cell->connections().at(ID(RD_तथ्य));
		पूर्णांक current = 0;
		क्रम(माप_प्रकार i = 0; i < mem->rd_ports.आकार(); i++) {
			पूर्णांक width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].तथ्य == rd_तथ्य.extract(current, width));
			current += width;
		}
		log_assert (current == rd_तथ्य.आकार());
		log_assert (!results.empty());
		गाँठ_2 गाँठ = results[0];
		क्रम(माप_प्रकार i = 1; i < results.आकार(); i++)
			गाँठ = factory.concat(गाँठ, results[i]);
		वापस गाँठ;
	}
	गाँठ_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<गाँठ_2> read_results;
		स्वतः &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->आकार), mem->width));
		state.set_initial_मूल्य(MemContents(mem));
		गाँठ_2 गाँठ = factory.मूल्य(state);
		क्रम (माप_प्रकार i = 0; i < mem->wr_ports.आकार(); i++) {
			स्थिर स्वतः &wr = mem->wr_ports[i];
			अगर (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाँठ_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			गाँठ_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			गाँठ_2 new_तथ्य = enqueue(driver_map(DriveSpec(wr.तथ्य)));
			गाँठ_2 old_तथ्य = factory.memory_read(गाँठ, addr);
			गाँठ_2 wr_तथ्य = simplifier.bitwise_mux(old_तथ्य, new_तथ्य, en);
			गाँठ = factory.memory_write(गाँठ, addr, wr_तथ्य);
		}
		अगर (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		क्रम (माप_प्रकार i = 0; i < mem->rd_ports.आकार(); i++) {
			स्थिर स्वतः &rd = mem->rd_ports[i];
			अगर (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाँठ_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.धकेलो_back(factory.memory_read(गाँठ, addr));
		}
		state.set_अग्र_मूल्य(गाँठ);
		वापस concatenate_read_results(mem, read_results);
	}
	व्योम process_cell(Cell *cell)
	{
		अगर (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			अगर (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			गाँठ_2 गाँठ = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_तथ्य)}), गाँठ);
		} अन्यथा अगर (cell->is_builtin_ff()) {
			Ff_तथ्य ff(&ff_initvals, cell);
			अगर (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			स्वतः &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			गाँठ_2 q_मूल्य = factory.मूल्य(state);
			factory.suggest_name(q_मूल्य, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_मूल्य);
			state.set_अग्र_मूल्य(enqueue(ff.sig_d));
			state.set_initial_मूल्य(ff.val_init);
		} अन्यथा {
			dict<IdString, गाँठ_2> connections;
			IdString output_name; // for the single output case
			पूर्णांक n_outputs = 0;
			क्रम(स्वतः स्थिर &[name, sigspec] : cell->connections()) {
				अगर(driver_map.celltypes.cell_निवेश(cell->type, name) && sigspec.आकार() > 0)
					connections.जोड़ो({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				अगर(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, गाँठ_2>, गाँठ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			अगर(स्वतः *nodep = std::get_if<गाँठ_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} अन्यथा {
				क्रम(स्वतः [name, गाँठ] : std::लो<dict<IdString, गाँठ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), गाँठ);
			}
		}
	}
	व्योम undriven(स्थिर std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	व्योम check_undriven(DriveSpec स्थिर& spec, std::string स्थिर& name) {
		क्रम(स्वतः स्थिर &chunk : spec.chunks())
			अगर(chunk.is_none())
				undriven(name);
	}
खुला:
	व्योम process_queue()
	{
		क्रम (; !queue.empty(); queue.निकालो_front()) {
			अगर(स्वतः p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				जारी;
			}

			DriveSpec spec = std::लो<DriveSpec>(queue.front());
			गाँठ_2 pending = graph_nodes.at(spec);

			अगर (spec.chunks().आकार() > 1) {
				स्वतः chunks = spec.chunks();
				गाँठ_2 गाँठ = enqueue(chunks[0]);
				क्रम(माप_प्रकार i = 1; i < chunks.आकार(); i++)
					गाँठ = factory.concat(गाँठ, enqueue(chunks[i]));
				factory.update_pending(pending, गाँठ);
			} अन्यथा अगर (spec.chunks().आकार() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				अगर (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					अगर (wire_chunk.is_whole()) {
						अगर (wire_chunk.wire->port_निवेश) {
							गाँठ_2 गाँठ = factory.मूल्य(factory.ir().निवेश(wire_chunk.wire->name));
							factory.suggest_name(गाँठ, wire_chunk.wire->name);
							factory.update_pending(pending, गाँठ);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							गाँठ_2 गाँठ = enqueue(driver);
							factory.suggest_name(गाँठ, wire_chunk.wire->name);
							factory.update_pending(pending, गाँठ);
						}
					} अन्यथा {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						गाँठ_2 गाँठ = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, गाँठ);
					}
				} अन्यथा अगर (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					अगर (port_chunk.is_whole()) {
						अगर (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							गाँठ_2 गाँठ = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, गाँठ);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} अन्यथा {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, Get_आकार(port_chunk.cell->connections().at(port_chunk.port)));
						गाँठ_2 गाँठ = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, गाँठ);
					}
				} अन्यथा अगर (chunk.is_constant()) {
					गाँठ_2 गाँठ = factory.constant(chunk.constant());
					factory.suggest_name(गाँठ, "$const" + std::to_string(chunk.आकार()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, गाँठ);
				} अन्यथा अगर (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} अन्यथा अगर (chunk.is_none()) {
					undriven(log_signal(chunk));
				} अन्यथा {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} अन्यथा {
				log_abort();
			}
		}
	}
};

IR IR::from_module(Module *module) {
	IR ir;
    ir.design = module->design;
    स्वतः factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_queue();
    ir.topological_sort();
    ir.forward_buf();
    वापस ir;
}

व्योम IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    bool scc = false;
    std::vector<पूर्णांक> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](पूर्णांक *begin, पूर्णांक *end) {
        perm.जोड़ो(perm.end(), begin, end);
        अगर (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            क्रम (पूर्णांक *i = begin; i != end; ++i) {
				गाँठ_2 गाँठ(_graph[*i], design);
                log("- %s = %s\n", गाँठ.name().unescape(), गाँठ.to_string());
			}
            log("\n");
            scc = true;
        }
    });
	क्रम(स्थिर स्वतः &[name, state]: _states)
		अगर(state.has_अग्र_मूल्य())
			toposort.process(state.अग्र_मूल्य().id());
	क्रम(स्थिर स्वतः &[name, output]: _outputs)
		अगर(output.has_मूल्य())
			toposort.process(output.मूल्य().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    अगर(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

अटल IdString merge_name(IdString a, IdString b) {
	अगर(!a.isPublic() && b.isPublic())
		वापस b;
	अन्यथा
		वापस a;
}

व्योम IR::forward_buf() {
    std::vector<पूर्णांक> perm, alias;
    perm.सफा();

    क्रम (पूर्णांक i = 0; i < _graph.आकार(); ++i)
    {
        स्वतः गाँठ = _graph[i];
        अगर (गाँठ.function().fn() == Fn::buf && गाँठ.arg(0).सूचकांक() < i)
        {
            पूर्णांक target_सूचकांक = alias[गाँठ.arg(0).सूचकांक()];
            स्वतः target_गाँठ = _graph[perm[target_सूचकांक]];
			अगर(गाँठ.has_sparse_attr()) {
				अगर(target_गाँठ.has_sparse_attr()) {
					IdString id = merge_name(गाँठ.sparse_attr(), target_गाँठ.sparse_attr());
					target_गाँठ.sparse_attr() = id;
				} अन्यथा {
					IdString id = गाँठ.sparse_attr();
					target_गाँठ.sparse_attr() = id;
				}
			}
            alias.धकेलो_back(target_सूचकांक);
        }
        अन्यथा
        {
            alias.धकेलो_back(Get_आकार(perm));
            perm.धकेलो_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
अटल std::string quote_fmt(स्थिर अक्षर *fmt)
{
	std::string r;
	क्रम(स्थिर अक्षर *p = fmt; *p != 0; p++) {
		चयन(*p) {
		हाल '\n': r += "\\n"; अवरोध;
		हाल '\t': r += "\\t"; अवरोध;
		हाल '"': r += "\\\""; अवरोध;
		हाल '\\': r += "\\\\"; अवरोध;
		शेष: r += *p; अवरोध;
		}
	}
	वापस r;
}

व्योम Writer::print_impl(स्थिर अक्षर *fmt, vector<std::function<व्योम()>> &fns)
{
	माप_प्रकार अग्र_सूचकांक = 0;
	क्रम(स्थिर अक्षर *p = fmt; *p != 0; p++)
		चयन(*p) {
		हाल '{':
			अगर(*++p == '{') {
				*os << '{';
			} अन्यथा {
				अक्षर *pe;
				माप_प्रकार सूचकांक = म_से_अदीर्घ(p, &pe, 10);
				अगर(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				अगर(p == pe)
					सूचकांक = अग्र_सूचकांक;
				अन्यथा
					p = pe;
				अगर(सूचकांक >= fns.आकार())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", सूचकांक, fns.आकार(), quote_fmt(fmt));
				fns[सूचकांक]();
				अग्र_सूचकांक = सूचकांक + 1;
			}
			अवरोध;
		हाल '}':
			p++;
			अगर(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			अवरोध;
		शेष:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
