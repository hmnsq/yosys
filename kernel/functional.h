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

#અન્તર્ભાવ "kernel/yosys.h"
#અન્તર્ભાવ "kernel/compute_graph.h"
#અન્તર્ભાવ "kernel/drivertools.h"
#અન્તર્ભાવ "kernel/mem.h"
#અન્તર્ભાવ "kernel/utils.h"

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
	ગણના class Fn {
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
		logical_shift_ડાબું,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_જમણું,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_જમણું,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		આગત,
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
	અચલ અક્ષર *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<પૂર્ણાંક, std::pair<પૂર્ણાંક, પૂર્ણાંક>> _v;
	જાહેર:
		explicit Sort(પૂર્ણાંક width) : _v(width) { }
		Sort(પૂર્ણાંક addr_width, પૂર્ણાંક data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() અચલ { પાછા _v.index() == 0; }
		bool is_memory() અચલ { પાછા _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		પૂર્ણાંક width() અચલ { પાછા std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		પૂર્ણાંક addr_width() અચલ { પાછા std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		પૂર્ણાંક data_width() અચલ { પાછા std::get<1>(_v).second; }
		bool operator==(Sort અચલ& other) અચલ { પાછા _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) અચલ { h.eat(_v); પાછા h; }
	};
	class IR;
	class Factory;
	class ગાંઠ_2;
	class IRInput {
		friend class Factory;
	જાહેર:
		PooledName name;
		PooledName kind;
		Sort sort;
	ખાનગી:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	જાહેર:
		PooledName name;
		PooledName kind;
		Sort sort;
	ખાનગી:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	જાહેર:
		ગાંઠ_2 કિંમત() અચલ;
		bool has_કિંમત() અચલ;
		શૂન્ય set_કિંમત(ગાંઠ_2 કિંમત);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	જાહેર:
		PooledName name;
		PooledName kind;
		Sort sort;
	ખાનગી:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	જાહેર:
		ગાંઠ_2 next_કિંમત() અચલ;
		bool has_next_કિંમત() અચલ;
		RTLIL::Const અચલ& initial_કિંમત_signal() અચલ { પાછા std::get<RTLIL::Const>(_initial); }
		MemContents અચલ& initial_કિંમત_memory() અચલ { પાછા std::get<MemContents>(_initial); }
		શૂન્ય set_next_કિંમત(ગાંઠ_2 કિંમત);
		શૂન્ય set_initial_કિંમત(RTLIL::Const કિંમત) { કિંમત.extu(sort.width()); _initial = std::move(કિંમત); }
		શૂન્ય set_initial_કિંમત(MemContents કિંમત) { log_assert(Sort(કિંમત.addr_width(), કિંમત.data_width()) == sort); _initial = std::move(કિંમત); }
	};
	class IR {
		friend class Factory;
		friend class ગાંઠ_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ગાંઠ_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				પૂર્ણાંક
			> _extra;
		જાહેર:
			ગાંઠ_Data() : _fn(Fn::invalid) {}
			ગાંઠ_Data(Fn fn) : _fn(fn) {}
			template<class T> ગાંઠ_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() અચલ { પાછા _fn; }
			અચલ RTLIL::Const &as_const() અચલ { પાછા std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() અચલ { પાછા std::get<std::pair<IdString, IdString>>(_extra); }
			પૂર્ણાંક as_int() અચલ { પાછા std::get<પૂર્ણાંક>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) અચલ {
				h.eat((unsigned પૂર્ણાંક) _fn);
				h.eat(_extra);
				પાછા h;
			}
			bool operator==(ગાંઠ_Data અચલ &other) અચલ {
				પાછા _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		સંરચના Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<ગાંઠ_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(ગાંઠ_2 n);
	જાહેર:
		Design *design = nullptr;
		સ્થિર IR from_module(Module *module);
		Factory factory();
		પૂર્ણાંક size() અચલ { પાછા _graph.size(); }
		ગાંઠ_2 operator[](પૂર્ણાંક i);
		શૂન્ય topological_sort();
		શૂન્ય forward_buf();
		IRInput અચલ& આગત(IdString name, IdString kind) અચલ { પાછા _inputs.at({name, kind}); }
		IRInput અચલ& આગત(IdString name) અચલ { પાછા આગત(name, ID($આગત)); }
		IROutput અચલ& output(IdString name, IdString kind) અચલ { પાછા _outputs.at({name, kind}); }
		IROutput અચલ& output(IdString name) અચલ { પાછા output(name, ID($output)); }
		IRState અચલ& state(IdString name, IdString kind) અચલ { પાછા _states.at({name, kind}); }
		IRState અચલ& state(IdString name) અચલ { પાછા state(name, ID($state)); }
		bool has_આગત(IdString name, IdString kind) અચલ { પાછા _inputs.ગણતરી({name, kind}); }
		bool has_output(IdString name, IdString kind) અચલ { પાછા _outputs.ગણતરી({name, kind}); }
		bool has_state(IdString name, IdString kind) અચલ { પાછા _states.ગણતરી({name, kind}); }
		vector<IRInput અચલ*> inputs(IdString kind) અચલ;
		vector<IRInput અચલ*> inputs() અચલ { પાછા inputs(ID($આગત)); }
		vector<IROutput અચલ*> outputs(IdString kind) અચલ;
		vector<IROutput અચલ*> outputs() અચલ { પાછા outputs(ID($output)); }
		vector<IRState અચલ*> states(IdString kind) અચલ;
		vector<IRState અચલ*> states() અચલ { પાછા states(ID($state)); }
		vector<IRInput અચલ*> all_inputs() અચલ;
		vector<IROutput અચલ*> all_outputs() અચલ;
		vector<IRState અચલ*> all_states() અચલ;
		class iterator {
			friend class IR;
			IR *_ir;
			પૂર્ણાંક _index;
			iterator(IR *ir, પૂર્ણાંક index) : _ir(ir), _index(index) {}
		જાહેર:
			using iterator_category = std::આગત_iterator_tag;
			using કિંમત_type = ગાંઠ_2;
			using pointer = arrow_proxy<ગાંઠ_2>;
			using reference = ગાંઠ_2;
			using difference_type = ptrdiff_t;
			ગાંઠ_2 operator*();
			iterator &operator++() { _index++; પાછા *this; }
			bool operator!=(iterator અચલ &other) અચલ { પાછા _ir != other._ir || _index != other._index; }
			bool operator==(iterator અચલ &other) અચલ { પાછા !(*this != other); }
			pointer operator->();
		};
		iterator begin() { પાછા iterator(this, 0); }
		iterator end() { પાછા iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ગાંઠ_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		ગાંઠ_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { પાછા _ref; }
	જાહેર:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		પૂર્ણાંક id() અચલ { પાછા _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() અચલ {
			જો(_ref.has_sparse_attr())
				પાછા {design, _ref.sparse_attr()};
			નહીંતર
				પાછા {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() અચલ { પાછા _ref.function().fn(); }
		Sort sort() અચલ { પાછા _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		પૂર્ણાંક width() અચલ { પાછા sort().width(); }
		size_t arg_ગણતરી() અચલ { પાછા _ref.size(); }
		ગાંઠ_2 arg(પૂર્ણાંક n) અચલ { પાછા ગાંઠ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) અચલ
		{
			// currently templated but could be switched to AbstractVisitor &
			પસંદગી(_ref.function().fn()) {
			સ્થિતિ Fn::invalid: log_error("invalid node in visit\n"); તોડો;
			સ્થિતિ Fn::buf: પાછા v.buf(*this, arg(0)); તોડો;
			સ્થિતિ Fn::slice: પાછા v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); તોડો;
			સ્થિતિ Fn::zero_extend: પાછા v.zero_extend(*this, arg(0), width()); તોડો;
			સ્થિતિ Fn::sign_extend: પાછા v.sign_extend(*this, arg(0), width()); તોડો;
			સ્થિતિ Fn::concat: પાછા v.concat(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::add: પાછા v.add(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::sub: પાછા v.sub(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::mul: પાછા v.mul(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::unsigned_div: પાછા v.unsigned_div(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::unsigned_mod: પાછા v.unsigned_mod(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::bitwise_and: પાછા v.bitwise_and(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::bitwise_or: પાછા v.bitwise_or(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::bitwise_xor: પાછા v.bitwise_xor(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::bitwise_not: પાછા v.bitwise_not(*this, arg(0)); તોડો;
			સ્થિતિ Fn::unary_minus: પાછા v.unary_minus(*this, arg(0)); તોડો;
			સ્થિતિ Fn::reduce_and: પાછા v.reduce_and(*this, arg(0)); તોડો;
			સ્થિતિ Fn::reduce_or: પાછા v.reduce_or(*this, arg(0)); તોડો;
			સ્થિતિ Fn::reduce_xor: પાછા v.reduce_xor(*this, arg(0)); તોડો;
			સ્થિતિ Fn::equal: પાછા v.equal(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::not_equal: પાછા v.not_equal(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::signed_greater_than: પાછા v.signed_greater_than(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::signed_greater_equal: પાછા v.signed_greater_equal(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::unsigned_greater_than: પાછા v.unsigned_greater_than(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::unsigned_greater_equal: પાછા v.unsigned_greater_equal(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::logical_shift_ડાબું: પાછા v.logical_shift_ડાબું(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::logical_shift_જમણું: પાછા v.logical_shift_જમણું(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::arithmetic_shift_જમણું: પાછા v.arithmetic_shift_જમણું(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::mux: પાછા v.mux(*this, arg(0), arg(1), arg(2)); તોડો;
			સ્થિતિ Fn::constant: પાછા v.constant(*this, _ref.function().as_const()); તોડો;
			સ્થિતિ Fn::આગત: પાછા v.આગત(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); તોડો;
			સ્થિતિ Fn::state: પાછા v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); તોડો;
			સ્થિતિ Fn::memory_read: પાછા v.memory_read(*this, arg(0), arg(1)); તોડો;
			સ્થિતિ Fn::memory_write: પાછા v.memory_write(*this, arg(0), arg(1), arg(2)); તોડો;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(ગાંઠ_2)>);
	};
	અંતઃરેખ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	અંતઃરેખ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	અંતઃરેખ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	અંતઃરેખ IR::Graph::Ref IR::mutate(ગાંઠ_2 n) { પાછા _graph[n._ref.index()]; }
	અંતઃરેખ ગાંઠ_2 IR::operator[](પૂર્ણાંક i) { પાછા ગાંઠ_2(_graph[i], design); }
	અંતઃરેખ ગાંઠ_2 IROutput::કિંમત() અચલ { પાછા ગાંઠ_2(_ir._graph({name, kind, false}), _ir.design); }
	અંતઃરેખ bool IROutput::has_કિંમત() અચલ { પાછા _ir._graph.has_ચાવી({name, kind, false}); }
	અંતઃરેખ શૂન્ય IROutput::set_કિંમત(ગાંઠ_2 કિંમત) { log_assert(sort == કિંમત.sort()); _ir.mutate(કિંમત).assign_ચાવી({name, kind, false}); }
	અંતઃરેખ ગાંઠ_2 IRState::next_કિંમત() અચલ { પાછા ગાંઠ_2(_ir._graph({name, kind, true}), _ir.design); }
	અંતઃરેખ bool IRState::has_next_કિંમત() અચલ { પાછા _ir._graph.has_ચાવી({name, kind, true}); }
	અંતઃરેખ શૂન્ય IRState::set_next_કિંમત(ગાંઠ_2 કિંમત) { log_assert(sort == કિંમત.sort()); _ir.mutate(કિંમત).assign_ચાવી({name, kind, true}); }
	અંતઃરેખ ગાંઠ_2 IR::iterator::operator*() { પાછા ગાંઠ_2(_ir->_graph[_index], _ir->design); }
	અંતઃરેખ arrow_proxy<ગાંઠ_2> IR::iterator::operator->() { પાછા arrow_proxy<ગાંઠ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> સંરચના AbstractVisitor {
		આભાસી T buf(ગાંઠ_2 self, ગાંઠ_2 n) = 0;
		આભાસી T slice(ગાંઠ_2 self, ગાંઠ_2 a, પૂર્ણાંક offset, પૂર્ણાંક out_width) = 0;
		આભાસી T zero_extend(ગાંઠ_2 self, ગાંઠ_2 a, પૂર્ણાંક out_width) = 0;
		આભાસી T sign_extend(ગાંઠ_2 self, ગાંઠ_2 a, પૂર્ણાંક out_width) = 0;
		આભાસી T concat(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T add(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T sub(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T mul(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T unsigned_div(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T unsigned_mod(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T bitwise_and(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T bitwise_or(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T bitwise_xor(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T bitwise_not(ગાંઠ_2 self, ગાંઠ_2 a) = 0;
		આભાસી T unary_minus(ગાંઠ_2 self, ગાંઠ_2 a) = 0;
		આભાસી T reduce_and(ગાંઠ_2 self, ગાંઠ_2 a) = 0;
		આભાસી T reduce_or(ગાંઠ_2 self, ગાંઠ_2 a) = 0;
		આભાસી T reduce_xor(ગાંઠ_2 self, ગાંઠ_2 a) = 0;
		આભાસી T equal(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T not_equal(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T signed_greater_than(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T signed_greater_equal(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T unsigned_greater_than(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T unsigned_greater_equal(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T logical_shift_ડાબું(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T logical_shift_જમણું(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T arithmetic_shift_જમણું(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b) = 0;
		આભાસી T mux(ગાંઠ_2 self, ગાંઠ_2 a, ગાંઠ_2 b, ગાંઠ_2 s) = 0;
		આભાસી T constant(ગાંઠ_2 self, RTLIL::Const અચલ & કિંમત) = 0;
		આભાસી T આગત(ગાંઠ_2 self, IdString name, IdString kind) = 0;
		આભાસી T state(ગાંઠ_2 self, IdString name, IdString kind) = 0;
		આભાસી T memory_read(ગાંઠ_2 self, ગાંઠ_2 mem, ગાંઠ_2 addr) = 0;
		આભાસી T memory_write(ગાંઠ_2 self, ગાંઠ_2 mem, ગાંઠ_2 addr, ગાંઠ_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> સંરચના DefaultVisitor : જાહેર AbstractVisitor<T> {
		આભાસી T default_handler(ગાંઠ_2 self) = 0;
		T buf(ગાંઠ_2 self, ગાંઠ_2) override { પાછા default_handler(self); }
		T slice(ગાંઠ_2 self, ગાંઠ_2, પૂર્ણાંક, પૂર્ણાંક) override { પાછા default_handler(self); }
		T zero_extend(ગાંઠ_2 self, ગાંઠ_2, પૂર્ણાંક) override { પાછા default_handler(self); }
		T sign_extend(ગાંઠ_2 self, ગાંઠ_2, પૂર્ણાંક) override { પાછા default_handler(self); }
		T concat(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T add(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T sub(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T mul(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T unsigned_div(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T unsigned_mod(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T bitwise_and(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T bitwise_or(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T bitwise_xor(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T bitwise_not(ગાંઠ_2 self, ગાંઠ_2) override { પાછા default_handler(self); }
		T unary_minus(ગાંઠ_2 self, ગાંઠ_2) override { પાછા default_handler(self); }
		T reduce_and(ગાંઠ_2 self, ગાંઠ_2) override { પાછા default_handler(self); }
		T reduce_or(ગાંઠ_2 self, ગાંઠ_2) override { પાછા default_handler(self); }
		T reduce_xor(ગાંઠ_2 self, ગાંઠ_2) override { પાછા default_handler(self); }
		T equal(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T not_equal(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T signed_greater_than(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T signed_greater_equal(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T unsigned_greater_than(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T unsigned_greater_equal(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T logical_shift_ડાબું(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T logical_shift_જમણું(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T arithmetic_shift_જમણું(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T mux(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T constant(ગાંઠ_2 self, RTLIL::Const અચલ &) override { પાછા default_handler(self); }
		T આગત(ગાંઠ_2 self, IdString, IdString) override { પાછા default_handler(self); }
		T state(ગાંઠ_2 self, IdString, IdString) override { પાછા default_handler(self); }
		T memory_read(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
		T memory_write(ગાંઠ_2 self, ગાંઠ_2, ગાંઠ_2, ગાંઠ_2) override { પાછા default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ગાંઠ_2 add(IR::ગાંઠ_Data &&fn, Sort અચલ &sort, std::initializer_list<ગાંઠ_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			માટે (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			પાછા ગાંઠ_2(ref, _ir.design);
		}
		શૂન્ય check_basic_દ્વિઅંકી(ગાંઠ_2 અચલ &a, ગાંઠ_2 અચલ &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		શૂન્ય check_shift(ગાંઠ_2 અચલ &a, ગાંઠ_2 અચલ &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		શૂન્ય check_unary(ગાંઠ_2 અચલ &a) { log_assert(a.sort().is_signal()); }
	જાહેર:
		IR &ir() { પાછા _ir; }
		ગાંઠ_2 slice(ગાંઠ_2 a, પૂર્ણાંક offset, પૂર્ણાંક out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			જો(offset == 0 && out_width == a.width())
				પાછા a;
			પાછા add(IR::ગાંઠ_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ગાંઠ_2 extend(ગાંઠ_2 a, પૂર્ણાંક out_width, bool is_signed) {
			પૂર્ણાંક in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			જો(in_width == out_width)
				પાછા a;
			જો(in_width > out_width)
				પાછા slice(a, 0, out_width);
			જો(is_signed)
				પાછા add(Fn::sign_extend, Sort(out_width), {a});
			નહીંતર
				પાછા add(Fn::zero_extend, Sort(out_width), {a});
		}
		ગાંઠ_2 concat(ગાંઠ_2 a, ગાંઠ_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			પાછા add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		ગાંઠ_2 add(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::add, a.sort(), {a, b}); }
		ગાંઠ_2 sub(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::sub, a.sort(), {a, b}); }
		ગાંઠ_2 mul(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::mul, a.sort(), {a, b}); }
		ગાંઠ_2 unsigned_div(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::unsigned_div, a.sort(), {a, b}); }
		ગાંઠ_2 unsigned_mod(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::unsigned_mod, a.sort(), {a, b}); }
		ગાંઠ_2 bitwise_and(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::bitwise_and, a.sort(), {a, b}); }
		ગાંઠ_2 bitwise_or(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::bitwise_or, a.sort(), {a, b}); }
		ગાંઠ_2 bitwise_xor(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::bitwise_xor, a.sort(), {a, b}); }
		ગાંઠ_2 bitwise_not(ગાંઠ_2 a) { check_unary(a); પાછા add(Fn::bitwise_not, a.sort(), {a}); }
		ગાંઠ_2 unary_minus(ગાંઠ_2 a) { check_unary(a); પાછા add(Fn::unary_minus, a.sort(), {a}); }
		ગાંઠ_2 reduce_and(ગાંઠ_2 a) {
			check_unary(a);
			જો(a.width() == 1)
				પાછા a;
			પાછા add(Fn::reduce_and, Sort(1), {a});
		}
		ગાંઠ_2 reduce_or(ગાંઠ_2 a) {
			check_unary(a);
			જો(a.width() == 1)
				પાછા a;
			પાછા add(Fn::reduce_or, Sort(1), {a});
		}
		ગાંઠ_2 reduce_xor(ગાંઠ_2 a) {
			check_unary(a);
			જો(a.width() == 1)
				પાછા a;
			પાછા add(Fn::reduce_xor, Sort(1), {a});
		}
		ગાંઠ_2 equal(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::equal, Sort(1), {a, b}); }
		ગાંઠ_2 not_equal(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::not_equal, Sort(1), {a, b}); }
		ગાંઠ_2 signed_greater_than(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::signed_greater_than, Sort(1), {a, b}); }
		ગાંઠ_2 signed_greater_equal(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		ગાંઠ_2 unsigned_greater_than(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		ગાંઠ_2 unsigned_greater_equal(ગાંઠ_2 a, ગાંઠ_2 b) { check_basic_દ્વિઅંકી(a, b); પાછા add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		ગાંઠ_2 logical_shift_ડાબું(ગાંઠ_2 a, ગાંઠ_2 b) { check_shift(a, b); પાછા add(Fn::logical_shift_ડાબું, a.sort(), {a, b}); }
		ગાંઠ_2 logical_shift_જમણું(ગાંઠ_2 a, ગાંઠ_2 b) { check_shift(a, b); પાછા add(Fn::logical_shift_જમણું, a.sort(), {a, b}); }
		ગાંઠ_2 arithmetic_shift_જમણું(ગાંઠ_2 a, ગાંઠ_2 b) { check_shift(a, b); પાછા add(Fn::arithmetic_shift_જમણું, a.sort(), {a, b}); }
		ગાંઠ_2 mux(ગાંઠ_2 a, ગાંઠ_2 b, ગાંઠ_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			પાછા add(Fn::mux, a.sort(), {a, b, s});
		}
		ગાંઠ_2 memory_read(ગાંઠ_2 mem, ગાંઠ_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			પાછા add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		ગાંઠ_2 memory_write(ગાંઠ_2 mem, ગાંઠ_2 addr, ગાંઠ_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			પાછા add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		ગાંઠ_2 constant(RTLIL::Const કિંમત) {
			પૂર્ણાંક s = કિંમત.size();
			પાછા add(IR::ગાંઠ_Data(Fn::constant, std::move(કિંમત)), Sort(s), {});
		}
		ગાંઠ_2 create_pending(પૂર્ણાંક width) {
			પાછા add(Fn::buf, Sort(width), {});
		}
		શૂન્ય update_pending(ગાંઠ_2 ગાંઠ, ગાંઠ_2 કિંમત) {
			log_assert(ગાંઠ._ref.function() == Fn::buf && ગાંઠ._ref.size() == 0);
			log_assert(ગાંઠ.sort() == કિંમત.sort());
			_ir.mutate(ગાંઠ).append_arg(કિંમત._ref);
		}
		IRInput &add_આગત(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			જો (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			પાછા it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			જો (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			પાછા it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			જો (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			પાછા it->second;
		}
		ગાંઠ_2 કિંમત(IRInput અચલ& આગત) {
			પાછા add(IR::ગાંઠ_Data(Fn::આગત, std::pair(આગત.name, આગત.kind)), આગત.sort, {});
		}
		ગાંઠ_2 કિંમત(IRState અચલ& state) {
			પાછા add(IR::ગાંઠ_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		શૂન્ય suggest_name(ગાંઠ_2 ગાંઠ, IdString name) {
			_ir.mutate(ગાંઠ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	અંતઃરેખ Factory IR::factory() { પાછા Factory(*this); }
	template<class Id> class Scope {
	સુરક્ષિત:
		અક્ષર substitution_character = '_';
		આભાસી bool is_character_legal(અક્ષર, પૂર્ણાંક) = 0;
	ખાનગી:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	જાહેર:
		શૂન્ય reserve(std::string name) {
			_used_names.ઉમેરો(std::move(name));
		}
		template<typename N> std::string unique_name(અચલ N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			માટે(size_t i = 0; i < str.size(); i++)
				જો(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			જો(_used_names.ગણતરી(str) == 0) {
				_used_names.ઉમેરો(str);
				પાછા str;
			}
			માટે (પૂર્ણાંક idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				જો(_used_names.ગણતરી(suffixed) == 0) {
					_used_names.ઉમેરો(suffixed);
					પાછા suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, અચલ N &suggestion) {
			auto it = _by_id.find(id);
			જો(it != _by_id.end())
				પાછા it->second;
			std::string str = unique_name(suggestion);
			_by_id.ઉમેરો({id, str});
			પાછા str;
		}
	};
	class Writer {
		std::ostream *os;
		શૂન્ય print_impl(અચલ અક્ષર *fmt, vector<std::function<શૂન્ય()>>& fns);
	જાહેર:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); પાછા *this; }
		template<typename... Args>
		શૂન્ય print(અચલ અક્ષર *fmt, Args&&... args)
		{
			vector<std::function<શૂન્ય()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		શૂન્ય print_with(Fn fn, અચલ અક્ષર *fmt, Args&&... args)
		{
			vector<std::function<શૂન્ય()>> fns { [&]() {
				જો constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				નહીંતર
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
