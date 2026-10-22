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

#ଅନ୍ତର୍ଭାଵ "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename ନିବେଶ_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
ଶୂନ୍ୟ manufacture_info(ନିବେଶ_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr ତାର୍କିକ have_cell = std::is_same_v<ନିବେଶ_Type, Cell*>;
	ଯଦି constexpr (std::is_same_v<ନିବେଶ_Type, IdString>) {
		type = flop;
	} ଅନ୍ୟଥା {
		static_assert(std::is_same_v<ନିବେଶ_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	ଯଦି constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		ଯଦି (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	ଯଦି (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		ଯଦି (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = ସତ୍ୟ;
			ଯଦି constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			ଯଦି (type == ID($anyinit)) {
				info.is_anyinit = ସତ୍ୟ;
				ଯଦି constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} ଅନ୍ୟଥା ଯଦି (type == ID($sr)) {
			// No data input at all.
		} ଅନ୍ୟଥା ଯଦି (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = ସତ୍ୟ;
			ଯଦି constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} ଅନ୍ୟଥା {
			info.has_clk = ସତ୍ୟ;
			ଯଦି constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		ଯଦି (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = ସତ୍ୟ;
			ଯଦି constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		ଯଦି (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = ସତ୍ୟ;
			ଯଦି constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		ଯଦି (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = ସତ୍ୟ;
			ଯଦି constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		ଯଦି (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = ସତ୍ୟ;
			ଯଦି constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_ମୂଲ୍ୟ);
			}
		}
		ଯଦି (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = ସତ୍ୟ;
			ଯଦି constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_ମୂଲ୍ୟ);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} ଅନ୍ୟଥା ଯଦି (type == ID($_FF_)) {
		info.is_fine = ସତ୍ୟ;
		info.has_gclk = ସତ୍ୟ;
		ଯଦି constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = ସତ୍ୟ;
		info.has_sr = ସତ୍ୟ;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[6] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = ସତ୍ୟ;
		info.pol_ce = type_str[8] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = ସତ୍ୟ;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = ସତ୍ୟ;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = ସତ୍ୟ;
		info.pol_ce = type_str[10] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = ସତ୍ୟ;
		info.pol_aload = type_str[9] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = ସତ୍ୟ;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = ସତ୍ୟ;
		info.pol_ce = type_str[11] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = ସତ୍ୟ;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = ସତ୍ୟ;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = ସତ୍ୟ;
		info.pol_ce = type_str[12] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = ସତ୍ୟ;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = ସତ୍ୟ;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = ସତ୍ୟ;
		info.pol_ce = type_str[11] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = ସତ୍ୟ;
		info.has_clk = ସତ୍ୟ;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = ସତ୍ୟ;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = ସତ୍ୟ;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = ସତ୍ୟ;
		ଯଦି constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = ସତ୍ୟ;
		info.has_aload = ସତ୍ୟ;
		info.has_aload = ସତ୍ୟ;
		info.pol_aload = type_str[9] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = ସତ୍ୟ;
		info.has_aload = ସତ୍ୟ;
		info.has_aload = ସତ୍ୟ;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = ସତ୍ୟ;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		ଯଦି constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ଅନ୍ୟଥା ଯଦି (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = ସତ୍ୟ;
		info.has_aload = ସତ୍ୟ;
		info.has_aload = ସତ୍ୟ;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = ସତ୍ୟ;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		ଯଦି constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ଅନ୍ୟଥା {
		log_assert(0);
	}
	ଯଦି constexpr (have_cell)
		ଯଦି (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = ମିଥ୍ୟା;
			info.has_arst = ସତ୍ୟ;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *ଏହି, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *ଏହି, initvals);
}

FfData FfData::slice(ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &bits) {
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
	ଯଦି (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	ଯଦି (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	ଯଦି (initvals)
		init_bits.emplace(bits.size());

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i : bits) {
		res.sig_q.append(sig_q[i]);
		ଯଦି (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		ଯଦି (has_aload)
			res.sig_ad.append(sig_ad[i]);
		ଯଦି (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		ଯଦି (has_arst)
			arst_bits->ଠେଲ_back(val_arst[i]);
		ଯଦି (has_srst)
			srst_bits->ଠେଲ_back(val_srst[i]);
		ଯଦି (initvals)
			init_bits->ଠେଲ_back(val_init[i]);
	}

	ଯଦି (has_arst)
		res.val_arst = arst_bits->build();
	ଯଦି (has_srst)
		res.val_srst = srst_bits->build();
	ଯଦି (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	ଫେରନ୍ତୁ res;
}

ଶୂନ୍ୟ FfData::add_dummy_ce() {
	ଯଦି (has_ce)
		ଫେରନ୍ତୁ;
	has_ce = ସତ୍ୟ;
	pol_ce = ସତ୍ୟ;
	sig_ce = State::S1;
	ce_over_srst = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ FfData::add_dummy_srst() {
	ଯଦି (has_srst)
		ଫେରନ୍ତୁ;
	has_srst = ସତ୍ୟ;
	pol_srst = ସତ୍ୟ;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ FfData::add_dummy_arst() {
	ଯଦି (has_arst)
		ଫେରନ୍ତୁ;
	has_arst = ସତ୍ୟ;
	pol_arst = ସତ୍ୟ;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

ଶୂନ୍ୟ FfData::add_dummy_aload() {
	ଯଦି (has_aload)
		ଫେରନ୍ତୁ;
	has_aload = ସତ୍ୟ;
	pol_aload = ସତ୍ୟ;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

ଶୂନ୍ୟ FfData::add_dummy_sr() {
	ଯଦି (has_sr)
		ଫେରନ୍ତୁ;
	has_sr = ସତ୍ୟ;
	pol_clr = ସତ୍ୟ;
	pol_set = ସତ୍ୟ;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

ଶୂନ୍ୟ FfData::add_dummy_clk() {
	ଯଦି (has_clk)
		ଫେରନ୍ତୁ;
	has_clk = ସତ୍ୟ;
	pol_clk = ସତ୍ୟ;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

ଶୂନ୍ୟ FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = ସତ୍ୟ;
	has_arst = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = ସତ୍ୟ;
	has_arst = ମିଥ୍ୟା;
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++) {
		ଯଦି (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		ଅନ୍ୟଥା
			sig_clr[i] = sig_arst;
	}
}

ଶୂନ୍ୟ FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = ସତ୍ୟ;
	has_aload = ମିଥ୍ୟା;
	ଯଦି (!is_fine) {
		pol_clr = ମିଥ୍ୟା;
		pol_set = ସତ୍ୟ;
		ଯଦି (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} ଅନ୍ୟଥା {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} ଅନ୍ୟଥା {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		ଯଦି (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} ଅନ୍ୟଥା {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

ଶୂନ୍ୟ FfData::convert_ce_over_srst(ତାର୍କିକ val) {
	ଯଦି (!has_ce || !has_srst || ce_over_srst == val)
		ଫେରନ୍ତୁ;
	ଯଦି (val) {
		// sdffe to sdffce
		ଯଦି (!is_fine) {
			ଯଦି (pol_ce) {
				ଯଦି (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} ଅନ୍ୟଥା {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} ଅନ୍ୟଥା {
				ଯଦି (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} ଅନ୍ୟଥା {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} ଅନ୍ୟଥା {
			ଯଦି (pol_ce) {
				ଯଦି (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} ଅନ୍ୟଥା {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} ଅନ୍ୟଥା {
				ଯଦି (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} ଅନ୍ୟଥା {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} ଅନ୍ୟଥା {
		// sdffce to sdffe
		ଯଦି (!is_fine) {
			ଯଦି (pol_srst) {
				ଯଦି (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} ଅନ୍ୟଥା {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} ଅନ୍ୟଥା {
				ଯଦି (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} ଅନ୍ୟଥା {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} ଅନ୍ୟଥା {
			ଯଦି (pol_srst) {
				ଯଦି (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} ଅନ୍ୟଥା {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} ଅନ୍ୟଥା {
				ଯଦି (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} ଅନ୍ୟଥା {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

ଶୂନ୍ୟ FfData::unmap_ce() {
	ଯଦି (!has_ce)
		ଫେରନ୍ତୁ;
	log_assert(has_clk);
	ଯଦି (has_srst && ce_over_srst)
		unmap_srst();

	ଯଦି (!is_fine) {
		ଯଦି (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		ଅନ୍ୟଥା
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} ଅନ୍ୟଥା {
		ଯଦି (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		ଅନ୍ୟଥା
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = ମିଥ୍ୟା;
}

ଶୂନ୍ୟ FfData::unmap_srst() {
	ଯଦି (!has_srst)
		ଫେରନ୍ତୁ;
	ଯଦି (has_ce && !ce_over_srst)
		unmap_ce();

	ଯଦି (!is_fine) {
		ଯଦି (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		ଅନ୍ୟଥା
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} ଅନ୍ୟଥା {
		ଯଦି (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		ଅନ୍ୟଥା
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = ମିଥ୍ୟା;
}

Cell *FfData::emit() {
	remove();
	ଯଦି (!width)
		ଫେରନ୍ତୁ nullptr;
	ଯଦି (!has_aload && !has_clk && !has_gclk && !has_sr) {
		ଯଦି (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} ଅନ୍ୟଥା {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			ଫେରନ୍ତୁ nullptr;
		}
	}
	ଯଦି (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	ଯଦି (!is_fine) {
		ଯଦି (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			ଯଦି (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} ଅନ୍ୟଥା {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} ଅନ୍ୟଥା ଯଦି (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ଅନ୍ୟଥା ଯଦି (!has_clk) {
			log_assert(!has_srst);
			ଯଦି (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ଅନ୍ୟଥା ଯଦି (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			ଅନ୍ୟଥା
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ଅନ୍ୟଥା {
			ଯଦି (has_sr) {
				ଯଦି (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ଅନ୍ୟଥା
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ଅନ୍ୟଥା ଯଦି (has_arst) {
				ଯଦି (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				ଅନ୍ୟଥା
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} ଅନ୍ୟଥା ଯଦି (has_aload) {
				ଯଦି (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ଅନ୍ୟଥା
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ଅନ୍ୟଥା ଯଦି (has_srst) {
				ଯଦି (has_ce)
					ଯଦି (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					ଅନ୍ୟଥା
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				ଅନ୍ୟଥା
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} ଅନ୍ୟଥା {
				ଯଦି (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ଅନ୍ୟଥା
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} ଅନ୍ୟଥା {
		ଯଦି (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} ଅନ୍ୟଥା ଯଦି (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ଅନ୍ୟଥା ଯଦି (!has_clk) {
			log_assert(!has_srst);
			ଯଦି (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ଅନ୍ୟଥା ଯଦି (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			ଅନ୍ୟଥା
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ଅନ୍ୟଥା {
			ଯଦି (has_sr) {
				ଯଦି (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ଅନ୍ୟଥା
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ଅନ୍ୟଥା ଯଦି (has_arst) {
				ଯଦି (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				ଅନ୍ୟଥା
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} ଅନ୍ୟଥା ଯଦି (has_aload) {
				ଯଦି (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ଅନ୍ୟଥା
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ଅନ୍ୟଥା ଯଦି (has_srst) {
				ଯଦି (has_ce)
					ଯଦି (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					ଅନ୍ୟଥା
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				ଅନ୍ୟଥା
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} ଅନ୍ୟଥା {
				ଯଦି (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ଅନ୍ୟଥା
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	ଫେରନ୍ତୁ cell;
}

ଶୂନ୍ୟ FfData::remove() {
	ଯଦି (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

ନାମାକାଶ {
	State invert(State s) {
		ବାଛ (s) {
			ସ୍ଥିତି State::S0: ଫେରନ୍ତୁ State::S1;
			ସ୍ଥିତି State::S1: ଫେରନ୍ତୁ State::S0;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ s;
		}
	}
}

ଶୂନ୍ୟ FfData::flip_rst_bits(ଅଚଳ pool<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &bits) {
	ଯଦି (!bits.size())
		ଫେରନ୍ତୁ;

	remove_init();

	ପାଇଁ (auto bit: bits) {
		ଯଦି (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		ଯଦି (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

ଶୂନ୍ୟ FfData::flip_bits(ଅଚଳ pool<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &bits) {
	ଯଦି (!bits.size())
		ଫେରନ୍ତୁ;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	ଯଦି (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	ଯଦି (is_fine) {
		ଯଦି (has_sr) {
			ତାର୍କିକ new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			ଯଦି (pol_set) {
				ଯଦି (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} ଅନ୍ୟଥା {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} ଅନ୍ୟଥା {
				ଯଦି (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} ଅନ୍ୟଥା {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		ଯଦି (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		ଯଦି (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	ଅନ୍ୟଥା
	{
		ଯଦି (has_sr) {
			SigSpec not_clr;
			ଯଦି (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = ସତ୍ୟ;
			} ଅନ୍ୟଥା {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			ଯଦି (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = ସତ୍ୟ;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			ପାଇଁ (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		ପାଇଁ (auto bit: bits)
			mask.set(bit, State::S1);

		ଯଦି (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		ଯଦି (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
