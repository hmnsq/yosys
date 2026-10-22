#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NEWCELLTYPES_H
#ವ್ಯಾಖ್ಯಾನಿಸು NEWCELLTYPES_H

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

/**
 * This API is unstable.
 * It may change or be removed in future versions and break dependent code.
 */

namespace StaticCellTypes {

// Given by last internal cell type IdString constids.inc, compilation error if too low
constexpr ಪೂರ್ಣಾಂಕ MAX_CELLS = 300;
// Currently given by _MUX16_, compilation error if too low
constexpr ಪೂರ್ಣಾಂಕ MAX_PORTS = 20;
ರಚನೆ Cell_ಕೋಷ್ಟಕ_Builder {
	ರಚನೆ ಪೋರ್ಟ್_List {
		std::array<RTLIL::IdString, MAX_PORTS> ports{};
		ಗಾತ್ರ_t ಎಣಿಕೆ_2 = 0;
		constexpr ಪೋರ್ಟ್_List() = ಪೂರ್ವನಿಯೋಜಿತ;
		constexpr ಪೋರ್ಟ್_List(std::initializer_list<RTLIL::IdString> init) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ p : init) {
				ports[ಎಣಿಕೆ_2++] = p;
			}
		}
		constexpr ಸ್ವಯಂಚಾಲಿತ begin() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ports.begin(); }
		constexpr ಸ್ವಯಂಚಾಲಿತ ಕೊನೆ() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ports.begin() + ಎಣಿಕೆ_2; }
		constexpr bool ಒಳಗೊಂಡಿದೆ(RTLIL::IdString ಪೋರ್ಟ್) ಸ್ಥಿರಾಂಕ {
			ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < ಎಣಿಕೆ_2; i++) {
				ಆದರೆ (ಪೋರ್ಟ್ == ports[i])
					ಹಿಂತಿರುಗಿ true;
			}

			ಹಿಂತಿರುಗಿ false;
		}
		constexpr ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2; }
	};
	ರಚನೆ Features {
		bool is_evaluable = false;
		bool is_combinatorial = false;
		bool is_synthesizable = false;
		bool is_stdcell = false;
		bool is_ff = false;
		bool is_mem_noff = false;
		bool is_anyinit = false;
		bool is_tristate = false;
	};
	ರಚನೆ CellInfo {
		RTLIL::IdString type;
		ಪೋರ್ಟ್_List inputs, outputs;
		Features features;
	};
	std::array<CellInfo, MAX_CELLS> cells{};
	ಗಾತ್ರ_t ಎಣಿಕೆ_2 = 0;

	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_type(RTLIL::IdString type, std::initializer_list<RTLIL::IdString> inputs, std::initializer_list<RTLIL::IdString> outputs, ಸ್ಥಿರಾಂಕ Features& features) {
		cells[ಎಣಿಕೆ_2++] = {type, ಪೋರ್ಟ್_List(inputs), ಪೋರ್ಟ್_List(outputs), features};
	}
	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_other()
	{
		Features features {};
		features.is_tristate = true;
		ಸಿದ್ಧತೆ_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y}, features);

		features = {};
		ಸಿದ್ಧತೆ_type(ID($barrier), {ID::A}, {ID::Y}, features);

		features = {};
		ಸಿದ್ಧತೆ_type(ID($ಪ್ರತಿಜ್ಞಾ), {ID::A, ID::EN}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($assume), {ID::A, ID::EN}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($ಜೀವಂತ), {ID::A, ID::EN}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($fair), {ID::A, ID::EN}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($cover), {ID::A, ID::EN}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($initstate), {}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($anyconst), {}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($anyseq), {}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($allconst), {}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($allseq), {}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($equiv), {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($print), {ID::EN, ID::ವಾದಗಳು, ID::TRG}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($check), {ID::A, ID::EN, ID::ವಾದಗಳು, ID::TRG}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($ಗಣ_ಟ್ಯಾಗ್), {ID::A, ID::ಗಣ_2, ID::CLR}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($get_ಟ್ಯಾಗ್), {ID::A}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($overwrite_ಟ್ಯಾಗ್), {ID::A, ID::ಗಣ_2, ID::CLR}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($original_ಟ್ಯಾಗ್), {ID::A}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($future_ff), {ID::A}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($scopeinfo), {}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($ಒಳಹರಿವು_ಪೋರ್ಟ್), {}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($connect), {ID::A, ID::B}, {}, features);
	}
	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_eval()
	{
		Features features {};
		features.is_evaluable = true;
		std::initializer_list<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ತುಂಡು), ID($lut), ID($sop)
		};

		std::initializer_list<RTLIL::IdString> ದ್ವಿಮಾನ_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ಸರಿಸು), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($ಕೂಡಿಸು), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ಘಾತ),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : unary_ops)
			ಸಿದ್ಧತೆ_type(type, {ID::A}, {ID::Y}, features);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : ದ್ವಿಮಾನ_ops)
			ಸಿದ್ಧತೆ_type(type, {ID::A, ID::B}, {ID::Y}, features);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : {ID($mux), ID($pmux), ID($bwmux)})
			ಸಿದ್ಧತೆ_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, features);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ type : {ID($bmux), ID($demux)})
			ಸಿದ್ಧತೆ_type(type, {ID::A, ID::S}, {ID::Y}, features);

		ಸಿದ್ಧತೆ_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, features);
		ಸಿದ್ಧತೆ_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, features);
		ಸಿದ್ಧತೆ_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, features);
	}
	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_ff()
	{
		Features features {};
		features.is_ff = true;
		ಸಿದ್ಧತೆ_type(ID($sr), {ID::ಗಣ_2, ID::CLR}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($ff), {ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($dff), {ID::CLK, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($dffsr), {ID::CLK, ID::ಗಣ_2, ID::CLR, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($dffsre), {ID::CLK, ID::ಗಣ_2, ID::CLR, ID::D, ID::EN}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($dlatchsr), {ID::EN, ID::ಗಣ_2, ID::CLR, ID::D}, {ID::Q}, features);
	}
	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_anyinit()
	{
		Features features {};
		features.is_anyinit = true;
		ಸಿದ್ಧತೆ_type(ID($anyinit), {ID::D}, {ID::Q}, features);
	}
	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_internals_mem_noff()
	{
		Features features {};
		features.is_mem_noff = true;
		// NOT setup_internals_ff()

		ಸಿದ್ಧತೆ_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::ದತ್ತಾಂಶ_2}, features);
		ಸಿದ್ಧತೆ_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::ದತ್ತಾಂಶ_2}, features);
		ಸಿದ್ಧತೆ_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::ದತ್ತಾಂಶ_2}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::ದತ್ತಾಂಶ_2}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($meminit), {ID::ADDR, ID::ದತ್ತಾಂಶ_2}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($meminit_v2), {ID::ADDR, ID::ದತ್ತಾಂಶ_2, ID::EN}, {}, features);
		ಸಿದ್ಧತೆ_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ದತ್ತಾಂಶ}, {ID::RD_ದತ್ತಾಂಶ}, features);
		ಸಿದ್ಧತೆ_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ದತ್ತಾಂಶ}, {ID::RD_ದತ್ತಾಂಶ}, features);

		// What?
		ಸಿದ್ಧತೆ_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_ಒಳ}, {ID::CTRL_ಹೊರಹೋಗು}, features);
	}
	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_stdcells_tristate()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_tristate = true;
		ಸಿದ್ಧತೆ_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y}, features);
	}

	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_stdcells_eval()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_evaluable = true;
		ಸಿದ್ಧತೆ_type(ID($_BUF_), {ID::A}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_NOT_), {ID::A}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
		ಸಿದ್ಧತೆ_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
	}

	constexpr ಶೂನ್ಯ ಸಿದ್ಧತೆ_stdcells_ff() {
		Features features {};
		features.is_stdcell = true;
		features.is_ff = true;

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_SR_") + c1 + c2 + "_", {ID::S, ID::R}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SR_NN_), {ID::S, ID::R}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SR_NP_), {ID::S, ID::R}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SR_PN_), {ID::S, ID::R}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SR_PP_), {ID::S, ID::R}, {ID::Q}, features);

		ಸಿದ್ಧತೆ_type(ID($_FF_), {ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DFF_") + c1 + "_", {ID::C, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_N_), {ID::C, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_P_), {ID::C, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + "_", {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_ALDFF_") + c1 + c2 + "_", {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFF_NN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFF_NP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFF_PN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFF_PP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_ALDFFE_") + c1 + c2 + c3 + "_", {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_NNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_NNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_NPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_NPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_PNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_PNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_PPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_ALDFFE_PPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DFFSR_") + c1 + c2 + c3 + "_", {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_NNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_NNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_NPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_NPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_PNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_PNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_PPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSR_PPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFSRE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_NPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DFFSRE_PPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_SDFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFCE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_SDFFCE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DLATCH_") + c1 + "_", {ID::E, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_N_), {ID::E, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_P_), {ID::E, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DLATCH_") + c1 + c2 + c3 + "_", {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_NN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_NN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_NP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_NP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_PN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_PN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_PP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCH_PP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DLATCHSR_") + c1 + c2 + c3 + "_", {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_NNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_NNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_NPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_NPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_PNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_PNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_PPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ಸಿದ್ಧತೆ_type(ID($_DLATCHSR_PPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
	}
	constexpr Cell_ಕೋಷ್ಟಕ_Builder() {
		ಸಿದ್ಧತೆ_internals_other();
		ಸಿದ್ಧತೆ_internals_eval();
		ಸಿದ್ಧತೆ_internals_ff();
		ಸಿದ್ಧತೆ_internals_anyinit();
		ಸಿದ್ಧತೆ_internals_mem_noff();
		ಸಿದ್ಧತೆ_stdcells_tristate();
		ಸಿದ್ಧತೆ_stdcells_eval();
		ಸಿದ್ಧತೆ_stdcells_ff();
	}

};

constexpr Cell_ಕೋಷ್ಟಕ_Builder builder {};

ರಚನೆ ಪೋರ್ಟ್_Info {
	ರಚನೆ ಪೋರ್ಟ್_Lists {
		std::array<Cell_ಕೋಷ್ಟಕ_Builder::ಪೋರ್ಟ್_List, MAX_CELLS> ದತ್ತಾಂಶ{};
		constexpr Cell_ಕೋಷ್ಟಕ_Builder::ಪೋರ್ಟ್_List ಕ್ರಿಯಾಕಾರಕ()(IdString type) ಸ್ಥಿರಾಂಕ {
			ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ[type.raw()];
		}
		constexpr Cell_ಕೋಷ್ಟಕ_Builder::ಪೋರ್ಟ್_List& ಕ್ರಿಯಾಕಾರಕ[](ಗಾತ್ರ_t idx) {
			ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ[idx];
		}
		constexpr ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ಗಾತ್ರ_2(); }
	};
	ಪೋರ್ಟ್_Lists inputs {};
	ಪೋರ್ಟ್_Lists outputs {};
	constexpr ಪೋರ್ಟ್_Info() {
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < builder.ಎಣಿಕೆ_2; ++i) {
			ಸ್ವಯಂಚಾಲಿತ& cell = builder.cells[i];
			ಗಾತ್ರ_t idx = cell.type.raw();
			inputs[idx] = cell.inputs;
			outputs[idx] = cell.outputs;
		}
	}
};

