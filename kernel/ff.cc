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

#अन्तर्भाव "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename निवेशः_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
शून्य manufacture_info(निवेशः_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr तार्किक have_cell = std::is_same_v<निवेशः_Type, Cell*>;
	जइ constexpr (std::is_same_v<निवेशः_Type, IdString>) {
		type = flop;
	} अण्णहा {
		static_assert(std::is_same_v<निवेशः_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	जइ constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		जइ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	जइ (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		जइ (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = सच्च;
			जइ constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			जइ (type == ID($anyinit)) {
				info.is_anyinit = सच्च;
				जइ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} अण्णहा जइ (type == ID($sr)) {
			// No data input at all.
		} अण्णहा जइ (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = सच्च;
			जइ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} अण्णहा {
			info.has_clk = सच्च;
			जइ constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		जइ (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = सच्च;
			जइ constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		जइ (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = सच्च;
			जइ constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		जइ (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = सच्च;
			जइ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		जइ (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = सच्च;
			जइ constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_मानम्);
			}
		}
		जइ (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = सच्च;
			जइ constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_मानम्);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} अण्णहा जइ (type == ID($_FF_)) {
		info.is_fine = सच्च;
		info.has_gclk = सच्च;
		जइ constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} अण्णहा जइ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = सच्च;
		info.has_sr = सच्च;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		जइ constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} अण्णहा जइ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[6] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} अण्णहा जइ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = सच्च;
		info.pol_ce = type_str[8] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} अण्णहा जइ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = सच्च;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} अण्णहा जइ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = सच्च;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = सच्च;
		info.pol_ce = type_str[10] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} अण्णहा जइ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = सच्च;
		info.pol_aload = type_str[9] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} अण्णहा जइ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = सच्च;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = सच्च;
		info.pol_ce = type_str[11] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} अण्णहा जइ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = सच्च;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} अण्णहा जइ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = सच्च;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = सच्च;
		info.pol_ce = type_str[12] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} अण्णहा जइ (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = सच्च;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} अण्णहा जइ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = सच्च;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = सच्च;
		info.pol_ce = type_str[11] == 'P';
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} अण्णहा जइ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = सच्च;
		info.has_clk = सच्च;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = सच्च;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = सच्च;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = सच्च;
		जइ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} अण्णहा जइ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = सच्च;
		info.has_aload = सच्च;
		info.has_aload = सच्च;
		info.pol_aload = type_str[9] == 'P';
		जइ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} अण्णहा जइ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = सच्च;
		info.has_aload = सच्च;
		info.has_aload = सच्च;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = सच्च;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		जइ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} अण्णहा जइ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = सच्च;
		info.has_aload = सच्च;
		info.has_aload = सच्च;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = सच्च;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		जइ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} अण्णहा {
		log_assert(0);
	}
	जइ constexpr (have_cell)
		जइ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = मिच्छ;
			info.has_arst = सच्च;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *एतत्, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *एतत्, initvals);
}

