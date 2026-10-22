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
	enum class Fn {
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
	const char *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<정수, std::pair<정수, 정수>> _v;
	public:
		explicit Sort(정수 width) : _v(width) { }
		Sort(정수 addr_width, 정수 data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() const { 반환 _v.index() == 0; }
		bool is_memory() const { 반환 _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		정수 width() const { 반환 std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		정수 addr_width() const { 반환 std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		정수 data_width() const { 반환 std::get<1>(_v).second; }
		bool operator==(Sort const& other) const { 반환 _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) const { h.eat(_v); 반환 h; }
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
		RTLIL::Const const& initial_value_signal() const { 반환 std::get<RTLIL::Const>(_initial); }
		MemContents const& initial_value_memory() const { 반환 std::get<MemContents>(_initial); }
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
				정수
			> _extra;
		public:
			NodeData() : _fn(Fn::invalid) {}
			NodeData(Fn fn) : _fn(fn) {}
			template<class T> NodeData(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() const { 반환 _fn; }
			const RTLIL::Const &as_const() const { 반환 std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() const { 반환 std::get<std::pair<IdString, IdString>>(_extra); }
			정수 as_int() const { 반환 std::get<정수>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) const {
				h.eat((unsigned 정수) _fn);
				h.eat(_extra);
				반환 h;
			}
			bool operator==(NodeData const &other) const {
				반환 _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		struct Attr {
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
		정수 size() const { 반환 _graph.size(); }
		Node operator[](정수 i);
		void topological_sort();
		void forward_buf();
		IRInput const& input(IdString name, IdString kind) const { 반환 _inputs.at({name, kind}); }
		IRInput const& input(IdString name) const { 반환 input(name, ID($input)); }
		IROutput const& output(IdString name, IdString kind) const { 반환 _outputs.at({name, kind}); }
		IROutput const& output(IdString name) const { 반환 output(name, ID($output)); }
		IRState const& state(IdString name, IdString kind) const { 반환 _states.at({name, kind}); }
		IRState const& state(IdString name) const { 반환 state(name, ID($state)); }
		bool has_input(IdString name, IdString kind) const { 반환 _inputs.count({name, kind}); }
		bool has_output(IdString name, IdString kind) const { 반환 _outputs.count({name, kind}); }
		bool has_state(IdString name, IdString kind) const { 반환 _states.count({name, kind}); }
		vector<IRInput const*> inputs(IdString kind) const;
		vector<IRInput const*> inputs() const { 반환 inputs(ID($input)); }
		vector<IROutput const*> outputs(IdString kind) const;
		vector<IROutput const*> outputs() const { 반환 outputs(ID($output)); }
		vector<IRState const*> states(IdString kind) const;
		vector<IRState const*> states() const { 반환 states(ID($state)); }
		vector<IRInput const*> all_inputs() const;
		vector<IROutput const*> all_outputs() const;
		vector<IRState const*> all_states() const;
		class iterator {
			friend class IR;
			IR *_ir;
			정수 _index;
			iterator(IR *ir, 정수 index) : _ir(ir), _index(index) {}
		public:
			using iterator_category = std::input_iterator_tag;
			using value_type = Node;
			using pointer = arrow_proxy<Node>;
			using reference = Node;
			using difference_type = ptrdiff_t;
			Node operator*();
			iterator &operator++() { _index++; 반환 *this; }
			bool operator!=(iterator const &other) const { 반환 _ir != other._ir || _index != other._index; }
			bool operator==(iterator const &other) const { 반환 !(*this != other); }
			pointer operator->();
		};
		iterator begin() { 반환 iterator(this, 0); }
		iterator end() { 반환 iterator(this, _graph.size()); }
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
		explicit operator IR::Graph::ConstRef() { 반환 _ref; }
	public:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		정수 id() const { 반환 _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() const {
			만약(_ref.has_sparse_attr())
				반환 {design, _ref.sparse_attr()};
			아니면
				반환 {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() const { 반환 _ref.function().fn(); }
		Sort sort() const { 반환 _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		정수 width() const { 반환 sort().width(); }
		size_t arg_count() const { 반환 _ref.size(); }
		Node arg(정수 n) const { 반환 Node(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) const
		{
			// currently templated but could be switched to AbstractVisitor &
			switch(_ref.function().fn()) {
			case Fn::invalid: log_error("invalid node in visit\n"); break;
			case Fn::buf: 반환 v.buf(*this, arg(0)); break;
			case Fn::slice: 반환 v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); break;
			case Fn::zero_extend: 반환 v.zero_extend(*this, arg(0), width()); break;
			case Fn::sign_extend: 반환 v.sign_extend(*this, arg(0), width()); break;
			case Fn::concat: 반환 v.concat(*this, arg(0), arg(1)); break;
			case Fn::add: 반환 v.add(*this, arg(0), arg(1)); break;
			case Fn::sub: 반환 v.sub(*this, arg(0), arg(1)); break;
			case Fn::mul: 반환 v.mul(*this, arg(0), arg(1)); break;
			case Fn::unsigned_div: 반환 v.unsigned_div(*this, arg(0), arg(1)); break;
			case Fn::unsigned_mod: 반환 v.unsigned_mod(*this, arg(0), arg(1)); break;
			case Fn::bitwise_and: 반환 v.bitwise_and(*this, arg(0), arg(1)); break;
			case Fn::bitwise_or: 반환 v.bitwise_or(*this, arg(0), arg(1)); break;
			case Fn::bitwise_xor: 반환 v.bitwise_xor(*this, arg(0), arg(1)); break;
			case Fn::bitwise_not: 반환 v.bitwise_not(*this, arg(0)); break;
			case Fn::unary_minus: 반환 v.unary_minus(*this, arg(0)); break;
			case Fn::reduce_and: 반환 v.reduce_and(*this, arg(0)); break;
			case Fn::reduce_or: 반환 v.reduce_or(*this, arg(0)); break;
			case Fn::reduce_xor: 반환 v.reduce_xor(*this, arg(0)); break;
			case Fn::equal: 반환 v.equal(*this, arg(0), arg(1)); break;
			case Fn::not_equal: 반환 v.not_equal(*this, arg(0), arg(1)); break;
			case Fn::signed_greater_than: 반환 v.signed_greater_than(*this, arg(0), arg(1)); break;
			case Fn::signed_greater_equal: 반환 v.signed_greater_equal(*this, arg(0), arg(1)); break;
			case Fn::unsigned_greater_than: 반환 v.unsigned_greater_than(*this, arg(0), arg(1)); break;
			case Fn::unsigned_greater_equal: 반환 v.unsigned_greater_equal(*this, arg(0), arg(1)); break;
			case Fn::logical_shift_left: 반환 v.logical_shift_left(*this, arg(0), arg(1)); break;
			case Fn::logical_shift_right: 반환 v.logical_shift_right(*this, arg(0), arg(1)); break;
			case Fn::arithmetic_shift_right: 반환 v.arithmetic_shift_right(*this, arg(0), arg(1)); break;
			case Fn::mux: 반환 v.mux(*this, arg(0), arg(1), arg(2)); break;
			case Fn::constant: 반환 v.constant(*this, _ref.function().as_const()); break;
			case Fn::input: 반환 v.input(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); break;
			case Fn::state: 반환 v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); break;
			case Fn::memory_read: 반환 v.memory_read(*this, arg(0), arg(1)); break;
			case Fn::memory_write: 반환 v.memory_write(*this, arg(0), arg(1), arg(2)); break;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(Node)>);
	};
	inline IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	inline IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	inline IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	inline IR::Graph::Ref IR::mutate(Node n) { 반환 _graph[n._ref.index()]; }
	inline Node IR::operator[](정수 i) { 반환 Node(_graph[i], design); }
	inline Node IROutput::value() const { 반환 Node(_ir._graph({name, kind, false}), _ir.design); }
	inline bool IROutput::has_value() const { 반환 _ir._graph.has_key({name, kind, false}); }
	inline void IROutput::set_value(Node value) { log_assert(sort == value.sort()); _ir.mutate(value).assign_key({name, kind, false}); }
	inline Node IRState::next_value() const { 반환 Node(_ir._graph({name, kind, true}), _ir.design); }
	inline bool IRState::has_next_value() const { 반환 _ir._graph.has_key({name, kind, true}); }
	inline void IRState::set_next_value(Node value) { log_assert(sort == value.sort()); _ir.mutate(value).assign_key({name, kind, true}); }
	inline Node IR::iterator::operator*() { 반환 Node(_ir->_graph[_index], _ir->design); }
	inline arrow_proxy<Node> IR::iterator::operator->() { 반환 arrow_proxy<Node>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> struct AbstractVisitor {
		virtual T buf(Node self, Node n) = 0;
		virtual T slice(Node self, Node a, 정수 offset, 정수 out_width) = 0;
		virtual T zero_extend(Node self, Node a, 정수 out_width) = 0;
		virtual T sign_extend(Node self, Node a, 정수 out_width) = 0;
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
	template<class T> struct DefaultVisitor : public AbstractVisitor<T> {
		virtual T default_handler(Node self) = 0;
		T buf(Node self, Node) override { 반환 default_handler(self); }
		T slice(Node self, Node, 정수, 정수) override { 반환 default_handler(self); }
		T zero_extend(Node self, Node, 정수) override { 반환 default_handler(self); }
		T sign_extend(Node self, Node, 정수) override { 반환 default_handler(self); }
		T concat(Node self, Node, Node) override { 반환 default_handler(self); }
		T add(Node self, Node, Node) override { 반환 default_handler(self); }
		T sub(Node self, Node, Node) override { 반환 default_handler(self); }
		T mul(Node self, Node, Node) override { 반환 default_handler(self); }
		T unsigned_div(Node self, Node, Node) override { 반환 default_handler(self); }
		T unsigned_mod(Node self, Node, Node) override { 반환 default_handler(self); }
		T bitwise_and(Node self, Node, Node) override { 반환 default_handler(self); }
		T bitwise_or(Node self, Node, Node) override { 반환 default_handler(self); }
		T bitwise_xor(Node self, Node, Node) override { 반환 default_handler(self); }
		T bitwise_not(Node self, Node) override { 반환 default_handler(self); }
		T unary_minus(Node self, Node) override { 반환 default_handler(self); }
		T reduce_and(Node self, Node) override { 반환 default_handler(self); }
		T reduce_or(Node self, Node) override { 반환 default_handler(self); }
		T reduce_xor(Node self, Node) override { 반환 default_handler(self); }
		T equal(Node self, Node, Node) override { 반환 default_handler(self); }
		T not_equal(Node self, Node, Node) override { 반환 default_handler(self); }
		T signed_greater_than(Node self, Node, Node) override { 반환 default_handler(self); }
		T signed_greater_equal(Node self, Node, Node) override { 반환 default_handler(self); }
		T unsigned_greater_than(Node self, Node, Node) override { 반환 default_handler(self); }
		T unsigned_greater_equal(Node self, Node, Node) override { 반환 default_handler(self); }
		T logical_shift_left(Node self, Node, Node) override { 반환 default_handler(self); }
		T logical_shift_right(Node self, Node, Node) override { 반환 default_handler(self); }
		T arithmetic_shift_right(Node self, Node, Node) override { 반환 default_handler(self); }
		T mux(Node self, Node, Node, Node) override { 반환 default_handler(self); }
		T constant(Node self, RTLIL::Const const &) override { 반환 default_handler(self); }
		T input(Node self, IdString, IdString) override { 반환 default_handler(self); }
		T state(Node self, IdString, IdString) override { 반환 default_handler(self); }
		T memory_read(Node self, Node, Node) override { 반환 default_handler(self); }
		T memory_write(Node self, Node, Node, Node) override { 반환 default_handler(self); }
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
			위해 (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			반환 Node(ref, _ir.design);
		}
		void check_basic_binary(Node const &a, Node const &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		void check_shift(Node const &a, Node const &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		void check_unary(Node const &a) { log_assert(a.sort().is_signal()); }
	public:
		IR &ir() { 반환 _ir; }
		Node slice(Node a, 정수 offset, 정수 out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			만약(offset == 0 && out_width == a.width())
				반환 a;
			반환 add(IR::NodeData(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		Node extend(Node a, 정수 out_width, bool is_signed) {
			정수 in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			만약(in_width == out_width)
				반환 a;
			만약(in_width > out_width)
				반환 slice(a, 0, out_width);
			만약(is_signed)
				반환 add(Fn::sign_extend, Sort(out_width), {a});
			아니면
				반환 add(Fn::zero_extend, Sort(out_width), {a});
		}
		Node concat(Node a, Node b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			반환 add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		Node add(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::add, a.sort(), {a, b}); }
		Node sub(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::sub, a.sort(), {a, b}); }
		Node mul(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::mul, a.sort(), {a, b}); }
		Node unsigned_div(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::unsigned_div, a.sort(), {a, b}); }
		Node unsigned_mod(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::unsigned_mod, a.sort(), {a, b}); }
		Node bitwise_and(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::bitwise_and, a.sort(), {a, b}); }
		Node bitwise_or(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::bitwise_or, a.sort(), {a, b}); }
		Node bitwise_xor(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::bitwise_xor, a.sort(), {a, b}); }
		Node bitwise_not(Node a) { check_unary(a); 반환 add(Fn::bitwise_not, a.sort(), {a}); }
		Node unary_minus(Node a) { check_unary(a); 반환 add(Fn::unary_minus, a.sort(), {a}); }
		Node reduce_and(Node a) {
			check_unary(a);
			만약(a.width() == 1)
				반환 a;
			반환 add(Fn::reduce_and, Sort(1), {a});
		}
		Node reduce_or(Node a) {
			check_unary(a);
			만약(a.width() == 1)
				반환 a;
			반환 add(Fn::reduce_or, Sort(1), {a});
		}
		Node reduce_xor(Node a) {
			check_unary(a);
			만약(a.width() == 1)
				반환 a;
			반환 add(Fn::reduce_xor, Sort(1), {a});
		}
		Node equal(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::equal, Sort(1), {a, b}); }
		Node not_equal(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::not_equal, Sort(1), {a, b}); }
		Node signed_greater_than(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::signed_greater_than, Sort(1), {a, b}); }
		Node signed_greater_equal(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		Node unsigned_greater_than(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		Node unsigned_greater_equal(Node a, Node b) { check_basic_binary(a, b); 반환 add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		Node logical_shift_left(Node a, Node b) { check_shift(a, b); 반환 add(Fn::logical_shift_left, a.sort(), {a, b}); }
		Node logical_shift_right(Node a, Node b) { check_shift(a, b); 반환 add(Fn::logical_shift_right, a.sort(), {a, b}); }
		Node arithmetic_shift_right(Node a, Node b) { check_shift(a, b); 반환 add(Fn::arithmetic_shift_right, a.sort(), {a, b}); }
		Node mux(Node a, Node b, Node s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			반환 add(Fn::mux, a.sort(), {a, b, s});
		}
		Node memory_read(Node mem, Node addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			반환 add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		Node memory_write(Node mem, Node addr, Node data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			반환 add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		Node constant(RTLIL::Const value) {
			정수 s = value.size();
			반환 add(IR::NodeData(Fn::constant, std::move(value)), Sort(s), {});
		}
		Node create_pending(정수 width) {
			반환 add(Fn::buf, Sort(width), {});
		}
		void update_pending(Node node, Node value) {
			log_assert(node._ref.function() == Fn::buf && node._ref.size() == 0);
			log_assert(node.sort() == value.sort());
			_ir.mutate(node).append_arg(value._ref);
		}
		IRInput &add_input(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			만약 (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			반환 it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			만약 (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			반환 it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			만약 (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			반환 it->second;
		}
		Node value(IRInput const& input) {
			반환 add(IR::NodeData(Fn::input, std::pair(input.name, input.kind)), input.sort, {});
		}
		Node value(IRState const& state) {
			반환 add(IR::NodeData(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		void suggest_name(Node node, IdString name) {
			_ir.mutate(node).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	inline Factory IR::factory() { 반환 Factory(*this); }
	template<class Id> class Scope {
	protected:
		char substitution_character = '_';
		virtual bool is_character_legal(char, 정수) = 0;
	private:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	public:
		void reserve(std::string name) {
			_used_names.insert(std::move(name));
		}
		template<typename N> std::string unique_name(const N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			위해(size_t i = 0; i < str.size(); i++)
				만약(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			만약(_used_names.count(str) == 0) {
				_used_names.insert(str);
				반환 str;
			}
			위해 (정수 idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				만약(_used_names.count(suffixed) == 0) {
					_used_names.insert(suffixed);
					반환 suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, const N &suggestion) {
			auto it = _by_id.find(id);
			만약(it != _by_id.end())
				반환 it->second;
			std::string str = unique_name(suggestion);
			_by_id.insert({id, str});
			반환 str;
		}
	};
	class Writer {
		std::ostream *os;
		void print_impl(const char *fmt, vector<std::function<void()>>& fns);
	public:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); 반환 *this; }
		template<typename... Args>
		void print(const char *fmt, Args&&... args)
		{
			vector<std::function<void()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		void print_with(Fn fn, const char *fmt, Args&&... args)
		{
			vector<std::function<void()>> fns { [&]() {
				만약 constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				아니면
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
