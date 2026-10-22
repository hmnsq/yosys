#که_نه_تعریف_شوی NEWCELLTYPES_H
#تعریف_کړه NEWCELLTYPES_H

#ضمن "kernel/rtlil.h"
#ضمن "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

/**
 * This API is unstable.
 * It may change or be removed in future versions and break dependent code.
 */

namespace StaticCellTypes {

// Given by last internal cell type IdString constids.inc, compilation error if too low
constexpr صحيح MAX_CELLS = 300;
// Currently given by _MUX16_, compilation error if too low
constexpr صحيح MAX_PORTS = 20;
بنية Cell_جدول_Builder {
	بنية پورټ_List {
		std::array<RTLIL::IdString, MAX_PORTS> ports{};
		اندازه_t شمېر = 0;
		constexpr پورټ_List() = افتراضي;
		constexpr پورټ_List(std::initializer_list<RTLIL::IdString> init) {
			لكل (خپلکاری p : init) {
				ports[شمېر++] = p;
			}
		}
		constexpr خپلکاری begin() ثابت { أرجع ports.begin(); }
		constexpr خپلکاری پای_ټکی() ثابت { أرجع ports.begin() + شمېر; }
		constexpr bool لري(RTLIL::IdString پورټ) ثابت {
			لكل (اندازه_t i = 0; i < شمېر; i++) {
				إذا (پورټ == ports[i])
					أرجع true;
			}

			أرجع false;
		}
		constexpr اندازه_t اندازه() ثابت { أرجع شمېر; }
	};
	بنية Features {
		bool is_evaluable = false;
		bool is_combinatorial = false;
		bool is_synthesizable = false;
		bool is_stdcell = false;
		bool is_ff = false;
		bool is_mem_noff = false;
		bool is_anyinit = false;
		bool is_tristate = false;
	};
	بنية CellInfo {
		RTLIL::IdString type;
		پورټ_List inputs, outputs;
		Features features;
	};
	std::array<CellInfo, MAX_CELLS> cells{};
	اندازه_t شمېر = 0;

	constexpr فراغ تیاری_type(RTLIL::IdString type, std::initializer_list<RTLIL::IdString> inputs, std::initializer_list<RTLIL::IdString> outputs, ثابت Features& features) {
		cells[شمېر++] = {type, پورټ_List(inputs), پورټ_List(outputs), features};
	}
	constexpr فراغ تیاری_internals_other()
	{
		Features features {};
		features.is_tristate = true;
		تیاری_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y}, features);

		features = {};
		تیاری_type(ID($barrier), {ID::A}, {ID::Y}, features);

