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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FUNCTIONAL_H
#ವ್ಯಾಖ್ಯಾನಿಸು FUNCTIONAL_H

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
	ಎಣಿಕೆ class Fn {
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
		ತುಂಡು,
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
		ಕೂಡಿಸು,
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
		logical_ಸರಿಸು_ಎಡ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_ಸರಿಸು_ಬಲ,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_ಸರಿಸು_ಬಲ,
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
		ಸ್ಮರಣೆ_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		ಸ್ಮರಣೆ_write
	};
	// returns the name of a Fn value, as a string literal
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn_ಗಮ್ಯ_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class ವಿಂಗಡಿಸು_2 {
		std::variant<ಪೂರ್ಣಾಂಕ, std::pair<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ>> _v;
	ಸಾರ್ವಜನಿಕ:
		explicit ವಿಂಗಡಿಸು_2(ಪೂರ್ಣಾಂಕ ಅಗಲ) : _v(ಅಗಲ) { }
		ವಿಂಗಡಿಸು_2(ಪೂರ್ಣಾಂಕ addr_ಅಗಲ, ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ) : _v(std::make_pair(addr_ಅಗಲ, ದತ್ತಾಂಶ_ಅಗಲ)) { }
		bool is_signal() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _v.ಸೂಚ್ಯಂಕ() == 0; }
		bool is_ಸ್ಮರಣೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _v.ಸೂಚ್ಯಂಕ() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		ಪೂರ್ಣಾಂಕ ಅಗಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		ಪೂರ್ಣಾಂಕ addr_ಅಗಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<1>(_v).ಮೊದಲ; }
		// returns the data width of a bitvector sort, errors out for other sorts
		ಪೂರ್ಣಾಂಕ ದತ್ತಾಂಶ_ಅಗಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<1>(_v).ಎರಡನೇ; }
		bool ಕ್ರಿಯಾಕಾರಕ==(ವಿಂಗಡಿಸು_2 ಸ್ಥಿರಾಂಕ& other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ { h.eat(_v); ಹಿಂತಿರುಗಿ h; }
	};
	class IR;
	class Factory;
	class ಗಂಟು_2;
	class IRInput {
		ಮಿತ್ರ class Factory;
	ಸಾರ್ವಜನಿಕ:
		PooledName name;
		PooledName ಬಗೆ;
		ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು;
	ಖಾಸಗಿ:
		IRInput(IR &ir, IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು);
	};
	class IROutput {
		ಮಿತ್ರ class Factory;
		IR &_ir;
	ಸಾರ್ವಜನಿಕ:
		PooledName name;
		PooledName ಬಗೆ;
		ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು;
	ಖಾಸಗಿ:
		IROutput(IR &ir, IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು);
	ಸಾರ್ವಜನಿಕ:
		ಗಂಟು_2 ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;
		bool has_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;
		ಶೂನ್ಯ ಗಣ_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ);
	};
	class IRState {
		ಮಿತ್ರ class Factory;
		IR &_ir;
	ಸಾರ್ವಜನಿಕ:
		PooledName name;
		PooledName ಬಗೆ;
		ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು;
	ಖಾಸಗಿ:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು);
	ಸಾರ್ವಜನಿಕ:
		ಗಂಟು_2 ಮುಂದಿನ_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;
		bool has_ಮುಂದಿನ_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ;
		RTLIL::Const ಸ್ಥಿರಾಂಕ& initial_ಮೌಲ್ಯ_signal() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<RTLIL::Const>(_initial); }
		MemContents ಸ್ಥಿರಾಂಕ& initial_ಮೌಲ್ಯ_ಸ್ಮರಣೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<MemContents>(_initial); }
		ಶೂನ್ಯ ಗಣ_ಮುಂದಿನ_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ);
		ಶೂನ್ಯ ಗಣ_initial_ಮೌಲ್ಯ(RTLIL::Const ಮೌಲ್ಯ) { ಮೌಲ್ಯ.extu(ವಿಂಗಡಿಸು.ಅಗಲ()); _initial = std::move(ಮೌಲ್ಯ); }
		ಶೂನ್ಯ ಗಣ_initial_ಮೌಲ್ಯ(MemContents ಮೌಲ್ಯ) { log_assert(ವಿಂಗಡಿಸು_2(ಮೌಲ್ಯ.addr_ಅಗಲ(), ಮೌಲ್ಯ.ದತ್ತಾಂಶ_ಅಗಲ()) == ವಿಂಗಡಿಸು); _initial = std::move(ಮೌಲ್ಯ); }
	};
	class IR {
		ಮಿತ್ರ class Factory;
		ಮಿತ್ರ class ಗಂಟು_2;
		ಮಿತ್ರ class IRInput;
		ಮಿತ್ರ class IROutput;
		ಮಿತ್ರ class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ಗಂಟು_ದತ್ತಾಂಶ {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				ಪೂರ್ಣಾಂಕ
			> _extra;
		ಸಾರ್ವಜನಿಕ:
			ಗಂಟು_ದತ್ತಾಂಶ() : _fn(Fn::invalid) {}
			ಗಂಟು_ದತ್ತಾಂಶ(Fn fn) : _fn(fn) {}
			ಮಾದರಿ<class T> ಗಂಟು_ದತ್ತಾಂಶ(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _fn; }
			ಸ್ಥಿರಾಂಕ RTLIL::Const &as_const() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<std::pair<IdString, IdString>>(_extra); }
			ಪೂರ್ಣಾಂಕ as_int() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ std::get<ಪೂರ್ಣಾಂಕ>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ಸ್ಥಿರಾಂಕ {
				h.eat((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ) _fn);
				h.eat(_extra);
				ಹಿಂತಿರುಗಿ h;
			}
			bool ಕ್ರಿಯಾಕಾರಕ==(ಗಂಟು_ದತ್ತಾಂಶ ಸ್ಥಿರಾಂಕ &other) ಸ್ಥಿರಾಂಕ {
				ಹಿಂತಿರುಗಿ _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		ರಚನೆ Attr {
			ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using ನಕ್ಷೆ = Compute_ನಕ್ಷೆ<ಗಂಟು_ದತ್ತಾಂಶ, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		ನಕ್ಷೆ ನಕ್ಷೆ_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> ಸ್ಥಿತಿಗಳು_2;
		IR::ನಕ್ಷೆ::Ref mutate(ಗಂಟು_2 n);
	ಸಾರ್ವಜನಿಕ:
		Design *design = nullptr;
		ಸ್ಥಿರ IR from_module(Module *module);
		Factory factory();
		ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_4.ಗಾತ್ರ_2(); }
		ಗಂಟು_2 ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i);
		ಶೂನ್ಯ topological_ವಿಂಗಡಿಸು();
		ಶೂನ್ಯ forward_buf();
		IRInput ಸ್ಥಿರಾಂಕ& ಒಳಹರಿವು(IdString name, IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _inputs.at({name, ಬಗೆ}); }
		IRInput ಸ್ಥಿರಾಂಕ& ಒಳಹರಿವು(IdString name) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಒಳಹರಿವು(name, ID($ಒಳಹರಿವು)); }
		IROutput ಸ್ಥಿರಾಂಕ& output(IdString name, IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _outputs.at({name, ಬಗೆ}); }
		IROutput ಸ್ಥಿರಾಂಕ& output(IdString name) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ output(name, ID($output)); }
		IRState ಸ್ಥಿರಾಂಕ& state(IdString name, IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿಗಳು_2.at({name, ಬಗೆ}); }
		IRState ಸ್ಥಿರಾಂಕ& state(IdString name) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ state(name, ID($state)); }
		bool has_ಒಳಹರಿವು(IdString name, IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _inputs.ಎಣಿಕೆ_2({name, ಬಗೆ}); }
		bool has_output(IdString name, IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _outputs.ಎಣಿಕೆ_2({name, ಬಗೆ}); }
		bool has_state(IdString name, IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿಗಳು_2.ಎಣಿಕೆ_2({name, ಬಗೆ}); }
		ಸದಿಶ<IRInput ಸ್ಥಿರಾಂಕ*> inputs(IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ;
		ಸದಿಶ<IRInput ಸ್ಥಿರಾಂಕ*> inputs() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ inputs(ID($ಒಳಹರಿವು)); }
		ಸದಿಶ<IROutput ಸ್ಥಿರಾಂಕ*> outputs(IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ;
		ಸದಿಶ<IROutput ಸ್ಥಿರಾಂಕ*> outputs() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ outputs(ID($output)); }
		ಸದಿಶ<IRState ಸ್ಥಿರಾಂಕ*> ಸ್ಥಿತಿಗಳು(IdString ಬಗೆ) ಸ್ಥಿರಾಂಕ;
		ಸದಿಶ<IRState ಸ್ಥಿರಾಂಕ*> ಸ್ಥಿತಿಗಳು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿಗಳು(ID($state)); }
		ಸದಿಶ<IRInput ಸ್ಥಿರಾಂಕ*> all_inputs() ಸ್ಥಿರಾಂಕ;
		ಸದಿಶ<IROutput ಸ್ಥಿರಾಂಕ*> all_outputs() ಸ್ಥಿರಾಂಕ;
		ಸದಿಶ<IRState ಸ್ಥಿರಾಂಕ*> all_ಸ್ಥಿತಿಗಳು() ಸ್ಥಿರಾಂಕ;
		class iterator {
			ಮಿತ್ರ class IR;
			IR *_ir;
			ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ_3;
			iterator(IR *ir, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ) : _ir(ir), ಸೂಚ್ಯಂಕ_3(ಸೂಚ್ಯಂಕ) {}
		ಸಾರ್ವಜನಿಕ:
			using iterator_category = std::ಒಳಹರಿವು_iterator_ಟ್ಯಾಗ್;
			using ಮೌಲ್ಯ_type = ಗಂಟು_2;
			using ಸೂಚಕ = arrow_proxy<ಗಂಟು_2>;
			using reference = ಗಂಟು_2;
			using difference_type = ptrdiff_t;
			ಗಂಟು_2 ಕ್ರಿಯಾಕಾರಕ*();
			iterator &ಕ್ರಿಯಾಕಾರಕ++() { ಸೂಚ್ಯಂಕ_3++; ಹಿಂತಿರುಗಿ *this; }
			bool ಕ್ರಿಯಾಕಾರಕ!=(iterator ಸ್ಥಿರಾಂಕ &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _ir != other._ir || ಸೂಚ್ಯಂಕ_3 != other.ಸೂಚ್ಯಂಕ_3; }
			bool ಕ್ರಿಯಾಕಾರಕ==(iterator ಸ್ಥಿರಾಂಕ &other) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ !(*this != other); }
			ಸೂಚಕ ಕ್ರಿಯಾಕಾರಕ->();
		};
		iterator begin() { ಹಿಂತಿರುಗಿ iterator(this, 0); }
		iterator ಕೊನೆ() { ಹಿಂತಿರುಗಿ iterator(this, ನಕ್ಷೆ_4.ಗಾತ್ರ_2()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ಗಂಟು_2 {
		ಮಿತ್ರ class Factory;
		ಮಿತ್ರ class IR;
		ಮಿತ್ರ class IRInput;
		ಮಿತ್ರ class IROutput;
		ಮಿತ್ರ class IRState;
		IR::ನಕ್ಷೆ::ConstRef _ref;
		ಗಂಟು_2(IR::ನಕ್ಷೆ::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit ಕ್ರಿಯಾಕಾರಕ IR::ನಕ್ಷೆ::ConstRef() { ಹಿಂತಿರುಗಿ _ref; }
	ಸಾರ್ವಜನಿಕ:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		ಪೂರ್ಣಾಂಕ id() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _ref.ಸೂಚ್ಯಂಕ(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ಸ್ಥಿರಾಂಕ {
			ಆದರೆ(_ref.has_sparse_attr())
				ಹಿಂತಿರುಗಿ {design, _ref.sparse_attr()};
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ {design, design->twines().ಕೂಡಿಸು(std::string("\\n") + std::ಗಮ್ಯ_string(id()))};
		}
		Fn fn() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _ref.function().fn(); }
		ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _ref.attr().ವಿಂಗಡಿಸು; }
		// returns the width of a bitvector node, errors out for other nodes
		ಪೂರ್ಣಾಂಕ ಅಗಲ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ವಿಂಗಡಿಸು().ಅಗಲ(); }
		ಗಾತ್ರ_t arg_ಎಣಿಕೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _ref.ಗಾತ್ರ_2(); }
		ಗಂಟು_2 arg(ಪೂರ್ಣಾಂಕ n) ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಂಟು_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		ಮಾದರಿ<class Visitor> ಸ್ವಯಂಚಾಲಿತ ಭೇಟಿ(Visitor v) ಸ್ಥಿರಾಂಕ
		{
			// currently templated but could be switched to AbstractVisitor &
			ಆಯ್ಕೆ(_ref.function().fn()) {
			ಸ್ಥಿತಿ Fn::invalid: log_error("invalid node in visit\n"); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::buf: ಹಿಂತಿರುಗಿ v.buf(*this, arg(0)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::ತುಂಡು: ಹಿಂತಿರುಗಿ v.ತುಂಡು(*this, arg(0), _ref.function().as_int(), ವಿಂಗಡಿಸು().ಅಗಲ()); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::zero_extend: ಹಿಂತಿರುಗಿ v.zero_extend(*this, arg(0), ಅಗಲ()); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::sign_extend: ಹಿಂತಿರುಗಿ v.sign_extend(*this, arg(0), ಅಗಲ()); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::concat: ಹಿಂತಿರುಗಿ v.concat(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::ಕೂಡಿಸು: ಹಿಂತಿರುಗಿ v.ಕೂಡಿಸು(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::sub: ಹಿಂತಿರುಗಿ v.sub(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::mul: ಹಿಂತಿರುಗಿ v.mul(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::unsigned_div: ಹಿಂತಿರುಗಿ v.unsigned_div(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::unsigned_mod: ಹಿಂತಿರುಗಿ v.unsigned_mod(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::bitwise_and: ಹಿಂತಿರುಗಿ v.bitwise_and(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::bitwise_or: ಹಿಂತಿರುಗಿ v.bitwise_or(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::bitwise_xor: ಹಿಂತಿರುಗಿ v.bitwise_xor(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::bitwise_not: ಹಿಂತಿರುಗಿ v.bitwise_not(*this, arg(0)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::unary_minus: ಹಿಂತಿರುಗಿ v.unary_minus(*this, arg(0)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::reduce_and: ಹಿಂತಿರುಗಿ v.reduce_and(*this, arg(0)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::reduce_or: ಹಿಂತಿರುಗಿ v.reduce_or(*this, arg(0)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::reduce_xor: ಹಿಂತಿರುಗಿ v.reduce_xor(*this, arg(0)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::equal: ಹಿಂತಿರುಗಿ v.equal(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::not_equal: ಹಿಂತಿರುಗಿ v.not_equal(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::signed_greater_than: ಹಿಂತಿರುಗಿ v.signed_greater_than(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::signed_greater_equal: ಹಿಂತಿರುಗಿ v.signed_greater_equal(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::unsigned_greater_than: ಹಿಂತಿರುಗಿ v.unsigned_greater_than(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::unsigned_greater_equal: ಹಿಂತಿರುಗಿ v.unsigned_greater_equal(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::logical_ಸರಿಸು_ಎಡ: ಹಿಂತಿರುಗಿ v.logical_ಸರಿಸು_ಎಡ(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::logical_ಸರಿಸು_ಬಲ: ಹಿಂತಿರುಗಿ v.logical_ಸರಿಸು_ಬಲ(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::arithmetic_ಸರಿಸು_ಬಲ: ಹಿಂತಿರುಗಿ v.arithmetic_ಸರಿಸು_ಬಲ(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::mux: ಹಿಂತಿರುಗಿ v.mux(*this, arg(0), arg(1), arg(2)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::constant: ಹಿಂತಿರುಗಿ v.constant(*this, _ref.function().as_const()); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::ಒಳಹರಿವು: ಹಿಂತಿರುಗಿ v.ಒಳಹರಿವು(*this, _ref.function().as_idstring_pair().ಮೊದಲ, _ref.function().as_idstring_pair().ಎರಡನೇ); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::state: ಹಿಂತಿರುಗಿ v.state(*this, _ref.function().as_idstring_pair().ಮೊದಲ, _ref.function().as_idstring_pair().ಎರಡನೇ); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::ಸ್ಮರಣೆ_read: ಹಿಂತಿರುಗಿ v.ಸ್ಮರಣೆ_read(*this, arg(0), arg(1)); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ Fn::ಸ್ಮರಣೆ_write: ಹಿಂತಿರುಗಿ v.ಸ್ಮರಣೆ_write(*this, arg(0), arg(1), arg(2)); ನಿಲ್ಲಿಸು;
			}
			log_abort();
		}
		std::string ಗಮ್ಯ_string();
		std::string ಗಮ್ಯ_string(std::function<std::string(ಗಂಟು_2)>);
	};
	ಸಾಲೊಳಗೆ IRInput::IRInput(IR &ir, IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು)
		: name(ir.design, name), ಬಗೆ(ir.design, ಬಗೆ), ವಿಂಗಡಿಸು(std::move(ವಿಂಗಡಿಸು)) {}
	ಸಾಲೊಳಗೆ IROutput::IROutput(IR &ir, IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು)
		: _ir(ir), name(ir.design, name), ಬಗೆ(ir.design, ಬಗೆ), ವಿಂಗಡಿಸು(std::move(ವಿಂಗಡಿಸು)) {}
	ಸಾಲೊಳಗೆ IRState::IRState(IR &ir, IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು)
		: _ir(ir), name(ir.design, name), ಬಗೆ(ir.design, ಬಗೆ), ವಿಂಗಡಿಸು(std::move(ವಿಂಗಡಿಸು)) {}
	ಸಾಲೊಳಗೆ IR::ನಕ್ಷೆ::Ref IR::mutate(ಗಂಟು_2 n) { ಹಿಂತಿರುಗಿ ನಕ್ಷೆ_4[n._ref.ಸೂಚ್ಯಂಕ()]; }
	ಸಾಲೊಳಗೆ ಗಂಟು_2 IR::ಕ್ರಿಯಾಕಾರಕ[](ಪೂರ್ಣಾಂಕ i) { ಹಿಂತಿರುಗಿ ಗಂಟು_2(ನಕ್ಷೆ_4[i], design); }
	ಸಾಲೊಳಗೆ ಗಂಟು_2 IROutput::ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಂಟು_2(_ir.ನಕ್ಷೆ_4({name, ಬಗೆ, false}), _ir.design); }
	ಸಾಲೊಳಗೆ bool IROutput::has_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _ir.ನಕ್ಷೆ_4.has_ಕೀಲಿ({name, ಬಗೆ, false}); }
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ IROutput::ಗಣ_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ) { log_assert(ವಿಂಗಡಿಸು == ಮೌಲ್ಯ.ವಿಂಗಡಿಸು()); _ir.mutate(ಮೌಲ್ಯ).ನಿಯೋಜಿಸು_ಕೀಲಿ({name, ಬಗೆ, false}); }
	ಸಾಲೊಳಗೆ ಗಂಟು_2 IRState::ಮುಂದಿನ_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಗಂಟು_2(_ir.ನಕ್ಷೆ_4({name, ಬಗೆ, true}), _ir.design); }
	ಸಾಲೊಳಗೆ bool IRState::has_ಮುಂದಿನ_ಮೌಲ್ಯ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ _ir.ನಕ್ಷೆ_4.has_ಕೀಲಿ({name, ಬಗೆ, true}); }
	ಸಾಲೊಳಗೆ ಶೂನ್ಯ IRState::ಗಣ_ಮುಂದಿನ_ಮೌಲ್ಯ(ಗಂಟು_2 ಮೌಲ್ಯ) { log_assert(ವಿಂಗಡಿಸು == ಮೌಲ್ಯ.ವಿಂಗಡಿಸು()); _ir.mutate(ಮೌಲ್ಯ).ನಿಯೋಜಿಸು_ಕೀಲಿ({name, ಬಗೆ, true}); }
	ಸಾಲೊಳಗೆ ಗಂಟು_2 IR::iterator::ಕ್ರಿಯಾಕಾರಕ*() { ಹಿಂತಿರುಗಿ ಗಂಟು_2(_ir->ನಕ್ಷೆ_4[ಸೂಚ್ಯಂಕ_3], _ir->design); }
	ಸಾಲೊಳಗೆ arrow_proxy<ಗಂಟು_2> IR::iterator::ಕ್ರಿಯಾಕಾರಕ->() { ಹಿಂತಿರುಗಿ arrow_proxy<ಗಂಟು_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	ಮಾದರಿ<class T> ರಚನೆ AbstractVisitor {
		ವಾಸ್ತವಿಕ T buf(ಗಂಟು_2 self, ಗಂಟು_2 n) = 0;
		ವಾಸ್ತವಿಕ T ತುಂಡು(ಗಂಟು_2 self, ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ) = 0;
		ವಾಸ್ತವಿಕ T zero_extend(ಗಂಟು_2 self, ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ) = 0;
		ವಾಸ್ತವಿಕ T sign_extend(ಗಂಟು_2 self, ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ) = 0;
		ವಾಸ್ತವಿಕ T concat(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T ಕೂಡಿಸು(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T sub(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T mul(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T unsigned_div(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T unsigned_mod(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T bitwise_and(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T bitwise_or(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T bitwise_xor(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T bitwise_not(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ವಾಸ್ತವಿಕ T unary_minus(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ವಾಸ್ತವಿಕ T reduce_and(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ವಾಸ್ತವಿಕ T reduce_or(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ವಾಸ್ತವಿಕ T reduce_xor(ಗಂಟು_2 self, ಗಂಟು_2 a) = 0;
		ವಾಸ್ತವಿಕ T equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T not_equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T signed_greater_than(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T signed_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T unsigned_greater_than(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T unsigned_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T logical_ಸರಿಸು_ಎಡ(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T logical_ಸರಿಸು_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T arithmetic_ಸರಿಸು_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b) = 0;
		ವಾಸ್ತವಿಕ T mux(ಗಂಟು_2 self, ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) = 0;
		ವಾಸ್ತವಿಕ T constant(ಗಂಟು_2 self, RTLIL::Const ಸ್ಥಿರಾಂಕ & ಮೌಲ್ಯ) = 0;
		ವಾಸ್ತವಿಕ T ಒಳಹರಿವು(ಗಂಟು_2 self, IdString name, IdString ಬಗೆ) = 0;
		ವಾಸ್ತವಿಕ T state(ಗಂಟು_2 self, IdString name, IdString ಬಗೆ) = 0;
		ವಾಸ್ತವಿಕ T ಸ್ಮರಣೆ_read(ಗಂಟು_2 self, ಗಂಟು_2 mem, ಗಂಟು_2 addr) = 0;
		ವಾಸ್ತವಿಕ T ಸ್ಮರಣೆ_write(ಗಂಟು_2 self, ಗಂಟು_2 mem, ಗಂಟು_2 addr, ಗಂಟು_2 ದತ್ತಾಂಶ) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	ಮಾದರಿ<class T> ರಚನೆ DefaultVisitor : ಸಾರ್ವಜನಿಕ AbstractVisitor<T> {
		ವಾಸ್ತವಿಕ T default_handler(ಗಂಟು_2 self) = 0;
		T buf(ಗಂಟು_2 self, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T ತುಂಡು(ಗಂಟು_2 self, ಗಂಟು_2, ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T zero_extend(ಗಂಟು_2 self, ಗಂಟು_2, ಪೂರ್ಣಾಂಕ) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T sign_extend(ಗಂಟು_2 self, ಗಂಟು_2, ಪೂರ್ಣಾಂಕ) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T concat(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T ಕೂಡಿಸು(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T sub(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T mul(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T unsigned_div(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T unsigned_mod(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T bitwise_and(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T bitwise_or(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T bitwise_xor(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T bitwise_not(ಗಂಟು_2 self, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T unary_minus(ಗಂಟು_2 self, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T reduce_and(ಗಂಟು_2 self, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T reduce_or(ಗಂಟು_2 self, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T reduce_xor(ಗಂಟು_2 self, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T not_equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T signed_greater_than(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T signed_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T unsigned_greater_than(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T unsigned_greater_equal(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T logical_ಸರಿಸು_ಎಡ(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T logical_ಸರಿಸು_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T arithmetic_ಸರಿಸು_ಬಲ(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T mux(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T constant(ಗಂಟು_2 self, RTLIL::Const ಸ್ಥಿರಾಂಕ &) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T ಒಳಹರಿವು(ಗಂಟು_2 self, IdString, IdString) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T state(ಗಂಟು_2 self, IdString, IdString) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T ಸ್ಮರಣೆ_read(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
		T ಸ್ಮರಣೆ_write(ಗಂಟು_2 self, ಗಂಟು_2, ಗಂಟು_2, ಗಂಟು_2) override { ಹಿಂತಿರುಗಿ default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		ಮಿತ್ರ class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ಗಂಟು_2 ಕೂಡಿಸು(IR::ಗಂಟು_ದತ್ತಾಂಶ &&fn, ವಿಂಗಡಿಸು_2 ಸ್ಥಿರಾಂಕ &ವಿಂಗಡಿಸು, std::initializer_list<ಗಂಟು_2> ವಾದಗಳು_2) {
			log_assert(!ವಿಂಗಡಿಸು.is_signal() || ವಿಂಗಡಿಸು.ಅಗಲ() > 0);
			log_assert(!ವಿಂಗಡಿಸು.is_ಸ್ಮರಣೆ() || (ವಿಂಗಡಿಸು.addr_ಅಗಲ() > 0 && ವಿಂಗಡಿಸು.ದತ್ತಾಂಶ_ಅಗಲ() > 0));
			IR::ನಕ್ಷೆ::Ref ref = _ir.ನಕ್ಷೆ_4.ಕೂಡಿಸು(std::move(fn), {std::move(ವಿಂಗಡಿಸು)});
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ arg : ವಾದಗಳು_2)
				ref.append_arg(IR::ನಕ್ಷೆ::ConstRef(arg));
			ಹಿಂತಿರುಗಿ ಗಂಟು_2(ref, _ir.design);
		}
		ಶೂನ್ಯ check_basic_ದ್ವಿಮಾನ(ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &a, ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &b) { log_assert(a.ವಿಂಗಡಿಸು().is_signal() && a.ವಿಂಗಡಿಸು() == b.ವಿಂಗಡಿಸು()); }
		ಶೂನ್ಯ check_ಸರಿಸು(ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &a, ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &b) { log_assert(a.ವಿಂಗಡಿಸು().is_signal() && b.ವಿಂಗಡಿಸು().is_signal() && b.ಅಗಲ() == ceil_log2(a.ಅಗಲ())); }
		ಶೂನ್ಯ check_unary(ಗಂಟು_2 ಸ್ಥಿರಾಂಕ &a) { log_assert(a.ವಿಂಗಡಿಸು().is_signal()); }
	ಸಾರ್ವಜನಿಕ:
		IR &ir() { ಹಿಂತಿರುಗಿ _ir; }
		ಗಂಟು_2 ತುಂಡು(ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ) {
			log_assert(a.ವಿಂಗಡಿಸು().is_signal() && ಸ್ಥಾನಾಂತರ + ಹೊರಹೋಗು_ಅಗಲ <= a.ವಿಂಗಡಿಸು().ಅಗಲ());
			ಆದರೆ(ಸ್ಥಾನಾಂತರ == 0 && ಹೊರಹೋಗು_ಅಗಲ == a.ಅಗಲ())
				ಹಿಂತಿರುಗಿ a;
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(IR::ಗಂಟು_ದತ್ತಾಂಶ(Fn::ತುಂಡು, ಸ್ಥಾನಾಂತರ), ವಿಂಗಡಿಸು_2(ಹೊರಹೋಗು_ಅಗಲ), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ಗಂಟು_2 extend(ಗಂಟು_2 a, ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ, bool is_signed) {
			ಪೂರ್ಣಾಂಕ ಒಳ_ಅಗಲ = a.ವಿಂಗಡಿಸು().ಅಗಲ();
			log_assert(a.ವಿಂಗಡಿಸು().is_signal());
			ಆದರೆ(ಒಳ_ಅಗಲ == ಹೊರಹೋಗು_ಅಗಲ)
				ಹಿಂತಿರುಗಿ a;
			ಆದರೆ(ಒಳ_ಅಗಲ > ಹೊರಹೋಗು_ಅಗಲ)
				ಹಿಂತಿರುಗಿ ತುಂಡು(a, 0, ಹೊರಹೋಗು_ಅಗಲ);
			ಆದರೆ(is_signed)
				ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::sign_extend, ವಿಂಗಡಿಸು_2(ಹೊರಹೋಗು_ಅಗಲ), {a});
			ಇಲ್ಲದಿದ್ದರೆ
				ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::zero_extend, ವಿಂಗಡಿಸು_2(ಹೊರಹೋಗು_ಅಗಲ), {a});
		}
		ಗಂಟು_2 concat(ಗಂಟು_2 a, ಗಂಟು_2 b) {
			log_assert(a.ವಿಂಗಡಿಸು().is_signal() && b.ವಿಂಗಡಿಸು().is_signal());
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::concat, ವಿಂಗಡಿಸು_2(a.ವಿಂಗಡಿಸು().ಅಗಲ() + b.ವಿಂಗಡಿಸು().ಅಗಲ()), {a, b});
		}
		ಗಂಟು_2 ಕೂಡಿಸು(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::ಕೂಡಿಸು, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 sub(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::sub, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 mul(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::mul, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 unsigned_div(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::unsigned_div, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 unsigned_mod(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::unsigned_mod, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 bitwise_and(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::bitwise_and, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 bitwise_or(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::bitwise_or, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 bitwise_xor(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::bitwise_xor, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 bitwise_not(ಗಂಟು_2 a) { check_unary(a); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::bitwise_not, a.ವಿಂಗಡಿಸು(), {a}); }
		ಗಂಟು_2 unary_minus(ಗಂಟು_2 a) { check_unary(a); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::unary_minus, a.ವಿಂಗಡಿಸು(), {a}); }
		ಗಂಟು_2 reduce_and(ಗಂಟು_2 a) {
			check_unary(a);
			ಆದರೆ(a.ಅಗಲ() == 1)
				ಹಿಂತಿರುಗಿ a;
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::reduce_and, ವಿಂಗಡಿಸು_2(1), {a});
		}
		ಗಂಟು_2 reduce_or(ಗಂಟು_2 a) {
			check_unary(a);
			ಆದರೆ(a.ಅಗಲ() == 1)
				ಹಿಂತಿರುಗಿ a;
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::reduce_or, ವಿಂಗಡಿಸು_2(1), {a});
		}
		ಗಂಟು_2 reduce_xor(ಗಂಟು_2 a) {
			check_unary(a);
			ಆದರೆ(a.ಅಗಲ() == 1)
				ಹಿಂತಿರುಗಿ a;
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::reduce_xor, ವಿಂಗಡಿಸು_2(1), {a});
		}
		ಗಂಟು_2 equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::equal, ವಿಂಗಡಿಸು_2(1), {a, b}); }
		ಗಂಟು_2 not_equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::not_equal, ವಿಂಗಡಿಸು_2(1), {a, b}); }
		ಗಂಟು_2 signed_greater_than(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::signed_greater_than, ವಿಂಗಡಿಸು_2(1), {a, b}); }
		ಗಂಟು_2 signed_greater_equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::signed_greater_equal, ವಿಂಗಡಿಸು_2(1), {a, b}); }
		ಗಂಟು_2 unsigned_greater_than(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::unsigned_greater_than, ವಿಂಗಡಿಸು_2(1), {a, b}); }
		ಗಂಟು_2 unsigned_greater_equal(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_basic_ದ್ವಿಮಾನ(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::unsigned_greater_equal, ವಿಂಗಡಿಸು_2(1), {a, b}); }
		ಗಂಟು_2 logical_ಸರಿಸು_ಎಡ(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_ಸರಿಸು(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::logical_ಸರಿಸು_ಎಡ, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 logical_ಸರಿಸು_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_ಸರಿಸು(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::logical_ಸರಿಸು_ಬಲ, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 arithmetic_ಸರಿಸು_ಬಲ(ಗಂಟು_2 a, ಗಂಟು_2 b) { check_ಸರಿಸು(a, b); ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::arithmetic_ಸರಿಸು_ಬಲ, a.ವಿಂಗಡಿಸು(), {a, b}); }
		ಗಂಟು_2 mux(ಗಂಟು_2 a, ಗಂಟು_2 b, ಗಂಟು_2 s) {
			log_assert(a.ವಿಂಗಡಿಸು().is_signal() && a.ವಿಂಗಡಿಸು() == b.ವಿಂಗಡಿಸು() && s.ವಿಂಗಡಿಸು() == ವಿಂಗಡಿಸು_2(1));
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::mux, a.ವಿಂಗಡಿಸು(), {a, b, s});
		}
		ಗಂಟು_2 ಸ್ಮರಣೆ_read(ಗಂಟು_2 mem, ಗಂಟು_2 addr) {
			log_assert(mem.ವಿಂಗಡಿಸು().is_ಸ್ಮರಣೆ() && addr.ವಿಂಗಡಿಸು().is_signal() && mem.ವಿಂಗಡಿಸು().addr_ಅಗಲ() == addr.ವಿಂಗಡಿಸು().ಅಗಲ());
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::ಸ್ಮರಣೆ_read, ವಿಂಗಡಿಸು_2(mem.ವಿಂಗಡಿಸು().ದತ್ತಾಂಶ_ಅಗಲ()), {mem, addr});
		}
		ಗಂಟು_2 ಸ್ಮರಣೆ_write(ಗಂಟು_2 mem, ಗಂಟು_2 addr, ಗಂಟು_2 ದತ್ತಾಂಶ) {
			log_assert(mem.ವಿಂಗಡಿಸು().is_ಸ್ಮರಣೆ() && addr.ವಿಂಗಡಿಸು().is_signal() && ದತ್ತಾಂಶ.ವಿಂಗಡಿಸು().is_signal() &&
				mem.ವಿಂಗಡಿಸು().addr_ಅಗಲ() == addr.ವಿಂಗಡಿಸು().ಅಗಲ() && mem.ವಿಂಗಡಿಸು().ದತ್ತಾಂಶ_ಅಗಲ() == ದತ್ತಾಂಶ.ವಿಂಗಡಿಸು().ಅಗಲ());
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::ಸ್ಮರಣೆ_write, mem.ವಿಂಗಡಿಸು(), {mem, addr, ದತ್ತಾಂಶ});
		}
		ಗಂಟು_2 constant(RTLIL::Const ಮೌಲ್ಯ) {
			ಪೂರ್ಣಾಂಕ s = ಮೌಲ್ಯ.ಗಾತ್ರ_2();
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(IR::ಗಂಟು_ದತ್ತಾಂಶ(Fn::constant, std::move(ಮೌಲ್ಯ)), ವಿಂಗಡಿಸು_2(s), {});
		}
		ಗಂಟು_2 create_pending(ಪೂರ್ಣಾಂಕ ಅಗಲ) {
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(Fn::buf, ವಿಂಗಡಿಸು_2(ಅಗಲ), {});
		}
		ಶೂನ್ಯ update_pending(ಗಂಟು_2 ಗಂಟು, ಗಂಟು_2 ಮೌಲ್ಯ) {
			log_assert(ಗಂಟು._ref.function() == Fn::buf && ಗಂಟು._ref.ಗಾತ್ರ_2() == 0);
			log_assert(ಗಂಟು.ವಿಂಗಡಿಸು() == ಮೌಲ್ಯ.ವಿಂಗಡಿಸು());
			_ir.mutate(ಗಂಟು).append_arg(ಮೌಲ್ಯ._ref);
		}
		IRInput &ಕೂಡಿಸು_ಒಳಹರಿವು(IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು) {
			ಸ್ವಯಂಚಾಲಿತ [it, inserted] = _ir._inputs.emplace({name, ಬಗೆ}, IRInput(_ir, name, ಬಗೆ, std::move(ವಿಂಗಡಿಸು)));
			ಆದರೆ (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
		}
		IROutput &ಕೂಡಿಸು_output(IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು) {
			ಸ್ವಯಂಚಾಲಿತ [it, inserted] = _ir._outputs.emplace({name, ಬಗೆ}, IROutput(_ir, name, ಬಗೆ, std::move(ವಿಂಗಡಿಸು)));
			ಆದರೆ (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
		}
		IRState &ಕೂಡಿಸು_state(IdString name, IdString ಬಗೆ, ವಿಂಗಡಿಸು_2 ವಿಂಗಡಿಸು) {
			ಸ್ವಯಂಚಾಲಿತ [it, inserted] = _ir.ಸ್ಥಿತಿಗಳು_2.emplace({name, ಬಗೆ}, IRState(_ir, name, ಬಗೆ, std::move(ವಿಂಗಡಿಸು)));
			ಆದರೆ (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
		}
		ಗಂಟು_2 ಮೌಲ್ಯ(IRInput ಸ್ಥಿರಾಂಕ& ಒಳಹರಿವು) {
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(IR::ಗಂಟು_ದತ್ತಾಂಶ(Fn::ಒಳಹರಿವು, std::pair(ಒಳಹರಿವು.name, ಒಳಹರಿವು.ಬಗೆ)), ಒಳಹರಿವು.ವಿಂಗಡಿಸು, {});
		}
		ಗಂಟು_2 ಮೌಲ್ಯ(IRState ಸ್ಥಿರಾಂಕ& state) {
			ಹಿಂತಿರುಗಿ ಕೂಡಿಸು(IR::ಗಂಟು_ದತ್ತಾಂಶ(Fn::state, std::pair(state.name, state.ಬಗೆ)), state.ವಿಂಗಡಿಸು, {});
		}
		ಶೂನ್ಯ suggest_name(ಗಂಟು_2 ಗಂಟು, IdString name) {
			_ir.mutate(ಗಂಟು).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	ಸಾಲೊಳಗೆ Factory IR::factory() { ಹಿಂತಿರುಗಿ Factory(*this); }
	ಮಾದರಿ<class Id> class Scope {
	ಸಂರಕ್ಷಿತ:
		ಅಕ್ಷರ substitution_character = '_';
		ವಾಸ್ತವಿಕ bool is_character_legal(ಅಕ್ಷರ, ಪೂರ್ಣಾಂಕ) = 0;
	ಖಾಸಗಿ:
		pool<std::string> ಬಳಸಿದ_ಹೆಸರುಗಳು;
		dict<Id, std::string> _by_id;
	ಸಾರ್ವಜನಿಕ:
		ಶೂನ್ಯ reserve(std::string name) {
			ಬಳಸಿದ_ಹೆಸರುಗಳು.ಸೇರಿಸು(std::move(name));
		}
		ಮಾದರಿ<typename N> std::string unique_name(ಸ್ಥಿರಾಂಕ N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			ಗಾಗಿ(ಗಾತ್ರ_t i = 0; i < str.ಗಾತ್ರ_2(); i++)
				ಆದರೆ(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			ಆದರೆ(ಬಳಸಿದ_ಹೆಸರುಗಳು.ಎಣಿಕೆ_2(str) == 0) {
				ಬಳಸಿದ_ಹೆಸರುಗಳು.ಸೇರಿಸು(str);
				ಹಿಂತಿರುಗಿ str;
			}
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::ಗಮ್ಯ_string(idx);
				ಆದರೆ(ಬಳಸಿದ_ಹೆಸರುಗಳು.ಎಣಿಕೆ_2(suffixed) == 0) {
					ಬಳಸಿದ_ಹೆಸರುಗಳು.ಸೇರಿಸು(suffixed);
					ಹಿಂತಿರುಗಿ suffixed;
				}
			}
		}
		ಮಾದರಿ<typename N> std::string ಕ್ರಿಯಾಕಾರಕ()(Id id, ಸ್ಥಿರಾಂಕ N &suggestion) {
			ಸ್ವಯಂಚಾಲಿತ it = _by_id.ಕಂಡುಹಿಡಿ(id);
			ಆದರೆ(it != _by_id.ಕೊನೆ())
				ಹಿಂತಿರುಗಿ it->ಎರಡನೇ;
			std::string str = unique_name(suggestion);
			_by_id.ಸೇರಿಸು({id, str});
			ಹಿಂತಿರುಗಿ str;
		}
	};
	class Writer {
		std::ostream *os;
		ಶೂನ್ಯ print_impl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ಸದಿಶ<std::function<ಶೂನ್ಯ()>>& fns);
	ಸಾರ್ವಜನಿಕ:
		Writer(std::ostream &os) : os(&os) {}
		ಮಾದರಿ<class T> Writer& ಕ್ರಿಯಾಕಾರಕ <<(T&& arg) { *os << std::forward<T>(arg); ಹಿಂತಿರುಗಿ *this; }
		ಮಾದರಿ<typename... ವಾದಗಳು_3>
		ಶೂನ್ಯ print(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ವಾದಗಳು_3&&... ವಾದಗಳು_2)
		{
			ಸದಿಶ<std::function<ಶೂನ್ಯ()>> fns { [&]() { *this << ವಾದಗಳು_2; }... };
			print_impl(fmt, fns);
		}
		ಮಾದರಿ<typename Fn, typename... ವಾದಗಳು_3>
		ಶೂನ್ಯ print_with(Fn fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ವಾದಗಳು_3&&... ವಾದಗಳು_2)
		{
			ಸದಿಶ<std::function<ಶೂನ್ಯ()>> fns { [&]() {
				ಆದರೆ constexpr (std::is_invocable_v<Fn, ವಾದಗಳು_3>)
					*this << fn(ವಾದಗಳು_2);
				ಇಲ್ಲದಿದ್ದರೆ
					*this << ವಾದಗಳು_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
