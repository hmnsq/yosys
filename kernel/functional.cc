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
	حالة Fn::ټوټه: أرجع "slice";
	حالة Fn::zero_extend: أرجع "zero_extend";
	حالة Fn::sign_extend: أرجع "sign_extend";
	حالة Fn::concat: أرجع "concat";
	حالة Fn::جمع_کړه: أرجع "add";
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
	حالة Fn::logical_ښويدنه_کیڼ: أرجع "logical_shift_left";
	حالة Fn::logical_ښويدنه_ښی: أرجع "logical_shift_right";
	حالة Fn::arithmetic_ښويدنه_ښی: أرجع "arithmetic_shift_right";
	حالة Fn::mux: أرجع "mux";
	حالة Fn::constant: أرجع "constant";
	حالة Fn::ننوت: أرجع "input";
	حالة Fn::state: أرجع "state";
	حالة Fn::حافظه_read: أرجع "memory_read";
	حالة Fn::حافظه_write: أرجع "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (صحيح)fn);
}

ویکټور<IRInput ثابت*> IR::inputs(IdString ډول) ثابت {
	ویکټور<IRInput ثابت*> ret;
	لكل (ثابت خپلکاری &[name, ننوت] : _inputs)
		إذا(ننوت.ډول == ډول)
			ret.ټېل_شا(&ننوت);
	أرجع ret;
}

ویکټور<IROutput ثابت*> IR::outputs(IdString ډول) ثابت {
	ویکټور<IROutput ثابت*> ret;
	لكل (ثابت خپلکاری &[name, output] : _outputs)
		إذا(output.ډول == ډول)
			ret.ټېل_شا(&output);
	أرجع ret;
}

ویکټور<IRState ثابت*> IR::حالتونه(IdString ډول) ثابت {
	ویکټور<IRState ثابت*> ret;
	لكل (ثابت خپلکاری &[name, state] : حالتونه_2)
		إذا(state.ډول == ډول)
			ret.ټېل_شا(&state);
	أرجع ret;
}

ویکټور<IRInput ثابت*> IR::all_inputs() ثابت {
	ویکټور<IRInput ثابت*> ret;
	لكل (ثابت خپلکاری &[name, ننوت] : _inputs)
		ret.ټېل_شا(&ننوت);
	أرجع ret;
}

ویکټور<IROutput ثابت*> IR::all_outputs() ثابت {
	ویکټور<IROutput ثابت*> ret;
	لكل (ثابت خپلکاری &[name, output] : _outputs)
		ret.ټېل_شا(&output);
	أرجع ret;
}

ویکټور<IRState ثابت*> IR::all_حالتونه() ثابت {
	ویکټور<IRState ثابت*> ret;
	لكل (ثابت خپلکاری &[name, state] : حالتونه_2)
		ret.ټېل_شا(&state);
	أرجع ret;
}