ರಚನೆ Categories {
	ರಚನೆ Category {
		std::array<bool, MAX_CELLS> ದತ್ತಾಂಶ{};
		constexpr bool ಕ್ರಿಯಾಕಾರಕ()(IdString type) ಸ್ಥಿರಾಂಕ {
			ಗಾತ್ರ_t idx = type.raw();
			ಆದರೆ (idx >= MAX_CELLS)
				ಹಿಂತಿರುಗಿ false;
			ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ[idx];
		}
		constexpr bool ಕ್ರಿಯಾಕಾರಕ[](ಗಾತ್ರ_t idx) {
			ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ[idx];
		}
		constexpr ಶೂನ್ಯ ಗಣ_id(IdString type, bool val = true) {
			ಗಾತ್ರ_t idx = type.raw();
			ಆದರೆ (idx >= MAX_CELLS) {
				ಆದರೆ (std::is_constant_evaluated())
					throw "cell type index out of range";
				log_abort();
			}
			ದತ್ತಾಂಶ[idx] = val;
		}
		constexpr ಶೂನ್ಯ ಗಣ(ಗಾತ್ರ_t idx, bool val = true) {
			ದತ್ತಾಂಶ[idx] = val;
		}
		constexpr ಗಾತ್ರ_t ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ.ಗಾತ್ರ_2(); }
	};
	Category ಖಾಲಿಗಳು {};
	Category is_ತಿಳಿದ {};
	Category is_evaluable {};
	Category is_combinatorial {};
	Category is_synthesizable {};
	Category is_stdcell {};
	Category is_ff {};
	Category is_mem_noff {};
	Category is_anyinit {};
	Category is_tristate {};
	constexpr Categories() {
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < builder.ಎಣಿಕೆ_2; ++i) {
			ಸ್ವಯಂಚಾಲಿತ& cell = builder.cells[i];
			ಗಾತ್ರ_t idx = cell.type.raw();
			is_ತಿಳಿದ.ಗಣ(idx);
			is_evaluable.ಗಣ(idx, cell.features.is_evaluable);
			is_combinatorial.ಗಣ(idx, cell.features.is_combinatorial);
			is_synthesizable.ಗಣ(idx, cell.features.is_synthesizable);
			is_stdcell.ಗಣ(idx, cell.features.is_stdcell);
			is_ff.ಗಣ(idx, cell.features.is_ff);
			is_mem_noff.ಗಣ(idx, cell.features.is_mem_noff);
			is_anyinit.ಗಣ(idx, cell.features.is_anyinit);
			is_tristate.ಗಣ(idx, cell.features.is_tristate);
		}
	}
	constexpr ಸ್ಥಿರ Category join(Category ಎಡ_3, Category ಬಲ_3) {
		Category c {};
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < MAX_CELLS; ++i) {
			c.ಗಣ(i, ಎಡ_3[i] || ಬಲ_3[i]);
		}
		ಹಿಂತಿರುಗಿ c;
	}
	constexpr ಸ್ಥಿರ Category meet(Category ಎಡ_3, Category ಬಲ_3) {
		Category c {};
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < MAX_CELLS; ++i) {
			c.ಗಣ(i, ಎಡ_3[i] && ಬಲ_3[i]);
		}
		ಹಿಂತಿರುಗಿ c;
	}
	// Sketchy! Make sure to always meet with only the known universe.
	// In other words, no modus tollens allowed
	constexpr ಸ್ಥಿರ Category complement(Category arg) {
		Category c {};
		ಗಾಗಿ (ಗಾತ್ರ_t i = 0; i < MAX_CELLS; ++i) {
			c.ಗಣ(i, !arg[i]);
		}
		ಹಿಂತಿರುಗಿ c;
	}
};