FfData FfData::slice(ध्रुव std::vector<पूर्णाङ्क> &bits) {
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
	जइ (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	जइ (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	जइ (initvals)
		init_bits.emplace(bits.size());

	प्रति (पूर्णाङ्क i : bits) {
		res.sig_q.append(sig_q[i]);
		जइ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		जइ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		जइ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		जइ (has_arst)
			arst_bits->प्रक्षिप_back(val_arst[i]);
		जइ (has_srst)
			srst_bits->प्रक्षिप_back(val_srst[i]);
		जइ (initvals)
			init_bits->प्रक्षिप_back(val_init[i]);
	}

	जइ (has_arst)
		res.val_arst = arst_bits->build();
	जइ (has_srst)
		res.val_srst = srst_bits->build();
	जइ (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	प्रत्यागम res;
}

शून्य FfData::add_dummy_ce() {
	जइ (has_ce)
		प्रत्यागम;
	has_ce = सच्च;
	pol_ce = सच्च;
	sig_ce = State::S1;
	ce_over_srst = मिच्छ;
}

शून्य FfData::add_dummy_srst() {
	जइ (has_srst)
		प्रत्यागम;
	has_srst = सच्च;
	pol_srst = सच्च;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = मिच्छ;
}

शून्य FfData::add_dummy_arst() {
	जइ (has_arst)
		प्रत्यागम;
	has_arst = सच्च;
	pol_arst = सच्च;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

शून्य FfData::add_dummy_aload() {
	जइ (has_aload)
		प्रत्यागम;
	has_aload = सच्च;
	pol_aload = सच्च;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

शून्य FfData::add_dummy_sr() {
	जइ (has_sr)
		प्रत्यागम;
	has_sr = सच्च;
	pol_clr = सच्च;
	pol_set = सच्च;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

शून्य FfData::add_dummy_clk() {
	जइ (has_clk)
		प्रत्यागम;
	has_clk = सच्च;
	pol_clk = सच्च;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

शून्य FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = सच्च;
	has_arst = मिच्छ;
}

शून्य FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = सच्च;
	has_arst = मिच्छ;
	प्रति (पूर्णाङ्क i = 0; i < width; i++) {
		जइ (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		अण्णहा
			sig_clr[i] = sig_arst;
	}
}

शून्य FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = सच्च;
	has_aload = मिच्छ;
	जइ (!is_fine) {
		pol_clr = मिच्छ;
		pol_set = सच्च;
		जइ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} अण्णहा {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} अण्णहा {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		जइ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} अण्णहा {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

शून्य FfData::convert_ce_over_srst(तार्किक val) {
	जइ (!has_ce || !has_srst || ce_over_srst == val)
		प्रत्यागम;
	जइ (val) {
		// sdffe to sdffce
		जइ (!is_fine) {
			जइ (pol_ce) {
				जइ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} अण्णहा {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} अण्णहा {
				जइ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} अण्णहा {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} अण्णहा {
			जइ (pol_ce) {
				जइ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} अण्णहा {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} अण्णहा {
				जइ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} अण्णहा {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} अण्णहा {
		// sdffce to sdffe
		जइ (!is_fine) {
			जइ (pol_srst) {
				जइ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} अण्णहा {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} अण्णहा {
				जइ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} अण्णहा {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} अण्णहा {
			जइ (pol_srst) {
				जइ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} अण्णहा {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} अण्णहा {
				जइ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} अण्णहा {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

शून्य FfData::unmap_ce() {
	जइ (!has_ce)
		प्रत्यागम;
	log_assert(has_clk);
	जइ (has_srst && ce_over_srst)
		unmap_srst();

	जइ (!is_fine) {
		जइ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		अण्णहा
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} अण्णहा {
		जइ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		अण्णहा
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = मिच्छ;
}

शून्य FfData::unmap_srst() {
	जइ (!has_srst)
		प्रत्यागम;
	जइ (has_ce && !ce_over_srst)
		unmap_ce();

	जइ (!is_fine) {
		जइ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		अण्णहा
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} अण्णहा {
		जइ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		अण्णहा
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = मिच्छ;
}

Cell *FfData::emit() {
	remove();
	जइ (!width)
		प्रत्यागम nullptr;
	जइ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		जइ (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} अण्णहा {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			प्रत्यागम nullptr;
		}
	}
	जइ (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	जइ (!is_fine) {
		जइ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			जइ (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} अण्णहा {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} अण्णहा जइ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} अण्णहा जइ (!has_clk) {
			log_assert(!has_srst);
			जइ (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			अण्णहा जइ (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			अण्णहा
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} अण्णहा {
			जइ (has_sr) {
				जइ (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				अण्णहा
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} अण्णहा जइ (has_arst) {
				जइ (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				अण्णहा
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} अण्णहा जइ (has_aload) {
				जइ (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				अण्णहा
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} अण्णहा जइ (has_srst) {
				जइ (has_ce)
					जइ (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					अण्णहा
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				अण्णहा
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} अण्णहा {
				जइ (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				अण्णहा
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} अण्णहा {
		जइ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} अण्णहा जइ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} अण्णहा जइ (!has_clk) {
			log_assert(!has_srst);
			जइ (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			अण्णहा जइ (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			अण्णहा
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} अण्णहा {
			जइ (has_sr) {
				जइ (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				अण्णहा
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} अण्णहा जइ (has_arst) {
				जइ (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				अण्णहा
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} अण्णहा जइ (has_aload) {
				जइ (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				अण्णहा
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} अण्णहा जइ (has_srst) {
				जइ (has_ce)
					जइ (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					अण्णहा
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				अण्णहा
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} अण्णहा {
				जइ (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				अण्णहा
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	प्रत्यागम cell;
}

शून्य FfData::remove() {
	जइ (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

नामाकाश {
	State invert(State s) {
		विकल्प (s) {
			पक्ष State::S0: प्रत्यागम State::S1;
			पक्ष State::S1: प्रत्यागम State::S0;
			स्वभाव: प्रत्यागम s;
		}
	}
}

शून्य FfData::flip_rst_bits(ध्रुव pool<पूर्णाङ्क> &bits) {
	जइ (!bits.size())
		प्रत्यागम;

	remove_init();

	प्रति (auto bit: bits) {
		जइ (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		जइ (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

शून्य FfData::flip_bits(ध्रुव pool<पूर्णाङ्क> &bits) {
	जइ (!bits.size())
		प्रत्यागम;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	जइ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	जइ (is_fine) {
		जइ (has_sr) {
			तार्किक new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			जइ (pol_set) {
				जइ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} अण्णहा {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} अण्णहा {
				जइ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} अण्णहा {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		जइ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		जइ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	अण्णहा
	{
		जइ (has_sr) {
			SigSpec not_clr;
			जइ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = सच्च;
			} अण्णहा {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			जइ (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = सच्च;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			प्रति (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		प्रति (auto bit: bits)
			mask.set(bit, State::S1);

		जइ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		जइ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
