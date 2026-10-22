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

#यदि_अपरिभाषितम् FUNCTIONAL_H
#परिभाषय FUNCTIONAL_H

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
		खण्डिका,
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
		सङ्कलय,
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
		logical_सरणम्_वामः,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_सरणम्_दक्षिणः,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_सरणम्_दक्षिणः,
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
		स्मृतिः_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		स्मृतिः_write
	};
	// returns the name of a Fn value, as a string literal
	ध्रुव वर्ण *fn_गन्तव्यम्_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class क्रमय_2 {
		std::variant<पूर्णाङ्क, std::pair<पूर्णाङ्क, पूर्णाङ्क>> _v;
	सार्वजनिक:
		explicit क्रमय_2(पूर्णाङ्क विस्तारः) : _v(विस्तारः) { }
		क्रमय_2(पूर्णाङ्क addr_विस्तारः, पूर्णाङ्क दत्त_विस्तारः) : _v(std::make_pair(addr_विस्तारः, दत्त_विस्तारः)) { }
		bool is_signal() ध्रुव { प्रत्यागम _v.सूचकांक() == 0; }
		bool is_स्मृतिः() ध्रुव { प्रत्यागम _v.सूचकांक() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क विस्तारः() ध्रुव { प्रत्यागम std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क addr_विस्तारः() ध्रुव { प्रत्यागम std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		पूर्णाङ्क दत्त_विस्तारः() ध्रुव { प्रत्यागम std::get<1>(_v).second; }
		bool संक्रियकः==(क्रमय_2 ध्रुव& other) ध्रुव { प्रत्यागम _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव { h.eat(_v); प्रत्यागम h; }
	};
	class IR;
	class Factory;
	class ग्रन्थिः_2;
	class IRInput {
		मित्रम् class Factory;
	सार्वजनिक:
		PooledName name;
		PooledName विधा;
		क्रमय_2 क्रमय;
	गुप्त:
		IRInput(IR &ir, IdString name, IdString विधा, क्रमय_2 क्रमय);
	};
	class IROutput {
		मित्रम् class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName विधा;
		क्रमय_2 क्रमय;
	गुप्त:
		IROutput(IR &ir, IdString name, IdString विधा, क्रमय_2 क्रमय);
	सार्वजनिक:
		ग्रन्थिः_2 मानम्() ध्रुव;
		bool has_मानम्() ध्रुव;
		शून्य समुच्चयः_मानम्(ग्रन्थिः_2 मानम्);
	};
	class IRState {
		मित्रम् class Factory;
		IR &_ir;
	सार्वजनिक:
		PooledName name;
		PooledName विधा;
		क्रमय_2 क्रमय;
	गुप्त:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString विधा, क्रमय_2 क्रमय);
	सार्वजनिक:
		ग्रन्थिः_2 अग्र_मानम्() ध्रुव;
		bool has_अग्र_मानम्() ध्रुव;
		RTLIL::Const ध्रुव& initial_मानम्_signal() ध्रुव { प्रत्यागम std::get<RTLIL::Const>(_initial); }
		MemContents ध्रुव& initial_मानम्_स्मृतिः() ध्रुव { प्रत्यागम std::get<MemContents>(_initial); }
		शून्य समुच्चयः_अग्र_मानम्(ग्रन्थिः_2 मानम्);
		शून्य समुच्चयः_initial_मानम्(RTLIL::Const मानम्) { मानम्.extu(क्रमय.विस्तारः()); _initial = std::move(मानम्); }
		शून्य समुच्चयः_initial_मानम्(MemContents मानम्) { log_assert(क्रमय_2(मानम्.addr_विस्तारः(), मानम्.दत्त_विस्तारः()) == क्रमय); _initial = std::move(मानम्); }
	};
	class IR {
		मित्रम् class Factory;
		मित्रम् class ग्रन्थिः_2;
		मित्रम् class IRInput;
		मित्रम् class IROutput;
		मित्रम् class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class ग्रन्थिः_दत्त {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				पूर्णाङ्क
			> _extra;
		सार्वजनिक:
			ग्रन्थिः_दत्त() : _fn(Fn::invalid) {}
			ग्रन्थिः_दत्त(Fn fn) : _fn(fn) {}
			प्रतिरूपम्<class T> ग्रन्थिः_दत्त(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ध्रुव { प्रत्यागम _fn; }
			ध्रुव RTLIL::Const &as_const() ध्रुव { प्रत्यागम std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ध्रुव { प्रत्यागम std::get<std::pair<IdString, IdString>>(_extra); }
			पूर्णाङ्क as_int() ध्रुव { प्रत्यागम std::get<पूर्णाङ्क>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ध्रुव {
				h.eat((अचिह्नित पूर्णाङ्क) _fn);
				h.eat(_extra);
				प्रत्यागम h;
			}
			bool संक्रियकः==(ग्रन्थिः_दत्त ध्रुव &other) ध्रुव {
				प्रत्यागम _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		संरचना Attr {
			क्रमय_2 क्रमय;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<ग्रन्थिः_दत्त, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> अवस्थाः_2;
		IR::Graph::Ref mutate(ग्रन्थिः_2 n);
	सार्वजनिक:
		Design *design = nullptr;
		स्थिर IR from_module(Module *module);
		Factory factory();
		पूर्णाङ्क आकार() ध्रुव { प्रत्यागम _graph.आकार(); }
		ग्रन्थिः_2 संक्रियकः[](पूर्णाङ्क i);
		शून्य topological_क्रमय();
		शून्य forward_buf();
		IRInput ध्रुव& निवेशः(IdString name, IdString विधा) ध्रुव { प्रत्यागम _inputs.at({name, विधा}); }
		IRInput ध्रुव& निवेशः(IdString name) ध्रुव { प्रत्यागम निवेशः(name, ID($निवेशः)); }
		IROutput ध्रुव& output(IdString name, IdString विधा) ध्रुव { प्रत्यागम _outputs.at({name, विधा}); }
		IROutput ध्रुव& output(IdString name) ध्रुव { प्रत्यागम output(name, ID($output)); }
		IRState ध्रुव& state(IdString name, IdString विधा) ध्रुव { प्रत्यागम अवस्थाः_2.at({name, विधा}); }
		IRState ध्रुव& state(IdString name) ध्रुव { प्रत्यागम state(name, ID($state)); }
		bool has_निवेशः(IdString name, IdString विधा) ध्रुव { प्रत्यागम _inputs.गणनम्({name, विधा}); }
		bool has_output(IdString name, IdString विधा) ध्रुव { प्रत्यागम _outputs.गणनम्({name, विधा}); }
		bool has_state(IdString name, IdString विधा) ध्रुव { प्रत्यागम अवस्थाः_2.गणनम्({name, विधा}); }
		सदिशम्<IRInput ध्रुव*> inputs(IdString विधा) ध्रुव;
		सदिशम्<IRInput ध्रुव*> inputs() ध्रुव { प्रत्यागम inputs(ID($निवेशः)); }
		सदिशम्<IROutput ध्रुव*> outputs(IdString विधा) ध्रुव;
		सदिशम्<IROutput ध्रुव*> outputs() ध्रुव { प्रत्यागम outputs(ID($output)); }
		सदिशम्<IRState ध्रुव*> अवस्थाः(IdString विधा) ध्रुव;
		सदिशम्<IRState ध्रुव*> अवस्थाः() ध्रुव { प्रत्यागम अवस्थाः(ID($state)); }
		सदिशम्<IRInput ध्रुव*> all_inputs() ध्रुव;
		सदिशम्<IROutput ध्रुव*> all_outputs() ध्रुव;
		सदिशम्<IRState ध्रुव*> all_अवस्थाः() ध्रुव;
		class iterator {
			मित्रम् class IR;
			IR *_ir;
			पूर्णाङ्क सूचकांक_3;
			iterator(IR *ir, पूर्णाङ्क सूचकांक) : _ir(ir), सूचकांक_3(सूचकांक) {}
		सार्वजनिक:
			using iterator_category = std::निवेशः_iterator_चिह्नकम्;
			using मानम्_type = ग्रन्थिः_2;
			using pointer = arrow_proxy<ग्रन्थिः_2>;
			using reference = ग्रन्थिः_2;
			using difference_type = ptrdiff_t;
			ग्रन्थिः_2 संक्रियकः*();
			iterator &संक्रियकः++() { सूचकांक_3++; प्रत्यागम *this; }
			bool संक्रियकः!=(iterator ध्रुव &other) ध्रुव { प्रत्यागम _ir != other._ir || सूचकांक_3 != other.सूचकांक_3; }
			bool संक्रियकः==(iterator ध्रुव &other) ध्रुव { प्रत्यागम !(*this != other); }
			pointer संक्रियकः->();
		};
		iterator begin() { प्रत्यागम iterator(this, 0); }
		iterator अन्तः() { प्रत्यागम iterator(this, _graph.आकार()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class ग्रन्थिः_2 {
		मित्रम् class Factory;
		मित्रम् class IR;
		मित्रम् class IRInput;
		मित्रम् class IROutput;
		मित्रम् class IRState;
		IR::Graph::ConstRef _ref;
		ग्रन्थिः_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit संक्रियकः IR::Graph::ConstRef() { प्रत्यागम _ref; }
	सार्वजनिक:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		पूर्णाङ्क id() ध्रुव { प्रत्यागम _ref.सूचकांक(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ध्रुव {
			यदि(_ref.has_sparse_attr())
				प्रत्यागम {design, _ref.sparse_attr()};
			अन्यथा
				प्रत्यागम {design, design->twines().सङ्कलय(std::string("\\n") + std::गन्तव्यम्_string(id()))};
		}
		Fn fn() ध्रुव { प्रत्यागम _ref.function().fn(); }
		क्रमय_2 क्रमय() ध्रुव { प्रत्यागम _ref.attr().क्रमय; }
		// returns the width of a bitvector node, errors out for other nodes
		पूर्णाङ्क विस्तारः() ध्रुव { प्रत्यागम क्रमय().विस्तारः(); }
		आकार_t arg_गणनम्() ध्रुव { प्रत्यागम _ref.आकार(); }
		ग्रन्थिः_2 arg(पूर्णाङ्क n) ध्रुव { प्रत्यागम ग्रन्थिः_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		प्रतिरूपम्<class Visitor> स्वचालित दर्शय(Visitor v) ध्रुव
		{
			// currently templated but could be switched to AbstractVisitor &
			विकल्प(_ref.function().fn()) {
			पक्ष Fn::invalid: log_error("invalid node in visit\n"); भङ्ग;
			पक्ष Fn::buf: प्रत्यागम v.buf(*this, arg(0)); भङ्ग;
			पक्ष Fn::खण्डिका: प्रत्यागम v.खण्डिका(*this, arg(0), _ref.function().as_int(), क्रमय().विस्तारः()); भङ्ग;
			पक्ष Fn::zero_extend: प्रत्यागम v.zero_extend(*this, arg(0), विस्तारः()); भङ्ग;
			पक्ष Fn::sign_extend: प्रत्यागम v.sign_extend(*this, arg(0), विस्तारः()); भङ्ग;
			पक्ष Fn::concat: प्रत्यागम v.concat(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::सङ्कलय: प्रत्यागम v.सङ्कलय(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::sub: प्रत्यागम v.sub(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::mul: प्रत्यागम v.mul(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_div: प्रत्यागम v.unsigned_div(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_mod: प्रत्यागम v.unsigned_mod(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_and: प्रत्यागम v.bitwise_and(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_or: प्रत्यागम v.bitwise_or(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_xor: प्रत्यागम v.bitwise_xor(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::bitwise_not: प्रत्यागम v.bitwise_not(*this, arg(0)); भङ्ग;
			पक्ष Fn::unary_minus: प्रत्यागम v.unary_minus(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_and: प्रत्यागम v.reduce_and(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_or: प्रत्यागम v.reduce_or(*this, arg(0)); भङ्ग;
			पक्ष Fn::reduce_xor: प्रत्यागम v.reduce_xor(*this, arg(0)); भङ्ग;
			पक्ष Fn::equal: प्रत्यागम v.equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::not_equal: प्रत्यागम v.not_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::signed_greater_than: प्रत्यागम v.signed_greater_than(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::signed_greater_equal: प्रत्यागम v.signed_greater_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_greater_than: प्रत्यागम v.unsigned_greater_than(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::unsigned_greater_equal: प्रत्यागम v.unsigned_greater_equal(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::logical_सरणम्_वामः: प्रत्यागम v.logical_सरणम्_वामः(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::logical_सरणम्_दक्षिणः: प्रत्यागम v.logical_सरणम्_दक्षिणः(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::arithmetic_सरणम्_दक्षिणः: प्रत्यागम v.arithmetic_सरणम्_दक्षिणः(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::mux: प्रत्यागम v.mux(*this, arg(0), arg(1), arg(2)); भङ्ग;
			पक्ष Fn::constant: प्रत्यागम v.constant(*this, _ref.function().as_const()); भङ्ग;
			पक्ष Fn::निवेशः: प्रत्यागम v.निवेशः(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); भङ्ग;
			पक्ष Fn::state: प्रत्यागम v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); भङ्ग;
			पक्ष Fn::स्मृतिः_read: प्रत्यागम v.स्मृतिः_read(*this, arg(0), arg(1)); भङ्ग;
			पक्ष Fn::स्मृतिः_write: प्रत्यागम v.स्मृतिः_write(*this, arg(0), arg(1), arg(2)); भङ्ग;
			}
			log_abort();
		}
		std::string गन्तव्यम्_string();
		std::string गन्तव्यम्_string(std::function<std::string(ग्रन्थिः_2)>);
	};
	अन्तःस्थ IRInput::IRInput(IR &ir, IdString name, IdString विधा, क्रमय_2 क्रमय)
		: name(ir.design, name), विधा(ir.design, विधा), क्रमय(std::move(क्रमय)) {}
	अन्तःस्थ IROutput::IROutput(IR &ir, IdString name, IdString विधा, क्रमय_2 क्रमय)
		: _ir(ir), name(ir.design, name), विधा(ir.design, विधा), क्रमय(std::move(क्रमय)) {}
	अन्तःस्थ IRState::IRState(IR &ir, IdString name, IdString विधा, क्रमय_2 क्रमय)
		: _ir(ir), name(ir.design, name), विधा(ir.design, विधा), क्रमय(std::move(क्रमय)) {}
	अन्तःस्थ IR::Graph::Ref IR::mutate(ग्रन्थिः_2 n) { प्रत्यागम _graph[n._ref.सूचकांक()]; }
	अन्तःस्थ ग्रन्थिः_2 IR::संक्रियकः[](पूर्णाङ्क i) { प्रत्यागम ग्रन्थिः_2(_graph[i], design); }
	अन्तःस्थ ग्रन्थिः_2 IROutput::मानम्() ध्रुव { प्रत्यागम ग्रन्थिः_2(_ir._graph({name, विधा, false}), _ir.design); }
	अन्तःस्थ bool IROutput::has_मानम्() ध्रुव { प्रत्यागम _ir._graph.has_कुञ्चिका({name, विधा, false}); }
	अन्तःस्थ शून्य IROutput::समुच्चयः_मानम्(ग्रन्थिः_2 मानम्) { log_assert(क्रमय == मानम्.क्रमय()); _ir.mutate(मानम्).नियोजय_कुञ्चिका({name, विधा, false}); }
	अन्तःस्थ ग्रन्थिः_2 IRState::अग्र_मानम्() ध्रुव { प्रत्यागम ग्रन्थिः_2(_ir._graph({name, विधा, true}), _ir.design); }
	अन्तःस्थ bool IRState::has_अग्र_मानम्() ध्रुव { प्रत्यागम _ir._graph.has_कुञ्चिका({name, विधा, true}); }
	अन्तःस्थ शून्य IRState::समुच्चयः_अग्र_मानम्(ग्रन्थिः_2 मानम्) { log_assert(क्रमय == मानम्.क्रमय()); _ir.mutate(मानम्).नियोजय_कुञ्चिका({name, विधा, true}); }
	अन्तःस्थ ग्रन्थिः_2 IR::iterator::संक्रियकः*() { प्रत्यागम ग्रन्थिः_2(_ir->_graph[सूचकांक_3], _ir->design); }
	अन्तःस्थ arrow_proxy<ग्रन्थिः_2> IR::iterator::संक्रियकः->() { प्रत्यागम arrow_proxy<ग्रन्थिः_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	प्रतिरूपम्<class T> संरचना AbstractVisitor {
		आभास T buf(ग्रन्थिः_2 self, ग्रन्थिः_2 n) = 0;
		आभास T खण्डिका(ग्रन्थिः_2 self, ग्रन्थिः_2 a, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क बहिर्गमनम्_विस्तारः) = 0;
		आभास T zero_extend(ग्रन्थिः_2 self, ग्रन्थिः_2 a, पूर्णाङ्क बहिर्गमनम्_विस्तारः) = 0;
		आभास T sign_extend(ग्रन्थिः_2 self, ग्रन्थिः_2 a, पूर्णाङ्क बहिर्गमनम्_विस्तारः) = 0;
		आभास T concat(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T सङ्कलय(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
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
		आभास T logical_सरणम्_वामः(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T logical_सरणम्_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T arithmetic_सरणम्_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b) = 0;
		आभास T mux(ग्रन्थिः_2 self, ग्रन्थिः_2 a, ग्रन्थिः_2 b, ग्रन्थिः_2 s) = 0;
		आभास T constant(ग्रन्थिः_2 self, RTLIL::Const ध्रुव & मानम्) = 0;
		आभास T निवेशः(ग्रन्थिः_2 self, IdString name, IdString विधा) = 0;
		आभास T state(ग्रन्थिः_2 self, IdString name, IdString विधा) = 0;
		आभास T स्मृतिः_read(ग्रन्थिः_2 self, ग्रन्थिः_2 mem, ग्रन्थिः_2 addr) = 0;
		आभास T स्मृतिः_write(ग्रन्थिः_2 self, ग्रन्थिः_2 mem, ग्रन्थिः_2 addr, ग्रन्थिः_2 दत्त) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	प्रतिरूपम्<class T> संरचना DefaultVisitor : सार्वजनिक AbstractVisitor<T> {
		आभास T default_handler(ग्रन्थिः_2 self) = 0;
		T buf(ग्रन्थिः_2 self, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T खण्डिका(ग्रन्थिः_2 self, ग्रन्थिः_2, पूर्णाङ्क, पूर्णाङ्क) override { प्रत्यागम default_handler(self); }
		T zero_extend(ग्रन्थिः_2 self, ग्रन्थिः_2, पूर्णाङ्क) override { प्रत्यागम default_handler(self); }
		T sign_extend(ग्रन्थिः_2 self, ग्रन्थिः_2, पूर्णाङ्क) override { प्रत्यागम default_handler(self); }
		T concat(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T सङ्कलय(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T sub(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T mul(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T unsigned_div(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T unsigned_mod(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T bitwise_and(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T bitwise_or(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T bitwise_xor(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T bitwise_not(ग्रन्थिः_2 self, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T unary_minus(ग्रन्थिः_2 self, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T reduce_and(ग्रन्थिः_2 self, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T reduce_or(ग्रन्थिः_2 self, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T reduce_xor(ग्रन्थिः_2 self, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T not_equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T signed_greater_than(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T signed_greater_equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T unsigned_greater_than(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T unsigned_greater_equal(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T logical_सरणम्_वामः(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T logical_सरणम्_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T arithmetic_सरणम्_दक्षिणः(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T mux(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T constant(ग्रन्थिः_2 self, RTLIL::Const ध्रुव &) override { प्रत्यागम default_handler(self); }
		T निवेशः(ग्रन्थिः_2 self, IdString, IdString) override { प्रत्यागम default_handler(self); }
		T state(ग्रन्थिः_2 self, IdString, IdString) override { प्रत्यागम default_handler(self); }
		T स्मृतिः_read(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
		T स्मृतिः_write(ग्रन्थिः_2 self, ग्रन्थिः_2, ग्रन्थिः_2, ग्रन्थिः_2) override { प्रत्यागम default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		मित्रम् class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		ग्रन्थिः_2 सङ्कलय(IR::ग्रन्थिः_दत्त &&fn, क्रमय_2 ध्रुव &क्रमय, std::initializer_list<ग्रन्थिः_2> तर्काः_2) {
			log_assert(!क्रमय.is_signal() || क्रमय.विस्तारः() > 0);
			log_assert(!क्रमय.is_स्मृतिः() || (क्रमय.addr_विस्तारः() > 0 && क्रमय.दत्त_विस्तारः() > 0));
			IR::Graph::Ref ref = _ir._graph.सङ्कलय(std::move(fn), {std::move(क्रमय)});
			प्रति (स्वचालित arg : तर्काः_2)
				ref.append_arg(IR::Graph::ConstRef(arg));
			प्रत्यागम ग्रन्थिः_2(ref, _ir.design);
		}
		शून्य check_basic_द्विमान(ग्रन्थिः_2 ध्रुव &a, ग्रन्थिः_2 ध्रुव &b) { log_assert(a.क्रमय().is_signal() && a.क्रमय() == b.क्रमय()); }
		शून्य check_सरणम्(ग्रन्थिः_2 ध्रुव &a, ग्रन्थिः_2 ध्रुव &b) { log_assert(a.क्रमय().is_signal() && b.क्रमय().is_signal() && b.विस्तारः() == ceil_log2(a.विस्तारः())); }
		शून्य check_unary(ग्रन्थिः_2 ध्रुव &a) { log_assert(a.क्रमय().is_signal()); }
	सार्वजनिक:
		IR &ir() { प्रत्यागम _ir; }
		ग्रन्थिः_2 खण्डिका(ग्रन्थिः_2 a, पूर्णाङ्क विस्थापनम्, पूर्णाङ्क बहिर्गमनम्_विस्तारः) {
			log_assert(a.क्रमय().is_signal() && विस्थापनम् + बहिर्गमनम्_विस्तारः <= a.क्रमय().विस्तारः());
			यदि(विस्थापनम् == 0 && बहिर्गमनम्_विस्तारः == a.विस्तारः())
				प्रत्यागम a;
			प्रत्यागम सङ्कलय(IR::ग्रन्थिः_दत्त(Fn::खण्डिका, विस्थापनम्), क्रमय_2(बहिर्गमनम्_विस्तारः), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		ग्रन्थिः_2 extend(ग्रन्थिः_2 a, पूर्णाङ्क बहिर्गमनम्_विस्तारः, bool is_signed) {
			पूर्णाङ्क अन्तर्_विस्तारः = a.क्रमय().विस्तारः();
			log_assert(a.क्रमय().is_signal());
			यदि(अन्तर्_विस्तारः == बहिर्गमनम्_विस्तारः)
				प्रत्यागम a;
			यदि(अन्तर्_विस्तारः > बहिर्गमनम्_विस्तारः)
				प्रत्यागम खण्डिका(a, 0, बहिर्गमनम्_विस्तारः);
			यदि(is_signed)
				प्रत्यागम सङ्कलय(Fn::sign_extend, क्रमय_2(बहिर्गमनम्_विस्तारः), {a});
			अन्यथा
				प्रत्यागम सङ्कलय(Fn::zero_extend, क्रमय_2(बहिर्गमनम्_विस्तारः), {a});
		}
		ग्रन्थिः_2 concat(ग्रन्थिः_2 a, ग्रन्थिः_2 b) {
			log_assert(a.क्रमय().is_signal() && b.क्रमय().is_signal());
			प्रत्यागम सङ्कलय(Fn::concat, क्रमय_2(a.क्रमय().विस्तारः() + b.क्रमय().विस्तारः()), {a, b});
		}
		ग्रन्थिः_2 सङ्कलय(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::सङ्कलय, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 sub(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::sub, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 mul(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::mul, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 unsigned_div(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::unsigned_div, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 unsigned_mod(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::unsigned_mod, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 bitwise_and(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::bitwise_and, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 bitwise_or(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::bitwise_or, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 bitwise_xor(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::bitwise_xor, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 bitwise_not(ग्रन्थिः_2 a) { check_unary(a); प्रत्यागम सङ्कलय(Fn::bitwise_not, a.क्रमय(), {a}); }
		ग्रन्थिः_2 unary_minus(ग्रन्थिः_2 a) { check_unary(a); प्रत्यागम सङ्कलय(Fn::unary_minus, a.क्रमय(), {a}); }
		ग्रन्थिः_2 reduce_and(ग्रन्थिः_2 a) {
			check_unary(a);
			यदि(a.विस्तारः() == 1)
				प्रत्यागम a;
			प्रत्यागम सङ्कलय(Fn::reduce_and, क्रमय_2(1), {a});
		}
		ग्रन्थिः_2 reduce_or(ग्रन्थिः_2 a) {
			check_unary(a);
			यदि(a.विस्तारः() == 1)
				प्रत्यागम a;
			प्रत्यागम सङ्कलय(Fn::reduce_or, क्रमय_2(1), {a});
		}
		ग्रन्थिः_2 reduce_xor(ग्रन्थिः_2 a) {
			check_unary(a);
			यदि(a.विस्तारः() == 1)
				प्रत्यागम a;
			प्रत्यागम सङ्कलय(Fn::reduce_xor, क्रमय_2(1), {a});
		}
		ग्रन्थिः_2 equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::equal, क्रमय_2(1), {a, b}); }
		ग्रन्थिः_2 not_equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::not_equal, क्रमय_2(1), {a, b}); }
		ग्रन्थिः_2 signed_greater_than(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::signed_greater_than, क्रमय_2(1), {a, b}); }
		ग्रन्थिः_2 signed_greater_equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::signed_greater_equal, क्रमय_2(1), {a, b}); }
		ग्रन्थिः_2 unsigned_greater_than(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::unsigned_greater_than, क्रमय_2(1), {a, b}); }
		ग्रन्थिः_2 unsigned_greater_equal(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_basic_द्विमान(a, b); प्रत्यागम सङ्कलय(Fn::unsigned_greater_equal, क्रमय_2(1), {a, b}); }
		ग्रन्थिः_2 logical_सरणम्_वामः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_सरणम्(a, b); प्रत्यागम सङ्कलय(Fn::logical_सरणम्_वामः, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 logical_सरणम्_दक्षिणः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_सरणम्(a, b); प्रत्यागम सङ्कलय(Fn::logical_सरणम्_दक्षिणः, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 arithmetic_सरणम्_दक्षिणः(ग्रन्थिः_2 a, ग्रन्थिः_2 b) { check_सरणम्(a, b); प्रत्यागम सङ्कलय(Fn::arithmetic_सरणम्_दक्षिणः, a.क्रमय(), {a, b}); }
		ग्रन्थिः_2 mux(ग्रन्थिः_2 a, ग्रन्थिः_2 b, ग्रन्थिः_2 s) {
			log_assert(a.क्रमय().is_signal() && a.क्रमय() == b.क्रमय() && s.क्रमय() == क्रमय_2(1));
			प्रत्यागम सङ्कलय(Fn::mux, a.क्रमय(), {a, b, s});
		}
		ग्रन्थिः_2 स्मृतिः_read(ग्रन्थिः_2 mem, ग्रन्थिः_2 addr) {
			log_assert(mem.क्रमय().is_स्मृतिः() && addr.क्रमय().is_signal() && mem.क्रमय().addr_विस्तारः() == addr.क्रमय().विस्तारः());
			प्रत्यागम सङ्कलय(Fn::स्मृतिः_read, क्रमय_2(mem.क्रमय().दत्त_विस्तारः()), {mem, addr});
		}
		ग्रन्थिः_2 स्मृतिः_write(ग्रन्थिः_2 mem, ग्रन्थिः_2 addr, ग्रन्थिः_2 दत्त) {
			log_assert(mem.क्रमय().is_स्मृतिः() && addr.क्रमय().is_signal() && दत्त.क्रमय().is_signal() &&
				mem.क्रमय().addr_विस्तारः() == addr.क्रमय().विस्तारः() && mem.क्रमय().दत्त_विस्तारः() == दत्त.क्रमय().विस्तारः());
			प्रत्यागम सङ्कलय(Fn::स्मृतिः_write, mem.क्रमय(), {mem, addr, दत्त});
		}
		ग्रन्थिः_2 constant(RTLIL::Const मानम्) {
			पूर्णाङ्क s = मानम्.आकार();
			प्रत्यागम सङ्कलय(IR::ग्रन्थिः_दत्त(Fn::constant, std::move(मानम्)), क्रमय_2(s), {});
		}
		ग्रन्थिः_2 create_pending(पूर्णाङ्क विस्तारः) {
			प्रत्यागम सङ्कलय(Fn::buf, क्रमय_2(विस्तारः), {});
		}
		शून्य update_pending(ग्रन्थिः_2 ग्रन्थिः, ग्रन्थिः_2 मानम्) {
			log_assert(ग्रन्थिः._ref.function() == Fn::buf && ग्रन्थिः._ref.आकार() == 0);
			log_assert(ग्रन्थिः.क्रमय() == मानम्.क्रमय());
			_ir.mutate(ग्रन्थिः).append_arg(मानम्._ref);
		}
		IRInput &सङ्कलय_निवेशः(IdString name, IdString विधा, क्रमय_2 क्रमय) {
			स्वचालित [it, inserted] = _ir._inputs.emplace({name, विधा}, IRInput(_ir, name, विधा, std::move(क्रमय)));
			यदि (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			प्रत्यागम it->second;
		}
		IROutput &सङ्कलय_output(IdString name, IdString विधा, क्रमय_2 क्रमय) {
			स्वचालित [it, inserted] = _ir._outputs.emplace({name, विधा}, IROutput(_ir, name, विधा, std::move(क्रमय)));
			यदि (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			प्रत्यागम it->second;
		}
		IRState &सङ्कलय_state(IdString name, IdString विधा, क्रमय_2 क्रमय) {
			स्वचालित [it, inserted] = _ir.अवस्थाः_2.emplace({name, विधा}, IRState(_ir, name, विधा, std::move(क्रमय)));
			यदि (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			प्रत्यागम it->second;
		}
		ग्रन्थिः_2 मानम्(IRInput ध्रुव& निवेशः) {
			प्रत्यागम सङ्कलय(IR::ग्रन्थिः_दत्त(Fn::निवेशः, std::pair(निवेशः.name, निवेशः.विधा)), निवेशः.क्रमय, {});
		}
		ग्रन्थिः_2 मानम्(IRState ध्रुव& state) {
			प्रत्यागम सङ्कलय(IR::ग्रन्थिः_दत्त(Fn::state, std::pair(state.name, state.विधा)), state.क्रमय, {});
		}
		शून्य suggest_name(ग्रन्थिः_2 ग्रन्थिः, IdString name) {
			_ir.mutate(ग्रन्थिः).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	अन्तःस्थ Factory IR::factory() { प्रत्यागम Factory(*this); }
	प्रतिरूपम्<class Id> class Scope {
	रक्षित:
		वर्ण substitution_character = '_';
		आभास bool is_character_legal(वर्ण, पूर्णाङ्क) = 0;
	गुप्त:
		pool<std::string> प्रयुक्तम्_नामानि;
		dict<Id, std::string> _by_id;
	सार्वजनिक:
		शून्य reserve(std::string name) {
			प्रयुक्तम्_नामानि.निवेशय(std::move(name));
		}
		प्रतिरूपम्<typename N> std::string unique_name(ध्रुव N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			प्रति(आकार_t i = 0; i < str.आकार(); i++)
				यदि(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			यदि(प्रयुक्तम्_नामानि.गणनम्(str) == 0) {
				प्रयुक्तम्_नामानि.निवेशय(str);
				प्रत्यागम str;
			}
			प्रति (पूर्णाङ्क idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::गन्तव्यम्_string(idx);
				यदि(प्रयुक्तम्_नामानि.गणनम्(suffixed) == 0) {
					प्रयुक्तम्_नामानि.निवेशय(suffixed);
					प्रत्यागम suffixed;
				}
			}
		}
		प्रतिरूपम्<typename N> std::string संक्रियकः()(Id id, ध्रुव N &suggestion) {
			स्वचालित it = _by_id.find(id);
			यदि(it != _by_id.अन्तः())
				प्रत्यागम it->second;
			std::string str = unique_name(suggestion);
			_by_id.निवेशय({id, str});
			प्रत्यागम str;
		}
	};
	class Writer {
		std::ostream *os;
		शून्य print_impl(ध्रुव वर्ण *fmt, सदिशम्<std::function<शून्य()>>& fns);
	सार्वजनिक:
		Writer(std::ostream &os) : os(&os) {}
		प्रतिरूपम्<class T> Writer& संक्रियकः <<(T&& arg) { *os << std::forward<T>(arg); प्रत्यागम *this; }
		प्रतिरूपम्<typename... तर्काः_3>
		शून्य print(ध्रुव वर्ण *fmt, तर्काः_3&&... तर्काः_2)
		{
			सदिशम्<std::function<शून्य()>> fns { [&]() { *this << तर्काः_2; }... };
			print_impl(fmt, fns);
		}
		प्रतिरूपम्<typename Fn, typename... तर्काः_3>
		शून्य print_with(Fn fn, ध्रुव वर्ण *fmt, तर्काः_3&&... तर्काः_2)
		{
			सदिशम्<std::function<शून्य()>> fns { [&]() {
				यदि constexpr (std::is_invocable_v<Fn, तर्काः_3>)
					*this << fn(तर्काः_2);
				अन्यथा
					*this << तर्काः_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