		features = {};
		تیاری_type(ID($أكد), {ID::A, ID::EN}, {}, features);
		تیاری_type(ID($assume), {ID::A, ID::EN}, {}, features);
		تیاری_type(ID($ژوندی), {ID::A, ID::EN}, {}, features);
		تیاری_type(ID($fair), {ID::A, ID::EN}, {}, features);
		تیاری_type(ID($cover), {ID::A, ID::EN}, {}, features);
		تیاری_type(ID($initstate), {}, {ID::Y}, features);
		تیاری_type(ID($anyconst), {}, {ID::Y}, features);
		تیاری_type(ID($anyseq), {}, {ID::Y}, features);
		تیاری_type(ID($allconst), {}, {ID::Y}, features);
		تیاری_type(ID($allseq), {}, {ID::Y}, features);
		تیاری_type(ID($equiv), {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, {}, features);
		تیاری_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, {}, features);
		تیاری_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, {}, features);
		تیاری_type(ID($print), {ID::EN, ID::دلیلونه, ID::TRG}, {}, features);
		تیاری_type(ID($check), {ID::A, ID::EN, ID::دلیلونه, ID::TRG}, {}, features);
		تیاری_type(ID($سېټ_نښان), {ID::A, ID::سېټ_2, ID::CLR}, {ID::Y}, features);
		تیاری_type(ID($get_نښان), {ID::A}, {ID::Y}, features);
		تیاری_type(ID($overwrite_نښان), {ID::A, ID::سېټ_2, ID::CLR}, {}, features);
		تیاری_type(ID($original_نښان), {ID::A}, {ID::Y}, features);
		تیاری_type(ID($future_ff), {ID::A}, {ID::Y}, features);
		تیاری_type(ID($scopeinfo), {}, {}, features);
		تیاری_type(ID($ننوت_پورټ), {}, {ID::Y}, features);
		تیاری_type(ID($connect), {ID::A, ID::B}, {}, features);
	}
	constexpr فراغ تیاری_internals_eval()
	{
		Features features {};
		features.is_evaluable = true;
		std::initializer_list<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ټوټه), ID($lut), ID($sop)
		};

		std::initializer_list<RTLIL::IdString> دوګونی_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ښويدنه), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($جمع_کړه), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		لكل (خپلکاری type : unary_ops)
			تیاری_type(type, {ID::A}, {ID::Y}, features);

		لكل (خپلکاری type : دوګونی_ops)
			تیاری_type(type, {ID::A, ID::B}, {ID::Y}, features);

		لكل (خپلکاری type : {ID($mux), ID($pmux), ID($bwmux)})
			تیاری_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, features);

		لكل (خپلکاری type : {ID($bmux), ID($demux)})
			تیاری_type(type, {ID::A, ID::S}, {ID::Y}, features);

		تیاری_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, features);
		تیاری_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, features);
		تیاری_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		تیاری_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, features);
	}
	constexpr فراغ تیاری_internals_ff()
	{
		Features features {};
		features.is_ff = true;
		تیاری_type(ID($sr), {ID::سېټ_2, ID::CLR}, {ID::Q}, features);
		تیاری_type(ID($ff), {ID::D}, {ID::Q}, features);
		تیاری_type(ID($dff), {ID::CLK, ID::D}, {ID::Q}, features);
		تیاری_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q}, features);
		تیاری_type(ID($dffsr), {ID::CLK, ID::سېټ_2, ID::CLR, ID::D}, {ID::Q}, features);
		تیاری_type(ID($dffsre), {ID::CLK, ID::سېټ_2, ID::CLR, ID::D, ID::EN}, {ID::Q}, features);
		تیاری_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q}, features);
		تیاری_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q}, features);
		تیاری_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q}, features);
		تیاری_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q}, features);
		تیاری_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q}, features);
		تیاری_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		تیاری_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		تیاری_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q}, features);
		تیاری_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q}, features);
		تیاری_type(ID($dlatchsr), {ID::EN, ID::سېټ_2, ID::CLR, ID::D}, {ID::Q}, features);
	}
	constexpr فراغ تیاری_internals_anyinit()
	{
		Features features {};
		features.is_anyinit = true;
		تیاری_type(ID($anyinit), {ID::D}, {ID::Q}, features);
	}
	constexpr فراغ تیاری_internals_mem_noff()
	{
		Features features {};
		features.is_mem_noff = true;
		// NOT setup_internals_ff()

		تیاری_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::معلومات_2}, features);
		تیاری_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::معلومات_2}, features);
		تیاری_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::معلومات_2}, {}, features);
		تیاری_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::معلومات_2}, {}, features);
		تیاری_type(ID($meminit), {ID::ADDR, ID::معلومات_2}, {}, features);
		تیاری_type(ID($meminit_v2), {ID::ADDR, ID::معلومات_2, ID::EN}, {}, features);
		تیاری_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_معلومات}, {ID::RD_معلومات}, features);
		تیاری_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_معلومات}, {ID::RD_معلومات}, features);

		// What?
		تیاری_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_دننه}, {ID::CTRL_بهر}, features);
	}
	constexpr فراغ تیاری_stdcells_tristate()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_tristate = true;
		تیاری_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y}, features);
	}

	constexpr فراغ تیاری_stdcells_eval()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_evaluable = true;
		تیاری_type(ID($_BUF_), {ID::A}, {ID::Y}, features);
		تیاری_type(ID($_NOT_), {ID::A}, {ID::Y}, features);
		تیاری_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, features);
		تیاری_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		تیاری_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		تیاری_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, features);
		تیاری_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, features);
		تیاری_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, features);
		تیاری_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		تیاری_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		تیاری_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
		تیاری_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
	}

	constexpr فراغ تیاری_stdcells_ff() {
		Features features {};
		features.is_stdcell = true;
		features.is_ff = true;

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_SR_") + c1 + c2 + "_", {ID::S, ID::R}, {ID::Q}, features);
		تیاری_type(ID($_SR_NN_), {ID::S, ID::R}, {ID::Q}, features);
		تیاری_type(ID($_SR_NP_), {ID::S, ID::R}, {ID::Q}, features);
		تیاری_type(ID($_SR_PN_), {ID::S, ID::R}, {ID::Q}, features);
		تیاری_type(ID($_SR_PP_), {ID::S, ID::R}, {ID::Q}, features);

		تیاری_type(ID($_FF_), {ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DFF_") + c1 + "_", {ID::C, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_N_), {ID::C, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_P_), {ID::C, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + "_", {ID::C, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_ALDFF_") + c1 + c2 + "_", {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_ALDFF_NN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_ALDFF_NP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_ALDFF_PN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_ALDFF_PP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_ALDFFE_") + c1 + c2 + c3 + "_", {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_NNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_NNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_NPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_NPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_PNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_PNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_PPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_ALDFFE_PPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DFFSR_") + c1 + c2 + c3 + "_", {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_NNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_NNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_NPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_NPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_PNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_PNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_PPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DFFSR_PPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFSRE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_NPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_DFFSRE_PPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_SDFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_SDFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFCE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		تیاری_type(ID($_SDFFCE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DLATCH_") + c1 + "_", {ID::E, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_N_), {ID::E, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_P_), {ID::E, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DLATCH_") + c1 + c2 + c3 + "_", {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_NN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_NN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_NP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_NP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_PN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_PN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_PP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCH_PP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DLATCHSR_") + c1 + c2 + c3 + "_", {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_NNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_NNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_NPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_NPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_PNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_PNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_PPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		تیاری_type(ID($_DLATCHSR_PPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
	}
	constexpr Cell_جدول_Builder() {
		تیاری_internals_other();
		تیاری_internals_eval();
		تیاری_internals_ff();
		تیاری_internals_anyinit();
		تیاری_internals_mem_noff();
		تیاری_stdcells_tristate();
		تیاری_stdcells_eval();
		تیاری_stdcells_ff();
	}

};

constexpr Cell_جدول_Builder builder {};

بنية پورټ_Info {
	بنية پورټ_Lists {
		std::array<Cell_جدول_Builder::پورټ_List, MAX_CELLS> معلومات{};
		constexpr Cell_جدول_Builder::پورټ_List چلوونکی()(IdString type) ثابت {
			أرجع معلومات[type.raw()];
		}
		constexpr Cell_جدول_Builder::پورټ_List& چلوونکی[](اندازه_t idx) {
			أرجع معلومات[idx];
		}
		constexpr اندازه_t اندازه() ثابت { أرجع معلومات.اندازه(); }
	};
	پورټ_Lists inputs {};
	پورټ_Lists outputs {};
	constexpr پورټ_Info() {
		لكل (اندازه_t i = 0; i < builder.شمېر; ++i) {
			خپلکاری& cell = builder.cells[i];
			اندازه_t idx = cell.type.raw();
			inputs[idx] = cell.inputs;
			outputs[idx] = cell.outputs;
		}
	}
};

بنية Categories {
	بنية Category {
		std::array<bool, MAX_CELLS> معلومات{};
		constexpr bool چلوونکی()(IdString type) ثابت {
			اندازه_t idx = type.raw();
			إذا (idx >= MAX_CELLS)
				أرجع false;
			أرجع معلومات[idx];
		}
		constexpr bool چلوونکی[](اندازه_t idx) {
			أرجع معلومات[idx];
		}
		constexpr فراغ سېټ_id(IdString type, bool val = true) {
			اندازه_t idx = type.raw();
			إذا (idx >= MAX_CELLS) {
				إذا (std::is_constant_evaluated())
					throw "cell type index out of range";
				log_abort();
			}
			معلومات[idx] = val;
		}
		constexpr فراغ سېټ(اندازه_t idx, bool val = true) {
			معلومات[idx] = val;
		}
		constexpr اندازه_t اندازه() ثابت { أرجع معلومات.اندازه(); }
	};
	Category تشه {};
	Category is_معلوم {};
	Category is_evaluable {};
	Category is_combinatorial {};
	Category is_synthesizable {};
	Category is_stdcell {};
	Category is_ff {};
	Category is_mem_noff {};
	Category is_anyinit {};
	Category is_tristate {};
	constexpr Categories() {
		لكل (اندازه_t i = 0; i < builder.شمېر; ++i) {
			خپلکاری& cell = builder.cells[i];
			اندازه_t idx = cell.type.raw();
			is_معلوم.سېټ(idx);
			is_evaluable.سېټ(idx, cell.features.is_evaluable);
			is_combinatorial.سېټ(idx, cell.features.is_combinatorial);
			is_synthesizable.سېټ(idx, cell.features.is_synthesizable);
			is_stdcell.سېټ(idx, cell.features.is_stdcell);
			is_ff.سېټ(idx, cell.features.is_ff);
			is_mem_noff.سېټ(idx, cell.features.is_mem_noff);
			is_anyinit.سېټ(idx, cell.features.is_anyinit);
			is_tristate.سېټ(idx, cell.features.is_tristate);
		}
	}
	constexpr ساكن Category join(Category کیڼ_3, Category ښی_3) {
		Category c {};
		لكل (اندازه_t i = 0; i < MAX_CELLS; ++i) {
			c.سېټ(i, کیڼ_3[i] || ښی_3[i]);
		}
		أرجع c;
	}
	constexpr ساكن Category meet(Category کیڼ_3, Category ښی_3) {
		Category c {};
		لكل (اندازه_t i = 0; i < MAX_CELLS; ++i) {
			c.سېټ(i, کیڼ_3[i] && ښی_3[i]);
		}
		أرجع c;
	}
	// Sketchy! Make sure to always meet with only the known universe.
	// In other words, no modus tollens allowed
	constexpr ساكن Category complement(Category arg) {
		Category c {};
		لكل (اندازه_t i = 0; i < MAX_CELLS; ++i) {
			c.سېټ(i, !arg[i]);
		}
		أرجع c;
	}
};

//خالص
ساكن constexpr پورټ_Info پورټ_info;
ساكن constexpr Categories categories;

// Legacy
namespace Compat {
	ساكن constexpr خپلکاری internals_all = Categories::meet(categories.is_معلوم, Categories::complement(categories.is_stdcell));
	ساكن constexpr خپلکاری mem_ff = Categories::join(categories.is_ff, categories.is_mem_noff);
	// old setup_internals + setup_stdcells
	ساكن constexpr خپلکاری nomem_noff = Categories::meet(categories.is_معلوم, Categories::complement(mem_ff));
	ساكن constexpr خپلکاری internals_mem_ff = Categories::meet(internals_all, mem_ff);
	// old setup_internals
	ساكن constexpr خپلکاری internals_nomem_noff = Categories::meet(internals_all, nomem_noff);
	// old setup_stdcells
	ساكن constexpr خپلکاری stdcells_nomem_noff = Categories::meet(categories.is_stdcell, nomem_noff);
	ساكن constexpr خپلکاری stdcells_mem = Categories::meet(categories.is_stdcell, categories.is_mem_noff);
	// old setup_internals_eval
	// static constexpr auto internals_eval = Categories::meet(internals_all, categories.is_evaluable);
};

namespace {
	static_assert(categories.is_evaluable(ID($and)));
	static_assert(!categories.is_ff(ID($and)));
	static_assert(Categories::join(categories.is_evaluable, categories.is_ff)(ID($and)));
	static_assert(Categories::join(categories.is_evaluable, categories.is_ff)(ID($dffsr)));
	static_assert(!Categories::join(categories.is_evaluable, categories.is_ff)(ID($anyinit)));
}

};

بنية NewCellType {
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

بنية NewCellTypes {
	StaticCellTypes::Categories::Category static_cell_types = StaticCellTypes::categories.تشه;
	dict<IdString, NewCellType> custom_cell_types {};

	NewCellTypes() {
		static_cell_types = StaticCellTypes::categories.تشه;
	}

	NewCellTypes(RTLIL::Design *design) {
		static_cell_types = StaticCellTypes::categories.تشه;
		تیاری(design);
	}
	فراغ تیاری(RTLIL::Design *design = عدم) {
		إذا (design)
			تیاری_design(design);
		static_cell_types = StaticCellTypes::categories.is_معلوم;
	}
	فراغ تیاری_design(RTLIL::Design *design) {
		لكل (خپلکاری module : design->modules())
			تیاری_module(module);
	}

	فراغ تیاری_module(RTLIL::Module *module) {
		pool<IdString> inputs, outputs;
		لكل (خپلکاری wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			إذا (wire->پورټ_ننوت)
				inputs.ورننباسه(wire->name);
			إذا (wire->پورټ_output)
				outputs.ورننباسه(wire->name);
		}
		تیاری_type(module->name, inputs, outputs);
	}

	فراغ تیاری_type(RTLIL::IdString type, ثابت pool<RTLIL::IdString> &inputs, ثابت pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false) {
		NewCellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		custom_cell_types[ct.type] = ct;
	}

	فراغ clear() {
		custom_cell_types.clear();
		static_cell_types = StaticCellTypes::categories.تشه;
	}

	bool cell_معلوم(IdString type) ثابت {
		أرجع static_cell_types(type) || custom_cell_types.شمېر(type) != 0;
	}

	bool cell_output(IdString type, IdString پورټ) ثابت
	{
		إذا (static_cell_types(type) && StaticCellTypes::پورټ_info.outputs(type).لري(پورټ)) {
			أرجع true;
		}
		خپلکاری it = custom_cell_types.پیدا_کړه(type);
		أرجع it != custom_cell_types.پای_ټکی() && it->دوهم.outputs.شمېر(پورټ) != 0;
	}

	bool cell_ننوت(IdString type, IdString پورټ) ثابت
	{
		إذا (static_cell_types(type) && StaticCellTypes::پورټ_info.inputs(type).لري(پورټ)) {
			أرجع true;
		}
		خپلکاری it = custom_cell_types.پیدا_کړه(type);
		أرجع it != custom_cell_types.پای_ټکی() && it->دوهم.inputs.شمېر(پورټ) != 0;
	}

	RTLIL::پورټ_Dir cell_پورټ_dir(RTLIL::IdString type, RTLIL::IdString پورټ) ثابت
	{
		bool is_ننوت, is_output;
		إذا (static_cell_types(type)) {
			is_ننوت = StaticCellTypes::پورټ_info.inputs(type).لري(پورټ);
			is_output = StaticCellTypes::پورټ_info.outputs(type).لري(پورټ);
		} وإلا {
			خپلکاری it = custom_cell_types.پیدا_کړه(type);
			إذا (it == custom_cell_types.پای_ټکی())
				أرجع RTLIL::PD_UNKNOWN;
			is_ننوت = it->دوهم.inputs.شمېر(پورټ);
			is_output = it->دوهم.outputs.شمېر(پورټ);
		}
		أرجع RTLIL::پورټ_Dir(is_ننوت + is_output * 2);
	}
	bool cell_evaluable(IdString type) ثابت
	{
		أرجع static_cell_types(type) && StaticCellTypes::categories.is_evaluable(type);
	}
};

خارجي NewCellTypes yosys_celltypes;

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
