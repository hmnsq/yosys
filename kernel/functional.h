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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/compute_graph.h"
#ಅನ್ತರ್ಭಾವ "kernel/drivertools.h"
#ಅನ್ತರ್ಭಾವ "kernel/mem.h"
#ಅನ್ತರ್ಭಾವ "kernel/utils.h"

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
	ಗಣನಾ class Fn {
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
		logical_shift_ಎಡ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_ಬಲ,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_ಬಲ,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		ಒಳಹರಿವು,
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
	ಧ್ರುವ ವರ್ಣ *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<ಪೂರ್ಣಾಙ್ಕ, std::pair<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ>> _v;
	ಸಾರ್ವಜನಿಕ:
		explicit Sort(ಪೂರ್ಣಾಙ್ಕ width) : _v(width) { }
		Sort(ಪೂರ್ಣಾಙ್ಕ addr_width, ಪೂರ್ಣಾಙ್ಕ data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _v.index() == 0; }
		bool is_memory() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		ಪೂರ್ಣಾಙ್ಕ width() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		ಪೂರ್ಣಾಙ್ಕ addr_width() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		ಪೂರ್ಣಾಙ್ಕ data_width() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<1>(_v).second; }
		bool operator==(Sort ಧ್ರುವ& other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ { h.eat(_v); ಪ್ರತ್ಯಾಗಮ h; }
	};
	class IR;
	class Factory;
	class ಗಂಟು_2;
	class IRInput {
		friend class Factory;
	ಸಾರ್ವಜನಿಕ:
		PooledName name;
		PooledName kind;
		Sort sort;
	ಗುಪ್ತ:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	ಸಾರ್ವಜನಿಕ:
		PooledName name;
		PooledName kind;
		Sort sort;
	ಗುಪ್ತ:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	ಸಾರ್ವಜನಿಕ:
		ಗಂಟು_2 ಮೌಲ್ಯ() ಧ್ರುವ;
		bool has_ಮೌಲ್ಯ() ಧ್ರುವ;
		ಶೂನ್ಯ set_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	ಸಾರ್ವಜನಿಕ:
		PooledName name;
		PooledName kind;
		Sort sort;
	ಗುಪ್ತ:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	ಸಾರ್ವಜನಿಕ:
		ಗಂಟು_2 next_ಮೌಲ್ಯ() ಧ್ರುವ;
		bool has_next_ಮೌಲ್ಯ() ಧ್ರುವ;
		RTLIL::Const ಧ್ರುವ& initial_ಮೌಲ್ಯ_signal() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<RTLIL::Const>(_initial); }
		MemContents ಧ್ರುವ& initial_ಮೌಲ್ಯ_memory() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<MemContents>(_initial); }
		ಶೂನ್ಯ set_next_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ);
		ಶೂನ್ಯ set_initial_ಮೌಲ್ಯ(RTLIL::Const ಮೌಲ್ಯ) { ಮೌಲ್ಯ.extu(sort.width()); _initial = std::move(ಮೌಲ್ಯ); }
		ಶೂನ್ಯ set_initial_ಮೌಲ್ಯ(MemContents ಮೌಲ್ಯ) { log_assert(Sort(ಮೌಲ್ಯ.addr_width(), ಮೌಲ್ಯ.data_width()) == sort); _initial = std::move(ಮೌಲ್ಯ); }
	};
	class IR {
		friend class Factory;
		friend class ಗಂಟು_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ಗಂಟು_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				ಪೂರ್ಣಾಙ್ಕ
			> _extra;
		ಸಾರ್ವಜನಿಕ:
			ಗಂಟು_Data() : _fn(Fn::invalid) {}
			ಗಂಟು_Data(Fn fn) : _fn(fn) {}
			template<class T> ಗಂಟು_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _fn; }
			ಧ್ರುವ RTLIL::Const &as_const() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<std::pair<IdString, IdString>>(_extra); }
			ಪೂರ್ಣಾಙ್ಕ as_int() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ std::get<ಪೂರ್ಣಾಙ್ಕ>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ಧ್ರುವ {
				h.eat((unsigned ಪೂರ್ಣಾಙ್ಕ) _fn);
				h.eat(_extra);
				ಪ್ರತ್ಯಾಗಮ h;
			}
			bool operator==(ಗಂಟು_Data ಧ್ರುವ &other) ಧ್ರುವ {
				ಪ್ರತ್ಯಾಗಮ _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		ಸಂರಚನಾ Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<ಗಂಟು_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(ಗಂಟು_2 n);
	ಸಾರ್ವಜನಿಕ:
		Design *design = nullptr;
		ಸ್ಥಿರ IR from_module(Module *module);
		Factory factory();
		ಪೂರ್ಣಾಙ್ಕ size() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _graph.size(); }
		ಗಂಟು_2 operator[](ಪೂರ್ಣಾಙ್ಕ i);
		ಶೂನ್ಯ topological_sort();
		ಶೂನ್ಯ forward_buf();
		IRInput ಧ್ರುವ& ಒಳಹರಿವು(IdString name, IdString kind) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _inputs.at({name, kind}); }
		IRInput ಧ್ರುವ& ಒಳಹರಿವು(IdString name) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಒಳಹರಿವು(name, ID($ಒಳಹರಿವು)); }
		IROutput ಧ್ರುವ& output(IdString name, IdString kind) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _outputs.at({name, kind}); }
		IROutput ಧ್ರುವ& output(IdString name) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ output(name, ID($output)); }
		IRState ಧ್ರುವ& state(IdString name, IdString kind) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _states.at({name, kind}); }
		IRState ಧ್ರುವ& state(IdString name) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ state(name, ID($state)); }
		bool has_ಒಳಹರಿವು(IdString name, IdString kind) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _inputs.ಎಣಿಕೆ({name, kind}); }
		bool has_output(IdString name, IdString kind) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _outputs.ಎಣಿಕೆ({name, kind}); }
		bool has_state(IdString name, IdString kind) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _states.ಎಣಿಕೆ({name, kind}); }
		vector<IRInput ಧ್ರುವ*> inputs(IdString kind) ಧ್ರುವ;
		vector<IRInput ಧ್ರುವ*> inputs() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ inputs(ID($ಒಳಹರಿವು)); }
		vector<IROutput ಧ್ರುವ*> outputs(IdString kind) ಧ್ರುವ;
		vector<IROutput ಧ್ರುವ*> outputs() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ outputs(ID($output)); }
		vector<IRState ಧ್ರುವ*> states(IdString kind) ಧ್ರುವ;
		vector<IRState ಧ್ರುವ*> states() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ states(ID($state)); }
		vector<IRInput ಧ್ರುವ*> all_inputs() ಧ್ರುವ;
		vector<IROutput ಧ್ರುವ*> all_outputs() ಧ್ರುವ;
		vector<IRState ಧ್ರುವ*> all_states() ಧ್ರುವ;
		class iterator {
			friend class IR;
			IR *_ir;
			ಪೂರ್ಣಾಙ್ಕ _index;
			iterator(IR *ir, ಪೂರ್ಣಾಙ್ಕ index) : _ir(ir), _index(index) {}
		ಸಾರ್ವಜನಿಕ:
			using iterator_category = std::ಒಳಹರಿವು_iterator_tag;
			using ಮೌಲ್ಯ_type = ಗಂಟು_2;
			using pointer = arrow_proxy<ಗಂಟು_2>;
			using reference = ಗಂಟು_2;
			using difference_type = ptrdiff_t;
			ಗಂಟು_2 operator*();
			iterator &operator++() { _index++; ಪ್ರತ್ಯಾಗಮ *this; }
			bool operator!=(iterator ಧ್ರುವ &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ir != other._ir || _index != other._index; }
			bool operator==(iterator ಧ್ರುವ &other) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ !(*this != other); }
			pointer operator->();
		};
		iterator begin() { ಪ್ರತ್ಯಾಗಮ iterator(this, 0); }
		iterator end() { ಪ್ರತ್ಯಾಗಮ iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ಗಂಟು_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		ಗಂಟು_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { ಪ್ರತ್ಯಾಗಮ _ref; }
	ಸಾರ್ವಜನಿಕ:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		ಪೂರ್ಣಾಙ್ಕ id() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ಧ್ರುವ {
			ಯದಿ(_ref.has_sparse_attr())
				ಪ್ರತ್ಯಾಗಮ {design, _ref.sparse_attr()};
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ref.function().fn(); }
		Sort sort() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		ಪೂರ್ಣಾಙ್ಕ width() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ sort().width(); }
		size_t arg_ಎಣಿಕೆ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ref.size(); }
		ಗಂಟು_2 arg(ಪೂರ್ಣಾಙ್ಕ n) ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಗಂಟು_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) ಧ್ರುವ
		{
			// currently templated but could be switched to AbstractVisitor &
			ವಿಕಲ್ಪ(_ref.function().fn()) {
			ಪಕ್ಷ Fn::invalid: log_error("invalid node in visit\n"); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::buf: ಪ್ರತ್ಯಾಗಮ v.buf(*this, arg(0)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::slice: ಪ್ರತ್ಯಾಗಮ v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::zero_extend: ಪ್ರತ್ಯಾಗಮ v.zero_extend(*this, arg(0), width()); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::sign_extend: ಪ್ರತ್ಯಾಗಮ v.sign_extend(*this, arg(0), width()); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::concat: ಪ್ರತ್ಯಾಗಮ v.concat(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::add: ಪ್ರತ್ಯಾಗಮ v.add(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::sub: ಪ್ರತ್ಯಾಗಮ v.sub(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::mul: ಪ್ರತ್ಯಾಗಮ v.mul(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::unsigned_div: ಪ್ರತ್ಯಾಗಮ v.unsigned_div(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::unsigned_mod: ಪ್ರತ್ಯಾಗಮ v.unsigned_mod(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::bitwise_and: ಪ್ರತ್ಯಾಗಮ v.bitwise_and(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::bitwise_or: ಪ್ರತ್ಯಾಗಮ v.bitwise_or(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::bitwise_xor: ಪ್ರತ್ಯಾಗಮ v.bitwise_xor(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::bitwise_not: ಪ್ರತ್ಯಾಗಮ v.bitwise_not(*this, arg(0)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::unary_minus: ಪ್ರತ್ಯಾಗಮ v.unary_minus(*this, arg(0)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::reduce_and: ಪ್ರತ್ಯಾಗಮ v.reduce_and(*this, arg(0)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::reduce_or: ಪ್ರತ್ಯಾಗಮ v.reduce_or(*this, arg(0)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::reduce_xor: ಪ್ರತ್ಯಾಗಮ v.reduce_xor(*this, arg(0)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::equal: ಪ್ರತ್ಯಾಗಮ v.equal(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::not_equal: ಪ್ರತ್ಯಾಗಮ v.not_equal(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::signed_greater_than: ಪ್ರತ್ಯಾಗಮ v.signed_greater_than(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::signed_greater_equal: ಪ್ರತ್ಯಾಗಮ v.signed_greater_equal(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::unsigned_greater_than: ಪ್ರತ್ಯಾಗಮ v.unsigned_greater_than(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::unsigned_greater_equal: ಪ್ರತ್ಯಾಗಮ v.unsigned_greater_equal(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::logical_shift_ಎಡ: ಪ್ರತ್ಯಾಗಮ v.logical_shift_ಎಡ(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::logical_shift_ಬಲ: ಪ್ರತ್ಯಾಗಮ v.logical_shift_ಬಲ(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::arithmetic_shift_ಬಲ: ಪ್ರತ್ಯಾಗಮ v.arithmetic_shift_ಬಲ(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::mux: ಪ್ರತ್ಯಾಗಮ v.mux(*this, arg(0), arg(1), arg(2)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::constant: ಪ್ರತ್ಯಾಗಮ v.constant(*this, _ref.function().as_const()); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::ಒಳಹರಿವು: ಪ್ರತ್ಯಾಗಮ v.ಒಳಹರಿವು(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::state: ಪ್ರತ್ಯಾಗಮ v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::memory_read: ಪ್ರತ್ಯಾಗಮ v.memory_read(*this, arg(0), arg(1)); ಭಙ್ಗ;
			ಪಕ್ಷ Fn::memory_write: ಪ್ರತ್ಯಾಗಮ v.memory_write(*this, arg(0), arg(1), arg(2)); ಭಙ್ಗ;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(ಗಂಟು_2)>);
	};
	ಅನ್ತಃಸ್ಥ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ಅನ್ತಃಸ್ಥ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ಅನ್ತಃಸ್ಥ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	ಅನ್ತಃಸ್ಥ IR::Graph::Ref IR::mutate(ಗಂಟು_2 n) { ಪ್ರತ್ಯಾಗಮ _graph[n._ref.index()]; }
	ಅನ್ತಃಸ್ಥ ಗಂಟು_2 IR::operator[](ಪೂರ್ಣಾಙ್ಕ i) { ಪ್ರತ್ಯಾಗಮ ಗಂಟು_2(_graph[i], design); }
	ಅನ್ತಃಸ್ಥ ಗಂಟು_2 IROutput::ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಗಂಟು_2(_ir._graph({name, kind, false}), _ir.design); }
	ಅನ್ತಃಸ್ಥ bool IROutput::has_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ir._graph.has_ಕೀಲಿ({name, kind, false}); }
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ IROutput::set_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ) { log_assert(sort == ಮೌಲ್ಯ.sort()); _ir.mutate(ಮೌಲ್ಯ).assign_ಕೀಲಿ({name, kind, false}); }
	ಅನ್ತಃಸ್ಥ ಗಂಟು_2 IRState::next_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ ಗಂಟು_2(_ir._graph({name, kind, true}), _ir.design); }
	ಅನ್ತಃಸ್ಥ bool IRState::has_next_ಮೌಲ್ಯ() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _ir._graph.has_ಕೀಲಿ({name, kind, true}); }
	ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ IRState::set_next_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ) { log_assert(sort == ಮೌಲ್ಯ.sort()); _ir.mutate(ಮೌಲ್ಯ).assign_ಕೀಲಿ({name, kind, true}); }
	ಅನ್ತಃಸ್ಥ ಗಂಟು_2 IR::iterator::operator*() { ಪ್ರತ್ಯಾಗಮ ಗಂಟು_2(_ir->_graph[_index], _ir->design); }
	ಅನ್ತಃಸ್ಥ arrow_proxy<ಗಂಟು_2> IR::iterator::operator->() { ಪ್ರತ್ಯಾಗಮ arrow_proxy<ಗಂಟು_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> ಸಂರಚನಾ AbstractVisitor {
		ಆಭಾಸ T buf(ಗಂಟು_2 self, ಗಂಟು_2 n) = 0;
		ಆಭಾಸ T slice(ಗಂಟು_2 self, ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ out_width) = 0;
		ಆಭಾಸ T zero_extend(ಗಂಟು_2 self, ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ out_width) = 0;
		ಆಭಾಸ T sign_extend(ಗಂಟು_2 self, ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ out_width) = 0;
		ಆಭಾಸ T concat(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T add(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T sub(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T mul(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T unsigned_div(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T unsigned_mod(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T bitwise_and(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T bitwise_or(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T bitwise_xor(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T bitwise_not(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ಆಭಾಸ T unary_minus(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ಆಭಾಸ T reduce_and(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ಆಭಾಸ T reduce_or(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ಆಭಾಸ T reduce_xor(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ಆಭಾಸ T equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T not_equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T signed_greater_than(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T signed_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T unsigned_greater_than(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T unsigned_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T logical_shift_ಎಡ(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T logical_shift_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T arithmetic_shift_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ಆಭಾಸ T mux(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) = 0;
		ಆಭಾಸ T constant(ಗಂಟು_2 self, RTLIL::Const ಧ್ರುವ & ಮೌಲ್ಯ) = 0;
		ಆಭಾಸ T ಒಳಹರಿವು(ಗಂಟು_2 self, IdString name, IdString kind) = 0;
		ಆಭಾಸ T state(ಗಂಟು_2 self, IdString name, IdString kind) = 0;
		ಆಭಾಸ T memory_read(ಗಂಟು_2 self, ಗಂಟು_2 mem, ಗಂಟು_2 addr) = 0;
		ಆಭಾಸ T memory_write(ಗಂಟು_2 self, ಗಂಟು_2 mem, ಗಂಟು_2 addr, ಗಂಟು_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> ಸಂರಚನಾ DefaultVisitor : ಸಾರ್ವಜನಿಕ AbstractVisitor<T> {
		ಆಭಾಸ T default_handler(ಗಂಟು_2 self) = 0;
		T buf(ಗಂಟು_2 self, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T slice(ಗಂಟು_2 self, ಗಂಟು_2, ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T zero_extend(ಗಂಟು_2 self, ಗಂಟು_2, ಪೂರ್ಣಾಙ್ಕ) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T sign_extend(ಗಂಟು_2 self, ಗಂಟು_2, ಪೂರ್ಣಾಙ್ಕ) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T concat(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T add(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T sub(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T mul(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T unsigned_div(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T unsigned_mod(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T bitwise_and(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T bitwise_or(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T bitwise_xor(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T bitwise_not(ಗಂಟು_2 self, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T unary_minus(ಗಂಟು_2 self, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T reduce_and(ಗಂಟು_2 self, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T reduce_or(ಗಂಟು_2 self, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T reduce_xor(ಗಂಟು_2 self, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T not_equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T signed_greater_than(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T signed_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T unsigned_greater_than(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T unsigned_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T logical_shift_ಎಡ(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T logical_shift_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T arithmetic_shift_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T mux(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T constant(ಗಂಟು_2 self, RTLIL::Const ಧ್ರುವ &) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T ಒಳಹರಿವು(ಗಂಟು_2 self, IdString, IdString) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T state(ಗಂಟು_2 self, IdString, IdString) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T memory_read(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
		T memory_write(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2, ಗಂಟು_2) override { ಪ್ರತ್ಯಾಗಮ default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ಗಂಟು_2 add(IR::ಗಂಟು_Data &&fn, Sort ಧ್ರುವ &sort, std::initializer_list<ಗಂಟು_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			ಪ್ರತಿ (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			ಪ್ರತ್ಯಾಗಮ ಗಂಟು_2(ref, _ir.design);
		}
		ಶೂನ್ಯ check_basic_ದ್ವಿಮಾನ(ಗಂಟು_2 ಧ್ರುವ &a, ಗಂಟು_2 ಧ್ರುವ &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		ಶೂನ್ಯ check_shift(ಗಂಟು_2 ಧ್ರುವ &a, ಗಂಟು_2 ಧ್ರುವ &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		ಶೂನ್ಯ check_unary(ಗಂಟು_2 ಧ್ರುವ &a) { log_assert(a.sort().is_signal()); }
	ಸಾರ್ವಜನಿಕ:
		IR &ir() { ಪ್ರತ್ಯಾಗಮ _ir; }
		ಗಂಟು_2 slice(ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			ಯದಿ(offset == 0 && out_width == a.width())
				ಪ್ರತ್ಯಾಗಮ a;
			ಪ್ರತ್ಯಾಗಮ add(IR::ಗಂಟು_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ಗಂಟು_2 extend(ಗಂಟು_2 a, ಪೂರ್ಣಾಙ್ಕ out_width, bool is_signed) {
			ಪೂರ್ಣಾಙ್ಕ in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			ಯದಿ(in_width == out_width)
				ಪ್ರತ್ಯಾಗಮ a;
			ಯದಿ(in_width > out_width)
				ಪ್ರತ್ಯಾಗಮ slice(a, 0, out_width);
			ಯದಿ(is_signed)
				ಪ್ರತ್ಯಾಗಮ add(Fn::sign_extend, Sort(out_width), {a});
			ಅನ್ಯಥಾ
				ಪ್ರತ್ಯಾಗಮ add(Fn::zero_extend, Sort(out_width), {a});
		}
		ಗಂಟು_2 concat(ಗಂಟು_2 a, ಗಂಟು_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			ಪ್ರತ್ಯಾಗಮ add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		ಗಂಟು_2 add(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::add, a.sort(), {a, b}); }
		ಗಂಟು_2 sub(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::sub, a.sort(), {a, b}); }
		ಗಂಟು_2 mul(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::mul, a.sort(), {a, b}); }
		ಗಂಟು_2 unsigned_div(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::unsigned_div, a.sort(), {a, b}); }
		ಗಂಟು_2 unsigned_mod(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::unsigned_mod, a.sort(), {a, b}); }
		ಗಂಟು_2 bitwise_and(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::bitwise_and, a.sort(), {a, b}); }
		ಗಂಟು_2 bitwise_or(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::bitwise_or, a.sort(), {a, b}); }
		ಗಂಟು_2 bitwise_xor(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::bitwise_xor, a.sort(), {a, b}); }
		ಗಂಟು_2 bitwise_not(ಗಂಟು_2 a) { check_unary(a); ಪ್ರತ್ಯಾಗಮ add(Fn::bitwise_not, a.sort(), {a}); }
		ಗಂಟು_2 unary_minus(ಗಂಟು_2 a) { check_unary(a); ಪ್ರತ್ಯಾಗಮ add(Fn::unary_minus, a.sort(), {a}); }
		ಗಂಟು_2 reduce_and(ಗಂಟು_2 a) {
			check_unary(a);
			ಯದಿ(a.width() == 1)
				ಪ್ರತ್ಯಾಗಮ a;
			ಪ್ರತ್ಯಾಗಮ add(Fn::reduce_and, Sort(1), {a});
		}
		ಗಂಟು_2 reduce_or(ಗಂಟು_2 a) {
			check_unary(a);
			ಯದಿ(a.width() == 1)
				ಪ್ರತ್ಯಾಗಮ a;
			ಪ್ರತ್ಯಾಗಮ add(Fn::reduce_or, Sort(1), {a});
		}
		ಗಂಟು_2 reduce_xor(ಗಂಟು_2 a) {
			check_unary(a);
			ಯದಿ(a.width() == 1)
				ಪ್ರತ್ಯಾಗಮ a;
			ಪ್ರತ್ಯಾಗಮ add(Fn::reduce_xor, Sort(1), {a});
		}
		ಗಂಟು_2 equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::equal, Sort(1), {a, b}); }
		ಗಂಟು_2 not_equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::not_equal, Sort(1), {a, b}); }
		ಗಂಟು_2 signed_greater_than(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::signed_greater_than, Sort(1), {a, b}); }
		ಗಂಟು_2 signed_greater_equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		ಗಂಟು_2 unsigned_greater_than(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		ಗಂಟು_2 unsigned_greater_equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		ಗಂಟು_2 logical_shift_ಎಡ(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_shift(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::logical_shift_ಎಡ, a.sort(), {a, b}); }
		ಗಂಟು_2 logical_shift_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_shift(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::logical_shift_ಬಲ, a.sort(), {a, b}); }
		ಗಂಟು_2 arithmetic_shift_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_shift(a, b); ಪ್ರತ್ಯಾಗಮ add(Fn::arithmetic_shift_ಬಲ, a.sort(), {a, b}); }
		ಗಂಟು_2 mux(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			ಪ್ರತ್ಯಾಗಮ add(Fn::mux, a.sort(), {a, b, s});
		}
		ಗಂಟು_2 memory_read(ಗಂಟು_2 mem, ಗಂಟು_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			ಪ್ರತ್ಯಾಗಮ add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		ಗಂಟು_2 memory_write(ಗಂಟು_2 mem, ಗಂಟು_2 addr, ಗಂಟು_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			ಪ್ರತ್ಯಾಗಮ add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		ಗಂಟು_2 constant(RTLIL::Const ಮೌಲ್ಯ) {
			ಪೂರ್ಣಾಙ್ಕ s = ಮೌಲ್ಯ.size();
			ಪ್ರತ್ಯಾಗಮ add(IR::ಗಂಟು_Data(Fn::constant, std::move(ಮೌಲ್ಯ)), Sort(s), {});
		}
		ಗಂಟು_2 create_pending(ಪೂರ್ಣಾಙ್ಕ width) {
			ಪ್ರತ್ಯಾಗಮ add(Fn::buf, Sort(width), {});
		}
		ಶೂನ್ಯ update_pending(ಗಂಟು_2 ಗಂಟು, ಗಂಟು_2 ಮೌಲ್ಯ) {
			log_assert(ಗಂಟು._ref.function() == Fn::buf && ಗಂಟು._ref.size() == 0);
			log_assert(ಗಂಟು.sort() == ಮೌಲ್ಯ.sort());
			_ir.mutate(ಗಂಟು).append_arg(ಮೌಲ್ಯ._ref);
		}
		IRInput &add_ಒಳಹರಿವು(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			ಯದಿ (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			ಪ್ರತ್ಯಾಗಮ it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			ಯದಿ (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			ಪ್ರತ್ಯಾಗಮ it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			ಯದಿ (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			ಪ್ರತ್ಯಾಗಮ it->second;
		}
		ಗಂಟು_2 ಮೌಲ್ಯ(IRInput ಧ್ರುವ& ಒಳಹರಿವು) {
			ಪ್ರತ್ಯಾಗಮ add(IR::ಗಂಟು_Data(Fn::ಒಳಹರಿವು, std::pair(ಒಳಹರಿವು.name, ಒಳಹರಿವು.kind)), ಒಳಹರಿವು.sort, {});
		}
		ಗಂಟು_2 ಮೌಲ್ಯ(IRState ಧ್ರುವ& state) {
			ಪ್ರತ್ಯಾಗಮ add(IR::ಗಂಟು_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		ಶೂನ್ಯ suggest_name(ಗಂಟು_2 ಗಂಟು, IdString name) {
			_ir.mutate(ಗಂಟು).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	ಅನ್ತಃಸ್ಥ Factory IR::factory() { ಪ್ರತ್ಯಾಗಮ Factory(*this); }
	template<class Id> class Scope {
	ರಕ್ಷಿತ:
		ವರ್ಣ substitution_character = '_';
		ಆಭಾಸ bool is_character_legal(ವರ್ಣ, ಪೂರ್ಣಾಙ್ಕ) = 0;
	ಗುಪ್ತ:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	ಸಾರ್ವಜನಿಕ:
		ಶೂನ್ಯ reserve(std::string name) {
			_used_names.ಸೇರಿಸು(std::move(name));
		}
		template<typename N> std::string unique_name(ಧ್ರುವ N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			ಪ್ರತಿ(size_t i = 0; i < str.size(); i++)
				ಯದಿ(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			ಯದಿ(_used_names.ಎಣಿಕೆ(str) == 0) {
				_used_names.ಸೇರಿಸು(str);
				ಪ್ರತ್ಯಾಗಮ str;
			}
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				ಯದಿ(_used_names.ಎಣಿಕೆ(suffixed) == 0) {
					_used_names.ಸೇರಿಸು(suffixed);
					ಪ್ರತ್ಯಾಗಮ suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, ಧ್ರುವ N &suggestion) {
			auto it = _by_id.find(id);
			ಯದಿ(it != _by_id.end())
				ಪ್ರತ್ಯಾಗಮ it->second;
			std::string str = unique_name(suggestion);
			_by_id.ಸೇರಿಸು({id, str});
			ಪ್ರತ್ಯಾಗಮ str;
		}
	};
	class Writer {
		std::ostream *os;
		ಶೂನ್ಯ print_impl(ಧ್ರುವ ವರ್ಣ *fmt, vector<std::function<ಶೂನ್ಯ()>>& fns);
	ಸಾರ್ವಜನಿಕ:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); ಪ್ರತ್ಯಾಗಮ *this; }
		template<typename... Args>
		ಶೂನ್ಯ print(ಧ್ರುವ ವರ್ಣ *fmt, Args&&... args)
		{
			vector<std::function<ಶೂನ್ಯ()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		ಶೂನ್ಯ print_with(Fn fn, ಧ್ರುವ ವರ್ಣ *fmt, Args&&... args)
		{
			vector<std::function<ಶೂನ್ಯ()>> fns { [&]() {
				ಯದಿ constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				ಅನ್ಯಥಾ
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
