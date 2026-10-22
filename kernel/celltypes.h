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

#నిర్వచితం_కాకపోతే CELLTYPES_H
#నిర్వచించు CELLTYPES_H

#అన్తర్భావ "kernel/yosys.h"
#అన్తర్భావ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

నిర్మాణం CellType
{
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

నిర్మాణం CellTypes
{
	dict<RTLIL::IdString, CellType> cell_types;

	CellTypes()
	{
	}

	CellTypes(RTLIL::Design *design)
	{
		అమరిక(design);
	}

	శూన్య అమరిక(RTLIL::Design *design = ఖాళీ)
	{
		ఒకవేళ (design)
			అమరిక_design(design);

		అమరిక_internals();
		అమరిక_internals_mem();
		అమరిక_internals_anyinit();
		అమరిక_stdcells();
		అమరిక_stdcells_mem();
	}

	శూన్య అమరిక_type(RTLIL::IdString type, స్థిరరాశి pool<RTLIL::IdString> &inputs, స్థిరరాశి pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		CellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		cell_types[ct.type] = ct;
	}

	మూస<typename N, YS_NAME_STRING(N)>
	శూన్య అమరిక_type(స్థిరరాశి N &type_str, స్థిరరాశి pool<RTLIL::IdString> &inputs, స్థిరరాశి pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		అమరిక_type(ID::lookup(type_str), inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable);
	}

	శూన్య అమరిక_module(RTLIL::Module *module)
	{
		pool<IdString> inputs, outputs;
		కొరకు (స్వయంచాలక wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			ఒకవేళ (wire->పోర్టు_ఆదానం)
				inputs.చొప్పించు(wire->name);
			ఒకవేళ (wire->పోర్టు_output)
				outputs.చొప్పించు(wire->name);
		}
		అమరిక_type(module->name, inputs, outputs);
	}

	శూన్య అమరిక_design(RTLIL::Design *design)
	{
		కొరకు (స్వయంచాలక module : design->modules())
			అమరిక_module(module);
	}

	శూన్య అమరిక_internals()
	{
		అమరిక_internals_eval();

		అమరిక_type(ID($barrier), {ID::A}, {ID::Y});

		అమరిక_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y});

		అమరిక_type(ID($ప్రతిజ్ఞా), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		అమరిక_type(ID($assume), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		అమరిక_type(ID($సజీవ), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		అమరిక_type(ID($fair), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		అమరిక_type(ID($cover), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		అమరిక_type(ID($initstate), pool<RTLIL::IdString>(), {ID::Y});
		అమరిక_type(ID($anyconst), pool<RTLIL::IdString>(), {ID::Y});
		అమరిక_type(ID($anyseq), pool<RTLIL::IdString>(), {ID::Y});
		అమరిక_type(ID($allconst), pool<RTLIL::IdString>(), {ID::Y});
		అమరిక_type(ID($allseq), pool<RTLIL::IdString>(), {ID::Y});
		అమరిక_type(ID($equiv), {ID::A, ID::B}, {ID::Y});
		అమరిక_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		అమరిక_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, pool<RTLIL::IdString>());
		అమరిక_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		అమరిక_type(ID($print), {ID::EN, ID::వాదనలు, ID::TRG}, pool<RTLIL::IdString>());
		అమరిక_type(ID($check), {ID::A, ID::EN, ID::వాదనలు, ID::TRG}, pool<RTLIL::IdString>());
		అమరిక_type(ID($సమితి_ట్యాగ్), {ID::A, ID::సమితి_2, ID::CLR}, {ID::Y});
		అమరిక_type(ID($get_ట్యాగ్), {ID::A}, {ID::Y});
		అమరిక_type(ID($overwrite_ట్యాగ్), {ID::A, ID::సమితి_2, ID::CLR}, pool<RTLIL::IdString>());
		అమరిక_type(ID($original_ట్యాగ్), {ID::A}, {ID::Y});
		అమరిక_type(ID($future_ff), {ID::A}, {ID::Y});
		అమరిక_type(ID($scopeinfo), {}, {});
		అమరిక_type(ID($ఆదానం_పోర్టు), {}, {ID::Y});
		అమరిక_type(ID($connect), {ID::A, ID::B}, {});
	}

	శూన్య అమరిక_internals_eval()
	{
		std::సదిశ<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ముక్క), ID($lut), ID($sop)
		};

		std::సదిశ<RTLIL::IdString> ద్విమాన_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($జరుపు), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($కూడు), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ఘాతం),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		కొరకు (స్వయంచాలక type : unary_ops)
			అమరిక_type(type, {ID::A}, {ID::Y}, true);

		కొరకు (స్వయంచాలక type : ద్విమాన_ops)
			అమరిక_type(type, {ID::A, ID::B}, {ID::Y}, true);

		కొరకు (స్వయంచాలక type : std::సదిశ<RTLIL::IdString>({ID($mux), ID($pmux), ID($bwmux)}))
			అమరిక_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, true);

		కొరకు (స్వయంచాలక type : std::సదిశ<RTLIL::IdString>({ID($bmux), ID($demux)}))
			అమరిక_type(type, {ID::A, ID::S}, {ID::Y}, true);

		అమరిక_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, true);
		అమరిక_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, true);
		అమరిక_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		అమరిక_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, true);
	}

	శూన్య అమరిక_internals_ff()
	{
		అమరిక_type(ID($sr), {ID::సమితి_2, ID::CLR}, {ID::Q});
		అమరిక_type(ID($ff), {ID::D}, {ID::Q});
		అమరిక_type(ID($dff), {ID::CLK, ID::D}, {ID::Q});
		అమరిక_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q});
		అమరిక_type(ID($dffsr), {ID::CLK, ID::సమితి_2, ID::CLR, ID::D}, {ID::Q});
		అమరిక_type(ID($dffsre), {ID::CLK, ID::సమితి_2, ID::CLR, ID::D, ID::EN}, {ID::Q});
		అమరిక_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q});
		అమరిక_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q});
		అమరిక_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q});
		అమరిక_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q});
		అమరిక_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q});
		అమరిక_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		అమరిక_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		అమరిక_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q});
		అమరిక_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q});
		అమరిక_type(ID($dlatchsr), {ID::EN, ID::సమితి_2, ID::CLR, ID::D}, {ID::Q});
	}

	శూన్య అమరిక_internals_anyinit()
	{
		అమరిక_type(ID($anyinit), {ID::D}, {ID::Q});
	}

	శూన్య అమరిక_internals_mem()
	{
		అమరిక_internals_ff();

		అమరిక_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::దత్తాంశం_2});
		అమరిక_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::దత్తాంశం_2});
		అమరిక_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::దత్తాంశం_2}, pool<RTLIL::IdString>());
		అమరిక_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::దత్తాంశం_2}, pool<RTLIL::IdString>());
		అమరిక_type(ID($meminit), {ID::ADDR, ID::దత్తాంశం_2}, pool<RTLIL::IdString>());
		అమరిక_type(ID($meminit_v2), {ID::ADDR, ID::దత్తాంశం_2, ID::EN}, pool<RTLIL::IdString>());
		అమరిక_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_దత్తాంశం}, {ID::RD_దత్తాంశం});
		అమరిక_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_దత్తాంశం}, {ID::RD_దత్తాంశం});

		అమరిక_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_లోపల}, {ID::CTRL_బహిర్గతం});
	}

	శూన్య అమరిక_stdcells()
	{
		అమరిక_stdcells_eval();

		అమరిక_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y});
	}

	శూన్య అమరిక_stdcells_eval()
	{
		అమరిక_type(ID($_BUF_), {ID::A}, {ID::Y}, true);
		అమరిక_type(ID($_NOT_), {ID::A}, {ID::Y}, true);
		అమరిక_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, true);
		అమరిక_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		అమరిక_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		అమరిక_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, true);
		అమరిక_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, true);
		అమరిక_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, true);
		అమరిక_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		అమరిక_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		అమరిక_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
		అమరిక_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
	}

	శూన్య అమరిక_stdcells_mem()
	{
		std::సదిశ<అక్షరం> list_np = {'N', 'P'}, list_01 = {'0', '1'};

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
			అమరిక_type(stringf("$_SR_%c%c_", c1, c2), {ID::S, ID::R}, {ID::Q});

		అమరిక_type(ID($_FF_), {ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
			అమరిక_type(stringf("$_DFF_%c_", c1), {ID::C, ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
			అమరిక_type(stringf("$_DFFE_%c%c_", c1, c2), {ID::C, ID::D, ID::E}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_01)
			అమరిక_type(stringf("$_DFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_01)
		కొరకు (స్వయంచాలక c4 : list_np)
			అమరిక_type(stringf("$_DFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
			అమరిక_type(stringf("$_ALDFF_%c%c_", c1, c2), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_np)
			అమరిక_type(stringf("$_ALDFFE_%c%c%c_", c1, c2, c3), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_np)
			అమరిక_type(stringf("$_DFFSR_%c%c%c_", c1, c2, c3), {ID::C, ID::S, ID::R, ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_np)
		కొరకు (స్వయంచాలక c4 : list_np)
			అమరిక_type(stringf("$_DFFSRE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_01)
			అమరిక_type(stringf("$_SDFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_01)
		కొరకు (స్వయంచాలక c4 : list_np)
			అమరిక_type(stringf("$_SDFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_01)
		కొరకు (స్వయంచాలక c4 : list_np)
			అమరిక_type(stringf("$_SDFFCE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
			అమరిక_type(stringf("$_DLATCH_%c_", c1), {ID::E, ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_01)
			అమరిక_type(stringf("$_DLATCH_%c%c%c_", c1, c2, c3), {ID::E, ID::R, ID::D}, {ID::Q});

		కొరకు (స్వయంచాలక c1 : list_np)
		కొరకు (స్వయంచాలక c2 : list_np)
		కొరకు (స్వయంచాలక c3 : list_np)
			అమరిక_type(stringf("$_DLATCHSR_%c%c%c_", c1, c2, c3), {ID::E, ID::S, ID::R, ID::D}, {ID::Q});
	}

	శూన్య clear()
	{
		cell_types.clear();
	}

	bool cell_తెలిసిన(RTLIL::IdString type) స్థిరరాశి
	{
		తిరిగి cell_types.లెక్క(type) != 0;
	}

	bool cell_output(RTLIL::IdString type, RTLIL::IdString పోర్టు) స్థిరరాశి
	{
		స్వయంచాలక it = cell_types.కనుగొను(type);
		తిరిగి it != cell_types.చివర() && it->రెండవ.outputs.లెక్క(పోర్టు) != 0;
	}

	bool cell_ఆదానం(RTLIL::IdString type, RTLIL::IdString పోర్టు) స్థిరరాశి
	{
		స్వయంచాలక it = cell_types.కనుగొను(type);
		తిరిగి it != cell_types.చివర() && it->రెండవ.inputs.లెక్క(పోర్టు) != 0;
	}

	RTLIL::పోర్టు_Dir cell_పోర్టు_dir(RTLIL::IdString type, RTLIL::IdString పోర్టు) స్థిరరాశి
	{
		స్వయంచాలక it = cell_types.కనుగొను(type);
		ఒకవేళ (it == cell_types.చివర())
			తిరిగి RTLIL::PD_UNKNOWN;
		bool is_ఆదానం = it->రెండవ.inputs.లెక్క(పోర్టు);
		bool is_output = it->రెండవ.outputs.లెక్క(పోర్టు);
		తిరిగి RTLIL::పోర్టు_Dir(is_ఆదానం + is_output * 2);
	}

	bool cell_evaluable(RTLIL::IdString type) స్థిరరాశి
	{
		స్వయంచాలక it = cell_types.కనుగొను(type);
		తిరిగి it != cell_types.చివర() && it->రెండవ.is_evaluable;
	}

	స్థిర RTLIL::Const eval_not(RTLIL::Const v)
	{
		కొరకు (స్వయంచాలక బిట్టు : v)
			ఒకవేళ (బిట్టు == State::S0) బిట్టు = State::S1;
			లేకపోతే ఒకవేళ (బిట్టు == State::S1) బిట్టు = State::S0;
		తిరిగి v;
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	స్థిర RTLIL::Const eval(RTLIL::IdString type, స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool signed1, bool signed2, పూర్ణాంకం ఫలితం_len, bool *errp = nullptr)
	{
		ఒకవేళ (type == ID($sshr) && !signed1)
			type = ID($shr);
		ఒకవేళ (type == ID($sshl) && !signed1)
			type = ID($shl);

		ఒకవేళ (type != ID($sshr) && type != ID($sshl) && type != ID($shr) && type != ID($shl) && type != ID($జరుపు) && type != ID($shiftx) &&
				type != ID($pos) && type != ID($buf) && type != ID($neg) && type != ID($not)) {
			ఒకవేళ (!signed1 || !signed2)
				signed1 = false, signed2 = false;
		}

#నిర్వచించు HANDLE_CELL_TYPE(_t) ఒకవేళ (type == constid<ID::lookup("$" #_t).raw()>) తిరిగి const_ ## _t(arg1, arg2, signed1, signed2, ఫలితం_len);
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
		HANDLE_CELL_TYPE(జరుపు)
		HANDLE_CELL_TYPE(shiftx)
		HANDLE_CELL_TYPE(lt)
		HANDLE_CELL_TYPE(le)
		HANDLE_CELL_TYPE(eq)
		HANDLE_CELL_TYPE(ne)
		HANDLE_CELL_TYPE(eqx)
		HANDLE_CELL_TYPE(nex)
		HANDLE_CELL_TYPE(ge)
		HANDLE_CELL_TYPE(gt)
		HANDLE_CELL_TYPE(కూడు)
		HANDLE_CELL_TYPE(sub)
		HANDLE_CELL_TYPE(mul)
		HANDLE_CELL_TYPE(div)
		HANDLE_CELL_TYPE(mod)
		HANDLE_CELL_TYPE(divfloor)
		HANDLE_CELL_TYPE(modfloor)
		HANDLE_CELL_TYPE(ఘాతం)
		HANDLE_CELL_TYPE(pos)
		HANDLE_CELL_TYPE(neg)
#నిర్వచనం_రద్దు HANDLE_CELL_TYPE

		ఒకవేళ (type.లోపల(ID($_BUF_), ID($buf)))
			తిరిగి arg1;
		ఒకవేళ (type == ID($_NOT_))
			తిరిగి eval_not(arg1);
		ఒకవేళ (type == ID($_AND_))
			తిరిగి const_and(arg1, arg2, false, false, 1);
		ఒకవేళ (type == ID($_NAND_))
			తిరిగి eval_not(const_and(arg1, arg2, false, false, 1));
		ఒకవేళ (type == ID($_OR_))
			తిరిగి const_or(arg1, arg2, false, false, 1);
		ఒకవేళ (type == ID($_NOR_))
			తిరిగి eval_not(const_or(arg1, arg2, false, false, 1));
		ఒకవేళ (type == ID($_XOR_))
			తిరిగి const_xor(arg1, arg2, false, false, 1);
		ఒకవేళ (type == ID($_XNOR_))
			తిరిగి const_xnor(arg1, arg2, false, false, 1);
		ఒకవేళ (type == ID($_ANDNOT_))
			తిరిగి const_and(arg1, eval_not(arg2), false, false, 1);
		ఒకవేళ (type == ID($_ORNOT_))
			తిరిగి const_or(arg1, eval_not(arg2), false, false, 1);

		ఒకవేళ (errp != nullptr) {
			*errp = true;
			తిరిగి State::Sm;
		}

		log_abort();
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	స్థిర RTLIL::Const eval(RTLIL::Cell *cell, స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, bool *errp = nullptr)
	{
		ఒకవేళ (cell->type == ID($ముక్క)) {
			పూర్ణాంకం వెడల్పు = cell->parameters.at(ID::Y_వెడల్పు).as_int();
			పూర్ణాంకం స్థానభ్రంశం = cell->parameters.at(ID::స్థానభ్రంశం_2).as_int();
			తిరిగి arg1.వెలికితీయి(స్థానభ్రంశం, వెడల్పు);
		}

		ఒకవేళ (cell->type == ID($concat)) {
			RTLIL::Const ret = arg1;
			ret.append(arg2);
			తిరిగి ret;
		}

		ఒకవేళ (cell->type == ID($bmux))
		{
			తిరిగి const_bmux(arg1, arg2);
		}

		ఒకవేళ (cell->type == ID($demux))
		{
			తిరిగి const_demux(arg1, arg2);
		}

		ఒకవేళ (cell->type == ID($bweqx))
		{
			తిరిగి const_bweqx(arg1, arg2);
		}

		ఒకవేళ (cell->type == ID($lut))
		{
			పూర్ణాంకం వెడల్పు = cell->parameters.at(ID::వెడల్పు_2).as_int();

			std::సదిశ<RTLIL::State> t = cell->parameters.at(ID::LUT).గమ్యం_బిట్లు();
			అయితే (Get_కొలత(t) < (1 << వెడల్పు))
				t.నెట్టు_వెనుక(State::S0);
			t.resize(1 << వెడల్పు);

			తిరిగి const_bmux(t, arg1);
		}

		ఒకవేళ (cell->type == ID($sop))
		{
			పూర్ణాంకం వెడల్పు = cell->parameters.at(ID::వెడల్పు_2).as_int();
			పూర్ణాంకం లోతు = cell->parameters.at(ID::లోతు_2).as_int();
			std::సదిశ<RTLIL::State> t = cell->parameters.at(ID::పట్టిక).గమ్యం_బిట్లు();

			అయితే (Get_కొలత(t) < వెడల్పు*లోతు*2)
				t.నెట్టు_వెనుక(State::S0);

			RTLIL::State default_ret = State::S0;

			కొరకు (పూర్ణాంకం i = 0; i < లోతు; i++)
			{
				bool సరిపోలిక = true;
				bool సరిపోలిక_x = true;

				కొరకు (పూర్ణాంకం j = 0; j < వెడల్పు; j++) {
					RTLIL::State a = arg1.at(j);
					ఒకవేళ (t.at(2*వెడల్పు*i + 2*j + 0) == State::S1) {
						ఒకవేళ (a == State::S1) సరిపోలిక_x = false;
						ఒకవేళ (a != State::S0) సరిపోలిక = false;
					}
					ఒకవేళ (t.at(2*వెడల్పు*i + 2*j + 1) == State::S1) {
						ఒకవేళ (a == State::S0) సరిపోలిక_x = false;
						ఒకవేళ (a != State::S1) సరిపోలిక = false;
					}
				}

				ఒకవేళ (సరిపోలిక)
					తిరిగి State::S1;

				ఒకవేళ (సరిపోలిక_x)
					default_ret = State::Sx;
			}

			తిరిగి default_ret;
		}

		bool signed_a = cell->parameters.లెక్క(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
		bool signed_b = cell->parameters.లెక్క(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();
		పూర్ణాంకం ఫలితం_len = cell->parameters.లెక్క(ID::Y_వెడల్పు) > 0 ? cell->parameters[ID::Y_వెడల్పు].as_int() : -1;
		తిరిగి eval(cell->type, arg1, arg2, signed_a, signed_b, ఫలితం_len, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	స్థిర RTLIL::Const eval(RTLIL::Cell *cell, స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3, bool *errp = nullptr)
	{
		ఒకవేళ (cell->type.లోపల(ID($mux), ID($_MUX_)))
			తిరిగి const_mux(arg1, arg2, arg3);
		ఒకవేళ (cell->type == ID($_NMUX_))
			తిరిగి eval_not(const_mux(arg1, arg2, arg3));
		ఒకవేళ (cell->type == ID($bwmux))
			తిరిగి const_bwmux(arg1, arg2, arg3);
		ఒకవేళ (cell->type == ID($pmux))
			తిరిగి const_pmux(arg1, arg2, arg3);
		ఒకవేళ (cell->type == ID($_AOI3_))
			తిరిగి eval_not(const_or(const_and(arg1, arg2, false, false, 1), arg3, false, false, 1));
		ఒకవేళ (cell->type == ID($_OAI3_))
			తిరిగి eval_not(const_and(const_or(arg1, arg2, false, false, 1), arg3, false, false, 1));

		log_assert(arg3.కొలత() == 0);
		తిరిగి eval(cell, arg1, arg2, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	స్థిర RTLIL::Const eval(RTLIL::Cell *cell, స్థిరరాశి RTLIL::Const &arg1, స్థిరరాశి RTLIL::Const &arg2, స్థిరరాశి RTLIL::Const &arg3, స్థిరరాశి RTLIL::Const &arg4, bool *errp = nullptr)
	{
		ఒకవేళ (cell->type == ID($_AOI4_))
			తిరిగి eval_not(const_or(const_and(arg1, arg2, false, false, 1), const_and(arg3, arg4, false, false, 1), false, false, 1));
		ఒకవేళ (cell->type == ID($_OAI4_))
			తిరిగి eval_not(const_and(const_or(arg1, arg2, false, false, 1), const_or(arg3, arg4, false, false, 1), false, false, 1));

		log_assert(arg4.కొలత() == 0);
		తిరిగి eval(cell, arg1, arg2, arg3, errp);
	}
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
