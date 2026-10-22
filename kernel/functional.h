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
		logical_shift_वामः,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_दक्षिणः,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_दक्षिणः,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		निवेशः,
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
		bool is_signal() ध्रुव { पटिनिवत्त _v.index() == 0; }
		bool is_memory() ध्रुव { पटिनिवत्त _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क width() ध्रुव { पटिनिवत्त std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क addr_width() ध्रुव { पटिनिवत्त std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क data_width() ध्रुव { पटिनिवत्त std::get<1>(_v).second; }
		bool operator==(Sort ध्रुव& other) ध्रुव { पटिनिवत्त _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(_v); पटिनिवत्त h; }
	};
	class IR;
	class Factory;
	class ग्रन्थिः_2;
	class IRInput {
		friend class Factory;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	गुप्त:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	गुप्त:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	सार्वजनिक:
		ग्रन्थिः_2 मानम्() ध्रुव;
		bool has_मानम्() ध्रुव;
		शून्य set_मानम्(ग्रन्थिः_2 मानम्);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName kind;
		Sort sort;
	गुप्त:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	सार्वजनिक:
		ग्रन्थिः_2 next_मानम्() ध्रुव;
		bool has_next_मानम्() ध्रुव;
		RTLIL::Const ध्रुव& initial_मानम्_signal() ध्रुव { पटिनिवत्त std::get<RTLIL::Const>(_initial); }
		MemContents ध्रुव& initial_मानम्_memory() ध्रुव { पटिनिवत्त std::get<MemContents>(_initial); }
		शून्य set_next_मानम्(ग्रन्थिः_2 मानम्);
		शून्य set_initial_मानम्(RTLIL::Const मानम्) { मानम्.extu(sort.width()); _initial = std::move(मानम्); }
		शून्य set_initial_मानम्(MemContents मानम्) { log_assert(Sort(मानम्.addr_width(), मानम्.data_width()) == sort); _initial = std::move(मानम्); }
	};
	class IR {
		friend class Factory;
		friend class ग्रन्थिः_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ग्रन्थिः_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				पूर्णाङ्क
			> _extra;
		सार्वजनिक:
			ग्रन्थिः_Data() : _fn(Fn::invalid) {}
			ग्रन्थिः_Data(Fn fn) : _fn(fn) {}
			template<class T> ग्रन्थिः_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ध्रुव { पटिनिवत्त _fn; }
			ध्रुव RTLIL::Const &as_const() ध्रुव { पटिनिवत्त std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ध्रुव { पटिनिवत्त std::get<std::pair<IdString, IdString>>(_extra); }
			पूर्णाङ्क as_int() ध्रुव { पटिनिवत्त std::get<पूर्णाङ्क>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
				h.eat((unsigned पूर्णाङ्क) _fn);
				h.eat(_extra);
				पटिनिवत्त h;
			}
			bool operator==(ग्रन्थिः_Data ध्रुव &other) ध्रुव {
				पटिनिवत्त _fn == other._fn && _extra == other._extra;
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
		using Graph = ComputeGraph<ग्रन्थिः_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(ग्रन्थिः_2 n);
	सार्वजनिक:
		Design *design = nullptr;
		स्थिर IR from_module(Module *module);
		Factory factory();
		पूर्णाङ्क size() ध्रुव { पटिनिवत्त _graph.size(); }
		ग्रन्थिः_2 operator[](पूर्णाङ्क i);
		शून्य topological_sort();
		शून्य forward_buf();
		IRInput ध्रुव& निवेशः(IdString name, IdString kind) ध्रुव { पटिनिवत्त _inputs.at({name, kind}); }
		IRInput ध्रुव& निवेशः(IdString name) ध्रुव { पटिनिवत्त निवेशः(name, ID($निवेशः)); }
		IROutput ध्रुव& output(IdString name, IdString kind) ध्रुव { पटिनिवत्त _outputs.at({name, kind}); }
		IROutput ध्रुव& output(IdString name) ध्रुव { पटिनिवत्त output(name, ID($output)); }
		IRState ध्रुव& state(IdString name, IdString kind) ध्रुव { पटिनिवत्त _states.at({name, kind}); }
		IRState ध्रुव& state(IdString name) ध्रुव { पटिनिवत्त state(name, ID($state)); }
		bool has_निवेशः(IdString name, IdString kind) ध्रुव { पटिनिवत्त _inputs.गणनम्({name, kind}); }
		bool has_output(IdString name, IdString kind) ध्रुव { पटिनिवत्त _outputs.गणनम्({name, kind}); }
		bool has_state(IdString name, IdString kind) ध्रुव { पटिनिवत्त _states.गणनम्({name, kind}); }
		vector<IRInput ध्रुव*> inputs(IdString kind) ध्रुव;
		vector<IRInput ध्रुव*> inputs() ध्रुव { पटिनिवत्त inputs(ID($निवेशः)); }
		vector<IROutput ध्रुव*> outputs(IdString kind) ध्रुव;
		vector<IROutput ध्रुव*> outputs() ध्रुव { पटिनिवत्त outputs(ID($output)); }
		vector<IRState ध्रुव*> states(IdString kind) ध्रुव;
		vector<IRState ध्रुव*> states() ध्रुव { पटिनिवत्त states(ID($state)); }
		vector<IRInput ध्रुव*> all_inputs() ध्रुव;
		vector<IROutput ध्रुव*> all_outputs() ध्रुव;
		vector<IRState ध्रुव*> all_states() ध्रुव;
		class iterator {
			friend class IR;
			IR *_ir;
			पूर्णाङ्क _index;
			iterator(IR *ir, पूर्णाङ्क index) : _ir(ir), _index(index) {}
		सार्वजनिक:
			using iterator_category = std::निवेशः_iterator_tag;
			using मानम्_type = ग्रन्थिः_2;
			using pointer = arrow_proxy<ग्रन्थिः_2>;
			using reference = ग्रन्थिः_2;
			using difference_type = ptrdiff_t;
			ग्रन्थिः_2 operator*();
			iterator &operator++() { _index++; पटिनिवत्त *this; }
			bool operator!=(iterator ध्रुव &other) ध्रुव { पटिनिवत्त _ir != other._ir || _index != other._index; }
			bool operator==(iterator ध्रुव &other) ध्रुव { पटिनिवत्त !(*this != other); }
			pointer operator->();
		};
		iterator begin() { पटिनिवत्त iterator(this, 0); }
		iterator end() { पटिनिवत्त iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ग्रन्थिः_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		ग्रन्थिः_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { पटिनिवत्त _ref; }
	सार्वजनिक:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		पूर्णाङ्क id() ध्रुव { पटिनिवत्त _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ध्रुव {
			सचे(_ref.has_sparse_attr())
				पटिनिवत्त {design, _ref.sparse_attr()};
			अञ्ञथा
				पटिनिवत्त {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() ध्रुव { पटिनिवत्त _ref.function().fn(); }
		Sort sort() ध्रुव { पटिनिवत्त _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		पूर्णाङ्क width() ध्रुव { पटिनिवत्त sort().width(); }
		size_t arg_गणनम्() ध्रुव { पटिनिवत्त _ref.size(); }
		ग्रन्थिः_2 arg(पूर्णाङ्क n) ध्रुव { पटिनिवत्त ग्रन्थिः_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) ध्रुव
		{
			// currently templated but could be switched to AbstractVisitor &
			विकल्प(_ref.function().fn()) {
			पक्ष Fn::invalid: log_error("invalid node in visit\n"); भङ्ग;
			पक्ष Fn::buf: पटिनिवत्त v.buf(*this, arg(0)); भङ्ग;
			पक्ष Fn::slice: पटिनिवत्त v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); भङ्ग;
			पक्ष Fn::zero_extend: पटिनिवत्त v.zero_extend(*this, arg(0), width()); भङ्ग;
			पक्ष Fn::sign_extend: पटिनिवत्त v.sign_extend(*this, arg(0), width()); भङ्ग;
			पक्ष Fn::concat: पटिनिवत्त v.concat(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::add: पटिनिवत्त v.add(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::sub: पटिनिवत्त v.sub(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::mul: पटिनिवत्त v.mul(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_div: पटिनिवत्त v.unsigned_div(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_mod: पटिनिवत्त v.unsigned_mod(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_and: पटिनिवत्त v.bitwise_and(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_or: पटिनिवत्त v.bitwise_or(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_xor: पटिनिवत्त v.bitwise_xor(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_not: पटिनिवत्त v.bitwise_not(*this, arg(0)); भङ्ग;
			पक्ष Fn::unary_minus: पटिनिवत्त v.unary_minus(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_and: पटिनिवत्त v.reduce_and(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_or: पटिनिवत्त v.reduce_or(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_xor: पटिनिवत्त v.reduce_xor(*this, arg(0)); भङ्ग;
			पक्ष Fn::equal: पटिनिवत्त v.equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::not_equal: पटिनिवत्त v.not_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::signed_greater_than: पटिनिवत्त v.signed_greater_than(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::signed_greater_equal: पटिनिवत्त v.signed_greater_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_greater_than: पटिनिवत्त v.unsigned_greater_than(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_greater_equal: पटिनिवत्त v.unsigned_greater_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::logical_shift_वामः: पटिनिवत्त v.logical_shift_वामः(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::logical_shift_दक्षिणः: पटिनिवत्त v.logical_shift_दक्षिणः(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::arithmetic_shift_दक्षिणः: पटिनिवत्त v.arithmetic_shift_दक्षिणः(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::mux: पटिनिवत्त v.mux(*this, arg(0), arg(1), arg(2)); भङ्ग;
			पक्ष Fn::constant: पटिनिवत्त v.constant(*this, _ref.function().as_const()); भङ्ग;
			पक्ष Fn::निवेशः: पटिनिवत्त v.निवेशः(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); भङ्ग;
			पक्ष Fn::state: पटिनिवत्त v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); भङ्ग;
			पक्ष Fn::memory_read: पटिनिवत्त v.memory_read(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::memory_write: पटिनिवत्त v.memory_write(*this, arg(0), arg(1), arg(2)); भङ्ग;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(ग्रन्थिः_2)>);
	};
	अन्तःस्थ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अन्तःस्थ IR::Graph::Ref IR::mutate(ग्रन्थिः_2 n) { पटिनिवत्त _graph[n._ref.index()]; }
	अन्तःस्थ ग्रन्थिः_2 IR::operator[](पूर्णाङ्क i) { पटिनिवत्त ग्रन्थिः_2(_graph[i], design); }
	अन्तःस्थ ग्रन्थिः_2 IROutput::मानम्() ध्रुव { पटिनिवत्त ग्रन्थिः_2(_ir._graph({name, kind, false}), _ir.design); }
	अन्तःस्थ bool IROutput::has_मानम्() ध्रुव { पटिनिवत्त _ir._graph.has_कुञ्चिका({name, kind, false}); }
	अन्तःस्थ शून्य IROutput::set_मानम्(ग्रन्थिः_2 मानम्) { log_assert(sort == मानम्.sort()); _ir.mutate(मानम्).assign_कुञ्चिका({name, kind, false}); }
	अन्तःस्थ ग्रन्थिः_2 IRState::next_मानम्() ध्रुव { पटिनिवत्त ग्रन्थिः_2(_ir._graph({name, kind, true}), _ir.design); }
	अन्तःस्थ bool IRState::has_next_मानम्() ध्रुव { पटिनिवत्त _ir._graph.has_कुञ्चिका({name, kind, true}); }
	अन्तःस्थ शून्य IRState::set_next_मानम्(ग्रन्थिः_2 मानम्) { log_assert(sort == मानम्.sort()); _ir.mutate(मानम्).assign_कुञ्चिका({name, kind, true}); }
	अन्तःस्थ ग्रन्थिः_2 IR::iterator::operator*() { पटिनिवत्त ग्रन्थिः_2(_ir->_graph[_index], _ir->design); }
	अन्तःस्थ arrow_proxy<ग्रन्थिः_2> IR::iterator::operator->() { पटिनिवत्त arrow_proxy<ग्रन्थिः_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> संरचना AbstractVisitor {
		आभास T buf(ग्रन्थिः_2 self, ग्रन्थिः_2 n) = 0;
		आभास T slice(ग्रन्थिः_2 self, ग्रन्थिः_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) = 0;
		आभास T zero_extend(ग्रन्थिः_2 self, ग्रन्थिः_2 a, पूर्णाङ्क out_width) = 0;
		आभास T sign_extend(ग्रन्थिः_2 self, ग्रन्थिः_2 a, पूर्णाङ्क out_width) = 0;
		आभास T concat(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T add(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T sub(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T mul(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T unsigned_div(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T unsigned_mod(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T bitwise_and(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T bitwise_or(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T bitwise_xor(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T bitwise_not(ग्रन्थिः_2 self, ग्रन्थिः_2 a) = 0;
		आभास T unary_minus(ग्रन्थिः_2 self, ग्रन्थिः_2 a) = 0;
		आभास T reduce_and(ग्रन्थिः_2 self, ग्रन्थिः_2 a) = 0;
		आभास T reduce_or(ग्रन्थिः_2 self, ग्रन्थिः_2 a) = 0;
		आभास T reduce_xor(ग्रन्थिः_2 self, ग्रन्थिः_2 a) = 0;
		आभास T equal(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T not_equal(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T signed_greater_than(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T signed_greater_equal(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T unsigned_greater_than(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T unsigned_greater_equal(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T logical_shift_वामः(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T logical_shift_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T arithmetic_shift_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T mux(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b, ग्रन्थिः_2 s) = 0;
		आभास T constant(ग्रन्थिः_2 self, RTLIL::Const ध्रुव & मानम्) = 0;
		आभास T निवेशः(ग्रन्थिः_2 self, IdString name, IdString kind) = 0;
		आभास T state(ग्रन्थिः_2 self, IdString name, IdString kind) = 0;
		आभास T memory_read(ग्रन्थिः_2 self, ग्रन्थिः_2 mem, ग्रन्थिः_2 addr) = 0;
		आभास T memory_write(ग्रन्थिः_2 self, ग्रन्थिः_2 mem, ग्रन्थिः_2 addr, ग्रन्थिः_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> संरचना DefaultVisitor : सार्वजनिक AbstractVisitor<T> {
		आभास T default_handler(ग्रन्थिः_2 self) = 0;
		T buf(ग्रन्थिः_2 self, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T slice(ग्रन्थिः_2 self, ग्रन्थिः_2, पूर्णाङ्क, पूर्णाङ्क) override { पटिनिवत्त default_handler(self); }
		T zero_extend(ग्रन्थिः_2 self, ग्रन्थिः_2, पूर्णाङ्क) override { पटिनिवत्त default_handler(self); }
		T sign_extend(ग्रन्थिः_2 self, ग्रन्थिः_2, पूर्णाङ्क) override { पटिनिवत्त default_handler(self); }
		T concat(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T add(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T sub(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T mul(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T unsigned_div(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T unsigned_mod(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T bitwise_and(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T bitwise_or(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T bitwise_xor(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T bitwise_not(ग्रन्थिः_2 self, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T unary_minus(ग्रन्थिः_2 self, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T reduce_and(ग्रन्थिः_2 self, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T reduce_or(ग्रन्थिः_2 self, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T reduce_xor(ग्रन्थिः_2 self, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T not_equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T signed_greater_than(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T signed_greater_equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T unsigned_greater_than(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T unsigned_greater_equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T logical_shift_वामः(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T logical_shift_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T arithmetic_shift_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T mux(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T constant(ग्रन्थिः_2 self, RTLIL::Const ध्रुव &) override { पटिनिवत्त default_handler(self); }
		T निवेशः(ग्रन्थिः_2 self, IdString, IdString) override { पटिनिवत्त default_handler(self); }
		T state(ग्रन्थिः_2 self, IdString, IdString) override { पटिनिवत्त default_handler(self); }
		T memory_read(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
		T memory_write(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2, ग्रन्थिः_2) override { पटिनिवत्त default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ग्रन्थिः_2 add(IR::ग्रन्थिः_Data &&fn, Sort ध्रुव &sort, std::initializer_list<ग्रन्थिः_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			अत्थाय (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			पटिनिवत्त ग्रन्थिः_2(ref, _ir.design);
		}
		शून्य check_basic_द्विमान(ग्रन्थिः_2 ध्रुव &a, ग्रन्थिः_2 ध्रुव &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		शून्य check_shift(ग्रन्थिः_2 ध्रुव &a, ग्रन्थिः_2 ध्रुव &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		शून्य check_unary(ग्रन्थिः_2 ध्रुव &a) { log_assert(a.sort().is_signal()); }
	सार्वजनिक:
		IR &ir() { पटिनिवत्त _ir; }
		ग्रन्थिः_2 slice(ग्रन्थिः_2 a, पूर्णाङ्क offset, पूर्णाङ्क out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			सचे(offset == 0 && out_width == a.width())
				पटिनिवत्त a;
			पटिनिवत्त add(IR::ग्रन्थिः_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ग्रन्थिः_2 extend(ग्रन्थिः_2 a, पूर्णाङ्क out_width, bool is_signed) {
			पूर्णाङ्क in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			सचे(in_width == out_width)
				पटिनिवत्त a;
			सचे(in_width > out_width)
				पटिनिवत्त slice(a, 0, out_width);
			सचे(is_signed)
				पटिनिवत्त add(Fn::sign_extend, Sort(out_width), {a});
			अञ्ञथा
				पटिनिवत्त add(Fn::zero_extend, Sort(out_width), {a});
		}
		ग्रन्थिः_2 concat(ग्रन्थिः_2 a, ग्रन्थिः_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			पटिनिवत्त add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		ग्रन्थिः_2 add(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::add, a.sort(), {a, b}); }
		ग्रन्थिः_2 sub(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::sub, a.sort(), {a, b}); }
		ग्रन्थिः_2 mul(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::mul, a.sort(), {a, b}); }
		ग्रन्थिः_2 unsigned_div(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::unsigned_div, a.sort(), {a, b}); }
		ग्रन्थिः_2 unsigned_mod(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::unsigned_mod, a.sort(), {a, b}); }
		ग्रन्थिः_2 bitwise_and(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::bitwise_and, a.sort(), {a, b}); }
		ग्रन्थिः_2 bitwise_or(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::bitwise_or, a.sort(), {a, b}); }
		ग्रन्थिः_2 bitwise_xor(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::bitwise_xor, a.sort(), {a, b}); }
		ग्रन्थिः_2 bitwise_not(ग्रन्थिः_2 a) { check_unary(a); पटिनिवत्त add(Fn::bitwise_not, a.sort(), {a}); }
		ग्रन्थिः_2 unary_minus(ग्रन्थिः_2 a) { check_unary(a); पटिनिवत्त add(Fn::unary_minus, a.sort(), {a}); }
		ग्रन्थिः_2 reduce_and(ग्रन्थिः_2 a) {
			check_unary(a);
			सचे(a.width() == 1)
				पटिनिवत्त a;
			पटिनिवत्त add(Fn::reduce_and, Sort(1), {a});
		}
		ग्रन्थिः_2 reduce_or(ग्रन्थिः_2 a) {
			check_unary(a);
			सचे(a.width() == 1)
				पटिनिवत्त a;
			पटिनिवत्त add(Fn::reduce_or, Sort(1), {a});
		}
		ग्रन्थिः_2 reduce_xor(ग्रन्थिः_2 a) {
			check_unary(a);
			सचे(a.width() == 1)
				पटिनिवत्त a;
			पटिनिवत्त add(Fn::reduce_xor, Sort(1), {a});
		}
		ग्रन्थिः_2 equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::equal, Sort(1), {a, b}); }
		ग्रन्थिः_2 not_equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::not_equal, Sort(1), {a, b}); }
		ग्रन्थिः_2 signed_greater_than(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::signed_greater_than, Sort(1), {a, b}); }
		ग्रन्थिः_2 signed_greater_equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		ग्रन्थिः_2 unsigned_greater_than(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		ग्रन्थिः_2 unsigned_greater_equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); पटिनिवत्त add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		ग्रन्थिः_2 logical_shift_वामः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_shift(a, b); पटिनिवत्त add(Fn::logical_shift_वामः, a.sort(), {a, b}); }
		ग्रन्थिः_2 logical_shift_दक्षिणः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_shift(a, b); पटिनिवत्त add(Fn::logical_shift_दक्षिणः, a.sort(), {a, b}); }
		ग्रन्थिः_2 arithmetic_shift_दक्षिणः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_shift(a, b); पटिनिवत्त add(Fn::arithmetic_shift_दक्षिणः, a.sort(), {a, b}); }
		ग्रन्थिः_2 mux(ग्रन्थिः_2 a, ग्रन्थिः_2 b, ग्रन्थिः_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			पटिनिवत्त add(Fn::mux, a.sort(), {a, b, s});
		}
		ग्रन्थिः_2 memory_read(ग्रन्थिः_2 mem, ग्रन्थिः_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			पटिनिवत्त add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		ग्रन्थिः_2 memory_write(ग्रन्थिः_2 mem, ग्रन्थिः_2 addr, ग्रन्थिः_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			पटिनिवत्त add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		ग्रन्थिः_2 constant(RTLIL::Const मानम्) {
			पूर्णाङ्क s = मानम्.size();
			पटिनिवत्त add(IR::ग्रन्थिः_Data(Fn::constant, std::move(मानम्)), Sort(s), {});
		}
		ग्रन्थिः_2 create_pending(पूर्णाङ्क width) {
			पटिनिवत्त add(Fn::buf, Sort(width), {});
		}
		शून्य update_pending(ग्रन्थिः_2 ग्रन्थिः, ग्रन्थिः_2 मानम्) {
			log_assert(ग्रन्थिः._ref.function() == Fn::buf && ग्रन्थिः._ref.size() == 0);
			log_assert(ग्रन्थिः.sort() == मानम्.sort());
			_ir.mutate(ग्रन्थिः).append_arg(मानम्._ref);
		}
		IRInput &add_निवेशः(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			सचे (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			पटिनिवत्त it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			सचे (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			पटिनिवत्त it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			सचे (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			पटिनिवत्त it->second;
		}
		ग्रन्थिः_2 मानम्(IRInput ध्रुव& निवेशः) {
			पटिनिवत्त add(IR::ग्रन्थिः_Data(Fn::निवेशः, std::pair(निवेशः.name, निवेशः.kind)), निवेशः.sort, {});
		}
		ग्रन्थिः_2 मानम्(IRState ध्रुव& state) {
			पटिनिवत्त add(IR::ग्रन्थिः_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		शून्य suggest_name(ग्रन्थिः_2 ग्रन्थिः, IdString name) {
			_ir.mutate(ग्रन्थिः).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	अन्तःस्थ Factory IR::factory() { पटिनिवत्त Factory(*this); }
	template<class Id> class Scope {
	रक्षित:
		वर्ण substitution_character = '_';
		आभास bool is_character_legal(वर्ण, पूर्णाङ्क) = 0;
	गुप्त:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	सार्वजनिक:
		शून्य reserve(std::string name) {
			_used_names.निवेशय(std::move(name));
		}
		template<typename N> std::string unique_name(ध्रुव N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			अत्थाय(size_t i = 0; i < str.size(); i++)
				सचे(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			सचे(_used_names.गणनम्(str) == 0) {
				_used_names.निवेशय(str);
				पटिनिवत्त str;
			}
			अत्थाय (पूर्णाङ्क idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				सचे(_used_names.गणनम्(suffixed) == 0) {
					_used_names.निवेशय(suffixed);
					पटिनिवत्त suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, ध्रुव N &suggestion) {
			auto it = _by_id.find(id);
			सचे(it != _by_id.end())
				पटिनिवत्त it->second;
			std::string str = unique_name(suggestion);
			_by_id.निवेशय({id, str});
			पटिनिवत्त str;
		}
	};
	class Writer {
		std::ostream *os;
		शून्य print_impl(ध्रुव वर्ण *fmt, vector<std::function<शून्य()>>& fns);
	सार्वजनिक:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); पटिनिवत्त *this; }
		template<typename... Args>
		शून्य print(ध्रुव वर्ण *fmt, Args&&... args)
		{
			vector<std::function<शून्य()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		शून्य print_with(Fn fn, ध्रुव वर्ण *fmt, Args&&... args)
		{
			vector<std::function<शून्य()>> fns { [&]() {
				सचे constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				अञ्ञथा
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
