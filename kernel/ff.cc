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

#അന്തര്ഭാവ "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename നിവേശം_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
ശൂന്യം manufacture_info(നിവേശം_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr തർക്കം have_cell = std::is_same_v<നിവേശം_Type, Cell*>;
	എങ്കിൽ constexpr (std::is_same_v<നിവേശം_Type, IdString>) {
		type = flop;
	} അല്ലെങ്കിൽ {
		static_assert(std::is_same_v<നിവേശം_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	എങ്കിൽ constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		എങ്കിൽ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	എങ്കിൽ (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		എങ്കിൽ (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = സത്യം;
			എങ്കിൽ constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			എങ്കിൽ (type == ID($anyinit)) {
				info.is_anyinit = സത്യം;
				എങ്കിൽ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} അല്ലെങ്കിൽ എങ്കിൽ (type == ID($sr)) {
			// No data input at all.
		} അല്ലെങ്കിൽ എങ്കിൽ (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = സത്യം;
			എങ്കിൽ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} അല്ലെങ്കിൽ {
			info.has_clk = സത്യം;
			എങ്കിൽ constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		എങ്കിൽ (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = സത്യം;
			എങ്കിൽ constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		എങ്കിൽ (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = സത്യം;
			എങ്കിൽ constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		എങ്കിൽ (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = സത്യം;
			എങ്കിൽ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		എങ്കിൽ (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = സത്യം;
			എങ്കിൽ constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_മൂല്യം);
			}
		}
		എങ്കിൽ (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = സത്യം;
			എങ്കിൽ constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_മൂല്യം);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type == ID($_FF_)) {
		info.is_fine = സത്യം;
		info.has_gclk = സത്യം;
		എങ്കിൽ constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = സത്യം;
		info.has_sr = സത്യം;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[6] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = സത്യം;
		info.pol_ce = type_str[8] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = സത്യം;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = സത്യം;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = സത്യം;
		info.pol_ce = type_str[10] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = സത്യം;
		info.pol_aload = type_str[9] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = സത്യം;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = സത്യം;
		info.pol_ce = type_str[11] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = സത്യം;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = സത്യം;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = സത്യം;
		info.pol_ce = type_str[12] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = സത്യം;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = സത്യം;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = സത്യം;
		info.pol_ce = type_str[11] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = സത്യം;
		info.has_clk = സത്യം;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = സത്യം;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = സത്യം;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = സത്യം;
		എങ്കിൽ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = സത്യം;
		info.has_aload = സത്യം;
		info.has_aload = സത്യം;
		info.pol_aload = type_str[9] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = സത്യം;
		info.has_aload = സത്യം;
		info.has_aload = സത്യം;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = സത്യം;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		എങ്കിൽ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} അല്ലെങ്കിൽ എങ്കിൽ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = സത്യം;
		info.has_aload = സത്യം;
		info.has_aload = സത്യം;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = സത്യം;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		എങ്കിൽ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} അല്ലെങ്കിൽ {
		log_assert(0);
	}
	എങ്കിൽ constexpr (have_cell)
		എങ്കിൽ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = അസത്യം;
			info.has_arst = സത്യം;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *ഇത്, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *ഇത്, initvals);
}

FfData FfData::slice(സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &bits) {
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
	എങ്കിൽ (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	എങ്കിൽ (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	എങ്കിൽ (initvals)
		init_bits.emplace(bits.size());

	വേണ്ടി (പൂർണ്ണസംഖ്യ i : bits) {
		res.sig_q.append(sig_q[i]);
		എങ്കിൽ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		എങ്കിൽ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		എങ്കിൽ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		എങ്കിൽ (has_arst)
			arst_bits->തള്ളുക_back(val_arst[i]);
		എങ്കിൽ (has_srst)
			srst_bits->തള്ളുക_back(val_srst[i]);
		എങ്കിൽ (initvals)
			init_bits->തള്ളുക_back(val_init[i]);
	}

	എങ്കിൽ (has_arst)
		res.val_arst = arst_bits->build();
	എങ്കിൽ (has_srst)
		res.val_srst = srst_bits->build();
	എങ്കിൽ (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	തിരികെ res;
}

ശൂന്യം FfData::add_dummy_ce() {
	എങ്കിൽ (has_ce)
		തിരികെ;
	has_ce = സത്യം;
	pol_ce = സത്യം;
	sig_ce = State::S1;
	ce_over_srst = അസത്യം;
}

ശൂന്യം FfData::add_dummy_srst() {
	എങ്കിൽ (has_srst)
		തിരികെ;
	has_srst = സത്യം;
	pol_srst = സത്യം;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = അസത്യം;
}

ശൂന്യം FfData::add_dummy_arst() {
	എങ്കിൽ (has_arst)
		തിരികെ;
	has_arst = സത്യം;
	pol_arst = സത്യം;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

ശൂന്യം FfData::add_dummy_aload() {
	എങ്കിൽ (has_aload)
		തിരികെ;
	has_aload = സത്യം;
	pol_aload = സത്യം;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

ശൂന്യം FfData::add_dummy_sr() {
	എങ്കിൽ (has_sr)
		തിരികെ;
	has_sr = സത്യം;
	pol_clr = സത്യം;
	pol_set = സത്യം;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

ശൂന്യം FfData::add_dummy_clk() {
	എങ്കിൽ (has_clk)
		തിരികെ;
	has_clk = സത്യം;
	pol_clk = സത്യം;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

ശൂന്യം FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = സത്യം;
	has_arst = അസത്യം;
}

ശൂന്യം FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = സത്യം;
	has_arst = അസത്യം;
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++) {
		എങ്കിൽ (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		അല്ലെങ്കിൽ
			sig_clr[i] = sig_arst;
	}
}

ശൂന്യം FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = സത്യം;
	has_aload = അസത്യം;
	എങ്കിൽ (!is_fine) {
		pol_clr = അസത്യം;
		pol_set = സത്യം;
		എങ്കിൽ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} അല്ലെങ്കിൽ {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} അല്ലെങ്കിൽ {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		എങ്കിൽ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} അല്ലെങ്കിൽ {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

ശൂന്യം FfData::convert_ce_over_srst(തർക്കം val) {
	എങ്കിൽ (!has_ce || !has_srst || ce_over_srst == val)
		തിരികെ;
	എങ്കിൽ (val) {
		// sdffe to sdffce
		എങ്കിൽ (!is_fine) {
			എങ്കിൽ (pol_ce) {
				എങ്കിൽ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} അല്ലെങ്കിൽ {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} അല്ലെങ്കിൽ {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (pol_ce) {
				എങ്കിൽ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} അല്ലെങ്കിൽ {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} അല്ലെങ്കിൽ {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} അല്ലെങ്കിൽ {
		// sdffce to sdffe
		എങ്കിൽ (!is_fine) {
			എങ്കിൽ (pol_srst) {
				എങ്കിൽ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} അല്ലെങ്കിൽ {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} അല്ലെങ്കിൽ {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (pol_srst) {
				എങ്കിൽ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} അല്ലെങ്കിൽ {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} അല്ലെങ്കിൽ {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

ശൂന്യം FfData::unmap_ce() {
	എങ്കിൽ (!has_ce)
		തിരികെ;
	log_assert(has_clk);
	എങ്കിൽ (has_srst && ce_over_srst)
		unmap_srst();

	എങ്കിൽ (!is_fine) {
		എങ്കിൽ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		അല്ലെങ്കിൽ
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		അല്ലെങ്കിൽ
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = അസത്യം;
}

ശൂന്യം FfData::unmap_srst() {
	എങ്കിൽ (!has_srst)
		തിരികെ;
	എങ്കിൽ (has_ce && !ce_over_srst)
		unmap_ce();

	എങ്കിൽ (!is_fine) {
		എങ്കിൽ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		അല്ലെങ്കിൽ
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		അല്ലെങ്കിൽ
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = അസത്യം;
}

Cell *FfData::emit() {
	remove();
	എങ്കിൽ (!width)
		തിരികെ nullptr;
	എങ്കിൽ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		എങ്കിൽ (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} അല്ലെങ്കിൽ {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			തിരികെ nullptr;
		}
	}
	എങ്കിൽ (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	എങ്കിൽ (!is_fine) {
		എങ്കിൽ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			എങ്കിൽ (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} അല്ലെങ്കിൽ {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} അല്ലെങ്കിൽ എങ്കിൽ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} അല്ലെങ്കിൽ എങ്കിൽ (!has_clk) {
			log_assert(!has_srst);
			എങ്കിൽ (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			അല്ലെങ്കിൽ എങ്കിൽ (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			അല്ലെങ്കിൽ
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (has_sr) {
				എങ്കിൽ (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				അല്ലെങ്കിൽ
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} അല്ലെങ്കിൽ എങ്കിൽ (has_arst) {
				എങ്കിൽ (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				അല്ലെങ്കിൽ
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} അല്ലെങ്കിൽ എങ്കിൽ (has_aload) {
				എങ്കിൽ (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				അല്ലെങ്കിൽ
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} അല്ലെങ്കിൽ എങ്കിൽ (has_srst) {
				എങ്കിൽ (has_ce)
					എങ്കിൽ (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					അല്ലെങ്കിൽ
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				അല്ലെങ്കിൽ
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				അല്ലെങ്കിൽ
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} അല്ലെങ്കിൽ {
		എങ്കിൽ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} അല്ലെങ്കിൽ എങ്കിൽ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} അല്ലെങ്കിൽ എങ്കിൽ (!has_clk) {
			log_assert(!has_srst);
			എങ്കിൽ (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			അല്ലെങ്കിൽ എങ്കിൽ (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			അല്ലെങ്കിൽ
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (has_sr) {
				എങ്കിൽ (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				അല്ലെങ്കിൽ
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} അല്ലെങ്കിൽ എങ്കിൽ (has_arst) {
				എങ്കിൽ (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				അല്ലെങ്കിൽ
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} അല്ലെങ്കിൽ എങ്കിൽ (has_aload) {
				എങ്കിൽ (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				അല്ലെങ്കിൽ
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} അല്ലെങ്കിൽ എങ്കിൽ (has_srst) {
				എങ്കിൽ (has_ce)
					എങ്കിൽ (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					അല്ലെങ്കിൽ
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				അല്ലെങ്കിൽ
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				അല്ലെങ്കിൽ
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	തിരികെ cell;
}

ശൂന്യം FfData::remove() {
	എങ്കിൽ (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

നാമാകാശ {
	State invert(State s) {
		തിരഞ്ഞെടുപ്പ് (s) {
			അവസ്ഥ State::S0: തിരികെ State::S1;
			അവസ്ഥ State::S1: തിരികെ State::S0;
			സ്ഥിരസ്ഥിതി: തിരികെ s;
		}
	}
}

ശൂന്യം FfData::flip_rst_bits(സ്ഥിരം pool<പൂർണ്ണസംഖ്യ> &bits) {
	എങ്കിൽ (!bits.size())
		തിരികെ;

	remove_init();

	വേണ്ടി (auto bit: bits) {
		എങ്കിൽ (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		എങ്കിൽ (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

ശൂന്യം FfData::flip_bits(സ്ഥിരം pool<പൂർണ്ണസംഖ്യ> &bits) {
	എങ്കിൽ (!bits.size())
		തിരികെ;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	എങ്കിൽ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	എങ്കിൽ (is_fine) {
		എങ്കിൽ (has_sr) {
			തർക്കം new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			എങ്കിൽ (pol_set) {
				എങ്കിൽ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} അല്ലെങ്കിൽ {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} അല്ലെങ്കിൽ {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		എങ്കിൽ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		എങ്കിൽ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	അല്ലെങ്കിൽ
	{
		എങ്കിൽ (has_sr) {
			SigSpec not_clr;
			എങ്കിൽ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = സത്യം;
			} അല്ലെങ്കിൽ {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			എങ്കിൽ (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = സത്യം;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			വേണ്ടി (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		വേണ്ടി (auto bit: bits)
			mask.set(bit, State::S1);

		എങ്കിൽ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		എങ്കിൽ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
