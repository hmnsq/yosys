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

#ضمن "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
template<typename InputType, typename OutputType, typename = std::enable_if_t<std::is_base_of_v<FfTypeData, OutputType>>>
فراغ manufacture_info(InputType flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = nullptr;
	IdString type;
	constexpr منطقي have_cell = std::is_same_v<InputType, Cell*>;
	ܐܢ constexpr (std::is_same_v<InputType, IdString>) {
		type = flop;
	} ܐܚܪܢܐ {
		static_assert(std::is_same_v<InputType, Cell*>);
		cell = flop;
		type = flop->type;
	}
	ܐܢ constexpr (have_cell) {
		info.sig_q = cell->getPort(ID::Q);
		info.width = GetSize(info.sig_q);
		info.attributes = cell->attributes;
		ܐܢ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	ܐܢ (type.in(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		ܐܢ (type.in(ID($anyinit), ID($ff))) {
			info.has_gclk = ܫܪܪܐ;
			ܐܢ constexpr (have_cell)
				info.sig_d = cell->getPort(ID::D);
			ܐܢ (type == ID($anyinit)) {
				info.is_anyinit = ܫܪܪܐ;
				ܐܢ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} ܐܚܪܢܐ ܐܢ (type == ID($sr)) {
			// No data input at all.
		} ܐܚܪܢܐ ܐܢ (type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = ܫܪܪܐ;
			ܐܢ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::D);
			}
		} ܐܚܪܢܐ {
			info.has_clk = ܫܪܪܐ;
			ܐܢ constexpr (have_cell) {
				info.sig_clk = cell->getPort(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->getPort(ID::D);
			}
		}
		ܐܢ (type.in(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = ܫܪܪܐ;
			ܐܢ constexpr (have_cell) {
				info.sig_ce = cell->getPort(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		ܐܢ (type.in(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = ܫܪܪܐ;
			ܐܢ constexpr (have_cell) {
				info.sig_clr = cell->getPort(ID::CLR);
				info.sig_set = cell->getPort(ID::SET);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_set = cell->getParam(ID::SET_POLARITY).as_bool();
			}
		}
		ܐܢ (type.in(ID($aldff), ID($aldffe))) {
			info.has_aload = ܫܪܪܐ;
			ܐܢ constexpr (have_cell) {
				info.sig_aload = cell->getPort(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->getPort(ID::AD);
			}
		}
		ܐܢ (type.in(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = ܫܪܪܐ;
			ܐܢ constexpr (have_cell) {
				info.sig_arst = cell->getPort(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_VALUE);
			}
		}
		ܐܢ (type.in(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = ܫܪܪܐ;
			ܐܢ constexpr (have_cell) {
				info.sig_srst = cell->getPort(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_VALUE);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} ܐܚܪܢܐ ܐܢ (type == ID($_FF_)) {
		info.is_fine = ܫܪܪܐ;
		info.has_gclk = ܫܪܪܐ;
		ܐܢ constexpr (have_cell)
			info.sig_d = cell->getPort(ID::D);
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = ܫܪܪܐ;
		info.has_sr = ܫܪܪܐ;
		info.pol_set = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 8) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[6] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 10) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = ܫܪܪܐ;
		info.pol_ce = type_str[8] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 6) == "$_DFF_" && type_str.size() == 10) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = ܫܪܪܐ;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 7) == "$_DFFE_" && type_str.size() == 12) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = ܫܪܪܐ;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = ܫܪܪܐ;
		info.pol_ce = type_str[10] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_arst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.size() == 11) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = ܫܪܪܐ;
		info.pol_aload = type_str[9] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.size() == 13) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = ܫܪܪܐ;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = ܫܪܪܐ;
		info.pol_ce = type_str[11] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_aload = cell->getPort(ID::L);
			info.sig_ad = cell->getPort(ID::AD);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.size() == 12) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = ܫܪܪܐ;
		info.pol_set = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.size() == 14) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = ܫܪܪܐ;
		info.pol_set = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = ܫܪܪܐ;
		info.pol_ce = type_str[12] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 7) == "$_SDFF_" && type_str.size() == 11) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = ܫܪܪܐ;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.size() == 13) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = ܫܪܪܐ;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = ܫܪܪܐ;
		info.pol_ce = type_str[11] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.size() == 14) {
		info.is_fine = ܫܪܪܐ;
		info.has_clk = ܫܪܪܐ;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = ܫܪܪܐ;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = ܫܪܪܐ;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = ܫܪܪܐ;
		ܐܢ constexpr (have_cell) {
			info.sig_d = cell->getPort(ID::D);
			info.sig_clk = cell->getPort(ID::C);
			info.sig_srst = cell->getPort(ID::R);
			info.sig_ce = cell->getPort(ID::E);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 11) {
		info.is_fine = ܫܪܪܐ;
		info.has_aload = ܫܪܪܐ;
		info.has_aload = ܫܪܪܐ;
		info.pol_aload = type_str[9] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.size() == 13) {
		info.is_fine = ܫܪܪܐ;
		info.has_aload = ܫܪܪܐ;
		info.has_aload = ܫܪܪܐ;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = ܫܪܪܐ;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		ܐܢ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_arst = cell->getPort(ID::R);
		}
	} ܐܚܪܢܐ ܐܢ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.size() == 15) {
		info.is_fine = ܫܪܪܐ;
		info.has_aload = ܫܪܪܐ;
		info.has_aload = ܫܪܪܐ;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = ܫܪܪܐ;
		info.pol_set = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		ܐܢ constexpr (have_cell) {
			info.sig_ad = cell->getPort(ID::D);
			info.sig_aload = cell->getPort(ID::E);
			info.sig_set = cell->getPort(ID::S);
			info.sig_clr = cell->getPort(ID::R);
		}
	} ܐܚܪܢܐ {
		log_assert(0);
	}
	ܐܢ constexpr (have_cell)
		ܐܢ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = ܕܓܠܐ;
			info.has_arst = ܫܪܪܐ;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

