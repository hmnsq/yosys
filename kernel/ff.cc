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

#অন্তর্ভাব "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename প্রবেশ_Type, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<Ff_Type_তথ্য, OutputType>>>
শূন্য manufacture_info(প্রবেশ_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr যৌক্তিক have_cell = std::is_same_v<প্রবেশ_Type, Cell*>;
	যদি constexpr (std::is_same_v<প্রবেশ_Type, IdString>) {
		type = flop;
	} অন্যথা {
		static_assert(std::is_same_v<প্রবেশ_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	যদি constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = Get_আকার(info.sig_q);
		info.attributes = cell->attributes;
		যদি (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	যদি (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		যদি (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = সত্য;
			যদি constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			যদি (type == ID($anyinit)) {
				info.is_anyinit = সত্য;
				যদি constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} অন্যথা যদি (type == ID($sr)) {
			// No data input at all.
		} অন্যথা যদি (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = সত্য;
			যদি constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} অন্যথা {
			info.has_clk = সত্য;
			যদি constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		যদি (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = সত্য;
			যদি constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		যদি (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = সত্য;
			যদি constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		যদি (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = সত্য;
			যদি constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		যদি (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = সত্য;
			যদি constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_মান);
			}
		}
		যদি (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = সত্য;
			যদি constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_মান);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} অন্যথা যদি (type == ID($_FF_)) {
		info.is_fine = সত্য;
		info.has_gclk = সত্য;
		যদি constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} অন্যথা যদি (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = সত্য;
		info.has_sr = সত্য;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		যদি constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} অন্যথা যদি (type_str.substr(0, 6) == "$_DFF_" && type_str.আকার_2() == 8) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[6] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} অন্যথা যদি (type_str.substr(0, 7) == "$_DFFE_" && type_str.আকার_2() == 10) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = সত্য;
		info.pol_ce = type_str[8] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} অন্যথা যদি (type_str.substr(0, 6) == "$_DFF_" && type_str.আকার_2() == 10) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = সত্য;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} অন্যথা যদি (type_str.substr(0, 7) == "$_DFFE_" && type_str.আকার_2() == 12) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = সত্য;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = সত্য;
		info.pol_ce = type_str[10] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} অন্যথা যদি (type_str.substr(0, 8) == "$_ALDFF_" && type_str.আকার_2() == 11) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = সত্য;
		info.pol_aload = type_str[9] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} অন্যথা যদি (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.আকার_2() == 13) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = সত্য;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = সত্য;
		info.pol_ce = type_str[11] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} অন্যথা যদি (type_str.substr(0, 8) == "$_DFFSR_" && type_str.আকার_2() == 12) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = সত্য;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} অন্যথা যদি (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.আকার_2() == 14) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = সত্য;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = সত্য;
		info.pol_ce = type_str[12] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} অন্যথা যদি (type_str.substr(0, 7) == "$_SDFF_" && type_str.আকার_2() == 11) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = সত্য;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} অন্যথা যদি (type_str.substr(0, 8) == "$_SDFFE_" && type_str.আকার_2() == 13) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = সত্য;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = সত্য;
		info.pol_ce = type_str[11] == 'P';
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} অন্যথা যদি (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.আকার_2() == 14) {
		info.is_fine = সত্য;
		info.has_clk = সত্য;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = সত্য;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = সত্য;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = সত্য;
		যদি constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} অন্যথা যদি (type_str.substr(0, 9) == "$_DLATCH_" && type_str.আকার_2() == 11) {
		info.is_fine = সত্য;
		info.has_aload = সত্য;
		info.has_aload = সত্য;
		info.pol_aload = type_str[9] == 'P';
		যদি constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} অন্যথা যদি (type_str.substr(0, 9) == "$_DLATCH_" && type_str.আকার_2() == 13) {
		info.is_fine = সত্য;
		info.has_aload = সত্য;
		info.has_aload = সত্য;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = সত্য;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		যদি constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} অন্যথা যদি (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.আকার_2() == 15) {
		info.is_fine = সত্য;
		info.has_aload = সত্য;
		info.has_aload = সত্য;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = সত্য;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		যদি constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} অন্যথা {
		log_assert(0);
	}
	যদি constexpr (have_cell)
		যদি (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = মিথ্যা;
			info.has_arst = সত্য;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_তথ্য::Ff_Type_তথ্য(IdString type) : Ff_Type_তথ্য()
{
	manufacture_info(type, *এই, nullptr);
}

Ff_তথ্য::Ff_তথ্য(FfInitVals *initvals, Cell *cell_) : Ff_তথ্য(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *এই, initvals);
}

