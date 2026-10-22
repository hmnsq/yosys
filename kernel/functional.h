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

#నిర్వచితం_కాకపోతే FUNCTIONAL_H
#నిర్వచించు FUNCTIONAL_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/compute_graph.h"
#అన్తర్భావ "kernel/drivertools.h"
#అన్తర్భావ "kernel/mem.h"
#అన్తర్భావ "kernel/utils.h"

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
	గణన class Fn {
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
		ముక్క,
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
		కూడు,
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
		logical_జరుపు_ఎడమ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_జరుపు_కుడి,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_జరుపు_కుడి,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		ఆదానం,
		// state(a: IdString): any
		// returns the current value of the state variable with the specified name
		state,
		// memory_read(memory: memory[addr_width, data_width], addr: bit[addr_width]): bit[data_width] = memory[addr]
		జ్ఞాపకం_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		జ్ఞాపకం_write
	};
	// returns the name of a Fn value, as a string literal
	స్థిరరాశి అక్షరం *fn_గమ్యం_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class క్రమీకరించు_2 {
		std::variant<పూర్ణాంకం, std::pair<పూర్ణాంకం, పూర్ణాంకం>> _v;
	బహిరంగ:
		explicit క్రమీకరించు_2(పూర్ణాంకం వెడల్పు) : _v(వెడల్పు) { }
		క్రమీకరించు_2(పూర్ణాంకం addr_వెడల్పు, పూర్ణాంకం దత్తాంశం_వెడల్పు) : _v(std::make_pair(addr_వెడల్పు, దత్తాంశం_వెడల్పు)) { }
		bool is_signal() స్థిరరాశి { తిరిగి _v.సూచీ() == 0; }
		bool is_జ్ఞాపకం() స్థిరరాశి { తిరిగి _v.సూచీ() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		పూర్ణాంకం వెడల్పు() స్థిరరాశి { తిరిగి std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		పూర్ణాంకం addr_వెడల్పు() స్థిరరాశి { తిరిగి std::get<1>(_v).మొదటి; }
		// returns the data width of a bitvector sort, errors out for other sorts
		పూర్ణాంకం దత్తాంశం_వెడల్పు() స్థిరరాశి { తిరిగి std::get<1>(_v).రెండవ; }
		bool క్రియాకారకం==(క్రమీకరించు_2 స్థిరరాశి& other) స్థిరరాశి { తిరిగి _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి { h.eat(_v); తిరిగి h; }
	};
	class IR;
	class Factory;
	class కణుపు_2;
	class IRInput {
		మిత్ర class Factory;
	బహిరంగ:
		PooledName name;
		PooledName రకం;
		క్రమీకరించు_2 క్రమీకరించు;
	అంతరంగ:
		IRInput(IR &ir, IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు);
	};
	class IROutput {
		మిత్ర class Factory;
		IR &_ir;
	బహిరంగ:
		PooledName name;
		PooledName రకం;
		క్రమీకరించు_2 క్రమీకరించు;
	అంతరంగ:
		IROutput(IR &ir, IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు);
	బహిరంగ:
		కణుపు_2 విలువ() స్థిరరాశి;
		bool has_విలువ() స్థిరరాశి;
		శూన్య సమితి_విలువ(కణుపు_2 విలువ);
	};
	class IRState {
		మిత్ర class Factory;
		IR &_ir;
	బహిరంగ:
		PooledName name;
		PooledName రకం;
		క్రమీకరించు_2 క్రమీకరించు;
	అంతరంగ:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు);
	బహిరంగ:
		కణుపు_2 తదుపరి_విలువ() స్థిరరాశి;
		bool has_తదుపరి_విలువ() స్థిరరాశి;
		RTLIL::Const స్థిరరాశి& initial_విలువ_signal() స్థిరరాశి { తిరిగి std::get<RTLIL::Const>(_initial); }
		MemContents స్థిరరాశి& initial_విలువ_జ్ఞాపకం() స్థిరరాశి { తిరిగి std::get<MemContents>(_initial); }
		శూన్య సమితి_తదుపరి_విలువ(కణుపు_2 విలువ);
		శూన్య సమితి_initial_విలువ(RTLIL::Const విలువ) { విలువ.extu(క్రమీకరించు.వెడల్పు()); _initial = std::move(విలువ); }
		శూన్య సమితి_initial_విలువ(MemContents విలువ) { log_assert(క్రమీకరించు_2(విలువ.addr_వెడల్పు(), విలువ.దత్తాంశం_వెడల్పు()) == క్రమీకరించు); _initial = std::move(విలువ); }
	};
	class IR {
		మిత్ర class Factory;
		మిత్ర class కణుపు_2;
		మిత్ర class IRInput;
		మిత్ర class IROutput;
		మిత్ర class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class కణుపు_దత్తాంశం {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				పూర్ణాంకం
			> _extra;
		బహిరంగ:
			కణుపు_దత్తాంశం() : _fn(Fn::invalid) {}
			కణుపు_దత్తాంశం(Fn fn) : _fn(fn) {}
			మూస<class T> కణుపు_దత్తాంశం(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() స్థిరరాశి { తిరిగి _fn; }
			స్థిరరాశి RTLIL::Const &as_const() స్థిరరాశి { తిరిగి std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() స్థిరరాశి { తిరిగి std::get<std::pair<IdString, IdString>>(_extra); }
			పూర్ణాంకం as_int() స్థిరరాశి { తిరిగి std::get<పూర్ణాంకం>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) స్థిరరాశి {
				h.eat((గుర్తులేని పూర్ణాంకం) _fn);
				h.eat(_extra);
				తిరిగి h;
			}
			bool క్రియాకారకం==(కణుపు_దత్తాంశం స్థిరరాశి &other) స్థిరరాశి {
				తిరిగి _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		నిర్మాణం Attr {
			క్రమీకరించు_2 క్రమీకరించు;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using లేఖాచిత్రం = Compute_లేఖాచిత్రం<కణుపు_దత్తాంశం, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		లేఖాచిత్రం లేఖాచిత్రం_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> స్థితులు_2;
		IR::లేఖాచిత్రం::Ref mutate(కణుపు_2 n);
	బహిరంగ:
		Design *design = nullptr;
		స్థిర IR from_module(Module *module);
		Factory factory();
		పూర్ణాంకం కొలత() స్థిరరాశి { తిరిగి లేఖాచిత్రం_4.కొలత(); }
		కణుపు_2 క్రియాకారకం[](పూర్ణాంకం i);
		శూన్య topological_క్రమీకరించు();
		శూన్య forward_buf();
		IRInput స్థిరరాశి& ఆదానం(IdString name, IdString రకం) స్థిరరాశి { తిరిగి _inputs.at({name, రకం}); }
		IRInput స్థిరరాశి& ఆదానం(IdString name) స్థిరరాశి { తిరిగి ఆదానం(name, ID($ఆదానం)); }
		IROutput స్థిరరాశి& output(IdString name, IdString రకం) స్థిరరాశి { తిరిగి _outputs.at({name, రకం}); }
		IROutput స్థిరరాశి& output(IdString name) స్థిరరాశి { తిరిగి output(name, ID($output)); }
		IRState స్థిరరాశి& state(IdString name, IdString రకం) స్థిరరాశి { తిరిగి స్థితులు_2.at({name, రకం}); }
		IRState స్థిరరాశి& state(IdString name) స్థిరరాశి { తిరిగి state(name, ID($state)); }
		bool has_ఆదానం(IdString name, IdString రకం) స్థిరరాశి { తిరిగి _inputs.లెక్క({name, రకం}); }
		bool has_output(IdString name, IdString రకం) స్థిరరాశి { తిరిగి _outputs.లెక్క({name, రకం}); }
		bool has_state(IdString name, IdString రకం) స్థిరరాశి { తిరిగి స్థితులు_2.లెక్క({name, రకం}); }
		సదిశ<IRInput స్థిరరాశి*> inputs(IdString రకం) స్థిరరాశి;
		సదిశ<IRInput స్థిరరాశి*> inputs() స్థిరరాశి { తిరిగి inputs(ID($ఆదానం)); }
		సదిశ<IROutput స్థిరరాశి*> outputs(IdString రకం) స్థిరరాశి;
		సదిశ<IROutput స్థిరరాశి*> outputs() స్థిరరాశి { తిరిగి outputs(ID($output)); }
		సదిశ<IRState స్థిరరాశి*> స్థితులు(IdString రకం) స్థిరరాశి;
		సదిశ<IRState స్థిరరాశి*> స్థితులు() స్థిరరాశి { తిరిగి స్థితులు(ID($state)); }
		సదిశ<IRInput స్థిరరాశి*> all_inputs() స్థిరరాశి;
		సదిశ<IROutput స్థిరరాశి*> all_outputs() స్థిరరాశి;
		సదిశ<IRState స్థిరరాశి*> all_స్థితులు() స్థిరరాశి;
		class iterator {
			మిత్ర class IR;
			IR *_ir;
			పూర్ణాంకం సూచీ_3;
			iterator(IR *ir, పూర్ణాంకం సూచీ) : _ir(ir), సూచీ_3(సూచీ) {}
		బహిరంగ:
			using iterator_category = std::ఆదానం_iterator_ట్యాగ్;
			using విలువ_type = కణుపు_2;
			using సూచిక = arrow_proxy<కణుపు_2>;
			using reference = కణుపు_2;
			using difference_type = ptrdiff_t;
			కణుపు_2 క్రియాకారకం*();
			iterator &క్రియాకారకం++() { సూచీ_3++; తిరిగి *this; }
			bool క్రియాకారకం!=(iterator స్థిరరాశి &other) స్థిరరాశి { తిరిగి _ir != other._ir || సూచీ_3 != other.సూచీ_3; }
			bool క్రియాకారకం==(iterator స్థిరరాశి &other) స్థిరరాశి { తిరిగి !(*this != other); }
			సూచిక క్రియాకారకం->();
		};
		iterator begin() { తిరిగి iterator(this, 0); }
		iterator చివర() { తిరిగి iterator(this, లేఖాచిత్రం_4.కొలత()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class కణుపు_2 {
		మిత్ర class Factory;
		మిత్ర class IR;
		మిత్ర class IRInput;
		మిత్ర class IROutput;
		మిత్ర class IRState;
		IR::లేఖాచిత్రం::ConstRef _ref;
		కణుపు_2(IR::లేఖాచిత్రం::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit క్రియాకారకం IR::లేఖాచిత్రం::ConstRef() { తిరిగి _ref; }
	బహిరంగ:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		పూర్ణాంకం id() స్థిరరాశి { తిరిగి _ref.సూచీ(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() స్థిరరాశి {
			ఒకవేళ(_ref.has_sparse_attr())
				తిరిగి {design, _ref.sparse_attr()};
			లేకపోతే
				తిరిగి {design, design->twines().కూడు(std::string("\\n") + std::గమ్యం_string(id()))};
		}
		Fn fn() స్థిరరాశి { తిరిగి _ref.function().fn(); }
		క్రమీకరించు_2 క్రమీకరించు() స్థిరరాశి { తిరిగి _ref.attr().క్రమీకరించు; }
		// returns the width of a bitvector node, errors out for other nodes
		పూర్ణాంకం వెడల్పు() స్థిరరాశి { తిరిగి క్రమీకరించు().వెడల్పు(); }
		కొలత_t arg_లెక్క() స్థిరరాశి { తిరిగి _ref.కొలత(); }
		కణుపు_2 arg(పూర్ణాంకం n) స్థిరరాశి { తిరిగి కణుపు_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		మూస<class Visitor> స్వయంచాలక సందర్శించు(Visitor v) స్థిరరాశి
		{
			// currently templated but could be switched to AbstractVisitor &
			ఎంపిక(_ref.function().fn()) {
			స్థితి Fn::invalid: log_error("invalid node in visit\n"); ఆపు;
			స్థితి Fn::buf: తిరిగి v.buf(*this, arg(0)); ఆపు;
			స్థితి Fn::ముక్క: తిరిగి v.ముక్క(*this, arg(0), _ref.function().as_int(), క్రమీకరించు().వెడల్పు()); ఆపు;
			స్థితి Fn::zero_extend: తిరిగి v.zero_extend(*this, arg(0), వెడల్పు()); ఆపు;
			స్థితి Fn::sign_extend: తిరిగి v.sign_extend(*this, arg(0), వెడల్పు()); ఆపు;
			స్థితి Fn::concat: తిరిగి v.concat(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::కూడు: తిరిగి v.కూడు(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::sub: తిరిగి v.sub(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::mul: తిరిగి v.mul(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::unsigned_div: తిరిగి v.unsigned_div(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::unsigned_mod: తిరిగి v.unsigned_mod(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::bitwise_and: తిరిగి v.bitwise_and(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::bitwise_or: తిరిగి v.bitwise_or(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::bitwise_xor: తిరిగి v.bitwise_xor(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::bitwise_not: తిరిగి v.bitwise_not(*this, arg(0)); ఆపు;
			స్థితి Fn::unary_minus: తిరిగి v.unary_minus(*this, arg(0)); ఆపు;
			స్థితి Fn::reduce_and: తిరిగి v.reduce_and(*this, arg(0)); ఆపు;
			స్థితి Fn::reduce_or: తిరిగి v.reduce_or(*this, arg(0)); ఆపు;
			స్థితి Fn::reduce_xor: తిరిగి v.reduce_xor(*this, arg(0)); ఆపు;
			స్థితి Fn::equal: తిరిగి v.equal(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::not_equal: తిరిగి v.not_equal(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::signed_greater_than: తిరిగి v.signed_greater_than(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::signed_greater_equal: తిరిగి v.signed_greater_equal(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::unsigned_greater_than: తిరిగి v.unsigned_greater_than(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::unsigned_greater_equal: తిరిగి v.unsigned_greater_equal(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::logical_జరుపు_ఎడమ: తిరిగి v.logical_జరుపు_ఎడమ(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::logical_జరుపు_కుడి: తిరిగి v.logical_జరుపు_కుడి(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::arithmetic_జరుపు_కుడి: తిరిగి v.arithmetic_జరుపు_కుడి(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::mux: తిరిగి v.mux(*this, arg(0), arg(1), arg(2)); ఆపు;
			స్థితి Fn::constant: తిరిగి v.constant(*this, _ref.function().as_const()); ఆపు;
			స్థితి Fn::ఆదానం: తిరిగి v.ఆదానం(*this, _ref.function().as_idstring_pair().మొదటి, _ref.function().as_idstring_pair().రెండవ); ఆపు;
			స్థితి Fn::state: తిరిగి v.state(*this, _ref.function().as_idstring_pair().మొదటి, _ref.function().as_idstring_pair().రెండవ); ఆపు;
			స్థితి Fn::జ్ఞాపకం_read: తిరిగి v.జ్ఞాపకం_read(*this, arg(0), arg(1)); ఆపు;
			స్థితి Fn::జ్ఞాపకం_write: తిరిగి v.జ్ఞాపకం_write(*this, arg(0), arg(1), arg(2)); ఆపు;
			}
			log_abort();
		}
		std::string గమ్యం_string();
		std::string గమ్యం_string(std::function<std::string(కణుపు_2)>);
	};
	అంతఃవరుస IRInput::IRInput(IR &ir, IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు)
		: name(ir.design, name), రకం(ir.design, రకం), క్రమీకరించు(std::move(క్రమీకరించు)) {}
	అంతఃవరుస IROutput::IROutput(IR &ir, IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు)
		: _ir(ir), name(ir.design, name), రకం(ir.design, రకం), క్రమీకరించు(std::move(క్రమీకరించు)) {}
	అంతఃవరుస IRState::IRState(IR &ir, IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు)
		: _ir(ir), name(ir.design, name), రకం(ir.design, రకం), క్రమీకరించు(std::move(క్రమీకరించు)) {}
	అంతఃవరుస IR::లేఖాచిత్రం::Ref IR::mutate(కణుపు_2 n) { తిరిగి లేఖాచిత్రం_4[n._ref.సూచీ()]; }
	అంతఃవరుస కణుపు_2 IR::క్రియాకారకం[](పూర్ణాంకం i) { తిరిగి కణుపు_2(లేఖాచిత్రం_4[i], design); }
	అంతఃవరుస కణుపు_2 IROutput::విలువ() స్థిరరాశి { తిరిగి కణుపు_2(_ir.లేఖాచిత్రం_4({name, రకం, false}), _ir.design); }
	అంతఃవరుస bool IROutput::has_విలువ() స్థిరరాశి { తిరిగి _ir.లేఖాచిత్రం_4.has_కీలకం({name, రకం, false}); }
	అంతఃవరుస శూన్య IROutput::సమితి_విలువ(కణుపు_2 విలువ) { log_assert(క్రమీకరించు == విలువ.క్రమీకరించు()); _ir.mutate(విలువ).కేటాయించు_కీలకం({name, రకం, false}); }
	అంతఃవరుస కణుపు_2 IRState::తదుపరి_విలువ() స్థిరరాశి { తిరిగి కణుపు_2(_ir.లేఖాచిత్రం_4({name, రకం, true}), _ir.design); }
	అంతఃవరుస bool IRState::has_తదుపరి_విలువ() స్థిరరాశి { తిరిగి _ir.లేఖాచిత్రం_4.has_కీలకం({name, రకం, true}); }
	అంతఃవరుస శూన్య IRState::సమితి_తదుపరి_విలువ(కణుపు_2 విలువ) { log_assert(క్రమీకరించు == విలువ.క్రమీకరించు()); _ir.mutate(విలువ).కేటాయించు_కీలకం({name, రకం, true}); }
	అంతఃవరుస కణుపు_2 IR::iterator::క్రియాకారకం*() { తిరిగి కణుపు_2(_ir->లేఖాచిత్రం_4[సూచీ_3], _ir->design); }
	అంతఃవరుస arrow_proxy<కణుపు_2> IR::iterator::క్రియాకారకం->() { తిరిగి arrow_proxy<కణుపు_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	మూస<class T> నిర్మాణం AbstractVisitor {
		మాయా T buf(కణుపు_2 self, కణుపు_2 n) = 0;
		మాయా T ముక్క(కణుపు_2 self, కణుపు_2 a, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం బహిర్గతం_వెడల్పు) = 0;
		మాయా T zero_extend(కణుపు_2 self, కణుపు_2 a, పూర్ణాంకం బహిర్గతం_వెడల్పు) = 0;
		మాయా T sign_extend(కణుపు_2 self, కణుపు_2 a, పూర్ణాంకం బహిర్గతం_వెడల్పు) = 0;
		మాయా T concat(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T కూడు(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T sub(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T mul(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T unsigned_div(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T unsigned_mod(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T bitwise_and(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T bitwise_or(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T bitwise_xor(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T bitwise_not(కణుపు_2 self, కణుపు_2 a) = 0;
		మాయా T unary_minus(కణుపు_2 self, కణుపు_2 a) = 0;
		మాయా T reduce_and(కణుపు_2 self, కణుపు_2 a) = 0;
		మాయా T reduce_or(కణుపు_2 self, కణుపు_2 a) = 0;
		మాయా T reduce_xor(కణుపు_2 self, కణుపు_2 a) = 0;
		మాయా T equal(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T not_equal(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T signed_greater_than(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T signed_greater_equal(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T unsigned_greater_than(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T unsigned_greater_equal(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T logical_జరుపు_ఎడమ(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T logical_జరుపు_కుడి(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T arithmetic_జరుపు_కుడి(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b) = 0;
		మాయా T mux(కణుపు_2 self, కణుపు_2 a, కణుపు_2 b, కణుపు_2 s) = 0;
		మాయా T constant(కణుపు_2 self, RTLIL::Const స్థిరరాశి & విలువ) = 0;
		మాయా T ఆదానం(కణుపు_2 self, IdString name, IdString రకం) = 0;
		మాయా T state(కణుపు_2 self, IdString name, IdString రకం) = 0;
		మాయా T జ్ఞాపకం_read(కణుపు_2 self, కణుపు_2 mem, కణుపు_2 addr) = 0;
		మాయా T జ్ఞాపకం_write(కణుపు_2 self, కణుపు_2 mem, కణుపు_2 addr, కణుపు_2 దత్తాంశం) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	మూస<class T> నిర్మాణం DefaultVisitor : బహిరంగ AbstractVisitor<T> {
		మాయా T default_handler(కణుపు_2 self) = 0;
		T buf(కణుపు_2 self, కణుపు_2) override { తిరిగి default_handler(self); }
		T ముక్క(కణుపు_2 self, కణుపు_2, పూర్ణాంకం, పూర్ణాంకం) override { తిరిగి default_handler(self); }
		T zero_extend(కణుపు_2 self, కణుపు_2, పూర్ణాంకం) override { తిరిగి default_handler(self); }
		T sign_extend(కణుపు_2 self, కణుపు_2, పూర్ణాంకం) override { తిరిగి default_handler(self); }
		T concat(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T కూడు(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T sub(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T mul(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T unsigned_div(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T unsigned_mod(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T bitwise_and(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T bitwise_or(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T bitwise_xor(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T bitwise_not(కణుపు_2 self, కణుపు_2) override { తిరిగి default_handler(self); }
		T unary_minus(కణుపు_2 self, కణుపు_2) override { తిరిగి default_handler(self); }
		T reduce_and(కణుపు_2 self, కణుపు_2) override { తిరిగి default_handler(self); }
		T reduce_or(కణుపు_2 self, కణుపు_2) override { తిరిగి default_handler(self); }
		T reduce_xor(కణుపు_2 self, కణుపు_2) override { తిరిగి default_handler(self); }
		T equal(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T not_equal(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T signed_greater_than(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T signed_greater_equal(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T unsigned_greater_than(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T unsigned_greater_equal(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T logical_జరుపు_ఎడమ(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T logical_జరుపు_కుడి(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T arithmetic_జరుపు_కుడి(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T mux(కణుపు_2 self, కణుపు_2, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T constant(కణుపు_2 self, RTLIL::Const స్థిరరాశి &) override { తిరిగి default_handler(self); }
		T ఆదానం(కణుపు_2 self, IdString, IdString) override { తిరిగి default_handler(self); }
		T state(కణుపు_2 self, IdString, IdString) override { తిరిగి default_handler(self); }
		T జ్ఞాపకం_read(కణుపు_2 self, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
		T జ్ఞాపకం_write(కణుపు_2 self, కణుపు_2, కణుపు_2, కణుపు_2) override { తిరిగి default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		మిత్ర class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		కణుపు_2 కూడు(IR::కణుపు_దత్తాంశం &&fn, క్రమీకరించు_2 స్థిరరాశి &క్రమీకరించు, std::initializer_list<కణుపు_2> వాదనలు_2) {
			log_assert(!క్రమీకరించు.is_signal() || క్రమీకరించు.వెడల్పు() > 0);
			log_assert(!క్రమీకరించు.is_జ్ఞాపకం() || (క్రమీకరించు.addr_వెడల్పు() > 0 && క్రమీకరించు.దత్తాంశం_వెడల్పు() > 0));
			IR::లేఖాచిత్రం::Ref ref = _ir.లేఖాచిత్రం_4.కూడు(std::move(fn), {std::move(క్రమీకరించు)});
			కొరకు (స్వయంచాలక arg : వాదనలు_2)
				ref.append_arg(IR::లేఖాచిత్రం::ConstRef(arg));
			తిరిగి కణుపు_2(ref, _ir.design);
		}
		శూన్య check_basic_ద్విమాన(కణుపు_2 స్థిరరాశి &a, కణుపు_2 స్థిరరాశి &b) { log_assert(a.క్రమీకరించు().is_signal() && a.క్రమీకరించు() == b.క్రమీకరించు()); }
		శూన్య check_జరుపు(కణుపు_2 స్థిరరాశి &a, కణుపు_2 స్థిరరాశి &b) { log_assert(a.క్రమీకరించు().is_signal() && b.క్రమీకరించు().is_signal() && b.వెడల్పు() == ceil_log2(a.వెడల్పు())); }
		శూన్య check_unary(కణుపు_2 స్థిరరాశి &a) { log_assert(a.క్రమీకరించు().is_signal()); }
	బహిరంగ:
		IR &ir() { తిరిగి _ir; }
		కణుపు_2 ముక్క(కణుపు_2 a, పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం బహిర్గతం_వెడల్పు) {
			log_assert(a.క్రమీకరించు().is_signal() && స్థానభ్రంశం + బహిర్గతం_వెడల్పు <= a.క్రమీకరించు().వెడల్పు());
			ఒకవేళ(స్థానభ్రంశం == 0 && బహిర్గతం_వెడల్పు == a.వెడల్పు())
				తిరిగి a;
			తిరిగి కూడు(IR::కణుపు_దత్తాంశం(Fn::ముక్క, స్థానభ్రంశం), క్రమీకరించు_2(బహిర్గతం_వెడల్పు), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		కణుపు_2 extend(కణుపు_2 a, పూర్ణాంకం బహిర్గతం_వెడల్పు, bool is_signed) {
			పూర్ణాంకం లోపల_వెడల్పు = a.క్రమీకరించు().వెడల్పు();
			log_assert(a.క్రమీకరించు().is_signal());
			ఒకవేళ(లోపల_వెడల్పు == బహిర్గతం_వెడల్పు)
				తిరిగి a;
			ఒకవేళ(లోపల_వెడల్పు > బహిర్గతం_వెడల్పు)
				తిరిగి ముక్క(a, 0, బహిర్గతం_వెడల్పు);
			ఒకవేళ(is_signed)
				తిరిగి కూడు(Fn::sign_extend, క్రమీకరించు_2(బహిర్గతం_వెడల్పు), {a});
			లేకపోతే
				తిరిగి కూడు(Fn::zero_extend, క్రమీకరించు_2(బహిర్గతం_వెడల్పు), {a});
		}
		కణుపు_2 concat(కణుపు_2 a, కణుపు_2 b) {
			log_assert(a.క్రమీకరించు().is_signal() && b.క్రమీకరించు().is_signal());
			తిరిగి కూడు(Fn::concat, క్రమీకరించు_2(a.క్రమీకరించు().వెడల్పు() + b.క్రమీకరించు().వెడల్పు()), {a, b});
		}
		కణుపు_2 కూడు(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::కూడు, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 sub(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::sub, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 mul(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::mul, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 unsigned_div(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::unsigned_div, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 unsigned_mod(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::unsigned_mod, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 bitwise_and(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::bitwise_and, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 bitwise_or(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::bitwise_or, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 bitwise_xor(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::bitwise_xor, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 bitwise_not(కణుపు_2 a) { check_unary(a); తిరిగి కూడు(Fn::bitwise_not, a.క్రమీకరించు(), {a}); }
		కణుపు_2 unary_minus(కణుపు_2 a) { check_unary(a); తిరిగి కూడు(Fn::unary_minus, a.క్రమీకరించు(), {a}); }
		కణుపు_2 reduce_and(కణుపు_2 a) {
			check_unary(a);
			ఒకవేళ(a.వెడల్పు() == 1)
				తిరిగి a;
			తిరిగి కూడు(Fn::reduce_and, క్రమీకరించు_2(1), {a});
		}
		కణుపు_2 reduce_or(కణుపు_2 a) {
			check_unary(a);
			ఒకవేళ(a.వెడల్పు() == 1)
				తిరిగి a;
			తిరిగి కూడు(Fn::reduce_or, క్రమీకరించు_2(1), {a});
		}
		కణుపు_2 reduce_xor(కణుపు_2 a) {
			check_unary(a);
			ఒకవేళ(a.వెడల్పు() == 1)
				తిరిగి a;
			తిరిగి కూడు(Fn::reduce_xor, క్రమీకరించు_2(1), {a});
		}
		కణుపు_2 equal(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::equal, క్రమీకరించు_2(1), {a, b}); }
		కణుపు_2 not_equal(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::not_equal, క్రమీకరించు_2(1), {a, b}); }
		కణుపు_2 signed_greater_than(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::signed_greater_than, క్రమీకరించు_2(1), {a, b}); }
		కణుపు_2 signed_greater_equal(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::signed_greater_equal, క్రమీకరించు_2(1), {a, b}); }
		కణుపు_2 unsigned_greater_than(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::unsigned_greater_than, క్రమీకరించు_2(1), {a, b}); }
		కణుపు_2 unsigned_greater_equal(కణుపు_2 a, కణుపు_2 b) { check_basic_ద్విమాన(a, b); తిరిగి కూడు(Fn::unsigned_greater_equal, క్రమీకరించు_2(1), {a, b}); }
		కణుపు_2 logical_జరుపు_ఎడమ(కణుపు_2 a, కణుపు_2 b) { check_జరుపు(a, b); తిరిగి కూడు(Fn::logical_జరుపు_ఎడమ, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 logical_జరుపు_కుడి(కణుపు_2 a, కణుపు_2 b) { check_జరుపు(a, b); తిరిగి కూడు(Fn::logical_జరుపు_కుడి, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 arithmetic_జరుపు_కుడి(కణుపు_2 a, కణుపు_2 b) { check_జరుపు(a, b); తిరిగి కూడు(Fn::arithmetic_జరుపు_కుడి, a.క్రమీకరించు(), {a, b}); }
		కణుపు_2 mux(కణుపు_2 a, కణుపు_2 b, కణుపు_2 s) {
			log_assert(a.క్రమీకరించు().is_signal() && a.క్రమీకరించు() == b.క్రమీకరించు() && s.క్రమీకరించు() == క్రమీకరించు_2(1));
			తిరిగి కూడు(Fn::mux, a.క్రమీకరించు(), {a, b, s});
		}
		కణుపు_2 జ్ఞాపకం_read(కణుపు_2 mem, కణుపు_2 addr) {
			log_assert(mem.క్రమీకరించు().is_జ్ఞాపకం() && addr.క్రమీకరించు().is_signal() && mem.క్రమీకరించు().addr_వెడల్పు() == addr.క్రమీకరించు().వెడల్పు());
			తిరిగి కూడు(Fn::జ్ఞాపకం_read, క్రమీకరించు_2(mem.క్రమీకరించు().దత్తాంశం_వెడల్పు()), {mem, addr});
		}
		కణుపు_2 జ్ఞాపకం_write(కణుపు_2 mem, కణుపు_2 addr, కణుపు_2 దత్తాంశం) {
			log_assert(mem.క్రమీకరించు().is_జ్ఞాపకం() && addr.క్రమీకరించు().is_signal() && దత్తాంశం.క్రమీకరించు().is_signal() &&
				mem.క్రమీకరించు().addr_వెడల్పు() == addr.క్రమీకరించు().వెడల్పు() && mem.క్రమీకరించు().దత్తాంశం_వెడల్పు() == దత్తాంశం.క్రమీకరించు().వెడల్పు());
			తిరిగి కూడు(Fn::జ్ఞాపకం_write, mem.క్రమీకరించు(), {mem, addr, దత్తాంశం});
		}
		కణుపు_2 constant(RTLIL::Const విలువ) {
			పూర్ణాంకం s = విలువ.కొలత();
			తిరిగి కూడు(IR::కణుపు_దత్తాంశం(Fn::constant, std::move(విలువ)), క్రమీకరించు_2(s), {});
		}
		కణుపు_2 create_pending(పూర్ణాంకం వెడల్పు) {
			తిరిగి కూడు(Fn::buf, క్రమీకరించు_2(వెడల్పు), {});
		}
		శూన్య update_pending(కణుపు_2 కణుపు, కణుపు_2 విలువ) {
			log_assert(కణుపు._ref.function() == Fn::buf && కణుపు._ref.కొలత() == 0);
			log_assert(కణుపు.క్రమీకరించు() == విలువ.క్రమీకరించు());
			_ir.mutate(కణుపు).append_arg(విలువ._ref);
		}
		IRInput &కూడు_ఆదానం(IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు) {
			స్వయంచాలక [it, inserted] = _ir._inputs.emplace({name, రకం}, IRInput(_ir, name, రకం, std::move(క్రమీకరించు)));
			ఒకవేళ (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			తిరిగి it->రెండవ;
		}
		IROutput &కూడు_output(IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు) {
			స్వయంచాలక [it, inserted] = _ir._outputs.emplace({name, రకం}, IROutput(_ir, name, రకం, std::move(క్రమీకరించు)));
			ఒకవేళ (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			తిరిగి it->రెండవ;
		}
		IRState &కూడు_state(IdString name, IdString రకం, క్రమీకరించు_2 క్రమీకరించు) {
			స్వయంచాలక [it, inserted] = _ir.స్థితులు_2.emplace({name, రకం}, IRState(_ir, name, రకం, std::move(క్రమీకరించు)));
			ఒకవేళ (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			తిరిగి it->రెండవ;
		}
		కణుపు_2 విలువ(IRInput స్థిరరాశి& ఆదానం) {
			తిరిగి కూడు(IR::కణుపు_దత్తాంశం(Fn::ఆదానం, std::pair(ఆదానం.name, ఆదానం.రకం)), ఆదానం.క్రమీకరించు, {});
		}
		కణుపు_2 విలువ(IRState స్థిరరాశి& state) {
			తిరిగి కూడు(IR::కణుపు_దత్తాంశం(Fn::state, std::pair(state.name, state.రకం)), state.క్రమీకరించు, {});
		}
		శూన్య suggest_name(కణుపు_2 కణుపు, IdString name) {
			_ir.mutate(కణుపు).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	అంతఃవరుస Factory IR::factory() { తిరిగి Factory(*this); }
	మూస<class Id> class Scope {
	రక్షిత:
		అక్షరం substitution_character = '_';
		మాయా bool is_character_legal(అక్షరం, పూర్ణాంకం) = 0;
	అంతరంగ:
		pool<std::string> వాడిన_పేర్లు;
		dict<Id, std::string> _by_id;
	బహిరంగ:
		శూన్య reserve(std::string name) {
			వాడిన_పేర్లు.చొప్పించు(std::move(name));
		}
		మూస<typename N> std::string unique_name(స్థిరరాశి N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			కొరకు(కొలత_t i = 0; i < str.కొలత(); i++)
				ఒకవేళ(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			ఒకవేళ(వాడిన_పేర్లు.లెక్క(str) == 0) {
				వాడిన_పేర్లు.చొప్పించు(str);
				తిరిగి str;
			}
			కొరకు (పూర్ణాంకం idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::గమ్యం_string(idx);
				ఒకవేళ(వాడిన_పేర్లు.లెక్క(suffixed) == 0) {
					వాడిన_పేర్లు.చొప్పించు(suffixed);
					తిరిగి suffixed;
				}
			}
		}
		మూస<typename N> std::string క్రియాకారకం()(Id id, స్థిరరాశి N &suggestion) {
			స్వయంచాలక it = _by_id.కనుగొను(id);
			ఒకవేళ(it != _by_id.చివర())
				తిరిగి it->రెండవ;
			std::string str = unique_name(suggestion);
			_by_id.చొప్పించు({id, str});
			తిరిగి str;
		}
	};
	class Writer {
		std::ostream *os;
		శూన్య print_impl(స్థిరరాశి అక్షరం *fmt, సదిశ<std::function<శూన్య()>>& fns);
	బహిరంగ:
		Writer(std::ostream &os) : os(&os) {}
		మూస<class T> Writer& క్రియాకారకం <<(T&& arg) { *os << std::forward<T>(arg); తిరిగి *this; }
		మూస<typename... వాదనలు_3>
		శూన్య print(స్థిరరాశి అక్షరం *fmt, వాదనలు_3&&... వాదనలు_2)
		{
			సదిశ<std::function<శూన్య()>> fns { [&]() { *this << వాదనలు_2; }... };
			print_impl(fmt, fns);
		}
		మూస<typename Fn, typename... వాదనలు_3>
		శూన్య print_with(Fn fn, స్థిరరాశి అక్షరం *fmt, వాదనలు_3&&... వాదనలు_2)
		{
			సదిశ<std::function<శూన్య()>> fns { [&]() {
				ఒకవేళ constexpr (std::is_invocable_v<Fn, వాదనలు_3>)
					*this << fn(వాదనలు_2);
				లేకపోతే
					*this << వాదనలు_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
