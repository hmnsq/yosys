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
	حالة Fn::ٽڪرو: أرجع "slice";
	حالة Fn::zero_extend: أرجع "zero_extend";
	حالة Fn::sign_extend: أرجع "sign_extend";
	حالة Fn::concat: أرجع "concat";
	حالة Fn::جوڙيو: أرجع "add";
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
	حالة Fn::logical_سرڪايو_کاٻو: أرجع "logical_shift_left";
	حالة Fn::logical_سرڪايو_ساڄو: أرجع "logical_shift_right";
	حالة Fn::arithmetic_سرڪايو_ساڄو: أرجع "arithmetic_shift_right";
	حالة Fn::mux: أرجع "mux";
	حالة Fn::constant: أرجع "constant";
	حالة Fn::داخلا: أرجع "input";
	حالة Fn::state: أرجع "state";
	حالة Fn::يادگيري_read: أرجع "memory_read";
	حالة Fn::يادگيري_write: أرجع "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحيح)fn);
}

ويڪٽر<IRInput ثابت*> IR::inputs(IdString قسم) ثابت {
	ويڪٽر<IRInput ثابت*> ret;
	لكل (ثابت خودڪار &[name, داخلا] : _inputs)
		إذا(داخلا.قسم == قسم)
			ret.ڌڪو_پٺ(&داخلا);
	أرجع ret;
}

ويڪٽر<IROutput ثابت*> IR::outputs(IdString قسم) ثابت {
	ويڪٽر<IROutput ثابت*> ret;
	لكل (ثابت خودڪار &[name, output] : _outputs)
		إذا(output.قسم == قسم)
			ret.ڌڪو_پٺ(&output);
	أرجع ret;
}

ويڪٽر<IRState ثابت*> IR::حالتون(IdString قسم) ثابت {
	ويڪٽر<IRState ثابت*> ret;
	لكل (ثابت خودڪار &[name, state] : حالتون_2)
		إذا(state.قسم == قسم)
			ret.ڌڪو_پٺ(&state);
	أرجع ret;
}

ويڪٽر<IRInput ثابت*> IR::all_inputs() ثابت {
	ويڪٽر<IRInput ثابت*> ret;
	لكل (ثابت خودڪار &[name, داخلا] : _inputs)
		ret.ڌڪو_پٺ(&داخلا);
	أرجع ret;
}

ويڪٽر<IROutput ثابت*> IR::all_outputs() ثابت {
	ويڪٽر<IROutput ثابت*> ret;
	لكل (ثابت خودڪار &[name, output] : _outputs)
		ret.ڌڪو_پٺ(&output);
	أرجع ret;
}

ويڪٽر<IRState ثابت*> IR::all_حالتون() ثابت {
	ويڪٽر<IRState ثابت*> ret;
	لكل (ثابت خودڪار &[name, state] : حالتون_2)
		ret.ڌڪو_پٺ(&state);
	أرجع ret;
}

