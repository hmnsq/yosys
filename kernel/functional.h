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
		logical_shift_سول,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_ئوڭ,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_ئوڭ,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		كىرگۈزۈش,
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
	ثابت حرف *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<صحيح, std::pair<صحيح, صحيح>> _v;
	عام:
		explicit Sort(صحيح width) : _v(width) { }
		Sort(صحيح addr_width, صحيح data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() ثابت { أرجع _v.index() == 0; }
		bool is_memory() ثابت { أرجع _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		صحيح width() ثابت { أرجع std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		صحيح addr_width() ثابت { أرجع std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		صحيح data_width() ثابت { أرجع std::get<1>(_v).second; }
		bool operator==(Sort ثابت& other) ثابت { أرجع _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(_v); أرجع h; }
	};
	class IR;
	class Factory;
	class تۈگۈن_2;
	class IRInput {
		friend class Factory;
	عام:
		PooledName name;
		PooledName kind;
		Sort sort;
	خاص:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName kind;
		Sort sort;
	خاص:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	عام:
		تۈگۈن_2 قىممەت() ثابت;
		bool has_قىممەت() ثابت;
		فراغ set_قىممەت(تۈگۈن_2 قىممەت);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName kind;
		Sort sort;
	خاص:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	عام:
		تۈگۈن_2 next_قىممەت() ثابت;
		bool has_next_قىممەت() ثابت;
		RTLIL::Const ثابت& initial_قىممەت_signal() ثابت { أرجع std::get<RTLIL::Const>(_initial); }
		MemContents ثابت& initial_قىممەت_memory() ثابت { أرجع std::get<MemContents>(_initial); }
		فراغ set_next_قىممەت(تۈگۈن_2 قىممەت);
		فراغ set_initial_قىممەت(RTLIL::Const قىممەت) { قىممەت.extu(sort.width()); _initial = std::move(قىممەت); }
		فراغ set_initial_قىممەت(MemContents قىممەت) { log_assert(Sort(قىممەت.addr_width(), قىممەت.data_width()) == sort); _initial = std::move(قىممەت); }
	};
	class IR {
		friend class Factory;
		friend class تۈگۈن_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class تۈگۈن_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				صحيح
			> _extra;
		عام:
			تۈگۈن_Data() : _fn(Fn::invalid) {}
			تۈگۈن_Data(Fn fn) : _fn(fn) {}
			template<class T> تۈگۈن_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ثابت { أرجع _fn; }
			ثابت RTLIL::Const &as_const() ثابت { أرجع std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ثابت { أرجع std::get<std::pair<IdString, IdString>>(_extra); }
			صحيح as_int() ثابت { أرجع std::get<صحيح>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
				h.eat((unsigned صحيح) _fn);
				h.eat(_extra);
				أرجع h;
			}
			bool operator==(تۈگۈن_Data ثابت &other) ثابت {
				أرجع _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		بنية Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<تۈگۈن_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(تۈگۈن_2 n);
	عام:
		Design *design = nullptr;
		ساكن IR from_module(Module *module);
		Factory factory();
		صحيح size() ثابت { أرجع _graph.size(); }
		تۈگۈن_2 operator[](صحيح i);
		فراغ topological_sort();
		فراغ forward_buf();
		IRInput ثابت& كىرگۈزۈش(IdString name, IdString kind) ثابت { أرجع _inputs.at({name, kind}); }
		IRInput ثابت& كىرگۈزۈش(IdString name) ثابت { أرجع كىرگۈزۈش(name, ID($كىرگۈزۈش)); }
		IROutput ثابت& output(IdString name, IdString kind) ثابت { أرجع _outputs.at({name, kind}); }
		IROutput ثابت& output(IdString name) ثابت { أرجع output(name, ID($output)); }
		IRState ثابت& state(IdString name, IdString kind) ثابت { أرجع _states.at({name, kind}); }
		IRState ثابت& state(IdString name) ثابت { أرجع state(name, ID($state)); }
		bool has_كىرگۈزۈش(IdString name, IdString kind) ثابت { أرجع _inputs.سانى({name, kind}); }
		bool has_output(IdString name, IdString kind) ثابت { أرجع _outputs.سانى({name, kind}); }
		bool has_state(IdString name, IdString kind) ثابت { أرجع _states.سانى({name, kind}); }
		vector<IRInput ثابت*> inputs(IdString kind) ثابت;
		vector<IRInput ثابت*> inputs() ثابت { أرجع inputs(ID($كىرگۈزۈش)); }
		vector<IROutput ثابت*> outputs(IdString kind) ثابت;
		vector<IROutput ثابت*> outputs() ثابت { أرجع outputs(ID($output)); }
		vector<IRState ثابت*> states(IdString kind) ثابت;
		vector<IRState ثابت*> states() ثابت { أرجع states(ID($state)); }
		vector<IRInput ثابت*> all_inputs() ثابت;
		vector<IROutput ثابت*> all_outputs() ثابت;
		vector<IRState ثابت*> all_states() ثابت;
		class iterator {
			friend class IR;
			IR *_ir;
			صحيح _index;
			iterator(IR *ir, صحيح index) : _ir(ir), _index(index) {}
		عام:
			using iterator_category = std::كىرگۈزۈش_iterator_tag;
			using قىممەت_type = تۈگۈن_2;
			using pointer = arrow_proxy<تۈگۈن_2>;
			using reference = تۈگۈن_2;
			using difference_type = ptrdiff_t;
			تۈگۈن_2 operator*();
			iterator &operator++() { _index++; أرجع *this; }
			bool operator!=(iterator ثابت &other) ثابت { أرجع _ir != other._ir || _index != other._index; }
			bool operator==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
			pointer operator->();
		};
		iterator begin() { أرجع iterator(this, 0); }
		iterator end() { أرجع iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class تۈگۈن_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		تۈگۈن_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { أرجع _ref; }
	عام:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		صحيح id() ثابت { أرجع _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ثابت {
			إذا(_ref.has_sparse_attr())
				أرجع {design, _ref.sparse_attr()};
			وإلا
				أرجع {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() ثابت { أرجع _ref.function().fn(); }
		Sort sort() ثابت { أرجع _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		صحيح width() ثابت { أرجع sort().width(); }
		size_t arg_سانى() ثابت { أرجع _ref.size(); }
		تۈگۈن_2 arg(صحيح n) ثابت { أرجع تۈگۈن_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) ثابت
		{
			// currently templated but could be switched to AbstractVisitor &
			بدل(_ref.function().fn()) {
			حالة Fn::invalid: log_error("invalid node in visit\n"); اكسر;
			حالة Fn::buf: أرجع v.buf(*this, arg(0)); اكسر;
			حالة Fn::slice: أرجع v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); اكسر;
			حالة Fn::zero_extend: أرجع v.zero_extend(*this, arg(0), width()); اكسر;
			حالة Fn::sign_extend: أرجع v.sign_extend(*this, arg(0), width()); اكسر;
			حالة Fn::concat: أرجع v.concat(*this, arg(0), arg(1)); اكسر;
			حالة Fn::add: أرجع v.add(*this, arg(0), arg(1)); اكسر;
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
			حالة Fn::logical_shift_سول: أرجع v.logical_shift_سول(*this, arg(0), arg(1)); اكسر;
			حالة Fn::logical_shift_ئوڭ: أرجع v.logical_shift_ئوڭ(*this, arg(0), arg(1)); اكسر;
			حالة Fn::arithmetic_shift_ئوڭ: أرجع v.arithmetic_shift_ئوڭ(*this, arg(0), arg(1)); اكسر;
			حالة Fn::mux: أرجع v.mux(*this, arg(0), arg(1), arg(2)); اكسر;
			حالة Fn::constant: أرجع v.constant(*this, _ref.function().as_const()); اكسر;
			حالة Fn::كىرگۈزۈش: أرجع v.كىرگۈزۈش(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); اكسر;
			حالة Fn::state: أرجع v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); اكسر;
			حالة Fn::memory_read: أرجع v.memory_read(*this, arg(0), arg(1)); اكسر;
			حالة Fn::memory_write: أرجع v.memory_write(*this, arg(0), arg(1), arg(2)); اكسر;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(تۈگۈن_2)>);
	};
	مضمن IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	مضمن IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	مضمن IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	مضمن IR::Graph::Ref IR::mutate(تۈگۈن_2 n) { أرجع _graph[n._ref.index()]; }
	مضمن تۈگۈن_2 IR::operator[](صحيح i) { أرجع تۈگۈن_2(_graph[i], design); }
	مضمن تۈگۈن_2 IROutput::قىممەت() ثابت { أرجع تۈگۈن_2(_ir._graph({name, kind, false}), _ir.design); }
	مضمن bool IROutput::has_قىممەت() ثابت { أرجع _ir._graph.has_ئاچقۇچ({name, kind, false}); }
	مضمن فراغ IROutput::set_قىممەت(تۈگۈن_2 قىممەت) { log_assert(sort == قىممەت.sort()); _ir.mutate(قىممەت).assign_ئاچقۇچ({name, kind, false}); }
	مضمن تۈگۈن_2 IRState::next_قىممەت() ثابت { أرجع تۈگۈن_2(_ir._graph({name, kind, true}), _ir.design); }
	مضمن bool IRState::has_next_قىممەت() ثابت { أرجع _ir._graph.has_ئاچقۇچ({name, kind, true}); }
	مضمن فراغ IRState::set_next_قىممەت(تۈگۈن_2 قىممەت) { log_assert(sort == قىممەت.sort()); _ir.mutate(قىممەت).assign_ئاچقۇچ({name, kind, true}); }
	مضمن تۈگۈن_2 IR::iterator::operator*() { أرجع تۈگۈن_2(_ir->_graph[_index], _ir->design); }
	مضمن arrow_proxy<تۈگۈن_2> IR::iterator::operator->() { أرجع arrow_proxy<تۈگۈن_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> بنية AbstractVisitor {
		virtual T buf(تۈگۈن_2 self, تۈگۈن_2 n) = 0;
		virtual T slice(تۈگۈن_2 self, تۈگۈن_2 a, صحيح offset, صحيح out_width) = 0;
		virtual T zero_extend(تۈگۈن_2 self, تۈگۈن_2 a, صحيح out_width) = 0;
		virtual T sign_extend(تۈگۈن_2 self, تۈگۈن_2 a, صحيح out_width) = 0;
		virtual T concat(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T add(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T sub(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T mul(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T unsigned_div(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T unsigned_mod(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T bitwise_and(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T bitwise_or(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T bitwise_xor(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T bitwise_not(تۈگۈن_2 self, تۈگۈن_2 a) = 0;
		virtual T unary_minus(تۈگۈن_2 self, تۈگۈن_2 a) = 0;
		virtual T reduce_and(تۈگۈن_2 self, تۈگۈن_2 a) = 0;
		virtual T reduce_or(تۈگۈن_2 self, تۈگۈن_2 a) = 0;
		virtual T reduce_xor(تۈگۈن_2 self, تۈگۈن_2 a) = 0;
		virtual T equal(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T not_equal(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T signed_greater_than(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T signed_greater_equal(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T unsigned_greater_than(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T unsigned_greater_equal(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T logical_shift_سول(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T logical_shift_ئوڭ(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T arithmetic_shift_ئوڭ(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b) = 0;
		virtual T mux(تۈگۈن_2 self, تۈگۈن_2 a, تۈگۈن_2 b, تۈگۈن_2 s) = 0;
		virtual T constant(تۈگۈن_2 self, RTLIL::Const ثابت & قىممەت) = 0;
		virtual T كىرگۈزۈش(تۈگۈن_2 self, IdString name, IdString kind) = 0;
		virtual T state(تۈگۈن_2 self, IdString name, IdString kind) = 0;
		virtual T memory_read(تۈگۈن_2 self, تۈگۈن_2 mem, تۈگۈن_2 addr) = 0;
		virtual T memory_write(تۈگۈن_2 self, تۈگۈن_2 mem, تۈگۈن_2 addr, تۈگۈن_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> بنية DefaultVisitor : عام AbstractVisitor<T> {
		virtual T default_handler(تۈگۈن_2 self) = 0;
		T buf(تۈگۈن_2 self, تۈگۈن_2) override { أرجع default_handler(self); }
		T slice(تۈگۈن_2 self, تۈگۈن_2, صحيح, صحيح) override { أرجع default_handler(self); }
		T zero_extend(تۈگۈن_2 self, تۈگۈن_2, صحيح) override { أرجع default_handler(self); }
		T sign_extend(تۈگۈن_2 self, تۈگۈن_2, صحيح) override { أرجع default_handler(self); }
		T concat(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T add(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T sub(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T mul(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T unsigned_div(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T unsigned_mod(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T bitwise_and(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T bitwise_or(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T bitwise_xor(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T bitwise_not(تۈگۈن_2 self, تۈگۈن_2) override { أرجع default_handler(self); }
		T unary_minus(تۈگۈن_2 self, تۈگۈن_2) override { أرجع default_handler(self); }
		T reduce_and(تۈگۈن_2 self, تۈگۈن_2) override { أرجع default_handler(self); }
		T reduce_or(تۈگۈن_2 self, تۈگۈن_2) override { أرجع default_handler(self); }
		T reduce_xor(تۈگۈن_2 self, تۈگۈن_2) override { أرجع default_handler(self); }
		T equal(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T not_equal(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T signed_greater_than(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T signed_greater_equal(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T unsigned_greater_than(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T unsigned_greater_equal(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T logical_shift_سول(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T logical_shift_ئوڭ(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T arithmetic_shift_ئوڭ(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T mux(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T constant(تۈگۈن_2 self, RTLIL::Const ثابت &) override { أرجع default_handler(self); }
		T كىرگۈزۈش(تۈگۈن_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T state(تۈگۈن_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T memory_read(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
		T memory_write(تۈگۈن_2 self, تۈگۈن_2, تۈگۈن_2, تۈگۈن_2) override { أرجع default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		تۈگۈن_2 add(IR::تۈگۈن_Data &&fn, Sort ثابت &sort, std::initializer_list<تۈگۈن_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			لكل (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			أرجع تۈگۈن_2(ref, _ir.design);
		}
		فراغ check_basic_ئىككىلىك(تۈگۈن_2 ثابت &a, تۈگۈن_2 ثابت &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		فراغ check_shift(تۈگۈن_2 ثابت &a, تۈگۈن_2 ثابت &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		فراغ check_unary(تۈگۈن_2 ثابت &a) { log_assert(a.sort().is_signal()); }
	عام:
		IR &ir() { أرجع _ir; }
		تۈگۈن_2 slice(تۈگۈن_2 a, صحيح offset, صحيح out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			إذا(offset == 0 && out_width == a.width())
				أرجع a;
			أرجع add(IR::تۈگۈن_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		تۈگۈن_2 extend(تۈگۈن_2 a, صحيح out_width, bool is_signed) {
			صحيح in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			إذا(in_width == out_width)
				أرجع a;
			إذا(in_width > out_width)
				أرجع slice(a, 0, out_width);
			إذا(is_signed)
				أرجع add(Fn::sign_extend, Sort(out_width), {a});
			وإلا
				أرجع add(Fn::zero_extend, Sort(out_width), {a});
		}
		تۈگۈن_2 concat(تۈگۈن_2 a, تۈگۈن_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			أرجع add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		تۈگۈن_2 add(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::add, a.sort(), {a, b}); }
		تۈگۈن_2 sub(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::sub, a.sort(), {a, b}); }
		تۈگۈن_2 mul(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::mul, a.sort(), {a, b}); }
		تۈگۈن_2 unsigned_div(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::unsigned_div, a.sort(), {a, b}); }
		تۈگۈن_2 unsigned_mod(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::unsigned_mod, a.sort(), {a, b}); }
		تۈگۈن_2 bitwise_and(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::bitwise_and, a.sort(), {a, b}); }
		تۈگۈن_2 bitwise_or(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::bitwise_or, a.sort(), {a, b}); }
		تۈگۈن_2 bitwise_xor(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::bitwise_xor, a.sort(), {a, b}); }
		تۈگۈن_2 bitwise_not(تۈگۈن_2 a) { check_unary(a); أرجع add(Fn::bitwise_not, a.sort(), {a}); }
		تۈگۈن_2 unary_minus(تۈگۈن_2 a) { check_unary(a); أرجع add(Fn::unary_minus, a.sort(), {a}); }
		تۈگۈن_2 reduce_and(تۈگۈن_2 a) {
			check_unary(a);
			إذا(a.width() == 1)
				أرجع a;
			أرجع add(Fn::reduce_and, Sort(1), {a});
		}
		تۈگۈن_2 reduce_or(تۈگۈن_2 a) {
			check_unary(a);
			إذا(a.width() == 1)
				أرجع a;
			أرجع add(Fn::reduce_or, Sort(1), {a});
		}
		تۈگۈن_2 reduce_xor(تۈگۈن_2 a) {
			check_unary(a);
			إذا(a.width() == 1)
				أرجع a;
			أرجع add(Fn::reduce_xor, Sort(1), {a});
		}
		تۈگۈن_2 equal(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::equal, Sort(1), {a, b}); }
		تۈگۈن_2 not_equal(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::not_equal, Sort(1), {a, b}); }
		تۈگۈن_2 signed_greater_than(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::signed_greater_than, Sort(1), {a, b}); }
		تۈگۈن_2 signed_greater_equal(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		تۈگۈن_2 unsigned_greater_than(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		تۈگۈن_2 unsigned_greater_equal(تۈگۈن_2 a, تۈگۈن_2 b) { check_basic_ئىككىلىك(a, b); أرجع add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		تۈگۈن_2 logical_shift_سول(تۈگۈن_2 a, تۈگۈن_2 b) { check_shift(a, b); أرجع add(Fn::logical_shift_سول, a.sort(), {a, b}); }
		تۈگۈن_2 logical_shift_ئوڭ(تۈگۈن_2 a, تۈگۈن_2 b) { check_shift(a, b); أرجع add(Fn::logical_shift_ئوڭ, a.sort(), {a, b}); }
		تۈگۈن_2 arithmetic_shift_ئوڭ(تۈگۈن_2 a, تۈگۈن_2 b) { check_shift(a, b); أرجع add(Fn::arithmetic_shift_ئوڭ, a.sort(), {a, b}); }
		تۈگۈن_2 mux(تۈگۈن_2 a, تۈگۈن_2 b, تۈگۈن_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			أرجع add(Fn::mux, a.sort(), {a, b, s});
		}
		تۈگۈن_2 memory_read(تۈگۈن_2 mem, تۈگۈن_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			أرجع add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		تۈگۈن_2 memory_write(تۈگۈن_2 mem, تۈگۈن_2 addr, تۈگۈن_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			أرجع add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		تۈگۈن_2 constant(RTLIL::Const قىممەت) {
			صحيح s = قىممەت.size();
			أرجع add(IR::تۈگۈن_Data(Fn::constant, std::move(قىممەت)), Sort(s), {});
		}
		تۈگۈن_2 create_pending(صحيح width) {
			أرجع add(Fn::buf, Sort(width), {});
		}
		فراغ update_pending(تۈگۈن_2 تۈگۈن, تۈگۈن_2 قىممەت) {
			log_assert(تۈگۈن._ref.function() == Fn::buf && تۈگۈن._ref.size() == 0);
			log_assert(تۈگۈن.sort() == قىممەت.sort());
			_ir.mutate(تۈگۈن).append_arg(قىممەت._ref);
		}
		IRInput &add_كىرگۈزۈش(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			إذا (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			إذا (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			إذا (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->second;
		}
		تۈگۈن_2 قىممەت(IRInput ثابت& كىرگۈزۈش) {
			أرجع add(IR::تۈگۈن_Data(Fn::كىرگۈزۈش, std::pair(كىرگۈزۈش.name, كىرگۈزۈش.kind)), كىرگۈزۈش.sort, {});
		}
		تۈگۈن_2 قىممەت(IRState ثابت& state) {
			أرجع add(IR::تۈگۈن_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		فراغ suggest_name(تۈگۈن_2 تۈگۈن, IdString name) {
			_ir.mutate(تۈگۈن).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	مضمن Factory IR::factory() { أرجع Factory(*this); }
	template<class Id> class Scope {
	محمي:
		حرف substitution_character = '_';
		virtual bool is_character_legal(حرف, صحيح) = 0;
	خاص:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	عام:
		فراغ reserve(std::string name) {
			_used_names.قىستۇرۇش(std::move(name));
		}
		template<typename N> std::string unique_name(ثابت N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			لكل(size_t i = 0; i < str.size(); i++)
				إذا(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			إذا(_used_names.سانى(str) == 0) {
				_used_names.قىستۇرۇش(str);
				أرجع str;
			}
			لكل (صحيح idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				إذا(_used_names.سانى(suffixed) == 0) {
					_used_names.قىستۇرۇش(suffixed);
					أرجع suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, ثابت N &suggestion) {
			auto it = _by_id.find(id);
			إذا(it != _by_id.end())
				أرجع it->second;
			std::string str = unique_name(suggestion);
			_by_id.قىستۇرۇش({id, str});
			أرجع str;
		}
	};
	class Writer {
		std::ostream *os;
		فراغ print_impl(ثابت حرف *fmt, vector<std::function<فراغ()>>& fns);
	عام:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); أرجع *this; }
		template<typename... Args>
		فراغ print(ثابت حرف *fmt, Args&&... args)
		{
			vector<std::function<فراغ()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		فراغ print_with(Fn fn, ثابت حرف *fmt, Args&&... args)
		{
			vector<std::function<فراغ()>> fns { [&]() {
				إذا constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				وإلا
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
