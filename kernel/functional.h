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

#அந்தர்பாவ "kernel/yosys.h"
#அந்தர்பாவ "kernel/compute_graph.h"
#அந்தர்பாவ "kernel/drivertools.h"
#அந்தர்பாவ "kernel/mem.h"
#அந்தர்பாவ "kernel/utils.h"

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
	எண்ணிக்கை class Fn {
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
		logical_shift_இடது,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_வலது,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_வலது,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		உள்ளீடு,
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
	மாறிலி எழுத்து *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<முழு, std::pair<முழு, முழு>> _v;
	பொது:
		explicit Sort(முழு width) : _v(width) { }
		Sort(முழு addr_width, முழு data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() மாறிலி { திரும்பு _v.index() == 0; }
		bool is_memory() மாறிலி { திரும்பு _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		முழு width() மாறிலி { திரும்பு std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		முழு addr_width() மாறிலி { திரும்பு std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		முழு data_width() மாறிலி { திரும்பு std::get<1>(_v).second; }
		bool operator==(Sort மாறிலி& other) மாறிலி { திரும்பு _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி { h.eat(_v); திரும்பு h; }
	};
	class IR;
	class Factory;
	class முனை_2;
	class IRInput {
		friend class Factory;
	பொது:
		PooledName name;
		PooledName kind;
		Sort sort;
	தனிப்பட்ட:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	பொது:
		PooledName name;
		PooledName kind;
		Sort sort;
	தனிப்பட்ட:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	பொது:
		முனை_2 மதிப்பு() மாறிலி;
		bool has_மதிப்பு() மாறிலி;
		வெறுமை set_மதிப்பு(முனை_2 மதிப்பு);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	பொது:
		PooledName name;
		PooledName kind;
		Sort sort;
	தனிப்பட்ட:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	பொது:
		முனை_2 next_மதிப்பு() மாறிலி;
		bool has_next_மதிப்பு() மாறிலி;
		RTLIL::Const மாறிலி& initial_மதிப்பு_signal() மாறிலி { திரும்பு std::get<RTLIL::Const>(_initial); }
		MemContents மாறிலி& initial_மதிப்பு_memory() மாறிலி { திரும்பு std::get<MemContents>(_initial); }
		வெறுமை set_next_மதிப்பு(முனை_2 மதிப்பு);
		வெறுமை set_initial_மதிப்பு(RTLIL::Const மதிப்பு) { மதிப்பு.extu(sort.width()); _initial = std::move(மதிப்பு); }
		வெறுமை set_initial_மதிப்பு(MemContents மதிப்பு) { log_assert(Sort(மதிப்பு.addr_width(), மதிப்பு.data_width()) == sort); _initial = std::move(மதிப்பு); }
	};
	class IR {
		friend class Factory;
		friend class முனை_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class முனை_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				முழு
			> _extra;
		பொது:
			முனை_Data() : _fn(Fn::invalid) {}
			முனை_Data(Fn fn) : _fn(fn) {}
			template<class T> முனை_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() மாறிலி { திரும்பு _fn; }
			மாறிலி RTLIL::Const &as_const() மாறிலி { திரும்பு std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() மாறிலி { திரும்பு std::get<std::pair<IdString, IdString>>(_extra); }
			முழு as_int() மாறிலி { திரும்பு std::get<முழு>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) மாறிலி {
				h.eat((unsigned முழு) _fn);
				h.eat(_extra);
				திரும்பு h;
			}
			bool operator==(முனை_Data மாறிலி &other) மாறிலி {
				திரும்பு _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		கட்டமைப்பு Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<முனை_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(முனை_2 n);
	பொது:
		Design *design = nullptr;
		நிலையான IR from_module(Module *module);
		Factory factory();
		முழு size() மாறிலி { திரும்பு _graph.size(); }
		முனை_2 operator[](முழு i);
		வெறுமை topological_sort();
		வெறுமை forward_buf();
		IRInput மாறிலி& உள்ளீடு(IdString name, IdString kind) மாறிலி { திரும்பு _inputs.at({name, kind}); }
		IRInput மாறிலி& உள்ளீடு(IdString name) மாறிலி { திரும்பு உள்ளீடு(name, ID($உள்ளீடு)); }
		IROutput மாறிலி& output(IdString name, IdString kind) மாறிலி { திரும்பு _outputs.at({name, kind}); }
		IROutput மாறிலி& output(IdString name) மாறிலி { திரும்பு output(name, ID($output)); }
		IRState மாறிலி& state(IdString name, IdString kind) மாறிலி { திரும்பு _states.at({name, kind}); }
		IRState மாறிலி& state(IdString name) மாறிலி { திரும்பு state(name, ID($state)); }
		bool has_உள்ளீடு(IdString name, IdString kind) மாறிலி { திரும்பு _inputs.மொத்தம்({name, kind}); }
		bool has_output(IdString name, IdString kind) மாறிலி { திரும்பு _outputs.மொத்தம்({name, kind}); }
		bool has_state(IdString name, IdString kind) மாறிலி { திரும்பு _states.மொத்தம்({name, kind}); }
		vector<IRInput மாறிலி*> inputs(IdString kind) மாறிலி;
		vector<IRInput மாறிலி*> inputs() மாறிலி { திரும்பு inputs(ID($உள்ளீடு)); }
		vector<IROutput மாறிலி*> outputs(IdString kind) மாறிலி;
		vector<IROutput மாறிலி*> outputs() மாறிலி { திரும்பு outputs(ID($output)); }
		vector<IRState மாறிலி*> states(IdString kind) மாறிலி;
		vector<IRState மாறிலி*> states() மாறிலி { திரும்பு states(ID($state)); }
		vector<IRInput மாறிலி*> all_inputs() மாறிலி;
		vector<IROutput மாறிலி*> all_outputs() மாறிலி;
		vector<IRState மாறிலி*> all_states() மாறிலி;
		class iterator {
			friend class IR;
			IR *_ir;
			முழு _index;
			iterator(IR *ir, முழு index) : _ir(ir), _index(index) {}
		பொது:
			using iterator_category = std::உள்ளீடு_iterator_tag;
			using மதிப்பு_type = முனை_2;
			using pointer = arrow_proxy<முனை_2>;
			using reference = முனை_2;
			using difference_type = ptrdiff_t;
			முனை_2 operator*();
			iterator &operator++() { _index++; திரும்பு *this; }
			bool operator!=(iterator மாறிலி &other) மாறிலி { திரும்பு _ir != other._ir || _index != other._index; }
			bool operator==(iterator மாறிலி &other) மாறிலி { திரும்பு !(*this != other); }
			pointer operator->();
		};
		iterator begin() { திரும்பு iterator(this, 0); }
		iterator end() { திரும்பு iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class முனை_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		முனை_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { திரும்பு _ref; }
	பொது:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		முழு id() மாறிலி { திரும்பு _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() மாறிலி {
			எனில்(_ref.has_sparse_attr())
				திரும்பு {design, _ref.sparse_attr()};
			இல்லையெனில்
				திரும்பு {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() மாறிலி { திரும்பு _ref.function().fn(); }
		Sort sort() மாறிலி { திரும்பு _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		முழு width() மாறிலி { திரும்பு sort().width(); }
		size_t arg_மொத்தம்() மாறிலி { திரும்பு _ref.size(); }
		முனை_2 arg(முழு n) மாறிலி { திரும்பு முனை_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) மாறிலி
		{
			// currently templated but could be switched to AbstractVisitor &
			தேர்வு(_ref.function().fn()) {
			நிலை Fn::invalid: log_error("invalid node in visit\n"); நிறுத்து;
			நிலை Fn::buf: திரும்பு v.buf(*this, arg(0)); நிறுத்து;
			நிலை Fn::slice: திரும்பு v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); நிறுத்து;
			நிலை Fn::zero_extend: திரும்பு v.zero_extend(*this, arg(0), width()); நிறுத்து;
			நிலை Fn::sign_extend: திரும்பு v.sign_extend(*this, arg(0), width()); நிறுத்து;
			நிலை Fn::concat: திரும்பு v.concat(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::add: திரும்பு v.add(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::sub: திரும்பு v.sub(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::mul: திரும்பு v.mul(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::unsigned_div: திரும்பு v.unsigned_div(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::unsigned_mod: திரும்பு v.unsigned_mod(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::bitwise_and: திரும்பு v.bitwise_and(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::bitwise_or: திரும்பு v.bitwise_or(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::bitwise_xor: திரும்பு v.bitwise_xor(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::bitwise_not: திரும்பு v.bitwise_not(*this, arg(0)); நிறுத்து;
			நிலை Fn::unary_minus: திரும்பு v.unary_minus(*this, arg(0)); நிறுத்து;
			நிலை Fn::reduce_and: திரும்பு v.reduce_and(*this, arg(0)); நிறுத்து;
			நிலை Fn::reduce_or: திரும்பு v.reduce_or(*this, arg(0)); நிறுத்து;
			நிலை Fn::reduce_xor: திரும்பு v.reduce_xor(*this, arg(0)); நிறுத்து;
			நிலை Fn::equal: திரும்பு v.equal(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::not_equal: திரும்பு v.not_equal(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::signed_greater_than: திரும்பு v.signed_greater_than(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::signed_greater_equal: திரும்பு v.signed_greater_equal(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::unsigned_greater_than: திரும்பு v.unsigned_greater_than(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::unsigned_greater_equal: திரும்பு v.unsigned_greater_equal(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::logical_shift_இடது: திரும்பு v.logical_shift_இடது(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::logical_shift_வலது: திரும்பு v.logical_shift_வலது(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::arithmetic_shift_வலது: திரும்பு v.arithmetic_shift_வலது(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::mux: திரும்பு v.mux(*this, arg(0), arg(1), arg(2)); நிறுத்து;
			நிலை Fn::constant: திரும்பு v.constant(*this, _ref.function().as_const()); நிறுத்து;
			நிலை Fn::உள்ளீடு: திரும்பு v.உள்ளீடு(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); நிறுத்து;
			நிலை Fn::state: திரும்பு v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); நிறுத்து;
			நிலை Fn::memory_read: திரும்பு v.memory_read(*this, arg(0), arg(1)); நிறுத்து;
			நிலை Fn::memory_write: திரும்பு v.memory_write(*this, arg(0), arg(1), arg(2)); நிறுத்து;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(முனை_2)>);
	};
	உள்வரி IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	உள்வரி IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	உள்வரி IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	உள்வரி IR::Graph::Ref IR::mutate(முனை_2 n) { திரும்பு _graph[n._ref.index()]; }
	உள்வரி முனை_2 IR::operator[](முழு i) { திரும்பு முனை_2(_graph[i], design); }
	உள்வரி முனை_2 IROutput::மதிப்பு() மாறிலி { திரும்பு முனை_2(_ir._graph({name, kind, false}), _ir.design); }
	உள்வரி bool IROutput::has_மதிப்பு() மாறிலி { திரும்பு _ir._graph.has_சாவி({name, kind, false}); }
	உள்வரி வெறுமை IROutput::set_மதிப்பு(முனை_2 மதிப்பு) { log_assert(sort == மதிப்பு.sort()); _ir.mutate(மதிப்பு).assign_சாவி({name, kind, false}); }
	உள்வரி முனை_2 IRState::next_மதிப்பு() மாறிலி { திரும்பு முனை_2(_ir._graph({name, kind, true}), _ir.design); }
	உள்வரி bool IRState::has_next_மதிப்பு() மாறிலி { திரும்பு _ir._graph.has_சாவி({name, kind, true}); }
	உள்வரி வெறுமை IRState::set_next_மதிப்பு(முனை_2 மதிப்பு) { log_assert(sort == மதிப்பு.sort()); _ir.mutate(மதிப்பு).assign_சாவி({name, kind, true}); }
	உள்வரி முனை_2 IR::iterator::operator*() { திரும்பு முனை_2(_ir->_graph[_index], _ir->design); }
	உள்வரி arrow_proxy<முனை_2> IR::iterator::operator->() { திரும்பு arrow_proxy<முனை_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> கட்டமைப்பு AbstractVisitor {
		மெய்நிகர் T buf(முனை_2 self, முனை_2 n) = 0;
		மெய்நிகர் T slice(முனை_2 self, முனை_2 a, முழு offset, முழு out_width) = 0;
		மெய்நிகர் T zero_extend(முனை_2 self, முனை_2 a, முழு out_width) = 0;
		மெய்நிகர் T sign_extend(முனை_2 self, முனை_2 a, முழு out_width) = 0;
		மெய்நிகர் T concat(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T add(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T sub(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T mul(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T unsigned_div(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T unsigned_mod(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T bitwise_and(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T bitwise_or(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T bitwise_xor(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T bitwise_not(முனை_2 self, முனை_2 a) = 0;
		மெய்நிகர் T unary_minus(முனை_2 self, முனை_2 a) = 0;
		மெய்நிகர் T reduce_and(முனை_2 self, முனை_2 a) = 0;
		மெய்நிகர் T reduce_or(முனை_2 self, முனை_2 a) = 0;
		மெய்நிகர் T reduce_xor(முனை_2 self, முனை_2 a) = 0;
		மெய்நிகர் T equal(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T not_equal(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T signed_greater_than(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T signed_greater_equal(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T unsigned_greater_than(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T unsigned_greater_equal(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T logical_shift_இடது(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T logical_shift_வலது(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T arithmetic_shift_வலது(முனை_2 self, முனை_2 a, முனை_2 b) = 0;
		மெய்நிகர் T mux(முனை_2 self, முனை_2 a, முனை_2 b, முனை_2 s) = 0;
		மெய்நிகர் T constant(முனை_2 self, RTLIL::Const மாறிலி & மதிப்பு) = 0;
		மெய்நிகர் T உள்ளீடு(முனை_2 self, IdString name, IdString kind) = 0;
		மெய்நிகர் T state(முனை_2 self, IdString name, IdString kind) = 0;
		மெய்நிகர் T memory_read(முனை_2 self, முனை_2 mem, முனை_2 addr) = 0;
		மெய்நிகர் T memory_write(முனை_2 self, முனை_2 mem, முனை_2 addr, முனை_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> கட்டமைப்பு DefaultVisitor : பொது AbstractVisitor<T> {
		மெய்நிகர் T default_handler(முனை_2 self) = 0;
		T buf(முனை_2 self, முனை_2) override { திரும்பு default_handler(self); }
		T slice(முனை_2 self, முனை_2, முழு, முழு) override { திரும்பு default_handler(self); }
		T zero_extend(முனை_2 self, முனை_2, முழு) override { திரும்பு default_handler(self); }
		T sign_extend(முனை_2 self, முனை_2, முழு) override { திரும்பு default_handler(self); }
		T concat(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T add(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T sub(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T mul(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T unsigned_div(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T unsigned_mod(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T bitwise_and(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T bitwise_or(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T bitwise_xor(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T bitwise_not(முனை_2 self, முனை_2) override { திரும்பு default_handler(self); }
		T unary_minus(முனை_2 self, முனை_2) override { திரும்பு default_handler(self); }
		T reduce_and(முனை_2 self, முனை_2) override { திரும்பு default_handler(self); }
		T reduce_or(முனை_2 self, முனை_2) override { திரும்பு default_handler(self); }
		T reduce_xor(முனை_2 self, முனை_2) override { திரும்பு default_handler(self); }
		T equal(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T not_equal(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T signed_greater_than(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T signed_greater_equal(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T unsigned_greater_than(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T unsigned_greater_equal(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T logical_shift_இடது(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T logical_shift_வலது(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T arithmetic_shift_வலது(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T mux(முனை_2 self, முனை_2, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T constant(முனை_2 self, RTLIL::Const மாறிலி &) override { திரும்பு default_handler(self); }
		T உள்ளீடு(முனை_2 self, IdString, IdString) override { திரும்பு default_handler(self); }
		T state(முனை_2 self, IdString, IdString) override { திரும்பு default_handler(self); }
		T memory_read(முனை_2 self, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
		T memory_write(முனை_2 self, முனை_2, முனை_2, முனை_2) override { திரும்பு default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		முனை_2 add(IR::முனை_Data &&fn, Sort மாறிலி &sort, std::initializer_list<முனை_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			க்கு (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			திரும்பு முனை_2(ref, _ir.design);
		}
		வெறுமை check_basic_இருமம்(முனை_2 மாறிலி &a, முனை_2 மாறிலி &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		வெறுமை check_shift(முனை_2 மாறிலி &a, முனை_2 மாறிலி &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		வெறுமை check_unary(முனை_2 மாறிலி &a) { log_assert(a.sort().is_signal()); }
	பொது:
		IR &ir() { திரும்பு _ir; }
		முனை_2 slice(முனை_2 a, முழு offset, முழு out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			எனில்(offset == 0 && out_width == a.width())
				திரும்பு a;
			திரும்பு add(IR::முனை_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		முனை_2 extend(முனை_2 a, முழு out_width, bool is_signed) {
			முழு in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			எனில்(in_width == out_width)
				திரும்பு a;
			எனில்(in_width > out_width)
				திரும்பு slice(a, 0, out_width);
			எனில்(is_signed)
				திரும்பு add(Fn::sign_extend, Sort(out_width), {a});
			இல்லையெனில்
				திரும்பு add(Fn::zero_extend, Sort(out_width), {a});
		}
		முனை_2 concat(முனை_2 a, முனை_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			திரும்பு add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		முனை_2 add(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::add, a.sort(), {a, b}); }
		முனை_2 sub(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::sub, a.sort(), {a, b}); }
		முனை_2 mul(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::mul, a.sort(), {a, b}); }
		முனை_2 unsigned_div(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::unsigned_div, a.sort(), {a, b}); }
		முனை_2 unsigned_mod(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::unsigned_mod, a.sort(), {a, b}); }
		முனை_2 bitwise_and(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::bitwise_and, a.sort(), {a, b}); }
		முனை_2 bitwise_or(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::bitwise_or, a.sort(), {a, b}); }
		முனை_2 bitwise_xor(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::bitwise_xor, a.sort(), {a, b}); }
		முனை_2 bitwise_not(முனை_2 a) { check_unary(a); திரும்பு add(Fn::bitwise_not, a.sort(), {a}); }
		முனை_2 unary_minus(முனை_2 a) { check_unary(a); திரும்பு add(Fn::unary_minus, a.sort(), {a}); }
		முனை_2 reduce_and(முனை_2 a) {
			check_unary(a);
			எனில்(a.width() == 1)
				திரும்பு a;
			திரும்பு add(Fn::reduce_and, Sort(1), {a});
		}
		முனை_2 reduce_or(முனை_2 a) {
			check_unary(a);
			எனில்(a.width() == 1)
				திரும்பு a;
			திரும்பு add(Fn::reduce_or, Sort(1), {a});
		}
		முனை_2 reduce_xor(முனை_2 a) {
			check_unary(a);
			எனில்(a.width() == 1)
				திரும்பு a;
			திரும்பு add(Fn::reduce_xor, Sort(1), {a});
		}
		முனை_2 equal(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::equal, Sort(1), {a, b}); }
		முனை_2 not_equal(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::not_equal, Sort(1), {a, b}); }
		முனை_2 signed_greater_than(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::signed_greater_than, Sort(1), {a, b}); }
		முனை_2 signed_greater_equal(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		முனை_2 unsigned_greater_than(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		முனை_2 unsigned_greater_equal(முனை_2 a, முனை_2 b) { check_basic_இருமம்(a, b); திரும்பு add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		முனை_2 logical_shift_இடது(முனை_2 a, முனை_2 b) { check_shift(a, b); திரும்பு add(Fn::logical_shift_இடது, a.sort(), {a, b}); }
		முனை_2 logical_shift_வலது(முனை_2 a, முனை_2 b) { check_shift(a, b); திரும்பு add(Fn::logical_shift_வலது, a.sort(), {a, b}); }
		முனை_2 arithmetic_shift_வலது(முனை_2 a, முனை_2 b) { check_shift(a, b); திரும்பு add(Fn::arithmetic_shift_வலது, a.sort(), {a, b}); }
		முனை_2 mux(முனை_2 a, முனை_2 b, முனை_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			திரும்பு add(Fn::mux, a.sort(), {a, b, s});
		}
		முனை_2 memory_read(முனை_2 mem, முனை_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			திரும்பு add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		முனை_2 memory_write(முனை_2 mem, முனை_2 addr, முனை_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			திரும்பு add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		முனை_2 constant(RTLIL::Const மதிப்பு) {
			முழு s = மதிப்பு.size();
			திரும்பு add(IR::முனை_Data(Fn::constant, std::move(மதிப்பு)), Sort(s), {});
		}
		முனை_2 create_pending(முழு width) {
			திரும்பு add(Fn::buf, Sort(width), {});
		}
		வெறுமை update_pending(முனை_2 முனை, முனை_2 மதிப்பு) {
			log_assert(முனை._ref.function() == Fn::buf && முனை._ref.size() == 0);
			log_assert(முனை.sort() == மதிப்பு.sort());
			_ir.mutate(முனை).append_arg(மதிப்பு._ref);
		}
		IRInput &add_உள்ளீடு(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			எனில் (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			திரும்பு it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			எனில் (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			திரும்பு it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			எனில் (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			திரும்பு it->second;
		}
		முனை_2 மதிப்பு(IRInput மாறிலி& உள்ளீடு) {
			திரும்பு add(IR::முனை_Data(Fn::உள்ளீடு, std::pair(உள்ளீடு.name, உள்ளீடு.kind)), உள்ளீடு.sort, {});
		}
		முனை_2 மதிப்பு(IRState மாறிலி& state) {
			திரும்பு add(IR::முனை_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		வெறுமை suggest_name(முனை_2 முனை, IdString name) {
			_ir.mutate(முனை).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	உள்வரி Factory IR::factory() { திரும்பு Factory(*this); }
	template<class Id> class Scope {
	பாதுகாக்கப்பட்ட:
		எழுத்து substitution_character = '_';
		மெய்நிகர் bool is_character_legal(எழுத்து, முழு) = 0;
	தனிப்பட்ட:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	பொது:
		வெறுமை reserve(std::string name) {
			_used_names.செருகு(std::move(name));
		}
		template<typename N> std::string unique_name(மாறிலி N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			க்கு(size_t i = 0; i < str.size(); i++)
				எனில்(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			எனில்(_used_names.மொத்தம்(str) == 0) {
				_used_names.செருகு(str);
				திரும்பு str;
			}
			க்கு (முழு idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				எனில்(_used_names.மொத்தம்(suffixed) == 0) {
					_used_names.செருகு(suffixed);
					திரும்பு suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, மாறிலி N &suggestion) {
			auto it = _by_id.find(id);
			எனில்(it != _by_id.end())
				திரும்பு it->second;
			std::string str = unique_name(suggestion);
			_by_id.செருகு({id, str});
			திரும்பு str;
		}
	};
	class Writer {
		std::ostream *os;
		வெறுமை print_impl(மாறிலி எழுத்து *fmt, vector<std::function<வெறுமை()>>& fns);
	பொது:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); திரும்பு *this; }
		template<typename... Args>
		வெறுமை print(மாறிலி எழுத்து *fmt, Args&&... args)
		{
			vector<std::function<வெறுமை()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		வெறுமை print_with(Fn fn, மாறிலி எழுத்து *fmt, Args&&... args)
		{
			vector<std::function<வெறுமை()>> fns { [&]() {
				எனில் constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				இல்லையெனில்
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
