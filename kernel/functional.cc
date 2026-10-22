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

#अन्तर्भाव "kernel/functional.h"
#अन्तर्भाव "kernel/topo_scc.h"
#अन्तर्भाव "ff.h"
#अन्तर्भाव "ffinit.h"
#अन्तर्भाव <deque>

YOSYS_NAMESPACE_BEGIN
नामाकाश Functional {

स्थिरांक अक्षर *fn_to_string(Fn fn) {
	निवड(fn) {
	स्थिती Fn::invalid: परत "invalid";
	स्थिती Fn::buf: परत "buf";
	स्थिती Fn::slice: परत "slice";
	स्थिती Fn::zero_extend: परत "zero_extend";
	स्थिती Fn::sign_extend: परत "sign_extend";
	स्थिती Fn::concat: परत "concat";
	स्थिती Fn::add: परत "add";
	स्थिती Fn::sub: परत "sub";
	स्थिती Fn::mul: परत "mul";
	स्थिती Fn::unsigned_div: परत "unsigned_div";
	स्थिती Fn::unsigned_mod: परत "unsigned_mod";
	स्थिती Fn::bitwise_and: परत "bitwise_and";
	स्थिती Fn::bitwise_or: परत "bitwise_or";
	स्थिती Fn::bitwise_xor: परत "bitwise_xor";
	स्थिती Fn::bitwise_not: परत "bitwise_not";
	स्थिती Fn::reduce_and: परत "reduce_and";
	स्थिती Fn::reduce_or: परत "reduce_or";
	स्थिती Fn::reduce_xor: परत "reduce_xor";
	स्थिती Fn::unary_minus: परत "unary_minus";
	स्थिती Fn::equal: परत "equal";
	स्थिती Fn::not_equal: परत "not_equal";
	स्थिती Fn::signed_greater_than: परत "signed_greater_than";
	स्थिती Fn::signed_greater_equal: परत "signed_greater_equal";
	स्थिती Fn::unsigned_greater_than: परत "unsigned_greater_than";
	स्थिती Fn::unsigned_greater_equal: परत "unsigned_greater_equal";
	स्थिती Fn::logical_shift_डावा: परत "logical_shift_left";
	स्थिती Fn::logical_shift_उजवा: परत "logical_shift_right";
	स्थिती Fn::arithmetic_shift_उजवा: परत "arithmetic_shift_right";
	स्थिती Fn::mux: परत "mux";
	स्थिती Fn::constant: परत "constant";
	स्थिती Fn::आदान: परत "input";
	स्थिती Fn::state: परत "state";
	स्थिती Fn::memory_read: परत "memory_read";
	स्थिती Fn::memory_write: परत "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (पूर्णांक)fn);
}

vector<IRInput स्थिरांक*> IR::inputs(IdString kind) स्थिरांक {
	vector<IRInput स्थिरांक*> ret;
	साठी (स्थिरांक स्वतः &[name, आदान] : _inputs)
		जर(आदान.kind == kind)
			ret.ढकला_back(&आदान);
	परत ret;
}

vector<IROutput स्थिरांक*> IR::outputs(IdString kind) स्थिरांक {
	vector<IROutput स्थिरांक*> ret;
	साठी (स्थिरांक स्वतः &[name, output] : _outputs)
		जर(output.kind == kind)
			ret.ढकला_back(&output);
	परत ret;
}

vector<IRState स्थिरांक*> IR::states(IdString kind) स्थिरांक {
	vector<IRState स्थिरांक*> ret;
	साठी (स्थिरांक स्वतः &[name, state] : _states)
		जर(state.kind == kind)
			ret.ढकला_back(&state);
	परत ret;
}

vector<IRInput स्थिरांक*> IR::all_inputs() स्थिरांक {
	vector<IRInput स्थिरांक*> ret;
	साठी (स्थिरांक स्वतः &[name, आदान] : _inputs)
		ret.ढकला_back(&आदान);
	परत ret;
}

vector<IROutput स्थिरांक*> IR::all_outputs() स्थिरांक {
	vector<IROutput स्थिरांक*> ret;
	साठी (स्थिरांक स्वतः &[name, output] : _outputs)
		ret.ढकला_back(&output);
	परत ret;
}

vector<IRState स्थिरांक*> IR::all_states() स्थिरांक {
	vector<IRState स्थिरांक*> ret;
	साठी (स्थिरांक स्वतः &[name, state] : _states)
		ret.ढकला_back(&state);
	परत ret;
}

संरचना PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(गाठ_2)> np;
	PrintVisitor(std::function<std::string(गाठ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(गाठ_2, गाठ_2 a, पूर्णांक offset, पूर्णांक out_width) override { परत "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(गाठ_2, गाठ_2 a, पूर्णांक out_width) override { परत "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(गाठ_2, गाठ_2 a, पूर्णांक out_width) override { परत "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(गाठ_2, RTLIL::Const स्थिरांक& मूल्य) override { परत "constant(" + मूल्य.as_string() + ")"; }
	std::string आदान(गाठ_2 self, IdString name, IdString kind) override { परत "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(गाठ_2 self, IdString name, IdString kind) override { परत "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(गाठ_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		साठी(माप_प्रकार i = 0; i < self.arg_मोजणी(); i++) {
			जर(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		परत ret;
	}
};

std::string गाठ_2::to_string()
{
	परत to_string([](गाठ_2 n) { परत n.name().unescape(); });
}

std::string गाठ_2::to_string(std::function<std::string(गाठ_2)> np)
{
	परत visit(PrintVisitor(np));
}

वर्ग CellSimplifier {
	Factory &factory;
	गाठ_2 sign(गाठ_2 a) {
		परत factory.slice(a, a.width() - 1, 1);
	}
	गाठ_2 neg_if(गाठ_2 a, गाठ_2 s) {
		परत factory.mux(a, factory.unary_minus(a), s);
	}
	गाठ_2 असल(गाठ_2 a) {
		परत neg_if(a, sign(a));
	}
	गाठ_2 handle_shift(गाठ_2 a, गाठ_2 b, तार्किक is_उजवा, तार्किक is_signed) {
		// to prevent new_width == 0, we handle this case separately
		जर(a.width() == 1) {
			जर(!is_signed)
				परत factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			अन्यथा
				परत a;
		}
		पूर्णांक new_width = ceil_log2(a.width());
		गाठ_2 b_truncated = factory.extend(b, new_width, असत्य);
		गाठ_2 y =
			!is_उजवा ? factory.logical_shift_डावा(a, b_truncated) :
			!is_signed ? factory.logical_shift_उजवा(a, b_truncated) :
			factory.arithmetic_shift_उजवा(a, b_truncated);
		जर(b.width() <= new_width)
			परत y;
		गाठ_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		गाठ_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), सत्य) : factory.constant(RTLIL::Const(State::S0, a.width()));
		परत factory.mux(y, y_if_overflow, overflow);
	}
सार्वजनिक:
	गाठ_2 logical_shift_डावा(गाठ_2 a, गाठ_2 b) { परत handle_shift(a, b, असत्य, असत्य); }
	गाठ_2 logical_shift_उजवा(गाठ_2 a, गाठ_2 b) { परत handle_shift(a, b, सत्य, असत्य); }
	गाठ_2 arithmetic_shift_उजवा(गाठ_2 a, गाठ_2 b) { परत handle_shift(a, b, सत्य, सत्य); }
	गाठ_2 bitwise_mux(गाठ_2 a, गाठ_2 b, गाठ_2 s) {
		गाठ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		गाठ_2 bb = factory.bitwise_and(b, s);
		परत factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
खाजगी:
	गाठ_2 handle_pow(गाठ_2 a0, गाठ_2 b, पूर्णांक y_width, तार्किक is_signed) {
		गाठ_2 a = factory.extend(a0, y_width, is_signed);
		गाठ_2 r = factory.constant(Const(1, y_width));
		साठी(पूर्णांक i = 0; i < b.width(); i++) {
			गाठ_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		जर (is_signed) {
			गाठ_2 a_ge_1 = factory.unsigned_greater_than(असल(a0), factory.constant(Const(1, a0.width())));
			गाठ_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		परत r;
	}
	गाठ_2 handle_bmux(गाठ_2 a, गाठ_2 s, पूर्णांक a_offset, पूर्णांक width, पूर्णांक sn) {
		जर(sn < 1)
			परत factory.slice(a, a_offset, width);
		अन्यथा {
			गाठ_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			गाठ_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			परत factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	गाठ_2 handle_pmux(गाठ_2 a, गाठ_2 b, गाठ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		गाठ_2 y = a;
		साठी(पूर्णांक i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		परत y;
	}
	dict<IdString, गाठ_2> handle_fa(गाठ_2 a, गाठ_2 b, गाठ_2 c) {
		गाठ_2 t1 = factory.bitwise_xor(a, b);
		गाठ_2 t2 = factory.bitwise_and(a, b);
		गाठ_2 t3 = factory.bitwise_and(c, t1);
		गाठ_2 y = factory.bitwise_xor(c, t1);
		गाठ_2 x = factory.bitwise_or(t2, t3);
		परत {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, गाठ_2> handle_alu(गाठ_2 a_in, गाठ_2 b_in, पूर्णांक y_width, तार्किक is_signed, गाठ_2 ci, गाठ_2 bi) {
		गाठ_2 a = factory.extend(a_in, y_width, is_signed);
		गाठ_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		गाठ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		गाठ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		गाठ_2 a_extra = factory.extend(a, y_width + 1, असत्य);
		गाठ_2 b_extra = factory.extend(b, y_width + 1, असत्य);
		गाठ_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, असत्य));
		गाठ_2 y = factory.slice(y_extra, 0, y_width);
		गाठ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		गाठ_2 co = factory.slice(carries, 1, y_width);
		परत {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	गाठ_2 handle_lcu(गाठ_2 p, गाठ_2 g, गाठ_2 ci) {
		परत handle_alu(g, factory.bitwise_or(p, g), g.width(), असत्य, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
सार्वजनिक:
	std::variant<dict<IdString, गाठ_2>, गाठ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, गाठ_2> inputs)
	{
		पूर्णांक a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		पूर्णांक b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		पूर्णांक y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		तार्किक a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		तार्किक b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		जर(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			तार्किक is_signed = a_signed && b_signed;
			गाठ_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			गाठ_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			जर(cellType == ID($add))
				परत factory.add(a, b);
			अन्यथा जर(cellType == ID($sub))
				परत factory.sub(a, b);
			अन्यथा जर(cellType == ID($mul))
				परत factory.mul(a, b);
			अन्यथा जर(cellType == ID($and))
				परत factory.bitwise_and(a, b);
			अन्यथा जर(cellType == ID($or))
				परत factory.bitwise_or(a, b);
			अन्यथा जर(cellType == ID($xor))
				परत factory.bitwise_xor(a, b);
			अन्यथा जर(cellType == ID($xnor))
				परत factory.bitwise_not(factory.bitwise_xor(a, b));
			अन्यथा
				log_abort();
		}अन्यथा जर(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			तार्किक is_signed = a_signed && b_signed;
			पूर्णांक width = max(a_width, b_width);
			गाठ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाठ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			जर(cellType.in(ID($eq), ID($eqx)))
				परत factory.extend(factory.equal(a, b), y_width, असत्य);
			अन्यथा जर(cellType.in(ID($ne), ID($nex)))
				परत factory.extend(factory.not_equal(a, b), y_width, असत्य);
			अन्यथा जर(cellType == ID($lt))
				परत factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, असत्य);
			अन्यथा जर(cellType == ID($le))
				परत factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, असत्य);
			अन्यथा जर(cellType == ID($gt))
				परत factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, असत्य);
			अन्यथा जर(cellType == ID($ge))
				परत factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, असत्य);
			अन्यथा
				log_abort();
		}अन्यथा जर(cellType.in(ID($logic_or), ID($logic_and))){
			गाठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाठ_2 b = factory.reduce_or(inputs.at(ID(B)));
			गाठ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			परत factory.extend(y, y_width, असत्य);
		}अन्यथा जर(cellType == ID($not)){
			गाठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			परत factory.bitwise_not(a);
		}अन्यथा जर(cellType == ID($pos)){
			परत factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}अन्यथा जर(cellType == ID($barrier)){
			परत inputs.at(ID(A));
		}अन्यथा जर(cellType == ID($neg)){
			गाठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			परत factory.unary_minus(a);
		}अन्यथा जर(cellType == ID($logic_not)){
			गाठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाठ_2 y = factory.bitwise_not(a);
			परत factory.extend(y, y_width, असत्य);
		}अन्यथा जर(cellType.in(ID($reduce_or), ID($reduce_bool))){
			गाठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			परत factory.extend(a, y_width, असत्य);
		}अन्यथा जर(cellType == ID($reduce_and)){
			गाठ_2 a = factory.reduce_and(inputs.at(ID(A)));
			परत factory.extend(a, y_width, असत्य);
		}अन्यथा जर(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			गाठ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			गाठ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			परत factory.extend(y, y_width, असत्य);
		}अन्यथा जर(cellType == ID($shl) || cellType == ID($sshl)){
			गाठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			गाठ_2 b = inputs.at(ID(B));
			परत logical_shift_डावा(a, b);
		}अन्यथा जर(cellType == ID($shr) || cellType == ID($sshr)){
			पूर्णांक width = max(a_width, y_width);
			गाठ_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			गाठ_2 b = inputs.at(ID(B));
			गाठ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_उजवा(a, b) :
				logical_shift_उजवा(a, b);
			परत factory.extend(y, y_width, a_signed);
		}अन्यथा जर(cellType == ID($shiftx) || cellType == ID($shift)){
			पूर्णांक width = max(a_width, y_width);
			गाठ_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			गाठ_2 b = inputs.at(ID(B));
			गाठ_2 shr = logical_shift_उजवा(a, b);
			जर(b_signed) {
				गाठ_2 shl = logical_shift_डावा(a, factory.unary_minus(b));
				गाठ_2 y = factory.mux(shr, shl, sign(b));
				परत factory.extend(y, y_width, असत्य);
			} अन्यथा {
				परत factory.extend(shr, y_width, असत्य);
			}
		}अन्यथा जर(cellType == ID($mux)){
			परत factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा जर(cellType == ID($pmux)){
			परत handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा जर(cellType == ID($concat)){
			गाठ_2 a = inputs.at(ID(A));
			गाठ_2 b = inputs.at(ID(B));
			परत factory.concat(a, b);
		}अन्यथा जर(cellType == ID($slice)){
			पूर्णांक offset = parameters.at(ID(OFFSET)).as_int();
			गाठ_2 a = inputs.at(ID(A));
			परत factory.slice(a, offset, y_width);
		}अन्यथा जर(cellType.in(ID($भाग), ID($mod), ID($divfloor), ID($modfloor))) {
			पूर्णांक width = max(a_width, b_width);
			तार्किक is_signed = a_signed && b_signed;
			गाठ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाठ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			जर(is_signed) {
				जर(cellType == ID($भाग)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					गाठ_2 abs_y = factory.unsigned_div(असल(a), असल(b));
					गाठ_2 out_sign = factory.not_equal(sign(a), sign(b));
					परत neg_if(factory.extend(abs_y, y_width, असत्य), out_sign);
				} अन्यथा जर(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					गाठ_2 abs_y = factory.unsigned_mod(असल(a), असल(b));
					परत neg_if(factory.extend(abs_y, y_width, असत्य), sign(a));
				} अन्यथा जर(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					गाठ_2 b_sign = sign(b);
					गाठ_2 a1 = neg_if(a, b_sign);
					गाठ_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					गाठ_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					गाठ_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					गाठ_2 y1 = factory.unsigned_div(a2, b1);
					गाठ_2 y2 = factory.extend(y1, y_width, असत्य);
					परत factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} अन्यथा जर(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					गाठ_2 abs_b = असल(b);
					गाठ_2 abs_y = factory.unsigned_mod(असल(a), abs_b);
					गाठ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					गाठ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					गाठ_2 y = neg_if(y_flipped, sign(b));
					परत factory.extend(y, y_width, सत्य);
				} अन्यथा
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} अन्यथा {
				जर(cellType.in(ID($mod), ID($modfloor)))
					परत factory.extend(factory.unsigned_mod(a, b), y_width, असत्य);
				अन्यथा
					परत factory.extend(factory.unsigned_div(a, b), y_width, असत्य);
			}
		} अन्यथा जर(cellType == ID($घात)) {
			परत handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} अन्यथा जर (cellType == ID($lut)) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			परत handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} अन्यथा जर (cellType == ID($bwmux)) {
			गाठ_2 a = inputs.at(ID(A));
			गाठ_2 b = inputs.at(ID(B));
			गाठ_2 s = inputs.at(ID(S));
			परत factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} अन्यथा जर (cellType == ID($bweqx)) {
			गाठ_2 a = inputs.at(ID(A));
			गाठ_2 b = inputs.at(ID(B));
			परत factory.bitwise_not(factory.bitwise_xor(a, b));
		} अन्यथा जर(cellType == ID($bmux)) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			पूर्णांक s_width = parameters.at(ID(S_WIDTH)).as_int();
			परत handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} अन्यथा जर(cellType == ID($demux)) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			पूर्णांक s_width = parameters.at(ID(S_WIDTH)).as_int();
			पूर्णांक y_width = width << s_width;
			पूर्णांक b_width = ceil_log2(y_width);
			गाठ_2 a = factory.extend(inputs.at(ID(A)), y_width, असत्य);
			गाठ_2 s = factory.extend(inputs.at(ID(S)), b_width, असत्य);
			गाठ_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			परत factory.logical_shift_डावा(a, b);
		} अन्यथा जर(cellType == ID($fa)) {
			परत handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} अन्यथा जर(cellType == ID($lcu)) {
			परत handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} अन्यथा जर(cellType == ID($alu)) {
			परत handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} अन्यथा जर(cellType.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover))) {
			गाठ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			स्वतः &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_मूल्य(a);
			परत {};
		} अन्यथा जर(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			पूर्णांक width = parameters.at(ID(WIDTH)).as_int();
			स्वतः &आदान = factory.add_आदान(cellName, cellType, Sort(width));
			परत factory.मूल्य(आदान);
		} अन्यथा जर(cellType == ID($initstate)) {
			जर(factory.ir().has_state(ID($initstate), ID($state)))
				परत factory.मूल्य(factory.ir().state(ID($initstate)));
			अन्यथा {
				स्वतः &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_मूल्य(RTLIL::Const(State::S1, 1));
				state.set_next_मूल्य(factory.constant(RTLIL::Const(State::S0, 1)));
				परत factory.मूल्य(state);
			}
		} अन्यथा जर(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} अन्यथा {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

वर्ग FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, गाठ_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, गाठ_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	गाठ_2 enqueue(DriveSpec स्थिरांक &spec)
	{
		स्वतः it = graph_nodes.find(spec);
		जर(it == graph_nodes.end()){
			स्वतः गाठ = factory.create_pending(spec.size());
			graph_nodes.घाला({spec, गाठ});
			queue.emplace_back(spec);
			परत गाठ;
		}अन्यथा
			परत it->second;
	}
	गाठ_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		स्वतः it = cell_outputs.find({cell, port_name});
		जर(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<गाठ_2> rv;
			साठी(स्वतः स्थिरांक &[name, sigspec] : cell->connections())
				जर(driver_map.celltypes.cell_output(cell->type, name)) {
					स्वतः गाठ = factory.create_pending(sigspec.size());
					factory.suggest_name(गाठ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, गाठ);
					जर(name == port_name)
						rv = गाठ;
				}
			परत *rv;
		} अन्यथा
			परत it->second;
	}
सार्वजनिक:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		साठी (स्वतः cell : module->cells()) {
			जर (cell->type.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		साठी (स्वतः riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			स्वतः *wire = module->wire(*riter);
			जर (wire && wire->port_आदान) {
				factory.add_आदान(wire->name, ID($आदान), Sort(wire->width));
			}
			जर (wire && wire->port_output) {
				स्वतः &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_मूल्य(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		साठी (स्वतः &mem : memories_vector) {
			जर (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
खाजगी:
	गाठ_2 concatenate_read_results(Mem *mem, vector<गाठ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		स्थिरांक SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		पूर्णांक current = 0;
		साठी(माप_प्रकार i = 0; i < mem->rd_ports.size(); i++) {
			पूर्णांक width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		गाठ_2 गाठ = results[0];
		साठी(माप_प्रकार i = 1; i < results.size(); i++)
			गाठ = factory.concat(गाठ, results[i]);
		परत गाठ;
	}
	गाठ_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<गाठ_2> read_results;
		स्वतः &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_मूल्य(MemContents(mem));
		गाठ_2 गाठ = factory.मूल्य(state);
		साठी (माप_प्रकार i = 0; i < mem->wr_ports.size(); i++) {
			स्थिरांक स्वतः &wr = mem->wr_ports[i];
			जर (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाठ_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			गाठ_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			गाठ_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			गाठ_2 old_data = factory.memory_read(गाठ, addr);
			गाठ_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			गाठ = factory.memory_write(गाठ, addr, wr_data);
		}
		जर (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		साठी (माप_प्रकार i = 0; i < mem->rd_ports.size(); i++) {
			स्थिरांक स्वतः &rd = mem->rd_ports[i];
			जर (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाठ_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.ढकला_back(factory.memory_read(गाठ, addr));
		}
		state.set_next_मूल्य(गाठ);
		परत concatenate_read_results(mem, read_results);
	}
	शून्य process_cell(Cell *cell)
	{
		जर (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			जर (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			गाठ_2 गाठ = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), गाठ);
		} अन्यथा जर (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			जर (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			स्वतः &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			गाठ_2 q_मूल्य = factory.मूल्य(state);
			factory.suggest_name(q_मूल्य, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_मूल्य);
			state.set_next_मूल्य(enqueue(ff.sig_d));
			state.set_initial_मूल्य(ff.val_init);
		} अन्यथा {
			dict<IdString, गाठ_2> connections;
			IdString output_name; // for the single output case
			पूर्णांक n_outputs = 0;
			साठी(स्वतः स्थिरांक &[name, sigspec] : cell->connections()) {
				जर(driver_map.celltypes.cell_आदान(cell->type, name) && sigspec.size() > 0)
					connections.घाला({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				जर(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, गाठ_2>, गाठ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			जर(स्वतः *nodep = std::get_if<गाठ_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} अन्यथा {
				साठी(स्वतः [name, गाठ] : std::get<dict<IdString, गाठ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), गाठ);
			}
		}
	}
	शून्य undriven(स्थिरांक std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	शून्य check_undriven(DriveSpec स्थिरांक& spec, std::string स्थिरांक& name) {
		साठी(स्वतः स्थिरांक &chunk : spec.chunks())
			जर(chunk.is_none())
				undriven(name);
	}
सार्वजनिक:
	शून्य process_queue()
	{
		साठी (; !queue.empty(); queue.काढा_front()) {
			जर(स्वतः p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				सुरूठेवा;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			गाठ_2 pending = graph_nodes.at(spec);

			जर (spec.chunks().size() > 1) {
				स्वतः chunks = spec.chunks();
				गाठ_2 गाठ = enqueue(chunks[0]);
				साठी(माप_प्रकार i = 1; i < chunks.size(); i++)
					गाठ = factory.concat(गाठ, enqueue(chunks[i]));
				factory.update_pending(pending, गाठ);
			} अन्यथा जर (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				जर (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					जर (wire_chunk.is_whole()) {
						जर (wire_chunk.wire->port_आदान) {
							गाठ_2 गाठ = factory.मूल्य(factory.ir().आदान(wire_chunk.wire->name));
							factory.suggest_name(गाठ, wire_chunk.wire->name);
							factory.update_pending(pending, गाठ);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							गाठ_2 गाठ = enqueue(driver);
							factory.suggest_name(गाठ, wire_chunk.wire->name);
							factory.update_pending(pending, गाठ);
						}
					} अन्यथा {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						गाठ_2 गाठ = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, गाठ);
					}
				} अन्यथा जर (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					जर (port_chunk.is_whole()) {
						जर (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							गाठ_2 गाठ = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, गाठ);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} अन्यथा {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						गाठ_2 गाठ = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, गाठ);
					}
				} अन्यथा जर (chunk.is_constant()) {
					गाठ_2 गाठ = factory.constant(chunk.constant());
					factory.suggest_name(गाठ, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, गाठ);
				} अन्यथा जर (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} अन्यथा जर (chunk.is_none()) {
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
    परत ir;
}

शून्य IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    तार्किक scc = असत्य;
    std::vector<पूर्णांक> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](पूर्णांक *begin, पूर्णांक *end) {
        perm.घाला(perm.end(), begin, end);
        जर (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            साठी (पूर्णांक *i = begin; i != end; ++i) {
				गाठ_2 गाठ(_graph[*i], design);
                log("- %s = %s\n", गाठ.name().unescape(), गाठ.to_string());
			}
            log("\n");
            scc = सत्य;
        }
    });
	साठी(स्थिरांक स्वतः &[name, state]: _states)
		जर(state.has_next_मूल्य())
			toposort.process(state.next_मूल्य().id());
	साठी(स्थिरांक स्वतः &[name, output]: _outputs)
		जर(output.has_मूल्य())
			toposort.process(output.मूल्य().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    जर(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

स्थिर IdString merge_name(IdString a, IdString b) {
	जर(!a.isPublic() && b.isPublic())
		परत b;
	अन्यथा
		परत a;
}

शून्य IR::forward_buf() {
    std::vector<पूर्णांक> perm, alias;
    perm.clear();

    साठी (पूर्णांक i = 0; i < _graph.size(); ++i)
    {
        स्वतः गाठ = _graph[i];
        जर (गाठ.function().fn() == Fn::buf && गाठ.arg(0).index() < i)
        {
            पूर्णांक target_index = alias[गाठ.arg(0).index()];
            स्वतः target_गाठ = _graph[perm[target_index]];
			जर(गाठ.has_sparse_attr()) {
				जर(target_गाठ.has_sparse_attr()) {
					IdString id = merge_name(गाठ.sparse_attr(), target_गाठ.sparse_attr());
					target_गाठ.sparse_attr() = id;
				} अन्यथा {
					IdString id = गाठ.sparse_attr();
					target_गाठ.sparse_attr() = id;
				}
			}
            alias.ढकला_back(target_index);
        }
        अन्यथा
        {
            alias.ढकला_back(GetSize(perm));
            perm.ढकला_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
स्थिर std::string quote_fmt(स्थिरांक अक्षर *fmt)
{
	std::string r;
	साठी(स्थिरांक अक्षर *p = fmt; *p != 0; p++) {
		निवड(*p) {
		स्थिती '\n': r += "\\n"; थांबा;
		स्थिती '\t': r += "\\t"; थांबा;
		स्थिती '"': r += "\\\""; थांबा;
		स्थिती '\\': r += "\\\\"; थांबा;
		पूर्वनिर्धारित: r += *p; थांबा;
		}
	}
	परत r;
}

शून्य Writer::print_impl(स्थिरांक अक्षर *fmt, vector<std::function<शून्य()>> &fns)
{
	माप_प्रकार next_index = 0;
	साठी(स्थिरांक अक्षर *p = fmt; *p != 0; p++)
		निवड(*p) {
		स्थिती '{':
			जर(*++p == '{') {
				*os << '{';
			} अन्यथा {
				अक्षर *pe;
				माप_प्रकार index = म_से_अदीर्घ(p, &pe, 10);
				जर(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				जर(p == pe)
					index = next_index;
				अन्यथा
					p = pe;
				जर(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			थांबा;
		स्थिती '}':
			p++;
			जर(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			थांबा;
		पूर्वनिर्धारित:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
