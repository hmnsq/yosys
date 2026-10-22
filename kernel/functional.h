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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FUNCTIONAL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FUNCTIONAL_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/compute_graph.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/drivertools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/mem.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/utils.h"

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
	ਗਿਣਤੀ class Fn {
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
		ਟੁਕੜਾ,
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
		ਜੋੜੋ,
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
		logical_ਖਿਸਕਾਓ_ਖੱਬਾ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_ਖਿਸਕਾਓ_ਸੱਜਾ,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		ਨਿਵੇਸ਼,
		// state(a: IdString): any
		// returns the current value of the state variable with the specified name
		state,
		// memory_read(memory: memory[addr_width, data_width], addr: bit[addr_width]): bit[data_width] = memory[addr]
		ਸਮ੍ਰਿਤੀ_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		ਸਮ੍ਰਿਤੀ_write
	};
	// returns the name of a Fn value, as a string literal
	ਅਚਲ ਅੱਖਰ *fn_ਮੰਜ਼ਿਲ_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class ਛਾਂਟੋ_2 {
		std::variant<ਪੂਰਨ, std::pair<ਪੂਰਨ, ਪੂਰਨ>> _v;
	ਜਨਤਕ:
		explicit ਛਾਂਟੋ_2(ਪੂਰਨ ਚੌੜਾਈ) : _v(ਚੌੜਾਈ) { }
		ਛਾਂਟੋ_2(ਪੂਰਨ addr_ਚੌੜਾਈ, ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ) : _v(std::make_pair(addr_ਚੌੜਾਈ, ਅੰਕੜੇ_ਚੌੜਾਈ)) { }
		bool is_signal() ਅਚਲ { ਵਾਪਸ _v.ਸੂਚਕਾਂਕ() == 0; }
		bool is_ਸਮ੍ਰਿਤੀ() ਅਚਲ { ਵਾਪਸ _v.ਸੂਚਕਾਂਕ() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		ਪੂਰਨ ਚੌੜਾਈ() ਅਚਲ { ਵਾਪਸ std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		ਪੂਰਨ addr_ਚੌੜਾਈ() ਅਚਲ { ਵਾਪਸ std::get<1>(_v).ਪਹਿਲਾ; }
		// returns the data width of a bitvector sort, errors out for other sorts
		ਪੂਰਨ ਅੰਕੜੇ_ਚੌੜਾਈ() ਅਚਲ { ਵਾਪਸ std::get<1>(_v).ਦੂਜਾ; }
		bool ਸੰਚਾਲਕ==(ਛਾਂਟੋ_2 ਅਚਲ& other) ਅਚਲ { ਵਾਪਸ _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ { h.eat(_v); ਵਾਪਸ h; }
	};
	class IR;
	class Factory;
	class ਗੰਢ_2;
	class IRInput {
		ਮਿੱਤਰ class Factory;
	ਜਨਤਕ:
		PooledName name;
		PooledName ਕਿਸਮ;
		ਛਾਂਟੋ_2 ਛਾਂਟੋ;
	ਨਿਜੀ:
		IRInput(IR &ir, IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ);
	};
	class IROutput {
		ਮਿੱਤਰ class Factory;
		IR &_ir;
	ਜਨਤਕ:
		PooledName name;
		PooledName ਕਿਸਮ;
		ਛਾਂਟੋ_2 ਛਾਂਟੋ;
	ਨਿਜੀ:
		IROutput(IR &ir, IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ);
	ਜਨਤਕ:
		ਗੰਢ_2 ਮੁੱਲ() ਅਚਲ;
		bool has_ਮੁੱਲ() ਅਚਲ;
		ਖਾਲੀ ਸੈੱਟ_ਮੁੱਲ(ਗੰਢ_2 ਮੁੱਲ);
	};
	class IRState {
		ਮਿੱਤਰ class Factory;
		IR &_ir;
	ਜਨਤਕ:
		PooledName name;
		PooledName ਕਿਸਮ;
		ਛਾਂਟੋ_2 ਛਾਂਟੋ;
	ਨਿਜੀ:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ);
	ਜਨਤਕ:
		ਗੰਢ_2 ਅਗਲਾ_ਮੁੱਲ() ਅਚਲ;
		bool has_ਅਗਲਾ_ਮੁੱਲ() ਅਚਲ;
		RTLIL::Const ਅਚਲ& initial_ਮੁੱਲ_signal() ਅਚਲ { ਵਾਪਸ std::get<RTLIL::Const>(_initial); }
		MemContents ਅਚਲ& initial_ਮੁੱਲ_ਸਮ੍ਰਿਤੀ() ਅਚਲ { ਵਾਪਸ std::get<MemContents>(_initial); }
		ਖਾਲੀ ਸੈੱਟ_ਅਗਲਾ_ਮੁੱਲ(ਗੰਢ_2 ਮੁੱਲ);
		ਖਾਲੀ ਸੈੱਟ_initial_ਮੁੱਲ(RTLIL::Const ਮੁੱਲ) { ਮੁੱਲ.extu(ਛਾਂਟੋ.ਚੌੜਾਈ()); _initial = std::move(ਮੁੱਲ); }
		ਖਾਲੀ ਸੈੱਟ_initial_ਮੁੱਲ(MemContents ਮੁੱਲ) { log_assert(ਛਾਂਟੋ_2(ਮੁੱਲ.addr_ਚੌੜਾਈ(), ਮੁੱਲ.ਅੰਕੜੇ_ਚੌੜਾਈ()) == ਛਾਂਟੋ); _initial = std::move(ਮੁੱਲ); }
	};
	class IR {
		ਮਿੱਤਰ class Factory;
		ਮਿੱਤਰ class ਗੰਢ_2;
		ਮਿੱਤਰ class IRInput;
		ਮਿੱਤਰ class IROutput;
		ਮਿੱਤਰ class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ਗੰਢ_ਅੰਕੜੇ {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				ਪੂਰਨ
			> _extra;
		ਜਨਤਕ:
			ਗੰਢ_ਅੰਕੜੇ() : _fn(Fn::invalid) {}
			ਗੰਢ_ਅੰਕੜੇ(Fn fn) : _fn(fn) {}
			ਸਾਂਚਾ<class T> ਗੰਢ_ਅੰਕੜੇ(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ਅਚਲ { ਵਾਪਸ _fn; }
			ਅਚਲ RTLIL::Const &as_const() ਅਚਲ { ਵਾਪਸ std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ਅਚਲ { ਵਾਪਸ std::get<std::pair<IdString, IdString>>(_extra); }
			ਪੂਰਨ as_int() ਅਚਲ { ਵਾਪਸ std::get<ਪੂਰਨ>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ਅਚਲ {
				h.eat((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ) _fn);
				h.eat(_extra);
				ਵਾਪਸ h;
			}
			bool ਸੰਚਾਲਕ==(ਗੰਢ_ਅੰਕੜੇ ਅਚਲ &other) ਅਚਲ {
				ਵਾਪਸ _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		ਬਣਤਰ Attr {
			ਛਾਂਟੋ_2 ਛਾਂਟੋ;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using ਗ੍ਰਾਫ = Compute_ਗ੍ਰਾਫ<ਗੰਢ_ਅੰਕੜੇ, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		ਗ੍ਰਾਫ ਗ੍ਰਾਫ_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> ਹਾਲਤਾਂ_2;
		IR::ਗ੍ਰਾਫ::Ref mutate(ਗੰਢ_2 n);
	ਜਨਤਕ:
		Design *design = nullptr;
		ਸਥਿਰ IR from_module(Module *module);
		Factory factory();
		ਪੂਰਨ ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਗ੍ਰਾਫ_4.ਮਾਪ(); }
		ਗੰਢ_2 ਸੰਚਾਲਕ[](ਪੂਰਨ i);
		ਖਾਲੀ topological_ਛਾਂਟੋ();
		ਖਾਲੀ forward_buf();
		IRInput ਅਚਲ& ਨਿਵੇਸ਼(IdString name, IdString ਕਿਸਮ) ਅਚਲ { ਵਾਪਸ _inputs.at({name, ਕਿਸਮ}); }
		IRInput ਅਚਲ& ਨਿਵੇਸ਼(IdString name) ਅਚਲ { ਵਾਪਸ ਨਿਵੇਸ਼(name, ID($ਨਿਵੇਸ਼)); }
		IROutput ਅਚਲ& output(IdString name, IdString ਕਿਸਮ) ਅਚਲ { ਵਾਪਸ _outputs.at({name, ਕਿਸਮ}); }
		IROutput ਅਚਲ& output(IdString name) ਅਚਲ { ਵਾਪਸ output(name, ID($output)); }
		IRState ਅਚਲ& state(IdString name, IdString ਕਿਸਮ) ਅਚਲ { ਵਾਪਸ ਹਾਲਤਾਂ_2.at({name, ਕਿਸਮ}); }
		IRState ਅਚਲ& state(IdString name) ਅਚਲ { ਵਾਪਸ state(name, ID($state)); }
		bool has_ਨਿਵੇਸ਼(IdString name, IdString ਕਿਸਮ) ਅਚਲ { ਵਾਪਸ _inputs.ਗਿਣਤੀ_2({name, ਕਿਸਮ}); }
		bool has_output(IdString name, IdString ਕਿਸਮ) ਅਚਲ { ਵਾਪਸ _outputs.ਗਿਣਤੀ_2({name, ਕਿਸਮ}); }
		bool has_state(IdString name, IdString ਕਿਸਮ) ਅਚਲ { ਵਾਪਸ ਹਾਲਤਾਂ_2.ਗਿਣਤੀ_2({name, ਕਿਸਮ}); }
		ਸਦਿਸ਼<IRInput ਅਚਲ*> inputs(IdString ਕਿਸਮ) ਅਚਲ;
		ਸਦਿਸ਼<IRInput ਅਚਲ*> inputs() ਅਚਲ { ਵਾਪਸ inputs(ID($ਨਿਵੇਸ਼)); }
		ਸਦਿਸ਼<IROutput ਅਚਲ*> outputs(IdString ਕਿਸਮ) ਅਚਲ;
		ਸਦਿਸ਼<IROutput ਅਚਲ*> outputs() ਅਚਲ { ਵਾਪਸ outputs(ID($output)); }
		ਸਦਿਸ਼<IRState ਅਚਲ*> ਹਾਲਤਾਂ(IdString ਕਿਸਮ) ਅਚਲ;
		ਸਦਿਸ਼<IRState ਅਚਲ*> ਹਾਲਤਾਂ() ਅਚਲ { ਵਾਪਸ ਹਾਲਤਾਂ(ID($state)); }
		ਸਦਿਸ਼<IRInput ਅਚਲ*> all_inputs() ਅਚਲ;
		ਸਦਿਸ਼<IROutput ਅਚਲ*> all_outputs() ਅਚਲ;
		ਸਦਿਸ਼<IRState ਅਚਲ*> all_ਹਾਲਤਾਂ() ਅਚਲ;
		class iterator {
			ਮਿੱਤਰ class IR;
			IR *_ir;
			ਪੂਰਨ ਸੂਚਕਾਂਕ_3;
			iterator(IR *ir, ਪੂਰਨ ਸੂਚਕਾਂਕ) : _ir(ir), ਸੂਚਕਾਂਕ_3(ਸੂਚਕਾਂਕ) {}
		ਜਨਤਕ:
			using iterator_category = std::ਨਿਵੇਸ਼_iterator_ਟੈਗ;
			using ਮੁੱਲ_type = ਗੰਢ_2;
			using ਸੂਚਕ = arrow_proxy<ਗੰਢ_2>;
			using reference = ਗੰਢ_2;
			using difference_type = ptrdiff_t;
			ਗੰਢ_2 ਸੰਚਾਲਕ*();
			iterator &ਸੰਚਾਲਕ++() { ਸੂਚਕਾਂਕ_3++; ਵਾਪਸ *this; }
			bool ਸੰਚਾਲਕ!=(iterator ਅਚਲ &other) ਅਚਲ { ਵਾਪਸ _ir != other._ir || ਸੂਚਕਾਂਕ_3 != other.ਸੂਚਕਾਂਕ_3; }
			bool ਸੰਚਾਲਕ==(iterator ਅਚਲ &other) ਅਚਲ { ਵਾਪਸ !(*this != other); }
			ਸੂਚਕ ਸੰਚਾਲਕ->();
		};
		iterator begin() { ਵਾਪਸ iterator(this, 0); }
		iterator ਸਿਰਾ() { ਵਾਪਸ iterator(this, ਗ੍ਰਾਫ_4.ਮਾਪ()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ਗੰਢ_2 {
		ਮਿੱਤਰ class Factory;
		ਮਿੱਤਰ class IR;
		ਮਿੱਤਰ class IRInput;
		ਮਿੱਤਰ class IROutput;
		ਮਿੱਤਰ class IRState;
		IR::ਗ੍ਰਾਫ::ConstRef _ref;
		ਗੰਢ_2(IR::ਗ੍ਰਾਫ::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit ਸੰਚਾਲਕ IR::ਗ੍ਰਾਫ::ConstRef() { ਵਾਪਸ _ref; }
	ਜਨਤਕ:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		ਪੂਰਨ id() ਅਚਲ { ਵਾਪਸ _ref.ਸੂਚਕਾਂਕ(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ਅਚਲ {
			ਜੇ(_ref.has_sparse_attr())
				ਵਾਪਸ {design, _ref.sparse_attr()};
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ {design, design->twines().ਜੋੜੋ(std::string("\\n") + std::ਮੰਜ਼ਿਲ_string(id()))};
		}
		Fn fn() ਅਚਲ { ਵਾਪਸ _ref.function().fn(); }
		ਛਾਂਟੋ_2 ਛਾਂਟੋ() ਅਚਲ { ਵਾਪਸ _ref.attr().ਛਾਂਟੋ; }
		// returns the width of a bitvector node, errors out for other nodes
		ਪੂਰਨ ਚੌੜਾਈ() ਅਚਲ { ਵਾਪਸ ਛਾਂਟੋ().ਚੌੜਾਈ(); }
		ਮਾਪ_t arg_ਗਿਣਤੀ() ਅਚਲ { ਵਾਪਸ _ref.ਮਾਪ(); }
		ਗੰਢ_2 arg(ਪੂਰਨ n) ਅਚਲ { ਵਾਪਸ ਗੰਢ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		ਸਾਂਚਾ<class Visitor> ਸਵੈਚਾਲਿਤ ਦੇਖੋ(Visitor v) ਅਚਲ
		{
			// currently templated but could be switched to AbstractVisitor &
			ਚੋਣ(_ref.function().fn()) {
			ਹਾਲਤ Fn::invalid: log_error("invalid node in visit\n"); ਰੋਕੋ;
			ਹਾਲਤ Fn::buf: ਵਾਪਸ v.buf(*this, arg(0)); ਰੋਕੋ;
			ਹਾਲਤ Fn::ਟੁਕੜਾ: ਵਾਪਸ v.ਟੁਕੜਾ(*this, arg(0), _ref.function().as_int(), ਛਾਂਟੋ().ਚੌੜਾਈ()); ਰੋਕੋ;
			ਹਾਲਤ Fn::zero_extend: ਵਾਪਸ v.zero_extend(*this, arg(0), ਚੌੜਾਈ()); ਰੋਕੋ;
			ਹਾਲਤ Fn::sign_extend: ਵਾਪਸ v.sign_extend(*this, arg(0), ਚੌੜਾਈ()); ਰੋਕੋ;
			ਹਾਲਤ Fn::concat: ਵਾਪਸ v.concat(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::ਜੋੜੋ: ਵਾਪਸ v.ਜੋੜੋ(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::sub: ਵਾਪਸ v.sub(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::mul: ਵਾਪਸ v.mul(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::unsigned_div: ਵਾਪਸ v.unsigned_div(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::unsigned_mod: ਵਾਪਸ v.unsigned_mod(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::bitwise_and: ਵਾਪਸ v.bitwise_and(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::bitwise_or: ਵਾਪਸ v.bitwise_or(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::bitwise_xor: ਵਾਪਸ v.bitwise_xor(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::bitwise_not: ਵਾਪਸ v.bitwise_not(*this, arg(0)); ਰੋਕੋ;
			ਹਾਲਤ Fn::unary_minus: ਵਾਪਸ v.unary_minus(*this, arg(0)); ਰੋਕੋ;
			ਹਾਲਤ Fn::reduce_and: ਵਾਪਸ v.reduce_and(*this, arg(0)); ਰੋਕੋ;
			ਹਾਲਤ Fn::reduce_or: ਵਾਪਸ v.reduce_or(*this, arg(0)); ਰੋਕੋ;
			ਹਾਲਤ Fn::reduce_xor: ਵਾਪਸ v.reduce_xor(*this, arg(0)); ਰੋਕੋ;
			ਹਾਲਤ Fn::equal: ਵਾਪਸ v.equal(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::not_equal: ਵਾਪਸ v.not_equal(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::signed_greater_than: ਵਾਪਸ v.signed_greater_than(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::signed_greater_equal: ਵਾਪਸ v.signed_greater_equal(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::unsigned_greater_than: ਵਾਪਸ v.unsigned_greater_than(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::unsigned_greater_equal: ਵਾਪਸ v.unsigned_greater_equal(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::logical_ਖਿਸਕਾਓ_ਖੱਬਾ: ਵਾਪਸ v.logical_ਖਿਸਕਾਓ_ਖੱਬਾ(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::logical_ਖਿਸਕਾਓ_ਸੱਜਾ: ਵਾਪਸ v.logical_ਖਿਸਕਾਓ_ਸੱਜਾ(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ: ਵਾਪਸ v.arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::mux: ਵਾਪਸ v.mux(*this, arg(0), arg(1), arg(2)); ਰੋਕੋ;
			ਹਾਲਤ Fn::constant: ਵਾਪਸ v.constant(*this, _ref.function().as_const()); ਰੋਕੋ;
			ਹਾਲਤ Fn::ਨਿਵੇਸ਼: ਵਾਪਸ v.ਨਿਵੇਸ਼(*this, _ref.function().as_idstring_pair().ਪਹਿਲਾ, _ref.function().as_idstring_pair().ਦੂਜਾ); ਰੋਕੋ;
			ਹਾਲਤ Fn::state: ਵਾਪਸ v.state(*this, _ref.function().as_idstring_pair().ਪਹਿਲਾ, _ref.function().as_idstring_pair().ਦੂਜਾ); ਰੋਕੋ;
			ਹਾਲਤ Fn::ਸਮ੍ਰਿਤੀ_read: ਵਾਪਸ v.ਸਮ੍ਰਿਤੀ_read(*this, arg(0), arg(1)); ਰੋਕੋ;
			ਹਾਲਤ Fn::ਸਮ੍ਰਿਤੀ_write: ਵਾਪਸ v.ਸਮ੍ਰਿਤੀ_write(*this, arg(0), arg(1), arg(2)); ਰੋਕੋ;
			}
			log_abort();
		}
		std::string ਮੰਜ਼ਿਲ_string();
		std::string ਮੰਜ਼ਿਲ_string(std::function<std::string(ਗੰਢ_2)>);
	};
	ਸਤਰਅੰਦਰ IRInput::IRInput(IR &ir, IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ)
		: name(ir.design, name), ਕਿਸਮ(ir.design, ਕਿਸਮ), ਛਾਂਟੋ(std::move(ਛਾਂਟੋ)) {}
	ਸਤਰਅੰਦਰ IROutput::IROutput(IR &ir, IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ)
		: _ir(ir), name(ir.design, name), ਕਿਸਮ(ir.design, ਕਿਸਮ), ਛਾਂਟੋ(std::move(ਛਾਂਟੋ)) {}
	ਸਤਰਅੰਦਰ IRState::IRState(IR &ir, IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ)
		: _ir(ir), name(ir.design, name), ਕਿਸਮ(ir.design, ਕਿਸਮ), ਛਾਂਟੋ(std::move(ਛਾਂਟੋ)) {}
	ਸਤਰਅੰਦਰ IR::ਗ੍ਰਾਫ::Ref IR::mutate(ਗੰਢ_2 n) { ਵਾਪਸ ਗ੍ਰਾਫ_4[n._ref.ਸੂਚਕਾਂਕ()]; }
	ਸਤਰਅੰਦਰ ਗੰਢ_2 IR::ਸੰਚਾਲਕ[](ਪੂਰਨ i) { ਵਾਪਸ ਗੰਢ_2(ਗ੍ਰਾਫ_4[i], design); }
	ਸਤਰਅੰਦਰ ਗੰਢ_2 IROutput::ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ ਗੰਢ_2(_ir.ਗ੍ਰਾਫ_4({name, ਕਿਸਮ, false}), _ir.design); }
	ਸਤਰਅੰਦਰ bool IROutput::has_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ _ir.ਗ੍ਰਾਫ_4.has_ਕੁੰਜੀ({name, ਕਿਸਮ, false}); }
	ਸਤਰਅੰਦਰ ਖਾਲੀ IROutput::ਸੈੱਟ_ਮੁੱਲ(ਗੰਢ_2 ਮੁੱਲ) { log_assert(ਛਾਂਟੋ == ਮੁੱਲ.ਛਾਂਟੋ()); _ir.mutate(ਮੁੱਲ).ਸੌਂਪੋ_ਕੁੰਜੀ({name, ਕਿਸਮ, false}); }
	ਸਤਰਅੰਦਰ ਗੰਢ_2 IRState::ਅਗਲਾ_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ ਗੰਢ_2(_ir.ਗ੍ਰਾਫ_4({name, ਕਿਸਮ, true}), _ir.design); }
	ਸਤਰਅੰਦਰ bool IRState::has_ਅਗਲਾ_ਮੁੱਲ() ਅਚਲ { ਵਾਪਸ _ir.ਗ੍ਰਾਫ_4.has_ਕੁੰਜੀ({name, ਕਿਸਮ, true}); }
	ਸਤਰਅੰਦਰ ਖਾਲੀ IRState::ਸੈੱਟ_ਅਗਲਾ_ਮੁੱਲ(ਗੰਢ_2 ਮੁੱਲ) { log_assert(ਛਾਂਟੋ == ਮੁੱਲ.ਛਾਂਟੋ()); _ir.mutate(ਮੁੱਲ).ਸੌਂਪੋ_ਕੁੰਜੀ({name, ਕਿਸਮ, true}); }
	ਸਤਰਅੰਦਰ ਗੰਢ_2 IR::iterator::ਸੰਚਾਲਕ*() { ਵਾਪਸ ਗੰਢ_2(_ir->ਗ੍ਰਾਫ_4[ਸੂਚਕਾਂਕ_3], _ir->design); }
	ਸਤਰਅੰਦਰ arrow_proxy<ਗੰਢ_2> IR::iterator::ਸੰਚਾਲਕ->() { ਵਾਪਸ arrow_proxy<ਗੰਢ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	ਸਾਂਚਾ<class T> ਬਣਤਰ AbstractVisitor {
		ਆਭਾਸੀ T buf(ਗੰਢ_2 self, ਗੰਢ_2 n) = 0;
		ਆਭਾਸੀ T ਟੁਕੜਾ(ਗੰਢ_2 self, ਗੰਢ_2 a, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ) = 0;
		ਆਭਾਸੀ T zero_extend(ਗੰਢ_2 self, ਗੰਢ_2 a, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ) = 0;
		ਆਭਾਸੀ T sign_extend(ਗੰਢ_2 self, ਗੰਢ_2 a, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ) = 0;
		ਆਭਾਸੀ T concat(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T ਜੋੜੋ(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T sub(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T mul(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T unsigned_div(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T unsigned_mod(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T bitwise_and(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T bitwise_or(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T bitwise_xor(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T bitwise_not(ਗੰਢ_2 self, ਗੰਢ_2 a) = 0;
		ਆਭਾਸੀ T unary_minus(ਗੰਢ_2 self, ਗੰਢ_2 a) = 0;
		ਆਭਾਸੀ T reduce_and(ਗੰਢ_2 self, ਗੰਢ_2 a) = 0;
		ਆਭਾਸੀ T reduce_or(ਗੰਢ_2 self, ਗੰਢ_2 a) = 0;
		ਆਭਾਸੀ T reduce_xor(ਗੰਢ_2 self, ਗੰਢ_2 a) = 0;
		ਆਭਾਸੀ T equal(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T not_equal(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T signed_greater_than(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T signed_greater_equal(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T unsigned_greater_than(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T unsigned_greater_equal(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T logical_ਖਿਸਕਾਓ_ਖੱਬਾ(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T logical_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b) = 0;
		ਆਭਾਸੀ T mux(ਗੰਢ_2 self, ਗੰਢ_2 a, ਗੰਢ_2 b, ਗੰਢ_2 s) = 0;
		ਆਭਾਸੀ T constant(ਗੰਢ_2 self, RTLIL::Const ਅਚਲ & ਮੁੱਲ) = 0;
		ਆਭਾਸੀ T ਨਿਵੇਸ਼(ਗੰਢ_2 self, IdString name, IdString ਕਿਸਮ) = 0;
		ਆਭਾਸੀ T state(ਗੰਢ_2 self, IdString name, IdString ਕਿਸਮ) = 0;
		ਆਭਾਸੀ T ਸਮ੍ਰਿਤੀ_read(ਗੰਢ_2 self, ਗੰਢ_2 mem, ਗੰਢ_2 addr) = 0;
		ਆਭਾਸੀ T ਸਮ੍ਰਿਤੀ_write(ਗੰਢ_2 self, ਗੰਢ_2 mem, ਗੰਢ_2 addr, ਗੰਢ_2 ਅੰਕੜੇ) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	ਸਾਂਚਾ<class T> ਬਣਤਰ DefaultVisitor : ਜਨਤਕ AbstractVisitor<T> {
		ਆਭਾਸੀ T default_handler(ਗੰਢ_2 self) = 0;
		T buf(ਗੰਢ_2 self, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T ਟੁਕੜਾ(ਗੰਢ_2 self, ਗੰਢ_2, ਪੂਰਨ, ਪੂਰਨ) override { ਵਾਪਸ default_handler(self); }
		T zero_extend(ਗੰਢ_2 self, ਗੰਢ_2, ਪੂਰਨ) override { ਵਾਪਸ default_handler(self); }
		T sign_extend(ਗੰਢ_2 self, ਗੰਢ_2, ਪੂਰਨ) override { ਵਾਪਸ default_handler(self); }
		T concat(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T ਜੋੜੋ(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T sub(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T mul(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T unsigned_div(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T unsigned_mod(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T bitwise_and(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T bitwise_or(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T bitwise_xor(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T bitwise_not(ਗੰਢ_2 self, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T unary_minus(ਗੰਢ_2 self, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T reduce_and(ਗੰਢ_2 self, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T reduce_or(ਗੰਢ_2 self, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T reduce_xor(ਗੰਢ_2 self, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T equal(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T not_equal(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T signed_greater_than(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T signed_greater_equal(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T unsigned_greater_than(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T unsigned_greater_equal(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T logical_ਖਿਸਕਾਓ_ਖੱਬਾ(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T logical_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T mux(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T constant(ਗੰਢ_2 self, RTLIL::Const ਅਚਲ &) override { ਵਾਪਸ default_handler(self); }
		T ਨਿਵੇਸ਼(ਗੰਢ_2 self, IdString, IdString) override { ਵਾਪਸ default_handler(self); }
		T state(ਗੰਢ_2 self, IdString, IdString) override { ਵਾਪਸ default_handler(self); }
		T ਸਮ੍ਰਿਤੀ_read(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
		T ਸਮ੍ਰਿਤੀ_write(ਗੰਢ_2 self, ਗੰਢ_2, ਗੰਢ_2, ਗੰਢ_2) override { ਵਾਪਸ default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		ਮਿੱਤਰ class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ਗੰਢ_2 ਜੋੜੋ(IR::ਗੰਢ_ਅੰਕੜੇ &&fn, ਛਾਂਟੋ_2 ਅਚਲ &ਛਾਂਟੋ, std::initializer_list<ਗੰਢ_2> ਦਲੀਲਾਂ_2) {
			log_assert(!ਛਾਂਟੋ.is_signal() || ਛਾਂਟੋ.ਚੌੜਾਈ() > 0);
			log_assert(!ਛਾਂਟੋ.is_ਸਮ੍ਰਿਤੀ() || (ਛਾਂਟੋ.addr_ਚੌੜਾਈ() > 0 && ਛਾਂਟੋ.ਅੰਕੜੇ_ਚੌੜਾਈ() > 0));
			IR::ਗ੍ਰਾਫ::Ref ref = _ir.ਗ੍ਰਾਫ_4.ਜੋੜੋ(std::move(fn), {std::move(ਛਾਂਟੋ)});
			ਲਈ (ਸਵੈਚਾਲਿਤ arg : ਦਲੀਲਾਂ_2)
				ref.append_arg(IR::ਗ੍ਰਾਫ::ConstRef(arg));
			ਵਾਪਸ ਗੰਢ_2(ref, _ir.design);
		}
		ਖਾਲੀ check_basic_ਦੁਆਧਾਰੀ(ਗੰਢ_2 ਅਚਲ &a, ਗੰਢ_2 ਅਚਲ &b) { log_assert(a.ਛਾਂਟੋ().is_signal() && a.ਛਾਂਟੋ() == b.ਛਾਂਟੋ()); }
		ਖਾਲੀ check_ਖਿਸਕਾਓ(ਗੰਢ_2 ਅਚਲ &a, ਗੰਢ_2 ਅਚਲ &b) { log_assert(a.ਛਾਂਟੋ().is_signal() && b.ਛਾਂਟੋ().is_signal() && b.ਚੌੜਾਈ() == ceil_log2(a.ਚੌੜਾਈ())); }
		ਖਾਲੀ check_unary(ਗੰਢ_2 ਅਚਲ &a) { log_assert(a.ਛਾਂਟੋ().is_signal()); }
	ਜਨਤਕ:
		IR &ir() { ਵਾਪਸ _ir; }
		ਗੰਢ_2 ਟੁਕੜਾ(ਗੰਢ_2 a, ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ) {
			log_assert(a.ਛਾਂਟੋ().is_signal() && ਵਿੱਥ + ਬਾਹਰਲਾ_ਚੌੜਾਈ <= a.ਛਾਂਟੋ().ਚੌੜਾਈ());
			ਜੇ(ਵਿੱਥ == 0 && ਬਾਹਰਲਾ_ਚੌੜਾਈ == a.ਚੌੜਾਈ())
				ਵਾਪਸ a;
			ਵਾਪਸ ਜੋੜੋ(IR::ਗੰਢ_ਅੰਕੜੇ(Fn::ਟੁਕੜਾ, ਵਿੱਥ), ਛਾਂਟੋ_2(ਬਾਹਰਲਾ_ਚੌੜਾਈ), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ਗੰਢ_2 extend(ਗੰਢ_2 a, ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ, bool is_signed) {
			ਪੂਰਨ ਵਿੱਚ_ਚੌੜਾਈ = a.ਛਾਂਟੋ().ਚੌੜਾਈ();
			log_assert(a.ਛਾਂਟੋ().is_signal());
			ਜੇ(ਵਿੱਚ_ਚੌੜਾਈ == ਬਾਹਰਲਾ_ਚੌੜਾਈ)
				ਵਾਪਸ a;
			ਜੇ(ਵਿੱਚ_ਚੌੜਾਈ > ਬਾਹਰਲਾ_ਚੌੜਾਈ)
				ਵਾਪਸ ਟੁਕੜਾ(a, 0, ਬਾਹਰਲਾ_ਚੌੜਾਈ);
			ਜੇ(is_signed)
				ਵਾਪਸ ਜੋੜੋ(Fn::sign_extend, ਛਾਂਟੋ_2(ਬਾਹਰਲਾ_ਚੌੜਾਈ), {a});
			ਨਹੀਂਤਾਂ
				ਵਾਪਸ ਜੋੜੋ(Fn::zero_extend, ਛਾਂਟੋ_2(ਬਾਹਰਲਾ_ਚੌੜਾਈ), {a});
		}
		ਗੰਢ_2 concat(ਗੰਢ_2 a, ਗੰਢ_2 b) {
			log_assert(a.ਛਾਂਟੋ().is_signal() && b.ਛਾਂਟੋ().is_signal());
			ਵਾਪਸ ਜੋੜੋ(Fn::concat, ਛਾਂਟੋ_2(a.ਛਾਂਟੋ().ਚੌੜਾਈ() + b.ਛਾਂਟੋ().ਚੌੜਾਈ()), {a, b});
		}
		ਗੰਢ_2 ਜੋੜੋ(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::ਜੋੜੋ, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 sub(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::sub, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 mul(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::mul, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 unsigned_div(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::unsigned_div, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 unsigned_mod(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::unsigned_mod, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 bitwise_and(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::bitwise_and, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 bitwise_or(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::bitwise_or, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 bitwise_xor(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::bitwise_xor, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 bitwise_not(ਗੰਢ_2 a) { check_unary(a); ਵਾਪਸ ਜੋੜੋ(Fn::bitwise_not, a.ਛਾਂਟੋ(), {a}); }
		ਗੰਢ_2 unary_minus(ਗੰਢ_2 a) { check_unary(a); ਵਾਪਸ ਜੋੜੋ(Fn::unary_minus, a.ਛਾਂਟੋ(), {a}); }
		ਗੰਢ_2 reduce_and(ਗੰਢ_2 a) {
			check_unary(a);
			ਜੇ(a.ਚੌੜਾਈ() == 1)
				ਵਾਪਸ a;
			ਵਾਪਸ ਜੋੜੋ(Fn::reduce_and, ਛਾਂਟੋ_2(1), {a});
		}
		ਗੰਢ_2 reduce_or(ਗੰਢ_2 a) {
			check_unary(a);
			ਜੇ(a.ਚੌੜਾਈ() == 1)
				ਵਾਪਸ a;
			ਵਾਪਸ ਜੋੜੋ(Fn::reduce_or, ਛਾਂਟੋ_2(1), {a});
		}
		ਗੰਢ_2 reduce_xor(ਗੰਢ_2 a) {
			check_unary(a);
			ਜੇ(a.ਚੌੜਾਈ() == 1)
				ਵਾਪਸ a;
			ਵਾਪਸ ਜੋੜੋ(Fn::reduce_xor, ਛਾਂਟੋ_2(1), {a});
		}
		ਗੰਢ_2 equal(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::equal, ਛਾਂਟੋ_2(1), {a, b}); }
		ਗੰਢ_2 not_equal(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::not_equal, ਛਾਂਟੋ_2(1), {a, b}); }
		ਗੰਢ_2 signed_greater_than(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::signed_greater_than, ਛਾਂਟੋ_2(1), {a, b}); }
		ਗੰਢ_2 signed_greater_equal(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::signed_greater_equal, ਛਾਂਟੋ_2(1), {a, b}); }
		ਗੰਢ_2 unsigned_greater_than(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::unsigned_greater_than, ਛਾਂਟੋ_2(1), {a, b}); }
		ਗੰਢ_2 unsigned_greater_equal(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_basic_ਦੁਆਧਾਰੀ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::unsigned_greater_equal, ਛਾਂਟੋ_2(1), {a, b}); }
		ਗੰਢ_2 logical_ਖਿਸਕਾਓ_ਖੱਬਾ(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_ਖਿਸਕਾਓ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::logical_ਖਿਸਕਾਓ_ਖੱਬਾ, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 logical_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_ਖਿਸਕਾਓ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::logical_ਖਿਸਕਾਓ_ਸੱਜਾ, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ(ਗੰਢ_2 a, ਗੰਢ_2 b) { check_ਖਿਸਕਾਓ(a, b); ਵਾਪਸ ਜੋੜੋ(Fn::arithmetic_ਖਿਸਕਾਓ_ਸੱਜਾ, a.ਛਾਂਟੋ(), {a, b}); }
		ਗੰਢ_2 mux(ਗੰਢ_2 a, ਗੰਢ_2 b, ਗੰਢ_2 s) {
			log_assert(a.ਛਾਂਟੋ().is_signal() && a.ਛਾਂਟੋ() == b.ਛਾਂਟੋ() && s.ਛਾਂਟੋ() == ਛਾਂਟੋ_2(1));
			ਵਾਪਸ ਜੋੜੋ(Fn::mux, a.ਛਾਂਟੋ(), {a, b, s});
		}
		ਗੰਢ_2 ਸਮ੍ਰਿਤੀ_read(ਗੰਢ_2 mem, ਗੰਢ_2 addr) {
			log_assert(mem.ਛਾਂਟੋ().is_ਸਮ੍ਰਿਤੀ() && addr.ਛਾਂਟੋ().is_signal() && mem.ਛਾਂਟੋ().addr_ਚੌੜਾਈ() == addr.ਛਾਂਟੋ().ਚੌੜਾਈ());
			ਵਾਪਸ ਜੋੜੋ(Fn::ਸਮ੍ਰਿਤੀ_read, ਛਾਂਟੋ_2(mem.ਛਾਂਟੋ().ਅੰਕੜੇ_ਚੌੜਾਈ()), {mem, addr});
		}
		ਗੰਢ_2 ਸਮ੍ਰਿਤੀ_write(ਗੰਢ_2 mem, ਗੰਢ_2 addr, ਗੰਢ_2 ਅੰਕੜੇ) {
			log_assert(mem.ਛਾਂਟੋ().is_ਸਮ੍ਰਿਤੀ() && addr.ਛਾਂਟੋ().is_signal() && ਅੰਕੜੇ.ਛਾਂਟੋ().is_signal() &&
				mem.ਛਾਂਟੋ().addr_ਚੌੜਾਈ() == addr.ਛਾਂਟੋ().ਚੌੜਾਈ() && mem.ਛਾਂਟੋ().ਅੰਕੜੇ_ਚੌੜਾਈ() == ਅੰਕੜੇ.ਛਾਂਟੋ().ਚੌੜਾਈ());
			ਵਾਪਸ ਜੋੜੋ(Fn::ਸਮ੍ਰਿਤੀ_write, mem.ਛਾਂਟੋ(), {mem, addr, ਅੰਕੜੇ});
		}
		ਗੰਢ_2 constant(RTLIL::Const ਮੁੱਲ) {
			ਪੂਰਨ s = ਮੁੱਲ.ਮਾਪ();
			ਵਾਪਸ ਜੋੜੋ(IR::ਗੰਢ_ਅੰਕੜੇ(Fn::constant, std::move(ਮੁੱਲ)), ਛਾਂਟੋ_2(s), {});
		}
		ਗੰਢ_2 create_pending(ਪੂਰਨ ਚੌੜਾਈ) {
			ਵਾਪਸ ਜੋੜੋ(Fn::buf, ਛਾਂਟੋ_2(ਚੌੜਾਈ), {});
		}
		ਖਾਲੀ update_pending(ਗੰਢ_2 ਗੰਢ, ਗੰਢ_2 ਮੁੱਲ) {
			log_assert(ਗੰਢ._ref.function() == Fn::buf && ਗੰਢ._ref.ਮਾਪ() == 0);
			log_assert(ਗੰਢ.ਛਾਂਟੋ() == ਮੁੱਲ.ਛਾਂਟੋ());
			_ir.mutate(ਗੰਢ).append_arg(ਮੁੱਲ._ref);
		}
		IRInput &ਜੋੜੋ_ਨਿਵੇਸ਼(IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ) {
			ਸਵੈਚਾਲਿਤ [it, inserted] = _ir._inputs.emplace({name, ਕਿਸਮ}, IRInput(_ir, name, ਕਿਸਮ, std::move(ਛਾਂਟੋ)));
			ਜੇ (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			ਵਾਪਸ it->ਦੂਜਾ;
		}
		IROutput &ਜੋੜੋ_output(IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ) {
			ਸਵੈਚਾਲਿਤ [it, inserted] = _ir._outputs.emplace({name, ਕਿਸਮ}, IROutput(_ir, name, ਕਿਸਮ, std::move(ਛਾਂਟੋ)));
			ਜੇ (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			ਵਾਪਸ it->ਦੂਜਾ;
		}
		IRState &ਜੋੜੋ_state(IdString name, IdString ਕਿਸਮ, ਛਾਂਟੋ_2 ਛਾਂਟੋ) {
			ਸਵੈਚਾਲਿਤ [it, inserted] = _ir.ਹਾਲਤਾਂ_2.emplace({name, ਕਿਸਮ}, IRState(_ir, name, ਕਿਸਮ, std::move(ਛਾਂਟੋ)));
			ਜੇ (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			ਵਾਪਸ it->ਦੂਜਾ;
		}
		ਗੰਢ_2 ਮੁੱਲ(IRInput ਅਚਲ& ਨਿਵੇਸ਼) {
			ਵਾਪਸ ਜੋੜੋ(IR::ਗੰਢ_ਅੰਕੜੇ(Fn::ਨਿਵੇਸ਼, std::pair(ਨਿਵੇਸ਼.name, ਨਿਵੇਸ਼.ਕਿਸਮ)), ਨਿਵੇਸ਼.ਛਾਂਟੋ, {});
		}
		ਗੰਢ_2 ਮੁੱਲ(IRState ਅਚਲ& state) {
			ਵਾਪਸ ਜੋੜੋ(IR::ਗੰਢ_ਅੰਕੜੇ(Fn::state, std::pair(state.name, state.ਕਿਸਮ)), state.ਛਾਂਟੋ, {});
		}
		ਖਾਲੀ suggest_name(ਗੰਢ_2 ਗੰਢ, IdString name) {
			_ir.mutate(ਗੰਢ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	ਸਤਰਅੰਦਰ Factory IR::factory() { ਵਾਪਸ Factory(*this); }
	ਸਾਂਚਾ<class Id> class Scope {
	ਸੁਰੱਖਿਅਤ:
		ਅੱਖਰ substitution_character = '_';
		ਆਭਾਸੀ bool is_character_legal(ਅੱਖਰ, ਪੂਰਨ) = 0;
	ਨਿਜੀ:
		pool<std::string> ਵਰਤਿਆ_ਨਾਂ;
		dict<Id, std::string> _by_id;
	ਜਨਤਕ:
		ਖਾਲੀ reserve(std::string name) {
			ਵਰਤਿਆ_ਨਾਂ.ਪਾਓ(std::move(name));
		}
		ਸਾਂਚਾ<typename N> std::string unique_name(ਅਚਲ N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			ਲਈ(ਮਾਪ_t i = 0; i < str.ਮਾਪ(); i++)
				ਜੇ(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			ਜੇ(ਵਰਤਿਆ_ਨਾਂ.ਗਿਣਤੀ_2(str) == 0) {
				ਵਰਤਿਆ_ਨਾਂ.ਪਾਓ(str);
				ਵਾਪਸ str;
			}
			ਲਈ (ਪੂਰਨ idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::ਮੰਜ਼ਿਲ_string(idx);
				ਜੇ(ਵਰਤਿਆ_ਨਾਂ.ਗਿਣਤੀ_2(suffixed) == 0) {
					ਵਰਤਿਆ_ਨਾਂ.ਪਾਓ(suffixed);
					ਵਾਪਸ suffixed;
				}
			}
		}
		ਸਾਂਚਾ<typename N> std::string ਸੰਚਾਲਕ()(Id id, ਅਚਲ N &suggestion) {
			ਸਵੈਚਾਲਿਤ it = _by_id.ਲੱਭੋ(id);
			ਜੇ(it != _by_id.ਸਿਰਾ())
				ਵਾਪਸ it->ਦੂਜਾ;
			std::string str = unique_name(suggestion);
			_by_id.ਪਾਓ({id, str});
			ਵਾਪਸ str;
		}
	};
	class Writer {
		std::ostream *os;
		ਖਾਲੀ print_impl(ਅਚਲ ਅੱਖਰ *fmt, ਸਦਿਸ਼<std::function<ਖਾਲੀ()>>& fns);
	ਜਨਤਕ:
		Writer(std::ostream &os) : os(&os) {}
		ਸਾਂਚਾ<class T> Writer& ਸੰਚਾਲਕ <<(T&& arg) { *os << std::forward<T>(arg); ਵਾਪਸ *this; }
		ਸਾਂਚਾ<typename... ਦਲੀਲਾਂ_3>
		ਖਾਲੀ print(ਅਚਲ ਅੱਖਰ *fmt, ਦਲੀਲਾਂ_3&&... ਦਲੀਲਾਂ_2)
		{
			ਸਦਿਸ਼<std::function<ਖਾਲੀ()>> fns { [&]() { *this << ਦਲੀਲਾਂ_2; }... };
			print_impl(fmt, fns);
		}
		ਸਾਂਚਾ<typename Fn, typename... ਦਲੀਲਾਂ_3>
		ਖਾਲੀ print_with(Fn fn, ਅਚਲ ਅੱਖਰ *fmt, ਦਲੀਲਾਂ_3&&... ਦਲੀਲਾਂ_2)
		{
			ਸਦਿਸ਼<std::function<ਖਾਲੀ()>> fns { [&]() {
				ਜੇ constexpr (std::is_invocable_v<Fn, ਦਲੀਲਾਂ_3>)
					*this << fn(ਦਲੀਲਾਂ_2);
				ਨਹੀਂਤਾਂ
					*this << ਦਲੀਲਾਂ_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