بنية PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(ڳنڍ_2)> np;
	PrintVisitor(std::function<std::string(ڳنڍ_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string ٽڪرو(ڳنڍ_2, ڳنڍ_2 a, صحيح وٿي, صحيح ٻاهريون_ويڪر) override { أرجع "slice(" + np(a) + ", " + std::منزل_string(وٿي) + ", " + std::منزل_string(ٻاهريون_ويڪر) + ")"; }
	std::string zero_extend(ڳنڍ_2, ڳنڍ_2 a, صحيح ٻاهريون_ويڪر) override { أرجع "zero_extend(" + np(a) + ", " + std::منزل_string(ٻاهريون_ويڪر) + ")"; }
	std::string sign_extend(ڳنڍ_2, ڳنڍ_2 a, صحيح ٻاهريون_ويڪر) override { أرجع "sign_extend(" + np(a) + ", " + std::منزل_string(ٻاهريون_ويڪر) + ")"; }
	std::string constant(ڳنڍ_2, RTLIL::Const ثابت& قدر) override { أرجع "constant(" + قدر.as_string() + ")"; }
	std::string داخلا(ڳنڍ_2 self, IdString name, IdString قسم) override { أرجع "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(قسم) + ")"; }
	std::string state(ڳنڍ_2 self, IdString name, IdString قسم) override { أرجع "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(قسم) + ")"; }
	std::string default_handler(ڳنڍ_2 self) override {
		std::string ret = fn_منزل_string(self.fn());
		ret += "(";
		لكل(سائيز_t i = 0; i < self.arg_ڳڻپ(); i++) {
			إذا(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		أرجع ret;
	}
};

std::string ڳنڍ_2::منزل_string()
{
	أرجع منزل_string([](ڳنڍ_2 n) { أرجع n.name().unescape(); });
}

std::string ڳنڍ_2::منزل_string(std::function<std::string(ڳنڍ_2)> np)
{
	أرجع گهمو(PrintVisitor(np));
}

صنف CellSimplifier {
	Factory &factory;
	ڳنڍ_2 sign(ڳنڍ_2 a) {
		أرجع factory.ٽڪرو(a, a.ويڪر() - 1, 1);
	}
	ڳنڍ_2 neg_if(ڳنڍ_2 a, ڳنڍ_2 s) {
		أرجع factory.mux(a, factory.unary_minus(a), s);
	}
	ڳنڍ_2 مطلق_قدر(ڳنڍ_2 a) {
		أرجع neg_if(a, sign(a));
	}
	ڳنڍ_2 handle_سرڪايو(ڳنڍ_2 a, ڳنڍ_2 b, منطقي is_ساڄو, منطقي is_signed) {
		// to prevent new_width == 0, we handle this case separately
		إذا(a.ويڪر() == 1) {
			إذا(!is_signed)
				أرجع factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			وإلا
				أرجع a;
		}
		صحيح new_ويڪر = ceil_log2(a.ويڪر());
		ڳنڍ_2 b_truncated = factory.extend(b, new_ويڪر, خطأ);
		ڳنڍ_2 y =
			!is_ساڄو ? factory.logical_سرڪايو_کاٻو(a, b_truncated) :
			!is_signed ? factory.logical_سرڪايو_ساڄو(a, b_truncated) :
			factory.arithmetic_سرڪايو_ساڄو(a, b_truncated);
		إذا(b.ويڪر() <= new_ويڪر)
			أرجع y;
		ڳنڍ_2 حد_کان_مٿي = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.ويڪر(), b.ويڪر())));
		ڳنڍ_2 y_if_حد_کان_مٿي = is_signed ? factory.extend(sign(a), a.ويڪر(), سچ) : factory.constant(RTLIL::Const(State::S0, a.ويڪر()));
		أرجع factory.mux(y, y_if_حد_کان_مٿي, حد_کان_مٿي);
	}
عام:
	ڳنڍ_2 logical_سرڪايو_کاٻو(ڳنڍ_2 a, ڳنڍ_2 b) { أرجع handle_سرڪايو(a, b, خطأ, خطأ); }
	ڳنڍ_2 logical_سرڪايو_ساڄو(ڳنڍ_2 a, ڳنڍ_2 b) { أرجع handle_سرڪايو(a, b, سچ, خطأ); }
	ڳنڍ_2 arithmetic_سرڪايو_ساڄو(ڳنڍ_2 a, ڳنڍ_2 b) { أرجع handle_سرڪايو(a, b, سچ, سچ); }
	ڳنڍ_2 bitwise_mux(ڳنڍ_2 a, ڳنڍ_2 b, ڳنڍ_2 s) {
		ڳنڍ_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		ڳنڍ_2 bb = factory.bitwise_and(b, s);
		أرجع factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
خاص:
	ڳنڍ_2 handle_pow(ڳنڍ_2 a0, ڳنڍ_2 b, صحيح y_ويڪر, منطقي is_signed) {
		ڳنڍ_2 a = factory.extend(a0, y_ويڪر, is_signed);
		ڳنڍ_2 r = factory.constant(Const(1, y_ويڪر));
		لكل(صحيح i = 0; i < b.ويڪر(); i++) {
			ڳنڍ_2 b_ٻٽو_انگ = factory.ٽڪرو(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_ٻٽو_انگ);
			a = factory.mul(a, a);
		}
		إذا (is_signed) {
			ڳنڍ_2 a_ge_1 = factory.unsigned_greater_than(مطلق_قدر(a0), factory.constant(Const(1, a0.ويڪر())));
			ڳنڍ_2 zero_نتيجو = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_ويڪر)), zero_نتيجو);
		}
		أرجع r;
	}
	ڳنڍ_2 handle_bmux(ڳنڍ_2 a, ڳنڍ_2 s, صحيح a_وٿي, صحيح ويڪر, صحيح sn) {
		إذا(sn < 1)
			أرجع factory.ٽڪرو(a, a_وٿي, ويڪر);
		وإلا {
			ڳنڍ_2 y0 = handle_bmux(a, s, a_وٿي, ويڪر, sn - 1);
			ڳنڍ_2 y1 = handle_bmux(a, s, a_وٿي + (ويڪر << (sn - 1)), ويڪر, sn - 1);
			أرجع factory.mux(y0, y1, factory.ٽڪرو(s, sn - 1, 1));
		}
	}
	ڳنڍ_2 handle_pmux(ڳنڍ_2 a, ڳنڍ_2 b, ڳنڍ_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.ويڪر() == a.ويڪر() * s.ويڪر());
		ڳنڍ_2 y = a;
		لكل(صحيح i = 0; i < s.ويڪر(); i++)
			y = factory.mux(y, factory.ٽڪرو(b, a.ويڪر() * i, a.ويڪر()), factory.ٽڪرو(s, i, 1));
		أرجع y;
	}
	dict<IdString, ڳنڍ_2> handle_fa(ڳنڍ_2 a, ڳنڍ_2 b, ڳنڍ_2 c) {
		ڳنڍ_2 t1 = factory.bitwise_xor(a, b);
		ڳنڍ_2 t2 = factory.bitwise_and(a, b);
		ڳنڍ_2 t3 = factory.bitwise_and(c, t1);
		ڳنڍ_2 y = factory.bitwise_xor(c, t1);
		ڳنڍ_2 x = factory.bitwise_or(t2, t3);
		أرجع {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, ڳنڍ_2> handle_alu(ڳنڍ_2 a_اندريون, ڳنڍ_2 b_اندريون, صحيح y_ويڪر, منطقي is_signed, ڳنڍ_2 ci, ڳنڍ_2 bi) {
		ڳنڍ_2 a = factory.extend(a_اندريون, y_ويڪر, is_signed);
		ڳنڍ_2 b_uninverted = factory.extend(b_اندريون, y_ويڪر, is_signed);
		ڳنڍ_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		ڳنڍ_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		ڳنڍ_2 a_extra = factory.extend(a, y_ويڪر + 1, خطأ);
		ڳنڍ_2 b_extra = factory.extend(b, y_ويڪر + 1, خطأ);
		ڳنڍ_2 y_extra = factory.جوڙيو(factory.جوڙيو(a_extra, b_extra), factory.extend(ci, a.ويڪر() + 1, خطأ));
		ڳنڍ_2 y = factory.ٽڪرو(y_extra, 0, y_ويڪر);
		ڳنڍ_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		ڳنڍ_2 co = factory.ٽڪرو(carries, 1, y_ويڪر);
		أرجع {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	ڳنڍ_2 handle_lcu(ڳنڍ_2 p, ڳنڍ_2 g, ڳنڍ_2 ci) {
		أرجع handle_alu(g, factory.bitwise_or(p, g), g.ويڪر(), خطأ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عام:
	std::variant<dict<IdString, ڳنڍ_2>, ڳنڍ_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, ڳنڍ_2> inputs)
	{
		صحيح a_ويڪر = parameters.at(ID(A_ويڪر), Const(-1)).as_int();
		صحيح b_ويڪر = parameters.at(ID(B_ويڪر), Const(-1)).as_int();
		صحيح y_ويڪر = parameters.at(ID(Y_ويڪر), Const(-1)).as_int();
		منطقي a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقي b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		إذا(cellType.اندريون(ID($جوڙيو), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقي is_signed = a_signed && b_signed;
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), y_ويڪر, is_signed);
			ڳنڍ_2 b = factory.extend(inputs.at(ID(B)), y_ويڪر, is_signed);
			إذا(cellType == ID($جوڙيو))
				أرجع factory.جوڙيو(a, b);
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
		}وإلا إذا(cellType.اندريون(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقي is_signed = a_signed && b_signed;
			صحيح ويڪر = max(a_ويڪر, b_ويڪر);
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), ويڪر, is_signed);
			ڳنڍ_2 b = factory.extend(inputs.at(ID(B)), ويڪر, is_signed);
			إذا(cellType.اندريون(ID($eq), ID($eqx)))
				أرجع factory.extend(factory.equal(a, b), y_ويڪر, خطأ);
			وإلا إذا(cellType.اندريون(ID($ne), ID($nex)))
				أرجع factory.extend(factory.not_equal(a, b), y_ويڪر, خطأ);
			وإلا إذا(cellType == ID($lt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_ويڪر, خطأ);
			وإلا إذا(cellType == ID($le))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_ويڪر, خطأ);
			وإلا إذا(cellType == ID($gt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_ويڪر, خطأ);
			وإلا إذا(cellType == ID($ge))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_ويڪر, خطأ);
			وإلا
				log_abort();
		}وإلا إذا(cellType.اندريون(ID($logic_or), ID($logic_and))){
			ڳنڍ_2 a = factory.reduce_or(inputs.at(ID(A)));
			ڳنڍ_2 b = factory.reduce_or(inputs.at(ID(B)));
			ڳنڍ_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			أرجع factory.extend(y, y_ويڪر, خطأ);
		}وإلا إذا(cellType == ID($not)){
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), y_ويڪر, a_signed);
			أرجع factory.bitwise_not(a);
		}وإلا إذا(cellType == ID($pos)){
			أرجع factory.extend(inputs.at(ID(A)), y_ويڪر, a_signed);
		}وإلا إذا(cellType == ID($barrier)){
			أرجع inputs.at(ID(A));
		}وإلا إذا(cellType == ID($neg)){
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), y_ويڪر, a_signed);
			أرجع factory.unary_minus(a);
		}وإلا إذا(cellType == ID($logic_not)){
			ڳنڍ_2 a = factory.reduce_or(inputs.at(ID(A)));
			ڳنڍ_2 y = factory.bitwise_not(a);
			أرجع factory.extend(y, y_ويڪر, خطأ);
		}وإلا إذا(cellType.اندريون(ID($reduce_or), ID($reduce_bool))){
			ڳنڍ_2 a = factory.reduce_or(inputs.at(ID(A)));
			أرجع factory.extend(a, y_ويڪر, خطأ);
		}وإلا إذا(cellType == ID($reduce_and)){
			ڳنڍ_2 a = factory.reduce_and(inputs.at(ID(A)));
			أرجع factory.extend(a, y_ويڪر, خطأ);
		}وإلا إذا(cellType.اندريون(ID($reduce_xor), ID($reduce_xnor))){
			ڳنڍ_2 a = factory.reduce_xor(inputs.at(ID(A)));
			ڳنڍ_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			أرجع factory.extend(y, y_ويڪر, خطأ);
		}وإلا إذا(cellType == ID($shl) || cellType == ID($sshl)){
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), y_ويڪر, a_signed);
			ڳنڍ_2 b = inputs.at(ID(B));
			أرجع logical_سرڪايو_کاٻو(a, b);
		}وإلا إذا(cellType == ID($shr) || cellType == ID($sshr)){
			صحيح ويڪر = max(a_ويڪر, y_ويڪر);
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), ويڪر, a_signed);
			ڳنڍ_2 b = inputs.at(ID(B));
			ڳنڍ_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_سرڪايو_ساڄو(a, b) :
				logical_سرڪايو_ساڄو(a, b);
			أرجع factory.extend(y, y_ويڪر, a_signed);
		}وإلا إذا(cellType == ID($shiftx) || cellType == ID($سرڪايو)){
			صحيح ويڪر = max(a_ويڪر, y_ويڪر);
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), ويڪر, cellType == ID($سرڪايو) && a_signed);
			ڳنڍ_2 b = inputs.at(ID(B));
			ڳنڍ_2 shr = logical_سرڪايو_ساڄو(a, b);
			إذا(b_signed) {
				ڳنڍ_2 shl = logical_سرڪايو_کاٻو(a, factory.unary_minus(b));
				ڳنڍ_2 y = factory.mux(shr, shl, sign(b));
				أرجع factory.extend(y, y_ويڪر, خطأ);
			} وإلا {
				أرجع factory.extend(shr, y_ويڪر, خطأ);
			}
		}وإلا إذا(cellType == ID($mux)){
			أرجع factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($pmux)){
			أرجع handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($concat)){
			ڳنڍ_2 a = inputs.at(ID(A));
			ڳنڍ_2 b = inputs.at(ID(B));
			أرجع factory.concat(a, b);
		}وإلا إذا(cellType == ID($ٽڪرو)){
			صحيح وٿي = parameters.at(ID(وٿي_2)).as_int();
			ڳنڍ_2 a = inputs.at(ID(A));
			أرجع factory.ٽڪرو(a, وٿي, y_ويڪر);
		}وإلا إذا(cellType.اندريون(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحيح ويڪر = max(a_ويڪر, b_ويڪر);
			منطقي is_signed = a_signed && b_signed;
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), ويڪر, is_signed);
			ڳنڍ_2 b = factory.extend(inputs.at(ID(B)), ويڪر, is_signed);
			إذا(is_signed) {
				إذا(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					ڳنڍ_2 abs_y = factory.unsigned_div(مطلق_قدر(a), مطلق_قدر(b));
					ڳنڍ_2 ٻاهريون_sign = factory.not_equal(sign(a), sign(b));
					أرجع neg_if(factory.extend(abs_y, y_ويڪر, خطأ), ٻاهريون_sign);
				} وإلا إذا(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					ڳنڍ_2 abs_y = factory.unsigned_mod(مطلق_قدر(a), مطلق_قدر(b));
					أرجع neg_if(factory.extend(abs_y, y_ويڪر, خطأ), sign(a));
				} وإلا إذا(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					ڳنڍ_2 b_sign = sign(b);
					ڳنڍ_2 a1 = neg_if(a, b_sign);
					ڳنڍ_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					ڳنڍ_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					ڳنڍ_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					ڳنڍ_2 y1 = factory.unsigned_div(a2, b1);
					ڳنڍ_2 y2 = factory.extend(y1, y_ويڪر, خطأ);
					أرجع factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} وإلا إذا(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					ڳنڍ_2 abs_b = مطلق_قدر(b);
					ڳنڍ_2 abs_y = factory.unsigned_mod(مطلق_قدر(a), abs_b);
					ڳنڍ_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					ڳنڍ_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					ڳنڍ_2 y = neg_if(y_flipped, sign(b));
					أرجع factory.extend(y, y_ويڪر, سچ);
				} وإلا
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} وإلا {
				إذا(cellType.اندريون(ID($mod), ID($modfloor)))
					أرجع factory.extend(factory.unsigned_mod(a, b), y_ويڪر, خطأ);
				وإلا
					أرجع factory.extend(factory.unsigned_div(a, b), y_ويڪر, خطأ);
			}
		} وإلا إذا(cellType == ID($قوت)) {
			أرجع handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_ويڪر, a_signed && b_signed);
		} وإلا إذا (cellType == ID($lut)) {
			صحيح ويڪر = parameters.at(ID(ويڪر_2)).as_int();
			Const lut_جدول = parameters.at(ID(LUT));
			lut_جدول.extu(1 << ويڪر);
			أرجع handle_bmux(factory.constant(lut_جدول), inputs.at(ID(A)), 0, 1, ويڪر);
		} وإلا إذا (cellType == ID($bwmux)) {
			ڳنڍ_2 a = inputs.at(ID(A));
			ڳنڍ_2 b = inputs.at(ID(B));
			ڳنڍ_2 s = inputs.at(ID(S));
			أرجع factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} وإلا إذا (cellType == ID($bweqx)) {
			ڳنڍ_2 a = inputs.at(ID(A));
			ڳنڍ_2 b = inputs.at(ID(B));
			أرجع factory.bitwise_not(factory.bitwise_xor(a, b));
		} وإلا إذا(cellType == ID($bmux)) {
			صحيح ويڪر = parameters.at(ID(ويڪر_2)).as_int();
			صحيح s_ويڪر = parameters.at(ID(S_ويڪر)).as_int();
			أرجع handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, ويڪر, s_ويڪر);
		} وإلا إذا(cellType == ID($demux)) {
			صحيح ويڪر = parameters.at(ID(ويڪر_2)).as_int();
			صحيح s_ويڪر = parameters.at(ID(S_ويڪر)).as_int();
			صحيح y_ويڪر = ويڪر << s_ويڪر;
			صحيح b_ويڪر = ceil_log2(y_ويڪر);
			ڳنڍ_2 a = factory.extend(inputs.at(ID(A)), y_ويڪر, خطأ);
			ڳنڍ_2 s = factory.extend(inputs.at(ID(S)), b_ويڪر, خطأ);
			ڳنڍ_2 b = factory.mul(s, factory.constant(Const(ويڪر, b_ويڪر)));
			أرجع factory.logical_سرڪايو_کاٻو(a, b);
		} وإلا إذا(cellType == ID($fa)) {
			أرجع handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} وإلا إذا(cellType == ID($lcu)) {
			أرجع handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} وإلا إذا(cellType == ID($alu)) {
			أرجع handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_ويڪر, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} وإلا إذا(cellType.اندريون(ID($أكد), ID($assume), ID($جيئرو), ID($fair), ID($cover))) {
			ڳنڍ_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			خودڪار &output = factory.جوڙيو_output(cellName, cellType, ترتيب_ڏيو_2(1));
			output.سيٽ_قدر(a);
			أرجع {};
		} وإلا إذا(cellType.اندريون(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحيح ويڪر = parameters.at(ID(ويڪر_2)).as_int();
			خودڪار &داخلا = factory.جوڙيو_داخلا(cellName, cellType, ترتيب_ڏيو_2(ويڪر));
			أرجع factory.قدر(داخلا);
		} وإلا إذا(cellType == ID($initstate)) {
			إذا(factory.ir().has_state(ID($initstate), ID($state)))
				أرجع factory.قدر(factory.ir().state(ID($initstate)));
			وإلا {
				خودڪار &state = factory.جوڙيو_state(ID($initstate), ID($state), ترتيب_ڏيو_2(1));
				state.سيٽ_initial_قدر(RTLIL::Const(State::S1, 1));
				state.سيٽ_ايندڙ_قدر(factory.constant(RTLIL::Const(State::S0, 1)));
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
	dict<DriveSpec, ڳنڍ_2> گراف_nodes;
	dict<std::pair<Cell *, IdString>, ڳنڍ_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	ويڪٽر<Mem> memories_ويڪٽر;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	ڳنڍ_2 قطار_اندر_وجهو(DriveSpec ثابت &spec)
	{
		خودڪار it = گراف_nodes.ڳوليو(spec);
		إذا(it == گراف_nodes.پڇاڙي()){
			خودڪار ڳنڍ = factory.create_pending(spec.سائيز());
			گراف_nodes.وجهو({spec, ڳنڍ});
			قطار.emplace_پٺ(spec);
			أرجع ڳنڍ;
		}وإلا
			أرجع it->ٻيو;
	}
	ڳنڍ_2 قطار_اندر_وجهو_cell(Cell *cell, IdString پورٽ_name)
	{
		خودڪار it = cell_outputs.ڳوليو({cell, پورٽ_name});
		إذا(it == cell_outputs.پڇاڙي()) {
			قطار.emplace_پٺ(cell);
			std::optional<ڳنڍ_2> rv;
			لكل(خودڪار ثابت &[name, sigspec] : cell->connections())
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					خودڪار ڳنڍ = factory.create_pending(sigspec.سائيز());
					factory.suggest_name(ڳنڍ, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, ڳنڍ);
					إذا(name == پورٽ_name)
						rv = ڳنڍ;
				}
			أرجع *rv;
		} وإلا
			أرجع it->ٻيو;
	}
عام:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.جوڙيو(module);
		لكل (خودڪار cell : module->cells()) {
			إذا (cell->type.اندريون(ID($أكد), ID($assume), ID($جيئرو), ID($fair), ID($cover), ID($check)))
				قطار.emplace_پٺ(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		لكل (خودڪار riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			خودڪار *wire = module->wire(*riter);
			إذا (wire && wire->پورٽ_داخلا) {
				factory.جوڙيو_داخلا(wire->name, ID($داخلا), ترتيب_ڏيو_2(wire->ويڪر));
			}
			إذا (wire && wire->پورٽ_output) {
				خودڪار &output = factory.جوڙيو_output(wire->name, ID($output), ترتيب_ڏيو_2(wire->ويڪر));
				output.سيٽ_قدر(قطار_اندر_وجهو(DriveChunk(DriveChunkWire(wire, 0, wire->ويڪر))));
			}
		}
		memories_ويڪٽر = Mem::get_all_memories(module);
		لكل (خودڪار &mem : memories_ويڪٽر) {
			إذا (mem.cell != خالي_اشارو)
				memories[mem.cell] = &mem;
		}
	}
خاص:
	ڳنڍ_2 concatenate_read_results(Mem *mem, ويڪٽر<ڳنڍ_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_معلومات = mem->cell->connections().at(ID(RD_معلومات));
		صحيح current = 0;
		لكل(سائيز_t i = 0; i < mem->rd_ports.سائيز(); i++) {
			صحيح ويڪر = mem->ويڪر << mem->rd_ports[i].wide_log2;
			log_assert (results[i].ويڪر() == ويڪر);
			log_assert (mem->rd_ports[i].معلومات == rd_معلومات.ڪڍو_2(current, ويڪر));
			current += ويڪر;
		}
		log_assert (current == rd_معلومات.سائيز());
		log_assert (!results.خالي_جايون());
		ڳنڍ_2 ڳنڍ = results[0];
		لكل(سائيز_t i = 1; i < results.سائيز(); i++)
			ڳنڍ = factory.concat(ڳنڍ, results[i]);
		أرجع ڳنڍ;
	}
	ڳنڍ_2 handle_يادگيري(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		ويڪٽر<ڳنڍ_2> read_results;
		خودڪار &state = factory.جوڙيو_state(mem->cell->name, ID($state), ترتيب_ڏيو_2(ceil_log2(mem->سائيز), mem->ويڪر));
		state.سيٽ_initial_قدر(MemContents(mem));
		ڳنڍ_2 ڳنڍ = factory.قدر(state);
		لكل (سائيز_t i = 0; i < mem->wr_ports.سائيز(); i++) {
			ثابت خودڪار &wr = mem->wr_ports[i];
			إذا (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ڳنڍ_2 en = قطار_اندر_وجهو(driver_map(DriveSpec(wr.en)));
			ڳنڍ_2 addr = قطار_اندر_وجهو(driver_map(DriveSpec(wr.addr)));
			ڳنڍ_2 new_معلومات = قطار_اندر_وجهو(driver_map(DriveSpec(wr.معلومات)));
			ڳنڍ_2 old_معلومات = factory.يادگيري_read(ڳنڍ, addr);
			ڳنڍ_2 wr_معلومات = simplifier.bitwise_mux(old_معلومات, new_معلومات, en);
			ڳنڍ = factory.يادگيري_write(ڳنڍ, addr, wr_معلومات);
		}
		إذا (mem->rd_ports.خالي_جايون())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		لكل (سائيز_t i = 0; i < mem->rd_ports.سائيز(); i++) {
			ثابت خودڪار &rd = mem->rd_ports[i];
			إذا (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ڳنڍ_2 addr = قطار_اندر_وجهو(driver_map(DriveSpec(rd.addr)));
			read_results.ڌڪو_پٺ(factory.يادگيري_read(ڳنڍ, addr));
		}
		state.سيٽ_ايندڙ_قدر(ڳنڍ);
		أرجع concatenate_read_results(mem, read_results);
	}
	فراغ process_cell(Cell *cell)
	{
		إذا (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, خالي_اشارو);
			إذا (mem == خالي_اشارو) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			ڳنڍ_2 ڳنڍ = handle_يادگيري(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_معلومات)}), ڳنڍ);
		} وإلا إذا (cell->is_builtin_ff()) {
			Ff_معلومات ff(&ff_initvals, cell);
			إذا (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			خودڪار &state = factory.جوڙيو_state(ff.name, ID($state), ترتيب_ڏيو_2(ff.ويڪر));
			ڳنڍ_2 q_قدر = factory.قدر(state);
			factory.suggest_name(q_قدر, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_قدر);
			state.سيٽ_ايندڙ_قدر(قطار_اندر_وجهو(ff.sig_d));
			state.سيٽ_initial_قدر(ff.val_init);
		} وإلا {
			dict<IdString, ڳنڍ_2> connections;
			IdString output_name; // for the single output case
			صحيح n_outputs = 0;
			لكل(خودڪار ثابت &[name, sigspec] : cell->connections()) {
				إذا(driver_map.celltypes.cell_داخلا(cell->type, name) && sigspec.سائيز() > 0)
					connections.وجهو({ name, قطار_اندر_وجهو(Drive_Chunk_پورٽ(cell, {name, sigspec})) });
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, ڳنڍ_2>, ڳنڍ_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			إذا(خودڪار *nodep = std::get_if<ڳنڍ_2>(&outputs); nodep != خالي_اشارو) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} وإلا {
				لكل(خودڪار [name, ڳنڍ] : std::get<dict<IdString, ڳنڍ_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), ڳنڍ);
			}
		}
	}
	فراغ undriven(ثابت std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	فراغ check_undriven(DriveSpec ثابت& spec, std::string ثابت& name) {
		لكل(خودڪار ثابت &chunk : spec.chunks())
			إذا(chunk.is_none())
				undriven(name);
	}
عام:
	فراغ process_قطار()
	{
		لكل (; !قطار.خالي_جايون(); قطار.ڪڍو_front()) {
			إذا(خودڪار p = std::get_if<Cell *>(&قطار.front()); p != خالي_اشارو) {
				process_cell(*p);
				استمر;
			}

			DriveSpec spec = std::get<DriveSpec>(قطار.front());
			ڳنڍ_2 pending = گراف_nodes.at(spec);

			إذا (spec.chunks().سائيز() > 1) {
				خودڪار chunks = spec.chunks();
				ڳنڍ_2 ڳنڍ = قطار_اندر_وجهو(chunks[0]);
				لكل(سائيز_t i = 1; i < chunks.سائيز(); i++)
					ڳنڍ = factory.concat(ڳنڍ, قطار_اندر_وجهو(chunks[i]));
				factory.update_pending(pending, ڳنڍ);
			} وإلا إذا (spec.chunks().سائيز() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				إذا (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					إذا (wire_chunk.is_سڄو()) {
						إذا (wire_chunk.wire->پورٽ_داخلا) {
							ڳنڍ_2 ڳنڍ = factory.قدر(factory.ir().داخلا(wire_chunk.wire->name));
							factory.suggest_name(ڳنڍ, wire_chunk.wire->name);
							factory.update_pending(pending, ڳنڍ);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							ڳنڍ_2 ڳنڍ = قطار_اندر_وجهو(driver);
							factory.suggest_name(ڳنڍ, wire_chunk.wire->name);
							factory.update_pending(pending, ڳنڍ);
						}
					} وإلا {
						DriveChunkWire سڄو_wire(wire_chunk.wire, 0, wire_chunk.wire->ويڪر);
						ڳنڍ_2 ڳنڍ = factory.ٽڪرو(قطار_اندر_وجهو(سڄو_wire), wire_chunk.وٿي, wire_chunk.ويڪر);
						factory.update_pending(pending, ڳنڍ);
					}
				} وإلا إذا (chunk.is_پورٽ()) {
					Drive_Chunk_پورٽ پورٽ_chunk = chunk.پورٽ();
					إذا (پورٽ_chunk.is_سڄو()) {
						إذا (driver_map.celltypes.cell_output(پورٽ_chunk.cell->type, پورٽ_chunk.پورٽ)) {
							ڳنڍ_2 ڳنڍ = قطار_اندر_وجهو_cell(پورٽ_chunk.cell, پورٽ_chunk.پورٽ);
							factory.update_pending(pending, ڳنڍ);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(پورٽ_chunk));
							check_undriven(driver, پورٽ_chunk.cell->name.unescape() + " port " + PooledName(پورٽ_chunk.cell->module, پورٽ_chunk.پورٽ).unescape());
							factory.update_pending(pending, قطار_اندر_وجهو(driver));
						}
					} وإلا {
						Drive_Chunk_پورٽ سڄو_پورٽ(پورٽ_chunk.cell, پورٽ_chunk.پورٽ, 0, Get_سائيز(پورٽ_chunk.cell->connections().at(پورٽ_chunk.پورٽ)));
						ڳنڍ_2 ڳنڍ = factory.ٽڪرو(قطار_اندر_وجهو(سڄو_پورٽ), پورٽ_chunk.وٿي, پورٽ_chunk.ويڪر);
						factory.update_pending(pending, ڳنڍ);
					}
				} وإلا إذا (chunk.is_constant()) {
					ڳنڍ_2 ڳنڍ = factory.constant(chunk.constant());
					factory.suggest_name(ڳنڍ, "$const" + std::منزل_string(chunk.سائيز()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, ڳنڍ);
				} وإلا إذا (chunk.is_گڻو()) {
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
    خودڪار factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_قطار();
    ir.topological_ترتيب_ڏيو();
    ir.forward_buf();
    أرجع ir;
}

فراغ IR::topological_ترتيب_ڏيو() {
    گراف::SccAdaptor compute_گراف_scc(گراف_4);
    منطقي scc = خطأ;
    std::ويڪٽر<صحيح> perm;
    TopoSortedSccs toposort(compute_گراف_scc, [&](صحيح *begin, صحيح *پڇاڙي) {
        perm.وجهو(perm.پڇاڙي(), begin, پڇاڙي);
        إذا (پڇاڙي > begin + 1)
        {
            log_warning("Combinational loop:\n");
            لكل (صحيح *i = begin; i != پڇاڙي; ++i) {
				ڳنڍ_2 ڳنڍ(گراف_4[*i], design);
                لاگرٿم("- %s = %s\n", ڳنڍ.name().unescape(), ڳنڍ.منزل_string());
			}
            لاگرٿم("\n");
            scc = سچ;
        }
    });
	لكل(ثابت خودڪار &[name, state]: حالتون_2)
		إذا(state.has_ايندڙ_قدر())
			toposort.process(state.ايندڙ_قدر().id());
	لكل(ثابت خودڪار &[name, output]: _outputs)
		إذا(output.has_قدر())
			toposort.process(output.قدر().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    گراف_4.ترتيب_بدلايو(perm);
    إذا(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ساكن IdString ملايو_name(IdString a, IdString b) {
	إذا(!a.is_عوامي() && b.is_عوامي())
		أرجع b;
	وإلا
		أرجع a;
}

فراغ IR::forward_buf() {
    std::ويڪٽر<صحيح> perm, alias;
    perm.clear();

    لكل (صحيح i = 0; i < گراف_4.سائيز(); ++i)
    {
        خودڪار ڳنڍ = گراف_4[i];
        إذا (ڳنڍ.function().fn() == Fn::buf && ڳنڍ.arg(0).اشاريو() < i)
        {
            صحيح هدف_اشاريو = alias[ڳنڍ.arg(0).اشاريو()];
            خودڪار هدف_ڳنڍ = گراف_4[perm[هدف_اشاريو]];
			إذا(ڳنڍ.has_sparse_attr()) {
				إذا(هدف_ڳنڍ.has_sparse_attr()) {
					IdString id = ملايو_name(ڳنڍ.sparse_attr(), هدف_ڳنڍ.sparse_attr());
					هدف_ڳنڍ.sparse_attr() = id;
				} وإلا {
					IdString id = ڳنڍ.sparse_attr();
					هدف_ڳنڍ.sparse_attr() = id;
				}
			}
            alias.ڌڪو_پٺ(هدف_اشاريو);
        }
        وإلا
        {
            alias.ڌڪو_پٺ(Get_سائيز(perm));
            perm.ڌڪو_پٺ(i);
        }
    }
    گراف_4.ترتيب_بدلايو(perm, alias);
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

فراغ Writer::print_impl(ثابت حرف *fmt, ويڪٽر<std::function<فراغ()>> &fns)
{
	سائيز_t ايندڙ_اشاريو = 0;
	لكل(ثابت حرف *p = fmt; *p != 0; p++)
		بدل(*p) {
		حالة '{':
			إذا(*++p == '{') {
				*os << '{';
			} وإلا {
				حرف *pe;
				سائيز_t اشاريو = strtoul(p, &pe, 10);
				إذا(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				إذا(p == pe)
					اشاريو = ايندڙ_اشاريو;
				وإلا
					p = pe;
				إذا(اشاريو >= fns.سائيز())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", اشاريو, fns.سائيز(), quote_fmt(fmt));
				fns[اشاريو]();
				ايندڙ_اشاريو = اشاريو + 1;
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
YOSYS_NAMESPACE_پڇاڙي
