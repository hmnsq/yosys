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

#ଅନ୍ତର୍ଭାଵ "kernel/functional.h"
#ଅନ୍ତର୍ଭାଵ "kernel/topo_scc.h"
#ଅନ୍ତର୍ଭାଵ "ff.h"
#ଅନ୍ତର୍ଭାଵ "ffinit.h"
#ଅନ୍ତର୍ଭାଵ <deque>

YOSYS_NAMESPACE_BEGIN
ନାମାକାଶ Functional {

ଅଚଳ ଅକ୍ଷର *fn_to_string(Fn fn) {
	ବାଛ(fn) {
	ସ୍ଥିତି Fn::invalid: ଫେରନ୍ତୁ "invalid";
	ସ୍ଥିତି Fn::buf: ଫେରନ୍ତୁ "buf";
	ସ୍ଥିତି Fn::slice: ଫେରନ୍ତୁ "slice";
	ସ୍ଥିତି Fn::zero_extend: ଫେରନ୍ତୁ "zero_extend";
	ସ୍ଥିତି Fn::sign_extend: ଫେରନ୍ତୁ "sign_extend";
	ସ୍ଥିତି Fn::concat: ଫେରନ୍ତୁ "concat";
	ସ୍ଥିତି Fn::add: ଫେରନ୍ତୁ "add";
	ସ୍ଥିତି Fn::sub: ଫେରନ୍ତୁ "sub";
	ସ୍ଥିତି Fn::mul: ଫେରନ୍ତୁ "mul";
	ସ୍ଥିତି Fn::unsigned_div: ଫେରନ୍ତୁ "unsigned_div";
	ସ୍ଥିତି Fn::unsigned_mod: ଫେରନ୍ତୁ "unsigned_mod";
	ସ୍ଥିତି Fn::bitwise_and: ଫେରନ୍ତୁ "bitwise_and";
	ସ୍ଥିତି Fn::bitwise_or: ଫେରନ୍ତୁ "bitwise_or";
	ସ୍ଥିତି Fn::bitwise_xor: ଫେରନ୍ତୁ "bitwise_xor";
	ସ୍ଥିତି Fn::bitwise_not: ଫେରନ୍ତୁ "bitwise_not";
	ସ୍ଥିତି Fn::reduce_and: ଫେରନ୍ତୁ "reduce_and";
	ସ୍ଥିତି Fn::reduce_or: ଫେରନ୍ତୁ "reduce_or";
	ସ୍ଥିତି Fn::reduce_xor: ଫେରନ୍ତୁ "reduce_xor";
	ସ୍ଥିତି Fn::unary_minus: ଫେରନ୍ତୁ "unary_minus";
	ସ୍ଥିତି Fn::equal: ଫେରନ୍ତୁ "equal";
	ସ୍ଥିତି Fn::not_equal: ଫେରନ୍ତୁ "not_equal";
	ସ୍ଥିତି Fn::signed_greater_than: ଫେରନ୍ତୁ "signed_greater_than";
	ସ୍ଥିତି Fn::signed_greater_equal: ଫେରନ୍ତୁ "signed_greater_equal";
	ସ୍ଥିତି Fn::unsigned_greater_than: ଫେରନ୍ତୁ "unsigned_greater_than";
	ସ୍ଥିତି Fn::unsigned_greater_equal: ଫେରନ୍ତୁ "unsigned_greater_equal";
	ସ୍ଥିତି Fn::logical_shift_ବାମ: ଫେରନ୍ତୁ "logical_shift_left";
	ସ୍ଥିତି Fn::logical_shift_ଡାହାଣ: ଫେରନ୍ତୁ "logical_shift_right";
	ସ୍ଥିତି Fn::arithmetic_shift_ଡାହାଣ: ଫେରନ୍ତୁ "arithmetic_shift_right";
	ସ୍ଥିତି Fn::mux: ଫେରନ୍ତୁ "mux";
	ସ୍ଥିତି Fn::constant: ଫେରନ୍ତୁ "constant";
	ସ୍ଥିତି Fn::ନିବେଶ: ଫେରନ୍ତୁ "input";
	ସ୍ଥିତି Fn::state: ଫେରନ୍ତୁ "state";
	ସ୍ଥିତି Fn::memory_read: ଫେରନ୍ତୁ "memory_read";
	ସ୍ଥିତି Fn::memory_write: ଫେରନ୍ତୁ "memory_write";
	}
	log_error("fn_to_string: unknown Functional::Fn value %d\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fn);
}

vector<IRInput ଅଚଳ*> IR::inputs(IdString kind) ଅଚଳ {
	vector<IRInput ଅଚଳ*> ret;
	ପାଇଁ (ଅଚଳ auto &[name, ନିବେଶ] : _inputs)
		ଯଦି(ନିବେଶ.kind == kind)
			ret.ଠେଲ_back(&ନିବେଶ);
	ଫେରନ୍ତୁ ret;
}

vector<IROutput ଅଚଳ*> IR::outputs(IdString kind) ଅଚଳ {
	vector<IROutput ଅଚଳ*> ret;
	ପାଇଁ (ଅଚଳ auto &[name, output] : _outputs)
		ଯଦି(output.kind == kind)
			ret.ଠେଲ_back(&output);
	ଫେରନ୍ତୁ ret;
}

vector<IRState ଅଚଳ*> IR::states(IdString kind) ଅଚଳ {
	vector<IRState ଅଚଳ*> ret;
	ପାଇଁ (ଅଚଳ auto &[name, state] : _states)
		ଯଦି(state.kind == kind)
			ret.ଠେଲ_back(&state);
	ଫେରନ୍ତୁ ret;
}

vector<IRInput ଅଚଳ*> IR::all_inputs() ଅଚଳ {
	vector<IRInput ଅଚଳ*> ret;
	ପାଇଁ (ଅଚଳ auto &[name, ନିବେଶ] : _inputs)
		ret.ଠେଲ_back(&ନିବେଶ);
	ଫେରନ୍ତୁ ret;
}

vector<IROutput ଅଚଳ*> IR::all_outputs() ଅଚଳ {
	vector<IROutput ଅଚଳ*> ret;
	ପାଇଁ (ଅଚଳ auto &[name, output] : _outputs)
		ret.ଠେଲ_back(&output);
	ଫେରନ୍ତୁ ret;
}

vector<IRState ଅଚଳ*> IR::all_states() ଅଚଳ {
	vector<IRState ଅଚଳ*> ret;
	ପାଇଁ (ଅଚଳ auto &[name, state] : _states)
		ret.ଠେଲ_back(&state);
	ଫେରନ୍ତୁ ret;
}

ସଂରଚନା PrintVisitor : DefaultVisitor<std::string> {
	std::function<std::string(ଗଣ୍ଠି_2)> np;
	PrintVisitor(std::function<std::string(ଗଣ୍ଠି_2)> np) : np(np) { }
	// as a general rule the default handler is good enough iff the only arguments are of type Node
	std::string slice(ଗଣ୍ଠି_2, ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width) override { ଫେରନ୍ତୁ "slice(" + np(a) + ", " + std::to_string(offset) + ", " + std::to_string(out_width) + ")"; }
	std::string zero_extend(ଗଣ୍ଠି_2, ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width) override { ଫେରନ୍ତୁ "zero_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string sign_extend(ଗଣ୍ଠି_2, ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width) override { ଫେରନ୍ତୁ "sign_extend(" + np(a) + ", " + std::to_string(out_width) + ")"; }
	std::string constant(ଗଣ୍ଠି_2, RTLIL::Const ଅଚଳ& ମୂଲ୍ୟ) override { ଫେରନ୍ତୁ "constant(" + ମୂଲ୍ୟ.as_string() + ")"; }
	std::string ନିବେଶ(ଗଣ୍ଠି_2 self, IdString name, IdString kind) override { ଫେରନ୍ତୁ "input(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string state(ଗଣ୍ଠି_2 self, IdString name, IdString kind) override { ଫେରନ୍ତୁ "state(" + self.design->twines().str(name) + ", " + self.design->twines().str(kind) + ")"; }
	std::string default_handler(ଗଣ୍ଠି_2 self) override {
		std::string ret = fn_to_string(self.fn());
		ret += "(";
		ପାଇଁ(size_t i = 0; i < self.arg_ଗଣନା(); i++) {
			ଯଦି(i > 0) ret += ", ";
			ret += np(self.arg(i));
		}
		ret += ")";
		ଫେରନ୍ତୁ ret;
	}
};

std::string ଗଣ୍ଠି_2::to_string()
{
	ଫେରନ୍ତୁ to_string([](ଗଣ୍ଠି_2 n) { ଫେରନ୍ତୁ n.name().unescape(); });
}

std::string ଗଣ୍ଠି_2::to_string(std::function<std::string(ଗଣ୍ଠି_2)> np)
{
	ଫେରନ୍ତୁ visit(PrintVisitor(np));
}

ଶ୍ରେଣୀ CellSimplifier {
	Factory &factory;
	ଗଣ୍ଠି_2 sign(ଗଣ୍ଠି_2 a) {
		ଫେରନ୍ତୁ factory.slice(a, a.width() - 1, 1);
	}
	ଗଣ୍ଠି_2 neg_if(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 s) {
		ଫେରନ୍ତୁ factory.mux(a, factory.unary_minus(a), s);
	}
	ଗଣ୍ଠି_2 abs(ଗଣ୍ଠି_2 a) {
		ଫେରନ୍ତୁ neg_if(a, sign(a));
	}
	ଗଣ୍ଠି_2 handle_shift(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b, ତାର୍କିକ is_ଡାହାଣ, ତାର୍କିକ is_signed) {
		// to prevent new_width == 0, we handle this case separately
		ଯଦି(a.width() == 1) {
			ଯଦି(!is_signed)
				ଫେରନ୍ତୁ factory.bitwise_and(a, factory.bitwise_not(factory.reduce_or(b)));
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ a;
		}
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_width = ceil_log2(a.width());
		ଗଣ୍ଠି_2 b_truncated = factory.extend(b, new_width, ମିଥ୍ୟା);
		ଗଣ୍ଠି_2 y =
			!is_ଡାହାଣ ? factory.logical_shift_ବାମ(a, b_truncated) :
			!is_signed ? factory.logical_shift_ଡାହାଣ(a, b_truncated) :
			factory.arithmetic_shift_ଡାହାଣ(a, b_truncated);
		ଯଦି(b.width() <= new_width)
			ଫେରନ୍ତୁ y;
		ଗଣ୍ଠି_2 overflow = factory.unsigned_greater_equal(b, factory.constant(RTLIL::Const(a.width(), b.width())));
		ଗଣ୍ଠି_2 y_if_overflow = is_signed ? factory.extend(sign(a), a.width(), ସତ୍ୟ) : factory.constant(RTLIL::Const(State::S0, a.width()));
		ଫେରନ୍ତୁ factory.mux(y, y_if_overflow, overflow);
	}
ସର୍ବସାଧାରଣ:
	ଗଣ୍ଠି_2 logical_shift_ବାମ(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { ଫେରନ୍ତୁ handle_shift(a, b, ମିଥ୍ୟା, ମିଥ୍ୟା); }
	ଗଣ୍ଠି_2 logical_shift_ଡାହାଣ(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { ଫେରନ୍ତୁ handle_shift(a, b, ସତ୍ୟ, ମିଥ୍ୟା); }
	ଗଣ୍ଠି_2 arithmetic_shift_ଡାହାଣ(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { ଫେରନ୍ତୁ handle_shift(a, b, ସତ୍ୟ, ସତ୍ୟ); }
	ଗଣ୍ଠି_2 bitwise_mux(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b, ଗଣ୍ଠି_2 s) {
		ଗଣ୍ଠି_2 aa = factory.bitwise_and(a, factory.bitwise_not(s));
		ଗଣ୍ଠି_2 bb = factory.bitwise_and(b, s);
		ଫେରନ୍ତୁ factory.bitwise_or(aa, bb);
	}
	CellSimplifier(Factory &f) : factory(f) {}
ବ୍ୟକ୍ତିଗତ:
	ଗଣ୍ଠି_2 handle_pow(ଗଣ୍ଠି_2 a0, ଗଣ୍ଠି_2 b, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width, ତାର୍କିକ is_signed) {
		ଗଣ୍ଠି_2 a = factory.extend(a0, y_width, is_signed);
		ଗଣ୍ଠି_2 r = factory.constant(Const(1, y_width));
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < b.width(); i++) {
			ଗଣ୍ଠି_2 b_bit = factory.slice(b, i, 1);
			r = factory.mux(r, factory.mul(r, a), b_bit);
			a = factory.mul(a, a);
		}
		ଯଦି (is_signed) {
			ଗଣ୍ଠି_2 a_ge_1 = factory.unsigned_greater_than(abs(a0), factory.constant(Const(1, a0.width())));
			ଗଣ୍ଠି_2 zero_result = factory.bitwise_and(a_ge_1, sign(b));
			r = factory.mux(r, factory.constant(Const(0, y_width)), zero_result);
		}
		ଫେରନ୍ତୁ r;
	}
	ଗଣ୍ଠି_2 handle_bmux(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn) {
		ଯଦି(sn < 1)
			ଫେରନ୍ତୁ factory.slice(a, a_offset, width);
		ଅନ୍ୟଥା {
			ଗଣ୍ଠି_2 y0 = handle_bmux(a, s, a_offset, width, sn - 1);
			ଗଣ୍ଠି_2 y1 = handle_bmux(a, s, a_offset + (width << (sn - 1)), width, sn - 1);
			ଫେରନ୍ତୁ factory.mux(y0, y1, factory.slice(s, sn - 1, 1));
		}
	}
	ଗଣ୍ଠି_2 handle_pmux(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b, ଗଣ୍ଠି_2 s) {
		// TODO : what to do about multiple b bits set ?
		log_assert(b.width() == a.width() * s.width());
		ଗଣ୍ଠି_2 y = a;
		ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < s.width(); i++)
			y = factory.mux(y, factory.slice(b, a.width() * i, a.width()), factory.slice(s, i, 1));
		ଫେରନ୍ତୁ y;
	}
	dict<IdString, ଗଣ୍ଠି_2> handle_fa(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b, ଗଣ୍ଠି_2 c) {
		ଗଣ୍ଠି_2 t1 = factory.bitwise_xor(a, b);
		ଗଣ୍ଠି_2 t2 = factory.bitwise_and(a, b);
		ଗଣ୍ଠି_2 t3 = factory.bitwise_and(c, t1);
		ଗଣ୍ଠି_2 y = factory.bitwise_xor(c, t1);
		ଗଣ୍ଠି_2 x = factory.bitwise_or(t2, t3);
		ଫେରନ୍ତୁ {{ID(X), x}, {ID(Y), y}};
	}
	dict<IdString, ଗଣ୍ଠି_2> handle_alu(ଗଣ୍ଠି_2 a_in, ଗଣ୍ଠି_2 b_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width, ତାର୍କିକ is_signed, ଗଣ୍ଠି_2 ci, ଗଣ୍ଠି_2 bi) {
		ଗଣ୍ଠି_2 a = factory.extend(a_in, y_width, is_signed);
		ଗଣ୍ଠି_2 b_uninverted = factory.extend(b_in, y_width, is_signed);
		ଗଣ୍ଠି_2 b = factory.mux(b_uninverted, factory.bitwise_not(b_uninverted), bi);
		ଗଣ୍ଠି_2 x = factory.bitwise_xor(a, b);
		// we can compute the carry into each bit using (a+b+c)^a^b. since we want the carry out,
		// i.e. the carry into the next bit, we have to add an extra bit to a and b, and
		// then slice off the bottom bit of the result.
		ଗଣ୍ଠି_2 a_extra = factory.extend(a, y_width + 1, ମିଥ୍ୟା);
		ଗଣ୍ଠି_2 b_extra = factory.extend(b, y_width + 1, ମିଥ୍ୟା);
		ଗଣ୍ଠି_2 y_extra = factory.add(factory.add(a_extra, b_extra), factory.extend(ci, a.width() + 1, ମିଥ୍ୟା));
		ଗଣ୍ଠି_2 y = factory.slice(y_extra, 0, y_width);
		ଗଣ୍ଠି_2 carries = factory.bitwise_xor(y_extra, factory.bitwise_xor(a_extra, b_extra));
		ଗଣ୍ଠି_2 co = factory.slice(carries, 1, y_width);
		ଫେରନ୍ତୁ {{ID(X), x}, {ID(Y), y}, {ID(CO), co}};
	}
	ଗଣ୍ଠି_2 handle_lcu(ଗଣ୍ଠି_2 p, ଗଣ୍ଠି_2 g, ଗଣ୍ଠି_2 ci) {
		ଫେରନ୍ତୁ handle_alu(g, factory.bitwise_or(p, g), g.width(), ମିଥ୍ୟା, ci, factory.constant(Const(State::S0, 1))).at(ID(CO));
	}
ସର୍ବସାଧାରଣ:
	std::variant<dict<IdString, ଗଣ୍ଠି_2>, ଗଣ୍ଠି_2> handle(IdString cellName, IdString cellType, dict<IdString, Const> parameters, dict<IdString, ଗଣ୍ଠି_2> inputs)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = parameters.at(ID(A_WIDTH), Const(-1)).as_int();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = parameters.at(ID(B_WIDTH), Const(-1)).as_int();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = parameters.at(ID(Y_WIDTH), Const(-1)).as_int();
		ତାର୍କିକ a_signed = parameters.at(ID(A_SIGNED), Const(0)).as_bool();
		ତାର୍କିକ b_signed = parameters.at(ID(B_SIGNED), Const(0)).as_bool();
		ଯଦି(cellType.in(ID($add), ID($sub), ID($and), ID($or), ID($xor), ID($xnor), ID($mul))){
			ତାର୍କିକ is_signed = a_signed && b_signed;
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), y_width, is_signed);
			ଗଣ୍ଠି_2 b = factory.extend(inputs.at(ID(B)), y_width, is_signed);
			ଯଦି(cellType == ID($add))
				ଫେରନ୍ତୁ factory.add(a, b);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($sub))
				ଫେରନ୍ତୁ factory.sub(a, b);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($mul))
				ଫେରନ୍ତୁ factory.mul(a, b);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($and))
				ଫେରନ୍ତୁ factory.bitwise_and(a, b);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($or))
				ଫେରନ୍ତୁ factory.bitwise_or(a, b);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($xor))
				ଫେରନ୍ତୁ factory.bitwise_xor(a, b);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($xnor))
				ଫେରନ୍ତୁ factory.bitwise_not(factory.bitwise_xor(a, b));
			ଅନ୍ୟଥା
				log_abort();
		}ଅନ୍ୟଥା ଯଦି(cellType.in(ID($eq), ID($ne), ID($eqx), ID($nex), ID($le), ID($lt), ID($ge), ID($gt))){
			ତାର୍କିକ is_signed = a_signed && b_signed;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = max(a_width, b_width);
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			ଗଣ୍ଠି_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ଯଦି(cellType.in(ID($eq), ID($eqx)))
				ଫେରନ୍ତୁ factory.extend(factory.equal(a, b), y_width, ମିଥ୍ୟା);
			ଅନ୍ୟଥା ଯଦି(cellType.in(ID($ne), ID($nex)))
				ଫେରନ୍ତୁ factory.extend(factory.not_equal(a, b), y_width, ମିଥ୍ୟା);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($lt))
				ଫେରନ୍ତୁ factory.extend(is_signed ? factory.signed_greater_than(b, a) : factory.unsigned_greater_than(b, a), y_width, ମିଥ୍ୟା);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($le))
				ଫେରନ୍ତୁ factory.extend(is_signed ? factory.signed_greater_equal(b, a) : factory.unsigned_greater_equal(b, a), y_width, ମିଥ୍ୟା);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($gt))
				ଫେରନ୍ତୁ factory.extend(is_signed ? factory.signed_greater_than(a, b) : factory.unsigned_greater_than(a, b), y_width, ମିଥ୍ୟା);
			ଅନ୍ୟଥା ଯଦି(cellType == ID($ge))
				ଫେରନ୍ତୁ factory.extend(is_signed ? factory.signed_greater_equal(a, b) : factory.unsigned_greater_equal(a, b), y_width, ମିଥ୍ୟା);
			ଅନ୍ୟଥା
				log_abort();
		}ଅନ୍ୟଥା ଯଦି(cellType.in(ID($logic_or), ID($logic_and))){
			ଗଣ୍ଠି_2 a = factory.reduce_or(inputs.at(ID(A)));
			ଗଣ୍ଠି_2 b = factory.reduce_or(inputs.at(ID(B)));
			ଗଣ୍ଠି_2 y = cellType == ID($logic_and) ? factory.bitwise_and(a, b) : factory.bitwise_or(a, b);
			ଫେରନ୍ତୁ factory.extend(y, y_width, ମିଥ୍ୟା);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($not)){
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ଫେରନ୍ତୁ factory.bitwise_not(a);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($pos)){
			ଫେରନ୍ତୁ factory.extend(inputs.at(ID(A)), y_width, a_signed);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($barrier)){
			ଫେରନ୍ତୁ inputs.at(ID(A));
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($neg)){
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ଫେରନ୍ତୁ factory.unary_minus(a);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($logic_not)){
			ଗଣ୍ଠି_2 a = factory.reduce_or(inputs.at(ID(A)));
			ଗଣ୍ଠି_2 y = factory.bitwise_not(a);
			ଫେରନ୍ତୁ factory.extend(y, y_width, ମିଥ୍ୟା);
		}ଅନ୍ୟଥା ଯଦି(cellType.in(ID($reduce_or), ID($reduce_bool))){
			ଗଣ୍ଠି_2 a = factory.reduce_or(inputs.at(ID(A)));
			ଫେରନ୍ତୁ factory.extend(a, y_width, ମିଥ୍ୟା);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($reduce_and)){
			ଗଣ୍ଠି_2 a = factory.reduce_and(inputs.at(ID(A)));
			ଫେରନ୍ତୁ factory.extend(a, y_width, ମିଥ୍ୟା);
		}ଅନ୍ୟଥା ଯଦି(cellType.in(ID($reduce_xor), ID($reduce_xnor))){
			ଗଣ୍ଠି_2 a = factory.reduce_xor(inputs.at(ID(A)));
			ଗଣ୍ଠି_2 y = cellType == ID($reduce_xnor) ? factory.bitwise_not(a) : a;
			ଫେରନ୍ତୁ factory.extend(y, y_width, ମିଥ୍ୟା);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($shl) || cellType == ID($sshl)){
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), y_width, a_signed);
			ଗଣ୍ଠି_2 b = inputs.at(ID(B));
			ଫେରନ୍ତୁ logical_shift_ବାମ(a, b);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($shr) || cellType == ID($sshr)){
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = max(a_width, y_width);
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), width, a_signed);
			ଗଣ୍ଠି_2 b = inputs.at(ID(B));
			ଗଣ୍ଠି_2 y = a_signed && cellType == ID($sshr) ?
				arithmetic_shift_ଡାହାଣ(a, b) :
				logical_shift_ଡାହାଣ(a, b);
			ଫେରନ୍ତୁ factory.extend(y, y_width, a_signed);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($shiftx) || cellType == ID($shift)){
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = max(a_width, y_width);
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), width, cellType == ID($shift) && a_signed);
			ଗଣ୍ଠି_2 b = inputs.at(ID(B));
			ଗଣ୍ଠି_2 shr = logical_shift_ଡାହାଣ(a, b);
			ଯଦି(b_signed) {
				ଗଣ୍ଠି_2 shl = logical_shift_ବାମ(a, factory.unary_minus(b));
				ଗଣ୍ଠି_2 y = factory.mux(shr, shl, sign(b));
				ଫେରନ୍ତୁ factory.extend(y, y_width, ମିଥ୍ୟା);
			} ଅନ୍ୟଥା {
				ଫେରନ୍ତୁ factory.extend(shr, y_width, ମିଥ୍ୟା);
			}
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($mux)){
			ଫେରନ୍ତୁ factory.mux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($pmux)){
			ଫେରନ୍ତୁ handle_pmux(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(S)));
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($concat)){
			ଗଣ୍ଠି_2 a = inputs.at(ID(A));
			ଗଣ୍ଠି_2 b = inputs.at(ID(B));
			ଫେରନ୍ତୁ factory.concat(a, b);
		}ଅନ୍ୟଥା ଯଦି(cellType == ID($slice)){
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = parameters.at(ID(OFFSET)).as_int();
			ଗଣ୍ଠି_2 a = inputs.at(ID(A));
			ଫେରନ୍ତୁ factory.slice(a, offset, y_width);
		}ଅନ୍ୟଥା ଯଦି(cellType.in(ID($div), ID($mod), ID($divfloor), ID($modfloor))) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = max(a_width, b_width);
			ତାର୍କିକ is_signed = a_signed && b_signed;
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), width, is_signed);
			ଗଣ୍ଠି_2 b = factory.extend(inputs.at(ID(B)), width, is_signed);
			ଯଦି(is_signed) {
				ଯଦି(cellType == ID($div)) {
					// divide absolute values, then flip the sign if input signs differ
					// but extend the width first, to handle the case (most negative value) / (-1)
					ଗଣ୍ଠି_2 abs_y = factory.unsigned_div(abs(a), abs(b));
					ଗଣ୍ଠି_2 out_sign = factory.not_equal(sign(a), sign(b));
					ଫେରନ୍ତୁ neg_if(factory.extend(abs_y, y_width, ମିଥ୍ୟା), out_sign);
				} ଅନ୍ୟଥା ଯଦି(cellType == ID($mod)) {
					// similar to division but output sign == divisor sign
					ଗଣ୍ଠି_2 abs_y = factory.unsigned_mod(abs(a), abs(b));
					ଫେରନ୍ତୁ neg_if(factory.extend(abs_y, y_width, ମିଥ୍ୟା), sign(a));
				} ଅନ୍ୟଥା ଯଦି(cellType == ID($divfloor)) {
					// if b is negative, flip both signs so that b is positive
					ଗଣ୍ଠି_2 b_sign = sign(b);
					ଗଣ୍ଠି_2 a1 = neg_if(a, b_sign);
					ଗଣ୍ଠି_2 b1 = neg_if(b, b_sign);
					// if a is now negative, calculate ~((~a) / b) = -((-a - 1) / b + 1)
					// which equals the negative of (-a) / b with rounding up rather than down
					// note that to handle the case where a = most negative value properly,
					// we have to calculate a1_sign from the original values rather than using sign(a1)
					ଗଣ୍ଠି_2 a1_sign = factory.bitwise_and(factory.not_equal(sign(a), sign(b)), factory.reduce_or(a));
					ଗଣ୍ଠି_2 a2 = factory.mux(a1, factory.bitwise_not(a1), a1_sign);
					ଗଣ୍ଠି_2 y1 = factory.unsigned_div(a2, b1);
					ଗଣ୍ଠି_2 y2 = factory.extend(y1, y_width, ମିଥ୍ୟା);
					ଫେରନ୍ତୁ factory.mux(y2, factory.bitwise_not(y2), a1_sign);
				} ଅନ୍ୟଥା ଯଦି(cellType == ID($modfloor)) {
					// calculate |a| % |b| and then subtract from |b| if input signs differ and the remainder is non-zero
					ଗଣ୍ଠି_2 abs_b = abs(b);
					ଗଣ୍ଠି_2 abs_y = factory.unsigned_mod(abs(a), abs_b);
					ଗଣ୍ଠି_2 flip_y = factory.bitwise_and(factory.bitwise_xor(sign(a), sign(b)), factory.reduce_or(abs_y));
					ଗଣ୍ଠି_2 y_flipped = factory.mux(abs_y, factory.sub(abs_b, abs_y), flip_y);
					// since y_flipped is strictly less than |b|, the top bit is always 0 and we can just sign extend the flipped result
					ଗଣ୍ଠି_2 y = neg_if(y_flipped, sign(b));
					ଫେରନ୍ତୁ factory.extend(y, y_width, ସତ୍ୟ);
				} ଅନ୍ୟଥା
					log_error("unhandled cell in CellSimplifier %s\n", factory.ir().design->twines().str(cellType));
			} ଅନ୍ୟଥା {
				ଯଦି(cellType.in(ID($mod), ID($modfloor)))
					ଫେରନ୍ତୁ factory.extend(factory.unsigned_mod(a, b), y_width, ମିଥ୍ୟା);
				ଅନ୍ୟଥା
					ଫେରନ୍ତୁ factory.extend(factory.unsigned_div(a, b), y_width, ମିଥ୍ୟା);
			}
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($pow)) {
			ଫେରନ୍ତୁ handle_pow(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed);
		} ଅନ୍ୟଥା ଯଦି (cellType == ID($lut)) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = parameters.at(ID(WIDTH)).as_int();
			Const lut_table = parameters.at(ID(LUT));
			lut_table.extu(1 << width);
			ଫେରନ୍ତୁ handle_bmux(factory.constant(lut_table), inputs.at(ID(A)), 0, 1, width);
		} ଅନ୍ୟଥା ଯଦି (cellType == ID($bwmux)) {
			ଗଣ୍ଠି_2 a = inputs.at(ID(A));
			ଗଣ୍ଠି_2 b = inputs.at(ID(B));
			ଗଣ୍ଠି_2 s = inputs.at(ID(S));
			ଫେରନ୍ତୁ factory.bitwise_or(
				factory.bitwise_and(a, factory.bitwise_not(s)),
				factory.bitwise_and(b, s));
		} ଅନ୍ୟଥା ଯଦି (cellType == ID($bweqx)) {
			ଗଣ୍ଠି_2 a = inputs.at(ID(A));
			ଗଣ୍ଠି_2 b = inputs.at(ID(B));
			ଫେରନ୍ତୁ factory.bitwise_not(factory.bitwise_xor(a, b));
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($bmux)) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = parameters.at(ID(WIDTH)).as_int();
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_width = parameters.at(ID(S_WIDTH)).as_int();
			ଫେରନ୍ତୁ handle_bmux(inputs.at(ID(A)), inputs.at(ID(S)), 0, width, s_width);
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($demux)) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = parameters.at(ID(WIDTH)).as_int();
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_width = parameters.at(ID(S_WIDTH)).as_int();
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = width << s_width;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = ceil_log2(y_width);
			ଗଣ୍ଠି_2 a = factory.extend(inputs.at(ID(A)), y_width, ମିଥ୍ୟା);
			ଗଣ୍ଠି_2 s = factory.extend(inputs.at(ID(S)), b_width, ମିଥ୍ୟା);
			ଗଣ୍ଠି_2 b = factory.mul(s, factory.constant(Const(width, b_width)));
			ଫେରନ୍ତୁ factory.logical_shift_ବାମ(a, b);
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($fa)) {
			ଫେରନ୍ତୁ handle_fa(inputs.at(ID(A)), inputs.at(ID(B)), inputs.at(ID(C)));
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($lcu)) {
			ଫେରନ୍ତୁ handle_lcu(inputs.at(ID(P)), inputs.at(ID(G)), inputs.at(ID(CI)));
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($alu)) {
			ଫେରନ୍ତୁ handle_alu(inputs.at(ID(A)), inputs.at(ID(B)), y_width, a_signed && b_signed, inputs.at(ID(CI)), inputs.at(ID(BI)));
		} ଅନ୍ୟଥା ଯଦି(cellType.in(ID($ପ୍ରତିଜ୍ଞା), ID($assume), ID($live), ID($fair), ID($cover))) {
			ଗଣ୍ଠି_2 a = factory.mux(factory.constant(Const(State::S1, 1)), inputs.at(ID(A)), inputs.at(ID(EN)));
			auto &output = factory.add_output(cellName, cellType, Sort(1));
			output.set_ମୂଲ୍ୟ(a);
			ଫେରନ୍ତୁ {};
		} ଅନ୍ୟଥା ଯଦି(cellType.in(ID($anyconst), ID($allconst), ID($anyseq), ID($allseq))) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = parameters.at(ID(WIDTH)).as_int();
			auto &ନିବେଶ = factory.add_ନିବେଶ(cellName, cellType, Sort(width));
			ଫେରନ୍ତୁ factory.ମୂଲ୍ୟ(ନିବେଶ);
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($initstate)) {
			ଯଦି(factory.ir().has_state(ID($initstate), ID($state)))
				ଫେରନ୍ତୁ factory.ମୂଲ୍ୟ(factory.ir().state(ID($initstate)));
			ଅନ୍ୟଥା {
				auto &state = factory.add_state(ID($initstate), ID($state), Sort(1));
				state.set_initial_ମୂଲ୍ୟ(RTLIL::Const(State::S1, 1));
				state.set_next_ମୂଲ୍ୟ(factory.constant(RTLIL::Const(State::S0, 1)));
				ଫେରନ୍ତୁ factory.ମୂଲ୍ୟ(state);
			}
		} ଅନ୍ୟଥା ଯଦି(cellType == ID($check)) {
			log_error("The design contains a $check cell `%s'. This is not supported by the functional backend. Call `chformal -lower' to avoid this error.\n", factory.ir().design->twines().str(cellName));
		} ଅନ୍ୟଥା {
			log_error("`%s' cells are not supported by the functional backend\n", factory.ir().design->twines().str(cellType));
		}
	}
};

