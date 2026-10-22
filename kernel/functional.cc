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

#అన్తర్భావ "kernel/functional.h"
#అన్తర్భావ "kernel/topo_scc.h"
#అన్తర్భావ "ff.h"
#అన్తర్భావ "ffinit.h"
#అన్తర్భావ <deque>

YOSYS_NAMESPACE_BEGIN
నామాకాశ Functional {

స్థిరరాశి అక్షరం *fn_గమ్యం_string(Fn fn) {
	ఎంపిక(fn) {
	స్థితి Fn::invalid: తిరిగి "invalid";
	స్థితి Fn::buf: తిరిగి "buf";
	స్థితి Fn::ముక్క: తిరిగి "slice";
	స్థితి Fn::zero_extend: తిరిగి "zero_extend";
	స్థితి Fn::sign_extend: తిరిగి "sign_extend";
	స్థితి Fn::concat: తిరిగి "concat";
	స్థితి Fn::కూడు: తిరిగి "add";
	స్థితి Fn::sub: తిరిగి "sub";
	స్థితి Fn::mul: తిరిగి "mul";
	స్థితి Fn::unsigned_div: తిరిగి "unsigned_div";
	స్థితి Fn::unsigned_mod: తిరిగి "unsigned_mod";
	స్థితి Fn::bitwise_and: తిరిగి "bitwise_and";
	స్థితి Fn::bitwise_or: తిరిగి "bitwise_or";
	స్థితి Fn::bitwise_xor: తిరిగి "bitwise_xor";
	స్థితి Fn::bitwise_not: తిరిగి "bitwise_not";
	స్థితి Fn::reduce_and: తిరిగి "reduce_and";
	స్థితి Fn::reduce_or: తిరిగి "reduce_or";
	స్థితి Fn::reduce_xor: తిరిగి "reduce_xor";
	స్థితి Fn::unary_minus: తిరిగి "unary_minus";
	స్థితి Fn::equal: తిరిగి "equal";
	స్థితి Fn::not_equal: తిరిగి "not_equal";
	స్థితి Fn::signed_greater_than: తిరిగి "signed_greater_than";
	స్థితి Fn::signed_greater_equal: తిరిగి "signed_greater_equal";
	స్థితి Fn::unsigned_greater_than: తిరిగి "unsigned_greater_than";
	స్థితి Fn::unsigned_greater_equal: తిరిగి "unsigned_greater_equal";
	స్థితి Fn::logical_జరుపు_ఎడమ: తిరిగి "logical_shift_left";
	స్థితి Fn::logical_జరుపు_కుడి: తిరిగి "logical_shift_right";
	స్థితి Fn::arithmetic_జరుపు_కుడి: తిరిగి "arithmetic_shift_right";
	స్థితి Fn::mux: తిరిగి "mux";
	స్థితి Fn::constant: తిరిగి "constant";
	స్థితి Fn::ఆదానం: తిరిగి "input";
	స్థితి Fn::state: తిరిగి "state";
	స్థితి Fn::జ్ఞాపకం_read: తిరిగి "memory_read";
	స్థితి Fn::జ్ఞాపకం_write: తిరిగి "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (పూర్ణాంకం)fn);
}

సదిశ<IRInput స్థిరరాశి*> IR::inputs(IdString రకం) స్థిరరాశి {
	సదిశ<IRInput స్థిరరాశి*> ret;
	కొరకు (స్థిరరాశి స్వయంచాలక &[name, ఆదానం] : _inputs)
		ఒకవేళ(ఆదానం.రకం == రకం)
			ret.నెట్టు_వెనుక(&ఆదానం);
	తిరిగి ret;
}

సదిశ<IROutput స్థిరరాశి*> IR::outputs(IdString రకం) స్థిరరాశి {
	సదిశ<IROutput స్థిరరాశి*> ret;
	కొరకు (స్థిరరాశి స్వయంచాలక &[name, output] : _outputs)
		ఒకవేళ(output.రకం == రకం)
			ret.నెట్టు_వెనుక(&output);
	తిరిగి ret;
}

సదిశ<IRState స్థిరరాశి*> IR::స్థితులు(IdString రకం) స్థిరరాశి {
	సదిశ<IRState స్థిరరాశి*> ret;
	కొరకు (స్థిరరాశి స్వయంచాలక &[name, state] : స్థితులు_2)
		ఒకవేళ(state.రకం == రకం)
			ret.నెట్టు_వెనుక(&state);
	తిరిగి ret;
}

సదిశ<IRInput స్థిరరాశి*> IR::all_inputs() స్థిరరాశి {
	సదిశ<IRInput స్థిరరాశి*> ret;
	కొరకు (స్థిరరాశి స్వయంచాలక &[name, ఆదానం] : _inputs)
		ret.నెట్టు_వెనుక(&ఆదానం);
	తిరిగి ret;
}

సదిశ<IROutput స్థిరరాశి*> IR::all_outputs() స్థిరరాశి {
	సదిశ<IROutput స్థిరరాశి*> ret;
	కొరకు (స్థిరరాశి స్వయంచాలక &[name, output] : _outputs)
		ret.నెట్టు_వెనుక(&output);
	తిరిగి ret;
}

సదిశ<IRState స్థిరరాశి*> IR::all_స్థితులు() స్థిరరాశి {
	సదిశ<IRState స్థిరరాశి*> ret;
	కొరకు (స్థిరరాశి స్వయంచాలక &[name, state] : స్థితులు_2)
		ret.నెట్టు_వెనుక(&state);
	తిరిగి ret;
}

నిర్మాణం PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(కణుపు_2)> np;
	PrintVisitor(std::function<std::string(కణుపు_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string ముక్క(కణుపు_2, కణుపు_2 a, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం బహిర్గతం_వెడల్పు) override { తిరిగి "slice(" + np(a) + ", " + std::గమ్యం_string(స్థానభ్రంశం) + ", " + std::గమ్యం_string(బహిర్గతం_వెడల్పు) + ")"; }
	std::string zero_extend(కణుపు_2, కణుపు_2 a, పూర్ణాంకం బహిర్గతం_వెడల్పు) override { తిరిగి "zero_extend(" + np(a) + ", " + std::గమ్యం_string(బహిర్గతం_వెడల్పు) + ")"; }
	std::string sign_extend(కణుపు_2, కణుపు_2 a, పూర్ణాంకం బహిర్గతం_వెడల్పు) override { తిరిగి "sign_extend(" + np(a) + ", " + std::గమ్యం_string(బహిర్గతం_వెడల్పు) + ")"; }
	std::string constant(కణుపు_2, RTLIL::Const స్థిరరాశి& విలువ) override { తిరిగి "constant(" + విలువ.as_string() + ")"; }
	std::string ఆదానం(కణుపు_2 self, IdString name, IdString రకం) override { తిరిగి "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(రకం) + ")"; }
	std::string state(కణుపు_2 self, IdString name, IdString రకం) override { తిరిగి "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(రకం) + ")"; }
	std::string default_handler(కణుపు_2 self) override {
		std::string ret = fn_గమ్యం_string(self.fn());
		ret += "(";
		కొరకు(కొలత_t i = 0; i < self.arg_లెక్క(); i++) {
			ఒకవేళ(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		తిరిగి ret;
	}
};

std::string కణుపు_2::గమ్యం_string()
{
	తిరిగి గమ్యం_string([](కణుపు_2 n) { తిరిగి n.name().unescape(); });
}

std::string కణుపు_2::గమ్యం_string(std::function<std::string(కణుపు_2)> np)
{
	తిరిగి సందర్శించు(PrintVisitor(np));
}

తరగతి CellSimplifier {
	Factory &factory;
	కణుపు_2 sign(కణుపు_2 a) {
		తిరిగి factory.ముక్క(a, a.వెడల్పు() - 1, 1);
	}
	కణుపు_2 neg_if(కణుపు_2 a, కణుపు_2 s) {
		తిరిగి factory.mux(a, factory.unary_minus(a), s);
	}
	కణుపు_2 నిరపేక్ష(కణుపు_2 a) {
		తిరిగి neg_if(a, sign(a));
	}
	కణుపు_2 handle_జరుపు(కణుపు_2 a, కణుపు_2 b, తార్కిక is_కుడి, తార్కిక is_signed) {
		// to prevent new_width == 0, we handle this case separately
		ఒకవేళ(a.వెడల్పు() == 1) {
			ఒకవేళ(!is_signed)
				తిరిగి factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			లేకపోతే
				తిరిగి a;
		}
		పూర్ణాంకం new_వెడల్పు = ceil_log2(a.వెడల్పు());
		కణుపు_2 b_truncated = factory.extend(b, new_వెడల్పు, అబద్ధం);
		కణుపు_2 y =
			!is_కుడి ? factory.logical_జరుపు_ఎడమ(a, b_truncated) :
			!is_signed ? factory.logical_జరుపు_కుడి(a, b_truncated) :
			factory.arithmetic_జరుపు_కుడి(a, b_truncated);
		ఒకవేళ(b.వెడల్పు() <= new_వెడల్పు)
			తిరిగి y;
		కణుపు_2 అధికప్రవాహం = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.వెడల్పు(), b.వెడల్పు())));
		కణుపు_2 y_if_అధికప్రవాహం = is_signed ? factory.extend(sign(a), a.వెడల్పు(), నిజం) : factory.constant(RTLIL::Const(State::S0, a.వెడల్పు()));
		తిరిగి factory.mux(y, y_if_అధికప్రవాహం, అధికప్రవాహం);
	}
బహిరంగ:
	కణుపు_2 logical_జరుపు_ఎడమ(కణుపు_2 a, కణుపు_2 b) { తిరిగి handle_జరుపు(a, b, అబద్ధం, అబద్ధం); }
	కణుపు_2 logical_జరుపు_కుడి(కణుపు_2 a, కణుపు_2 b) { తిరిగి handle_జరుపు(a, b, నిజం, అబద్ధం); }
	కణుపు_2 arithmetic_జరుపు_కుడి(కణుపు_2 a, కణుపు_2 b) { తిరిగి handle_జరుపు(a, b, నిజం, నిజం); }
	కణుపు_2 bitwise_mux(కణుపు_2 a, కణుపు_2 b, కణుపు_2 s) {
		కణుపు_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		కణుపు_2 bb = factory.bitwise_and(b, s);
		తిరిగి factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
అంతరంగ:
	కణుపు_2 handle_pow(కణుపు_2 a0, కణుపు_2 b, పూర్ణాంకం y_వెడల్పు, తార్కిక is_signed) {
		కణుపు_2 a = factory.extend(a0, y_వెడల్పు, is_signed);
		కణుపు_2 r = factory.constant(Const(1, y_వెడల్పు));
		కొరకు(పూర్ణాంకం i = 0; i < b.వెడల్పు(); i++) {
			కణుపు_2 b_బిట్టు = factory.ముక్క(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_బిట్టు);
			a = factory.mul(a, a);
		}
		ఒకవేళ (is_signed) {
			కణుపు_2 a_ge_1 = factory.unsigned_greater_than(నిరపేక్ష(a0), factory.constant(Const(1, a0.వెడల్పు())));
			కణుపు_2 zero_ఫలితం = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_వెడల్పు)), zero_ఫలితం);
		}
		తిరిగి r;
	}
	కణుపు_2 handle_bmux(కణుపు_2 a, కణుపు_2 s, పూర్ణాంకం a_స్థానభ్రంశం, పూర్ణాంకం వెడల్పు, పూర్ణాంకం sn) {
		ఒకవేళ(sn < 1)
			తిరిగి factory.ముక్క(a, a_స్థానభ్రంశం, వెడల్పు);
		లేకపోతే {
			కణుపు_2 y0 = handle_bmux(a, s, a_స్థానభ్రంశం, వెడల్పు, sn - 1);
			కణుపు_2 y1 = handle_bmux(a, s, a_స్థానభ్రంశం + (వెడల్పు << (sn - 1)), వెడల్పు, sn - 1);
			తిరిగి factory.mux(y0, y1, factory.ముక్క(s, sn - 1, 1));
		}
	}
	కణుపు_2 handle_pmux(కణుపు_2 a, కణుపు_2 b, కణుపు_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.వెడల్పు() == a.వెడల్పు() * s.వెడల్పు());
		కణుపు_2 y = a;
		కొరకు(పూర్ణాంకం i = 0; i < s.వెడల్పు(); i++)
			y = factory.mux(y, factory.ముక్క(b, a.వెడల్పు() * i, a.వెడల్పు()), factory.ముక్క(s, i, 1));
		తిరిగి y;
	}
	dict<IdString, కణుపు_2> handle_fa(కణుపు_2 a, కణుపు_2 b, కణుపు_2 c) {
		కణుపు_2 t1 = factory.bitwise_xor(a, b);
		కణుపు_2 t2 = factory.bitwise_and(a, b);
		కణుపు_2 t3 = factory.bitwise_and(c, t1);
		కణుపు_2 y = factory.bitwise_xor(c, t1);
		కణుపు_2 x = factory.bitwise_or(t2, t3);
		తిరిగి {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, కణుపు_2> handle_alu(కణుపు_2 a_లోపల, కణుపు_2 b_లోపల, పూర్ణాంకం y_వెడల్పు, తార్కిక is_signed, కణుపు_2 ci, కణుపు_2 bi) {
		కణుపు_2 a = factory.extend(a_లోపల, y_వెడల్పు, is_signed);
		కణుపు_2 b_uninverted = factory.extend(b_లోపల, y_వెడల్పు, is_signed);
		కణుపు_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		కణుపు_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		కణుపు_2 a_extra = factory.extend(a, y_వెడల్పు + 1, అబద్ధం);
		కణుపు_2 b_extra = factory.extend(b, y_వెడల్పు + 1, అబద్ధం);
		కణుపు_2 y_extra = factory.కూడు(factory.కూడు(a_extra, b_extra), factory.extend(ci, a.వెడల్పు() + 1, అబద్ధం));
		కణుపు_2 y = factory.ముక్క(y_extra, 0, y_వెడల్పు);
		కణుపు_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		కణుపు_2 co = factory.ముక్క(carries, 1, y_వెడల్పు);
		తిరిగి {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	కణుపు_2 handle_lcu(కణుపు_2 p, కణుపు_2 g, కణుపు_2 ci) {
		తిరిగి handle_alu(g, factory.bitwise_or(p, g), g.వెడల్పు(), అబద్ధం, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
బహిరంగ:
	std::variant<dict<IdString, కణుపు_2>, కణుపు_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, కణుపు_2> inputs)
	{
		పూర్ణాంకం a_వెడల్పు = parameters.at(ID(A_వెడల్పు), Const(-1)).as_int();
		పూర్ణాంకం b_వెడల్పు = parameters.at(ID(B_వెడల్పు), Const(-1)).as_int();
		పూర్ణాంకం y_వెడల్పు = parameters.at(ID(Y_వెడల్పు), Const(-1)).as_int();
		తార్కిక a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		తార్కిక b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		ఒకవేళ(cellType.లోపల(ID($కూడు), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			తార్కిక is_signed = a_signed && b_signed;
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), y_వెడల్పు, is_signed);
			కణుపు_2 b = factory.extend(inputs.at(ID(B)), y_వెడల్పు, is_signed);
			ఒకవేళ(cellType == ID($కూడు))
				తిరిగి factory.కూడు(a, b);
			లేకపోతే ఒకవేళ(cellType == ID($sub))
				తిరిగి factory.sub(a, b);
			లేకపోతే ఒకవేళ(cellType == ID($mul))
				తిరిగి factory.mul(a, b);
			లేకపోతే ఒకవేళ(cellType == ID($and))
				తిరిగి factory.bitwise_and(a, b);
			లేకపోతే ఒకవేళ(cellType == ID($or))
				తిరిగి factory.bitwise_or(a, b);
			లేకపోతే ఒకవేళ(cellType == ID($xor))
				తిరిగి factory.bitwise_xor(a, b);
			లేకపోతే ఒకవేళ(cellType == ID($xnor))
				తిరిగి factory.bitwise_not(factory.bitwise_xor(a, b));
			లేకపోతే
				log_abort();
		}లేకపోతే ఒకవేళ(cellType.లోపల(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			తార్కిక is_signed = a_signed && b_signed;
			పూర్ణాంకం వెడల్పు = max(a_వెడల్పు, b_వెడల్పు);
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), వెడల్పు, is_signed);
			కణుపు_2 b = factory.extend(inputs.at(ID(B)), వెడల్పు, is_signed);
			ఒకవేళ(cellType.లోపల(ID($eq), ID($eqx)))
				తిరిగి factory.extend(factory.equal(a, b), y_వెడల్పు, అబద్ధం);
			లేకపోతే ఒకవేళ(cellType.లోపల(ID($ne), ID($nex)))
				తిరిగి factory.extend(factory.not_equal(a, b), y_వెడల్పు, అబద్ధం);
			లేకపోతే ఒకవేళ(cellType == ID($lt))
				తిరిగి factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_వెడల్పు, అబద్ధం);
			లేకపోతే ఒకవేళ(cellType == ID($le))
				తిరిగి factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_వెడల్పు, అబద్ధం);
			లేకపోతే ఒకవేళ(cellType == ID($gt))
				తిరిగి factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_వెడల్పు, అబద్ధం);
			లేకపోతే ఒకవేళ(cellType == ID($ge))
				తిరిగి factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_వెడల్పు, అబద్ధం);
			లేకపోతే
				log_abort();
		}లేకపోతే ఒకవేళ(cellType.లోపల(ID($logic_or), ID($logic_and))){
			కణుపు_2 a = factory.reduce_or(inputs.at(ID(A)));
			కణుపు_2 b = factory.reduce_or(inputs.at(ID(B)));
			కణుపు_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			తిరిగి factory.extend(y, y_వెడల్పు, అబద్ధం);
		}లేకపోతే ఒకవేళ(cellType == ID($not)){
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), y_వెడల్పు, a_signed);
			తిరిగి factory.bitwise_not(a);
		}లేకపోతే ఒకవేళ(cellType == ID($pos)){
			తిరిగి factory.extend(inputs.at(ID(A)), y_వెడల్పు, a_signed);
		}లేకపోతే ఒకవేళ(cellType == ID($barrier)){
			తిరిగి inputs.at(ID(A));
		}లేకపోతే ఒకవేళ(cellType == ID($neg)){
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), y_వెడల్పు, a_signed);
			తిరిగి factory.unary_minus(a);
		}లేకపోతే ఒకవేళ(cellType == ID($logic_not)){
			కణుపు_2 a = factory.reduce_or(inputs.at(ID(A)));
			కణుపు_2 y = factory.bitwise_not(a);
			తిరిగి factory.extend(y, y_వెడల్పు, అబద్ధం);
		}లేకపోతే ఒకవేళ(cellType.లోపల(ID($reduce_or), ID($reduce_bool))){
			కణుపు_2 a = factory.reduce_or(inputs.at(ID(A)));
			తిరిగి factory.extend(a, y_వెడల్పు, అబద్ధం);
		}లేకపోతే ఒకవేళ(cellType == ID($reduce_and)){
			కణుపు_2 a = factory.reduce_and(inputs.at(ID(A)));
			తిరిగి factory.extend(a, y_వెడల్పు, అబద్ధం);
		}లేకపోతే ఒకవేళ(cellType.లోపల(ID($reduce_xor), ID($reduce_xnor))){
			కణుపు_2 a = factory.reduce_xor(inputs.at(ID(A)));
			కణుపు_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			తిరిగి factory.extend(y, y_వెడల్పు, అబద్ధం);
		}లేకపోతే ఒకవేళ(cellType == ID($shl) || cellType == ID($sshl)){
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), y_వెడల్పు, a_signed);
			కణుపు_2 b = inputs.at(ID(B));
			తిరిగి logical_జరుపు_ఎడమ(a, b);
		}లేకపోతే ఒకవేళ(cellType == ID($shr) || cellType == ID($sshr)){
			పూర్ణాంకం వెడల్పు = max(a_వెడల్పు, y_వెడల్పు);
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), వెడల్పు, a_signed);
			కణుపు_2 b = inputs.at(ID(B));
			కణుపు_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_జరుపు_కుడి(a, b) :
				logical_జరుపు_కుడి(a, b);
			తిరిగి factory.extend(y, y_వెడల్పు, a_signed);
		}లేకపోతే ఒకవేళ(cellType == ID($shiftx) || cellType == ID($జరుపు)){
			పూర్ణాంకం వెడల్పు = max(a_వెడల్పు, y_వెడల్పు);
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), వెడల్పు, cellType == ID($జరుపు) && a_signed);
			కణుపు_2 b = inputs.at(ID(B));
			కణుపు_2 shr = logical_జరుపు_కుడి(a, b);
			ఒకవేళ(b_signed) {
				కణుపు_2 shl = logical_జరుపు_ఎడమ(a, factory.unary_minus(b));
				కణుపు_2 y = factory.mux(shr, shl, sign(b));
				తిరిగి factory.extend(y, y_వెడల్పు, అబద్ధం);
			} లేకపోతే {
				తిరిగి factory.extend(shr, y_వెడల్పు, అబద్ధం);
			}
		}లేకపోతే ఒకవేళ(cellType == ID($mux)){
			తిరిగి factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}లేకపోతే ఒకవేళ(cellType == ID($pmux)){
			తిరిగి handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}లేకపోతే ఒకవేళ(cellType == ID($concat)){
			కణుపు_2 a = inputs.at(ID(A));
			కణుపు_2 b = inputs.at(ID(B));
			తిరిగి factory.concat(a, b);
		}లేకపోతే ఒకవేళ(cellType == ID($ముక్క)){
			పూర్ణాంకం స్థానభ్రంశం = parameters.at(ID(స్థానభ్రంశం_2)).as_int();
			కణుపు_2 a = inputs.at(ID(A));
			తిరిగి factory.ముక్క(a, స్థానభ్రంశం, y_వెడల్పు);
		}లేకపోతే ఒకవేళ(cellType.లోపల(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			పూర్ణాంకం వెడల్పు = max(a_వెడల్పు, b_వెడల్పు);
			తార్కిక is_signed = a_signed && b_signed;
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), వెడల్పు, is_signed);
			కణుపు_2 b = factory.extend(inputs.at(ID(B)), వెడల్పు, is_signed);
			ఒకవేళ(is_signed) {
				ఒకవేళ(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					కణుపు_2 abs_y = factory.unsigned_div(నిరపేక్ష(a), నిరపేక్ష(b));
					కణుపు_2 బహిర్గతం_sign = factory.not_equal(sign(a), sign(b));
					తిరిగి neg_if(factory.extend(abs_y, y_వెడల్పు, అబద్ధం), బహిర్గతం_sign);
				} లేకపోతే ఒకవేళ(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					కణుపు_2 abs_y = factory.unsigned_mod(నిరపేక్ష(a), నిరపేక్ష(b));
					తిరిగి neg_if(factory.extend(abs_y, y_వెడల్పు, అబద్ధం), sign(a));
				} లేకపోతే ఒకవేళ(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					కణుపు_2 b_sign = sign(b);
					కణుపు_2 a1 = neg_if(a, b_sign);
					కణుపు_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					కణుపు_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					కణుపు_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					కణుపు_2 y1 = factory.unsigned_div(a2, b1);
					కణుపు_2 y2 = factory.extend(y1, y_వెడల్పు, అబద్ధం);
					తిరిగి factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} లేకపోతే ఒకవేళ(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					కణుపు_2 abs_b = నిరపేక్ష(b);
					కణుపు_2 abs_y = factory.unsigned_mod(నిరపేక్ష(a), abs_b);
					కణుపు_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					కణుపు_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					కణుపు_2 y = neg_if(y_flipped, sign(b));
					తిరిగి factory.extend(y, y_వెడల్పు, నిజం);
				} లేకపోతే
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} లేకపోతే {
				ఒకవేళ(cellType.లోపల(ID($mod), ID($modfloor)))
					తిరిగి factory.extend(factory.unsigned_mod(a, b), y_వెడల్పు, అబద్ధం);
				లేకపోతే
					తిరిగి factory.extend(factory.unsigned_div(a, b), y_వెడల్పు, అబద్ధం);
			}
		} లేకపోతే ఒకవేళ(cellType == ID($ఘాతం)) {
			తిరిగి handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_వెడల్పు, a_signed && b_signed);
		} లేకపోతే ఒకవేళ (cellType == ID($lut)) {
			పూర్ణాంకం వెడల్పు = parameters.at(ID(వెడల్పు_2)).as_int();
			Const lut_పట్టిక = parameters.at(ID(LUT));
			lut_పట్టిక.extu(1 << వెడల్పు);
			తిరిగి handle_bmux(factory.constant(lut_పట్టిక), inputs.at(ID(A)), 0, 1, వెడల్పు);
		} లేకపోతే ఒకవేళ (cellType == ID($bwmux)) {
			కణుపు_2 a = inputs.at(ID(A));
			కణుపు_2 b = inputs.at(ID(B));
			కణుపు_2 s = inputs.at(ID(S));
			తిరిగి factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} లేకపోతే ఒకవేళ (cellType == ID($bweqx)) {
			కణుపు_2 a = inputs.at(ID(A));
			కణుపు_2 b = inputs.at(ID(B));
			తిరిగి factory.bitwise_not(factory.bitwise_xor(a, b));
		} లేకపోతే ఒకవేళ(cellType == ID($bmux)) {
			పూర్ణాంకం వెడల్పు = parameters.at(ID(వెడల్పు_2)).as_int();
			పూర్ణాంకం s_వెడల్పు = parameters.at(ID(S_వెడల్పు)).as_int();
			తిరిగి handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, వెడల్పు, s_వెడల్పు);
		} లేకపోతే ఒకవేళ(cellType == ID($demux)) {
			పూర్ణాంకం వెడల్పు = parameters.at(ID(వెడల్పు_2)).as_int();
			పూర్ణాంకం s_వెడల్పు = parameters.at(ID(S_వెడల్పు)).as_int();
			పూర్ణాంకం y_వెడల్పు = వెడల్పు << s_వెడల్పు;
			పూర్ణాంకం b_వెడల్పు = ceil_log2(y_వెడల్పు);
			కణుపు_2 a = factory.extend(inputs.at(ID(A)), y_వెడల్పు, అబద్ధం);
			కణుపు_2 s = factory.extend(inputs.at(ID(S)), b_వెడల్పు, అబద్ధం);
			కణుపు_2 b = factory.mul(s, factory.constant(Const(వెడల్పు, b_వెడల్పు)));
			తిరిగి factory.logical_జరుపు_ఎడమ(a, b);
		} లేకపోతే ఒకవేళ(cellType == ID($fa)) {
			తిరిగి handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} లేకపోతే ఒకవేళ(cellType == ID($lcu)) {
			తిరిగి handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} లేకపోతే ఒకవేళ(cellType == ID($alu)) {
			తిరిగి handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_వెడల్పు, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} లేకపోతే ఒకవేళ(cellType.లోపల(ID($ప్రతిజ్ఞా), ID($assume), ID($సజీవ), ID($fair), ID($cover))) {
			కణుపు_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			స్వయంచాలక &output = factory.కూడు_output(cellName, cellType, క్రమీకరించు_2(1));
			output.సమితి_విలువ(a);
			తిరిగి {};
		} లేకపోతే ఒకవేళ(cellType.లోపల(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			పూర్ణాంకం వెడల్పు = parameters.at(ID(వెడల్పు_2)).as_int();
			స్వయంచాలక &ఆదానం = factory.కూడు_ఆదానం(cellName, cellType, క్రమీకరించు_2(వెడల్పు));
			తిరిగి factory.విలువ(ఆదానం);
		} లేకపోతే ఒకవేళ(cellType == ID($initstate)) {
			ఒకవేళ(factory.ir().has_state(ID($initstate), ID($state)))
				తిరిగి factory.విలువ(factory.ir().state(ID($initstate)));
			లేకపోతే {
				స్వయంచాలక &state = factory.కూడు_state(ID($initstate), ID($state), క్రమీకరించు_2(1));
				state.సమితి_initial_విలువ(RTLIL::Const(State::S1, 1));
				state.సమితి_తదుపరి_విలువ(factory.constant(RTLIL::Const(State::S0, 1)));
				తిరిగి factory.విలువ(state);
			}
		} లేకపోతే ఒకవేళ(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} లేకపోతే {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

తరగతి FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> వరుస;
	dict<DriveSpec, కణుపు_2> లేఖాచిత్రం_nodes;
	dict<std::pair<Cell *, IdString>, కణుపు_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	సదిశ<Mem> memories_సదిశ;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	కణుపు_2 వరుసలో_చేర్చు(DriveSpec స్థిరరాశి &spec)
	{
		స్వయంచాలక it = లేఖాచిత్రం_nodes.కనుగొను(spec);
		ఒకవేళ(it == లేఖాచిత్రం_nodes.చివర()){
			స్వయంచాలక కణుపు = factory.create_pending(spec.కొలత());
			లేఖాచిత్రం_nodes.చొప్పించు({spec, కణుపు});
			వరుస.emplace_వెనుక(spec);
			తిరిగి కణుపు;
		}లేకపోతే
			తిరిగి it->రెండవ;
	}
	కణుపు_2 వరుసలో_చేర్చు_cell(Cell *cell, IdString పోర్టు_name)
	{
		స్వయంచాలక it = cell_outputs.కనుగొను({cell, పోర్టు_name});
		ఒకవేళ(it == cell_outputs.చివర()) {
			వరుస.emplace_వెనుక(cell);
			std::optional<కణుపు_2> rv;
			కొరకు(స్వయంచాలక స్థిరరాశి &[name, sigspec] : cell->connections())
				ఒకవేళ(driver_map.celltypes.cell_output(cell->type, name)) {
					స్వయంచాలక కణుపు = factory.create_pending(sigspec.కొలత());
					factory.suggest_name(కణుపు, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, కణుపు);
					ఒకవేళ(name == పోర్టు_name)
						rv = కణుపు;
				}
			తిరిగి *rv;
		} లేకపోతే
			తిరిగి it->రెండవ;
	}
బహిరంగ:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.కూడు(module);
		కొరకు (స్వయంచాలక cell : module->cells()) {
			ఒకవేళ (cell->type.లోపల(ID($ప్రతిజ్ఞా), ID($assume), ID($సజీవ), ID($fair), ID($cover), ID($check)))
				వరుస.emplace_వెనుక(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		కొరకు (స్వయంచాలక riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			స్వయంచాలక *wire = module->wire(*riter);
			ఒకవేళ (wire && wire->పోర్టు_ఆదానం) {
				factory.కూడు_ఆదానం(wire->name, ID($ఆదానం), క్రమీకరించు_2(wire->వెడల్పు));
			}
			ఒకవేళ (wire && wire->పోర్టు_output) {
				స్వయంచాలక &output = factory.కూడు_output(wire->name, ID($output), క్రమీకరించు_2(wire->వెడల్పు));
				output.సమితి_విలువ(వరుసలో_చేర్చు(DriveChunk(DriveChunkWire(wire, 0, wire->వెడల్పు))));
			}
		}
		memories_సదిశ = Mem::get_all_memories(module);
		కొరకు (స్వయంచాలక &mem : memories_సదిశ) {
			ఒకవేళ (mem.cell != శూన్యసూచిక)
				memories[mem.cell] = &mem;
		}
	}
అంతరంగ:
	కణుపు_2 concatenate_read_results(Mem *mem, సదిశ<కణుపు_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		స్థిరరాశి SigSpec &rd_దత్తాంశం = mem->cell->connections().at(ID(RD_దత్తాంశం));
		పూర్ణాంకం current = 0;
		కొరకు(కొలత_t i = 0; i < mem->rd_ports.కొలత(); i++) {
			పూర్ణాంకం వెడల్పు = mem->వెడల్పు << mem->rd_ports[i].wide_log2;
			log_assert (results[i].వెడల్పు() == వెడల్పు);
			log_assert (mem->rd_ports[i].దత్తాంశం == rd_దత్తాంశం.వెలికితీయి(current, వెడల్పు));
			current += వెడల్పు;
		}
		log_assert (current == rd_దత్తాంశం.కొలత());
		log_assert (!results.ఖాళీలు());
		కణుపు_2 కణుపు = results[0];
		కొరకు(కొలత_t i = 1; i < results.కొలత(); i++)
			కణుపు = factory.concat(కణుపు, results[i]);
		తిరిగి కణుపు;
	}
	కణుపు_2 handle_జ్ఞాపకం(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		సదిశ<కణుపు_2> read_results;
		స్వయంచాలక &state = factory.కూడు_state(mem->cell->name, ID($state), క్రమీకరించు_2(ceil_log2(mem->కొలత), mem->వెడల్పు));
		state.సమితి_initial_విలువ(MemContents(mem));
		కణుపు_2 కణుపు = factory.విలువ(state);
		కొరకు (కొలత_t i = 0; i < mem->wr_ports.కొలత(); i++) {
			స్థిరరాశి స్వయంచాలక &wr = mem->wr_ports[i];
			ఒకవేళ (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			కణుపు_2 en = వరుసలో_చేర్చు(driver_map(DriveSpec(wr.en)));
			కణుపు_2 addr = వరుసలో_చేర్చు(driver_map(DriveSpec(wr.addr)));
			కణుపు_2 new_దత్తాంశం = వరుసలో_చేర్చు(driver_map(DriveSpec(wr.దత్తాంశం)));
			కణుపు_2 old_దత్తాంశం = factory.జ్ఞాపకం_read(కణుపు, addr);
			కణుపు_2 wr_దత్తాంశం = simplifier.bitwise_mux(old_దత్తాంశం, new_దత్తాంశం, en);
			కణుపు = factory.జ్ఞాపకం_write(కణుపు, addr, wr_దత్తాంశం);
		}
		ఒకవేళ (mem->rd_ports.ఖాళీలు())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		కొరకు (కొలత_t i = 0; i < mem->rd_ports.కొలత(); i++) {
			స్థిరరాశి స్వయంచాలక &rd = mem->rd_ports[i];
			ఒకవేళ (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			కణుపు_2 addr = వరుసలో_చేర్చు(driver_map(DriveSpec(rd.addr)));
			read_results.నెట్టు_వెనుక(factory.జ్ఞాపకం_read(కణుపు, addr));
		}
		state.సమితి_తదుపరి_విలువ(కణుపు);
		తిరిగి concatenate_read_results(mem, read_results);
	}
	శూన్య process_cell(Cell *cell)
	{
		ఒకవేళ (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, శూన్యసూచిక);
			ఒకవేళ (mem == శూన్యసూచిక) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			కణుపు_2 కణుపు = handle_జ్ఞాపకం(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_దత్తాంశం)}), కణుపు);
		} లేకపోతే ఒకవేళ (cell->is_builtin_ff()) {
			Ff_దత్తాంశం ff(&ff_initvals, cell);
			ఒకవేళ (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			స్వయంచాలక &state = factory.కూడు_state(ff.name, ID($state), క్రమీకరించు_2(ff.వెడల్పు));
			కణుపు_2 q_విలువ = factory.విలువ(state);
			factory.suggest_name(q_విలువ, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_విలువ);
			state.సమితి_తదుపరి_విలువ(వరుసలో_చేర్చు(ff.sig_d));
			state.సమితి_initial_విలువ(ff.val_init);
		} లేకపోతే {
			dict<IdString, కణుపు_2> connections;
			IdString output_name; // for the single output case
			పూర్ణాంకం n_outputs = 0;
			కొరకు(స్వయంచాలక స్థిరరాశి &[name, sigspec] : cell->connections()) {
				ఒకవేళ(driver_map.celltypes.cell_ఆదానం(cell->type, name) && sigspec.కొలత() > 0)
					connections.చొప్పించు({ name, వరుసలో_చేర్చు(Drive_Chunk_పోర్టు(cell, {name, sigspec})) });
				ఒకవేళ(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, కణుపు_2>, కణుపు_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			ఒకవేళ(స్వయంచాలక *nodep = std::get_if<కణుపు_2>(&outputs); nodep != శూన్యసూచిక) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} లేకపోతే {
				కొరకు(స్వయంచాలక [name, కణుపు] : std::get<dict<IdString, కణుపు_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), కణుపు);
			}
		}
	}
	శూన్య undriven(స్థిరరాశి std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	శూన్య check_undriven(DriveSpec స్థిరరాశి& spec, std::string స్థిరరాశి& name) {
		కొరకు(స్వయంచాలక స్థిరరాశి &chunk : spec.chunks())
			ఒకవేళ(chunk.is_none())
				undriven(name);
	}
బహిరంగ:
	శూన్య process_వరుస()
	{
		కొరకు (; !వరుస.ఖాళీలు(); వరుస.తీయి_front()) {
			ఒకవేళ(స్వయంచాలక p = std::get_if<Cell *>(&వరుస.front()); p != శూన్యసూచిక) {
				process_cell(*p);
				కొనసాగు;
			}

			DriveSpec spec = std::get<DriveSpec>(వరుస.front());
			కణుపు_2 pending = లేఖాచిత్రం_nodes.at(spec);

			ఒకవేళ (spec.chunks().కొలత() > 1) {
				స్వయంచాలక chunks = spec.chunks();
				కణుపు_2 కణుపు = వరుసలో_చేర్చు(chunks[0]);
				కొరకు(కొలత_t i = 1; i < chunks.కొలత(); i++)
					కణుపు = factory.concat(కణుపు, వరుసలో_చేర్చు(chunks[i]));
				factory.update_pending(pending, కణుపు);
			} లేకపోతే ఒకవేళ (spec.chunks().కొలత() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				ఒకవేళ (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					ఒకవేళ (wire_chunk.is_సంపూర్ణ()) {
						ఒకవేళ (wire_chunk.wire->పోర్టు_ఆదానం) {
							కణుపు_2 కణుపు = factory.విలువ(factory.ir().ఆదానం(wire_chunk.wire->name));
							factory.suggest_name(కణుపు, wire_chunk.wire->name);
							factory.update_pending(pending, కణుపు);
						} లేకపోతే {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							కణుపు_2 కణుపు = వరుసలో_చేర్చు(driver);
							factory.suggest_name(కణుపు, wire_chunk.wire->name);
							factory.update_pending(pending, కణుపు);
						}
					} లేకపోతే {
						DriveChunkWire సంపూర్ణ_wire(wire_chunk.wire, 0, wire_chunk.wire->వెడల్పు);
						కణుపు_2 కణుపు = factory.ముక్క(వరుసలో_చేర్చు(సంపూర్ణ_wire), wire_chunk.స్థానభ్రంశం, wire_chunk.వెడల్పు);
						factory.update_pending(pending, కణుపు);
					}
				} లేకపోతే ఒకవేళ (chunk.is_పోర్టు()) {
					Drive_Chunk_పోర్టు పోర్టు_chunk = chunk.పోర్టు();
					ఒకవేళ (పోర్టు_chunk.is_సంపూర్ణ()) {
						ఒకవేళ (driver_map.celltypes.cell_output(పోర్టు_chunk.cell->type, పోర్టు_chunk.పోర్టు)) {
							కణుపు_2 కణుపు = వరుసలో_చేర్చు_cell(పోర్టు_chunk.cell, పోర్టు_chunk.పోర్టు);
							factory.update_pending(pending, కణుపు);
						} లేకపోతే {
							DriveSpec driver = driver_map(DriveSpec(పోర్టు_chunk));
							check_undriven(driver, పోర్టు_chunk.cell->name.unescape() + " port " + PooledName(పోర్టు_chunk.cell->module, పోర్టు_chunk.పోర్టు).unescape());
							factory.update_pending(pending, వరుసలో_చేర్చు(driver));
						}
					} లేకపోతే {
						Drive_Chunk_పోర్టు సంపూర్ణ_పోర్టు(పోర్టు_chunk.cell, పోర్టు_chunk.పోర్టు, 0, Get_కొలత(పోర్టు_chunk.cell->connections().at(పోర్టు_chunk.పోర్టు)));
						కణుపు_2 కణుపు = factory.ముక్క(వరుసలో_చేర్చు(సంపూర్ణ_పోర్టు), పోర్టు_chunk.స్థానభ్రంశం, పోర్టు_chunk.వెడల్పు);
						factory.update_pending(pending, కణుపు);
					}
				} లేకపోతే ఒకవేళ (chunk.is_constant()) {
					కణుపు_2 కణుపు = factory.constant(chunk.constant());
					factory.suggest_name(కణుపు, "$const" + std::గమ్యం_string(chunk.కొలత()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, కణుపు);
				} లేకపోతే ఒకవేళ (chunk.is_గుణిజం()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} లేకపోతే ఒకవేళ (chunk.is_none()) {
					undriven(log_signal(chunk));
				} లేకపోతే {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} లేకపోతే {
				log_abort();
			}
		}
	}
};

IR IR::from_module(Module *module) {
	IR ir;
    ir.design = module->design;
    స్వయంచాలక factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_వరుస();
    ir.topological_క్రమీకరించు();
    ir.forward_buf();
    తిరిగి ir;
}

శూన్య IR::topological_క్రమీకరించు() {
    లేఖాచిత్రం::SccAdaptor compute_లేఖాచిత్రం_scc(లేఖాచిత్రం_4);
    తార్కిక scc = అబద్ధం;
    std::సదిశ<పూర్ణాంకం> perm;
    TopoSortedSccs toposort(compute_లేఖాచిత్రం_scc, [&](పూర్ణాంకం *begin, పూర్ణాంకం *చివర) {
        perm.చొప్పించు(perm.చివర(), begin, చివర);
        ఒకవేళ (చివర > begin + 1)
        {
            log_warning("Combinational loop:\n");
            కొరకు (పూర్ణాంకం *i = begin; i != చివర; ++i) {
				కణుపు_2 కణుపు(లేఖాచిత్రం_4[*i], design);
                సంవర్గమానం("- %s = %s\n", కణుపు.name().unescape(), కణుపు.గమ్యం_string());
			}
            సంవర్గమానం("\n");
            scc = నిజం;
        }
    });
	కొరకు(స్థిరరాశి స్వయంచాలక &[name, state]: స్థితులు_2)
		ఒకవేళ(state.has_తదుపరి_విలువ())
			toposort.process(state.తదుపరి_విలువ().id());
	కొరకు(స్థిరరాశి స్వయంచాలక &[name, output]: _outputs)
		ఒకవేళ(output.has_విలువ())
			toposort.process(output.విలువ().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    లేఖాచిత్రం_4.క్రమమార్పు(perm);
    ఒకవేళ(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

స్థిర IdString విలీనం_name(IdString a, IdString b) {
	ఒకవేళ(!a.is_ప్రజా() && b.is_ప్రజా())
		తిరిగి b;
	లేకపోతే
		తిరిగి a;
}

శూన్య IR::forward_buf() {
    std::సదిశ<పూర్ణాంకం> perm, alias;
    perm.clear();

    కొరకు (పూర్ణాంకం i = 0; i < లేఖాచిత్రం_4.కొలత(); ++i)
    {
        స్వయంచాలక కణుపు = లేఖాచిత్రం_4[i];
        ఒకవేళ (కణుపు.function().fn() == Fn::buf && కణుపు.arg(0).సూచీ() < i)
        {
            పూర్ణాంకం లక్ష్యం_సూచీ = alias[కణుపు.arg(0).సూచీ()];
            స్వయంచాలక లక్ష్యం_కణుపు = లేఖాచిత్రం_4[perm[లక్ష్యం_సూచీ]];
			ఒకవేళ(కణుపు.has_sparse_attr()) {
				ఒకవేళ(లక్ష్యం_కణుపు.has_sparse_attr()) {
					IdString id = విలీనం_name(కణుపు.sparse_attr(), లక్ష్యం_కణుపు.sparse_attr());
					లక్ష్యం_కణుపు.sparse_attr() = id;
				} లేకపోతే {
					IdString id = కణుపు.sparse_attr();
					లక్ష్యం_కణుపు.sparse_attr() = id;
				}
			}
            alias.నెట్టు_వెనుక(లక్ష్యం_సూచీ);
        }
        లేకపోతే
        {
            alias.నెట్టు_వెనుక(Get_కొలత(perm));
            perm.నెట్టు_వెనుక(i);
        }
    }
    లేఖాచిత్రం_4.క్రమమార్పు(perm, alias);
}

// Quoting routine to make error messages nicer
స్థిర std::string quote_fmt(స్థిరరాశి అక్షరం *fmt)
{
	std::string r;
	కొరకు(స్థిరరాశి అక్షరం *p = fmt; *p != 0; p++) {
		ఎంపిక(*p) {
		స్థితి '\n': r += "\\n"; ఆపు;
		స్థితి '\t': r += "\\t"; ఆపు;
		స్థితి '"': r += "\\\""; ఆపు;
		స్థితి '\\': r += "\\\\"; ఆపు;
		అప్రమేయ: r += *p; ఆపు;
		}
	}
	తిరిగి r;
}

శూన్య Writer::print_impl(స్థిరరాశి అక్షరం *fmt, సదిశ<std::function<శూన్య()>> &fns)
{
	కొలత_t తదుపరి_సూచీ = 0;
	కొరకు(స్థిరరాశి అక్షరం *p = fmt; *p != 0; p++)
		ఎంపిక(*p) {
		స్థితి '{':
			ఒకవేళ(*++p == '{') {
				*os << '{';
			} లేకపోతే {
				అక్షరం *pe;
				కొలత_t సూచీ = strtoul(p, &pe, 10);
				ఒకవేళ(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				ఒకవేళ(p == pe)
					సూచీ = తదుపరి_సూచీ;
				లేకపోతే
					p = pe;
				ఒకవేళ(సూచీ >= fns.కొలత())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", సూచీ, fns.కొలత(), quote_fmt(fmt));
				fns[సూచీ]();
				తదుపరి_సూచీ = సూచీ + 1;
			}
			ఆపు;
		స్థితి '}':
			p++;
			ఒకవేళ(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			ఆపు;
		అప్రమేయ:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_చివర
