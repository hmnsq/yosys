/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#ifndef CELLTYPES_H
#define CELLTYPES_H

#অন্তর্ভাৱ "kernel/yosys.h"
#অন্তর্ভাৱ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

সংরচনা CellType
{
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

সংরচনা CellTypes
{
	dict<RTLIL::IdString, CellType> cell_types;

	CellTypes()
	{
	}

	CellTypes(RTLIL::Design *design)
	{
		setup(design);
	}

	শূন্য setup(RTLIL::Design *design = রিক্ত)
	{
		যদি (design)
			setup_design(design);

		setup_internals();
		setup_internals_mem();
		setup_internals_anyinit();
		setup_stdcells();
		setup_stdcells_mem();
	}

	শূন্য setup_type(RTLIL::IdString type, ধ্রুৱ pool<RTLIL::IdString> &inputs, ধ্রুৱ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		CellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		cell_types[ct.type] = ct;
	}

	template<typename N, YS_NAME_STRING(N)>
	শূন্য setup_type(ধ্রুৱ N &type_str, ধ্রুৱ pool<RTLIL::IdString> &inputs, ধ্রুৱ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		setup_type(ID::lookup(type_str), inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable);
	}

	শূন্য setup_module(RTLIL::Module *module)
	{
		pool<IdString> inputs, outputs;
		বাবে (auto wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			যদি (wire->port_প্ৰবেশ)
				inputs.ঢোকাও(wire->name);
			যদি (wire->port_output)
				outputs.ঢোকাও(wire->name);
		}
		setup_type(module->name, inputs, outputs);
	}

	শূন্য setup_design(RTLIL::Design *design)
	{
		বাবে (auto module : design->modules())
			setup_module(module);
	}

	শূন্য setup_internals()
	{
		setup_internals_eval();

		setup_type(ID($barrier), {ID::A}, {ID::Y});

		setup_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y});

		setup_type(ID($প্রতিজ্ঞা), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		setup_type(ID($assume), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		setup_type(ID($live), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		setup_type(ID($fair), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		setup_type(ID($cover), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		setup_type(ID($initstate), pool<RTLIL::IdString>(), {ID::Y});
		setup_type(ID($anyconst), pool<RTLIL::IdString>(), {ID::Y});
		setup_type(ID($anyseq), pool<RTLIL::IdString>(), {ID::Y});
		setup_type(ID($allconst), pool<RTLIL::IdString>(), {ID::Y});
		setup_type(ID($allseq), pool<RTLIL::IdString>(), {ID::Y});
		setup_type(ID($equiv), {ID::A, ID::B}, {ID::Y});
		setup_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		setup_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, pool<RTLIL::IdString>());
		setup_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		setup_type(ID($print), {ID::EN, ID::ARGS, ID::TRG}, pool<RTLIL::IdString>());
		setup_type(ID($check), {ID::A, ID::EN, ID::ARGS, ID::TRG}, pool<RTLIL::IdString>());
		setup_type(ID($set_tag), {ID::A, ID::SET, ID::CLR}, {ID::Y});
		setup_type(ID($get_tag), {ID::A}, {ID::Y});
		setup_type(ID($overwrite_tag), {ID::A, ID::SET, ID::CLR}, pool<RTLIL::IdString>());
		setup_type(ID($original_tag), {ID::A}, {ID::Y});
		setup_type(ID($future_ff), {ID::A}, {ID::Y});
		setup_type(ID($scopeinfo), {}, {});
		setup_type(ID($প্ৰবেশ_port), {}, {ID::Y});
		setup_type(ID($connect), {ID::A, ID::B}, {});
	}

	শূন্য setup_internals_eval()
	{
		std::vector<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($slice), ID($lut), ID($sop)
		};

		std::vector<RTLIL::IdString> দ্বিমিক_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		বাবে (auto type : unary_ops)
			setup_type(type, {ID::A}, {ID::Y}, true);

		বাবে (auto type : দ্বিমিক_ops)
			setup_type(type, {ID::A, ID::B}, {ID::Y}, true);

		বাবে (auto type : std::vector<RTLIL::IdString>({ID($mux), ID($pmux), ID($bwmux)}))
			setup_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, true);

		বাবে (auto type : std::vector<RTLIL::IdString>({ID($bmux), ID($demux)}))
			setup_type(type, {ID::A, ID::S}, {ID::Y}, true);

		setup_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, true);
		setup_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, true);
		setup_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		setup_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, true);
	}

	শূন্য setup_internals_ff()
	{
		setup_type(ID($sr), {ID::SET, ID::CLR}, {ID::Q});
		setup_type(ID($ff), {ID::D}, {ID::Q});
		setup_type(ID($dff), {ID::CLK, ID::D}, {ID::Q});
		setup_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q});
		setup_type(ID($dffsr), {ID::CLK, ID::SET, ID::CLR, ID::D}, {ID::Q});
		setup_type(ID($dffsre), {ID::CLK, ID::SET, ID::CLR, ID::D, ID::EN}, {ID::Q});
		setup_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q});
		setup_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q});
		setup_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q});
		setup_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q});
		setup_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q});
		setup_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		setup_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		setup_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q});
		setup_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q});
		setup_type(ID($dlatchsr), {ID::EN, ID::SET, ID::CLR, ID::D}, {ID::Q});
	}

	শূন্য setup_internals_anyinit()
	{
		setup_type(ID($anyinit), {ID::D}, {ID::Q});
	}

	শূন্য setup_internals_mem()
	{
		setup_internals_ff();

		setup_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::DATA});
		setup_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::DATA});
		setup_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::DATA}, pool<RTLIL::IdString>());
		setup_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::DATA}, pool<RTLIL::IdString>());
		setup_type(ID($meminit), {ID::ADDR, ID::DATA}, pool<RTLIL::IdString>());
		setup_type(ID($meminit_v2), {ID::ADDR, ID::DATA, ID::EN}, pool<RTLIL::IdString>());
		setup_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_DATA}, {ID::RD_DATA});
		setup_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_DATA}, {ID::RD_DATA});

		setup_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_IN}, {ID::CTRL_OUT});
	}

	শূন্য setup_stdcells()
	{
		setup_stdcells_eval();

		setup_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y});
	}

	শূন্য setup_stdcells_eval()
	{
		setup_type(ID($_BUF_), {ID::A}, {ID::Y}, true);
		setup_type(ID($_NOT_), {ID::A}, {ID::Y}, true);
		setup_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, true);
		setup_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		setup_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		setup_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, true);
		setup_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, true);
		setup_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, true);
		setup_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		setup_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		setup_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
		setup_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
	}

	শূন্য setup_stdcells_mem()
	{
		std::vector<ৱর্ণ> list_np = {'N', 'P'}, list_01 = {'0', '1'};

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
			setup_type(stringf("$_SR_%c%c_", c1, c2), {ID::S, ID::R}, {ID::Q});

		setup_type(ID($_FF_), {ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
			setup_type(stringf("$_DFF_%c_", c1), {ID::C, ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
			setup_type(stringf("$_DFFE_%c%c_", c1, c2), {ID::C, ID::D, ID::E}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_01)
			setup_type(stringf("$_DFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_01)
		বাবে (auto c4 : list_np)
			setup_type(stringf("$_DFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
			setup_type(stringf("$_ALDFF_%c%c_", c1, c2), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_np)
			setup_type(stringf("$_ALDFFE_%c%c%c_", c1, c2, c3), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_np)
			setup_type(stringf("$_DFFSR_%c%c%c_", c1, c2, c3), {ID::C, ID::S, ID::R, ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_np)
		বাবে (auto c4 : list_np)
			setup_type(stringf("$_DFFSRE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_01)
			setup_type(stringf("$_SDFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_01)
		বাবে (auto c4 : list_np)
			setup_type(stringf("$_SDFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_01)
		বাবে (auto c4 : list_np)
			setup_type(stringf("$_SDFFCE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		বাবে (auto c1 : list_np)
			setup_type(stringf("$_DLATCH_%c_", c1), {ID::E, ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_01)
			setup_type(stringf("$_DLATCH_%c%c%c_", c1, c2, c3), {ID::E, ID::R, ID::D}, {ID::Q});

		বাবে (auto c1 : list_np)
		বাবে (auto c2 : list_np)
		বাবে (auto c3 : list_np)
			setup_type(stringf("$_DLATCHSR_%c%c%c_", c1, c2, c3), {ID::E, ID::S, ID::R, ID::D}, {ID::Q});
	}

	শূন্য clear()
	{
		cell_types.clear();
	}

	bool cell_known(RTLIL::IdString type) ধ্রুৱ
	{
		ঘূৰাই cell_types.গুনতি(type) != 0;
	}

	bool cell_output(RTLIL::IdString type, RTLIL::IdString port) ধ্রুৱ
	{
		auto it = cell_types.find(type);
		ঘূৰাই it != cell_types.end() && it->second.outputs.গুনতি(port) != 0;
	}

	bool cell_প্ৰবেশ(RTLIL::IdString type, RTLIL::IdString port) ধ্রুৱ
	{
		auto it = cell_types.find(type);
		ঘূৰাই it != cell_types.end() && it->second.inputs.গুনতি(port) != 0;
	}

	RTLIL::PortDir cell_port_dir(RTLIL::IdString type, RTLIL::IdString port) ধ্রুৱ
	{
		auto it = cell_types.find(type);
		যদি (it == cell_types.end())
			ঘূৰাই RTLIL::PD_UNKNOWN;
		bool is_প্ৰবেশ = it->second.inputs.গুনতি(port);
		bool is_output = it->second.outputs.গুনতি(port);
		ঘূৰাই RTLIL::PortDir(is_প্ৰবেশ + is_output * 2);
	}

	bool cell_evaluable(RTLIL::IdString type) ধ্রুৱ
	{
		auto it = cell_types.find(type);
		ঘূৰাই it != cell_types.end() && it->second.is_evaluable;
	}

	স্থির RTLIL::Const eval_not(RTLIL::Const v)
	{
		বাবে (auto bit : v)
			যদি (bit == State::S0) bit = State::S1;
			অন্যথা যদি (bit == State::S1) bit = State::S0;
		ঘূৰাই v;
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	স্থির RTLIL::Const eval(RTLIL::IdString type, ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, bool signed1, bool signed2, পূর্ণাঙ্ক result_len, bool *errp = nullptr)
	{
		যদি (type == ID($sshr) && !signed1)
			type = ID($shr);
		যদি (type == ID($sshl) && !signed1)
			type = ID($shl);

		যদি (type != ID($sshr) && type != ID($sshl) && type != ID($shr) && type != ID($shl) && type != ID($shift) && type != ID($shiftx) &&
				type != ID($pos) && type != ID($buf) && type != ID($neg) && type != ID($not)) {
			যদি (!signed1 || !signed2)
				signed1 = false, signed2 = false;
		}

#define HANDLE_CELL_TYPE(_t) যদি (type == constid<ID::lookup("$" #_t).raw()>) ঘূৰাই const_ ## _t(arg1, arg2, signed1, signed2, result_len);
		HANDLE_CELL_TYPE(not)
		HANDLE_CELL_TYPE(and)
		HANDLE_CELL_TYPE(or)
		HANDLE_CELL_TYPE(xor)
		HANDLE_CELL_TYPE(xnor)
		HANDLE_CELL_TYPE(reduce_and)
		HANDLE_CELL_TYPE(reduce_or)
		HANDLE_CELL_TYPE(reduce_xor)
		HANDLE_CELL_TYPE(reduce_xnor)
		HANDLE_CELL_TYPE(reduce_bool)
		HANDLE_CELL_TYPE(logic_not)
		HANDLE_CELL_TYPE(logic_and)
		HANDLE_CELL_TYPE(logic_or)
		HANDLE_CELL_TYPE(shl)
		HANDLE_CELL_TYPE(shr)
		HANDLE_CELL_TYPE(sshl)
		HANDLE_CELL_TYPE(sshr)
		HANDLE_CELL_TYPE(shift)
		HANDLE_CELL_TYPE(shiftx)
		HANDLE_CELL_TYPE(lt)
		HANDLE_CELL_TYPE(le)
		HANDLE_CELL_TYPE(eq)
		HANDLE_CELL_TYPE(ne)
		HANDLE_CELL_TYPE(eqx)
		HANDLE_CELL_TYPE(nex)
		HANDLE_CELL_TYPE(ge)
		HANDLE_CELL_TYPE(gt)
		HANDLE_CELL_TYPE(add)
		HANDLE_CELL_TYPE(sub)
		HANDLE_CELL_TYPE(mul)
		HANDLE_CELL_TYPE(div)
		HANDLE_CELL_TYPE(mod)
		HANDLE_CELL_TYPE(divfloor)
		HANDLE_CELL_TYPE(modfloor)
		HANDLE_CELL_TYPE(pow)
		HANDLE_CELL_TYPE(pos)
		HANDLE_CELL_TYPE(neg)
#undef HANDLE_CELL_TYPE

		যদি (type.in(ID($_BUF_), ID($buf)))
			ঘূৰাই arg1;
		যদি (type == ID($_NOT_))
			ঘূৰাই eval_not(arg1);
		যদি (type == ID($_AND_))
			ঘূৰাই const_and(arg1, arg2, false, false, 1);
		যদি (type == ID($_NAND_))
			ঘূৰাই eval_not(const_and(arg1, arg2, false, false, 1));
		যদি (type == ID($_OR_))
			ঘূৰাই const_or(arg1, arg2, false, false, 1);
		যদি (type == ID($_NOR_))
			ঘূৰাই eval_not(const_or(arg1, arg2, false, false, 1));
		যদি (type == ID($_XOR_))
			ঘূৰাই const_xor(arg1, arg2, false, false, 1);
		যদি (type == ID($_XNOR_))
			ঘূৰাই const_xnor(arg1, arg2, false, false, 1);
		যদি (type == ID($_ANDNOT_))
			ঘূৰাই const_and(arg1, eval_not(arg2), false, false, 1);
		যদি (type == ID($_ORNOT_))
			ঘূৰাই const_or(arg1, eval_not(arg2), false, false, 1);

		যদি (errp != nullptr) {
			*errp = true;
			ঘূৰাই State::Sm;
		}

		log_abort();
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	স্থির RTLIL::Const eval(RTLIL::Cell *cell, ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, bool *errp = nullptr)
	{
		যদি (cell->type == ID($slice)) {
			পূর্ণাঙ্ক width = cell->parameters.at(ID::Y_WIDTH).as_int();
			পূর্ণাঙ্ক offset = cell->parameters.at(ID::OFFSET).as_int();
			ঘূৰাই arg1.extract(offset, width);
		}

		যদি (cell->type == ID($concat)) {
			RTLIL::Const ret = arg1;
			ret.append(arg2);
			ঘূৰাই ret;
		}

		যদি (cell->type == ID($bmux))
		{
			ঘূৰাই const_bmux(arg1, arg2);
		}

		যদি (cell->type == ID($demux))
		{
			ঘূৰাই const_demux(arg1, arg2);
		}

		যদি (cell->type == ID($bweqx))
		{
			ঘূৰাই const_bweqx(arg1, arg2);
		}

		যদি (cell->type == ID($lut))
		{
			পূর্ণাঙ্ক width = cell->parameters.at(ID::WIDTH).as_int();

			std::vector<RTLIL::State> t = cell->parameters.at(ID::LUT).to_bits();
			যেতিয়ালৈ (GetSize(t) < (1 << width))
				t.ঠেলো_back(State::S0);
			t.resize(1 << width);

			ঘূৰাই const_bmux(t, arg1);
		}

		যদি (cell->type == ID($sop))
		{
			পূর্ণাঙ্ক width = cell->parameters.at(ID::WIDTH).as_int();
			পূর্ণাঙ্ক depth = cell->parameters.at(ID::DEPTH).as_int();
			std::vector<RTLIL::State> t = cell->parameters.at(ID::TABLE).to_bits();

			যেতিয়ালৈ (GetSize(t) < width*depth*2)
				t.ঠেলো_back(State::S0);

			RTLIL::State default_ret = State::S0;

			বাবে (পূর্ণাঙ্ক i = 0; i < depth; i++)
			{
				bool match = true;
				bool match_x = true;

				বাবে (পূর্ণাঙ্ক j = 0; j < width; j++) {
					RTLIL::State a = arg1.at(j);
					যদি (t.at(2*width*i + 2*j + 0) == State::S1) {
						যদি (a == State::S1) match_x = false;
						যদি (a != State::S0) match = false;
					}
					যদি (t.at(2*width*i + 2*j + 1) == State::S1) {
						যদি (a == State::S0) match_x = false;
						যদি (a != State::S1) match = false;
					}
				}

				যদি (match)
					ঘূৰাই State::S1;

				যদি (match_x)
					default_ret = State::Sx;
			}

			ঘূৰাই default_ret;
		}

		bool signed_a = cell->parameters.গুনতি(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
		bool signed_b = cell->parameters.গুনতি(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();
		পূর্ণাঙ্ক result_len = cell->parameters.গুনতি(ID::Y_WIDTH) > 0 ? cell->parameters[ID::Y_WIDTH].as_int() : -1;
		ঘূৰাই eval(cell->type, arg1, arg2, signed_a, signed_b, result_len, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	স্থির RTLIL::Const eval(RTLIL::Cell *cell, ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, ধ্রুৱ RTLIL::Const &arg3, bool *errp = nullptr)
	{
		যদি (cell->type.in(ID($mux), ID($_MUX_)))
			ঘূৰাই const_mux(arg1, arg2, arg3);
		যদি (cell->type == ID($_NMUX_))
			ঘূৰাই eval_not(const_mux(arg1, arg2, arg3));
		যদি (cell->type == ID($bwmux))
			ঘূৰাই const_bwmux(arg1, arg2, arg3);
		যদি (cell->type == ID($pmux))
			ঘূৰাই const_pmux(arg1, arg2, arg3);
		যদি (cell->type == ID($_AOI3_))
			ঘূৰাই eval_not(const_or(const_and(arg1, arg2, false, false, 1), arg3, false, false, 1));
		যদি (cell->type == ID($_OAI3_))
			ঘূৰাই eval_not(const_and(const_or(arg1, arg2, false, false, 1), arg3, false, false, 1));

		log_assert(arg3.size() == 0);
		ঘূৰাই eval(cell, arg1, arg2, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	স্থির RTLIL::Const eval(RTLIL::Cell *cell, ধ্রুৱ RTLIL::Const &arg1, ধ্রুৱ RTLIL::Const &arg2, ধ্রুৱ RTLIL::Const &arg3, ধ্রুৱ RTLIL::Const &arg4, bool *errp = nullptr)
	{
		যদি (cell->type == ID($_AOI4_))
			ঘূৰাই eval_not(const_or(const_and(arg1, arg2, false, false, 1), const_and(arg3, arg4, false, false, 1), false, false, 1));
		যদি (cell->type == ID($_OAI4_))
			ঘূৰাই eval_not(const_and(const_or(arg1, arg2, false, false, 1), const_or(arg3, arg4, false, false, 1), false, false, 1));

		log_assert(arg4.size() == 0);
		ঘূৰাই eval(cell, arg1, arg2, arg3, errp);
	}
};

YOSYS_NAMESPACE_END

#endif
