#यदि_अपरिभाषितम् NEWCELLTYPES_H
#परिभाषय NEWCELLTYPES_H

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

/**
 * This API is unstable.
 * It may change or be removed in future versions and break dependent code.
 */

namespace StaticCellTypes {

// Given by last internal cell type IdString constids.inc, compilation error if too low
constexpr पूर्णाङ्क MAX_CELLS = 300;
// Currently given by _MUX16_, compilation error if too low
constexpr पूर्णाङ्क MAX_PORTS = 20;
संरचना Cell_सारणी_Builder {
	संरचना पत्तनम्_List {
		std::array<RTLIL::IdString, MAX_PORTS> ports{};
		आकार_t गणनम् = 0;
		constexpr पत्तनम्_List() = स्वभाव;
		constexpr पत्तनम्_List(std::initializer_list<RTLIL::IdString> init) {
			प्रति (स्वचालित p : init) {
				ports[गणनम्++] = p;
			}
		}
		constexpr स्वचालित begin() ध्रुव { प्रत्यागम ports.begin(); }
		constexpr स्वचालित अन्तः() ध्रुव { प्रत्यागम ports.begin() + गणनम्; }
		constexpr bool अन्तर्भवति(RTLIL::IdString पत्तनम्) ध्रुव {
			प्रति (आकार_t i = 0; i < गणनम्; i++) {
				यदि (पत्तनम् == ports[i])
					प्रत्यागम true;
			}

			प्रत्यागम false;
		}
		constexpr आकार_t आकार() ध्रुव { प्रत्यागम गणनम्; }
	};
	संरचना Features {
		bool is_evaluable = false;
		bool is_combinatorial = false;
		bool is_synthesizable = false;
		bool is_stdcell = false;
		bool is_ff = false;
		bool is_mem_noff = false;
		bool is_anyinit = false;
		bool is_tristate = false;
	};
	संरचना CellInfo {
		RTLIL::IdString type;
		पत्तनम्_List inputs, outputs;
		Features features;
	};
	std::array<CellInfo, MAX_CELLS> cells{};
	आकार_t गणनम् = 0;

	constexpr शून्य सज्जता_type(RTLIL::IdString type, std::initializer_list<RTLIL::IdString> inputs, std::initializer_list<RTLIL::IdString> outputs, ध्रुव Features& features) {
		cells[गणनम्++] = {type, पत्तनम्_List(inputs), पत्तनम्_List(outputs), features};
	}
	constexpr शून्य सज्जता_internals_other()
	{
		Features features {};
		features.is_tristate = true;
		सज्जता_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y}, features);

		features = {};
		सज्जता_type(ID($barrier), {ID::A}, {ID::Y}, features);

