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

#اگر_غیر_تعریف_شدہ FUNCTIONAL_H
#تعریف_کرو FUNCTIONAL_H

#شامل "kernel/yosys.h"
#شامل "kernel/compute_graph.h"
#شامل "kernel/drivertools.h"
#شامل "kernel/mem.h"
#شامل "kernel/utils.h"

USING_YOSYS_NAMESPACE
YOSYS_NAMESPACE_BEGIN

namespace Functional {
	// each function is documented with a short pseudocode declaration or definition
	// standard C/Verilog operators are used to describe the result
	//
	// the sorts used in this are:
	// - bit[N]: a bitvector of N bits
	//   bit[N] can be indicated as signed or unsigned. this is not tracked by the functional backend
	//   but is meant to indicate how the value is interpreted
	//   if a bit[N] is marked as neither signed nor unsigned, this means the result should be valid with *either* interpretation
	// - memory[N, M]: a memory with N address and M data bits
	// - int: C++ int
	// - Const[N]: yosys RTLIL::Const (with size() == N)
	// - IdString: yosys IdString
	// - any: used in documentation to indicate that the sort is unconstrained
	//
	// nodes in the functional backend are either of sort bit[N] or memory[N,M] (for some N, M: int)
	// additionally, they can carry a constant of sort int, Const[N] or IdString
	// each node has a 'sort' field that stores the sort of the node
	// slice, zero_extend, sign_extend use the sort field to store out_width
	شمار class Fn {
		// invalid() = known-invalid/shouldn't happen value
		// TODO: maybe remove this and use e.g. std::optional instead?
		invalid,
		// buf(a: any): any = a
		// no-op operation
		// when constructing the compute graph we generate invalid buf() nodes as a placeholder
		// and later insert the argument
		buf,
		// slice(a: bit[in_width], offset: int, out_width: int): bit[out_width] = a[offset +: out_width]
		// required: offset + out_width <= in_width
		قاش,
		// zero_extend(a: unsigned bit[in_width], out_width: int): unsigned bit[out_width] = a (zero extended)
		// required: out_width > in_width
		zero_extend,
		// sign_extend(a: signed bit[in_width], out_width: int): signed bit[out_width] = a (sign extended)
		// required: out_width > in_width
		sign_extend,
		// concat(a: bit[N], b: bit[M]): bit[N+M] = {b, a} (verilog syntax)
		// concatenates two bitvectors, with a in the least significant position and b in the more significant position
		concat,
		// add(a: bit[N], b: bit[N]): bit[N] = a + b
		جوڑو,
		// sub(a: bit[N], b: bit[N]): bit[N] = a - b
		sub,
		// mul(a: bit[N], b: bit[N]): bit[N] = a * b
		mul,
		// unsigned_div(a: unsigned bit[N], b: unsigned bit[N]): bit[N] = a / b
		unsigned_div,
		// unsigned_mod(a: signed bit[N], b: signed bit[N]): bit[N] = a % b
		unsigned_mod,
		// bitwise_and(a: bit[N], b: bit[N]): bit[N] = a & b
		bitwise_and,
		// bitwise_or(a: bit[N], b: bit[N]): bit[N] = a | b
		bitwise_or,
		// bitwise_xor(a: bit[N], b: bit[N]): bit[N] = a ^ b
		bitwise_xor,
		// bitwise_not(a: bit[N]): bit[N] = ~a
		bitwise_not,
		// reduce_and(a: bit[N]): bit[1] = &a
		reduce_and,
		// reduce_or(a: bit[N]): bit[1] = |a
		reduce_or,
		// reduce_xor(a: bit[N]): bit[1] = ^a
		reduce_xor,
		// unary_minus(a: bit[N]): bit[N] = -a
		unary_minus,
		// equal(a: bit[N], b: bit[N]): bit[1] = (a == b)
		equal,
		// not_equal(a: bit[N], b: bit[N]): bit[1] = (a != b)
		not_equal,
		// signed_greater_than(a: signed bit[N], b: signed bit[N]): bit[1] = (a > b)
		signed_greater_than,
		// signed_greater_equal(a: signed bit[N], b: signed bit[N]): bit[1] = (a >= b)
		signed_greater_equal,
		// unsigned_greater_than(a: unsigned bit[N], b: unsigned bit[N]): bit[1] = (a > b)
		unsigned_greater_than,
		// unsigned_greater_equal(a: unsigned bit[N], b: unsigned bit[N]): bit[1] = (a >= b)
		unsigned_greater_equal,
		// logical_shift_left(a: bit[N], b: unsigned bit[M]): bit[N] = a << b
		// required: M == clog2(N)
		logical_سرکاؤ_بایاں,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_سرکاؤ_دایاں,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_سرکاؤ_دایاں,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		داخلہ,
		// state(a: IdString): any
		// returns the current value of the state variable with the specified name
		state,
		// memory_read(memory: memory[addr_width, data_width], addr: bit[addr_width]): bit[data_width] = memory[addr]
		حافظہ_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		حافظہ_write
	};
	// returns the name of a Fn value, as a string literal
	ثابت حرف *fn_مقصد_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class ترتیب_دو_2 {
		std::variant<صحیح, std::pair<صحیح, صحیح>> _v;
	عوامی:
		explicit ترتیب_دو_2(صحیح چوڑائی) : _v(چوڑائی) { }
		ترتیب_دو_2(صحیح addr_چوڑائی, صحیح ڈیٹا_چوڑائی) : _v(std::make_pair(addr_چوڑائی, ڈیٹا_چوڑائی)) { }
		bool is_signal() ثابت { واپس _v.اشاریہ() == 0; }
		bool is_حافظہ() ثابت { واپس _v.اشاریہ() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		صحیح چوڑائی() ثابت { واپس std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		صحیح addr_چوڑائی() ثابت { واپس std::get<1>(_v).پہلا; }
		// returns the data width of a bitvector sort, errors out for other sorts
		صحیح ڈیٹا_چوڑائی() ثابت { واپس std::get<1>(_v).دوسرا; }
		bool عامل==(ترتیب_دو_2 ثابت& other) ثابت { واپس _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(_v); واپس h; }
	};
	class IR;
	class Factory;
	class گرہ_2;
	class IRInput {
		دوست class Factory;
	عوامی:
		PooledName name;
		PooledName قسم;
		ترتیب_دو_2 ترتیب_دو;
	نجی:
		IRInput(IR &ir, IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو);
	};
	class IROutput {
		دوست class Factory;
		IR &_ir;
	عوامی:
		PooledName name;
		PooledName قسم;
		ترتیب_دو_2 ترتیب_دو;
	نجی:
		IROutput(IR &ir, IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو);
	عوامی:
		گرہ_2 قدر() ثابت;
		bool has_قدر() ثابت;
		خالی سیٹ_قدر(گرہ_2 قدر);
	};
	class IRState {
		دوست class Factory;
		IR &_ir;
	عوامی:
		PooledName name;
		PooledName قسم;
		ترتیب_دو_2 ترتیب_دو;
	نجی:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو);
	عوامی:
		گرہ_2 اگلا_قدر() ثابت;
		bool has_اگلا_قدر() ثابت;
		RTLIL::Const ثابت& initial_قدر_signal() ثابت { واپس std::get<RTLIL::Const>(_initial); }
		MemContents ثابت& initial_قدر_حافظہ() ثابت { واپس std::get<MemContents>(_initial); }
		خالی سیٹ_اگلا_قدر(گرہ_2 قدر);
		خالی سیٹ_initial_قدر(RTLIL::Const قدر) { قدر.extu(ترتیب_دو.چوڑائی()); _initial = std::move(قدر); }
		خالی سیٹ_initial_قدر(MemContents قدر) { log_assert(ترتیب_دو_2(قدر.addr_چوڑائی(), قدر.ڈیٹا_چوڑائی()) == ترتیب_دو); _initial = std::move(قدر); }
	};
	class IR {
		دوست class Factory;
		دوست class گرہ_2;
		دوست class IRInput;
		دوست class IROutput;
		دوست class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class گرہ_ڈیٹا {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				صحیح
			> _extra;
		عوامی:
			گرہ_ڈیٹا() : _fn(Fn::invalid) {}
			گرہ_ڈیٹا(Fn fn) : _fn(fn) {}
			سانچہ<class T> گرہ_ڈیٹا(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ثابت { واپس _fn; }
			ثابت RTLIL::Const &as_const() ثابت { واپس std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ثابت { واپس std::get<std::pair<IdString, IdString>>(_extra); }
			صحیح as_int() ثابت { واپس std::get<صحیح>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
				h.eat((غیرعلامتی صحیح) _fn);
				h.eat(_extra);
				واپس h;
			}
			bool عامل==(گرہ_ڈیٹا ثابت &other) ثابت {
				واپس _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		ساخت Attr {
			ترتیب_دو_2 ترتیب_دو;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using گراف = Compute_گراف<گرہ_ڈیٹا, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		گراف گراف_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> حالتیں_2;
		IR::گراف::Ref mutate(گرہ_2 n);
	عوامی:
		Design *design = nullptr;
		ساکن IR from_module(Module *module);
		Factory factory();
		صحیح سائز() ثابت { واپس گراف_4.سائز(); }
		گرہ_2 عامل[](صحیح i);
		خالی topological_ترتیب_دو();
		خالی forward_buf();
		IRInput ثابت& داخلہ(IdString name, IdString قسم) ثابت { واپس _inputs.at({name, قسم}); }
		IRInput ثابت& داخلہ(IdString name) ثابت { واپس داخلہ(name, ID($داخلہ)); }
		IROutput ثابت& output(IdString name, IdString قسم) ثابت { واپس _outputs.at({name, قسم}); }
		IROutput ثابت& output(IdString name) ثابت { واپس output(name, ID($output)); }
		IRState ثابت& state(IdString name, IdString قسم) ثابت { واپس حالتیں_2.at({name, قسم}); }
		IRState ثابت& state(IdString name) ثابت { واپس state(name, ID($state)); }
		bool has_داخلہ(IdString name, IdString قسم) ثابت { واپس _inputs.گنتی({name, قسم}); }
		bool has_output(IdString name, IdString قسم) ثابت { واپس _outputs.گنتی({name, قسم}); }
		bool has_state(IdString name, IdString قسم) ثابت { واپس حالتیں_2.گنتی({name, قسم}); }
		سمتیہ<IRInput ثابت*> inputs(IdString قسم) ثابت;
		سمتیہ<IRInput ثابت*> inputs() ثابت { واپس inputs(ID($داخلہ)); }
		سمتیہ<IROutput ثابت*> outputs(IdString قسم) ثابت;
		سمتیہ<IROutput ثابت*> outputs() ثابت { واپس outputs(ID($output)); }
		سمتیہ<IRState ثابت*> حالتیں(IdString قسم) ثابت;
		سمتیہ<IRState ثابت*> حالتیں() ثابت { واپس حالتیں(ID($state)); }
		سمتیہ<IRInput ثابت*> all_inputs() ثابت;
		سمتیہ<IROutput ثابت*> all_outputs() ثابت;
		سمتیہ<IRState ثابت*> all_حالتیں() ثابت;
		class iterator {
			دوست class IR;
			IR *_ir;
			صحیح اشاریہ_3;
			iterator(IR *ir, صحیح اشاریہ) : _ir(ir), اشاریہ_3(اشاریہ) {}
		عوامی:
			using iterator_category = std::داخلہ_iterator_لیبل;
			using قدر_type = گرہ_2;
			using اشارہ_گر = arrow_proxy<گرہ_2>;
			using reference = گرہ_2;
			using difference_type = ptrdiff_t;
			گرہ_2 عامل*();
			iterator &عامل++() { اشاریہ_3++; واپس *this; }
			bool عامل!=(iterator ثابت &other) ثابت { واپس _ir != other._ir || اشاریہ_3 != other.اشاریہ_3; }
			bool عامل==(iterator ثابت &other) ثابت { واپس !(*this != other); }
			اشارہ_گر عامل->();
		};
		iterator begin() { واپس iterator(this, 0); }
		iterator انتہا() { واپس iterator(this, گراف_4.سائز()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class گرہ_2 {
		دوست class Factory;
		دوست class IR;
		دوست class IRInput;
		دوست class IROutput;
		دوست class IRState;
		IR::گراف::ConstRef _ref;
		گرہ_2(IR::گراف::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit عامل IR::گراف::ConstRef() { واپس _ref; }
	عوامی:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		صحیح id() ثابت { واپس _ref.اشاریہ(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ثابت {
			اگر(_ref.has_sparse_attr())
				واپس {design, _ref.sparse_attr()};
			ورنہ
				واپس {design, design->twines().جوڑو(std::string("\\n") + std::مقصد_string(id()))};
		}
		Fn fn() ثابت { واپس _ref.function().fn(); }
		ترتیب_دو_2 ترتیب_دو() ثابت { واپس _ref.attr().ترتیب_دو; }
		// returns the width of a bitvector node, errors out for other nodes
		صحیح چوڑائی() ثابت { واپس ترتیب_دو().چوڑائی(); }
		سائز_t arg_گنتی() ثابت { واپس _ref.سائز(); }
		گرہ_2 arg(صحیح n) ثابت { واپس گرہ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		سانچہ<class Visitor> خودکار دورہ(Visitor v) ثابت
		{
			// currently templated but could be switched to AbstractVisitor &
			انتخاب(_ref.function().fn()) {
			حالت Fn::invalid: log_error("invalid node in visit\n"); توڑو;
			حالت Fn::buf: واپس v.buf(*this, arg(0)); توڑو;
			حالت Fn::قاش: واپس v.قاش(*this, arg(0), _ref.function().as_int(), ترتیب_دو().چوڑائی()); توڑو;
			حالت Fn::zero_extend: واپس v.zero_extend(*this, arg(0), چوڑائی()); توڑو;
			حالت Fn::sign_extend: واپس v.sign_extend(*this, arg(0), چوڑائی()); توڑو;
			حالت Fn::concat: واپس v.concat(*this, arg(0), arg(1)); توڑو;
			حالت Fn::جوڑو: واپس v.جوڑو(*this, arg(0), arg(1)); توڑو;
			حالت Fn::sub: واپس v.sub(*this, arg(0), arg(1)); توڑو;
			حالت Fn::mul: واپس v.mul(*this, arg(0), arg(1)); توڑو;
			حالت Fn::unsigned_div: واپس v.unsigned_div(*this, arg(0), arg(1)); توڑو;
			حالت Fn::unsigned_mod: واپس v.unsigned_mod(*this, arg(0), arg(1)); توڑو;
			حالت Fn::bitwise_and: واپس v.bitwise_and(*this, arg(0), arg(1)); توڑو;
			حالت Fn::bitwise_or: واپس v.bitwise_or(*this, arg(0), arg(1)); توڑو;
			حالت Fn::bitwise_xor: واپس v.bitwise_xor(*this, arg(0), arg(1)); توڑو;
			حالت Fn::bitwise_not: واپس v.bitwise_not(*this, arg(0)); توڑو;
			حالت Fn::unary_minus: واپس v.unary_minus(*this, arg(0)); توڑو;
			حالت Fn::reduce_and: واپس v.reduce_and(*this, arg(0)); توڑو;
			حالت Fn::reduce_or: واپس v.reduce_or(*this, arg(0)); توڑو;
			حالت Fn::reduce_xor: واپس v.reduce_xor(*this, arg(0)); توڑو;
			حالت Fn::equal: واپس v.equal(*this, arg(0), arg(1)); توڑو;
			حالت Fn::not_equal: واپس v.not_equal(*this, arg(0), arg(1)); توڑو;
			حالت Fn::signed_greater_than: واپس v.signed_greater_than(*this, arg(0), arg(1)); توڑو;
			حالت Fn::signed_greater_equal: واپس v.signed_greater_equal(*this, arg(0), arg(1)); توڑو;
			حالت Fn::unsigned_greater_than: واپس v.unsigned_greater_than(*this, arg(0), arg(1)); توڑو;
			حالت Fn::unsigned_greater_equal: واپس v.unsigned_greater_equal(*this, arg(0), arg(1)); توڑو;
			حالت Fn::logical_سرکاؤ_بایاں: واپس v.logical_سرکاؤ_بایاں(*this, arg(0), arg(1)); توڑو;
			حالت Fn::logical_سرکاؤ_دایاں: واپس v.logical_سرکاؤ_دایاں(*this, arg(0), arg(1)); توڑو;
			حالت Fn::arithmetic_سرکاؤ_دایاں: واپس v.arithmetic_سرکاؤ_دایاں(*this, arg(0), arg(1)); توڑو;
			حالت Fn::mux: واپس v.mux(*this, arg(0), arg(1), arg(2)); توڑو;
			حالت Fn::constant: واپس v.constant(*this, _ref.function().as_const()); توڑو;
			حالت Fn::داخلہ: واپس v.داخلہ(*this, _ref.function().as_idstring_pair().پہلا, _ref.function().as_idstring_pair().دوسرا); توڑو;
			حالت Fn::state: واپس v.state(*this, _ref.function().as_idstring_pair().پہلا, _ref.function().as_idstring_pair().دوسرا); توڑو;
			حالت Fn::حافظہ_read: واپس v.حافظہ_read(*this, arg(0), arg(1)); توڑو;
			حالت Fn::حافظہ_write: واپس v.حافظہ_write(*this, arg(0), arg(1), arg(2)); توڑو;
			}
			log_abort();
		}
		std::string مقصد_string();
		std::string مقصد_string(std::function<std::string(گرہ_2)>);
	};
	اندرونی IRInput::IRInput(IR &ir, IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو)
		: name(ir.design, name), قسم(ir.design, قسم), ترتیب_دو(std::move(ترتیب_دو)) {}
	اندرونی IROutput::IROutput(IR &ir, IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو)
		: _ir(ir), name(ir.design, name), قسم(ir.design, قسم), ترتیب_دو(std::move(ترتیب_دو)) {}
	اندرونی IRState::IRState(IR &ir, IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو)
		: _ir(ir), name(ir.design, name), قسم(ir.design, قسم), ترتیب_دو(std::move(ترتیب_دو)) {}
	اندرونی IR::گراف::Ref IR::mutate(گرہ_2 n) { واپس گراف_4[n._ref.اشاریہ()]; }
	اندرونی گرہ_2 IR::عامل[](صحیح i) { واپس گرہ_2(گراف_4[i], design); }
	اندرونی گرہ_2 IROutput::قدر() ثابت { واپس گرہ_2(_ir.گراف_4({name, قسم, false}), _ir.design); }
	اندرونی bool IROutput::has_قدر() ثابت { واپس _ir.گراف_4.has_کلید({name, قسم, false}); }
	اندرونی خالی IROutput::سیٹ_قدر(گرہ_2 قدر) { log_assert(ترتیب_دو == قدر.ترتیب_دو()); _ir.mutate(قدر).تفویض_کلید({name, قسم, false}); }
	اندرونی گرہ_2 IRState::اگلا_قدر() ثابت { واپس گرہ_2(_ir.گراف_4({name, قسم, true}), _ir.design); }
	اندرونی bool IRState::has_اگلا_قدر() ثابت { واپس _ir.گراف_4.has_کلید({name, قسم, true}); }
	اندرونی خالی IRState::سیٹ_اگلا_قدر(گرہ_2 قدر) { log_assert(ترتیب_دو == قدر.ترتیب_دو()); _ir.mutate(قدر).تفویض_کلید({name, قسم, true}); }
	اندرونی گرہ_2 IR::iterator::عامل*() { واپس گرہ_2(_ir->گراف_4[اشاریہ_3], _ir->design); }
	اندرونی arrow_proxy<گرہ_2> IR::iterator::عامل->() { واپس arrow_proxy<گرہ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	سانچہ<class T> ساخت AbstractVisitor {
		مجازی T buf(گرہ_2 self, گرہ_2 n) = 0;
		مجازی T قاش(گرہ_2 self, گرہ_2 a, صحیح انحراف, صحیح برآمد_چوڑائی) = 0;
		مجازی T zero_extend(گرہ_2 self, گرہ_2 a, صحیح برآمد_چوڑائی) = 0;
		مجازی T sign_extend(گرہ_2 self, گرہ_2 a, صحیح برآمد_چوڑائی) = 0;
		مجازی T concat(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T جوڑو(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T sub(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T mul(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T unsigned_div(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T unsigned_mod(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T bitwise_and(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T bitwise_or(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T bitwise_xor(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T bitwise_not(گرہ_2 self, گرہ_2 a) = 0;
		مجازی T unary_minus(گرہ_2 self, گرہ_2 a) = 0;
		مجازی T reduce_and(گرہ_2 self, گرہ_2 a) = 0;
		مجازی T reduce_or(گرہ_2 self, گرہ_2 a) = 0;
		مجازی T reduce_xor(گرہ_2 self, گرہ_2 a) = 0;
		مجازی T equal(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T not_equal(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T signed_greater_than(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T signed_greater_equal(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T unsigned_greater_than(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T unsigned_greater_equal(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T logical_سرکاؤ_بایاں(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T logical_سرکاؤ_دایاں(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T arithmetic_سرکاؤ_دایاں(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T mux(گرہ_2 self, گرہ_2 a, گرہ_2 b, گرہ_2 s) = 0;
		مجازی T constant(گرہ_2 self, RTLIL::Const ثابت & قدر) = 0;
		مجازی T داخلہ(گرہ_2 self, IdString name, IdString قسم) = 0;
		مجازی T state(گرہ_2 self, IdString name, IdString قسم) = 0;
		مجازی T حافظہ_read(گرہ_2 self, گرہ_2 mem, گرہ_2 addr) = 0;
		مجازی T حافظہ_write(گرہ_2 self, گرہ_2 mem, گرہ_2 addr, گرہ_2 ڈیٹا) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	سانچہ<class T> ساخت DefaultVisitor : عوامی AbstractVisitor<T> {
		مجازی T default_handler(گرہ_2 self) = 0;
		T buf(گرہ_2 self, گرہ_2) override { واپس default_handler(self); }
		T قاش(گرہ_2 self, گرہ_2, صحیح, صحیح) override { واپس default_handler(self); }
		T zero_extend(گرہ_2 self, گرہ_2, صحیح) override { واپس default_handler(self); }
		T sign_extend(گرہ_2 self, گرہ_2, صحیح) override { واپس default_handler(self); }
		T concat(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T جوڑو(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T sub(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T mul(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T unsigned_div(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T unsigned_mod(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T bitwise_and(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T bitwise_or(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T bitwise_xor(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T bitwise_not(گرہ_2 self, گرہ_2) override { واپس default_handler(self); }
		T unary_minus(گرہ_2 self, گرہ_2) override { واپس default_handler(self); }
		T reduce_and(گرہ_2 self, گرہ_2) override { واپس default_handler(self); }
		T reduce_or(گرہ_2 self, گرہ_2) override { واپس default_handler(self); }
		T reduce_xor(گرہ_2 self, گرہ_2) override { واپس default_handler(self); }
		T equal(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T not_equal(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T signed_greater_than(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T signed_greater_equal(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T unsigned_greater_than(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T unsigned_greater_equal(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T logical_سرکاؤ_بایاں(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T logical_سرکاؤ_دایاں(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T arithmetic_سرکاؤ_دایاں(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T mux(گرہ_2 self, گرہ_2, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T constant(گرہ_2 self, RTLIL::Const ثابت &) override { واپس default_handler(self); }
		T داخلہ(گرہ_2 self, IdString, IdString) override { واپس default_handler(self); }
		T state(گرہ_2 self, IdString, IdString) override { واپس default_handler(self); }
		T حافظہ_read(گرہ_2 self, گرہ_2, گرہ_2) override { واپس default_handler(self); }
		T حافظہ_write(گرہ_2 self, گرہ_2, گرہ_2, گرہ_2) override { واپس default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		دوست class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		گرہ_2 جوڑو(IR::گرہ_ڈیٹا &&fn, ترتیب_دو_2 ثابت &ترتیب_دو, std::initializer_list<گرہ_2> دلائل_2) {
			log_assert(!ترتیب_دو.is_signal() || ترتیب_دو.چوڑائی() > 0);
			log_assert(!ترتیب_دو.is_حافظہ() || (ترتیب_دو.addr_چوڑائی() > 0 && ترتیب_دو.ڈیٹا_چوڑائی() > 0));
			IR::گراف::Ref ref = _ir.گراف_4.جوڑو(std::move(fn), {std::move(ترتیب_دو)});
			برائے (خودکار arg : دلائل_2)
				ref.append_arg(IR::گراف::ConstRef(arg));
			واپس گرہ_2(ref, _ir.design);
		}
		خالی check_basic_ثنائی(گرہ_2 ثابت &a, گرہ_2 ثابت &b) { log_assert(a.ترتیب_دو().is_signal() && a.ترتیب_دو() == b.ترتیب_دو()); }
		خالی check_سرکاؤ(گرہ_2 ثابت &a, گرہ_2 ثابت &b) { log_assert(a.ترتیب_دو().is_signal() && b.ترتیب_دو().is_signal() && b.چوڑائی() == ceil_log2(a.چوڑائی())); }
		خالی check_unary(گرہ_2 ثابت &a) { log_assert(a.ترتیب_دو().is_signal()); }
	عوامی:
		IR &ir() { واپس _ir; }
		گرہ_2 قاش(گرہ_2 a, صحیح انحراف, صحیح برآمد_چوڑائی) {
			log_assert(a.ترتیب_دو().is_signal() && انحراف + برآمد_چوڑائی <= a.ترتیب_دو().چوڑائی());
			اگر(انحراف == 0 && برآمد_چوڑائی == a.چوڑائی())
				واپس a;
			واپس جوڑو(IR::گرہ_ڈیٹا(Fn::قاش, انحراف), ترتیب_دو_2(برآمد_چوڑائی), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		گرہ_2 extend(گرہ_2 a, صحیح برآمد_چوڑائی, bool is_signed) {
			صحیح اندر_چوڑائی = a.ترتیب_دو().چوڑائی();
			log_assert(a.ترتیب_دو().is_signal());
			اگر(اندر_چوڑائی == برآمد_چوڑائی)
				واپس a;
			اگر(اندر_چوڑائی > برآمد_چوڑائی)
				واپس قاش(a, 0, برآمد_چوڑائی);
			اگر(is_signed)
				واپس جوڑو(Fn::sign_extend, ترتیب_دو_2(برآمد_چوڑائی), {a});
			ورنہ
				واپس جوڑو(Fn::zero_extend, ترتیب_دو_2(برآمد_چوڑائی), {a});
		}
		گرہ_2 concat(گرہ_2 a, گرہ_2 b) {
			log_assert(a.ترتیب_دو().is_signal() && b.ترتیب_دو().is_signal());
			واپس جوڑو(Fn::concat, ترتیب_دو_2(a.ترتیب_دو().چوڑائی() + b.ترتیب_دو().چوڑائی()), {a, b});
		}
		گرہ_2 جوڑو(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::جوڑو, a.ترتیب_دو(), {a, b}); }
		گرہ_2 sub(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::sub, a.ترتیب_دو(), {a, b}); }
		گرہ_2 mul(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::mul, a.ترتیب_دو(), {a, b}); }
		گرہ_2 unsigned_div(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::unsigned_div, a.ترتیب_دو(), {a, b}); }
		گرہ_2 unsigned_mod(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::unsigned_mod, a.ترتیب_دو(), {a, b}); }
		گرہ_2 bitwise_and(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::bitwise_and, a.ترتیب_دو(), {a, b}); }
		گرہ_2 bitwise_or(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::bitwise_or, a.ترتیب_دو(), {a, b}); }
		گرہ_2 bitwise_xor(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::bitwise_xor, a.ترتیب_دو(), {a, b}); }
		گرہ_2 bitwise_not(گرہ_2 a) { check_unary(a); واپس جوڑو(Fn::bitwise_not, a.ترتیب_دو(), {a}); }
		گرہ_2 unary_minus(گرہ_2 a) { check_unary(a); واپس جوڑو(Fn::unary_minus, a.ترتیب_دو(), {a}); }
		گرہ_2 reduce_and(گرہ_2 a) {
			check_unary(a);
			اگر(a.چوڑائی() == 1)
				واپس a;
			واپس جوڑو(Fn::reduce_and, ترتیب_دو_2(1), {a});
		}
		گرہ_2 reduce_or(گرہ_2 a) {
			check_unary(a);
			اگر(a.چوڑائی() == 1)
				واپس a;
			واپس جوڑو(Fn::reduce_or, ترتیب_دو_2(1), {a});
		}
		گرہ_2 reduce_xor(گرہ_2 a) {
			check_unary(a);
			اگر(a.چوڑائی() == 1)
				واپس a;
			واپس جوڑو(Fn::reduce_xor, ترتیب_دو_2(1), {a});
		}
		گرہ_2 equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::equal, ترتیب_دو_2(1), {a, b}); }
		گرہ_2 not_equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::not_equal, ترتیب_دو_2(1), {a, b}); }
		گرہ_2 signed_greater_than(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::signed_greater_than, ترتیب_دو_2(1), {a, b}); }
		گرہ_2 signed_greater_equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::signed_greater_equal, ترتیب_دو_2(1), {a, b}); }
		گرہ_2 unsigned_greater_than(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::unsigned_greater_than, ترتیب_دو_2(1), {a, b}); }
		گرہ_2 unsigned_greater_equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); واپس جوڑو(Fn::unsigned_greater_equal, ترتیب_دو_2(1), {a, b}); }
		گرہ_2 logical_سرکاؤ_بایاں(گرہ_2 a, گرہ_2 b) { check_سرکاؤ(a, b); واپس جوڑو(Fn::logical_سرکاؤ_بایاں, a.ترتیب_دو(), {a, b}); }
		گرہ_2 logical_سرکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { check_سرکاؤ(a, b); واپس جوڑو(Fn::logical_سرکاؤ_دایاں, a.ترتیب_دو(), {a, b}); }
		گرہ_2 arithmetic_سرکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { check_سرکاؤ(a, b); واپس جوڑو(Fn::arithmetic_سرکاؤ_دایاں, a.ترتیب_دو(), {a, b}); }
		گرہ_2 mux(گرہ_2 a, گرہ_2 b, گرہ_2 s) {
			log_assert(a.ترتیب_دو().is_signal() && a.ترتیب_دو() == b.ترتیب_دو() && s.ترتیب_دو() == ترتیب_دو_2(1));
			واپس جوڑو(Fn::mux, a.ترتیب_دو(), {a, b, s});
		}
		گرہ_2 حافظہ_read(گرہ_2 mem, گرہ_2 addr) {
			log_assert(mem.ترتیب_دو().is_حافظہ() && addr.ترتیب_دو().is_signal() && mem.ترتیب_دو().addr_چوڑائی() == addr.ترتیب_دو().چوڑائی());
			واپس جوڑو(Fn::حافظہ_read, ترتیب_دو_2(mem.ترتیب_دو().ڈیٹا_چوڑائی()), {mem, addr});
		}
		گرہ_2 حافظہ_write(گرہ_2 mem, گرہ_2 addr, گرہ_2 ڈیٹا) {
			log_assert(mem.ترتیب_دو().is_حافظہ() && addr.ترتیب_دو().is_signal() && ڈیٹا.ترتیب_دو().is_signal() &&
				mem.ترتیب_دو().addr_چوڑائی() == addr.ترتیب_دو().چوڑائی() && mem.ترتیب_دو().ڈیٹا_چوڑائی() == ڈیٹا.ترتیب_دو().چوڑائی());
			واپس جوڑو(Fn::حافظہ_write, mem.ترتیب_دو(), {mem, addr, ڈیٹا});
		}
		گرہ_2 constant(RTLIL::Const قدر) {
			صحیح s = قدر.سائز();
			واپس جوڑو(IR::گرہ_ڈیٹا(Fn::constant, std::move(قدر)), ترتیب_دو_2(s), {});
		}
		گرہ_2 create_pending(صحیح چوڑائی) {
			واپس جوڑو(Fn::buf, ترتیب_دو_2(چوڑائی), {});
		}
		خالی update_pending(گرہ_2 گرہ, گرہ_2 قدر) {
			log_assert(گرہ._ref.function() == Fn::buf && گرہ._ref.سائز() == 0);
			log_assert(گرہ.ترتیب_دو() == قدر.ترتیب_دو());
			_ir.mutate(گرہ).append_arg(قدر._ref);
		}
		IRInput &جوڑو_داخلہ(IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو) {
			خودکار [it, inserted] = _ir._inputs.emplace({name, قسم}, IRInput(_ir, name, قسم, std::move(ترتیب_دو)));
			اگر (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			واپس it->دوسرا;
		}
		IROutput &جوڑو_output(IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو) {
			خودکار [it, inserted] = _ir._outputs.emplace({name, قسم}, IROutput(_ir, name, قسم, std::move(ترتیب_دو)));
			اگر (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			واپس it->دوسرا;
		}
		IRState &جوڑو_state(IdString name, IdString قسم, ترتیب_دو_2 ترتیب_دو) {
			خودکار [it, inserted] = _ir.حالتیں_2.emplace({name, قسم}, IRState(_ir, name, قسم, std::move(ترتیب_دو)));
			اگر (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			واپس it->دوسرا;
		}
		گرہ_2 قدر(IRInput ثابت& داخلہ) {
			واپس جوڑو(IR::گرہ_ڈیٹا(Fn::داخلہ, std::pair(داخلہ.name, داخلہ.قسم)), داخلہ.ترتیب_دو, {});
		}
		گرہ_2 قدر(IRState ثابت& state) {
			واپس جوڑو(IR::گرہ_ڈیٹا(Fn::state, std::pair(state.name, state.قسم)), state.ترتیب_دو, {});
		}
		خالی suggest_name(گرہ_2 گرہ, IdString name) {
			_ir.mutate(گرہ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	اندرونی Factory IR::factory() { واپس Factory(*this); }
	سانچہ<class Id> class Scope {
	محفوظ:
		حرف substitution_character = '_';
		مجازی bool is_character_legal(حرف, صحیح) = 0;
	نجی:
		pool<std::string> مستعمل_نام;
		dict<Id, std::string> _by_id;
	عوامی:
		خالی reserve(std::string name) {
			مستعمل_نام.داخل_کرو(std::move(name));
		}
		سانچہ<typename N> std::string unique_name(ثابت N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			برائے(سائز_t i = 0; i < str.سائز(); i++)
				اگر(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			اگر(مستعمل_نام.گنتی(str) == 0) {
				مستعمل_نام.داخل_کرو(str);
				واپس str;
			}
			برائے (صحیح idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::مقصد_string(idx);
				اگر(مستعمل_نام.گنتی(suffixed) == 0) {
					مستعمل_نام.داخل_کرو(suffixed);
					واپس suffixed;
				}
			}
		}
		سانچہ<typename N> std::string عامل()(Id id, ثابت N &suggestion) {
			خودکار it = _by_id.ڈھونڈو(id);
			اگر(it != _by_id.انتہا())
				واپس it->دوسرا;
			std::string str = unique_name(suggestion);
			_by_id.داخل_کرو({id, str});
			واپس str;
		}
	};
	class Writer {
		std::ostream *os;
		خالی print_impl(ثابت حرف *fmt, سمتیہ<std::function<خالی()>>& fns);
	عوامی:
		Writer(std::ostream &os) : os(&os) {}
		سانچہ<class T> Writer& عامل <<(T&& arg) { *os << std::forward<T>(arg); واپس *this; }
		سانچہ<typename... دلائل_3>
		خالی print(ثابت حرف *fmt, دلائل_3&&... دلائل_2)
		{
			سمتیہ<std::function<خالی()>> fns { [&]() { *this << دلائل_2; }... };
			print_impl(fmt, fns);
		}
		سانچہ<typename Fn, typename... دلائل_3>
		خالی print_with(Fn fn, ثابت حرف *fmt, دلائل_3&&... دلائل_2)
		{
			سمتیہ<std::function<خالی()>> fns { [&]() {
				اگر constexpr (std::is_invocable_v<Fn, دلائل_3>)
					*this << fn(دلائل_2);
				ورنہ
					*this << دلائل_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_انتہا

#اگر_ختم
