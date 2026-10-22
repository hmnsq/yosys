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

ثابت حرف *fn_مقصد_string(Fn fn) {
	بدل(fn) {
	حالة Fn::invalid: أرجع "invalid";
	حالة Fn::buf: أرجع "buf";
	حالة Fn::برش: أرجع "slice";
	حالة Fn::zero_extend: أرجع "zero_extend";
	حالة Fn::sign_extend: أرجع "sign_extend";
	حالة Fn::concat: أرجع "concat";
	حالة Fn::جمع_کن: أرجع "add";
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
	حالة Fn::logical_لغزش_چپ: أرجع "logical_shift_left";
	حالة Fn::logical_لغزش_راست: أرجع "logical_shift_right";
	حالة Fn::arithmetic_لغزش_راست: أرجع "arithmetic_shift_right";
	حالة Fn::mux: أرجع "mux";
	حالة Fn::constant: أرجع "constant";
	حالة Fn::درون_داد: أرجع "input";
	حالة Fn::state: أرجع "state";
	حالة Fn::حافظه_read: أرجع "memory_read";
	حالة Fn::حافظه_write: أرجع "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحيح)fn);
}

وکتور<IRInput ثابت*> IR::inputs(IdString نوع) ثابت {
	وکتور<IRInput ثابت*> ret;
	لكل (ثابت خودکار &[name, درون_داد] : _inputs)
		إذا(درون_داد.نوع == نوع)
			ret.افزودن_پشت(&درون_داد);
	أرجع ret;
}

وکتور<IROutput ثابت*> IR::outputs(IdString نوع) ثابت {
	وکتور<IROutput ثابت*> ret;
	لكل (ثابت خودکار &[name, output] : _outputs)
		إذا(output.نوع == نوع)
			ret.افزودن_پشت(&output);
	أرجع ret;
}

وکتور<IRState ثابت*> IR::حالتها(IdString نوع) ثابت {
	وکتور<IRState ثابت*> ret;
	لكل (ثابت خودکار &[name, state] : حالتها_2)
		إذا(state.نوع == نوع)
			ret.افزودن_پشت(&state);
	أرجع ret;
}

وکتور<IRInput ثابت*> IR::all_inputs() ثابت {
	وکتور<IRInput ثابت*> ret;
	لكل (ثابت خودکار &[name, درون_داد] : _inputs)
		ret.افزودن_پشت(&درون_داد);
	أرجع ret;
}

وکتور<IROutput ثابت*> IR::all_outputs() ثابت {
	وکتور<IROutput ثابت*> ret;
	لكل (ثابت خودکار &[name, output] : _outputs)
		ret.افزودن_پشت(&output);
	أرجع ret;
}

وکتور<IRState ثابت*> IR::all_حالتها() ثابت {
	وکتور<IRState ثابت*> ret;
	لكل (ثابت خودکار &[name, state] : حالتها_2)
		ret.افزودن_پشت(&state);
	أرجع ret;
}

بنية PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(گره_2)> np;
	PrintVisitor(std::function<std::string(گره_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string برش(گره_2, گره_2 a, صحيح جابه_جایی, صحيح بیرون_عرض) override { أرجع "slice(" + np(a) + ", " + std::مقصد_string(جابه_جایی) + ", " + std::مقصد_string(بیرون_عرض) + ")"; }
	std::string zero_extend(گره_2, گره_2 a, صحيح بیرون_عرض) override { أرجع "zero_extend(" + np(a) + ", " + std::مقصد_string(بیرون_عرض) + ")"; }
	std::string sign_extend(گره_2, گره_2 a, صحيح بیرون_عرض) override { أرجع "sign_extend(" + np(a) + ", " + std::مقصد_string(بیرون_عرض) + ")"; }
	std::string constant(گره_2, RTLIL::Const ثابت& مقدار) override { أرجع "constant(" + مقدار.as_string() + ")"; }
	std::string درون_داد(گره_2 self, IdString name, IdString نوع) override { أرجع "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(نوع) + ")"; }
	std::string state(گره_2 self, IdString name, IdString نوع) override { أرجع "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(نوع) + ")"; }
	std::string default_handler(گره_2 self) override {
		std::string ret = fn_مقصد_string(self.fn());
		ret += "(";
		لكل(اندازه_t i = 0; i < self.arg_شمار(); i++) {
			إذا(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		أرجع ret;
	}
};

std::string گره_2::مقصد_string()
{
	أرجع مقصد_string([](گره_2 n) { أرجع n.name().unescape(); });
}

std::string گره_2::مقصد_string(std::function<std::string(گره_2)> np)
{
	أرجع بازدید(PrintVisitor(np));
}

صنف CellSimplifier {
	Factory &factory;
	گره_2 sign(گره_2 a) {
		أرجع factory.برش(a, a.عرض() - 1, 1);
	}
	گره_2 neg_if(گره_2 a, گره_2 s) {
		أرجع factory.mux(a, factory.unary_minus(a), s);
	}
	گره_2 قدر_مطلق(گره_2 a) {
		أرجع neg_if(a, sign(a));
	}
	گره_2 handle_لغزش(گره_2 a, گره_2 b, منطقي is_راست, منطقي is_signed) {
		// to prevent new_width == 0, we handle this case separately
		إذا(a.عرض() == 1) {
			إذا(!is_signed)
				أرجع factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			وإلا
				أرجع a;
		}
		صحيح new_عرض = ceil_log2(a.عرض());
		گره_2 b_truncated = factory.extend(b, new_عرض, خطأ);
		گره_2 y =
			!is_راست ? factory.logical_لغزش_چپ(a, b_truncated) :
			!is_signed ? factory.logical_لغزش_راست(a, b_truncated) :
			factory.arithmetic_لغزش_راست(a, b_truncated);
		إذا(b.عرض() <= new_عرض)
			أرجع y;
		گره_2 سرریز = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.عرض(), b.عرض())));
		گره_2 y_if_سرریز = is_signed ? factory.extend(sign(a), a.عرض(), درست) : factory.constant(RTLIL::Const(State::S0, a.عرض()));
		أرجع factory.mux(y, y_if_سرریز, سرریز);
	}
عام:
	گره_2 logical_لغزش_چپ(گره_2 a, گره_2 b) { أرجع handle_لغزش(a, b, خطأ, خطأ); }
	گره_2 logical_لغزش_راست(گره_2 a, گره_2 b) { أرجع handle_لغزش(a, b, درست, خطأ); }
	گره_2 arithmetic_لغزش_راست(گره_2 a, گره_2 b) { أرجع handle_لغزش(a, b, درست, درست); }
	گره_2 bitwise_mux(گره_2 a, گره_2 b, گره_2 s) {
		گره_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		گره_2 bb = factory.bitwise_and(b, s);
		أرجع factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
خاص:
	گره_2 handle_pow(گره_2 a0, گره_2 b, صحيح y_عرض, منطقي is_signed) {
		گره_2 a = factory.extend(a0, y_عرض, is_signed);
		گره_2 r = factory.constant(Const(1, y_عرض));
		لكل(صحيح i = 0; i < b.عرض(); i++) {
			گره_2 b_بیت = factory.برش(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_بیت);
			a = factory.mul(a, a);
		}
		إذا (is_signed) {
			گره_2 a_ge_1 = factory.unsigned_greater_than(قدر_مطلق(a0), factory.constant(Const(1, a0.عرض())));
			گره_2 zero_نتیجه = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_عرض)), zero_نتیجه);
		}
		أرجع r;
	}
	گره_2 handle_bmux(گره_2 a, گره_2 s, صحيح a_جابه_جایی, صحيح عرض, صحيح sn) {
		إذا(sn < 1)
			أرجع factory.برش(a, a_جابه_جایی, عرض);
		وإلا {
			گره_2 y0 = handle_bmux(a, s, a_جابه_جایی, عرض, sn - 1);
			گره_2 y1 = handle_bmux(a, s, a_جابه_جایی + (عرض << (sn - 1)), عرض, sn - 1);
			أرجع factory.mux(y0, y1, factory.برش(s, sn - 1, 1));
		}
	}
	گره_2 handle_pmux(گره_2 a, گره_2 b, گره_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.عرض() == a.عرض() * s.عرض());
		گره_2 y = a;
		لكل(صحيح i = 0; i < s.عرض(); i++)
			y = factory.mux(y, factory.برش(b, a.عرض() * i, a.عرض()), factory.برش(s, i, 1));
		أرجع y;
	}
	dict<IdString, گره_2> handle_fa(گره_2 a, گره_2 b, گره_2 c) {
		گره_2 t1 = factory.bitwise_xor(a, b);
		گره_2 t2 = factory.bitwise_and(a, b);
		گره_2 t3 = factory.bitwise_and(c, t1);
		گره_2 y = factory.bitwise_xor(c, t1);
		گره_2 x = factory.bitwise_or(t2, t3);
		أرجع {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, گره_2> handle_alu(گره_2 a_درون, گره_2 b_درون, صحيح y_عرض, منطقي is_signed, گره_2 ci, گره_2 bi) {
		گره_2 a = factory.extend(a_درون, y_عرض, is_signed);
		گره_2 b_uninverted = factory.extend(b_درون, y_عرض, is_signed);
		گره_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		گره_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		گره_2 a_extra = factory.extend(a, y_عرض + 1, خطأ);
		گره_2 b_extra = factory.extend(b, y_عرض + 1, خطأ);
		گره_2 y_extra = factory.جمع_کن(factory.جمع_کن(a_extra, b_extra), factory.extend(ci, a.عرض() + 1, خطأ));
		گره_2 y = factory.برش(y_extra, 0, y_عرض);
		گره_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		گره_2 co = factory.برش(carries, 1, y_عرض);
		أرجع {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	گره_2 handle_lcu(گره_2 p, گره_2 g, گره_2 ci) {
		أرجع handle_alu(g, factory.bitwise_or(p, g), g.عرض(), خطأ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عام:
	std::variant<dict<IdString, گره_2>, گره_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, گره_2> inputs)
	{
		صحيح a_عرض = parameters.at(ID(A_عرض), Const(-1)).as_int();
		صحيح b_عرض = parameters.at(ID(B_عرض), Const(-1)).as_int();
		صحيح y_عرض = parameters.at(ID(Y_عرض), Const(-1)).as_int();
		منطقي a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقي b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		إذا(cellType.درون(ID($جمع_کن), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقي is_signed = a_signed && b_signed;
			گره_2 a = factory.extend(inputs.at(ID(A)), y_عرض, is_signed);
			گره_2 b = factory.extend(inputs.at(ID(B)), y_عرض, is_signed);
			إذا(cellType == ID($جمع_کن))
				أرجع factory.جمع_کن(a, b);
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
		}وإلا إذا(cellType.درون(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقي is_signed = a_signed && b_signed;
			صحيح عرض = max(a_عرض, b_عرض);
			گره_2 a = factory.extend(inputs.at(ID(A)), عرض, is_signed);
			گره_2 b = factory.extend(inputs.at(ID(B)), عرض, is_signed);
			إذا(cellType.درون(ID($eq), ID($eqx)))
				أرجع factory.extend(factory.equal(a, b), y_عرض, خطأ);
			وإلا إذا(cellType.درون(ID($ne), ID($nex)))
				أرجع factory.extend(factory.not_equal(a, b), y_عرض, خطأ);
			وإلا إذا(cellType == ID($lt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_عرض, خطأ);
			وإلا إذا(cellType == ID($le))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_عرض, خطأ);
			وإلا إذا(cellType == ID($gt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_عرض, خطأ);
			وإلا إذا(cellType == ID($ge))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_عرض, خطأ);
			وإلا
				log_abort();
		}وإلا إذا(cellType.درون(ID($logic_or), ID($logic_and))){
			گره_2 a = factory.reduce_or(inputs.at(ID(A)));
			گره_2 b = factory.reduce_or(inputs.at(ID(B)));
			گره_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			أرجع factory.extend(y, y_عرض, خطأ);
		}وإلا إذا(cellType == ID($not)){
			گره_2 a = factory.extend(inputs.at(ID(A)), y_عرض, a_signed);
			أرجع factory.bitwise_not(a);
		}وإلا إذا(cellType == ID($pos)){
			أرجع factory.extend(inputs.at(ID(A)), y_عرض, a_signed);
		}وإلا إذا(cellType == ID($barrier)){
			أرجع inputs.at(ID(A));
		}وإلا إذا(cellType == ID($neg)){
			گره_2 a = factory.extend(inputs.at(ID(A)), y_عرض, a_signed);
			أرجع factory.unary_minus(a);
		}وإلا إذا(cellType == ID($logic_not)){
			گره_2 a = factory.reduce_or(inputs.at(ID(A)));
			گره_2 y = factory.bitwise_not(a);
			أرجع factory.extend(y, y_عرض, خطأ);
		}وإلا إذا(cellType.درون(ID($reduce_or), ID($reduce_bool))){
			گره_2 a = factory.reduce_or(inputs.at(ID(A)));
			أرجع factory.extend(a, y_عرض, خطأ);
		}وإلا إذا(cellType == ID($reduce_and)){
			گره_2 a = factory.reduce_and(inputs.at(ID(A)));
			أرجع factory.extend(a, y_عرض, خطأ);
		}وإلا إذا(cellType.درون(ID($reduce_xor), ID($reduce_xnor))){
			گره_2 a = factory.reduce_xor(inputs.at(ID(A)));
			گره_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			أرجع factory.extend(y, y_عرض, خطأ);
		}وإلا إذا(cellType == ID($shl) || cellType == ID($sshl)){
			گره_2 a = factory.extend(inputs.at(ID(A)), y_عرض, a_signed);
			گره_2 b = inputs.at(ID(B));
			أرجع logical_لغزش_چپ(a, b);
		}وإلا إذا(cellType == ID($shr) || cellType == ID($sshr)){
			صحيح عرض = max(a_عرض, y_عرض);
			گره_2 a = factory.extend(inputs.at(ID(A)), عرض, a_signed);
			گره_2 b = inputs.at(ID(B));
			گره_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_لغزش_راست(a, b) :
				logical_لغزش_راست(a, b);
			أرجع factory.extend(y, y_عرض, a_signed);
		}وإلا إذا(cellType == ID($shiftx) || cellType == ID($لغزش)){
			صحيح عرض = max(a_عرض, y_عرض);
			گره_2 a = factory.extend(inputs.at(ID(A)), عرض, cellType == ID($لغزش) && a_signed);
			گره_2 b = inputs.at(ID(B));
			گره_2 shr = logical_لغزش_راست(a, b);
			إذا(b_signed) {
				گره_2 shl = logical_لغزش_چپ(a, factory.unary_minus(b));
				گره_2 y = factory.mux(shr, shl, sign(b));
				أرجع factory.extend(y, y_عرض, خطأ);
			} وإلا {
				أرجع factory.extend(shr, y_عرض, خطأ);
			}
		}وإلا إذا(cellType == ID($mux)){
			أرجع factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($pmux)){
			أرجع handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($concat)){
			گره_2 a = inputs.at(ID(A));
			گره_2 b = inputs.at(ID(B));
			أرجع factory.concat(a, b);
		}وإلا إذا(cellType == ID($برش)){
			صحيح جابه_جایی = parameters.at(ID(جابه_جایی_2)).as_int();
			گره_2 a = inputs.at(ID(A));
			أرجع factory.برش(a, جابه_جایی, y_عرض);
		}وإلا إذا(cellType.درون(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحيح عرض = max(a_عرض, b_عرض);
			منطقي is_signed = a_signed && b_signed;
			گره_2 a = factory.extend(inputs.at(ID(A)), عرض, is_signed);
			گره_2 b = factory.extend(inputs.at(ID(B)), عرض, is_signed);
			إذا(is_signed) {
				إذا(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					گره_2 abs_y = factory.unsigned_div(قدر_مطلق(a), قدر_مطلق(b));
					گره_2 بیرون_sign = factory.not_equal(sign(a), sign(b));
					أرجع neg_if(factory.extend(abs_y, y_عرض, خطأ), بیرون_sign);
				} وإلا إذا(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					گره_2 abs_y = factory.unsigned_mod(قدر_مطلق(a), قدر_مطلق(b));
					أرجع neg_if(factory.extend(abs_y, y_عرض, خطأ), sign(a));
				} وإلا إذا(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					گره_2 b_sign = sign(b);
					گره_2 a1 = neg_if(a, b_sign);
					گره_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					گره_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					گره_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					گره_2 y1 = factory.unsigned_div(a2, b1);
					گره_2 y2 = factory.extend(y1, y_عرض, خطأ);
					أرجع factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} وإلا إذا(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					گره_2 abs_b = قدر_مطلق(b);
					گره_2 abs_y = factory.unsigned_mod(قدر_مطلق(a), abs_b);
					گره_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					گره_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					گره_2 y = neg_if(y_flipped, sign(b));
					أرجع factory.extend(y, y_عرض, درست);
				} وإلا
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} وإلا {
				إذا(cellType.درون(ID($mod), ID($modfloor)))
					أرجع factory.extend(factory.unsigned_mod(a, b), y_عرض, خطأ);
				وإلا
					أرجع factory.extend(factory.unsigned_div(a, b), y_عرض, خطأ);
			}
		} وإلا إذا(cellType == ID($توان)) {
			أرجع handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_عرض, a_signed && b_signed);
		} وإلا إذا (cellType == ID($lut)) {
			صحيح عرض = parameters.at(ID(عرض_2)).as_int();
			Const lut_جدول = parameters.at(ID(LUT));
			lut_جدول.extu(1 << عرض);
			أرجع handle_bmux(factory.constant(lut_جدول), inputs.at(ID(A)), 0, 1, عرض);
		} وإلا إذا (cellType == ID($bwmux)) {
			گره_2 a = inputs.at(ID(A));
			گره_2 b = inputs.at(ID(B));
			گره_2 s = inputs.at(ID(S));
			أرجع factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} وإلا إذا (cellType == ID($bweqx)) {
			گره_2 a = inputs.at(ID(A));
			گره_2 b = inputs.at(ID(B));
			أرجع factory.bitwise_not(factory.bitwise_xor(a, b));
		} وإلا إذا(cellType == ID($bmux)) {
			صحيح عرض = parameters.at(ID(عرض_2)).as_int();
			صحيح s_عرض = parameters.at(ID(S_عرض)).as_int();
			أرجع handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, عرض, s_عرض);
		} وإلا إذا(cellType == ID($demux)) {
			صحيح عرض = parameters.at(ID(عرض_2)).as_int();
			صحيح s_عرض = parameters.at(ID(S_عرض)).as_int();
			صحيح y_عرض = عرض << s_عرض;
			صحيح b_عرض = ceil_log2(y_عرض);
			گره_2 a = factory.extend(inputs.at(ID(A)), y_عرض, خطأ);
			گره_2 s = factory.extend(inputs.at(ID(S)), b_عرض, خطأ);
			گره_2 b = factory.mul(s, factory.constant(Const(عرض, b_عرض)));
			أرجع factory.logical_لغزش_چپ(a, b);
		} وإلا إذا(cellType == ID($fa)) {
			أرجع handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} وإلا إذا(cellType == ID($lcu)) {
			أرجع handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} وإلا إذا(cellType == ID($alu)) {
			أرجع handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_عرض, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} وإلا إذا(cellType.درون(ID($أكد), ID($assume), ID($زنده), ID($fair), ID($cover))) {
			گره_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			خودکار &output = factory.جمع_کن_output(cellName, cellType, مرتب_سازی_2(1));
			output.مجموعه_مقدار(a);
			أرجع {};
		} وإلا إذا(cellType.درون(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحيح عرض = parameters.at(ID(عرض_2)).as_int();
			خودکار &درون_داد = factory.جمع_کن_درون_داد(cellName, cellType, مرتب_سازی_2(عرض));
			أرجع factory.مقدار(درون_داد);
		} وإلا إذا(cellType == ID($initstate)) {
			إذا(factory.ir().has_state(ID($initstate), ID($state)))
				أرجع factory.مقدار(factory.ir().state(ID($initstate)));
			وإلا {
				خودکار &state = factory.جمع_کن_state(ID($initstate), ID($state), مرتب_سازی_2(1));
				state.مجموعه_initial_مقدار(RTLIL::Const(State::S1, 1));
				state.مجموعه_بعدی_مقدار(factory.constant(RTLIL::Const(State::S0, 1)));
				أرجع factory.مقدار(state);
			}
		} وإلا إذا(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} وإلا {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

صنف FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> صف;
	dict<DriveSpec, گره_2> گراف_nodes;
	dict<std::pair<Cell *, IdString>, گره_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	وکتور<Mem> memories_وکتور;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	گره_2 افزودن_به_صف(DriveSpec ثابت &spec)
	{
		خودکار it = گراف_nodes.پیدا_کن(spec);
		إذا(it == گراف_nodes.انتها()){
			خودکار گره = factory.create_pending(spec.اندازه());
			گراف_nodes.درج({spec, گره});
			صف.emplace_پشت(spec);
			أرجع گره;
		}وإلا
			أرجع it->دومی;
	}
	گره_2 افزودن_به_صف_cell(Cell *cell, IdString پورت_name)
	{
		خودکار it = cell_outputs.پیدا_کن({cell, پورت_name});
		إذا(it == cell_outputs.انتها()) {
			صف.emplace_پشت(cell);
			std::optional<گره_2> rv;
			لكل(خودکار ثابت &[name, sigspec] : cell->connections())
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					خودکار گره = factory.create_pending(sigspec.اندازه());
					factory.suggest_name(گره, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, گره);
					إذا(name == پورت_name)
						rv = گره;
				}
			أرجع *rv;
		} وإلا
			أرجع it->دومی;
	}
عام:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.جمع_کن(module);
		لكل (خودکار cell : module->cells()) {
			إذا (cell->type.درون(ID($أكد), ID($assume), ID($زنده), ID($fair), ID($cover), ID($check)))
				صف.emplace_پشت(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		لكل (خودکار riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			خودکار *wire = module->wire(*riter);
			إذا (wire && wire->پورت_درون_داد) {
				factory.جمع_کن_درون_داد(wire->name, ID($درون_داد), مرتب_سازی_2(wire->عرض));
			}
			إذا (wire && wire->پورت_output) {
				خودکار &output = factory.جمع_کن_output(wire->name, ID($output), مرتب_سازی_2(wire->عرض));
				output.مجموعه_مقدار(افزودن_به_صف(DriveChunk(DriveChunkWire(wire, 0, wire->عرض))));
			}
		}
		memories_وکتور = Mem::get_all_memories(module);
		لكل (خودکار &mem : memories_وکتور) {
			إذا (mem.cell != اشاره_تهی)
				memories[mem.cell] = &mem;
		}
	}
خاص:
	گره_2 concatenate_read_results(Mem *mem, وکتور<گره_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_داده = mem->cell->connections().at(ID(RD_داده));
		صحيح current = 0;
		لكل(اندازه_t i = 0; i < mem->rd_ports.اندازه(); i++) {
			صحيح عرض = mem->عرض << mem->rd_ports[i].wide_log2;
			log_assert (results[i].عرض() == عرض);
			log_assert (mem->rd_ports[i].داده == rd_داده.استخراج(current, عرض));
			current += عرض;
		}
		log_assert (current == rd_داده.اندازه());
		log_assert (!results.خالی());
		گره_2 گره = results[0];
		لكل(اندازه_t i = 1; i < results.اندازه(); i++)
			گره = factory.concat(گره, results[i]);
		أرجع گره;
	}
	گره_2 handle_حافظه(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		وکتور<گره_2> read_results;
		خودکار &state = factory.جمع_کن_state(mem->cell->name, ID($state), مرتب_سازی_2(ceil_log2(mem->اندازه), mem->عرض));
		state.مجموعه_initial_مقدار(MemContents(mem));
		گره_2 گره = factory.مقدار(state);
		لكل (اندازه_t i = 0; i < mem->wr_ports.اندازه(); i++) {
			ثابت خودکار &wr = mem->wr_ports[i];
			إذا (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گره_2 en = افزودن_به_صف(driver_map(DriveSpec(wr.en)));
			گره_2 addr = افزودن_به_صف(driver_map(DriveSpec(wr.addr)));
			گره_2 new_داده = افزودن_به_صف(driver_map(DriveSpec(wr.داده)));
			گره_2 old_داده = factory.حافظه_read(گره, addr);
			گره_2 wr_داده = simplifier.bitwise_mux(old_داده, new_داده, en);
			گره = factory.حافظه_write(گره, addr, wr_داده);
		}
		إذا (mem->rd_ports.خالی())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		لكل (اندازه_t i = 0; i < mem->rd_ports.اندازه(); i++) {
			ثابت خودکار &rd = mem->rd_ports[i];
			إذا (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			گره_2 addr = افزودن_به_صف(driver_map(DriveSpec(rd.addr)));
			read_results.افزودن_پشت(factory.حافظه_read(گره, addr));
		}
		state.مجموعه_بعدی_مقدار(گره);
		أرجع concatenate_read_results(mem, read_results);
	}
	فراغ process_cell(Cell *cell)
	{
		إذا (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, اشاره_تهی);
			إذا (mem == اشاره_تهی) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			گره_2 گره = handle_حافظه(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_داده)}), گره);
		} وإلا إذا (cell->is_builtin_ff()) {
			Ff_داده ff(&ff_initvals, cell);
			إذا (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			خودکار &state = factory.جمع_کن_state(ff.name, ID($state), مرتب_سازی_2(ff.عرض));
			گره_2 q_مقدار = factory.مقدار(state);
			factory.suggest_name(q_مقدار, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_مقدار);
			state.مجموعه_بعدی_مقدار(افزودن_به_صف(ff.sig_d));
			state.مجموعه_initial_مقدار(ff.val_init);
		} وإلا {
			dict<IdString, گره_2> connections;
			IdString output_name; // for the single output case
			صحيح n_outputs = 0;
			لكل(خودکار ثابت &[name, sigspec] : cell->connections()) {
				إذا(driver_map.celltypes.cell_درون_داد(cell->type, name) && sigspec.اندازه() > 0)
					connections.درج({ name, افزودن_به_صف(Drive_Chunk_پورت(cell, {name, sigspec})) });
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, گره_2>, گره_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			إذا(خودکار *nodep = std::get_if<گره_2>(&outputs); nodep != اشاره_تهی) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} وإلا {
				لكل(خودکار [name, گره] : std::get<dict<IdString, گره_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), گره);
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
	فراغ process_صف()
	{
		لكل (; !صف.خالی(); صف.برداشت_front()) {
			إذا(خودکار p = std::get_if<Cell *>(&صف.front()); p != اشاره_تهی) {
				process_cell(*p);
				استمر;
			}

			DriveSpec spec = std::get<DriveSpec>(صف.front());
			گره_2 pending = گراف_nodes.at(spec);

			إذا (spec.chunks().اندازه() > 1) {
				خودکار chunks = spec.chunks();
				گره_2 گره = افزودن_به_صف(chunks[0]);
				لكل(اندازه_t i = 1; i < chunks.اندازه(); i++)
					گره = factory.concat(گره, افزودن_به_صف(chunks[i]));
				factory.update_pending(pending, گره);
			} وإلا إذا (spec.chunks().اندازه() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				إذا (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					إذا (wire_chunk.is_کامل()) {
						إذا (wire_chunk.wire->پورت_درون_داد) {
							گره_2 گره = factory.مقدار(factory.ir().درون_داد(wire_chunk.wire->name));
							factory.suggest_name(گره, wire_chunk.wire->name);
							factory.update_pending(pending, گره);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							گره_2 گره = افزودن_به_صف(driver);
							factory.suggest_name(گره, wire_chunk.wire->name);
							factory.update_pending(pending, گره);
						}
					} وإلا {
						DriveChunkWire کامل_wire(wire_chunk.wire, 0, wire_chunk.wire->عرض);
						گره_2 گره = factory.برش(افزودن_به_صف(کامل_wire), wire_chunk.جابه_جایی, wire_chunk.عرض);
						factory.update_pending(pending, گره);
					}
				} وإلا إذا (chunk.is_پورت()) {
					Drive_Chunk_پورت پورت_chunk = chunk.پورت();
					إذا (پورت_chunk.is_کامل()) {
						إذا (driver_map.celltypes.cell_output(پورت_chunk.cell->type, پورت_chunk.پورت)) {
							گره_2 گره = افزودن_به_صف_cell(پورت_chunk.cell, پورت_chunk.پورت);
							factory.update_pending(pending, گره);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(پورت_chunk));
							check_undriven(driver, پورت_chunk.cell->name.unescape() + " port " + PooledName(پورت_chunk.cell->module, پورت_chunk.پورت).unescape());
							factory.update_pending(pending, افزودن_به_صف(driver));
						}
					} وإلا {
						Drive_Chunk_پورت کامل_پورت(پورت_chunk.cell, پورت_chunk.پورت, 0, Get_اندازه(پورت_chunk.cell->connections().at(پورت_chunk.پورت)));
						گره_2 گره = factory.برش(افزودن_به_صف(کامل_پورت), پورت_chunk.جابه_جایی, پورت_chunk.عرض);
						factory.update_pending(pending, گره);
					}
				} وإلا إذا (chunk.is_constant()) {
					گره_2 گره = factory.constant(chunk.constant());
					factory.suggest_name(گره, "$const" + std::مقصد_string(chunk.اندازه()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, گره);
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
    ctor.process_صف();
    ir.topological_مرتب_سازی();
    ir.forward_buf();
    أرجع ir;
}

فراغ IR::topological_مرتب_سازی() {
    گراف::SccAdaptor compute_گراف_scc(گراف_4);
    منطقي scc = خطأ;
    std::وکتور<صحيح> perm;
    TopoSortedSccs toposort(compute_گراف_scc, [&](صحيح *begin, صحيح *انتها) {
        perm.درج(perm.انتها(), begin, انتها);
        إذا (انتها > begin + 1)
        {
            log_warning("Combinational loop:\n");
            لكل (صحيح *i = begin; i != انتها; ++i) {
				گره_2 گره(گراف_4[*i], design);
                لگاریتم("- %s = %s\n", گره.name().unescape(), گره.مقصد_string());
			}
            لگاریتم("\n");
            scc = درست;
        }
    });
	لكل(ثابت خودکار &[name, state]: حالتها_2)
		إذا(state.has_بعدی_مقدار())
			toposort.process(state.بعدی_مقدار().id());
	لكل(ثابت خودکار &[name, output]: _outputs)
		إذا(output.has_مقدار())
			toposort.process(output.مقدار().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    گراف_4.جایگشت(perm);
    إذا(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ساكن IdString ادغام_name(IdString a, IdString b) {
	إذا(!a.is_عمومی() && b.is_عمومی())
		أرجع b;
	وإلا
		أرجع a;
}

فراغ IR::forward_buf() {
    std::وکتور<صحيح> perm, alias;
    perm.clear();

    لكل (صحيح i = 0; i < گراف_4.اندازه(); ++i)
    {
        خودکار گره = گراف_4[i];
        إذا (گره.function().fn() == Fn::buf && گره.arg(0).شاخص() < i)
        {
            صحيح هدف_شاخص = alias[گره.arg(0).شاخص()];
            خودکار هدف_گره = گراف_4[perm[هدف_شاخص]];
			إذا(گره.has_sparse_attr()) {
				إذا(هدف_گره.has_sparse_attr()) {
					IdString id = ادغام_name(گره.sparse_attr(), هدف_گره.sparse_attr());
					هدف_گره.sparse_attr() = id;
				} وإلا {
					IdString id = گره.sparse_attr();
					هدف_گره.sparse_attr() = id;
				}
			}
            alias.افزودن_پشت(هدف_شاخص);
        }
        وإلا
        {
            alias.افزودن_پشت(Get_اندازه(perm));
            perm.افزودن_پشت(i);
        }
    }
    گراف_4.جایگشت(perm, alias);
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

فراغ Writer::print_impl(ثابت حرف *fmt, وکتور<std::function<فراغ()>> &fns)
{
	اندازه_t بعدی_شاخص = 0;
	لكل(ثابت حرف *p = fmt; *p != 0; p++)
		بدل(*p) {
		حالة '{':
			إذا(*++p == '{') {
				*os << '{';
			} وإلا {
				حرف *pe;
				اندازه_t شاخص = strtoul(p, &pe, 10);
				إذا(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				إذا(p == pe)
					شاخص = بعدی_شاخص;
				وإلا
					p = pe;
				إذا(شاخص >= fns.اندازه())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", شاخص, fns.اندازه(), quote_fmt(fmt));
				fns[شاخص]();
				بعدی_شاخص = شاخص + 1;
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
YOSYS_NAMESPACE_انتها