ଶ୍ରେଣୀ FunctionalIRConstruction {
	std::deque<std::variant<DriveSpec, Cell *>> queue;
	dict<DriveSpec, ଗଣ୍ଠି_2> graph_nodes;
	dict<std::pair<Cell *, IdString>, ଗଣ୍ଠି_2> cell_outputs;
	DriverMap driver_map;
	Factory& factory;
	CellSimplifier simplifier;
	vector<Mem> memories_vector;
	dict<Cell*, Mem*> memories;
	SigMap sig_map; // TODO: this is only for FfInitVals, remove this once FfInitVals supports DriverMap
	FfInitVals ff_initvals;

	ଗଣ୍ଠି_2 enqueue(DriveSpec ଅଚଳ &spec)
	{
		auto it = graph_nodes.find(spec);
		ଯଦି(it == graph_nodes.end()){
			auto ଗଣ୍ଠି = factory.create_pending(spec.size());
			graph_nodes.ଭର୍ତ୍ତି({spec, ଗଣ୍ଠି});
			queue.emplace_back(spec);
			ଫେରନ୍ତୁ ଗଣ୍ଠି;
		}ଅନ୍ୟଥା
			ଫେରନ୍ତୁ it->second;
	}
	ଗଣ୍ଠି_2 enqueue_cell(Cell *cell, IdString port_name)
	{
		auto it = cell_outputs.find({cell, port_name});
		ଯଦି(it == cell_outputs.end()) {
			queue.emplace_back(cell);
			std::optional<ଗଣ୍ଠି_2> rv;
			ପାଇଁ(auto ଅଚଳ &[name, sigspec] : cell->connections())
				ଯଦି(driver_map.celltypes.cell_output(cell->type, name)) {
					auto ଗଣ୍ଠି = factory.create_pending(sigspec.size());
					factory.suggest_name(ଗଣ୍ଠି, cell->name.str() + "$" + PooledName(cell->module, name).str());
					cell_outputs.emplace({cell, name}, ଗଣ୍ଠି);
					ଯଦି(name == port_name)
						rv = ଗଣ୍ଠି;
				}
			ଫେରନ୍ତୁ *rv;
		} ଅନ୍ୟଥା
			ଫେରନ୍ତୁ it->second;
	}
ସର୍ବସାଧାରଣ:
	FunctionalIRConstruction(Module *module, Factory &f)
		: factory(f)
		, simplifier(f)
		, sig_map(module)
		, ff_initvals(&sig_map, module)
	{
		driver_map.add(module);
		ପାଇଁ (auto cell : module->cells()) {
			ଯଦି (cell->type.in(ID($ପ୍ରତିଜ୍ଞା), ID($assume), ID($live), ID($fair), ID($cover), ID($check)))
				queue.emplace_back(cell);
		}
		// we are relying here on unsorted pools iterating last-in-first-out
		ପାଇଁ (auto riter = module->ports.rbegin(); riter != module->ports.rend(); ++riter) {
			auto *wire = module->wire(*riter);
			ଯଦି (wire && wire->port_ନିବେଶ) {
				factory.add_ନିବେଶ(wire->name, ID($ନିବେଶ), Sort(wire->width));
			}
			ଯଦି (wire && wire->port_output) {
				auto &output = factory.add_output(wire->name, ID($output), Sort(wire->width));
				output.set_ମୂଲ୍ୟ(enqueue(DriveChunk(DriveChunkWire(wire, 0, wire->width))));
			}
		}
		memories_vector = Mem::get_all_memories(module);
		ପାଇଁ (auto &mem : memories_vector) {
			ଯଦି (mem.cell != nullptr)
				memories[mem.cell] = &mem;
		}
	}
ବ୍ୟକ୍ତିଗତ:
	ଗଣ୍ଠି_2 concatenate_read_results(Mem *mem, vector<ଗଣ୍ଠି_2> results)
	{
		// sanity check: all read ports concatenated should equal to the RD_DATA port
		ଅଚଳ SigSpec &rd_data = mem->cell->connections().at(ID(RD_DATA));
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା current = 0;
		ପାଇଁ(size_t i = 0; i < mem->rd_ports.size(); i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = mem->width << mem->rd_ports[i].wide_log2;
			log_assert (results[i].width() == width);
			log_assert (mem->rd_ports[i].data == rd_data.extract(current, width));
			current += width;
		}
		log_assert (current == rd_data.size());
		log_assert (!results.empty());
		ଗଣ୍ଠି_2 ଗଣ୍ଠି = results[0];
		ପାଇଁ(size_t i = 1; i < results.size(); i++)
			ଗଣ୍ଠି = factory.concat(ଗଣ୍ଠି, results[i]);
		ଫେରନ୍ତୁ ଗଣ୍ଠି;
	}
	ଗଣ୍ଠି_2 handle_memory(Mem *mem)
	{
		// To simplify memory handling, the functional backend makes the following assumptions:
		// - Since async2sync or clk2fflogic must be run to use the functional backend,
		//   we can assume that all ports are asynchronous.
		// - Async rd/wr are always transparent and so we must do reads after writes,
		//   but we can ignore transparency_mask.
		// - We ignore collision_x_mask because x is a dont care value for us anyway.
		// - Since wr port j can only have priority over wr port i if j > i, if we do writes in
		//   ascending index order the result will obey the priorty relation.
		vector<ଗଣ୍ଠି_2> read_results;
		auto &state = factory.add_state(mem->cell->name, ID($state), Sort(ceil_log2(mem->size), mem->width));
		state.set_initial_ମୂଲ୍ୟ(MemContents(mem));
		ଗଣ୍ଠି_2 ଗଣ୍ଠି = factory.ମୂଲ୍ୟ(state);
		ପାଇଁ (size_t i = 0; i < mem->wr_ports.size(); i++) {
			ଅଚଳ auto &wr = mem->wr_ports[i];
			ଯଦି (wr.clk_enable)
				log_error("Write port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ଗଣ୍ଠି_2 en = enqueue(driver_map(DriveSpec(wr.en)));
			ଗଣ୍ଠି_2 addr = enqueue(driver_map(DriveSpec(wr.addr)));
			ଗଣ୍ଠି_2 new_data = enqueue(driver_map(DriveSpec(wr.data)));
			ଗଣ୍ଠି_2 old_data = factory.memory_read(ଗଣ୍ଠି, addr);
			ଗଣ୍ଠି_2 wr_data = simplifier.bitwise_mux(old_data, new_data, en);
			ଗଣ୍ଠି = factory.memory_write(ଗଣ୍ଠି, addr, wr_data);
		}
		ଯଦି (mem->rd_ports.empty())
			log_error("Memory %s.%s has no read ports. This is not supported by the functional backend. "
				"Call opt_clean to remove it.", mem->module, PooledName(mem->module, mem->memid).unescape());
		ପାଇଁ (size_t i = 0; i < mem->rd_ports.size(); i++) {
			ଅଚଳ auto &rd = mem->rd_ports[i];
			ଯଦି (rd.clk_enable)
				log_error("Read port %zd of memory %s.%s is clocked. This is not supported by the functional backend. "
					"Call memory_nordff to avoid this error.\n", i, mem->module, PooledName(mem->module, mem->memid).unescape());
			ଗଣ୍ଠି_2 addr = enqueue(driver_map(DriveSpec(rd.addr)));
			read_results.ଠେଲ_back(factory.memory_read(ଗଣ୍ଠି, addr));
		}
		state.set_next_ମୂଲ୍ୟ(ଗଣ୍ଠି);
		ଫେରନ୍ତୁ concatenate_read_results(mem, read_results);
	}
	ଶୂନ୍ୟ process_cell(Cell *cell)
	{
		ଯଦି (cell->is_mem_cell()) {
			Mem *mem = memories.at(cell, nullptr);
			ଯଦି (mem == nullptr) {
				log_assert(cell->has_memid());
				log_error("The design contains an unpacked memory at %s. This is not supported by the functional backend. "
					"Call memory_collect to avoid this error.\n", log_const(cell->parameters.at(ID(MEMID))));
			}
			ଗଣ୍ଠି_2 ଗଣ୍ଠି = handle_memory(mem);
			factory.update_pending(cell_outputs.at({cell, ID(RD_DATA)}), ଗଣ୍ଠି);
		} ଅନ୍ୟଥା ଯଦି (cell->is_builtin_ff()) {
			FfData ff(&ff_initvals, cell);
			ଯଦି (!ff.has_gclk)
				log_error("The design contains a %s flip-flop at %s. This is not supported by the functional backend. "
					"Call async2sync or clk2fflogic to avoid this error.\n", cell->type.unescape(), cell);
			auto &state = factory.add_state(ff.name, ID($state), Sort(ff.width));
			ଗଣ୍ଠି_2 q_ମୂଲ୍ୟ = factory.ମୂଲ୍ୟ(state);
			factory.suggest_name(q_ମୂଲ୍ୟ, ff.name);
			factory.update_pending(cell_outputs.at({cell, ID(Q)}), q_ମୂଲ୍ୟ);
			state.set_next_ମୂଲ୍ୟ(enqueue(ff.sig_d));
			state.set_initial_ମୂଲ୍ୟ(ff.val_init);
		} ଅନ୍ୟଥା {
			dict<IdString, ଗଣ୍ଠି_2> connections;
			IdString output_name; // for the single output case
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_outputs = 0;
			ପାଇଁ(auto ଅଚଳ &[name, sigspec] : cell->connections()) {
				ଯଦି(driver_map.celltypes.cell_ନିବେଶ(cell->type, name) && sigspec.size() > 0)
					connections.ଭର୍ତ୍ତି({ name, enqueue(DriveChunkPort(cell, {name, sigspec})) });
				ଯଦି(driver_map.celltypes.cell_output(cell->type, name)) {
					output_name = name;
					n_outputs++;
				}
			}
			std::variant<dict<IdString, ଗଣ୍ଠି_2>, ଗଣ୍ଠି_2> outputs = simplifier.handle(cell->name, cell->type, cell->parameters, connections);
			ଯଦି(auto *nodep = std::get_if<ଗଣ୍ଠି_2>(&outputs); nodep != nullptr) {
				log_assert(n_outputs == 1);
				factory.update_pending(cell_outputs.at({cell, output_name}), *nodep);
			} ଅନ୍ୟଥା {
				ପାଇଁ(auto [name, ଗଣ୍ଠି] : std::get<dict<IdString, ଗଣ୍ଠି_2>>(outputs))
					factory.update_pending(cell_outputs.at({cell, name}), ଗଣ୍ଠି);
			}
		}
	}
	ଶୂନ୍ୟ undriven(ଅଚଳ std::string& name) {
		log_error("The design contains an undriven signal %s. This is not supported by the functional backend. "
			"Call setundef with appropriate options to avoid this error.\n", name);
	}
	// we perform this check separately to give better error messages that include the wire or port name
	ଶୂନ୍ୟ check_undriven(DriveSpec ଅଚଳ& spec, std::string ଅଚଳ& name) {
		ପାଇଁ(auto ଅଚଳ &chunk : spec.chunks())
			ଯଦି(chunk.is_none())
				undriven(name);
	}
ସର୍ବସାଧାରଣ:
	ଶୂନ୍ୟ process_queue()
	{
		ପାଇଁ (; !queue.empty(); queue.କାଢ଼_front()) {
			ଯଦି(auto p = std::get_if<Cell *>(&queue.front()); p != nullptr) {
				process_cell(*p);
				ଜାରି;
			}

			DriveSpec spec = std::get<DriveSpec>(queue.front());
			ଗଣ୍ଠି_2 pending = graph_nodes.at(spec);

			ଯଦି (spec.chunks().size() > 1) {
				auto chunks = spec.chunks();
				ଗଣ୍ଠି_2 ଗଣ୍ଠି = enqueue(chunks[0]);
				ପାଇଁ(size_t i = 1; i < chunks.size(); i++)
					ଗଣ୍ଠି = factory.concat(ଗଣ୍ଠି, enqueue(chunks[i]));
				factory.update_pending(pending, ଗଣ୍ଠି);
			} ଅନ୍ୟଥା ଯଦି (spec.chunks().size() == 1) {
				DriveChunk chunk = spec.chunks()[0];
				ଯଦି (chunk.is_wire()) {
					DriveChunkWire wire_chunk = chunk.wire();
					ଯଦି (wire_chunk.is_whole()) {
						ଯଦି (wire_chunk.wire->port_ନିବେଶ) {
							ଗଣ୍ଠି_2 ଗଣ୍ଠି = factory.ମୂଲ୍ୟ(factory.ir().ନିବେଶ(wire_chunk.wire->name));
							factory.suggest_name(ଗଣ୍ଠି, wire_chunk.wire->name);
							factory.update_pending(pending, ଗଣ୍ଠି);
						} ଅନ୍ୟଥା {
							DriveSpec driver = driver_map(DriveSpec(wire_chunk));
							check_undriven(driver, wire_chunk.wire->name.unescape());
							ଗଣ୍ଠି_2 ଗଣ୍ଠି = enqueue(driver);
							factory.suggest_name(ଗଣ୍ଠି, wire_chunk.wire->name);
							factory.update_pending(pending, ଗଣ୍ଠି);
						}
					} ଅନ୍ୟଥା {
						DriveChunkWire whole_wire(wire_chunk.wire, 0, wire_chunk.wire->width);
						ଗଣ୍ଠି_2 ଗଣ୍ଠି = factory.slice(enqueue(whole_wire), wire_chunk.offset, wire_chunk.width);
						factory.update_pending(pending, ଗଣ୍ଠି);
					}
				} ଅନ୍ୟଥା ଯଦି (chunk.is_port()) {
					DriveChunkPort port_chunk = chunk.port();
					ଯଦି (port_chunk.is_whole()) {
						ଯଦି (driver_map.celltypes.cell_output(port_chunk.cell->type, port_chunk.port)) {
							ଗଣ୍ଠି_2 ଗଣ୍ଠି = enqueue_cell(port_chunk.cell, port_chunk.port);
							factory.update_pending(pending, ଗଣ୍ଠି);
						} ଅନ୍ୟଥା {
							DriveSpec driver = driver_map(DriveSpec(port_chunk));
							check_undriven(driver, port_chunk.cell->name.unescape() + " port " + PooledName(port_chunk.cell->module, port_chunk.port).unescape());
							factory.update_pending(pending, enqueue(driver));
						}
					} ଅନ୍ୟଥା {
						DriveChunkPort whole_port(port_chunk.cell, port_chunk.port, 0, GetSize(port_chunk.cell->connections().at(port_chunk.port)));
						ଗଣ୍ଠି_2 ଗଣ୍ଠି = factory.slice(enqueue(whole_port), port_chunk.offset, port_chunk.width);
						factory.update_pending(pending, ଗଣ୍ଠି);
					}
				} ଅନ୍ୟଥା ଯଦି (chunk.is_constant()) {
					ଗଣ୍ଠି_2 ଗଣ୍ଠି = factory.constant(chunk.constant());
					factory.suggest_name(ଗଣ୍ଠି, "$const" + std::to_string(chunk.size()) + "b" + chunk.constant().as_string());
					factory.update_pending(pending, ଗଣ୍ଠି);
				} ଅନ୍ୟଥା ଯଦି (chunk.is_multiple()) {
					log_error("Signal %s has multiple drivers. This is not supported by the functional backend. "
						"If tristate drivers are used, call tristate -formal to avoid this error.\n", log_signal(chunk));
				} ଅନ୍ୟଥା ଯଦି (chunk.is_none()) {
					undriven(log_signal(chunk));
				} ଅନ୍ୟଥା {
					log_error("unhandled drivespec: %s\n", log_signal(chunk));
					log_abort();
				}
			} ଅନ୍ୟଥା {
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
    ଫେରନ୍ତୁ ir;
}

ଶୂନ୍ୟ IR::topological_sort() {
    Graph::SccAdaptor compute_graph_scc(_graph);
    ତାର୍କିକ scc = ମିଥ୍ୟା;
    std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> perm;
    TopoSortedSccs toposort(compute_graph_scc, [&](ପୂର୍ଣ୍ଣସଂଖ୍ୟା *begin, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *end) {
        perm.ଭର୍ତ୍ତି(perm.end(), begin, end);
        ଯଦି (end > begin + 1)
        {
            log_warning("Combinational loop:\n");
            ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *i = begin; i != end; ++i) {
				ଗଣ୍ଠି_2 ଗଣ୍ଠି(_graph[*i], design);
                log("- %s = %s\n", ଗଣ୍ଠି.name().unescape(), ଗଣ୍ଠି.to_string());
			}
            log("\n");
            scc = ସତ୍ୟ;
        }
    });
	ପାଇଁ(ଅଚଳ auto &[name, state]: _states)
		ଯଦି(state.has_next_ମୂଲ୍ୟ())
			toposort.process(state.next_ମୂଲ୍ୟ().id());
	ପାଇଁ(ଅଚଳ auto &[name, output]: _outputs)
		ଯଦି(output.has_ମୂଲ୍ୟ())
			toposort.process(output.ମୂଲ୍ୟ().id());
	// any nodes untouched by this point are dead code and will be removed by permute
    _graph.permute(perm);
    ଯଦି(scc) log_error("The design contains combinational loops. This is not supported by the functional backend. "
		"Try `scc -select; simplemap; select -clear` to avoid this error.\n");
}

ସ୍ଥିର IdString merge_name(IdString a, IdString b) {
	ଯଦି(!a.isPublic() && b.isPublic())
		ଫେରନ୍ତୁ b;
	ଅନ୍ୟଥା
		ଫେରନ୍ତୁ a;
}

ଶୂନ୍ୟ IR::forward_buf() {
    std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> perm, alias;
    perm.clear();

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < _graph.size(); ++i)
    {
        auto ଗଣ୍ଠି = _graph[i];
        ଯଦି (ଗଣ୍ଠି.function().fn() == Fn::buf && ଗଣ୍ଠି.arg(0).index() < i)
        {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା target_index = alias[ଗଣ୍ଠି.arg(0).index()];
            auto target_ଗଣ୍ଠି = _graph[perm[target_index]];
			ଯଦି(ଗଣ୍ଠି.has_sparse_attr()) {
				ଯଦି(target_ଗଣ୍ଠି.has_sparse_attr()) {
					IdString id = merge_name(ଗଣ୍ଠି.sparse_attr(), target_ଗଣ୍ଠି.sparse_attr());
					target_ଗଣ୍ଠି.sparse_attr() = id;
				} ଅନ୍ୟଥା {
					IdString id = ଗଣ୍ଠି.sparse_attr();
					target_ଗଣ୍ଠି.sparse_attr() = id;
				}
			}
            alias.ଠେଲ_back(target_index);
        }
        ଅନ୍ୟଥା
        {
            alias.ଠେଲ_back(GetSize(perm));
            perm.ଠେଲ_back(i);
        }
    }
    _graph.permute(perm, alias);
}

// Quoting routine to make error messages nicer
ସ୍ଥିର std::string quote_fmt(ଅଚଳ ଅକ୍ଷର *fmt)
{
	std::string r;
	ପାଇଁ(ଅଚଳ ଅକ୍ଷର *p = fmt; *p != 0; p++) {
		ବାଛ(*p) {
		ସ୍ଥିତି '\n': r += "\\n"; ଭାଙ୍ଗ;
		ସ୍ଥିତି '\t': r += "\\t"; ଭାଙ୍ଗ;
		ସ୍ଥିତି '"': r += "\\\""; ଭାଙ୍ଗ;
		ସ୍ଥିତି '\\': r += "\\\\"; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ: r += *p; ଭାଙ୍ଗ;
		}
	}
	ଫେରନ୍ତୁ r;
}

ଶୂନ୍ୟ Writer::print_impl(ଅଚଳ ଅକ୍ଷର *fmt, vector<std::function<ଶୂନ୍ୟ()>> &fns)
{
	size_t next_index = 0;
	ପାଇଁ(ଅଚଳ ଅକ୍ଷର *p = fmt; *p != 0; p++)
		ବାଛ(*p) {
		ସ୍ଥିତି '{':
			ଯଦି(*++p == '{') {
				*os << '{';
			} ଅନ୍ୟଥା {
				ଅକ୍ଷର *pe;
				size_t index = strtoul(p, &pe, 10);
				ଯଦି(*pe != '}')
					log_error("invalid format string: expected {<number>}, {} or {{, got \"%s\": \"%s\"\n",
						quote_fmt(std::string(p - 1, pe - p + 2).c_str()).c_str(),
						quote_fmt(fmt).c_str());
				ଯଦି(p == pe)
					index = next_index;
				ଅନ୍ୟଥା
					p = pe;
				ଯଦି(index >= fns.size())
					log_error("invalid format string: index %zu out of bounds (%zu): \"%s\"\n", index, fns.size(), quote_fmt(fmt));
				fns[index]();
				next_index = index + 1;
			}
			ଭାଙ୍ଗ;
		ସ୍ଥିତି '}':
			p++;
			ଯଦି(*p != '}')
				log_error("invalid format string: unescaped }: \"%s\"\n", quote_fmt(fmt));
			*os << '}';
			ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			*os << *p;
		}
}

}
YOSYS_NAMESPACE_END