Ff_তথ্য Ff_তথ্য::slice(ধ্রুবক std::vector<পূর্ণসংখ্যা> &bits) {
	Ff_তথ্য res(module, initvals, module->twines().add(NEW_ID));
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
	যদি (has_arst)
		arst_bits.emplace(bits.আকার_2());
	std::optional<Const::Builder> srst_bits;
	যদি (has_srst)
		srst_bits.emplace(bits.আকার_2());
	std::optional<Const::Builder> init_bits;
	যদি (initvals)
		init_bits.emplace(bits.আকার_2());

	জন্য (পূর্ণসংখ্যা i : bits) {
		res.sig_q.append(sig_q[i]);
		যদি (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		যদি (has_aload)
			res.sig_ad.append(sig_ad[i]);
		যদি (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		যদি (has_arst)
			arst_bits->ঠেলো_back(val_arst[i]);
		যদি (has_srst)
			srst_bits->ঠেলো_back(val_srst[i]);
		যদি (initvals)
			init_bits->ঠেলো_back(val_init[i]);
	}

	যদি (has_arst)
		res.val_arst = arst_bits->build();
	যদি (has_srst)
		res.val_srst = srst_bits->build();
	যদি (initvals)
		res.val_init = init_bits->build();
	res.width = Get_আকার(res.sig_q);
	ফেরত res;
}

শূন্য Ff_তথ্য::add_dummy_ce() {
	যদি (has_ce)
		ফেরত;
	has_ce = সত্য;
	pol_ce = সত্য;
	sig_ce = State::S1;
	ce_over_srst = মিথ্যা;
}

শূন্য Ff_তথ্য::add_dummy_srst() {
	যদি (has_srst)
		ফেরত;
	has_srst = সত্য;
	pol_srst = সত্য;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = মিথ্যা;
}

শূন্য Ff_তথ্য::add_dummy_arst() {
	যদি (has_arst)
		ফেরত;
	has_arst = সত্য;
	pol_arst = সত্য;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

শূন্য Ff_তথ্য::add_dummy_aload() {
	যদি (has_aload)
		ফেরত;
	has_aload = সত্য;
	pol_aload = সত্য;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

শূন্য Ff_তথ্য::add_dummy_sr() {
	যদি (has_sr)
		ফেরত;
	has_sr = সত্য;
	pol_clr = সত্য;
	pol_set = সত্য;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

শূন্য Ff_তথ্য::add_dummy_clk() {
	যদি (has_clk)
		ফেরত;
	has_clk = সত্য;
	pol_clk = সত্য;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

শূন্য Ff_তথ্য::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = সত্য;
	has_arst = মিথ্যা;
}

শূন্য Ff_তথ্য::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = সত্য;
	has_arst = মিথ্যা;
	জন্য (পূর্ণসংখ্যা i = 0; i < width; i++) {
		যদি (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		অন্যথা
			sig_clr[i] = sig_arst;
	}
}

শূন্য Ff_তথ্য::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = সত্য;
	has_aload = মিথ্যা;
	যদি (!is_fine) {
		pol_clr = মিথ্যা;
		pol_set = সত্য;
		যদি (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} অন্যথা {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} অন্যথা {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		যদি (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} অন্যথা {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

শূন্য Ff_তথ্য::convert_ce_over_srst(যৌক্তিক val) {
	যদি (!has_ce || !has_srst || ce_over_srst == val)
		ফেরত;
	যদি (val) {
		// sdffe to sdffce
		যদি (!is_fine) {
			যদি (pol_ce) {
				যদি (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} অন্যথা {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} অন্যথা {
				যদি (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} অন্যথা {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} অন্যথা {
			যদি (pol_ce) {
				যদি (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} অন্যথা {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} অন্যথা {
				যদি (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} অন্যথা {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} অন্যথা {
		// sdffce to sdffe
		যদি (!is_fine) {
			যদি (pol_srst) {
				যদি (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} অন্যথা {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} অন্যথা {
				যদি (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} অন্যথা {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} অন্যথা {
			যদি (pol_srst) {
				যদি (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} অন্যথা {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} অন্যথা {
				যদি (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} অন্যথা {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

শূন্য Ff_তথ্য::unmap_ce() {
	যদি (!has_ce)
		ফেরত;
	log_assert(has_clk);
	যদি (has_srst && ce_over_srst)
		unmap_srst();

	যদি (!is_fine) {
		যদি (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		অন্যথা
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} অন্যথা {
		যদি (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		অন্যথা
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = মিথ্যা;
}

শূন্য Ff_তথ্য::unmap_srst() {
	যদি (!has_srst)
		ফেরত;
	যদি (has_ce && !ce_over_srst)
		unmap_ce();

	যদি (!is_fine) {
		যদি (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		অন্যথা
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} অন্যথা {
		যদি (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		অন্যথা
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = মিথ্যা;
}

Cell *Ff_তথ্য::emit() {
	remove();
	যদি (!width)
		ফেরত nullptr;
	যদি (!has_aload && !has_clk && !has_gclk && !has_sr) {
		যদি (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} অন্যথা {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			ফেরত nullptr;
		}
	}
	যদি (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	যদি (!is_fine) {
		যদি (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			যদি (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} অন্যথা {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} অন্যথা যদি (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} অন্যথা যদি (!has_clk) {
			log_assert(!has_srst);
			যদি (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			অন্যথা যদি (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			অন্যথা
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} অন্যথা {
			যদি (has_sr) {
				যদি (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				অন্যথা
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} অন্যথা যদি (has_arst) {
				যদি (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				অন্যথা
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} অন্যথা যদি (has_aload) {
				যদি (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				অন্যথা
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} অন্যথা যদি (has_srst) {
				যদি (has_ce)
					যদি (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					অন্যথা
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				অন্যথা
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} অন্যথা {
				যদি (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				অন্যথা
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} অন্যথা {
		যদি (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} অন্যথা যদি (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} অন্যথা যদি (!has_clk) {
			log_assert(!has_srst);
			যদি (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			অন্যথা যদি (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			অন্যথা
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} অন্যথা {
			যদি (has_sr) {
				যদি (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				অন্যথা
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} অন্যথা যদি (has_arst) {
				যদি (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				অন্যথা
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} অন্যথা যদি (has_aload) {
				যদি (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				অন্যথা
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} অন্যথা যদি (has_srst) {
				যদি (has_ce)
					যদি (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					অন্যথা
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				অন্যথা
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} অন্যথা {
				যদি (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				অন্যথা
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	ফেরত cell;
}

শূন্য Ff_তথ্য::remove() {
	যদি (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

নামাকাশ {
	State invert(State s) {
		নির্বাচন (s) {
			ক্ষেত্র State::S0: ফেরত State::S1;
			ক্ষেত্র State::S1: ফেরত State::S0;
			পূর্বনির্ধারিত: ফেরত s;
		}
	}
}

শূন্য Ff_তথ্য::flip_rst_bits(ধ্রুবক pool<পূর্ণসংখ্যা> &bits) {
	যদি (!bits.আকার_2())
		ফেরত;

	remove_init();

	জন্য (auto bit: bits) {
		যদি (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		যদি (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

শূন্য Ff_তথ্য::flip_bits(ধ্রুবক pool<পূর্ণসংখ্যা> &bits) {
	যদি (!bits.আকার_2())
		ফেরত;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	যদি (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	যদি (is_fine) {
		যদি (has_sr) {
			যৌক্তিক new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			যদি (pol_set) {
				যদি (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} অন্যথা {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} অন্যথা {
				যদি (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} অন্যথা {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		যদি (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		যদি (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	অন্যথা
	{
		যদি (has_sr) {
			SigSpec not_clr;
			যদি (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = সত্য;
			} অন্যথা {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			যদি (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = সত্য;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			জন্য (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		জন্য (auto bit: bits)
			mask.set(bit, State::S1);

		যদি (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		যদি (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
