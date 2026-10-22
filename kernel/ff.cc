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
template<typename ಒಳಹರಿವು_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
ಶೂನ್ಯ manufacture_info(ಒಳಹರಿವು_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr ತಾರ್ಕಿಕ have_cell = std::is_same_v<ಒಳಹರಿವು_Type, Cell*>;
	ಯದಿ constexpr (std::is_same_v<ಒಳಹರಿವು_Type, IdString>) {
		type = flop;
	} ಅನ್ಯಥಾ {
		static_assert(std::is_same_v<ಒಳಹರಿವು_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	ಯದಿ constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		ಯದಿ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	ಯದಿ (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		ಯದಿ (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			ಯದಿ (type == ID($anyinit)) {
				info.is_anyinit = ಸತ್ಯ;
				ಯದಿ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} ಅನ್ಯಥಾ ಯದಿ (type == ID($sr)) {
			// No data input at all.
		} ಅನ್ಯಥಾ ಯದಿ (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} ಅನ್ಯಥಾ {
			info.has_clk = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		ಯದಿ (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		ಯದಿ (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		ಯದಿ (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		ಯದಿ (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_ಮೌಲ್ಯ);
			}
		}
		ಯದಿ (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = ಸತ್ಯ;
			ಯದಿ constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_ಮೌಲ್ಯ);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type == ID($_FF_)) {
		info.is_fine = ಸತ್ಯ;
		info.has_gclk = ಸತ್ಯ;
		ಯದಿ constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = ಸತ್ಯ;
		info.has_sr = ಸತ್ಯ;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[6] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = ಸತ್ಯ;
		info.pol_ce = type_str[8] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = ಸತ್ಯ;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = ಸತ್ಯ;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = ಸತ್ಯ;
		info.pol_ce = type_str[10] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = ಸತ್ಯ;
		info.pol_aload = type_str[9] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = ಸತ್ಯ;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = ಸತ್ಯ;
		info.pol_ce = type_str[11] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = ಸತ್ಯ;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = ಸತ್ಯ;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = ಸತ್ಯ;
		info.pol_ce = type_str[12] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = ಸತ್ಯ;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = ಸತ್ಯ;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = ಸತ್ಯ;
		info.pol_ce = type_str[11] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = ಸತ್ಯ;
		info.has_clk = ಸತ್ಯ;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = ಸತ್ಯ;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = ಸತ್ಯ;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = ಸತ್ಯ;
		ಯದಿ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = ಸತ್ಯ;
		info.has_aload = ಸತ್ಯ;
		info.has_aload = ಸತ್ಯ;
		info.pol_aload = type_str[9] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = ಸತ್ಯ;
		info.has_aload = ಸತ್ಯ;
		info.has_aload = ಸತ್ಯ;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = ಸತ್ಯ;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		ಯದಿ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ಅನ್ಯಥಾ ಯದಿ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = ಸತ್ಯ;
		info.has_aload = ಸತ್ಯ;
		info.has_aload = ಸತ್ಯ;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = ಸತ್ಯ;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		ಯದಿ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ಅನ್ಯಥಾ {
		log_assert(0);
	}
	ಯದಿ constexpr (have_cell)
		ಯದಿ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = ಅಸತ್ಯ;
			info.has_arst = ಸತ್ಯ;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *ಏತತ್, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *ಏತತ್, initvals);
}

FfData FfData::slice(ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &bits) {
	FfData res(module, initvals, module->twines().add(NEW_ID));
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
	res.pol_set = pol_set;
	res.attributes = attributes;
	std::optional<Const::Builder> arst_bits;
	ಯದಿ (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	ಯದಿ (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	ಯದಿ (initvals)
		init_bits.emplace(bits.size());

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i : bits) {
		res.sig_q.append(sig_q[i]);
		ಯದಿ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		ಯದಿ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		ಯದಿ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		ಯದಿ (has_arst)
			arst_bits->ತಳ್ಳು_back(val_arst[i]);
		ಯದಿ (has_srst)
			srst_bits->ತಳ್ಳು_back(val_srst[i]);
		ಯದಿ (initvals)
			init_bits->ತಳ್ಳು_back(val_init[i]);
	}

	ಯದಿ (has_arst)
		res.val_arst = arst_bits->build();
	ಯದಿ (has_srst)
		res.val_srst = srst_bits->build();
	ಯದಿ (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	ಪ್ರತ್ಯಾಗಮ res;
}

ಶೂನ್ಯ FfData::add_dummy_ce() {
	ಯದಿ (has_ce)
		ಪ್ರತ್ಯಾಗಮ;
	has_ce = ಸತ್ಯ;
	pol_ce = ಸತ್ಯ;
	sig_ce = State::S1;
	ce_over_srst = ಅಸತ್ಯ;
}

ಶೂನ್ಯ FfData::add_dummy_srst() {
	ಯದಿ (has_srst)
		ಪ್ರತ್ಯಾಗಮ;
	has_srst = ಸತ್ಯ;
	pol_srst = ಸತ್ಯ;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = ಅಸತ್ಯ;
}

ಶೂನ್ಯ FfData::add_dummy_arst() {
	ಯದಿ (has_arst)
		ಪ್ರತ್ಯಾಗಮ;
	has_arst = ಸತ್ಯ;
	pol_arst = ಸತ್ಯ;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

ಶೂನ್ಯ FfData::add_dummy_aload() {
	ಯದಿ (has_aload)
		ಪ್ರತ್ಯಾಗಮ;
	has_aload = ಸತ್ಯ;
	pol_aload = ಸತ್ಯ;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

ಶೂನ್ಯ FfData::add_dummy_sr() {
	ಯದಿ (has_sr)
		ಪ್ರತ್ಯಾಗಮ;
	has_sr = ಸತ್ಯ;
	pol_clr = ಸತ್ಯ;
	pol_set = ಸತ್ಯ;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

ಶೂನ್ಯ FfData::add_dummy_clk() {
	ಯದಿ (has_clk)
		ಪ್ರತ್ಯಾಗಮ;
	has_clk = ಸತ್ಯ;
	pol_clk = ಸತ್ಯ;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

ಶೂನ್ಯ FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = ಸತ್ಯ;
	has_arst = ಅಸತ್ಯ;
}

ಶೂನ್ಯ FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = ಸತ್ಯ;
	has_arst = ಅಸತ್ಯ;
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++) {
		ಯದಿ (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		ಅನ್ಯಥಾ
			sig_clr[i] = sig_arst;
	}
}

ಶೂನ್ಯ FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = ಸತ್ಯ;
	has_aload = ಅಸತ್ಯ;
	ಯದಿ (!is_fine) {
		pol_clr = ಅಸತ್ಯ;
		pol_set = ಸತ್ಯ;
		ಯದಿ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} ಅನ್ಯಥಾ {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} ಅನ್ಯಥಾ {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		ಯದಿ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} ಅನ್ಯಥಾ {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

ಶೂನ್ಯ FfData::convert_ce_over_srst(ತಾರ್ಕಿಕ val) {
	ಯದಿ (!has_ce || !has_srst || ce_over_srst == val)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (val) {
		// sdffe to sdffce
		ಯದಿ (!is_fine) {
			ಯದಿ (pol_ce) {
				ಯದಿ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} ಅನ್ಯಥಾ {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} ಅನ್ಯಥಾ {
				ಯದಿ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} ಅನ್ಯಥಾ {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} ಅನ್ಯಥಾ {
			ಯದಿ (pol_ce) {
				ಯದಿ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} ಅನ್ಯಥಾ {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} ಅನ್ಯಥಾ {
				ಯದಿ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} ಅನ್ಯಥಾ {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} ಅನ್ಯಥಾ {
		// sdffce to sdffe
		ಯದಿ (!is_fine) {
			ಯದಿ (pol_srst) {
				ಯದಿ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} ಅನ್ಯಥಾ {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} ಅನ್ಯಥಾ {
				ಯದಿ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} ಅನ್ಯಥಾ {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} ಅನ್ಯಥಾ {
			ಯದಿ (pol_srst) {
				ಯದಿ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} ಅನ್ಯಥಾ {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} ಅನ್ಯಥಾ {
				ಯದಿ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} ಅನ್ಯಥಾ {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

ಶೂನ್ಯ FfData::unmap_ce() {
	ಯದಿ (!has_ce)
		ಪ್ರತ್ಯಾಗಮ;
	log_assert(has_clk);
	ಯದಿ (has_srst && ce_over_srst)
		unmap_srst();

	ಯದಿ (!is_fine) {
		ಯದಿ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		ಅನ್ಯಥಾ
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} ಅನ್ಯಥಾ {
		ಯದಿ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		ಅನ್ಯಥಾ
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = ಅಸತ್ಯ;
}

ಶೂನ್ಯ FfData::unmap_srst() {
	ಯದಿ (!has_srst)
		ಪ್ರತ್ಯಾಗಮ;
	ಯದಿ (has_ce && !ce_over_srst)
		unmap_ce();

	ಯದಿ (!is_fine) {
		ಯದಿ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		ಅನ್ಯಥಾ
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} ಅನ್ಯಥಾ {
		ಯದಿ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		ಅನ್ಯಥಾ
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = ಅಸತ್ಯ;
}

Cell *FfData::emit() {
	remove();
	ಯದಿ (!width)
		ಪ್ರತ್ಯಾಗಮ nullptr;
	ಯದಿ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		ಯದಿ (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} ಅನ್ಯಥಾ {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			ಪ್ರತ್ಯಾಗಮ nullptr;
		}
	}
	ಯದಿ (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	ಯದಿ (!is_fine) {
		ಯದಿ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			ಯದಿ (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} ಅನ್ಯಥಾ {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} ಅನ್ಯಥಾ ಯದಿ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ಅನ್ಯಥಾ ಯದಿ (!has_clk) {
			log_assert(!has_srst);
			ಯದಿ (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ಅನ್ಯಥಾ ಯದಿ (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			ಅನ್ಯಥಾ
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ಅನ್ಯಥಾ {
			ಯದಿ (has_sr) {
				ಯದಿ (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ಅನ್ಯಥಾ
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ಅನ್ಯಥಾ ಯದಿ (has_arst) {
				ಯದಿ (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				ಅನ್ಯಥಾ
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} ಅನ್ಯಥಾ ಯದಿ (has_aload) {
				ಯದಿ (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ಅನ್ಯಥಾ
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ಅನ್ಯಥಾ ಯದಿ (has_srst) {
				ಯದಿ (has_ce)
					ಯದಿ (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					ಅನ್ಯಥಾ
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				ಅನ್ಯಥಾ
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} ಅನ್ಯಥಾ {
				ಯದಿ (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ಅನ್ಯಥಾ
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} ಅನ್ಯಥಾ {
		ಯದಿ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} ಅನ್ಯಥಾ ಯದಿ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ಅನ್ಯಥಾ ಯದಿ (!has_clk) {
			log_assert(!has_srst);
			ಯದಿ (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ಅನ್ಯಥಾ ಯದಿ (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			ಅನ್ಯಥಾ
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ಅನ್ಯಥಾ {
			ಯದಿ (has_sr) {
				ಯದಿ (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ಅನ್ಯಥಾ
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ಅನ್ಯಥಾ ಯದಿ (has_arst) {
				ಯದಿ (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				ಅನ್ಯಥಾ
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} ಅನ್ಯಥಾ ಯದಿ (has_aload) {
				ಯದಿ (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ಅನ್ಯಥಾ
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ಅನ್ಯಥಾ ಯದಿ (has_srst) {
				ಯದಿ (has_ce)
					ಯದಿ (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					ಅನ್ಯಥಾ
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				ಅನ್ಯಥಾ
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} ಅನ್ಯಥಾ {
				ಯದಿ (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ಅನ್ಯಥಾ
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	ಪ್ರತ್ಯಾಗಮ cell;
}

ಶೂನ್ಯ FfData::remove() {
	ಯದಿ (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

ನಾಮಾಕಾಶ {
	State invert(State s) {
		ವಿಕಲ್ಪ (s) {
			ಪಕ್ಷ State::S0: ಪ್ರತ್ಯಾಗಮ State::S1;
			ಪಕ್ಷ State::S1: ಪ್ರತ್ಯಾಗಮ State::S0;
			ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ s;
		}
	}
}

ಶೂನ್ಯ FfData::flip_rst_bits(ಧ್ರುವ pool<ಪೂರ್ಣಾಙ್ಕ> &bits) {
	ಯದಿ (!bits.size())
		ಪ್ರತ್ಯಾಗಮ;

	remove_init();

	ಪ್ರತಿ (auto bit: bits) {
		ಯದಿ (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		ಯದಿ (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

ಶೂನ್ಯ FfData::flip_bits(ಧ್ರುವ pool<ಪೂರ್ಣಾಙ್ಕ> &bits) {
	ಯದಿ (!bits.size())
		ಪ್ರತ್ಯಾಗಮ;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	ಯದಿ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	ಯದಿ (is_fine) {
		ಯದಿ (has_sr) {
			ತಾರ್ಕಿಕ new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			ಯದಿ (pol_set) {
				ಯದಿ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} ಅನ್ಯಥಾ {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} ಅನ್ಯಥಾ {
				ಯದಿ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} ಅನ್ಯಥಾ {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		ಯದಿ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		ಯದಿ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	ಅನ್ಯಥಾ
	{
		ಯದಿ (has_sr) {
			SigSpec not_clr;
			ಯದಿ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = ಸತ್ಯ;
			} ಅನ್ಯಥಾ {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			ಯದಿ (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = ಸತ್ಯ;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			ಪ್ರತಿ (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		ಪ್ರತಿ (auto bit: bits)
			mask.set(bit, State::S1);

		ಯದಿ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		ಯದಿ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
