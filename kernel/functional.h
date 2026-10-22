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

#ଅନ୍ତର୍ଭାଵ "kernel/yosys.h"
#ଅନ୍ତର୍ଭାଵ "kernel/compute_graph.h"
#ଅନ୍ତର୍ଭାଵ "kernel/drivertools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/mem.h"
#ଅନ୍ତର୍ଭାଵ "kernel/utils.h"

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
	ଗଣନା class Fn {
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
		logical_shift_ବାମ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_ଡାହାଣ,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_ଡାହାଣ,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		ନିବେଶ,
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
	ଅଚଳ ଅକ୍ଷର *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, std::pair<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> _v;
	ସର୍ବସାଧାରଣ:
		explicit Sort(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) : _v(width) { }
		Sort(ପୂର୍ଣ୍ଣସଂଖ୍ୟା addr_width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() ଅଚଳ { ଫେରନ୍ତୁ _v.index() == 0; }
		bool is_memory() ଅଚଳ { ଫେରନ୍ତୁ _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width() ଅଚଳ { ଫେରନ୍ତୁ std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା addr_width() ଅଚଳ { ଫେରନ୍ତୁ std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା data_width() ଅଚଳ { ଫେରନ୍ତୁ std::get<1>(_v).second; }
		bool operator==(Sort ଅଚଳ& other) ଅଚଳ { ଫେରନ୍ତୁ _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ { h.eat(_v); ଫେରନ୍ତୁ h; }
	};
	class IR;
	class Factory;
	class ଗଣ୍ଠି_2;
	class IRInput {
		friend class Factory;
	ସର୍ବସାଧାରଣ:
		PooledName name;
		PooledName kind;
		Sort sort;
	ବ୍ୟକ୍ତିଗତ:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	ସର୍ବସାଧାରଣ:
		PooledName name;
		PooledName kind;
		Sort sort;
	ବ୍ୟକ୍ତିଗତ:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	ସର୍ବସାଧାରଣ:
		ଗଣ୍ଠି_2 ମୂଲ୍ୟ() ଅଚଳ;
		bool has_ମୂଲ୍ୟ() ଅଚଳ;
		ଶୂନ୍ୟ set_ମୂଲ୍ୟ(ଗଣ୍ଠି_2 ମୂଲ୍ୟ);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	ସର୍ବସାଧାରଣ:
		PooledName name;
		PooledName kind;
		Sort sort;
	ବ୍ୟକ୍ତିଗତ:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	ସର୍ବସାଧାରଣ:
		ଗଣ୍ଠି_2 next_ମୂଲ୍ୟ() ଅଚଳ;
		bool has_next_ମୂଲ୍ୟ() ଅଚଳ;
		RTLIL::Const ଅଚଳ& initial_ମୂଲ୍ୟ_signal() ଅଚଳ { ଫେରନ୍ତୁ std::get<RTLIL::Const>(_initial); }
		MemContents ଅଚଳ& initial_ମୂଲ୍ୟ_memory() ଅଚଳ { ଫେରନ୍ତୁ std::get<MemContents>(_initial); }
		ଶୂନ୍ୟ set_next_ମୂଲ୍ୟ(ଗଣ୍ଠି_2 ମୂଲ୍ୟ);
		ଶୂନ୍ୟ set_initial_ମୂଲ୍ୟ(RTLIL::Const ମୂଲ୍ୟ) { ମୂଲ୍ୟ.extu(sort.width()); _initial = std::move(ମୂଲ୍ୟ); }
		ଶୂନ୍ୟ set_initial_ମୂଲ୍ୟ(MemContents ମୂଲ୍ୟ) { log_assert(Sort(ମୂଲ୍ୟ.addr_width(), ମୂଲ୍ୟ.data_width()) == sort); _initial = std::move(ମୂଲ୍ୟ); }
	};
	class IR {
		friend class Factory;
		friend class ଗଣ୍ଠି_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ଗଣ୍ଠି_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା
			> _extra;
		ସର୍ବସାଧାରଣ:
			ଗଣ୍ଠି_Data() : _fn(Fn::invalid) {}
			ଗଣ୍ଠି_Data(Fn fn) : _fn(fn) {}
			template<class T> ଗଣ୍ଠି_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ଅଚଳ { ଫେରନ୍ତୁ _fn; }
			ଅଚଳ RTLIL::Const &as_const() ଅଚଳ { ଫେରନ୍ତୁ std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ଅଚଳ { ଫେରନ୍ତୁ std::get<std::pair<IdString, IdString>>(_extra); }
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା as_int() ଅଚଳ { ଫେରନ୍ତୁ std::get<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ଅଚଳ {
				h.eat((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା) _fn);
				h.eat(_extra);
				ଫେରନ୍ତୁ h;
			}
			bool operator==(ଗଣ୍ଠି_Data ଅଚଳ &other) ଅଚଳ {
				ଫେରନ୍ତୁ _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		ସଂରଚନା Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<ଗଣ୍ଠି_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(ଗଣ୍ଠି_2 n);
	ସର୍ବସାଧାରଣ:
		Design *design = nullptr;
		ସ୍ଥିର IR from_module(Module *module);
		Factory factory();
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା size() ଅଚଳ { ଫେରନ୍ତୁ _graph.size(); }
		ଗଣ୍ଠି_2 operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i);
		ଶୂନ୍ୟ topological_sort();
		ଶୂନ୍ୟ forward_buf();
		IRInput ଅଚଳ& ନିବେଶ(IdString name, IdString kind) ଅଚଳ { ଫେରନ୍ତୁ _inputs.at({name, kind}); }
		IRInput ଅଚଳ& ନିବେଶ(IdString name) ଅଚଳ { ଫେରନ୍ତୁ ନିବେଶ(name, ID($ନିବେଶ)); }
		IROutput ଅଚଳ& output(IdString name, IdString kind) ଅଚଳ { ଫେରନ୍ତୁ _outputs.at({name, kind}); }
		IROutput ଅଚଳ& output(IdString name) ଅଚଳ { ଫେରନ୍ତୁ output(name, ID($output)); }
		IRState ଅଚଳ& state(IdString name, IdString kind) ଅଚଳ { ଫେରନ୍ତୁ _states.at({name, kind}); }
		IRState ଅଚଳ& state(IdString name) ଅଚଳ { ଫେରନ୍ତୁ state(name, ID($state)); }
		bool has_ନିବେଶ(IdString name, IdString kind) ଅଚଳ { ଫେରନ୍ତୁ _inputs.ଗଣନା_2({name, kind}); }
		bool has_output(IdString name, IdString kind) ଅଚଳ { ଫେରନ୍ତୁ _outputs.ଗଣନା_2({name, kind}); }
		bool has_state(IdString name, IdString kind) ଅଚଳ { ଫେରନ୍ତୁ _states.ଗଣନା_2({name, kind}); }
		vector<IRInput ଅଚଳ*> inputs(IdString kind) ଅଚଳ;
		vector<IRInput ଅଚଳ*> inputs() ଅଚଳ { ଫେରନ୍ତୁ inputs(ID($ନିବେଶ)); }
		vector<IROutput ଅଚଳ*> outputs(IdString kind) ଅଚଳ;
		vector<IROutput ଅଚଳ*> outputs() ଅଚଳ { ଫେରନ୍ତୁ outputs(ID($output)); }
		vector<IRState ଅଚଳ*> states(IdString kind) ଅଚଳ;
		vector<IRState ଅଚଳ*> states() ଅଚଳ { ଫେରନ୍ତୁ states(ID($state)); }
		vector<IRInput ଅଚଳ*> all_inputs() ଅଚଳ;
		vector<IROutput ଅଚଳ*> all_outputs() ଅଚଳ;
		vector<IRState ଅଚଳ*> all_states() ଅଚଳ;
		class iterator {
			friend class IR;
			IR *_ir;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା _index;
			iterator(IR *ir, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index) : _ir(ir), _index(index) {}
		ସର୍ବସାଧାରଣ:
			using iterator_category = std::ନିବେଶ_iterator_tag;
			using ମୂଲ୍ୟ_type = ଗଣ୍ଠି_2;
			using pointer = arrow_proxy<ଗଣ୍ଠି_2>;
			using reference = ଗଣ୍ଠି_2;
			using difference_type = ptrdiff_t;
			ଗଣ୍ଠି_2 operator*();
			iterator &operator++() { _index++; ଫେରନ୍ତୁ *this; }
			bool operator!=(iterator ଅଚଳ &other) ଅଚଳ { ଫେରନ୍ତୁ _ir != other._ir || _index != other._index; }
			bool operator==(iterator ଅଚଳ &other) ଅଚଳ { ଫେରନ୍ତୁ !(*this != other); }
			pointer operator->();
		};
		iterator begin() { ଫେରନ୍ତୁ iterator(this, 0); }
		iterator end() { ଫେରନ୍ତୁ iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ଗଣ୍ଠି_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		ଗଣ୍ଠି_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { ଫେରନ୍ତୁ _ref; }
	ସର୍ବସାଧାରଣ:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା id() ଅଚଳ { ଫେରନ୍ତୁ _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ଅଚଳ {
			ଯଦି(_ref.has_sparse_attr())
				ଫେରନ୍ତୁ {design, _ref.sparse_attr()};
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() ଅଚଳ { ଫେରନ୍ତୁ _ref.function().fn(); }
		Sort sort() ଅଚଳ { ଫେରନ୍ତୁ _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width() ଅଚଳ { ଫେରନ୍ତୁ sort().width(); }
		size_t arg_ଗଣନା() ଅଚଳ { ଫେରନ୍ତୁ _ref.size(); }
		ଗଣ୍ଠି_2 arg(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) ଅଚଳ { ଫେରନ୍ତୁ ଗଣ୍ଠି_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) ଅଚଳ
		{
			// currently templated but could be switched to AbstractVisitor &
			ବାଛ(_ref.function().fn()) {
			ସ୍ଥିତି Fn::invalid: log_error("invalid node in visit\n"); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::buf: ଫେରନ୍ତୁ v.buf(*this, arg(0)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::slice: ଫେରନ୍ତୁ v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::zero_extend: ଫେରନ୍ତୁ v.zero_extend(*this, arg(0), width()); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::sign_extend: ଫେରନ୍ତୁ v.sign_extend(*this, arg(0), width()); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::concat: ଫେରନ୍ତୁ v.concat(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::add: ଫେରନ୍ତୁ v.add(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::sub: ଫେରନ୍ତୁ v.sub(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::mul: ଫେରନ୍ତୁ v.mul(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::unsigned_div: ଫେରନ୍ତୁ v.unsigned_div(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::unsigned_mod: ଫେରନ୍ତୁ v.unsigned_mod(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::bitwise_and: ଫେରନ୍ତୁ v.bitwise_and(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::bitwise_or: ଫେରନ୍ତୁ v.bitwise_or(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::bitwise_xor: ଫେରନ୍ତୁ v.bitwise_xor(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::bitwise_not: ଫେରନ୍ତୁ v.bitwise_not(*this, arg(0)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::unary_minus: ଫେରନ୍ତୁ v.unary_minus(*this, arg(0)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::reduce_and: ଫେରନ୍ତୁ v.reduce_and(*this, arg(0)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::reduce_or: ଫେରନ୍ତୁ v.reduce_or(*this, arg(0)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::reduce_xor: ଫେରନ୍ତୁ v.reduce_xor(*this, arg(0)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::equal: ଫେରନ୍ତୁ v.equal(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::not_equal: ଫେରନ୍ତୁ v.not_equal(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::signed_greater_than: ଫେରନ୍ତୁ v.signed_greater_than(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::signed_greater_equal: ଫେରନ୍ତୁ v.signed_greater_equal(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::unsigned_greater_than: ଫେରନ୍ତୁ v.unsigned_greater_than(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::unsigned_greater_equal: ଫେରନ୍ତୁ v.unsigned_greater_equal(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::logical_shift_ବାମ: ଫେରନ୍ତୁ v.logical_shift_ବାମ(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::logical_shift_ଡାହାଣ: ଫେରନ୍ତୁ v.logical_shift_ଡାହାଣ(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::arithmetic_shift_ଡାହାଣ: ଫେରନ୍ତୁ v.arithmetic_shift_ଡାହାଣ(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::mux: ଫେରନ୍ତୁ v.mux(*this, arg(0), arg(1), arg(2)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::constant: ଫେରନ୍ତୁ v.constant(*this, _ref.function().as_const()); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::ନିବେଶ: ଫେରନ୍ତୁ v.ନିବେଶ(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::state: ଫେରନ୍ତୁ v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::memory_read: ଫେରନ୍ତୁ v.memory_read(*this, arg(0), arg(1)); ଭାଙ୍ଗ;
			ସ୍ଥିତି Fn::memory_write: ଫେରନ୍ତୁ v.memory_write(*this, arg(0), arg(1), arg(2)); ଭାଙ୍ଗ;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(ଗଣ୍ଠି_2)>);
	};
	ଧାଡ଼ିଭିତରେ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ଧାଡ଼ିଭିତରେ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ଧାଡ଼ିଭିତରେ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ଧାଡ଼ିଭିତରେ IR::Graph::Ref IR::mutate(ଗଣ୍ଠି_2 n) { ଫେରନ୍ତୁ _graph[n._ref.index()]; }
	ଧାଡ଼ିଭିତରେ ଗଣ୍ଠି_2 IR::operator[](ପୂର୍ଣ୍ଣସଂଖ୍ୟା i) { ଫେରନ୍ତୁ ଗଣ୍ଠି_2(_graph[i], design); }
	ଧାଡ଼ିଭିତରେ ଗଣ୍ଠି_2 IROutput::ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ ଗଣ୍ଠି_2(_ir._graph({name, kind, false}), _ir.design); }
	ଧାଡ଼ିଭିତରେ bool IROutput::has_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ _ir._graph.has_ଚାବି({name, kind, false}); }
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ IROutput::set_ମୂଲ୍ୟ(ଗଣ୍ଠି_2 ମୂଲ୍ୟ) { log_assert(sort == ମୂଲ୍ୟ.sort()); _ir.mutate(ମୂଲ୍ୟ).assign_ଚାବି({name, kind, false}); }
	ଧାଡ଼ିଭିତରେ ଗଣ୍ଠି_2 IRState::next_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ ଗଣ୍ଠି_2(_ir._graph({name, kind, true}), _ir.design); }
	ଧାଡ଼ିଭିତରେ bool IRState::has_next_ମୂଲ୍ୟ() ଅଚଳ { ଫେରନ୍ତୁ _ir._graph.has_ଚାବି({name, kind, true}); }
	ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ IRState::set_next_ମୂଲ୍ୟ(ଗଣ୍ଠି_2 ମୂଲ୍ୟ) { log_assert(sort == ମୂଲ୍ୟ.sort()); _ir.mutate(ମୂଲ୍ୟ).assign_ଚାବି({name, kind, true}); }
	ଧାଡ଼ିଭିତରେ ଗଣ୍ଠି_2 IR::iterator::operator*() { ଫେରନ୍ତୁ ଗଣ୍ଠି_2(_ir->_graph[_index], _ir->design); }
	ଧାଡ଼ିଭିତରେ arrow_proxy<ଗଣ୍ଠି_2> IR::iterator::operator->() { ଫେରନ୍ତୁ arrow_proxy<ଗଣ୍ଠି_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> ସଂରଚନା AbstractVisitor {
		ଆଭାସୀ T buf(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 n) = 0;
		ଆଭାସୀ T slice(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width) = 0;
		ଆଭାସୀ T zero_extend(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width) = 0;
		ଆଭାସୀ T sign_extend(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width) = 0;
		ଆଭାସୀ T concat(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T add(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T sub(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T mul(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T unsigned_div(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T unsigned_mod(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T bitwise_and(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T bitwise_or(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T bitwise_xor(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T bitwise_not(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a) = 0;
		ଆଭାସୀ T unary_minus(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a) = 0;
		ଆଭାସୀ T reduce_and(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a) = 0;
		ଆଭାସୀ T reduce_or(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a) = 0;
		ଆଭାସୀ T reduce_xor(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a) = 0;
		ଆଭାସୀ T equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T not_equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T signed_greater_than(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T signed_greater_equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T unsigned_greater_than(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T unsigned_greater_equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T logical_shift_ବାମ(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T logical_shift_ଡାହାଣ(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T arithmetic_shift_ଡାହାଣ(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) = 0;
		ଆଭାସୀ T mux(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b, ଗଣ୍ଠି_2 s) = 0;
		ଆଭାସୀ T constant(ଗଣ୍ଠି_2 self, RTLIL::Const ଅଚଳ & ମୂଲ୍ୟ) = 0;
		ଆଭାସୀ T ନିବେଶ(ଗଣ୍ଠି_2 self, IdString name, IdString kind) = 0;
		ଆଭାସୀ T state(ଗଣ୍ଠି_2 self, IdString name, IdString kind) = 0;
		ଆଭାସୀ T memory_read(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 mem, ଗଣ୍ଠି_2 addr) = 0;
		ଆଭାସୀ T memory_write(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2 mem, ଗଣ୍ଠି_2 addr, ଗଣ୍ଠି_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> ସଂରଚନା DefaultVisitor : ସର୍ବସାଧାରଣ AbstractVisitor<T> {
		ଆଭାସୀ T default_handler(ଗଣ୍ଠି_2 self) = 0;
		T buf(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T slice(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) override { ଫେରନ୍ତୁ default_handler(self); }
		T zero_extend(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) override { ଫେରନ୍ତୁ default_handler(self); }
		T sign_extend(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) override { ଫେରନ୍ତୁ default_handler(self); }
		T concat(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T add(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T sub(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T mul(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T unsigned_div(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T unsigned_mod(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T bitwise_and(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T bitwise_or(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T bitwise_xor(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T bitwise_not(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T unary_minus(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T reduce_and(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T reduce_or(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T reduce_xor(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T not_equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T signed_greater_than(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T signed_greater_equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T unsigned_greater_than(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T unsigned_greater_equal(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T logical_shift_ବାମ(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T logical_shift_ଡାହାଣ(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T arithmetic_shift_ଡାହାଣ(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T mux(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T constant(ଗଣ୍ଠି_2 self, RTLIL::Const ଅଚଳ &) override { ଫେରନ୍ତୁ default_handler(self); }
		T ନିବେଶ(ଗଣ୍ଠି_2 self, IdString, IdString) override { ଫେରନ୍ତୁ default_handler(self); }
		T state(ଗଣ୍ଠି_2 self, IdString, IdString) override { ଫେରନ୍ତୁ default_handler(self); }
		T memory_read(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
		T memory_write(ଗଣ୍ଠି_2 self, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2, ଗଣ୍ଠି_2) override { ଫେରନ୍ତୁ default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ଗଣ୍ଠି_2 add(IR::ଗଣ୍ଠି_Data &&fn, Sort ଅଚଳ &sort, std::initializer_list<ଗଣ୍ଠି_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			ପାଇଁ (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			ଫେରନ୍ତୁ ଗଣ୍ଠି_2(ref, _ir.design);
		}
		ଶୂନ୍ୟ check_basic_ଦ୍ୱିମିକ(ଗଣ୍ଠି_2 ଅଚଳ &a, ଗଣ୍ଠି_2 ଅଚଳ &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		ଶୂନ୍ୟ check_shift(ଗଣ୍ଠି_2 ଅଚଳ &a, ଗଣ୍ଠି_2 ଅଚଳ &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		ଶୂନ୍ୟ check_unary(ଗଣ୍ଠି_2 ଅଚଳ &a) { log_assert(a.sort().is_signal()); }
	ସର୍ବସାଧାରଣ:
		IR &ir() { ଫେରନ୍ତୁ _ir; }
		ଗଣ୍ଠି_2 slice(ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			ଯଦି(offset == 0 && out_width == a.width())
				ଫେରନ୍ତୁ a;
			ଫେରନ୍ତୁ add(IR::ଗଣ୍ଠି_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ଗଣ୍ଠି_2 extend(ଗଣ୍ଠି_2 a, ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width, bool is_signed) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			ଯଦି(in_width == out_width)
				ଫେରନ୍ତୁ a;
			ଯଦି(in_width > out_width)
				ଫେରନ୍ତୁ slice(a, 0, out_width);
			ଯଦି(is_signed)
				ଫେରନ୍ତୁ add(Fn::sign_extend, Sort(out_width), {a});
			ଅନ୍ୟଥା
				ଫେରନ୍ତୁ add(Fn::zero_extend, Sort(out_width), {a});
		}
		ଗଣ୍ଠି_2 concat(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			ଫେରନ୍ତୁ add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		ଗଣ୍ଠି_2 add(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::add, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 sub(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::sub, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 mul(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::mul, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 unsigned_div(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::unsigned_div, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 unsigned_mod(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::unsigned_mod, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 bitwise_and(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::bitwise_and, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 bitwise_or(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::bitwise_or, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 bitwise_xor(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::bitwise_xor, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 bitwise_not(ଗଣ୍ଠି_2 a) { check_unary(a); ଫେରନ୍ତୁ add(Fn::bitwise_not, a.sort(), {a}); }
		ଗଣ୍ଠି_2 unary_minus(ଗଣ୍ଠି_2 a) { check_unary(a); ଫେରନ୍ତୁ add(Fn::unary_minus, a.sort(), {a}); }
		ଗଣ୍ଠି_2 reduce_and(ଗଣ୍ଠି_2 a) {
			check_unary(a);
			ଯଦି(a.width() == 1)
				ଫେରନ୍ତୁ a;
			ଫେରନ୍ତୁ add(Fn::reduce_and, Sort(1), {a});
		}
		ଗଣ୍ଠି_2 reduce_or(ଗଣ୍ଠି_2 a) {
			check_unary(a);
			ଯଦି(a.width() == 1)
				ଫେରନ୍ତୁ a;
			ଫେରନ୍ତୁ add(Fn::reduce_or, Sort(1), {a});
		}
		ଗଣ୍ଠି_2 reduce_xor(ଗଣ୍ଠି_2 a) {
			check_unary(a);
			ଯଦି(a.width() == 1)
				ଫେରନ୍ତୁ a;
			ଫେରନ୍ତୁ add(Fn::reduce_xor, Sort(1), {a});
		}
		ଗଣ୍ଠି_2 equal(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::equal, Sort(1), {a, b}); }
		ଗଣ୍ଠି_2 not_equal(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::not_equal, Sort(1), {a, b}); }
		ଗଣ୍ଠି_2 signed_greater_than(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::signed_greater_than, Sort(1), {a, b}); }
		ଗଣ୍ଠି_2 signed_greater_equal(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		ଗଣ୍ଠି_2 unsigned_greater_than(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		ଗଣ୍ଠି_2 unsigned_greater_equal(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_basic_ଦ୍ୱିମିକ(a, b); ଫେରନ୍ତୁ add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		ଗଣ୍ଠି_2 logical_shift_ବାମ(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_shift(a, b); ଫେରନ୍ତୁ add(Fn::logical_shift_ବାମ, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 logical_shift_ଡାହାଣ(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_shift(a, b); ଫେରନ୍ତୁ add(Fn::logical_shift_ଡାହାଣ, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 arithmetic_shift_ଡାହାଣ(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b) { check_shift(a, b); ଫେରନ୍ତୁ add(Fn::arithmetic_shift_ଡାହାଣ, a.sort(), {a, b}); }
		ଗଣ୍ଠି_2 mux(ଗଣ୍ଠି_2 a, ଗଣ୍ଠି_2 b, ଗଣ୍ଠି_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			ଫେରନ୍ତୁ add(Fn::mux, a.sort(), {a, b, s});
		}
		ଗଣ୍ଠି_2 memory_read(ଗଣ୍ଠି_2 mem, ଗଣ୍ଠି_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			ଫେରନ୍ତୁ add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		ଗଣ୍ଠି_2 memory_write(ଗଣ୍ଠି_2 mem, ଗଣ୍ଠି_2 addr, ଗଣ୍ଠି_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			ଫେରନ୍ତୁ add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		ଗଣ୍ଠି_2 constant(RTLIL::Const ମୂଲ୍ୟ) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା s = ମୂଲ୍ୟ.size();
			ଫେରନ୍ତୁ add(IR::ଗଣ୍ଠି_Data(Fn::constant, std::move(ମୂଲ୍ୟ)), Sort(s), {});
		}
		ଗଣ୍ଠି_2 create_pending(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width) {
			ଫେରନ୍ତୁ add(Fn::buf, Sort(width), {});
		}
		ଶୂନ୍ୟ update_pending(ଗଣ୍ଠି_2 ଗଣ୍ଠି, ଗଣ୍ଠି_2 ମୂଲ୍ୟ) {
			log_assert(ଗଣ୍ଠି._ref.function() == Fn::buf && ଗଣ୍ଠି._ref.size() == 0);
			log_assert(ଗଣ୍ଠି.sort() == ମୂଲ୍ୟ.sort());
			_ir.mutate(ଗଣ୍ଠି).append_arg(ମୂଲ୍ୟ._ref);
		}
		IRInput &add_ନିବେଶ(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			ଯଦି (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			ଫେରନ୍ତୁ it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			ଯଦି (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			ଫେରନ୍ତୁ it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			ଯଦି (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			ଫେରନ୍ତୁ it->second;
		}
		ଗଣ୍ଠି_2 ମୂଲ୍ୟ(IRInput ଅଚଳ& ନିବେଶ) {
			ଫେରନ୍ତୁ add(IR::ଗଣ୍ଠି_Data(Fn::ନିବେଶ, std::pair(ନିବେଶ.name, ନିବେଶ.kind)), ନିବେଶ.sort, {});
		}
		ଗଣ୍ଠି_2 ମୂଲ୍ୟ(IRState ଅଚଳ& state) {
			ଫେରନ୍ତୁ add(IR::ଗଣ୍ଠି_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		ଶୂନ୍ୟ suggest_name(ଗଣ୍ଠି_2 ଗଣ୍ଠି, IdString name) {
			_ir.mutate(ଗଣ୍ଠି).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	ଧାଡ଼ିଭିତରେ Factory IR::factory() { ଫେରନ୍ତୁ Factory(*this); }
	template<class Id> class Scope {
	ସୁରକ୍ଷିତ:
		ଅକ୍ଷର substitution_character = '_';
		ଆଭାସୀ bool is_character_legal(ଅକ୍ଷର, ପୂର୍ଣ୍ଣସଂଖ୍ୟା) = 0;
	ବ୍ୟକ୍ତିଗତ:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	ସର୍ବସାଧାରଣ:
		ଶୂନ୍ୟ reserve(std::string name) {
			_used_names.ଭର୍ତ୍ତି(std::move(name));
		}
		template<typename N> std::string unique_name(ଅଚଳ N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			ପାଇଁ(size_t i = 0; i < str.size(); i++)
				ଯଦି(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			ଯଦି(_used_names.ଗଣନା_2(str) == 0) {
				_used_names.ଭର୍ତ୍ତି(str);
				ଫେରନ୍ତୁ str;
			}
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				ଯଦି(_used_names.ଗଣନା_2(suffixed) == 0) {
					_used_names.ଭର୍ତ୍ତି(suffixed);
					ଫେରନ୍ତୁ suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, ଅଚଳ N &suggestion) {
			auto it = _by_id.find(id);
			ଯଦି(it != _by_id.end())
				ଫେରନ୍ତୁ it->second;
			std::string str = unique_name(suggestion);
			_by_id.ଭର୍ତ୍ତି({id, str});
			ଫେରନ୍ତୁ str;
		}
	};
	class Writer {
		std::ostream *os;
		ଶୂନ୍ୟ print_impl(ଅଚଳ ଅକ୍ଷର *fmt, vector<std::function<ଶୂନ୍ୟ()>>& fns);
	ସର୍ବସାଧାରଣ:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); ଫେରନ୍ତୁ *this; }
		template<typename... Args>
		ଶୂନ୍ୟ print(ଅଚଳ ଅକ୍ଷର *fmt, Args&&... args)
		{
			vector<std::function<ଶୂନ୍ୟ()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		ଶୂନ୍ୟ print_with(Fn fn, ଅଚଳ ଅକ୍ଷର *fmt, Args&&... args)
		{
			vector<std::function<ଶୂନ୍ୟ()>> fns { [&]() {
				ଯଦି constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				ଅନ୍ୟଥା
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
