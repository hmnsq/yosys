#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NEWCELLTYPES_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NEWCELLTYPES_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/yosys.h"

YOSYS_NAMESPACE_BEGIN

/**
 * This API is unstable.
 * It may change or be removed in future versions and break dependent code.
 */

namespace StaticCellTypes {

// Given by last internal cell type IdString constids.inc, compilation error if too low
constexpr ਪੂਰਨ MAX_CELLS = 300;
// Currently given by _MUX16_, compilation error if too low
constexpr ਪੂਰਨ MAX_PORTS = 20;
ਬਣਤਰ Cell_ਸਾਰਣੀ_Builder {
	ਬਣਤਰ ਪੋਰਟ_List {
		std::array<RTLIL::IdString, MAX_PORTS> ports{};
		ਮਾਪ_t ਗਿਣਤੀ_2 = 0;
		constexpr ਪੋਰਟ_List() = ਮੂਲ;
		constexpr ਪੋਰਟ_List(std::initializer_list<RTLIL::IdString> init) {
			ਲਈ (ਸਵੈਚਾਲਿਤ p : init) {
				ports[ਗਿਣਤੀ_2++] = p;
			}
		}
		constexpr ਸਵੈਚਾਲਿਤ begin() ਅਚਲ { ਵਾਪਸ ports.begin(); }
		constexpr ਸਵੈਚਾਲਿਤ ਸਿਰਾ() ਅਚਲ { ਵਾਪਸ ports.begin() + ਗਿਣਤੀ_2; }
		constexpr bool ਸ਼ਾਮਲ_ਹੈ(RTLIL::IdString ਪੋਰਟ) ਅਚਲ {
			ਲਈ (ਮਾਪ_t i = 0; i < ਗਿਣਤੀ_2; i++) {
				ਜੇ (ਪੋਰਟ == ports[i])
					ਵਾਪਸ true;
			}

			ਵਾਪਸ false;
		}
		constexpr ਮਾਪ_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਗਿਣਤੀ_2; }
	};
	ਬਣਤਰ Features {
		bool is_evaluable = false;
		bool is_combinatorial = false;
		bool is_synthesizable = false;
		bool is_stdcell = false;
		bool is_ff = false;
		bool is_mem_noff = false;
		bool is_anyinit = false;
		bool is_tristate = false;
	};
	ਬਣਤਰ CellInfo {
		RTLIL::IdString type;
		ਪੋਰਟ_List inputs, outputs;
		Features features;
	};
	std::array<CellInfo, MAX_CELLS> cells{};
	ਮਾਪ_t ਗਿਣਤੀ_2 = 0;

