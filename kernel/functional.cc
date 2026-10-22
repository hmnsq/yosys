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

#അന്തര്ഭാവ "kernel/functional.h"
#അന്തര്ഭാവ "kernel/topo_scc.h"
#അന്തര്ഭാവ "ff.h"
#അന്തര്ഭാവ "ffinit.h"
#അന്തര്ഭാവ <deque>

YOSYS_NAMESPACE_BEGIN
നാമാകാശ Functional {

സ്ഥിരം അക്ഷരം *fn_to_string(Fn fn) {
	തിരഞ്ഞെടുപ്പ്(fn) {
	അവസ്ഥ Fn::invalid: തിരികെ "invalid";
	അവസ്ഥ Fn::buf: തിരികെ "buf";
	അവസ്ഥ Fn::slice: തിരികെ "slice";
	അവസ്ഥ Fn::zero_extend: തിരികെ "zero_extend";
	അവസ്ഥ Fn::sign_extend: തിരികെ "sign_extend";
	അവസ്ഥ Fn::concat: തിരികെ "concat";
	അവസ്ഥ Fn::add: തിരികെ "add";
	അവസ്ഥ Fn::sub: തിരികെ "sub";
	അവസ്ഥ Fn::mul: തിരികെ "mul";
	അവസ്ഥ Fn::unsigned_div: തിരികെ "unsigned_div";
	അവസ്ഥ Fn::unsigned_mod: തിരികെ "unsigned_mod";
	അവസ്ഥ Fn::bitwise_and: തിരികെ "bitwise_and";
	അവസ്ഥ Fn::bitwise_or: തിരികെ "bitwise_or";
	അവസ്ഥ Fn::bitwise_xor: തിരികെ "bitwise_xor";
	അവസ്ഥ Fn::bitwise_not: തിരികെ "bitwise_not";
	അവസ്ഥ Fn::reduce_and: തിരികെ "reduce_and";
	അവസ്ഥ Fn::reduce_or: തിരികെ "reduce_or";
	അവസ്ഥ Fn::reduce_xor: തിരികെ "reduce_xor";
	അവസ്ഥ Fn::unary_minus: തിരികെ "unary_minus";
	അവസ്ഥ Fn::equal: തിരികെ "equal";
	അവസ്ഥ Fn::not_equal: തിരികെ "not_equal";
	അവസ്ഥ Fn::signed_greater_than: തിരികെ "signed_greater_than";
	അവസ്ഥ Fn::signed_greater_equal: തിരികെ "signed_greater_equal";
	അവസ്ഥ Fn::unsigned_greater_than: തിരികെ "unsigned_greater_than";
	അവസ്ഥ Fn::unsigned_greater_equal: തിരികെ "unsigned_greater_equal";
	അവസ്ഥ Fn::logical_shift_ഇടത്: തിരികെ "logical_shift_left";
	അവസ്ഥ Fn::logical_shift_വലത്: തിരികെ "logical_shift_right";
	അവസ്ഥ Fn::arithmetic_shift_വലത്: തിരികെ "arithmetic_shift_right";
	അവസ്ഥ Fn::mux: തിരികെ "mux";
	അവസ്ഥ Fn::constant: തിരികെ "constant";
	അവസ്ഥ Fn::നിവേശം: തിരികെ "input";
	അവസ്ഥ Fn::state: തിരികെ "state";
	അവസ്ഥ Fn::memory_read: തിരികെ "memory_read";
	അവസ്ഥ Fn::memory_write: തിരികെ "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (പൂർണ്ണസംഖ്യ)fn);
}

vector<IRInput സ്ഥിരം*> IR::inputs(IdString kind) സ്ഥിരം {
	vector<IRInput സ്ഥിരം*> ret;
	വേണ്ടി (സ്ഥിരം auto &[name, നിവേശം] : _inputs)
		എങ്കിൽ(നിവേശം.kind == kind)
			ret.തള്ളുക_back(&നിവേശം);
	തിരികെ ret;
}

vector<IROutput സ്ഥിരം*> IR::outputs(IdString kind) സ്ഥിരം {
	vector<IROutput സ്ഥിരം*> ret;
	വേണ്ടി (സ്ഥിരം auto &[name, output] : _outputs)
		എങ്കിൽ(output.kind == kind)
			ret.തള്ളുക_back(&output);
	തിരികെ ret;
}

vector<IRState സ്ഥിരം*> IR::states(IdString kind) സ്ഥിരം {
	vector<IRState സ്ഥിരം*> ret;
	വേണ്ടി (സ്ഥിരം auto &[name, state] : _states)
		എങ്കിൽ(state.kind == kind)
			ret.തള്ളുക_back(&state);
	തിരികെ ret;
}

vector<IRInput സ്ഥിരം*> IR::all_inputs() സ്ഥിരം {
	vector<IRInput സ്ഥിരം*> ret;
	വേണ്ടി (സ്ഥിരം auto &[name, നിവേശം] : _inputs)
		ret.തള്ളുക_back(&നിവേശം);
	തിരികെ ret;
}

vector<IROutput സ്ഥിരം*> IR::all_outputs() സ്ഥിരം {
	vector<IROutput സ്ഥിരം*> ret;
	വേണ്ടി (സ്ഥിരം auto &[name, output] : _outputs)
		ret.തള്ളുക_back(&output);
	തിരികെ ret;
}

vector<IRState സ്ഥിരം*> IR::all_states() സ്ഥിരം {
	vector<IRState സ്ഥിരം*> ret;
	വേണ്ടി (സ്ഥിരം auto &[name, state] : _states)
		ret.തള്ളുക_back(&state);
	തിരികെ ret;
}

ഘടന PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(കണ്ണി_2)> np;
	PrintVisitor(std::function<std::string(കണ്ണി_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(കണ്ണി_2, കണ്ണി_2 a, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ out_width) override { തിരികെ "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(കണ്ണി_2, കണ്ണി_2 a, പൂർണ്ണസംഖ്യ out_width) override { തിരികെ "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(കണ്ണി_2, കണ്ണി_2 a, പൂർണ്ണസംഖ്യ out_width) override { തിരികെ "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(കണ്ണി_2, RTLIL::Const സ്ഥിരം& മൂല്യം) override { തിരികെ "constant(" + മൂല്യം.as_string() + ")"; }
	std::string നിവേശം(കണ്ണി_2 self, IdString name, IdString kind) override { തിരികെ "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(കണ്ണി_2 self, IdString name, IdString kind) override { തിരികെ "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(കണ്ണി_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		വേണ്ടി(size_t i = 0; i < self.arg_എണ്ണം(); i++) {
			എങ്കിൽ(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		തിരികെ ret;
	}
};

std::string കണ്ണി_2::to_string()
{
	തിരികെ to_string([](കണ്ണി_2 n) { തിരികെ n.name().unescape(); });
}

std::string കണ്ണി_2::to_string(std::function<std::string(കണ്ണി_2)> np)
{
	തിരികെ visit(PrintVisitor(np));
}

വർഗ്ഗം CellSimplifier {
	Factory &factory;
	കണ്ണി_2 sign(കണ്ണി_2 a) {
		തിരികെ factory.slice(a, a.width() - 1, 1);
	}
	കണ്ണി_2 neg_if(കണ്ണി_2 a, കണ്ണി_2 s) {
		തിരികെ factory.mux(a, factory.unary_minus(a), s);
	}
	കണ്ണി_2 abs(കണ്ണി_2 a) {
		തിരികെ neg_if(a, sign(a));
	}
	കണ്ണി_2 handle_shift(കണ്ണി_2 a, കണ്ണി_2 b, തർക്കം is_വലത്, തർക്കം is_signed) {
		// to prevent new_width == 0, we handle this case separately
		എങ്കിൽ(a.width() == 1) {
			എങ്കിൽ(!is_signed)
				തിരികെ factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			അല്ലെങ്കിൽ
				തിരികെ a;
		}
		പൂർണ്ണസംഖ്യ new_width = ceil_log2(a.width());
		കണ്ണി_2 b_truncated = factory.extend(b, new_width, അസത്യം);
		കണ്ണി_2 y =
			!is_വലത് ? factory.logical_shift_ഇടത്(a, b_truncated) :
			!is_signed ? factory.logical_shift_വലത്(a, b_truncated) :
			factory.arithmetic_shift_വലത്(a, b_truncated);
		എങ്കിൽ(b.width() <= new_width)
			തിരികെ y;
		കണ്ണി_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		കണ്ണി_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), സത്യം) : factory.constant(RTLIL::Const(State::S0, a.width()));
		തിരികെ factory.mux(y, y_if_overflow, overflow);
	}
പൊതു:
	കണ്ണി_2 logical_shift_ഇടത്(കണ്ണി_2 a, കണ്ണി_2 b) { തിരികെ handle_shift(a, b, അസത്യം, അസത്യം); }
	കണ്ണി_2 logical_shift_വലത്(കണ്ണി_2 a, കണ്ണി_2 b) { തിരികെ handle_shift(a, b, സത്യം, അസത്യം); }
	കണ്ണി_2 arithmetic_shift_വലത്(കണ്ണി_2 a, കണ്ണി_2 b) { തിരികെ handle_shift(a, b, സത്യം, സത്യം); }
	കണ്ണി_2 bitwise_mux(കണ്ണി_2 a, കണ്ണി_2 b, കണ്ണി_2 s) {
		കണ്ണി_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		കണ്ണി_2 bb = factory.bitwise_and(b, s);
		തിരികെ factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
സ്വകാര്യ:
	കണ്ണി_2 handle_pow(കണ്ണി_2 a0, കണ്ണി_2 b, പൂർണ്ണസംഖ്യ y_width, തർക്കം is_signed) {
		കണ്ണി_2 a = factory.extend(a0, y_width, is_signed);
		കണ്ണി_2 r = factory.constant(Const(1, y_width));
		വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < b.width(); i++) {
			കണ്ണി_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		എങ്കിൽ (is_signed) {
			കണ്ണി_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			കണ്ണി_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		തിരികെ r;
	}
	കണ്ണി_2 handle_bmux(കണ്ണി_2 a, കണ്ണി_2 s, പൂർണ്ണസംഖ്യ a_offset, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ sn) {
		എങ്കിൽ(sn < 1)
			തിരികെ factory.slice(a, a_offset, width);
		അല്ലെങ്കിൽ {
			കണ്ണി_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			കണ്ണി_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			തിരികെ factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	കണ്ണി_2 handle_pmux(കണ്ണി_2 a, കണ്ണി_2 b, കണ്ണി_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		കണ്ണി_2 y = a;
		വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		തിരികെ y;
	}
	dict<IdString, കണ്ണി_2> handle_fa(കണ്ണി_2 a, കണ്ണി_2 b, കണ്ണി_2 c) {
		കണ്ണി_2 t1 = factory.bitwise_xor(a, b);
		കണ്ണി_2 t2 = factory.bitwise_and(a, b);
		കണ്ണി_2 t3 = factory.bitwise_and(c, t1);
		കണ്ണി_2 y = factory.bitwise_xor(c, t1);
		കണ്ണി_2 x = factory.bitwise_or(t2, t3);
		തിരികെ {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, കണ്ണി_2> handle_alu(കണ്ണി_2 a_in, കണ്ണി_2 b_in, പൂർണ്ണസംഖ്യ y_width, തർക്കം is_signed, കണ്ണി_2 ci, കണ്ണി_2 bi) {
		കണ്ണി_2 a = factory.extend(a_in, y_width, is_signed);
		കണ്ണി_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		കണ്ണി_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		കണ്ണി_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		കണ്ണി_2 a_extra = factory.extend(a, y_width + 1, അസത്യം);
		കണ്ണി_2 b_extra = factory.extend(b, y_width + 1, അസത്യം);
		കണ്ണി_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, അസത്യം));
		കണ്ണി_2 y = factory.slice(y_extra, 0, y_width);
		കണ്ണി_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		കണ്ണി_2 co = factory.slice(carries, 1, y_width);
		തിരികെ {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	കണ്ണി_2 handle_lcu(കണ്ണി_2 p, കണ്ണി_2 g, കണ്ണി_2 ci) {
		തിരികെ handle_alu(g, factory.bitwise_or(p, g), g.width(), അസത്യം, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
പൊതു:
	std::variant<dict<IdString, കണ്ണി_2>, കണ്ണി_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, കണ്ണി_2> inputs)
	{
		പൂർണ്ണസംഖ്യ a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		പൂർണ്ണസംഖ്യ b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		പൂർണ്ണസംഖ്യ y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		തർക്കം a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		തർക്കം b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		എങ്കിൽ(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			തർക്കം is_signed = a_signed && b_signed;
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			കണ്ണി_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			എങ്കിൽ(cellType == ID($add))
				തിരികെ factory.add(a, b);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($sub))
				തിരികെ factory.sub(a, b);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($mul))
				തിരികെ factory.mul(a, b);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($and))
				തിരികെ factory.bitwise_and(a, b);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($or))
				തിരികെ factory.bitwise_or(a, b);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($xor))
				തിരികെ factory.bitwise_xor(a, b);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($xnor))
				തിരികെ factory.bitwise_not(factory.bitwise_xor(a, b));
			അല്ലെങ്കിൽ
				log_abort();
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			തർക്കം is_signed = a_signed && b_signed;
			പൂർണ്ണസംഖ്യ width = max(a_width, b_width);
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			കണ്ണി_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			എങ്കിൽ(cellType.in(ID($eq), ID($eqx)))
				തിരികെ factory.extend(factory.equal(a, b), y_width, അസത്യം);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($ne), ID($nex)))
				തിരികെ factory.extend(factory.not_equal(a, b), y_width, അസത്യം);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($lt))
				തിരികെ factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, അസത്യം);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($le))
				തിരികെ factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, അസത്യം);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($gt))
				തിരികെ factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, അസത്യം);
			അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($ge))
				തിരികെ factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, അസത്യം);
			അല്ലെങ്കിൽ
				log_abort();
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($logic_or), ID($logic_and))){
			കണ്ണി_2 a = factory.reduce_or(inputs.at(ID(A)));
			കണ്ണി_2 b = factory.reduce_or(inputs.at(ID(B)));
			കണ്ണി_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			തിരികെ factory.extend(y, y_width, അസത്യം);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($not)){
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			തിരികെ factory.bitwise_not(a);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($pos)){
			തിരികെ factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($barrier)){
			തിരികെ inputs.at(ID(A));
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($neg)){
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			തിരികെ factory.unary_minus(a);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($logic_not)){
			കണ്ണി_2 a = factory.reduce_or(inputs.at(ID(A)));
			കണ്ണി_2 y = factory.bitwise_not(a);
			തിരികെ factory.extend(y, y_width, അസത്യം);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($reduce_or), ID($reduce_bool))){
			കണ്ണി_2 a = factory.reduce_or(inputs.at(ID(A)));
			തിരികെ factory.extend(a, y_width, അസത്യം);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($reduce_and)){
			കണ്ണി_2 a = factory.reduce_and(inputs.at(ID(A)));
			തിരികെ factory.extend(a, y_width, അസത്യം);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			കണ്ണി_2 a = factory.reduce_xor(inputs.at(ID(A)));
			കണ്ണി_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			തിരികെ factory.extend(y, y_width, അസത്യം);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($shl) || cellType == ID($sshl)){
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			കണ്ണി_2 b = inputs.at(ID(B));
			തിരികെ logical_shift_ഇടത്(a, b);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($shr) || cellType == ID($sshr)){
			പൂർണ്ണസംഖ്യ width = max(a_width, y_width);
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			കണ്ണി_2 b = inputs.at(ID(B));
			കണ്ണി_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_വലത്(a, b) :
				logical_shift_വലത്(a, b);
			തിരികെ factory.extend(y, y_width, a_signed);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($shiftx) || cellType == ID($shift)){
			പൂർണ്ണസംഖ്യ width = max(a_width, y_width);
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			കണ്ണി_2 b = inputs.at(ID(B));
			കണ്ണി_2 shr = logical_shift_വലത്(a, b);
			എങ്കിൽ(b_signed) {
				കണ്ണി_2 shl = logical_shift_ഇടത്(a, factory.unary_minus(b));
				കണ്ണി_2 y = factory.mux(shr, shl, sign(b));
				തിരികെ factory.extend(y, y_width, അസത്യം);
			} അല്ലെങ്കിൽ {
				തിരികെ factory.extend(shr, y_width, അസത്യം);
			}
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($mux)){
			തിരികെ factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($pmux)){
			തിരികെ handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($concat)){
			കണ്ണി_2 a = inputs.at(ID(A));
			കണ്ണി_2 b = inputs.at(ID(B));
			തിരികെ factory.concat(a, b);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($slice)){
			പൂർണ്ണസംഖ്യ offset = parameters.at(ID(OFFSET)).as_int();
			കണ്ണി_2 a = inputs.at(ID(A));
			തിരികെ factory.slice(a, offset, y_width);
		}അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			പൂർണ്ണസംഖ്യ width = max(a_width, b_width);
			തർക്കം is_signed = a_signed && b_signed;
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			കണ്ണി_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			എങ്കിൽ(is_signed) {
				എങ്കിൽ(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					കണ്ണി_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					കണ്ണി_2 out_sign = factory.not_equal(sign(a), sign(b));
					തിരികെ neg_if(factory.extend(abs_y, y_width, അസത്യം), out_sign);
				} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					കണ്ണി_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					തിരികെ neg_if(factory.extend(abs_y, y_width, അസത്യം), sign(a));
				} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					കണ്ണി_2 b_sign = sign(b);
					കണ്ണി_2 a1 = neg_if(a, b_sign);
					കണ്ണി_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					കണ്ണി_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					കണ്ണി_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					കണ്ണി_2 y1 = factory.unsigned_div(a2, b1);
					കണ്ണി_2 y2 = factory.extend(y1, y_width, അസത്യം);
					തിരികെ factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					കണ്ണി_2 abs_b = abs(b);
					കണ്ണി_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					കണ്ണി_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					കണ്ണി_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					കണ്ണി_2 y = neg_if(y_flipped, sign(b));
					തിരികെ factory.extend(y, y_width, സത്യം);
				} അല്ലെങ്കിൽ
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} അല്ലെങ്കിൽ {
				എങ്കിൽ(cellType.in(ID($mod), ID($modfloor)))
					തിരികെ factory.extend(factory.unsigned_mod(a, b), y_width, അസത്യം);
				അല്ലെങ്കിൽ
					തിരികെ factory.extend(factory.unsigned_div(a, b), y_width, അസത്യം);
			}
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($pow)) {
			തിരികെ handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} അല്ലെങ്കിൽ എങ്കിൽ (cellType == ID($lut)) {
			പൂർണ്ണസംഖ്യ width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			തിരികെ handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} അല്ലെങ്കിൽ എങ്കിൽ (cellType == ID($bwmux)) {
			കണ്ണി_2 a = inputs.at(ID(A));
			കണ്ണി_2 b = inputs.at(ID(B));
			കണ്ണി_2 s = inputs.at(ID(S));
			തിരികെ factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} അല്ലെങ്കിൽ എങ്കിൽ (cellType == ID($bweqx)) {
			കണ്ണി_2 a = inputs.at(ID(A));
			കണ്ണി_2 b = inputs.at(ID(B));
			തിരികെ factory.bitwise_not(factory.bitwise_xor(a, b));
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($bmux)) {
			പൂർണ്ണസംഖ്യ width = parameters.at(ID(WIDTH)).as_int();
			പൂർണ്ണസംഖ്യ s_width = parameters.at(ID(S_WIDTH)).as_int();
			തിരികെ handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($demux)) {
			പൂർണ്ണസംഖ്യ width = parameters.at(ID(WIDTH)).as_int();
			പൂർണ്ണസംഖ്യ s_width = parameters.at(ID(S_WIDTH)).as_int();
			പൂർണ്ണസംഖ്യ y_width = width << s_width;
			പൂർണ്ണസംഖ്യ b_width = ceil_log2(y_width);
			കണ്ണി_2 a = factory.extend(inputs.at(ID(A)), y_width, അസത്യം);
			കണ്ണി_2 s = factory.extend(inputs.at(ID(S)), b_width, അസത്യം);
			കണ്ണി_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			തിരികെ factory.logical_shift_ഇടത്(a, b);
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($fa)) {
			തിരികെ handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($lcu)) {
			തിരികെ handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($alu)) {
			തിരികെ handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($പ്രതിജ്ഞാ), ID($assume), ID($live), ID($fair), ID($cover))) {
			കണ്ണി_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_മൂല്യം(a);
			തിരികെ {};
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			പൂർണ്ണസംഖ്യ width = parameters.at(ID(WIDTH)).as_int();
			auto &നിവേശം = factory.add_നിവേശം(cellName, cellType, Sort(width));
			തിരികെ factory.മൂല്യം(നിവേശം);
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($initstate)) {
			എങ്കിൽ(factory.ir().has_state(ID($initstate), ID($state)))
				തിരികെ factory.മൂല്യം(factory.ir().state(ID($initstate)));
			അല്ലെങ്കിൽ {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_മൂല്യം(RTLIL::Const(State::S1, 1));
				state.set_next_മൂല്യം(factory.constant(RTLIL::Const(State::S0, 1)));
				തിരികെ factory.മൂല്യം(state);
			}
		} അല്ലെങ്കിൽ എങ്കിൽ(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} അല്ലെങ്കിൽ {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

വർഗ്ഗം FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, കണ്ണി_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, കണ്ണി_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	കണ്ണി_2 enqueue(DriveSpec സ്ഥിരം &spec)
	{
		auto it = graph_nodes.find(spec);
		എങ്കിൽ(it == graph_nodes.end()){
			auto കണ്ണി = factory.create_pending(spec.size());
			graph_nodes.തിരുകുക({spec, കണ്ണി});
			queue.emplace_back(spec);
			തിരികെ കണ്ണി;
		}അല്ലെങ്കിൽ
			തിരികെ it->second;
	}
	കണ്ണി_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		എങ്കിൽ(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<കണ്ണി_2> rv;
			വേണ്ടി(auto സ്ഥിരം &[name, sigspec] : cell->connections())
				എങ്കിൽ(driver_map.celltypes.cell_output(cell->type, name)) {
					auto കണ്ണി = factory.create_pending(sigspec.size());
					factory.suggest_name(കണ്ണി, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, കണ്ണി);
					എങ്കിൽ(name == port_name)
						rv = കണ്ണി;
				}
			തിരികെ *rv;
		} അല്ലെങ്കിൽ
			തിരികെ it->second;
	}
