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

#که_نه_تعریف_شوی FUNCTIONAL_H
#تعریف_کړه FUNCTIONAL_H

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
		ټوټه,
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
		جمع_کړه,
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
		logical_ښويدنه_کیڼ,
		// logical_shift_right(a: unsigned bit[N], b: unsigned bit[M]): unsigned bit[N] = a >> b
		// required: M == clog2(N)
		logical_ښويدنه_ښی,
		// arithmetic_shift_right(a: signed bit[N], b: unsigned bit[M]): signed bit[N] = a >> b
		// required: M == clog2(N)
		arithmetic_ښويدنه_ښی,
		// mux(a: bit[N], b: bit[N], s: bit[1]): bit[N] = s ? b : a
		mux,
		// constant(a: Const[N]): bit[N] = a
		constant,
		// input(a: IdString): any
		// returns the current value of the input with the specified name
		ننوت,
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
	ثابت حرف *fn_منزل_string(Fn);
	// Sort represents the sort or type of a node
	// currently the only two sorts are signal/bit and memory
	class ترتیبول_2 {
		std::variant<صحيح, std::pair<صحيح, صحيح>> _v;
	عام:
		explicit ترتیبول_2(صحيح پلنوالی) : _v(پلنوالی) { }
		ترتیبول_2(صحيح addr_پلنوالی, صحيح معلومات_پلنوالی) : _v(std::make_pair(addr_پلنوالی, معلومات_پلنوالی)) { }
		bool is_signal() ثابت { أرجع _v.شاخص() == 0; }
		bool is_حافظه() ثابت { أرجع _v.شاخص() == 1; }
		// returns the width of a bitvector sort, errors out for other sorts
		صحيح پلنوالی() ثابت { أرجع std::get<0>(_v); }
		// returns the address width of a bitvector sort, errors out for other sorts
		صحيح addr_پلنوالی() ثابت { أرجع std::get<1>(_v).لومړی; }
		// returns the data width of a bitvector sort, errors out for other sorts
		صحيح معلومات_پلنوالی() ثابت { أرجع std::get<1>(_v).دوهم; }
		bool چلوونکی==(ترتیبول_2 ثابت& other) ثابت { أرجع _v == other._v; }
		[[nodiscard]] Hasher hash_into(Hasher h) ثابت { h.eat(_v); أرجع h; }
	};
	class IR;
	class Factory;
	class غوټه_2;
	class IRInput {
		ملګری class Factory;
	عام:
		PooledName name;
		PooledName ډول;
		ترتیبول_2 ترتیبول;
	خاص:
		IRInput(IR &ir, IdString name, IdString ډول, ترتیبول_2 ترتیبول);
	};
	class IROutput {
		ملګری class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName ډول;
		ترتیبول_2 ترتیبول;
	خاص:
		IROutput(IR &ir, IdString name, IdString ډول, ترتیبول_2 ترتیبول);
	عام:
		غوټه_2 ارزښت() ثابت;
		bool has_ارزښت() ثابت;
		فراغ سېټ_ارزښت(غوټه_2 ارزښت);
	};
	class IRState {
		ملګری class Factory;
		IR &_ir;
	عام:
		PooledName name;
		PooledName ډول;
		ترتیبول_2 ترتیبول;
	خاص:
		std::variant<RTLIL::Const, MemContents> _initial;
		IRState(IR &ir, IdString name, IdString ډول, ترتیبول_2 ترتیبول);
	عام:
		غوټه_2 بل_ارزښت() ثابت;
		bool has_بل_ارزښت() ثابت;
		RTLIL::Const ثابت& initial_ارزښت_signal() ثابت { أرجع std::get<RTLIL::Const>(_initial); }
		MemContents ثابت& initial_ارزښت_حافظه() ثابت { أرجع std::get<MemContents>(_initial); }
		فراغ سېټ_بل_ارزښت(غوټه_2 ارزښت);
		فراغ سېټ_initial_ارزښت(RTLIL::Const ارزښت) { ارزښت.extu(ترتیبول.پلنوالی()); _initial = std::move(ارزښت); }
		فراغ سېټ_initial_ارزښت(MemContents ارزښت) { log_assert(ترتیبول_2(ارزښت.addr_پلنوالی(), ارزښت.معلومات_پلنوالی()) == ترتیبول); _initial = std::move(ارزښت); }
	};
	class IR {
		ملګری class Factory;
		ملګری class غوټه_2;
		ملګری class IRInput;
		ملګری class IROutput;
		ملګری class IRState;
		// one NodeData is stored per Node, containing the function and non-node arguments
		// note that NodeData is deduplicated by ComputeGraph
		class غوټه_معلومات {
			Fn _fn;
			std::variant<
				std::monostate,
				RTLIL::Const,
				std::pair<IdString, IdString>,
				صحيح
			> _extra;
		عام:
			غوټه_معلومات() : _fn(Fn::invalid) {}
			غوټه_معلومات(Fn fn) : _fn(fn) {}
			کينډۍ<class T> غوټه_معلومات(Fn fn, T &&extra) : _fn(fn), _extra(std::forward<T>(extra)) {}
			Fn fn() ثابت { أرجع _fn; }
			ثابت RTLIL::Const &as_const() ثابت { أرجع std::get<RTLIL::Const>(_extra); }
			std::pair<IdString, IdString> as_idstring_pair() ثابت { أرجع std::get<std::pair<IdString, IdString>>(_extra); }
			صحيح as_int() ثابت { أرجع std::get<صحيح>(_extra); }
			[[nodiscard]] Hasher hash_into(Hasher h) ثابت {
				h.eat((بینښه صحيح) _fn);
				h.eat(_extra);
				أرجع h;
			}
			bool چلوونکی==(غوټه_معلومات ثابت &other) ثابت {
				أرجع _fn == other._fn && _extra == other._extra;
			}
		};
		// Attr contains all the information about a note that should not be deduplicated
		بنية Attr {
			ترتیبول_2 ترتیبول;
		};
		// our specialised version of ComputeGraph
		// the sparse_attr IdString stores a naming suggestion, retrieved with name()
		// the key is currently used to identify the nodes that represent output and next state values
		// the bool is true for next state values
		using ګراف = Compute_ګراف<غوټه_معلومات, Attr, IdString, std::tuple<IdString, IdString, bool>>;
		ګراف ګراف_4;
		dict<std::pair<IdString, IdString>, IRInput> _inputs;
		dict<std::pair<IdString, IdString>, IROutput> _outputs;
		dict<std::pair<IdString, IdString>, IRState> حالتونه_2;
		IR::ګراف::Ref mutate(غوټه_2 n);
	عام:
		Design *design = nullptr;
		ساكن IR from_module(Module *module);
		Factory factory();
		صحيح اندازه() ثابت { أرجع ګراف_4.اندازه(); }
		غوټه_2 چلوونکی[](صحيح i);
		فراغ topological_ترتیبول();
		فراغ forward_buf();
		IRInput ثابت& ننوت(IdString name, IdString ډول) ثابت { أرجع _inputs.at({name, ډول}); }
		IRInput ثابت& ننوت(IdString name) ثابت { أرجع ننوت(name, ID($ننوت)); }
		IROutput ثابت& output(IdString name, IdString ډول) ثابت { أرجع _outputs.at({name, ډول}); }
		IROutput ثابت& output(IdString name) ثابت { أرجع output(name, ID($output)); }
		IRState ثابت& state(IdString name, IdString ډول) ثابت { أرجع حالتونه_2.at({name, ډول}); }
		IRState ثابت& state(IdString name) ثابت { أرجع state(name, ID($state)); }
		bool has_ننوت(IdString name, IdString ډول) ثابت { أرجع _inputs.شمېر({name, ډول}); }
		bool has_output(IdString name, IdString ډول) ثابت { أرجع _outputs.شمېر({name, ډول}); }
		bool has_state(IdString name, IdString ډول) ثابت { أرجع حالتونه_2.شمېر({name, ډول}); }
		ویکټور<IRInput ثابت*> inputs(IdString ډول) ثابت;
		ویکټور<IRInput ثابت*> inputs() ثابت { أرجع inputs(ID($ننوت)); }
		ویکټور<IROutput ثابت*> outputs(IdString ډول) ثابت;
		ویکټور<IROutput ثابت*> outputs() ثابت { أرجع outputs(ID($output)); }
		ویکټور<IRState ثابت*> حالتونه(IdString ډول) ثابت;
		ویکټور<IRState ثابت*> حالتونه() ثابت { أرجع حالتونه(ID($state)); }
		ویکټور<IRInput ثابت*> all_inputs() ثابت;
		ویکټور<IROutput ثابت*> all_outputs() ثابت;
		ویکټور<IRState ثابت*> all_حالتونه() ثابت;
		class iterator {
			ملګری class IR;
			IR *_ir;
			صحيح شاخص_3;
			iterator(IR *ir, صحيح شاخص) : _ir(ir), شاخص_3(شاخص) {}
		عام:
			using iterator_category = std::ننوت_iterator_نښان;
			using ارزښت_type = غوټه_2;
			using اشاره_ګر = arrow_proxy<غوټه_2>;
			using reference = غوټه_2;
			using difference_type = ptrdiff_t;
			غوټه_2 چلوونکی*();
			iterator &چلوونکی++() { شاخص_3++; أرجع *this; }
			bool چلوونکی!=(iterator ثابت &other) ثابت { أرجع _ir != other._ir || شاخص_3 != other.شاخص_3; }
			bool چلوونکی==(iterator ثابت &other) ثابت { أرجع !(*this != other); }
			اشاره_ګر چلوونکی->();
		};
		iterator begin() { أرجع iterator(this, 0); }
		iterator پای_ټکی() { أرجع iterator(this, ګراف_4.اندازه()); }
	};
	// Node is an immutable reference to a FunctionalIR node
	class غوټه_2 {
		ملګری class Factory;
		ملګری class IR;
		ملګری class IRInput;
		ملګری class IROutput;
		ملګری class IRState;
		IR::ګراف::ConstRef _ref;
		غوټه_2(IR::ګراف::ConstRef ref, Design *design) : _ref(ref), design(design) { }
		explicit چلوونکی IR::ګراف::ConstRef() { أرجع _ref; }
	عام:
		Design *design = nullptr;
		// the node's index. may change if nodes are added or removed
		صحيح id() ثابت { أرجع _ref.شاخص(); }
		// a name suggestion for the node, which need not be unique
		PooledName name() ثابت {
			إذا(_ref.has_sparse_attr())
				أرجع {design, _ref.sparse_attr()};
			وإلا
				أرجع {design, design->twines().جمع_کړه(std::string("\\n") + std::منزل_string(id()))};
		}
		Fn fn() ثابت { أرجع _ref.function().fn(); }
		ترتیبول_2 ترتیبول() ثابت { أرجع _ref.attr().ترتیبول; }
		// returns the width of a bitvector node, errors out for other nodes
		صحيح پلنوالی() ثابت { أرجع ترتیبول().پلنوالی(); }
		اندازه_t arg_شمېر() ثابت { أرجع _ref.اندازه(); }
		غوټه_2 arg(صحيح n) ثابت { أرجع غوټه_2(_ref.arg(n), design); }
		// visit calls the appropriate visitor method depending on the type of the node
		کينډۍ<class Visitor> خپلکاری لیدنه(Visitor v) ثابت
		{
			// currently templated but could be switched to AbstractVisitor &
			بدل(_ref.function().fn()) {
			حالة Fn::invalid: log_error("invalid node in visit\n"); اكسر;
			حالة Fn::buf: أرجع v.buf(*this, arg(0)); اكسر;
			حالة Fn::ټوټه: أرجع v.ټوټه(*this, arg(0), _ref.function().as_int(), ترتیبول().پلنوالی()); اكسر;
			حالة Fn::zero_extend: أرجع v.zero_extend(*this, arg(0), پلنوالی()); اكسر;
			حالة Fn::sign_extend: أرجع v.sign_extend(*this, arg(0), پلنوالی()); اكسر;
			حالة Fn::concat: أرجع v.concat(*this, arg(0), arg(1)); اكسر;
			حالة Fn::جمع_کړه: أرجع v.جمع_کړه(*this, arg(0), arg(1)); اكسر;
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
			حالة Fn::logical_ښويدنه_کیڼ: أرجع v.logical_ښويدنه_کیڼ(*this, arg(0), arg(1)); اكسر;
			حالة Fn::logical_ښويدنه_ښی: أرجع v.logical_ښويدنه_ښی(*this, arg(0), arg(1)); اكسر;
			حالة Fn::arithmetic_ښويدنه_ښی: أرجع v.arithmetic_ښويدنه_ښی(*this, arg(0), arg(1)); اكسر;
			حالة Fn::mux: أرجع v.mux(*this, arg(0), arg(1), arg(2)); اكسر;
			حالة Fn::constant: أرجع v.constant(*this, _ref.function().as_const()); اكسر;
			حالة Fn::ننوت: أرجع v.ننوت(*this, _ref.function().as_idstring_pair().لومړی, _ref.function().as_idstring_pair().دوهم); اكسر;
			حالة Fn::state: أرجع v.state(*this, _ref.function().as_idstring_pair().لومړی, _ref.function().as_idstring_pair().دوهم); اكسر;
			حالة Fn::حافظه_read: أرجع v.حافظه_read(*this, arg(0), arg(1)); اكسر;
			حالة Fn::حافظه_write: أرجع v.حافظه_write(*this, arg(0), arg(1), arg(2)); اكسر;
			}
			log_abort();
		}
		std::string منزل_string();
		std::string منزل_string(std::function<std::string(غوټه_2)>);
	};
	مضمن IRInput::IRInput(IR &ir, IdString name, IdString ډول, ترتیبول_2 ترتیبول)
		: name(ir.design, name), ډول(ir.design, ډول), ترتیبول(std::move(ترتیبول)) {}
	مضمن IROutput::IROutput(IR &ir, IdString name, IdString ډول, ترتیبول_2 ترتیبول)
		: _ir(ir), name(ir.design, name), ډول(ir.design, ډول), ترتیبول(std::move(ترتیبول)) {}
	مضمن IRState::IRState(IR &ir, IdString name, IdString ډول, ترتیبول_2 ترتیبول)
		: _ir(ir), name(ir.design, name), ډول(ir.design, ډول), ترتیبول(std::move(ترتیبول)) {}
	مضمن IR::ګراف::Ref IR::mutate(غوټه_2 n) { أرجع ګراف_4[n._ref.شاخص()]; }
	مضمن غوټه_2 IR::چلوونکی[](صحيح i) { أرجع غوټه_2(ګراف_4[i], design); }
	مضمن غوټه_2 IROutput::ارزښت() ثابت { أرجع غوټه_2(_ir.ګراف_4({name, ډول, false}), _ir.design); }
	مضمن bool IROutput::has_ارزښت() ثابت { أرجع _ir.ګراف_4.has_کلی({name, ډول, false}); }
	مضمن فراغ IROutput::سېټ_ارزښت(غوټه_2 ارزښت) { log_assert(ترتیبول == ارزښت.ترتیبول()); _ir.mutate(ارزښت).ټاکل_کلی({name, ډول, false}); }
	مضمن غوټه_2 IRState::بل_ارزښت() ثابت { أرجع غوټه_2(_ir.ګراف_4({name, ډول, true}), _ir.design); }
	مضمن bool IRState::has_بل_ارزښت() ثابت { أرجع _ir.ګراف_4.has_کلی({name, ډول, true}); }
	مضمن فراغ IRState::سېټ_بل_ارزښت(غوټه_2 ارزښت) { log_assert(ترتیبول == ارزښت.ترتیبول()); _ir.mutate(ارزښت).ټاکل_کلی({name, ډول, true}); }
	مضمن غوټه_2 IR::iterator::چلوونکی*() { أرجع غوټه_2(_ir->ګراف_4[شاخص_3], _ir->design); }
	مضمن arrow_proxy<غوټه_2> IR::iterator::چلوونکی->() { أرجع arrow_proxy<غوټه_2>(**this); }
	// AbstractVisitor provides an abstract base class for visitors
	کينډۍ<class T> بنية AbstractVisitor {
		مجازي T buf(غوټه_2 self, غوټه_2 n) = 0;
		مجازي T ټوټه(غوټه_2 self, غوټه_2 a, صحيح دننه_واټن, صحيح بهر_پلنوالی) = 0;
		مجازي T zero_extend(غوټه_2 self, غوټه_2 a, صحيح بهر_پلنوالی) = 0;
		مجازي T sign_extend(غوټه_2 self, غوټه_2 a, صحيح بهر_پلنوالی) = 0;
		مجازي T concat(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T جمع_کړه(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T sub(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T mul(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T unsigned_div(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T unsigned_mod(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T bitwise_and(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T bitwise_or(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T bitwise_xor(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T bitwise_not(غوټه_2 self, غوټه_2 a) = 0;
		مجازي T unary_minus(غوټه_2 self, غوټه_2 a) = 0;
		مجازي T reduce_and(غوټه_2 self, غوټه_2 a) = 0;
		مجازي T reduce_or(غوټه_2 self, غوټه_2 a) = 0;
		مجازي T reduce_xor(غوټه_2 self, غوټه_2 a) = 0;
		مجازي T equal(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T not_equal(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T signed_greater_than(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T signed_greater_equal(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T unsigned_greater_than(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T unsigned_greater_equal(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T logical_ښويدنه_کیڼ(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T logical_ښويدنه_ښی(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T arithmetic_ښويدنه_ښی(غوټه_2 self, غوټه_2 a, غوټه_2 b) = 0;
		مجازي T mux(غوټه_2 self, غوټه_2 a, غوټه_2 b, غوټه_2 s) = 0;
		مجازي T constant(غوټه_2 self, RTLIL::Const ثابت & ارزښت) = 0;
		مجازي T ننوت(غوټه_2 self, IdString name, IdString ډول) = 0;
		مجازي T state(غوټه_2 self, IdString name, IdString ډول) = 0;
		مجازي T حافظه_read(غوټه_2 self, غوټه_2 mem, غوټه_2 addr) = 0;
		مجازي T حافظه_write(غوټه_2 self, غوټه_2 mem, غوټه_2 addr, غوټه_2 معلومات) = 0;
	};
	// DefaultVisitor provides defaults for all visitor methods which just calls default_handler
	کينډۍ<class T> بنية DefaultVisitor : عام AbstractVisitor<T> {
		مجازي T default_handler(غوټه_2 self) = 0;
		T buf(غوټه_2 self, غوټه_2) override { أرجع default_handler(self); }
		T ټوټه(غوټه_2 self, غوټه_2, صحيح, صحيح) override { أرجع default_handler(self); }
		T zero_extend(غوټه_2 self, غوټه_2, صحيح) override { أرجع default_handler(self); }
		T sign_extend(غوټه_2 self, غوټه_2, صحيح) override { أرجع default_handler(self); }
		T concat(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T جمع_کړه(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T sub(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T mul(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T unsigned_div(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T unsigned_mod(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T bitwise_and(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T bitwise_or(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T bitwise_xor(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T bitwise_not(غوټه_2 self, غوټه_2) override { أرجع default_handler(self); }
		T unary_minus(غوټه_2 self, غوټه_2) override { أرجع default_handler(self); }
		T reduce_and(غوټه_2 self, غوټه_2) override { أرجع default_handler(self); }
		T reduce_or(غوټه_2 self, غوټه_2) override { أرجع default_handler(self); }
		T reduce_xor(غوټه_2 self, غوټه_2) override { أرجع default_handler(self); }
		T equal(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T not_equal(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T signed_greater_than(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T signed_greater_equal(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T unsigned_greater_than(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T unsigned_greater_equal(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T logical_ښويدنه_کیڼ(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T logical_ښويدنه_ښی(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T arithmetic_ښويدنه_ښی(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T mux(غوټه_2 self, غوټه_2, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T constant(غوټه_2 self, RTLIL::Const ثابت &) override { أرجع default_handler(self); }
		T ننوت(غوټه_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T state(غوټه_2 self, IdString, IdString) override { أرجع default_handler(self); }
		T حافظه_read(غوټه_2 self, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
		T حافظه_write(غوټه_2 self, غوټه_2, غوټه_2, غوټه_2) override { أرجع default_handler(self); }
	};
	// a factory is used to modify a FunctionalIR. it creates new nodes and allows for some modification of existing nodes.
	class Factory {
		ملګری class IR;
		IR &_ir;
		explicit Factory(IR &ir) : _ir(ir) {}
		غوټه_2 جمع_کړه(IR::غوټه_معلومات &&fn, ترتیبول_2 ثابت &ترتیبول, std::initializer_list<غوټه_2> دلیلونه_2) {
			log_assert(!ترتیبول.is_signal() || ترتیبول.پلنوالی() > 0);
			log_assert(!ترتیبول.is_حافظه() || (ترتیبول.addr_پلنوالی() > 0 && ترتیبول.معلومات_پلنوالی() > 0));
			IR::ګراف::Ref ref = _ir.ګراف_4.جمع_کړه(std::move(fn), {std::move(ترتیبول)});
			لكل (خپلکاری arg : دلیلونه_2)
				ref.append_arg(IR::ګراف::ConstRef(arg));
			أرجع غوټه_2(ref, _ir.design);
		}
		فراغ check_basic_دوګونی(غوټه_2 ثابت &a, غوټه_2 ثابت &b) { log_assert(a.ترتیبول().is_signal() && a.ترتیبول() == b.ترتیبول()); }
		فراغ check_ښويدنه(غوټه_2 ثابت &a, غوټه_2 ثابت &b) { log_assert(a.ترتیبول().is_signal() && b.ترتیبول().is_signal() && b.پلنوالی() == ceil_log2(a.پلنوالی())); }
		فراغ check_unary(غوټه_2 ثابت &a) { log_assert(a.ترتیبول().is_signal()); }
	عام:
		IR &ir() { أرجع _ir; }
		غوټه_2 ټوټه(غوټه_2 a, صحيح دننه_واټن, صحيح بهر_پلنوالی) {
			log_assert(a.ترتیبول().is_signal() && دننه_واټن + بهر_پلنوالی <= a.ترتیبول().پلنوالی());
			إذا(دننه_واټن == 0 && بهر_پلنوالی == a.پلنوالی())
				أرجع a;
			أرجع جمع_کړه(IR::غوټه_معلومات(Fn::ټوټه, دننه_واټن), ترتیبول_2(بهر_پلنوالی), {a});
		}
		// extend will either extend or truncate the provided value to reach the desired width
		غوټه_2 extend(غوټه_2 a, صحيح بهر_پلنوالی, bool is_signed) {
			صحيح دننه_پلنوالی = a.ترتیبول().پلنوالی();
			log_assert(a.ترتیبول().is_signal());
			إذا(دننه_پلنوالی == بهر_پلنوالی)
				أرجع a;
			إذا(دننه_پلنوالی > بهر_پلنوالی)
				أرجع ټوټه(a, 0, بهر_پلنوالی);
			إذا(is_signed)
				أرجع جمع_کړه(Fn::sign_extend, ترتیبول_2(بهر_پلنوالی), {a});
			وإلا
				أرجع جمع_کړه(Fn::zero_extend, ترتیبول_2(بهر_پلنوالی), {a});
		}
		غوټه_2 concat(غوټه_2 a, غوټه_2 b) {
			log_assert(a.ترتیبول().is_signal() && b.ترتیبول().is_signal());
			أرجع جمع_کړه(Fn::concat, ترتیبول_2(a.ترتیبول().پلنوالی() + b.ترتیبول().پلنوالی()), {a, b});
		}
		غوټه_2 جمع_کړه(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::جمع_کړه, a.ترتیبول(), {a, b}); }
		غوټه_2 sub(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::sub, a.ترتیبول(), {a, b}); }
		غوټه_2 mul(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::mul, a.ترتیبول(), {a, b}); }
		غوټه_2 unsigned_div(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::unsigned_div, a.ترتیبول(), {a, b}); }
		غوټه_2 unsigned_mod(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::unsigned_mod, a.ترتیبول(), {a, b}); }
		غوټه_2 bitwise_and(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::bitwise_and, a.ترتیبول(), {a, b}); }
		غوټه_2 bitwise_or(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::bitwise_or, a.ترتیبول(), {a, b}); }
		غوټه_2 bitwise_xor(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::bitwise_xor, a.ترتیبول(), {a, b}); }
		غوټه_2 bitwise_not(غوټه_2 a) { check_unary(a); أرجع جمع_کړه(Fn::bitwise_not, a.ترتیبول(), {a}); }
		غوټه_2 unary_minus(غوټه_2 a) { check_unary(a); أرجع جمع_کړه(Fn::unary_minus, a.ترتیبول(), {a}); }
		غوټه_2 reduce_and(غوټه_2 a) {
			check_unary(a);
			إذا(a.پلنوالی() == 1)
				أرجع a;
			أرجع جمع_کړه(Fn::reduce_and, ترتیبول_2(1), {a});
		}
		غوټه_2 reduce_or(غوټه_2 a) {
			check_unary(a);
			إذا(a.پلنوالی() == 1)
				أرجع a;
			أرجع جمع_کړه(Fn::reduce_or, ترتیبول_2(1), {a});
		}
		غوټه_2 reduce_xor(غوټه_2 a) {
			check_unary(a);
			إذا(a.پلنوالی() == 1)
				أرجع a;
			أرجع جمع_کړه(Fn::reduce_xor, ترتیبول_2(1), {a});
		}
		غوټه_2 equal(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::equal, ترتیبول_2(1), {a, b}); }
		غوټه_2 not_equal(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::not_equal, ترتیبول_2(1), {a, b}); }
		غوټه_2 signed_greater_than(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::signed_greater_than, ترتیبول_2(1), {a, b}); }
		غوټه_2 signed_greater_equal(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::signed_greater_equal, ترتیبول_2(1), {a, b}); }
		غوټه_2 unsigned_greater_than(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::unsigned_greater_than, ترتیبول_2(1), {a, b}); }
		غوټه_2 unsigned_greater_equal(غوټه_2 a, غوټه_2 b) { check_basic_دوګونی(a, b); أرجع جمع_کړه(Fn::unsigned_greater_equal, ترتیبول_2(1), {a, b}); }
		غوټه_2 logical_ښويدنه_کیڼ(غوټه_2 a, غوټه_2 b) { check_ښويدنه(a, b); أرجع جمع_کړه(Fn::logical_ښويدنه_کیڼ, a.ترتیبول(), {a, b}); }
		غوټه_2 logical_ښويدنه_ښی(غوټه_2 a, غوټه_2 b) { check_ښويدنه(a, b); أرجع جمع_کړه(Fn::logical_ښويدنه_ښی, a.ترتیبول(), {a, b}); }
		غوټه_2 arithmetic_ښويدنه_ښی(غوټه_2 a, غوټه_2 b) { check_ښويدنه(a, b); أرجع جمع_کړه(Fn::arithmetic_ښويدنه_ښی, a.ترتیبول(), {a, b}); }
		غوټه_2 mux(غوټه_2 a, غوټه_2 b, غوټه_2 s) {
			log_assert(a.ترتیبول().is_signal() && a.ترتیبول() == b.ترتیبول() && s.ترتیبول() == ترتیبول_2(1));
			أرجع جمع_کړه(Fn::mux, a.ترتیبول(), {a, b, s});
		}
		غوټه_2 حافظه_read(غوټه_2 mem, غوټه_2 addr) {
			log_assert(mem.ترتیبول().is_حافظه() && addr.ترتیبول().is_signal() && mem.ترتیبول().addr_پلنوالی() == addr.ترتیبول().پلنوالی());
			أرجع جمع_کړه(Fn::حافظه_read, ترتیبول_2(mem.ترتیبول().معلومات_پلنوالی()), {mem, addr});
		}
		غوټه_2 حافظه_write(غوټه_2 mem, غوټه_2 addr, غوټه_2 معلومات) {
			log_assert(mem.ترتیبول().is_حافظه() && addr.ترتیبول().is_signal() && معلومات.ترتیبول().is_signal() &&
				mem.ترتیبول().addr_پلنوالی() == addr.ترتیبول().پلنوالی() && mem.ترتیبول().معلومات_پلنوالی() == معلومات.ترتیبول().پلنوالی());
			أرجع جمع_کړه(Fn::حافظه_write, mem.ترتیبول(), {mem, addr, معلومات});
		}
		غوټه_2 constant(RTLIL::Const ارزښت) {
			صحيح s = ارزښت.اندازه();
			أرجع جمع_کړه(IR::غوټه_معلومات(Fn::constant, std::move(ارزښت)), ترتیبول_2(s), {});
		}
		غوټه_2 create_pending(صحيح پلنوالی) {
			أرجع جمع_کړه(Fn::buf, ترتیبول_2(پلنوالی), {});
		}
		فراغ update_pending(غوټه_2 غوټه, غوټه_2 ارزښت) {
			log_assert(غوټه._ref.function() == Fn::buf && غوټه._ref.اندازه() == 0);
			log_assert(غوټه.ترتیبول() == ارزښت.ترتیبول());
			_ir.mutate(غوټه).append_arg(ارزښت._ref);
		}
		IRInput &جمع_کړه_ننوت(IdString name, IdString ډول, ترتیبول_2 ترتیبول) {
			خپلکاری [it, inserted] = _ir._inputs.emplace({name, ډول}, IRInput(_ir, name, ډول, std::move(ترتیبول)));
			إذا (!inserted) log_error("input `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دوهم;
		}
		IROutput &جمع_کړه_output(IdString name, IdString ډول, ترتیبول_2 ترتیبول) {
			خپلکاری [it, inserted] = _ir._outputs.emplace({name, ډول}, IROutput(_ir, name, ډول, std::move(ترتیبول)));
			إذا (!inserted) log_error("output `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دوهم;
		}
		IRState &جمع_کړه_state(IdString name, IdString ډول, ترتیبول_2 ترتیبول) {
			خپلکاری [it, inserted] = _ir.حالتونه_2.emplace({name, ډول}, IRState(_ir, name, ډول, std::move(ترتیبول)));
			إذا (!inserted) log_error("state `%s` was re-defined\n", _ir.design->twines().str(name));
			أرجع it->دوهم;
		}
		غوټه_2 ارزښت(IRInput ثابت& ننوت) {
			أرجع جمع_کړه(IR::غوټه_معلومات(Fn::ننوت, std::pair(ننوت.name, ننوت.ډول)), ننوت.ترتیبول, {});
		}
		غوټه_2 ارزښت(IRState ثابت& state) {
			أرجع جمع_کړه(IR::غوټه_معلومات(Fn::state, std::pair(state.name, state.ډول)), state.ترتیبول, {});
		}
		فراغ suggest_name(غوټه_2 غوټه, IdString name) {
			_ir.mutate(غوټه).sparse_attr() = name;
		}
		YS_NAME_FWD_2ND_POOL(suggest_name, _ir.design->twines())
	};
	مضمن Factory IR::factory() { أرجع Factory(*this); }
	کينډۍ<class Id> class Scope {
	محمي:
		حرف substitution_character = '_';
		مجازي bool is_character_legal(حرف, صحيح) = 0;
	خاص:
		pool<std::string> کارول_شوی_نومونه;
		dict<Id, std::string> _by_id;
	عام:
		فراغ reserve(std::string name) {
			کارول_شوی_نومونه.ورننباسه(std::move(name));
		}
		کينډۍ<typename N> std::string unique_name(ثابت N &suggestion) {
			std::string str = RTLIL::unescape_id(suggestion);
			لكل(اندازه_t i = 0; i < str.اندازه(); i++)
				إذا(!is_character_legal(str[i], i))
					str[i] = substitution_character;
			إذا(کارول_شوی_نومونه.شمېر(str) == 0) {
				کارول_شوی_نومونه.ورننباسه(str);
				أرجع str;
			}
			لكل (صحيح idx = 0 ; ; idx++){
				std::string suffixed = str + "_" + std::منزل_string(idx);
				إذا(کارول_شوی_نومونه.شمېر(suffixed) == 0) {
					کارول_شوی_نومونه.ورننباسه(suffixed);
					أرجع suffixed;
				}
			}
		}
		کينډۍ<typename N> std::string چلوونکی()(Id id, ثابت N &suggestion) {
			خپلکاری it = _by_id.پیدا_کړه(id);
			إذا(it != _by_id.پای_ټکی())
				أرجع it->دوهم;
			std::string str = unique_name(suggestion);
			_by_id.ورننباسه({id, str});
			أرجع str;
		}
	};
	class Writer {
		std::ostream *os;
		فراغ print_impl(ثابت حرف *fmt, ویکټور<std::function<فراغ()>>& fns);
	عام:
		Writer(std::ostream &os) : os(&os) {}
		کينډۍ<class T> Writer& چلوونکی <<(T&& arg) { *os << std::forward<T>(arg); أرجع *this; }
		کينډۍ<typename... دلیلونه_3>
		فراغ print(ثابت حرف *fmt, دلیلونه_3&&... دلیلونه_2)
		{
			ویکټور<std::function<فراغ()>> fns { [&]() { *this << دلیلونه_2; }... };
			print_impl(fmt, fns);
		}
		کينډۍ<typename Fn, typename... دلیلونه_3>
		فراغ print_with(Fn fn, ثابت حرف *fmt, دلیلونه_3&&... دلیلونه_2)
		{
			ویکټور<std::function<فراغ()>> fns { [&]() {
				إذا constexpr (std::is_invocable_v<Fn, دلیلونه_3>)
					*this << fn(دلیلونه_2);
				وإلا
					*this << دلیلونه_2; }...
			};
			print_impl(fmt, fns);
		}
	};

}

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
