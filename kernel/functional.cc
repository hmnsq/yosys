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

#ضمن "kernel/functional.h"
#ضمن "kernel/topo_scc.h"
#ضمن "ff.h"
#ضمن "ffinit.h"
#ضمن <deque>

YOSYS_NAMESPACE_BEGIN
نطاق Functional {

ثابت حرف *fn_منزل_string(Fn fn) {
	بدل(fn) {
	حالة Fn::invalid: أرجع "invalid";
	حالة Fn::buf: أرجع "buf";
	حالة Fn::ٹکڑا: أرجع "slice";
	حالة Fn::zero_extend: أرجع "zero_extend";
	حالة Fn::sign_extend: أرجع "sign_extend";
	حالة Fn::concat: أرجع "concat";
	حالة Fn::جوڑو: أرجع "add";
	حالة Fn::sub: أرجع "sub";
	حالة Fn::mul: أرجع "mul";
	حالة Fn::unsigned_div: أرجع "unsigned_div";
	حالة Fn::unsigned_mod: أرجع "unsigned_mod";
	حالة Fn::bitwise_and: أرجع "bitwise_and";
	حالة Fn::bitwise_or: أرجع "bitwise_or";
	حالة Fn::bitwise_xor: أرجع "bitwise_xor";
	حالة Fn::bitwise_not: أرجع "bitwise_not";
	حالة Fn::reduce_and: أرجع "reduce_and";
	حالة Fn::reduce_or: أرجع "reduce_or";
	حالة Fn::reduce_xor: أرجع "reduce_xor";
	حالة Fn::unary_minus: أرجع "unary_minus";
	حالة Fn::equal: أرجع "equal";
	حالة Fn::not_equal: أرجع "not_equal";
	حالة Fn::signed_greater_than: أرجع "signed_greater_than";
	حالة Fn::signed_greater_equal: أرجع "signed_greater_equal";
	حالة Fn::unsigned_greater_than: أرجع "unsigned_greater_than";
	حالة Fn::unsigned_greater_equal: أرجع "unsigned_greater_equal";
	حالة Fn::logical_کھسکاؤ_بایاں: أرجع "logical_shift_left";
	حالة Fn::logical_کھسکاؤ_دایاں: أرجع "logical_shift_right";
	حالة Fn::arithmetic_کھسکاؤ_دایاں: أرجع "arithmetic_shift_right";
	حالة Fn::mux: أرجع "mux";
	حالة Fn::constant: أرجع "constant";
	حالة Fn::داخلہ: أرجع "input";
	حالة Fn::state: أرجع "state";
	حالة Fn::حافظہ_read: أرجع "memory_read";
	حالة Fn::حافظہ_write: أرجع "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحيح)fn);
}

سمتیہ<IRInput ثابت*> IR::inputs(IdString قسم) ثابت {
	سمتیہ<IRInput ثابت*> ret;
	لكل (ثابت خودکار &[name, داخلہ] : _inputs)
		إذا(داخلہ.قسم == قسم)
			ret.ڈالو_پٹھ(&داخلہ);
	أرجع ret;
}

سمتیہ<IROutput ثابت*> IR::outputs(IdString قسم) ثابت {
	سمتیہ<IROutput ثابت*> ret;
	لكل (ثابت خودکار &[name, output] : _outputs)
		إذا(output.قسم == قسم)
			ret.ڈالو_پٹھ(&output);
	أرجع ret;
}

سمتیہ<IRState ثابت*> IR::حالتاں(IdString قسم) ثابت {
	سمتیہ<IRState ثابت*> ret;
	لكل (ثابت خودکار &[name, state] : حالتاں_2)
		إذا(state.قسم == قسم)
			ret.ڈالو_پٹھ(&state);
	أرجع ret;
}

سمتیہ<IRInput ثابت*> IR::all_inputs() ثابت {
	سمتیہ<IRInput ثابت*> ret;
	لكل (ثابت خودکار &[name, داخلہ] : _inputs)
		ret.ڈالو_پٹھ(&داخلہ);
	أرجع ret;
}

سمتیہ<IROutput ثابت*> IR::all_outputs() ثابت {
	سمتیہ<IROutput ثابت*> ret;
	لكل (ثابت خودکار &[name, output] : _outputs)
		ret.ڈالو_پٹھ(&output);
	أرجع ret;
}

سمتیہ<IRState ثابت*> IR::all_حالتاں() ثابت {
	سمتیہ<IRState ثابت*> ret;
	لكل (ثابت خودکار &[name, state] : حالتاں_2)
		ret.ڈالو_پٹھ(&state);
	أرجع ret;
}

بنية PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(گرہ_2)> np;
	PrintVisitor(std::function<std::string(گرہ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string ٹکڑا(گرہ_2, گرہ_2 a, صحيح وتھ, صحيح باہرلا_چوڑائی) override { أرجع "slice(" + np(a) + ", " + std::منزل_string(وتھ) + ", " + std::منزل_string(باہرلا_چوڑائی) + ")"; }
	std::string zero_extend(گرہ_2, گرہ_2 a, صحيح باہرلا_چوڑائی) override { أرجع "zero_extend(" + np(a) + ", " + std::منزل_string(باہرلا_چوڑائی) + ")"; }
	std::string sign_extend(گرہ_2, گرہ_2 a, صحيح باہرلا_چوڑائی) override { أرجع "sign_extend(" + np(a) + ", " + std::منزل_string(باہرلا_چوڑائی) + ")"; }
	std::string constant(گرہ_2, RTLIL::Const ثابت& قدر) override { أرجع "constant(" + قدر.as_string() + ")"; }
	std::string داخلہ(گرہ_2 self, IdString name, IdString قسم) override { أرجع "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(قسم) + ")"; }
	std::string state(گرہ_2 self, IdString name, IdString قسم) override { أرجع "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(قسم) + ")"; }
	std::string default_handler(گرہ_2 self) override {
		std::string ret = fn_منزل_string(self.fn());
		ret += "(";
		لكل(ناپ_t i = 0; i < self.arg_گنتی(); i++) {
			إذا(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		أرجع ret;
	}
};

std::string گرہ_2::منزل_string()
{
	أرجع منزل_string([](گرہ_2 n) { أرجع n.name().unescape(); });
}

std::string گرہ_2::منزل_string(std::function<std::string(گرہ_2)> np)
{
	أرجع ویکھو(PrintVisitor(np));
}

صنف CellSimplifier {
	Factory &factory;
	گرہ_2 sign(گرہ_2 a) {
		أرجع factory.ٹکڑا(a, a.چوڑائی() - 1, 1);
	}
	گرہ_2 neg_if(گرہ_2 a, گرہ_2 s) {
		أرجع factory.mux(a, factory.unary_minus(a), s);
	}
	گرہ_2 مطلق_قدر(گرہ_2 a) {
		أرجع neg_if(a, sign(a));
	}
	گرہ_2 handle_کھسکاؤ(گرہ_2 a, گرہ_2 b, منطقي is_دایاں, منطقي is_signed) {
		// to prevent new_width == 0, we handle this case separately
		إذا(a.چوڑائی() == 1) {
			إذا(!is_signed)
				أرجع factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			وإلا
				أرجع a;
		}
		صحيح new_چوڑائی = ceil_log2(a.چوڑائی());
		گرہ_2 b_truncated = factory.extend(b, new_چوڑائی, خطأ);
		گرہ_2 y =
			!is_دایاں ? factory.logical_کھسکاؤ_بایاں(a, b_truncated) :
			!is_signed ? factory.logical_کھسکاؤ_دایاں(a, b_truncated) :
			factory.arithmetic_کھسکاؤ_دایاں(a, b_truncated);
		إذا(b.چوڑائی() <= new_چوڑائی)
			أرجع y;
		گرہ_2 حد_توں_ودھ = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.چوڑائی(), b.چوڑائی())));
		گرہ_2 y_if_حد_توں_ودھ = is_signed ? factory.extend(sign(a), a.چوڑائی(), سچ) : factory.constant(RTLIL::Const(State::S0, a.چوڑائی()));
		أرجع factory.mux(y, y_if_حد_توں_ودھ, حد_توں_ودھ);
	}
عام:
	گرہ_2 logical_کھسکاؤ_بایاں(گرہ_2 a, گرہ_2 b) { أرجع handle_کھسکاؤ(a, b, خطأ, خطأ); }
	گرہ_2 logical_کھسکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { أرجع handle_کھسکاؤ(a, b, سچ, خطأ); }
	گرہ_2 arithmetic_کھسکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { أرجع handle_کھسکاؤ(a, b, سچ, سچ); }
	گرہ_2 bitwise_mux(گرہ_2 a, گرہ_2 b, گرہ_2 s) {
		گرہ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		گرہ_2 bb = factory.bitwise_and(b, s);
		أرجع factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
خاص:
	گرہ_2 handle_pow(گرہ_2 a0, گرہ_2 b, صحيح y_چوڑائی, منطقي is_signed) {
		گرہ_2 a = factory.extend(a0, y_چوڑائی, is_signed);
		گرہ_2 r = factory.constant(Const(1, y_چوڑائی));
		لكل(صحيح i = 0; i < b.چوڑائی(); i++) {
			گرہ_2 b_ثنائی_ہندسہ = factory.ٹکڑا(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_ثنائی_ہندسہ);
			a = factory.mul(a, a);
		}
		إذا (is_signed) {
			گرہ_2 a_ge_1 = factory.unsigned_greater_than(مطلق_قدر(a0), factory.constant(Const(1, a0.چوڑائی())));
			گرہ_2 zero_نتیجہ = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_چوڑائی)), zero_نتیجہ);
		}
		أرجع r;
	}
	گرہ_2 handle_bmux(گرہ_2 a, گرہ_2 s, صحيح a_وتھ, صحيح چوڑائی, صحيح sn) {
		إذا(sn < 1)
			أرجع factory.ٹکڑا(a, a_وتھ, چوڑائی);
		وإلا {
			گرہ_2 y0 = handle_bmux(a, s, a_وتھ, چوڑائی, sn - 1);
			گرہ_2 y1 = handle_bmux(a, s, a_وتھ + (چوڑائی << (sn - 1)), چوڑائی, sn - 1);
			أرجع factory.mux(y0, y1, factory.ٹکڑا(s, sn - 1, 1));
		}
	}
	گرہ_2 handle_pmux(گرہ_2 a, گرہ_2 b, گرہ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.چوڑائی() == a.چوڑائی() * s.چوڑائی());
		گرہ_2 y = a;
		لكل(صحيح i = 0; i < s.چوڑائی(); i++)
			y = factory.mux(y, factory.ٹکڑا(b, a.چوڑائی() * i, a.چوڑائی()), factory.ٹکڑا(s, i, 1));
		أرجع y;
	}
	dict<IdString, گرہ_2> handle_fa(گرہ_2 a, گرہ_2 b, گرہ_2 c) {
		گرہ_2 t1 = factory.bitwise_xor(a, b);
		گرہ_2 t2 = factory.bitwise_and(a, b);
		گرہ_2 t3 = factory.bitwise_and(c, t1);
		گرہ_2 y = factory.bitwise_xor(c, t1);
		گرہ_2 x = factory.bitwise_or(t2, t3);
		أرجع {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, گرہ_2> handle_alu(گرہ_2 a_وچ, گرہ_2 b_وچ, صحيح y_چوڑائی, منطقي is_signed, گرہ_2 ci, گرہ_2 bi) {
		گرہ_2 a = factory.extend(a_وچ, y_چوڑائی, is_signed);
		گرہ_2 b_uninverted = factory.extend(b_وچ, y_چوڑائی, is_signed);
		گرہ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		گرہ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		گرہ_2 a_extra = factory.extend(a, y_چوڑائی + 1, خطأ);
		گرہ_2 b_extra = factory.extend(b, y_چوڑائی + 1, خطأ);
		گرہ_2 y_extra = factory.جوڑو(factory.جوڑو(a_extra, b_extra), factory.extend(ci, a.چوڑائی() + 1, خطأ));
		گرہ_2 y = factory.ٹکڑا(y_extra, 0, y_چوڑائی);
		گرہ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		گرہ_2 co = factory.ٹکڑا(carries, 1, y_چوڑائی);
		أرجع {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	گرہ_2 handle_lcu(گرہ_2 p, گرہ_2 g, گرہ_2 ci) {
		أرجع handle_alu(g, factory.bitwise_or(p, g), g.چوڑائی(), خطأ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عام:
	std::variant<dict<IdString, گرہ_2>, گرہ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, گرہ_2> inputs)
	{
		صحيح a_چوڑائی = parameters.at(ID(A_چوڑائی), Const(-1)).as_int();
		صحيح b_چوڑائی = parameters.at(ID(B_چوڑائی), Const(-1)).as_int();
		صحيح y_چوڑائی = parameters.at(ID(Y_چوڑائی), Const(-1)).as_int();
		منطقي a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقي b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		إذا(cellType.وچ(ID($جوڑو), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقي is_signed = a_signed && b_signed;
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, is_signed);
			گرہ_2 b = factory.extend(inputs.at(ID(B)), y_چوڑائی, is_signed);
			إذا(cellType == ID($جوڑو))
				أرجع factory.جوڑو(a, b);
			وإلا إذا(cellType == ID($sub))
				أرجع factory.sub(a, b);
			وإلا إذا(cellType == ID($mul))
				أرجع factory.mul(a, b);
			وإلا إذا(cellType == ID($and))
				أرجع factory.bitwise_and(a, b);
			وإلا إذا(cellType == ID($or))
				أرجع factory.bitwise_or(a, b);
			وإلا إذا(cellType == ID($xor))
				أرجع factory.bitwise_xor(a, b);
			وإلا إذا(cellType == ID($xnor))
				أرجع factory.bitwise_not(factory.bitwise_xor(a, b));
			وإلا
				log_abort();
		}وإلا إذا(cellType.وچ(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقي is_signed = a_signed && b_signed;
			صحيح چوڑائی = max(a_چوڑائی, b_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, is_signed);
			گرہ_2 b = factory.extend(inputs.at(ID(B)), چوڑائی, is_signed);
			إذا(cellType.وچ(ID($eq), ID($eqx)))
				أرجع factory.extend(factory.equal(a, b), y_چوڑائی, خطأ);
			وإلا إذا(cellType.وچ(ID($ne), ID($nex)))
				أرجع factory.extend(factory.not_equal(a, b), y_چوڑائی, خطأ);
			وإلا إذا(cellType == ID($lt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_چوڑائی, خطأ);
			وإلا إذا(cellType == ID($le))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_چوڑائی, خطأ);
			وإلا إذا(cellType == ID($gt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_چوڑائی, خطأ);
			وإلا إذا(cellType == ID($ge))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_چوڑائی, خطأ);
			وإلا
				log_abort();
		}وإلا إذا(cellType.وچ(ID($logic_or), ID($logic_and))){
			گرہ_2 a = factory.reduce_or(inputs.at(ID(A)));
			گرہ_2 b = factory.reduce_or(inputs.at(ID(B)));
			گرہ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			أرجع factory.extend(y, y_چوڑائی, خطأ);
		}وإلا إذا(cellType == ID($not)){
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
			أرجع factory.bitwise_not(a);
		}وإلا إذا(cellType == ID($pos)){
			أرجع factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
		}وإلا إذا(cellType == ID($barrier)){
			أرجع inputs.at(ID(A));
		}وإلا إذا(cellType == ID($neg)){
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
			أرجع factory.unary_minus(a);
		}وإلا إذا(cellType == ID($logic_not)){
			گرہ_2 a = factory.reduce_or(inputs.at(ID(A)));
			گرہ_2 y = factory.bitwise_not(a);
			أرجع factory.extend(y, y_چوڑائی, خطأ);
		}وإلا إذا(cellType.وچ(ID($reduce_or), ID($reduce_bool))){
			گرہ_2 a = factory.reduce_or(inputs.at(ID(A)));
			أرجع factory.extend(a, y_چوڑائی, خطأ);
		}وإلا إذا(cellType == ID($reduce_and)){
			گرہ_2 a = factory.reduce_and(inputs.at(ID(A)));
			أرجع factory.extend(a, y_چوڑائی, خطأ);
		}وإلا إذا(cellType.وچ(ID($reduce_xor), ID($reduce_xnor))){
			گرہ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			گرہ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			أرجع factory.extend(y, y_چوڑائی, خطأ);
		}وإلا إذا(cellType == ID($shl) || cellType == ID($sshl)){
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, a_signed);
			گرہ_2 b = inputs.at(ID(B));
			أرجع logical_کھسکاؤ_بایاں(a, b);
		}وإلا إذا(cellType == ID($shr) || cellType == ID($sshr)){
			صحيح چوڑائی = max(a_چوڑائی, y_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, a_signed);
			گرہ_2 b = inputs.at(ID(B));
			گرہ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_کھسکاؤ_دایاں(a, b) :
				logical_کھسکاؤ_دایاں(a, b);
			أرجع factory.extend(y, y_چوڑائی, a_signed);
		}وإلا إذا(cellType == ID($shiftx) || cellType == ID($کھسکاؤ)){
			صحيح چوڑائی = max(a_چوڑائی, y_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, cellType == ID($کھسکاؤ) && a_signed);
			گرہ_2 b = inputs.at(ID(B));
			گرہ_2 shr = logical_کھسکاؤ_دایاں(a, b);
			إذا(b_signed) {
				گرہ_2 shl = logical_کھسکاؤ_بایاں(a, factory.unary_minus(b));
				گرہ_2 y = factory.mux(shr, shl, sign(b));
				أرجع factory.extend(y, y_چوڑائی, خطأ);
			} وإلا {
				أرجع factory.extend(shr, y_چوڑائی, خطأ);
			}
		}وإلا إذا(cellType == ID($mux)){
			أرجع factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($pmux)){
			أرجع handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($concat)){
			گرہ_2 a = inputs.at(ID(A));
			گرہ_2 b = inputs.at(ID(B));
			أرجع factory.concat(a, b);
		}وإلا إذا(cellType == ID($ٹکڑا)){
			صحيح وتھ = parameters.at(ID(وتھ_2)).as_int();
			گرہ_2 a = inputs.at(ID(A));
			أرجع factory.ٹکڑا(a, وتھ, y_چوڑائی);
		}وإلا إذا(cellType.وچ(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحيح چوڑائی = max(a_چوڑائی, b_چوڑائی);
			منطقي is_signed = a_signed && b_signed;
			گرہ_2 a = factory.extend(inputs.at(ID(A)), چوڑائی, is_signed);
			گرہ_2 b = factory.extend(inputs.at(ID(B)), چوڑائی, is_signed);
			إذا(is_signed) {
				إذا(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					گرہ_2 abs_y = factory.unsigned_div(مطلق_قدر(a), مطلق_قدر(b));
					گرہ_2 باہرلا_sign = factory.not_equal(sign(a), sign(b));
					أرجع neg_if(factory.extend(abs_y, y_چوڑائی, خطأ), باہرلا_sign);
				} وإلا إذا(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					گرہ_2 abs_y = factory.unsigned_mod(مطلق_قدر(a), مطلق_قدر(b));
					أرجع neg_if(factory.extend(abs_y, y_چوڑائی, خطأ), sign(a));
				} وإلا إذا(cellType == ID($divfloor)) {
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
					گرہ_2 y2 = factory.extend(y1, y_چوڑائی, خطأ);
					أرجع factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} وإلا إذا(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					گرہ_2 abs_b = مطلق_قدر(b);
					گرہ_2 abs_y = factory.unsigned_mod(مطلق_قدر(a), abs_b);
					گرہ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					گرہ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					گرہ_2 y = neg_if(y_flipped, sign(b));
					أرجع factory.extend(y, y_چوڑائی, سچ);
				} وإلا
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} وإلا {
				إذا(cellType.وچ(ID($mod), ID($modfloor)))
					أرجع factory.extend(factory.unsigned_mod(a, b), y_چوڑائی, خطأ);
				وإلا
					أرجع factory.extend(factory.unsigned_div(a, b), y_چوڑائی, خطأ);
			}
		} وإلا إذا(cellType == ID($قوت)) {
			أرجع handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_چوڑائی, a_signed && b_signed);
		} وإلا إذا (cellType == ID($lut)) {
			صحيح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			Const lut_جدول = parameters.at(ID(LUT));
			lut_جدول.extu(1 << چوڑائی);
			أرجع handle_bmux(factory.constant(lut_جدول), inputs.at(ID(A)), 0, 1, چوڑائی);
		} وإلا إذا (cellType == ID($bwmux)) {
			گرہ_2 a = inputs.at(ID(A));
			گرہ_2 b = inputs.at(ID(B));
			گرہ_2 s = inputs.at(ID(S));
			أرجع factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} وإلا إذا (cellType == ID($bweqx)) {
			گرہ_2 a = inputs.at(ID(A));
			گرہ_2 b = inputs.at(ID(B));
			أرجع factory.bitwise_not(factory.bitwise_xor(a, b));
		} وإلا إذا(cellType == ID($bmux)) {
			صحيح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			صحيح s_چوڑائی = parameters.at(ID(S_چوڑائی)).as_int();
			أرجع handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, چوڑائی, s_چوڑائی);
		} وإلا إذا(cellType == ID($demux)) {
			صحيح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			صحيح s_چوڑائی = parameters.at(ID(S_چوڑائی)).as_int();
			صحيح y_چوڑائی = چوڑائی << s_چوڑائی;
			صحيح b_چوڑائی = ceil_log2(y_چوڑائی);
			گرہ_2 a = factory.extend(inputs.at(ID(A)), y_چوڑائی, خطأ);
			گرہ_2 s = factory.extend(inputs.at(ID(S)), b_چوڑائی, خطأ);
			گرہ_2 b = factory.mul(s, factory.constant(Const(چوڑائی, b_چوڑائی)));
			أرجع factory.logical_کھسکاؤ_بایاں(a, b);
		} وإلا إذا(cellType == ID($fa)) {
			أرجع handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} وإلا إذا(cellType == ID($lcu)) {
			أرجع handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} وإلا إذا(cellType == ID($alu)) {
			أرجع handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_چوڑائی, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} وإلا إذا(cellType.وچ(ID($أكد), ID($assume), ID($جیوندا), ID($fair), ID($cover))) {
			گرہ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			خودکار &output = factory.جوڑو_output(cellName, cellType, ترتیب_دیو_2(1));
			output.سیٹ_قدر(a);
			أرجع {};
		} وإلا إذا(cellType.وچ(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحيح چوڑائی = parameters.at(ID(چوڑائی_2)).as_int();
			خودکار &داخلہ = factory.جوڑو_داخلہ(cellName, cellType, ترتیب_دیو_2(چوڑائی));
			أرجع factory.قدر(داخلہ);
		} وإلا إذا(cellType == ID($initstate)) {
			إذا(factory.ir().has_state(ID($initstate), ID($state)))
				أرجع factory.قدر(factory.ir().state(ID($initstate)));
			وإلا {
				خودکار &state = factory.جوڑو_state(ID($initstate), ID($state), ترتیب_دیو_2(1));
				state.سیٹ_initial_قدر(RTLIL::Const(State::S1, 1));
				state.سیٹ_اگلا_قدر(factory.constant(RTLIL::Const(State::S0, 1)));
				أرجع factory.قدر(state);
			}
		} وإلا إذا(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} وإلا {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

صنف FunctionalIRConstruction {
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

	گرہ_2 قطار_وچ_پاؤ(DriveSpec ثابت &spec)
	{
		خودکار it = گراف_nodes.لبھو(spec);
		إذا(it == گراف_nodes.سرا()){
			خودکار گرہ = factory.create_pending(spec.ناپ());
			گراف_nodes.داخل_کرو({spec, گرہ});
			قطار.emplace_پٹھ(spec);
			أرجع گرہ;
		}وإلا
			أرجع it->دوجا;
	}
	گرہ_2 قطار_وچ_پاؤ_cell(Cell *cell, IdString پورٹ_name)
	{
		خودکار it = cell_outputs.لبھو({cell, پورٹ_name});
		إذا(it == cell_outputs.سرا()) {
			قطار.emplace_پٹھ(cell);
			std::optional<گرہ_2> rv;
			لكل(خودکار ثابت &[name, sigspec] : cell->connections())
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					خودکار گرہ = factory.create_pending(sigspec.ناپ());
					factory.suggest_name(گرہ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, گرہ);
					إذا(name == پورٹ_name)
						rv = گرہ;
				}
			أرجع *rv;
		} وإلا
			أرجع it->دوجا;
	}
عام:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.جوڑو(module);
		لكل (خودکار cell : module->cells()) {
			إذا (cell->type.وچ(ID($أكد), ID($assume), ID($جیوندا), ID($fair), ID($cover), ID($check)))
				قطار.emplace_پٹھ(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		لكل (خودکار riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			خودکار *wire = module->wire(*riter);
			إذا (wire && wire->پورٹ_داخلہ) {
				factory.جوڑو_داخلہ(wire->name, ID($داخلہ), ترتیب_دیو_2(wire->چوڑائی));
			}
			إذا (wire && wire->پورٹ_output) {
				خودکار &output = factory.جوڑو_output(wire->name, ID($output), ترتیب_دیو_2(wire->چوڑائی));
				output.سیٹ_قدر(قطار_وچ_پاؤ(DriveChunk(DriveChunkWire(wire, 0, wire->چوڑائی))));
			}
		}
		memories_سمتیہ = Mem::get_all_memories(module);
		لكل (خودکار &mem : memories_سمتیہ) {
			إذا (mem.cell != خالی_سوچک)
				memories[mem.cell] = &mem;
		}
	}
خاص:
	گرہ_2 concatenate_read_results(Mem *mem, سمتیہ<گرہ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_انکڑے = mem->cell->connections().at(ID(RD_انکڑے));
		صحيح current = 0;
		لكل(ناپ_t i = 0; i < mem->rd_ports.ناپ(); i++) {
			صحيح چوڑائی = mem->چوڑائی << mem->rd_ports[i].wide_log2;
			log_assert (results[i].چوڑائی() == چوڑائی);
			log_assert (mem->rd_ports[i].انکڑے == rd_انکڑے.کڈھو(current, چوڑائی));
			current += چوڑائی;
		}
		log_assert (current == rd_انکڑے.ناپ());
		log_assert (!results.خالی_تھاواں());
		گرہ_2 گرہ = results[0];
		لكل(ناپ_t i = 1; i < results.ناپ(); i++)
			گرہ = factory.concat(گرہ, results[i]);
		أرجع گرہ;
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
		خودکار &state = factory.جوڑو_state(mem->cell->name, ID($state), ترتیب_دیو_2(ceil_log2(mem->ناپ), mem->چوڑائی));
		state.سیٹ_initial_قدر(MemContents(mem));
		گرہ_2 گرہ = factory.قدر(state);
		لكل (ناپ_t i = 0; i < mem->wr_ports.ناپ(); i++) {
			ثابت خودکار &wr = mem->wr_ports[i];
			إذا (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گرہ_2 en = قطار_وچ_پاؤ(driver_map(DriveSpec(wr.en)));
			گرہ_2 addr = قطار_وچ_پاؤ(driver_map(DriveSpec(wr.addr)));
			گرہ_2 new_انکڑے = قطار_وچ_پاؤ(driver_map(DriveSpec(wr.انکڑے)));
			گرہ_2 old_انکڑے = factory.حافظہ_read(گرہ, addr);
			گرہ_2 wr_انکڑے = simplifier.bitwise_mux(old_انکڑے, new_انکڑے, en);
			گرہ = factory.حافظہ_write(گرہ, addr, wr_انکڑے);
		}
		إذا (mem->rd_ports.خالی_تھاواں())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		لكل (ناپ_t i = 0; i < mem->rd_ports.ناپ(); i++) {
			ثابت خودکار &rd = mem->rd_ports[i];
			إذا (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گرہ_2 addr = قطار_وچ_پاؤ(driver_map(DriveSpec(rd.addr)));
			read_results.ڈالو_پٹھ(factory.حافظہ_read(گرہ, addr));
		}
		state.سیٹ_اگلا_قدر(گرہ);
		أرجع concatenate_read_results(mem, read_results);
	}
	فراغ process_cell(Cell *cell)
	{
		إذا (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, خالی_سوچک);
			إذا (mem == خالی_سوچک) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			گرہ_2 گرہ = handle_حافظہ(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_انکڑے)}), گرہ);
		} وإلا إذا (cell->is_builtin_ff()) {
			Ff_انکڑے ff(&ff_initvals, cell);
			إذا (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			خودکار &state = factory.جوڑو_state(ff.name, ID($state), ترتیب_دیو_2(ff.چوڑائی));
			گرہ_2 q_قدر = factory.قدر(state);
			factory.suggest_name(q_قدر, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_قدر);
			state.سیٹ_اگلا_قدر(قطار_وچ_پاؤ(ff.sig_d));
			state.سیٹ_initial_قدر(ff.val_init);
		} وإلا {
			dict<IdString, گرہ_2> connections;
			IdString output_name; // for the single output case
			صحيح n_outputs = 0;
			لكل(خودکار ثابت &[name, sigspec] : cell->connections()) {
				إذا(driver_map.celltypes.cell_داخلہ(cell->type, name) && sigspec.ناپ() > 0)
					connections.داخل_کرو({ name, قطار_وچ_پاؤ(Drive_Chunk_پورٹ(cell, {name, sigspec})) });
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, گرہ_2>, گرہ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			إذا(خودکار *nodep = std::get_if<گرہ_2>(&outputs); nodep != خالی_سوچک) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} وإلا {
				لكل(خودکار [name, گرہ] : std::get<dict<IdString, گرہ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), گرہ);
			}
		}
	}
	فراغ undriven(ثابت std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	فراغ check_undriven(DriveSpec ثابت& spec, std::string ثابت& name) {
		لكل(خودکار ثابت &chunk : spec.chunks())
			إذا(chunk.is_none())
				undriven(name);
	}
عام:
	فراغ process_قطار()
	{
		لكل (; !قطار.خالی_تھاواں(); قطار.نکالو_front()) {
			إذا(خودکار p = std::get_if<Cell *>(&قطار.front()); p != خالی_سوچک) {
				process_cell(*p);
				استمر;
			}

			DriveSpec spec = std::get<DriveSpec>(قطار.front());
			گرہ_2 pending = گراف_nodes.at(spec);

			إذا (spec.chunks().ناپ() > 1) {
				خودکار chunks = spec.chunks();
				گرہ_2 گرہ = قطار_وچ_پاؤ(chunks[0]);
				لكل(ناپ_t i = 1; i < chunks.ناپ(); i++)
					گرہ = factory.concat(گرہ, قطار_وچ_پاؤ(chunks[i]));
				factory.update_pending(pending, گرہ);
			} وإلا إذا (spec.chunks().ناپ() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				إذا (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					إذا (wire_chunk.is_سارا()) {
						إذا (wire_chunk.wire->پورٹ_داخلہ) {
							گرہ_2 گرہ = factory.قدر(factory.ir().داخلہ(wire_chunk.wire->name));
							factory.suggest_name(گرہ, wire_chunk.wire->name);
							factory.update_pending(pending, گرہ);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							گرہ_2 گرہ = قطار_وچ_پاؤ(driver);
							factory.suggest_name(گرہ, wire_chunk.wire->name);
							factory.update_pending(pending, گرہ);
						}
					} وإلا {
						DriveChunkWire سارا_wire(wire_chunk.wire, 0, wire_chunk.wire->چوڑائی);
						گرہ_2 گرہ = factory.ٹکڑا(قطار_وچ_پاؤ(سارا_wire), wire_chunk.وتھ, wire_chunk.چوڑائی);
						factory.update_pending(pending, گرہ);
					}
				} وإلا إذا (chunk.is_پورٹ()) {
					Drive_Chunk_پورٹ پورٹ_chunk = chunk.پورٹ();
					إذا (پورٹ_chunk.is_سارا()) {
						إذا (driver_map.celltypes.cell_output(پورٹ_chunk.cell->type, پورٹ_chunk.پورٹ)) {
							گرہ_2 گرہ = قطار_وچ_پاؤ_cell(پورٹ_chunk.cell, پورٹ_chunk.پورٹ);
							factory.update_pending(pending, گرہ);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(پورٹ_chunk));
							check_undriven(driver, پورٹ_chunk.cell->name.unescape() + " port " + PooledName(پورٹ_chunk.cell->module, پورٹ_chunk.پورٹ).unescape());
							factory.update_pending(pending, قطار_وچ_پاؤ(driver));
						}
					} وإلا {
						Drive_Chunk_پورٹ سارا_پورٹ(پورٹ_chunk.cell, پورٹ_chunk.پورٹ, 0, Get_ناپ(پورٹ_chunk.cell->connections().at(پورٹ_chunk.پورٹ)));
						گرہ_2 گرہ = factory.ٹکڑا(قطار_وچ_پاؤ(سارا_پورٹ), پورٹ_chunk.وتھ, پورٹ_chunk.چوڑائی);
						factory.update_pending(pending, گرہ);
					}
				} وإلا إذا (chunk.is_constant()) {
					گرہ_2 گرہ = factory.constant(chunk.constant());
					factory.suggest_name(گرہ, "$const" + std::منزل_string(chunk.ناپ()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, گرہ);
				} وإلا إذا (chunk.is_مضرب()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} وإلا إذا (chunk.is_none()) {
					undriven(log_signal(chunk));
				} وإلا {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} وإلا {
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
    ir.topological_ترتیب_دیو();
    ir.forward_buf();
    أرجع ir;
}

فراغ IR::topological_ترتیب_دیو() {
    گراف::SccAdaptor compute_گراف_scc(گراف_4);
    منطقي scc = خطأ;
    std::سمتیہ<صحيح> perm;
    TopoSortedSccs toposort(compute_گراف_scc, [&](صحيح *begin, صحيح *سرا) {
        perm.داخل_کرو(perm.سرا(), begin, سرا);
        إذا (سرا > begin + 1)
        {
            log_warning("Combinational loop:\n");
            لكل (صحيح *i = begin; i != سرا; ++i) {
				گرہ_2 گرہ(گراف_4[*i], design);
                لوگارتھم("- %s = %s\n", گرہ.name().unescape(), گرہ.منزل_string());
			}
            لوگارتھم("\n");
            scc = سچ;
        }
    });
	لكل(ثابت خودکار &[name, state]: حالتاں_2)
		إذا(state.has_اگلا_قدر())
			toposort.process(state.اگلا_قدر().id());
	لكل(ثابت خودکار &[name, output]: _outputs)
		إذا(output.has_قدر())
			toposort.process(output.قدر().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    گراف_4.ترتیب_بدلو(perm);
    إذا(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ساكن IdString ملاؤ_name(IdString a, IdString b) {
	إذا(!a.is_عوامی() && b.is_عوامی())
		أرجع b;
	وإلا
		أرجع a;
}

فراغ IR::forward_buf() {
    std::سمتیہ<صحيح> perm, alias;
    perm.clear();

    لكل (صحيح i = 0; i < گراف_4.ناپ(); ++i)
    {
        خودکار گرہ = گراف_4[i];
        إذا (گرہ.function().fn() == Fn::buf && گرہ.arg(0).اشاریہ() < i)
        {
            صحيح نشانہ_اشاریہ = alias[گرہ.arg(0).اشاریہ()];
            خودکار نشانہ_گرہ = گراف_4[perm[نشانہ_اشاریہ]];
			إذا(گرہ.has_sparse_attr()) {
				إذا(نشانہ_گرہ.has_sparse_attr()) {
					IdString id = ملاؤ_name(گرہ.sparse_attr(), نشانہ_گرہ.sparse_attr());
					نشانہ_گرہ.sparse_attr() = id;
				} وإلا {
					IdString id = گرہ.sparse_attr();
					نشانہ_گرہ.sparse_attr() = id;
				}
			}
            alias.ڈالو_پٹھ(نشانہ_اشاریہ);
        }
        وإلا
        {
            alias.ڈالو_پٹھ(Get_ناپ(perm));
            perm.ڈالو_پٹھ(i);
        }
    }
    گراف_4.ترتیب_بدلو(perm, alias);
}

// Quoting routine to make error messages nicer
ساكن std::string quote_fmt(ثابت حرف *fmt)
{
	std::string r;
	لكل(ثابت حرف *p = fmt; *p != 0; p++) {
		بدل(*p) {
		حالة '\n': r += "\\n"; اكسر;
		حالة '\t': r += "\\t"; اكسر;
		حالة '"': r += "\\\""; اكسر;
		حالة '\\': r += "\\\\"; اكسر;
		افتراضي: r += *p; اكسر;
		}
	}
	أرجع r;
}

فراغ Writer::print_impl(ثابت حرف *fmt, سمتیہ<std::function<فراغ()>> &fns)
{
	ناپ_t اگلا_اشاریہ = 0;
	لكل(ثابت حرف *p = fmt; *p != 0; p++)
		بدل(*p) {
		حالة '{':
			إذا(*++p == '{') {
				*os << '{';
			} وإلا {
				حرف *pe;
				ناپ_t اشاریہ = strtoul(p, &pe, 10);
				إذا(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				إذا(p == pe)
					اشاریہ = اگلا_اشاریہ;
				وإلا
					p = pe;
				إذا(اشاریہ >= fns.ناپ())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", اشاریہ, fns.ناپ(), quote_fmt(fmt));
				fns[اشاریہ]();
				اگلا_اشاریہ = اشاریہ + 1;
			}
			اكسر;
		حالة '}':
			p++;
			إذا(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			اكسر;
		افتراضي:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_سرا