	constexpr ਖਾਲੀ ਤਿਆਰੀ_type(RTLIL::IdString type, std::initializer_list<RTLIL::IdString> inputs, std::initializer_list<RTLIL::IdString> outputs, ਅਚਲ Features& features) {
		cells[ਗਿਣਤੀ_2++] = {type, ਪੋਰਟ_List(inputs), ਪੋਰਟ_List(outputs), features};
	}
	constexpr ਖਾਲੀ ਤਿਆਰੀ_internals_other()
	{
		Features features {};
		features.is_tristate = true;
		ਤਿਆਰੀ_type(ID($tribuf), {ID::A, ID::EN}, {ID::Y}, features);

		features = {};
		ਤਿਆਰੀ_type(ID($barrier), {ID::A}, {ID::Y}, features);

		features = {};
		ਤਿਆਰੀ_type(ID($ਪ੍ਰਤਿਜ੍ਞਾ), {ID::A, ID::EN}, {}, features);
		ਤਿਆਰੀ_type(ID($assume), {ID::A, ID::EN}, {}, features);
		ਤਿਆਰੀ_type(ID($ਜੀਵਤ), {ID::A, ID::EN}, {}, features);
		ਤਿਆਰੀ_type(ID($fair), {ID::A, ID::EN}, {}, features);
		ਤਿਆਰੀ_type(ID($cover), {ID::A, ID::EN}, {}, features);
		ਤਿਆਰੀ_type(ID($initstate), {}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($anyconst), {}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($anyseq), {}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($allconst), {}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($allseq), {}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($equiv), {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($specify2), {ID::EN, ID::SRC, ID::DST}, {}, features);
		ਤਿਆਰੀ_type(ID($specify3), {ID::EN, ID::SRC, ID::DST, ID::DAT}, {}, features);
		ਤਿਆਰੀ_type(ID($specrule), {ID::SRC_EN, ID::DST_EN, ID::SRC, ID::DST}, {}, features);
		ਤਿਆਰੀ_type(ID($print), {ID::EN, ID::ਦਲੀਲਾਂ, ID::TRG}, {}, features);
		ਤਿਆਰੀ_type(ID($check), {ID::A, ID::EN, ID::ਦਲੀਲਾਂ, ID::TRG}, {}, features);
		ਤਿਆਰੀ_type(ID($ਸੈੱਟ_ਟੈਗ), {ID::A, ID::ਸੈੱਟ_2, ID::CLR}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($get_ਟੈਗ), {ID::A}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($overwrite_ਟੈਗ), {ID::A, ID::ਸੈੱਟ_2, ID::CLR}, {}, features);
		ਤਿਆਰੀ_type(ID($original_ਟੈਗ), {ID::A}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($future_ff), {ID::A}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($scopeinfo), {}, {}, features);
		ਤਿਆਰੀ_type(ID($ਨਿਵੇਸ਼_ਪੋਰਟ), {}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($connect), {ID::A, ID::B}, {}, features);
	}
	constexpr ਖਾਲੀ ਤਿਆਰੀ_internals_eval()
	{
		Features features {};
		features.is_evaluable = true;
		std::initializer_list<RTLIL::IdString> unary_ops = {
			ID($not), ID($pos), ID($buf), ID($neg),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($ਟੁਕੜਾ), ID($lut), ID($sop)
		};

		std::initializer_list<RTLIL::IdString> ਦੁਆਧਾਰੀ_ops = {
			ID($and), ID($or), ID($xor), ID($xnor),
			ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ਖਿਸਕਾਓ), ID($shiftx),
			ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt),
			ID($ਜੋੜੋ), ID($sub), ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ਘਾਤ),
			ID($logic_and), ID($logic_or), ID($concat), ID($macc),
			ID($bweqx)
		};

		ਲਈ (ਸਵੈਚਾਲਿਤ type : unary_ops)
			ਤਿਆਰੀ_type(type, {ID::A}, {ID::Y}, features);

		ਲਈ (ਸਵੈਚਾਲਿਤ type : ਦੁਆਧਾਰੀ_ops)
			ਤਿਆਰੀ_type(type, {ID::A, ID::B}, {ID::Y}, features);

		ਲਈ (ਸਵੈਚਾਲਿਤ type : {ID($mux), ID($pmux), ID($bwmux)})
			ਤਿਆਰੀ_type(type, {ID::A, ID::B, ID::S}, {ID::Y}, features);

		ਲਈ (ਸਵੈਚਾਲਿਤ type : {ID($bmux), ID($demux)})
			ਤਿਆਰੀ_type(type, {ID::A, ID::S}, {ID::Y}, features);

		ਤਿਆਰੀ_type(ID($lcu), {ID::P, ID::G, ID::CI}, {ID::CO}, features);
		ਤਿਆਰੀ_type(ID($alu), {ID::A, ID::B, ID::CI, ID::BI}, {ID::X, ID::Y, ID::CO}, features);
		ਤਿਆਰੀ_type(ID($macc_v2), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($fa), {ID::A, ID::B, ID::C}, {ID::X, ID::Y}, features);
	}
	constexpr ਖਾਲੀ ਤਿਆਰੀ_internals_ff()
	{
		Features features {};
		features.is_ff = true;
		ਤਿਆਰੀ_type(ID($sr), {ID::ਸੈੱਟ_2, ID::CLR}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($ff), {ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($dff), {ID::CLK, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($dffe), {ID::CLK, ID::EN, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($dffsr), {ID::CLK, ID::ਸੈੱਟ_2, ID::CLR, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($dffsre), {ID::CLK, ID::ਸੈੱਟ_2, ID::CLR, ID::D, ID::EN}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($adff), {ID::CLK, ID::ARST, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($adffe), {ID::CLK, ID::ARST, ID::D, ID::EN}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($aldff), {ID::CLK, ID::ALOAD, ID::AD, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($aldffe), {ID::CLK, ID::ALOAD, ID::AD, ID::D, ID::EN}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($sdff), {ID::CLK, ID::SRST, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($sdffe), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($sdffce), {ID::CLK, ID::SRST, ID::D, ID::EN}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($dlatch), {ID::EN, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($adlatch), {ID::EN, ID::D, ID::ARST}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($dlatchsr), {ID::EN, ID::ਸੈੱਟ_2, ID::CLR, ID::D}, {ID::Q}, features);
	}
	constexpr ਖਾਲੀ ਤਿਆਰੀ_internals_anyinit()
	{
		Features features {};
		features.is_anyinit = true;
		ਤਿਆਰੀ_type(ID($anyinit), {ID::D}, {ID::Q}, features);
	}
	constexpr ਖਾਲੀ ਤਿਆਰੀ_internals_mem_noff()
	{
		Features features {};
		features.is_mem_noff = true;
		// NOT setup_internals_ff()

		ਤਿਆਰੀ_type(ID($memrd), {ID::CLK, ID::EN, ID::ADDR}, {ID::ਅੰਕੜੇ_2}, features);
		ਤਿਆਰੀ_type(ID($memrd_v2), {ID::CLK, ID::EN, ID::ARST, ID::SRST, ID::ADDR}, {ID::ਅੰਕੜੇ_2}, features);
		ਤਿਆਰੀ_type(ID($memwr), {ID::CLK, ID::EN, ID::ADDR, ID::ਅੰਕੜੇ_2}, {}, features);
		ਤਿਆਰੀ_type(ID($memwr_v2), {ID::CLK, ID::EN, ID::ADDR, ID::ਅੰਕੜੇ_2}, {}, features);
		ਤਿਆਰੀ_type(ID($meminit), {ID::ADDR, ID::ਅੰਕੜੇ_2}, {}, features);
		ਤਿਆਰੀ_type(ID($meminit_v2), {ID::ADDR, ID::ਅੰਕੜੇ_2, ID::EN}, {}, features);
		ਤਿਆਰੀ_type(ID($mem), {ID::RD_CLK, ID::RD_EN, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ਅੰਕੜੇ}, {ID::RD_ਅੰਕੜੇ}, features);
		ਤਿਆਰੀ_type(ID($mem_v2), {ID::RD_CLK, ID::RD_EN, ID::RD_ARST, ID::RD_SRST, ID::RD_ADDR, ID::WR_CLK, ID::WR_EN, ID::WR_ADDR, ID::WR_ਅੰਕੜੇ}, {ID::RD_ਅੰਕੜੇ}, features);

		// What?
		ਤਿਆਰੀ_type(ID($fsm), {ID::CLK, ID::ARST, ID::CTRL_ਵਿੱਚ}, {ID::CTRL_ਬਾਹਰਲਾ}, features);
	}
	constexpr ਖਾਲੀ ਤਿਆਰੀ_stdcells_tristate()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_tristate = true;
		ਤਿਆਰੀ_type(ID($_TBUF_), {ID::A, ID::E}, {ID::Y}, features);
	}

	constexpr ਖਾਲੀ ਤਿਆਰੀ_stdcells_eval()
	{
		Features features {};
		features.is_stdcell = true;
		features.is_evaluable = true;
		ਤਿਆਰੀ_type(ID($_BUF_), {ID::A}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_NOT_), {ID::A}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_AND_), {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_NAND_), {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_OR_),  {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_NOR_),  {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_XOR_), {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_XNOR_), {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_ANDNOT_), {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_ORNOT_), {ID::A, ID::B}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_MUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_NMUX_), {ID::A, ID::B, ID::S}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_MUX4_), {ID::A, ID::B, ID::C, ID::D, ID::S, ID::T}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_MUX8_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::S, ID::T, ID::U}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_MUX16_), {ID::A, ID::B, ID::C, ID::D, ID::E, ID::F, ID::G, ID::H, ID::I, ID::J, ID::K, ID::L, ID::M, ID::N, ID::O, ID::P, ID::S, ID::T, ID::U, ID::V}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_AOI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_OAI3_), {ID::A, ID::B, ID::C}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_AOI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
		ਤਿਆਰੀ_type(ID($_OAI4_), {ID::A, ID::B, ID::C, ID::D}, {ID::Y}, features);
	}

	constexpr ਖਾਲੀ ਤਿਆਰੀ_stdcells_ff() {
		Features features {};
		features.is_stdcell = true;
		features.is_ff = true;

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_SR_") + c1 + c2 + "_", {ID::S, ID::R}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SR_NN_), {ID::S, ID::R}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SR_NP_), {ID::S, ID::R}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SR_PN_), {ID::S, ID::R}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SR_PP_), {ID::S, ID::R}, {ID::Q}, features);

		ਤਿਆਰੀ_type(ID($_FF_), {ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DFF_") + c1 + "_", {ID::C, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_N_), {ID::C, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_P_), {ID::C, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + "_", {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PN_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PP_), {ID::C, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// 	setup_type(std::string("$_ALDFF_") + c1 + c2 + "_", {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFF_NN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFF_NP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFF_PN_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFF_PP_), {ID::C, ID::L, ID::AD, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_ALDFFE_") + c1 + c2 + c3 + "_", {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_NNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_NNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_NPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_NPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_PNN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_PNP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_PPN_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_ALDFFE_PPP_), {ID::C, ID::L, ID::AD, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DFFSR_") + c1 + c2 + c3 + "_", {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_NNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_NNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_NPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_NPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_PNN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_PNP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_PPN_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSR_PPP_), {ID::C, ID::S, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_DFFSRE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_NPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PNNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PNNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PNPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PNPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PPNN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PPNP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PPPN_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DFFSRE_PPPP_), {ID::C, ID::S, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_SDFF_") + c1 + c2 + c3 + "_", {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_NN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_NN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_NP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_NP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_PN0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_PN1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_PP0_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFF_PP1_), {ID::C, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// for (auto c4 : list_np)
		// 	setup_type(std::string("$_SDFFCE_") + c1 + c2 + c3 + c4 + "_", {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_NP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PN0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PN0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PN1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PN1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PP0N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PP0P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PP1N_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_SDFFCE_PP1P_), {ID::C, ID::R, ID::D, ID::E}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// 	setup_type(std::string("$_DLATCH_") + c1 + "_", {ID::E, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_N_), {ID::E, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_P_), {ID::E, ID::D}, {ID::Q}, features);

		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_01)
		// 	setup_type(std::string("$_DLATCH_") + c1 + c2 + c3 + "_", {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_NN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_NN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_NP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_NP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_PN0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_PN1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_PP0_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCH_PP1_), {ID::E, ID::R, ID::D}, {ID::Q}, features);
		// for (auto c1 : list_np)
		// for (auto c2 : list_np)
		// for (auto c3 : list_np)
		// 	setup_type(std::string("$_DLATCHSR_") + c1 + c2 + c3 + "_", {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_NNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_NNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_NPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_NPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_PNN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_PNP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_PPN_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
		ਤਿਆਰੀ_type(ID($_DLATCHSR_PPP_), {ID::E, ID::S, ID::R, ID::D}, {ID::Q}, features);
	}
	constexpr Cell_ਸਾਰਣੀ_Builder() {
		ਤਿਆਰੀ_internals_other();
		ਤਿਆਰੀ_internals_eval();
		ਤਿਆਰੀ_internals_ff();
		ਤਿਆਰੀ_internals_anyinit();
		ਤਿਆਰੀ_internals_mem_noff();
		ਤਿਆਰੀ_stdcells_tristate();
		ਤਿਆਰੀ_stdcells_eval();
		ਤਿਆਰੀ_stdcells_ff();
	}

};

constexpr Cell_ਸਾਰਣੀ_Builder builder {};

ਬਣਤਰ ਪੋਰਟ_Info {
	ਬਣਤਰ ਪੋਰਟ_Lists {
		std::array<Cell_ਸਾਰਣੀ_Builder::ਪੋਰਟ_List, MAX_CELLS> ਅੰਕੜੇ{};
		constexpr Cell_ਸਾਰਣੀ_Builder::ਪੋਰਟ_List ਸੰਚਾਲਕ()(IdString type) ਅਚਲ {
			ਵਾਪਸ ਅੰਕੜੇ[type.raw()];
		}
		constexpr Cell_ਸਾਰਣੀ_Builder::ਪੋਰਟ_List& ਸੰਚਾਲਕ[](ਮਾਪ_t idx) {
			ਵਾਪਸ ਅੰਕੜੇ[idx];
		}
		constexpr ਮਾਪ_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ.ਮਾਪ(); }
	};
	ਪੋਰਟ_Lists inputs {};
	ਪੋਰਟ_Lists outputs {};
	constexpr ਪੋਰਟ_Info() {
		ਲਈ (ਮਾਪ_t i = 0; i < builder.ਗਿਣਤੀ_2; ++i) {
			ਸਵੈਚਾਲਿਤ& cell = builder.cells[i];
			ਮਾਪ_t idx = cell.type.raw();
			inputs[idx] = cell.inputs;
			outputs[idx] = cell.outputs;
		}
	}
};