FfTypeData::FfTypeData(IdString type) : FfTypeData()
{
	manufacture_info(type, *هذا, nullptr);
}

FfData::FfData(FfInitVals *initvals, Cell *cell_) : FfData(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *هذا, initvals);
}

FfData FfData::slice(ثابت std::vector<صحيح> &bits) {
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
	ܐܢ (has_arst)
		arst_bits.emplace(bits.size());
	std::optional<Const::Builder> srst_bits;
	ܐܢ (has_srst)
		srst_bits.emplace(bits.size());
	std::optional<Const::Builder> init_bits;
	ܐܢ (initvals)
		init_bits.emplace(bits.size());

	لكل (صحيح i : bits) {
		res.sig_q.append(sig_q[i]);
		ܐܢ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		ܐܢ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		ܐܢ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_set.append(sig_set[i]);
		}
		ܐܢ (has_arst)
			arst_bits->push_back(val_arst[i]);
		ܐܢ (has_srst)
			srst_bits->push_back(val_srst[i]);
		ܐܢ (initvals)
			init_bits->push_back(val_init[i]);
	}

	ܐܢ (has_arst)
		res.val_arst = arst_bits->build();
	ܐܢ (has_srst)
		res.val_srst = srst_bits->build();
	ܐܢ (initvals)
		res.val_init = init_bits->build();
	res.width = GetSize(res.sig_q);
	ܦܢܐ res;
}

فراغ FfData::add_dummy_ce() {
	ܐܢ (has_ce)
		ܦܢܐ;
	has_ce = ܫܪܪܐ;
	pol_ce = ܫܪܪܐ;
	sig_ce = State::S1;
	ce_over_srst = ܕܓܠܐ;
}

فراغ FfData::add_dummy_srst() {
	ܐܢ (has_srst)
		ܦܢܐ;
	has_srst = ܫܪܪܐ;
	pol_srst = ܫܪܪܐ;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, width);
	ce_over_srst = ܕܓܠܐ;
}

فراغ FfData::add_dummy_arst() {
	ܐܢ (has_arst)
		ܦܢܐ;
	has_arst = ܫܪܪܐ;
	pol_arst = ܫܪܪܐ;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, width);
}

فراغ FfData::add_dummy_aload() {
	ܐܢ (has_aload)
		ܦܢܐ;
	has_aload = ܫܪܪܐ;
	pol_aload = ܫܪܪܐ;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, width);
}

فراغ FfData::add_dummy_sr() {
	ܐܢ (has_sr)
		ܦܢܐ;
	has_sr = ܫܪܪܐ;
	pol_clr = ܫܪܪܐ;
	pol_set = ܫܪܪܐ;
	sig_clr = Const(State::S0, width);
	sig_set = Const(State::S0, width);
}

فراغ FfData::add_dummy_clk() {
	ܐܢ (has_clk)
		ܦܢܐ;
	has_clk = ܫܪܪܐ;
	pol_clk = ܫܪܪܐ;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, width);
}

فراغ FfData::arst_to_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = ܫܪܪܐ;
	has_arst = ܕܓܠܐ;
}

فراغ FfData::arst_to_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_set = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, width);
	sig_set = Const(pol_arst ? State::S0 : State::S1, width);
	has_sr = ܫܪܪܐ;
	has_arst = ܕܓܠܐ;
	لكل (صحيح i = 0; i < width; i++) {
		ܐܢ (val_arst[i] == State::S1)
			sig_set[i] = sig_arst;
		ܐܚܪܢܐ
			sig_clr[i] = sig_arst;
	}
}