		features = {};
		सज्जता_type(ID($प्रतिज्ञा), {ID::A, ID::EN}, {}, features);
		सज्जता_type(ID($assume), {ID::A, ID::EN}, {}, features);
		सज्जता_type(ID($जीवितम्), {ID::A, ID::EN}, {}, features);
		सज्जता_type(ID($fair), {ID::A, ID::EN}, {}, features);
		सज्जता_type(ID($cover), {ID::A, ID::EN}, {}, features);
		सज्जता_type(ID($initstate), {}, {ID::Y}, features);
		सज्जता_type(ID($anyconst), {}, {ID::Y}, features);
		सज्जता_type(ID($anyseq), {}, {ID::Y}, features);
		सज्जता_type(ID($allconst), {}, {ID::Y}, features);
		सज्जता_type(ID($allseq), {}, {ID::Y}, features);
		सज्जता_type(ID($equiv), {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, {}, features);
		सज्जता_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, {}, features);
		सज्जता_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, {}, features);
		सज्जता_type(ID($print), {ID::EN, ID::तर्काः, ID::TRG}, {}, features);
		सज्जता_type(ID($check), {ID::A, ID::EN, ID::तर्काः, ID::TRG}, {}, features);
		सज्जता_type(ID($समुच्चयः_चिह्नकम्), {ID::A, ID::समुच्चयः_2, ID::CLR}, {ID::Y}, features);
		सज्जता_type(ID($get_चिह्नकम्), {ID::A}, {ID::Y}, features);
		सज्जता_type(ID($overwrite_चिह्नकम्), {ID::A, ID::समुच्चयः_2, ID::CLR}, {}, features);
		सज्जता_type(ID($original_चिह्नकम्), {ID::A}, {ID::Y}, features);
		सज्जता_type(ID($future_ff), {ID::A}, {ID::Y}, features);
		सज्जता_type(ID($scopeinfo), {}, {}, features);
		सज्जता_type(ID($निवेशः_पत्तनम्), {}, {ID::Y}, features);
		सज्जता_type(ID($connect), {ID::A, ID::B}, {}, features);
	}
	constexpr शून्य सज्जता_internals_eval()
	{
		Features features {};
		features.is_evaluable = true;
		std::initializer_list<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($खण्डिका), ID($lut), ID($sop)
		};

		std::initializer_list<RTLIL::IdString> द्विमान_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($सरणम्), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($सङ्कलय), ID($sub), ID($mul), ID($भाजय), ID($mod), ID($divfloor), ID($modfloor), ID($घातः),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		प्रति (स्वचालित type : unary_ops)
			सज्जता_type(type, {ID::A}, {ID::Y}, features);

		प्रति (स्वचालित type : द्विमान_ops)
			सज्जता_type(type, {ID::A, ID::B}, {ID::Y}, features);

		प्रति (स्वचालित type : {ID($mux), ID($pmux), ID($bwmux)})
			सज्जता_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, features);

		प्रति (स्वचालित type : {ID($bmux), ID($demux)})
			सज्जता_type(type, {ID::A, ID::S}, {ID::Y}, features);

		सज्जता_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, features);
		सज्जता_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, features);
		सज्जता_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		सज्जता_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, features);
	}
	constexpr शून्य सज्जता_internals_ff()
	{
		Features features {};
		features.is_ff = true;
		सज्जता_type(ID($sr), {ID::समुच्चयः_2, ID::CLR}, {ID::Q}, features);
		सज्जता_type(ID($ff), {ID::D}, {ID::Q}, features);
		सज्जता_type(ID($dff), {ID::CLK, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($dffsr), {ID::CLK, ID::समुच्चयः_2, ID::CLR, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($dffsre), {ID::CLK, ID::समुच्चयः_2, ID::CLR, ID::D, ID::EN}, {ID::Q}, features);
		सज्जता_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q}, features);
		सज्जता_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q}, features);
		सज्जता_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		सज्जता_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		सज्जता_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q}, features);
		सज्जता_type(ID($dlatchsr), {ID::EN, ID::समुच्चयः_2, ID::CLR, ID::D}, {ID::Q}, features);
	}
	constexpr शून्य सज्जता_internals_anyinit()
	{
		Features features {};
		features.is_anyinit = true;
		सज्जता_type(ID($anyinit), {ID::D}, {ID::Q}, features);
	}
	constexpr शून्य सज्जता_internals_mem_noff()
	{
		Features features {};
		features.is_mem_noff = true;
		// NOT setup_internals_ff()

		सज्जता_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::दत्त_2}, features);
		सज्जता_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::दत्त_2}, features);
		सज्जता_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::दत्त_2}, {}, features);
		सज्जता_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::दत्त_2}, {}, features);
		सज्जता_type(ID($meminit), {ID::ADDR, ID::दत्त_2}, {}, features);
		सज्जता_type(ID($meminit_v2), {ID::ADDR, ID::दत्त_2, ID::EN}, {}, features);
		सज्जता_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_दत्त}, {ID::RD_दत्त}, features);
		सज्जता_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_दत्त}, {ID::RD_दत्त}, features);

		// What?
		सज्जता_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_अन्तर्}, {ID::CTRL_बहिर्गमनम्}, features);
	}
	constexpr शून्य सज्जता_stdcells_tristate()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_tristate = true;
		सज्जता_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y}, features);
	}

	constexpr शून्य सज्जता_stdcells_eval()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_evaluable = true;
		सज्जता_type(ID($_BUF_), {ID::A}, {ID::Y}, features);
		सज्जता_type(ID($_NOT_), {ID::A}, {ID::Y}, features);
		सज्जता_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, features);
		सज्जता_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		सज्जता_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		सज्जता_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, features);
		सज्जता_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, features);
		सज्जता_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, features);
		सज्जता_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		सज्जता_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		सज्जता_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
		सज्जता_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
	}

	constexpr शून्य सज्जता_stdcells_ff() {
		Features features {};
		features.is_stdcell = true;
		features.is_ff = true;

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_SR_") + c1 + c2 + "_", {ID::S, ID::R}, {ID::Q}, features);
		सज्जता_type(ID($_SR_NN_), {ID::S, ID::R}, {ID::Q}, features);
		सज्जता_type(ID($_SR_NP_), {ID::S, ID::R}, {ID::Q}, features);
		सज्जता_type(ID($_SR_PN_), {ID::S, ID::R}, {ID::Q}, features);
		सज्जता_type(ID($_SR_PP_), {ID::S, ID::R}, {ID::Q}, features);

		सज्जता_type(ID($_FF_), {ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DFF_") + c1 + "_", {ID::C, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_N_), {ID::C, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_P_), {ID::C, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + "_", {ID::C, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_ALDFF_") + c1 + c2 + "_", {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFF_NN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFF_NP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFF_PN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFF_PP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_ALDFFE_") + c1 + c2 + c3 + "_", {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_NNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_NNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_NPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_NPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_PNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_PNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_PPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_ALDFFE_PPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DFFSR_") + c1 + c2 + c3 + "_", {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_NNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_NNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_NPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_NPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_PNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_PNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_PPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSR_PPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFSRE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_NPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_DFFSRE_PPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_SDFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_SDFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFCE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		सज्जता_type(ID($_SDFFCE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DLATCH_") + c1 + "_", {ID::E, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_N_), {ID::E, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_P_), {ID::E, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DLATCH_") + c1 + c2 + c3 + "_", {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_NN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_NN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_NP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_NP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_PN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_PN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_PP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCH_PP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DLATCHSR_") + c1 + c2 + c3 + "_", {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_NNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_NNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_NPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_NPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_PNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_PNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_PPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		सज्जता_type(ID($_DLATCHSR_PPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
	}
	constexpr Cell_सारणी_Builder() {
		सज्जता_internals_other();
		सज्जता_internals_eval();
		सज्जता_internals_ff();
		सज्जता_internals_anyinit();
		सज्जता_internals_mem_noff();
		सज्जता_stdcells_tristate();
		सज्जता_stdcells_eval();
		सज्जता_stdcells_ff();
	}

};

constexpr Cell_सारणी_Builder builder {};

संरचना पत्तनम्_Info {
	संरचना पत्तनम्_Lists {
		std::array<Cell_सारणी_Builder::पत्तनम्_List, MAX_CELLS> दत्त{};
		constexpr Cell_सारणी_Builder::पत्तनम्_List संक्रियकः()(IdString type) ध्रुव {
			प्रत्यागम दत्त[type.raw()];
		}
		constexpr Cell_सारणी_Builder::पत्तनम्_List& संक्रियकः[](आकार_t idx) {
			प्रत्यागम दत्त[idx];
		}
		constexpr आकार_t आकार() ध्रुव { प्रत्यागम दत्त.आकार(); }
	};
	पत्तनम्_Lists inputs {};
	पत्तनम्_Lists outputs {};
	constexpr पत्तनम्_Info() {
		प्रति (आकार_t i = 0; i < builder.गणनम्; ++i) {
			स्वचालित& cell = builder.cells[i];
			आकार_t idx = cell.type.raw();
			inputs[idx] = cell.inputs;
			outputs[idx] = cell.outputs;
		}
	}
};

संरचना Categories {
	संरचना Category {
		std::array<bool, MAX_CELLS> दत्त{};
		constexpr bool संक्रियकः()(IdString type) ध्रुव {
			आकार_t idx = type.raw();
			यदि (idx >= MAX_CELLS)
				प्रत्यागम false;
			प्रत्यागम दत्त[idx];
		}
		constexpr bool संक्रियकः[](आकार_t idx) {
			प्रत्यागम दत्त[idx];
		}
		constexpr शून्य समुच्चयः_id(IdString type, bool val = true) {
			आकार_t idx = type.raw();
			यदि (idx >= MAX_CELLS) {
				यदि (std::is_constant_evaluated())
					throw "cell type index out of range";
				log_abort();
			}
			दत्त[idx] = val;
		}
		constexpr शून्य समुच्चयः(आकार_t idx, bool val = true) {
			दत्त[idx] = val;
		}
		constexpr आकार_t आकार() ध्रुव { प्रत्यागम दत्त.आकार(); }
	};
	Category रिक्तानि {};
	Category is_ज्ञातम् {};
	Category is_evaluable {};
	Category is_combinatorial {};
	Category is_synthesizable {};
	Category is_stdcell {};
	Category is_ff {};
	Category is_mem_noff {};
	Category is_anyinit {};
	Category is_tristate {};
	constexpr Categories() {
		प्रति (आकार_t i = 0; i < builder.गणनम्; ++i) {
			स्वचालित& cell = builder.cells[i];
			आकार_t idx = cell.type.raw();
			is_ज्ञातम्.समुच्चयः(idx);
			is_evaluable.समुच्चयः(idx, cell.features.is_evaluable);
			is_combinatorial.समुच्चयः(idx, cell.features.is_combinatorial);
			is_synthesizable.समुच्चयः(idx, cell.features.is_synthesizable);
			is_stdcell.समुच्चयः(idx, cell.features.is_stdcell);
			is_ff.समुच्चयः(idx, cell.features.is_ff);
			is_mem_noff.समुच्चयः(idx, cell.features.is_mem_noff);
			is_anyinit.समुच्चयः(idx, cell.features.is_anyinit);
			is_tristate.समुच्चयः(idx, cell.features.is_tristate);
		}
	}
	constexpr स्थिर Category join(Category वामः_2, Category दक्षिणः_2) {
		Category c {};
		प्रति (आकार_t i = 0; i < MAX_CELLS; ++i) {
			c.समुच्चयः(i, वामः_2[i] || दक्षिणः_2[i]);
		}
		प्रत्यागम c;
	}
	constexpr स्थिर Category meet(Category वामः_2, Category दक्षिणः_2) {
		Category c {};
		प्रति (आकार_t i = 0; i < MAX_CELLS; ++i) {
			c.समुच्चयः(i, वामः_2[i] && दक्षिणः_2[i]);
		}
		प्रत्यागम c;
	}
	// Sketchy! Make sure to always meet with only the known universe.
	// In other words, no modus tollens allowed
	constexpr स्थिर Category complement(Category arg) {
		Category c {};
		प्रति (आकार_t i = 0; i < MAX_CELLS; ++i) {
			c.समुच्चयः(i, !arg[i]);
		}
		प्रत्यागम c;
	}
};

//विशुद्धम्
स्थिर constexpr पत्तनम्_Info पत्तनम्_info;
स्थिर constexpr Categories categories;

// Legacy
namespace Compat {
	स्थिर constexpr स्वचालित internals_all = Categories::meet(categories.is_ज्ञातम्, Categories::complement(categories.is_stdcell));
	स्थिर constexpr स्वचालित mem_ff = Categories::join(categories.is_ff, categories.is_mem_noff);
	// old setup_internals + setup_stdcells
	स्थिर constexpr स्वचालित nomem_noff = Categories::meet(categories.is_ज्ञातम्, Categories::complement(mem_ff));
	स्थिर constexpr स्वचालित internals_mem_ff = Categories::meet(internals_all, mem_ff);
	// old setup_internals
	स्थिर constexpr स्वचालित internals_nomem_noff = Categories::meet(internals_all, nomem_noff);
	// old setup_stdcells
	स्थिर constexpr स्वचालित stdcells_nomem_noff = Categories::meet(categories.is_stdcell, nomem_noff);
	स्थिर constexpr स्वचालित stdcells_mem = Categories::meet(categories.is_stdcell, categories.is_mem_noff);
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

संरचना NewCellType {
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

संरचना NewCellTypes {
	StaticCellTypes::Categories::Category static_cell_types = StaticCellTypes::categories.रिक्तानि;
	dict<IdString, NewCellType> custom_cell_types {};

	NewCellTypes() {
		static_cell_types = StaticCellTypes::categories.रिक्तानि;
	}

	NewCellTypes(RTLIL::Design *design) {
		static_cell_types = StaticCellTypes::categories.रिक्तानि;
		सज्जता(design);
	}
	शून्य सज्जता(RTLIL::Design *design = रिक्त) {
		यदि (design)
			सज्जता_design(design);
		static_cell_types = StaticCellTypes::categories.is_ज्ञातम्;
	}
	शून्य सज्जता_design(RTLIL::Design *design) {
		प्रति (स्वचालित module : design->modules())
			सज्जता_module(module);
	}

	शून्य सज्जता_module(RTLIL::Module *module) {
		pool<IdString> inputs, outputs;
		प्रति (स्वचालित wire_name : module->ports) {
			RTLIL::Wire *wire = module->wire(wire_name);
			यदि (wire->पत्तनम्_निवेशः)
				inputs.निवेशय(wire->name);
			यदि (wire->पत्तनम्_output)
				outputs.निवेशय(wire->name);
		}
		सज्जता_type(module->name, inputs, outputs);
	}

	शून्य सज्जता_type(RTLIL::IdString type, ध्रुव pool<RTLIL::IdString> &inputs, ध्रुव pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false) {
		NewCellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		custom_cell_types[ct.type] = ct;
	}

	शून्य clear() {
		custom_cell_types.clear();
		static_cell_types = StaticCellTypes::categories.रिक्तानि;
	}

	bool cell_ज्ञातम्(IdString type) ध्रुव {
		प्रत्यागम static_cell_types(type) || custom_cell_types.गणनम्(type) != 0;
	}

	bool cell_output(IdString type, IdString पत्तनम्) ध्रुव
	{
		यदि (static_cell_types(type) && StaticCellTypes::पत्तनम्_info.outputs(type).अन्तर्भवति(पत्तनम्)) {
			प्रत्यागम true;
		}
		स्वचालित it = custom_cell_types.find(type);
		प्रत्यागम it != custom_cell_types.अन्तः() && it->second.outputs.गणनम्(पत्तनम्) != 0;
	}

	bool cell_निवेशः(IdString type, IdString पत्तनम्) ध्रुव
	{
		यदि (static_cell_types(type) && StaticCellTypes::पत्तनम्_info.inputs(type).अन्तर्भवति(पत्तनम्)) {
			प्रत्यागम true;
		}
		स्वचालित it = custom_cell_types.find(type);
		प्रत्यागम it != custom_cell_types.अन्तः() && it->second.inputs.गणनम्(पत्तनम्) != 0;
	}

	RTLIL::पत्तनम्_Dir cell_पत्तनम्_dir(RTLIL::IdString type, RTLIL::IdString पत्तनम्) ध्रुव
	{
		bool is_निवेशः, is_output;
		यदि (static_cell_types(type)) {
			is_निवेशः = StaticCellTypes::पत्तनम्_info.inputs(type).अन्तर्भवति(पत्तनम्);
			is_output = StaticCellTypes::पत्तनम्_info.outputs(type).अन्तर्भवति(पत्तनम्);
		} अन्यथा {
			स्वचालित it = custom_cell_types.find(type);
			यदि (it == custom_cell_types.अन्तः())
				प्रत्यागम RTLIL::PD_UNKNOWN;
			is_निवेशः = it->second.inputs.गणनम्(पत्तनम्);
			is_output = it->second.outputs.गणनम्(पत्तनम्);
		}
		प्रत्यागम RTLIL::पत्तनम्_Dir(is_निवेशः + is_output * 2);
	}
	bool cell_evaluable(IdString type) ध्रुव
	{
		प्रत्यागम static_cell_types(type) && StaticCellTypes::categories.is_evaluable(type);
	}
};

बाह्य NewCellTypes yosys_celltypes;

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
