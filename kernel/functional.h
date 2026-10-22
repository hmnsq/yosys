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

#अन्तर्भाव "kernel/yosys.h"
#अन्तर्भाव "kernel/compute_graph.h"
#अन्तर्भाव "kernel/drivertools.h"
#अन्तर्भाव "kernel/mem.h"
#अन्तर्भाव "kernel/utils.h"

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
	गणना class Fn {
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
		logical_shift_डावा,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_उजवा,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_उजवा,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		आदान,
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
	स्थिरांक अक्षर *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<पूर्णांक, std::pair<पूर्णांक, पूर्णांक>> _v;
	सार्वजनिक:
		explicit Sort(पूर्णांक width) : _v(width) { }
		Sort(पूर्णांक addr_width, पूर्णांक data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() स्थिरांक { परत _v.index() == 0; }
		bool is_memory() स्थिरांक { परत _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		पूर्णांक width() स्थिरांक { परत std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		पूर्णांक addr_width() स्थिरांक { परत std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		पूर्णांक data_width() स्थिरांक { परत std::get<1>(_v).second; }
		bool चालक==(Sort स्थिरांक& other) स्थिरांक { परत _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक { h.eat(_v); परत h; }
	};
	class IR;
	class Factory;
	class गाठ_2;
	class IRInput {
		मित्र class Factory;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	खाजगी:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		मित्र class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	खाजगी:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	सार्वजनिक:
		गाठ_2 मूल्य() स्थिरांक;
		bool has_मूल्य() स्थिरांक;
		शून्य set_मूल्य(गाठ_2 मूल्य);
	};
	class IRState {
		मित्र class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	खाजगी:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	सार्वजनिक:
		गाठ_2 next_मूल्य() स्थिरांक;
		bool has_next_मूल्य() स्थिरांक;
		RTLIL::Const स्थिरांक& initial_मूल्य_signal() स्थिरांक { परत std::get<RTLIL::Const>(_initial); }
		MemContents स्थिरांक& initial_मूल्य_memory() स्थिरांक { परत std::get<MemContents>(_initial); }
		शून्य set_next_मूल्य(गाठ_2 मूल्य);
		शून्य set_initial_मूल्य(RTLIL::Const मूल्य) { मूल्य.extu(sort.width()); _initial = std::move(मूल्य); }
		शून्य set_initial_मूल्य(MemContents मूल्य) { log_assert(Sort(मूल्य.addr_width(), मूल्य.data_width()) == sort); _initial = std::move(मूल्य); }
	};
	class IR {
		मित्र class Factory;
		मित्र class गाठ_2;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class गाठ_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				पूर्णांक
			> _extra;
		सार्वजनिक:
			गाठ_Data() : _fn(Fn::invalid) {}
			गाठ_Data(Fn fn) : _fn(fn) {}
			ढाँचा<class T> गाठ_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() स्थिरांक { परत _fn; }
			स्थिरांक RTLIL::Const &as_const() स्थिरांक { परत std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() स्थिरांक { परत std::get<std::pair<IdString, IdString>>(_extra); }
			पूर्णांक as_int() स्थिरांक { परत std::get<पूर्णांक>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) स्थिरांक {
				h.eat((अचिन्हित पूर्णांक) _fn);
				h.eat(_extra);
				परत h;
			}
			bool चालक==(गाठ_Data स्थिरांक &other) स्थिरांक {
				परत _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		संरचना Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<गाठ_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(गाठ_2 n);
	सार्वजनिक:
		Design *design = nullptr;
		स्थिर IR from_module(Module *module);
		Factory factory();
		पूर्णांक size() स्थिरांक { परत _graph.size(); }
		गाठ_2 चालक[](पूर्णांक i);
		शून्य topological_sort();
		शून्य forward_buf();
		IRInput स्थिरांक& आदान(IdString name, IdString kind) स्थिरांक { परत _inputs.at({name, kind}); }
		IRInput स्थिरांक& आदान(IdString name) स्थिरांक { परत आदान(name, ID($आदान)); }
		IROutput स्थिरांक& output(IdString name, IdString kind) स्थिरांक { परत _outputs.at({name, kind}); }
		IROutput स्थिरांक& output(IdString name) स्थिरांक { परत output(name, ID($output)); }
		IRState स्थिरांक& state(IdString name, IdString kind) स्थिरांक { परत _states.at({name, kind}); }
		IRState स्थिरांक& state(IdString name) स्थिरांक { परत state(name, ID($state)); }
		bool has_आदान(IdString name, IdString kind) स्थिरांक { परत _inputs.मोजणी({name, kind}); }
		bool has_output(IdString name, IdString kind) स्थिरांक { परत _outputs.मोजणी({name, kind}); }
		bool has_state(IdString name, IdString kind) स्थिरांक { परत _states.मोजणी({name, kind}); }
		vector<IRInput स्थिरांक*> inputs(IdString kind) स्थिरांक;
		vector<IRInput स्थिरांक*> inputs() स्थिरांक { परत inputs(ID($आदान)); }
		vector<IROutput स्थिरांक*> outputs(IdString kind) स्थिरांक;
		vector<IROutput स्थिरांक*> outputs() स्थिरांक { परत outputs(ID($output)); }
		vector<IRState स्थिरांक*> states(IdString kind) स्थिरांक;
		vector<IRState स्थिरांक*> states() स्थिरांक { परत states(ID($state)); }
		vector<IRInput स्थिरांक*> all_inputs() स्थिरांक;
		vector<IROutput स्थिरांक*> all_outputs() स्थिरांक;
		vector<IRState स्थिरांक*> all_states() स्थिरांक;
		class iterator {
			मित्र class IR;
			IR *_ir;
			पूर्णांक _index;
			iterator(IR *ir, पूर्णांक index) : _ir(ir), _index(index) {}
		सार्वजनिक:
			using iterator_category = std::आदान_iterator_tag;
			using मूल्य_type = गाठ_2;
			using pointer = arrow_proxy<गाठ_2>;
			using reference = गाठ_2;
			using difference_type = सूचक_भेद_प्रकार;
			गाठ_2 चालक*();
			iterator &चालक++() { _index++; परत *this; }
			bool चालक!=(iterator स्थिरांक &other) स्थिरांक { परत _ir != other._ir || _index != other._index; }
			bool चालक==(iterator स्थिरांक &other) स्थिरांक { परत !(*this != other); }
			pointer चालक->();
		};
		iterator begin() { परत iterator(this, 0); }
		iterator end() { परत iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class गाठ_2 {
		मित्र class Factory;
		मित्र class IR;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		IR::Graph::ConstRef _ref;
		गाठ_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit चालक IR::Graph::ConstRef() { परत _ref; }
	सार्वजनिक:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		पूर्णांक id() स्थिरांक { परत _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() स्थिरांक {
			जर(_ref.has_sparse_attr())
				परत {design, _ref.sparse_attr()};
			अन्यथा
				परत {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() स्थिरांक { परत _ref.function().fn(); }
		Sort sort() स्थिरांक { परत _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		पूर्णांक width() स्थिरांक { परत sort().width(); }
		माप_प्रकार arg_मोजणी() स्थिरांक { परत _ref.size(); }
		गाठ_2 arg(पूर्णांक n) स्थिरांक { परत गाठ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		ढाँचा<class Visitor> स्वतः visit(Visitor v) स्थिरांक
		{
			// currently templated but could be switched to AbstractVisitor &
			निवड(_ref.function().fn()) {
			स्थिती Fn::invalid: log_error("invalid node in visit\n"); थांबा;
			स्थिती Fn::buf: परत v.buf(*this, arg(0)); थांबा;
			स्थिती Fn::slice: परत v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); थांबा;
			स्थिती Fn::zero_extend: परत v.zero_extend(*this, arg(0), width()); थांबा;
			स्थिती Fn::sign_extend: परत v.sign_extend(*this, arg(0), width()); थांबा;
			स्थिती Fn::concat: परत v.concat(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::add: परत v.add(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::sub: परत v.sub(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::mul: परत v.mul(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::unsigned_div: परत v.unsigned_div(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::unsigned_mod: परत v.unsigned_mod(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::bitwise_and: परत v.bitwise_and(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::bitwise_or: परत v.bitwise_or(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::bitwise_xor: परत v.bitwise_xor(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::bitwise_not: परत v.bitwise_not(*this, arg(0)); थांबा;
			स्थिती Fn::unary_minus: परत v.unary_minus(*this, arg(0)); थांबा;
			स्थिती Fn::reduce_and: परत v.reduce_and(*this, arg(0)); थांबा;
			स्थिती Fn::reduce_or: परत v.reduce_or(*this, arg(0)); थांबा;
			स्थिती Fn::reduce_xor: परत v.reduce_xor(*this, arg(0)); थांबा;
			स्थिती Fn::equal: परत v.equal(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::not_equal: परत v.not_equal(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::signed_greater_than: परत v.signed_greater_than(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::signed_greater_equal: परत v.signed_greater_equal(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::unsigned_greater_than: परत v.unsigned_greater_than(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::unsigned_greater_equal: परत v.unsigned_greater_equal(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::logical_shift_डावा: परत v.logical_shift_डावा(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::logical_shift_उजवा: परत v.logical_shift_उजवा(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::arithmetic_shift_उजवा: परत v.arithmetic_shift_उजवा(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::mux: परत v.mux(*this, arg(0), arg(1), arg(2)); थांबा;
			स्थिती Fn::constant: परत v.constant(*this, _ref.function().as_const()); थांबा;
			स्थिती Fn::आदान: परत v.आदान(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); थांबा;
			स्थिती Fn::state: परत v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); थांबा;
			स्थिती Fn::memory_read: परत v.memory_read(*this, arg(0), arg(1)); थांबा;
			स्थिती Fn::memory_write: परत v.memory_write(*this, arg(0), arg(1), arg(2)); थांबा;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(गाठ_2)>);
	};
	ओळीत IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ओळीत IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ओळीत IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ओळीत IR::Graph::Ref IR::mutate(गाठ_2 n) { परत _graph[n._ref.index()]; }
	ओळीत गाठ_2 IR::चालक[](पूर्णांक i) { परत गाठ_2(_graph[i], design); }
	ओळीत गाठ_2 IROutput::मूल्य() स्थिरांक { परत गाठ_2(_ir._graph({name, kind, false}), _ir.design); }
	ओळीत bool IROutput::has_मूल्य() स्थिरांक { परत _ir._graph.has_किल्ली({name, kind, false}); }
	ओळीत शून्य IROutput::set_मूल्य(गाठ_2 मूल्य) { log_assert(sort == मूल्य.sort()); _ir.mutate(मूल्य).assign_किल्ली({name, kind, false}); }
	ओळीत गाठ_2 IRState::next_मूल्य() स्थिरांक { परत गाठ_2(_ir._graph({name, kind, true}), _ir.design); }
	ओळीत bool IRState::has_next_मूल्य() स्थिरांक { परत _ir._graph.has_किल्ली({name, kind, true}); }
	ओळीत शून्य IRState::set_next_मूल्य(गाठ_2 मूल्य) { log_assert(sort == मूल्य.sort()); _ir.mutate(मूल्य).assign_किल्ली({name, kind, true}); }
	ओळीत गाठ_2 IR::iterator::चालक*() { परत गाठ_2(_ir->_graph[_index], _ir->design); }
	ओळीत arrow_proxy<गाठ_2> IR::iterator::चालक->() { परत arrow_proxy<गाठ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	ढाँचा<class T> संरचना AbstractVisitor {
		आभासी T buf(गाठ_2 self, गाठ_2 n) = 0;
		आभासी T slice(गाठ_2 self, गाठ_2 a, पूर्णांक offset, पूर्णांक out_width) = 0;
		आभासी T zero_extend(गाठ_2 self, गाठ_2 a, पूर्णांक out_width) = 0;
		आभासी T sign_extend(गाठ_2 self, गाठ_2 a, पूर्णांक out_width) = 0;
		आभासी T concat(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T add(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T sub(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T mul(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T unsigned_div(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T unsigned_mod(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T bitwise_and(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T bitwise_or(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T bitwise_xor(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T bitwise_not(गाठ_2 self, गाठ_2 a) = 0;
		आभासी T unary_minus(गाठ_2 self, गाठ_2 a) = 0;
		आभासी T reduce_and(गाठ_2 self, गाठ_2 a) = 0;
		आभासी T reduce_or(गाठ_2 self, गाठ_2 a) = 0;
		आभासी T reduce_xor(गाठ_2 self, गाठ_2 a) = 0;
		आभासी T equal(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T not_equal(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T signed_greater_than(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T signed_greater_equal(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T unsigned_greater_than(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T unsigned_greater_equal(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T logical_shift_डावा(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T logical_shift_उजवा(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T arithmetic_shift_उजवा(गाठ_2 self, गाठ_2 a, गाठ_2 b) = 0;
		आभासी T mux(गाठ_2 self, गाठ_2 a, गाठ_2 b, गाठ_2 s) = 0;
		आभासी T constant(गाठ_2 self, RTLIL::Const स्थिरांक & मूल्य) = 0;
		आभासी T आदान(गाठ_2 self, IdString name, IdString kind) = 0;
		आभासी T state(गाठ_2 self, IdString name, IdString kind) = 0;
		आभासी T memory_read(गाठ_2 self, गाठ_2 mem, गाठ_2 addr) = 0;
		आभासी T memory_write(गाठ_2 self, गाठ_2 mem, गाठ_2 addr, गाठ_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	ढाँचा<class T> संरचना DefaultVisitor : सार्वजनिक AbstractVisitor<T> {
		आभासी T default_handler(गाठ_2 self) = 0;
		T buf(गाठ_2 self, गाठ_2) override { परत default_handler(self); }
		T slice(गाठ_2 self, गाठ_2, पूर्णांक, पूर्णांक) override { परत default_handler(self); }
		T zero_extend(गाठ_2 self, गाठ_2, पूर्णांक) override { परत default_handler(self); }
		T sign_extend(गाठ_2 self, गाठ_2, पूर्णांक) override { परत default_handler(self); }
		T concat(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T add(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T sub(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T mul(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T unsigned_div(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T unsigned_mod(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T bitwise_and(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T bitwise_or(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T bitwise_xor(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T bitwise_not(गाठ_2 self, गाठ_2) override { परत default_handler(self); }
		T unary_minus(गाठ_2 self, गाठ_2) override { परत default_handler(self); }
		T reduce_and(गाठ_2 self, गाठ_2) override { परत default_handler(self); }
		T reduce_or(गाठ_2 self, गाठ_2) override { परत default_handler(self); }
		T reduce_xor(गाठ_2 self, गाठ_2) override { परत default_handler(self); }
		T equal(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T not_equal(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T signed_greater_than(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T signed_greater_equal(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T unsigned_greater_than(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T unsigned_greater_equal(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T logical_shift_डावा(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T logical_shift_उजवा(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T arithmetic_shift_उजवा(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T mux(गाठ_2 self, गाठ_2, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T constant(गाठ_2 self, RTLIL::Const स्थिरांक &) override { परत default_handler(self); }
		T आदान(गाठ_2 self, IdString, IdString) override { परत default_handler(self); }
		T state(गाठ_2 self, IdString, IdString) override { परत default_handler(self); }
		T memory_read(गाठ_2 self, गाठ_2, गाठ_2) override { परत default_handler(self); }
		T memory_write(गाठ_2 self, गाठ_2, गाठ_2, गाठ_2) override { परत default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		मित्र class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		गाठ_2 add(IR::गाठ_Data &&fn, Sort स्थिरांक &sort, std::initializer_list<गाठ_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			साठी (स्वतः arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			परत गाठ_2(ref, _ir.design);
		}
		शून्य check_basic_द्विमान(गाठ_2 स्थिरांक &a, गाठ_2 स्थिरांक &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		शून्य check_shift(गाठ_2 स्थिरांक &a, गाठ_2 स्थिरांक &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		शून्य check_unary(गाठ_2 स्थिरांक &a) { log_assert(a.sort().is_signal()); }
	सार्वजनिक:
		IR &ir() { परत _ir; }
		गाठ_2 slice(गाठ_2 a, पूर्णांक offset, पूर्णांक out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			जर(offset == 0 && out_width == a.width())
				परत a;
			परत add(IR::गाठ_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		गाठ_2 extend(गाठ_2 a, पूर्णांक out_width, bool is_signed) {
			पूर्णांक in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			जर(in_width == out_width)
				परत a;
			जर(in_width > out_width)
				परत slice(a, 0, out_width);
			जर(is_signed)
				परत add(Fn::sign_extend, Sort(out_width), {a});
			अन्यथा
				परत add(Fn::zero_extend, Sort(out_width), {a});
		}
		गाठ_2 concat(गाठ_2 a, गाठ_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			परत add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		गाठ_2 add(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::add, a.sort(), {a, b}); }
		गाठ_2 sub(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::sub, a.sort(), {a, b}); }
		गाठ_2 mul(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::mul, a.sort(), {a, b}); }
		गाठ_2 unsigned_div(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::unsigned_div, a.sort(), {a, b}); }
		गाठ_2 unsigned_mod(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::unsigned_mod, a.sort(), {a, b}); }
		गाठ_2 bitwise_and(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::bitwise_and, a.sort(), {a, b}); }
		गाठ_2 bitwise_or(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::bitwise_or, a.sort(), {a, b}); }
		गाठ_2 bitwise_xor(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::bitwise_xor, a.sort(), {a, b}); }
		गाठ_2 bitwise_not(गाठ_2 a) { check_unary(a); परत add(Fn::bitwise_not, a.sort(), {a}); }
		गाठ_2 unary_minus(गाठ_2 a) { check_unary(a); परत add(Fn::unary_minus, a.sort(), {a}); }
		गाठ_2 reduce_and(गाठ_2 a) {
			check_unary(a);
			जर(a.width() == 1)
				परत a;
			परत add(Fn::reduce_and, Sort(1), {a});
		}
		गाठ_2 reduce_or(गाठ_2 a) {
			check_unary(a);
			जर(a.width() == 1)
				परत a;
			परत add(Fn::reduce_or, Sort(1), {a});
		}
		गाठ_2 reduce_xor(गाठ_2 a) {
			check_unary(a);
			जर(a.width() == 1)
				परत a;
			परत add(Fn::reduce_xor, Sort(1), {a});
		}
		गाठ_2 equal(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::equal, Sort(1), {a, b}); }
		गाठ_2 not_equal(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::not_equal, Sort(1), {a, b}); }
		गाठ_2 signed_greater_than(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::signed_greater_than, Sort(1), {a, b}); }
		गाठ_2 signed_greater_equal(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		गाठ_2 unsigned_greater_than(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		गाठ_2 unsigned_greater_equal(गाठ_2 a, गाठ_2 b) { check_basic_द्विमान(a, b); परत add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		गाठ_2 logical_shift_डावा(गाठ_2 a, गाठ_2 b) { check_shift(a, b); परत add(Fn::logical_shift_डावा, a.sort(), {a, b}); }
		गाठ_2 logical_shift_उजवा(गाठ_2 a, गाठ_2 b) { check_shift(a, b); परत add(Fn::logical_shift_उजवा, a.sort(), {a, b}); }
		गाठ_2 arithmetic_shift_उजवा(गाठ_2 a, गाठ_2 b) { check_shift(a, b); परत add(Fn::arithmetic_shift_उजवा, a.sort(), {a, b}); }
		गाठ_2 mux(गाठ_2 a, गाठ_2 b, गाठ_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			परत add(Fn::mux, a.sort(), {a, b, s});
		}
		गाठ_2 memory_read(गाठ_2 mem, गाठ_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			परत add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		गाठ_2 memory_write(गाठ_2 mem, गाठ_2 addr, गाठ_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			परत add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		गाठ_2 constant(RTLIL::Const मूल्य) {
			पूर्णांक s = मूल्य.size();
			परत add(IR::गाठ_Data(Fn::constant, std::move(मूल्य)), Sort(s), {});
		}
		गाठ_2 create_pending(पूर्णांक width) {
			परत add(Fn::buf, Sort(width), {});
		}
		शून्य update_pending(गाठ_2 गाठ, गाठ_2 मूल्य) {
			log_assert(गाठ._ref.function() == Fn::buf && गाठ._ref.size() == 0);
			log_assert(गाठ.sort() == मूल्य.sort());
			_ir.mutate(गाठ).append_arg(मूल्य._ref);
		}
		IRInput &add_आदान(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			जर (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			परत it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			जर (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			परत it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			जर (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			परत it->second;
		}
		गाठ_2 मूल्य(IRInput स्थिरांक& आदान) {
			परत add(IR::गाठ_Data(Fn::आदान, std::pair(आदान.name, आदान.kind)), आदान.sort, {});
		}
		गाठ_2 मूल्य(IRState स्थिरांक& state) {
			परत add(IR::गाठ_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		शून्य suggest_name(गाठ_2 गाठ, IdString name) {
			_ir.mutate(गाठ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	ओळीत Factory IR::factory() { परत Factory(*this); }
	ढाँचा<class Id> class Scope {
	संरक्षित:
		अक्षर substitution_character = '_';
		आभासी bool is_character_legal(अक्षर, पूर्णांक) = 0;
	खाजगी:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	सार्वजनिक:
		शून्य reserve(std::string name) {
			_used_names.घाला(std::move(name));
		}
		ढाँचा<typename N> std::string unique_name(स्थिरांक N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			साठी(माप_प्रकार i = 0; i < str.size(); i++)
				जर(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			जर(_used_names.मोजणी(str) == 0) {
				_used_names.घाला(str);
				परत str;
			}
			साठी (पूर्णांक idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				जर(_used_names.मोजणी(suffixed) == 0) {
					_used_names.घाला(suffixed);
					परत suffixed;
				}
			}
		}
		ढाँचा<typename N> std::string चालक()(Id id, स्थिरांक N &suggestion) {
			स्वतः it = _by_id.find(id);
			जर(it != _by_id.end())
				परत it->second;
			std::string str = unique_name(suggestion);
			_by_id.घाला({id, str});
			परत str;
		}
	};
	class Writer {
		std::ostream *os;
		शून्य print_impl(स्थिरांक अक्षर *fmt, vector<std::function<शून्य()>>& fns);
	सार्वजनिक:
		Writer(std::ostream &os) : os(&os) {}
		ढाँचा<class T> Writer& चालक <<(T&& arg) { *os << std::forward<T>(arg); परत *this; }
		ढाँचा<typename... Args>
		शून्य print(स्थिरांक अक्षर *fmt, Args&&... args)
		{
			vector<std::function<शून्य()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		ढाँचा<typename Fn, typename... Args>
		शून्य print_with(Fn fn, स्थिरांक अक्षर *fmt, Args&&... args)
		{
			vector<std::function<शून्य()>> fns { [&]() {
				जर constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				अन्यथा
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
