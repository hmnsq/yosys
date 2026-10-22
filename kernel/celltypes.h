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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CELLTYPES_H
#ವ್ಯಾಖ್ಯಾನಿಸು CELLTYPES_H

#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ರಚನೆ CellType
{
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

ರಚನೆ CellTypes
{
	dict<RTLIL::IdString, CellType> cell_types;

	CellTypes()
	{
	}

	CellTypes(RTLIL::Design *design)
	{
		ಸಿದ್ಧತೆ(design);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ(RTLIL::Design *design = ಖಾಲಿ)
	{
		ಆದರೆ (design)
			ಸಿದ್ಧತೆ_design(design);

		ಸಿದ್ಧತೆ_internals();
		ಸಿದ್ಧತೆ_internals_mem();
		ಸಿದ್ಧತೆ_internals_anyinit();
		ಸಿದ್ಧತೆ_stdcells();
		ಸಿದ್ಧತೆ_stdcells_mem();
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_type(RTLIL::IdString type, ಸ್ಥಿರಾಂಕ pool<RTLIL::IdString> &inputs, ಸ್ಥಿರಾಂಕ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		CellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		cell_types[ct.type] = ct;
	}

	ಮಾದರಿ<typename N, YS_NAME_STRING(N)>
	ಶೂನ್ಯ ಸಿದ್ಧತೆ_type(ಸ್ಥಿರಾಂಕ N &type_str, ಸ್ಥಿರಾಂಕ pool<RTLIL::IdString> &inputs, ಸ್ಥಿರಾಂಕ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		ಸಿದ್ಧತೆ_type(ID::lookup(type_str), inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_module(RTLIL::Module *module)
	{
		pool<IdString> inputs, outputs;
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			ಆದರೆ (wire->ಪೋರ್ಟ್_ಒಳಹರಿವು)
				inputs.ಸೇರಿಸು(wire->name);
			ಆದರೆ (wire->ಪೋರ್ಟ್_output)
				outputs.ಸೇರಿಸು(wire->name);
		}
		ಸಿದ್ಧತೆ_type(module->name, inputs, outputs);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_design(RTLIL::Design *design)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ module : design->modules())
			ಸಿದ್ಧತೆ_module(module);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals()
	{
		ಸಿದ್ಧತೆ_internals_eval();

		ಸಿದ್ಧತೆ_type(ID($barrier), {ID::A}, {ID::Y});

		ಸಿದ್ಧತೆ_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y});

		ಸಿದ್ಧತೆ_type(ID($ಪ್ರತಿಜ್ಞಾ), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($assume), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($ಜೀವಂತ), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($fair), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($cover), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($initstate), pool<RTLIL::IdString>(), {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($anyconst), pool<RTLIL::IdString>(), {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($anyseq), pool<RTLIL::IdString>(), {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($allconst), pool<RTLIL::IdString>(), {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($allseq), pool<RTLIL::IdString>(), {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($equiv), {ID::A, ID::B}, {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($print), {ID::EN, ID::ವಾದಗಳು, ID::TRG}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($check), {ID::A, ID::EN, ID::ವಾದಗಳು, ID::TRG}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($ಗಣ_ಟ್ಯಾಗ್), {ID::A, ID::ಗಣ_2, ID::CLR}, {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($get_ಟ್ಯಾಗ್), {ID::A}, {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($overwrite_ಟ್ಯಾಗ್), {ID::A, ID::ಗಣ_2, ID::CLR}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($original_ಟ್ಯಾಗ್), {ID::A}, {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($future_ff), {ID::A}, {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($scopeinfo), {}, {});
		ಸಿದ್ಧತೆ_type(ID($ಒಳಹರಿವು_ಪೋರ್ಟ್), {}, {ID::Y});
		ಸಿದ್ಧತೆ_type(ID($connect), {ID::A, ID::B}, {});
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_eval()
	{
		std::ಸದಿಶ<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ತುಂಡು), ID($lut), ID($sop)
		};

		std::ಸದಿಶ<RTLIL::IdString> ದ್ವಿಮಾನ_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ಸರಿಸು), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($ಕೂಡಿಸು), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ಘಾತ),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : unary_ops)
			ಸಿದ್ಧತೆ_type(type, {ID::A}, {ID::Y}, true);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : ದ್ವಿಮಾನ_ops)
			ಸಿದ್ಧತೆ_type(type, {ID::A, ID::B}, {ID::Y}, true);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : std::ಸದಿಶ<RTLIL::IdString>({ID($mux), ID($pmux), ID($bwmux)}))
			ಸಿದ್ಧತೆ_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, true);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : std::ಸದಿಶ<RTLIL::IdString>({ID($bmux), ID($demux)}))
			ಸಿದ್ಧತೆ_type(type, {ID::A, ID::S}, {ID::Y}, true);

		ಸಿದ್ಧತೆ_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, true);
		ಸಿದ್ಧತೆ_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, true);
		ಸಿದ್ಧತೆ_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, true);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_ff()
	{
		ಸಿದ್ಧತೆ_type(ID($sr), {ID::ಗಣ_2, ID::CLR}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($ff), {ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($dff), {ID::CLK, ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($dffsr), {ID::CLK, ID::ಗಣ_2, ID::CLR, ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($dffsre), {ID::CLK, ID::ಗಣ_2, ID::CLR, ID::D, ID::EN}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q});
		ಸಿದ್ಧತೆ_type(ID($dlatchsr), {ID::EN, ID::ಗಣ_2, ID::CLR, ID::D}, {ID::Q});
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_anyinit()
	{
		ಸಿದ್ಧತೆ_type(ID($anyinit), {ID::D}, {ID::Q});
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_mem()
	{
		ಸಿದ್ಧತೆ_internals_ff();

		ಸಿದ್ಧತೆ_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::ದತ್ತಾಂಶ_2});
		ಸಿದ್ಧತೆ_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::ದತ್ತಾಂಶ_2});
		ಸಿದ್ಧತೆ_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::ದತ್ತಾಂಶ_2}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::ದತ್ತಾಂಶ_2}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($meminit), {ID::ADDR, ID::ದತ್ತಾಂಶ_2}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($meminit_v2), {ID::ADDR, ID::ದತ್ತಾಂಶ_2, ID::EN}, pool<RTLIL::IdString>());
		ಸಿದ್ಧತೆ_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ದತ್ತಾಂಶ}, {ID::RD_ದತ್ತಾಂಶ});
		ಸಿದ್ಧತೆ_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ದತ್ತಾಂಶ}, {ID::RD_ದತ್ತಾಂಶ});

		ಸಿದ್ಧತೆ_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_ಒಳ}, {ID::CTRL_ಹೊರಹೋಗು});
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_stdcells()
	{
		ಸಿದ್ಧತೆ_stdcells_eval();

		ಸಿದ್ಧತೆ_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y});
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_stdcells_eval()
	{
		ಸಿದ್ಧತೆ_type(ID($_BUF_), {ID::A}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_NOT_), {ID::A}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
		ಸಿದ್ಧತೆ_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_stdcells_mem()
	{
		std::ಸದಿಶ<ಅಕ್ಷರ> list_np = {'N', 'P'}, list_01 = {'0', '1'};

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_SR_%c%c_", c1, c2), {ID::S, ID::R}, {ID::Q});

		ಸಿದ್ಧತೆ_type(ID($_FF_), {ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_DFF_%c_", c1), {ID::C, ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_DFFE_%c%c_", c1, c2), {ID::C, ID::D, ID::E}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_01)
			ಸಿದ್ಧತೆ_type(stringf("$_DFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_01)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c4 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_DFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_ALDFF_%c%c_", c1, c2), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_ALDFFE_%c%c%c_", c1, c2, c3), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_DFFSR_%c%c%c_", c1, c2, c3), {ID::C, ID::S, ID::R, ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c4 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_DFFSRE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_01)
			ಸಿದ್ಧತೆ_type(stringf("$_SDFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_01)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c4 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_SDFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_01)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c4 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_SDFFCE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_DLATCH_%c_", c1), {ID::E, ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_01)
			ಸಿದ್ಧತೆ_type(stringf("$_DLATCH_%c%c%c_", c1, c2, c3), {ID::E, ID::R, ID::D}, {ID::Q});

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c1 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c2 : list_np)
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ c3 : list_np)
			ಸಿದ್ಧತೆ_type(stringf("$_DLATCHSR_%c%c%c_", c1, c2, c3), {ID::E, ID::S, ID::R, ID::D}, {ID::Q});
	}

	ಶೂನ್ಯ clear()
	{
		cell_types.clear();
	}

	bool cell_ತಿಳಿದ(RTLIL::IdString type) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ cell_types.ಎಣಿಕೆ_2(type) != 0;
	}

	bool cell_output(RTLIL::IdString type, RTLIL::IdString ಪೋರ್ಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ವಯಂಚಾಲಿತ it = cell_types.ಕಂಡುಹಿಡಿ(type);
		ಹಿಂತಿರುಗಿ it != cell_types.ಕೊನೆ() && it->ಎರಡನೇ.outputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್) != 0;
	}

	bool cell_ಒಳಹರಿವು(RTLIL::IdString type, RTLIL::IdString ಪೋರ್ಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ವಯಂಚಾಲಿತ it = cell_types.ಕಂಡುಹಿಡಿ(type);
		ಹಿಂತಿರುಗಿ it != cell_types.ಕೊನೆ() && it->ಎರಡನೇ.inputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್) != 0;
	}

	RTLIL::ಪೋರ್ಟ್_Dir cell_ಪೋರ್ಟ್_dir(RTLIL::IdString type, RTLIL::IdString ಪೋರ್ಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ವಯಂಚಾಲಿತ it = cell_types.ಕಂಡುಹಿಡಿ(type);
		ಆದರೆ (it == cell_types.ಕೊನೆ())
			ಹಿಂತಿರುಗಿ RTLIL::PD_UNKNOWN;
		bool is_ಒಳಹರಿವು = it->ಎರಡನೇ.inputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್);
		bool is_output = it->ಎರಡನೇ.outputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್);
		ಹಿಂತಿರುಗಿ RTLIL::ಪೋರ್ಟ್_Dir(is_ಒಳಹರಿವು + is_output * 2);
	}

	bool cell_evaluable(RTLIL::IdString type) ಸ್ಥಿರಾಂಕ
	{
		ಸ್ವಯಂಚಾಲಿತ it = cell_types.ಕಂಡುಹಿಡಿ(type);
		ಹಿಂತಿರುಗಿ it != cell_types.ಕೊನೆ() && it->ಎರಡನೇ.is_evaluable;
	}

	ಸ್ಥಿರ RTLIL::Const eval_not(RTLIL::Const v)
	{
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : v)
			ಆದರೆ (ಬಿಟ್ == State::S0) ಬಿಟ್ = State::S1;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಿಟ್ == State::S1) ಬಿಟ್ = State::S0;
		ಹಿಂತಿರುಗಿ v;
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::IdString type, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool signed1, bool signed2, ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len, bool *errp = nullptr)
	{
		ಆದರೆ (type == ID($sshr) && !signed1)
			type = ID($shr);
		ಆದರೆ (type == ID($sshl) && !signed1)
			type = ID($shl);

		ಆದರೆ (type != ID($sshr) && type != ID($sshl) && type != ID($shr) && type != ID($shl) && type != ID($ಸರಿಸು) && type != ID($shiftx) &&
				type != ID($pos) && type != ID($buf) && type != ID($neg) && type != ID($not)) {
			ಆದರೆ (!signed1 || !signed2)
				signed1 = false, signed2 = false;
		}

#ವ್ಯಾಖ್ಯಾನಿಸು HANDLE_CELL_TYPE(_t) ಆದರೆ (type == constid<ID::lookup("$" #_t).raw()>) ಹಿಂತಿರುಗಿ const_ ## _t(arg1, arg2, signed1, signed2, ಫಲಿತಾಂಶ_len);
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
		HANDLE_CELL_TYPE(ಸರಿಸು)
		HANDLE_CELL_TYPE(shiftx)
		HANDLE_CELL_TYPE(lt)
		HANDLE_CELL_TYPE(le)
		HANDLE_CELL_TYPE(eq)
		HANDLE_CELL_TYPE(ne)
		HANDLE_CELL_TYPE(eqx)
		HANDLE_CELL_TYPE(nex)
		HANDLE_CELL_TYPE(ge)
		HANDLE_CELL_TYPE(gt)
		HANDLE_CELL_TYPE(ಕೂಡಿಸು)
		HANDLE_CELL_TYPE(sub)
		HANDLE_CELL_TYPE(mul)
		HANDLE_CELL_TYPE(div)
		HANDLE_CELL_TYPE(mod)
		HANDLE_CELL_TYPE(divfloor)
		HANDLE_CELL_TYPE(modfloor)
		HANDLE_CELL_TYPE(ಘಾತ)
		HANDLE_CELL_TYPE(pos)
		HANDLE_CELL_TYPE(neg)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು HANDLE_CELL_TYPE

		ಆದರೆ (type.ಒಳ(ID($_BUF_), ID($buf)))
			ಹಿಂತಿರುಗಿ arg1;
		ಆದರೆ (type == ID($_NOT_))
			ಹಿಂತಿರುಗಿ eval_not(arg1);
		ಆದರೆ (type == ID($_AND_))
			ಹಿಂತಿರುಗಿ const_and(arg1, arg2, false, false, 1);
		ಆದರೆ (type == ID($_NAND_))
			ಹಿಂತಿರುಗಿ eval_not(const_and(arg1, arg2, false, false, 1));
		ಆದರೆ (type == ID($_OR_))
			ಹಿಂತಿರುಗಿ const_or(arg1, arg2, false, false, 1);
		ಆದರೆ (type == ID($_NOR_))
			ಹಿಂತಿರುಗಿ eval_not(const_or(arg1, arg2, false, false, 1));
		ಆದರೆ (type == ID($_XOR_))
			ಹಿಂತಿರುಗಿ const_xor(arg1, arg2, false, false, 1);
		ಆದರೆ (type == ID($_XNOR_))
			ಹಿಂತಿರುಗಿ const_xnor(arg1, arg2, false, false, 1);
		ಆದರೆ (type == ID($_ANDNOT_))
			ಹಿಂತಿರುಗಿ const_and(arg1, eval_not(arg2), false, false, 1);
		ಆದರೆ (type == ID($_ORNOT_))
			ಹಿಂತಿರುಗಿ const_or(arg1, eval_not(arg2), false, false, 1);

		ಆದರೆ (errp != nullptr) {
			*errp = true;
			ಹಿಂತಿರುಗಿ State::Sm;
		}

		log_abort();
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::Cell *cell, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, bool *errp = nullptr)
	{
		ಆದರೆ (cell->type == ID($ತುಂಡು)) {
			ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->parameters.at(ID::Y_ಅಗಲ).as_int();
			ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = cell->parameters.at(ID::ಸ್ಥಾನಾಂತರ_2).as_int();
			ಹಿಂತಿರುಗಿ arg1.ಹೊರತೆಗೆ(ಸ್ಥಾನಾಂತರ, ಅಗಲ);
		}

		ಆದರೆ (cell->type == ID($concat)) {
			RTLIL::Const ret = arg1;
			ret.append(arg2);
			ಹಿಂತಿರುಗಿ ret;
		}

		ಆದರೆ (cell->type == ID($bmux))
		{
			ಹಿಂತಿರುಗಿ const_bmux(arg1, arg2);
		}

		ಆದರೆ (cell->type == ID($demux))
		{
			ಹಿಂತಿರುಗಿ const_demux(arg1, arg2);
		}

		ಆದರೆ (cell->type == ID($bweqx))
		{
			ಹಿಂತಿರುಗಿ const_bweqx(arg1, arg2);
		}

		ಆದರೆ (cell->type == ID($lut))
		{
			ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->parameters.at(ID::ಅಗಲ_2).as_int();

			std::ಸದಿಶ<RTLIL::State> t = cell->parameters.at(ID::LUT).ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು();
			ಆಗ (Get_ಗಾತ್ರ(t) < (1 << ಅಗಲ))
				t.ತಳ್ಳು_ಹಿಂಭಾಗ(State::S0);
			t.resize(1 << ಅಗಲ);

			ಹಿಂತಿರುಗಿ const_bmux(t, arg1);
		}

		ಆದರೆ (cell->type == ID($sop))
		{
			ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->parameters.at(ID::ಅಗಲ_2).as_int();
			ಪೂರ್ಣಾಂಕ ಆಳ = cell->parameters.at(ID::ಆಳ_2).as_int();
			std::ಸದಿಶ<RTLIL::State> t = cell->parameters.at(ID::ಕೋಷ್ಟಕ).ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು();

			ಆಗ (Get_ಗಾತ್ರ(t) < ಅಗಲ*ಆಳ*2)
				t.ತಳ್ಳು_ಹಿಂಭಾಗ(State::S0);

			RTLIL::State default_ret = State::S0;

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಆಳ; i++)
			{
				bool ಹೊಂದಿಕೆ = true;
				bool ಹೊಂದಿಕೆ_x = true;

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < ಅಗಲ; j++) {
					RTLIL::State a = arg1.at(j);
					ಆದರೆ (t.at(2*ಅಗಲ*i + 2*j + 0) == State::S1) {
						ಆದರೆ (a == State::S1) ಹೊಂದಿಕೆ_x = false;
						ಆದರೆ (a != State::S0) ಹೊಂದಿಕೆ = false;
					}
					ಆದರೆ (t.at(2*ಅಗಲ*i + 2*j + 1) == State::S1) {
						ಆದರೆ (a == State::S0) ಹೊಂದಿಕೆ_x = false;
						ಆದರೆ (a != State::S1) ಹೊಂದಿಕೆ = false;
					}
				}

				ಆದರೆ (ಹೊಂದಿಕೆ)
					ಹಿಂತಿರುಗಿ State::S1;

				ಆದರೆ (ಹೊಂದಿಕೆ_x)
					default_ret = State::Sx;
			}

			ಹಿಂತಿರುಗಿ default_ret;
		}

		bool signed_a = cell->parameters.ಎಣಿಕೆ_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
		bool signed_b = cell->parameters.ಎಣಿಕೆ_2(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();
		ಪೂರ್ಣಾಂಕ ಫಲಿತಾಂಶ_len = cell->parameters.ಎಣಿಕೆ_2(ID::Y_ಅಗಲ) > 0 ? cell->parameters[ID::Y_ಅಗಲ].as_int() : -1;
		ಹಿಂತಿರುಗಿ eval(cell->type, arg1, arg2, signed_a, signed_b, ಫಲಿತಾಂಶ_len, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::Cell *cell, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3, bool *errp = nullptr)
	{
		ಆದರೆ (cell->type.ಒಳ(ID($mux), ID($_MUX_)))
			ಹಿಂತಿರುಗಿ const_mux(arg1, arg2, arg3);
		ಆದರೆ (cell->type == ID($_NMUX_))
			ಹಿಂತಿರುಗಿ eval_not(const_mux(arg1, arg2, arg3));
		ಆದರೆ (cell->type == ID($bwmux))
			ಹಿಂತಿರುಗಿ const_bwmux(arg1, arg2, arg3);
		ಆದರೆ (cell->type == ID($pmux))
			ಹಿಂತಿರುಗಿ const_pmux(arg1, arg2, arg3);
		ಆದರೆ (cell->type == ID($_AOI3_))
			ಹಿಂತಿರುಗಿ eval_not(const_or(const_and(arg1, arg2, false, false, 1), arg3, false, false, 1));
		ಆದರೆ (cell->type == ID($_OAI3_))
			ಹಿಂತಿರುಗಿ eval_not(const_and(const_or(arg1, arg2, false, false, 1), arg3, false, false, 1));

		log_assert(arg3.ಗಾತ್ರ_2() == 0);
		ಹಿಂತಿರುಗಿ eval(cell, arg1, arg2, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ಸ್ಥಿರ RTLIL::Const eval(RTLIL::Cell *cell, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg1, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg2, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg3, ಸ್ಥಿರಾಂಕ RTLIL::Const &arg4, bool *errp = nullptr)
	{
		ಆದರೆ (cell->type == ID($_AOI4_))
			ಹಿಂತಿರುಗಿ eval_not(const_or(const_and(arg1, arg2, false, false, 1), const_and(arg3, arg4, false, false, 1), false, false, 1));
		ಆದರೆ (cell->type == ID($_OAI4_))
			ಹಿಂತಿರುಗಿ eval_not(const_and(const_or(arg1, arg2, false, false, 1), const_or(arg3, arg4, false, false, 1), false, false, 1));

		log_assert(arg4.ಗಾತ್ರ_2() == 0);
		ಹಿಂತಿರುಗಿ eval(cell, arg1, arg2, arg3, errp);
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
