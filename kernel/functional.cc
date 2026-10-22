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

अचर वर्ण *fn_to_string(Fn fn) {
	विकल्प(fn) {
	पक्ष Fn::invalid: फर्काऊ "invalid";
	पक्ष Fn::buf: फर्काऊ "buf";
	पक्ष Fn::slice: फर्काऊ "slice";
	पक्ष Fn::zero_extend: फर्काऊ "zero_extend";
	पक्ष Fn::sign_extend: फर्काऊ "sign_extend";
	पक्ष Fn::concat: फर्काऊ "concat";
	पक्ष Fn::add: फर्काऊ "add";
	पक्ष Fn::sub: फर्काऊ "sub";
	पक्ष Fn::mul: फर्काऊ "mul";
	पक्ष Fn::unsigned_div: फर्काऊ "unsigned_div";
	पक्ष Fn::unsigned_mod: फर्काऊ "unsigned_mod";
	पक्ष Fn::bitwise_and: फर्काऊ "bitwise_and";
	पक्ष Fn::bitwise_or: फर्काऊ "bitwise_or";
	पक्ष Fn::bitwise_xor: फर्काऊ "bitwise_xor";
	पक्ष Fn::bitwise_not: फर्काऊ "bitwise_not";
	पक्ष Fn::reduce_and: फर्काऊ "reduce_and";
	पक्ष Fn::reduce_or: फर्काऊ "reduce_or";
	पक्ष Fn::reduce_xor: फर्काऊ "reduce_xor";
	पक्ष Fn::unary_minus: फर्काऊ "unary_minus";
	पक्ष Fn::equal: फर्काऊ "equal";
	पक्ष Fn::not_equal: फर्काऊ "not_equal";
	पक्ष Fn::signed_greater_than: फर्काऊ "signed_greater_than";
	पक्ष Fn::signed_greater_equal: फर्काऊ "signed_greater_equal";
	पक्ष Fn::unsigned_greater_than: फर्काऊ "unsigned_greater_than";
	पक्ष Fn::unsigned_greater_equal: फर्काऊ "unsigned_greater_equal";
	पक्ष Fn::logical_shift_बायाँ: फर्काऊ "logical_shift_left";
	पक्ष Fn::logical_shift_दायाँ: फर्काऊ "logical_shift_right";
	पक्ष Fn::arithmetic_shift_दायाँ: फर्काऊ "arithmetic_shift_right";
	पक्ष Fn::mux: फर्काऊ "mux";
	पक्ष Fn::constant: फर्काऊ "constant";
	पक्ष Fn::आगत: फर्काऊ "input";
	पक्ष Fn::state: फर्काऊ "state";
	पक्ष Fn::memory_read: फर्काऊ "memory_read";
	पक्ष Fn::memory_write: फर्काऊ "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (पूर्णाङ्क)fn);
}

vector<IRInput अचर*> IR::inputs(IdString kind) अचर {
	vector<IRInput अचर*> ret;
	लागि (अचर स्वतः &[name, आगत] : _inputs)
		यदि(आगत.kind == kind)
			ret.थप_back(&आगत);
	फर्काऊ ret;
}

vector<IROutput अचर*> IR::outputs(IdString kind) अचर {
	vector<IROutput अचर*> ret;
	लागि (अचर स्वतः &[name, output] : _outputs)
		यदि(output.kind == kind)
			ret.थप_back(&output);
	फर्काऊ ret;
}

vector<IRState अचर*> IR::states(IdString kind) अचर {
	vector<IRState अचर*> ret;
	लागि (अचर स्वतः &[name, state] : _states)
		यदि(state.kind == kind)
			ret.थप_back(&state);
	फर्काऊ ret;
}

vector<IRInput अचर*> IR::all_inputs() अचर {
	vector<IRInput अचर*> ret;
	लागि (अचर स्वतः &[name, आगत] : _inputs)
		ret.थप_back(&आगत);
	फर्काऊ ret;
}

vector<IROutput अचर*> IR::all_outputs() अचर {
	vector<IROutput अचर*> ret;
	लागि (अचर स्वतः &[name, output] : _outputs)
		ret.थप_back(&output);
	फर्काऊ ret;
}

vector<IRState अचर*> IR::all_states() अचर {
	vector<IRState अचर*> ret;
	लागि (अचर स्वतः &[name, state] : _states)
		ret.थप_back(&state);
	फर्काऊ ret;
}

संरचना PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(गाँठो_2)> np;
	PrintVisitor(std::function<std::string(गाँठो_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(गाँठो_2, गाँठो_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) override { फर्काऊ "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(गाँठो_2, गाँठो_2 a, पूर्णाङ्क out_width) override { फर्काऊ "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(गाँठो_2, गाँठो_2 a, पूर्णाङ्क out_width) override { फर्काऊ "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(गाँठो_2, RTLIL::Const अचर& मान) override { फर्काऊ "constant(" + मान.as_string() + ")"; }
	std::string आगत(गाँठो_2 self, IdString name, IdString kind) override { फर्काऊ "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(गाँठो_2 self, IdString name, IdString kind) override { फर्काऊ "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(गाँठो_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		लागि(माप_प्रकार i = 0; i < self.arg_गन्ती(); i++) {
			यदि(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		फर्काऊ ret;
	}
};

std::string गाँठो_2::to_string()
{
	फर्काऊ to_string([](गाँठो_2 n) { फर्काऊ n.name().unescape(); });
}

std::string गाँठो_2::to_string(std::function<std::string(गाँठो_2)> np)
{
	फर्काऊ visit(PrintVisitor(np));
}

वर्ग CellSimplifier {
	Factory &factory;
	गाँठो_2 sign(गाँठो_2 a) {
		फर्काऊ factory.slice(a, a.width() - 1, 1);
	}
	गाँठो_2 neg_if(गाँठो_2 a, गाँठो_2 s) {
		फर्काऊ factory.mux(a, factory.unary_minus(a), s);
	}
	गाँठो_2 असल(गाँठो_2 a) {
		फर्काऊ neg_if(a, sign(a));
	}
	गाँठो_2 handle_shift(गाँठो_2 a, गाँठो_2 b, तार्किक is_दायाँ, तार्किक is_signed) {
		// to prevent new_width == 0, we handle this case separately
		यदि(a.width() == 1) {
			यदि(!is_signed)
				फर्काऊ factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			अन्यथा
				फर्काऊ a;
		}
		पूर्णाङ्क new_width = ceil_log2(a.width());
		गाँठो_2 b_truncated = factory.extend(b, new_width, असत्य);
		गाँठो_2 y =
			!is_दायाँ ? factory.logical_shift_बायाँ(a, b_truncated) :
			!is_signed ? factory.logical_shift_दायाँ(a, b_truncated) :
			factory.arithmetic_shift_दायाँ(a, b_truncated);
		यदि(b.width() <= new_width)
			फर्काऊ y;
		गाँठो_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		गाँठो_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), सत्य) : factory.constant(RTLIL::Const(State::S0, a.width()));
		फर्काऊ factory.mux(y, y_if_overflow, overflow);
	}
सार्वजनिक:
	गाँठो_2 logical_shift_बायाँ(गाँठो_2 a, गाँठो_2 b) { फर्काऊ handle_shift(a, b, असत्य, असत्य); }
	गाँठो_2 logical_shift_दायाँ(गाँठो_2 a, गाँठो_2 b) { फर्काऊ handle_shift(a, b, सत्य, असत्य); }
	गाँठो_2 arithmetic_shift_दायाँ(गाँठो_2 a, गाँठो_2 b) { फर्काऊ handle_shift(a, b, सत्य, सत्य); }
	गाँठो_2 bitwise_mux(गाँठो_2 a, गाँठो_2 b, गाँठो_2 s) {
		गाँठो_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		गाँठो_2 bb = factory.bitwise_and(b, s);
		फर्काऊ factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
गुप्त:
	गाँठो_2 handle_pow(गाँठो_2 a0, गाँठो_2 b, पूर्णाङ्क y_width, तार्किक is_signed) {
		गाँठो_2 a = factory.extend(a0, y_width, is_signed);
		गाँठो_2 r = factory.constant(Const(1, y_width));
		लागि(पूर्णाङ्क i = 0; i < b.width(); i++) {
			गाँठो_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		यदि (is_signed) {
			गाँठो_2 a_ge_1 = factory.unsigned_greater_than(असल(a0), factory.constant(Const(1, a0.width())));
			गाँठो_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		फर्काऊ r;
	}
	गाँठो_2 handle_bmux(गाँठो_2 a, गाँठो_2 s, पूर्णाङ्क a_offset, पूर्णाङ्क width, पूर्णाङ्क sn) {
		यदि(sn < 1)
			फर्काऊ factory.slice(a, a_offset, width);
		अन्यथा {
			गाँठो_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			गाँठो_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			फर्काऊ factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	गाँठो_2 handle_pmux(गाँठो_2 a, गाँठो_2 b, गाँठो_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		गाँठो_2 y = a;
		लागि(पूर्णाङ्क i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		फर्काऊ y;
	}
	dict<IdString, गाँठो_2> handle_fa(गाँठो_2 a, गाँठो_2 b, गाँठो_2 c) {
		गाँठो_2 t1 = factory.bitwise_xor(a, b);
		गाँठो_2 t2 = factory.bitwise_and(a, b);
		गाँठो_2 t3 = factory.bitwise_and(c, t1);
		गाँठो_2 y = factory.bitwise_xor(c, t1);
		गाँठो_2 x = factory.bitwise_or(t2, t3);
		फर्काऊ {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, गाँठो_2> handle_alu(गाँठो_2 a_in, गाँठो_2 b_in, पूर्णाङ्क y_width, तार्किक is_signed, गाँठो_2 ci, गाँठो_2 bi) {
		गाँठो_2 a = factory.extend(a_in, y_width, is_signed);
		गाँठो_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		गाँठो_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		गाँठो_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		गाँठो_2 a_extra = factory.extend(a, y_width + 1, असत्य);
		गाँठो_2 b_extra = factory.extend(b, y_width + 1, असत्य);
		गाँठो_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, असत्य));
		गाँठो_2 y = factory.slice(y_extra, 0, y_width);
		गाँठो_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		गाँठो_2 co = factory.slice(carries, 1, y_width);
		फर्काऊ {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	गाँठो_2 handle_lcu(गाँठो_2 p, गाँठो_2 g, गाँठो_2 ci) {
		फर्काऊ handle_alu(g, factory.bitwise_or(p, g), g.width(), असत्य, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
सार्वजनिक:
	std::variant<dict<IdString, गाँठो_2>, गाँठो_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, गाँठो_2> inputs)
	{
		पूर्णाङ्क a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		पूर्णाङ्क b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		पूर्णाङ्क y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		तार्किक a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		तार्किक b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		यदि(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			तार्किक is_signed = a_signed && b_signed;
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			गाँठो_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			यदि(cellType == ID($add))
				फर्काऊ factory.add(a, b);
			अन्यथा यदि(cellType == ID($sub))
				फर्काऊ factory.sub(a, b);
			अन्यथा यदि(cellType == ID($mul))
				फर्काऊ factory.mul(a, b);
			अन्यथा यदि(cellType == ID($and))
				फर्काऊ factory.bitwise_and(a, b);
			अन्यथा यदि(cellType == ID($or))
				फर्काऊ factory.bitwise_or(a, b);
			अन्यथा यदि(cellType == ID($xor))
				फर्काऊ factory.bitwise_xor(a, b);
			अन्यथा यदि(cellType == ID($xnor))
				फर्काऊ factory.bitwise_not(factory.bitwise_xor(a, b));
			अन्यथा
				log_abort();
		}अन्यथा यदि(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			तार्किक is_signed = a_signed && b_signed;
			पूर्णाङ्क width = max(a_width, b_width);
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाँठो_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			यदि(cellType.in(ID($eq), ID($eqx)))
				फर्काऊ factory.extend(factory.equal(a, b), y_width, असत्य);
			अन्यथा यदि(cellType.in(ID($ne), ID($nex)))
				फर्काऊ factory.extend(factory.not_equal(a, b), y_width, असत्य);
			अन्यथा यदि(cellType == ID($lt))
				फर्काऊ factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, असत्य);
			अन्यथा यदि(cellType == ID($le))
				फर्काऊ factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, असत्य);
			अन्यथा यदि(cellType == ID($gt))
				फर्काऊ factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, असत्य);
			अन्यथा यदि(cellType == ID($ge))
				फर्काऊ factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, असत्य);
			अन्यथा
				log_abort();
		}अन्यथा यदि(cellType.in(ID($logic_or), ID($logic_and))){
			गाँठो_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाँठो_2 b = factory.reduce_or(inputs.at(ID(B)));
			गाँठो_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			फर्काऊ factory.extend(y, y_width, असत्य);
		}अन्यथा यदि(cellType == ID($not)){
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			फर्काऊ factory.bitwise_not(a);
		}अन्यथा यदि(cellType == ID($pos)){
			फर्काऊ factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}अन्यथा यदि(cellType == ID($barrier)){
			फर्काऊ inputs.at(ID(A));
		}अन्यथा यदि(cellType == ID($neg)){
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			फर्काऊ factory.unary_minus(a);
		}अन्यथा यदि(cellType == ID($logic_not)){
			गाँठो_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाँठो_2 y = factory.bitwise_not(a);
			फर्काऊ factory.extend(y, y_width, असत्य);
		}अन्यथा यदि(cellType.in(ID($reduce_or), ID($reduce_bool))){
			गाँठो_2 a = factory.reduce_or(inputs.at(ID(A)));
			फर्काऊ factory.extend(a, y_width, असत्य);
		}अन्यथा यदि(cellType == ID($reduce_and)){
			गाँठो_2 a = factory.reduce_and(inputs.at(ID(A)));
			फर्काऊ factory.extend(a, y_width, असत्य);
		}अन्यथा यदि(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			गाँठो_2 a = factory.reduce_xor(inputs.at(ID(A)));
			गाँठो_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			फर्काऊ factory.extend(y, y_width, असत्य);
		}अन्यथा यदि(cellType == ID($shl) || cellType == ID($sshl)){
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			गाँठो_2 b = inputs.at(ID(B));
			फर्काऊ logical_shift_बायाँ(a, b);
		}अन्यथा यदि(cellType == ID($shr) || cellType == ID($sshr)){
			पूर्णाङ्क width = max(a_width, y_width);
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			गाँठो_2 b = inputs.at(ID(B));
			गाँठो_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_दायाँ(a, b) :
				logical_shift_दायाँ(a, b);
			फर्काऊ factory.extend(y, y_width, a_signed);
		}अन्यथा यदि(cellType == ID($shiftx) || cellType == ID($shift)){
			पूर्णाङ्क width = max(a_width, y_width);
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			गाँठो_2 b = inputs.at(ID(B));
			गाँठो_2 shr = logical_shift_दायाँ(a, b);
			यदि(b_signed) {
				गाँठो_2 shl = logical_shift_बायाँ(a, factory.unary_minus(b));
				गाँठो_2 y = factory.mux(shr, shl, sign(b));
				फर्काऊ factory.extend(y, y_width, असत्य);
			} अन्यथा {
				फर्काऊ factory.extend(shr, y_width, असत्य);
			}
		}अन्यथा यदि(cellType == ID($mux)){
			फर्काऊ factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा यदि(cellType == ID($pmux)){
			फर्काऊ handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा यदि(cellType == ID($concat)){
			गाँठो_2 a = inputs.at(ID(A));
			गाँठो_2 b = inputs.at(ID(B));
			फर्काऊ factory.concat(a, b);
		}अन्यथा यदि(cellType == ID($slice)){
			पूर्णाङ्क offset = parameters.at(ID(OFFSET)).as_int();
			गाँठो_2 a = inputs.at(ID(A));
			फर्काऊ factory.slice(a, offset, y_width);
		}अन्यथा यदि(cellType.in(ID($भाग), ID($mod), ID($divfloor), ID($modfloor))) {
			पूर्णाङ्क width = max(a_width, b_width);
			तार्किक is_signed = a_signed && b_signed;
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाँठो_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			यदि(is_signed) {
				यदि(cellType == ID($भाग)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					गाँठो_2 abs_y = factory.unsigned_div(असल(a), असल(b));
					गाँठो_2 out_sign = factory.not_equal(sign(a), sign(b));
					फर्काऊ neg_if(factory.extend(abs_y, y_width, असत्य), out_sign);
				} अन्यथा यदि(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					गाँठो_2 abs_y = factory.unsigned_mod(असल(a), असल(b));
					फर्काऊ neg_if(factory.extend(abs_y, y_width, असत्य), sign(a));
				} अन्यथा यदि(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					गाँठो_2 b_sign = sign(b);
					गाँठो_2 a1 = neg_if(a, b_sign);
					गाँठो_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					गाँठो_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					गाँठो_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					गाँठो_2 y1 = factory.unsigned_div(a2, b1);
					गाँठो_2 y2 = factory.extend(y1, y_width, असत्य);
					फर्काऊ factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} अन्यथा यदि(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					गाँठो_2 abs_b = असल(b);
					गाँठो_2 abs_y = factory.unsigned_mod(असल(a), abs_b);
					गाँठो_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					गाँठो_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					गाँठो_2 y = neg_if(y_flipped, sign(b));
					फर्काऊ factory.extend(y, y_width, सत्य);
				} अन्यथा
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} अन्यथा {
				यदि(cellType.in(ID($mod), ID($modfloor)))
					फर्काऊ factory.extend(factory.unsigned_mod(a, b), y_width, असत्य);
				अन्यथा
					फर्काऊ factory.extend(factory.unsigned_div(a, b), y_width, असत्य);
			}
		} अन्यथा यदि(cellType == ID($घात)) {
			फर्काऊ handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} अन्यथा यदि (cellType == ID($lut)) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			फर्काऊ handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} अन्यथा यदि (cellType == ID($bwmux)) {
			गाँठो_2 a = inputs.at(ID(A));
			गाँठो_2 b = inputs.at(ID(B));
			गाँठो_2 s = inputs.at(ID(S));
			फर्काऊ factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} अन्यथा यदि (cellType == ID($bweqx)) {
			गाँठो_2 a = inputs.at(ID(A));
			गाँठो_2 b = inputs.at(ID(B));
			फर्काऊ factory.bitwise_not(factory.bitwise_xor(a, b));
		} अन्यथा यदि(cellType == ID($bmux)) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			पूर्णाङ्क s_width = parameters.at(ID(S_WIDTH)).as_int();
			फर्काऊ handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} अन्यथा यदि(cellType == ID($demux)) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			पूर्णाङ्क s_width = parameters.at(ID(S_WIDTH)).as_int();
			पूर्णाङ्क y_width = width << s_width;
			पूर्णाङ्क b_width = ceil_log2(y_width);
			गाँठो_2 a = factory.extend(inputs.at(ID(A)), y_width, असत्य);
			गाँठो_2 s = factory.extend(inputs.at(ID(S)), b_width, असत्य);
			गाँठो_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			फर्काऊ factory.logical_shift_बायाँ(a, b);
		} अन्यथा यदि(cellType == ID($fa)) {
			फर्काऊ handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} अन्यथा यदि(cellType == ID($lcu)) {
			फर्काऊ handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} अन्यथा यदि(cellType == ID($alu)) {
			फर्काऊ handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} अन्यथा यदि(cellType.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover))) {
			गाँठो_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			स्वतः &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_मान(a);
			फर्काऊ {};
		} अन्यथा यदि(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			स्वतः &आगत = factory.add_आगत(cellName, cellType, Sort(width));
			फर्काऊ factory.मान(आगत);
		} अन्यथा यदि(cellType == ID($initstate)) {
			यदि(factory.ir().has_state(ID($initstate), ID($state)))
				फर्काऊ factory.मान(factory.ir().state(ID($initstate)));
			अन्यथा {
				स्वतः &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_मान(RTLIL::Const(State::S1, 1));
				state.set_next_मान(factory.constant(RTLIL::Const(State::S0, 1)));
				फर्काऊ factory.मान(state);
			}
		} अन्यथा यदि(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} अन्यथा {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

वर्ग FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, गाँठो_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, गाँठो_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	गाँठो_2 enqueue(DriveSpec अचर &spec)
	{
		स्वतः it = graph_nodes.find(spec);
		यदि(it == graph_nodes.end()){
			स्वतः गाँठो = factory.create_pending(spec.size());
			graph_nodes.घुसाउ({spec, गाँठो});
			queue.emplace_back(spec);
			फर्काऊ गाँठो;
		}अन्यथा
			फर्काऊ it->second;
	}
	गाँठो_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		स्वतः it = cell_outputs.find({cell, port_name});
		यदि(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<गाँठो_2> rv;
			लागि(स्वतः अचर &[name, sigspec] : cell->connections())
				यदि(driver_map.celltypes.cell_output(cell->type, name)) {
					स्वतः गाँठो = factory.create_pending(sigspec.size());
					factory.suggest_name(गाँठो, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, गाँठो);
					यदि(name == port_name)
						rv = गाँठो;
				}
			फर्काऊ *rv;
		} अन्यथा
			फर्काऊ it->second;
	}
सार्वजनिक:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		लागि (स्वतः cell : module->cells()) {
			यदि (cell->type.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		लागि (स्वतः riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			स्वतः *wire = module->wire(*riter);
			यदि (wire && wire->port_आगत) {
				factory.add_आगत(wire->name, ID($आगत), Sort(wire->width));
			}
			यदि (wire && wire->port_output) {
				स्वतः &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_मान(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		लागि (स्वतः &mem : memories_vector) {
			यदि (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
गुप्त:
	गाँठो_2 concatenate_read_results(Mem *mem, vector<गाँठो_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		अचर SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		पूर्णाङ्क current = 0;
		लागि(माप_प्रकार i = 0; i < mem->rd_ports.size(); i++) {
			पूर्णाङ्क width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		गाँठो_2 गाँठो = results[0];
		लागि(माप_प्रकार i = 1; i < results.size(); i++)
			गाँठो = factory.concat(गाँठो, results[i]);
		फर्काऊ गाँठो;
	}
	गाँठो_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<गाँठो_2> read_results;
		स्वतः &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_मान(MemContents(mem));
		गाँठो_2 गाँठो = factory.मान(state);
		लागि (माप_प्रकार i = 0; i < mem->wr_ports.size(); i++) {
			अचर स्वतः &wr = mem->wr_ports[i];
			यदि (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाँठो_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			गाँठो_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			गाँठो_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			गाँठो_2 old_data = factory.memory_read(गाँठो, addr);
			गाँठो_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			गाँठो = factory.memory_write(गाँठो, addr, wr_data);
		}
		यदि (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		लागि (माप_प्रकार i = 0; i < mem->rd_ports.size(); i++) {
			अचर स्वतः &rd = mem->rd_ports[i];
			यदि (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाँठो_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.थप_back(factory.memory_read(गाँठो, addr));
		}
		state.set_next_मान(गाँठो);
		फर्काऊ concatenate_read_results(mem, read_results);
	}
	शून्य process_cell(Cell *cell)
	{
		यदि (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			यदि (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			गाँठो_2 गाँठो = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), गाँठो);
		} अन्यथा यदि (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			यदि (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			स्वतः &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			गाँठो_2 q_मान = factory.मान(state);
			factory.suggest_name(q_मान, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_मान);
			state.set_next_मान(enqueue(ff.sig_d));
			state.set_initial_मान(ff.val_init);
		} अन्यथा {
			dict<IdString, गाँठो_2> connections;
			IdString output_name; // for the single output case
			पूर्णाङ्क n_outputs = 0;
			लागि(स्वतः अचर &[name, sigspec] : cell->connections()) {
				यदि(driver_map.celltypes.cell_आगत(cell->type, name) && sigspec.size() > 0)
					connections.घुसाउ({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				यदि(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, गाँठो_2>, गाँठो_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			यदि(स्वतः *nodep = std::get_if<गाँठो_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} अन्यथा {
				लागि(स्वतः [name, गाँठो] : std::get<dict<IdString, गाँठो_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), गाँठो);
			}
		}
	}
	शून्य undriven(अचर std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	शून्य check_undriven(DriveSpec अचर& spec, std::string अचर& name) {
		लागि(स्वतः अचर &chunk : spec.chunks())
			यदि(chunk.is_none())
				undriven(name);
	}
सार्वजनिक:
	शून्य process_queue()
	{
		लागि (; !queue.empty(); queue.झिक_front()) {
			यदि(स्वतः p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				जारी;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			गाँठो_2 pending = graph_nodes.at(spec);

			यदि (spec.chunks().size() > 1) {
				स्वतः chunks = spec.chunks();
				गाँठो_2 गाँठो = enqueue(chunks[0]);
				लागि(माप_प्रकार i = 1; i < chunks.size(); i++)
					गाँठो = factory.concat(गाँठो, enqueue(chunks[i]));
				factory.update_pending(pending, गाँठो);
			} अन्यथा यदि (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				यदि (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					यदि (wire_chunk.is_whole()) {
						यदि (wire_chunk.wire->port_आगत) {
							गाँठो_2 गाँठो = factory.मान(factory.ir().आगत(wire_chunk.wire->name));
							factory.suggest_name(गाँठो, wire_chunk.wire->name);
							factory.update_pending(pending, गाँठो);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							गाँठो_2 गाँठो = enqueue(driver);
							factory.suggest_name(गाँठो, wire_chunk.wire->name);
							factory.update_pending(pending, गाँठो);
						}
					} अन्यथा {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						गाँठो_2 गाँठो = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, गाँठो);
					}
				} अन्यथा यदि (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					यदि (port_chunk.is_whole()) {
						यदि (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							गाँठो_2 गाँठो = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, गाँठो);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} अन्यथा {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						गाँठो_2 गाँठो = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, गाँठो);
					}
				} अन्यथा यदि (chunk.is_constant()) {
					गाँठो_2 गाँठो = factory.constant(chunk.constant());
					factory.suggest_name(गाँठो, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, गाँठो);
				} अन्यथा यदि (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} अन्यथा यदि (chunk.is_none()) {
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
    फर्काऊ ir;
}

शून्य IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    तार्किक scc = असत्य;
    std::vector<पूर्णाङ्क> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](पूर्णाङ्क *begin, पूर्णाङ्क *end) {
        perm.घुसाउ(perm.end(), begin, end);
        यदि (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            लागि (पूर्णाङ्क *i = begin; i != end; ++i) {
				गाँठो_2 गाँठो(_graph[*i], design);
                log("- %s = %s\n", गाँठो.name().unescape(), गाँठो.to_string());
			}
            log("\n");
            scc = सत्य;
        }
    });
	लागि(अचर स्वतः &[name, state]: _states)
		यदि(state.has_next_मान())
			toposort.process(state.next_मान().id());
	लागि(अचर स्वतः &[name, output]: _outputs)
		यदि(output.has_मान())
			toposort.process(output.मान().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    यदि(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

स्थिर IdString merge_name(IdString a, IdString b) {
	यदि(!a.isPublic() && b.isPublic())
		फर्काऊ b;
	अन्यथा
		फर्काऊ a;
}

शून्य IR::forward_buf() {
    std::vector<पूर्णाङ्क> perm, alias;
    perm.clear();

    लागि (पूर्णाङ्क i = 0; i < _graph.size(); ++i)
    {
        स्वतः गाँठो = _graph[i];
        यदि (गाँठो.function().fn() == Fn::buf && गाँठो.arg(0).index() < i)
        {
            पूर्णाङ्क target_index = alias[गाँठो.arg(0).index()];
            स्वतः target_गाँठो = _graph[perm[target_index]];
			यदि(गाँठो.has_sparse_attr()) {
				यदि(target_गाँठो.has_sparse_attr()) {
					IdString id = merge_name(गाँठो.sparse_attr(), target_गाँठो.sparse_attr());
					target_गाँठो.sparse_attr() = id;
				} अन्यथा {
					IdString id = गाँठो.sparse_attr();
					target_गाँठो.sparse_attr() = id;
				}
			}
            alias.थप_back(target_index);
        }
        अन्यथा
        {
            alias.थप_back(GetSize(perm));
            perm.थप_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
स्थिर std::string quote_fmt(अचर वर्ण *fmt)
{
	std::string r;
	लागि(अचर वर्ण *p = fmt; *p != 0; p++) {
		विकल्प(*p) {
		पक्ष '\n': r += "\\n"; रोक;
		पक्ष '\t': r += "\\t"; रोक;
		पक्ष '"': r += "\\\""; रोक;
		पक्ष '\\': r += "\\\\"; रोक;
		स्वभाव: r += *p; रोक;
		}
	}
	फर्काऊ r;
}

शून्य Writer::print_impl(अचर वर्ण *fmt, vector<std::function<शून्य()>> &fns)
{
	माप_प्रकार next_index = 0;
	लागि(अचर वर्ण *p = fmt; *p != 0; p++)
		विकल्प(*p) {
		पक्ष '{':
			यदि(*++p == '{') {
				*os << '{';
			} अन्यथा {
				वर्ण *pe;
				माप_प्रकार index = म_से_अदीर्घ(p, &pe, 10);
				यदि(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				यदि(p == pe)
					index = next_index;
				अन्यथा
					p = pe;
				यदि(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			रोक;
		पक्ष '}':
			p++;
			यदि(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			रोक;
		स्वभाव:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
