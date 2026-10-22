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

#அந்தர்பாவ "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename உள்ளீடு_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
வெறுமை manufacture_info(உள்ளீடு_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr தருக்க have_cell = std::is_same_v<உள்ளீடு_Type, Cell*>;
	எனில் constexpr (std::is_same_v<உள்ளீடு_Type, IdString>) {
		type = flop;
	} இல்லையெனில் {
		static_assert(std::is_same_v<உள்ளீடு_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	எனில் constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		எனில் (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	எனில் (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		எனில் (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = உண்மை;
			எனில் constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			எனில் (type == ID($anyinit)) {
				info.is_anyinit = உண்மை;
				எனில் constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} இல்லையெனில் எனில் (type == ID($sr)) {
			// No data input at all.
		} இல்லையெனில் எனில் (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = உண்மை;
			எனில் constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} இல்லையெனில் {
			info.has_clk = உண்மை;
			எனில் constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		எனில் (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = உண்மை;
			எனில் constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		எனில் (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = உண்மை;
			எனில் constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		எனில் (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = உண்மை;
			எனில் constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		எனில் (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = உண்மை;
			எனில் constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_மதிப்பு);
			}
		}
		எனில் (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = உண்மை;
			எனில் constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_மதிப்பு);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} இல்லையெனில் எனில் (type == ID($_FF_)) {
		info.is_fine = உண்மை;
		info.has_gclk = உண்மை;
		எனில் constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} இல்லையெனில் எனில் (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = உண்மை;
		info.has_sr = உண்மை;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[6] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = உண்மை;
		info.pol_ce = type_str[8] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = உண்மை;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = உண்மை;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = உண்மை;
		info.pol_ce = type_str[10] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = உண்மை;
		info.pol_aload = type_str[9] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = உண்மை;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = உண்மை;
		info.pol_ce = type_str[11] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = உண்மை;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = உண்மை;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = உண்மை;
		info.pol_ce = type_str[12] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = உண்மை;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = உண்மை;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = உண்மை;
		info.pol_ce = type_str[11] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = உண்மை;
		info.has_clk = உண்மை;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = உண்மை;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = உண்மை;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = உண்மை;
		எனில் constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = உண்மை;
		info.has_aload = உண்மை;
		info.has_aload = உண்மை;
		info.pol_aload = type_str[9] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = உண்மை;
		info.has_aload = உண்மை;
		info.has_aload = உண்மை;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = உண்மை;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		எனில் constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} இல்லையெனில் எனில் (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = உண்மை;
		info.has_aload = உண்மை;
		info.has_aload = உண்மை;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = உண்மை;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		எனில் constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} இல்லையெனில் {
		log_assert(0);
	}
	எனில் constexpr (have_cell)
		எனில் (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = பொய்;
			info.has_arst = உண்மை;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *இது, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *இது, initvals);
}

FfData FfData::slice(மாறிலி std::vector<முழு> &bits) {
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
	எனில் (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	எனில் (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	எனில் (initvals)
		init_bits.emplace(bits.size());

	க்கு (முழு i : bits) {
		res.sig_q.append(sig_q[i]);
		எனில் (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		எனில் (has_aload)
			res.sig_ad.append(sig_ad[i]);
		எனில் (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		எனில் (has_arst)
			arst_bits->தள்ளு_back(val_arst[i]);
		எனில் (has_srst)
			srst_bits->தள்ளு_back(val_srst[i]);
		எனில் (initvals)
			init_bits->தள்ளு_back(val_init[i]);
	}

	எனில் (has_arst)
		res.val_arst = arst_bits->build();
	எனில் (has_srst)
		res.val_srst = srst_bits->build();
	எனில் (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	திரும்பு res;
}

வெறுமை FfData::add_dummy_ce() {
	எனில் (has_ce)
		திரும்பு;
	has_ce = உண்மை;
	pol_ce = உண்மை;
	sig_ce = State::S1;
	ce_over_srst = பொய்;
}

வெறுமை FfData::add_dummy_srst() {
	எனில் (has_srst)
		திரும்பு;
	has_srst = உண்மை;
	pol_srst = உண்மை;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = பொய்;
}

வெறுமை FfData::add_dummy_arst() {
	எனில் (has_arst)
		திரும்பு;
	has_arst = உண்மை;
	pol_arst = உண்மை;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

வெறுமை FfData::add_dummy_aload() {
	எனில் (has_aload)
		திரும்பு;
	has_aload = உண்மை;
	pol_aload = உண்மை;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

வெறுமை FfData::add_dummy_sr() {
	எனில் (has_sr)
		திரும்பு;
	has_sr = உண்மை;
	pol_clr = உண்மை;
	pol_set = உண்மை;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

வெறுமை FfData::add_dummy_clk() {
	எனில் (has_clk)
		திரும்பு;
	has_clk = உண்மை;
	pol_clk = உண்மை;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

வெறுமை FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = உண்மை;
	has_arst = பொய்;
}

வெறுமை FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = உண்மை;
	has_arst = பொய்;
	க்கு (முழு i = 0; i < width; i++) {
		எனில் (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		இல்லையெனில்
			sig_clr[i] = sig_arst;
	}
}

வெறுமை FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = உண்மை;
	has_aload = பொய்;
	எனில் (!is_fine) {
		pol_clr = பொய்;
		pol_set = உண்மை;
		எனில் (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} இல்லையெனில் {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} இல்லையெனில் {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		எனில் (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} இல்லையெனில் {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

வெறுமை FfData::convert_ce_over_srst(தருக்க val) {
	எனில் (!has_ce || !has_srst || ce_over_srst == val)
		திரும்பு;
	எனில் (val) {
		// sdffe to sdffce
		எனில் (!is_fine) {
			எனில் (pol_ce) {
				எனில் (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} இல்லையெனில் {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} இல்லையெனில் {
				எனில் (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} இல்லையெனில் {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} இல்லையெனில் {
			எனில் (pol_ce) {
				எனில் (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} இல்லையெனில் {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} இல்லையெனில் {
				எனில் (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} இல்லையெனில் {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} இல்லையெனில் {
		// sdffce to sdffe
		எனில் (!is_fine) {
			எனில் (pol_srst) {
				எனில் (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} இல்லையெனில் {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} இல்லையெனில் {
				எனில் (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} இல்லையெனில் {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} இல்லையெனில் {
			எனில் (pol_srst) {
				எனில் (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} இல்லையெனில் {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} இல்லையெனில் {
				எனில் (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} இல்லையெனில் {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

வெறுமை FfData::unmap_ce() {
	எனில் (!has_ce)
		திரும்பு;
	log_assert(has_clk);
	எனில் (has_srst && ce_over_srst)
		unmap_srst();

	எனில் (!is_fine) {
		எனில் (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		இல்லையெனில்
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} இல்லையெனில் {
		எனில் (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		இல்லையெனில்
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = பொய்;
}

வெறுமை FfData::unmap_srst() {
	எனில் (!has_srst)
		திரும்பு;
	எனில் (has_ce && !ce_over_srst)
		unmap_ce();

	எனில் (!is_fine) {
		எனில் (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		இல்லையெனில்
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} இல்லையெனில் {
		எனில் (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		இல்லையெனில்
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = பொய்;
}

Cell *FfData::emit() {
	remove();
	எனில் (!width)
		திரும்பு nullptr;
	எனில் (!has_aload && !has_clk && !has_gclk && !has_sr) {
		எனில் (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} இல்லையெனில் {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			திரும்பு nullptr;
		}
	}
	எனில் (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	எனில் (!is_fine) {
		எனில் (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			எனில் (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} இல்லையெனில் {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} இல்லையெனில் எனில் (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} இல்லையெனில் எனில் (!has_clk) {
			log_assert(!has_srst);
			எனில் (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			இல்லையெனில் எனில் (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			இல்லையெனில்
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} இல்லையெனில் {
			எனில் (has_sr) {
				எனில் (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				இல்லையெனில்
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} இல்லையெனில் எனில் (has_arst) {
				எனில் (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				இல்லையெனில்
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} இல்லையெனில் எனில் (has_aload) {
				எனில் (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				இல்லையெனில்
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} இல்லையெனில் எனில் (has_srst) {
				எனில் (has_ce)
					எனில் (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					இல்லையெனில்
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				இல்லையெனில்
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} இல்லையெனில் {
				எனில் (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				இல்லையெனில்
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} இல்லையெனில் {
		எனில் (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} இல்லையெனில் எனில் (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} இல்லையெனில் எனில் (!has_clk) {
			log_assert(!has_srst);
			எனில் (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			இல்லையெனில் எனில் (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			இல்லையெனில்
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} இல்லையெனில் {
			எனில் (has_sr) {
				எனில் (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				இல்லையெனில்
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} இல்லையெனில் எனில் (has_arst) {
				எனில் (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				இல்லையெனில்
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} இல்லையெனில் எனில் (has_aload) {
				எனில் (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				இல்லையெனில்
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} இல்லையெனில் எனில் (has_srst) {
				எனில் (has_ce)
					எனில் (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					இல்லையெனில்
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				இல்லையெனில்
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} இல்லையெனில் {
				எனில் (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				இல்லையெனில்
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	திரும்பு cell;
}

வெறுமை FfData::remove() {
	எனில் (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

நாமாகாஶ {
	State invert(State s) {
		தேர்வு (s) {
			நிலை State::S0: திரும்பு State::S1;
			நிலை State::S1: திரும்பு State::S0;
			இயல்பு: திரும்பு s;
		}
	}
}

வெறுமை FfData::flip_rst_bits(மாறிலி pool<முழு> &bits) {
	எனில் (!bits.size())
		திரும்பு;

	remove_init();

	க்கு (auto bit: bits) {
		எனில் (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		எனில் (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

வெறுமை FfData::flip_bits(மாறிலி pool<முழு> &bits) {
	எனில் (!bits.size())
		திரும்பு;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	எனில் (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	எனில் (is_fine) {
		எனில் (has_sr) {
			தருக்க new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			எனில் (pol_set) {
				எனில் (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} இல்லையெனில் {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} இல்லையெனில் {
				எனில் (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} இல்லையெனில் {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		எனில் (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		எனில் (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	இல்லையெனில்
	{
		எனில் (has_sr) {
			SigSpec not_clr;
			எனில் (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = உண்மை;
			} இல்லையெனில் {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			எனில் (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = உண்மை;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			க்கு (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		க்கு (auto bit: bits)
			mask.set(bit, State::S1);

		எனில் (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		எனில் (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