പൊതു:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		വേണ്ടി (auto cell : module->cells()) {
			എങ്കിൽ (cell->type.in(ID($പ്രതിജ്ഞാ), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		വേണ്ടി (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			എങ്കിൽ (wire && wire->port_നിവേശം) {
				factory.add_നിവേശം(wire->name, ID($നിവേശം), Sort(wire->width));
			}
			എങ്കിൽ (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_മൂല്യം(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		വേണ്ടി (auto &mem : memories_vector) {
			എങ്കിൽ (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
സ്വകാര്യ:
	കണ്ണി_2 concatenate_read_results(Mem *mem, vector<കണ്ണി_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		സ്ഥിരം SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		പൂർണ്ണസംഖ്യ current = 0;
		വേണ്ടി(size_t i = 0; i < mem->rd_ports.size(); i++) {
			പൂർണ്ണസംഖ്യ width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		കണ്ണി_2 കണ്ണി = results[0];
		വേണ്ടി(size_t i = 1; i < results.size(); i++)
			കണ്ണി = factory.concat(കണ്ണി, results[i]);
		തിരികെ കണ്ണി;
	}
	കണ്ണി_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<കണ്ണി_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_മൂല്യം(MemContents(mem));
		കണ്ണി_2 കണ്ണി = factory.മൂല്യം(state);
		വേണ്ടി (size_t i = 0; i < mem->wr_ports.size(); i++) {
			സ്ഥിരം auto &wr = mem->wr_ports[i];
			എങ്കിൽ (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			കണ്ണി_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			കണ്ണി_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			കണ്ണി_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			കണ്ണി_2 old_data = factory.memory_read(കണ്ണി, addr);
			കണ്ണി_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			കണ്ണി = factory.memory_write(കണ്ണി, addr, wr_data);
		}
		എങ്കിൽ (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		വേണ്ടി (size_t i = 0; i < mem->rd_ports.size(); i++) {
			സ്ഥിരം auto &rd = mem->rd_ports[i];
			എങ്കിൽ (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			കണ്ണി_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.തള്ളുക_back(factory.memory_read(കണ്ണി, addr));
		}
		state.set_next_മൂല്യം(കണ്ണി);
		തിരികെ concatenate_read_results(mem, read_results);
	}
	ശൂന്യം process_cell(Cell *cell)
	{
		എങ്കിൽ (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			എങ്കിൽ (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			കണ്ണി_2 കണ്ണി = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), കണ്ണി);
		} അല്ലെങ്കിൽ എങ്കിൽ (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			എങ്കിൽ (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			കണ്ണി_2 q_മൂല്യം = factory.മൂല്യം(state);
			factory.suggest_name(q_മൂല്യം, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_മൂല്യം);
			state.set_next_മൂല്യം(enqueue(ff.sig_d));
			state.set_initial_മൂല്യം(ff.val_init);
		} അല്ലെങ്കിൽ {
			dict<IdString, കണ്ണി_2> connections;
			IdString output_name; // for the single output case
			പൂർണ്ണസംഖ്യ n_outputs = 0;
			വേണ്ടി(auto സ്ഥിരം &[name, sigspec] : cell->connections()) {
				എങ്കിൽ(driver_map.celltypes.cell_നിവേശം(cell->type, name) && sigspec.size() > 0)
					connections.തിരുകുക({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				എങ്കിൽ(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, കണ്ണി_2>, കണ്ണി_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			എങ്കിൽ(auto *nodep = std::get_if<കണ്ണി_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} അല്ലെങ്കിൽ {
				വേണ്ടി(auto [name, കണ്ണി] : std::get<dict<IdString, കണ്ണി_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), കണ്ണി);
			}
		}
	}
	ശൂന്യം undriven(സ്ഥിരം std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	ശൂന്യം check_undriven(DriveSpec സ്ഥിരം& spec, std::string സ്ഥിരം& name) {
		വേണ്ടി(auto സ്ഥിരം &chunk : spec.chunks())
			എങ്കിൽ(chunk.is_none())
				undriven(name);
	}
പൊതു:
	ശൂന്യം process_queue()
	{
		വേണ്ടി (; !queue.empty(); queue.എടുക്കുക_front()) {
			എങ്കിൽ(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				തുടരുക;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			കണ്ണി_2 pending = graph_nodes.at(spec);

			എങ്കിൽ (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				കണ്ണി_2 കണ്ണി = enqueue(chunks[0]);
				വേണ്ടി(size_t i = 1; i < chunks.size(); i++)
					കണ്ണി = factory.concat(കണ്ണി, enqueue(chunks[i]));
				factory.update_pending(pending, കണ്ണി);
			} അല്ലെങ്കിൽ എങ്കിൽ (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				എങ്കിൽ (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					എങ്കിൽ (wire_chunk.is_whole()) {
						എങ്കിൽ (wire_chunk.wire->port_നിവേശം) {
							കണ്ണി_2 കണ്ണി = factory.മൂല്യം(factory.ir().നിവേശം(wire_chunk.wire->name));
							factory.suggest_name(കണ്ണി, wire_chunk.wire->name);
							factory.update_pending(pending, കണ്ണി);
						} അല്ലെങ്കിൽ {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							കണ്ണി_2 കണ്ണി = enqueue(driver);
							factory.suggest_name(കണ്ണി, wire_chunk.wire->name);
							factory.update_pending(pending, കണ്ണി);
						}
					} അല്ലെങ്കിൽ {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						കണ്ണി_2 കണ്ണി = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, കണ്ണി);
					}
				} അല്ലെങ്കിൽ എങ്കിൽ (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					എങ്കിൽ (port_chunk.is_whole()) {
						എങ്കിൽ (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							കണ്ണി_2 കണ്ണി = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, കണ്ണി);
						} അല്ലെങ്കിൽ {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} അല്ലെങ്കിൽ {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						കണ്ണി_2 കണ്ണി = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, കണ്ണി);
					}
				} അല്ലെങ്കിൽ എങ്കിൽ (chunk.is_constant()) {
					കണ്ണി_2 കണ്ണി = factory.constant(chunk.constant());
					factory.suggest_name(കണ്ണി, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, കണ്ണി);
				} അല്ലെങ്കിൽ എങ്കിൽ (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} അല്ലെങ്കിൽ എങ്കിൽ (chunk.is_none()) {
					undriven(log_signal(chunk));
				} അല്ലെങ്കിൽ {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} അല്ലെങ്കിൽ {
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
    തിരികെ ir;
}

ശൂന്യം IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    തർക്കം scc = അസത്യം;
    std::vector<പൂർണ്ണസംഖ്യ> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](പൂർണ്ണസംഖ്യ *begin, പൂർണ്ണസംഖ്യ *end) {
        perm.തിരുകുക(perm.end(), begin, end);
        എങ്കിൽ (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            വേണ്ടി (പൂർണ്ണസംഖ്യ *i = begin; i != end; ++i) {
				കണ്ണി_2 കണ്ണി(_graph[*i], design);
                log("- %s = %s\n", കണ്ണി.name().unescape(), കണ്ണി.to_string());
			}
            log("\n");
            scc = സത്യം;
        }
    });
	വേണ്ടി(സ്ഥിരം auto &[name, state]: _states)
		എങ്കിൽ(state.has_next_മൂല്യം())
			toposort.process(state.next_മൂല്യം().id());
	വേണ്ടി(സ്ഥിരം auto &[name, output]: _outputs)
		എങ്കിൽ(output.has_മൂല്യം())
			toposort.process(output.മൂല്യം().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    എങ്കിൽ(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

സ്ഥിതം IdString merge_name(IdString a, IdString b) {
	എങ്കിൽ(!a.isPublic() && b.isPublic())
		തിരികെ b;
	അല്ലെങ്കിൽ
		തിരികെ a;
}

ശൂന്യം IR::forward_buf() {
    std::vector<പൂർണ്ണസംഖ്യ> perm, alias;
    perm.clear();

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < _graph.size(); ++i)
    {
        auto കണ്ണി = _graph[i];
        എങ്കിൽ (കണ്ണി.function().fn() == Fn::buf && കണ്ണി.arg(0).index() < i)
        {
            പൂർണ്ണസംഖ്യ target_index = alias[കണ്ണി.arg(0).index()];
            auto target_കണ്ണി = _graph[perm[target_index]];
			എങ്കിൽ(കണ്ണി.has_sparse_attr()) {
				എങ്കിൽ(target_കണ്ണി.has_sparse_attr()) {
					IdString id = merge_name(കണ്ണി.sparse_attr(), target_കണ്ണി.sparse_attr());
					target_കണ്ണി.sparse_attr() = id;
				} അല്ലെങ്കിൽ {
					IdString id = കണ്ണി.sparse_attr();
					target_കണ്ണി.sparse_attr() = id;
				}
			}
            alias.തള്ളുക_back(target_index);
        }
        അല്ലെങ്കിൽ
        {
            alias.തള്ളുക_back(GetSize(perm));
            perm.തള്ളുക_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
സ്ഥിതം std::string quote_fmt(സ്ഥിരം അക്ഷരം *fmt)
{
	std::string r;
	വേണ്ടി(സ്ഥിരം അക്ഷരം *p = fmt; *p != 0; p++) {
		തിരഞ്ഞെടുപ്പ്(*p) {
		അവസ്ഥ '\n': r += "\\n"; നിർത്തുക;
		അവസ്ഥ '\t': r += "\\t"; നിർത്തുക;
		അവസ്ഥ '"': r += "\\\""; നിർത്തുക;
		അവസ്ഥ '\\': r += "\\\\"; നിർത്തുക;
		സ്ഥിരസ്ഥിതി: r += *p; നിർത്തുക;
		}
	}
	തിരികെ r;
}

ശൂന്യം Writer::print_impl(സ്ഥിരം അക്ഷരം *fmt, vector<std::function<ശൂന്യം()>> &fns)
{
	size_t next_index = 0;
	വേണ്ടി(സ്ഥിരം അക്ഷരം *p = fmt; *p != 0; p++)
		തിരഞ്ഞെടുപ്പ്(*p) {
		അവസ്ഥ '{':
			എങ്കിൽ(*++p == '{') {
				*os << '{';
			} അല്ലെങ്കിൽ {
				അക്ഷരം *pe;
				size_t index = strtoul(p, &pe, 10);
				എങ്കിൽ(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				എങ്കിൽ(p == pe)
					index = next_index;
				അല്ലെങ്കിൽ
					p = pe;
				എങ്കിൽ(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			നിർത്തുക;
		അവസ്ഥ '}':
			p++;
			എങ്കിൽ(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
