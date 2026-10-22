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

#شامل "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename درون_داد_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
تهی manufacture_info(درون_داد_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr منطقی have_cell = std::is_same_v<درون_داد_Type, Cell*>;
	اگر constexpr (std::is_same_v<درون_داد_Type, IdString>) {
		type = flop;
	} وگرنه {
		static_assert(std::is_same_v<درون_داد_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	اگر constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		اگر (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	اگر (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		اگر (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = درست;
			اگر constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			اگر (type == ID($anyinit)) {
				info.is_anyinit = درست;
				اگر constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} وگرنه اگر (type == ID($sr)) {
			// No data input at all.
		} وگرنه اگر (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = درست;
			اگر constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} وگرنه {
			info.has_clk = درست;
			اگر constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		اگر (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = درست;
			اگر constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		اگر (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = درست;
			اگر constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		اگر (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = درست;
			اگر constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		اگر (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = درست;
			اگر constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_مقدار);
			}
		}
		اگر (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = درست;
			اگر constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_مقدار);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} وگرنه اگر (type == ID($_FF_)) {
		info.is_fine = درست;
		info.has_gclk = درست;
		اگر constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} وگرنه اگر (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = درست;
		info.has_sr = درست;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		اگر constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} وگرنه اگر (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[6] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} وگرنه اگر (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = درست;
		info.pol_ce = type_str[8] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} وگرنه اگر (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = درست;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} وگرنه اگر (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = درست;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = درست;
		info.pol_ce = type_str[10] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} وگرنه اگر (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = درست;
		info.pol_aload = type_str[9] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} وگرنه اگر (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = درست;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = درست;
		info.pol_ce = type_str[11] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} وگرنه اگر (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = درست;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} وگرنه اگر (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = درست;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = درست;
		info.pol_ce = type_str[12] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} وگرنه اگر (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = درست;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} وگرنه اگر (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = درست;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = درست;
		info.pol_ce = type_str[11] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} وگرنه اگر (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = درست;
		info.has_clk = درست;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = درست;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = درست;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = درست;
		اگر constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} وگرنه اگر (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = درست;
		info.has_aload = درست;
		info.has_aload = درست;
		info.pol_aload = type_str[9] == 'P';
		اگر constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} وگرنه اگر (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = درست;
		info.has_aload = درست;
		info.has_aload = درست;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = درست;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		اگر constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} وگرنه اگر (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = درست;
		info.has_aload = درست;
		info.has_aload = درست;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = درست;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		اگر constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} وگرنه {
		log_assert(0);
	}
	اگر constexpr (have_cell)
		اگر (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = نادرست;
			info.has_arst = درست;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *این, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *این, initvals);
}

FfData FfData::slice(ثابت std::vector<صحیح> &bits) {
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
	اگر (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	اگر (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	اگر (initvals)
		init_bits.emplace(bits.size());

	برای (صحیح i : bits) {
		res.sig_q.append(sig_q[i]);
		اگر (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		اگر (has_aload)
			res.sig_ad.append(sig_ad[i]);
		اگر (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		اگر (has_arst)
			arst_bits->افزودن_back(val_arst[i]);
		اگر (has_srst)
			srst_bits->افزودن_back(val_srst[i]);
		اگر (initvals)
			init_bits->افزودن_back(val_init[i]);
	}

	اگر (has_arst)
		res.val_arst = arst_bits->build();
	اگر (has_srst)
		res.val_srst = srst_bits->build();
	اگر (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	بازگشت res;
}

تهی FfData::add_dummy_ce() {
	اگر (has_ce)
		بازگشت;
	has_ce = درست;
	pol_ce = درست;
	sig_ce = State::S1;
	ce_over_srst = نادرست;
}

تهی FfData::add_dummy_srst() {
	اگر (has_srst)
		بازگشت;
	has_srst = درست;
	pol_srst = درست;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = نادرست;
}

تهی FfData::add_dummy_arst() {
	اگر (has_arst)
		بازگشت;
	has_arst = درست;
	pol_arst = درست;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

تهی FfData::add_dummy_aload() {
	اگر (has_aload)
		بازگشت;
	has_aload = درست;
	pol_aload = درست;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

تهی FfData::add_dummy_sr() {
	اگر (has_sr)
		بازگشت;
	has_sr = درست;
	pol_clr = درست;
	pol_set = درست;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

تهی FfData::add_dummy_clk() {
	اگر (has_clk)
		بازگشت;
	has_clk = درست;
	pol_clk = درست;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

تهی FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = درست;
	has_arst = نادرست;
}

تهی FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = درست;
	has_arst = نادرست;
	برای (صحیح i = 0; i < width; i++) {
		اگر (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		وگرنه
			sig_clr[i] = sig_arst;
	}
}

تهی FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = درست;
	has_aload = نادرست;
	اگر (!is_fine) {
		pol_clr = نادرست;
		pol_set = درست;
		اگر (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} وگرنه {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} وگرنه {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		اگر (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} وگرنه {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

تهی FfData::convert_ce_over_srst(منطقی val) {
	اگر (!has_ce || !has_srst || ce_over_srst == val)
		بازگشت;
	اگر (val) {
		// sdffe to sdffce
		اگر (!is_fine) {
			اگر (pol_ce) {
				اگر (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} وگرنه {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} وگرنه {
				اگر (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} وگرنه {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} وگرنه {
			اگر (pol_ce) {
				اگر (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} وگرنه {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} وگرنه {
				اگر (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} وگرنه {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} وگرنه {
		// sdffce to sdffe
		اگر (!is_fine) {
			اگر (pol_srst) {
				اگر (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} وگرنه {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} وگرنه {
				اگر (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} وگرنه {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} وگرنه {
			اگر (pol_srst) {
				اگر (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} وگرنه {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} وگرنه {
				اگر (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} وگرنه {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

تهی FfData::unmap_ce() {
	اگر (!has_ce)
		بازگشت;
	log_assert(has_clk);
	اگر (has_srst && ce_over_srst)
		unmap_srst();

	اگر (!is_fine) {
		اگر (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		وگرنه
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} وگرنه {
		اگر (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		وگرنه
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = نادرست;
}

تهی FfData::unmap_srst() {
	اگر (!has_srst)
		بازگشت;
	اگر (has_ce && !ce_over_srst)
		unmap_ce();

	اگر (!is_fine) {
		اگر (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		وگرنه
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} وگرنه {
		اگر (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		وگرنه
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = نادرست;
}

Cell *FfData::emit() {
	remove();
	اگر (!width)
		بازگشت nullptr;
	اگر (!has_aload && !has_clk && !has_gclk && !has_sr) {
		اگر (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} وگرنه {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			بازگشت nullptr;
		}
	}
	اگر (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	اگر (!is_fine) {
		اگر (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			اگر (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} وگرنه {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} وگرنه اگر (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} وگرنه اگر (!has_clk) {
			log_assert(!has_srst);
			اگر (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			وگرنه اگر (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			وگرنه
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} وگرنه {
			اگر (has_sr) {
				اگر (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				وگرنه
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} وگرنه اگر (has_arst) {
				اگر (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				وگرنه
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} وگرنه اگر (has_aload) {
				اگر (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				وگرنه
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} وگرنه اگر (has_srst) {
				اگر (has_ce)
					اگر (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					وگرنه
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				وگرنه
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} وگرنه {
				اگر (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				وگرنه
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} وگرنه {
		اگر (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} وگرنه اگر (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} وگرنه اگر (!has_clk) {
			log_assert(!has_srst);
			اگر (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			وگرنه اگر (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			وگرنه
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} وگرنه {
			اگر (has_sr) {
				اگر (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				وگرنه
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} وگرنه اگر (has_arst) {
				اگر (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				وگرنه
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} وگرنه اگر (has_aload) {
				اگر (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				وگرنه
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} وگرنه اگر (has_srst) {
				اگر (has_ce)
					اگر (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					وگرنه
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				وگرنه
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} وگرنه {
				اگر (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				وگرنه
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	بازگشت cell;
}

تهی FfData::remove() {
	اگر (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

فضاینام {
	State invert(State s) {
		انتخاب (s) {
			حالت State::S0: بازگشت State::S1;
			حالت State::S1: بازگشت State::S0;
			پیشفرض: بازگشت s;
		}
	}
}

تهی FfData::flip_rst_bits(ثابت pool<صحیح> &bits) {
	اگر (!bits.size())
		بازگشت;

	remove_init();

	برای (auto bit: bits) {
		اگر (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		اگر (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

تهی FfData::flip_bits(ثابت pool<صحیح> &bits) {
	اگر (!bits.size())
		بازگشت;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	اگر (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	اگر (is_fine) {
		اگر (has_sr) {
			منطقی new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			اگر (pol_set) {
				اگر (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} وگرنه {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} وگرنه {
				اگر (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} وگرنه {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		اگر (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		اگر (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	وگرنه
	{
		اگر (has_sr) {
			SigSpec not_clr;
			اگر (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = درست;
			} وگرنه {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			اگر (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = درست;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			برای (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		برای (auto bit: bits)
			mask.set(bit, State::S1);

		اگر (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		اگر (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
