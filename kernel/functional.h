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
		logical_shift_बायाँ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_दायाँ,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_दायाँ,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		आगत,
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
	अचर वर्ण *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<पूर्णाङ्क, std::pair<पूर्णाङ्क, पूर्णाङ्क>> _v;
	सार्वजनिक:
		explicit Sort(पूर्णाङ्क width) : _v(width) { }
		Sort(पूर्णाङ्क addr_width, पूर्णाङ्क data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() अचर { फर्काऊ _v.index() == 0; }
		bool is_memory() अचर { फर्काऊ _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क width() अचर { फर्काऊ std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क addr_width() अचर { फर्काऊ std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क data_width() अचर { फर्काऊ std::get<1>(_v).second; }
		bool चालक==(Sort अचर& other) अचर { फर्काऊ _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) अचर { h.eat(_v); फर्काऊ h; }
	};
	class IR;
	class Factory;
	class गाँठो_2;
	class IRInput {
		मित्र class Factory;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	गुप्त:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		मित्र class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	गुप्त:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	सार्वजनिक:
		गाँठो_2 मान() अचर;
		bool has_मान() अचर;
		शून्य set_मान(गाँठो_2 मान);
	};
	class IRState {
		मित्र class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	गुप्त:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	सार्वजनिक:
		गाँठो_2 next_मान() अचर;
		bool has_next_मान() अचर;
		RTLIL::Const अचर& initial_मान_signal() अचर { फर्काऊ std::get<RTLIL::Const>(_initial); }
		MemContents अचर& initial_मान_memory() अचर { फर्काऊ std::get<MemContents>(_initial); }
		शून्य set_next_मान(गाँठो_2 मान);
		शून्य set_initial_मान(RTLIL::Const मान) { मान.extu(sort.width()); _initial = std::move(मान); }
		शून्य set_initial_मान(MemContents मान) { log_assert(Sort(मान.addr_width(), मान.data_width()) == sort); _initial = std::move(मान); }
	};
	class IR {
		मित्र class Factory;
		मित्र class गाँठो_2;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class गाँठो_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				पूर्णाङ्क
			> _extra;
		सार्वजनिक:
			गाँठो_Data() : _fn(Fn::invalid) {}
			गाँठो_Data(Fn fn) : _fn(fn) {}
			ढाँचा<class T> गाँठो_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() अचर { फर्काऊ _fn; }
			अचर RTLIL::Const &as_const() अचर { फर्काऊ std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() अचर { फर्काऊ std::get<std::pair<IdString, IdString>>(_extra); }
			पूर्णाङ्क as_int() अचर { फर्काऊ std::get<पूर्णाङ्क>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) अचर {
				h.eat((अचिन्हित पूर्णाङ्क) _fn);
				h.eat(_extra);
				फर्काऊ h;
			}
			bool चालक==(गाँठो_Data अचर &other) अचर {
				फर्काऊ _fn == other._fn && _extra == other._extra;
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
		using Graph = ComputeGraph<गाँठो_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(गाँठो_2 n);
	सार्वजनिक:
		Design *design = nullptr;
		स्थिर IR from_module(Module *module);
		Factory factory();
		पूर्णाङ्क size() अचर { फर्काऊ _graph.size(); }
		गाँठो_2 चालक[](पूर्णाङ्क i);
		शून्य topological_sort();
		शून्य forward_buf();
		IRInput अचर& आगत(IdString name, IdString kind) अचर { फर्काऊ _inputs.at({name, kind}); }
		IRInput अचर& आगत(IdString name) अचर { फर्काऊ आगत(name, ID($आगत)); }
		IROutput अचर& output(IdString name, IdString kind) अचर { फर्काऊ _outputs.at({name, kind}); }
		IROutput अचर& output(IdString name) अचर { फर्काऊ output(name, ID($output)); }
		IRState अचर& state(IdString name, IdString kind) अचर { फर्काऊ _states.at({name, kind}); }
		IRState अचर& state(IdString name) अचर { फर्काऊ state(name, ID($state)); }
		bool has_आगत(IdString name, IdString kind) अचर { फर्काऊ _inputs.गन्ती({name, kind}); }
		bool has_output(IdString name, IdString kind) अचर { फर्काऊ _outputs.गन्ती({name, kind}); }
		bool has_state(IdString name, IdString kind) अचर { फर्काऊ _states.गन्ती({name, kind}); }
		vector<IRInput अचर*> inputs(IdString kind) अचर;
		vector<IRInput अचर*> inputs() अचर { फर्काऊ inputs(ID($आगत)); }
		vector<IROutput अचर*> outputs(IdString kind) अचर;
		vector<IROutput अचर*> outputs() अचर { फर्काऊ outputs(ID($output)); }
		vector<IRState अचर*> states(IdString kind) अचर;
		vector<IRState अचर*> states() अचर { फर्काऊ states(ID($state)); }
		vector<IRInput अचर*> all_inputs() अचर;
		vector<IROutput अचर*> all_outputs() अचर;
		vector<IRState अचर*> all_states() अचर;
		class iterator {
			मित्र class IR;
			IR *_ir;
			पूर्णाङ्क _index;
			iterator(IR *ir, पूर्णाङ्क index) : _ir(ir), _index(index) {}
		सार्वजनिक:
			using iterator_category = std::आगत_iterator_tag;
			using मान_type = गाँठो_2;
			using pointer = arrow_proxy<गाँठो_2>;
			using reference = गाँठो_2;
			using difference_type = सूचक_भेद_प्रकार;
			गाँठो_2 चालक*();
			iterator &चालक++() { _index++; फर्काऊ *this; }
			bool चालक!=(iterator अचर &other) अचर { फर्काऊ _ir != other._ir || _index != other._index; }
			bool चालक==(iterator अचर &other) अचर { फर्काऊ !(*this != other); }
			pointer चालक->();
		};
		iterator begin() { फर्काऊ iterator(this, 0); }
		iterator end() { फर्काऊ iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class गाँठो_2 {
		मित्र class Factory;
		मित्र class IR;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		IR::Graph::ConstRef _ref;
		गाँठो_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit चालक IR::Graph::ConstRef() { फर्काऊ _ref; }
	सार्वजनिक:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		पूर्णाङ्क id() अचर { फर्काऊ _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() अचर {
			यदि(_ref.has_sparse_attr())
				फर्काऊ {design, _ref.sparse_attr()};
			अन्यथा
				फर्काऊ {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() अचर { फर्काऊ _ref.function().fn(); }
		Sort sort() अचर { फर्काऊ _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		पूर्णाङ्क width() अचर { फर्काऊ sort().width(); }
		माप_प्रकार arg_गन्ती() अचर { फर्काऊ _ref.size(); }
		गाँठो_2 arg(पूर्णाङ्क n) अचर { फर्काऊ गाँठो_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		ढाँचा<class Visitor> स्वतः visit(Visitor v) अचर
		{
			// currently templated but could be switched to AbstractVisitor &
			विकल्प(_ref.function().fn()) {
			पक्ष Fn::invalid: log_error("invalid node in visit\n"); रोक;
			पक्ष Fn::buf: फर्काऊ v.buf(*this, arg(0)); रोक;
			पक्ष Fn::slice: फर्काऊ v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); रोक;
			पक्ष Fn::zero_extend: फर्काऊ v.zero_extend(*this, arg(0), width()); रोक;
			पक्ष Fn::sign_extend: फर्काऊ v.sign_extend(*this, arg(0), width()); रोक;
			पक्ष Fn::concat: फर्काऊ v.concat(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::add: फर्काऊ v.add(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::sub: फर्काऊ v.sub(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::mul: फर्काऊ v.mul(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::unsigned_div: फर्काऊ v.unsigned_div(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::unsigned_mod: फर्काऊ v.unsigned_mod(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::bitwise_and: फर्काऊ v.bitwise_and(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::bitwise_or: फर्काऊ v.bitwise_or(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::bitwise_xor: फर्काऊ v.bitwise_xor(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::bitwise_not: फर्काऊ v.bitwise_not(*this, arg(0)); रोक;
			पक्ष Fn::unary_minus: फर्काऊ v.unary_minus(*this, arg(0)); रोक;
			पक्ष Fn::reduce_and: फर्काऊ v.reduce_and(*this, arg(0)); रोक;
			पक्ष Fn::reduce_or: फर्काऊ v.reduce_or(*this, arg(0)); रोक;
			पक्ष Fn::reduce_xor: फर्काऊ v.reduce_xor(*this, arg(0)); रोक;
			पक्ष Fn::equal: फर्काऊ v.equal(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::not_equal: फर्काऊ v.not_equal(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::signed_greater_than: फर्काऊ v.signed_greater_than(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::signed_greater_equal: फर्काऊ v.signed_greater_equal(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::unsigned_greater_than: फर्काऊ v.unsigned_greater_than(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::unsigned_greater_equal: फर्काऊ v.unsigned_greater_equal(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::logical_shift_बायाँ: फर्काऊ v.logical_shift_बायाँ(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::logical_shift_दायाँ: फर्काऊ v.logical_shift_दायाँ(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::arithmetic_shift_दायाँ: फर्काऊ v.arithmetic_shift_दायाँ(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::mux: फर्काऊ v.mux(*this, arg(0), arg(1), arg(2)); रोक;
			पक्ष Fn::constant: फर्काऊ v.constant(*this, _ref.function().as_const()); रोक;
			पक्ष Fn::आगत: फर्काऊ v.आगत(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); रोक;
			पक्ष Fn::state: फर्काऊ v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); रोक;
			पक्ष Fn::memory_read: फर्काऊ v.memory_read(*this, arg(0), arg(1)); रोक;
			पक्ष Fn::memory_write: फर्काऊ v.memory_write(*this, arg(0), arg(1), arg(2)); रोक;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(गाँठो_2)>);
	};
	अन्तःस्थ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IR::Graph::Ref IR::mutate(गाँठो_2 n) { फर्काऊ _graph[n._ref.index()]; }
	अन्तःस्थ गाँठो_2 IR::चालक[](पूर्णाङ्क i) { फर्काऊ गाँठो_2(_graph[i], design); }
	अन्तःस्थ गाँठो_2 IROutput::मान() अचर { फर्काऊ गाँठो_2(_ir._graph({name, kind, false}), _ir.design); }
	अन्तःस्थ bool IROutput::has_मान() अचर { फर्काऊ _ir._graph.has_साँचो({name, kind, false}); }
	अन्तःस्थ शून्य IROutput::set_मान(गाँठो_2 मान) { log_assert(sort == मान.sort()); _ir.mutate(मान).assign_साँचो({name, kind, false}); }
	अन्तःस्थ गाँठो_2 IRState::next_मान() अचर { फर्काऊ गाँठो_2(_ir._graph({name, kind, true}), _ir.design); }
	अन्तःस्थ bool IRState::has_next_मान() अचर { फर्काऊ _ir._graph.has_साँचो({name, kind, true}); }
	अन्तःस्थ शून्य IRState::set_next_मान(गाँठो_2 मान) { log_assert(sort == मान.sort()); _ir.mutate(मान).assign_साँचो({name, kind, true}); }
	अन्तःस्थ गाँठो_2 IR::iterator::चालक*() { फर्काऊ गाँठो_2(_ir->_graph[_index], _ir->design); }
	अन्तःस्थ arrow_proxy<गाँठो_2> IR::iterator::चालक->() { फर्काऊ arrow_proxy<गाँठो_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	ढाँचा<class T> संरचना AbstractVisitor {
		आभास T buf(गाँठो_2 self, गाँठो_2 n) = 0;
		आभास T slice(गाँठो_2 self, गाँठो_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) = 0;
		आभास T zero_extend(गाँठो_2 self, गाँठो_2 a, पूर्णाङ्क out_width) = 0;
		आभास T sign_extend(गाँठो_2 self, गाँठो_2 a, पूर्णाङ्क out_width) = 0;
		आभास T concat(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T add(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T sub(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T mul(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T unsigned_div(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T unsigned_mod(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T bitwise_and(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T bitwise_or(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T bitwise_xor(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T bitwise_not(गाँठो_2 self, गाँठो_2 a) = 0;
		आभास T unary_minus(गाँठो_2 self, गाँठो_2 a) = 0;
		आभास T reduce_and(गाँठो_2 self, गाँठो_2 a) = 0;
		आभास T reduce_or(गाँठो_2 self, गाँठो_2 a) = 0;
		आभास T reduce_xor(गाँठो_2 self, गाँठो_2 a) = 0;
		आभास T equal(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T not_equal(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T signed_greater_than(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T signed_greater_equal(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T unsigned_greater_than(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T unsigned_greater_equal(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T logical_shift_बायाँ(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T logical_shift_दायाँ(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T arithmetic_shift_दायाँ(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b) = 0;
		आभास T mux(गाँठो_2 self, गाँठो_2 a, गाँठो_2 b, गाँठो_2 s) = 0;
		आभास T constant(गाँठो_2 self, RTLIL::Const अचर & मान) = 0;
		आभास T आगत(गाँठो_2 self, IdString name, IdString kind) = 0;
		आभास T state(गाँठो_2 self, IdString name, IdString kind) = 0;
		आभास T memory_read(गाँठो_2 self, गाँठो_2 mem, गाँठो_2 addr) = 0;
		आभास T memory_write(गाँठो_2 self, गाँठो_2 mem, गाँठो_2 addr, गाँठो_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	ढाँचा<class T> संरचना DefaultVisitor : सार्वजनिक AbstractVisitor<T> {
		आभास T default_handler(गाँठो_2 self) = 0;
		T buf(गाँठो_2 self, गाँठो_2) override { फर्काऊ default_handler(self); }
		T slice(गाँठो_2 self, गाँठो_2, पूर्णाङ्क, पूर्णाङ्क) override { फर्काऊ default_handler(self); }
		T zero_extend(गाँठो_2 self, गाँठो_2, पूर्णाङ्क) override { फर्काऊ default_handler(self); }
		T sign_extend(गाँठो_2 self, गाँठो_2, पूर्णाङ्क) override { फर्काऊ default_handler(self); }
		T concat(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T add(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T sub(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T mul(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T unsigned_div(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T unsigned_mod(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T bitwise_and(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T bitwise_or(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T bitwise_xor(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T bitwise_not(गाँठो_2 self, गाँठो_2) override { फर्काऊ default_handler(self); }
		T unary_minus(गाँठो_2 self, गाँठो_2) override { फर्काऊ default_handler(self); }
		T reduce_and(गाँठो_2 self, गाँठो_2) override { फर्काऊ default_handler(self); }
		T reduce_or(गाँठो_2 self, गाँठो_2) override { फर्काऊ default_handler(self); }
		T reduce_xor(गाँठो_2 self, गाँठो_2) override { फर्काऊ default_handler(self); }
		T equal(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T not_equal(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T signed_greater_than(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T signed_greater_equal(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T unsigned_greater_than(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T unsigned_greater_equal(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T logical_shift_बायाँ(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T logical_shift_दायाँ(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T arithmetic_shift_दायाँ(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T mux(गाँठो_2 self, गाँठो_2, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T constant(गाँठो_2 self, RTLIL::Const अचर &) override { फर्काऊ default_handler(self); }
		T आगत(गाँठो_2 self, IdString, IdString) override { फर्काऊ default_handler(self); }
		T state(गाँठो_2 self, IdString, IdString) override { फर्काऊ default_handler(self); }
		T memory_read(गाँठो_2 self, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
		T memory_write(गाँठो_2 self, गाँठो_2, गाँठो_2, गाँठो_2) override { फर्काऊ default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		मित्र class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		गाँठो_2 add(IR::गाँठो_Data &&fn, Sort अचर &sort, std::initializer_list<गाँठो_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			लागि (स्वतः arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			फर्काऊ गाँठो_2(ref, _ir.design);
		}
		शून्य check_basic_द्विआधारी(गाँठो_2 अचर &a, गाँठो_2 अचर &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		शून्य check_shift(गाँठो_2 अचर &a, गाँठो_2 अचर &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		शून्य check_unary(गाँठो_2 अचर &a) { log_assert(a.sort().is_signal()); }
	सार्वजनिक:
		IR &ir() { फर्काऊ _ir; }
		गाँठो_2 slice(गाँठो_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			यदि(offset == 0 && out_width == a.width())
				फर्काऊ a;
			फर्काऊ add(IR::गाँठो_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		गाँठो_2 extend(गाँठो_2 a, पूर्णाङ्क out_width, bool is_signed) {
			पूर्णाङ्क in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			यदि(in_width == out_width)
				फर्काऊ a;
			यदि(in_width > out_width)
				फर्काऊ slice(a, 0, out_width);
			यदि(is_signed)
				फर्काऊ add(Fn::sign_extend, Sort(out_width), {a});
			अन्यथा
				फर्काऊ add(Fn::zero_extend, Sort(out_width), {a});
		}
		गाँठो_2 concat(गाँठो_2 a, गाँठो_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			फर्काऊ add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		गाँठो_2 add(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::add, a.sort(), {a, b}); }
		गाँठो_2 sub(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::sub, a.sort(), {a, b}); }
		गाँठो_2 mul(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::mul, a.sort(), {a, b}); }
		गाँठो_2 unsigned_div(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::unsigned_div, a.sort(), {a, b}); }
		गाँठो_2 unsigned_mod(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::unsigned_mod, a.sort(), {a, b}); }
		गाँठो_2 bitwise_and(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::bitwise_and, a.sort(), {a, b}); }
		गाँठो_2 bitwise_or(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::bitwise_or, a.sort(), {a, b}); }
		गाँठो_2 bitwise_xor(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::bitwise_xor, a.sort(), {a, b}); }
		गाँठो_2 bitwise_not(गाँठो_2 a) { check_unary(a); फर्काऊ add(Fn::bitwise_not, a.sort(), {a}); }
		गाँठो_2 unary_minus(गाँठो_2 a) { check_unary(a); फर्काऊ add(Fn::unary_minus, a.sort(), {a}); }
		गाँठो_2 reduce_and(गाँठो_2 a) {
			check_unary(a);
			यदि(a.width() == 1)
				फर्काऊ a;
			फर्काऊ add(Fn::reduce_and, Sort(1), {a});
		}
		गाँठो_2 reduce_or(गाँठो_2 a) {
			check_unary(a);
			यदि(a.width() == 1)
				फर्काऊ a;
			फर्काऊ add(Fn::reduce_or, Sort(1), {a});
		}
		गाँठो_2 reduce_xor(गाँठो_2 a) {
			check_unary(a);
			यदि(a.width() == 1)
				फर्काऊ a;
			फर्काऊ add(Fn::reduce_xor, Sort(1), {a});
		}
		गाँठो_2 equal(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::equal, Sort(1), {a, b}); }
		गाँठो_2 not_equal(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::not_equal, Sort(1), {a, b}); }
		गाँठो_2 signed_greater_than(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::signed_greater_than, Sort(1), {a, b}); }
		गाँठो_2 signed_greater_equal(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		गाँठो_2 unsigned_greater_than(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		गाँठो_2 unsigned_greater_equal(गाँठो_2 a, गाँठो_2 b) { check_basic_द्विआधारी(a, b); फर्काऊ add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		गाँठो_2 logical_shift_बायाँ(गाँठो_2 a, गाँठो_2 b) { check_shift(a, b); फर्काऊ add(Fn::logical_shift_बायाँ, a.sort(), {a, b}); }
		गाँठो_2 logical_shift_दायाँ(गाँठो_2 a, गाँठो_2 b) { check_shift(a, b); फर्काऊ add(Fn::logical_shift_दायाँ, a.sort(), {a, b}); }
		गाँठो_2 arithmetic_shift_दायाँ(गाँठो_2 a, गाँठो_2 b) { check_shift(a, b); फर्काऊ add(Fn::arithmetic_shift_दायाँ, a.sort(), {a, b}); }
		गाँठो_2 mux(गाँठो_2 a, गाँठो_2 b, गाँठो_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			फर्काऊ add(Fn::mux, a.sort(), {a, b, s});
		}
		गाँठो_2 memory_read(गाँठो_2 mem, गाँठो_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			फर्काऊ add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		गाँठो_2 memory_write(गाँठो_2 mem, गाँठो_2 addr, गाँठो_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			फर्काऊ add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		गाँठो_2 constant(RTLIL::Const मान) {
			पूर्णाङ्क s = मान.size();
			फर्काऊ add(IR::गाँठो_Data(Fn::constant, std::move(मान)), Sort(s), {});
		}
		गाँठो_2 create_pending(पूर्णाङ्क width) {
			फर्काऊ add(Fn::buf, Sort(width), {});
		}
		शून्य update_pending(गाँठो_2 गाँठो, गाँठो_2 मान) {
			log_assert(गाँठो._ref.function() == Fn::buf && गाँठो._ref.size() == 0);
			log_assert(गाँठो.sort() == मान.sort());
			_ir.mutate(गाँठो).append_arg(मान._ref);
		}
		IRInput &add_आगत(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			यदि (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			फर्काऊ it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			यदि (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			फर्काऊ it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			यदि (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			फर्काऊ it->second;
		}
		गाँठो_2 मान(IRInput अचर& आगत) {
			फर्काऊ add(IR::गाँठो_Data(Fn::आगत, std::pair(आगत.name, आगत.kind)), आगत.sort, {});
		}
		गाँठो_2 मान(IRState अचर& state) {
			फर्काऊ add(IR::गाँठो_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		शून्य suggest_name(गाँठो_2 गाँठो, IdString name) {
			_ir.mutate(गाँठो).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	अन्तःस्थ Factory IR::factory() { फर्काऊ Factory(*this); }
	ढाँचा<class Id> class Scope {
	रक्षित:
		वर्ण substitution_character = '_';
		आभास bool is_character_legal(वर्ण, पूर्णाङ्क) = 0;
	गुप्त:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	सार्वजनिक:
		शून्य reserve(std::string name) {
			_used_names.घुसाउ(std::move(name));
		}
		ढाँचा<typename N> std::string unique_name(अचर N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			लागि(माप_प्रकार i = 0; i < str.size(); i++)
				यदि(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			यदि(_used_names.गन्ती(str) == 0) {
				_used_names.घुसाउ(str);
				फर्काऊ str;
			}
			लागि (पूर्णाङ्क idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				यदि(_used_names.गन्ती(suffixed) == 0) {
					_used_names.घुसाउ(suffixed);
					फर्काऊ suffixed;
				}
			}
		}
		ढाँचा<typename N> std::string चालक()(Id id, अचर N &suggestion) {
			स्वतः it = _by_id.find(id);
			यदि(it != _by_id.end())
				फर्काऊ it->second;
			std::string str = unique_name(suggestion);
			_by_id.घुसाउ({id, str});
			फर्काऊ str;
		}
	};
	class Writer {
		std::ostream *os;
		शून्य print_impl(अचर वर्ण *fmt, vector<std::function<शून्य()>>& fns);
	सार्वजनिक:
		Writer(std::ostream &os) : os(&os) {}
		ढाँचा<class T> Writer& चालक <<(T&& arg) { *os << std::forward<T>(arg); फर्काऊ *this; }
		ढाँचा<typename... Args>
		शून्य print(अचर वर्ण *fmt, Args&&... args)
		{
			vector<std::function<शून्य()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		ढाँचा<typename Fn, typename... Args>
		शून्य print_with(Fn fn, अचर वर्ण *fmt, Args&&... args)
		{
			vector<std::function<शून्य()>> fns { [&]() {
				यदि constexpr (std::is_invocable_v<Fn, Args>)
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