بنية PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(غوټه_2)> np;
	PrintVisitor(std::function<std::string(غوټه_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string ټوټه(غوټه_2, غوټه_2 a, صحيح دننه_واټن, صحيح بهر_پلنوالی) override { أرجع "slice(" + np(a) + ", " + std::منزل_string(دننه_واټن) + ", " + std::منزل_string(بهر_پلنوالی) + ")"; }
	std::string zero_extend(غوټه_2, غوټه_2 a, صحيح بهر_پلنوالی) override { أرجع "zero_extend(" + np(a) + ", " + std::منزل_string(بهر_پلنوالی) + ")"; }
	std::string sign_extend(غوټه_2, غوټه_2 a, صحيح بهر_پلنوالی) override { أرجع "sign_extend(" + np(a) + ", " + std::منزل_string(بهر_پلنوالی) + ")"; }
	std::string constant(غوټه_2, RTLIL::Const ثابت& ارزښت) override { أرجع "constant(" + ارزښت.as_string() + ")"; }
	std::string ننوت(غوټه_2 self, IdString name, IdString ډول) override { أرجع "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(ډول) + ")"; }
	std::string state(غوټه_2 self, IdString name, IdString ډول) override { أرجع "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(ډول) + ")"; }
	std::string default_handler(غوټه_2 self) override {
		std::string ret = fn_منزل_string(self.fn());
		ret += "(";
		لكل(اندازه_t i = 0; i < self.arg_شمېر(); i++) {
			إذا(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		أرجع ret;
	}
};

std::string غوټه_2::منزل_string()
{
	أرجع منزل_string([](غوټه_2 n) { أرجع n.name().unescape(); });
}

std::string غوټه_2::منزل_string(std::function<std::string(غوټه_2)> np)
{
	أرجع لیدنه(PrintVisitor(np));
}

صنف CellSimplifier {
	Factory &factory;
	غوټه_2 sign(غوټه_2 a) {
		أرجع factory.ټوټه(a, a.پلنوالی() - 1, 1);
	}
	غوټه_2 neg_if(غوټه_2 a, غوټه_2 s) {
		أرجع factory.mux(a, factory.unary_minus(a), s);
	}
	غوټه_2 مطلق_قیمت(غوټه_2 a) {
		أرجع neg_if(a, sign(a));
	}
	غوټه_2 handle_ښويدنه(غوټه_2 a, غوټه_2 b, منطقي is_ښی, منطقي is_signed) {
		// to prevent new_width == 0, we handle this case separately
		إذا(a.پلنوالی() == 1) {
			إذا(!is_signed)
				أرجع factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			وإلا
				أرجع a;
		}
		صحيح new_پلنوالی = ceil_log2(a.پلنوالی());
		غوټه_2 b_truncated = factory.extend(b, new_پلنوالی, خطأ);
		غوټه_2 y =
			!is_ښی ? factory.logical_ښويدنه_کیڼ(a, b_truncated) :
			!is_signed ? factory.logical_ښويدنه_ښی(a, b_truncated) :
			factory.arithmetic_ښويدنه_ښی(a, b_truncated);
		إذا(b.پلنوالی() <= new_پلنوالی)
			أرجع y;
		غوټه_2 ډکېدنه = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.پلنوالی(), b.پلنوالی())));
		غوټه_2 y_if_ډکېدنه = is_signed ? factory.extend(sign(a), a.پلنوالی(), ریښتیا) : factory.constant(RTLIL::Const(State::S0, a.پلنوالی()));
		أرجع factory.mux(y, y_if_ډکېدنه, ډکېدنه);
	}
عام:
	غوټه_2 logical_ښويدنه_کیڼ(غوټه_2 a, غوټه_2 b) { أرجع handle_ښويدنه(a, b, خطأ, خطأ); }
	غوټه_2 logical_ښويدنه_ښی(غوټه_2 a, غوټه_2 b) { أرجع handle_ښويدنه(a, b, ریښتیا, خطأ); }
	غوټه_2 arithmetic_ښويدنه_ښی(غوټه_2 a, غوټه_2 b) { أرجع handle_ښويدنه(a, b, ریښتیا, ریښتیا); }
	غوټه_2 bitwise_mux(غوټه_2 a, غوټه_2 b, غوټه_2 s) {
		غوټه_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		غوټه_2 bb = factory.bitwise_and(b, s);
		أرجع factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
خاص:
	غوټه_2 handle_pow(غوټه_2 a0, غوټه_2 b, صحيح y_پلنوالی, منطقي is_signed) {
		غوټه_2 a = factory.extend(a0, y_پلنوالی, is_signed);
		غوټه_2 r = factory.constant(Const(1, y_پلنوالی));
		لكل(صحيح i = 0; i < b.پلنوالی(); i++) {
			غوټه_2 b_بټ = factory.ټوټه(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_بټ);
			a = factory.mul(a, a);
		}
		إذا (is_signed) {
			غوټه_2 a_ge_1 = factory.unsigned_greater_than(مطلق_قیمت(a0), factory.constant(Const(1, a0.پلنوالی())));
			غوټه_2 zero_پایله = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_پلنوالی)), zero_پایله);
		}
		أرجع r;
	}
	غوټه_2 handle_bmux(غوټه_2 a, غوټه_2 s, صحيح a_دننه_واټن, صحيح پلنوالی, صحيح sn) {
		إذا(sn < 1)
			أرجع factory.ټوټه(a, a_دننه_واټن, پلنوالی);
		وإلا {
			غوټه_2 y0 = handle_bmux(a, s, a_دننه_واټن, پلنوالی, sn - 1);
			غوټه_2 y1 = handle_bmux(a, s, a_دننه_واټن + (پلنوالی << (sn - 1)), پلنوالی, sn - 1);
			أرجع factory.mux(y0, y1, factory.ټوټه(s, sn - 1, 1));
		}
	}
	غوټه_2 handle_pmux(غوټه_2 a, غوټه_2 b, غوټه_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.پلنوالی() == a.پلنوالی() * s.پلنوالی());
		غوټه_2 y = a;
		لكل(صحيح i = 0; i < s.پلنوالی(); i++)
			y = factory.mux(y, factory.ټوټه(b, a.پلنوالی() * i, a.پلنوالی()), factory.ټوټه(s, i, 1));
		أرجع y;
	}
	dict<IdString, غوټه_2> handle_fa(غوټه_2 a, غوټه_2 b, غوټه_2 c) {
		غوټه_2 t1 = factory.bitwise_xor(a, b);
		غوټه_2 t2 = factory.bitwise_and(a, b);
		غوټه_2 t3 = factory.bitwise_and(c, t1);
		غوټه_2 y = factory.bitwise_xor(c, t1);
		غوټه_2 x = factory.bitwise_or(t2, t3);
		أرجع {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, غوټه_2> handle_alu(غوټه_2 a_دننه, غوټه_2 b_دننه, صحيح y_پلنوالی, منطقي is_signed, غوټه_2 ci, غوټه_2 bi) {
		غوټه_2 a = factory.extend(a_دننه, y_پلنوالی, is_signed);
		غوټه_2 b_uninverted = factory.extend(b_دننه, y_پلنوالی, is_signed);
		غوټه_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		غوټه_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		غوټه_2 a_extra = factory.extend(a, y_پلنوالی + 1, خطأ);
		غوټه_2 b_extra = factory.extend(b, y_پلنوالی + 1, خطأ);
		غوټه_2 y_extra = factory.جمع_کړه(factory.جمع_کړه(a_extra, b_extra), factory.extend(ci, a.پلنوالی() + 1, خطأ));
		غوټه_2 y = factory.ټوټه(y_extra, 0, y_پلنوالی);
		غوټه_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		غوټه_2 co = factory.ټوټه(carries, 1, y_پلنوالی);
		أرجع {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	غوټه_2 handle_lcu(غوټه_2 p, غوټه_2 g, غوټه_2 ci) {
		أرجع handle_alu(g, factory.bitwise_or(p, g), g.پلنوالی(), خطأ, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
عام:
	std::variant<dict<IdString, غوټه_2>, غوټه_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, غوټه_2> inputs)
	{
		صحيح a_پلنوالی = parameters.at(ID(A_پلنوالی), Const(-1)).as_int();
		صحيح b_پلنوالی = parameters.at(ID(B_پلنوالی), Const(-1)).as_int();
		صحيح y_پلنوالی = parameters.at(ID(Y_پلنوالی), Const(-1)).as_int();
		منطقي a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		منطقي b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		إذا(cellType.دننه(ID($جمع_کړه), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			منطقي is_signed = a_signed && b_signed;
			غوټه_2 a = factory.extend(inputs.at(ID(A)), y_پلنوالی, is_signed);
			غوټه_2 b = factory.extend(inputs.at(ID(B)), y_پلنوالی, is_signed);
			إذا(cellType == ID($جمع_کړه))
				أرجع factory.جمع_کړه(a, b);
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
		}وإلا إذا(cellType.دننه(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			منطقي is_signed = a_signed && b_signed;
			صحيح پلنوالی = max(a_پلنوالی, b_پلنوالی);
			غوټه_2 a = factory.extend(inputs.at(ID(A)), پلنوالی, is_signed);
			غوټه_2 b = factory.extend(inputs.at(ID(B)), پلنوالی, is_signed);
			إذا(cellType.دننه(ID($eq), ID($eqx)))
				أرجع factory.extend(factory.equal(a, b), y_پلنوالی, خطأ);
			وإلا إذا(cellType.دننه(ID($ne), ID($nex)))
				أرجع factory.extend(factory.not_equal(a, b), y_پلنوالی, خطأ);
			وإلا إذا(cellType == ID($lt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_پلنوالی, خطأ);
			وإلا إذا(cellType == ID($le))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_پلنوالی, خطأ);
			وإلا إذا(cellType == ID($gt))
				أرجع factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_پلنوالی, خطأ);
			وإلا إذا(cellType == ID($ge))
				أرجع factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_پلنوالی, خطأ);
			وإلا
				log_abort();
		}وإلا إذا(cellType.دننه(ID($logic_or), ID($logic_and))){
			غوټه_2 a = factory.reduce_or(inputs.at(ID(A)));
			غوټه_2 b = factory.reduce_or(inputs.at(ID(B)));
			غوټه_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			أرجع factory.extend(y, y_پلنوالی, خطأ);
		}وإلا إذا(cellType == ID($not)){
			غوټه_2 a = factory.extend(inputs.at(ID(A)), y_پلنوالی, a_signed);
			أرجع factory.bitwise_not(a);
		}وإلا إذا(cellType == ID($pos)){
			أرجع factory.extend(inputs.at(ID(A)), y_پلنوالی, a_signed);
		}وإلا إذا(cellType == ID($barrier)){
			أرجع inputs.at(ID(A));
		}وإلا إذا(cellType == ID($neg)){
			غوټه_2 a = factory.extend(inputs.at(ID(A)), y_پلنوالی, a_signed);
			أرجع factory.unary_minus(a);
		}وإلا إذا(cellType == ID($logic_not)){
			غوټه_2 a = factory.reduce_or(inputs.at(ID(A)));
			غوټه_2 y = factory.bitwise_not(a);
			أرجع factory.extend(y, y_پلنوالی, خطأ);
		}وإلا إذا(cellType.دننه(ID($reduce_or), ID($reduce_bool))){
			غوټه_2 a = factory.reduce_or(inputs.at(ID(A)));
			أرجع factory.extend(a, y_پلنوالی, خطأ);
		}وإلا إذا(cellType == ID($reduce_and)){
			غوټه_2 a = factory.reduce_and(inputs.at(ID(A)));
			أرجع factory.extend(a, y_پلنوالی, خطأ);
		}وإلا إذا(cellType.دننه(ID($reduce_xor), ID($reduce_xnor))){
			غوټه_2 a = factory.reduce_xor(inputs.at(ID(A)));
			غوټه_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			أرجع factory.extend(y, y_پلنوالی, خطأ);
		}وإلا إذا(cellType == ID($shl) || cellType == ID($sshl)){
			غوټه_2 a = factory.extend(inputs.at(ID(A)), y_پلنوالی, a_signed);
			غوټه_2 b = inputs.at(ID(B));
			أرجع logical_ښويدنه_کیڼ(a, b);
		}وإلا إذا(cellType == ID($shr) || cellType == ID($sshr)){
			صحيح پلنوالی = max(a_پلنوالی, y_پلنوالی);
			غوټه_2 a = factory.extend(inputs.at(ID(A)), پلنوالی, a_signed);
			غوټه_2 b = inputs.at(ID(B));
			غوټه_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_ښويدنه_ښی(a, b) :
				logical_ښويدنه_ښی(a, b);
			أرجع factory.extend(y, y_پلنوالی, a_signed);
		}وإلا إذا(cellType == ID($shiftx) || cellType == ID($ښويدنه)){
			صحيح پلنوالی = max(a_پلنوالی, y_پلنوالی);
			غوټه_2 a = factory.extend(inputs.at(ID(A)), پلنوالی, cellType == ID($ښويدنه) && a_signed);
			غوټه_2 b = inputs.at(ID(B));
			غوټه_2 shr = logical_ښويدنه_ښی(a, b);
			إذا(b_signed) {
				غوټه_2 shl = logical_ښويدنه_کیڼ(a, factory.unary_minus(b));
				غوټه_2 y = factory.mux(shr, shl, sign(b));
				أرجع factory.extend(y, y_پلنوالی, خطأ);
			} وإلا {
				أرجع factory.extend(shr, y_پلنوالی, خطأ);
			}
		}وإلا إذا(cellType == ID($mux)){
			أرجع factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($pmux)){
			أرجع handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}وإلا إذا(cellType == ID($concat)){
			غوټه_2 a = inputs.at(ID(A));
			غوټه_2 b = inputs.at(ID(B));
			أرجع factory.concat(a, b);
		}وإلا إذا(cellType == ID($ټوټه)){
			صحيح دننه_واټن = parameters.at(ID(دننه_واټن_2)).as_int();
			غوټه_2 a = inputs.at(ID(A));
			أرجع factory.ټوټه(a, دننه_واټن, y_پلنوالی);
		}وإلا إذا(cellType.دننه(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			صحيح پلنوالی = max(a_پلنوالی, b_پلنوالی);
			منطقي is_signed = a_signed && b_signed;
			غوټه_2 a = factory.extend(inputs.at(ID(A)), پلنوالی, is_signed);
			غوټه_2 b = factory.extend(inputs.at(ID(B)), پلنوالی, is_signed);
			إذا(is_signed) {
				إذا(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					غوټه_2 abs_y = factory.unsigned_div(مطلق_قیمت(a), مطلق_قیمت(b));
					غوټه_2 بهر_sign = factory.not_equal(sign(a), sign(b));
					أرجع neg_if(factory.extend(abs_y, y_پلنوالی, خطأ), بهر_sign);
				} وإلا إذا(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					غوټه_2 abs_y = factory.unsigned_mod(مطلق_قیمت(a), مطلق_قیمت(b));
					أرجع neg_if(factory.extend(abs_y, y_پلنوالی, خطأ), sign(a));
				} وإلا إذا(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					غوټه_2 b_sign = sign(b);
					غوټه_2 a1 = neg_if(a, b_sign);
					غوټه_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					غوټه_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					غوټه_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					غوټه_2 y1 = factory.unsigned_div(a2, b1);
					غوټه_2 y2 = factory.extend(y1, y_پلنوالی, خطأ);
					أرجع factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} وإلا إذا(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					غوټه_2 abs_b = مطلق_قیمت(b);
					غوټه_2 abs_y = factory.unsigned_mod(مطلق_قیمت(a), abs_b);
					غوټه_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					غوټه_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					غوټه_2 y = neg_if(y_flipped, sign(b));
					أرجع factory.extend(y, y_پلنوالی, ریښتیا);
				} وإلا
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} وإلا {
				إذا(cellType.دننه(ID($mod), ID($modfloor)))
					أرجع factory.extend(factory.unsigned_mod(a, b), y_پلنوالی, خطأ);
				وإلا
					أرجع factory.extend(factory.unsigned_div(a, b), y_پلنوالی, خطأ);
			}
		} وإلا إذا(cellType == ID($توان)) {
			أرجع handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_پلنوالی, a_signed && b_signed);
		} وإلا إذا (cellType == ID($lut)) {
			صحيح پلنوالی = parameters.at(ID(پلنوالی_2)).as_int();
			Const lut_جدول = parameters.at(ID(LUT));
			lut_جدول.extu(1 << پلنوالی);
			أرجع handle_bmux(factory.constant(lut_جدول), inputs.at(ID(A)), 0, 1, پلنوالی);
		} وإلا إذا (cellType == ID($bwmux)) {
			غوټه_2 a = inputs.at(ID(A));
			غوټه_2 b = inputs.at(ID(B));
			غوټه_2 s = inputs.at(ID(S));
			أرجع factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} وإلا إذا (cellType == ID($bweqx)) {
			غوټه_2 a = inputs.at(ID(A));
			غوټه_2 b = inputs.at(ID(B));
			أرجع factory.bitwise_not(factory.bitwise_xor(a, b));
		} وإلا إذا(cellType == ID($bmux)) {
			صحيح پلنوالی = parameters.at(ID(پلنوالی_2)).as_int();
			صحيح s_پلنوالی = parameters.at(ID(S_پلنوالی)).as_int();
			أرجع handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, پلنوالی, s_پلنوالی);
		} وإلا إذا(cellType == ID($demux)) {
			صحيح پلنوالی = parameters.at(ID(پلنوالی_2)).as_int();
			صحيح s_پلنوالی = parameters.at(ID(S_پلنوالی)).as_int();
			صحيح y_پلنوالی = پلنوالی << s_پلنوالی;
			صحيح b_پلنوالی = ceil_log2(y_پلنوالی);
			غوټه_2 a = factory.extend(inputs.at(ID(A)), y_پلنوالی, خطأ);
			غوټه_2 s = factory.extend(inputs.at(ID(S)), b_پلنوالی, خطأ);
			غوټه_2 b = factory.mul(s, factory.constant(Const(پلنوالی, b_پلنوالی)));
			أرجع factory.logical_ښويدنه_کیڼ(a, b);
		} وإلا إذا(cellType == ID($fa)) {
			أرجع handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} وإلا إذا(cellType == ID($lcu)) {
			أرجع handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} وإلا إذا(cellType == ID($alu)) {
			أرجع handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_پلنوالی, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} وإلا إذا(cellType.دننه(ID($أكد), ID($assume), ID($ژوندی), ID($fair), ID($cover))) {
			غوټه_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			خپلکاری &output = factory.جمع_کړه_output(cellName, cellType, ترتیبول_2(1));
			output.سېټ_ارزښت(a);
			أرجع {};
		} وإلا إذا(cellType.دننه(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			صحيح پلنوالی = parameters.at(ID(پلنوالی_2)).as_int();
			خپلکاری &ننوت = factory.جمع_کړه_ننوت(cellName, cellType, ترتیبول_2(پلنوالی));
			أرجع factory.ارزښت(ننوت);
		} وإلا إذا(cellType == ID($initstate)) {
			إذا(factory.ir().has_state(ID($initstate), ID($state)))
				أرجع factory.ارزښت(factory.ir().state(ID($initstate)));
			وإلا {
				خپلکاری &state = factory.جمع_کړه_state(ID($initstate), ID($state), ترتیبول_2(1));
				state.سېټ_initial_ارزښت(RTLIL::Const(State::S1, 1));
				state.سېټ_بل_ارزښت(factory.constant(RTLIL::Const(State::S0, 1)));
				أرجع factory.ارزښت(state);
			}
		} وإلا إذا(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} وإلا {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

صنف FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> کتار;
	dict<DriveSpec, غوټه_2> ګراف_nodes;
	dict<std::pair<Cell *, IdString>, غوټه_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	ویکټور<Mem> memories_ویکټور;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	غوټه_2 کتار_ته_ورزیاتول(DriveSpec ثابت &spec)
	{
		خپلکاری it = ګراف_nodes.پیدا_کړه(spec);
		إذا(it == ګراف_nodes.پای_ټکی()){
			خپلکاری غوټه = factory.create_pending(spec.اندازه());
			ګراف_nodes.ورننباسه({spec, غوټه});
			کتار.emplace_شا(spec);
			أرجع غوټه;
		}وإلا
			أرجع it->دوهم;
	}
	غوټه_2 کتار_ته_ورزیاتول_cell(Cell *cell, IdString پورټ_name)
	{
		خپلکاری it = cell_outputs.پیدا_کړه({cell, پورټ_name});
		إذا(it == cell_outputs.پای_ټکی()) {
			کتار.emplace_شا(cell);
			std::optional<غوټه_2> rv;
			لكل(خپلکاری ثابت &[name, sigspec] : cell->connections())
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					خپلکاری غوټه = factory.create_pending(sigspec.اندازه());
					factory.suggest_name(غوټه, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, غوټه);
					إذا(name == پورټ_name)
						rv = غوټه;
				}
			أرجع *rv;
		} وإلا
			أرجع it->دوهم;
	}
عام:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.جمع_کړه(module);
		لكل (خپلکاری cell : module->cells()) {
			إذا (cell->type.دننه(ID($أكد), ID($assume), ID($ژوندی), ID($fair), ID($cover), ID($check)))
				کتار.emplace_شا(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		لكل (خپلکاری riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			خپلکاری *wire = module->wire(*riter);
			إذا (wire && wire->پورټ_ننوت) {
				factory.جمع_کړه_ننوت(wire->name, ID($ننوت), ترتیبول_2(wire->پلنوالی));
			}
			إذا (wire && wire->پورټ_output) {
				خپلکاری &output = factory.جمع_کړه_output(wire->name, ID($output), ترتیبول_2(wire->پلنوالی));
				output.سېټ_ارزښت(کتار_ته_ورزیاتول(DriveChunk(DriveChunkWire(wire, 0, wire->پلنوالی))));
			}
		}
		memories_ویکټور = Mem::get_all_memories(module);
		لكل (خپلکاری &mem : memories_ویکټور) {
			إذا (mem.cell != تش_اشاره)
				memories[mem.cell] = &mem;
		}
	}
خاص:
	غوټه_2 concatenate_read_results(Mem *mem, ویکټور<غوټه_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ثابت SigSpec &rd_معلومات = mem->cell->connections().at(ID(RD_معلومات));
		صحيح current = 0;
		لكل(اندازه_t i = 0; i < mem->rd_ports.اندازه(); i++) {
			صحيح پلنوالی = mem->پلنوالی << mem->rd_ports[i].wide_log2;
			log_assert (results[i].پلنوالی() == پلنوالی);
			log_assert (mem->rd_ports[i].معلومات == rd_معلومات.ایستل(current, پلنوالی));
			current += پلنوالی;
		}
		log_assert (current == rd_معلومات.اندازه());
		log_assert (!results.تشه());
		غوټه_2 غوټه = results[0];
		لكل(اندازه_t i = 1; i < results.اندازه(); i++)
			غوټه = factory.concat(غوټه, results[i]);
		أرجع غوټه;
	}
	غوټه_2 handle_حافظه(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		ویکټور<غوټه_2> read_results;
		خپلکاری &state = factory.جمع_کړه_state(mem->cell->name, ID($state), ترتیبول_2(ceil_log2(mem->اندازه), mem->پلنوالی));
		state.سېټ_initial_ارزښت(MemContents(mem));
		غوټه_2 غوټه = factory.ارزښت(state);
		لكل (اندازه_t i = 0; i < mem->wr_ports.اندازه(); i++) {
			ثابت خپلکاری &wr = mem->wr_ports[i];
			إذا (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			غوټه_2 en = کتار_ته_ورزیاتول(driver_map(DriveSpec(wr.en)));
			غوټه_2 addr = کتار_ته_ورزیاتول(driver_map(DriveSpec(wr.addr)));
			غوټه_2 new_معلومات = کتار_ته_ورزیاتول(driver_map(DriveSpec(wr.معلومات)));
			غوټه_2 old_معلومات = factory.حافظه_read(غوټه, addr);
			غوټه_2 wr_معلومات = simplifier.bitwise_mux(old_معلومات, new_معلومات, en);
			غوټه = factory.حافظه_write(غوټه, addr, wr_معلومات);
		}
		إذا (mem->rd_ports.تشه())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		لكل (اندازه_t i = 0; i < mem->rd_ports.اندازه(); i++) {
			ثابت خپلکاری &rd = mem->rd_ports[i];
			إذا (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			غوټه_2 addr = کتار_ته_ورزیاتول(driver_map(DriveSpec(rd.addr)));
			read_results.ټېل_شا(factory.حافظه_read(غوټه, addr));
		}
		state.سېټ_بل_ارزښت(غوټه);
		أرجع concatenate_read_results(mem, read_results);
	}
	فراغ process_cell(Cell *cell)
	{
		إذا (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, تش_اشاره);
			إذا (mem == تش_اشاره) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			غوټه_2 غوټه = handle_حافظه(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_معلومات)}), غوټه);
		} وإلا إذا (cell->is_builtin_ff()) {
			Ff_معلومات ff(&ff_initvals, cell);
			إذا (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			خپلکاری &state = factory.جمع_کړه_state(ff.name, ID($state), ترتیبول_2(ff.پلنوالی));
			غوټه_2 q_ارزښت = factory.ارزښت(state);
			factory.suggest_name(q_ارزښت, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_ارزښت);
			state.سېټ_بل_ارزښت(کتار_ته_ورزیاتول(ff.sig_d));
			state.سېټ_initial_ارزښت(ff.val_init);
		} وإلا {
			dict<IdString, غوټه_2> connections;
			IdString output_name; // for the single output case
			صحيح n_outputs = 0;
			لكل(خپلکاری ثابت &[name, sigspec] : cell->connections()) {
				إذا(driver_map.celltypes.cell_ننوت(cell->type, name) && sigspec.اندازه() > 0)
					connections.ورننباسه({ name, کتار_ته_ورزیاتول(Drive_Chunk_پورټ(cell, {name, sigspec})) });
				إذا(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, غوټه_2>, غوټه_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			إذا(خپلکاری *nodep = std::get_if<غوټه_2>(&outputs); nodep != تش_اشاره) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} وإلا {
				لكل(خپلکاری [name, غوټه] : std::get<dict<IdString, غوټه_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), غوټه);
			}
		}
	}
	فراغ undriven(ثابت std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	فراغ check_undriven(DriveSpec ثابت& spec, std::string ثابت& name) {
		لكل(خپلکاری ثابت &chunk : spec.chunks())
			إذا(chunk.is_none())
				undriven(name);
	}
عام:
	فراغ process_کتار()
	{
		لكل (; !کتار.تشه(); کتار.راوباسه_front()) {
			إذا(خپلکاری p = std::get_if<Cell *>(&کتار.front()); p != تش_اشاره) {
				process_cell(*p);
				استمر;
			}

			DriveSpec spec = std::get<DriveSpec>(کتار.front());
			غوټه_2 pending = ګراف_nodes.at(spec);

			إذا (spec.chunks().اندازه() > 1) {
				خپلکاری chunks = spec.chunks();
				غوټه_2 غوټه = کتار_ته_ورزیاتول(chunks[0]);
				لكل(اندازه_t i = 1; i < chunks.اندازه(); i++)
					غوټه = factory.concat(غوټه, کتار_ته_ورزیاتول(chunks[i]));
				factory.update_pending(pending, غوټه);
			} وإلا إذا (spec.chunks().اندازه() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				إذا (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					إذا (wire_chunk.is_ټولیز()) {
						إذا (wire_chunk.wire->پورټ_ننوت) {
							غوټه_2 غوټه = factory.ارزښت(factory.ir().ننوت(wire_chunk.wire->name));
							factory.suggest_name(غوټه, wire_chunk.wire->name);
							factory.update_pending(pending, غوټه);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							غوټه_2 غوټه = کتار_ته_ورزیاتول(driver);
							factory.suggest_name(غوټه, wire_chunk.wire->name);
							factory.update_pending(pending, غوټه);
						}
					} وإلا {
						DriveChunkWire ټولیز_wire(wire_chunk.wire, 0, wire_chunk.wire->پلنوالی);
						غوټه_2 غوټه = factory.ټوټه(کتار_ته_ورزیاتول(ټولیز_wire), wire_chunk.دننه_واټن, wire_chunk.پلنوالی);
						factory.update_pending(pending, غوټه);
					}
				} وإلا إذا (chunk.is_پورټ()) {
					Drive_Chunk_پورټ پورټ_chunk = chunk.پورټ();
					إذا (پورټ_chunk.is_ټولیز()) {
						إذا (driver_map.celltypes.cell_output(پورټ_chunk.cell->type, پورټ_chunk.پورټ)) {
							غوټه_2 غوټه = کتار_ته_ورزیاتول_cell(پورټ_chunk.cell, پورټ_chunk.پورټ);
							factory.update_pending(pending, غوټه);
						} وإلا {
							DriveSpec driver = driver_map(DriveSpec(پورټ_chunk));
							check_undriven(driver, پورټ_chunk.cell->name.unescape() + " port " + PooledName(پورټ_chunk.cell->module, پورټ_chunk.پورټ).unescape());
							factory.update_pending(pending, کتار_ته_ورزیاتول(driver));
						}
					} وإلا {
						Drive_Chunk_پورټ ټولیز_پورټ(پورټ_chunk.cell, پورټ_chunk.پورټ, 0, Get_اندازه(پورټ_chunk.cell->connections().at(پورټ_chunk.پورټ)));
						غوټه_2 غوټه = factory.ټوټه(کتار_ته_ورزیاتول(ټولیز_پورټ), پورټ_chunk.دننه_واټن, پورټ_chunk.پلنوالی);
						factory.update_pending(pending, غوټه);
					}
				} وإلا إذا (chunk.is_constant()) {
					غوټه_2 غوټه = factory.constant(chunk.constant());
					factory.suggest_name(غوټه, "$const" + std::منزل_string(chunk.اندازه()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, غوټه);
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
    خپلکاری factory = ir.factory();
    FunctionalIRConstruction ctor(module, factory);
    ctor.process_کتار();
    ir.topological_ترتیبول();
    ir.forward_buf();
    أرجع ir;
}

فراغ IR::topological_ترتیبول() {
    ګراف::SccAdaptor compute_ګراف_scc(ګراف_4);
    منطقي scc = خطأ;
    std::ویکټور<صحيح> perm;
    TopoSortedSccs toposort(compute_ګراف_scc, [&](صحيح *begin, صحيح *پای_ټکی) {
        perm.ورننباسه(perm.پای_ټکی(), begin, پای_ټکی);
        إذا (پای_ټکی > begin + 1)
        {
            log_warning("Combinational loop:\n");
            لكل (صحيح *i = begin; i != پای_ټکی; ++i) {
				غوټه_2 غوټه(ګراف_4[*i], design);
                لوګاریتم("- %s = %s\n", غوټه.name().unescape(), غوټه.منزل_string());
			}
            لوګاریتم("\n");
            scc = ریښتیا;
        }
    });
	لكل(ثابت خپلکاری &[name, state]: حالتونه_2)
		إذا(state.has_بل_ارزښت())
			toposort.process(state.بل_ارزښت().id());
	لكل(ثابت خپلکاری &[name, output]: _outputs)
		إذا(output.has_ارزښت())
			toposort.process(output.ارزښت().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    ګراف_4.ترتیب_بدلول(perm);
    إذا(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ساكن IdString یوځای_name(IdString a, IdString b) {
	إذا(!a.is_عامه() && b.is_عامه())
		أرجع b;
	وإلا
		أرجع a;
}

فراغ IR::forward_buf() {
    std::ویکټور<صحيح> perm, alias;
    perm.clear();

    لكل (صحيح i = 0; i < ګراف_4.اندازه(); ++i)
    {
        خپلکاری غوټه = ګراف_4[i];
        إذا (غوټه.function().fn() == Fn::buf && غوټه.arg(0).شاخص() < i)
        {
            صحيح موخه_شاخص = alias[غوټه.arg(0).شاخص()];
            خپلکاری موخه_غوټه = ګراف_4[perm[موخه_شاخص]];
			إذا(غوټه.has_sparse_attr()) {
				إذا(موخه_غوټه.has_sparse_attr()) {
					IdString id = یوځای_name(غوټه.sparse_attr(), موخه_غوټه.sparse_attr());
					موخه_غوټه.sparse_attr() = id;
				} وإلا {
					IdString id = غوټه.sparse_attr();
					موخه_غوټه.sparse_attr() = id;
				}
			}
            alias.ټېل_شا(موخه_شاخص);
        }
        وإلا
        {
            alias.ټېل_شا(Get_اندازه(perm));
            perm.ټېل_شا(i);
        }
    }
    ګراف_4.ترتیب_بدلول(perm, alias);
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

فراغ Writer::print_impl(ثابت حرف *fmt, ویکټور<std::function<فراغ()>> &fns)
{
	اندازه_t بل_شاخص = 0;
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
					شاخص = بل_شاخص;
				وإلا
					p = pe;
				إذا(شاخص >= fns.اندازه())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", شاخص, fns.اندازه(), quote_fmt(fmt));
				fns[شاخص]();
				بل_شاخص = شاخص + 1;
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
YOSYS_NAMESPACE_پای_ټکی
