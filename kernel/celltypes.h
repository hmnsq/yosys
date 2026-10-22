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

#جے_بنا_تعریف CELLTYPES_H
#تعریف_کرو CELLTYPES_H

#ضمن "kernel/yosys.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

بنية CellType
{
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

بنية CellTypes
{
	dict<RTLIL::IdString, CellType> cell_types;

	CellTypes()
	{
	}

	CellTypes(RTLIL::Design *design)
	{
		تیاری(design);
	}

	فراغ تیاری(RTLIL::Design *design = عدم)
	{
		إذا (design)
			تیاری_design(design);

		تیاری_internals();
		تیاری_internals_mem();
		تیاری_internals_anyinit();
		تیاری_stdcells();
		تیاری_stdcells_mem();
	}

	فراغ تیاری_type(RTLIL::IdString type, ثابت pool<RTLIL::IdString> &inputs, ثابت pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		CellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		cell_types[ct.type] = ct;
	}

	سانچا<typename N, YS_NAME_STRING(N)>
	فراغ تیاری_type(ثابت N &type_str, ثابت pool<RTLIL::IdString> &inputs, ثابت pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false)
	{
		تیاری_type(ID::lookup(type_str), inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable);
	}

	فراغ تیاری_module(RTLIL::Module *module)
	{
		pool<IdString> inputs, outputs;
		لكل (خودکار wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			إذا (wire->پورٹ_داخلہ)
				inputs.داخل_کرو(wire->name);
			إذا (wire->پورٹ_output)
				outputs.داخل_کرو(wire->name);
		}
		تیاری_type(module->name, inputs, outputs);
	}

	فراغ تیاری_design(RTLIL::Design *design)
	{
		لكل (خودکار module : design->modules())
			تیاری_module(module);
	}

	فراغ تیاری_internals()
	{
		تیاری_internals_eval();

		تیاری_type(ID($barrier), {ID::A}, {ID::Y});

		تیاری_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y});

		تیاری_type(ID($أكد), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		تیاری_type(ID($assume), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		تیاری_type(ID($جیوندا), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		تیاری_type(ID($fair), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		تیاری_type(ID($cover), {ID::A, ID::EN}, pool<RTLIL::IdString>());
		تیاری_type(ID($initstate), pool<RTLIL::IdString>(), {ID::Y});
		تیاری_type(ID($anyconst), pool<RTLIL::IdString>(), {ID::Y});
		تیاری_type(ID($anyseq), pool<RTLIL::IdString>(), {ID::Y});
		تیاری_type(ID($allconst), pool<RTLIL::IdString>(), {ID::Y});
		تیاری_type(ID($allseq), pool<RTLIL::IdString>(), {ID::Y});
		تیاری_type(ID($equiv), {ID::A, ID::B}, {ID::Y});
		تیاری_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		تیاری_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, pool<RTLIL::IdString>());
		تیاری_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, pool<RTLIL::IdString>());
		تیاری_type(ID($print), {ID::EN, ID::دلیلاں, ID::TRG}, pool<RTLIL::IdString>());
		تیاری_type(ID($check), {ID::A, ID::EN, ID::دلیلاں, ID::TRG}, pool<RTLIL::IdString>());
		تیاری_type(ID($سیٹ_ٹیگ), {ID::A, ID::سیٹ_2, ID::CLR}, {ID::Y});
		تیاری_type(ID($get_ٹیگ), {ID::A}, {ID::Y});
		تیاری_type(ID($overwrite_ٹیگ), {ID::A, ID::سیٹ_2, ID::CLR}, pool<RTLIL::IdString>());
		تیاری_type(ID($original_ٹیگ), {ID::A}, {ID::Y});
		تیاری_type(ID($future_ff), {ID::A}, {ID::Y});
		تیاری_type(ID($scopeinfo), {}, {});
		تیاری_type(ID($داخلہ_پورٹ), {}, {ID::Y});
		تیاری_type(ID($connect), {ID::A, ID::B}, {});
	}

	فراغ تیاری_internals_eval()
	{
		std::سمتیہ<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ٹکڑا), ID($lut), ID($sop)
		};

		std::سمتیہ<RTLIL::IdString> ثنائی_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($کھسکاؤ), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($جوڑو), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		لكل (خودکار type : unary_ops)
			تیاری_type(type, {ID::A}, {ID::Y}, true);

		لكل (خودکار type : ثنائی_ops)
			تیاری_type(type, {ID::A, ID::B}, {ID::Y}, true);

		لكل (خودکار type : std::سمتیہ<RTLIL::IdString>({ID($mux), ID($pmux), ID($bwmux)}))
			تیاری_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, true);

		لكل (خودکار type : std::سمتیہ<RTLIL::IdString>({ID($bmux), ID($demux)}))
			تیاری_type(type, {ID::A, ID::S}, {ID::Y}, true);

		تیاری_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, true);
		تیاری_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, true);
		تیاری_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		تیاری_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, true);
	}

	فراغ تیاری_internals_ff()
	{
		تیاری_type(ID($sr), {ID::سیٹ_2, ID::CLR}, {ID::Q});
		تیاری_type(ID($ff), {ID::D}, {ID::Q});
		تیاری_type(ID($dff), {ID::CLK, ID::D}, {ID::Q});
		تیاری_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q});
		تیاری_type(ID($dffsr), {ID::CLK, ID::سیٹ_2, ID::CLR, ID::D}, {ID::Q});
		تیاری_type(ID($dffsre), {ID::CLK, ID::سیٹ_2, ID::CLR, ID::D, ID::EN}, {ID::Q});
		تیاری_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q});
		تیاری_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q});
		تیاری_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q});
		تیاری_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q});
		تیاری_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q});
		تیاری_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		تیاری_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q});
		تیاری_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q});
		تیاری_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q});
		تیاری_type(ID($dlatchsr), {ID::EN, ID::سیٹ_2, ID::CLR, ID::D}, {ID::Q});
	}

	فراغ تیاری_internals_anyinit()
	{
		تیاری_type(ID($anyinit), {ID::D}, {ID::Q});
	}

	فراغ تیاری_internals_mem()
	{
		تیاری_internals_ff();

		تیاری_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::انکڑے_2});
		تیاری_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::انکڑے_2});
		تیاری_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::انکڑے_2}, pool<RTLIL::IdString>());
		تیاری_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::انکڑے_2}, pool<RTLIL::IdString>());
		تیاری_type(ID($meminit), {ID::ADDR, ID::انکڑے_2}, pool<RTLIL::IdString>());
		تیاری_type(ID($meminit_v2), {ID::ADDR, ID::انکڑے_2, ID::EN}, pool<RTLIL::IdString>());
		تیاری_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_انکڑے}, {ID::RD_انکڑے});
		تیاری_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_انکڑے}, {ID::RD_انکڑے});

		تیاری_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_وچ}, {ID::CTRL_باہرلا});
	}

	فراغ تیاری_stdcells()
	{
		تیاری_stdcells_eval();

		تیاری_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y});
	}

	فراغ تیاری_stdcells_eval()
	{
		تیاری_type(ID($_BUF_), {ID::A}, {ID::Y}, true);
		تیاری_type(ID($_NOT_), {ID::A}, {ID::Y}, true);
		تیاری_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, true);
		تیاری_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		تیاری_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, true);
		تیاری_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, true);
		تیاری_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, true);
		تیاری_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, true);
		تیاری_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		تیاری_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, true);
		تیاری_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
		تیاری_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, true);
	}

	فراغ تیاری_stdcells_mem()
	{
		std::سمتیہ<حرف> list_np = {'N', 'P'}, list_01 = {'0', '1'};

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
			تیاری_type(stringf("$_SR_%c%c_", c1, c2), {ID::S, ID::R}, {ID::Q});

		تیاری_type(ID($_FF_), {ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
			تیاری_type(stringf("$_DFF_%c_", c1), {ID::C, ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
			تیاری_type(stringf("$_DFFE_%c%c_", c1, c2), {ID::C, ID::D, ID::E}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_01)
			تیاری_type(stringf("$_DFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_01)
		لكل (خودکار c4 : list_np)
			تیاری_type(stringf("$_DFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
			تیاری_type(stringf("$_ALDFF_%c%c_", c1, c2), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_np)
			تیاری_type(stringf("$_ALDFFE_%c%c%c_", c1, c2, c3), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_np)
			تیاری_type(stringf("$_DFFSR_%c%c%c_", c1, c2, c3), {ID::C, ID::S, ID::R, ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_np)
		لكل (خودکار c4 : list_np)
			تیاری_type(stringf("$_DFFSRE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_01)
			تیاری_type(stringf("$_SDFF_%c%c%c_", c1, c2, c3), {ID::C, ID::R, ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_01)
		لكل (خودکار c4 : list_np)
			تیاری_type(stringf("$_SDFFE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_01)
		لكل (خودکار c4 : list_np)
			تیاری_type(stringf("$_SDFFCE_%c%c%c%c_", c1, c2, c3, c4), {ID::C, ID::R, ID::D, ID::E}, {ID::Q});

		لكل (خودکار c1 : list_np)
			تیاری_type(stringf("$_DLATCH_%c_", c1), {ID::E, ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_01)
			تیاری_type(stringf("$_DLATCH_%c%c%c_", c1, c2, c3), {ID::E, ID::R, ID::D}, {ID::Q});

		لكل (خودکار c1 : list_np)
		لكل (خودکار c2 : list_np)
		لكل (خودکار c3 : list_np)
			تیاری_type(stringf("$_DLATCHSR_%c%c%c_", c1, c2, c3), {ID::E, ID::S, ID::R, ID::D}, {ID::Q});
	}

	فراغ clear()
	{
		cell_types.clear();
	}

	bool cell_معلوم(RTLIL::IdString type) ثابت
	{
		أرجع cell_types.گنتی(type) != 0;
	}

	bool cell_output(RTLIL::IdString type, RTLIL::IdString پورٹ) ثابت
	{
		خودکار it = cell_types.لبھو(type);
		أرجع it != cell_types.سرا() && it->دوجا.outputs.گنتی(پورٹ) != 0;
	}

	bool cell_داخلہ(RTLIL::IdString type, RTLIL::IdString پورٹ) ثابت
	{
		خودکار it = cell_types.لبھو(type);
		أرجع it != cell_types.سرا() && it->دوجا.inputs.گنتی(پورٹ) != 0;
	}

	RTLIL::پورٹ_Dir cell_پورٹ_dir(RTLIL::IdString type, RTLIL::IdString پورٹ) ثابت
	{
		خودکار it = cell_types.لبھو(type);
		إذا (it == cell_types.سرا())
			أرجع RTLIL::PD_UNKNOWN;
		bool is_داخلہ = it->دوجا.inputs.گنتی(پورٹ);
		bool is_output = it->دوجا.outputs.گنتی(پورٹ);
		أرجع RTLIL::پورٹ_Dir(is_داخلہ + is_output * 2);
	}

	bool cell_evaluable(RTLIL::IdString type) ثابت
	{
		خودکار it = cell_types.لبھو(type);
		أرجع it != cell_types.سرا() && it->دوجا.is_evaluable;
	}

	ساكن RTLIL::Const eval_not(RTLIL::Const v)
	{
		لكل (خودکار ثنائی_ہندسہ : v)
			إذا (ثنائی_ہندسہ == State::S0) ثنائی_ہندسہ = State::S1;
			وإلا إذا (ثنائی_ہندسہ == State::S1) ثنائی_ہندسہ = State::S0;
		أرجع v;
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ساكن RTLIL::Const eval(RTLIL::IdString type, ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool signed1, bool signed2, صحيح نتیجہ_len, bool *errp = nullptr)
	{
		إذا (type == ID($sshr) && !signed1)
			type = ID($shr);
		إذا (type == ID($sshl) && !signed1)
			type = ID($shl);

		إذا (type != ID($sshr) && type != ID($sshl) && type != ID($shr) && type != ID($shl) && type != ID($کھسکاؤ) && type != ID($shiftx) &&
				type != ID($pos) && type != ID($buf) && type != ID($neg) && type != ID($not)) {
			إذا (!signed1 || !signed2)
				signed1 = false, signed2 = false;
		}

#تعریف_کرو HANDLE_CELL_TYPE(_t) إذا (type == constid<ID::lookup("$" #_t).raw()>) أرجع const_ ## _t(arg1, arg2, signed1, signed2, نتیجہ_len);
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
		HANDLE_CELL_TYPE(کھسکاؤ)
		HANDLE_CELL_TYPE(shiftx)
		HANDLE_CELL_TYPE(lt)
		HANDLE_CELL_TYPE(le)
		HANDLE_CELL_TYPE(eq)
		HANDLE_CELL_TYPE(ne)
		HANDLE_CELL_TYPE(eqx)
		HANDLE_CELL_TYPE(nex)
		HANDLE_CELL_TYPE(ge)
		HANDLE_CELL_TYPE(gt)
		HANDLE_CELL_TYPE(جوڑو)
		HANDLE_CELL_TYPE(sub)
		HANDLE_CELL_TYPE(mul)
		HANDLE_CELL_TYPE(div)
		HANDLE_CELL_TYPE(mod)
		HANDLE_CELL_TYPE(divfloor)
		HANDLE_CELL_TYPE(modfloor)
		HANDLE_CELL_TYPE(قوت)
		HANDLE_CELL_TYPE(pos)
		HANDLE_CELL_TYPE(neg)
#تعریف_رد HANDLE_CELL_TYPE

		إذا (type.وچ(ID($_BUF_), ID($buf)))
			أرجع arg1;
		إذا (type == ID($_NOT_))
			أرجع eval_not(arg1);
		إذا (type == ID($_AND_))
			أرجع const_and(arg1, arg2, false, false, 1);
		إذا (type == ID($_NAND_))
			أرجع eval_not(const_and(arg1, arg2, false, false, 1));
		إذا (type == ID($_OR_))
			أرجع const_or(arg1, arg2, false, false, 1);
		إذا (type == ID($_NOR_))
			أرجع eval_not(const_or(arg1, arg2, false, false, 1));
		إذا (type == ID($_XOR_))
			أرجع const_xor(arg1, arg2, false, false, 1);
		إذا (type == ID($_XNOR_))
			أرجع const_xnor(arg1, arg2, false, false, 1);
		إذا (type == ID($_ANDNOT_))
			أرجع const_and(arg1, eval_not(arg2), false, false, 1);
		إذا (type == ID($_ORNOT_))
			أرجع const_or(arg1, eval_not(arg2), false, false, 1);

		إذا (errp != nullptr) {
			*errp = true;
			أرجع State::Sm;
		}

		log_abort();
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ساكن RTLIL::Const eval(RTLIL::Cell *cell, ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, bool *errp = nullptr)
	{
		إذا (cell->type == ID($ٹکڑا)) {
			صحيح چوڑائی = cell->parameters.at(ID::Y_چوڑائی).as_int();
			صحيح وتھ = cell->parameters.at(ID::وتھ_2).as_int();
			أرجع arg1.کڈھو(وتھ, چوڑائی);
		}

		إذا (cell->type == ID($concat)) {
			RTLIL::Const ret = arg1;
			ret.append(arg2);
			أرجع ret;
		}

		إذا (cell->type == ID($bmux))
		{
			أرجع const_bmux(arg1, arg2);
		}

		إذا (cell->type == ID($demux))
		{
			أرجع const_demux(arg1, arg2);
		}

		إذا (cell->type == ID($bweqx))
		{
			أرجع const_bweqx(arg1, arg2);
		}

		إذا (cell->type == ID($lut))
		{
			صحيح چوڑائی = cell->parameters.at(ID::چوڑائی_2).as_int();

			std::سمتیہ<RTLIL::State> t = cell->parameters.at(ID::LUT).منزل_بٹ();
			طالما (Get_ناپ(t) < (1 << چوڑائی))
				t.ڈالو_پٹھ(State::S0);
			t.resize(1 << چوڑائی);

			أرجع const_bmux(t, arg1);
		}

		إذا (cell->type == ID($sop))
		{
			صحيح چوڑائی = cell->parameters.at(ID::چوڑائی_2).as_int();
			صحيح ڈونگھائی = cell->parameters.at(ID::ڈونگھائی_2).as_int();
			std::سمتیہ<RTLIL::State> t = cell->parameters.at(ID::جدول).منزل_بٹ();

			طالما (Get_ناپ(t) < چوڑائی*ڈونگھائی*2)
				t.ڈالو_پٹھ(State::S0);

			RTLIL::State default_ret = State::S0;

			لكل (صحيح i = 0; i < ڈونگھائی; i++)
			{
				bool میل = true;
				bool میل_x = true;

				لكل (صحيح j = 0; j < چوڑائی; j++) {
					RTLIL::State a = arg1.at(j);
					إذا (t.at(2*چوڑائی*i + 2*j + 0) == State::S1) {
						إذا (a == State::S1) میل_x = false;
						إذا (a != State::S0) میل = false;
					}
					إذا (t.at(2*چوڑائی*i + 2*j + 1) == State::S1) {
						إذا (a == State::S0) میل_x = false;
						إذا (a != State::S1) میل = false;
					}
				}

				إذا (میل)
					أرجع State::S1;

				إذا (میل_x)
					default_ret = State::Sx;
			}

			أرجع default_ret;
		}

		bool signed_a = cell->parameters.گنتی(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
		bool signed_b = cell->parameters.گنتی(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();
		صحيح نتیجہ_len = cell->parameters.گنتی(ID::Y_چوڑائی) > 0 ? cell->parameters[ID::Y_چوڑائی].as_int() : -1;
		أرجع eval(cell->type, arg1, arg2, signed_a, signed_b, نتیجہ_len, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ساكن RTLIL::Const eval(RTLIL::Cell *cell, ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3, bool *errp = nullptr)
	{
		إذا (cell->type.وچ(ID($mux), ID($_MUX_)))
			أرجع const_mux(arg1, arg2, arg3);
		إذا (cell->type == ID($_NMUX_))
			أرجع eval_not(const_mux(arg1, arg2, arg3));
		إذا (cell->type == ID($bwmux))
			أرجع const_bwmux(arg1, arg2, arg3);
		إذا (cell->type == ID($pmux))
			أرجع const_pmux(arg1, arg2, arg3);
		إذا (cell->type == ID($_AOI3_))
			أرجع eval_not(const_or(const_and(arg1, arg2, false, false, 1), arg3, false, false, 1));
		إذا (cell->type == ID($_OAI3_))
			أرجع eval_not(const_and(const_or(arg1, arg2, false, false, 1), arg3, false, false, 1));

		log_assert(arg3.ناپ() == 0);
		أرجع eval(cell, arg1, arg2, errp);
	}

	// Consider using the ConstEval struct instead if you need named ports and/or multiple outputs
	ساكن RTLIL::Const eval(RTLIL::Cell *cell, ثابت RTLIL::Const &arg1, ثابت RTLIL::Const &arg2, ثابت RTLIL::Const &arg3, ثابت RTLIL::Const &arg4, bool *errp = nullptr)
	{
		إذا (cell->type == ID($_AOI4_))
			أرجع eval_not(const_or(const_and(arg1, arg2, false, false, 1), const_and(arg3, arg4, false, false, 1), false, false, 1));
		إذا (cell->type == ID($_OAI4_))
			أرجع eval_not(const_and(const_or(arg1, arg2, false, false, 1), const_or(arg3, arg4, false, false, 1), false, false, 1));

		log_assert(arg4.ناپ() == 0);
		أرجع eval(cell, arg1, arg2, arg3, errp);
	}
};

YOSYS_NAMESPACE_سرا

#جے_انت
