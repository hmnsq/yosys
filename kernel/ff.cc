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

#અન્તર્ભાવ "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename આગત_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
શૂન્ય manufacture_info(આગત_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr તાર્કિક have_cell = std::is_same_v<આગત_Type, Cell*>;
	જો constexpr (std::is_same_v<આગત_Type, IdString>) {
		type = flop;
	} નહીંતર {
		static_assert(std::is_same_v<આગત_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	જો constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		જો (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	જો (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		જો (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = સાચું;
			જો constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			જો (type == ID($anyinit)) {
				info.is_anyinit = સાચું;
				જો constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} નહીંતર જો (type == ID($sr)) {
			// No data input at all.
		} નહીંતર જો (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = સાચું;
			જો constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} નહીંતર {
			info.has_clk = સાચું;
			જો constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		જો (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = સાચું;
			જો constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		જો (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = સાચું;
			જો constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		જો (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = સાચું;
			જો constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		જો (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = સાચું;
			જો constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_કિંમત);
			}
		}
		જો (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = સાચું;
			જો constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_કિંમત);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} નહીંતર જો (type == ID($_FF_)) {
		info.is_fine = સાચું;
		info.has_gclk = સાચું;
		જો constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} નહીંતર જો (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = સાચું;
		info.has_sr = સાચું;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		જો constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} નહીંતર જો (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[6] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} નહીંતર જો (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = સાચું;
		info.pol_ce = type_str[8] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} નહીંતર જો (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = સાચું;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} નહીંતર જો (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = સાચું;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = સાચું;
		info.pol_ce = type_str[10] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} નહીંતર જો (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = સાચું;
		info.pol_aload = type_str[9] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} નહીંતર જો (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = સાચું;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = સાચું;
		info.pol_ce = type_str[11] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} નહીંતર જો (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = સાચું;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} નહીંતર જો (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = સાચું;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = સાચું;
		info.pol_ce = type_str[12] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} નહીંતર જો (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = સાચું;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} નહીંતર જો (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = સાચું;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = સાચું;
		info.pol_ce = type_str[11] == 'P';
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} નહીંતર જો (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = સાચું;
		info.has_clk = સાચું;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = સાચું;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = સાચું;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = સાચું;
		જો constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} નહીંતર જો (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = સાચું;
		info.has_aload = સાચું;
		info.has_aload = સાચું;
		info.pol_aload = type_str[9] == 'P';
		જો constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} નહીંતર જો (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = સાચું;
		info.has_aload = સાચું;
		info.has_aload = સાચું;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = સાચું;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		જો constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} નહીંતર જો (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = સાચું;
		info.has_aload = સાચું;
		info.has_aload = સાચું;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = સાચું;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		જો constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} નહીંતર {
		log_assert(0);
	}
	જો constexpr (have_cell)
		જો (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = ખોટું;
			info.has_arst = સાચું;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *આ, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *આ, initvals);
}

FfData FfData::slice(અચલ std::vector<પૂર્ણાંક> &bits) {
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
	જો (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	જો (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	જો (initvals)
		init_bits.emplace(bits.size());

	માટે (પૂર્ણાંક i : bits) {
		res.sig_q.append(sig_q[i]);
		જો (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		જો (has_aload)
			res.sig_ad.append(sig_ad[i]);
		જો (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		જો (has_arst)
			arst_bits->ધકેલો_back(val_arst[i]);
		જો (has_srst)
			srst_bits->ધકેલો_back(val_srst[i]);
		જો (initvals)
			init_bits->ધકેલો_back(val_init[i]);
	}

	જો (has_arst)
		res.val_arst = arst_bits->build();
	જો (has_srst)
		res.val_srst = srst_bits->build();
	જો (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	પાછા res;
}

શૂન્ય FfData::add_dummy_ce() {
	જો (has_ce)
		પાછા;
	has_ce = સાચું;
	pol_ce = સાચું;
	sig_ce = State::S1;
	ce_over_srst = ખોટું;
}

શૂન્ય FfData::add_dummy_srst() {
	જો (has_srst)
		પાછા;
	has_srst = સાચું;
	pol_srst = સાચું;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = ખોટું;
}

શૂન્ય FfData::add_dummy_arst() {
	જો (has_arst)
		પાછા;
	has_arst = સાચું;
	pol_arst = સાચું;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

શૂન્ય FfData::add_dummy_aload() {
	જો (has_aload)
		પાછા;
	has_aload = સાચું;
	pol_aload = સાચું;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

શૂન્ય FfData::add_dummy_sr() {
	જો (has_sr)
		પાછા;
	has_sr = સાચું;
	pol_clr = સાચું;
	pol_set = સાચું;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

શૂન્ય FfData::add_dummy_clk() {
	જો (has_clk)
		પાછા;
	has_clk = સાચું;
	pol_clk = સાચું;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

શૂન્ય FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = સાચું;
	has_arst = ખોટું;
}

શૂન્ય FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = સાચું;
	has_arst = ખોટું;
	માટે (પૂર્ણાંક i = 0; i < width; i++) {
		જો (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		નહીંતર
			sig_clr[i] = sig_arst;
	}
}

શૂન્ય FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = સાચું;
	has_aload = ખોટું;
	જો (!is_fine) {
		pol_clr = ખોટું;
		pol_set = સાચું;
		જો (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} નહીંતર {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} નહીંતર {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		જો (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} નહીંતર {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

શૂન્ય FfData::convert_ce_over_srst(તાર્કિક val) {
	જો (!has_ce || !has_srst || ce_over_srst == val)
		પાછા;
	જો (val) {
		// sdffe to sdffce
		જો (!is_fine) {
			જો (pol_ce) {
				જો (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} નહીંતર {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} નહીંતર {
				જો (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} નહીંતર {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} નહીંતર {
			જો (pol_ce) {
				જો (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} નહીંતર {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} નહીંતર {
				જો (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} નહીંતર {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} નહીંતર {
		// sdffce to sdffe
		જો (!is_fine) {
			જો (pol_srst) {
				જો (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} નહીંતર {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} નહીંતર {
				જો (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} નહીંતર {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} નહીંતર {
			જો (pol_srst) {
				જો (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} નહીંતર {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} નહીંતર {
				જો (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} નહીંતર {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

શૂન્ય FfData::unmap_ce() {
	જો (!has_ce)
		પાછા;
	log_assert(has_clk);
	જો (has_srst && ce_over_srst)
		unmap_srst();

	જો (!is_fine) {
		જો (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		નહીંતર
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} નહીંતર {
		જો (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		નહીંતર
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = ખોટું;
}

શૂન્ય FfData::unmap_srst() {
	જો (!has_srst)
		પાછા;
	જો (has_ce && !ce_over_srst)
		unmap_ce();

	જો (!is_fine) {
		જો (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		નહીંતર
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} નહીંતર {
		જો (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		નહીંતર
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = ખોટું;
}

Cell *FfData::emit() {
	remove();
	જો (!width)
		પાછા nullptr;
	જો (!has_aload && !has_clk && !has_gclk && !has_sr) {
		જો (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} નહીંતર {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			પાછા nullptr;
		}
	}
	જો (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	જો (!is_fine) {
		જો (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			જો (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} નહીંતર {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} નહીંતર જો (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} નહીંતર જો (!has_clk) {
			log_assert(!has_srst);
			જો (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			નહીંતર જો (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			નહીંતર
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} નહીંતર {
			જો (has_sr) {
				જો (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				નહીંતર
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} નહીંતર જો (has_arst) {
				જો (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				નહીંતર
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} નહીંતર જો (has_aload) {
				જો (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				નહીંતર
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} નહીંતર જો (has_srst) {
				જો (has_ce)
					જો (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					નહીંતર
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				નહીંતર
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} નહીંતર {
				જો (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				નહીંતર
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} નહીંતર {
		જો (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} નહીંતર જો (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} નહીંતર જો (!has_clk) {
			log_assert(!has_srst);
			જો (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			નહીંતર જો (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			નહીંતર
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} નહીંતર {
			જો (has_sr) {
				જો (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				નહીંતર
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} નહીંતર જો (has_arst) {
				જો (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				નહીંતર
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} નહીંતર જો (has_aload) {
				જો (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				નહીંતર
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} નહીંતર જો (has_srst) {
				જો (has_ce)
					જો (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					નહીંતર
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				નહીંતર
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} નહીંતર {
				જો (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				નહીંતર
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	પાછા cell;
}

શૂન્ય FfData::remove() {
	જો (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

નામાકાશ {
	State invert(State s) {
		પસંદગી (s) {
			સ્થિતિ State::S0: પાછા State::S1;
			સ્થિતિ State::S1: પાછા State::S0;
			મૂળભૂત: પાછા s;
		}
	}
}

શૂન્ય FfData::flip_rst_bits(અચલ pool<પૂર્ણાંક> &bits) {
	જો (!bits.size())
		પાછા;

	remove_init();

	માટે (auto bit: bits) {
		જો (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		જો (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

શૂન્ય FfData::flip_bits(અચલ pool<પૂર્ણાંક> &bits) {
	જો (!bits.size())
		પાછા;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	જો (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	જો (is_fine) {
		જો (has_sr) {
			તાર્કિક new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			જો (pol_set) {
				જો (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} નહીંતર {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} નહીંતર {
				જો (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} નહીંતર {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		જો (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		જો (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	નહીંતર
	{
		જો (has_sr) {
			SigSpec not_clr;
			જો (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = સાચું;
			} નહીંતર {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			જો (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = સાચું;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			માટે (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		માટે (auto bit: bits)
			mask.set(bit, State::S1);

		જો (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		જો (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
