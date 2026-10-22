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

ध्रुव वर्ण *fn_गन्तव्यम्_string(Fn fn) {
	विकल्प(fn) {
	पक्ष Fn::invalid: प्रत्यागम "invalid";
	पक्ष Fn::buf: प्रत्यागम "buf";
	पक्ष Fn::खण्डिका: प्रत्यागम "slice";
	पक्ष Fn::zero_extend: प्रत्यागम "zero_extend";
	पक्ष Fn::sign_extend: प्रत्यागम "sign_extend";
	पक्ष Fn::concat: प्रत्यागम "concat";
	पक्ष Fn::सङ्कलय: प्रत्यागम "add";
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
	पक्ष Fn::logical_सरणम्_वामः: प्रत्यागम "logical_shift_left";
	पक्ष Fn::logical_सरणम्_दक्षिणः: प्रत्यागम "logical_shift_right";
	पक्ष Fn::arithmetic_सरणम्_दक्षिणः: प्रत्यागम "arithmetic_shift_right";
	पक्ष Fn::mux: प्रत्यागम "mux";
	पक्ष Fn::constant: प्रत्यागम "constant";
	पक्ष Fn::निवेशः: प्रत्यागम "input";
	पक्ष Fn::state: प्रत्यागम "state";
	पक्ष Fn::स्मृतिः_read: प्रत्यागम "memory_read";
	पक्ष Fn::स्मृतिः_write: प्रत्यागम "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (पूर्णाङ्क)fn);
}

सदिशम्<IRInput ध्रुव*> IR::inputs(IdString विधा) ध्रुव {
	सदिशम्<IRInput ध्रुव*> ret;
	प्रति (ध्रुव स्वचालित &[name, निवेशः] : _inputs)
		यदि(निवेशः.विधा == विधा)
			ret.प्रक्षिप_पृष्ठभागः(&निवेशः);
	प्रत्यागम ret;
}

सदिशम्<IROutput ध्रुव*> IR::outputs(IdString विधा) ध्रुव {
	सदिशम्<IROutput ध्रुव*> ret;
	प्रति (ध्रुव स्वचालित &[name, output] : _outputs)
		यदि(output.विधा == विधा)
			ret.प्रक्षिप_पृष्ठभागः(&output);
	प्रत्यागम ret;
}

सदिशम्<IRState ध्रुव*> IR::अवस्थाः(IdString विधा) ध्रुव {
	सदिशम्<IRState ध्रुव*> ret;
	प्रति (ध्रुव स्वचालित &[name, state] : अवस्थाः_2)
		यदि(state.विधा == विधा)
			ret.प्रक्षिप_पृष्ठभागः(&state);
	प्रत्यागम ret;
}

सदिशम्<IRInput ध्रुव*> IR::all_inputs() ध्रुव {
	सदिशम्<IRInput ध्रुव*> ret;
	प्रति (ध्रुव स्वचालित &[name, निवेशः] : _inputs)
		ret.प्रक्षिप_पृष्ठभागः(&निवेशः);
	प्रत्यागम ret;
}

सदिशम्<IROutput ध्रुव*> IR::all_outputs() ध्रुव {
	सदिशम्<IROutput ध्रुव*> ret;
	प्रति (ध्रुव स्वचालित &[name, output] : _outputs)
		ret.प्रक्षिप_पृष्ठभागः(&output);
	प्रत्यागम ret;
}

सदिशम्<IRState ध्रुव*> IR::all_अवस्थाः() ध्रुव {
	सदिशम्<IRState ध्रुव*> ret;
	प्रति (ध्रुव स्वचालित &[name, state] : अवस्थाः_2)
		ret.प्रक्षिप_पृष्ठभागः(&state);
	प्रत्यागम ret;
}

संरचना PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(ग्रन्थिः_2)> np;
	PrintVisitor(std::function<std::string(ग्रन्थिः_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string खण्डिका(ग्रन्थिः_2, ग्रन्थिः_2 a, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क बहिर्गमनम्_विस्तारः) override { प्रत्यागम "slice(" + np(a) + ", " + std::गन्तव्यम्_string(विस्थापनम्) + ", " + std::गन्तव्यम्_string(बहिर्गमनम्_विस्तारः) + ")"; }
	std::string zero_extend(ग्रन्थिः_2, ग्रन्थिः_2 a, पूर्णाङ्क बहिर्गमनम्_विस्तारः) override { प्रत्यागम "zero_extend(" + np(a) + ", " + std::गन्तव्यम्_string(बहिर्गमनम्_विस्तारः) + ")"; }
	std::string sign_extend(ग्रन्थिः_2, ग्रन्थिः_2 a, पूर्णाङ्क बहिर्गमनम्_विस्तारः) override { प्रत्यागम "sign_extend(" + np(a) + ", " + std::गन्तव्यम्_string(बहिर्गमनम्_विस्तारः) + ")"; }
	std::string constant(ग्रन्थिः_2, RTLIL::Const ध्रुव& मानम्) override { प्रत्यागम "constant(" + मानम्.as_string() + ")"; }
	std::string निवेशः(ग्रन्थिः_2 self, IdString name, IdString विधा) override { प्रत्यागम "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(विधा) + ")"; }
	std::string state(ग्रन्थिः_2 self, IdString name, IdString विधा) override { प्रत्यागम "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(विधा) + ")"; }
	std::string default_handler(ग्रन्थिः_2 self) override {
		std::string ret = fn_गन्तव्यम्_string(self.fn());
		ret += "(";
		प्रति(आकार_t i = 0; i < self.arg_गणनम्(); i++) {
			यदि(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		प्रत्यागम ret;
	}
};

std::string ग्रन्थिः_2::गन्तव्यम्_string()
{
	प्रत्यागम गन्तव्यम्_string([](ग्रन्थिः_2 n) { प्रत्यागम n.name().unescape(); });
}

std::string ग्रन्थिः_2::गन्तव्यम्_string(std::function<std::string(ग्रन्थिः_2)> np)
{
	प्रत्यागम दर्शय(PrintVisitor(np));
}

वर्ग CellSimplifier {
	Factory &factory;
	ग्रन्थिः_2 sign(ग्रन्थिः_2 a) {
		प्रत्यागम factory.खण्डिका(a, a.विस्तारः() - 1, 1);
	}
	ग्रन्थिः_2 neg_if(ग्रन्थिः_2 a, ग्रन्थिः_2 s) {
		प्रत्यागम factory.mux(a, factory.unary_minus(a), s);
	}
	ग्रन्थिः_2 निरपेक्षम्(ग्रन्थिः_2 a) {
		प्रत्यागम neg_if(a, sign(a));
	}
	ग्रन्थिः_2 handle_सरणम्(ग्रन्थिः_2 a, ग्रन्थिः_2 b, तार्किक is_दक्षिणः, तार्किक is_signed) {
		// to prevent new_width == 0, we handle this case separately
		यदि(a.विस्तारः() == 1) {
			यदि(!is_signed)
				प्रत्यागम factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			अन्यथा
				प्रत्यागम a;
		}
		पूर्णाङ्क new_विस्तारः = ceil_log2(a.विस्तारः());
		ग्रन्थिः_2 b_truncated = factory.extend(b, new_विस्तारः, असत्य);
		ग्रन्थिः_2 y =
			!is_दक्षिणः ? factory.logical_सरणम्_वामः(a, b_truncated) :
			!is_signed ? factory.logical_सरणम्_दक्षिणः(a, b_truncated) :
			factory.arithmetic_सरणम्_दक्षिणः(a, b_truncated);
		यदि(b.विस्तारः() <= new_विस्तारः)
			प्रत्यागम y;
		ग्रन्थिः_2 अतिप्रवाहः = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.विस्तारः(), b.विस्तारः())));
		ग्रन्थिः_2 y_if_अतिप्रवाहः = is_signed ? factory.extend(sign(a), a.विस्तारः(), सत्य) : factory.constant(RTLIL::Const(State::S0, a.विस्तारः()));
		प्रत्यागम factory.mux(y, y_if_अतिप्रवाहः, अतिप्रवाहः);
	}
सार्वजनिक:
	ग्रन्थिः_2 logical_सरणम्_वामः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { प्रत्यागम handle_सरणम्(a, b, असत्य, असत्य); }
	ग्रन्थिः_2 logical_सरणम्_दक्षिणः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { प्रत्यागम handle_सरणम्(a, b, सत्य, असत्य); }
	ग्रन्थिः_2 arithmetic_सरणम्_दक्षिणः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { प्रत्यागम handle_सरणम्(a, b, सत्य, सत्य); }
	ग्रन्थिः_2 bitwise_mux(ग्रन्थिः_2 a, ग्रन्थिः_2 b, ग्रन्थिः_2 s) {
		ग्रन्थिः_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		ग्रन्थिः_2 bb = factory.bitwise_and(b, s);
		प्रत्यागम factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
गुप्त:
	ग्रन्थिः_2 handle_pow(ग्रन्थिः_2 a0, ग्रन्थिः_2 b, पूर्णाङ्क y_विस्तारः, तार्किक is_signed) {
		ग्रन्थिः_2 a = factory.extend(a0, y_विस्तारः, is_signed);
		ग्रन्थिः_2 r = factory.constant(Const(1, y_विस्तारः));
		प्रति(पूर्णाङ्क i = 0; i < b.विस्तारः(); i++) {
			ग्रन्थिः_2 b_bit = factory.खण्डिका(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		यदि (is_signed) {
			ग्रन्थिः_2 a_ge_1 = factory.unsigned_greater_than(निरपेक्षम्(a0), factory.constant(Const(1, a0.विस्तारः())));
			ग्रन्थिः_2 zero_परिणाम = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_विस्तारः)), zero_परिणाम);
		}
		प्रत्यागम r;
	}
	ग्रन्थिः_2 handle_bmux(ग्रन्थिः_2 a, ग्रन्थिः_2 s, पूर्णाङ्क a_विस्थापनम्, पूर्णाङ्क विस्तारः, पूर्णाङ्क sn) {
		यदि(sn < 1)
			प्रत्यागम factory.खण्डिका(a, a_विस्थापनम्, विस्तारः);
		अन्यथा {
			ग्रन्थिः_2 y0 = handle_bmux(a, s, a_विस्थापनम्, विस्तारः, sn - 1);
			ग्रन्थिः_2 y1 = handle_bmux(a, s, a_विस्थापनम् + (विस्तारः << (sn - 1)), विस्तारः, sn - 1);
			प्रत्यागम factory.mux(y0, y1, factory.खण्डिका(s, sn - 1, 1));
		}
	}
	ग्रन्थिः_2 handle_pmux(ग्रन्थिः_2 a, ग्रन्थिः_2 b, ग्रन्थिः_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.विस्तारः() == a.विस्तारः() * s.विस्तारः());
		ग्रन्थिः_2 y = a;
		प्रति(पूर्णाङ्क i = 0; i < s.विस्तारः(); i++)
			y = factory.mux(y, factory.खण्डिका(b, a.विस्तारः() * i, a.विस्तारः()), factory.खण्डिका(s, i, 1));
		प्रत्यागम y;
	}
	dict<IdString, ग्रन्थिः_2> handle_fa(ग्रन्थिः_2 a, ग्रन्थिः_2 b, ग्रन्थिः_2 c) {
		ग्रन्थिः_2 t1 = factory.bitwise_xor(a, b);
		ग्रन्थिः_2 t2 = factory.bitwise_and(a, b);
		ग्रन्थिः_2 t3 = factory.bitwise_and(c, t1);
		ग्रन्थिः_2 y = factory.bitwise_xor(c, t1);
		ग्रन्थिः_2 x = factory.bitwise_or(t2, t3);
		प्रत्यागम {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, ग्रन्थिः_2> handle_alu(ग्रन्थिः_2 a_अन्तर्, ग्रन्थिः_2 b_अन्तर्, पूर्णाङ्क y_विस्तारः, तार्किक is_signed, ग्रन्थिः_2 ci, ग्रन्थिः_2 bi) {
		ग्रन्थिः_2 a = factory.extend(a_अन्तर्, y_विस्तारः, is_signed);
		ग्रन्थिः_2 b_uninverted = factory.extend(b_अन्तर्, y_विस्तारः, is_signed);
		ग्रन्थिः_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		ग्रन्थिः_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		ग्रन्थिः_2 a_extra = factory.extend(a, y_विस्तारः + 1, असत्य);
		ग्रन्थिः_2 b_extra = factory.extend(b, y_विस्तारः + 1, असत्य);
		ग्रन्थिः_2 y_extra = factory.सङ्कलय(factory.सङ्कलय(a_extra, b_extra), factory.extend(ci, a.विस्तारः() + 1, असत्य));
		ग्रन्थिः_2 y = factory.खण्डिका(y_extra, 0, y_विस्तारः);
		ग्रन्थिः_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		ग्रन्थिः_2 co = factory.खण्डिका(carries, 1, y_विस्तारः);
		प्रत्यागम {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	ग्रन्थिः_2 handle_lcu(ग्रन्थिः_2 p, ग्रन्थिः_2 g, ग्रन्थिः_2 ci) {
		प्रत्यागम handle_alu(g, factory.bitwise_or(p, g), g.विस्तारः(), असत्य, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
सार्वजनिक:
	std::variant<dict<IdString, ग्रन्थिः_2>, ग्रन्थिः_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, ग्रन्थिः_2> inputs)
	{
		पूर्णाङ्क a_विस्तारः = parameters.at(ID(A_विस्तारः), Const(-1)).as_int();
		पूर्णाङ्क b_विस्तारः = parameters.at(ID(B_विस्तारः), Const(-1)).as_int();
		पूर्णाङ्क y_विस्तारः = parameters.at(ID(Y_विस्तारः), Const(-1)).as_int();
		तार्किक a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		तार्किक b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		यदि(cellType.अन्तर्(ID($सङ्कलय), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			तार्किक is_signed = a_signed && b_signed;
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), y_विस्तारः, is_signed);
			ग्रन्थिः_2 b = factory.extend(inputs.at(ID(B)), y_विस्तारः, is_signed);
			यदि(cellType == ID($सङ्कलय))
				प्रत्यागम factory.सङ्कलय(a, b);
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
		}अन्यथा यदि(cellType.अन्तर्(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			तार्किक is_signed = a_signed && b_signed;
			पूर्णाङ्क विस्तारः = max(a_विस्तारः, b_विस्तारः);
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), विस्तारः, is_signed);
			ग्रन्थिः_2 b = factory.extend(inputs.at(ID(B)), विस्तारः, is_signed);
			यदि(cellType.अन्तर्(ID($eq), ID($eqx)))
				प्रत्यागम factory.extend(factory.equal(a, b), y_विस्तारः, असत्य);
			अन्यथा यदि(cellType.अन्तर्(ID($ne), ID($nex)))
				प्रत्यागम factory.extend(factory.not_equal(a, b), y_विस्तारः, असत्य);
			अन्यथा यदि(cellType == ID($lt))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_विस्तारः, असत्य);
			अन्यथा यदि(cellType == ID($le))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_विस्तारः, असत्य);
			अन्यथा यदि(cellType == ID($gt))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_विस्तारः, असत्य);
			अन्यथा यदि(cellType == ID($ge))
				प्रत्यागम factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_विस्तारः, असत्य);
			अन्यथा
				log_abort();
		}अन्यथा यदि(cellType.अन्तर्(ID($logic_or), ID($logic_and))){
			ग्रन्थिः_2 a = factory.reduce_or(inputs.at(ID(A)));
			ग्रन्थिः_2 b = factory.reduce_or(inputs.at(ID(B)));
			ग्रन्थिः_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			प्रत्यागम factory.extend(y, y_विस्तारः, असत्य);
		}अन्यथा यदि(cellType == ID($not)){
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), y_विस्तारः, a_signed);
			प्रत्यागम factory.bitwise_not(a);
		}अन्यथा यदि(cellType == ID($pos)){
			प्रत्यागम factory.extend(inputs.at(ID(A)), y_विस्तारः, a_signed);
		}अन्यथा यदि(cellType == ID($barrier)){
			प्रत्यागम inputs.at(ID(A));
		}अन्यथा यदि(cellType == ID($neg)){
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), y_विस्तारः, a_signed);
			प्रत्यागम factory.unary_minus(a);
		}अन्यथा यदि(cellType == ID($logic_not)){
			ग्रन्थिः_2 a = factory.reduce_or(inputs.at(ID(A)));
			ग्रन्थिः_2 y = factory.bitwise_not(a);
			प्रत्यागम factory.extend(y, y_विस्तारः, असत्य);
		}अन्यथा यदि(cellType.अन्तर्(ID($reduce_or), ID($reduce_bool))){
			ग्रन्थिः_2 a = factory.reduce_or(inputs.at(ID(A)));
			प्रत्यागम factory.extend(a, y_विस्तारः, असत्य);
		}अन्यथा यदि(cellType == ID($reduce_and)){
			ग्रन्थिः_2 a = factory.reduce_and(inputs.at(ID(A)));
			प्रत्यागम factory.extend(a, y_विस्तारः, असत्य);
		}अन्यथा यदि(cellType.अन्तर्(ID($reduce_xor), ID($reduce_xnor))){
			ग्रन्थिः_2 a = factory.reduce_xor(inputs.at(ID(A)));
			ग्रन्थिः_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			प्रत्यागम factory.extend(y, y_विस्तारः, असत्य);
		}अन्यथा यदि(cellType == ID($shl) || cellType == ID($sshl)){
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), y_विस्तारः, a_signed);
			ग्रन्थिः_2 b = inputs.at(ID(B));
			प्रत्यागम logical_सरणम्_वामः(a, b);
		}अन्यथा यदि(cellType == ID($shr) || cellType == ID($sshr)){
			पूर्णाङ्क विस्तारः = max(a_विस्तारः, y_विस्तारः);
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), विस्तारः, a_signed);
			ग्रन्थिः_2 b = inputs.at(ID(B));
			ग्रन्थिः_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_सरणम्_दक्षिणः(a, b) :
				logical_सरणम्_दक्षिणः(a, b);
			प्रत्यागम factory.extend(y, y_विस्तारः, a_signed);
		}अन्यथा यदि(cellType == ID($shiftx) || cellType == ID($सरणम्)){
			पूर्णाङ्क विस्तारः = max(a_विस्तारः, y_विस्तारः);
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), विस्तारः, cellType == ID($सरणम्) && a_signed);
			ग्रन्थिः_2 b = inputs.at(ID(B));
			ग्रन्थिः_2 shr = logical_सरणम्_दक्षिणः(a, b);
			यदि(b_signed) {
				ग्रन्थिः_2 shl = logical_सरणम्_वामः(a, factory.unary_minus(b));
				ग्रन्थिः_2 y = factory.mux(shr, shl, sign(b));
				प्रत्यागम factory.extend(y, y_विस्तारः, असत्य);
			} अन्यथा {
				प्रत्यागम factory.extend(shr, y_विस्तारः, असत्य);
			}
		}अन्यथा यदि(cellType == ID($mux)){
			प्रत्यागम factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा यदि(cellType == ID($pmux)){
			प्रत्यागम handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}अन्यथा यदि(cellType == ID($concat)){
			ग्रन्थिः_2 a = inputs.at(ID(A));
			ग्रन्थिः_2 b = inputs.at(ID(B));
			प्रत्यागम factory.concat(a, b);
		}अन्यथा यदि(cellType == ID($खण्डिका)){
			पूर्णाङ्क विस्थापनम् = parameters.at(ID(विस्थापनम्_2)).as_int();
			ग्रन्थिः_2 a = inputs.at(ID(A));
			प्रत्यागम factory.खण्डिका(a, विस्थापनम्, y_विस्तारः);
		}अन्यथा यदि(cellType.अन्तर्(ID($भाजय), ID($mod), ID($divfloor), ID($modfloor))) {
			पूर्णाङ्क विस्तारः = max(a_विस्तारः, b_विस्तारः);
			तार्किक is_signed = a_signed && b_signed;
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), विस्तारः, is_signed);
			ग्रन्थिः_2 b = factory.extend(inputs.at(ID(B)), विस्तारः, is_signed);
			यदि(is_signed) {
				यदि(cellType == ID($भाजय)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					ग्रन्थिः_2 abs_y = factory.unsigned_div(निरपेक्षम्(a), निरपेक्षम्(b));
					ग्रन्थिः_2 बहिर्गमनम्_sign = factory.not_equal(sign(a), sign(b));
					प्रत्यागम neg_if(factory.extend(abs_y, y_विस्तारः, असत्य), बहिर्गमनम्_sign);
				} अन्यथा यदि(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					ग्रन्थिः_2 abs_y = factory.unsigned_mod(निरपेक्षम्(a), निरपेक्षम्(b));
					प्रत्यागम neg_if(factory.extend(abs_y, y_विस्तारः, असत्य), sign(a));
				} अन्यथा यदि(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					ग्रन्थिः_2 b_sign = sign(b);
					ग्रन्थिः_2 a1 = neg_if(a, b_sign);
					ग्रन्थिः_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					ग्रन्थिः_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					ग्रन्थिः_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					ग्रन्थिः_2 y1 = factory.unsigned_div(a2, b1);
					ग्रन्थिः_2 y2 = factory.extend(y1, y_विस्तारः, असत्य);
					प्रत्यागम factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} अन्यथा यदि(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					ग्रन्थिः_2 abs_b = निरपेक्षम्(b);
					ग्रन्थिः_2 abs_y = factory.unsigned_mod(निरपेक्षम्(a), abs_b);
					ग्रन्थिः_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					ग्रन्थिः_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					ग्रन्थिः_2 y = neg_if(y_flipped, sign(b));
					प्रत्यागम factory.extend(y, y_विस्तारः, सत्य);
				} अन्यथा
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} अन्यथा {
				यदि(cellType.अन्तर्(ID($mod), ID($modfloor)))
					प्रत्यागम factory.extend(factory.unsigned_mod(a, b), y_विस्तारः, असत्य);
				अन्यथा
					प्रत्यागम factory.extend(factory.unsigned_div(a, b), y_विस्तारः, असत्य);
			}
		} अन्यथा यदि(cellType == ID($घातः)) {
			प्रत्यागम handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_विस्तारः, a_signed && b_signed);
		} अन्यथा यदि (cellType == ID($lut)) {
			पूर्णाङ्क विस्तारः = parameters.at(ID(विस्तारः_2)).as_int();
			Const lut_सारणी = parameters.at(ID(LUT));
			lut_सारणी.extu(1 << विस्तारः);
			प्रत्यागम handle_bmux(factory.constant(lut_सारणी), inputs.at(ID(A)), 0, 1, विस्तारः);
		} अन्यथा यदि (cellType == ID($bwmux)) {
			ग्रन्थिः_2 a = inputs.at(ID(A));
			ग्रन्थिः_2 b = inputs.at(ID(B));
			ग्रन्थिः_2 s = inputs.at(ID(S));
			प्रत्यागम factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} अन्यथा यदि (cellType == ID($bweqx)) {
			ग्रन्थिः_2 a = inputs.at(ID(A));
			ग्रन्थिः_2 b = inputs.at(ID(B));
			प्रत्यागम factory.bitwise_not(factory.bitwise_xor(a, b));
		} अन्यथा यदि(cellType == ID($bmux)) {
			पूर्णाङ्क विस्तारः = parameters.at(ID(विस्तारः_2)).as_int();
			पूर्णाङ्क s_विस्तारः = parameters.at(ID(S_विस्तारः)).as_int();
			प्रत्यागम handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, विस्तारः, s_विस्तारः);
		} अन्यथा यदि(cellType == ID($demux)) {
			पूर्णाङ्क विस्तारः = parameters.at(ID(विस्तारः_2)).as_int();
			पूर्णाङ्क s_विस्तारः = parameters.at(ID(S_विस्तारः)).as_int();
			पूर्णाङ्क y_विस्तारः = विस्तारः << s_विस्तारः;
			पूर्णाङ्क b_विस्तारः = ceil_log2(y_विस्तारः);
			ग्रन्थिः_2 a = factory.extend(inputs.at(ID(A)), y_विस्तारः, असत्य);
			ग्रन्थिः_2 s = factory.extend(inputs.at(ID(S)), b_विस्तारः, असत्य);
			ग्रन्थिः_2 b = factory.mul(s, factory.constant(Const(विस्तारः, b_विस्तारः)));
			प्रत्यागम factory.logical_सरणम्_वामः(a, b);
		} अन्यथा यदि(cellType == ID($fa)) {
			प्रत्यागम handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} अन्यथा यदि(cellType == ID($lcu)) {
			प्रत्यागम handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} अन्यथा यदि(cellType == ID($alu)) {
			प्रत्यागम handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_विस्तारः, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} अन्यथा यदि(cellType.अन्तर्(ID($प्रतिज्ञा), ID($assume), ID($जीवितम्), ID($fair), ID($cover))) {
			ग्रन्थिः_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			स्वचालित &output = factory.सङ्कलय_output(cellName, cellType, क्रमय_2(1));
			output.समुच्चयः_मानम्(a);
			प्रत्यागम {};
		} अन्यथा यदि(cellType.अन्तर्(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			पूर्णाङ्क विस्तारः = parameters.at(ID(विस्तारः_2)).as_int();
			स्वचालित &निवेशः = factory.सङ्कलय_निवेशः(cellName, cellType, क्रमय_2(विस्तारः));
			प्रत्यागम factory.मानम्(निवेशः);
		} अन्यथा यदि(cellType == ID($initstate)) {
			यदि(factory.ir().has_state(ID($initstate), ID($state)))
				प्रत्यागम factory.मानम्(factory.ir().state(ID($initstate)));
			अन्यथा {
				स्वचालित &state = factory.सङ्कलय_state(ID($initstate), ID($state), क्रमय_2(1));
				state.समुच्चयः_initial_मानम्(RTLIL::Const(State::S1, 1));
				state.समुच्चयः_अग्र_मानम्(factory.constant(RTLIL::Const(State::S0, 1)));
				प्रत्यागम factory.मानम्(state);
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
	dict<DriveSpec, ग्रन्थिः_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, ग्रन्थिः_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	सदिशम्<Mem> memories_सदिशम्;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	ग्रन्थिः_2 पङ्क्तौ_निक्षिप(DriveSpec ध्रुव &spec)
	{
		स्वचालित it = graph_nodes.find(spec);
		यदि(it == graph_nodes.अन्तः()){
			स्वचालित ग्रन्थिः = factory.create_pending(spec.आकार());
			graph_nodes.निवेशय({spec, ग्रन्थिः});
			queue.emplace_पृष्ठभागः(spec);
			प्रत्यागम ग्रन्थिः;
		}अन्यथा
			प्रत्यागम it->second;
	}
	ग्रन्थिः_2 पङ्क्तौ_निक्षिप_cell(Cell *cell, IdString पत्तनम्_name)
	{
		स्वचालित it = cell_outputs.find({cell, पत्तनम्_name});
		यदि(it == cell_outputs.अन्तः()) {
			queue.emplace_पृष्ठभागः(cell);
			std::optional<ग्रन्थिः_2> rv;
			प्रति(स्वचालित ध्रुव &[name, sigspec] : cell->connections())
				यदि(driver_map.celltypes.cell_output(cell->type, name)) {
					स्वचालित ग्रन्थिः = factory.create_pending(sigspec.आकार());
					factory.suggest_name(ग्रन्थिः, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, ग्रन्थिः);
					यदि(name == पत्तनम्_name)
						rv = ग्रन्थिः;
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
		driver_map.सङ्कलय(module);
		प्रति (स्वचालित cell : module->cells()) {
			यदि (cell->type.अन्तर्(ID($प्रतिज्ञा), ID($assume), ID($जीवितम्), ID($fair), ID($cover), ID($check)))
				queue.emplace_पृष्ठभागः(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		प्रति (स्वचालित riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			स्वचालित *wire = module->wire(*riter);
			यदि (wire && wire->पत्तनम्_निवेशः) {
				factory.सङ्कलय_निवेशः(wire->name, ID($निवेशः), क्रमय_2(wire->विस्तारः));
			}
			यदि (wire && wire->पत्तनम्_output) {
				स्वचालित &output = factory.सङ्कलय_output(wire->name, ID($output), क्रमय_2(wire->विस्तारः));
				output.समुच्चयः_मानम्(पङ्क्तौ_निक्षिप(DriveChunk(DriveChunkWire(wire, 0, wire->विस्तारः))));
			}
		}
		memories_सदिशम् = Mem::get_all_memories(module);
		प्रति (स्वचालित &mem : memories_सदिशम्) {
			यदि (mem.cell != शून्यसूचकः)
				memories[mem.cell] = &mem;
		}
	}
गुप्त:
	ग्रन्थिः_2 concatenate_read_results(Mem *mem, सदिशम्<ग्रन्थिः_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ध्रुव SigSpec &rd_दत्त = mem->cell->connections().at(ID(RD_दत्त));
		पूर्णाङ्क current = 0;
		प्रति(आकार_t i = 0; i < mem->rd_ports.आकार(); i++) {
			पूर्णाङ्क विस्तारः = mem->विस्तारः << mem->rd_ports[i].wide_log2;
			log_assert (results[i].विस्तारः() == विस्तारः);
			log_assert (mem->rd_ports[i].दत्त == rd_दत्त.extract(current, विस्तारः));
			current += विस्तारः;
		}
		log_assert (current == rd_दत्त.आकार());
		log_assert (!results.रिक्तानि());
		ग्रन्थिः_2 ग्रन्थिः = results[0];
		प्रति(आकार_t i = 1; i < results.आकार(); i++)
			ग्रन्थिः = factory.concat(ग्रन्थिः, results[i]);
		प्रत्यागम ग्रन्थिः;
	}
	ग्रन्थिः_2 handle_स्मृतिः(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		सदिशम्<ग्रन्थिः_2> read_results;
		स्वचालित &state = factory.सङ्कलय_state(mem->cell->name, ID($state), क्रमय_2(ceil_log2(mem->आकार), mem->विस्तारः));
		state.समुच्चयः_initial_मानम्(MemContents(mem));
		ग्रन्थिः_2 ग्रन्थिः = factory.मानम्(state);
		प्रति (आकार_t i = 0; i < mem->wr_ports.आकार(); i++) {
			ध्रुव स्वचालित &wr = mem->wr_ports[i];
			यदि (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ग्रन्थिः_2 en = पङ्क्तौ_निक्षिप(driver_map(DriveSpec(wr.en)));
			ग्रन्थिः_2 addr = पङ्क्तौ_निक्षिप(driver_map(DriveSpec(wr.addr)));
			ग्रन्थिः_2 new_दत्त = पङ्क्तौ_निक्षिप(driver_map(DriveSpec(wr.दत्त)));
			ग्रन्थिः_2 old_दत्त = factory.स्मृतिः_read(ग्रन्थिः, addr);
			ग्रन्थिः_2 wr_दत्त = simplifier.bitwise_mux(old_दत्त, new_दत्त, en);
			ग्रन्थिः = factory.स्मृतिः_write(ग्रन्थिः, addr, wr_दत्त);
		}
		यदि (mem->rd_ports.रिक्तानि())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		प्रति (आकार_t i = 0; i < mem->rd_ports.आकार(); i++) {
			ध्रुव स्वचालित &rd = mem->rd_ports[i];
			यदि (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ग्रन्थिः_2 addr = पङ्क्तौ_निक्षिप(driver_map(DriveSpec(rd.addr)));
			read_results.प्रक्षिप_पृष्ठभागः(factory.स्मृतिः_read(ग्रन्थिः, addr));
		}
		state.समुच्चयः_अग्र_मानम्(ग्रन्थिः);
		प्रत्यागम concatenate_read_results(mem, read_results);
	}
	शून्य process_cell(Cell *cell)
	{
		यदि (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, शून्यसूचकः);
			यदि (mem == शून्यसूचकः) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			ग्रन्थिः_2 ग्रन्थिः = handle_स्मृतिः(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_दत्त)}), ग्रन्थिः);
		} अन्यथा यदि (cell->is_builtin_ff()) {
			Ff_दत्त ff(&ff_initvals, cell);
			यदि (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			स्वचालित &state = factory.सङ्कलय_state(ff.name, ID($state), क्रमय_2(ff.विस्तारः));
			ग्रन्थिः_2 q_मानम् = factory.मानम्(state);
			factory.suggest_name(q_मानम्, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_मानम्);
			state.समुच्चयः_अग्र_मानम्(पङ्क्तौ_निक्षिप(ff.sig_d));
			state.समुच्चयः_initial_मानम्(ff.val_init);
		} अन्यथा {
			dict<IdString, ग्रन्थिः_2> connections;
			IdString output_name; // for the single output case
			पूर्णाङ्क n_outputs = 0;
			प्रति(स्वचालित ध्रुव &[name, sigspec] : cell->connections()) {
				यदि(driver_map.celltypes.cell_निवेशः(cell->type, name) && sigspec.आकार() > 0)
					connections.निवेशय({ name, पङ्क्तौ_निक्षिप(Drive_Chunk_पत्तनम्(cell, {name, sigspec})) });
				यदि(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, ग्रन्थिः_2>, ग्रन्थिः_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			यदि(स्वचालित *nodep = std::get_if<ग्रन्थिः_2>(&outputs); nodep != शून्यसूचकः) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} अन्यथा {
				प्रति(स्वचालित [name, ग्रन्थिः] : std::get<dict<IdString, ग्रन्थिः_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), ग्रन्थिः);
			}
		}
	}
	शून्य undriven(ध्रुव std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	शून्य check_undriven(DriveSpec ध्रुव& spec, std::string ध्रुव& name) {
		प्रति(स्वचालित ध्रुव &chunk : spec.chunks())
			यदि(chunk.is_none())
				undriven(name);
	}
सार्वजनिक:
	शून्य process_queue()
	{
		प्रति (; !queue.रिक्तानि(); queue.निष्कासय_front()) {
			यदि(स्वचालित p = std::get_if<Cell *>(&queue.front()); p != शून्यसूचकः) {
				process_cell(*p);
				अनुवर्त;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			ग्रन्थिः_2 pending = graph_nodes.at(spec);

			यदि (spec.chunks().आकार() > 1) {
				स्वचालित chunks = spec.chunks();
				ग्रन्थिः_2 ग्रन्थिः = पङ्क्तौ_निक्षिप(chunks[0]);
				प्रति(आकार_t i = 1; i < chunks.आकार(); i++)
					ग्रन्थिः = factory.concat(ग्रन्थिः, पङ्क्तौ_निक्षिप(chunks[i]));
				factory.update_pending(pending, ग्रन्थिः);
			} अन्यथा यदि (spec.chunks().आकार() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				यदि (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					यदि (wire_chunk.is_सम्पूर्णम्()) {
						यदि (wire_chunk.wire->पत्तनम्_निवेशः) {
							ग्रन्थिः_2 ग्रन्थिः = factory.मानम्(factory.ir().निवेशः(wire_chunk.wire->name));
							factory.suggest_name(ग्रन्थिः, wire_chunk.wire->name);
							factory.update_pending(pending, ग्रन्थिः);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							ग्रन्थिः_2 ग्रन्थिः = पङ्क्तौ_निक्षिप(driver);
							factory.suggest_name(ग्रन्थिः, wire_chunk.wire->name);
							factory.update_pending(pending, ग्रन्थिः);
						}
					} अन्यथा {
						DriveChunkWire सम्पूर्णम्_wire(wire_chunk.wire, 0, wire_chunk.wire->विस्तारः);
						ग्रन्थिः_2 ग्रन्थिः = factory.खण्डिका(पङ्क्तौ_निक्षिप(सम्पूर्णम्_wire), wire_chunk.विस्थापनम्, wire_chunk.विस्तारः);
						factory.update_pending(pending, ग्रन्थिः);
					}
				} अन्यथा यदि (chunk.is_पत्तनम्()) {
					Drive_Chunk_पत्तनम् पत्तनम्_chunk = chunk.पत्तनम्();
					यदि (पत्तनम्_chunk.is_सम्पूर्णम्()) {
						यदि (driver_map.celltypes.cell_output(पत्तनम्_chunk.cell->type, पत्तनम्_chunk.पत्तनम्)) {
							ग्रन्थिः_2 ग्रन्थिः = पङ्क्तौ_निक्षिप_cell(पत्तनम्_chunk.cell, पत्तनम्_chunk.पत्तनम्);
							factory.update_pending(pending, ग्रन्थिः);
						} अन्यथा {
							DriveSpec driver = driver_map(DriveSpec(पत्तनम्_chunk));
							check_undriven(driver, पत्तनम्_chunk.cell->name.unescape() + " port " + PooledName(पत्तनम्_chunk.cell->module, पत्तनम्_chunk.पत्तनम्).unescape());
							factory.update_pending(pending, पङ्क्तौ_निक्षिप(driver));
						}
					} अन्यथा {
						Drive_Chunk_पत्तनम् सम्पूर्णम्_पत्तनम्(पत्तनम्_chunk.cell, पत्तनम्_chunk.पत्तनम्, 0, Get_आकार(पत्तनम्_chunk.cell->connections().at(पत्तनम्_chunk.पत्तनम्)));
						ग्रन्थिः_2 ग्रन्थिः = factory.खण्डिका(पङ्क्तौ_निक्षिप(सम्पूर्णम्_पत्तनम्), पत्तनम्_chunk.विस्थापनम्, पत्तनम्_chunk.विस्तारः);
						factory.update_pending(pending, ग्रन्थिः);
					}
				} अन्यथा यदि (chunk.is_constant()) {
					ग्रन्थिः_2 ग्रन्थिः = factory.constant(chunk.constant());
					factory.suggest_name(ग्रन्थिः, "$const" + std::गन्तव्यम्_string(chunk.आकार()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, ग्रन्थिः);
				} अन्यथा यदि (chunk.is_गुणजः()) {
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
    स्वचालित factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_queue();
    ir.topological_क्रमय();
    ir.forward_buf();
    प्रत्यागम ir;
}

शून्य IR::topological_क्रमय() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    तार्किक scc = असत्य;
    std::सदिशम्<पूर्णाङ्क> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](पूर्णाङ्क *begin, पूर्णाङ्क *अन्तः) {
        perm.निवेशय(perm.अन्तः(), begin, अन्तः);
        यदि (अन्तः > begin + 1)
        {
            log_warning("Combinational loop:\n");
            प्रति (पूर्णाङ्क *i = begin; i != अन्तः; ++i) {
				ग्रन्थिः_2 ग्रन्थिः(_graph[*i], design);
                लघुगणकः("- %s = %s\n", ग्रन्थिः.name().unescape(), ग्रन्थिः.गन्तव्यम्_string());
			}
            लघुगणकः("\n");
            scc = सत्य;
        }
    });
	प्रति(ध्रुव स्वचालित &[name, state]: अवस्थाः_2)
		यदि(state.has_अग्र_मानम्())
			toposort.process(state.अग्र_मानम्().id());
	प्रति(ध्रुव स्वचालित &[name, output]: _outputs)
		यदि(output.has_मानम्())
			toposort.process(output.मानम्().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    यदि(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

स्थिर IdString merge_name(IdString a, IdString b) {
	यदि(!a.is_लौकिकम्() && b.is_लौकिकम्())
		प्रत्यागम b;
	अन्यथा
		प्रत्यागम a;
}

शून्य IR::forward_buf() {
    std::सदिशम्<पूर्णाङ्क> perm, alias;
    perm.clear();

    प्रति (पूर्णाङ्क i = 0; i < _graph.आकार(); ++i)
    {
        स्वचालित ग्रन्थिः = _graph[i];
        यदि (ग्रन्थिः.function().fn() == Fn::buf && ग्रन्थिः.arg(0).सूचकांक() < i)
        {
            पूर्णाङ्क target_सूचकांक = alias[ग्रन्थिः.arg(0).सूचकांक()];
            स्वचालित target_ग्रन्थिः = _graph[perm[target_सूचकांक]];
			यदि(ग्रन्थिः.has_sparse_attr()) {
				यदि(target_ग्रन्थिः.has_sparse_attr()) {
					IdString id = merge_name(ग्रन्थिः.sparse_attr(), target_ग्रन्थिः.sparse_attr());
					target_ग्रन्थिः.sparse_attr() = id;
				} अन्यथा {
					IdString id = ग्रन्थिः.sparse_attr();
					target_ग्रन्थिः.sparse_attr() = id;
				}
			}
            alias.प्रक्षिप_पृष्ठभागः(target_सूचकांक);
        }
        अन्यथा
        {
            alias.प्रक्षिप_पृष्ठभागः(Get_आकार(perm));
            perm.प्रक्षिप_पृष्ठभागः(i);
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

शून्य Writer::print_impl(ध्रुव वर्ण *fmt, सदिशम्<std::function<शून्य()>> &fns)
{
	आकार_t अग्र_सूचकांक = 0;
	प्रति(ध्रुव वर्ण *p = fmt; *p != 0; p++)
		विकल्प(*p) {
		पक्ष '{':
			यदि(*++p == '{') {
				*os << '{';
			} अन्यथा {
				वर्ण *pe;
				आकार_t सूचकांक = strtoul(p, &pe, 10);
				यदि(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				यदि(p == pe)
					सूचकांक = अग्र_सूचकांक;
				अन्यथा
					p = pe;
				यदि(सूचकांक >= fns.आकार())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", सूचकांक, fns.आकार(), quote_fmt(fmt));
				fns[सूचकांक]();
				अग्र_सूचकांक = सूचकांक + 1;
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
YOSYS_NAMESPACE_अन्तः
