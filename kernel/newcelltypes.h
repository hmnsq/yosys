#నిర్వచితం_కాకపోతే NEWCELLTYPES_H
#నిర్వచించు NEWCELLTYPES_H

#అన్తర్భావ "kernel/rtlil.h"
#అన్తర్భావ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

/**
 * This API is unstable.
 * It may change or be removed in future versions and break dependent code.
 */

namespace StaticCellTypes {

// Given by last internal cell type IdString constids.inc, compilation error if too low
constexpr పూర్ణాంకం MAX_CELLS = 300;
// Currently given by _MUX16_, compilation error if too low
constexpr పూర్ణాంకం MAX_PORTS = 20;
నిర్మాణం Cell_పట్టిక_Builder {
	నిర్మాణం పోర్టు_List {
		std::array<RTLIL::IdString, MAX_PORTS> ports{};
		కొలత_t లెక్క = 0;
		constexpr పోర్టు_List() = అప్రమేయ;
		constexpr పోర్టు_List(std::initializer_list<RTLIL::IdString> init) {
			కొరకు (స్వయంచాలక p : init) {
				ports[లెక్క++] = p;
			}
		}
		constexpr స్వయంచాలక begin() స్థిరరాశి { తిరిగి ports.begin(); }
		constexpr స్వయంచాలక చివర() స్థిరరాశి { తిరిగి ports.begin() + లెక్క; }
		constexpr bool కలిగియుంది(RTLIL::IdString పోర్టు) స్థిరరాశి {
			కొరకు (కొలత_t i = 0; i < లెక్క; i++) {
				ఒకవేళ (పోర్టు == ports[i])
					తిరిగి true;
			}

			తిరిగి false;
		}
		constexpr కొలత_t కొలత() స్థిరరాశి { తిరిగి లెక్క; }
	};
	నిర్మాణం Features {
		bool is_evaluable = false;
		bool is_combinatorial = false;
		bool is_synthesizable = false;
		bool is_stdcell = false;
		bool is_ff = false;
		bool is_mem_noff = false;
		bool is_anyinit = false;
		bool is_tristate = false;
	};
	నిర్మాణం CellInfo {
		RTLIL::IdString type;
		పోర్టు_List inputs, outputs;
		Features features;
	};
	std::array<CellInfo, MAX_CELLS> cells{};
	కొలత_t లెక్క = 0;

	constexpr శూన్య అమరిక_type(RTLIL::IdString type, std::initializer_list<RTLIL::IdString> inputs, std::initializer_list<RTLIL::IdString> outputs, స్థిరరాశి Features& features) {
		cells[లెక్క++] = {type, పోర్టు_List(inputs), పోర్టు_List(outputs), features};
	}
	constexpr శూన్య అమరిక_internals_other()
	{
		Features features {};
		features.is_tristate = true;
		అమరిక_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y}, features);

		features = {};
		అమరిక_type(ID($barrier), {ID::A}, {ID::Y}, features);

