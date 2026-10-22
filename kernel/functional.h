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

#അന്തര്ഭാവ "kernel/yosys.h"
#അന്തര്ഭാവ "kernel/compute_graph.h"
#അന്തര്ഭാവ "kernel/drivertools.h"
#അന്തര്ഭാവ "kernel/mem.h"
#അന്തര്ഭാവ "kernel/utils.h"

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
	എണ്ണൽ class Fn {
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
		logical_shift_ഇടത്,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_വലത്,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_വലത്,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		നിവേശം,
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
	സ്ഥിരം അക്ഷരം *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<പൂർണ്ണസംഖ്യ, std::pair<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ>> _v;
	പൊതു:
		explicit Sort(പൂർണ്ണസംഖ്യ width) : _v(width) { }
		Sort(പൂർണ്ണസംഖ്യ addr_width, പൂർണ്ണസംഖ്യ data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() സ്ഥിരം { തിരികെ _v.index() == 0; }
		bool is_memory() സ്ഥിരം { തിരികെ _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		പൂർണ്ണസംഖ്യ width() സ്ഥിരം { തിരികെ std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		പൂർണ്ണസംഖ്യ addr_width() സ്ഥിരം { തിരികെ std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		പൂർണ്ണസംഖ്യ data_width() സ്ഥിരം { തിരികെ std::get<1>(_v).second; }
		bool operator==(Sort സ്ഥിരം& other) സ്ഥിരം { തിരികെ _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം { h.eat(_v); തിരികെ h; }
	};
	class IR;
	class Factory;
	class കണ്ണി_2;
	class IRInput {
		friend class Factory;
	പൊതു:
		PooledName name;
		PooledName kind;
		Sort sort;
	സ്വകാര്യ:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	പൊതു:
		PooledName name;
		PooledName kind;
		Sort sort;
	സ്വകാര്യ:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	പൊതു:
		കണ്ണി_2 മൂല്യം() സ്ഥിരം;
		bool has_മൂല്യം() സ്ഥിരം;
		ശൂന്യം set_മൂല്യം(കണ്ണി_2 മൂല്യം);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	പൊതു:
		PooledName name;
		PooledName kind;
		Sort sort;
	സ്വകാര്യ:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	പൊതു:
		കണ്ണി_2 next_മൂല്യം() സ്ഥിരം;
		bool has_next_മൂല്യം() സ്ഥിരം;
		RTLIL::Const സ്ഥിരം& initial_മൂല്യം_signal() സ്ഥിരം { തിരികെ std::get<RTLIL::Const>(_initial); }
		MemContents സ്ഥിരം& initial_മൂല്യം_memory() സ്ഥിരം { തിരികെ std::get<MemContents>(_initial); }
		ശൂന്യം set_next_മൂല്യം(കണ്ണി_2 മൂല്യം);
		ശൂന്യം set_initial_മൂല്യം(RTLIL::Const മൂല്യം) { മൂല്യം.extu(sort.width()); _initial = std::move(മൂല്യം); }
		ശൂന്യം set_initial_മൂല്യം(MemContents മൂല്യം) { log_assert(Sort(മൂല്യം.addr_width(), മൂല്യം.data_width()) == sort); _initial = std::move(മൂല്യം); }
	};
	class IR {
		friend class Factory;
		friend class കണ്ണി_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class കണ്ണി_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				പൂർണ്ണസംഖ്യ
			> _extra;
		പൊതു:
			കണ്ണി_Data() : _fn(Fn::invalid) {}
			കണ്ണി_Data(Fn fn) : _fn(fn) {}
			template<class T> കണ്ണി_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() സ്ഥിരം { തിരികെ _fn; }
			സ്ഥിരം RTLIL::Const &as_const() സ്ഥിരം { തിരികെ std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() സ്ഥിരം { തിരികെ std::get<std::pair<IdString, IdString>>(_extra); }
			പൂർണ്ണസംഖ്യ as_int() സ്ഥിരം { തിരികെ std::get<പൂർണ്ണസംഖ്യ>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) സ്ഥിരം {
				h.eat((unsigned പൂർണ്ണസംഖ്യ) _fn);
				h.eat(_extra);
				തിരികെ h;
			}
			bool operator==(കണ്ണി_Data സ്ഥിരം &other) സ്ഥിരം {
				തിരികെ _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		ഘടന Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<കണ്ണി_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(കണ്ണി_2 n);
	പൊതു:
		Design *design = nullptr;
		സ്ഥിതം IR from_module(Module *module);
		Factory factory();
		പൂർണ്ണസംഖ്യ size() സ്ഥിരം { തിരികെ _graph.size(); }
		കണ്ണി_2 operator[](പൂർണ്ണസംഖ്യ i);
		ശൂന്യം topological_sort();
		ശൂന്യം forward_buf();
		IRInput സ്ഥിരം& നിവേശം(IdString name, IdString kind) സ്ഥിരം { തിരികെ _inputs.at({name, kind}); }
		IRInput സ്ഥിരം& നിവേശം(IdString name) സ്ഥിരം { തിരികെ നിവേശം(name, ID($നിവേശം)); }
		IROutput സ്ഥിരം& output(IdString name, IdString kind) സ്ഥിരം { തിരികെ _outputs.at({name, kind}); }
		IROutput സ്ഥിരം& output(IdString name) സ്ഥിരം { തിരികെ output(name, ID($output)); }
		IRState സ്ഥിരം& state(IdString name, IdString kind) സ്ഥിരം { തിരികെ _states.at({name, kind}); }
		IRState സ്ഥിരം& state(IdString name) സ്ഥിരം { തിരികെ state(name, ID($state)); }
		bool has_നിവേശം(IdString name, IdString kind) സ്ഥിരം { തിരികെ _inputs.എണ്ണം({name, kind}); }
		bool has_output(IdString name, IdString kind) സ്ഥിരം { തിരികെ _outputs.എണ്ണം({name, kind}); }
		bool has_state(IdString name, IdString kind) സ്ഥിരം { തിരികെ _states.എണ്ണം({name, kind}); }
		vector<IRInput സ്ഥിരം*> inputs(IdString kind) സ്ഥിരം;
		vector<IRInput സ്ഥിരം*> inputs() സ്ഥിരം { തിരികെ inputs(ID($നിവേശം)); }
		vector<IROutput സ്ഥിരം*> outputs(IdString kind) സ്ഥിരം;
		vector<IROutput സ്ഥിരം*> outputs() സ്ഥിരം { തിരികെ outputs(ID($output)); }
		vector<IRState സ്ഥിരം*> states(IdString kind) സ്ഥിരം;
		vector<IRState സ്ഥിരം*> states() സ്ഥിരം { തിരികെ states(ID($state)); }
		vector<IRInput സ്ഥിരം*> all_inputs() സ്ഥിരം;
		vector<IROutput സ്ഥിരം*> all_outputs() സ്ഥിരം;
		vector<IRState സ്ഥിരം*> all_states() സ്ഥിരം;
		class iterator {
			friend class IR;
			IR *_ir;
			പൂർണ്ണസംഖ്യ _index;
			iterator(IR *ir, പൂർണ്ണസംഖ്യ index) : _ir(ir), _index(index) {}
		പൊതു:
			using iterator_category = std::നിവേശം_iterator_tag;
			using മൂല്യം_type = കണ്ണി_2;
			using pointer = arrow_proxy<കണ്ണി_2>;
			using reference = കണ്ണി_2;
			using difference_type = ptrdiff_t;
			കണ്ണി_2 operator*();
			iterator &operator++() { _index++; തിരികെ *this; }
			bool operator!=(iterator സ്ഥിരം &other) സ്ഥിരം { തിരികെ _ir != other._ir || _index != other._index; }
			bool operator==(iterator സ്ഥിരം &other) സ്ഥിരം { തിരികെ !(*this != other); }
			pointer operator->();
		};
		iterator begin() { തിരികെ iterator(this, 0); }
		iterator end() { തിരികെ iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class കണ്ണി_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		കണ്ണി_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { തിരികെ _ref; }
	പൊതു:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		പൂർണ്ണസംഖ്യ id() സ്ഥിരം { തിരികെ _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() സ്ഥിരം {
			എങ്കിൽ(_ref.has_sparse_attr())
				തിരികെ {design, _ref.sparse_attr()};
			അല്ലെങ്കിൽ
				തിരികെ {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() സ്ഥിരം { തിരികെ _ref.function().fn(); }
		Sort sort() സ്ഥിരം { തിരികെ _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		പൂർണ്ണസംഖ്യ width() സ്ഥിരം { തിരികെ sort().width(); }
		size_t arg_എണ്ണം() സ്ഥിരം { തിരികെ _ref.size(); }
		കണ്ണി_2 arg(പൂർണ്ണസംഖ്യ n) സ്ഥിരം { തിരികെ കണ്ണി_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) സ്ഥിരം
		{
			// currently templated but could be switched to AbstractVisitor &
			തിരഞ്ഞെടുപ്പ്(_ref.function().fn()) {
			അവസ്ഥ Fn::invalid: log_error("invalid node in visit\n"); നിർത്തുക;
			അവസ്ഥ Fn::buf: തിരികെ v.buf(*this, arg(0)); നിർത്തുക;
			അവസ്ഥ Fn::slice: തിരികെ v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); നിർത്തുക;
			അവസ്ഥ Fn::zero_extend: തിരികെ v.zero_extend(*this, arg(0), width()); നിർത്തുക;
			അവസ്ഥ Fn::sign_extend: തിരികെ v.sign_extend(*this, arg(0), width()); നിർത്തുക;
			അവസ്ഥ Fn::concat: തിരികെ v.concat(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::add: തിരികെ v.add(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::sub: തിരികെ v.sub(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::mul: തിരികെ v.mul(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::unsigned_div: തിരികെ v.unsigned_div(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::unsigned_mod: തിരികെ v.unsigned_mod(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::bitwise_and: തിരികെ v.bitwise_and(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::bitwise_or: തിരികെ v.bitwise_or(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::bitwise_xor: തിരികെ v.bitwise_xor(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::bitwise_not: തിരികെ v.bitwise_not(*this, arg(0)); നിർത്തുക;
			അവസ്ഥ Fn::unary_minus: തിരികെ v.unary_minus(*this, arg(0)); നിർത്തുക;
			അവസ്ഥ Fn::reduce_and: തിരികെ v.reduce_and(*this, arg(0)); നിർത്തുക;
			അവസ്ഥ Fn::reduce_or: തിരികെ v.reduce_or(*this, arg(0)); നിർത്തുക;
			അവസ്ഥ Fn::reduce_xor: തിരികെ v.reduce_xor(*this, arg(0)); നിർത്തുക;
			അവസ്ഥ Fn::equal: തിരികെ v.equal(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::not_equal: തിരികെ v.not_equal(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::signed_greater_than: തിരികെ v.signed_greater_than(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::signed_greater_equal: തിരികെ v.signed_greater_equal(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::unsigned_greater_than: തിരികെ v.unsigned_greater_than(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::unsigned_greater_equal: തിരികെ v.unsigned_greater_equal(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::logical_shift_ഇടത്: തിരികെ v.logical_shift_ഇടത്(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::logical_shift_വലത്: തിരികെ v.logical_shift_വലത്(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::arithmetic_shift_വലത്: തിരികെ v.arithmetic_shift_വലത്(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::mux: തിരികെ v.mux(*this, arg(0), arg(1), arg(2)); നിർത്തുക;
			അവസ്ഥ Fn::constant: തിരികെ v.constant(*this, _ref.function().as_const()); നിർത്തുക;
			അവസ്ഥ Fn::നിവേശം: തിരികെ v.നിവേശം(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); നിർത്തുക;
			അവസ്ഥ Fn::state: തിരികെ v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); നിർത്തുക;
			അവസ്ഥ Fn::memory_read: തിരികെ v.memory_read(*this, arg(0), arg(1)); നിർത്തുക;
			അവസ്ഥ Fn::memory_write: തിരികെ v.memory_write(*this, arg(0), arg(1), arg(2)); നിർത്തുക;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(കണ്ണി_2)>);
	};
	വരിയിൽ IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	വരിയിൽ IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	വരിയിൽ IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	വരിയിൽ IR::Graph::Ref IR::mutate(കണ്ണി_2 n) { തിരികെ _graph[n._ref.index()]; }
	വരിയിൽ കണ്ണി_2 IR::operator[](പൂർണ്ണസംഖ്യ i) { തിരികെ കണ്ണി_2(_graph[i], design); }
	വരിയിൽ കണ്ണി_2 IROutput::മൂല്യം() സ്ഥിരം { തിരികെ കണ്ണി_2(_ir._graph({name, kind, false}), _ir.design); }
	വരിയിൽ bool IROutput::has_മൂല്യം() സ്ഥിരം { തിരികെ _ir._graph.has_താക്കോൽ({name, kind, false}); }
	വരിയിൽ ശൂന്യം IROutput::set_മൂല്യം(കണ്ണി_2 മൂല്യം) { log_assert(sort == മൂല്യം.sort()); _ir.mutate(മൂല്യം).assign_താക്കോൽ({name, kind, false}); }
	വരിയിൽ കണ്ണി_2 IRState::next_മൂല്യം() സ്ഥിരം { തിരികെ കണ്ണി_2(_ir._graph({name, kind, true}), _ir.design); }
	വരിയിൽ bool IRState::has_next_മൂല്യം() സ്ഥിരം { തിരികെ _ir._graph.has_താക്കോൽ({name, kind, true}); }
	വരിയിൽ ശൂന്യം IRState::set_next_മൂല്യം(കണ്ണി_2 മൂല്യം) { log_assert(sort == മൂല്യം.sort()); _ir.mutate(മൂല്യം).assign_താക്കോൽ({name, kind, true}); }
	വരിയിൽ കണ്ണി_2 IR::iterator::operator*() { തിരികെ കണ്ണി_2(_ir->_graph[_index], _ir->design); }
	വരിയിൽ arrow_proxy<കണ്ണി_2> IR::iterator::operator->() { തിരികെ arrow_proxy<കണ്ണി_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> ഘടന AbstractVisitor {
		മായാ T buf(കണ്ണി_2 self, കണ്ണി_2 n) = 0;
		മായാ T slice(കണ്ണി_2 self, കണ്ണി_2 a, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ out_width) = 0;
		മായാ T zero_extend(കണ്ണി_2 self, കണ്ണി_2 a, പൂർണ്ണസംഖ്യ out_width) = 0;
		മായാ T sign_extend(കണ്ണി_2 self, കണ്ണി_2 a, പൂർണ്ണസംഖ്യ out_width) = 0;
		മായാ T concat(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T add(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T sub(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T mul(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T unsigned_div(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T unsigned_mod(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T bitwise_and(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T bitwise_or(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T bitwise_xor(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T bitwise_not(കണ്ണി_2 self, കണ്ണി_2 a) = 0;
		മായാ T unary_minus(കണ്ണി_2 self, കണ്ണി_2 a) = 0;
		മായാ T reduce_and(കണ്ണി_2 self, കണ്ണി_2 a) = 0;
		മായാ T reduce_or(കണ്ണി_2 self, കണ്ണി_2 a) = 0;
		മായാ T reduce_xor(കണ്ണി_2 self, കണ്ണി_2 a) = 0;
		മായാ T equal(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T not_equal(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T signed_greater_than(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T signed_greater_equal(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T unsigned_greater_than(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T unsigned_greater_equal(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T logical_shift_ഇടത്(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T logical_shift_വലത്(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T arithmetic_shift_വലത്(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b) = 0;
		മായാ T mux(കണ്ണി_2 self, കണ്ണി_2 a, കണ്ണി_2 b, കണ്ണി_2 s) = 0;
		മായാ T constant(കണ്ണി_2 self, RTLIL::Const സ്ഥിരം & മൂല്യം) = 0;
		മായാ T നിവേശം(കണ്ണി_2 self, IdString name, IdString kind) = 0;
		മായാ T state(കണ്ണി_2 self, IdString name, IdString kind) = 0;
		മായാ T memory_read(കണ്ണി_2 self, കണ്ണി_2 mem, കണ്ണി_2 addr) = 0;
		മായാ T memory_write(കണ്ണി_2 self, കണ്ണി_2 mem, കണ്ണി_2 addr, കണ്ണി_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> ഘടന DefaultVisitor : പൊതു AbstractVisitor<T> {
		മായാ T default_handler(കണ്ണി_2 self) = 0;
		T buf(കണ്ണി_2 self, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T slice(കണ്ണി_2 self, കണ്ണി_2, പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ) override { തിരികെ default_handler(self); }
		T zero_extend(കണ്ണി_2 self, കണ്ണി_2, പൂർണ്ണസംഖ്യ) override { തിരികെ default_handler(self); }
		T sign_extend(കണ്ണി_2 self, കണ്ണി_2, പൂർണ്ണസംഖ്യ) override { തിരികെ default_handler(self); }
		T concat(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T add(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T sub(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T mul(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T unsigned_div(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T unsigned_mod(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T bitwise_and(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T bitwise_or(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T bitwise_xor(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T bitwise_not(കണ്ണി_2 self, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T unary_minus(കണ്ണി_2 self, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T reduce_and(കണ്ണി_2 self, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T reduce_or(കണ്ണി_2 self, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T reduce_xor(കണ്ണി_2 self, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T equal(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T not_equal(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T signed_greater_than(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T signed_greater_equal(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T unsigned_greater_than(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T unsigned_greater_equal(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T logical_shift_ഇടത്(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T logical_shift_വലത്(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T arithmetic_shift_വലത്(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T mux(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T constant(കണ്ണി_2 self, RTLIL::Const സ്ഥിരം &) override { തിരികെ default_handler(self); }
		T നിവേശം(കണ്ണി_2 self, IdString, IdString) override { തിരികെ default_handler(self); }
		T state(കണ്ണി_2 self, IdString, IdString) override { തിരികെ default_handler(self); }
		T memory_read(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
		T memory_write(കണ്ണി_2 self, കണ്ണി_2, കണ്ണി_2, കണ്ണി_2) override { തിരികെ default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		കണ്ണി_2 add(IR::കണ്ണി_Data &&fn, Sort സ്ഥിരം &sort, std::initializer_list<കണ്ണി_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			വേണ്ടി (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			തിരികെ കണ്ണി_2(ref, _ir.design);
		}
		ശൂന്യം check_basic_ദ്വിമാന(കണ്ണി_2 സ്ഥിരം &a, കണ്ണി_2 സ്ഥിരം &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		ശൂന്യം check_shift(കണ്ണി_2 സ്ഥിരം &a, കണ്ണി_2 സ്ഥിരം &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		ശൂന്യം check_unary(കണ്ണി_2 സ്ഥിരം &a) { log_assert(a.sort().is_signal()); }
	പൊതു:
		IR &ir() { തിരികെ _ir; }
		കണ്ണി_2 slice(കണ്ണി_2 a, പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			എങ്കിൽ(offset == 0 && out_width == a.width())
				തിരികെ a;
			തിരികെ add(IR::കണ്ണി_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		കണ്ണി_2 extend(കണ്ണി_2 a, പൂർണ്ണസംഖ്യ out_width, bool is_signed) {
			പൂർണ്ണസംഖ്യ in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			എങ്കിൽ(in_width == out_width)
				തിരികെ a;
			എങ്കിൽ(in_width > out_width)
				തിരികെ slice(a, 0, out_width);
			എങ്കിൽ(is_signed)
				തിരികെ add(Fn::sign_extend, Sort(out_width), {a});
			അല്ലെങ്കിൽ
				തിരികെ add(Fn::zero_extend, Sort(out_width), {a});
		}
		കണ്ണി_2 concat(കണ്ണി_2 a, കണ്ണി_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			തിരികെ add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		കണ്ണി_2 add(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::add, a.sort(), {a, b}); }
		കണ്ണി_2 sub(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::sub, a.sort(), {a, b}); }
		കണ്ണി_2 mul(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::mul, a.sort(), {a, b}); }
		കണ്ണി_2 unsigned_div(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::unsigned_div, a.sort(), {a, b}); }
		കണ്ണി_2 unsigned_mod(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::unsigned_mod, a.sort(), {a, b}); }
		കണ്ണി_2 bitwise_and(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::bitwise_and, a.sort(), {a, b}); }
		കണ്ണി_2 bitwise_or(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::bitwise_or, a.sort(), {a, b}); }
		കണ്ണി_2 bitwise_xor(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::bitwise_xor, a.sort(), {a, b}); }
		കണ്ണി_2 bitwise_not(കണ്ണി_2 a) { check_unary(a); തിരികെ add(Fn::bitwise_not, a.sort(), {a}); }
		കണ്ണി_2 unary_minus(കണ്ണി_2 a) { check_unary(a); തിരികെ add(Fn::unary_minus, a.sort(), {a}); }
		കണ്ണി_2 reduce_and(കണ്ണി_2 a) {
			check_unary(a);
			എങ്കിൽ(a.width() == 1)
				തിരികെ a;
			തിരികെ add(Fn::reduce_and, Sort(1), {a});
		}
		കണ്ണി_2 reduce_or(കണ്ണി_2 a) {
			check_unary(a);
			എങ്കിൽ(a.width() == 1)
				തിരികെ a;
			തിരികെ add(Fn::reduce_or, Sort(1), {a});
		}
		കണ്ണി_2 reduce_xor(കണ്ണി_2 a) {
			check_unary(a);
			എങ്കിൽ(a.width() == 1)
				തിരികെ a;
			തിരികെ add(Fn::reduce_xor, Sort(1), {a});
		}
		കണ്ണി_2 equal(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::equal, Sort(1), {a, b}); }
		കണ്ണി_2 not_equal(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::not_equal, Sort(1), {a, b}); }
		കണ്ണി_2 signed_greater_than(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::signed_greater_than, Sort(1), {a, b}); }
		കണ്ണി_2 signed_greater_equal(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		കണ്ണി_2 unsigned_greater_than(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		കണ്ണി_2 unsigned_greater_equal(കണ്ണി_2 a, കണ്ണി_2 b) { check_basic_ദ്വിമാന(a, b); തിരികെ add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		കണ്ണി_2 logical_shift_ഇടത്(കണ്ണി_2 a, കണ്ണി_2 b) { check_shift(a, b); തിരികെ add(Fn::logical_shift_ഇടത്, a.sort(), {a, b}); }
		കണ്ണി_2 logical_shift_വലത്(കണ്ണി_2 a, കണ്ണി_2 b) { check_shift(a, b); തിരികെ add(Fn::logical_shift_വലത്, a.sort(), {a, b}); }
		കണ്ണി_2 arithmetic_shift_വലത്(കണ്ണി_2 a, കണ്ണി_2 b) { check_shift(a, b); തിരികെ add(Fn::arithmetic_shift_വലത്, a.sort(), {a, b}); }
		കണ്ണി_2 mux(കണ്ണി_2 a, കണ്ണി_2 b, കണ്ണി_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			തിരികെ add(Fn::mux, a.sort(), {a, b, s});
		}
		കണ്ണി_2 memory_read(കണ്ണി_2 mem, കണ്ണി_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			തിരികെ add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		കണ്ണി_2 memory_write(കണ്ണി_2 mem, കണ്ണി_2 addr, കണ്ണി_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			തിരികെ add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		കണ്ണി_2 constant(RTLIL::Const മൂല്യം) {
			പൂർണ്ണസംഖ്യ s = മൂല്യം.size();
			തിരികെ add(IR::കണ്ണി_Data(Fn::constant, std::move(മൂല്യം)), Sort(s), {});
		}
		കണ്ണി_2 create_pending(പൂർണ്ണസംഖ്യ width) {
			തിരികെ add(Fn::buf, Sort(width), {});
		}
		ശൂന്യം update_pending(കണ്ണി_2 കണ്ണി, കണ്ണി_2 മൂല്യം) {
			log_assert(കണ്ണി._ref.function() == Fn::buf && കണ്ണി._ref.size() == 0);
			log_assert(കണ്ണി.sort() == മൂല്യം.sort());
			_ir.mutate(കണ്ണി).append_arg(മൂല്യം._ref);
		}
		IRInput &add_നിവേശം(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			എങ്കിൽ (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			തിരികെ it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			എങ്കിൽ (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			തിരികെ it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			എങ്കിൽ (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			തിരികെ it->second;
		}
		കണ്ണി_2 മൂല്യം(IRInput സ്ഥിരം& നിവേശം) {
			തിരികെ add(IR::കണ്ണി_Data(Fn::നിവേശം, std::pair(നിവേശം.name, നിവേശം.kind)), നിവേശം.sort, {});
		}
		കണ്ണി_2 മൂല്യം(IRState സ്ഥിരം& state) {
			തിരികെ add(IR::കണ്ണി_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		ശൂന്യം suggest_name(കണ്ണി_2 കണ്ണി, IdString name) {
			_ir.mutate(കണ്ണി).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	വരിയിൽ Factory IR::factory() { തിരികെ Factory(*this); }
	template<class Id> class Scope {
	സംരക്ഷിത:
		അക്ഷരം substitution_character = '_';
		മായാ bool is_character_legal(അക്ഷരം, പൂർണ്ണസംഖ്യ) = 0;
	സ്വകാര്യ:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	പൊതു:
		ശൂന്യം reserve(std::string name) {
			_used_names.തിരുകുക(std::move(name));
		}
		template<typename N> std::string unique_name(സ്ഥിരം N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			വേണ്ടി(size_t i = 0; i < str.size(); i++)
				എങ്കിൽ(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			എങ്കിൽ(_used_names.എണ്ണം(str) == 0) {
				_used_names.തിരുകുക(str);
				തിരികെ str;
			}
			വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				എങ്കിൽ(_used_names.എണ്ണം(suffixed) == 0) {
					_used_names.തിരുകുക(suffixed);
					തിരികെ suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, സ്ഥിരം N &suggestion) {
			auto it = _by_id.find(id);
			എങ്കിൽ(it != _by_id.end())
				തിരികെ it->second;
			std::string str = unique_name(suggestion);
			_by_id.തിരുകുക({id, str});
			തിരികെ str;
		}
	};
	class Writer {
		std::ostream *os;
		ശൂന്യം print_impl(സ്ഥിരം അക്ഷരം *fmt, vector<std::function<ശൂന്യം()>>& fns);
	പൊതു:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); തിരികെ *this; }
		template<typename... Args>
		ശൂന്യം print(സ്ഥിരം അക്ഷരം *fmt, Args&&... args)
		{
			vector<std::function<ശൂന്യം()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		ശൂന്യം print_with(Fn fn, സ്ഥിരം അക്ഷരം *fmt, Args&&... args)
		{
			vector<std::function<ശൂന്യം()>> fns { [&]() {
				എങ്കിൽ constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				അല്ലെങ്കിൽ
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
