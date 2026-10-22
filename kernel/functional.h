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

#شامل "kernel/yosys.h"
#شامل "kernel/compute_graph.h"
#شامل "kernel/drivertools.h"
#شامل "kernel/mem.h"
#شامل "kernel/utils.h"

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
	شمارش class Fn {
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
		logical_shift_چپ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_shift_راست,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_shift_راست,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		درون_داد,
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
	ثابت نویسه *fn_to_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class Sort {
		std::variant<صحیح, std::pair<صحیح, صحیح>> _v;
	عمومی:
		explicit Sort(صحیح width) : _v(width) { }
		Sort(صحیح addr_width, صحیح data_width) : _v(std::make_pair(addr_width, data_width)) { }
		bool is_signal() ثابت { بازگشت _v.index() == 0; }
		bool is_memory() ثابت { بازگشت _v.index() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		صحیح width() ثابت { بازگشت std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		صحیح addr_width() ثابت { بازگشت std::get<1>(_v).first; }
		// returns the data width of a bitvector sort, errors out for other sorts
		صحیح data_width() ثابت { بازگشت std::get<1>(_v).second; }
		bool operator==(Sort ثابت& other) ثابت { بازگشت _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(_v); بازگشت h; }
	};
	class IR;
	class Factory;
	class گره_2;
	class IRInput {
		friend class Factory;
	عمومی:
		PooledName name;
		PooledName kind;
		Sort sort;
	خصوصی:
		IRInput(IR &ir, IdString name, IdString kind, Sort sort);
	};
	class IROutput {
		friend class Factory;
		IR &_ir;
	عمومی:
		PooledName name;
		PooledName kind;
		Sort sort;
	خصوصی:
		IROutput(IR &ir, IdString name, IdString kind, Sort sort);
	عمومی:
		گره_2 مقدار() ثابت;
		bool has_مقدار() ثابت;
		تهی set_مقدار(گره_2 مقدار);
	};
	class IRState {
		friend class Factory;
		IR &_ir;
	عمومی:
		PooledName name;
		PooledName kind;
		Sort sort;
	خصوصی:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString kind, Sort sort);
	عمومی:
		گره_2 next_مقدار() ثابت;
		bool has_next_مقدار() ثابت;
		RTLIL::Const ثابت& initial_مقدار_signal() ثابت { بازگشت std::get<RTLIL::Const>(_initial); }
		MemContents ثابت& initial_مقدار_memory() ثابت { بازگشت std::get<MemContents>(_initial); }
		تهی set_next_مقدار(گره_2 مقدار);
		تهی set_initial_مقدار(RTLIL::Const مقدار) { مقدار.extu(sort.width()); _initial = std::move(مقدار); }
		تهی set_initial_مقدار(MemContents مقدار) { log_assert(Sort(مقدار.addr_width(), مقدار.data_width()) == sort); _initial = std::move(مقدار); }
	};
	class IR {
		friend class Factory;
		friend class گره_2;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class گره_Data {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				صحیح
			> _extra;
		عمومی:
			گره_Data() : _fn(Fn::invalid) {}
			گره_Data(Fn fn) : _fn(fn) {}
			template<class T> گره_Data(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ثابت { بازگشت _fn; }
			ثابت RTLIL::Const &as_const() ثابت { بازگشت std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ثابت { بازگشت std::get<std::pair<IdString, IdString>>(_extra); }
			صحیح as_int() ثابت { بازگشت std::get<صحیح>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
				h.eat((unsigned صحیح) _fn);
				h.eat(_extra);
				بازگشت h;
			}
			bool operator==(گره_Data ثابت &other) ثابت {
				بازگشت _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		ساختار Attr {
			Sort sort;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using Graph = ComputeGraph<گره_Data, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		Graph _graph;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> _states;
		IR::Graph::Ref mutate(گره_2 n);
	عمومی:
		Design *design = nullptr;
		ایستا IR from_module(Module *module);
		Factory factory();
		صحیح size() ثابت { بازگشت _graph.size(); }
		گره_2 operator[](صحیح i);
		تهی topological_sort();
		تهی forward_buf();
		IRInput ثابت& درون_داد(IdString name, IdString kind) ثابت { بازگشت _inputs.at({name, kind}); }
		IRInput ثابت& درون_داد(IdString name) ثابت { بازگشت درون_داد(name, ID($درون_داد)); }
		IROutput ثابت& output(IdString name, IdString kind) ثابت { بازگشت _outputs.at({name, kind}); }
		IROutput ثابت& output(IdString name) ثابت { بازگشت output(name, ID($output)); }
		IRState ثابت& state(IdString name, IdString kind) ثابت { بازگشت _states.at({name, kind}); }
		IRState ثابت& state(IdString name) ثابت { بازگشت state(name, ID($state)); }
		bool has_درون_داد(IdString name, IdString kind) ثابت { بازگشت _inputs.شمار({name, kind}); }
		bool has_output(IdString name, IdString kind) ثابت { بازگشت _outputs.شمار({name, kind}); }
		bool has_state(IdString name, IdString kind) ثابت { بازگشت _states.شمار({name, kind}); }
		vector<IRInput ثابت*> inputs(IdString kind) ثابت;
		vector<IRInput ثابت*> inputs() ثابت { بازگشت inputs(ID($درون_داد)); }
		vector<IROutput ثابت*> outputs(IdString kind) ثابت;
		vector<IROutput ثابت*> outputs() ثابت { بازگشت outputs(ID($output)); }
		vector<IRState ثابت*> states(IdString kind) ثابت;
		vector<IRState ثابت*> states() ثابت { بازگشت states(ID($state)); }
		vector<IRInput ثابت*> all_inputs() ثابت;
		vector<IROutput ثابت*> all_outputs() ثابت;
		vector<IRState ثابت*> all_states() ثابت;
		class iterator {
			friend class IR;
			IR *_ir;
			صحیح _index;
			iterator(IR *ir, صحیح index) : _ir(ir), _index(index) {}
		عمومی:
			using iterator_category = std::درون_داد_iterator_tag;
			using مقدار_type = گره_2;
			using pointer = arrow_proxy<گره_2>;
			using reference = گره_2;
			using difference_type = ptrdiff_t;
			گره_2 operator*();
			iterator &operator++() { _index++; بازگشت *this; }
			bool operator!=(iterator ثابت &other) ثابت { بازگشت _ir != other._ir || _index != other._index; }
			bool operator==(iterator ثابت &other) ثابت { بازگشت !(*this != other); }
			pointer operator->();
		};
		iterator begin() { بازگشت iterator(this, 0); }
		iterator end() { بازگشت iterator(this, _graph.size()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class گره_2 {
		friend class Factory;
		friend class IR;
		friend class IRInput;
		friend class IROutput;
		friend class IRState;
		IR::Graph::ConstRef _ref;
		گره_2(IR::Graph::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit operator IR::Graph::ConstRef() { بازگشت _ref; }
	عمومی:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		صحیح id() ثابت { بازگشت _ref.index(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ثابت {
			اگر(_ref.has_sparse_attr())
				بازگشت {design, _ref.sparse_attr()};
			وگرنه
				بازگشت {design, design->twines().add(std::string("\\n") + std::to_string(id()))};
		}
		Fn fn() ثابت { بازگشت _ref.function().fn(); }
		Sort sort() ثابت { بازگشت _ref.attr().sort; }
		// returns the width of a bitvector node, errors out for other nodes
		صحیح width() ثابت { بازگشت sort().width(); }
		size_t arg_شمار() ثابت { بازگشت _ref.size(); }
		گره_2 arg(صحیح n) ثابت { بازگشت گره_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		template<class Visitor> auto visit(Visitor v) ثابت
		{
			// currently templated but could be switched to AbstractVisitor &
			انتخاب(_ref.function().fn()) {
			حالت Fn::invalid: log_error("invalid node in visit\n"); بشکن;
			حالت Fn::buf: بازگشت v.buf(*this, arg(0)); بشکن;
			حالت Fn::slice: بازگشت v.slice(*this, arg(0), _ref.function().as_int(), sort().width()); بشکن;
			حالت Fn::zero_extend: بازگشت v.zero_extend(*this, arg(0), width()); بشکن;
			حالت Fn::sign_extend: بازگشت v.sign_extend(*this, arg(0), width()); بشکن;
			حالت Fn::concat: بازگشت v.concat(*this, arg(0), arg(1)); بشکن;
			حالت Fn::add: بازگشت v.add(*this, arg(0), arg(1)); بشکن;
			حالت Fn::sub: بازگشت v.sub(*this, arg(0), arg(1)); بشکن;
			حالت Fn::mul: بازگشت v.mul(*this, arg(0), arg(1)); بشکن;
			حالت Fn::unsigned_div: بازگشت v.unsigned_div(*this, arg(0), arg(1)); بشکن;
			حالت Fn::unsigned_mod: بازگشت v.unsigned_mod(*this, arg(0), arg(1)); بشکن;
			حالت Fn::bitwise_and: بازگشت v.bitwise_and(*this, arg(0), arg(1)); بشکن;
			حالت Fn::bitwise_or: بازگشت v.bitwise_or(*this, arg(0), arg(1)); بشکن;
			حالت Fn::bitwise_xor: بازگشت v.bitwise_xor(*this, arg(0), arg(1)); بشکن;
			حالت Fn::bitwise_not: بازگشت v.bitwise_not(*this, arg(0)); بشکن;
			حالت Fn::unary_minus: بازگشت v.unary_minus(*this, arg(0)); بشکن;
			حالت Fn::reduce_and: بازگشت v.reduce_and(*this, arg(0)); بشکن;
			حالت Fn::reduce_or: بازگشت v.reduce_or(*this, arg(0)); بشکن;
			حالت Fn::reduce_xor: بازگشت v.reduce_xor(*this, arg(0)); بشکن;
			حالت Fn::equal: بازگشت v.equal(*this, arg(0), arg(1)); بشکن;
			حالت Fn::not_equal: بازگشت v.not_equal(*this, arg(0), arg(1)); بشکن;
			حالت Fn::signed_greater_than: بازگشت v.signed_greater_than(*this, arg(0), arg(1)); بشکن;
			حالت Fn::signed_greater_equal: بازگشت v.signed_greater_equal(*this, arg(0), arg(1)); بشکن;
			حالت Fn::unsigned_greater_than: بازگشت v.unsigned_greater_than(*this, arg(0), arg(1)); بشکن;
			حالت Fn::unsigned_greater_equal: بازگشت v.unsigned_greater_equal(*this, arg(0), arg(1)); بشکن;
			حالت Fn::logical_shift_چپ: بازگشت v.logical_shift_چپ(*this, arg(0), arg(1)); بشکن;
			حالت Fn::logical_shift_راست: بازگشت v.logical_shift_راست(*this, arg(0), arg(1)); بشکن;
			حالت Fn::arithmetic_shift_راست: بازگشت v.arithmetic_shift_راست(*this, arg(0), arg(1)); بشکن;
			حالت Fn::mux: بازگشت v.mux(*this, arg(0), arg(1), arg(2)); بشکن;
			حالت Fn::constant: بازگشت v.constant(*this, _ref.function().as_const()); بشکن;
			حالت Fn::درون_داد: بازگشت v.درون_داد(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); بشکن;
			حالت Fn::state: بازگشت v.state(*this, _ref.function().as_idstring_pair().first, _ref.function().as_idstring_pair().second); بشکن;
			حالت Fn::memory_read: بازگشت v.memory_read(*this, arg(0), arg(1)); بشکن;
			حالت Fn::memory_write: بازگشت v.memory_write(*this, arg(0), arg(1), arg(2)); بشکن;
			}
			log_abort();
		}
		std::string to_string();
		std::string to_string(std::function<std::string(گره_2)>);
	};
	درونخطی IRInput::IRInput(IR &ir, IdString name, IdString kind, Sort sort)
		: name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	درونخطی IROutput::IROutput(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	درونخطی IRState::IRState(IR &ir, IdString name, IdString kind, Sort sort)
		: _ir(ir), name(ir.design, name), kind(ir.design, kind), sort(std::move(sort)) {}
	درونخطی IR::Graph::Ref IR::mutate(گره_2 n) { بازگشت _graph[n._ref.index()]; }
	درونخطی گره_2 IR::operator[](صحیح i) { بازگشت گره_2(_graph[i], design); }
	درونخطی گره_2 IROutput::مقدار() ثابت { بازگشت گره_2(_ir._graph({name, kind, false}), _ir.design); }
	درونخطی bool IROutput::has_مقدار() ثابت { بازگشت _ir._graph.has_کلید({name, kind, false}); }
	درونخطی تهی IROutput::set_مقدار(گره_2 مقدار) { log_assert(sort == مقدار.sort()); _ir.mutate(مقدار).assign_کلید({name, kind, false}); }
	درونخطی گره_2 IRState::next_مقدار() ثابت { بازگشت گره_2(_ir._graph({name, kind, true}), _ir.design); }
	درونخطی bool IRState::has_next_مقدار() ثابت { بازگشت _ir._graph.has_کلید({name, kind, true}); }
	درونخطی تهی IRState::set_next_مقدار(گره_2 مقدار) { log_assert(sort == مقدار.sort()); _ir.mutate(مقدار).assign_کلید({name, kind, true}); }
	درونخطی گره_2 IR::iterator::operator*() { بازگشت گره_2(_ir->_graph[_index], _ir->design); }
	درونخطی arrow_proxy<گره_2> IR::iterator::operator->() { بازگشت arrow_proxy<گره_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	template<class T> ساختار AbstractVisitor {
		مجازی T buf(گره_2 self, گره_2 n) = 0;
		مجازی T slice(گره_2 self, گره_2 a, صحیح offset, صحیح out_width) = 0;
		مجازی T zero_extend(گره_2 self, گره_2 a, صحیح out_width) = 0;
		مجازی T sign_extend(گره_2 self, گره_2 a, صحیح out_width) = 0;
		مجازی T concat(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T add(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T sub(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T mul(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T unsigned_div(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T unsigned_mod(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T bitwise_and(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T bitwise_or(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T bitwise_xor(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T bitwise_not(گره_2 self, گره_2 a) = 0;
		مجازی T unary_minus(گره_2 self, گره_2 a) = 0;
		مجازی T reduce_and(گره_2 self, گره_2 a) = 0;
		مجازی T reduce_or(گره_2 self, گره_2 a) = 0;
		مجازی T reduce_xor(گره_2 self, گره_2 a) = 0;
		مجازی T equal(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T not_equal(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T signed_greater_than(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T signed_greater_equal(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T unsigned_greater_than(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T unsigned_greater_equal(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T logical_shift_چپ(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T logical_shift_راست(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T arithmetic_shift_راست(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T mux(گره_2 self, گره_2 a, گره_2 b, گره_2 s) = 0;
		مجازی T constant(گره_2 self, RTLIL::Const ثابت & مقدار) = 0;
		مجازی T درون_داد(گره_2 self, IdString name, IdString kind) = 0;
		مجازی T state(گره_2 self, IdString name, IdString kind) = 0;
		مجازی T memory_read(گره_2 self, گره_2 mem, گره_2 addr) = 0;
		مجازی T memory_write(گره_2 self, گره_2 mem, گره_2 addr, گره_2 data) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	template<class T> ساختار DefaultVisitor : عمومی AbstractVisitor<T> {
		مجازی T default_handler(گره_2 self) = 0;
		T buf(گره_2 self, گره_2) override { بازگشت default_handler(self); }
		T slice(گره_2 self, گره_2, صحیح, صحیح) override { بازگشت default_handler(self); }
		T zero_extend(گره_2 self, گره_2, صحیح) override { بازگشت default_handler(self); }
		T sign_extend(گره_2 self, گره_2, صحیح) override { بازگشت default_handler(self); }
		T concat(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T add(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T sub(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T mul(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T unsigned_div(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T unsigned_mod(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T bitwise_and(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T bitwise_or(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T bitwise_xor(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T bitwise_not(گره_2 self, گره_2) override { بازگشت default_handler(self); }
		T unary_minus(گره_2 self, گره_2) override { بازگشت default_handler(self); }
		T reduce_and(گره_2 self, گره_2) override { بازگشت default_handler(self); }
		T reduce_or(گره_2 self, گره_2) override { بازگشت default_handler(self); }
		T reduce_xor(گره_2 self, گره_2) override { بازگشت default_handler(self); }
		T equal(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T not_equal(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T signed_greater_than(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T signed_greater_equal(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T unsigned_greater_than(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T unsigned_greater_equal(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T logical_shift_چپ(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T logical_shift_راست(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T arithmetic_shift_راست(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T mux(گره_2 self, گره_2, گره_2, گره_2) override { بازگشت default_handler(self); }
		T constant(گره_2 self, RTLIL::Const ثابت &) override { بازگشت default_handler(self); }
		T درون_داد(گره_2 self, IdString, IdString) override { بازگشت default_handler(self); }
		T state(گره_2 self, IdString, IdString) override { بازگشت default_handler(self); }
		T memory_read(گره_2 self, گره_2, گره_2) override { بازگشت default_handler(self); }
		T memory_write(گره_2 self, گره_2, گره_2, گره_2) override { بازگشت default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		friend class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		گره_2 add(IR::گره_Data &&fn, Sort ثابت &sort, std::initializer_list<گره_2> args) {
			log_assert(!sort.is_signal() || sort.width() > 0);
			log_assert(!sort.is_memory() || (sort.addr_width() > 0 && sort.data_width() > 0));
			IR::Graph::Ref ref = _ir._graph.add(std::move(fn), {std::move(sort)});
			برای (auto arg : args)
				ref.append_arg(IR::Graph::ConstRef(arg));
			بازگشت گره_2(ref, _ir.design);
		}
		تهی check_basic_دودویی(گره_2 ثابت &a, گره_2 ثابت &b) { log_assert(a.sort().is_signal() && a.sort() == b.sort()); }
		تهی check_shift(گره_2 ثابت &a, گره_2 ثابت &b) { log_assert(a.sort().is_signal() && b.sort().is_signal() && b.width() == ceil_log2(a.width())); }
		تهی check_unary(گره_2 ثابت &a) { log_assert(a.sort().is_signal()); }
	عمومی:
		IR &ir() { بازگشت _ir; }
		گره_2 slice(گره_2 a, صحیح offset, صحیح out_width) {
			log_assert(a.sort().is_signal() && offset + out_width <= a.sort().width());
			اگر(offset == 0 && out_width == a.width())
				بازگشت a;
			بازگشت add(IR::گره_Data(Fn::slice, offset), Sort(out_width), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		گره_2 extend(گره_2 a, صحیح out_width, bool is_signed) {
			صحیح in_width = a.sort().width();
			log_assert(a.sort().is_signal());
			اگر(in_width == out_width)
				بازگشت a;
			اگر(in_width > out_width)
				بازگشت slice(a, 0, out_width);
			اگر(is_signed)
				بازگشت add(Fn::sign_extend, Sort(out_width), {a});
			وگرنه
				بازگشت add(Fn::zero_extend, Sort(out_width), {a});
		}
		گره_2 concat(گره_2 a, گره_2 b) {
			log_assert(a.sort().is_signal() && b.sort().is_signal());
			بازگشت add(Fn::concat, Sort(a.sort().width() + b.sort().width()), {a, b});
		}
		گره_2 add(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::add, a.sort(), {a, b}); }
		گره_2 sub(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::sub, a.sort(), {a, b}); }
		گره_2 mul(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::mul, a.sort(), {a, b}); }
		گره_2 unsigned_div(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::unsigned_div, a.sort(), {a, b}); }
		گره_2 unsigned_mod(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::unsigned_mod, a.sort(), {a, b}); }
		گره_2 bitwise_and(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::bitwise_and, a.sort(), {a, b}); }
		گره_2 bitwise_or(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::bitwise_or, a.sort(), {a, b}); }
		گره_2 bitwise_xor(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::bitwise_xor, a.sort(), {a, b}); }
		گره_2 bitwise_not(گره_2 a) { check_unary(a); بازگشت add(Fn::bitwise_not, a.sort(), {a}); }
		گره_2 unary_minus(گره_2 a) { check_unary(a); بازگشت add(Fn::unary_minus, a.sort(), {a}); }
		گره_2 reduce_and(گره_2 a) {
			check_unary(a);
			اگر(a.width() == 1)
				بازگشت a;
			بازگشت add(Fn::reduce_and, Sort(1), {a});
		}
		گره_2 reduce_or(گره_2 a) {
			check_unary(a);
			اگر(a.width() == 1)
				بازگشت a;
			بازگشت add(Fn::reduce_or, Sort(1), {a});
		}
		گره_2 reduce_xor(گره_2 a) {
			check_unary(a);
			اگر(a.width() == 1)
				بازگشت a;
			بازگشت add(Fn::reduce_xor, Sort(1), {a});
		}
		گره_2 equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::equal, Sort(1), {a, b}); }
		گره_2 not_equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::not_equal, Sort(1), {a, b}); }
		گره_2 signed_greater_than(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::signed_greater_than, Sort(1), {a, b}); }
		گره_2 signed_greater_equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::signed_greater_equal, Sort(1), {a, b}); }
		گره_2 unsigned_greater_than(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::unsigned_greater_than, Sort(1), {a, b}); }
		گره_2 unsigned_greater_equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); بازگشت add(Fn::unsigned_greater_equal, Sort(1), {a, b}); }
		گره_2 logical_shift_چپ(گره_2 a, گره_2 b) { check_shift(a, b); بازگشت add(Fn::logical_shift_چپ, a.sort(), {a, b}); }
		گره_2 logical_shift_راست(گره_2 a, گره_2 b) { check_shift(a, b); بازگشت add(Fn::logical_shift_راست, a.sort(), {a, b}); }
		گره_2 arithmetic_shift_راست(گره_2 a, گره_2 b) { check_shift(a, b); بازگشت add(Fn::arithmetic_shift_راست, a.sort(), {a, b}); }
		گره_2 mux(گره_2 a, گره_2 b, گره_2 s) {
			log_assert(a.sort().is_signal() && a.sort() == b.sort() && s.sort() == Sort(1));
			بازگشت add(Fn::mux, a.sort(), {a, b, s});
		}
		گره_2 memory_read(گره_2 mem, گره_2 addr) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && mem.sort().addr_width() == addr.sort().width());
			بازگشت add(Fn::memory_read, Sort(mem.sort().data_width()), {mem, addr});
		}
		گره_2 memory_write(گره_2 mem, گره_2 addr, گره_2 data) {
			log_assert(mem.sort().is_memory() && addr.sort().is_signal() && data.sort().is_signal() &&
				mem.sort().addr_width() == addr.sort().width() && mem.sort().data_width() == data.sort().width());
			بازگشت add(Fn::memory_write, mem.sort(), {mem, addr, data});
		}
		گره_2 constant(RTLIL::Const مقدار) {
			صحیح s = مقدار.size();
			بازگشت add(IR::گره_Data(Fn::constant, std::move(مقدار)), Sort(s), {});
		}
		گره_2 create_pending(صحیح width) {
			بازگشت add(Fn::buf, Sort(width), {});
		}
		تهی update_pending(گره_2 گره, گره_2 مقدار) {
			log_assert(گره._ref.function() == Fn::buf && گره._ref.size() == 0);
			log_assert(گره.sort() == مقدار.sort());
			_ir.mutate(گره).append_arg(مقدار._ref);
		}
		IRInput &add_درون_داد(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._inputs.emplace({name, kind}, IRInput(_ir, name, kind, std::move(sort)));
			اگر (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			بازگشت it->second;
		}
		IROutput &add_output(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._outputs.emplace({name, kind}, IROutput(_ir, name, kind, std::move(sort)));
			اگر (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			بازگشت it->second;
		}
		IRState &add_state(IdString name, IdString kind, Sort sort) {
			auto [it, inserted] = _ir._states.emplace({name, kind}, IRState(_ir, name, kind, std::move(sort)));
			اگر (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			بازگشت it->second;
		}
		گره_2 مقدار(IRInput ثابت& درون_داد) {
			بازگشت add(IR::گره_Data(Fn::درون_داد, std::pair(درون_داد.name, درون_داد.kind)), درون_داد.sort, {});
		}
		گره_2 مقدار(IRState ثابت& state) {
			بازگشت add(IR::گره_Data(Fn::state, std::pair(state.name, state.kind)), state.sort, {});
		}
		تهی suggest_name(گره_2 گره, IdString name) {
			_ir.mutate(گره).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	درونخطی Factory IR::factory() { بازگشت Factory(*this); }
	template<class Id> class Scope {
	محافظتشده:
		نویسه substitution_character = '_';
		مجازی bool is_character_legal(نویسه, صحیح) = 0;
	خصوصی:
		pool<std::string> _used_names;
		dict<Id, std::string> _by_id;
	عمومی:
		تهی reserve(std::string name) {
			_used_names.درج(std::move(name));
		}
		template<typename N> std::string unique_name(ثابت N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			برای(size_t i = 0; i < str.size(); i++)
				اگر(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			اگر(_used_names.شمار(str) == 0) {
				_used_names.درج(str);
				بازگشت str;
			}
			برای (صحیح idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::to_string(idx);
				اگر(_used_names.شمار(suffixed) == 0) {
					_used_names.درج(suffixed);
					بازگشت suffixed;
				}
			}
		}
		template<typename N> std::string operator()(Id id, ثابت N &suggestion) {
			auto it = _by_id.find(id);
			اگر(it != _by_id.end())
				بازگشت it->second;
			std::string str = unique_name(suggestion);
			_by_id.درج({id, str});
			بازگشت str;
		}
	};
	class Writer {
		std::ostream *os;
		تهی print_impl(ثابت نویسه *fmt, vector<std::function<تهی()>>& fns);
	عمومی:
		Writer(std::ostream &os) : os(&os) {}
		template<class T> Writer& operator <<(T&& arg) { *os << std::forward<T>(arg); بازگشت *this; }
		template<typename... Args>
		تهی print(ثابت نویسه *fmt, Args&&... args)
		{
			vector<std::function<تهی()>> fns { [&]() { *this << args; }... };
			print_impl(fmt, fns);
		}
		template<typename Fn, typename... Args>
		تهی print_with(Fn fn, ثابت نویسه *fmt, Args&&... args)
		{
			vector<std::function<تهی()>> fns { [&]() {
				اگر constexpr (std::is_invocable_v<Fn, Args>)
					*this << fn(args);
				وگرنه
					*this << args; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_END

#endif
