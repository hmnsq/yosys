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
		निवेश,
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
	ध्रुव वर्ण *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<पूर्णाङ्क, std::pair<पूर्णाङ्क, पूर्णाङ्क>> _v;
	सार्वजनिक:
		explicit Sort(पूर्णाङ्क width) : _v(width) { }
		Sort(पूर्णाङ्क addr_width, पूर्णाङ्क data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() ध्रुव { प्रत्यागम _v.index() == 0; }
		bool is_memory() ध्रुव { प्रत्यागम _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क width() ध्रुव { प्रत्यागम std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क addr_width() ध्रुव { प्रत्यागम std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क data_width() ध्रुव { प्रत्यागम std::get<1>(_v).second; }
		bool चालक==(Sort ध्रुव& other) ध्रुव { प्रत्यागम _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(_v); प्रत्यागम h; }
	};
	class IR;
	class Factory;
	class गाँठ_2;
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
		गाँठ_2 मूल्य() ध्रुव;
		bool has_मूल्य() ध्रुव;
		शून्य set_मूल्य(गाँठ_2 मूल्य);
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
		गाँठ_2 next_मूल्य() ध्रुव;
		bool has_next_मूल्य() ध्रुव;
		RTLIL::Const ध्रुव& initial_मूल्य_signal() ध्रुव { प्रत्यागम std::get<RTLIL::Const>(_initial); }
		MemContents ध्रुव& initial_मूल्य_memory() ध्रुव { प्रत्यागम std::get<MemContents>(_initial); }
		शून्य set_next_मूल्य(गाँठ_2 मूल्य);
		शून्य set_initial_मूल्य(RTLIL::Const मूल्य) { मूल्य.extu(sort.width()); _initial = std::move(मूल्य); }
		शून्य set_initial_मूल्य(MemContents मूल्य) { log_assert(Sort(मूल्य.addr_width(), मूल्य.data_width()) == sort); _initial = std::move(मूल्य); }
	};
	class IR {
		मित्र class Factory;
		मित्र class गाँठ_2;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class गाँठ_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				पूर्णाङ्क
			> _extra;
		सार्वजनिक:
			गाँठ_Data() : _fn(Fn::invalid) {}
			गाँठ_Data(Fn fn) : _fn(fn) {}
			ढाँचा<class T> गाँठ_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ध्रुव { प्रत्यागम _fn; }
			ध्रुव RTLIL::Const &as_const() ध्रुव { प्रत्यागम std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ध्रुव { प्रत्यागम std::get<std::pair<IdString, IdString>>(_extra); }
			पूर्णाङ्क as_int() ध्रुव { प्रत्यागम std::get<पूर्णाङ्क>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
				h.eat((अचिन्हित पूर्णाङ्क) _fn);
				h.eat(_extra);
				प्रत्यागम h;
			}
			bool चालक==(गाँठ_Data ध्रुव &other) ध्रुव {
				प्रत्यागम _fn == other._fn && _extra == other._extra;
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
		using Graph = ComputeGraph<गाँठ_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(गाँठ_2 n);
	सार्वजनिक:
		Design *design = nullptr;
		स्थिर IR from_module(Module *module);
		Factory factory();
		पूर्णाङ्क size() ध्रुव { प्रत्यागम _graph.size(); }
		गाँठ_2 चालक[](पूर्णाङ्क i);
		शून्य topological_sort();
		शून्य forward_buf();
		IRInput ध्रुव& निवेश(IdString name, IdString kind) ध्रुव { प्रत्यागम _inputs.at({name, kind}); }
		IRInput ध्रुव& निवेश(IdString name) ध्रुव { प्रत्यागम निवेश(name, ID($निवेश)); }
		IROutput ध्रुव& output(IdString name, IdString kind) ध्रुव { प्रत्यागम _outputs.at({name, kind}); }
		IROutput ध्रुव& output(IdString name) ध्रुव { प्रत्यागम output(name, ID($output)); }
		IRState ध्रुव& state(IdString name, IdString kind) ध्रुव { प्रत्यागम _states.at({name, kind}); }
		IRState ध्रुव& state(IdString name) ध्रुव { प्रत्यागम state(name, ID($state)); }
		bool has_निवेश(IdString name, IdString kind) ध्रुव { प्रत्यागम _inputs.गिनती({name, kind}); }
		bool has_output(IdString name, IdString kind) ध्रुव { प्रत्यागम _outputs.गिनती({name, kind}); }
		bool has_state(IdString name, IdString kind) ध्रुव { प्रत्यागम _states.गिनती({name, kind}); }
		vector<IRInput ध्रुव*> inputs(IdString kind) ध्रुव;
		vector<IRInput ध्रुव*> inputs() ध्रुव { प्रत्यागम inputs(ID($निवेश)); }
		vector<IROutput ध्रुव*> outputs(IdString kind) ध्रुव;
		vector<IROutput ध्रुव*> outputs() ध्रुव { प्रत्यागम outputs(ID($output)); }
		vector<IRState ध्रुव*> states(IdString kind) ध्रुव;
		vector<IRState ध्रुव*> states() ध्रुव { प्रत्यागम states(ID($state)); }
		vector<IRInput ध्रुव*> all_inputs() ध्रुव;
		vector<IROutput ध्रुव*> all_outputs() ध्रुव;
		vector<IRState ध्रुव*> all_states() ध्रुव;
		class iterator {
			मित्र class IR;
			IR *_ir;
			पूर्णाङ्क _index;
			iterator(IR *ir, पूर्णाङ्क index) : _ir(ir), _index(index) {}
		सार्वजनिक:
			using iterator_category = std::निवेश_iterator_tag;
			using मूल्य_type = गाँठ_2;
			using pointer = arrow_proxy<गाँठ_2>;
			using reference = गाँठ_2;
			using difference_type = सूचक_भेद_प्रकार;
			गाँठ_2 चालक*();
			iterator &चालक++() { _index++; प्रत्यागम *this; }
			bool चालक!=(iterator ध्रुव &other) ध्रुव { प्रत्यागम _ir != other._ir || _index != other._index; }
			bool चालक==(iterator ध्रुव &other) ध्रुव { प्रत्यागम !(*this != other); }
			pointer चालक->();
		};
		iterator begin() { प्रत्यागम iterator(this, 0); }
		iterator end() { प्रत्यागम iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class गाँठ_2 {
		मित्र class Factory;
		मित्र class IR;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		IR::Graph::ConstRef _ref;
		गाँठ_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit चालक IR::Graph::ConstRef() { प्रत्यागम _ref; }
	सार्वजनिक:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		पूर्णाङ्क id() ध्रुव { प्रत्यागम _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ध्रुव {
			यदि(_ref.has_sparse_attr())
				प्रत्यागम {design, _ref.sparse_attr()};
			अन्यथा
				प्रत्यागम {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() ध्रुव { प्रत्यागम _ref.function().fn(); }
		Sort sort() ध्रुव { प्रत्यागम _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		पूर्णाङ्क width() ध्रुव { प्रत्यागम sort().width(); }
		माप_प्रकार arg_गिनती() ध्रुव { प्रत्यागम _ref.size(); }
		गाँठ_2 arg(पूर्णाङ्क n) ध्रुव { प्रत्यागम गाँठ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		ढाँचा<class Visitor> स्वतः visit(Visitor v) ध्रुव
		{
			// currently templated but could be switched to AbstractVisitor &
			विकल्प(_ref.function().fn()) {
			पक्ष Fn::invalid: log_error("invalid node in visit\n"); भङ्ग;
			पक्ष Fn::buf: प्रत्यागम v.buf(*this, arg(0)); भङ्ग;
			पक्ष Fn::slice: प्रत्यागम v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); भङ्ग;
			पक्ष Fn::zero_extend: प्रत्यागम v.zero_extend(*this, arg(0), width()); भङ्ग;
			पक्ष Fn::sign_extend: प्रत्यागम v.sign_extend(*this, arg(0), width()); भङ्ग;
			पक्ष Fn::concat: प्रत्यागम v.concat(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::add: प्रत्यागम v.add(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::sub: प्रत्यागम v.sub(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::mul: प्रत्यागम v.mul(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_div: प्रत्यागम v.unsigned_div(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_mod: प्रत्यागम v.unsigned_mod(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_and: प्रत्यागम v.bitwise_and(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_or: प्रत्यागम v.bitwise_or(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_xor: प्रत्यागम v.bitwise_xor(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_not: प्रत्यागम v.bitwise_not(*this, arg(0)); भङ्ग;
			पक्ष Fn::unary_minus: प्रत्यागम v.unary_minus(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_and: प्रत्यागम v.reduce_and(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_or: प्रत्यागम v.reduce_or(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_xor: प्रत्यागम v.reduce_xor(*this, arg(0)); भङ्ग;
			पक्ष Fn::equal: प्रत्यागम v.equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::not_equal: प्रत्यागम v.not_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::signed_greater_than: प्रत्यागम v.signed_greater_than(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::signed_greater_equal: प्रत्यागम v.signed_greater_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_greater_than: प्रत्यागम v.unsigned_greater_than(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_greater_equal: प्रत्यागम v.unsigned_greater_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::logical_shift_बायाँ: प्रत्यागम v.logical_shift_बायाँ(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::logical_shift_दायाँ: प्रत्यागम v.logical_shift_दायाँ(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::arithmetic_shift_दायाँ: प्रत्यागम v.arithmetic_shift_दायाँ(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::mux: प्रत्यागम v.mux(*this, arg(0), arg(1), arg(2)); भङ्ग;
			पक्ष Fn::constant: प्रत्यागम v.constant(*this, _ref.function().as_const()); भङ्ग;
			पक्ष Fn::निवेश: प्रत्यागम v.निवेश(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); भङ्ग;
			पक्ष Fn::state: प्रत्यागम v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); भङ्ग;
			पक्ष Fn::memory_read: प्रत्यागम v.memory_read(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::memory_write: प्रत्यागम v.memory_write(*this, arg(0), arg(1), arg(2)); भङ्ग;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(गाँठ_2)>);
	};
	अन्तःस्थ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IR::Graph::Ref IR::mutate(गाँठ_2 n) { प्रत्यागम _graph[n._ref.index()]; }
	अन्तःस्थ गाँठ_2 IR::चालक[](पूर्णाङ्क i) { प्रत्यागम गाँठ_2(_graph[i], design); }
	अन्तःस्थ गाँठ_2 IROutput::मूल्य() ध्रुव { प्रत्यागम गाँठ_2(_ir._graph({name, kind, false}), _ir.design); }
	अन्तःस्थ bool IROutput::has_मूल्य() ध्रुव { प्रत्यागम _ir._graph.has_कुंजी({name, kind, false}); }
	अन्तःस्थ शून्य IROutput::set_मूल्य(गाँठ_2 मूल्य) { log_assert(sort == मूल्य.sort()); _ir.mutate(मूल्य).assign_कुंजी({name, kind, false}); }
	अन्तःस्थ गाँठ_2 IRState::next_मूल्य() ध्रुव { प्रत्यागम गाँठ_2(_ir._graph({name, kind, true}), _ir.design); }
	अन्तःस्थ bool IRState::has_next_मूल्य() ध्रुव { प्रत्यागम _ir._graph.has_कुंजी({name, kind, true}); }
	अन्तःस्थ शून्य IRState::set_next_मूल्य(गाँठ_2 मूल्य) { log_assert(sort == मूल्य.sort()); _ir.mutate(मूल्य).assign_कुंजी({name, kind, true}); }
	अन्तःस्थ गाँठ_2 IR::iterator::चालक*() { प्रत्यागम गाँठ_2(_ir->_graph[_index], _ir->design); }
	अन्तःस्थ arrow_proxy<गाँठ_2> IR::iterator::चालक->() { प्रत्यागम arrow_proxy<गाँठ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	ढाँचा<class T> संरचना AbstractVisitor {
		आभास T buf(गाँठ_2 self, गाँठ_2 n) = 0;
		आभास T slice(गाँठ_2 self, गाँठ_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) = 0;
		आभास T zero_extend(गाँठ_2 self, गाँठ_2 a, पूर्णाङ्क out_width) = 0;
		आभास T sign_extend(गाँठ_2 self, गाँठ_2 a, पूर्णाङ्क out_width) = 0;
		आभास T concat(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T add(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T sub(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T mul(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T unsigned_div(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T unsigned_mod(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T bitwise_and(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T bitwise_or(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T bitwise_xor(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T bitwise_not(गाँठ_2 self, गाँठ_2 a) = 0;
		आभास T unary_minus(गाँठ_2 self, गाँठ_2 a) = 0;
		आभास T reduce_and(गाँठ_2 self, गाँठ_2 a) = 0;
		आभास T reduce_or(गाँठ_2 self, गाँठ_2 a) = 0;
		आभास T reduce_xor(गाँठ_2 self, गाँठ_2 a) = 0;
		आभास T equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T not_equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T signed_greater_than(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T signed_greater_equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T unsigned_greater_than(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T unsigned_greater_equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T logical_shift_बायाँ(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T logical_shift_दायाँ(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T arithmetic_shift_दायाँ(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		आभास T mux(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) = 0;
		आभास T constant(गाँठ_2 self, RTLIL::Const ध्रुव & मूल्य) = 0;
		आभास T निवेश(गाँठ_2 self, IdString name, IdString kind) = 0;
		आभास T state(गाँठ_2 self, IdString name, IdString kind) = 0;
		आभास T memory_read(गाँठ_2 self, गाँठ_2 mem, गाँठ_2 addr) = 0;
		आभास T memory_write(गाँठ_2 self, गाँठ_2 mem, गाँठ_2 addr, गाँठ_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	ढाँचा<class T> संरचना DefaultVisitor : सार्वजनिक AbstractVisitor<T> {
		आभास T default_handler(गाँठ_2 self) = 0;
		T buf(गाँठ_2 self, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T slice(गाँठ_2 self, गाँठ_2, पूर्णाङ्क, पूर्णाङ्क) override { प्रत्यागम default_handler(self); }
		T zero_extend(गाँठ_2 self, गाँठ_2, पूर्णाङ्क) override { प्रत्यागम default_handler(self); }
		T sign_extend(गाँठ_2 self, गाँठ_2, पूर्णाङ्क) override { प्रत्यागम default_handler(self); }
		T concat(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T add(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T sub(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T mul(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T unsigned_div(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T unsigned_mod(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T bitwise_and(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T bitwise_or(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T bitwise_xor(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T bitwise_not(गाँठ_2 self, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T unary_minus(गाँठ_2 self, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T reduce_and(गाँठ_2 self, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T reduce_or(गाँठ_2 self, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T reduce_xor(गाँठ_2 self, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T not_equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T signed_greater_than(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T signed_greater_equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T unsigned_greater_than(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T unsigned_greater_equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T logical_shift_बायाँ(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T logical_shift_दायाँ(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T arithmetic_shift_दायाँ(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T mux(गाँठ_2 self, गाँठ_2, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T constant(गाँठ_2 self, RTLIL::Const ध्रुव &) override { प्रत्यागम default_handler(self); }
		T निवेश(गाँठ_2 self, IdString, IdString) override { प्रत्यागम default_handler(self); }
		T state(गाँठ_2 self, IdString, IdString) override { प्रत्यागम default_handler(self); }
		T memory_read(गाँठ_2 self, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
		T memory_write(गाँठ_2 self, गाँठ_2, गाँठ_2, गाँठ_2) override { प्रत्यागम default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		मित्र class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		गाँठ_2 add(IR::गाँठ_Data &&fn, Sort ध्रुव &sort, std::initializer_list<गाँठ_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			प्रति (स्वतः arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			प्रत्यागम गाँठ_2(ref, _ir.design);
		}
		शून्य check_basic_द्विआधारी(गाँठ_2 ध्रुव &a, गाँठ_2 ध्रुव &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		शून्य check_shift(गाँठ_2 ध्रुव &a, गाँठ_2 ध्रुव &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		शून्य check_unary(गाँठ_2 ध्रुव &a) { log_assert(a.sort().is_signal()); }
	सार्वजनिक:
		IR &ir() { प्रत्यागम _ir; }
		गाँठ_2 slice(गाँठ_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			यदि(offset == 0 && out_width == a.width())
				प्रत्यागम a;
			प्रत्यागम add(IR::गाँठ_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		गाँठ_2 extend(गाँठ_2 a, पूर्णाङ्क out_width, bool is_signed) {
			पूर्णाङ्क in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			यदि(in_width == out_width)
				प्रत्यागम a;
			यदि(in_width > out_width)
				प्रत्यागम slice(a, 0, out_width);
			यदि(is_signed)
				प्रत्यागम add(Fn::sign_extend, Sort(out_width), {a});
			अन्यथा
				प्रत्यागम add(Fn::zero_extend, Sort(out_width), {a});
		}
		गाँठ_2 concat(गाँठ_2 a, गाँठ_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			प्रत्यागम add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		गाँठ_2 add(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::add, a.sort(), {a, b}); }
		गाँठ_2 sub(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::sub, a.sort(), {a, b}); }
		गाँठ_2 mul(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::mul, a.sort(), {a, b}); }
		गाँठ_2 unsigned_div(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::unsigned_div, a.sort(), {a, b}); }
		गाँठ_2 unsigned_mod(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::unsigned_mod, a.sort(), {a, b}); }
		गाँठ_2 bitwise_and(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::bitwise_and, a.sort(), {a, b}); }
		गाँठ_2 bitwise_or(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::bitwise_or, a.sort(), {a, b}); }
		गाँठ_2 bitwise_xor(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::bitwise_xor, a.sort(), {a, b}); }
		गाँठ_2 bitwise_not(गाँठ_2 a) { check_unary(a); प्रत्यागम add(Fn::bitwise_not, a.sort(), {a}); }
		गाँठ_2 unary_minus(गाँठ_2 a) { check_unary(a); प्रत्यागम add(Fn::unary_minus, a.sort(), {a}); }
		गाँठ_2 reduce_and(गाँठ_2 a) {
			check_unary(a);
			यदि(a.width() == 1)
				प्रत्यागम a;
			प्रत्यागम add(Fn::reduce_and, Sort(1), {a});
		}
		गाँठ_2 reduce_or(गाँठ_2 a) {
			check_unary(a);
			यदि(a.width() == 1)
				प्रत्यागम a;
			प्रत्यागम add(Fn::reduce_or, Sort(1), {a});
		}
		गाँठ_2 reduce_xor(गाँठ_2 a) {
			check_unary(a);
			यदि(a.width() == 1)
				प्रत्यागम a;
			प्रत्यागम add(Fn::reduce_xor, Sort(1), {a});
		}
		गाँठ_2 equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::equal, Sort(1), {a, b}); }
		गाँठ_2 not_equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::not_equal, Sort(1), {a, b}); }
		गाँठ_2 signed_greater_than(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::signed_greater_than, Sort(1), {a, b}); }
		गाँठ_2 signed_greater_equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		गाँठ_2 unsigned_greater_than(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		गाँठ_2 unsigned_greater_equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); प्रत्यागम add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		गाँठ_2 logical_shift_बायाँ(गाँठ_2 a, गाँठ_2 b) { check_shift(a, b); प्रत्यागम add(Fn::logical_shift_बायाँ, a.sort(), {a, b}); }
		गाँठ_2 logical_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { check_shift(a, b); प्रत्यागम add(Fn::logical_shift_दायाँ, a.sort(), {a, b}); }
		गाँठ_2 arithmetic_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { check_shift(a, b); प्रत्यागम add(Fn::arithmetic_shift_दायाँ, a.sort(), {a, b}); }
		गाँठ_2 mux(गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			प्रत्यागम add(Fn::mux, a.sort(), {a, b, s});
		}
		गाँठ_2 memory_read(गाँठ_2 mem, गाँठ_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			प्रत्यागम add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		गाँठ_2 memory_write(गाँठ_2 mem, गाँठ_2 addr, गाँठ_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			प्रत्यागम add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		गाँठ_2 constant(RTLIL::Const मूल्य) {
			पूर्णाङ्क s = मूल्य.size();
			प्रत्यागम add(IR::गाँठ_Data(Fn::constant, std::move(मूल्य)), Sort(s), {});
		}
		गाँठ_2 create_pending(पूर्णाङ्क width) {
			प्रत्यागम add(Fn::buf, Sort(width), {});
		}
		शून्य update_pending(गाँठ_2 गाँठ, गाँठ_2 मूल्य) {
			log_assert(गाँठ._ref.function() == Fn::buf && गाँठ._ref.size() == 0);
			log_assert(गाँठ.sort() == मूल्य.sort());
			_ir.mutate(गाँठ).append_arg(मूल्य._ref);
		}
		IRInput &add_निवेश(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			यदि (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			प्रत्यागम it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			यदि (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			प्रत्यागम it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			यदि (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			प्रत्यागम it->second;
		}
		गाँठ_2 मूल्य(IRInput ध्रुव& निवेश) {
			प्रत्यागम add(IR::गाँठ_Data(Fn::निवेश, std::pair(निवेश.name, निवेश.kind)), निवेश.sort, {});
		}
		गाँठ_2 मूल्य(IRState ध्रुव& state) {
			प्रत्यागम add(IR::गाँठ_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		शून्य suggest_name(गाँठ_2 गाँठ, IdString name) {
			_ir.mutate(गाँठ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	अन्तःस्थ Factory IR::factory() { प्रत्यागम Factory(*this); }
	ढाँचा<class Id> class Scope {
	रक्षित:
		वर्ण substitution_character = '_';
		आभास bool is_character_legal(वर्ण, पूर्णाङ्क) = 0;
	गुप्त:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	सार्वजनिक:
		शून्य reserve(std::string name) {
			_used_names.जोड़ो(std::move(name));
		}
		ढाँचा<typename N> std::string unique_name(ध्रुव N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			प्रति(माप_प्रकार i = 0; i < str.size(); i++)
				यदि(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			यदि(_used_names.गिनती(str) == 0) {
				_used_names.जोड़ो(str);
				प्रत्यागम str;
			}
			प्रति (पूर्णाङ्क idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				यदि(_used_names.गिनती(suffixed) == 0) {
					_used_names.जोड़ो(suffixed);
					प्रत्यागम suffixed;
				}
			}
		}
		ढाँचा<typename N> std::string चालक()(Id id, ध्रुव N &suggestion) {
			स्वतः it = _by_id.find(id);
			यदि(it != _by_id.end())
				प्रत्यागम it->second;
			std::string str = unique_name(suggestion);
			_by_id.जोड़ो({id, str});
			प्रत्यागम str;
		}
	};
	class Writer {
		std::ostream *os;
		शून्य print_impl(ध्रुव वर्ण *fmt, vector<std::function<शून्य()>>& fns);
	सार्वजनिक:
		Writer(std::ostream &os) : os(&os) {}
		ढाँचा<class T> Writer& चालक <<(T&& arg) { *os << std::forward<T>(arg); प्रत्यागम *this; }
		ढाँचा<typename... Args>
		शून्य print(ध्रुव वर्ण *fmt, Args&&... args)
		{
			vector<std::function<शून्य()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		ढाँचा<typename Fn, typename... Args>
		शून्य print_with(Fn fn, ध्रुव वर्ण *fmt, Args&&... args)
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
