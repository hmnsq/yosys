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

#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/compute_graph.h"
#অন্তর্ভাৱ "kernel/drivertools.h"
#অন্তর্ভাৱ "kernel/mem.h"
#অন্তর্ভাৱ "kernel/utils.h"

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
	গণনা class Fn {
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
		slice,
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
		add,
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
		logical_shift_বাম,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_ডান,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_ডান,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		প্ৰবেশ,
		// state(a: IdString): any
		// returns the current value of the state variable with the specified name
		state,
		// memory_read(memory: memory[addr_width, data_width], addr: bit[addr_width]): bit[data_width] = memory[addr]
		memory_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		memory_write
	};
	// returns the name of a Fn value, as a string literal
	ধ্রুৱ ৱর্ণ *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<পূর্ণাঙ্ক, std::pair<পূর্ণাঙ্ক, পূর্ণাঙ্ক>> _v;
	সার্ৱজনিক:
		explicit Sort(পূর্ণাঙ্ক width) : _v(width) { }
		Sort(পূর্ণাঙ্ক addr_width, পূর্ণাঙ্ক data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() ধ্রুৱ { ঘূৰাই _v.index() == 0; }
		bool is_memory() ধ্রুৱ { ঘূৰাই _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		পূর্ণাঙ্ক width() ধ্রুৱ { ঘূৰাই std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		পূর্ণাঙ্ক addr_width() ধ্রুৱ { ঘূৰাই std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		পূর্ণাঙ্ক data_width() ধ্রুৱ { ঘূৰাই std::get<1>(_v).second; }
		bool operator==(Sort ধ্রুৱ& other) ধ্রুৱ { ঘূৰাই _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ { h.eat(_v); ঘূৰাই h; }
	};
	class IR;
	class Factory;
	class গ্ৰন্থি_2;
	class IRInput {
		friend class Factory;
	সার্ৱজনিক:
		PooledName name;
		PooledName kind;
		Sort sort;
	গুপ্ত:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	সার্ৱজনিক:
		PooledName name;
		PooledName kind;
		Sort sort;
	গুপ্ত:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	সার্ৱজনিক:
		গ্ৰন্থি_2 মান() ধ্রুৱ;
		bool has_মান() ধ্রুৱ;
		শূন্য set_মান(গ্ৰন্থি_2 মান);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	সার্ৱজনিক:
		PooledName name;
		PooledName kind;
		Sort sort;
	গুপ্ত:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	সার্ৱজনিক:
		গ্ৰন্থি_2 next_মান() ধ্রুৱ;
		bool has_next_মান() ধ্রুৱ;
		RTLIL::Const ধ্রুৱ& initial_মান_signal() ধ্রুৱ { ঘূৰাই std::get<RTLIL::Const>(_initial); }
		MemContents ধ্রুৱ& initial_মান_memory() ধ্রুৱ { ঘূৰাই std::get<MemContents>(_initial); }
		শূন্য set_next_মান(গ্ৰন্থি_2 মান);
		শূন্য set_initial_মান(RTLIL::Const মান) { মান.extu(sort.width()); _initial = std::move(মান); }
		শূন্য set_initial_মান(MemContents মান) { log_assert(Sort(মান.addr_width(), মান.data_width()) == sort); _initial = std::move(মান); }
	};
	class IR {
		friend class Factory;
		friend class গ্ৰন্থি_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class গ্ৰন্থি_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				পূর্ণাঙ্ক
			> _extra;
		সার্ৱজনিক:
			গ্ৰন্থি_Data() : _fn(Fn::invalid) {}
			গ্ৰন্থি_Data(Fn fn) : _fn(fn) {}
			template<class T> গ্ৰন্থি_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ধ্রুৱ { ঘূৰাই _fn; }
			ধ্রুৱ RTLIL::Const &as_const() ধ্রুৱ { ঘূৰাই std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ধ্রুৱ { ঘূৰাই std::get<std::pair<IdString, IdString>>(_extra); }
			পূর্ণাঙ্ক as_int() ধ্রুৱ { ঘূৰাই std::get<পূর্ণাঙ্ক>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ধ্রুৱ {
				h.eat((unsigned পূর্ণাঙ্ক) _fn);
				h.eat(_extra);
				ঘূৰাই h;
			}
			bool operator==(গ্ৰন্থি_Data ধ্রুৱ &other) ধ্রুৱ {
				ঘূৰাই _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		সংরচনা Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<গ্ৰন্থি_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(গ্ৰন্থি_2 n);
	সার্ৱজনিক:
		Design *design = nullptr;
		স্থির IR from_module(Module *module);
		Factory factory();
		পূর্ণাঙ্ক size() ধ্রুৱ { ঘূৰাই _graph.size(); }
		গ্ৰন্থি_2 operator[](পূর্ণাঙ্ক i);
		শূন্য topological_sort();
		শূন্য forward_buf();
		IRInput ধ্রুৱ& প্ৰবেশ(IdString name, IdString kind) ধ্রুৱ { ঘূৰাই _inputs.at({name, kind}); }
		IRInput ধ্রুৱ& প্ৰবেশ(IdString name) ধ্রুৱ { ঘূৰাই প্ৰবেশ(name, ID($প্ৰবেশ)); }
		IROutput ধ্রুৱ& output(IdString name, IdString kind) ধ্রুৱ { ঘূৰাই _outputs.at({name, kind}); }
		IROutput ধ্রুৱ& output(IdString name) ধ্রুৱ { ঘূৰাই output(name, ID($output)); }
		IRState ধ্রুৱ& state(IdString name, IdString kind) ধ্রুৱ { ঘূৰাই _states.at({name, kind}); }
		IRState ধ্রুৱ& state(IdString name) ধ্রুৱ { ঘূৰাই state(name, ID($state)); }
		bool has_প্ৰবেশ(IdString name, IdString kind) ধ্রুৱ { ঘূৰাই _inputs.গুনতি({name, kind}); }
		bool has_output(IdString name, IdString kind) ধ্রুৱ { ঘূৰাই _outputs.গুনতি({name, kind}); }
		bool has_state(IdString name, IdString kind) ধ্রুৱ { ঘূৰাই _states.গুনতি({name, kind}); }
		vector<IRInput ধ্রুৱ*> inputs(IdString kind) ধ্রুৱ;
		vector<IRInput ধ্রুৱ*> inputs() ধ্রুৱ { ঘূৰাই inputs(ID($প্ৰবেশ)); }
		vector<IROutput ধ্রুৱ*> outputs(IdString kind) ধ্রুৱ;
		vector<IROutput ধ্রুৱ*> outputs() ধ্রুৱ { ঘূৰাই outputs(ID($output)); }
		vector<IRState ধ্রুৱ*> states(IdString kind) ধ্রুৱ;
		vector<IRState ধ্রুৱ*> states() ধ্রুৱ { ঘূৰাই states(ID($state)); }
		vector<IRInput ধ্রুৱ*> all_inputs() ধ্রুৱ;
		vector<IROutput ধ্রুৱ*> all_outputs() ধ্রুৱ;
		vector<IRState ধ্রুৱ*> all_states() ধ্রুৱ;
		class iterator {
			friend class IR;
			IR *_ir;
			পূর্ণাঙ্ক _index;
			iterator(IR *ir, পূর্ণাঙ্ক index) : _ir(ir), _index(index) {}
		সার্ৱজনিক:
			using iterator_category = std::প্ৰবেশ_iterator_tag;
			using মান_type = গ্ৰন্থি_2;
			using pointer = arrow_proxy<গ্ৰন্থি_2>;
			using reference = গ্ৰন্থি_2;
			using difference_type = ptrdiff_t;
			গ্ৰন্থি_2 operator*();
			iterator &operator++() { _index++; ঘূৰাই *this; }
			bool operator!=(iterator ধ্রুৱ &other) ধ্রুৱ { ঘূৰাই _ir != other._ir || _index != other._index; }
			bool operator==(iterator ধ্রুৱ &other) ধ্রুৱ { ঘূৰাই !(*this != other); }
			pointer operator->();
		};
		iterator begin() { ঘূৰাই iterator(this, 0); }
		iterator end() { ঘূৰাই iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class গ্ৰন্থি_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		গ্ৰন্থি_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { ঘূৰাই _ref; }
	সার্ৱজনিক:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		পূর্ণাঙ্ক id() ধ্রুৱ { ঘূৰাই _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ধ্রুৱ {
			যদি(_ref.has_sparse_attr())
				ঘূৰাই {design, _ref.sparse_attr()};
			অন্যথা
				ঘূৰাই {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() ধ্রুৱ { ঘূৰাই _ref.function().fn(); }
		Sort sort() ধ্রুৱ { ঘূৰাই _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		পূর্ণাঙ্ক width() ধ্রুৱ { ঘূৰাই sort().width(); }
		size_t arg_গুনতি() ধ্রুৱ { ঘূৰাই _ref.size(); }
		গ্ৰন্থি_2 arg(পূর্ণাঙ্ক n) ধ্রুৱ { ঘূৰাই গ্ৰন্থি_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) ধ্রুৱ
		{
			// currently templated but could be switched to AbstractVisitor &
			ৱিকল্প(_ref.function().fn()) {
			পক্ষ Fn::invalid: log_error("invalid node in visit\n"); ভঙ্গ;
			পক্ষ Fn::buf: ঘূৰাই v.buf(*this, arg(0)); ভঙ্গ;
			পক্ষ Fn::slice: ঘূৰাই v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); ভঙ্গ;
			পক্ষ Fn::zero_extend: ঘূৰাই v.zero_extend(*this, arg(0), width()); ভঙ্গ;
			পক্ষ Fn::sign_extend: ঘূৰাই v.sign_extend(*this, arg(0), width()); ভঙ্গ;
			পক্ষ Fn::concat: ঘূৰাই v.concat(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::add: ঘূৰাই v.add(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::sub: ঘূৰাই v.sub(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::mul: ঘূৰাই v.mul(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::unsigned_div: ঘূৰাই v.unsigned_div(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::unsigned_mod: ঘূৰাই v.unsigned_mod(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::bitwise_and: ঘূৰাই v.bitwise_and(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::bitwise_or: ঘূৰাই v.bitwise_or(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::bitwise_xor: ঘূৰাই v.bitwise_xor(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::bitwise_not: ঘূৰাই v.bitwise_not(*this, arg(0)); ভঙ্গ;
			পক্ষ Fn::unary_minus: ঘূৰাই v.unary_minus(*this, arg(0)); ভঙ্গ;
			পক্ষ Fn::reduce_and: ঘূৰাই v.reduce_and(*this, arg(0)); ভঙ্গ;
			পক্ষ Fn::reduce_or: ঘূৰাই v.reduce_or(*this, arg(0)); ভঙ্গ;
			পক্ষ Fn::reduce_xor: ঘূৰাই v.reduce_xor(*this, arg(0)); ভঙ্গ;
			পক্ষ Fn::equal: ঘূৰাই v.equal(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::not_equal: ঘূৰাই v.not_equal(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::signed_greater_than: ঘূৰাই v.signed_greater_than(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::signed_greater_equal: ঘূৰাই v.signed_greater_equal(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::unsigned_greater_than: ঘূৰাই v.unsigned_greater_than(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::unsigned_greater_equal: ঘূৰাই v.unsigned_greater_equal(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::logical_shift_বাম: ঘূৰাই v.logical_shift_বাম(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::logical_shift_ডান: ঘূৰাই v.logical_shift_ডান(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::arithmetic_shift_ডান: ঘূৰাই v.arithmetic_shift_ডান(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::mux: ঘূৰাই v.mux(*this, arg(0), arg(1), arg(2)); ভঙ্গ;
			পক্ষ Fn::constant: ঘূৰাই v.constant(*this, _ref.function().as_const()); ভঙ্গ;
			পক্ষ Fn::প্ৰবেশ: ঘূৰাই v.প্ৰবেশ(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); ভঙ্গ;
			পক্ষ Fn::state: ঘূৰাই v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); ভঙ্গ;
			পক্ষ Fn::memory_read: ঘূৰাই v.memory_read(*this, arg(0), arg(1)); ভঙ্গ;
			পক্ষ Fn::memory_write: ঘূৰাই v.memory_write(*this, arg(0), arg(1), arg(2)); ভঙ্গ;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(গ্ৰন্থি_2)>);
	};
	অন্তঃস্থ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	অন্তঃস্থ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	অন্তঃস্থ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	অন্তঃস্থ IR::Graph::Ref IR::mutate(গ্ৰন্থি_2 n) { ঘূৰাই _graph[n._ref.index()]; }
	অন্তঃস্থ গ্ৰন্থি_2 IR::operator[](পূর্ণাঙ্ক i) { ঘূৰাই গ্ৰন্থি_2(_graph[i], design); }
	অন্তঃস্থ গ্ৰন্থি_2 IROutput::মান() ধ্রুৱ { ঘূৰাই গ্ৰন্থি_2(_ir._graph({name, kind, false}), _ir.design); }
	অন্তঃস্থ bool IROutput::has_মান() ধ্রুৱ { ঘূৰাই _ir._graph.has_চাবি({name, kind, false}); }
	অন্তঃস্থ শূন্য IROutput::set_মান(গ্ৰন্থি_2 মান) { log_assert(sort == মান.sort()); _ir.mutate(মান).assign_চাবি({name, kind, false}); }
	অন্তঃস্থ গ্ৰন্থি_2 IRState::next_মান() ধ্রুৱ { ঘূৰাই গ্ৰন্থি_2(_ir._graph({name, kind, true}), _ir.design); }
	অন্তঃস্থ bool IRState::has_next_মান() ধ্রুৱ { ঘূৰাই _ir._graph.has_চাবি({name, kind, true}); }
	অন্তঃস্থ শূন্য IRState::set_next_মান(গ্ৰন্থি_2 মান) { log_assert(sort == মান.sort()); _ir.mutate(মান).assign_চাবি({name, kind, true}); }
	অন্তঃস্থ গ্ৰন্থি_2 IR::iterator::operator*() { ঘূৰাই গ্ৰন্থি_2(_ir->_graph[_index], _ir->design); }
	অন্তঃস্থ arrow_proxy<গ্ৰন্থি_2> IR::iterator::operator->() { ঘূৰাই arrow_proxy<গ্ৰন্থি_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> সংরচনা AbstractVisitor {
		আভাস T buf(গ্ৰন্থি_2 self, গ্ৰন্থি_2 n) = 0;
		আভাস T slice(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক out_width) = 0;
		আভাস T zero_extend(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, পূর্ণাঙ্ক out_width) = 0;
		আভাস T sign_extend(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, পূর্ণাঙ্ক out_width) = 0;
		আভাস T concat(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T add(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T sub(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T mul(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T unsigned_div(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T unsigned_mod(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T bitwise_and(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T bitwise_or(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T bitwise_xor(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T bitwise_not(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a) = 0;
		আভাস T unary_minus(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a) = 0;
		আভাস T reduce_and(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a) = 0;
		আভাস T reduce_or(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a) = 0;
		আভাস T reduce_xor(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a) = 0;
		আভাস T equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T not_equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T signed_greater_than(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T signed_greater_equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T unsigned_greater_than(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T unsigned_greater_equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T logical_shift_বাম(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T logical_shift_ডান(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T arithmetic_shift_ডান(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) = 0;
		আভাস T mux(গ্ৰন্থি_2 self, গ্ৰন্থি_2 a, গ্ৰন্থি_2 b, গ্ৰন্থি_2 s) = 0;
		আভাস T constant(গ্ৰন্থি_2 self, RTLIL::Const ধ্রুৱ & মান) = 0;
		আভাস T প্ৰবেশ(গ্ৰন্থি_2 self, IdString name, IdString kind) = 0;
		আভাস T state(গ্ৰন্থি_2 self, IdString name, IdString kind) = 0;
		আভাস T memory_read(গ্ৰন্থি_2 self, গ্ৰন্থি_2 mem, গ্ৰন্থি_2 addr) = 0;
		আভাস T memory_write(গ্ৰন্থি_2 self, গ্ৰন্থি_2 mem, গ্ৰন্থি_2 addr, গ্ৰন্থি_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> সংরচনা DefaultVisitor : সার্ৱজনিক AbstractVisitor<T> {
		আভাস T default_handler(গ্ৰন্থি_2 self) = 0;
		T buf(গ্ৰন্থি_2 self, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T slice(গ্ৰন্থি_2 self, গ্ৰন্থি_2, পূর্ণাঙ্ক, পূর্ণাঙ্ক) override { ঘূৰাই default_handler(self); }
		T zero_extend(গ্ৰন্থি_2 self, গ্ৰন্থি_2, পূর্ণাঙ্ক) override { ঘূৰাই default_handler(self); }
		T sign_extend(গ্ৰন্থি_2 self, গ্ৰন্থি_2, পূর্ণাঙ্ক) override { ঘূৰাই default_handler(self); }
		T concat(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T add(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T sub(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T mul(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T unsigned_div(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T unsigned_mod(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T bitwise_and(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T bitwise_or(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T bitwise_xor(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T bitwise_not(গ্ৰন্থি_2 self, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T unary_minus(গ্ৰন্থি_2 self, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T reduce_and(গ্ৰন্থি_2 self, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T reduce_or(গ্ৰন্থি_2 self, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T reduce_xor(গ্ৰন্থি_2 self, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T not_equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T signed_greater_than(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T signed_greater_equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T unsigned_greater_than(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T unsigned_greater_equal(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T logical_shift_বাম(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T logical_shift_ডান(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T arithmetic_shift_ডান(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T mux(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T constant(গ্ৰন্থি_2 self, RTLIL::Const ধ্রুৱ &) override { ঘূৰাই default_handler(self); }
		T প্ৰবেশ(গ্ৰন্থি_2 self, IdString, IdString) override { ঘূৰাই default_handler(self); }
		T state(গ্ৰন্থি_2 self, IdString, IdString) override { ঘূৰাই default_handler(self); }
		T memory_read(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
		T memory_write(গ্ৰন্থি_2 self, গ্ৰন্থি_2, গ্ৰন্থি_2, গ্ৰন্থি_2) override { ঘূৰাই default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		গ্ৰন্থি_2 add(IR::গ্ৰন্থি_Data &&fn, Sort ধ্রুৱ &sort, std::initializer_list<গ্ৰন্থি_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			বাবে (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			ঘূৰাই গ্ৰন্থি_2(ref, _ir.design);
		}
		শূন্য check_basic_দ্বিমিক(গ্ৰন্থি_2 ধ্রুৱ &a, গ্ৰন্থি_2 ধ্রুৱ &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		শূন্য check_shift(গ্ৰন্থি_2 ধ্রুৱ &a, গ্ৰন্থি_2 ধ্রুৱ &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		শূন্য check_unary(গ্ৰন্থি_2 ধ্রুৱ &a) { log_assert(a.sort().is_signal()); }
	সার্ৱজনিক:
		IR &ir() { ঘূৰাই _ir; }
		গ্ৰন্থি_2 slice(গ্ৰন্থি_2 a, পূর্ণাঙ্ক offset, পূর্ণাঙ্ক out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			যদি(offset == 0 && out_width == a.width())
				ঘূৰাই a;
			ঘূৰাই add(IR::গ্ৰন্থি_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		গ্ৰন্থি_2 extend(গ্ৰন্থি_2 a, পূর্ণাঙ্ক out_width, bool is_signed) {
			পূর্ণাঙ্ক in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			যদি(in_width == out_width)
				ঘূৰাই a;
			যদি(in_width > out_width)
				ঘূৰাই slice(a, 0, out_width);
			যদি(is_signed)
				ঘূৰাই add(Fn::sign_extend, Sort(out_width), {a});
			অন্যথা
				ঘূৰাই add(Fn::zero_extend, Sort(out_width), {a});
		}
		গ্ৰন্থি_2 concat(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			ঘূৰাই add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		গ্ৰন্থি_2 add(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::add, a.sort(), {a, b}); }
		গ্ৰন্থি_2 sub(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::sub, a.sort(), {a, b}); }
		গ্ৰন্থি_2 mul(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::mul, a.sort(), {a, b}); }
		গ্ৰন্থি_2 unsigned_div(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::unsigned_div, a.sort(), {a, b}); }
		গ্ৰন্থি_2 unsigned_mod(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::unsigned_mod, a.sort(), {a, b}); }
		গ্ৰন্থি_2 bitwise_and(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::bitwise_and, a.sort(), {a, b}); }
		গ্ৰন্থি_2 bitwise_or(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::bitwise_or, a.sort(), {a, b}); }
		গ্ৰন্থি_2 bitwise_xor(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::bitwise_xor, a.sort(), {a, b}); }
		গ্ৰন্থি_2 bitwise_not(গ্ৰন্থি_2 a) { check_unary(a); ঘূৰাই add(Fn::bitwise_not, a.sort(), {a}); }
		গ্ৰন্থি_2 unary_minus(গ্ৰন্থি_2 a) { check_unary(a); ঘূৰাই add(Fn::unary_minus, a.sort(), {a}); }
		গ্ৰন্থি_2 reduce_and(গ্ৰন্থি_2 a) {
			check_unary(a);
			যদি(a.width() == 1)
				ঘূৰাই a;
			ঘূৰাই add(Fn::reduce_and, Sort(1), {a});
		}
		গ্ৰন্থি_2 reduce_or(গ্ৰন্থি_2 a) {
			check_unary(a);
			যদি(a.width() == 1)
				ঘূৰাই a;
			ঘূৰাই add(Fn::reduce_or, Sort(1), {a});
		}
		গ্ৰন্থি_2 reduce_xor(গ্ৰন্থি_2 a) {
			check_unary(a);
			যদি(a.width() == 1)
				ঘূৰাই a;
			ঘূৰাই add(Fn::reduce_xor, Sort(1), {a});
		}
		গ্ৰন্থি_2 equal(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::equal, Sort(1), {a, b}); }
		গ্ৰন্থি_2 not_equal(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::not_equal, Sort(1), {a, b}); }
		গ্ৰন্থি_2 signed_greater_than(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::signed_greater_than, Sort(1), {a, b}); }
		গ্ৰন্থি_2 signed_greater_equal(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		গ্ৰন্থি_2 unsigned_greater_than(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		গ্ৰন্থি_2 unsigned_greater_equal(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_basic_দ্বিমিক(a, b); ঘূৰাই add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		গ্ৰন্থি_2 logical_shift_বাম(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_shift(a, b); ঘূৰাই add(Fn::logical_shift_বাম, a.sort(), {a, b}); }
		গ্ৰন্থি_2 logical_shift_ডান(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_shift(a, b); ঘূৰাই add(Fn::logical_shift_ডান, a.sort(), {a, b}); }
		গ্ৰন্থি_2 arithmetic_shift_ডান(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b) { check_shift(a, b); ঘূৰাই add(Fn::arithmetic_shift_ডান, a.sort(), {a, b}); }
		গ্ৰন্থি_2 mux(গ্ৰন্থি_2 a, গ্ৰন্থি_2 b, গ্ৰন্থি_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			ঘূৰাই add(Fn::mux, a.sort(), {a, b, s});
		}
		গ্ৰন্থি_2 memory_read(গ্ৰন্থি_2 mem, গ্ৰন্থি_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			ঘূৰাই add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		গ্ৰন্থি_2 memory_write(গ্ৰন্থি_2 mem, গ্ৰন্থি_2 addr, গ্ৰন্থি_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			ঘূৰাই add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		গ্ৰন্থি_2 constant(RTLIL::Const মান) {
			পূর্ণাঙ্ক s = মান.size();
			ঘূৰাই add(IR::গ্ৰন্থি_Data(Fn::constant, std::move(মান)), Sort(s), {});
		}
		গ্ৰন্থি_2 create_pending(পূর্ণাঙ্ক width) {
			ঘূৰাই add(Fn::buf, Sort(width), {});
		}
		শূন্য update_pending(গ্ৰন্থি_2 গ্ৰন্থি, গ্ৰন্থি_2 মান) {
			log_assert(গ্ৰন্থি._ref.function() == Fn::buf && গ্ৰন্থি._ref.size() == 0);
			log_assert(গ্ৰন্থি.sort() == মান.sort());
			_ir.mutate(গ্ৰন্থি).append_arg(মান._ref);
		}
		IRInput &add_প্ৰবেশ(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			যদি (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			ঘূৰাই it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			যদি (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			ঘূৰাই it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			যদি (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			ঘূৰাই it->second;
		}
		গ্ৰন্থি_2 মান(IRInput ধ্রুৱ& প্ৰবেশ) {
			ঘূৰাই add(IR::গ্ৰন্থি_Data(Fn::প্ৰবেশ, std::pair(প্ৰবেশ.name, প্ৰবেশ.kind)), প্ৰবেশ.sort, {});
		}
		গ্ৰন্থি_2 মান(IRState ধ্রুৱ& state) {
			ঘূৰাই add(IR::গ্ৰন্থি_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		শূন্য suggest_name(গ্ৰন্থি_2 গ্ৰন্থি, IdString name) {
			_ir.mutate(গ্ৰন্থি).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	অন্তঃস্থ Factory IR::factory() { ঘূৰাই Factory(*this); }
	template<class Id> class Scope {
	রক্ষিত:
		ৱর্ণ substitution_character = '_';
		আভাস bool is_character_legal(ৱর্ণ, পূর্ণাঙ্ক) = 0;
	গুপ্ত:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	সার্ৱজনিক:
		শূন্য reserve(std::string name) {
			_used_names.ঢোকাও(std::move(name));
		}
		template<typename N> std::string unique_name(ধ্রুৱ N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			বাবে(size_t i = 0; i < str.size(); i++)
				যদি(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			যদি(_used_names.গুনতি(str) == 0) {
				_used_names.ঢোকাও(str);
				ঘূৰাই str;
			}
			বাবে (পূর্ণাঙ্ক idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				যদি(_used_names.গুনতি(suffixed) == 0) {
					_used_names.ঢোকাও(suffixed);
					ঘূৰাই suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, ধ্রুৱ N &suggestion) {
			auto it = _by_id.find(id);
			যদি(it != _by_id.end())
				ঘূৰাই it->second;
			std::string str = unique_name(suggestion);
			_by_id.ঢোকাও({id, str});
			ঘূৰাই str;
		}
	};
	class Writer {
		std::ostream *os;
		শূন্য print_impl(ধ্রুৱ ৱর্ণ *fmt, vector<std::function<শূন্য()>>& fns);
	সার্ৱজনিক:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); ঘূৰাই *this; }
		template<typename... Args>
		শূন্য print(ধ্রুৱ ৱর্ণ *fmt, Args&&... args)
		{
			vector<std::function<শূন্য()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		শূন্য print_with(Fn fn, ধ্রুৱ ৱর্ণ *fmt, Args&&... args)
		{
			vector<std::function<শূন্য()>> fns { [&]() {
				যদি constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				অন্যথা
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