ਬਣਤਰ Categories {
	ਬਣਤਰ Category {
		std::array<bool, MAX_CELLS> ਅੰਕੜੇ{};
		constexpr bool ਸੰਚਾਲਕ()(IdString type) ਅਚਲ {
			ਮਾਪ_t idx = type.raw();
			ਜੇ (idx >= MAX_CELLS)
				ਵਾਪਸ false;
			ਵਾਪਸ ਅੰਕੜੇ[idx];
		}
		constexpr bool ਸੰਚਾਲਕ[](ਮਾਪ_t idx) {
			ਵਾਪਸ ਅੰਕੜੇ[idx];
		}
		constexpr ਖਾਲੀ ਸੈੱਟ_id(IdString type, bool val = true) {
			ਮਾਪ_t idx = type.raw();
			ਜੇ (idx >= MAX_CELLS) {
				ਜੇ (std::is_constant_evaluated())
					throw "cell type index out of range";
				log_abort();
			}
			ਅੰਕੜੇ[idx] = val;
		}
		constexpr ਖਾਲੀ ਸੈੱਟ(ਮਾਪ_t idx, bool val = true) {
			ਅੰਕੜੇ[idx] = val;
		}
		constexpr ਮਾਪ_t ਮਾਪ() ਅਚਲ { ਵਾਪਸ ਅੰਕੜੇ.ਮਾਪ(); }
	};
	Category ਖਾਲੀ_ਥਾਵਾਂ {};
	Category is_ਗਿਆਤ {};
	Category is_evaluable {};
	Category is_combinatorial {};
	Category is_synthesizable {};
	Category is_stdcell {};
	Category is_ff {};
	Category is_mem_noff {};
	Category is_anyinit {};
	Category is_tristate {};
	constexpr Categories() {
		ਲਈ (ਮਾਪ_t i = 0; i < builder.ਗਿਣਤੀ_2; ++i) {
			ਸਵੈਚਾਲਿਤ& cell = builder.cells[i];
			ਮਾਪ_t idx = cell.type.raw();
			is_ਗਿਆਤ.ਸੈੱਟ(idx);
			is_evaluable.ਸੈੱਟ(idx, cell.features.is_evaluable);
			is_combinatorial.ਸੈੱਟ(idx, cell.features.is_combinatorial);
			is_synthesizable.ਸੈੱਟ(idx, cell.features.is_synthesizable);
			is_stdcell.ਸੈੱਟ(idx, cell.features.is_stdcell);
			is_ff.ਸੈੱਟ(idx, cell.features.is_ff);
			is_mem_noff.ਸੈੱਟ(idx, cell.features.is_mem_noff);
			is_anyinit.ਸੈੱਟ(idx, cell.features.is_anyinit);
			is_tristate.ਸੈੱਟ(idx, cell.features.is_tristate);
		}
	}
	constexpr ਸਥਿਰ Category join(Category ਖੱਬਾ_2, Category ਸੱਜਾ_2) {
		Category c {};
		ਲਈ (ਮਾਪ_t i = 0; i < MAX_CELLS; ++i) {
			c.ਸੈੱਟ(i, ਖੱਬਾ_2[i] || ਸੱਜਾ_2[i]);
		}
		ਵਾਪਸ c;
	}
	constexpr ਸਥਿਰ Category meet(Category ਖੱਬਾ_2, Category ਸੱਜਾ_2) {
		Category c {};
		ਲਈ (ਮਾਪ_t i = 0; i < MAX_CELLS; ++i) {
			c.ਸੈੱਟ(i, ਖੱਬਾ_2[i] && ਸੱਜਾ_2[i]);
		}
		ਵਾਪਸ c;
	}
	// Sketchy! Make sure to always meet with only the known universe.
	// In other words, no modus tollens allowed
	constexpr ਸਥਿਰ Category complement(Category arg) {
		Category c {};
		ਲਈ (ਮਾਪ_t i = 0; i < MAX_CELLS; ++i) {
			c.ਸੈੱਟ(i, !arg[i]);
		}
		ਵਾਪਸ c;
	}
};