فراغ FfData::aload_to_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = ܫܪܪܐ;
	has_aload = ܕܓܠܐ;
	ܐܢ (!is_fine) {
		pol_clr = ܕܓܠܐ;
		pol_set = ܫܪܪܐ;
		ܐܢ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, width), sig_ad, sig_aload);
			sig_set = module->Mux(NEW_ID, Const(State::S0, width), sig_ad, sig_aload);
		} ܐܚܪܢܐ {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, width), sig_aload);
			sig_set = module->Mux(NEW_ID, sig_ad, Const(State::S0, width), sig_aload);
		}
	} ܐܚܪܢܐ {
		pol_clr = pol_aload;
		pol_set = pol_aload;
		ܐܢ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} ܐܚܪܢܐ {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_set = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

فراغ FfData::convert_ce_over_srst(منطقي val) {
	ܐܢ (!has_ce || !has_srst || ce_over_srst == val)
		ܦܢܐ;
	ܐܢ (val) {
		// sdffe to sdffce
		ܐܢ (!is_fine) {
			ܐܢ (pol_ce) {
				ܐܢ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} ܐܚܪܢܐ {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} ܐܚܪܢܐ {
				ܐܢ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} ܐܚܪܢܐ {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} ܐܚܪܢܐ {
			ܐܢ (pol_ce) {
				ܐܢ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} ܐܚܪܢܐ {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} ܐܚܪܢܐ {
				ܐܢ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} ܐܚܪܢܐ {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} ܐܚܪܢܐ {
		// sdffce to sdffe
		ܐܢ (!is_fine) {
			ܐܢ (pol_srst) {
				ܐܢ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} ܐܚܪܢܐ {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} ܐܚܪܢܐ {
				ܐܢ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} ܐܚܪܢܐ {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} ܐܚܪܢܐ {
			ܐܢ (pol_srst) {
				ܐܢ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} ܐܚܪܢܐ {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} ܐܚܪܢܐ {
				ܐܢ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} ܐܚܪܢܐ {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

فراغ FfData::unmap_ce() {
	ܐܢ (!has_ce)
		ܦܢܐ;
	log_assert(has_clk);
	ܐܢ (has_srst && ce_over_srst)
		unmap_srst();

	ܐܢ (!is_fine) {
		ܐܢ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		ܐܚܪܢܐ
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} ܐܚܪܢܐ {
		ܐܢ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		ܐܚܪܢܐ
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = ܕܓܠܐ;
}

فراغ FfData::unmap_srst() {
	ܐܢ (!has_srst)
		ܦܢܐ;
	ܐܢ (has_ce && !ce_over_srst)
		unmap_ce();

	ܐܢ (!is_fine) {
		ܐܢ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		ܐܚܪܢܐ
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} ܐܚܪܢܐ {
		ܐܢ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		ܐܚܪܢܐ
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = ܕܓܠܐ;
}

Cell *FfData::emit() {
	remove();
	ܐܢ (!width)
		ܦܢܐ nullptr;
	ܐܢ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		ܐܢ (has_arst) {
			// Convert this case to a D latch.
			arst_to_aload();
		} ܐܚܪܢܐ {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			ܦܢܐ nullptr;
		}
	}
	ܐܢ (initvals && !is_anyinit)
		initvals->set_init(sig_q, val_init);
	ܐܢ (!is_fine) {
		ܐܢ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			ܐܢ (is_anyinit) {
				cell = module->addAnyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} ܐܚܪܢܐ {
				cell = module->addFf(name, sig_d, sig_q);
			}
		} ܐܚܪܢܐ ܐܢ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSr(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ܐܚܪܢܐ ܐܢ (!has_clk) {
			log_assert(!has_srst);
			ܐܢ (has_sr)
				cell = module->addDlatchsr(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ܐܚܪܢܐ ܐܢ (has_arst)
				cell = module->addAdlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			ܐܚܪܢܐ
				cell = module->addDlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ܐܚܪܢܐ {
			ܐܢ (has_sr) {
				ܐܢ (has_ce)
					cell = module->addDffsre(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ܐܚܪܢܐ
					cell = module->addDffsr(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ܐܚܪܢܐ ܐܢ (has_arst) {
				ܐܢ (has_ce)
					cell = module->addAdffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				ܐܚܪܢܐ
					cell = module->addAdff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} ܐܚܪܢܐ ܐܢ (has_aload) {
				ܐܢ (has_ce)
					cell = module->addAldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ܐܚܪܢܐ
					cell = module->addAldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ܐܚܪܢܐ ܐܢ (has_srst) {
				ܐܢ (has_ce)
					ܐܢ (ce_over_srst)
						cell = module->addSdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					ܐܚܪܢܐ
						cell = module->addSdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				ܐܚܪܢܐ
					cell = module->addSdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} ܐܚܪܢܐ {
				ܐܢ (has_ce)
					cell = module->addDffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ܐܚܪܢܐ
					cell = module->addDff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} ܐܚܪܢܐ {
		ܐܢ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->addFfGate(name, sig_d, sig_q);
		} ܐܚܪܢܐ ܐܢ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->addSrGate(name, sig_set, sig_clr, sig_q, pol_set, pol_clr);
		} ܐܚܪܢܐ ܐܢ (!has_clk) {
			log_assert(!has_srst);
			ܐܢ (has_sr)
				cell = module->addDlatchsrGate(name, sig_aload, sig_set, sig_clr, sig_ad, sig_q, pol_aload, pol_set, pol_clr);
			ܐܚܪܢܐ ܐܢ (has_arst)
				cell = module->addAdlatchGate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			ܐܚܪܢܐ
				cell = module->addDlatchGate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ܐܚܪܢܐ {
			ܐܢ (has_sr) {
				ܐܢ (has_ce)
					cell = module->addDffsreGate(name, sig_clk, sig_ce, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_set, pol_clr);
				ܐܚܪܢܐ
					cell = module->addDffsrGate(name, sig_clk, sig_set, sig_clr, sig_d, sig_q, pol_clk, pol_set, pol_clr);
			} ܐܚܪܢܐ ܐܢ (has_arst) {
				ܐܢ (has_ce)
					cell = module->addAdffeGate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				ܐܚܪܢܐ
					cell = module->addAdffGate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} ܐܚܪܢܐ ܐܢ (has_aload) {
				ܐܢ (has_ce)
					cell = module->addAldffeGate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ܐܚܪܢܐ
					cell = module->addAldffGate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ܐܚܪܢܐ ܐܢ (has_srst) {
				ܐܢ (has_ce)
					ܐܢ (ce_over_srst)
						cell = module->addSdffceGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					ܐܚܪܢܐ
						cell = module->addSdffeGate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				ܐܚܪܢܐ
					cell = module->addSdffGate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} ܐܚܪܢܐ {
				ܐܢ (has_ce)
					cell = module->addDffeGate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ܐܚܪܢܐ
					cell = module->addDffGate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->attributes = attributes;
	ܦܢܐ cell;
}

فراغ FfData::remove() {
	ܐܢ (cell) {
		remove_init();
		module->remove(cell);
		cell = nullptr;
	}
}

نطاق {
	State invert(State s) {
		بدل (s) {
			حالة State::S0: ܦܢܐ State::S1;
			حالة State::S1: ܦܢܐ State::S0;
			افتراضي: ܦܢܐ s;
		}
	}
}

فراغ FfData::flip_rst_bits(ثابت pool<صحيح> &bits) {
	ܐܢ (!bits.size())
		ܦܢܐ;

	remove_init();

	لكل (auto bit: bits) {
		ܐܢ (has_arst)
			val_arst.set(bit, invert(val_arst[bit]));
		ܐܢ (has_srst)
			val_srst.set(bit, invert(val_srst[bit]));
		val_init.set(bit, invert(val_init[bit]));
	}
}

فراغ FfData::flip_bits(ثابت pool<صحيح> &bits) {
	ܐܢ (!bits.size())
		ܦܢܐ;

	flip_rst_bits(bits);

	Wire *new_q = module->addWire(NEW_ID, width);

	ܐܢ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	ܐܢ (is_fine) {
		ܐܢ (has_sr) {
			منطقي new_pol_clr = pol_set;
			SigSpec new_sig_clr;
			ܐܢ (pol_set) {
				ܐܢ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_set, sig_clr);
				} ܐܚܪܢܐ {
					new_sig_clr = module->AndGate(NEW_ID, sig_set, sig_clr);
				}
			} ܐܚܪܢܐ {
				ܐܢ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_set, sig_clr);
				} ܐܚܪܢܐ {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_set, sig_clr);
				}
			}
			pol_set = pol_clr;
			sig_set = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		ܐܢ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		ܐܢ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->addNotGate(NEW_ID, new_q, sig_q);
	}
	ܐܚܪܢܐ
	{
		ܐܢ (has_sr) {
			SigSpec not_clr;
			ܐܢ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = ܫܪܪܐ;
			} ܐܚܪܢܐ {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			ܐܢ (!pol_set) {
				sig_set = module->Not(NEW_ID, sig_set);
				pol_set = ܫܪܪܐ;
			}

			SigSpec masked_set = module->And(NEW_ID, sig_set, not_clr);
			لكل (auto bit: bits) {
				sig_set[bit] = sig_clr[bit];
				sig_clr[bit] = masked_set[bit];
			}
		}

		Const mask = Const(State::S0, width);
		لكل (auto bit: bits)
			mask.set(bit, State::S1);

		ܐܢ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, mask);
		ܐܢ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, mask);
		module->addXor(NEW_ID, new_q, mask, sig_q);
	}

	sig_q = new_q;
}
