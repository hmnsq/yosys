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

#include "kernel/yosys.h"
#include "kernel/compute_graph.h"
#include "kernel/drivertools.h"
#include "kernel/mem.h"
#include "kernel/utils.h"

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
	ga_nanaa class Fn {
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
		logical_shift_left,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_right,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_right,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		input,
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
	const var_na *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<puur_naa_nka, std::pair<puur_naa_nka, puur_naa_nka>> _v;
	public:
		explicit Sort(puur_naa_nka width) : _v(width) { }
		Sort(puur_naa_nka addr_width, puur_naa_nka data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() const { ꦮꦭꦶ _v.index() == 0; }
		bool is_memory() const { ꦮꦭꦶ _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		puur_naa_nka width() const { ꦮꦭꦶ std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		puur_naa_nka addr_width() const { ꦮꦭꦶ std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		puur_naa_nka data_width() const { ꦮꦭꦶ std::get<1>(_v).second; }
		bool operator==(Sort const& other) const { ꦮꦭꦶ _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) const { h.eat(_v); ꦮꦭꦶ h; }
	};
	class IR;
	class Factory;
	class Node;
	class IRInput {
		friend class Factory;
	public:
		PooledName name;
		PooledName kind;
		Sort sort;
	private:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	public:
		PooledName name;
		PooledName kind;
		Sort sort;
	private:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	public:
		Node value() const;
		bool has_value() const;
		void set_value(Node value);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	public:
		PooledName name;
		PooledName kind;
		Sort sort;
	private:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	public:
		Node next_value() const;
		bool has_next_value() const;
		RTLIL::Const const& initial_value_signal() const { ꦮꦭꦶ std::get<RTLIL::Const>(_initial); }
		MemContents const& initial_value_memory() const { ꦮꦭꦶ std::get<MemContents>(_initial); }
		void set_next_value(Node value);
		void set_initial_value(RTLIL::Const value) { value.extu(sort.width()); _initial = std::move(value); }
		void set_initial_value(MemContents value) { log_assert(Sort(value.addr_width(), value.data_width()) == sort); _initial = std::move(value); }
	};
	class IR {
		friend class Factory;
		friend class Node;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class NodeData {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				puur_naa_nka
			> _extra;
		public:
			NodeData() : _fn(Fn::invalid) {}
			NodeData(Fn fn) : _fn(fn) {}
			template<class T> NodeData(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() const { ꦮꦭꦶ _fn; }
			const RTLIL::Const &as_const() const { ꦮꦭꦶ std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() const { ꦮꦭꦶ std::get<std::pair<IdString, IdString>>(_extra); }
			puur_naa_nka as_int() const { ꦮꦭꦶ std::get<puur_naa_nka>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) const {
				h.eat((unsigned puur_naa_nka) _fn);
				h.eat(_extra);
				ꦮꦭꦶ h;
			}
			bool operator==(NodeData const &other) const {
				ꦮꦭꦶ _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		sa_nracanaa Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<NodeData, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(Node n);
	public:
		Design *design = nullptr;
		static IR from_module(Module *module);
		Factory factory();
		puur_naa_nka size() const { ꦮꦭꦶ _graph.size(); }
		Node operator[](puur_naa_nka i);
		void topological_sort();
		void forward_buf();
		IRInput const& input(IdString name, IdString kind) const { ꦮꦭꦶ _inputs.at({name, kind}); }
		IRInput const& input(IdString name) const { ꦮꦭꦶ input(name, ID($input)); }
		IROutput const& output(IdString name, IdString kind) const { ꦮꦭꦶ _outputs.at({name, kind}); }
		IROutput const& output(IdString name) const { ꦮꦭꦶ output(name, ID($output)); }
		IRState const& state(IdString name, IdString kind) const { ꦮꦭꦶ _states.at({name, kind}); }
		IRState const& state(IdString name) const { ꦮꦭꦶ state(name, ID($state)); }
		bool has_input(IdString name, IdString kind) const { ꦮꦭꦶ _inputs.count({name, kind}); }
		bool has_output(IdString name, IdString kind) const { ꦮꦭꦶ _outputs.count({name, kind}); }
		bool has_state(IdString name, IdString kind) const { ꦮꦭꦶ _states.count({name, kind}); }
		vector<IRInput const*> inputs(IdString kind) const;
		vector<IRInput const*> inputs() const { ꦮꦭꦶ inputs(ID($input)); }
		vector<IROutput const*> outputs(IdString kind) const;
		vector<IROutput const*> outputs() const { ꦮꦭꦶ outputs(ID($output)); }
		vector<IRState const*> states(IdString kind) const;
		vector<IRState const*> states() const { ꦮꦭꦶ states(ID($state)); }
		vector<IRInput const*> all_inputs() const;
		vector<IROutput const*> all_outputs() const;
		vector<IRState const*> all_states() const;
		class iterator {
			friend class IR;
			IR *_ir;
			puur_naa_nka _index;
			iterator(IR *ir, puur_naa_nka index) : _ir(ir), _index(index) {}
		public:
			using iterator_category = std::input_iterator_tag;
			using value_type = Node;
			using pointer = arrow_proxy<Node>;
			using reference = Node;
			using difference_type = ptrdiff_t;
			Node operator*();
			iterator &operator++() { _index++; ꦮꦭꦶ *this; }
			bool operator!=(iterator const &other) const { ꦮꦭꦶ _ir != other._ir || _index != other._index; }
			bool operator==(iterator const &other) const { ꦮꦭꦶ !(*this != other); }
			pointer operator->();
		};
		iterator begin() { ꦮꦭꦶ iterator(this, 0); }
		iterator end() { ꦮꦭꦶ iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class Node {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		Node(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { ꦮꦭꦶ _ref; }
	public:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		puur_naa_nka id() const { ꦮꦭꦶ _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() const {
			ꦪꦼꦤ꧀(_ref.has_sparse_attr())
				ꦮꦭꦶ {design, _ref.sparse_attr()};
			ꦭꦶꦪ
				ꦮꦭꦶ {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() const { ꦮꦭꦶ _ref.function().fn(); }
		Sort sort() const { ꦮꦭꦶ _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		puur_naa_nka width() const { ꦮꦭꦶ sort().width(); }
		size_t arg_count() const { ꦮꦭꦶ _ref.size(); }
		Node arg(puur_naa_nka n) const { ꦮꦭꦶ Node(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) const
		{
			// currently templated but could be switched to AbstractVisitor &
			switch(_ref.function().fn()) {
			pak_sa Fn::invalid: log_error("invalid node in visit\n"); bha_nga;
			pak_sa Fn::buf: ꦮꦭꦶ v.buf(*this, arg(0)); bha_nga;
			pak_sa Fn::slice: ꦮꦭꦶ v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); bha_nga;
			pak_sa Fn::zero_extend: ꦮꦭꦶ v.zero_extend(*this, arg(0), width()); bha_nga;
			pak_sa Fn::sign_extend: ꦮꦭꦶ v.sign_extend(*this, arg(0), width()); bha_nga;
			pak_sa Fn::concat: ꦮꦭꦶ v.concat(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::add: ꦮꦭꦶ v.add(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::sub: ꦮꦭꦶ v.sub(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::mul: ꦮꦭꦶ v.mul(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::unsigned_div: ꦮꦭꦶ v.unsigned_div(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::unsigned_mod: ꦮꦭꦶ v.unsigned_mod(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::bitwise_and: ꦮꦭꦶ v.bitwise_and(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::bitwise_or: ꦮꦭꦶ v.bitwise_or(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::bitwise_xor: ꦮꦭꦶ v.bitwise_xor(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::bitwise_not: ꦮꦭꦶ v.bitwise_not(*this, arg(0)); bha_nga;
			pak_sa Fn::unary_minus: ꦮꦭꦶ v.unary_minus(*this, arg(0)); bha_nga;
			pak_sa Fn::reduce_and: ꦮꦭꦶ v.reduce_and(*this, arg(0)); bha_nga;
			pak_sa Fn::reduce_or: ꦮꦭꦶ v.reduce_or(*this, arg(0)); bha_nga;
			pak_sa Fn::reduce_xor: ꦮꦭꦶ v.reduce_xor(*this, arg(0)); bha_nga;
			pak_sa Fn::equal: ꦮꦭꦶ v.equal(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::not_equal: ꦮꦭꦶ v.not_equal(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::signed_greater_than: ꦮꦭꦶ v.signed_greater_than(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::signed_greater_equal: ꦮꦭꦶ v.signed_greater_equal(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::unsigned_greater_than: ꦮꦭꦶ v.unsigned_greater_than(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::unsigned_greater_equal: ꦮꦭꦶ v.unsigned_greater_equal(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::logical_shift_left: ꦮꦭꦶ v.logical_shift_left(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::logical_shift_right: ꦮꦭꦶ v.logical_shift_right(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::arithmetic_shift_right: ꦮꦭꦶ v.arithmetic_shift_right(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::mux: ꦮꦭꦶ v.mux(*this, arg(0), arg(1), arg(2)); bha_nga;
			pak_sa Fn::constant: ꦮꦭꦶ v.constant(*this, _ref.function().as_const()); bha_nga;
			pak_sa Fn::input: ꦮꦭꦶ v.input(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); bha_nga;
			pak_sa Fn::state: ꦮꦭꦶ v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); bha_nga;
			pak_sa Fn::memory_read: ꦮꦭꦶ v.memory_read(*this, arg(0), arg(1)); bha_nga;
			pak_sa Fn::memory_write: ꦮꦭꦶ v.memory_write(*this, arg(0), arg(1), arg(2)); bha_nga;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(Node)>);
	};
	anta_hstha IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	anta_hstha IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	anta_hstha IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	anta_hstha IR::Graph::Ref IR::mutate(Node n) { ꦮꦭꦶ _graph[n._ref.index()]; }
	anta_hstha Node IR::operator[](puur_naa_nka i) { ꦮꦭꦶ Node(_graph[i], design); }
	anta_hstha Node IROutput::value() const { ꦮꦭꦶ Node(_ir._graph({name, kind, false}), _ir.design); }
	anta_hstha bool IROutput::has_value() const { ꦮꦭꦶ _ir._graph.has_key({name, kind, false}); }
	anta_hstha void IROutput::set_value(Node value) { log_assert(sort == value.sort()); _ir.mutate(value).assign_key({name, kind, false}); }
	anta_hstha Node IRState::next_value() const { ꦮꦭꦶ Node(_ir._graph({name, kind, true}), _ir.design); }
	anta_hstha bool IRState::has_next_value() const { ꦮꦭꦶ _ir._graph.has_key({name, kind, true}); }
	anta_hstha void IRState::set_next_value(Node value) { log_assert(sort == value.sort()); _ir.mutate(value).assign_key({name, kind, true}); }
	anta_hstha Node IR::iterator::operator*() { ꦮꦭꦶ Node(_ir->_graph[_index], _ir->design); }
	anta_hstha arrow_proxy<Node> IR::iterator::operator->() { ꦮꦭꦶ arrow_proxy<Node>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> sa_nracanaa AbstractVisitor {
		virtual T buf(Node self, Node n) = 0;
		virtual T slice(Node self, Node a, puur_naa_nka offset, puur_naa_nka out_width) = 0;
		virtual T zero_extend(Node self, Node a, puur_naa_nka out_width) = 0;
		virtual T sign_extend(Node self, Node a, puur_naa_nka out_width) = 0;
		virtual T concat(Node self, Node a, Node b) = 0;
		virtual T add(Node self, Node a, Node b) = 0;
		virtual T sub(Node self, Node a, Node b) = 0;
		virtual T mul(Node self, Node a, Node b) = 0;
		virtual T unsigned_div(Node self, Node a, Node b) = 0;
		virtual T unsigned_mod(Node self, Node a, Node b) = 0;
		virtual T bitwise_and(Node self, Node a, Node b) = 0;
		virtual T bitwise_or(Node self, Node a, Node b) = 0;
		virtual T bitwise_xor(Node self, Node a, Node b) = 0;
		virtual T bitwise_not(Node self, Node a) = 0;
		virtual T unary_minus(Node self, Node a) = 0;
		virtual T reduce_and(Node self, Node a) = 0;
		virtual T reduce_or(Node self, Node a) = 0;
		virtual T reduce_xor(Node self, Node a) = 0;
		virtual T equal(Node self, Node a, Node b) = 0;
		virtual T not_equal(Node self, Node a, Node b) = 0;
		virtual T signed_greater_than(Node self, Node a, Node b) = 0;
		virtual T signed_greater_equal(Node self, Node a, Node b) = 0;
		virtual T unsigned_greater_than(Node self, Node a, Node b) = 0;
		virtual T unsigned_greater_equal(Node self, Node a, Node b) = 0;
		virtual T logical_shift_left(Node self, Node a, Node b) = 0;
		virtual T logical_shift_right(Node self, Node a, Node b) = 0;
		virtual T arithmetic_shift_right(Node self, Node a, Node b) = 0;
		virtual T mux(Node self, Node a, Node b, Node s) = 0;
		virtual T constant(Node self, RTLIL::Const const & value) = 0;
		virtual T input(Node self, IdString name, IdString kind) = 0;
		virtual T state(Node self, IdString name, IdString kind) = 0;
		virtual T memory_read(Node self, Node mem, Node addr) = 0;
		virtual T memory_write(Node self, Node mem, Node addr, Node data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> sa_nracanaa DefaultVisitor : public AbstractVisitor<T> {
		virtual T default_handler(Node self) = 0;
		T buf(Node self, Node) override { ꦮꦭꦶ default_handler(self); }
		T slice(Node self, Node, puur_naa_nka, puur_naa_nka) override { ꦮꦭꦶ default_handler(self); }
		T zero_extend(Node self, Node, puur_naa_nka) override { ꦮꦭꦶ default_handler(self); }
		T sign_extend(Node self, Node, puur_naa_nka) override { ꦮꦭꦶ default_handler(self); }
		T concat(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T add(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T sub(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T mul(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T unsigned_div(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T unsigned_mod(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T bitwise_and(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T bitwise_or(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T bitwise_xor(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T bitwise_not(Node self, Node) override { ꦮꦭꦶ default_handler(self); }
		T unary_minus(Node self, Node) override { ꦮꦭꦶ default_handler(self); }
		T reduce_and(Node self, Node) override { ꦮꦭꦶ default_handler(self); }
		T reduce_or(Node self, Node) override { ꦮꦭꦶ default_handler(self); }
		T reduce_xor(Node self, Node) override { ꦮꦭꦶ default_handler(self); }
		T equal(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T not_equal(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T signed_greater_than(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T signed_greater_equal(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T unsigned_greater_than(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T unsigned_greater_equal(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T logical_shift_left(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T logical_shift_right(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T arithmetic_shift_right(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T mux(Node self, Node, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T constant(Node self, RTLIL::Const const &) override { ꦮꦭꦶ default_handler(self); }
		T input(Node self, IdString, IdString) override { ꦮꦭꦶ default_handler(self); }
		T state(Node self, IdString, IdString) override { ꦮꦭꦶ default_handler(self); }
		T memory_read(Node self, Node, Node) override { ꦮꦭꦶ default_handler(self); }
		T memory_write(Node self, Node, Node, Node) override { ꦮꦭꦶ default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		Node add(IR::NodeData &&fn, Sort const &sort, std::initializer_list<Node> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			ꦏꦁꦒꦺꦴ (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			ꦮꦭꦶ Node(ref, _ir.design);
		}
		void check_basic_binary(Node const &a, Node const &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		void check_shift(Node const &a, Node const &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		void check_unary(Node const &a) { log_assert(a.sort().is_signal()); }
	public:
		IR &ir() { ꦮꦭꦶ _ir; }
		Node slice(Node a, puur_naa_nka offset, puur_naa_nka out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			ꦪꦼꦤ꧀(offset == 0 && out_width == a.width())
				ꦮꦭꦶ a;
			ꦮꦭꦶ add(IR::NodeData(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		Node extend(Node a, puur_naa_nka out_width, bool is_signed) {
			puur_naa_nka in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			ꦪꦼꦤ꧀(in_width == out_width)
				ꦮꦭꦶ a;
			ꦪꦼꦤ꧀(in_width > out_width)
				ꦮꦭꦶ slice(a, 0, out_width);
			ꦪꦼꦤ꧀(is_signed)
				ꦮꦭꦶ add(Fn::sign_extend, Sort(out_width), {a});
			ꦭꦶꦪ
				ꦮꦭꦶ add(Fn::zero_extend, Sort(out_width), {a});
		}
		Node concat(Node a, Node b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			ꦮꦭꦶ add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		Node add(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::add, a.sort(), {a, b}); }
		Node sub(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::sub, a.sort(), {a, b}); }
		Node mul(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::mul, a.sort(), {a, b}); }
		Node unsigned_div(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::unsigned_div, a.sort(), {a, b}); }
		Node unsigned_mod(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::unsigned_mod, a.sort(), {a, b}); }
		Node bitwise_and(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::bitwise_and, a.sort(), {a, b}); }
		Node bitwise_or(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::bitwise_or, a.sort(), {a, b}); }
		Node bitwise_xor(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::bitwise_xor, a.sort(), {a, b}); }
		Node bitwise_not(Node a) { check_unary(a); ꦮꦭꦶ add(Fn::bitwise_not, a.sort(), {a}); }
		Node unary_minus(Node a) { check_unary(a); ꦮꦭꦶ add(Fn::unary_minus, a.sort(), {a}); }
		Node reduce_and(Node a) {
			check_unary(a);
			ꦪꦼꦤ꧀(a.width() == 1)
				ꦮꦭꦶ a;
			ꦮꦭꦶ add(Fn::reduce_and, Sort(1), {a});
		}
		Node reduce_or(Node a) {
			check_unary(a);
			ꦪꦼꦤ꧀(a.width() == 1)
				ꦮꦭꦶ a;
			ꦮꦭꦶ add(Fn::reduce_or, Sort(1), {a});
		}
		Node reduce_xor(Node a) {
			check_unary(a);
			ꦪꦼꦤ꧀(a.width() == 1)
				ꦮꦭꦶ a;
			ꦮꦭꦶ add(Fn::reduce_xor, Sort(1), {a});
		}
		Node equal(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::equal, Sort(1), {a, b}); }
		Node not_equal(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::not_equal, Sort(1), {a, b}); }
		Node signed_greater_than(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::signed_greater_than, Sort(1), {a, b}); }
		Node signed_greater_equal(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		Node unsigned_greater_than(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		Node unsigned_greater_equal(Node a, Node b) { check_basic_binary(a, b); ꦮꦭꦶ add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		Node logical_shift_left(Node a, Node b) { check_shift(a, b); ꦮꦭꦶ add(Fn::logical_shift_left, a.sort(), {a, b}); }
		Node logical_shift_right(Node a, Node b) { check_shift(a, b); ꦮꦭꦶ add(Fn::logical_shift_right, a.sort(), {a, b}); }
		Node arithmetic_shift_right(Node a, Node b) { check_shift(a, b); ꦮꦭꦶ add(Fn::arithmetic_shift_right, a.sort(), {a, b}); }
		Node mux(Node a, Node b, Node s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			ꦮꦭꦶ add(Fn::mux, a.sort(), {a, b, s});
		}
		Node memory_read(Node mem, Node addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			ꦮꦭꦶ add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		Node memory_write(Node mem, Node addr, Node data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			ꦮꦭꦶ add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		Node constant(RTLIL::Const value) {
			puur_naa_nka s = value.size();
			ꦮꦭꦶ add(IR::NodeData(Fn::constant, std::move(value)), Sort(s), {});
		}
		Node create_pending(puur_naa_nka width) {
			ꦮꦭꦶ add(Fn::buf, Sort(width), {});
		}
		void update_pending(Node node, Node value) {
			log_assert(node._ref.function() == Fn::buf && node._ref.size() == 0);
			log_assert(node.sort() == value.sort());
			_ir.mutate(node).append_arg(value._ref);
		}
		IRInput &add_input(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			ꦪꦼꦤ꧀ (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			ꦮꦭꦶ it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			ꦪꦼꦤ꧀ (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			ꦮꦭꦶ it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			ꦪꦼꦤ꧀ (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			ꦮꦭꦶ it->second;
		}
		Node value(IRInput const& input) {
			ꦮꦭꦶ add(IR::NodeData(Fn::input, std::pair(input.name, input.kind)), input.sort, {});
		}
		Node value(IRState const& state) {
			ꦮꦭꦶ add(IR::NodeData(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		void suggest_name(Node node, IdString name) {
			_ir.mutate(node).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	anta_hstha Factory IR::factory() { ꦮꦭꦶ Factory(*this); }
	template<class Id> class Scope {
	rak_sita:
		var_na substitution_character = '_';
		virtual bool is_character_legal(var_na, puur_naa_nka) = 0;
	private:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	public:
		void reserve(std::string name) {
			_used_names.insert(std::move(name));
		}
		template<typename N> std::string unique_name(const N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			ꦏꦁꦒꦺꦴ(size_t i = 0; i < str.size(); i++)
				ꦪꦼꦤ꧀(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			ꦪꦼꦤ꧀(_used_names.count(str) == 0) {
				_used_names.insert(str);
				ꦮꦭꦶ str;
			}
			ꦏꦁꦒꦺꦴ (puur_naa_nka idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				ꦪꦼꦤ꧀(_used_names.count(suffixed) == 0) {
					_used_names.insert(suffixed);
					ꦮꦭꦶ suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, const N &suggestion) {
			auto it = _by_id.find(id);
			ꦪꦼꦤ꧀(it != _by_id.end())
				ꦮꦭꦶ it->second;
			std::string str = unique_name(suggestion);
			_by_id.insert({id, str});
			ꦮꦭꦶ str;
		}
	};
	class Writer {
		std::ostream *os;
		void print_impl(const var_na *fmt, vector<std::function<void()>>& fns);
	public:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); ꦮꦭꦶ *this; }
		template<typename... Args>
		void print(const var_na *fmt, Args&&... args)
		{
			vector<std::function<void()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		void print_with(Fn fn, const var_na *fmt, Args&&... args)
		{
			vector<std::function<void()>> fns { [&]() {
				ꦪꦼꦤ꧀ constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				ꦭꦶꦪ
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
