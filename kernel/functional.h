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

#جيڪڏهن_وصف_نه_ٿيل FUNCTIONAL_H
#وصف_ڪريو FUNCTIONAL_H

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
		ٽڪرو,
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
		جوڙيو,
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
		logical_سرڪايو_کاٻو,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_سرڪايو_ساڄو,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_سرڪايو_ساڄو,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		داخلا,
		// state(a: IdString): any
		// returns the current value of the state variable with the specified name
		state,
		// memory_read(memory: memory[addr_width, data_width], addr: bit[addr_width]): bit[data_width] = memory[addr]
		يادگيري_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		يادگيري_write
	};
	// returns the name of a Fn value, as a string literal
	ثابت حرف *fn_منزل_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class ترتيب_ڏيو_2 {
		std::variant<صحيح, std::pair<صحيح, صحيح>> _v;
	عام:
		explicit ترتيب_ڏيو_2(صحيح ويڪر) : _v(ويڪر) { }
		ترتيب_ڏيو_2(صحيح addr_ويڪر, صحيح معلومات_ويڪر) : _v(std::make_pair(addr_ويڪر, معلومات_ويڪر)) { }
		bool is_signal() ثابت { أرجع _v.اشاريو() == 0; }
		bool is_يادگيري() ثابت { أرجع _v.اشاريو() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		صحيح ويڪر() ثابت { أرجع std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		صحيح addr_ويڪر() ثابت { أرجع std::get<1>(_v).پهريون; }
		// returns the data width of a bitvector sort, errors out for other sorts
		صحيح معلومات_ويڪر() ثابت { أرجع std::get<1>(_v).ٻيو; }
		bool عامل==(ترتيب_ڏيو_2 ثابت& other) ثابت { أرجع _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(_v); أرجع h; }
	};
	class IR;
	class Factory;
	class ڳنڍ_2;
	class IRInput {
		دوست class Factory;
	عام:
		PooledName name;
		PooledName قسم;
		ترتيب_ڏيو_2 ترتيب_ڏيو;
	خاص:
		IRInput(IR &ir, IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو);
	};
	class IROutput {
		دوست class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName قسم;
		ترتيب_ڏيو_2 ترتيب_ڏيو;
	خاص:
		IROutput(IR &ir, IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو);
	عام:
		ڳنڍ_2 قدر() ثابت;
		bool has_قدر() ثابت;
		فراغ سيٽ_قدر(ڳنڍ_2 قدر);
	};
	class IRState {
		دوست class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName قسم;
		ترتيب_ڏيو_2 ترتيب_ڏيو;
	خاص:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو);
	عام:
		ڳنڍ_2 ايندڙ_قدر() ثابت;
		bool has_ايندڙ_قدر() ثابت;
		RTLIL::Const ثابت& initial_قدر_signal() ثابت { أرجع std::get<RTLIL::Const>(_initial); }
		MemContents ثابت& initial_قدر_يادگيري() ثابت { أرجع std::get<MemContents>(_initial); }
		فراغ سيٽ_ايندڙ_قدر(ڳنڍ_2 قدر);
		فراغ سيٽ_initial_قدر(RTLIL::Const قدر) { قدر.extu(ترتيب_ڏيو.ويڪر()); _initial = std::move(قدر); }
		فراغ سيٽ_initial_قدر(MemContents قدر) { log_assert(ترتيب_ڏيو_2(قدر.addr_ويڪر(), قدر.معلومات_ويڪر()) == ترتيب_ڏيو); _initial = std::move(قدر); }
	};
	class IR {
		دوست class Factory;
		دوست class ڳنڍ_2;
		دوست class IRInput;
		دوست class IROutput;
		دوست class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ڳنڍ_معلومات {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				صحيح
			> _extra;
		عام:
			ڳنڍ_معلومات() : _fn(Fn::invalid) {}
			ڳنڍ_معلومات(Fn fn) : _fn(fn) {}
			سانچو<class T> ڳنڍ_معلومات(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ثابت { أرجع _fn; }
			ثابت RTLIL::Const &as_const() ثابت { أرجع std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ثابت { أرجع std::get<std::pair<IdString, IdString>>(_extra); }
			صحيح as_int() ثابت { أرجع std::get<صحيح>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
				h.eat((بنانشان صحيح) _fn);
				h.eat(_extra);
				أرجع h;
			}
			bool عامل==(ڳنڍ_معلومات ثابت &other) ثابت {
				أرجع _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		بنية Attr {
			ترتيب_ڏيو_2 ترتيب_ڏيو;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using گراف = Compute_گراف<ڳنڍ_معلومات, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		گراف گراف_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> حالتون_2;
		IR::گراف::Ref mutate(ڳنڍ_2 n);
	عام:
		Design *design = nullptr;
		ساكن IR from_module(Module *module);
		Factory factory();
		صحيح سائيز() ثابت { أرجع گراف_4.سائيز(); }
		ڳنڍ_2 عامل[](صحيح i);
		فراغ topological_ترتيب_ڏيو();
		فراغ forward_buf();
		IRInput ثابت& داخلا(IdString name, IdString قسم) ثابت { أرجع _inputs.at({name, قسم}); }
		IRInput ثابت& داخلا(IdString name) ثابت { أرجع داخلا(name, ID($داخلا)); }
		IROutput ثابت& output(IdString name, IdString قسم) ثابت { أرجع _outputs.at({name, قسم}); }
		IROutput ثابت& output(IdString name) ثابت { أرجع output(name, ID($output)); }
		IRState ثابت& state(IdString name, IdString قسم) ثابت { أرجع حالتون_2.at({name, قسم}); }
		IRState ثابت& state(IdString name) ثابت { أرجع state(name, ID($state)); }
		bool has_داخلا(IdString name, IdString قسم) ثابت { أرجع _inputs.ڳڻپ({name, قسم}); }
		bool has_output(IdString name, IdString قسم) ثابت { أرجع _outputs.ڳڻپ({name, قسم}); }
		bool has_state(IdString name, IdString قسم) ثابت { أرجع حالتون_2.ڳڻپ({name, قسم}); }
		ويڪٽر<IRInput ثابت*> inputs(IdString قسم) ثابت;
		ويڪٽر<IRInput ثابت*> inputs() ثابت { أرجع inputs(ID($داخلا)); }
		ويڪٽر<IROutput ثابت*> outputs(IdString قسم) ثابت;
		ويڪٽر<IROutput ثابت*> outputs() ثابت { أرجع outputs(ID($output)); }
		ويڪٽر<IRState ثابت*> حالتون(IdString قسم) ثابت;
		ويڪٽر<IRState ثابت*> حالتون() ثابت { أرجع حالتون(ID($state)); }
		ويڪٽر<IRInput ثابت*> all_inputs() ثابت;
		ويڪٽر<IROutput ثابت*> all_outputs() ثابت;
		ويڪٽر<IRState ثابت*> all_حالتون() ثابت;
		class iterator {
			دوست class IR;
			IR *_ir;
			صحيح اشاريو_3;
			iterator(IR *ir, صحيح اشاريو) : _ir(ir), اشاريو_3(اشاريو) {}
		عام:
			using iterator_category = std::داخلا_iterator_ٽيگ;
			using قدر_type = ڳنڍ_2;
			using اشارو = arrow_proxy<ڳنڍ_2>;
			using reference = ڳنڍ_2;
			using difference_type = ptrdiff_t;
			ڳنڍ_2 عامل*();
			iterator &عامل++() { اشاريو_3++; أرجع *this; }
			bool عامل!=(iterator ثابت &other) ثابت { أرجع _ir != other._ir || اشاريو_3 != other.اشاريو_3; }
			bool عامل==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
			اشارو عامل->();
		};
		iterator begin() { أرجع iterator(this, 0); }
		iterator پڇاڙي() { أرجع iterator(this, گراف_4.سائيز()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ڳنڍ_2 {
		دوست class Factory;
		دوست class IR;
		دوست class IRInput;
		دوست class IROutput;
		دوست class IRState;
		IR::گراف::ConstRef _ref;
		ڳنڍ_2(IR::گراف::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit عامل IR::گراف::ConstRef() { أرجع _ref; }
	عام:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		صحيح id() ثابت { أرجع _ref.اشاريو(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ثابت {
			إذا(_ref.has_sparse_attr())
				أرجع {design, _ref.sparse_attr()};
			وإلا
				أرجع {design, design->twines().جوڙيو(std::string("\\n") + std::منزل_string(id()))};
		}
		Fn fn() ثابت { أرجع _ref.function().fn(); }
		ترتيب_ڏيو_2 ترتيب_ڏيو() ثابت { أرجع _ref.attr().ترتيب_ڏيو; }
		// returns the width of a bitvector node, errors out for other nodes
		صحيح ويڪر() ثابت { أرجع ترتيب_ڏيو().ويڪر(); }
		سائيز_t arg_ڳڻپ() ثابت { أرجع _ref.سائيز(); }
		ڳنڍ_2 arg(صحيح n) ثابت { أرجع ڳنڍ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		سانچو<class Visitor> خودڪار گهمو(Visitor v) ثابت
		{
			// currently templated but could be switched to AbstractVisitor &
			بدل(_ref.function().fn()) {
			حالة Fn::invalid: log_error("invalid node in visit\n"); اكسر;
			حالة Fn::buf: أرجع v.buf(*this, arg(0)); اكسر;
			حالة Fn::ٽڪرو: أرجع v.ٽڪرو(*this, arg(0), _ref.function().as_int(), ترتيب_ڏيو().ويڪر()); اكسر;
			حالة Fn::zero_extend: أرجع v.zero_extend(*this, arg(0), ويڪر()); اكسر;
			حالة Fn::sign_extend: أرجع v.sign_extend(*this, arg(0), ويڪر()); اكسر;
			حالة Fn::concat: أرجع v.concat(*this, arg(0), arg(1)); اكسر;
			حالة Fn::جوڙيو: أرجع v.جوڙيو(*this, arg(0), arg(1)); اكسر;
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
			حالة Fn::logical_سرڪايو_کاٻو: أرجع v.logical_سرڪايو_کاٻو(*this, arg(0), arg(1)); اكسر;
			حالة Fn::logical_سرڪايو_ساڄو: أرجع v.logical_سرڪايو_ساڄو(*this, arg(0), arg(1)); اكسر;
			حالة Fn::arithmetic_سرڪايو_ساڄو: أرجع v.arithmetic_سرڪايو_ساڄو(*this, arg(0), arg(1)); اكسر;
			حالة Fn::mux: أرجع v.mux(*this, arg(0), arg(1), arg(2)); اكسر;
			حالة Fn::constant: أرجع v.constant(*this, _ref.function().as_const()); اكسر;
			حالة Fn::داخلا: أرجع v.داخلا(*this, _ref.function().as_idstring_pair().پهريون, _ref.function().as_idstring_pair().ٻيو); اكسر;
			حالة Fn::state: أرجع v.state(*this, _ref.function().as_idstring_pair().پهريون, _ref.function().as_idstring_pair().ٻيو); اكسر;
			حالة Fn::يادگيري_read: أرجع v.يادگيري_read(*this, arg(0), arg(1)); اكسر;
			حالة Fn::يادگيري_write: أرجع v.يادگيري_write(*this, arg(0), arg(1), arg(2)); اكسر;
			}
			log_abort();
		}
		std::string منزل_string();
		std::string منزل_string(std::function<std::string(ڳنڍ_2)>);
	};
	مضمن IRInput::IRInput(IR &ir, IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو)
		: name(ir.design, name), قسم(ir.design, قسم), ترتيب_ڏيو(std::move(ترتيب_ڏيو)) {}
	مضمن IROutput::IROutput(IR &ir, IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو)
		: _ir(ir), name(ir.design, name), قسم(ir.design, قسم), ترتيب_ڏيو(std::move(ترتيب_ڏيو)) {}
	مضمن IRState::IRState(IR &ir, IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو)
		: _ir(ir), name(ir.design, name), قسم(ir.design, قسم), ترتيب_ڏيو(std::move(ترتيب_ڏيو)) {}
	مضمن IR::گراف::Ref IR::mutate(ڳنڍ_2 n) { أرجع گراف_4[n._ref.اشاريو()]; }
	مضمن ڳنڍ_2 IR::عامل[](صحيح i) { أرجع ڳنڍ_2(گراف_4[i], design); }
	مضمن ڳنڍ_2 IROutput::قدر() ثابت { أرجع ڳنڍ_2(_ir.گراف_4({name, قسم, false}), _ir.design); }
	مضمن bool IROutput::has_قدر() ثابت { أرجع _ir.گراف_4.has_ڪنجي({name, قسم, false}); }
	مضمن فراغ IROutput::سيٽ_قدر(ڳنڍ_2 قدر) { log_assert(ترتيب_ڏيو == قدر.ترتيب_ڏيو()); _ir.mutate(قدر).سونپيو_ڪنجي({name, قسم, false}); }
	مضمن ڳنڍ_2 IRState::ايندڙ_قدر() ثابت { أرجع ڳنڍ_2(_ir.گراف_4({name, قسم, true}), _ir.design); }
	مضمن bool IRState::has_ايندڙ_قدر() ثابت { أرجع _ir.گراف_4.has_ڪنجي({name, قسم, true}); }
	مضمن فراغ IRState::سيٽ_ايندڙ_قدر(ڳنڍ_2 قدر) { log_assert(ترتيب_ڏيو == قدر.ترتيب_ڏيو()); _ir.mutate(قدر).سونپيو_ڪنجي({name, قسم, true}); }
	مضمن ڳنڍ_2 IR::iterator::عامل*() { أرجع ڳنڍ_2(_ir->گراف_4[اشاريو_3], _ir->design); }
	مضمن arrow_proxy<ڳنڍ_2> IR::iterator::عامل->() { أرجع arrow_proxy<ڳنڍ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	سانچو<class T> بنية AbstractVisitor {
		مجازي T buf(ڳنڍ_2 self, ڳنڍ_2 n) = 0;
		مجازي T ٽڪرو(ڳنڍ_2 self, ڳنڍ_2 a, صحيح وٿي, صحيح ٻاهريون_ويڪر) = 0;
		مجازي T zero_extend(ڳنڍ_2 self, ڳنڍ_2 a, صحيح ٻاهريون_ويڪر) = 0;
		مجازي T sign_extend(ڳنڍ_2 self, ڳنڍ_2 a, صحيح ٻاهريون_ويڪر) = 0;
		مجازي T concat(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T جوڙيو(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T sub(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T mul(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T unsigned_div(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T unsigned_mod(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T bitwise_and(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T bitwise_or(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T bitwise_xor(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T bitwise_not(ڳنڍ_2 self, ڳنڍ_2 a) = 0;
		مجازي T unary_minus(ڳنڍ_2 self, ڳنڍ_2 a) = 0;
		مجازي T reduce_and(ڳنڍ_2 self, ڳنڍ_2 a) = 0;
		مجازي T reduce_or(ڳنڍ_2 self, ڳنڍ_2 a) = 0;
		مجازي T reduce_xor(ڳنڍ_2 self, ڳنڍ_2 a) = 0;
		مجازي T equal(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T not_equal(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T signed_greater_than(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T signed_greater_equal(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T unsigned_greater_than(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T unsigned_greater_equal(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T logical_سرڪايو_کاٻو(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T logical_سرڪايو_ساڄو(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T arithmetic_سرڪايو_ساڄو(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b) = 0;
		مجازي T mux(ڳنڍ_2 self, ڳنڍ_2 a, ڳنڍ_2 b, ڳنڍ_2 s) = 0;
		مجازي T constant(ڳنڍ_2 self, RTLIL::Const ثابت & قدر) = 0;
		مجازي T داخلا(ڳنڍ_2 self, IdString name, IdString قسم) = 0;
		مجازي T state(ڳنڍ_2 self, IdString name, IdString قسم) = 0;
		مجازي T يادگيري_read(ڳنڍ_2 self, ڳنڍ_2 mem, ڳنڍ_2 addr) = 0;
		مجازي T يادگيري_write(ڳنڍ_2 self, ڳنڍ_2 mem, ڳنڍ_2 addr, ڳنڍ_2 معلومات) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	سانچو<class T> بنية DefaultVisitor : عام AbstractVisitor<T> {
		مجازي T default_handler(ڳنڍ_2 self) = 0;
		T buf(ڳنڍ_2 self, ڳنڍ_2) override { أرجع default_handler(self); }
		T ٽڪرو(ڳنڍ_2 self, ڳنڍ_2, صحيح, صحيح) override { أرجع default_handler(self); }
		T zero_extend(ڳنڍ_2 self, ڳنڍ_2, صحيح) override { أرجع default_handler(self); }
		T sign_extend(ڳنڍ_2 self, ڳنڍ_2, صحيح) override { أرجع default_handler(self); }
		T concat(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T جوڙيو(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T sub(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T mul(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T unsigned_div(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T unsigned_mod(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T bitwise_and(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T bitwise_or(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T bitwise_xor(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T bitwise_not(ڳنڍ_2 self, ڳنڍ_2) override { أرجع default_handler(self); }
		T unary_minus(ڳنڍ_2 self, ڳنڍ_2) override { أرجع default_handler(self); }
		T reduce_and(ڳنڍ_2 self, ڳنڍ_2) override { أرجع default_handler(self); }
		T reduce_or(ڳنڍ_2 self, ڳنڍ_2) override { أرجع default_handler(self); }
		T reduce_xor(ڳنڍ_2 self, ڳنڍ_2) override { أرجع default_handler(self); }
		T equal(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T not_equal(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T signed_greater_than(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T signed_greater_equal(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T unsigned_greater_than(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T unsigned_greater_equal(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T logical_سرڪايو_کاٻو(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T logical_سرڪايو_ساڄو(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T arithmetic_سرڪايو_ساڄو(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T mux(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T constant(ڳنڍ_2 self, RTLIL::Const ثابت &) override { أرجع default_handler(self); }
		T داخلا(ڳنڍ_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T state(ڳنڍ_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T يادگيري_read(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
		T يادگيري_write(ڳنڍ_2 self, ڳنڍ_2, ڳنڍ_2, ڳنڍ_2) override { أرجع default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		دوست class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ڳنڍ_2 جوڙيو(IR::ڳنڍ_معلومات &&fn, ترتيب_ڏيو_2 ثابت &ترتيب_ڏيو, std::initializer_list<ڳنڍ_2> دليلون_2) {
			log_assert(!ترتيب_ڏيو.is_signal() || ترتيب_ڏيو.ويڪر() > 0);
			log_assert(!ترتيب_ڏيو.is_يادگيري() || (ترتيب_ڏيو.addr_ويڪر() > 0 && ترتيب_ڏيو.معلومات_ويڪر() > 0));
			IR::گراف::Ref ref = _ir.گراف_4.جوڙيو(std::move(fn), {std::move(ترتيب_ڏيو)});
			لكل (خودڪار arg : دليلون_2)
				ref.append_arg(IR::گراف::ConstRef(arg));
			أرجع ڳنڍ_2(ref, _ir.design);
		}
		فراغ check_basic_ٻٽو(ڳنڍ_2 ثابت &a, ڳنڍ_2 ثابت &b) { log_assert(a.ترتيب_ڏيو().is_signal() && a.ترتيب_ڏيو() == b.ترتيب_ڏيو()); }
		فراغ check_سرڪايو(ڳنڍ_2 ثابت &a, ڳنڍ_2 ثابت &b) { log_assert(a.ترتيب_ڏيو().is_signal() && b.ترتيب_ڏيو().is_signal() && b.ويڪر() == ceil_log2(a.ويڪر())); }
		فراغ check_unary(ڳنڍ_2 ثابت &a) { log_assert(a.ترتيب_ڏيو().is_signal()); }
	عام:
		IR &ir() { أرجع _ir; }
		ڳنڍ_2 ٽڪرو(ڳنڍ_2 a, صحيح وٿي, صحيح ٻاهريون_ويڪر) {
			log_assert(a.ترتيب_ڏيو().is_signal() && وٿي + ٻاهريون_ويڪر <= a.ترتيب_ڏيو().ويڪر());
			إذا(وٿي == 0 && ٻاهريون_ويڪر == a.ويڪر())
				أرجع a;
			أرجع جوڙيو(IR::ڳنڍ_معلومات(Fn::ٽڪرو, وٿي), ترتيب_ڏيو_2(ٻاهريون_ويڪر), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ڳنڍ_2 extend(ڳنڍ_2 a, صحيح ٻاهريون_ويڪر, bool is_signed) {
			صحيح اندريون_ويڪر = a.ترتيب_ڏيو().ويڪر();
			log_assert(a.ترتيب_ڏيو().is_signal());
			إذا(اندريون_ويڪر == ٻاهريون_ويڪر)
				أرجع a;
			إذا(اندريون_ويڪر > ٻاهريون_ويڪر)
				أرجع ٽڪرو(a, 0, ٻاهريون_ويڪر);
			إذا(is_signed)
				أرجع جوڙيو(Fn::sign_extend, ترتيب_ڏيو_2(ٻاهريون_ويڪر), {a});
			وإلا
				أرجع جوڙيو(Fn::zero_extend, ترتيب_ڏيو_2(ٻاهريون_ويڪر), {a});
		}
		ڳنڍ_2 concat(ڳنڍ_2 a, ڳنڍ_2 b) {
			log_assert(a.ترتيب_ڏيو().is_signal() && b.ترتيب_ڏيو().is_signal());
			أرجع جوڙيو(Fn::concat, ترتيب_ڏيو_2(a.ترتيب_ڏيو().ويڪر() + b.ترتيب_ڏيو().ويڪر()), {a, b});
		}
		ڳنڍ_2 جوڙيو(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::جوڙيو, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 sub(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::sub, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 mul(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::mul, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 unsigned_div(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::unsigned_div, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 unsigned_mod(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::unsigned_mod, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 bitwise_and(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::bitwise_and, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 bitwise_or(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::bitwise_or, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 bitwise_xor(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::bitwise_xor, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 bitwise_not(ڳنڍ_2 a) { check_unary(a); أرجع جوڙيو(Fn::bitwise_not, a.ترتيب_ڏيو(), {a}); }
		ڳنڍ_2 unary_minus(ڳنڍ_2 a) { check_unary(a); أرجع جوڙيو(Fn::unary_minus, a.ترتيب_ڏيو(), {a}); }
		ڳنڍ_2 reduce_and(ڳنڍ_2 a) {
			check_unary(a);
			إذا(a.ويڪر() == 1)
				أرجع a;
			أرجع جوڙيو(Fn::reduce_and, ترتيب_ڏيو_2(1), {a});
		}
		ڳنڍ_2 reduce_or(ڳنڍ_2 a) {
			check_unary(a);
			إذا(a.ويڪر() == 1)
				أرجع a;
			أرجع جوڙيو(Fn::reduce_or, ترتيب_ڏيو_2(1), {a});
		}
		ڳنڍ_2 reduce_xor(ڳنڍ_2 a) {
			check_unary(a);
			إذا(a.ويڪر() == 1)
				أرجع a;
			أرجع جوڙيو(Fn::reduce_xor, ترتيب_ڏيو_2(1), {a});
		}
		ڳنڍ_2 equal(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::equal, ترتيب_ڏيو_2(1), {a, b}); }
		ڳنڍ_2 not_equal(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::not_equal, ترتيب_ڏيو_2(1), {a, b}); }
		ڳنڍ_2 signed_greater_than(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::signed_greater_than, ترتيب_ڏيو_2(1), {a, b}); }
		ڳنڍ_2 signed_greater_equal(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::signed_greater_equal, ترتيب_ڏيو_2(1), {a, b}); }
		ڳنڍ_2 unsigned_greater_than(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::unsigned_greater_than, ترتيب_ڏيو_2(1), {a, b}); }
		ڳنڍ_2 unsigned_greater_equal(ڳنڍ_2 a, ڳنڍ_2 b) { check_basic_ٻٽو(a, b); أرجع جوڙيو(Fn::unsigned_greater_equal, ترتيب_ڏيو_2(1), {a, b}); }
		ڳنڍ_2 logical_سرڪايو_کاٻو(ڳنڍ_2 a, ڳنڍ_2 b) { check_سرڪايو(a, b); أرجع جوڙيو(Fn::logical_سرڪايو_کاٻو, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 logical_سرڪايو_ساڄو(ڳنڍ_2 a, ڳنڍ_2 b) { check_سرڪايو(a, b); أرجع جوڙيو(Fn::logical_سرڪايو_ساڄو, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 arithmetic_سرڪايو_ساڄو(ڳنڍ_2 a, ڳنڍ_2 b) { check_سرڪايو(a, b); أرجع جوڙيو(Fn::arithmetic_سرڪايو_ساڄو, a.ترتيب_ڏيو(), {a, b}); }
		ڳنڍ_2 mux(ڳنڍ_2 a, ڳنڍ_2 b, ڳنڍ_2 s) {
			log_assert(a.ترتيب_ڏيو().is_signal() && a.ترتيب_ڏيو() == b.ترتيب_ڏيو() && s.ترتيب_ڏيو() == ترتيب_ڏيو_2(1));
			أرجع جوڙيو(Fn::mux, a.ترتيب_ڏيو(), {a, b, s});
		}
		ڳنڍ_2 يادگيري_read(ڳنڍ_2 mem, ڳنڍ_2 addr) {
			log_assert(mem.ترتيب_ڏيو().is_يادگيري() && addr.ترتيب_ڏيو().is_signal() && mem.ترتيب_ڏيو().addr_ويڪر() == addr.ترتيب_ڏيو().ويڪر());
			أرجع جوڙيو(Fn::يادگيري_read, ترتيب_ڏيو_2(mem.ترتيب_ڏيو().معلومات_ويڪر()), {mem, addr});
		}
		ڳنڍ_2 يادگيري_write(ڳنڍ_2 mem, ڳنڍ_2 addr, ڳنڍ_2 معلومات) {
			log_assert(mem.ترتيب_ڏيو().is_يادگيري() && addr.ترتيب_ڏيو().is_signal() && معلومات.ترتيب_ڏيو().is_signal() &&
				mem.ترتيب_ڏيو().addr_ويڪر() == addr.ترتيب_ڏيو().ويڪر() && mem.ترتيب_ڏيو().معلومات_ويڪر() == معلومات.ترتيب_ڏيو().ويڪر());
			أرجع جوڙيو(Fn::يادگيري_write, mem.ترتيب_ڏيو(), {mem, addr, معلومات});
		}
		ڳنڍ_2 constant(RTLIL::Const قدر) {
			صحيح s = قدر.سائيز();
			أرجع جوڙيو(IR::ڳنڍ_معلومات(Fn::constant, std::move(قدر)), ترتيب_ڏيو_2(s), {});
		}
		ڳنڍ_2 create_pending(صحيح ويڪر) {
			أرجع جوڙيو(Fn::buf, ترتيب_ڏيو_2(ويڪر), {});
		}
		فراغ update_pending(ڳنڍ_2 ڳنڍ, ڳنڍ_2 قدر) {
			log_assert(ڳنڍ._ref.function() == Fn::buf && ڳنڍ._ref.سائيز() == 0);
			log_assert(ڳنڍ.ترتيب_ڏيو() == قدر.ترتيب_ڏيو());
			_ir.mutate(ڳنڍ).append_arg(قدر._ref);
		}
		IRInput &جوڙيو_داخلا(IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو) {
			خودڪار [it, inserted] = _ir._inputs.emplace({name, قسم}, IRInput(_ir, name, قسم, std::move(ترتيب_ڏيو)));
			إذا (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->ٻيو;
		}
		IROutput &جوڙيو_output(IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو) {
			خودڪار [it, inserted] = _ir._outputs.emplace({name, قسم}, IROutput(_ir, name, قسم, std::move(ترتيب_ڏيو)));
			إذا (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->ٻيو;
		}
		IRState &جوڙيو_state(IdString name, IdString قسم, ترتيب_ڏيو_2 ترتيب_ڏيو) {
			خودڪار [it, inserted] = _ir.حالتون_2.emplace({name, قسم}, IRState(_ir, name, قسم, std::move(ترتيب_ڏيو)));
			إذا (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->ٻيو;
		}
		ڳنڍ_2 قدر(IRInput ثابت& داخلا) {
			أرجع جوڙيو(IR::ڳنڍ_معلومات(Fn::داخلا, std::pair(داخلا.name, داخلا.قسم)), داخلا.ترتيب_ڏيو, {});
		}
		ڳنڍ_2 قدر(IRState ثابت& state) {
			أرجع جوڙيو(IR::ڳنڍ_معلومات(Fn::state, std::pair(state.name, state.قسم)), state.ترتيب_ڏيو, {});
		}
		فراغ suggest_name(ڳنڍ_2 ڳنڍ, IdString name) {
			_ir.mutate(ڳنڍ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	مضمن Factory IR::factory() { أرجع Factory(*this); }
	سانچو<class Id> class Scope {
	محمي:
		حرف substitution_character = '_';
		مجازي bool is_character_legal(حرف, صحيح) = 0;
	خاص:
		pool<std::string> استعمال_ٿيل_نالا;
		dict<Id, std::string> _by_id;
	عام:
		فراغ reserve(std::string name) {
			استعمال_ٿيل_نالا.وجهو(std::move(name));
		}
		سانچو<typename N> std::string unique_name(ثابت N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			لكل(سائيز_t i = 0; i < str.سائيز(); i++)
				إذا(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			إذا(استعمال_ٿيل_نالا.ڳڻپ(str) == 0) {
				استعمال_ٿيل_نالا.وجهو(str);
				أرجع str;
			}
			لكل (صحيح idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::منزل_string(idx);
				إذا(استعمال_ٿيل_نالا.ڳڻپ(suffixed) == 0) {
					استعمال_ٿيل_نالا.وجهو(suffixed);
					أرجع suffixed;
				}
			}
		}
		سانچو<typename N> std::string عامل()(Id id, ثابت N &suggestion) {
			خودڪار it = _by_id.ڳوليو(id);
			إذا(it != _by_id.پڇاڙي())
				أرجع it->ٻيو;
			std::string str = unique_name(suggestion);
			_by_id.وجهو({id, str});
			أرجع str;
		}
	};
	class Writer {
		std::ostream *os;
		فراغ print_impl(ثابت حرف *fmt, ويڪٽر<std::function<فراغ()>>& fns);
	عام:
		Writer(std::ostream &os) : os(&os) {}
		سانچو<class T> Writer& عامل <<(T&& arg) { *os << std::forward<T>(arg); أرجع *this; }
		سانچو<typename... دليلون_3>
		فراغ print(ثابت حرف *fmt, دليلون_3&&... دليلون_2)
		{
			ويڪٽر<std::function<فراغ()>> fns { [&]() { *this << دليلون_2; }... };
			print_impl(fmt, fns);
		}
		سانچو<typename Fn, typename... دليلون_3>
		فراغ print_with(Fn fn, ثابت حرف *fmt, دليلون_3&&... دليلون_2)
		{
			ويڪٽر<std::function<فراغ()>> fns { [&]() {
				إذا constexpr (std::is_invocable_v<Fn, دليلون_3>)
					*this << fn(دليلون_2);
				وإلا
					*this << دليلون_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
