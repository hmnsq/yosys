/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2020  Marcelina Kościelnicka <mwk@0x04.net>
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

#ಅನ್ತರ್ಭಾವ "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
ಮಾದರಿ<ಬಗೆ_ಹೆಸರು ಒಳಹರಿವು_Type, ಬಗೆ_ಹೆಸರು OutputType, ಬಗೆ_ಹೆಸರು = std::enable_if_t<std::is_ತಳ_ದ_v<Ff_Type_ದತ್ತಾಂಶ, OutputType>>>
ಶೂನ್ಯ manufacture_info(ಒಳಹರಿವು_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = ಶೂನ್ಯಸೂಚಕ;
	IdString type;
	constexpr ತಾರ್ಕಿಕ have_cell = std::is_same_v<ಒಳಹರಿವು_Type, Cell*>;
	ಆದರೆ constexpr (std::is_same_v<ಒಳಹರಿವು_Type, IdString>) {
		type = flop;
	} ಇಲ್ಲದಿದ್ದರೆ {
		static_assert(std::is_same_v<ಒಳಹರಿವು_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	ಆದರೆ constexpr (have_cell) {
		info.sig_q = cell->get_ಪೋರ್ಟ್(ID::Q);
		info.ಅಗಲ = Get_ಗಾತ್ರ(info.sig_q);
		info.ಗುಣಲಕ್ಷಣಗಳು = cell->ಗುಣಲಕ್ಷಣಗಳು;
		ಆದರೆ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	ಆದರೆ (type.ಒಳ(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		ಆದರೆ (type.ಒಳ(ID($anyinit), ID($ff))) {
			info.has_gclk = ನಿಜ;
			ಆದರೆ constexpr (have_cell)
				info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			ಆದರೆ (type == ID($anyinit)) {
				info.is_anyinit = ನಿಜ;
				ಆದರೆ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ID($sr)) {
			// No data input at all.
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type.ಒಳ(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = ನಿಜ;
			ಆದರೆ constexpr (have_cell) {
				info.sig_aload = cell->get_ಪೋರ್ಟ್(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->get_ಪೋರ್ಟ್(ID::D);
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			info.has_clk = ನಿಜ;
			ಆದರೆ constexpr (have_cell) {
				info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			}
		}
		ಆದರೆ (type.ಒಳ(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = ನಿಜ;
			ಆದರೆ constexpr (have_cell) {
				info.sig_ce = cell->get_ಪೋರ್ಟ್(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		ಆದರೆ (type.ಒಳ(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = ನಿಜ;
			ಆದರೆ constexpr (have_cell) {
				info.sig_clr = cell->get_ಪೋರ್ಟ್(ID::CLR);
				info.sig_ಗಣ = cell->get_ಪೋರ್ಟ್(ID::ಗಣ_2);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_ಗಣ = cell->getParam(ID::ಗಣ_POLARITY).as_bool();
			}
		}
		ಆದರೆ (type.ಒಳ(ID($aldff), ID($aldffe))) {
			info.has_aload = ನಿಜ;
			ಆದರೆ constexpr (have_cell) {
				info.sig_aload = cell->get_ಪೋರ್ಟ್(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->get_ಪೋರ್ಟ್(ID::AD);
			}
		}
		ಆದರೆ (type.ಒಳ(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = ನಿಜ;
			ಆದರೆ constexpr (have_cell) {
				info.sig_arst = cell->get_ಪೋರ್ಟ್(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_ಮೌಲ್ಯ);
			}
		}
		ಆದರೆ (type.ಒಳ(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = ನಿಜ;
			ಆದರೆ constexpr (have_cell) {
				info.sig_srst = cell->get_ಪೋರ್ಟ್(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_ಮೌಲ್ಯ);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ID($_FF_)) {
		info.is_fine = ನಿಜ;
		info.has_gclk = ನಿಜ;
		ಆದರೆ constexpr (have_cell)
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = ನಿಜ;
		info.has_sr = ನಿಜ;
		info.pol_ಗಣ = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_ಗಣ = cell->get_ಪೋರ್ಟ್(ID::S);
			info.sig_clr = cell->get_ಪೋರ್ಟ್(ID::R);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 6) == "$_DFF_" && type_str.ಗಾತ್ರ_2() == 8) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[6] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 7) == "$_DFFE_" && type_str.ಗಾತ್ರ_2() == 10) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = ನಿಜ;
		info.pol_ce = type_str[8] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_ce = cell->get_ಪೋರ್ಟ್(ID::E);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 6) == "$_DFF_" && type_str.ಗಾತ್ರ_2() == 10) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = ನಿಜ;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_arst = cell->get_ಪೋರ್ಟ್(ID::R);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 7) == "$_DFFE_" && type_str.ಗಾತ್ರ_2() == 12) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = ನಿಜ;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = ನಿಜ;
		info.pol_ce = type_str[10] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_arst = cell->get_ಪೋರ್ಟ್(ID::R);
			info.sig_ce = cell->get_ಪೋರ್ಟ್(ID::E);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.ಗಾತ್ರ_2() == 11) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = ನಿಜ;
		info.pol_aload = type_str[9] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_aload = cell->get_ಪೋರ್ಟ್(ID::L);
			info.sig_ad = cell->get_ಪೋರ್ಟ್(ID::AD);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.ಗಾತ್ರ_2() == 13) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = ನಿಜ;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = ನಿಜ;
		info.pol_ce = type_str[11] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_aload = cell->get_ಪೋರ್ಟ್(ID::L);
			info.sig_ad = cell->get_ಪೋರ್ಟ್(ID::AD);
			info.sig_ce = cell->get_ಪೋರ್ಟ್(ID::E);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.ಗಾತ್ರ_2() == 12) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = ನಿಜ;
		info.pol_ಗಣ = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_ಗಣ = cell->get_ಪೋರ್ಟ್(ID::S);
			info.sig_clr = cell->get_ಪೋರ್ಟ್(ID::R);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.ಗಾತ್ರ_2() == 14) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = ನಿಜ;
		info.pol_ಗಣ = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = ನಿಜ;
		info.pol_ce = type_str[12] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_ಗಣ = cell->get_ಪೋರ್ಟ್(ID::S);
			info.sig_clr = cell->get_ಪೋರ್ಟ್(ID::R);
			info.sig_ce = cell->get_ಪೋರ್ಟ್(ID::E);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 7) == "$_SDFF_" && type_str.ಗಾತ್ರ_2() == 11) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = ನಿಜ;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_srst = cell->get_ಪೋರ್ಟ್(ID::R);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.ಗಾತ್ರ_2() == 13) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = ನಿಜ;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = ನಿಜ;
		info.pol_ce = type_str[11] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_srst = cell->get_ಪೋರ್ಟ್(ID::R);
			info.sig_ce = cell->get_ಪೋರ್ಟ್(ID::E);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.ಗಾತ್ರ_2() == 14) {
		info.is_fine = ನಿಜ;
		info.has_clk = ನಿಜ;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = ನಿಜ;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = ನಿಜ;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = ನಿಜ;
		ಆದರೆ constexpr (have_cell) {
			info.sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_clk = cell->get_ಪೋರ್ಟ್(ID::C);
			info.sig_srst = cell->get_ಪೋರ್ಟ್(ID::R);
			info.sig_ce = cell->get_ಪೋರ್ಟ್(ID::E);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.ಗಾತ್ರ_2() == 11) {
		info.is_fine = ನಿಜ;
		info.has_aload = ನಿಜ;
		info.has_aload = ನಿಜ;
		info.pol_aload = type_str[9] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_ad = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_aload = cell->get_ಪೋರ್ಟ್(ID::E);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.ಗಾತ್ರ_2() == 13) {
		info.is_fine = ನಿಜ;
		info.has_aload = ನಿಜ;
		info.has_aload = ನಿಜ;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = ನಿಜ;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		ಆದರೆ constexpr (have_cell) {
			info.sig_ad = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_aload = cell->get_ಪೋರ್ಟ್(ID::E);
			info.sig_arst = cell->get_ಪೋರ್ಟ್(ID::R);
		}
	} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.ಗಾತ್ರ_2() == 15) {
		info.is_fine = ನಿಜ;
		info.has_aload = ನಿಜ;
		info.has_aload = ನಿಜ;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = ನಿಜ;
		info.pol_ಗಣ = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		ಆದರೆ constexpr (have_cell) {
			info.sig_ad = cell->get_ಪೋರ್ಟ್(ID::D);
			info.sig_aload = cell->get_ಪೋರ್ಟ್(ID::E);
			info.sig_ಗಣ = cell->get_ಪೋರ್ಟ್(ID::S);
			info.sig_clr = cell->get_ಪೋರ್ಟ್(ID::R);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		log_assert(0);
	}
	ಆದರೆ constexpr (have_cell)
		ಆದರೆ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = ಸುಳ್ಳು;
			info.has_arst = ನಿಜ;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_ದತ್ತಾಂಶ::Ff_Type_ದತ್ತಾಂಶ(IdString type) : Ff_Type_ದತ್ತಾಂಶ()
{
	manufacture_info(type, *ಇದು, ಶೂನ್ಯಸೂಚಕ);
}

Ff_ದತ್ತಾಂಶ::Ff_ದತ್ತಾಂಶ(FfInitVals *initvals, Cell *cell_) : Ff_ದತ್ತಾಂಶ(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *ಇದು, initvals);
}

Ff_ದತ್ತಾಂಶ Ff_ದತ್ತಾಂಶ::ತುಂಡು(ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &ಬಿಟ್ಟುಗಳು) {
	Ff_ದತ್ತಾಂಶ res(module, initvals, module->twines().ಕೂಡಿಸು(NEW_ID));
	res.sig_clk = sig_clk;
	res.sig_ce = sig_ce;
	res.sig_aload = sig_aload;
	res.sig_arst = sig_arst;
	res.sig_srst = sig_srst;
	res.has_clk = has_clk;
	res.has_gclk = has_gclk;
	res.has_ce = has_ce;
	res.has_aload = has_aload;
	res.has_arst = has_arst;
	res.has_srst = has_srst;
	res.has_sr = has_sr;
	res.ce_over_srst = ce_over_srst;
	res.is_fine = is_fine;
	res.is_anyinit = is_anyinit;
	res.pol_clk = pol_clk;
	res.pol_ce = pol_ce;
	res.pol_aload = pol_aload;
	res.pol_arst = pol_arst;
	res.pol_srst = pol_srst;
	res.pol_clr = pol_clr;
	res.pol_ಗಣ = pol_ಗಣ;
	res.ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು;
	std::optional<Const::Builder> arst_ಬಿಟ್ಟುಗಳು;
	ಆದರೆ (has_arst)
		arst_ಬಿಟ್ಟುಗಳು.emplace(ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2());
	std::optional<Const::Builder> srst_ಬಿಟ್ಟುಗಳು;
	ಆದರೆ (has_srst)
		srst_ಬಿಟ್ಟುಗಳು.emplace(ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2());
	std::optional<Const::Builder> init_ಬಿಟ್ಟುಗಳು;
	ಆದರೆ (initvals)
		init_ಬಿಟ್ಟುಗಳು.emplace(ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2());

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i : ಬಿಟ್ಟುಗಳು) {
		res.sig_q.append(sig_q[i]);
		ಆದರೆ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		ಆದರೆ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		ಆದರೆ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_ಗಣ.append(sig_ಗಣ[i]);
		}
		ಆದರೆ (has_arst)
			arst_ಬಿಟ್ಟುಗಳು->ತಳ್ಳು_ಹಿಂಭಾಗ(val_arst[i]);
		ಆದರೆ (has_srst)
			srst_ಬಿಟ್ಟುಗಳು->ತಳ್ಳು_ಹಿಂಭಾಗ(val_srst[i]);
		ಆದರೆ (initvals)
			init_ಬಿಟ್ಟುಗಳು->ತಳ್ಳು_ಹಿಂಭಾಗ(val_init[i]);
	}

	ಆದರೆ (has_arst)
		res.val_arst = arst_ಬಿಟ್ಟುಗಳು->build();
	ಆದರೆ (has_srst)
		res.val_srst = srst_ಬಿಟ್ಟುಗಳು->build();
	ಆದರೆ (initvals)
		res.val_init = init_ಬಿಟ್ಟುಗಳು->build();
	res.ಅಗಲ = Get_ಗಾತ್ರ(res.sig_q);
	ಹಿಂತಿರುಗಿ res;
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::ಕೂಡಿಸು_dummy_ce() {
	ಆದರೆ (has_ce)
		ಹಿಂತಿರುಗಿ;
	has_ce = ನಿಜ;
	pol_ce = ನಿಜ;
	sig_ce = State::S1;
	ce_over_srst = ಸುಳ್ಳು;
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::ಕೂಡಿಸು_dummy_srst() {
	ಆದರೆ (has_srst)
		ಹಿಂತಿರುಗಿ;
	has_srst = ನಿಜ;
	pol_srst = ನಿಜ;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, ಅಗಲ);
	ce_over_srst = ಸುಳ್ಳು;
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::ಕೂಡಿಸು_dummy_arst() {
	ಆದರೆ (has_arst)
		ಹಿಂತಿರುಗಿ;
	has_arst = ನಿಜ;
	pol_arst = ನಿಜ;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, ಅಗಲ);
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::ಕೂಡಿಸು_dummy_aload() {
	ಆದರೆ (has_aload)
		ಹಿಂತಿರುಗಿ;
	has_aload = ನಿಜ;
	pol_aload = ನಿಜ;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, ಅಗಲ);
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::ಕೂಡಿಸು_dummy_sr() {
	ಆದರೆ (has_sr)
		ಹಿಂತಿರುಗಿ;
	has_sr = ನಿಜ;
	pol_clr = ನಿಜ;
	pol_ಗಣ = ನಿಜ;
	sig_clr = Const(State::S0, ಅಗಲ);
	sig_ಗಣ = Const(State::S0, ಅಗಲ);
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::ಕೂಡಿಸು_dummy_clk() {
	ಆದರೆ (has_clk)
		ಹಿಂತಿರುಗಿ;
	has_clk = ನಿಜ;
	pol_clk = ನಿಜ;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, ಅಗಲ);
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::arst_ಗಮ್ಯ_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = ನಿಜ;
	has_arst = ಸುಳ್ಳು;
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::arst_ಗಮ್ಯ_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_ಗಣ = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, ಅಗಲ);
	sig_ಗಣ = Const(pol_arst ? State::S0 : State::S1, ಅಗಲ);
	has_sr = ನಿಜ;
	has_arst = ಸುಳ್ಳು;
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++) {
		ಆದರೆ (val_arst[i] == State::S1)
			sig_ಗಣ[i] = sig_arst;
		ಇಲ್ಲದಿದ್ದರೆ
			sig_clr[i] = sig_arst;
	}
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::aload_ಗಮ್ಯ_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = ನಿಜ;
	has_aload = ಸುಳ್ಳು;
	ಆದರೆ (!is_fine) {
		pol_clr = ಸುಳ್ಳು;
		pol_ಗಣ = ನಿಜ;
		ಆದರೆ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, ಅಗಲ), sig_ad, sig_aload);
			sig_ಗಣ = module->Mux(NEW_ID, Const(State::S0, ಅಗಲ), sig_ad, sig_aload);
		} ಇಲ್ಲದಿದ್ದರೆ {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, ಅಗಲ), sig_aload);
			sig_ಗಣ = module->Mux(NEW_ID, sig_ad, Const(State::S0, ಅಗಲ), sig_aload);
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		pol_clr = pol_aload;
		pol_ಗಣ = pol_aload;
		ಆದರೆ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_ಗಣ = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} ಇಲ್ಲದಿದ್ದರೆ {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_ಗಣ = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::convert_ce_over_srst(ತಾರ್ಕಿಕ val) {
	ಆದರೆ (!has_ce || !has_srst || ce_over_srst == val)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (val) {
		// sdffe to sdffce
		ಆದರೆ (!is_fine) {
			ಆದರೆ (pol_ce) {
				ಆದರೆ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} ಇಲ್ಲದಿದ್ದರೆ {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} ಇಲ್ಲದಿದ್ದರೆ {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (pol_ce) {
				ಆದರೆ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} ಇಲ್ಲದಿದ್ದರೆ {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} ಇಲ್ಲದಿದ್ದರೆ {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		// sdffce to sdffe
		ಆದರೆ (!is_fine) {
			ಆದರೆ (pol_srst) {
				ಆದರೆ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} ಇಲ್ಲದಿದ್ದರೆ {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} ಇಲ್ಲದಿದ್ದರೆ {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (pol_srst) {
				ಆದರೆ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} ಇಲ್ಲದಿದ್ದರೆ {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} ಇಲ್ಲದಿದ್ದರೆ {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::unmap_ce() {
	ಆದರೆ (!has_ce)
		ಹಿಂತಿರುಗಿ;
	log_assert(has_clk);
	ಆದರೆ (has_srst && ce_over_srst)
		unmap_srst();

	ಆದರೆ (!is_fine) {
		ಆದರೆ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		ಇಲ್ಲದಿದ್ದರೆ
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		ಇಲ್ಲದಿದ್ದರೆ
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = ಸುಳ್ಳು;
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::unmap_srst() {
	ಆದರೆ (!has_srst)
		ಹಿಂತಿರುಗಿ;
	ಆದರೆ (has_ce && !ce_over_srst)
		unmap_ce();

	ಆದರೆ (!is_fine) {
		ಆದರೆ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		ಇಲ್ಲದಿದ್ದರೆ
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		ಇಲ್ಲದಿದ್ದರೆ
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = ಸುಳ್ಳು;
}

Cell *Ff_ದತ್ತಾಂಶ::emit() {
	remove();
	ಆದರೆ (!ಅಗಲ)
		ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
	ಆದರೆ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		ಆದರೆ (has_arst) {
			// Convert this case to a D latch.
			arst_ಗಮ್ಯ_aload();
		} ಇಲ್ಲದಿದ್ದರೆ {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
		}
	}
	ಆದರೆ (initvals && !is_anyinit)
		initvals->ಗಣ_init(sig_q, val_init);
	ಆದರೆ (!is_fine) {
		ಆದರೆ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			ಆದರೆ (is_anyinit) {
				cell = module->ಕೂಡಿಸು_Anyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} ಇಲ್ಲದಿದ್ದರೆ {
				cell = module->ಕೂಡಿಸು_Ff(name, sig_d, sig_q);
			}
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->ಕೂಡಿಸು_Sr(name, sig_ಗಣ, sig_clr, sig_q, pol_ಗಣ, pol_clr);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!has_clk) {
			log_assert(!has_srst);
			ಆದರೆ (has_sr)
				cell = module->ಕೂಡಿಸು_Dlatchsr(name, sig_aload, sig_ಗಣ, sig_clr, sig_ad, sig_q, pol_aload, pol_ಗಣ, pol_clr);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_arst)
				cell = module->ಕೂಡಿಸು_Adlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			ಇಲ್ಲದಿದ್ದರೆ
				cell = module->ಕೂಡಿಸು_Dlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (has_sr) {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Dffsre(name, sig_clk, sig_ce, sig_ಗಣ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_ಗಣ, pol_clr);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Dffsr(name, sig_clk, sig_ಗಣ, sig_clr, sig_d, sig_q, pol_clk, pol_ಗಣ, pol_clr);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_arst) {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Adffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Adff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_aload) {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Aldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Aldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_srst) {
				ಆದರೆ (has_ce)
					ಆದರೆ (ce_over_srst)
						cell = module->ಕೂಡಿಸು_Sdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					ಇಲ್ಲದಿದ್ದರೆ
						cell = module->ಕೂಡಿಸು_Sdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Sdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Dffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Dff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} ಇಲ್ಲದಿದ್ದರೆ {
		ಆದರೆ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->ಕೂಡಿಸು_Ff_Gate(name, sig_d, sig_q);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->ಕೂಡಿಸು_Sr_Gate(name, sig_ಗಣ, sig_clr, sig_q, pol_ಗಣ, pol_clr);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!has_clk) {
			log_assert(!has_srst);
			ಆದರೆ (has_sr)
				cell = module->ಕೂಡಿಸು_Dlatchsr_Gate(name, sig_aload, sig_ಗಣ, sig_clr, sig_ad, sig_q, pol_aload, pol_ಗಣ, pol_clr);
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_arst)
				cell = module->ಕೂಡಿಸು_Adlatch_Gate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			ಇಲ್ಲದಿದ್ದರೆ
				cell = module->ಕೂಡಿಸು_Dlatch_Gate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (has_sr) {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Dffsre_Gate(name, sig_clk, sig_ce, sig_ಗಣ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_ಗಣ, pol_clr);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Dffsr_Gate(name, sig_clk, sig_ಗಣ, sig_clr, sig_d, sig_q, pol_clk, pol_ಗಣ, pol_clr);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_arst) {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Adffe_Gate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Adff_Gate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_aload) {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Aldffe_Gate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Aldff_Gate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_srst) {
				ಆದರೆ (has_ce)
					ಆದರೆ (ce_over_srst)
						cell = module->ಕೂಡಿಸು_Sdffce_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					ಇಲ್ಲದಿದ್ದರೆ
						cell = module->ಕೂಡಿಸು_Sdffe_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Sdff_Gate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (has_ce)
					cell = module->ಕೂಡಿಸು_Dffe_Gate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ಇಲ್ಲದಿದ್ದರೆ
					cell = module->ಕೂಡಿಸು_Dff_Gate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->ಗುಣಲಕ್ಷಣಗಳು = ಗುಣಲಕ್ಷಣಗಳು;
	ಹಿಂತಿರುಗಿ cell;
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::remove() {
	ಆದರೆ (cell) {
		remove_init();
		module->remove(cell);
		cell = ಶೂನ್ಯಸೂಚಕ;
	}
}

ನಾಮಾಕಾಶ {
	State invert(State s) {
		ಆಯ್ಕೆ (s) {
			ಸ್ಥಿತಿ State::S0: ಹಿಂತಿರುಗಿ State::S1;
			ಸ್ಥಿತಿ State::S1: ಹಿಂತಿರುಗಿ State::S0;
			ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ s;
		}
	}
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::flip_rst_ಬಿಟ್ಟುಗಳು(ಸ್ಥಿರಾಂಕ pool<ಪೂರ್ಣಾಂಕ> &ಬಿಟ್ಟುಗಳು) {
	ಆದರೆ (!ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ;

	remove_init();

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್: ಬಿಟ್ಟುಗಳು) {
		ಆದರೆ (has_arst)
			val_arst.ಗಣ(ಬಿಟ್, invert(val_arst[ಬಿಟ್]));
		ಆದರೆ (has_srst)
			val_srst.ಗಣ(ಬಿಟ್, invert(val_srst[ಬಿಟ್]));
		val_init.ಗಣ(ಬಿಟ್, invert(val_init[ಬಿಟ್]));
	}
}

ಶೂನ್ಯ Ff_ದತ್ತಾಂಶ::flip_ಬಿಟ್ಟುಗಳು(ಸ್ಥಿರಾಂಕ pool<ಪೂರ್ಣಾಂಕ> &ಬಿಟ್ಟುಗಳು) {
	ಆದರೆ (!ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2())
		ಹಿಂತಿರುಗಿ;

	flip_rst_ಬಿಟ್ಟುಗಳು(ಬಿಟ್ಟುಗಳು);

	Wire *new_q = module->ಕೂಡಿಸು_Wire(NEW_ID, ಅಗಲ);

	ಆದರೆ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	ಆದರೆ (is_fine) {
		ಆದರೆ (has_sr) {
			ತಾರ್ಕಿಕ new_pol_clr = pol_ಗಣ;
			SigSpec new_sig_clr;
			ಆದರೆ (pol_ಗಣ) {
				ಆದರೆ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_ಗಣ, sig_clr);
				} ಇಲ್ಲದಿದ್ದರೆ {
					new_sig_clr = module->AndGate(NEW_ID, sig_ಗಣ, sig_clr);
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_ಗಣ, sig_clr);
				} ಇಲ್ಲದಿದ್ದರೆ {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_ಗಣ, sig_clr);
				}
			}
			pol_ಗಣ = pol_clr;
			sig_ಗಣ = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		ಆದರೆ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		ಆದರೆ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->ಕೂಡಿಸು_Not_Gate(NEW_ID, new_q, sig_q);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
		ಆದರೆ (has_sr) {
			SigSpec not_clr;
			ಆದರೆ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = ನಿಜ;
			} ಇಲ್ಲದಿದ್ದರೆ {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			ಆದರೆ (!pol_ಗಣ) {
				sig_ಗಣ = module->Not(NEW_ID, sig_ಗಣ);
				pol_ಗಣ = ನಿಜ;
			}

			SigSpec masked_ಗಣ = module->And(NEW_ID, sig_ಗಣ, not_clr);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್: ಬಿಟ್ಟುಗಳು) {
				sig_ಗಣ[ಬಿಟ್] = sig_clr[ಬಿಟ್];
				sig_clr[ಬಿಟ್] = masked_ಗಣ[ಬಿಟ್];
			}
		}

		Const ಮುಖವಾಡ = Const(State::S0, ಅಗಲ);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್: ಬಿಟ್ಟುಗಳು)
			ಮುಖವಾಡ.ಗಣ(ಬಿಟ್, State::S1);

		ಆದರೆ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, ಮುಖವಾಡ);
		ಆದರೆ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, ಮುಖವಾಡ);
		module->ಕೂಡಿಸು_Xor(NEW_ID, new_q, ಮುಖವಾಡ, sig_q);
	}

	sig_q = new_q;
}
