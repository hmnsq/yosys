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

ध्रुव वर्ण *fn_to_string(Fn fn) {
	विकल्प(fn) {
	पक्ष Fn::invalid: प्रत्यागम "invalid";
	पक्ष Fn::buf: प्रत्यागम "buf";
	पक्ष Fn::slice: प्रत्यागम "slice";
	पक्ष Fn::zero_extend: प्रत्यागम "zero_extend";
	पक्ष Fn::sign_extend: प्रत्यागम "sign_extend";
	पक्ष Fn::concat: प्रत्यागम "concat";
	पक्ष Fn::add: प्रत्यागम "add";
	पक्ष Fn::sub: प्रत्यागम "sub";
	पक्ष Fn::mul: प्रत्यागम "mul";
	पक्ष Fn::unsigned_div: प्रत्यागम "unsigned_div";
	पक्ष Fn::unsigned_mod: प्रत्यागम "unsigned_mod";
	पक्ष Fn::bitwise_and: प्रत्यागम "bitwise_and";
	पक्ष Fn::bitwise_or: प्रत्यागम "bitwise_or";
	पक्ष Fn::bitwise_xor: प्रत्यागम "bitwise_xor";
	पक्ष Fn::bitwise_not: प्रत्यागम "bitwise_not";
	पक्ष Fn::reduce_and: प्रत्यागम "reduce_and";
	पक्ष Fn::reduce_or: प्रत्यागम "reduce_or";
	पक्ष Fn::reduce_xor: प्रत्यागम "reduce_xor";
	पक्ष Fn::unary_minus: प्रत्यागम "unary_minus";
	पक्ष Fn::equal: प्रत्यागम "equal";
	पक्ष Fn::not_equal: प्रत्यागम "not_equal";
	पक्ष Fn::signed_greater_than: प्रत्यागम "signed_greater_than";
	पक्ष Fn::signed_greater_equal: प्रत्यागम "signed_greater_equal";
	पक्ष Fn::unsigned_greater_than: प्रत्यागम "unsigned_greater_than";
	पक्ष Fn::unsigned_greater_equal: प्रत्यागम "unsigned_greater_equal";
	पक्ष Fn::logical_shift_बायाँ: प्रत्यागम "logical_shift_left";
	पक्ष Fn::logical_shift_दायाँ: प्रत्यागम "logical_shift_right";
	पक्ष Fn::arithmetic_shift_दायाँ: प्रत्यागम "arithmetic_shift_right";
	पक्ष Fn::mux: प्रत्यागम "mux";
	पक्ष Fn::constant: प्रत्यागम "constant";
	पक्ष Fn::निवेश: प्रत्यागम "input";
	पक्ष Fn::state: प्रत्यागम "state";
	पक्ष Fn::memory_read: प्रत्यागम "memory_read";
	पक्ष Fn::memory_write: प्रत्यागम "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (पूर्णाङ्क)fn);
}

vector<IRInput ध्रुव*> IR::inputs(IdString kind) ध्रुव {
	vector<IRInput ध्रुव*> ret;
	प्रति (ध्रुव auto &[name, निवेश] : _inputs)
		यदि(निवेश.kind == kind)
			ret.धकेलो_back(&निवेश);
	प्रत्यागम ret;
}

vector<IROutput ध्रुव*> IR::outputs(IdString kind) ध्रुव {
	vector<IROutput ध्रुव*> ret;
	प्रति (ध्रुव auto &[name, output] : _outputs)
		यदि(output.kind == kind)
			ret.धकेलो_back(&output);
	प्रत्यागम ret;
}

vector<IRState ध्रुव*> IR::states(IdString kind) ध्रुव {
	vector<IRState ध्रुव*> ret;
	प्रति (ध्रुव auto &[name, state] : _states)
		यदि(state.kind == kind)
			ret.धकेलो_back(&state);
	प्रत्यागम ret;
}

vector<IRInput ध्रुव*> IR::all_inputs() ध्रुव {
	vector<IRInput ध्रुव*> ret;
	प्रति (ध्रुव auto &[name, निवेश] : _inputs)
		ret.धकेलो_back(&निवेश);
	प्रत्यागम ret;
}

vector<IROutput ध्रुव*> IR::all_outputs() ध्रुव {
	vector<IROutput ध्रुव*> ret;
	प्रति (ध्रुव auto &[name, output] : _outputs)
		ret.धकेलो_back(&output);
	प्रत्यागम ret;
}

vector<IRState ध्रुव*> IR::all_states() ध्रुव {
	vector<IRState ध्रुव*> ret;
	प्रति (ध्रुव auto &[name, state] : _states)
		ret.धकेलो_back(&state);
	प्रत्यागम ret;
}

संरचना PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(गाँठ_2)> np;
	PrintVisitor(std::function<std::string(गाँठ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(गाँठ_2, गाँठ_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) override { प्रत्यागम "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(गाँठ_2, गाँठ_2 a, पूर्णाङ्क out_width) override { प्रत्यागम "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(गाँठ_2, गाँठ_2 a, पूर्णाङ्क out_width) override { प्रत्यागम "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(गाँठ_2, RTLIL::Const ध्रुव& मूल्य) override { प्रत्यागम "constant(" + मूल्य.as_string() + ")"; }
	std::string निवेश(गाँठ_2 self, IdString name, IdString kind) override { प्रत्यागम "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(गाँठ_2 self, IdString name, IdString kind) override { प्रत्यागम "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(गाँठ_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		प्रति(size_t i = 0; i < self.arg_गिनती(); i++) {
			यदि(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		प्रत्यागम ret;
	}
};

std::string गाँठ_2::to_string()
{
	प्रत्यागम to_string([](गाँठ_2 n) { प्रत्यागम n.name().unescape(); });
}

std::string गाँठ_2::to_string(std::function<std::string(गाँठ_2)> np)
{
	प्रत्यागम visit(PrintVisitor(np));
}

वर्ग CellSimplifier {
	Factory &factory;
	गाँठ_2 sign(गाँठ_2 a) {
		प्रत्यागम factory.slice(a, a.width() - 1, 1);
	}
	गाँठ_2 neg_if(गाँठ_2 a, गाँठ_2 s) {
		प्रत्यागम factory.mux(a, factory.unary_minus(a), s);
	}
	गाँठ_2 abs(गाँठ_2 a) {
		प्रत्यागम neg_if(a, sign(a));
	}
	गाँठ_2 handle_shift(गाँठ_2 a, गाँठ_2 b, तार्किक is_दायाँ, तार्किक is_signed) {
		// to prevent new_width == 0, we handle this case separately
		यदि(a.width() == 1) {
			यदि(!is_signed)
				प्रत्यागम factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			अन्यथा
				प्रत्यागम a;
		}
		पूर्णाङ्क new_width = ceil_log2(a.width());
		गाँठ_2 b_truncated = factory.extend(b, new_width, असत्य);
		गाँठ_2 y =
			!is_दायाँ ? factory.logical_shift_बायाँ(a, b_truncated) :
			!is_signed ? factory.logical_shift_दायाँ(a, b_truncated) :
			factory.arithmetic_shift_दायाँ(a, b_truncated);
		यदि(b.width() <= new_width)
			प्रत्यागम y;
		गाँठ_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		गाँठ_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), सत्य) : factory.constant(RTLIL::Const(State::S0, a.width()));
		प्रत्यागम factory.mux(y, y_if_overflow, overflow);
	}
सार्वजनिक:
	गाँठ_2 logical_shift_बायाँ(गाँठ_2 a, गाँठ_2 b) { प्रत्यागम handle_shift(a, b, असत्य, असत्य); }
	गाँठ_2 logical_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { प्रत्यागम handle_shift(a, b, सत्य, असत्य); }
	गाँठ_2 arithmetic_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { प्रत्यागम handle_shift(a, b, सत्य, सत्य); }
	गाँठ_2 bitwise_mux(गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) {
		गाँठ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		गाँठ_2 bb = factory.bitwise_and(b, s);
		प्रत्यागम factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
गुप्त:
	गाँठ_2 handle_pow(गाँठ_2 a0, गाँठ_2 b, पूर्णाङ्क y_width, तार्किक is_signed) {
		गाँठ_2 a = factory.extend(a0, y_width, is_signed);
		गाँठ_2 r = factory.constant(Const(1, y_width));
		प्रति(पूर्णाङ्क i = 0; i < b.width(); i++) {
			गाँठ_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		यदि (is_signed) {
			गाँठ_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			गाँठ_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		प्रत्यागम r;
	}
	गाँठ_2 handle_bmux(गाँठ_2 a, गाँठ_2 s, पूर्णाङ्क a_offset, पूर्णाङ्क width, पूर्णाङ्क sn) {
		यदि(sn < 1)
			प्रत्यागम factory.slice(a, a_offset, width);
		अन्यथा {
			गाँठ_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			गाँठ_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			प्रत्यागम factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	गाँठ_2 handle_pmux(गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		गाँठ_2 y = a;
		प्रति(पूर्णाङ्क i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		प्रत्यागम y;
	}
	dict<IdString, गाँठ_2> handle_fa(गाँठ_2 a, गाँठ_2 b, गाँठ_2 c) {
		गाँठ_2 t1 = factory.bitwise_xor(a, b);
		गाँठ_2 t2 = factory.bitwise_and(a, b);
		गाँठ_2 t3 = factory.bitwise_and(c, t1);
		गाँठ_2 y = factory.bitwise_xor(c, t1);
		गाँठ_2 x = factory.bitwise_or(t2, t3);
		प्रत्यागम {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, गाँठ_2> handle_alu(गाँठ_2 a_in, गाँठ_2 b_in, पूर्णाङ्क y_width, तार्किक is_signed, गाँठ_2 ci, गाँठ_2 bi) {
		गाँठ_2 a = factory.extend(a_in, y_width, is_signed);
		गाँठ_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		गाँठ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		गाँठ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		गाँठ_2 a_extra = factory.extend(a, y_width + 1, असत्य);
		गाँठ_2 b_extra = factory.extend(b, y_width + 1, असत्य);
		गाँठ_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, असत्य));
		गाँठ_2 y = factory.slice(y_extra, 0, y_width);
		गाँठ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		गाँठ_2 co = factory.slice(carries, 1, y_width);
		प्रत्यागम {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	गाँठ_2 handle_lcu(गाँठ_2 p, गाँठ_2 g, गाँठ_2 ci) {
		प्रत्यागम handle_alu(g, factory.bitwise_or(p, g), g.width(), असत्य, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
सार्वजनिक:
	std::variant<dict<IdString, गाँठ_2>, गाँठ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, गाँठ_2> inputs)
	{
		पूर्णाङ्क a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		पूर्णाङ्क b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		पूर्णाङ्क y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		तार्किक a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		तार्किक b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		यदि(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			तार्किक is_signed = a_signed && b_signed;
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			गाँठ_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			यदि(cellType == ID($add))
				प्रत्यागम factory.add(a, b);
			अन्यथा यदि(cellType == ID($sub))
				प्रत्यागम factory.sub(a, b);
			अन्यथा यदि(cellType == ID($mul))
				प्रत्यागम factory.mul(a, b);
			अन्यथा यदि(cellType == ID($and))
				प्रत्यागम factory.bitwise_and(a, b);
			अन्यथा यदि(cellType == ID($or))
				प्रत्यागम factory.bitwise_or(a, b);
			अन्यथा यदि(cellType == ID($xor))
				प्रत्यागम factory.bitwise_xor(a, b);
			अन्यथा यदि(cellType == ID($xnor))
				प्रत्यागम factory.bitwise_not(factory.bitwise_xor(a, b));
			अन्यथा
				log_abort();
		}अन्यथा यदि(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			तार्किक is_signed = a_signed && b_signed;
			पूर्णाङ्क width = max(a_width, b_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाँठ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			यदि(cellType.in(ID($eq), ID($eqx)))
				प्रत्यागम factory.extend(factory.equal(a, b), y_width, असत्य);
			अन्यथा यदि(cellType.in(ID($ne), ID($nex)))
				प्रत्यागम factory.extend(factory.not_equal(a, b), y_width, असत्य);
			अन्यथा यदि(cellType == ID($lt))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, असत्य);
			अन्यथा यदि(cellType == ID($le))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, असत्य);
			अन्यथा यदि(cellType == ID($gt))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, असत्य);
			अन्यथा यदि(cellType == ID($ge))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, असत्य);
			अन्यथा
				log_abort();
		}अन्यथा यदि(cellType.in(ID($logic_or), ID($logic_and))){
			गाँठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाँठ_2 b = factory.reduce_or(inputs.at(ID(B)));
			गाँठ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			प्रत्यागम factory.extend(y, y_width, असत्य);
		}अन्यथा यदि(cellType == ID($not)){
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			प्रत्यागम factory.bitwise_not(a);
		}अन्यथा यदि(cellType == ID($pos)){
			प्रत्यागम factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}अन्यथा यदि(cellType == ID($barrier)){
			प्रत्यागम inputs.at(ID(A));
		}अन्यथा यदि(cellType == ID($neg)){
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			प्रत्यागम factory.unary_minus(a);
		}अन्यथा यदि(cellType == ID($logic_not)){
			गाँठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			गाँठ_2 y = factory.bitwise_not(a);
			प्रत्यागम factory.extend(y, y_width, असत्य);
		}अन्यथा यदि(cellType.in(ID($reduce_or), ID($reduce_bool))){
			गाँठ_2 a = factory.reduce_or(inputs.at(ID(A)));
			प्रत्यागम factory.extend(a, y_width, असत्य);
		}अन्यथा यदि(cellType == ID($reduce_and)){
			गाँठ_2 a = factory.reduce_and(inputs.at(ID(A)));
			प्रत्यागम factory.extend(a, y_width, असत्य);
		}अन्यथा यदि(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			गाँठ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			गाँठ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			प्रत्यागम factory.extend(y, y_width, असत्य);
		}अन्यथा यदि(cellType == ID($shl) || cellType == ID($sshl)){
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			गाँठ_2 b = inputs.at(ID(B));
			प्रत्यागम logical_shift_बायाँ(a, b);
		}अन्यथा यदि(cellType == ID($shr) || cellType == ID($sshr)){
			पूर्णाङ्क width = max(a_width, y_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			गाँठ_2 b = inputs.at(ID(B));
			गाँठ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_दायाँ(a, b) :
				logical_shift_दायाँ(a, b);
			प्रत्यागम factory.extend(y, y_width, a_signed);
		}अन्यथा यदि(cellType == ID($shiftx) || cellType == ID($shift)){
			पूर्णाङ्क width = max(a_width, y_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			गाँठ_2 b = inputs.at(ID(B));
			गाँठ_2 shr = logical_shift_दायाँ(a, b);
			यदि(b_signed) {
				गाँठ_2 shl = logical_shift_बायाँ(a, factory.unary_minus(b));
				गाँठ_2 y = factory.mux(shr, shl, sign(b));
				प्रत्यागम factory.extend(y, y_width, असत्य);
			} अन्यथा {
				प्रत्यागम factory.extend(shr, y_width, असत्य);
			}
		}अन्यथा यदि(cellType == ID($mux)){
			प्रत्यागम factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा यदि(cellType == ID($pmux)){
			प्रत्यागम handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा यदि(cellType == ID($concat)){
			गाँठ_2 a = inputs.at(ID(A));
			गाँठ_2 b = inputs.at(ID(B));
			प्रत्यागम factory.concat(a, b);
		}अन्यथा यदि(cellType == ID($slice)){
			पूर्णाङ्क offset = parameters.at(ID(OFFSET)).as_int();
			गाँठ_2 a = inputs.at(ID(A));
			प्रत्यागम factory.slice(a, offset, y_width);
		}अन्यथा यदि(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			पूर्णाङ्क width = max(a_width, b_width);
			तार्किक is_signed = a_signed && b_signed;
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			गाँठ_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			यदि(is_signed) {
				यदि(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					गाँठ_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					गाँठ_2 out_sign = factory.not_equal(sign(a), sign(b));
					प्रत्यागम neg_if(factory.extend(abs_y, y_width, असत्य), out_sign);
				} अन्यथा यदि(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					गाँठ_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					प्रत्यागम neg_if(factory.extend(abs_y, y_width, असत्य), sign(a));
				} अन्यथा यदि(cellType == ID($divfloor)) {
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
					गाँठ_2 y2 = factory.extend(y1, y_width, असत्य);
					प्रत्यागम factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} अन्यथा यदि(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					गाँठ_2 abs_b = abs(b);
					गाँठ_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					गाँठ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					गाँठ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					गाँठ_2 y = neg_if(y_flipped, sign(b));
					प्रत्यागम factory.extend(y, y_width, सत्य);
				} अन्यथा
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} अन्यथा {
				यदि(cellType.in(ID($mod), ID($modfloor)))
					प्रत्यागम factory.extend(factory.unsigned_mod(a, b), y_width, असत्य);
				अन्यथा
					प्रत्यागम factory.extend(factory.unsigned_div(a, b), y_width, असत्य);
			}
		} अन्यथा यदि(cellType == ID($pow)) {
			प्रत्यागम handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} अन्यथा यदि (cellType == ID($lut)) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			प्रत्यागम handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} अन्यथा यदि (cellType == ID($bwmux)) {
			गाँठ_2 a = inputs.at(ID(A));
			गाँठ_2 b = inputs.at(ID(B));
			गाँठ_2 s = inputs.at(ID(S));
			प्रत्यागम factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} अन्यथा यदि (cellType == ID($bweqx)) {
			गाँठ_2 a = inputs.at(ID(A));
			गाँठ_2 b = inputs.at(ID(B));
			प्रत्यागम factory.bitwise_not(factory.bitwise_xor(a, b));
		} अन्यथा यदि(cellType == ID($bmux)) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			पूर्णाङ्क s_width = parameters.at(ID(S_WIDTH)).as_int();
			प्रत्यागम handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} अन्यथा यदि(cellType == ID($demux)) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			पूर्णाङ्क s_width = parameters.at(ID(S_WIDTH)).as_int();
			पूर्णाङ्क y_width = width << s_width;
			पूर्णाङ्क b_width = ceil_log2(y_width);
			गाँठ_2 a = factory.extend(inputs.at(ID(A)), y_width, असत्य);
			गाँठ_2 s = factory.extend(inputs.at(ID(S)), b_width, असत्य);
			गाँठ_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			प्रत्यागम factory.logical_shift_बायाँ(a, b);
		} अन्यथा यदि(cellType == ID($fa)) {
			प्रत्यागम handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} अन्यथा यदि(cellType == ID($lcu)) {
			प्रत्यागम handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} अन्यथा यदि(cellType == ID($alu)) {
			प्रत्यागम handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} अन्यथा यदि(cellType.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover))) {
			गाँठ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_मूल्य(a);
			प्रत्यागम {};
		} अन्यथा यदि(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			पूर्णाङ्क width = parameters.at(ID(WIDTH)).as_int();
			auto &निवेश = factory.add_निवेश(cellName, cellType, Sort(width));
			प्रत्यागम factory.मूल्य(निवेश);
		} अन्यथा यदि(cellType == ID($initstate)) {
			यदि(factory.ir().has_state(ID($initstate), ID($state)))
				प्रत्यागम factory.मूल्य(factory.ir().state(ID($initstate)));
			अन्यथा {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_मूल्य(RTLIL::Const(State::S1, 1));
				state.set_next_मूल्य(factory.constant(RTLIL::Const(State::S0, 1)));
				प्रत्यागम factory.मूल्य(state);
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
	dict<DriveSpec, गाँठ_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, गाँठ_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	गाँठ_2 enqueue(DriveSpec ध्रुव &spec)
	{
		auto it = graph_nodes.find(spec);
		यदि(it == graph_nodes.end()){
			auto गाँठ = factory.create_pending(spec.size());
			graph_nodes.जोड़ो({spec, गाँठ});
			queue.emplace_back(spec);
			प्रत्यागम गाँठ;
		}अन्यथा
			प्रत्यागम it->second;
	}
	गाँठ_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		यदि(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<गाँठ_2> rv;
			प्रति(auto ध्रुव &[name, sigspec] : cell->connections())
				यदि(driver_map.celltypes.cell_output(cell->type, name)) {
					auto गाँठ = factory.create_pending(sigspec.size());
					factory.suggest_name(गाँठ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, गाँठ);
					यदि(name == port_name)
						rv = गाँठ;
				}
			प्रत्यागम *rv;
		} अन्यथा
			प्रत्यागम it->second;
	}
सार्वजनिक:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		प्रति (auto cell : module->cells()) {
			यदि (cell->type.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		प्रति (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			यदि (wire && wire->port_निवेश) {
				factory.add_निवेश(wire->name, ID($निवेश), Sort(wire->width));
			}
			यदि (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_मूल्य(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		प्रति (auto &mem : memories_vector) {
			यदि (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
गुप्त:
	गाँठ_2 concatenate_read_results(Mem *mem, vector<गाँठ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ध्रुव SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		पूर्णाङ्क current = 0;
		प्रति(size_t i = 0; i < mem->rd_ports.size(); i++) {
			पूर्णाङ्क width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		गाँठ_2 गाँठ = results[0];
		प्रति(size_t i = 1; i < results.size(); i++)
			गाँठ = factory.concat(गाँठ, results[i]);
		प्रत्यागम गाँठ;
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
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_मूल्य(MemContents(mem));
		गाँठ_2 गाँठ = factory.मूल्य(state);
		प्रति (size_t i = 0; i < mem->wr_ports.size(); i++) {
			ध्रुव auto &wr = mem->wr_ports[i];
			यदि (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाँठ_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			गाँठ_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			गाँठ_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			गाँठ_2 old_data = factory.memory_read(गाँठ, addr);
			गाँठ_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			गाँठ = factory.memory_write(गाँठ, addr, wr_data);
		}
		यदि (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		प्रति (size_t i = 0; i < mem->rd_ports.size(); i++) {
			ध्रुव auto &rd = mem->rd_ports[i];
			यदि (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			गाँठ_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.धकेलो_back(factory.memory_read(गाँठ, addr));
		}
		state.set_next_मूल्य(गाँठ);
		प्रत्यागम concatenate_read_results(mem, read_results);
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
			गाँठ_2 गाँठ = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), गाँठ);
		} अन्यथा यदि (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			यदि (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			गाँठ_2 q_मूल्य = factory.मूल्य(state);
			factory.suggest_name(q_मूल्य, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_मूल्य);
			state.set_next_मूल्य(enqueue(ff.sig_d));
			state.set_initial_मूल्य(ff.val_init);
		} अन्यथा {
			dict<IdString, गाँठ_2> connections;
			IdString output_name; // for the single output case
			पूर्णाङ्क n_outputs = 0;
			प्रति(auto ध्रुव &[name, sigspec] : cell->connections()) {
				यदि(driver_map.celltypes.cell_निवेश(cell->type, name) && sigspec.size() > 0)
					connections.जोड़ो({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				यदि(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, गाँठ_2>, गाँठ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			यदि(auto *nodep = std::get_if<गाँठ_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} अन्यथा {
				प्रति(auto [name, गाँठ] : std::get<dict<IdString, गाँठ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), गाँठ);
			}
		}
	}
	शून्य undriven(ध्रुव std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	शून्य check_undriven(DriveSpec ध्रुव& spec, std::string ध्रुव& name) {
		प्रति(auto ध्रुव &chunk : spec.chunks())
			यदि(chunk.is_none())
				undriven(name);
	}
सार्वजनिक:
	शून्य process_queue()
	{
		प्रति (; !queue.empty(); queue.निकालो_front()) {
			यदि(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				अनुवर्त;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			गाँठ_2 pending = graph_nodes.at(spec);

			यदि (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				गाँठ_2 गाँठ = enqueue(chunks[0]);
				प्रति(size_t i = 1; i < chunks.size(); i++)
					गाँठ = factory.concat(गाँठ, enqueue(chunks[i]));
				factory.update_pending(pending, गाँठ);
			} अन्यथा यदि (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				यदि (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					यदि (wire_chunk.is_whole()) {
						यदि (wire_chunk.wire->port_निवेश) {
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
				} अन्यथा यदि (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					यदि (port_chunk.is_whole()) {
						यदि (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							गाँठ_2 गाँठ = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, गाँठ);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} अन्यथा {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						गाँठ_2 गाँठ = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, गाँठ);
					}
				} अन्यथा यदि (chunk.is_constant()) {
					गाँठ_2 गाँठ = factory.constant(chunk.constant());
					factory.suggest_name(गाँठ, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, गाँठ);
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
    auto factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_queue();
    ir.topological_sort();
    ir.forward_buf();
    प्रत्यागम ir;
}

शून्य IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    तार्किक scc = असत्य;
    std::vector<पूर्णाङ्क> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](पूर्णाङ्क *begin, पूर्णाङ्क *end) {
        perm.जोड़ो(perm.end(), begin, end);
        यदि (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            प्रति (पूर्णाङ्क *i = begin; i != end; ++i) {
				गाँठ_2 गाँठ(_graph[*i], design);
                log("- %s = %s\n", गाँठ.name().unescape(), गाँठ.to_string());
			}
            log("\n");
            scc = सत्य;
        }
    });
	प्रति(ध्रुव auto &[name, state]: _states)
		यदि(state.has_next_मूल्य())
			toposort.process(state.next_मूल्य().id());
	प्रति(ध्रुव auto &[name, output]: _outputs)
		यदि(output.has_मूल्य())
			toposort.process(output.मूल्य().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    यदि(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

स्थिर IdString merge_name(IdString a, IdString b) {
	यदि(!a.isPublic() && b.isPublic())
		प्रत्यागम b;
	अन्यथा
		प्रत्यागम a;
}

शून्य IR::forward_buf() {
    std::vector<पूर्णाङ्क> perm, alias;
    perm.clear();

    प्रति (पूर्णाङ्क i = 0; i < _graph.size(); ++i)
    {
        auto गाँठ = _graph[i];
        यदि (गाँठ.function().fn() == Fn::buf && गाँठ.arg(0).index() < i)
        {
            पूर्णाङ्क target_index = alias[गाँठ.arg(0).index()];
            auto target_गाँठ = _graph[perm[target_index]];
			यदि(गाँठ.has_sparse_attr()) {
				यदि(target_गाँठ.has_sparse_attr()) {
					IdString id = merge_name(गाँठ.sparse_attr(), target_गाँठ.sparse_attr());
					target_गाँठ.sparse_attr() = id;
				} अन्यथा {
					IdString id = गाँठ.sparse_attr();
					target_गाँठ.sparse_attr() = id;
				}
			}
            alias.धकेलो_back(target_index);
        }
        अन्यथा
        {
            alias.धकेलो_back(GetSize(perm));
            perm.धकेलो_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
स्थिर std::string quote_fmt(ध्रुव वर्ण *fmt)
{
	std::string r;
	प्रति(ध्रुव वर्ण *p = fmt; *p != 0; p++) {
		विकल्प(*p) {
		पक्ष '\n': r += "\\n"; भङ्ग;
		पक्ष '\t': r += "\\t"; भङ्ग;
		पक्ष '"': r += "\\\""; भङ्ग;
		पक्ष '\\': r += "\\\\"; भङ्ग;
		स्वभाव: r += *p; भङ्ग;
		}
	}
	प्रत्यागम r;
}

शून्य Writer::print_impl(ध्रुव वर्ण *fmt, vector<std::function<शून्य()>> &fns)
{
	size_t next_index = 0;
	प्रति(ध्रुव वर्ण *p = fmt; *p != 0; p++)
		विकल्प(*p) {
		पक्ष '{':
			यदि(*++p == '{') {
				*os << '{';
			} अन्यथा {
				वर्ण *pe;
				size_t index = strtoul(p, &pe, 10);
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
			भङ्ग;
		पक्ष '}':
			p++;
			यदि(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			भङ्ग;
		स्वभाव:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