//ಶುದ್ಧ
ಸ್ಥಿರ constexpr ಪೋರ್ಟ್_Info ಪೋರ್ಟ್_info;
ಸ್ಥಿರ constexpr Categories categories;

// Legacy
namespace Compat {
	ಸ್ಥಿರ constexpr ಸ್ವಯಂಚಾಲಿತ internals_all = Categories::meet(categories.is_ತಿಳಿದ, Categories::complement(categories.is_stdcell));
	ಸ್ಥಿರ constexpr ಸ್ವಯಂಚಾಲಿತ mem_ff = Categories::join(categories.is_ff, categories.is_mem_noff);
	// old setup_internals + setup_stdcells
	ಸ್ಥಿರ constexpr ಸ್ವಯಂಚಾಲಿತ nomem_noff = Categories::meet(categories.is_ತಿಳಿದ, Categories::complement(mem_ff));
	ಸ್ಥಿರ constexpr ಸ್ವಯಂಚಾಲಿತ internals_mem_ff = Categories::meet(internals_all, mem_ff);
	// old setup_internals
	ಸ್ಥಿರ constexpr ಸ್ವಯಂಚಾಲಿತ internals_nomem_noff = Categories::meet(internals_all, nomem_noff);
	// old setup_stdcells
	ಸ್ಥಿರ constexpr ಸ್ವಯಂಚಾಲಿತ stdcells_nomem_noff = Categories::meet(categories.is_stdcell, nomem_noff);
	ಸ್ಥಿರ constexpr ಸ್ವಯಂಚಾಲಿತ stdcells_mem = Categories::meet(categories.is_stdcell, categories.is_mem_noff);
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

ರಚನೆ NewCellType {
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

ರಚನೆ NewCellTypes {
	StaticCellTypes::Categories::Category static_cell_types = StaticCellTypes::categories.ಖಾಲಿಗಳು;
	dict<IdString, NewCellType> custom_cell_types {};

	NewCellTypes() {
		static_cell_types = StaticCellTypes::categories.ಖಾಲಿಗಳು;
	}

	NewCellTypes(RTLIL::Design *design) {
		static_cell_types = StaticCellTypes::categories.ಖಾಲಿಗಳು;
		ಸಿದ್ಧತೆ(design);
	}
	ಶೂನ್ಯ ಸಿದ್ಧತೆ(RTLIL::Design *design = ಖಾಲಿ) {
		ಆದರೆ (design)
			ಸಿದ್ಧತೆ_design(design);
		static_cell_types = StaticCellTypes::categories.is_ತಿಳಿದ;
	}
	ಶೂನ್ಯ ಸಿದ್ಧತೆ_design(RTLIL::Design *design) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ module : design->modules())
			ಸಿದ್ಧತೆ_module(module);
	}

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_module(RTLIL::Module *module) {
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

	ಶೂನ್ಯ ಸಿದ್ಧತೆ_type(RTLIL::IdString type, ಸ್ಥಿರಾಂಕ pool<RTLIL::IdString> &inputs, ಸ್ಥಿರಾಂಕ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false) {
		NewCellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		custom_cell_types[ct.type] = ct;
	}

	ಶೂನ್ಯ clear() {
		custom_cell_types.clear();
		static_cell_types = StaticCellTypes::categories.ಖಾಲಿಗಳು;
	}

	bool cell_ತಿಳಿದ(IdString type) ಸ್ಥಿರಾಂಕ {
		ಹಿಂತಿರುಗಿ static_cell_types(type) || custom_cell_types.ಎಣಿಕೆ_2(type) != 0;
	}

	bool cell_output(IdString type, IdString ಪೋರ್ಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (static_cell_types(type) && StaticCellTypes::ಪೋರ್ಟ್_info.outputs(type).ಒಳಗೊಂಡಿದೆ(ಪೋರ್ಟ್)) {
			ಹಿಂತಿರುಗಿ true;
		}
		ಸ್ವಯಂಚಾಲಿತ it = custom_cell_types.ಕಂಡುಹಿಡಿ(type);
		ಹಿಂತಿರುಗಿ it != custom_cell_types.ಕೊನೆ() && it->ಎರಡನೇ.outputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್) != 0;
	}

