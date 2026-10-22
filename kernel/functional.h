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

#اگر_تعریف_نشده FUNCTIONAL_H
#تعریف_کن FUNCTIONAL_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/compute_graph.h"
#ضمن "kernel/drivertools.h"
#ضمن "kernel/mem.h"
#ضمن "kernel/utils.h"

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
	تعداد class Fn {
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
		برش,
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
		جمع_کن,
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
		logical_لغزش_چپ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_لغزش_راست,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_لغزش_راست,
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
		حافظه_read,
		// memory_write(memory: memory[addr_width, data_width], addr: bit[addr_width], data: bit[data_width]): memory[addr_width, data_width]
		// returns a copy of `memory` but with the value at `addr` changed to `data`
		حافظه_write
	};
	// returns the name of a Fn value, as a string literal
	ثابت حرف *fn_مقصد_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class مرتب_سازی_2 {
		std::variant<صحيح, std::pair<صحيح, صحيح>> _v;
	عام:
		explicit مرتب_سازی_2(صحيح عرض) : _v(عرض) { }
		مرتب_سازی_2(صحيح addr_عرض, صحيح داده_عرض) : _v(std::make_pair(addr_عرض, داده_عرض)) { }
		bool is_signal() ثابت { أرجع _v.شاخص() == 0; }
		bool is_حافظه() ثابت { أرجع _v.شاخص() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		صحيح عرض() ثابت { أرجع std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		صحيح addr_عرض() ثابت { أرجع std::get<1>(_v).اولی; }
		// returns the data width of a bitvector sort, errors out for other sorts
		صحيح داده_عرض() ثابت { أرجع std::get<1>(_v).دومی; }
		bool عملگر==(مرتب_سازی_2 ثابت& other) ثابت { أرجع _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(_v); أرجع h; }
	};
	class IR;
	class Factory;
	class گره_2;
	class IRInput {
		دوست class Factory;
	عام:
		PooledName name;
		PooledName نوع;
		مرتب_سازی_2 مرتب_سازی;
	خاص:
		IRInput(IR &ir, IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی);
	};
	class IROutput {
		دوست class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName نوع;
		مرتب_سازی_2 مرتب_سازی;
	خاص:
		IROutput(IR &ir, IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی);
	عام:
		گره_2 مقدار() ثابت;
		bool has_مقدار() ثابت;
		فراغ مجموعه_مقدار(گره_2 مقدار);
	};
	class IRState {
		دوست class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName نوع;
		مرتب_سازی_2 مرتب_سازی;
	خاص:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی);
	عام:
		گره_2 بعدی_مقدار() ثابت;
		bool has_بعدی_مقدار() ثابت;
		RTLIL::Const ثابت& initial_مقدار_signal() ثابت { أرجع std::get<RTLIL::Const>(_initial); }
		MemContents ثابت& initial_مقدار_حافظه() ثابت { أرجع std::get<MemContents>(_initial); }
		فراغ مجموعه_بعدی_مقدار(گره_2 مقدار);
		فراغ مجموعه_initial_مقدار(RTLIL::Const مقدار) { مقدار.extu(مرتب_سازی.عرض()); _initial = std::move(مقدار); }
		فراغ مجموعه_initial_مقدار(MemContents مقدار) { log_assert(مرتب_سازی_2(مقدار.addr_عرض(), مقدار.داده_عرض()) == مرتب_سازی); _initial = std::move(مقدار); }
	};
	class IR {
		دوست class Factory;
		دوست class گره_2;
		دوست class IRInput;
		دوست class IROutput;
		دوست class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class گره_داده {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				صحيح
			> _extra;
		عام:
			گره_داده() : _fn(Fn::invalid) {}
			گره_داده(Fn fn) : _fn(fn) {}
			قالب<class T> گره_داده(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ثابت { أرجع _fn; }
			ثابت RTLIL::Const &as_const() ثابت { أرجع std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ثابت { أرجع std::get<std::pair<IdString, IdString>>(_extra); }
			صحيح as_int() ثابت { أرجع std::get<صحيح>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
				h.eat((بیعلامت صحيح) _fn);
				h.eat(_extra);
				أرجع h;
			}
			bool عملگر==(گره_داده ثابت &other) ثابت {
				أرجع _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		بنية Attr {
			مرتب_سازی_2 مرتب_سازی;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using گراف = Compute_گراف<گره_داده, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		گراف گراف_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> حالتها_2;
		IR::گراف::Ref mutate(گره_2 n);
	عام:
		Design *design = nullptr;
		ساكن IR from_module(Module *module);
		Factory factory();
		صحيح اندازه() ثابت { أرجع گراف_4.اندازه(); }
		گره_2 عملگر[](صحيح i);
		فراغ topological_مرتب_سازی();
		فراغ forward_buf();
		IRInput ثابت& درون_داد(IdString name, IdString نوع) ثابت { أرجع _inputs.at({name, نوع}); }
		IRInput ثابت& درون_داد(IdString name) ثابت { أرجع درون_داد(name, ID($درون_داد)); }
		IROutput ثابت& output(IdString name, IdString نوع) ثابت { أرجع _outputs.at({name, نوع}); }
		IROutput ثابت& output(IdString name) ثابت { أرجع output(name, ID($output)); }
		IRState ثابت& state(IdString name, IdString نوع) ثابت { أرجع حالتها_2.at({name, نوع}); }
		IRState ثابت& state(IdString name) ثابت { أرجع state(name, ID($state)); }
		bool has_درون_داد(IdString name, IdString نوع) ثابت { أرجع _inputs.شمار({name, نوع}); }
		bool has_output(IdString name, IdString نوع) ثابت { أرجع _outputs.شمار({name, نوع}); }
		bool has_state(IdString name, IdString نوع) ثابت { أرجع حالتها_2.شمار({name, نوع}); }
		وکتور<IRInput ثابت*> inputs(IdString نوع) ثابت;
		وکتور<IRInput ثابت*> inputs() ثابت { أرجع inputs(ID($درون_داد)); }
		وکتور<IROutput ثابت*> outputs(IdString نوع) ثابت;
		وکتور<IROutput ثابت*> outputs() ثابت { أرجع outputs(ID($output)); }
		وکتور<IRState ثابت*> حالتها(IdString نوع) ثابت;
		وکتور<IRState ثابت*> حالتها() ثابت { أرجع حالتها(ID($state)); }
		وکتور<IRInput ثابت*> all_inputs() ثابت;
		وکتور<IROutput ثابت*> all_outputs() ثابت;
		وکتور<IRState ثابت*> all_حالتها() ثابت;
		class iterator {
			دوست class IR;
			IR *_ir;
			صحيح شاخص_3;
			iterator(IR *ir, صحيح شاخص) : _ir(ir), شاخص_3(شاخص) {}
		عام:
			using iterator_category = std::درون_داد_iterator_برچسب;
			using مقدار_type = گره_2;
			using اشاره_گر = arrow_proxy<گره_2>;
			using reference = گره_2;
			using difference_type = ptrdiff_t;
			گره_2 عملگر*();
			iterator &عملگر++() { شاخص_3++; أرجع *this; }
			bool عملگر!=(iterator ثابت &other) ثابت { أرجع _ir != other._ir || شاخص_3 != other.شاخص_3; }
			bool عملگر==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
			اشاره_گر عملگر->();
		};
		iterator begin() { أرجع iterator(this, 0); }
		iterator انتها() { أرجع iterator(this, گراف_4.اندازه()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class گره_2 {
		دوست class Factory;
		دوست class IR;
		دوست class IRInput;
		دوست class IROutput;
		دوست class IRState;
		IR::گراف::ConstRef _ref;
		گره_2(IR::گراف::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit عملگر IR::گراف::ConstRef() { أرجع _ref; }
	عام:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		صحيح id() ثابت { أرجع _ref.شاخص(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ثابت {
			إذا(_ref.has_sparse_attr())
				أرجع {design, _ref.sparse_attr()};
			وإلا
				أرجع {design, design->twines().جمع_کن(std::string("\\n") + std::مقصد_string(id()))};
		}
		Fn fn() ثابت { أرجع _ref.function().fn(); }
		مرتب_سازی_2 مرتب_سازی() ثابت { أرجع _ref.attr().مرتب_سازی; }
		// returns the width of a bitvector node, errors out for other nodes
		صحيح عرض() ثابت { أرجع مرتب_سازی().عرض(); }
		اندازه_t arg_شمار() ثابت { أرجع _ref.اندازه(); }
		گره_2 arg(صحيح n) ثابت { أرجع گره_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		قالب<class Visitor> خودکار بازدید(Visitor v) ثابت
		{
			// currently templated but could be switched to AbstractVisitor &
			بدل(_ref.function().fn()) {
			حالة Fn::invalid: log_error("invalid node in visit\n"); اكسر;
			حالة Fn::buf: أرجع v.buf(*this, arg(0)); اكسر;
			حالة Fn::برش: أرجع v.برش(*this, arg(0), _ref.function().as_int(), مرتب_سازی().عرض()); اكسر;
			حالة Fn::zero_extend: أرجع v.zero_extend(*this, arg(0), عرض()); اكسر;
			حالة Fn::sign_extend: أرجع v.sign_extend(*this, arg(0), عرض()); اكسر;
			حالة Fn::concat: أرجع v.concat(*this, arg(0), arg(1)); اكسر;
			حالة Fn::جمع_کن: أرجع v.جمع_کن(*this, arg(0), arg(1)); اكسر;
			حالة Fn::sub: أرجع v.sub(*this, arg(0), arg(1)); اكسر;
			حالة Fn::mul: أرجع v.mul(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_div: أرجع v.unsigned_div(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_mod: أرجع v.unsigned_mod(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_and: أرجع v.bitwise_and(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_or: أرجع v.bitwise_or(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_xor: أرجع v.bitwise_xor(*this, arg(0), arg(1)); اكسر;
			حالة Fn::bitwise_not: أرجع v.bitwise_not(*this, arg(0)); اكسر;
			حالة Fn::unary_minus: أرجع v.unary_minus(*this, arg(0)); اكسر;
			حالة Fn::reduce_and: أرجع v.reduce_and(*this, arg(0)); اكسر;
			حالة Fn::reduce_or: أرجع v.reduce_or(*this, arg(0)); اكسر;
			حالة Fn::reduce_xor: أرجع v.reduce_xor(*this, arg(0)); اكسر;
			حالة Fn::equal: أرجع v.equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::not_equal: أرجع v.not_equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::signed_greater_than: أرجع v.signed_greater_than(*this, arg(0), arg(1)); اكسر;
			حالة Fn::signed_greater_equal: أرجع v.signed_greater_equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_greater_than: أرجع v.unsigned_greater_than(*this, arg(0), arg(1)); اكسر;
			حالة Fn::unsigned_greater_equal: أرجع v.unsigned_greater_equal(*this, arg(0), arg(1)); اكسر;
			حالة Fn::logical_لغزش_چپ: أرجع v.logical_لغزش_چپ(*this, arg(0), arg(1)); اكسر;
			حالة Fn::logical_لغزش_راست: أرجع v.logical_لغزش_راست(*this, arg(0), arg(1)); اكسر;
			حالة Fn::arithmetic_لغزش_راست: أرجع v.arithmetic_لغزش_راست(*this, arg(0), arg(1)); اكسر;
			حالة Fn::mux: أرجع v.mux(*this, arg(0), arg(1), arg(2)); اكسر;
			حالة Fn::constant: أرجع v.constant(*this, _ref.function().as_const()); اكسر;
			حالة Fn::درون_داد: أرجع v.درون_داد(*this, _ref.function().as_idstring_pair().اولی, _ref.function().as_idstring_pair().دومی); اكسر;
			حالة Fn::state: أرجع v.state(*this, _ref.function().as_idstring_pair().اولی, _ref.function().as_idstring_pair().دومی); اكسر;
			حالة Fn::حافظه_read: أرجع v.حافظه_read(*this, arg(0), arg(1)); اكسر;
			حالة Fn::حافظه_write: أرجع v.حافظه_write(*this, arg(0), arg(1), arg(2)); اكسر;
			}
			log_abort();
		}
		std::string مقصد_string();
		std::string مقصد_string(std::function<std::string(گره_2)>);
	};
	مضمن IRInput::IRInput(IR &ir, IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی)
		: name(ir.design, name), نوع(ir.design, نوع), مرتب_سازی(std::move(مرتب_سازی)) {}
	مضمن IROutput::IROutput(IR &ir, IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی)
		: _ir(ir), name(ir.design, name), نوع(ir.design, نوع), مرتب_سازی(std::move(مرتب_سازی)) {}
	مضمن IRState::IRState(IR &ir, IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی)
		: _ir(ir), name(ir.design, name), نوع(ir.design, نوع), مرتب_سازی(std::move(مرتب_سازی)) {}
	مضمن IR::گراف::Ref IR::mutate(گره_2 n) { أرجع گراف_4[n._ref.شاخص()]; }
	مضمن گره_2 IR::عملگر[](صحيح i) { أرجع گره_2(گراف_4[i], design); }
	مضمن گره_2 IROutput::مقدار() ثابت { أرجع گره_2(_ir.گراف_4({name, نوع, false}), _ir.design); }
	مضمن bool IROutput::has_مقدار() ثابت { أرجع _ir.گراف_4.has_کلید({name, نوع, false}); }
	مضمن فراغ IROutput::مجموعه_مقدار(گره_2 مقدار) { log_assert(مرتب_سازی == مقدار.مرتب_سازی()); _ir.mutate(مقدار).تخصیص_بده_کلید({name, نوع, false}); }
	مضمن گره_2 IRState::بعدی_مقدار() ثابت { أرجع گره_2(_ir.گراف_4({name, نوع, true}), _ir.design); }
	مضمن bool IRState::has_بعدی_مقدار() ثابت { أرجع _ir.گراف_4.has_کلید({name, نوع, true}); }
	مضمن فراغ IRState::مجموعه_بعدی_مقدار(گره_2 مقدار) { log_assert(مرتب_سازی == مقدار.مرتب_سازی()); _ir.mutate(مقدار).تخصیص_بده_کلید({name, نوع, true}); }
	مضمن گره_2 IR::iterator::عملگر*() { أرجع گره_2(_ir->گراف_4[شاخص_3], _ir->design); }
	مضمن arrow_proxy<گره_2> IR::iterator::عملگر->() { أرجع arrow_proxy<گره_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	قالب<class T> بنية AbstractVisitor {
		مجازی T buf(گره_2 self, گره_2 n) = 0;
		مجازی T برش(گره_2 self, گره_2 a, صحيح جابه_جایی, صحيح بیرون_عرض) = 0;
		مجازی T zero_extend(گره_2 self, گره_2 a, صحيح بیرون_عرض) = 0;
		مجازی T sign_extend(گره_2 self, گره_2 a, صحيح بیرون_عرض) = 0;
		مجازی T concat(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T جمع_کن(گره_2 self, گره_2 a, گره_2 b) = 0;
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
		مجازی T logical_لغزش_چپ(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T logical_لغزش_راست(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T arithmetic_لغزش_راست(گره_2 self, گره_2 a, گره_2 b) = 0;
		مجازی T mux(گره_2 self, گره_2 a, گره_2 b, گره_2 s) = 0;
		مجازی T constant(گره_2 self, RTLIL::Const ثابت & مقدار) = 0;
		مجازی T درون_داد(گره_2 self, IdString name, IdString نوع) = 0;
		مجازی T state(گره_2 self, IdString name, IdString نوع) = 0;
		مجازی T حافظه_read(گره_2 self, گره_2 mem, گره_2 addr) = 0;
		مجازی T حافظه_write(گره_2 self, گره_2 mem, گره_2 addr, گره_2 داده) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	قالب<class T> بنية DefaultVisitor : عام AbstractVisitor<T> {
		مجازی T default_handler(گره_2 self) = 0;
		T buf(گره_2 self, گره_2) override { أرجع default_handler(self); }
		T برش(گره_2 self, گره_2, صحيح, صحيح) override { أرجع default_handler(self); }
		T zero_extend(گره_2 self, گره_2, صحيح) override { أرجع default_handler(self); }
		T sign_extend(گره_2 self, گره_2, صحيح) override { أرجع default_handler(self); }
		T concat(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T جمع_کن(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T sub(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T mul(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T unsigned_div(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T unsigned_mod(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T bitwise_and(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T bitwise_or(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T bitwise_xor(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T bitwise_not(گره_2 self, گره_2) override { أرجع default_handler(self); }
		T unary_minus(گره_2 self, گره_2) override { أرجع default_handler(self); }
		T reduce_and(گره_2 self, گره_2) override { أرجع default_handler(self); }
		T reduce_or(گره_2 self, گره_2) override { أرجع default_handler(self); }
		T reduce_xor(گره_2 self, گره_2) override { أرجع default_handler(self); }
		T equal(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T not_equal(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T signed_greater_than(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T signed_greater_equal(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T unsigned_greater_than(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T unsigned_greater_equal(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T logical_لغزش_چپ(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T logical_لغزش_راست(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T arithmetic_لغزش_راست(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T mux(گره_2 self, گره_2, گره_2, گره_2) override { أرجع default_handler(self); }
		T constant(گره_2 self, RTLIL::Const ثابت &) override { أرجع default_handler(self); }
		T درون_داد(گره_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T state(گره_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T حافظه_read(گره_2 self, گره_2, گره_2) override { أرجع default_handler(self); }
		T حافظه_write(گره_2 self, گره_2, گره_2, گره_2) override { أرجع default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		دوست class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		گره_2 جمع_کن(IR::گره_داده &&fn, مرتب_سازی_2 ثابت &مرتب_سازی, std::initializer_list<گره_2> آرگومانها_2) {
			log_assert(!مرتب_سازی.is_signal() || مرتب_سازی.عرض() > 0);
			log_assert(!مرتب_سازی.is_حافظه() || (مرتب_سازی.addr_عرض() > 0 && مرتب_سازی.داده_عرض() > 0));
			IR::گراف::Ref ref = _ir.گراف_4.جمع_کن(std::move(fn), {std::move(مرتب_سازی)});
			لكل (خودکار arg : آرگومانها_2)
				ref.append_arg(IR::گراف::ConstRef(arg));
			أرجع گره_2(ref, _ir.design);
		}
		فراغ check_basic_دودویی(گره_2 ثابت &a, گره_2 ثابت &b) { log_assert(a.مرتب_سازی().is_signal() && a.مرتب_سازی() == b.مرتب_سازی()); }
		فراغ check_لغزش(گره_2 ثابت &a, گره_2 ثابت &b) { log_assert(a.مرتب_سازی().is_signal() && b.مرتب_سازی().is_signal() && b.عرض() == ceil_log2(a.عرض())); }
		فراغ check_unary(گره_2 ثابت &a) { log_assert(a.مرتب_سازی().is_signal()); }
	عام:
		IR &ir() { أرجع _ir; }
		گره_2 برش(گره_2 a, صحيح جابه_جایی, صحيح بیرون_عرض) {
			log_assert(a.مرتب_سازی().is_signal() && جابه_جایی + بیرون_عرض <= a.مرتب_سازی().عرض());
			إذا(جابه_جایی == 0 && بیرون_عرض == a.عرض())
				أرجع a;
			أرجع جمع_کن(IR::گره_داده(Fn::برش, جابه_جایی), مرتب_سازی_2(بیرون_عرض), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		گره_2 extend(گره_2 a, صحيح بیرون_عرض, bool is_signed) {
			صحيح درون_عرض = a.مرتب_سازی().عرض();
			log_assert(a.مرتب_سازی().is_signal());
			إذا(درون_عرض == بیرون_عرض)
				أرجع a;
			إذا(درون_عرض > بیرون_عرض)
				أرجع برش(a, 0, بیرون_عرض);
			إذا(is_signed)
				أرجع جمع_کن(Fn::sign_extend, مرتب_سازی_2(بیرون_عرض), {a});
			وإلا
				أرجع جمع_کن(Fn::zero_extend, مرتب_سازی_2(بیرون_عرض), {a});
		}
		گره_2 concat(گره_2 a, گره_2 b) {
			log_assert(a.مرتب_سازی().is_signal() && b.مرتب_سازی().is_signal());
			أرجع جمع_کن(Fn::concat, مرتب_سازی_2(a.مرتب_سازی().عرض() + b.مرتب_سازی().عرض()), {a, b});
		}
		گره_2 جمع_کن(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::جمع_کن, a.مرتب_سازی(), {a, b}); }
		گره_2 sub(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::sub, a.مرتب_سازی(), {a, b}); }
		گره_2 mul(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::mul, a.مرتب_سازی(), {a, b}); }
		گره_2 unsigned_div(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::unsigned_div, a.مرتب_سازی(), {a, b}); }
		گره_2 unsigned_mod(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::unsigned_mod, a.مرتب_سازی(), {a, b}); }
		گره_2 bitwise_and(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::bitwise_and, a.مرتب_سازی(), {a, b}); }
		گره_2 bitwise_or(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::bitwise_or, a.مرتب_سازی(), {a, b}); }
		گره_2 bitwise_xor(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::bitwise_xor, a.مرتب_سازی(), {a, b}); }
		گره_2 bitwise_not(گره_2 a) { check_unary(a); أرجع جمع_کن(Fn::bitwise_not, a.مرتب_سازی(), {a}); }
		گره_2 unary_minus(گره_2 a) { check_unary(a); أرجع جمع_کن(Fn::unary_minus, a.مرتب_سازی(), {a}); }
		گره_2 reduce_and(گره_2 a) {
			check_unary(a);
			إذا(a.عرض() == 1)
				أرجع a;
			أرجع جمع_کن(Fn::reduce_and, مرتب_سازی_2(1), {a});
		}
		گره_2 reduce_or(گره_2 a) {
			check_unary(a);
			إذا(a.عرض() == 1)
				أرجع a;
			أرجع جمع_کن(Fn::reduce_or, مرتب_سازی_2(1), {a});
		}
		گره_2 reduce_xor(گره_2 a) {
			check_unary(a);
			إذا(a.عرض() == 1)
				أرجع a;
			أرجع جمع_کن(Fn::reduce_xor, مرتب_سازی_2(1), {a});
		}
		گره_2 equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::equal, مرتب_سازی_2(1), {a, b}); }
		گره_2 not_equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::not_equal, مرتب_سازی_2(1), {a, b}); }
		گره_2 signed_greater_than(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::signed_greater_than, مرتب_سازی_2(1), {a, b}); }
		گره_2 signed_greater_equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::signed_greater_equal, مرتب_سازی_2(1), {a, b}); }
		گره_2 unsigned_greater_than(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::unsigned_greater_than, مرتب_سازی_2(1), {a, b}); }
		گره_2 unsigned_greater_equal(گره_2 a, گره_2 b) { check_basic_دودویی(a, b); أرجع جمع_کن(Fn::unsigned_greater_equal, مرتب_سازی_2(1), {a, b}); }
		گره_2 logical_لغزش_چپ(گره_2 a, گره_2 b) { check_لغزش(a, b); أرجع جمع_کن(Fn::logical_لغزش_چپ, a.مرتب_سازی(), {a, b}); }
		گره_2 logical_لغزش_راست(گره_2 a, گره_2 b) { check_لغزش(a, b); أرجع جمع_کن(Fn::logical_لغزش_راست, a.مرتب_سازی(), {a, b}); }
		گره_2 arithmetic_لغزش_راست(گره_2 a, گره_2 b) { check_لغزش(a, b); أرجع جمع_کن(Fn::arithmetic_لغزش_راست, a.مرتب_سازی(), {a, b}); }
		گره_2 mux(گره_2 a, گره_2 b, گره_2 s) {
			log_assert(a.مرتب_سازی().is_signal() && a.مرتب_سازی() == b.مرتب_سازی() && s.مرتب_سازی() == مرتب_سازی_2(1));
			أرجع جمع_کن(Fn::mux, a.مرتب_سازی(), {a, b, s});
		}
		گره_2 حافظه_read(گره_2 mem, گره_2 addr) {
			log_assert(mem.مرتب_سازی().is_حافظه() && addr.مرتب_سازی().is_signal() && mem.مرتب_سازی().addr_عرض() == addr.مرتب_سازی().عرض());
			أرجع جمع_کن(Fn::حافظه_read, مرتب_سازی_2(mem.مرتب_سازی().داده_عرض()), {mem, addr});
		}
		گره_2 حافظه_write(گره_2 mem, گره_2 addr, گره_2 داده) {
			log_assert(mem.مرتب_سازی().is_حافظه() && addr.مرتب_سازی().is_signal() && داده.مرتب_سازی().is_signal() &&
				mem.مرتب_سازی().addr_عرض() == addr.مرتب_سازی().عرض() && mem.مرتب_سازی().داده_عرض() == داده.مرتب_سازی().عرض());
			أرجع جمع_کن(Fn::حافظه_write, mem.مرتب_سازی(), {mem, addr, داده});
		}
		گره_2 constant(RTLIL::Const مقدار) {
			صحيح s = مقدار.اندازه();
			أرجع جمع_کن(IR::گره_داده(Fn::constant, std::move(مقدار)), مرتب_سازی_2(s), {});
		}
		گره_2 create_pending(صحيح عرض) {
			أرجع جمع_کن(Fn::buf, مرتب_سازی_2(عرض), {});
		}
		فراغ update_pending(گره_2 گره, گره_2 مقدار) {
			log_assert(گره._ref.function() == Fn::buf && گره._ref.اندازه() == 0);
			log_assert(گره.مرتب_سازی() == مقدار.مرتب_سازی());
			_ir.mutate(گره).append_arg(مقدار._ref);
		}
		IRInput &جمع_کن_درون_داد(IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی) {
			خودکار [it, inserted] = _ir._inputs.emplace({name, نوع}, IRInput(_ir, name, نوع, std::move(مرتب_سازی)));
			إذا (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دومی;
		}
		IROutput &جمع_کن_output(IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی) {
			خودکار [it, inserted] = _ir._outputs.emplace({name, نوع}, IROutput(_ir, name, نوع, std::move(مرتب_سازی)));
			إذا (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دومی;
		}
		IRState &جمع_کن_state(IdString name, IdString نوع, مرتب_سازی_2 مرتب_سازی) {
			خودکار [it, inserted] = _ir.حالتها_2.emplace({name, نوع}, IRState(_ir, name, نوع, std::move(مرتب_سازی)));
			إذا (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دومی;
		}
		گره_2 مقدار(IRInput ثابت& درون_داد) {
			أرجع جمع_کن(IR::گره_داده(Fn::درون_داد, std::pair(درون_داد.name, درون_داد.نوع)), درون_داد.مرتب_سازی, {});
		}
		گره_2 مقدار(IRState ثابت& state) {
			أرجع جمع_کن(IR::گره_داده(Fn::state, std::pair(state.name, state.نوع)), state.مرتب_سازی, {});
		}
		فراغ suggest_name(گره_2 گره, IdString name) {
			_ir.mutate(گره).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	مضمن Factory IR::factory() { أرجع Factory(*this); }
	قالب<class Id> class Scope {
	محمي:
		حرف substitution_character = '_';
		مجازی bool is_character_legal(حرف, صحيح) = 0;
	خاص:
		pool<std::string> استفاده_شده_نامها;
		dict<Id, std::string> _by_id;
	عام:
		فراغ reserve(std::string name) {
			استفاده_شده_نامها.درج(std::move(name));
		}
		قالب<typename N> std::string unique_name(ثابت N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			لكل(اندازه_t i = 0; i < str.اندازه(); i++)
				إذا(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			إذا(استفاده_شده_نامها.شمار(str) == 0) {
				استفاده_شده_نامها.درج(str);
				أرجع str;
			}
			لكل (صحيح idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::مقصد_string(idx);
				إذا(استفاده_شده_نامها.شمار(suffixed) == 0) {
					استفاده_شده_نامها.درج(suffixed);
					أرجع suffixed;
				}
			}
		}
		قالب<typename N> std::string عملگر()(Id id, ثابت N &suggestion) {
			خودکار it = _by_id.پیدا_کن(id);
			إذا(it != _by_id.انتها())
				أرجع it->دومی;
			std::string str = unique_name(suggestion);
			_by_id.درج({id, str});
			أرجع str;
		}
	};
	class Writer {
		std::ostream *os;
		فراغ print_impl(ثابت حرف *fmt, وکتور<std::function<فراغ()>>& fns);
	عام:
		Writer(std::ostream &os) : os(&os) {}
		قالب<class T> Writer& عملگر <<(T&& arg) { *os << std::forward<T>(arg); أرجع *this; }
		قالب<typename... آرگومانها_3>
		فراغ print(ثابت حرف *fmt, آرگومانها_3&&... آرگومانها_2)
		{
			وکتور<std::function<فراغ()>> fns { [&]() { *this << آرگومانها_2; }... };
			print_impl(fmt, fns);
		}
		قالب<typename Fn, typename... آرگومانها_3>
		فراغ print_with(Fn fn, ثابت حرف *fmt, آرگومانها_3&&... آرگومانها_2)
		{
			وکتور<std::function<فراغ()>> fns { [&]() {
				إذا constexpr (std::is_invocable_v<Fn, آرگومانها_3>)
					*this << fn(آرگومانها_2);
				وإلا
					*this << آرگومانها_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_انتها

#پایان_اگر
