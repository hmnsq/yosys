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

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ಸಂರಚನಾ CellType
{
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

ಸಂರಚನಾ CellTypes
{
	dict<RTLIL::IdString, CellType> cell_types;

	CellTypes()
	{
	}

	CellTypes(RTLIL::Design *design)
	{
		setup(design);
	}

	ಶೂನ್ಯ setup(RTLIL::Design *design = ರಿಕ್ತ)
	{
		ಯದಿ (design)
			setup_design(design);

		setup_internals();
		setup_internals_mem();
		setup_internals_anyinit();
		setup_stdcells();
		setup_stdcells_mem();
	}

	ಶೂನ್ಯ setup_type(RTLIL::IdString type, ಧ್ರುವ pool<RTLIL::IdString> &inputs, ಧ್ರುವ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		CellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		cell_types[ct.type] = ct;
	}

	template<typename N, YS_NAME_STRING(N)>
	ಶೂನ್ಯ setup_type(ಧ್ರುವ N &type_str, ಧ್ರುವ pool<RTLIL::IdString> &inputs, ಧ್ರುವ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		setup_type(ID::lookup(type_str), inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable);
	}

	ಶೂನ್ಯ setup_module(RTLIL::Module *module)
	{
		pool<IdString> inputs, outputs;
		ಪ್ರತಿ (auto wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			ಯದಿ (wire->port_ಒಳಹರಿವು)
				inputs.ಸೇರಿಸು(wire->name);
			ಯದಿ (wire->port_output)
				outputs.ಸೇರಿಸು(wire->name);
		}
		setup_type(module->name, inputs, outputs);
	}

	ಶೂನ್ಯ setup_design(RTLIL::Design *design)
	{
		ಪ್ರತಿ (auto module : design->modules())
			setup_module(module);
	}

	ಶೂನ್ಯ setup_internals()
	{
		setup_internals_eval();

		setup_type(ID($barrier), {ID::A}, {ID::Y});

		setup_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y});

		setup_type(ID($ಪ್ರತಿಜ್ಞಾ), {ID::A, ID::EN}, pool<RTLIL::IdString>());
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
		setup_type(ID($ಒಳಹರಿವು_port), {}, {ID::Y});
		setup_type(ID($connect), {ID::A, ID::B}, {});
	}

	ಶೂನ್ಯ setup_internals_eval()
	{
		std::vector<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($slice), ID($lut), ID($sop)
		};

		std::vector<RTLIL::IdString> ದ್ವಿಮಾನ_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($add), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		ಪ್ರತಿ (auto type : unary_ops)
			setup_type(type, {ID::A}, {ID::Y}, true);

		ಪ್ರತಿ (auto type : ದ್ವಿಮಾನ_ops)
			setup_type(type, {ID::A, ID::B}, {ID::Y}, true);

		ಪ್ರತಿ (auto type : std::vector<RTLIL::IdString>({ID($mux), ID($pmux), ID($bwmux)}))
			setup_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, true);

		ಪ್ರತಿ (auto type : std::vector<RTLIL::IdString>({ID($bmux), ID($demux)}))
			setup_type(type, {ID::A, ID::S}, {ID::Y}, true);

		setup_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, true);
		setup_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, true);
		setup_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		setup_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, true);
	}

	ಶೂನ್ಯ setup_internals_ff()
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

	ಶೂನ್ಯ setup_internals_anyinit()
	{
		setup_type(ID($anyinit), {ID::D}, {ID::Q});
	}

	ಶೂನ್ಯ setup_internals_mem()
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

	ಶೂನ್ಯ setup_stdcells()
	{
		setup_stdcells_eval();

		setup_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y});
	}

	ಶೂನ್ಯ setup_stdcells_eval()
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

	ಶೂನ್ಯ setup_stdcells_mem()
	{
		std::vector<ವರ್ಣ> list_np = {'N', 'P'}, list_01 = {'0', '1'};

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
			setup_type(stringf("$_SR_%c%c_", c1, c2), {ID::S, ID::R}, {ID::Q});

		setup_type(ID($_FF_), {ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
			setup_type(stringf("$_DFF_%c_", c1), {ID::C, ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
			setup_type(stringf("$_DFFE_%c%c_", c1, c2), {ID::C, ID::D, ID::E}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_01)
			setup_type(stringf("$_DFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_01)
		ಪ್ರತಿ (auto c4 : list_np)
			setup_type(stringf("$_DFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
			setup_type(stringf("$_ALDFF_%c%c_", c1, c2), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_np)
			setup_type(stringf("$_ALDFFE_%c%c%c_", c1, c2, c3), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_np)
			setup_type(stringf("$_DFFSR_%c%c%c_", c1, c2, c3), {ID::C, ID::S, ID::R, ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_np)
		ಪ್ರತಿ (auto c4 : list_np)
			setup_type(stringf("$_DFFSRE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_01)
			setup_type(stringf("$_SDFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_01)
		ಪ್ರತಿ (auto c4 : list_np)
			setup_type(stringf("$_SDFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_01)
		ಪ್ರತಿ (auto c4 : list_np)
			setup_type(stringf("$_SDFFCE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
			setup_type(stringf("$_DLATCH_%c_", c1), {ID::E, ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_01)
			setup_type(stringf("$_DLATCH_%c%c%c_", c1, c2, c3), {ID::E, ID::R, ID::D}, {ID::Q});

		ಪ್ರತಿ (auto c1 : list_np)
		ಪ್ರತಿ (auto c2 : list_np)
		ಪ್ರತಿ (auto c3 : list_np)
			setup_type(stringf("$_DLATCHSR_%c%c%c_", c1, c2, c3), {ID::E, ID::S, ID::R, ID::D}, {ID::Q});
	}

	ಶೂನ್ಯ clear()
	{
		cell_types.clear();
	}

	bool cell_known(RTLIL::IdString type) ಧ್ರುವ
	{
		ಪ್ರತ್ಯಾಗಮ cell_types.ಎಣಿಕೆ(type) != 0;
	}

	bool cell_output(RTLIL::IdString type, RTLIL::IdString port) ಧ್ರುವ
	{
		auto it = cell_types.find(type);
		ಪ್ರತ್ಯಾಗಮ it != cell_types.end() && it->second.outputs.ಎಣಿಕೆ(port) != 0;
	}

	bool cell_ಒಳಹರಿವು(RTLIL::IdString type, RTLIL::IdString port) ಧ್ರುವ
	{
		auto it = cell_types.find(type);
		ಪ್ರತ್ಯಾಗಮ it != cell_types.end() && it->second.inputs.ಎಣಿಕೆ(port) != 0;
	}

	RTLIL::PortDir cell_port_dir(RTLIL::IdString type, RTLIL::IdString port) ಧ್ರುವ
	{
		auto it = cell_types.find(type);
		ಯದಿ (it == cell_types.end())
			ಪ್ರತ್ಯಾಗಮ RTLIL::PD_UNKNOWN;
		bool is_ಒಳಹರಿವು = it->second.inputs.ಎಣಿಕೆ(port);
		bool is_output = it->second.outputs.ಎಣಿಕೆ(port);
		ಪ್ರತ್ಯಾಗಮ RTLIL::PortDir(is_ಒಳಹರಿವು + is_output * 2);
	}

	bool cell_evaluable(RTLIL::IdString type) ಧ್ರುವ
	{
		auto it = cell_types.find(type);
		ಪ್ರತ್ಯಾಗಮ it != cell_types.end() && it->second.is_evaluable;
	}

	ಸ್ಥಿರ RTLIL::Const eval_not(RTLIL::Const v)
	{
		ಪ್ರತಿ (auto bit : v)
			ಯದಿ (bit == State::S0) bit = State::S1;
			ಅನ್ಯಥಾ ಯದಿ (bit == State::S1) bit = State::S0;
		ಪ್ರತ್ಯಾಗಮ v;
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::IdString type, ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಙ್ಕ result_len, bool *errp = nullptr)
	{
		ಯದಿ (type == ID($sshr) && !signed1)
			type = ID($shr);
		ಯದಿ (type == ID($sshl) && !signed1)
			type = ID($shl);

		ಯದಿ (type != ID($sshr) && type != ID($sshl) && type != ID($shr) && type != ID($shl) && type != ID($shift) && type != ID($shiftx) &&
				type != ID($pos) && type != ID($buf) && type != ID($neg) && type != ID($not)) {
			ಯದಿ (!signed1 || !signed2)
				signed1 = false, signed2 = false;
		}

#define HANDLE_CELL_TYPE(_t) ಯದಿ (type == constid<ID::lookup("$" #_t).raw()>) ಪ್ರತ್ಯಾಗಮ const_ ## _t(arg1, arg2, signed1, signed2, result_len);
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

		ಯದಿ (type.in(ID($_BUF_), ID($buf)))
			ಪ್ರತ್ಯಾಗಮ arg1;
		ಯದಿ (type == ID($_NOT_))
			ಪ್ರತ್ಯಾಗಮ eval_not(arg1);
		ಯದಿ (type == ID($_AND_))
			ಪ್ರತ್ಯಾಗಮ const_and(arg1, arg2, false, false, 1);
		ಯದಿ (type == ID($_NAND_))
			ಪ್ರತ್ಯಾಗಮ eval_not(const_and(arg1, arg2, false, false, 1));
		ಯದಿ (type == ID($_OR_))
			ಪ್ರತ್ಯಾಗಮ const_or(arg1, arg2, false, false, 1);
		ಯದಿ (type == ID($_NOR_))
			ಪ್ರತ್ಯಾಗಮ eval_not(const_or(arg1, arg2, false, false, 1));
		ಯದಿ (type == ID($_XOR_))
			ಪ್ರತ್ಯಾಗಮ const_xor(arg1, arg2, false, false, 1);
		ಯದಿ (type == ID($_XNOR_))
			ಪ್ರತ್ಯಾಗಮ const_xnor(arg1, arg2, false, false, 1);
		ಯದಿ (type == ID($_ANDNOT_))
			ಪ್ರತ್ಯಾಗಮ const_and(arg1, eval_not(arg2), false, false, 1);
		ಯದಿ (type == ID($_ORNOT_))
			ಪ್ರತ್ಯಾಗಮ const_or(arg1, eval_not(arg2), false, false, 1);

		ಯದಿ (errp != nullptr) {
			*errp = true;
			ಪ್ರತ್ಯಾಗಮ State::Sm;
		}

		log_abort();
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::Cell *cell, ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, bool *errp = nullptr)
	{
		ಯದಿ (cell->type == ID($slice)) {
			ಪೂರ್ಣಾಙ್ಕ width = cell->parameters.at(ID::Y_WIDTH).as_int();
			ಪೂರ್ಣಾಙ್ಕ offset = cell->parameters.at(ID::OFFSET).as_int();
			ಪ್ರತ್ಯಾಗಮ arg1.extract(offset, width);
		}

		ಯದಿ (cell->type == ID($concat)) {
			RTLIL::Const ret = arg1;
			ret.append(arg2);
			ಪ್ರತ್ಯಾಗಮ ret;
		}

		ಯದಿ (cell->type == ID($bmux))
		{
			ಪ್ರತ್ಯಾಗಮ const_bmux(arg1, arg2);
		}

		ಯದಿ (cell->type == ID($demux))
		{
			ಪ್ರತ್ಯಾಗಮ const_demux(arg1, arg2);
		}

		ಯದಿ (cell->type == ID($bweqx))
		{
			ಪ್ರತ್ಯಾಗಮ const_bweqx(arg1, arg2);
		}

		ಯದಿ (cell->type == ID($lut))
		{
			ಪೂರ್ಣಾಙ್ಕ width = cell->parameters.at(ID::WIDTH).as_int();

			std::vector<RTLIL::State> t = cell->parameters.at(ID::LUT).to_bits();
			ಯಾವತ್ (GetSize(t) < (1 << width))
				t.ತಳ್ಳು_back(State::S0);
			t.resize(1 << width);

			ಪ್ರತ್ಯಾಗಮ const_bmux(t, arg1);
		}

		ಯದಿ (cell->type == ID($sop))
		{
			ಪೂರ್ಣಾಙ್ಕ width = cell->parameters.at(ID::WIDTH).as_int();
			ಪೂರ್ಣಾಙ್ಕ depth = cell->parameters.at(ID::DEPTH).as_int();
			std::vector<RTLIL::State> t = cell->parameters.at(ID::TABLE).to_bits();

			ಯಾವತ್ (GetSize(t) < width*depth*2)
				t.ತಳ್ಳು_back(State::S0);

			RTLIL::State default_ret = State::S0;

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < depth; i++)
			{
				bool match = true;
				bool match_x = true;

				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < width; j++) {
					RTLIL::State a = arg1.at(j);
					ಯದಿ (t.at(2*width*i + 2*j + 0) == State::S1) {
						ಯದಿ (a == State::S1) match_x = false;
						ಯದಿ (a != State::S0) match = false;
					}
					ಯದಿ (t.at(2*width*i + 2*j + 1) == State::S1) {
						ಯದಿ (a == State::S0) match_x = false;
						ಯದಿ (a != State::S1) match = false;
					}
				}

				ಯದಿ (match)
					ಪ್ರತ್ಯಾಗಮ State::S1;

				ಯದಿ (match_x)
					default_ret = State::Sx;
			}

			ಪ್ರತ್ಯಾಗಮ default_ret;
		}

		bool signed_a = cell->parameters.ಎಣಿಕೆ(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
		bool signed_b = cell->parameters.ಎಣಿಕೆ(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();
		ಪೂರ್ಣಾಙ್ಕ result_len = cell->parameters.ಎಣಿಕೆ(ID::Y_WIDTH) > 0 ? cell->parameters[ID::Y_WIDTH].as_int() : -1;
		ಪ್ರತ್ಯಾಗಮ eval(cell->type, arg1, arg2, signed_a, signed_b, result_len, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::Cell *cell, ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3, bool *errp = nullptr)
	{
		ಯದಿ (cell->type.in(ID($mux), ID($_MUX_)))
			ಪ್ರತ್ಯಾಗಮ const_mux(arg1, arg2, arg3);
		ಯದಿ (cell->type == ID($_NMUX_))
			ಪ್ರತ್ಯಾಗಮ eval_not(const_mux(arg1, arg2, arg3));
		ಯದಿ (cell->type == ID($bwmux))
			ಪ್ರತ್ಯಾಗಮ const_bwmux(arg1, arg2, arg3);
		ಯದಿ (cell->type == ID($pmux))
			ಪ್ರತ್ಯಾಗಮ const_pmux(arg1, arg2, arg3);
		ಯದಿ (cell->type == ID($_AOI3_))
			ಪ್ರತ್ಯಾಗಮ eval_not(const_or(const_and(arg1, arg2, false, false, 1), arg3, false, false, 1));
		ಯದಿ (cell->type == ID($_OAI3_))
			ಪ್ರತ್ಯಾಗಮ eval_not(const_and(const_or(arg1, arg2, false, false, 1), arg3, false, false, 1));

		log_assert(arg3.size() == 0);
		ಪ್ರತ್ಯಾಗಮ eval(cell, arg1, arg2, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::Cell *cell, ಧ್ರುವ RTLIL::Const &arg1, ಧ್ರುವ RTLIL::Const &arg2, ಧ್ರುವ RTLIL::Const &arg3, ಧ್ರುವ RTLIL::Const &arg4, bool *errp = nullptr)
	{
		ಯದಿ (cell->type == ID($_AOI4_))
			ಪ್ರತ್ಯಾಗಮ eval_not(const_or(const_and(arg1, arg2, false, false, 1), const_and(arg3, arg4, false, false, 1), false, false, 1));
		ಯದಿ (cell->type == ID($_OAI4_))
			ಪ್ರತ್ಯಾಗಮ eval_not(const_and(const_or(arg1, arg2, false, false, 1), const_or(arg3, arg4, false, false, 1), false, false, 1));

		log_assert(arg4.size() == 0);
		ಪ್ರತ್ಯಾಗಮ eval(cell, arg1, arg2, arg3, errp);
	}
};

YOSYS_NAMESPACE_END

#endif
