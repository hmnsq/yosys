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

#جے_بنا_تعریف FUNCTIONAL_H
#تعریف_کرو FUNCTIONAL_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/compute_graph.h"
#ضمن "kernel/drivertools.h"
#ضمن "kernel/mem.h"
#ضمن "kernel/utils.h"

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
	تعداد class Fn {
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
		ٹکڑا,
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
		logical_کھسکاؤ_بایاں,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_کھسکاؤ_دایاں,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_کھسکاؤ_دایاں,
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
	ثابت حرف *fn_منزل_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class ترتیب_دیو_2 {
		std::variant<صحيح, std::pair<صحيح, صحيح>> _v;
	عام:
		explicit ترتیب_دیو_2(صحيح چوڑائی) : _v(چوڑائی) { }
		ترتیب_دیو_2(صحيح addr_چوڑائی, صحيح انکڑے_چوڑائی) : _v(std::make_pair(addr_چوڑائی, انکڑے_چوڑائی)) { }
		bool is_signal() ثابت { أرجع _v.اشاریہ() == 0; }
		bool is_حافظہ() ثابت { أرجع _v.اشاریہ() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		صحيح چوڑائی() ثابت { أرجع std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		صحيح addr_چوڑائی() ثابت { أرجع std::get<1>(_v).پہلا; }
		// returns the data width of a bitvector sort, errors out for other sorts
		صحيح انکڑے_چوڑائی() ثابت { أرجع std::get<1>(_v).دوجا; }
		bool عامل==(ترتیب_دیو_2 ثابت& other) ثابت { أرجع _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(_v); أرجع h; }
	};
	class IR;
	class Factory;
	class گرہ_2;
	class IRInput {
		یار class Factory;
	عام:
		PooledName name;
		PooledName قسم;
		ترتیب_دیو_2 ترتیب_دیو;
	خاص:
		IRInput(IR &ir, IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو);
	};
	class IROutput {
		یار class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName قسم;
		ترتیب_دیو_2 ترتیب_دیو;
	خاص:
		IROutput(IR &ir, IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو);
	عام:
		گرہ_2 قدر() ثابت;
		bool has_قدر() ثابت;
		فراغ سیٹ_قدر(گرہ_2 قدر);
	};
	class IRState {
		یار class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName قسم;
		ترتیب_دیو_2 ترتیب_دیو;
	خاص:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو);
	عام:
		گرہ_2 اگلا_قدر() ثابت;
		bool has_اگلا_قدر() ثابت;
		RTLIL::Const ثابت& initial_قدر_signal() ثابت { أرجع std::get<RTLIL::Const>(_initial); }
		MemContents ثابت& initial_قدر_حافظہ() ثابت { أرجع std::get<MemContents>(_initial); }
		فراغ سیٹ_اگلا_قدر(گرہ_2 قدر);
		فراغ سیٹ_initial_قدر(RTLIL::Const قدر) { قدر.extu(ترتیب_دیو.چوڑائی()); _initial = std::move(قدر); }
		فراغ سیٹ_initial_قدر(MemContents قدر) { log_assert(ترتیب_دیو_2(قدر.addr_چوڑائی(), قدر.انکڑے_چوڑائی()) == ترتیب_دیو); _initial = std::move(قدر); }
	};
	class IR {
		یار class Factory;
		یار class گرہ_2;
		یار class IRInput;
		یار class IROutput;
		یار class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class گرہ_انکڑے {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				صحيح
			> _extra;
		عام:
			گرہ_انکڑے() : _fn(Fn::invalid) {}
			گرہ_انکڑے(Fn fn) : _fn(fn) {}
			سانچا<class T> گرہ_انکڑے(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ثابت { أرجع _fn; }
			ثابت RTLIL::Const &as_const() ثابت { أرجع std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ثابت { أرجع std::get<std::pair<IdString, IdString>>(_extra); }
			صحيح as_int() ثابت { أرجع std::get<صحيح>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
				h.eat((بنانشانی صحيح) _fn);
				h.eat(_extra);
				أرجع h;
			}
			bool عامل==(گرہ_انکڑے ثابت &other) ثابت {
				أرجع _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		بنية Attr {
			ترتیب_دیو_2 ترتیب_دیو;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using گراف = Compute_گراف<گرہ_انکڑے, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		گراف گراف_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> حالتاں_2;
		IR::گراف::Ref mutate(گرہ_2 n);
	عام:
		Design *design = nullptr;
		ساكن IR from_module(Module *module);
		Factory factory();
		صحيح ناپ() ثابت { أرجع گراف_4.ناپ(); }
		گرہ_2 عامل[](صحيح i);
		فراغ topological_ترتیب_دیو();
		فراغ forward_buf();
		IRInput ثابت& داخلہ(IdString name, IdString قسم) ثابت { أرجع _inputs.at({name, قسم}); }
		IRInput ثابت& داخلہ(IdString name) ثابت { أرجع داخلہ(name, ID($داخلہ)); }
		IROutput ثابت& output(IdString name, IdString قسم) ثابت { أرجع _outputs.at({name, قسم}); }
		IROutput ثابت& output(IdString name) ثابت { أرجع output(name, ID($output)); }
		IRState ثابت& state(IdString name, IdString قسم) ثابت { أرجع حالتاں_2.at({name, قسم}); }
		IRState ثابت& state(IdString name) ثابت { أرجع state(name, ID($state)); }
		bool has_داخلہ(IdString name, IdString قسم) ثابت { أرجع _inputs.گنتی({name, قسم}); }
		bool has_output(IdString name, IdString قسم) ثابت { أرجع _outputs.گنتی({name, قسم}); }
		bool has_state(IdString name, IdString قسم) ثابت { أرجع حالتاں_2.گنتی({name, قسم}); }
		سمتیہ<IRInput ثابت*> inputs(IdString قسم) ثابت;
		سمتیہ<IRInput ثابت*> inputs() ثابت { أرجع inputs(ID($داخلہ)); }
		سمتیہ<IROutput ثابت*> outputs(IdString قسم) ثابت;
		سمتیہ<IROutput ثابت*> outputs() ثابت { أرجع outputs(ID($output)); }
		سمتیہ<IRState ثابت*> حالتاں(IdString قسم) ثابت;
		سمتیہ<IRState ثابت*> حالتاں() ثابت { أرجع حالتاں(ID($state)); }
		سمتیہ<IRInput ثابت*> all_inputs() ثابت;
		سمتیہ<IROutput ثابت*> all_outputs() ثابت;
		سمتیہ<IRState ثابت*> all_حالتاں() ثابت;
		class iterator {
			یار class IR;
			IR *_ir;
			صحيح اشاریہ_3;
			iterator(IR *ir, صحيح اشاریہ) : _ir(ir), اشاریہ_3(اشاریہ) {}
		عام:
			using iterator_category = std::داخلہ_iterator_ٹیگ;
			using قدر_type = گرہ_2;
			using اشارہ_گر = arrow_proxy<گرہ_2>;
			using reference = گرہ_2;
			using difference_type = ptrdiff_t;
			گرہ_2 عامل*();
			iterator &عامل++() { اشاریہ_3++; أرجع *this; }
			bool عامل!=(iterator ثابت &other) ثابت { أرجع _ir != other._ir || اشاریہ_3 != other.اشاریہ_3; }
			bool عامل==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
			اشارہ_گر عامل->();
		};
		iterator begin() { أرجع iterator(this, 0); }
		iterator سرا() { أرجع iterator(this, گراف_4.ناپ()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class گرہ_2 {
		یار class Factory;
		یار class IR;
		یار class IRInput;
		یار class IROutput;
		یار class IRState;
		IR::گراف::ConstRef _ref;
		گرہ_2(IR::گراف::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit عامل IR::گراف::ConstRef() { أرجع _ref; }
	عام:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		صحيح id() ثابت { أرجع _ref.اشاریہ(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ثابت {
			إذا(_ref.has_sparse_attr())
				أرجع {design, _ref.sparse_attr()};
			وإلا
				أرجع {design, design->twines().جوڑو(std::string("\\n") + std::منزل_string(id()))};
		}
		Fn fn() ثابت { أرجع _ref.function().fn(); }
		ترتیب_دیو_2 ترتیب_دیو() ثابت { أرجع _ref.attr().ترتیب_دیو; }
		// returns the width of a bitvector node, errors out for other nodes
		صحيح چوڑائی() ثابت { أرجع ترتیب_دیو().چوڑائی(); }
		ناپ_t arg_گنتی() ثابت { أرجع _ref.ناپ(); }
		گرہ_2 arg(صحيح n) ثابت { أرجع گرہ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		سانچا<class Visitor> خودکار ویکھو(Visitor v) ثابت
		{
			// currently templated but could be switched to AbstractVisitor &
			بدل(_ref.function().fn()) {
			حالة Fn::invalid: log_error("invalid node in visit\n"); اكسر;
			حالة Fn::buf: أرجع v.buf(*this, arg(0)); اكسر;
			حالة Fn::ٹکڑا: أرجع v.ٹکڑا(*this, arg(0), _ref.function().as_int(), ترتیب_دیو().چوڑائی()); اكسر;
			حالة Fn::zero_extend: أرجع v.zero_extend(*this, arg(0), چوڑائی()); اكسر;
			حالة Fn::sign_extend: أرجع v.sign_extend(*this, arg(0), چوڑائی()); اكسر;
			حالة Fn::concat: أرجع v.concat(*this, arg(0), arg(1)); اكسر;
			حالة Fn::جوڑو: أرجع v.جوڑو(*this, arg(0), arg(1)); اكسر;
			حالة Fn::sub: أرجع v.sub(*this, arg(0), arg(1)); اكسر;
			حالة Fn::mul: أرجع v.mul(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_div: أرجع v.unsigned_div(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_mod: أرجع v.unsigned_mod(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_and: أرجع v.bitwise_and(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_or: أرجع v.bitwise_or(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_xor: أرجع v.bitwise_xor(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_not: أرجع v.bitwise_not(*this, arg(0)); اكسر;
			حالة Fn::unary_minus: أرجع v.unary_minus(*this, arg(0)); اكسر;
			حالة Fn::reduce_and: أرجع v.reduce_and(*this, arg(0)); اكسر;
			حالة Fn::reduce_or: أرجع v.reduce_or(*this, arg(0)); اكسر;
			حالة Fn::reduce_xor: أرجع v.reduce_xor(*this, arg(0)); اكسر;
			حالة Fn::equal: أرجع v.equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::not_equal: أرجع v.not_equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::signed_greater_than: أرجع v.signed_greater_than(*this, arg(0), arg(1)); اكسر;
			حالة Fn::signed_greater_equal: أرجع v.signed_greater_equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_greater_than: أرجع v.unsigned_greater_than(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_greater_equal: أرجع v.unsigned_greater_equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::logical_کھسکاؤ_بایاں: أرجع v.logical_کھسکاؤ_بایاں(*this, arg(0), arg(1)); اكسر;
			حالة Fn::logical_کھسکاؤ_دایاں: أرجع v.logical_کھسکاؤ_دایاں(*this, arg(0), arg(1)); اكسر;
			حالة Fn::arithmetic_کھسکاؤ_دایاں: أرجع v.arithmetic_کھسکاؤ_دایاں(*this, arg(0), arg(1)); اكسر;
			حالة Fn::mux: أرجع v.mux(*this, arg(0), arg(1), arg(2)); اكسر;
			حالة Fn::constant: أرجع v.constant(*this, _ref.function().as_const()); اكسر;
			حالة Fn::داخلہ: أرجع v.داخلہ(*this, _ref.function().as_idstring_pair().پہلا, _ref.function().as_idstring_pair().دوجا); اكسر;
			حالة Fn::state: أرجع v.state(*this, _ref.function().as_idstring_pair().پہلا, _ref.function().as_idstring_pair().دوجا); اكسر;
			حالة Fn::حافظہ_read: أرجع v.حافظہ_read(*this, arg(0), arg(1)); اكسر;
			حالة Fn::حافظہ_write: أرجع v.حافظہ_write(*this, arg(0), arg(1), arg(2)); اكسر;
			}
			log_abort();
		}
		std::string منزل_string();
		std::string منزل_string(std::function<std::string(گرہ_2)>);
	};
	مضمن IRInput::IRInput(IR &ir, IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو)
		: name(ir.design, name), قسم(ir.design, قسم), ترتیب_دیو(std::move(ترتیب_دیو)) {}
	مضمن IROutput::IROutput(IR &ir, IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو)
		: _ir(ir), name(ir.design, name), قسم(ir.design, قسم), ترتیب_دیو(std::move(ترتیب_دیو)) {}
	مضمن IRState::IRState(IR &ir, IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو)
		: _ir(ir), name(ir.design, name), قسم(ir.design, قسم), ترتیب_دیو(std::move(ترتیب_دیو)) {}
	مضمن IR::گراف::Ref IR::mutate(گرہ_2 n) { أرجع گراف_4[n._ref.اشاریہ()]; }
	مضمن گرہ_2 IR::عامل[](صحيح i) { أرجع گرہ_2(گراف_4[i], design); }
	مضمن گرہ_2 IROutput::قدر() ثابت { أرجع گرہ_2(_ir.گراف_4({name, قسم, false}), _ir.design); }
	مضمن bool IROutput::has_قدر() ثابت { أرجع _ir.گراف_4.has_کلید({name, قسم, false}); }
	مضمن فراغ IROutput::سیٹ_قدر(گرہ_2 قدر) { log_assert(ترتیب_دیو == قدر.ترتیب_دیو()); _ir.mutate(قدر).سونپو_کلید({name, قسم, false}); }
	مضمن گرہ_2 IRState::اگلا_قدر() ثابت { أرجع گرہ_2(_ir.گراف_4({name, قسم, true}), _ir.design); }
	مضمن bool IRState::has_اگلا_قدر() ثابت { أرجع _ir.گراف_4.has_کلید({name, قسم, true}); }
	مضمن فراغ IRState::سیٹ_اگلا_قدر(گرہ_2 قدر) { log_assert(ترتیب_دیو == قدر.ترتیب_دیو()); _ir.mutate(قدر).سونپو_کلید({name, قسم, true}); }
	مضمن گرہ_2 IR::iterator::عامل*() { أرجع گرہ_2(_ir->گراف_4[اشاریہ_3], _ir->design); }
	مضمن arrow_proxy<گرہ_2> IR::iterator::عامل->() { أرجع arrow_proxy<گرہ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	سانچا<class T> بنية AbstractVisitor {
		مجازی T buf(گرہ_2 self, گرہ_2 n) = 0;
		مجازی T ٹکڑا(گرہ_2 self, گرہ_2 a, صحيح وتھ, صحيح باہرلا_چوڑائی) = 0;
		مجازی T zero_extend(گرہ_2 self, گرہ_2 a, صحيح باہرلا_چوڑائی) = 0;
		مجازی T sign_extend(گرہ_2 self, گرہ_2 a, صحيح باہرلا_چوڑائی) = 0;
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
		مجازی T logical_کھسکاؤ_بایاں(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T logical_کھسکاؤ_دایاں(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T arithmetic_کھسکاؤ_دایاں(گرہ_2 self, گرہ_2 a, گرہ_2 b) = 0;
		مجازی T mux(گرہ_2 self, گرہ_2 a, گرہ_2 b, گرہ_2 s) = 0;
		مجازی T constant(گرہ_2 self, RTLIL::Const ثابت & قدر) = 0;
		مجازی T داخلہ(گرہ_2 self, IdString name, IdString قسم) = 0;
		مجازی T state(گرہ_2 self, IdString name, IdString قسم) = 0;
		مجازی T حافظہ_read(گرہ_2 self, گرہ_2 mem, گرہ_2 addr) = 0;
		مجازی T حافظہ_write(گرہ_2 self, گرہ_2 mem, گرہ_2 addr, گرہ_2 انکڑے) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	سانچا<class T> بنية DefaultVisitor : عام AbstractVisitor<T> {
		مجازی T default_handler(گرہ_2 self) = 0;
		T buf(گرہ_2 self, گرہ_2) override { أرجع default_handler(self); }
		T ٹکڑا(گرہ_2 self, گرہ_2, صحيح, صحيح) override { أرجع default_handler(self); }
		T zero_extend(گرہ_2 self, گرہ_2, صحيح) override { أرجع default_handler(self); }
		T sign_extend(گرہ_2 self, گرہ_2, صحيح) override { أرجع default_handler(self); }
		T concat(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T جوڑو(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T sub(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T mul(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T unsigned_div(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T unsigned_mod(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T bitwise_and(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T bitwise_or(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T bitwise_xor(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T bitwise_not(گرہ_2 self, گرہ_2) override { أرجع default_handler(self); }
		T unary_minus(گرہ_2 self, گرہ_2) override { أرجع default_handler(self); }
		T reduce_and(گرہ_2 self, گرہ_2) override { أرجع default_handler(self); }
		T reduce_or(گرہ_2 self, گرہ_2) override { أرجع default_handler(self); }
		T reduce_xor(گرہ_2 self, گرہ_2) override { أرجع default_handler(self); }
		T equal(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T not_equal(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T signed_greater_than(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T signed_greater_equal(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T unsigned_greater_than(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T unsigned_greater_equal(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T logical_کھسکاؤ_بایاں(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T logical_کھسکاؤ_دایاں(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T arithmetic_کھسکاؤ_دایاں(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T mux(گرہ_2 self, گرہ_2, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T constant(گرہ_2 self, RTLIL::Const ثابت &) override { أرجع default_handler(self); }
		T داخلہ(گرہ_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T state(گرہ_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T حافظہ_read(گرہ_2 self, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
		T حافظہ_write(گرہ_2 self, گرہ_2, گرہ_2, گرہ_2) override { أرجع default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		یار class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		گرہ_2 جوڑو(IR::گرہ_انکڑے &&fn, ترتیب_دیو_2 ثابت &ترتیب_دیو, std::initializer_list<گرہ_2> دلیلاں_2) {
			log_assert(!ترتیب_دیو.is_signal() || ترتیب_دیو.چوڑائی() > 0);
			log_assert(!ترتیب_دیو.is_حافظہ() || (ترتیب_دیو.addr_چوڑائی() > 0 && ترتیب_دیو.انکڑے_چوڑائی() > 0));
			IR::گراف::Ref ref = _ir.گراف_4.جوڑو(std::move(fn), {std::move(ترتیب_دیو)});
			لكل (خودکار arg : دلیلاں_2)
				ref.append_arg(IR::گراف::ConstRef(arg));
			أرجع گرہ_2(ref, _ir.design);
		}
		فراغ check_basic_ثنائی(گرہ_2 ثابت &a, گرہ_2 ثابت &b) { log_assert(a.ترتیب_دیو().is_signal() && a.ترتیب_دیو() == b.ترتیب_دیو()); }
		فراغ check_کھسکاؤ(گرہ_2 ثابت &a, گرہ_2 ثابت &b) { log_assert(a.ترتیب_دیو().is_signal() && b.ترتیب_دیو().is_signal() && b.چوڑائی() == ceil_log2(a.چوڑائی())); }
		فراغ check_unary(گرہ_2 ثابت &a) { log_assert(a.ترتیب_دیو().is_signal()); }
	عام:
		IR &ir() { أرجع _ir; }
		گرہ_2 ٹکڑا(گرہ_2 a, صحيح وتھ, صحيح باہرلا_چوڑائی) {
			log_assert(a.ترتیب_دیو().is_signal() && وتھ + باہرلا_چوڑائی <= a.ترتیب_دیو().چوڑائی());
			إذا(وتھ == 0 && باہرلا_چوڑائی == a.چوڑائی())
				أرجع a;
			أرجع جوڑو(IR::گرہ_انکڑے(Fn::ٹکڑا, وتھ), ترتیب_دیو_2(باہرلا_چوڑائی), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		گرہ_2 extend(گرہ_2 a, صحيح باہرلا_چوڑائی, bool is_signed) {
			صحيح وچ_چوڑائی = a.ترتیب_دیو().چوڑائی();
			log_assert(a.ترتیب_دیو().is_signal());
			إذا(وچ_چوڑائی == باہرلا_چوڑائی)
				أرجع a;
			إذا(وچ_چوڑائی > باہرلا_چوڑائی)
				أرجع ٹکڑا(a, 0, باہرلا_چوڑائی);
			إذا(is_signed)
				أرجع جوڑو(Fn::sign_extend, ترتیب_دیو_2(باہرلا_چوڑائی), {a});
			وإلا
				أرجع جوڑو(Fn::zero_extend, ترتیب_دیو_2(باہرلا_چوڑائی), {a});
		}
		گرہ_2 concat(گرہ_2 a, گرہ_2 b) {
			log_assert(a.ترتیب_دیو().is_signal() && b.ترتیب_دیو().is_signal());
			أرجع جوڑو(Fn::concat, ترتیب_دیو_2(a.ترتیب_دیو().چوڑائی() + b.ترتیب_دیو().چوڑائی()), {a, b});
		}
		گرہ_2 جوڑو(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::جوڑو, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 sub(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::sub, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 mul(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::mul, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 unsigned_div(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::unsigned_div, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 unsigned_mod(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::unsigned_mod, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 bitwise_and(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::bitwise_and, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 bitwise_or(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::bitwise_or, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 bitwise_xor(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::bitwise_xor, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 bitwise_not(گرہ_2 a) { check_unary(a); أرجع جوڑو(Fn::bitwise_not, a.ترتیب_دیو(), {a}); }
		گرہ_2 unary_minus(گرہ_2 a) { check_unary(a); أرجع جوڑو(Fn::unary_minus, a.ترتیب_دیو(), {a}); }
		گرہ_2 reduce_and(گرہ_2 a) {
			check_unary(a);
			إذا(a.چوڑائی() == 1)
				أرجع a;
			أرجع جوڑو(Fn::reduce_and, ترتیب_دیو_2(1), {a});
		}
		گرہ_2 reduce_or(گرہ_2 a) {
			check_unary(a);
			إذا(a.چوڑائی() == 1)
				أرجع a;
			أرجع جوڑو(Fn::reduce_or, ترتیب_دیو_2(1), {a});
		}
		گرہ_2 reduce_xor(گرہ_2 a) {
			check_unary(a);
			إذا(a.چوڑائی() == 1)
				أرجع a;
			أرجع جوڑو(Fn::reduce_xor, ترتیب_دیو_2(1), {a});
		}
		گرہ_2 equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::equal, ترتیب_دیو_2(1), {a, b}); }
		گرہ_2 not_equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::not_equal, ترتیب_دیو_2(1), {a, b}); }
		گرہ_2 signed_greater_than(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::signed_greater_than, ترتیب_دیو_2(1), {a, b}); }
		گرہ_2 signed_greater_equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::signed_greater_equal, ترتیب_دیو_2(1), {a, b}); }
		گرہ_2 unsigned_greater_than(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::unsigned_greater_than, ترتیب_دیو_2(1), {a, b}); }
		گرہ_2 unsigned_greater_equal(گرہ_2 a, گرہ_2 b) { check_basic_ثنائی(a, b); أرجع جوڑو(Fn::unsigned_greater_equal, ترتیب_دیو_2(1), {a, b}); }
		گرہ_2 logical_کھسکاؤ_بایاں(گرہ_2 a, گرہ_2 b) { check_کھسکاؤ(a, b); أرجع جوڑو(Fn::logical_کھسکاؤ_بایاں, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 logical_کھسکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { check_کھسکاؤ(a, b); أرجع جوڑو(Fn::logical_کھسکاؤ_دایاں, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 arithmetic_کھسکاؤ_دایاں(گرہ_2 a, گرہ_2 b) { check_کھسکاؤ(a, b); أرجع جوڑو(Fn::arithmetic_کھسکاؤ_دایاں, a.ترتیب_دیو(), {a, b}); }
		گرہ_2 mux(گرہ_2 a, گرہ_2 b, گرہ_2 s) {
			log_assert(a.ترتیب_دیو().is_signal() && a.ترتیب_دیو() == b.ترتیب_دیو() && s.ترتیب_دیو() == ترتیب_دیو_2(1));
			أرجع جوڑو(Fn::mux, a.ترتیب_دیو(), {a, b, s});
		}
		گرہ_2 حافظہ_read(گرہ_2 mem, گرہ_2 addr) {
			log_assert(mem.ترتیب_دیو().is_حافظہ() && addr.ترتیب_دیو().is_signal() && mem.ترتیب_دیو().addr_چوڑائی() == addr.ترتیب_دیو().چوڑائی());
			أرجع جوڑو(Fn::حافظہ_read, ترتیب_دیو_2(mem.ترتیب_دیو().انکڑے_چوڑائی()), {mem, addr});
		}
		گرہ_2 حافظہ_write(گرہ_2 mem, گرہ_2 addr, گرہ_2 انکڑے) {
			log_assert(mem.ترتیب_دیو().is_حافظہ() && addr.ترتیب_دیو().is_signal() && انکڑے.ترتیب_دیو().is_signal() &&
				mem.ترتیب_دیو().addr_چوڑائی() == addr.ترتیب_دیو().چوڑائی() && mem.ترتیب_دیو().انکڑے_چوڑائی() == انکڑے.ترتیب_دیو().چوڑائی());
			أرجع جوڑو(Fn::حافظہ_write, mem.ترتیب_دیو(), {mem, addr, انکڑے});
		}
		گرہ_2 constant(RTLIL::Const قدر) {
			صحيح s = قدر.ناپ();
			أرجع جوڑو(IR::گرہ_انکڑے(Fn::constant, std::move(قدر)), ترتیب_دیو_2(s), {});
		}
		گرہ_2 create_pending(صحيح چوڑائی) {
			أرجع جوڑو(Fn::buf, ترتیب_دیو_2(چوڑائی), {});
		}
		فراغ update_pending(گرہ_2 گرہ, گرہ_2 قدر) {
			log_assert(گرہ._ref.function() == Fn::buf && گرہ._ref.ناپ() == 0);
			log_assert(گرہ.ترتیب_دیو() == قدر.ترتیب_دیو());
			_ir.mutate(گرہ).append_arg(قدر._ref);
		}
		IRInput &جوڑو_داخلہ(IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو) {
			خودکار [it, inserted] = _ir._inputs.emplace({name, قسم}, IRInput(_ir, name, قسم, std::move(ترتیب_دیو)));
			إذا (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دوجا;
		}
		IROutput &جوڑو_output(IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو) {
			خودکار [it, inserted] = _ir._outputs.emplace({name, قسم}, IROutput(_ir, name, قسم, std::move(ترتیب_دیو)));
			إذا (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دوجا;
		}
		IRState &جوڑو_state(IdString name, IdString قسم, ترتیب_دیو_2 ترتیب_دیو) {
			خودکار [it, inserted] = _ir.حالتاں_2.emplace({name, قسم}, IRState(_ir, name, قسم, std::move(ترتیب_دیو)));
			إذا (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دوجا;
		}
		گرہ_2 قدر(IRInput ثابت& داخلہ) {
			أرجع جوڑو(IR::گرہ_انکڑے(Fn::داخلہ, std::pair(داخلہ.name, داخلہ.قسم)), داخلہ.ترتیب_دیو, {});
		}
		گرہ_2 قدر(IRState ثابت& state) {
			أرجع جوڑو(IR::گرہ_انکڑے(Fn::state, std::pair(state.name, state.قسم)), state.ترتیب_دیو, {});
		}
		فراغ suggest_name(گرہ_2 گرہ, IdString name) {
			_ir.mutate(گرہ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	مضمن Factory IR::factory() { أرجع Factory(*this); }
	سانچا<class Id> class Scope {
	محمي:
		حرف substitution_character = '_';
		مجازی bool is_character_legal(حرف, صحيح) = 0;
	خاص:
		pool<std::string> ورتیا_ناں;
		dict<Id, std::string> _by_id;
	عام:
		فراغ reserve(std::string name) {
			ورتیا_ناں.داخل_کرو(std::move(name));
		}
		سانچا<typename N> std::string unique_name(ثابت N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			لكل(ناپ_t i = 0; i < str.ناپ(); i++)
				إذا(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			إذا(ورتیا_ناں.گنتی(str) == 0) {
				ورتیا_ناں.داخل_کرو(str);
				أرجع str;
			}
			لكل (صحيح idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::منزل_string(idx);
				إذا(ورتیا_ناں.گنتی(suffixed) == 0) {
					ورتیا_ناں.داخل_کرو(suffixed);
					أرجع suffixed;
				}
			}
		}
		سانچا<typename N> std::string عامل()(Id id, ثابت N &suggestion) {
			خودکار it = _by_id.لبھو(id);
			إذا(it != _by_id.سرا())
				أرجع it->دوجا;
			std::string str = unique_name(suggestion);
			_by_id.داخل_کرو({id, str});
			أرجع str;
		}
	};
	class Writer {
		std::ostream *os;
		فراغ print_impl(ثابت حرف *fmt, سمتیہ<std::function<فراغ()>>& fns);
	عام:
		Writer(std::ostream &os) : os(&os) {}
		سانچا<class T> Writer& عامل <<(T&& arg) { *os << std::forward<T>(arg); أرجع *this; }
		سانچا<typename... دلیلاں_3>
		فراغ print(ثابت حرف *fmt, دلیلاں_3&&... دلیلاں_2)
		{
			سمتیہ<std::function<فراغ()>> fns { [&]() { *this << دلیلاں_2; }... };
			print_impl(fmt, fns);
		}
		سانچا<typename Fn, typename... دلیلاں_3>
		فراغ print_with(Fn fn, ثابت حرف *fmt, دلیلاں_3&&... دلیلاں_2)
		{
			سمتیہ<std::function<فراغ()>> fns { [&]() {
				إذا constexpr (std::is_invocable_v<Fn, دلیلاں_3>)
					*this << fn(دلیلاں_2);
				وإلا
					*this << دلیلاں_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_سرا

#جے_انت