		features = {};
		అమరిక_type(ID($ప్రతిజ్ఞా), {ID::A, ID::EN}, {}, features);
		అమరిక_type(ID($assume), {ID::A, ID::EN}, {}, features);
		అమరిక_type(ID($సజీవ), {ID::A, ID::EN}, {}, features);
		అమరిక_type(ID($fair), {ID::A, ID::EN}, {}, features);
		అమరిక_type(ID($cover), {ID::A, ID::EN}, {}, features);
		అమరిక_type(ID($initstate), {}, {ID::Y}, features);
		అమరిక_type(ID($anyconst), {}, {ID::Y}, features);
		అమరిక_type(ID($anyseq), {}, {ID::Y}, features);
		అమరిక_type(ID($allconst), {}, {ID::Y}, features);
		అమరిక_type(ID($allseq), {}, {ID::Y}, features);
		అమరిక_type(ID($equiv), {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, {}, features);
		అమరిక_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, {}, features);
		అమరిక_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, {}, features);
		అమరిక_type(ID($print), {ID::EN, ID::వాదనలు, ID::TRG}, {}, features);
		అమరిక_type(ID($check), {ID::A, ID::EN, ID::వాదనలు, ID::TRG}, {}, features);
		అమరిక_type(ID($సమితి_ట్యాగ్), {ID::A, ID::సమితి_2, ID::CLR}, {ID::Y}, features);
		అమరిక_type(ID($get_ట్యాగ్), {ID::A}, {ID::Y}, features);
		అమరిక_type(ID($overwrite_ట్యాగ్), {ID::A, ID::సమితి_2, ID::CLR}, {}, features);
		అమరిక_type(ID($original_ట్యాగ్), {ID::A}, {ID::Y}, features);
		అమరిక_type(ID($future_ff), {ID::A}, {ID::Y}, features);
		అమరిక_type(ID($scopeinfo), {}, {}, features);
		అమరిక_type(ID($ఆదానం_పోర్టు), {}, {ID::Y}, features);
		అమరిక_type(ID($connect), {ID::A, ID::B}, {}, features);
	}
	constexpr శూన్య అమరిక_internals_eval()
	{
		Features features {};
		features.is_evaluable = true;
		std::initializer_list<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ముక్క), ID($lut), ID($sop)
		};

		std::initializer_list<RTLIL::IdString> ద్విమాన_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($జరుపు), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($కూడు), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ఘాతం),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		కొరకు (స్వయంచాలక type : unary_ops)
			అమరిక_type(type, {ID::A}, {ID::Y}, features);

		కొరకు (స్వయంచాలక type : ద్విమాన_ops)
			అమరిక_type(type, {ID::A, ID::B}, {ID::Y}, features);

		కొరకు (స్వయంచాలక type : {ID($mux), ID($pmux), ID($bwmux)})
			అమరిక_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, features);

		కొరకు (స్వయంచాలక type : {ID($bmux), ID($demux)})
			అమరిక_type(type, {ID::A, ID::S}, {ID::Y}, features);

		అమరిక_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, features);
		అమరిక_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, features);
		అమరిక_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		అమరిక_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, features);
	}
	constexpr శూన్య అమరిక_internals_ff()
	{
		Features features {};
		features.is_ff = true;
		అమరిక_type(ID($sr), {ID::సమితి_2, ID::CLR}, {ID::Q}, features);
		అమరిక_type(ID($ff), {ID::D}, {ID::Q}, features);
		అమరిక_type(ID($dff), {ID::CLK, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($dffsr), {ID::CLK, ID::సమితి_2, ID::CLR, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($dffsre), {ID::CLK, ID::సమితి_2, ID::CLR, ID::D, ID::EN}, {ID::Q}, features);
		అమరిక_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q}, features);
		అమరిక_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q}, features);
		అమరిక_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		అమరిక_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		అమరిక_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q}, features);
		అమరిక_type(ID($dlatchsr), {ID::EN, ID::సమితి_2, ID::CLR, ID::D}, {ID::Q}, features);
	}
	constexpr శూన్య అమరిక_internals_anyinit()
	{
		Features features {};
		features.is_anyinit = true;
		అమరిక_type(ID($anyinit), {ID::D}, {ID::Q}, features);
	}
	constexpr శూన్య అమరిక_internals_mem_noff()
	{
		Features features {};
		features.is_mem_noff = true;
		// NOT setup_internals_ff()

		అమరిక_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::దత్తాంశం_2}, features);
		అమరిక_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::దత్తాంశం_2}, features);
		అమరిక_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::దత్తాంశం_2}, {}, features);
		అమరిక_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::దత్తాంశం_2}, {}, features);
		అమరిక_type(ID($meminit), {ID::ADDR, ID::దత్తాంశం_2}, {}, features);
		అమరిక_type(ID($meminit_v2), {ID::ADDR, ID::దత్తాంశం_2, ID::EN}, {}, features);
		అమరిక_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_దత్తాంశం}, {ID::RD_దత్తాంశం}, features);
		అమరిక_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_దత్తాంశం}, {ID::RD_దత్తాంశం}, features);

		// What?
		అమరిక_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_లోపల}, {ID::CTRL_బహిర్గతం}, features);
	}
	constexpr శూన్య అమరిక_stdcells_tristate()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_tristate = true;
		అమరిక_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y}, features);
	}

	constexpr శూన్య అమరిక_stdcells_eval()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_evaluable = true;
		అమరిక_type(ID($_BUF_), {ID::A}, {ID::Y}, features);
		అమరిక_type(ID($_NOT_), {ID::A}, {ID::Y}, features);
		అమరిక_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, features);
		అమరిక_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		అమరిక_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		అమరిక_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, features);
		అమరిక_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, features);
		అమరిక_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, features);
		అమరిక_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		అమరిక_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		అమరిక_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
		అమరిక_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
	}

	constexpr శూన్య అమరిక_stdcells_ff() {
		Features features {};
		features.is_stdcell = true;
		features.is_ff = true;

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_SR_") + c1 + c2 + "_", {ID::S, ID::R}, {ID::Q}, features);
		అమరిక_type(ID($_SR_NN_), {ID::S, ID::R}, {ID::Q}, features);
		అమరిక_type(ID($_SR_NP_), {ID::S, ID::R}, {ID::Q}, features);
		అమరిక_type(ID($_SR_PN_), {ID::S, ID::R}, {ID::Q}, features);
		అమరిక_type(ID($_SR_PP_), {ID::S, ID::R}, {ID::Q}, features);

		అమరిక_type(ID($_FF_), {ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DFF_") + c1 + "_", {ID::C, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_N_), {ID::C, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_P_), {ID::C, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + "_", {ID::C, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_ALDFF_") + c1 + c2 + "_", {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFF_NN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFF_NP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFF_PN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFF_PP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_ALDFFE_") + c1 + c2 + c3 + "_", {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_NNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_NNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_NPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_NPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_PNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_PNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_PPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_ALDFFE_PPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DFFSR_") + c1 + c2 + c3 + "_", {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_NNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_NNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_NPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_NPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_PNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_PNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_PPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSR_PPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFSRE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_NPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_DFFSRE_PPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_SDFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_SDFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFCE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		అమరిక_type(ID($_SDFFCE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DLATCH_") + c1 + "_", {ID::E, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_N_), {ID::E, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_P_), {ID::E, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DLATCH_") + c1 + c2 + c3 + "_", {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_NN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_NN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_NP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_NP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_PN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_PN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_PP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCH_PP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DLATCHSR_") + c1 + c2 + c3 + "_", {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_NNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_NNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_NPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_NPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_PNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_PNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_PPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		అమరిక_type(ID($_DLATCHSR_PPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
	}
	constexpr Cell_పట్టిక_Builder() {
		అమరిక_internals_other();
		అమరిక_internals_eval();
		అమరిక_internals_ff();
		అమరిక_internals_anyinit();
		అమరిక_internals_mem_noff();
		అమరిక_stdcells_tristate();
		అమరిక_stdcells_eval();
		అమరిక_stdcells_ff();
	}

};

constexpr Cell_పట్టిక_Builder builder {};

నిర్మాణం పోర్టు_Info {
	నిర్మాణం పోర్టు_Lists {
		std::array<Cell_పట్టిక_Builder::పోర్టు_List, MAX_CELLS> దత్తాంశం{};
		constexpr Cell_పట్టిక_Builder::పోర్టు_List క్రియాకారకం()(IdString type) స్థిరరాశి {
			తిరిగి దత్తాంశం[type.raw()];
		}
		constexpr Cell_పట్టిక_Builder::పోర్టు_List& క్రియాకారకం[](కొలత_t idx) {
			తిరిగి దత్తాంశం[idx];
		}
		constexpr కొలత_t కొలత() స్థిరరాశి { తిరిగి దత్తాంశం.కొలత(); }
	};
	పోర్టు_Lists inputs {};
	పోర్టు_Lists outputs {};
	constexpr పోర్టు_Info() {
		కొరకు (కొలత_t i = 0; i < builder.లెక్క; ++i) {
			స్వయంచాలక& cell = builder.cells[i];
			కొలత_t idx = cell.type.raw();
			inputs[idx] = cell.inputs;
			outputs[idx] = cell.outputs;
		}
	}
};

నిర్మాణం Categories {
	నిర్మాణం Category {
		std::array<bool, MAX_CELLS> దత్తాంశం{};
		constexpr bool క్రియాకారకం()(IdString type) స్థిరరాశి {
			కొలత_t idx = type.raw();
			ఒకవేళ (idx >= MAX_CELLS)
				తిరిగి false;
			తిరిగి దత్తాంశం[idx];
		}
		constexpr bool క్రియాకారకం[](కొలత_t idx) {
			తిరిగి దత్తాంశం[idx];
		}
		constexpr శూన్య సమితి_id(IdString type, bool val = true) {
			కొలత_t idx = type.raw();
			ఒకవేళ (idx >= MAX_CELLS) {
				ఒకవేళ (std::is_constant_evaluated())
					throw "cell type index out of range";
				log_abort();
			}
			దత్తాంశం[idx] = val;
		}
		constexpr శూన్య సమితి(కొలత_t idx, bool val = true) {
			దత్తాంశం[idx] = val;
		}
		constexpr కొలత_t కొలత() స్థిరరాశి { తిరిగి దత్తాంశం.కొలత(); }
	};
	Category ఖాళీలు {};
	Category is_తెలిసిన {};
	Category is_evaluable {};
	Category is_combinatorial {};
	Category is_synthesizable {};
	Category is_stdcell {};
	Category is_ff {};
	Category is_mem_noff {};
	Category is_anyinit {};
	Category is_tristate {};
	constexpr Categories() {
		కొరకు (కొలత_t i = 0; i < builder.లెక్క; ++i) {
			స్వయంచాలక& cell = builder.cells[i];
			కొలత_t idx = cell.type.raw();
			is_తెలిసిన.సమితి(idx);
			is_evaluable.సమితి(idx, cell.features.is_evaluable);
			is_combinatorial.సమితి(idx, cell.features.is_combinatorial);
			is_synthesizable.సమితి(idx, cell.features.is_synthesizable);
			is_stdcell.సమితి(idx, cell.features.is_stdcell);
			is_ff.సమితి(idx, cell.features.is_ff);
			is_mem_noff.సమితి(idx, cell.features.is_mem_noff);
			is_anyinit.సమితి(idx, cell.features.is_anyinit);
			is_tristate.సమితి(idx, cell.features.is_tristate);
		}
	}
	constexpr స్థిర Category join(Category ఎడమ_3, Category కుడి_3) {
		Category c {};
		కొరకు (కొలత_t i = 0; i < MAX_CELLS; ++i) {
			c.సమితి(i, ఎడమ_3[i] || కుడి_3[i]);
		}
		తిరిగి c;
	}
	constexpr స్థిర Category meet(Category ఎడమ_3, Category కుడి_3) {
		Category c {};
		కొరకు (కొలత_t i = 0; i < MAX_CELLS; ++i) {
			c.సమితి(i, ఎడమ_3[i] && కుడి_3[i]);
		}
		తిరిగి c;
	}
	// Sketchy! Make sure to always meet with only the known universe.
	// In other words, no modus tollens allowed
	constexpr స్థిర Category complement(Category arg) {
		Category c {};
		కొరకు (కొలత_t i = 0; i < MAX_CELLS; ++i) {
			c.సమితి(i, !arg[i]);
		}
		తిరిగి c;
	}
};

//శుద్ధ
స్థిర constexpr పోర్టు_Info పోర్టు_info;
స్థిర constexpr Categories categories;

// Legacy
namespace Compat {
	స్థిర constexpr స్వయంచాలక internals_all = Categories::meet(categories.is_తెలిసిన, Categories::complement(categories.is_stdcell));
	స్థిర constexpr స్వయంచాలక mem_ff = Categories::join(categories.is_ff, categories.is_mem_noff);
	// old setup_internals + setup_stdcells
	స్థిర constexpr స్వయంచాలక nomem_noff = Categories::meet(categories.is_తెలిసిన, Categories::complement(mem_ff));
	స్థిర constexpr స్వయంచాలక internals_mem_ff = Categories::meet(internals_all, mem_ff);
	// old setup_internals
	స్థిర constexpr స్వయంచాలక internals_nomem_noff = Categories::meet(internals_all, nomem_noff);
	// old setup_stdcells
	స్థిర constexpr స్వయంచాలక stdcells_nomem_noff = Categories::meet(categories.is_stdcell, nomem_noff);
	స్థిర constexpr స్వయంచాలక stdcells_mem = Categories::meet(categories.is_stdcell, categories.is_mem_noff);
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

నిర్మాణం NewCellType {
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

నిర్మాణం NewCellTypes {
	StaticCellTypes::Categories::Category static_cell_types = StaticCellTypes::categories.ఖాళీలు;
	dict<IdString, NewCellType> custom_cell_types {};

	NewCellTypes() {
		static_cell_types = StaticCellTypes::categories.ఖాళీలు;
	}

	NewCellTypes(RTLIL::Design *design) {
		static_cell_types = StaticCellTypes::categories.ఖాళీలు;
		అమరిక(design);
	}
	శూన్య అమరిక(RTLIL::Design *design = ఖాళీ) {
		ఒకవేళ (design)
			అమరిక_design(design);
		static_cell_types = StaticCellTypes::categories.is_తెలిసిన;
	}
	శూన్య అమరిక_design(RTLIL::Design *design) {
		కొరకు (స్వయంచాలక module : design->modules())
			అమరిక_module(module);
	}

	శూన్య అమరిక_module(RTLIL::Module *module) {
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

	శూన్య అమరిక_type(RTLIL::IdString type, స్థిరరాశి pool<RTLIL::IdString> &inputs, స్థిరరాశి pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false) {
		NewCellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		custom_cell_types[ct.type] = ct;
	}

	శూన్య clear() {
		custom_cell_types.clear();
		static_cell_types = StaticCellTypes::categories.ఖాళీలు;
	}

	bool cell_తెలిసిన(IdString type) స్థిరరాశి {
		తిరిగి static_cell_types(type) || custom_cell_types.లెక్క(type) != 0;
	}

	bool cell_output(IdString type, IdString పోర్టు) స్థిరరాశి
	{
		ఒకవేళ (static_cell_types(type) && StaticCellTypes::పోర్టు_info.outputs(type).కలిగియుంది(పోర్టు)) {
			తిరిగి true;
		}
		స్వయంచాలక it = custom_cell_types.కనుగొను(type);
		తిరిగి it != custom_cell_types.చివర() && it->రెండవ.outputs.లెక్క(పోర్టు) != 0;
	}

	bool cell_ఆదానం(IdString type, IdString పోర్టు) స్థిరరాశి
	{
		ఒకవేళ (static_cell_types(type) && StaticCellTypes::పోర్టు_info.inputs(type).కలిగియుంది(పోర్టు)) {
			తిరిగి true;
		}
		స్వయంచాలక it = custom_cell_types.కనుగొను(type);
		తిరిగి it != custom_cell_types.చివర() && it->రెండవ.inputs.లెక్క(పోర్టు) != 0;
	}

	RTLIL::పోర్టు_Dir cell_పోర్టు_dir(RTLIL::IdString type, RTLIL::IdString పోర్టు) స్థిరరాశి
	{
		bool is_ఆదానం, is_output;
		ఒకవేళ (static_cell_types(type)) {
			is_ఆదానం = StaticCellTypes::పోర్టు_info.inputs(type).కలిగియుంది(పోర్టు);
			is_output = StaticCellTypes::పోర్టు_info.outputs(type).కలిగియుంది(పోర్టు);
		} లేకపోతే {
			స్వయంచాలక it = custom_cell_types.కనుగొను(type);
			ఒకవేళ (it == custom_cell_types.చివర())
				తిరిగి RTLIL::PD_UNKNOWN;
			is_ఆదానం = it->రెండవ.inputs.లెక్క(పోర్టు);
			is_output = it->రెండవ.outputs.లెక్క(పోర్టు);
		}
		తిరిగి RTLIL::పోర్టు_Dir(is_ఆదానం + is_output * 2);
	}
	bool cell_evaluable(IdString type) స్థిరరాశి
	{
		తిరిగి static_cell_types(type) && StaticCellTypes::categories.is_evaluable(type);
	}
};

బాహ్య NewCellTypes yosys_celltypes;

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