//ਸ਼ੁੱਧ
ਸਥਿਰ constexpr ਪੋਰਟ_Info ਪੋਰਟ_info;
ਸਥਿਰ constexpr Categories categories;

// Legacy
namespace Compat {
	ਸਥਿਰ constexpr ਸਵੈਚਾਲਿਤ internals_all = Categories::meet(categories.is_ਗਿਆਤ, Categories::complement(categories.is_stdcell));
	ਸਥਿਰ constexpr ਸਵੈਚਾਲਿਤ mem_ff = Categories::join(categories.is_ff, categories.is_mem_noff);
	// old setup_internals + setup_stdcells
	ਸਥਿਰ constexpr ਸਵੈਚਾਲਿਤ nomem_noff = Categories::meet(categories.is_ਗਿਆਤ, Categories::complement(mem_ff));
	ਸਥਿਰ constexpr ਸਵੈਚਾਲਿਤ internals_mem_ff = Categories::meet(internals_all, mem_ff);
	// old setup_internals
	ਸਥਿਰ constexpr ਸਵੈਚਾਲਿਤ internals_nomem_noff = Categories::meet(internals_all, nomem_noff);
	// old setup_stdcells
	ਸਥਿਰ constexpr ਸਵੈਚਾਲਿਤ stdcells_nomem_noff = Categories::meet(categories.is_stdcell, nomem_noff);
	ਸਥਿਰ constexpr ਸਵੈਚਾਲਿਤ stdcells_mem = Categories::meet(categories.is_stdcell, categories.is_mem_noff);
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

ਬਣਤਰ NewCellType {
	RTLIL::IdString type;
	pool<RTLIL::IdString> inputs, outputs;
	bool is_evaluable;
	bool is_combinatorial;
	bool is_synthesizable;
};

ਬਣਤਰ NewCellTypes {
	StaticCellTypes::Categories::Category static_cell_types = StaticCellTypes::categories.ਖਾਲੀ_ਥਾਵਾਂ;
	dict<IdString, NewCellType> custom_cell_types {};

	NewCellTypes() {
		static_cell_types = StaticCellTypes::categories.ਖਾਲੀ_ਥਾਵਾਂ;
	}

	NewCellTypes(RTLIL::Design *design) {
		static_cell_types = StaticCellTypes::categories.ਖਾਲੀ_ਥਾਵਾਂ;
		ਤਿਆਰੀ(design);
	}
	ਖਾਲੀ ਤਿਆਰੀ(RTLIL::Design *design = ਨਿਰਾ) {
		ਜੇ (design)
			ਤਿਆਰੀ_design(design);
		static_cell_types = StaticCellTypes::categories.is_ਗਿਆਤ;
	}
	ਖਾਲੀ ਤਿਆਰੀ_design(RTLIL::Design *design) {
		ਲਈ (ਸਵੈਚਾਲਿਤ module : design->modules())
			ਤਿਆਰੀ_module(module);
	}

	ਖਾਲੀ ਤਿਆਰੀ_module(RTLIL::Module *module) {
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

	ਖਾਲੀ ਤਿਆਰੀ_type(RTLIL::IdString type, ਅਚਲ pool<RTLIL::IdString> &inputs, ਅਚਲ pool<RTLIL::IdString> &outputs, bool is_evaluable = false, bool is_combinatorial = false, bool is_synthesizable = false) {
		NewCellType ct = {type, inputs, outputs, is_evaluable, is_combinatorial, is_synthesizable};
		custom_cell_types[ct.type] = ct;
	}

	ਖਾਲੀ clear() {
		custom_cell_types.clear();
		static_cell_types = StaticCellTypes::categories.ਖਾਲੀ_ਥਾਵਾਂ;
	}

	bool cell_ਗਿਆਤ(IdString type) ਅਚਲ {
		ਵਾਪਸ static_cell_types(type) || custom_cell_types.ਗਿਣਤੀ_2(type) != 0;
	}

	bool cell_output(IdString type, IdString ਪੋਰਟ) ਅਚਲ
	{
		ਜੇ (static_cell_types(type) && StaticCellTypes::ਪੋਰਟ_info.outputs(type).ਸ਼ਾਮਲ_ਹੈ(ਪੋਰਟ)) {
			ਵਾਪਸ true;
		}
		ਸਵੈਚਾਲਿਤ it = custom_cell_types.ਲੱਭੋ(type);
		ਵਾਪਸ it != custom_cell_types.ਸਿਰਾ() && it->ਦੂਜਾ.outputs.ਗਿਣਤੀ_2(ਪੋਰਟ) != 0;
	}

	bool cell_ਨਿਵੇਸ਼(IdString type, IdString ਪੋਰਟ) ਅਚਲ
	{
		ਜੇ (static_cell_types(type) && StaticCellTypes::ਪੋਰਟ_info.inputs(type).ਸ਼ਾਮਲ_ਹੈ(ਪੋਰਟ)) {
			ਵਾਪਸ true;
		}
		ਸਵੈਚਾਲਿਤ it = custom_cell_types.ਲੱਭੋ(type);
		ਵਾਪਸ it != custom_cell_types.ਸਿਰਾ() && it->ਦੂਜਾ.inputs.ਗਿਣਤੀ_2(ਪੋਰਟ) != 0;
	}

	RTLIL::ਪੋਰਟ_Dir cell_ਪੋਰਟ_dir(RTLIL::IdString type, RTLIL::IdString ਪੋਰਟ) ਅਚਲ
	{
		bool is_ਨਿਵੇਸ਼, is_output;
		ਜੇ (static_cell_types(type)) {
			is_ਨਿਵੇਸ਼ = StaticCellTypes::ਪੋਰਟ_info.inputs(type).ਸ਼ਾਮਲ_ਹੈ(ਪੋਰਟ);
			is_output = StaticCellTypes::ਪੋਰਟ_info.outputs(type).ਸ਼ਾਮਲ_ਹੈ(ਪੋਰਟ);
		} ਨਹੀਂਤਾਂ {
			ਸਵੈਚਾਲਿਤ it = custom_cell_types.ਲੱਭੋ(type);
			ਜੇ (it == custom_cell_types.ਸਿਰਾ())
				ਵਾਪਸ RTLIL::PD_UNKNOWN;
			is_ਨਿਵੇਸ਼ = it->ਦੂਜਾ.inputs.ਗਿਣਤੀ_2(ਪੋਰਟ);
			is_output = it->ਦੂਜਾ.outputs.ਗਿਣਤੀ_2(ਪੋਰਟ);
		}
		ਵਾਪਸ RTLIL::ਪੋਰਟ_Dir(is_ਨਿਵੇਸ਼ + is_output * 2);
	}
	bool cell_evaluable(IdString type) ਅਚਲ
	{
		ਵਾਪਸ static_cell_types(type) && StaticCellTypes::categories.is_evaluable(type);
	}
};

ਬਾਹਰੀ NewCellTypes yosys_celltypes;

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
