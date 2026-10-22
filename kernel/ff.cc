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

#include "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename InputType, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
void manufacture_info(InputType flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr bool have_cell = std::is_same_v<InputType, Cell*>;
	ᤀᤡ᤺ constexpr (std::is_same_v<InputType, IdString>) {
		type = flop;
	} ᤀᤡ᤺ᤆᤡ {
		static_assert(std::is_same_v<InputType, Cell*>);
		cell = flop;
		type = flop->type;
	}
	ᤀᤡ᤺ constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		ᤀᤡ᤺ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	ᤀᤡ᤺ (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		ᤀᤡ᤺ (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = true;
			ᤀᤡ᤺ constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			ᤀᤡ᤺ (type == ID($anyinit)) {
				info.is_anyinit = true;
				ᤀᤡ᤺ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type == ID($sr)) {
			// No data input at all.
		} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = true;
			ᤀᤡ᤺ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} ᤀᤡ᤺ᤆᤡ {
			info.has_clk = true;
			ᤀᤡ᤺ constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		ᤀᤡ᤺ (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = true;
			ᤀᤡ᤺ constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		ᤀᤡ᤺ (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = true;
			ᤀᤡ᤺ constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		ᤀᤡ᤺ (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = true;
			ᤀᤡ᤺ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		ᤀᤡ᤺ (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = true;
			ᤀᤡ᤺ constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_VALUE);
			}
		}
		ᤀᤡ᤺ (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = true;
			ᤀᤡ᤺ constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_VALUE);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type == ID($_FF_)) {
		info.is_fine = true;
		info.has_gclk = true;
		ᤀᤡ᤺ constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = true;
		info.has_sr = true;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[6] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = true;
		info.pol_ce = type_str[8] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = true;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = true;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = true;
		info.pol_ce = type_str[10] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = true;
		info.pol_aload = type_str[9] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = true;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = true;
		info.pol_ce = type_str[11] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = true;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = true;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = true;
		info.pol_ce = type_str[12] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = true;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = true;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = true;
		info.pol_ce = type_str[11] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = true;
		info.has_clk = true;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = true;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = true;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = true;
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = true;
		info.has_aload = true;
		info.has_aload = true;
		info.pol_aload = type_str[9] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = true;
		info.has_aload = true;
		info.has_aload = true;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = true;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = true;
		info.has_aload = true;
		info.has_aload = true;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = true;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		ᤀᤡ᤺ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ᤀᤡ᤺ᤆᤡ {
		log_assert(0);
	}
	ᤀᤡ᤺ constexpr (have_cell)
		ᤀᤡ᤺ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = false;
			info.has_arst = true;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *this, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *this, initvals);
}

FfData FfData::slice(const std::vector<puur_naa_nka> &bits) {
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
	ᤀᤡ᤺ (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	ᤀᤡ᤺ (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	ᤀᤡ᤺ (initvals)
		init_bits.emplace(bits.size());

	for (puur_naa_nka i : bits) {
		res.sig_q.append(sig_q[i]);
		ᤀᤡ᤺ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		ᤀᤡ᤺ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		ᤀᤡ᤺ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		ᤀᤡ᤺ (has_arst)
			arst_bits->push_back(val_arst[i]);
		ᤀᤡ᤺ (has_srst)
			srst_bits->push_back(val_srst[i]);
		ᤀᤡ᤺ (initvals)
			init_bits->push_back(val_init[i]);
	}

	ᤀᤡ᤺ (has_arst)
		res.val_arst = arst_bits->build();
	ᤀᤡ᤺ (has_srst)
		res.val_srst = srst_bits->build();
	ᤀᤡ᤺ (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	ᤀᤡᤴ res;
}

void FfData::add_dummy_ce() {
	ᤀᤡ᤺ (has_ce)
		ᤀᤡᤴ;
	has_ce = true;
	pol_ce = true;
	sig_ce = State::S1;
	ce_over_srst = false;
}

void FfData::add_dummy_srst() {
	ᤀᤡ᤺ (has_srst)
		ᤀᤡᤴ;
	has_srst = true;
	pol_srst = true;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = false;
}

void FfData::add_dummy_arst() {
	ᤀᤡ᤺ (has_arst)
		ᤀᤡᤴ;
	has_arst = true;
	pol_arst = true;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

void FfData::add_dummy_aload() {
	ᤀᤡ᤺ (has_aload)
		ᤀᤡᤴ;
	has_aload = true;
	pol_aload = true;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

void FfData::add_dummy_sr() {
	ᤀᤡ᤺ (has_sr)
		ᤀᤡᤴ;
	has_sr = true;
	pol_clr = true;
	pol_set = true;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

void FfData::add_dummy_clk() {
	ᤀᤡ᤺ (has_clk)
		ᤀᤡᤴ;
	has_clk = true;
	pol_clk = true;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

void FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = true;
	has_arst = false;
}

void FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = true;
	has_arst = false;
	for (puur_naa_nka i = 0; i < width; i++) {
		ᤀᤡ᤺ (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		ᤀᤡ᤺ᤆᤡ
			sig_clr[i] = sig_arst;
	}
}

void FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = true;
	has_aload = false;
	ᤀᤡ᤺ (!is_fine) {
		pol_clr = false;
		pol_set = true;
		ᤀᤡ᤺ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} ᤀᤡ᤺ᤆᤡ {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} ᤀᤡ᤺ᤆᤡ {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		ᤀᤡ᤺ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} ᤀᤡ᤺ᤆᤡ {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

void FfData::convert_ce_over_srst(bool val) {
	ᤀᤡ᤺ (!has_ce || !has_srst || ce_over_srst == val)
		ᤀᤡᤴ;
	ᤀᤡ᤺ (val) {
		// sdffe to sdffce
		ᤀᤡ᤺ (!is_fine) {
			ᤀᤡ᤺ (pol_ce) {
				ᤀᤡ᤺ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} ᤀᤡ᤺ᤆᤡ {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} ᤀᤡ᤺ᤆᤡ {
				ᤀᤡ᤺ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} ᤀᤡ᤺ᤆᤡ {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} ᤀᤡ᤺ᤆᤡ {
			ᤀᤡ᤺ (pol_ce) {
				ᤀᤡ᤺ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} ᤀᤡ᤺ᤆᤡ {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} ᤀᤡ᤺ᤆᤡ {
				ᤀᤡ᤺ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} ᤀᤡ᤺ᤆᤡ {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} ᤀᤡ᤺ᤆᤡ {
		// sdffce to sdffe
		ᤀᤡ᤺ (!is_fine) {
			ᤀᤡ᤺ (pol_srst) {
				ᤀᤡ᤺ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} ᤀᤡ᤺ᤆᤡ {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} ᤀᤡ᤺ᤆᤡ {
				ᤀᤡ᤺ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} ᤀᤡ᤺ᤆᤡ {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} ᤀᤡ᤺ᤆᤡ {
			ᤀᤡ᤺ (pol_srst) {
				ᤀᤡ᤺ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} ᤀᤡ᤺ᤆᤡ {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} ᤀᤡ᤺ᤆᤡ {
				ᤀᤡ᤺ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} ᤀᤡ᤺ᤆᤡ {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

void FfData::unmap_ce() {
	ᤀᤡ᤺ (!has_ce)
		ᤀᤡᤴ;
	log_assert(has_clk);
	ᤀᤡ᤺ (has_srst && ce_over_srst)
		unmap_srst();

	ᤀᤡ᤺ (!is_fine) {
		ᤀᤡ᤺ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		ᤀᤡ᤺ᤆᤡ
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} ᤀᤡ᤺ᤆᤡ {
		ᤀᤡ᤺ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		ᤀᤡ᤺ᤆᤡ
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = false;
}

void FfData::unmap_srst() {
	ᤀᤡ᤺ (!has_srst)
		ᤀᤡᤴ;
	ᤀᤡ᤺ (has_ce && !ce_over_srst)
		unmap_ce();

	ᤀᤡ᤺ (!is_fine) {
		ᤀᤡ᤺ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		ᤀᤡ᤺ᤆᤡ
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} ᤀᤡ᤺ᤆᤡ {
		ᤀᤡ᤺ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		ᤀᤡ᤺ᤆᤡ
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = false;
}

Cell *FfData::emit() {
	remove();
	ᤀᤡ᤺ (!width)
		ᤀᤡᤴ nullptr;
	ᤀᤡ᤺ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		ᤀᤡ᤺ (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} ᤀᤡ᤺ᤆᤡ {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			ᤀᤡᤴ nullptr;
		}
	}
	ᤀᤡ᤺ (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	ᤀᤡ᤺ (!is_fine) {
		ᤀᤡ᤺ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			ᤀᤡ᤺ (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} ᤀᤡ᤺ᤆᤡ {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!has_clk) {
			log_assert(!has_srst);
			ᤀᤡ᤺ (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			ᤀᤡ᤺ᤆᤡ
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ᤀᤡ᤺ᤆᤡ {
			ᤀᤡ᤺ (has_sr) {
				ᤀᤡ᤺ (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_arst) {
				ᤀᤡ᤺ (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_aload) {
				ᤀᤡ᤺ (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_srst) {
				ᤀᤡ᤺ (has_ce)
					ᤀᤡ᤺ (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					ᤀᤡ᤺ᤆᤡ
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} ᤀᤡ᤺ᤆᤡ {
				ᤀᤡ᤺ (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} ᤀᤡ᤺ᤆᤡ {
		ᤀᤡ᤺ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!has_clk) {
			log_assert(!has_srst);
			ᤀᤡ᤺ (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			ᤀᤡ᤺ᤆᤡ
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ᤀᤡ᤺ᤆᤡ {
			ᤀᤡ᤺ (has_sr) {
				ᤀᤡ᤺ (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_arst) {
				ᤀᤡ᤺ (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_aload) {
				ᤀᤡ᤺ (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (has_srst) {
				ᤀᤡ᤺ (has_ce)
					ᤀᤡ᤺ (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					ᤀᤡ᤺ᤆᤡ
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} ᤀᤡ᤺ᤆᤡ {
				ᤀᤡ᤺ (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ᤀᤡ᤺ᤆᤡ
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	ᤀᤡᤴ cell;
}

void FfData::remove() {
	ᤀᤡ᤺ (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

namespace {
	State invert(State s) {
		switch (s) {
			pak_sa State::S0: ᤀᤡᤴ State::S1;
			pak_sa State::S1: ᤀᤡᤴ State::S0;
			default: ᤀᤡᤴ s;
		}
	}
}

void FfData::flip_rst_bits(const pool<puur_naa_nka> &bits) {
	ᤀᤡ᤺ (!bits.size())
		ᤀᤡᤴ;

	remove_init();

	for (auto bit: bits) {
		ᤀᤡ᤺ (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		ᤀᤡ᤺ (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

void FfData::flip_bits(const pool<puur_naa_nka> &bits) {
	ᤀᤡ᤺ (!bits.size())
		ᤀᤡᤴ;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	ᤀᤡ᤺ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	ᤀᤡ᤺ (is_fine) {
		ᤀᤡ᤺ (has_sr) {
			bool new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			ᤀᤡ᤺ (pol_set) {
				ᤀᤡ᤺ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} ᤀᤡ᤺ᤆᤡ {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} ᤀᤡ᤺ᤆᤡ {
				ᤀᤡ᤺ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} ᤀᤡ᤺ᤆᤡ {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		ᤀᤡ᤺ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		ᤀᤡ᤺ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	ᤀᤡ᤺ᤆᤡ
	{
		ᤀᤡ᤺ (has_sr) {
			SigSpec not_clr;
			ᤀᤡ᤺ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = true;
			} ᤀᤡ᤺ᤆᤡ {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			ᤀᤡ᤺ (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = true;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			for (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		for (auto bit: bits)
			mask.set(bit, State::S1);

		ᤀᤡ᤺ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		ᤀᤡ᤺ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
