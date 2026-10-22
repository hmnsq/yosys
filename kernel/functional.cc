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

#شامل "kernel/functional.h"
#شامل "kernel/topo_scc.h"
#شامل "ff.h"
#شامل "ffinit.h"
#شامل <deque>

YOSYS_NAMESPACE_BEGIN
نامخلا Functional {

ثابت حرف *fn_مقصد_string(Fn fn) {
	انتخاب(fn) {
	حالت Fn::invalid: واپس "invalid";
	حالت Fn::buf: واپس "buf";
	حالت Fn::قاش: واپس "slice";
	حالت Fn::zero_extend: واپس "zero_extend";
	حالت Fn::sign_extend: واپس "sign_extend";
	حالت Fn::concat: واپس "concat";
	حالت Fn::جوڑو: واپس "add";
	حالت Fn::sub: واپس "sub";
	حالت Fn::mul: واپس "mul";
	حالت Fn::unsigned_div: واپس "unsigned_div";
	حالت Fn::unsigned_mod: واپس "unsigned_mod";
	حالت Fn::bitwise_and: واپس "bitwise_and";
	حالت Fn::bitwise_or: واپس "bitwise_or";
	حالت Fn::bitwise_xor: واپس "bitwise_xor";
	حالت Fn::bitwise_not: واپس "bitwise_not";
	حالت Fn::reduce_and: واپس "reduce_and";
	حالت Fn::reduce_or: واپس "reduce_or";
	حالت Fn::reduce_xor: واپس "reduce_xor";
	حالت Fn::unary_minus: واپس "unary_minus";
	حالت Fn::equal: واپس "equal";
	حالت Fn::not_equal: واپس "not_equal";
	حالت Fn::signed_greater_than: واپس "signed_greater_than";
	حالت Fn::signed_greater_equal: واپس "signed_greater_equal";
	حالت Fn::unsigned_greater_than: واپس "unsigned_greater_than";
	حالت Fn::unsigned_greater_equal: واپس "unsigned_greater_equal";
	حالت Fn::logical_سرکاؤ_بایاں: واپس "logical_shift_left";
	حالت Fn::logical_سرکاؤ_دایاں: واپس "logical_shift_right";
	حالت Fn::arithmetic_سرکاؤ_دایاں: واپس "arithmetic_shift_right";
	حالت Fn::mux: واپس "mux";
	حالت Fn::constant: واپس "constant";
	حالت Fn::داخلہ: واپس "input";
	حالت Fn::state: واپس "state";
	حالت Fn::حافظہ_read: واپس "memory_read";
	حالت Fn::حافظہ_write: واپس "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحیح)fn);
}

سمتیہ<IRInput ثابت*> IR::inputs(IdString قسم) ثابت {
	سمتیہ<IRInput ثابت*> ret;
	برائے (ثابت خودکار &[name, داخلہ] : _inputs)
		اگر(داخلہ.قسم == قسم)
			ret.ڈالو_عقب(&داخلہ);
	واپس ret;
}

سمتیہ<IROutput ثابت*> IR::outputs(IdString قسم) ثابت {
	سمتیہ<IROutput ثابت*> ret;
	برائے (ثابت خودکار &[name, output] : _outputs)
		اگر(output.قسم == قسم)
			ret.ڈالو_عقب(&output);
	واپس ret;
}

سمتیہ<IRState ثابت*> IR::حالتیں(IdString قسم) ثابت {
	سمتیہ<IRState ثابت*> ret;
	برائے (ثابت خودکار &[name, state] : حالتیں_2)
		اگر(state.قسم == قسم)
			ret.ڈالو_عقب(&state);
	واپس ret;
}

سمتیہ<IRInput ثابت*> IR::all_inputs() ثابت {
	سمتیہ<IRInput ثابت*> ret;
	برائے (ثابت خودکار &[name, داخلہ] : _inputs)
		ret.ڈالو_عقب(&داخلہ);
	واپس ret;
}

سمتیہ<IROutput ثابت*> IR::all_outputs() ثابت {
	سمتیہ<IROutput ثابت*> ret;
	برائے (ثابت خودکار &[name, output] : _outputs)
		ret.ڈالو_عقب(&output);
	واپس ret;
}

سمتیہ<IRState ثابت*> IR::all_حالتیں() ثابت {
	سمتیہ<IRState ثابت*> ret;
	برائے (ثابت خودکار &[name, state] : حالتیں_2)
		ret.ڈالو_عقب(&state);
	واپس ret;
}

ساخت PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(گرہ_2)> np;
	PrintVisitor(std::function<std::string(گرہ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string قاش(گرہ_2, گرہ_2 a, صحیح انحراف, صحیح برآمد_چوڑائی) override { واپس "slice(" + np(a) + ", " + std::مقصد_string(انحراف) + ", " + std::مقصد_string(برآمد_چوڑائی) + ")"; }
	std::string zero_extend(گرہ_2, گرہ_2 a, صحیح برآمد_چوڑائی) override { واپس "zero_extend(" + np(a) + ", " + std::مقصد_string(برآمد_چوڑائی) + ")"; }
	std::string sign_extend(گرہ_2, گرہ_2 a, صحیح برآمد_چوڑائی) override { واپس "sign_extend(" + np(a) + ", " + std::مقصد_string(برآمد_چوڑائی) + ")"; }
	std::string constant(گرہ_2, RTLIL::Const ثابت& قدر) override { واپس "constant(" + قدر.as_string() + ")"; }
	std::string داخلہ(گرہ_2 self, IdString name, IdString قسم) override { واپس "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(قسم) + ")"; }
	std::string state(گرہ_2 self, IdString name, IdString قسم) override { واپس "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(قسم) + ")"; }
	std::string default_handler(گرہ_2 self) override {
		std::string ret = fn_مقصد_string(self.fn());
		ret += "(";
		برائے(سائز_t i = 0; i < self.arg_گنتی(); i++) {
			اگر(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		واپس ret;
	}
};

std::string گرہ_2::مقصد_string()
{
	واپس مقصد_string([](گرہ_2 n) { واپس n.name().unescape(); });
}

std::string گرہ_2::مقصد_string(std::function<std::string(گرہ_2)> np)
{
	واپس دورہ(PrintVisitor(np));
}

جماعت CellSimplifier {
	Factory &factory;
	گرہ_2 sign(گرہ_2 a) {
		واپس factory.قاش(a, a.چوڑائی() - 1, 1);
	}
	گرہ_2 neg_if(گرہ_2 a, گرہ_2 s) {
		واپس factory.mux(a, factory.unary_minus(a), s);
	}
	گرہ_2 مطلق_قدر(گرہ_2 a) {
		واپس neg_if(a, sign(a));
	}
	گرہ_2 handle_سرکاؤ(گرہ_2 a, گرہ_2 b, منطقی is_دایاں, منطقی is_signed) {
		// to prevent new_width == 0, we handle this case separately
		اگر(a.چوڑائی() == 1) {
			اگر(!is_signed)
				واپس factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			ورنہ
				واپس a;
		}
		صحیح new_چوڑائی = ceil_log2(a.چوڑائی());
		گرہ_2 b_truncated = factory.extend(b, new_چوڑائی, جھوٹ);
		گرہ_2 y =
			!is_دایاں ? factory.logical_سرکاؤ_بایاں(a, b_truncated) :
			!is_signed ? factory.logical_سرکاؤ_دایاں(a, b_truncated) :
			factory.arithmetic_سرکاؤ_دایاں(a, b_truncated);
		اگر(b.چوڑائی() <= new_چوڑائی)
			واپس y;
		گرہ_2 حد_سے_تجاوز = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.چوڑائی(), b.چوڑائی())));
		گرہ_2 y_if_حد_سے_تجاوز = is_signed ? factory.extend(sign(a), a.چوڑائی(), سچ) : factory.constant(RTLIL::Const(State::S0, a.چوڑائی()));
		واپس factory.mux(y, y_if_حد_سے_تجاوز, حد_سے_تجاوز);
	}
عوامی:
	گرہ_2 logical_سرکاؤ_بایاں(گرہ_2 a, گرہ_2 b) { واپس handle_سرکاؤ(a, b, جھوٹ, جھوٹ); }
	گرہ_2 logical_سرکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { واپس handle_سرکاؤ(a, b, سچ, جھوٹ); }
	گرہ_2 arithmetic_سرکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { واپس handle_سرکاؤ(a, b, سچ, سچ); }
	گرہ_2 bitwise_mux(گرہ_2 a, گرہ_2 b, گرہ_2 s) {
		گرہ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		گرہ_2 bb = factory.bitwise_and(b, s);
		واپس factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
نجی:
	گرہ_2 handle_pow(گرہ_2 a0, گرہ_2 b, صحیح y_چوڑائی, منطقی is_signed) {
		گرہ_2 a = factory.extend(a0, y_چوڑائی, is_signed);
		گرہ_2 r = factory.constant(Const(1, y_چوڑائی));
		برائے(صحیح i = 0; i < b.چوڑائی(); i++) {
			گرہ_2 b_ثنائی_ہندسہ = factory.قاش(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_ثنائی_ہندسہ);
			a = factory.mul(a, a);
		}
		اگر (is_signed) {
			گرہ_2 a_ge_1 = factory.unsigned_greater_than(مطلق_قدر(a0), factory.constant(Const(1, a0.چوڑائی())));
			گرہ_2 zero_نتیجہ = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_چوڑائی)), zero_نتیجہ);
		}
		واپس r;
	}
	گرہ_2 handle_bmux(گرہ_2 a, گرہ_2 s, صحیح a_انحراف, صحیح چوڑائی, صحیح sn) {
		اگر(sn < 1)
			واپس factory.قاش(a, a_انحراف, چوڑائی);
		ورنہ {
			گرہ_2 y0 = handle_bmux(a, s, a_انحراف, چوڑائی, sn - 1);
			گرہ_2 y1 = handle_bmux(a, s, a_انحراف + (چوڑائی << (sn - 1)), چوڑائی, sn - 1);
			واپس factory.mux(y0, y1, factory.قاش(s, sn - 1, 1));
		}
	}
	گرہ_2 handle_pmux(گرہ_2 a, گرہ_2 b, گرہ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.چوڑائی() == a.چوڑائی() * s.چوڑائی());
		گرہ_2 y = a;
		برائے(صحیح i = 0; i < s.چوڑائی(); i++)
			y = factory.mux(y, factory.قاش(b, a.چوڑائی() * i, a.چوڑائی()), factory.قاش(s, i, 1));
		واپس y;
	}
	dict<IdString, گرہ_2> handle_fa(گرہ_2 a, گرہ_2 b, گرہ_2 c) {
		گرہ_2 t1 = factory.bitwise_xor(a, b);
		گرہ_2 t2 = factory.bitwise_and(a, b);
		گرہ_2 t3 = factory.bitwise_and(c, t1);
		گرہ_2 y = factory.bitwise_xor(c, t1);
		گرہ_2 x = factory.bitwise_or(t2, t3);
		واپس {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, گرہ_2> handle_alu(گرہ_2 a_اندر, گرہ_2 b_اندر, صحیح y_چوڑائی, منطقی is_signed, گرہ_2 ci, گرہ_2 bi) {
		گرہ_2 a = factory.extend(a_اندر, y_چوڑائی, is_signed);
		گرہ_2 b_uninverted = factory.extend(b_اندر, y_چوڑائی, is_signed);
		گرہ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		گرہ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		گرہ_2 a_extra = factory.extend(a, y_چوڑائی + 1, جھوٹ);
		گرہ_2 b_extra = factory.extend(b, y_چوڑائی + 1, جھوٹ);
		گرہ_2 y_extra = factory.جوڑو(factory.جوڑو(a_extra, b_extra), factory.extend(ci, a.چوڑائی() + 1, جھوٹ));
		گرہ_2 y = factory.قاش(y_extra, 0, y_چوڑائی);
		گرہ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		گرہ_2 co = factory.قاش(carries, 1, y_چوڑائی);
		واپس {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	گرہ_2 handle_lcu(گرہ_2 p, گرہ_2 g, گرہ_2 ci) {
		واپس handle_alu(g, factory.bitwise_or(p, g), g.چوڑائی(), جھوٹ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عوامی:
	std::variant<dict<IdString, گرہ_2>, گرہ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, گرہ_2> inputs)
	{
		صحیح a_چوڑائی = parameters.at(ID(A_چوڑائی), Const(-1)).as_int();
		صحیح b_چوڑائی = parameters.at(ID(B_چوڑائی), Const(-1)).as_int();
		صحیح y_چوڑائی = parameters.at(ID(Y_چوڑائی), Const(-1)).as_int();
		منطقی a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقی b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		اگر(cellType.اندر(ID($جوڑو), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقی is_signed = a_signed && b_signed;
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, is_signed);
			گرہ_2 b = factory.extend(inputs.at(ID(B)), y_چوڑائی, is_signed);
			اگر(cellType == ID($جوڑو))
				واپس factory.جوڑو(a, b);
			ورنہ اگر(cellType == ID($sub))
				واپس factory.sub(a, b);
			ورنہ اگر(cellType == ID($mul))
				واپس factory.mul(a, b);
			ورنہ اگر(cellType == ID($and))
				واپس factory.bitwise_and(a, b);
			ورنہ اگر(cellType == ID($or))
				واپس factory.bitwise_or(a, b);
			ورنہ اگر(cellType == ID($xor))
				واپس factory.bitwise_xor(a, b);
			ورنہ اگر(cellType == ID($xnor))
				واپس factory.bitwise_not(factory.bitwise_xor(a, b));
			ورنہ
				log_abort();
		}ورنہ اگر(cellType.اندر(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقی is_signed = a_signed && b_signed;
			صحیح چوڑائی = max(a_چوڑائی, b_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, is_signed);
			گرہ_2 b = factory.extend(inputs.at(ID(B)), چوڑائی, is_signed);
			اگر(cellType.اندر(ID($eq), ID($eqx)))
				واپس factory.extend(factory.equal(a, b), y_چوڑائی, جھوٹ);
			ورنہ اگر(cellType.اندر(ID($ne), ID($nex)))
				واپس factory.extend(factory.not_equal(a, b), y_چوڑائی, جھوٹ);
			ورنہ اگر(cellType == ID($lt))
				واپس factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_چوڑائی, جھوٹ);
			ورنہ اگر(cellType == ID($le))
				واپس factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_چوڑائی, جھوٹ);
			ورنہ اگر(cellType == ID($gt))
				واپس factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_چوڑائی, جھوٹ);
			ورنہ اگر(cellType == ID($ge))
				واپس factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_چوڑائی, جھوٹ);
			ورنہ
				log_abort();
		}ورنہ اگر(cellType.اندر(ID($logic_or), ID($logic_and))){
			گرہ_2 a = factory.reduce_or(inputs.at(ID(A)));
			گرہ_2 b = factory.reduce_or(inputs.at(ID(B)));
			گرہ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			واپس factory.extend(y, y_چوڑائی, جھوٹ);
		}ورنہ اگر(cellType == ID($not)){
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
			واپس factory.bitwise_not(a);
		}ورنہ اگر(cellType == ID($pos)){
			واپس factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
		}ورنہ اگر(cellType == ID($barrier)){
			واپس inputs.at(ID(A));
		}ورنہ اگر(cellType == ID($neg)){
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
			واپس factory.unary_minus(a);
		}ورنہ اگر(cellType == ID($logic_not)){
			گرہ_2 a = factory.reduce_or(inputs.at(ID(A)));
			گرہ_2 y = factory.bitwise_not(a);
			واپس factory.extend(y, y_چوڑائی, جھوٹ);
		}ورنہ اگر(cellType.اندر(ID($reduce_or), ID($reduce_bool))){
			گرہ_2 a = factory.reduce_or(inputs.at(ID(A)));
			واپس factory.extend(a, y_چوڑائی, جھوٹ);
		}ورنہ اگر(cellType == ID($reduce_and)){
			گرہ_2 a = factory.reduce_and(inputs.at(ID(A)));
			واپس factory.extend(a, y_چوڑائی, جھوٹ);
		}ورنہ اگر(cellType.اندر(ID($reduce_xor), ID($reduce_xnor))){
			گرہ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			گرہ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			واپس factory.extend(y, y_چوڑائی, جھوٹ);
		}ورنہ اگر(cellType == ID($shl) || cellType == ID($sshl)){
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
			گرہ_2 b = inputs.at(ID(B));
			واپس logical_سرکاؤ_بایاں(a, b);
		}ورنہ اگر(cellType == ID($shr) || cellType == ID($sshr)){
			صحیح چوڑائی = max(a_چوڑائی, y_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, a_signed);
			گرہ_2 b = inputs.at(ID(B));
			گرہ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_سرکاؤ_دایاں(a, b) :
				logical_سرکاؤ_دایاں(a, b);
			واپس factory.extend(y, y_چوڑائی, a_signed);
		}ورنہ اگر(cellType == ID($shiftx) || cellType == ID($سرکاؤ)){
			صحیح چوڑائی = max(a_چوڑائی, y_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, cellType == ID($سرکاؤ) && a_signed);
			گرہ_2 b = inputs.at(ID(B));
			گرہ_2 shr = logical_سرکاؤ_دایاں(a, b);
			اگر(b_signed) {
				گرہ_2 shl = logical_سرکاؤ_بایاں(a, factory.unary_minus(b));
				گرہ_2 y = factory.mux(shr, shl, sign(b));
				واپس factory.extend(y, y_چوڑائی, جھوٹ);
			} ورنہ {
				واپس factory.extend(shr, y_چوڑائی, جھوٹ);
			}
		}ورنہ اگر(cellType == ID($mux)){
			واپس factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ورنہ اگر(cellType == ID($pmux)){
			واپس handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ورنہ اگر(cellType == ID($concat)){
			گرہ_2 a = inputs.at(ID(A));
			گرہ_2 b = inputs.at(ID(B));
			واپس factory.concat(a, b);
		}ورنہ اگر(cellType == ID($قاش)){
			صحیح انحراف = parameters.at(ID(انحراف_2)).as_int();
			گرہ_2 a = inputs.at(ID(A));
			واپس factory.قاش(a, انحراف, y_چوڑائی);
		}ورنہ اگر(cellType.اندر(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحیح چوڑائی = max(a_چوڑائی, b_چوڑائی);
			منطقی is_signed = a_signed && b_signed;
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, is_signed);
			گرہ_2 b = factory.extend(inputs.at(ID(B)), چوڑائی, is_signed);
			اگر(is_signed) {
				اگر(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					گرہ_2 abs_y = factory.unsigned_div(مطلق_قدر(a), مطلق_قدر(b));
					گرہ_2 برآمد_sign = factory.not_equal(sign(a), sign(b));
					واپس neg_if(factory.extend(abs_y, y_چوڑائی, جھوٹ), برآمد_sign);
				} ورنہ اگر(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					گرہ_2 abs_y = factory.unsigned_mod(مطلق_قدر(a), مطلق_قدر(b));
					واپس neg_if(factory.extend(abs_y, y_چوڑائی, جھوٹ), sign(a));
				} ورنہ اگر(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					گرہ_2 b_sign = sign(b);
					گرہ_2 a1 = neg_if(a, b_sign);
					گرہ_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					گرہ_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					گرہ_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					گرہ_2 y1 = factory.unsigned_div(a2, b1);
					گرہ_2 y2 = factory.extend(y1, y_چوڑائی, جھوٹ);
					واپس factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} ورنہ اگر(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					گرہ_2 abs_b = مطلق_قدر(b);
					گرہ_2 abs_y = factory.unsigned_mod(مطلق_قدر(a), abs_b);
					گرہ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					گرہ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					گرہ_2 y = neg_if(y_flipped, sign(b));
					واپس factory.extend(y, y_چوڑائی, سچ);
				} ورنہ
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} ورنہ {
				اگر(cellType.اندر(ID($mod), ID($modfloor)))
					واپس factory.extend(factory.unsigned_mod(a, b), y_چوڑائی, جھوٹ);
				ورنہ
					واپس factory.extend(factory.unsigned_div(a, b), y_چوڑائی, جھوٹ);
			}
		} ورنہ اگر(cellType == ID($قوت)) {
			واپس handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_چوڑائی, a_signed && b_signed);
		} ورنہ اگر (cellType == ID($lut)) {
			صحیح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			Const lut_جدول = parameters.at(ID(LUT));
			lut_جدول.extu(1 << چوڑائی);
			واپس handle_bmux(factory.constant(lut_جدول), inputs.at(ID(A)), 0, 1, چوڑائی);
		} ورنہ اگر (cellType == ID($bwmux)) {
			گرہ_2 a = inputs.at(ID(A));
			گرہ_2 b = inputs.at(ID(B));
			گرہ_2 s = inputs.at(ID(S));
			واپس factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} ورنہ اگر (cellType == ID($bweqx)) {
			گرہ_2 a = inputs.at(ID(A));
			گرہ_2 b = inputs.at(ID(B));
			واپس factory.bitwise_not(factory.bitwise_xor(a, b));
		} ورنہ اگر(cellType == ID($bmux)) {
			صحیح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			صحیح s_چوڑائی = parameters.at(ID(S_چوڑائی)).as_int();
			واپس handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, چوڑائی, s_چوڑائی);
		} ورنہ اگر(cellType == ID($demux)) {
			صحیح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			صحیح s_چوڑائی = parameters.at(ID(S_چوڑائی)).as_int();
			صحیح y_چوڑائی = چوڑائی << s_چوڑائی;
			صحیح b_چوڑائی = ceil_log2(y_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, جھوٹ);
			گرہ_2 s = factory.extend(inputs.at(ID(S)), b_چوڑائی, جھوٹ);
			گرہ_2 b = factory.mul(s, factory.constant(Const(چوڑائی, b_چوڑائی)));
			واپس factory.logical_سرکاؤ_بایاں(a, b);
		} ورنہ اگر(cellType == ID($fa)) {
			واپس handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} ورنہ اگر(cellType == ID($lcu)) {
			واپس handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} ورنہ اگر(cellType == ID($alu)) {
			واپس handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_چوڑائی, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} ورنہ اگر(cellType.اندر(ID($أكد), ID($assume), ID($زندہ), ID($fair), ID($cover))) {
			گرہ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			خودکار &output = factory.جوڑو_output(cellName, cellType, ترتیب_دو_2(1));
			output.سیٹ_قدر(a);
			واپس {};
		} ورنہ اگر(cellType.اندر(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحیح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			خودکار &داخلہ = factory.جوڑو_داخلہ(cellName, cellType, ترتیب_دو_2(چوڑائی));
			واپس factory.قدر(داخلہ);
		} ورنہ اگر(cellType == ID($initstate)) {
			اگر(factory.ir().has_state(ID($initstate), ID($state)))
				واپس factory.قدر(factory.ir().state(ID($initstate)));
			ورنہ {
				خودکار &state = factory.جوڑو_state(ID($initstate), ID($state), ترتیب_دو_2(1));
				state.سیٹ_initial_قدر(RTLIL::Const(State::S1, 1));
				state.سیٹ_اگلا_قدر(factory.constant(RTLIL::Const(State::S0, 1)));
				واپس factory.قدر(state);
			}
		} ورنہ اگر(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} ورنہ {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

جماعت FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> قطار;
	dict<DriveSpec, گرہ_2> گراف_nodes;
	dict<std::pair<Cell *, IdString>, گرہ_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	سمتیہ<Mem> memories_سمتیہ;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	گرہ_2 قطار_میں_ڈالو(DriveSpec ثابت &spec)
	{
		خودکار it = گراف_nodes.ڈھونڈو(spec);
		اگر(it == گراف_nodes.انتہا()){
			خودکار گرہ = factory.create_pending(spec.سائز());
			گراف_nodes.داخل_کرو({spec, گرہ});
			قطار.emplace_عقب(spec);
			واپس گرہ;
		}ورنہ
			واپس it->دوسرا;
	}
	گرہ_2 قطار_میں_ڈالو_cell(Cell *cell, IdString پورٹ_name)
	{
		خودکار it = cell_outputs.ڈھونڈو({cell, پورٹ_name});
		اگر(it == cell_outputs.انتہا()) {
			قطار.emplace_عقب(cell);
			std::optional<گرہ_2> rv;
			برائے(خودکار ثابت &[name, sigspec] : cell->connections())
				اگر(driver_map.celltypes.cell_output(cell->type, name)) {
					خودکار گرہ = factory.create_pending(sigspec.سائز());
					factory.suggest_name(گرہ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, گرہ);
					اگر(name == پورٹ_name)
						rv = گرہ;
				}
			واپس *rv;
		} ورنہ
			واپس it->دوسرا;
	}
عوامی:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.جوڑو(module);
		برائے (خودکار cell : module->cells()) {
			اگر (cell->type.اندر(ID($أكد), ID($assume), ID($زندہ), ID($fair), ID($cover), ID($check)))
				قطار.emplace_عقب(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		برائے (خودکار riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			خودکار *wire = module->wire(*riter);
			اگر (wire && wire->پورٹ_داخلہ) {
				factory.جوڑو_داخلہ(wire->name, ID($داخلہ), ترتیب_دو_2(wire->چوڑائی));
			}
			اگر (wire && wire->پورٹ_output) {
				خودکار &output = factory.جوڑو_output(wire->name, ID($output), ترتیب_دو_2(wire->چوڑائی));
				output.سیٹ_قدر(قطار_میں_ڈالو(DriveChunk(DriveChunkWire(wire, 0, wire->چوڑائی))));
			}
		}
		memories_سمتیہ = Mem::get_all_memories(module);
		برائے (خودکار &mem : memories_سمتیہ) {
			اگر (mem.cell != خالی_اشاریہ)
				memories[mem.cell] = &mem;
		}
	}
نجی:
	گرہ_2 concatenate_read_results(Mem *mem, سمتیہ<گرہ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_ڈیٹا = mem->cell->connections().at(ID(RD_ڈیٹا));
		صحیح current = 0;
		برائے(سائز_t i = 0; i < mem->rd_ports.سائز(); i++) {
			صحیح چوڑائی = mem->چوڑائی << mem->rd_ports[i].wide_log2;
			log_assert (results[i].چوڑائی() == چوڑائی);
			log_assert (mem->rd_ports[i].ڈیٹا == rd_ڈیٹا.نکالو_2(current, چوڑائی));
			current += چوڑائی;
		}
		log_assert (current == rd_ڈیٹا.سائز());
		log_assert (!results.خالی_خانے());
		گرہ_2 گرہ = results[0];
		برائے(سائز_t i = 1; i < results.سائز(); i++)
			گرہ = factory.concat(گرہ, results[i]);
		واپس گرہ;
	}
	گرہ_2 handle_حافظہ(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		سمتیہ<گرہ_2> read_results;
		خودکار &state = factory.جوڑو_state(mem->cell->name, ID($state), ترتیب_دو_2(ceil_log2(mem->سائز), mem->چوڑائی));
		state.سیٹ_initial_قدر(MemContents(mem));
		گرہ_2 گرہ = factory.قدر(state);
		برائے (سائز_t i = 0; i < mem->wr_ports.سائز(); i++) {
			ثابت خودکار &wr = mem->wr_ports[i];
			اگر (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گرہ_2 en = قطار_میں_ڈالو(driver_map(DriveSpec(wr.en)));
			گرہ_2 addr = قطار_میں_ڈالو(driver_map(DriveSpec(wr.addr)));
			گرہ_2 new_ڈیٹا = قطار_میں_ڈالو(driver_map(DriveSpec(wr.ڈیٹا)));
			گرہ_2 old_ڈیٹا = factory.حافظہ_read(گرہ, addr);
			گرہ_2 wr_ڈیٹا = simplifier.bitwise_mux(old_ڈیٹا, new_ڈیٹا, en);
			گرہ = factory.حافظہ_write(گرہ, addr, wr_ڈیٹا);
		}
		اگر (mem->rd_ports.خالی_خانے())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		برائے (سائز_t i = 0; i < mem->rd_ports.سائز(); i++) {
			ثابت خودکار &rd = mem->rd_ports[i];
			اگر (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گرہ_2 addr = قطار_میں_ڈالو(driver_map(DriveSpec(rd.addr)));
			read_results.ڈالو_عقب(factory.حافظہ_read(گرہ, addr));
		}
		state.سیٹ_اگلا_قدر(گرہ);
		واپس concatenate_read_results(mem, read_results);
	}
	خالی process_cell(Cell *cell)
	{
		اگر (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, خالی_اشاریہ);
			اگر (mem == خالی_اشاریہ) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			گرہ_2 گرہ = handle_حافظہ(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_ڈیٹا)}), گرہ);
		} ورنہ اگر (cell->is_builtin_ff()) {
			Ff_ڈیٹا ff(&ff_initvals, cell);
			اگر (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			خودکار &state = factory.جوڑو_state(ff.name, ID($state), ترتیب_دو_2(ff.چوڑائی));
			گرہ_2 q_قدر = factory.قدر(state);
			factory.suggest_name(q_قدر, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_قدر);
			state.سیٹ_اگلا_قدر(قطار_میں_ڈالو(ff.sig_d));
			state.سیٹ_initial_قدر(ff.val_init);
		} ورنہ {
			dict<IdString, گرہ_2> connections;
			IdString output_name; // for the single output case
			صحیح n_outputs = 0;
			برائے(خودکار ثابت &[name, sigspec] : cell->connections()) {
				اگر(driver_map.celltypes.cell_داخلہ(cell->type, name) && sigspec.سائز() > 0)
					connections.داخل_کرو({ name, قطار_میں_ڈالو(Drive_Chunk_پورٹ(cell, {name, sigspec})) });
				اگر(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, گرہ_2>, گرہ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			اگر(خودکار *nodep = std::get_if<گرہ_2>(&outputs); nodep != خالی_اشاریہ) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} ورنہ {
				برائے(خودکار [name, گرہ] : std::get<dict<IdString, گرہ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), گرہ);
			}
		}
	}
	خالی undriven(ثابت std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	خالی check_undriven(DriveSpec ثابت& spec, std::string ثابت& name) {
		برائے(خودکار ثابت &chunk : spec.chunks())
			اگر(chunk.is_none())
				undriven(name);
	}
عوامی:
	خالی process_قطار()
	{
		برائے (; !قطار.خالی_خانے(); قطار.نکالو_front()) {
			اگر(خودکار p = std::get_if<Cell *>(&قطار.front()); p != خالی_اشاریہ) {
				process_cell(*p);
				جاری;
			}

			DriveSpec spec = std::get<DriveSpec>(قطار.front());
			گرہ_2 pending = گراف_nodes.at(spec);

			اگر (spec.chunks().سائز() > 1) {
				خودکار chunks = spec.chunks();
				گرہ_2 گرہ = قطار_میں_ڈالو(chunks[0]);
				برائے(سائز_t i = 1; i < chunks.سائز(); i++)
					گرہ = factory.concat(گرہ, قطار_میں_ڈالو(chunks[i]));
				factory.update_pending(pending, گرہ);
			} ورنہ اگر (spec.chunks().سائز() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				اگر (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					اگر (wire_chunk.is_پورا()) {
						اگر (wire_chunk.wire->پورٹ_داخلہ) {
							گرہ_2 گرہ = factory.قدر(factory.ir().داخلہ(wire_chunk.wire->name));
							factory.suggest_name(گرہ, wire_chunk.wire->name);
							factory.update_pending(pending, گرہ);
						} ورنہ {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							گرہ_2 گرہ = قطار_میں_ڈالو(driver);
							factory.suggest_name(گرہ, wire_chunk.wire->name);
							factory.update_pending(pending, گرہ);
						}
					} ورنہ {
						DriveChunkWire پورا_wire(wire_chunk.wire, 0, wire_chunk.wire->چوڑائی);
						گرہ_2 گرہ = factory.قاش(قطار_میں_ڈالو(پورا_wire), wire_chunk.انحراف, wire_chunk.چوڑائی);
						factory.update_pending(pending, گرہ);
					}
				} ورنہ اگر (chunk.is_پورٹ()) {
					Drive_Chunk_پورٹ پورٹ_chunk = chunk.پورٹ();
					اگر (پورٹ_chunk.is_پورا()) {
						اگر (driver_map.celltypes.cell_output(پورٹ_chunk.cell->type, پورٹ_chunk.پورٹ)) {
							گرہ_2 گرہ = قطار_میں_ڈالو_cell(پورٹ_chunk.cell, پورٹ_chunk.پورٹ);
							factory.update_pending(pending, گرہ);
						} ورنہ {
							DriveSpec driver = driver_map(DriveSpec(پورٹ_chunk));
							check_undriven(driver, پورٹ_chunk.cell->name.unescape() + " port " + PooledName(پورٹ_chunk.cell->module, پورٹ_chunk.پورٹ).unescape());
							factory.update_pending(pending, قطار_میں_ڈالو(driver));
						}
					} ورنہ {
						Drive_Chunk_پورٹ پورا_پورٹ(پورٹ_chunk.cell, پورٹ_chunk.پورٹ, 0, Get_سائز(پورٹ_chunk.cell->connections().at(پورٹ_chunk.پورٹ)));
						گرہ_2 گرہ = factory.قاش(قطار_میں_ڈالو(پورا_پورٹ), پورٹ_chunk.انحراف, پورٹ_chunk.چوڑائی);
						factory.update_pending(pending, گرہ);
					}
				} ورنہ اگر (chunk.is_constant()) {
					گرہ_2 گرہ = factory.constant(chunk.constant());
					factory.suggest_name(گرہ, "$const" + std::مقصد_string(chunk.سائز()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, گرہ);
				} ورنہ اگر (chunk.is_مضرب()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} ورنہ اگر (chunk.is_none()) {
					undriven(log_signal(chunk));
				} ورنہ {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} ورنہ {
				log_abort();
			}
		}
	}
};

IR IR::from_module(Module *module) {
	IR ir;
    ir.design = module->design;
    خودکار factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_قطار();
    ir.topological_ترتیب_دو();
    ir.forward_buf();
    واپس ir;
}

خالی IR::topological_ترتیب_دو() {
    گراف::SccAdaptor compute_گراف_scc(گراف_4);
    منطقی scc = جھوٹ;
    std::سمتیہ<صحیح> perm;
    TopoSortedSccs toposort(compute_گراف_scc, [&](صحیح *begin, صحیح *انتہا) {
        perm.داخل_کرو(perm.انتہا(), begin, انتہا);
        اگر (انتہا > begin + 1)
        {
            log_warning("Combinational loop:\n");
            برائے (صحیح *i = begin; i != انتہا; ++i) {
				گرہ_2 گرہ(گراف_4[*i], design);
                لوگارتھم("- %s = %s\n", گرہ.name().unescape(), گرہ.مقصد_string());
			}
            لوگارتھم("\n");
            scc = سچ;
        }
    });
	برائے(ثابت خودکار &[name, state]: حالتیں_2)
		اگر(state.has_اگلا_قدر())
			toposort.process(state.اگلا_قدر().id());
	برائے(ثابت خودکار &[name, output]: _outputs)
		اگر(output.has_قدر())
			toposort.process(output.قدر().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    گراف_4.ترتیب_بدلو(perm);
    اگر(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ساکن IdString ملاؤ_name(IdString a, IdString b) {
	اگر(!a.is_عوامی() && b.is_عوامی())
		واپس b;
	ورنہ
		واپس a;
}

خالی IR::forward_buf() {
    std::سمتیہ<صحیح> perm, alias;
    perm.clear();

    برائے (صحیح i = 0; i < گراف_4.سائز(); ++i)
    {
        خودکار گرہ = گراف_4[i];
        اگر (گرہ.function().fn() == Fn::buf && گرہ.arg(0).اشاریہ() < i)
        {
            صحیح ہدف_اشاریہ = alias[گرہ.arg(0).اشاریہ()];
            خودکار ہدف_گرہ = گراف_4[perm[ہدف_اشاریہ]];
			اگر(گرہ.has_sparse_attr()) {
				اگر(ہدف_گرہ.has_sparse_attr()) {
					IdString id = ملاؤ_name(گرہ.sparse_attr(), ہدف_گرہ.sparse_attr());
					ہدف_گرہ.sparse_attr() = id;
				} ورنہ {
					IdString id = گرہ.sparse_attr();
					ہدف_گرہ.sparse_attr() = id;
				}
			}
            alias.ڈالو_عقب(ہدف_اشاریہ);
        }
        ورنہ
        {
            alias.ڈالو_عقب(Get_سائز(perm));
            perm.ڈالو_عقب(i);
        }
    }
    گراف_4.ترتیب_بدلو(perm, alias);
}

// Quoting routine to make error messages nicer
ساکن std::string quote_fmt(ثابت حرف *fmt)
{
	std::string r;
	برائے(ثابت حرف *p = fmt; *p != 0; p++) {
		انتخاب(*p) {
		حالت '\n': r += "\\n"; توڑو;
		حالت '\t': r += "\\t"; توڑو;
		حالت '"': r += "\\\""; توڑو;
		حالت '\\': r += "\\\\"; توڑو;
		طےشدہ: r += *p; توڑو;
		}
	}
	واپس r;
}

خالی Writer::print_impl(ثابت حرف *fmt, سمتیہ<std::function<خالی()>> &fns)
{
	سائز_t اگلا_اشاریہ = 0;
	برائے(ثابت حرف *p = fmt; *p != 0; p++)
		انتخاب(*p) {
		حالت '{':
			اگر(*++p == '{') {
				*os << '{';
			} ورنہ {
				حرف *pe;
				سائز_t اشاریہ = strtoul(p, &pe, 10);
				اگر(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				اگر(p == pe)
					اشاریہ = اگلا_اشاریہ;
				ورنہ
					p = pe;
				اگر(اشاریہ >= fns.سائز())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", اشاریہ, fns.سائز(), quote_fmt(fmt));
				fns[اشاریہ]();
				اگلا_اشاریہ = اشاریہ + 1;
			}
			توڑو;
		حالت '}':
			p++;
			اگر(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			توڑو;
		طےشدہ:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_انتہا