	bool cell_ಒಳಹರಿವು(IdString type, IdString ಪೋರ್ಟ್) ಸ್ಥಿರಾಂಕ
	{
		ಆದರೆ (static_cell_types(type) && StaticCellTypes::ಪೋರ್ಟ್_info.inputs(type).ಒಳಗೊಂಡಿದೆ(ಪೋರ್ಟ್)) {
			ಹಿಂತಿರುಗಿ true;
		}
		ಸ್ವಯಂಚಾಲಿತ it = custom_cell_types.ಕಂಡುಹಿಡಿ(type);
		ಹಿಂತಿರುಗಿ it != custom_cell_types.ಕೊನೆ() && it->ಎರಡನೇ.inputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್) != 0;
	}

	RTLIL::ಪೋರ್ಟ್_Dir cell_ಪೋರ್ಟ್_dir(RTLIL::IdString type, RTLIL::IdString ಪೋರ್ಟ್) ಸ್ಥಿರಾಂಕ
	{
		bool is_ಒಳಹರಿವು, is_output;
		ಆದರೆ (static_cell_types(type)) {
			is_ಒಳಹರಿವು = StaticCellTypes::ಪೋರ್ಟ್_info.inputs(type).ಒಳಗೊಂಡಿದೆ(ಪೋರ್ಟ್);
			is_output = StaticCellTypes::ಪೋರ್ಟ್_info.outputs(type).ಒಳಗೊಂಡಿದೆ(ಪೋರ್ಟ್);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಸ್ವಯಂಚಾಲಿತ it = custom_cell_types.ಕಂಡುಹಿಡಿ(type);
			ಆದರೆ (it == custom_cell_types.ಕೊನೆ())
				ಹಿಂತಿರುಗಿ RTLIL::PD_UNKNOWN;
			is_ಒಳಹರಿವು = it->ಎರಡನೇ.inputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್);
			is_output = it->ಎರಡನೇ.outputs.ಎಣಿಕೆ_2(ಪೋರ್ಟ್);
		}
		ಹಿಂತಿರುಗಿ RTLIL::ಪೋರ್ಟ್_Dir(is_ಒಳಹರಿವು + is_output * 2);
	}
	bool cell_evaluable(IdString type) ಸ್ಥಿರಾಂಕ
	{
		ಹಿಂತಿರುಗಿ static_cell_types(type) && StaticCellTypes::categories.is_evaluable(type);
	}
};

ಬಾಹ್ಯ NewCellTypes yosys_celltypes;

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
