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

#अगर_अघोषित FUNCTIONAL_H
#घोषणा FUNCTIONAL_H

#समावेश "kernel/yosys.h"
#समावेश "kernel/compute_graph.h"
#समावेश "kernel/drivertools.h"
#समावेश "kernel/mem.h"
#समावेश "kernel/utils.h"

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
	क्रमागत class Fn {
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
		logical_shift_बायाँ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_दायाँ,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_दायाँ,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		निवेश,
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
	स्थिर अक्षर *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<पूर्णांक, std::pair<पूर्णांक, पूर्णांक>> _v;
	खुला:
		explicit Sort(पूर्णांक width) : _v(width) { }
		Sort(पूर्णांक addr_width, पूर्णांक तथ्य_width) : _v(std::make_pair(addr_width, तथ्य_width)) { }
		bool is_signal() स्थिर { वापस _v.सूचकांक() == 0; }
		bool is_memory() स्थिर { वापस _v.सूचकांक() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		पूर्णांक width() स्थिर { वापस std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		पूर्णांक addr_width() स्थिर { वापस std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		पूर्णांक तथ्य_width() स्थिर { वापस std::get<1>(_v).second; }
		bool चालक==(Sort स्थिर& other) स्थिर { वापस _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) स्थिर { h.eat(_v); वापस h; }
	};
	class IR;
	class Factory;
	class गाँठ_2;
	class IRInput {
		मित्र class Factory;
	खुला:
		PooledName name;
		PooledName kind;
		Sort sort;
	निजी:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		मित्र class Factory;
		IR &_ir;
	खुला:
		PooledName name;
		PooledName kind;
		Sort sort;
	निजी:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	खुला:
		गाँठ_2 मूल्य() स्थिर;
		bool has_मूल्य() स्थिर;
		व्योम set_मूल्य(गाँठ_2 मूल्य);
	};
	class IRState {
		मित्र class Factory;
		IR &_ir;
	खुला:
		PooledName name;
		PooledName kind;
		Sort sort;
	निजी:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	खुला:
		गाँठ_2 अग्र_मूल्य() स्थिर;
		bool has_अग्र_मूल्य() स्थिर;
		RTLIL::Const स्थिर& initial_मूल्य_signal() स्थिर { वापस std::get<RTLIL::Const>(_initial); }
		MemContents स्थिर& initial_मूल्य_memory() स्थिर { वापस std::get<MemContents>(_initial); }
		व्योम set_अग्र_मूल्य(गाँठ_2 मूल्य);
		व्योम set_initial_मूल्य(RTLIL::Const मूल्य) { मूल्य.extu(sort.width()); _initial = std::move(मूल्य); }
		व्योम set_initial_मूल्य(MemContents मूल्य) { log_assert(Sort(मूल्य.addr_width(), मूल्य.तथ्य_width()) == sort); _initial = std::move(मूल्य); }
	};
	class IR {
		मित्र class Factory;
		मित्र class गाँठ_2;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class गाँठ_तथ्य {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				पूर्णांक
			> _extra;
		खुला:
			गाँठ_तथ्य() : _fn(Fn::invalid) {}
			गाँठ_तथ्य(Fn fn) : _fn(fn) {}
			ढाँचा<class T> गाँठ_तथ्य(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() स्थिर { वापस _fn; }
			स्थिर RTLIL::Const &as_const() स्थिर { वापस std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() स्थिर { वापस std::get<std::pair<IdString, IdString>>(_extra); }
			पूर्णांक as_int() स्थिर { वापस std::get<पूर्णांक>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) स्थिर {
				h.eat((अचिन्हित पूर्णांक) _fn);
				h.eat(_extra);
				वापस h;
			}
			bool चालक==(गाँठ_तथ्य स्थिर &other) स्थिर {
				वापस _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		काष्ठा Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<गाँठ_तथ्य, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(गाँठ_2 n);
	खुला:
		Design *design = nullptr;
		अटल IR from_module(Module *module);
		Factory factory();
		पूर्णांक आकार() स्थिर { वापस _graph.आकार(); }
		गाँठ_2 चालक[](पूर्णांक i);
		व्योम topological_sort();
		व्योम forward_buf();
		IRInput स्थिर& निवेश(IdString name, IdString kind) स्थिर { वापस _inputs.at({name, kind}); }
		IRInput स्थिर& निवेश(IdString name) स्थिर { वापस निवेश(name, ID($निवेश)); }
		IROutput स्थिर& output(IdString name, IdString kind) स्थिर { वापस _outputs.at({name, kind}); }
		IROutput स्थिर& output(IdString name) स्थिर { वापस output(name, ID($output)); }
		IRState स्थिर& state(IdString name, IdString kind) स्थिर { वापस _states.at({name, kind}); }
		IRState स्थिर& state(IdString name) स्थिर { वापस state(name, ID($state)); }
		bool has_निवेश(IdString name, IdString kind) स्थिर { वापस _inputs.गिनती({name, kind}); }
		bool has_output(IdString name, IdString kind) स्थिर { वापस _outputs.गिनती({name, kind}); }
		bool has_state(IdString name, IdString kind) स्थिर { वापस _states.गिनती({name, kind}); }
		vector<IRInput स्थिर*> inputs(IdString kind) स्थिर;
		vector<IRInput स्थिर*> inputs() स्थिर { वापस inputs(ID($निवेश)); }
		vector<IROutput स्थिर*> outputs(IdString kind) स्थिर;
		vector<IROutput स्थिर*> outputs() स्थिर { वापस outputs(ID($output)); }
		vector<IRState स्थिर*> states(IdString kind) स्थिर;
		vector<IRState स्थिर*> states() स्थिर { वापस states(ID($state)); }
		vector<IRInput स्थिर*> all_inputs() स्थिर;
		vector<IROutput स्थिर*> all_outputs() स्थिर;
		vector<IRState स्थिर*> all_states() स्थिर;
		class iterator {
			मित्र class IR;
			IR *_ir;
			पूर्णांक सूचकांक_3;
			iterator(IR *ir, पूर्णांक सूचकांक) : _ir(ir), सूचकांक_3(सूचकांक) {}
		खुला:
			using iterator_category = std::निवेश_iterator_tag;
			using मूल्य_type = गाँठ_2;
			using pointer = arrow_proxy<गाँठ_2>;
			using reference = गाँठ_2;
			using difference_type = सूचक_भेद_प्रकार;
			गाँठ_2 चालक*();
			iterator &चालक++() { सूचकांक_3++; वापस *this; }
			bool चालक!=(iterator स्थिर &other) स्थिर { वापस _ir != other._ir || सूचकांक_3 != other.सूचकांक_3; }
			bool चालक==(iterator स्थिर &other) स्थिर { वापस !(*this != other); }
			pointer चालक->();
		};
		iterator begin() { वापस iterator(this, 0); }
		iterator end() { वापस iterator(this, _graph.आकार()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class गाँठ_2 {
		मित्र class Factory;
		मित्र class IR;
		मित्र class IRInput;
		मित्र class IROutput;
		मित्र class IRState;
		IR::Graph::ConstRef _ref;
		गाँठ_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit चालक IR::Graph::ConstRef() { वापस _ref; }
	खुला:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		पूर्णांक id() स्थिर { वापस _ref.सूचकांक(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() स्थिर {
			अगर(_ref.has_sparse_attr())
				वापस {design, _ref.sparse_attr()};
			अन्यथा
				वापस {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() स्थिर { वापस _ref.function().fn(); }
		Sort sort() स्थिर { वापस _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		पूर्णांक width() स्थिर { वापस sort().width(); }
		माप_प्रकार arg_गिनती() स्थिर { वापस _ref.आकार(); }
		गाँठ_2 arg(पूर्णांक n) स्थिर { वापस गाँठ_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		ढाँचा<class Visitor> स्वतः visit(Visitor v) स्थिर
		{
			// currently templated but could be switched to AbstractVisitor &
			चयन(_ref.function().fn()) {
			हाल Fn::invalid: log_error("invalid node in visit\n"); अवरोध;
			हाल Fn::buf: वापस v.buf(*this, arg(0)); अवरोध;
			हाल Fn::slice: वापस v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); अवरोध;
			हाल Fn::zero_extend: वापस v.zero_extend(*this, arg(0), width()); अवरोध;
			हाल Fn::sign_extend: वापस v.sign_extend(*this, arg(0), width()); अवरोध;
			हाल Fn::concat: वापस v.concat(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::add: वापस v.add(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::sub: वापस v.sub(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::mul: वापस v.mul(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::unsigned_div: वापस v.unsigned_div(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::unsigned_mod: वापस v.unsigned_mod(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::bitwise_and: वापस v.bitwise_and(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::bitwise_or: वापस v.bitwise_or(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::bitwise_xor: वापस v.bitwise_xor(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::bitwise_not: वापस v.bitwise_not(*this, arg(0)); अवरोध;
			हाल Fn::unary_minus: वापस v.unary_minus(*this, arg(0)); अवरोध;
			हाल Fn::reduce_and: वापस v.reduce_and(*this, arg(0)); अवरोध;
			हाल Fn::reduce_or: वापस v.reduce_or(*this, arg(0)); अवरोध;
			हाल Fn::reduce_xor: वापस v.reduce_xor(*this, arg(0)); अवरोध;
			हाल Fn::equal: वापस v.equal(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::not_equal: वापस v.not_equal(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::signed_greater_than: वापस v.signed_greater_than(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::signed_greater_equal: वापस v.signed_greater_equal(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::unsigned_greater_than: वापस v.unsigned_greater_than(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::unsigned_greater_equal: वापस v.unsigned_greater_equal(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::logical_shift_बायाँ: वापस v.logical_shift_बायाँ(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::logical_shift_दायाँ: वापस v.logical_shift_दायाँ(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::arithmetic_shift_दायाँ: वापस v.arithmetic_shift_दायाँ(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::mux: वापस v.mux(*this, arg(0), arg(1), arg(2)); अवरोध;
			हाल Fn::constant: वापस v.constant(*this, _ref.function().as_const()); अवरोध;
			हाल Fn::निवेश: वापस v.निवेश(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); अवरोध;
			हाल Fn::state: वापस v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); अवरोध;
			हाल Fn::memory_read: वापस v.memory_read(*this, arg(0), arg(1)); अवरोध;
			हाल Fn::memory_write: वापस v.memory_write(*this, arg(0), arg(1), arg(2)); अवरोध;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(गाँठ_2)>);
	};
	अंतरभूत IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अंतरभूत IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अंतरभूत IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	अंतरभूत IR::Graph::Ref IR::mutate(गाँठ_2 n) { वापस _graph[n._ref.सूचकांक()]; }
	अंतरभूत गाँठ_2 IR::चालक[](पूर्णांक i) { वापस गाँठ_2(_graph[i], design); }
	अंतरभूत गाँठ_2 IROutput::मूल्य() स्थिर { वापस गाँठ_2(_ir._graph({name, kind, false}), _ir.design); }
	अंतरभूत bool IROutput::has_मूल्य() स्थिर { वापस _ir._graph.has_कुंजी({name, kind, false}); }
	अंतरभूत व्योम IROutput::set_मूल्य(गाँठ_2 मूल्य) { log_assert(sort == मूल्य.sort()); _ir.mutate(मूल्य).assign_कुंजी({name, kind, false}); }
	अंतरभूत गाँठ_2 IRState::अग्र_मूल्य() स्थिर { वापस गाँठ_2(_ir._graph({name, kind, true}), _ir.design); }
	अंतरभूत bool IRState::has_अग्र_मूल्य() स्थिर { वापस _ir._graph.has_कुंजी({name, kind, true}); }
	अंतरभूत व्योम IRState::set_अग्र_मूल्य(गाँठ_2 मूल्य) { log_assert(sort == मूल्य.sort()); _ir.mutate(मूल्य).assign_कुंजी({name, kind, true}); }
	अंतरभूत गाँठ_2 IR::iterator::चालक*() { वापस गाँठ_2(_ir->_graph[सूचकांक_3], _ir->design); }
	अंतरभूत arrow_proxy<गाँठ_2> IR::iterator::चालक->() { वापस arrow_proxy<गाँठ_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	ढाँचा<class T> काष्ठा AbstractVisitor {
		भव T buf(गाँठ_2 self, गाँठ_2 n) = 0;
		भव T slice(गाँठ_2 self, गाँठ_2 a, पूर्णांक offset, पूर्णांक out_width) = 0;
		भव T zero_extend(गाँठ_2 self, गाँठ_2 a, पूर्णांक out_width) = 0;
		भव T sign_extend(गाँठ_2 self, गाँठ_2 a, पूर्णांक out_width) = 0;
		भव T concat(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T add(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T sub(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T mul(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T unsigned_div(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T unsigned_mod(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T bitwise_and(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T bitwise_or(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T bitwise_xor(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T bitwise_not(गाँठ_2 self, गाँठ_2 a) = 0;
		भव T unary_minus(गाँठ_2 self, गाँठ_2 a) = 0;
		भव T reduce_and(गाँठ_2 self, गाँठ_2 a) = 0;
		भव T reduce_or(गाँठ_2 self, गाँठ_2 a) = 0;
		भव T reduce_xor(गाँठ_2 self, गाँठ_2 a) = 0;
		भव T equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T not_equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T signed_greater_than(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T signed_greater_equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T unsigned_greater_than(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T unsigned_greater_equal(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T logical_shift_बायाँ(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T logical_shift_दायाँ(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T arithmetic_shift_दायाँ(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b) = 0;
		भव T mux(गाँठ_2 self, गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) = 0;
		भव T constant(गाँठ_2 self, RTLIL::Const स्थिर & मूल्य) = 0;
		भव T निवेश(गाँठ_2 self, IdString name, IdString kind) = 0;
		भव T state(गाँठ_2 self, IdString name, IdString kind) = 0;
		भव T memory_read(गाँठ_2 self, गाँठ_2 mem, गाँठ_2 addr) = 0;
		भव T memory_write(गाँठ_2 self, गाँठ_2 mem, गाँठ_2 addr, गाँठ_2 तथ्य) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	ढाँचा<class T> काष्ठा DefaultVisitor : खुला AbstractVisitor<T> {
		भव T default_handler(गाँठ_2 self) = 0;
		T buf(गाँठ_2 self, गाँठ_2) override { वापस default_handler(self); }
		T slice(गाँठ_2 self, गाँठ_2, पूर्णांक, पूर्णांक) override { वापस default_handler(self); }
		T zero_extend(गाँठ_2 self, गाँठ_2, पूर्णांक) override { वापस default_handler(self); }
		T sign_extend(गाँठ_2 self, गाँठ_2, पूर्णांक) override { वापस default_handler(self); }
		T concat(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T add(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T sub(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T mul(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T unsigned_div(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T unsigned_mod(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T bitwise_and(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T bitwise_or(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T bitwise_xor(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T bitwise_not(गाँठ_2 self, गाँठ_2) override { वापस default_handler(self); }
		T unary_minus(गाँठ_2 self, गाँठ_2) override { वापस default_handler(self); }
		T reduce_and(गाँठ_2 self, गाँठ_2) override { वापस default_handler(self); }
		T reduce_or(गाँठ_2 self, गाँठ_2) override { वापस default_handler(self); }
		T reduce_xor(गाँठ_2 self, गाँठ_2) override { वापस default_handler(self); }
		T equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T not_equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T signed_greater_than(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T signed_greater_equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T unsigned_greater_than(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T unsigned_greater_equal(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T logical_shift_बायाँ(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T logical_shift_दायाँ(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T arithmetic_shift_दायाँ(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T mux(गाँठ_2 self, गाँठ_2, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T constant(गाँठ_2 self, RTLIL::Const स्थिर &) override { वापस default_handler(self); }
		T निवेश(गाँठ_2 self, IdString, IdString) override { वापस default_handler(self); }
		T state(गाँठ_2 self, IdString, IdString) override { वापस default_handler(self); }
		T memory_read(गाँठ_2 self, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
		T memory_write(गाँठ_2 self, गाँठ_2, गाँठ_2, गाँठ_2) override { वापस default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		मित्र class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		गाँठ_2 add(IR::गाँठ_तथ्य &&fn, Sort स्थिर &sort, std::initializer_list<गाँठ_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.तथ्य_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			क्रम (स्वतः arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			वापस गाँठ_2(ref, _ir.design);
		}
		व्योम check_basic_द्विआधारी(गाँठ_2 स्थिर &a, गाँठ_2 स्थिर &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		व्योम check_shift(गाँठ_2 स्थिर &a, गाँठ_2 स्थिर &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		व्योम check_unary(गाँठ_2 स्थिर &a) { log_assert(a.sort().is_signal()); }
	खुला:
		IR &ir() { वापस _ir; }
		गाँठ_2 slice(गाँठ_2 a, पूर्णांक offset, पूर्णांक out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			अगर(offset == 0 && out_width == a.width())
				वापस a;
			वापस add(IR::गाँठ_तथ्य(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		गाँठ_2 extend(गाँठ_2 a, पूर्णांक out_width, bool is_signed) {
			पूर्णांक in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			अगर(in_width == out_width)
				वापस a;
			अगर(in_width > out_width)
				वापस slice(a, 0, out_width);
			अगर(is_signed)
				वापस add(Fn::sign_extend, Sort(out_width), {a});
			अन्यथा
				वापस add(Fn::zero_extend, Sort(out_width), {a});
		}
		गाँठ_2 concat(गाँठ_2 a, गाँठ_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			वापस add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		गाँठ_2 add(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::add, a.sort(), {a, b}); }
		गाँठ_2 sub(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::sub, a.sort(), {a, b}); }
		गाँठ_2 mul(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::mul, a.sort(), {a, b}); }
		गाँठ_2 unsigned_div(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::unsigned_div, a.sort(), {a, b}); }
		गाँठ_2 unsigned_mod(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::unsigned_mod, a.sort(), {a, b}); }
		गाँठ_2 bitwise_and(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::bitwise_and, a.sort(), {a, b}); }
		गाँठ_2 bitwise_or(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::bitwise_or, a.sort(), {a, b}); }
		गाँठ_2 bitwise_xor(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::bitwise_xor, a.sort(), {a, b}); }
		गाँठ_2 bitwise_not(गाँठ_2 a) { check_unary(a); वापस add(Fn::bitwise_not, a.sort(), {a}); }
		गाँठ_2 unary_minus(गाँठ_2 a) { check_unary(a); वापस add(Fn::unary_minus, a.sort(), {a}); }
		गाँठ_2 reduce_and(गाँठ_2 a) {
			check_unary(a);
			अगर(a.width() == 1)
				वापस a;
			वापस add(Fn::reduce_and, Sort(1), {a});
		}
		गाँठ_2 reduce_or(गाँठ_2 a) {
			check_unary(a);
			अगर(a.width() == 1)
				वापस a;
			वापस add(Fn::reduce_or, Sort(1), {a});
		}
		गाँठ_2 reduce_xor(गाँठ_2 a) {
			check_unary(a);
			अगर(a.width() == 1)
				वापस a;
			वापस add(Fn::reduce_xor, Sort(1), {a});
		}
		गाँठ_2 equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::equal, Sort(1), {a, b}); }
		गाँठ_2 not_equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::not_equal, Sort(1), {a, b}); }
		गाँठ_2 signed_greater_than(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::signed_greater_than, Sort(1), {a, b}); }
		गाँठ_2 signed_greater_equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		गाँठ_2 unsigned_greater_than(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		गाँठ_2 unsigned_greater_equal(गाँठ_2 a, गाँठ_2 b) { check_basic_द्विआधारी(a, b); वापस add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		गाँठ_2 logical_shift_बायाँ(गाँठ_2 a, गाँठ_2 b) { check_shift(a, b); वापस add(Fn::logical_shift_बायाँ, a.sort(), {a, b}); }
		गाँठ_2 logical_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { check_shift(a, b); वापस add(Fn::logical_shift_दायाँ, a.sort(), {a, b}); }
		गाँठ_2 arithmetic_shift_दायाँ(गाँठ_2 a, गाँठ_2 b) { check_shift(a, b); वापस add(Fn::arithmetic_shift_दायाँ, a.sort(), {a, b}); }
		गाँठ_2 mux(गाँठ_2 a, गाँठ_2 b, गाँठ_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			वापस add(Fn::mux, a.sort(), {a, b, s});
		}
		गाँठ_2 memory_read(गाँठ_2 mem, गाँठ_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			वापस add(Fn::memory_read, Sort(mem.sort().तथ्य_width()), {mem, addr});
		}
		गाँठ_2 memory_write(गाँठ_2 mem, गाँठ_2 addr, गाँठ_2 तथ्य) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && तथ्य.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().तथ्य_width() == तथ्य.sort().width());
			वापस add(Fn::memory_write, mem.sort(), {mem, addr, तथ्य});
		}
		गाँठ_2 constant(RTLIL::Const मूल्य) {
			पूर्णांक s = मूल्य.आकार();
			वापस add(IR::गाँठ_तथ्य(Fn::constant, std::move(मूल्य)), Sort(s), {});
		}
		गाँठ_2 create_pending(पूर्णांक width) {
			वापस add(Fn::buf, Sort(width), {});
		}
		व्योम update_pending(गाँठ_2 गाँठ, गाँठ_2 मूल्य) {
			log_assert(गाँठ._ref.function() == Fn::buf && गाँठ._ref.आकार() == 0);
			log_assert(गाँठ.sort() == मूल्य.sort());
			_ir.mutate(गाँठ).append_arg(मूल्य._ref);
		}
		IRInput &add_निवेश(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			अगर (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			वापस it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			अगर (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			वापस it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			स्वतः [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			अगर (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			वापस it->second;
		}
		गाँठ_2 मूल्य(IRInput स्थिर& निवेश) {
			वापस add(IR::गाँठ_तथ्य(Fn::निवेश, std::pair(निवेश.name, निवेश.kind)), निवेश.sort, {});
		}
		गाँठ_2 मूल्य(IRState स्थिर& state) {
			वापस add(IR::गाँठ_तथ्य(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		व्योम suggest_name(गाँठ_2 गाँठ, IdString name) {
			_ir.mutate(गाँठ).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	अंतरभूत Factory IR::factory() { वापस Factory(*this); }
	ढाँचा<class Id> class Scope {
	रक्षित:
		अक्षर substitution_character = '_';
		भव bool is_character_legal(अक्षर, पूर्णांक) = 0;
	निजी:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	खुला:
		व्योम reserve(std::string name) {
			_used_names.जोड़ो(std::move(name));
		}
		ढाँचा<typename N> std::string unique_name(स्थिर N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			क्रम(माप_प्रकार i = 0; i < str.आकार(); i++)
				अगर(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			अगर(_used_names.गिनती(str) == 0) {
				_used_names.जोड़ो(str);
				वापस str;
			}
			क्रम (पूर्णांक idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				अगर(_used_names.गिनती(suffixed) == 0) {
					_used_names.जोड़ो(suffixed);
					वापस suffixed;
				}
			}
		}
		ढाँचा<typename N> std::string चालक()(Id id, स्थिर N &suggestion) {
			स्वतः it = _by_id.find(id);
			अगर(it != _by_id.end())
				वापस it->second;
			std::string str = unique_name(suggestion);
			_by_id.जोड़ो({id, str});
			वापस str;
		}
	};
	class Writer {
		std::ostream *os;
		व्योम print_impl(स्थिर अक्षर *fmt, vector<std::function<व्योम()>>& fns);
	खुला:
		Writer(std::ostream &os) : os(&os) {}
		ढाँचा<class T> Writer& चालक <<(T&& arg) { *os << std::forward<T>(arg); वापस *this; }
		ढाँचा<typename... Args>
		व्योम print(स्थिर अक्षर *fmt, Args&&... args)
		{
			vector<std::function<व्योम()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		ढाँचा<typename Fn, typename... Args>
		व्योम print_with(Fn fn, स्थिर अक्षर *fmt, Args&&... args)
		{
			vector<std::function<व्योम()>> fns { [&]() {
				अगर constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				अन्यथा
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#पूर्ण_अगर
