#اگر_تعریف_نشده NEWCELLTYPES_H
#تعریف_کن NEWCELLTYPES_H

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
	بنية پورت_List {
		std::array<RTLIL::IdString, MAX_PORTS> ports{};
		اندازه_t شمار = 0;
		constexpr پورت_List() = افتراضي;
		constexpr پورت_List(std::initializer_list<RTLIL::IdString> init) {
			لكل (خودکار p : init) {
				ports[شمار++] = p;
			}
		}
		constexpr خودکار begin() ثابت { أرجع ports.begin(); }
		constexpr خودکار انتها() ثابت { أرجع ports.begin() + شمار; }
		constexpr bool شامل_است(RTLIL::IdString پورت) ثابت {
			لكل (اندازه_t i = 0; i < شمار; i++) {
				إذا (پورت == ports[i])
					أرجع true;
			}

			أرجع false;
		}
		constexpr اندازه_t اندازه() ثابت { أرجع شمار; }
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
		پورت_List inputs, outputs;
		Features features;
	};
	std::array<CellInfo, MAX_CELLS> cells{};
	اندازه_t شمار = 0;

	constexpr فراغ راهاندازی_type(RTLIL::IdString type, std::initializer_list<RTLIL::IdString> inputs, std::initializer_list<RTLIL::IdString> outputs, ثابت Features& features) {
		cells[شمار++] = {type, پورت_List(inputs), پورت_List(outputs), features};
	}
	constexpr فراغ راهاندازی_internals_other()
	{
		Features features {};
		features.is_tristate = true;
		راهاندازی_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y}, features);

		features = {};
		راهاندازی_type(ID($barrier), {ID::A}, {ID::Y}, features);

		features = {};
		راهاندازی_type(ID($أكد), {ID::A, ID::EN}, {}, features);
		راهاندازی_type(ID($assume), {ID::A, ID::EN}, {}, features);
		راهاندازی_type(ID($زنده), {ID::A, ID::EN}, {}, features);
		راهاندازی_type(ID($fair), {ID::A, ID::EN}, {}, features);
		راهاندازی_type(ID($cover), {ID::A, ID::EN}, {}, features);
		راهاندازی_type(ID($initstate), {}, {ID::Y}, features);
		راهاندازی_type(ID($anyconst), {}, {ID::Y}, features);
		راهاندازی_type(ID($anyseq), {}, {ID::Y}, features);
		راهاندازی_type(ID($allconst), {}, {ID::Y}, features);
		راهاندازی_type(ID($allseq), {}, {ID::Y}, features);
		راهاندازی_type(ID($equiv), {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, {}, features);
		راهاندازی_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, {}, features);
		راهاندازی_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, {}, features);
		راهاندازی_type(ID($print), {ID::EN, ID::آرگومانها, ID::TRG}, {}, features);
		راهاندازی_type(ID($check), {ID::A, ID::EN, ID::آرگومانها, ID::TRG}, {}, features);
		راهاندازی_type(ID($مجموعه_برچسب), {ID::A, ID::مجموعه_2, ID::CLR}, {ID::Y}, features);
		راهاندازی_type(ID($get_برچسب), {ID::A}, {ID::Y}, features);
		راهاندازی_type(ID($overwrite_برچسب), {ID::A, ID::مجموعه_2, ID::CLR}, {}, features);
		راهاندازی_type(ID($original_برچسب), {ID::A}, {ID::Y}, features);
		راهاندازی_type(ID($future_ff), {ID::A}, {ID::Y}, features);
		راهاندازی_type(ID($scopeinfo), {}, {}, features);
		راهاندازی_type(ID($درون_داد_پورت), {}, {ID::Y}, features);
		راهاندازی_type(ID($connect), {ID::A, ID::B}, {}, features);
	}
	constexpr فراغ راهاندازی_internals_eval()
	{
		Features features {};
		features.is_evaluable = true;
		std::initializer_list<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($برش), ID($lut), ID($sop)
		};

		std::initializer_list<RTLIL::IdString> دودویی_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($لغزش), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($جمع_کن), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		لكل (خودکار type : unary_ops)
			راهاندازی_type(type, {ID::A}, {ID::Y}, features);

		لكل (خودکار type : دودویی_ops)
			راهاندازی_type(type, {ID::A, ID::B}, {ID::Y}, features);

		لكل (خودکار type : {ID($mux), ID($pmux), ID($bwmux)})
			راهاندازی_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, features);

		لكل (خودکار type : {ID($bmux), ID($demux)})
			راهاندازی_type(type, {ID::A, ID::S}, {ID::Y}, features);

		راهاندازی_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, features);
		راهاندازی_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, features);
		راهاندازی_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		راهاندازی_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, features);
	}
	constexpr فراغ راهاندازی_internals_ff()
	{
		Features features {};
		features.is_ff = true;
		راهاندازی_type(ID($sr), {ID::مجموعه_2, ID::CLR}, {ID::Q}, features);
		راهاندازی_type(ID($ff), {ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($dff), {ID::CLK, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($dffsr), {ID::CLK, ID::مجموعه_2, ID::CLR, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($dffsre), {ID::CLK, ID::مجموعه_2, ID::CLR, ID::D, ID::EN}, {ID::Q}, features);
		راهاندازی_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q}, features);
		راهاندازی_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q}, features);
		راهاندازی_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		راهاندازی_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		راهاندازی_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q}, features);
		راهاندازی_type(ID($dlatchsr), {ID::EN, ID::مجموعه_2, ID::CLR, ID::D}, {ID::Q}, features);
	}
	constexpr فراغ راهاندازی_internals_anyinit()
	{
		Features features {};
		features.is_anyinit = true;
		راهاندازی_type(ID($anyinit), {ID::D}, {ID::Q}, features);
	}
	constexpr فراغ راهاندازی_internals_mem_noff()
	{
		Features features {};
		features.is_mem_noff = true;
		// NOT setup_internals_ff()

		راهاندازی_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::داده_2}, features);
		راهاندازی_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::داده_2}, features);
		راهاندازی_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::داده_2}, {}, features);
		راهاندازی_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::داده_2}, {}, features);
		راهاندازی_type(ID($meminit), {ID::ADDR, ID::داده_2}, {}, features);
		راهاندازی_type(ID($meminit_v2), {ID::ADDR, ID::داده_2, ID::EN}, {}, features);
		راهاندازی_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_داده}, {ID::RD_داده}, features);
		راهاندازی_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_داده}, {ID::RD_داده}, features);

		// What?
		راهاندازی_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_درون}, {ID::CTRL_بیرون}, features);
	}
	constexpr فراغ راهاندازی_stdcells_tristate()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_tristate = true;
		راهاندازی_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y}, features);
	}

	constexpr فراغ راهاندازی_stdcells_eval()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_evaluable = true;
		راهاندازی_type(ID($_BUF_), {ID::A}, {ID::Y}, features);
		راهاندازی_type(ID($_NOT_), {ID::A}, {ID::Y}, features);
		راهاندازی_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, features);
		راهاندازی_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		راهاندازی_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		راهاندازی_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, features);
		راهاندازی_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, features);
		راهاندازی_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, features);
		راهاندازی_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		راهاندازی_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		راهاندازی_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
		راهاندازی_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
	}

	constexpr فراغ راهاندازی_stdcells_ff() {
		Features features {};
		features.is_stdcell = true;
		features.is_ff = true;

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_SR_") + c1 + c2 + "_", {ID::S, ID::R}, {ID::Q}, features);
		راهاندازی_type(ID($_SR_NN_), {ID::S, ID::R}, {ID::Q}, features);
		راهاندازی_type(ID($_SR_NP_), {ID::S, ID::R}, {ID::Q}, features);
		راهاندازی_type(ID($_SR_PN_), {ID::S, ID::R}, {ID::Q}, features);
		راهاندازی_type(ID($_SR_PP_), {ID::S, ID::R}, {ID::Q}, features);

		راهاندازی_type(ID($_FF_), {ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DFF_") + c1 + "_", {ID::C, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_N_), {ID::C, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_P_), {ID::C, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + "_", {ID::C, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_ALDFF_") + c1 + c2 + "_", {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFF_NN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFF_NP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFF_PN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFF_PP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_ALDFFE_") + c1 + c2 + c3 + "_", {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_NNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_NNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_NPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_NPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_PNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_PNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_PPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_ALDFFE_PPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DFFSR_") + c1 + c2 + c3 + "_", {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_NNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_NNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_NPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_NPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_PNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_PNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_PPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSR_PPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFSRE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_NPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_DFFSRE_PPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_SDFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFCE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		راهاندازی_type(ID($_SDFFCE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DLATCH_") + c1 + "_", {ID::E, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_N_), {ID::E, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_P_), {ID::E, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DLATCH_") + c1 + c2 + c3 + "_", {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_NN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_NN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_NP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_NP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_PN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_PN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_PP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCH_PP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DLATCHSR_") + c1 + c2 + c3 + "_", {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_NNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_NNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_NPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_NPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_PNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_PNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_PPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		راهاندازی_type(ID($_DLATCHSR_PPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
	}
	constexpr Cell_جدول_Builder() {
		راهاندازی_internals_other();
		راهاندازی_internals_eval();
		راهاندازی_internals_ff();
		راهاندازی_internals_anyinit();
		راهاندازی_internals_mem_noff();
		راهاندازی_stdcells_tristate();
		راهاندازی_stdcells_eval();
		راهاندازی_stdcells_ff();
	}

};

constexpr Cell_جدول_Builder builder {};

بنية پورت_Info {
	بنية پورت_Lists {
		std::array<Cell_جدول_Builder::پورت_List, MAX_CELLS> داده{};
		constexpr Cell_جدول_Builder::پورت_List عملگر()(IdString type) ثابت {
			أرجع داده[type.raw()];
		}
		constexpr Cell_جدول_Builder::پورت_List& عملگر[](اندازه_t idx) {
			أرجع داده[idx];
		}
		constexpr اندازه_t اندازه() ثابت { أرجع داده.اندازه(); }
	};
	پورت_Lists inputs {};
	پورت_Lists outputs {};
	constexpr پورت_Info() {
		لكل (اندازه_t i = 0; i < builder.شمار; ++i) {
			خودکار& cell = builder.cells[i];
			اندازه_t idx = cell.type.raw();
			inputs[idx] = cell.inputs;
			outputs[idx] = cell.outputs;
		}
	}
};

بنية Categories {
	بنية Category {
		std::array<bool, MAX_CELLS> داده{};
		constexpr bool عملگر()(IdString type) ثابت {
			اندازه_t idx = type.raw();
			إذا (idx >= MAX_CELLS)
				أرجع false;
			أرجع داده[idx];
		}
		constexpr bool عملگر[](اندازه_t idx) {
			أرجع داده[idx];
		}
		constexpr فراغ مجموعه_id(IdString type, bool val = true) {
			اندازه_t idx = type.raw();
			إذا (idx >= MAX_CELLS) {
				إذا (std::is_constant_evaluated())
					throw "cell type index out of range";
				log_abort();
			}
			داده[idx] = val;
		}
		constexpr فراغ مجموعه(اندازه_t idx, bool val = true) {
			داده[idx] = val;
		}
		constexpr اندازه_t اندازه() ثابت { أرجع داده.اندازه(); }
	};
	Category خالی {};
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
		لكل (اندازه_t i = 0; i < builder.شمار; ++i) {
			خودکار& cell = builder.cells[i];
			اندازه_t idx = cell.type.raw();
			is_معلوم.مجموعه(idx);
			is_evaluable.مجموعه(idx, cell.features.is_evaluable);
			is_combinatorial.مجموعه(idx, cell.features.is_combinatorial);
			is_synthesizable.مجموعه(idx, cell.features.is_synthesizable);
			is_stdcell.مجموعه(idx, cell.features.is_stdcell);
			is_ff.مجموعه(idx, cell.features.is_ff);
			is_mem_noff.مجموعه(idx, cell.features.is_mem_noff);
			is_anyinit.مجموعه(idx, cell.features.is_anyinit);
			is_tristate.مجموعه(idx, cell.features.is_tristate);
		}
	}
	constexpr ساكن Category join(Category چپ_3, Category راست_3) {
		Category c {};
		لكل (اندازه_t i = 0; i < MAX_CELLS; ++i) {
			c.مجموعه(i, چپ_3[i] || راست_3[i]);
		}
		أرجع c;
	}
	constexpr ساكن Category meet(Category چپ_3, Category راست_3) {
		Category c {};
		لكل (اندازه_t i = 0; i < MAX_CELLS; ++i) {
			c.مجموعه(i, چپ_3[i] && راست_3[i]);
		}
		أرجع c;
	}
	// Sketchy! Make sure to always meet with only the known universe.
	// In other words, no modus tollens allowed
	constexpr ساكن Category complement(Category arg) {
		Category c {};
		لكل (اندازه_t i = 0; i < MAX_CELLS; ++i) {
			c.مجموعه(i, !arg[i]);
		}
		أرجع c;
	}
};

//خالص
ساكن constexpr پورت_Info پورت_info;
ساكن constexpr Categories categories;

// Legacy
namespace Compat {
	ساكن constexpr خودکار internals_all = Categories::meet(categories.is_معلوم, Categories::complement(categories.is_stdcell));
	ساكن constexpr خودکار mem_ff = Categories::join(categories.is_ff, categories.is_mem_noff);
	// old setup_internals + setup_stdcells
	ساكن constexpr خودکار nomem_noff = Categories::meet(categories.is_معلوم, Categories::complement(mem_ff));
	ساكن constexpr خودکار internals_mem_ff = Categories::meet(internals_all, mem_ff);
	// old setup_internals
	ساكن constexpr خودکار internals_nomem_noff = Categories::meet(internals_all, nomem_noff);
	// old setup_stdcells
	ساكن constexpr خودکار stdcells_nomem_noff = Categories::meet(categories.is_stdcell, nomem_noff);
	ساكن constexpr خودکار stdcells_mem = Categories::meet(categories.is_stdcell, categories.is_mem_noff);
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
	StaticCellTypes::Categories::Category static_cell_types = StaticCellTypes::categories.خالی;
	dict<IdString, NewCellType> custom_cell_types {};

	NewCellTypes() {
		static_cell_types = StaticCellTypes::categories.خالی;
	}

	NewCellTypes(RTLIL::Design *design) {
		static_cell_types = StaticCellTypes::categories.خالی;
		راهاندازی(design);
	}
	فراغ راهاندازی(RTLIL::Design *design = عدم) {
		إذا (design)
			راهاندازی_design(design);
		static_cell_types = StaticCellTypes::categories.is_معلوم;
	}
	فراغ راهاندازی_design(RTLIL::Design *design) {
		لكل (خودکار module : design->modules())
			راهاندازی_module(module);
	}

	فراغ راهاندازی_module(RTLIL::Module *module) {
		pool<IdString> inputs, outputs;
		لكل (خودکار wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			إذا (wire->پورت_درون_داد)
				inputs.درج(wire->name);
			إذا (wire->پورت_output)
				outputs.درج(wire->name);
		}
		راهاندازی_type(module->name, inputs, outputs);
	}

	فراغ راهاندازی_type(RTLIL::IdString type, ثابت pool<RTLIL::IdString> &inputs, ثابت pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false) {
		NewCellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		custom_cell_types[ct.type] = ct;
	}

	فراغ clear() {
		custom_cell_types.clear();
		static_cell_types = StaticCellTypes::categories.خالی;
	}

	bool cell_معلوم(IdString type) ثابت {
		أرجع static_cell_types(type) || custom_cell_types.شمار(type) != 0;
	}

	bool cell_output(IdString type, IdString پورت) ثابت
	{
		إذا (static_cell_types(type) && StaticCellTypes::پورت_info.outputs(type).شامل_است(پورت)) {
			أرجع true;
		}
		خودکار it = custom_cell_types.پیدا_کن(type);
		أرجع it != custom_cell_types.انتها() && it->دومی.outputs.شمار(پورت) != 0;
	}

	bool cell_درون_داد(IdString type, IdString پورت) ثابت
	{
		إذا (static_cell_types(type) && StaticCellTypes::پورت_info.inputs(type).شامل_است(پورت)) {
			أرجع true;
		}
		خودکار it = custom_cell_types.پیدا_کن(type);
		أرجع it != custom_cell_types.انتها() && it->دومی.inputs.شمار(پورت) != 0;
	}

	RTLIL::پورت_Dir cell_پورت_dir(RTLIL::IdString type, RTLIL::IdString پورت) ثابت
	{
		bool is_درون_داد, is_output;
		إذا (static_cell_types(type)) {
			is_درون_داد = StaticCellTypes::پورت_info.inputs(type).شامل_است(پورت);
			is_output = StaticCellTypes::پورت_info.outputs(type).شامل_است(پورت);
		} وإلا {
			خودکار it = custom_cell_types.پیدا_کن(type);
			إذا (it == custom_cell_types.انتها())
				أرجع RTLIL::PD_UNKNOWN;
			is_درون_داد = it->دومی.inputs.شمار(پورت);
			is_output = it->دومی.outputs.شمار(پورت);
		}
		أرجع RTLIL::پورت_Dir(is_درون_داد + is_output * 2);
	}
	bool cell_evaluable(IdString type) ثابت
	{
		أرجع static_cell_types(type) && StaticCellTypes::categories.is_evaluable(type);
	}
};

خارجي NewCellTypes yosys_celltypes;

YOSYS_NAMESPACE_انتها

#پایان_اگر
