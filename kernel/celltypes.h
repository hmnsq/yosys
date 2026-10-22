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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CELLTYPES_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CELLTYPES_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

ਬਣਤਰ CellType
{
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

ਬਣਤਰ CellTypes
{
	dict<RTLIL::IdString, CellType> cell_types;

	CellTypes()
	{
	}

	CellTypes(RTLIL::Design *design)
	{
		ਤਿਆਰੀ(design);
	}

	ਖਾਲੀ ਤਿਆਰੀ(RTLIL::Design *design = ਨਿਰਾ)
	{
		ਜੇ (design)
			ਤਿਆਰੀ_design(design);

		ਤਿਆਰੀ_internals();
		ਤਿਆਰੀ_internals_mem();
		ਤਿਆਰੀ_internals_anyinit();
		ਤਿਆਰੀ_stdcells();
		ਤਿਆਰੀ_stdcells_mem();
	}

	ਖਾਲੀ ਤਿਆਰੀ_type(RTLIL::IdString type, ਅਚਲ pool<RTLIL::IdString> &inputs, ਅਚਲ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		CellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		cell_types[ct.type] = ct;
	}

	ਸਾਂਚਾ<typename N, YS_NAME_STRING(N)>
	ਖਾਲੀ ਤਿਆਰੀ_type(ਅਚਲ N &type_str, ਅਚਲ pool<RTLIL::IdString> &inputs, ਅਚਲ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		ਤਿਆਰੀ_type(ID::lookup(type_str), inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable);
	}

	ਖਾਲੀ ਤਿਆਰੀ_module(RTLIL::Module *module)
	{
		pool<IdString> inputs, outputs;
		ਲਈ (ਸਵੈਚਾਲਿਤ wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			ਜੇ (wire->ਪੋਰਟ_ਨਿਵੇਸ਼)
				inputs.ਪਾਓ(wire->name);
			ਜੇ (wire->ਪੋਰਟ_output)
				outputs.ਪਾਓ(wire->name);
		}
		ਤਿਆਰੀ_type(module->name, inputs, outputs);
	}

	ਖਾਲੀ ਤਿਆਰੀ_design(RTLIL::Design *design)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ module : design->modules())
			ਤਿਆਰੀ_module(module);
	}

	ਖਾਲੀ ਤਿਆਰੀ_internals()
	{
		ਤਿਆਰੀ_internals_eval();

		ਤਿਆਰੀ_type(ID($barrier), {ID::A}, {ID::Y});

		ਤਿਆਰੀ_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y});

		ਤਿਆਰੀ_type(ID($ਪ੍ਰਤਿਜ੍ਞਾ), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($assume), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($ਜੀਵਤ), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($fair), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($cover), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($initstate), pool<RTLIL::IdString>(), {ID::Y});
		ਤਿਆਰੀ_type(ID($anyconst), pool<RTLIL::IdString>(), {ID::Y});
		ਤਿਆਰੀ_type(ID($anyseq), pool<RTLIL::IdString>(), {ID::Y});
		ਤਿਆਰੀ_type(ID($allconst), pool<RTLIL::IdString>(), {ID::Y});
		ਤਿਆਰੀ_type(ID($allseq), pool<RTLIL::IdString>(), {ID::Y});
		ਤਿਆਰੀ_type(ID($equiv), {ID::A, ID::B}, {ID::Y});
		ਤਿਆਰੀ_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($print), {ID::EN, ID::ਦਲੀਲਾਂ, ID::TRG}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($check), {ID::A, ID::EN, ID::ਦਲੀਲਾਂ, ID::TRG}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($ਸੈੱਟ_ਟੈਗ), {ID::A, ID::ਸੈੱਟ_2, ID::CLR}, {ID::Y});
		ਤਿਆਰੀ_type(ID($get_ਟੈਗ), {ID::A}, {ID::Y});
		ਤਿਆਰੀ_type(ID($overwrite_ਟੈਗ), {ID::A, ID::ਸੈੱਟ_2, ID::CLR}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($original_ਟੈਗ), {ID::A}, {ID::Y});
		ਤਿਆਰੀ_type(ID($future_ff), {ID::A}, {ID::Y});
		ਤਿਆਰੀ_type(ID($scopeinfo), {}, {});
		ਤਿਆਰੀ_type(ID($ਨਿਵੇਸ਼_ਪੋਰਟ), {}, {ID::Y});
		ਤਿਆਰੀ_type(ID($connect), {ID::A, ID::B}, {});
	}

	ਖਾਲੀ ਤਿਆਰੀ_internals_eval()
	{
		std::ਸਦਿਸ਼<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ਟੁਕੜਾ), ID($lut), ID($sop)
		};

		std::ਸਦਿਸ਼<RTLIL::IdString> ਦੁਆਧਾਰੀ_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ਖਿਸਕਾਓ), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($ਜੋੜੋ), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ਘਾਤ),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		ਲਈ (ਸਵੈਚਾਲਿਤ type : unary_ops)
			ਤਿਆਰੀ_type(type, {ID::A}, {ID::Y}, true);

		ਲਈ (ਸਵੈਚਾਲਿਤ type : ਦੁਆਧਾਰੀ_ops)
			ਤਿਆਰੀ_type(type, {ID::A, ID::B}, {ID::Y}, true);

		ਲਈ (ਸਵੈਚਾਲਿਤ type : std::ਸਦਿਸ਼<RTLIL::IdString>({ID($mux), ID($pmux), ID($bwmux)}))
			ਤਿਆਰੀ_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, true);

		ਲਈ (ਸਵੈਚਾਲਿਤ type : std::ਸਦਿਸ਼<RTLIL::IdString>({ID($bmux), ID($demux)}))
			ਤਿਆਰੀ_type(type, {ID::A, ID::S}, {ID::Y}, true);

		ਤਿਆਰੀ_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, true);
		ਤਿਆਰੀ_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, true);
		ਤਿਆਰੀ_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, true);
	}

	ਖਾਲੀ ਤਿਆਰੀ_internals_ff()
	{
		ਤਿਆਰੀ_type(ID($sr), {ID::ਸੈੱਟ_2, ID::CLR}, {ID::Q});
		ਤਿਆਰੀ_type(ID($ff), {ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($dff), {ID::CLK, ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($dffsr), {ID::CLK, ID::ਸੈੱਟ_2, ID::CLR, ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($dffsre), {ID::CLK, ID::ਸੈੱਟ_2, ID::CLR, ID::D, ID::EN}, {ID::Q});
		ਤਿਆਰੀ_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q});
		ਤਿਆਰੀ_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q});
		ਤਿਆਰੀ_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		ਤਿਆਰੀ_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		ਤਿਆਰੀ_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q});
		ਤਿਆਰੀ_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q});
		ਤਿਆਰੀ_type(ID($dlatchsr), {ID::EN, ID::ਸੈੱਟ_2, ID::CLR, ID::D}, {ID::Q});
	}

	ਖਾਲੀ ਤਿਆਰੀ_internals_anyinit()
	{
		ਤਿਆਰੀ_type(ID($anyinit), {ID::D}, {ID::Q});
	}

	ਖਾਲੀ ਤਿਆਰੀ_internals_mem()
	{
		ਤਿਆਰੀ_internals_ff();

		ਤਿਆਰੀ_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::ਅੰਕੜੇ_2});
		ਤਿਆਰੀ_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::ਅੰਕੜੇ_2});
		ਤਿਆਰੀ_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::ਅੰਕੜੇ_2}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::ਅੰਕੜੇ_2}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($meminit), {ID::ADDR, ID::ਅੰਕੜੇ_2}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($meminit_v2), {ID::ADDR, ID::ਅੰਕੜੇ_2, ID::EN}, pool<RTLIL::IdString>());
		ਤਿਆਰੀ_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ਅੰਕੜੇ}, {ID::RD_ਅੰਕੜੇ});
		ਤਿਆਰੀ_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ਅੰਕੜੇ}, {ID::RD_ਅੰਕੜੇ});

		ਤਿਆਰੀ_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_ਵਿੱਚ}, {ID::CTRL_ਬਾਹਰਲਾ});
	}

	ਖਾਲੀ ਤਿਆਰੀ_stdcells()
	{
		ਤਿਆਰੀ_stdcells_eval();

		ਤਿਆਰੀ_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y});
	}

	ਖਾਲੀ ਤਿਆਰੀ_stdcells_eval()
	{
		ਤਿਆਰੀ_type(ID($_BUF_), {ID::A}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_NOT_), {ID::A}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
		ਤਿਆਰੀ_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
	}

	ਖਾਲੀ ਤਿਆਰੀ_stdcells_mem()
	{
		std::ਸਦਿਸ਼<ਅੱਖਰ> list_np = {'N', 'P'}, list_01 = {'0', '1'};

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
			ਤਿਆਰੀ_type(stringf("$_SR_%c%c_", c1, c2), {ID::S, ID::R}, {ID::Q});

		ਤਿਆਰੀ_type(ID($_FF_), {ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
			ਤਿਆਰੀ_type(stringf("$_DFF_%c_", c1), {ID::C, ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
			ਤਿਆਰੀ_type(stringf("$_DFFE_%c%c_", c1, c2), {ID::C, ID::D, ID::E}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_01)
			ਤਿਆਰੀ_type(stringf("$_DFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_01)
		ਲਈ (ਸਵੈਚਾਲਿਤ c4 : list_np)
			ਤਿਆਰੀ_type(stringf("$_DFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
			ਤਿਆਰੀ_type(stringf("$_ALDFF_%c%c_", c1, c2), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_np)
			ਤਿਆਰੀ_type(stringf("$_ALDFFE_%c%c%c_", c1, c2, c3), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_np)
			ਤਿਆਰੀ_type(stringf("$_DFFSR_%c%c%c_", c1, c2, c3), {ID::C, ID::S, ID::R, ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c4 : list_np)
			ਤਿਆਰੀ_type(stringf("$_DFFSRE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_01)
			ਤਿਆਰੀ_type(stringf("$_SDFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_01)
		ਲਈ (ਸਵੈਚਾਲਿਤ c4 : list_np)
			ਤਿਆਰੀ_type(stringf("$_SDFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_01)
		ਲਈ (ਸਵੈਚਾਲਿਤ c4 : list_np)
			ਤਿਆਰੀ_type(stringf("$_SDFFCE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
			ਤਿਆਰੀ_type(stringf("$_DLATCH_%c_", c1), {ID::E, ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_01)
			ਤਿਆਰੀ_type(stringf("$_DLATCH_%c%c%c_", c1, c2, c3), {ID::E, ID::R, ID::D}, {ID::Q});

		ਲਈ (ਸਵੈਚਾਲਿਤ c1 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c2 : list_np)
		ਲਈ (ਸਵੈਚਾਲਿਤ c3 : list_np)
			ਤਿਆਰੀ_type(stringf("$_DLATCHSR_%c%c%c_", c1, c2, c3), {ID::E, ID::S, ID::R, ID::D}, {ID::Q});
	}

	ਖਾਲੀ clear()
	{
		cell_types.clear();
	}

	bool cell_ਗਿਆਤ(RTLIL::IdString type) ਅਚਲ
	{
		ਵਾਪਸ cell_types.ਗਿਣਤੀ_2(type) != 0;
	}

	bool cell_output(RTLIL::IdString type, RTLIL::IdString ਪੋਰਟ) ਅਚਲ
	{
		ਸਵੈਚਾਲਿਤ it = cell_types.ਲੱਭੋ(type);
		ਵਾਪਸ it != cell_types.ਸਿਰਾ() && it->ਦੂਜਾ.outputs.ਗਿਣਤੀ_2(ਪੋਰਟ) != 0;
	}

	bool cell_ਨਿਵੇਸ਼(RTLIL::IdString type, RTLIL::IdString ਪੋਰਟ) ਅਚਲ
	{
		ਸਵੈਚਾਲਿਤ it = cell_types.ਲੱਭੋ(type);
		ਵਾਪਸ it != cell_types.ਸਿਰਾ() && it->ਦੂਜਾ.inputs.ਗਿਣਤੀ_2(ਪੋਰਟ) != 0;
	}

	RTLIL::ਪੋਰਟ_Dir cell_ਪੋਰਟ_dir(RTLIL::IdString type, RTLIL::IdString ਪੋਰਟ) ਅਚਲ
	{
		ਸਵੈਚਾਲਿਤ it = cell_types.ਲੱਭੋ(type);
		ਜੇ (it == cell_types.ਸਿਰਾ())
			ਵਾਪਸ RTLIL::PD_UNKNOWN;
		bool is_ਨਿਵੇਸ਼ = it->ਦੂਜਾ.inputs.ਗਿਣਤੀ_2(ਪੋਰਟ);
		bool is_output = it->ਦੂਜਾ.outputs.ਗਿਣਤੀ_2(ਪੋਰਟ);
		ਵਾਪਸ RTLIL::ਪੋਰਟ_Dir(is_ਨਿਵੇਸ਼ + is_output * 2);
	}

	bool cell_evaluable(RTLIL::IdString type) ਅਚਲ
	{
		ਸਵੈਚਾਲਿਤ it = cell_types.ਲੱਭੋ(type);
		ਵਾਪਸ it != cell_types.ਸਿਰਾ() && it->ਦੂਜਾ.is_evaluable;
	}

	ਸਥਿਰ RTLIL::Const eval_not(RTLIL::Const v)
	{
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : v)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::S0) ਦੋਆਧਾਰੀ_ਅੰਕ = State::S1;
			ਨਹੀਂਤਾਂ ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ == State::S1) ਦੋਆਧਾਰੀ_ਅੰਕ = State::S0;
		ਵਾਪਸ v;
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ਸਥਿਰ RTLIL::Const eval(RTLIL::IdString type, ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool signed1, bool signed2, ਪੂਰਨ ਨਤੀਜਾ_len, bool *errp = nullptr)
	{
		ਜੇ (type == ID($sshr) && !signed1)
			type = ID($shr);
		ਜੇ (type == ID($sshl) && !signed1)
			type = ID($shl);

		ਜੇ (type != ID($sshr) && type != ID($sshl) && type != ID($shr) && type != ID($shl) && type != ID($ਖਿਸਕਾਓ) && type != ID($shiftx) &&
				type != ID($pos) && type != ID($buf) && type != ID($neg) && type != ID($not)) {
			ਜੇ (!signed1 || !signed2)
				signed1 = false, signed2 = false;
		}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HANDLE_CELL_TYPE(_t) ਜੇ (type == constid<ID::lookup("$" #_t).raw()>) ਵਾਪਸ const_ ## _t(arg1, arg2, signed1, signed2, ਨਤੀਜਾ_len);
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
		HANDLE_CELL_TYPE(ਖਿਸਕਾਓ)
		HANDLE_CELL_TYPE(shiftx)
		HANDLE_CELL_TYPE(lt)
		HANDLE_CELL_TYPE(le)
		HANDLE_CELL_TYPE(eq)
		HANDLE_CELL_TYPE(ne)
		HANDLE_CELL_TYPE(eqx)
		HANDLE_CELL_TYPE(nex)
		HANDLE_CELL_TYPE(ge)
		HANDLE_CELL_TYPE(gt)
		HANDLE_CELL_TYPE(ਜੋੜੋ)
		HANDLE_CELL_TYPE(sub)
		HANDLE_CELL_TYPE(mul)
		HANDLE_CELL_TYPE(div)
		HANDLE_CELL_TYPE(mod)
		HANDLE_CELL_TYPE(divfloor)
		HANDLE_CELL_TYPE(modfloor)
		HANDLE_CELL_TYPE(ਘਾਤ)
		HANDLE_CELL_TYPE(pos)
		HANDLE_CELL_TYPE(neg)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ HANDLE_CELL_TYPE

		ਜੇ (type.ਵਿੱਚ(ID($_BUF_), ID($buf)))
			ਵਾਪਸ arg1;
		ਜੇ (type == ID($_NOT_))
			ਵਾਪਸ eval_not(arg1);
		ਜੇ (type == ID($_AND_))
			ਵਾਪਸ const_and(arg1, arg2, false, false, 1);
		ਜੇ (type == ID($_NAND_))
			ਵਾਪਸ eval_not(const_and(arg1, arg2, false, false, 1));
		ਜੇ (type == ID($_OR_))
			ਵਾਪਸ const_or(arg1, arg2, false, false, 1);
		ਜੇ (type == ID($_NOR_))
			ਵਾਪਸ eval_not(const_or(arg1, arg2, false, false, 1));
		ਜੇ (type == ID($_XOR_))
			ਵਾਪਸ const_xor(arg1, arg2, false, false, 1);
		ਜੇ (type == ID($_XNOR_))
			ਵਾਪਸ const_xnor(arg1, arg2, false, false, 1);
		ਜੇ (type == ID($_ANDNOT_))
			ਵਾਪਸ const_and(arg1, eval_not(arg2), false, false, 1);
		ਜੇ (type == ID($_ORNOT_))
			ਵਾਪਸ const_or(arg1, eval_not(arg2), false, false, 1);

		ਜੇ (errp != nullptr) {
			*errp = true;
			ਵਾਪਸ State::Sm;
		}

		log_abort();
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ਸਥਿਰ RTLIL::Const eval(RTLIL::Cell *cell, ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, bool *errp = nullptr)
	{
		ਜੇ (cell->type == ID($ਟੁਕੜਾ)) {
			ਪੂਰਨ ਚੌੜਾਈ = cell->parameters.at(ID::Y_ਚੌੜਾਈ).as_int();
			ਪੂਰਨ ਵਿੱਥ = cell->parameters.at(ID::ਵਿੱਥ_2).as_int();
			ਵਾਪਸ arg1.ਕੱਢੋ_2(ਵਿੱਥ, ਚੌੜਾਈ);
		}

		ਜੇ (cell->type == ID($concat)) {
			RTLIL::Const ret = arg1;
			ret.append(arg2);
			ਵਾਪਸ ret;
		}

		ਜੇ (cell->type == ID($bmux))
		{
			ਵਾਪਸ const_bmux(arg1, arg2);
		}

		ਜੇ (cell->type == ID($demux))
		{
			ਵਾਪਸ const_demux(arg1, arg2);
		}

		ਜੇ (cell->type == ID($bweqx))
		{
			ਵਾਪਸ const_bweqx(arg1, arg2);
		}

		ਜੇ (cell->type == ID($lut))
		{
			ਪੂਰਨ ਚੌੜਾਈ = cell->parameters.at(ID::ਚੌੜਾਈ_2).as_int();

			std::ਸਦਿਸ਼<RTLIL::State> t = cell->parameters.at(ID::LUT).ਮੰਜ਼ਿਲ_ਬਿੱਟ();
			ਜਦੋਂ (Get_ਮਾਪ(t) < (1 << ਚੌੜਾਈ))
				t.ਧੱਕੋ_ਪਿੱਠ(State::S0);
			t.resize(1 << ਚੌੜਾਈ);

			ਵਾਪਸ const_bmux(t, arg1);
		}

		ਜੇ (cell->type == ID($sop))
		{
			ਪੂਰਨ ਚੌੜਾਈ = cell->parameters.at(ID::ਚੌੜਾਈ_2).as_int();
			ਪੂਰਨ ਡੂੰਘਾਈ = cell->parameters.at(ID::ਡੂੰਘਾਈ_2).as_int();
			std::ਸਦਿਸ਼<RTLIL::State> t = cell->parameters.at(ID::ਸਾਰਣੀ).ਮੰਜ਼ਿਲ_ਬਿੱਟ();

			ਜਦੋਂ (Get_ਮਾਪ(t) < ਚੌੜਾਈ*ਡੂੰਘਾਈ*2)
				t.ਧੱਕੋ_ਪਿੱਠ(State::S0);

			RTLIL::State default_ret = State::S0;

			ਲਈ (ਪੂਰਨ i = 0; i < ਡੂੰਘਾਈ; i++)
			{
				bool ਮੇਲ = true;
				bool ਮੇਲ_x = true;

				ਲਈ (ਪੂਰਨ j = 0; j < ਚੌੜਾਈ; j++) {
					RTLIL::State a = arg1.at(j);
					ਜੇ (t.at(2*ਚੌੜਾਈ*i + 2*j + 0) == State::S1) {
						ਜੇ (a == State::S1) ਮੇਲ_x = false;
						ਜੇ (a != State::S0) ਮੇਲ = false;
					}
					ਜੇ (t.at(2*ਚੌੜਾਈ*i + 2*j + 1) == State::S1) {
						ਜੇ (a == State::S0) ਮੇਲ_x = false;
						ਜੇ (a != State::S1) ਮੇਲ = false;
					}
				}

				ਜੇ (ਮੇਲ)
					ਵਾਪਸ State::S1;

				ਜੇ (ਮੇਲ_x)
					default_ret = State::Sx;
			}

			ਵਾਪਸ default_ret;
		}

		bool signed_a = cell->parameters.ਗਿਣਤੀ_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
		bool signed_b = cell->parameters.ਗਿਣਤੀ_2(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();
		ਪੂਰਨ ਨਤੀਜਾ_len = cell->parameters.ਗਿਣਤੀ_2(ID::Y_ਚੌੜਾਈ) > 0 ? cell->parameters[ID::Y_ਚੌੜਾਈ].as_int() : -1;
		ਵਾਪਸ eval(cell->type, arg1, arg2, signed_a, signed_b, ਨਤੀਜਾ_len, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ਸਥਿਰ RTLIL::Const eval(RTLIL::Cell *cell, ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3, bool *errp = nullptr)
	{
		ਜੇ (cell->type.ਵਿੱਚ(ID($mux), ID($_MUX_)))
			ਵਾਪਸ const_mux(arg1, arg2, arg3);
		ਜੇ (cell->type == ID($_NMUX_))
			ਵਾਪਸ eval_not(const_mux(arg1, arg2, arg3));
		ਜੇ (cell->type == ID($bwmux))
			ਵਾਪਸ const_bwmux(arg1, arg2, arg3);
		ਜੇ (cell->type == ID($pmux))
			ਵਾਪਸ const_pmux(arg1, arg2, arg3);
		ਜੇ (cell->type == ID($_AOI3_))
			ਵਾਪਸ eval_not(const_or(const_and(arg1, arg2, false, false, 1), arg3, false, false, 1));
		ਜੇ (cell->type == ID($_OAI3_))
			ਵਾਪਸ eval_not(const_and(const_or(arg1, arg2, false, false, 1), arg3, false, false, 1));

		log_assert(arg3.ਮਾਪ() == 0);
		ਵਾਪਸ eval(cell, arg1, arg2, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ਸਥਿਰ RTLIL::Const eval(RTLIL::Cell *cell, ਅਚਲ RTLIL::Const &arg1, ਅਚਲ RTLIL::Const &arg2, ਅਚਲ RTLIL::Const &arg3, ਅਚਲ RTLIL::Const &arg4, bool *errp = nullptr)
	{
		ਜੇ (cell->type == ID($_AOI4_))
			ਵਾਪਸ eval_not(const_or(const_and(arg1, arg2, false, false, 1), const_and(arg3, arg4, false, false, 1), false, false, 1));
		ਜੇ (cell->type == ID($_OAI4_))
			ਵਾਪਸ eval_not(const_and(const_or(arg1, arg2, false, false, 1), const_or(arg3, arg4, false, false, 1), false, false, 1));

		log_assert(arg4.ਮਾਪ() == 0);
		ਵਾਪਸ eval(cell, arg1, arg2, arg3, errp);
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
