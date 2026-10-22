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
کينډۍ<ډول_نوم ننوت_Type, ډول_نوم OutputType, ډول_نوم = std::enable_if_t<std::is_بنسټ_د_v<Ff_Type_معلومات, OutputType>>>
فراغ manufacture_info(ننوت_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = تش_اشاره;
	IdString type;
	constexpr منطقي have_cell = std::is_same_v<ننوت_Type, Cell*>;
	إذا constexpr (std::is_same_v<ننوت_Type, IdString>) {
		type = flop;
	} وإلا {
		static_assert(std::is_same_v<ننوت_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	إذا constexpr (have_cell) {
		info.sig_q = cell->get_پورټ(ID::Q);
		info.پلنوالی = Get_اندازه(info.sig_q);
		info.ځانګړتیاوې = cell->ځانګړتیاوې;
		إذا (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	إذا (type.دننه(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		إذا (type.دننه(ID($anyinit), ID($ff))) {
			info.has_gclk = ریښتیا;
			إذا constexpr (have_cell)
				info.sig_d = cell->get_پورټ(ID::D);
			إذا (type == ID($anyinit)) {
				info.is_anyinit = ریښتیا;
				إذا constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} وإلا إذا (type == ID($sr)) {
			// No data input at all.
		} وإلا إذا (type.دننه(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = ریښتیا;
			إذا constexpr (have_cell) {
				info.sig_aload = cell->get_پورټ(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->get_پورټ(ID::D);
			}
		} وإلا {
			info.has_clk = ریښتیا;
			إذا constexpr (have_cell) {
				info.sig_clk = cell->get_پورټ(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->get_پورټ(ID::D);
			}
		}
		إذا (type.دننه(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = ریښتیا;
			إذا constexpr (have_cell) {
				info.sig_ce = cell->get_پورټ(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		إذا (type.دننه(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = ریښتیا;
			إذا constexpr (have_cell) {
				info.sig_clr = cell->get_پورټ(ID::CLR);
				info.sig_سېټ = cell->get_پورټ(ID::سېټ_2);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_سېټ = cell->getParam(ID::سېټ_POLARITY).as_bool();
			}
		}
		إذا (type.دننه(ID($aldff), ID($aldffe))) {
			info.has_aload = ریښتیا;
			إذا constexpr (have_cell) {
				info.sig_aload = cell->get_پورټ(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->get_پورټ(ID::AD);
			}
		}
		إذا (type.دننه(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = ریښتیا;
			إذا constexpr (have_cell) {
				info.sig_arst = cell->get_پورټ(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_ارزښت);
			}
		}
		إذا (type.دننه(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = ریښتیا;
			إذا constexpr (have_cell) {
				info.sig_srst = cell->get_پورټ(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_ارزښت);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} وإلا إذا (type == ID($_FF_)) {
		info.is_fine = ریښتیا;
		info.has_gclk = ریښتیا;
		إذا constexpr (have_cell)
			info.sig_d = cell->get_پورټ(ID::D);
	} وإلا إذا (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = ریښتیا;
		info.has_sr = ریښتیا;
		info.pol_سېټ = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		إذا constexpr (have_cell) {
			info.sig_سېټ = cell->get_پورټ(ID::S);
			info.sig_clr = cell->get_پورټ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 6) == "$_DFF_" && type_str.اندازه() == 8) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[6] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
		}
	} وإلا إذا (type_str.substr(0, 7) == "$_DFFE_" && type_str.اندازه() == 10) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = ریښتیا;
		info.pol_ce = type_str[8] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_ce = cell->get_پورټ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 6) == "$_DFF_" && type_str.اندازه() == 10) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = ریښتیا;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_arst = cell->get_پورټ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 7) == "$_DFFE_" && type_str.اندازه() == 12) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = ریښتیا;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = ریښتیا;
		info.pol_ce = type_str[10] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_arst = cell->get_پورټ(ID::R);
			info.sig_ce = cell->get_پورټ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 8) == "$_ALDFF_" && type_str.اندازه() == 11) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = ریښتیا;
		info.pol_aload = type_str[9] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_aload = cell->get_پورټ(ID::L);
			info.sig_ad = cell->get_پورټ(ID::AD);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.اندازه() == 13) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = ریښتیا;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = ریښتیا;
		info.pol_ce = type_str[11] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_aload = cell->get_پورټ(ID::L);
			info.sig_ad = cell->get_پورټ(ID::AD);
			info.sig_ce = cell->get_پورټ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 8) == "$_DFFSR_" && type_str.اندازه() == 12) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = ریښتیا;
		info.pol_سېټ = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_سېټ = cell->get_پورټ(ID::S);
			info.sig_clr = cell->get_پورټ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.اندازه() == 14) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = ریښتیا;
		info.pol_سېټ = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = ریښتیا;
		info.pol_ce = type_str[12] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_سېټ = cell->get_پورټ(ID::S);
			info.sig_clr = cell->get_پورټ(ID::R);
			info.sig_ce = cell->get_پورټ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 7) == "$_SDFF_" && type_str.اندازه() == 11) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = ریښتیا;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_srst = cell->get_پورټ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 8) == "$_SDFFE_" && type_str.اندازه() == 13) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = ریښتیا;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = ریښتیا;
		info.pol_ce = type_str[11] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_srst = cell->get_پورټ(ID::R);
			info.sig_ce = cell->get_پورټ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.اندازه() == 14) {
		info.is_fine = ریښتیا;
		info.has_clk = ریښتیا;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = ریښتیا;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = ریښتیا;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = ریښتیا;
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورټ(ID::D);
			info.sig_clk = cell->get_پورټ(ID::C);
			info.sig_srst = cell->get_پورټ(ID::R);
			info.sig_ce = cell->get_پورټ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_DLATCH_" && type_str.اندازه() == 11) {
		info.is_fine = ریښتیا;
		info.has_aload = ریښتیا;
		info.has_aload = ریښتیا;
		info.pol_aload = type_str[9] == 'P';
		إذا constexpr (have_cell) {
			info.sig_ad = cell->get_پورټ(ID::D);
			info.sig_aload = cell->get_پورټ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_DLATCH_" && type_str.اندازه() == 13) {
		info.is_fine = ریښتیا;
		info.has_aload = ریښتیا;
		info.has_aload = ریښتیا;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = ریښتیا;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		إذا constexpr (have_cell) {
			info.sig_ad = cell->get_پورټ(ID::D);
			info.sig_aload = cell->get_پورټ(ID::E);
			info.sig_arst = cell->get_پورټ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.اندازه() == 15) {
		info.is_fine = ریښتیا;
		info.has_aload = ریښتیا;
		info.has_aload = ریښتیا;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = ریښتیا;
		info.pol_سېټ = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		إذا constexpr (have_cell) {
			info.sig_ad = cell->get_پورټ(ID::D);
			info.sig_aload = cell->get_پورټ(ID::E);
			info.sig_سېټ = cell->get_پورټ(ID::S);
			info.sig_clr = cell->get_پورټ(ID::R);
		}
	} وإلا {
		log_assert(0);
	}
	إذا constexpr (have_cell)
		إذا (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = خطأ;
			info.has_arst = ریښتیا;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_معلومات::Ff_Type_معلومات(IdString type) : Ff_Type_معلومات()
{
	manufacture_info(type, *هذا, تش_اشاره);
}

Ff_معلومات::Ff_معلومات(FfInitVals *initvals, Cell *cell_) : Ff_معلومات(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *هذا, initvals);
}

Ff_معلومات Ff_معلومات::ټوټه(ثابت std::ویکټور<صحيح> &بټونه) {
	Ff_معلومات res(module, initvals, module->twines().جمع_کړه(NEW_ID));
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
	res.pol_سېټ = pol_سېټ;
	res.ځانګړتیاوې = ځانګړتیاوې;
	std::optional<Const::Builder> arst_بټونه;
	إذا (has_arst)
		arst_بټونه.emplace(بټونه.اندازه());
	std::optional<Const::Builder> srst_بټونه;
	إذا (has_srst)
		srst_بټونه.emplace(بټونه.اندازه());
	std::optional<Const::Builder> init_بټونه;
	إذا (initvals)
		init_بټونه.emplace(بټونه.اندازه());

	لكل (صحيح i : بټونه) {
		res.sig_q.append(sig_q[i]);
		إذا (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		إذا (has_aload)
			res.sig_ad.append(sig_ad[i]);
		إذا (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_سېټ.append(sig_سېټ[i]);
		}
		إذا (has_arst)
			arst_بټونه->ټېل_شا(val_arst[i]);
		إذا (has_srst)
			srst_بټونه->ټېل_شا(val_srst[i]);
		إذا (initvals)
			init_بټونه->ټېل_شا(val_init[i]);
	}

	إذا (has_arst)
		res.val_arst = arst_بټونه->build();
	إذا (has_srst)
		res.val_srst = srst_بټونه->build();
	إذا (initvals)
		res.val_init = init_بټونه->build();
	res.پلنوالی = Get_اندازه(res.sig_q);
	أرجع res;
}

فراغ Ff_معلومات::جمع_کړه_dummy_ce() {
	إذا (has_ce)
		أرجع;
	has_ce = ریښتیا;
	pol_ce = ریښتیا;
	sig_ce = State::S1;
	ce_over_srst = خطأ;
}

فراغ Ff_معلومات::جمع_کړه_dummy_srst() {
	إذا (has_srst)
		أرجع;
	has_srst = ریښتیا;
	pol_srst = ریښتیا;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, پلنوالی);
	ce_over_srst = خطأ;
}

فراغ Ff_معلومات::جمع_کړه_dummy_arst() {
	إذا (has_arst)
		أرجع;
	has_arst = ریښتیا;
	pol_arst = ریښتیا;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, پلنوالی);
}

فراغ Ff_معلومات::جمع_کړه_dummy_aload() {
	إذا (has_aload)
		أرجع;
	has_aload = ریښتیا;
	pol_aload = ریښتیا;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, پلنوالی);
}

فراغ Ff_معلومات::جمع_کړه_dummy_sr() {
	إذا (has_sr)
		أرجع;
	has_sr = ریښتیا;
	pol_clr = ریښتیا;
	pol_سېټ = ریښتیا;
	sig_clr = Const(State::S0, پلنوالی);
	sig_سېټ = Const(State::S0, پلنوالی);
}

فراغ Ff_معلومات::جمع_کړه_dummy_clk() {
	إذا (has_clk)
		أرجع;
	has_clk = ریښتیا;
	pol_clk = ریښتیا;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, پلنوالی);
}

فراغ Ff_معلومات::arst_منزل_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = ریښتیا;
	has_arst = خطأ;
}

فراغ Ff_معلومات::arst_منزل_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_سېټ = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, پلنوالی);
	sig_سېټ = Const(pol_arst ? State::S0 : State::S1, پلنوالی);
	has_sr = ریښتیا;
	has_arst = خطأ;
	لكل (صحيح i = 0; i < پلنوالی; i++) {
		إذا (val_arst[i] == State::S1)
			sig_سېټ[i] = sig_arst;
		وإلا
			sig_clr[i] = sig_arst;
	}
}

فراغ Ff_معلومات::aload_منزل_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = ریښتیا;
	has_aload = خطأ;
	إذا (!is_fine) {
		pol_clr = خطأ;
		pol_سېټ = ریښتیا;
		إذا (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, پلنوالی), sig_ad, sig_aload);
			sig_سېټ = module->Mux(NEW_ID, Const(State::S0, پلنوالی), sig_ad, sig_aload);
		} وإلا {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, پلنوالی), sig_aload);
			sig_سېټ = module->Mux(NEW_ID, sig_ad, Const(State::S0, پلنوالی), sig_aload);
		}
	} وإلا {
		pol_clr = pol_aload;
		pol_سېټ = pol_aload;
		إذا (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_سېټ = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} وإلا {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_سېټ = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

فراغ Ff_معلومات::convert_ce_over_srst(منطقي val) {
	إذا (!has_ce || !has_srst || ce_over_srst == val)
		أرجع;
	إذا (val) {
		// sdffe to sdffce
		إذا (!is_fine) {
			إذا (pol_ce) {
				إذا (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} وإلا {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} وإلا {
				إذا (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} وإلا {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} وإلا {
			إذا (pol_ce) {
				إذا (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} وإلا {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} وإلا {
				إذا (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} وإلا {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} وإلا {
		// sdffce to sdffe
		إذا (!is_fine) {
			إذا (pol_srst) {
				إذا (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} وإلا {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} وإلا {
				إذا (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} وإلا {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} وإلا {
			إذا (pol_srst) {
				إذا (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} وإلا {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} وإلا {
				إذا (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} وإلا {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

فراغ Ff_معلومات::unmap_ce() {
	إذا (!has_ce)
		أرجع;
	log_assert(has_clk);
	إذا (has_srst && ce_over_srst)
		unmap_srst();

	إذا (!is_fine) {
		إذا (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		وإلا
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} وإلا {
		إذا (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		وإلا
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = خطأ;
}

فراغ Ff_معلومات::unmap_srst() {
	إذا (!has_srst)
		أرجع;
	إذا (has_ce && !ce_over_srst)
		unmap_ce();

	إذا (!is_fine) {
		إذا (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		وإلا
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} وإلا {
		إذا (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		وإلا
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = خطأ;
}

Cell *Ff_معلومات::emit() {
	remove();
	إذا (!پلنوالی)
		أرجع تش_اشاره;
	إذا (!has_aload && !has_clk && !has_gclk && !has_sr) {
		إذا (has_arst) {
			// Convert this case to a D latch.
			arst_منزل_aload();
		} وإلا {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			أرجع تش_اشاره;
		}
	}
	إذا (initvals && !is_anyinit)
		initvals->سېټ_init(sig_q, val_init);
	إذا (!is_fine) {
		إذا (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			إذا (is_anyinit) {
				cell = module->جمع_کړه_Anyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} وإلا {
				cell = module->جمع_کړه_Ff(name, sig_d, sig_q);
			}
		} وإلا إذا (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->جمع_کړه_Sr(name, sig_سېټ, sig_clr, sig_q, pol_سېټ, pol_clr);
		} وإلا إذا (!has_clk) {
			log_assert(!has_srst);
			إذا (has_sr)
				cell = module->جمع_کړه_Dlatchsr(name, sig_aload, sig_سېټ, sig_clr, sig_ad, sig_q, pol_aload, pol_سېټ, pol_clr);
			وإلا إذا (has_arst)
				cell = module->جمع_کړه_Adlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			وإلا
				cell = module->جمع_کړه_Dlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} وإلا {
			إذا (has_sr) {
				إذا (has_ce)
					cell = module->جمع_کړه_Dffsre(name, sig_clk, sig_ce, sig_سېټ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_سېټ, pol_clr);
				وإلا
					cell = module->جمع_کړه_Dffsr(name, sig_clk, sig_سېټ, sig_clr, sig_d, sig_q, pol_clk, pol_سېټ, pol_clr);
			} وإلا إذا (has_arst) {
				إذا (has_ce)
					cell = module->جمع_کړه_Adffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				وإلا
					cell = module->جمع_کړه_Adff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} وإلا إذا (has_aload) {
				إذا (has_ce)
					cell = module->جمع_کړه_Aldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				وإلا
					cell = module->جمع_کړه_Aldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} وإلا إذا (has_srst) {
				إذا (has_ce)
					إذا (ce_over_srst)
						cell = module->جمع_کړه_Sdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					وإلا
						cell = module->جمع_کړه_Sdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				وإلا
					cell = module->جمع_کړه_Sdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} وإلا {
				إذا (has_ce)
					cell = module->جمع_کړه_Dffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				وإلا
					cell = module->جمع_کړه_Dff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} وإلا {
		إذا (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->جمع_کړه_Ff_Gate(name, sig_d, sig_q);
		} وإلا إذا (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->جمع_کړه_Sr_Gate(name, sig_سېټ, sig_clr, sig_q, pol_سېټ, pol_clr);
		} وإلا إذا (!has_clk) {
			log_assert(!has_srst);
			إذا (has_sr)
				cell = module->جمع_کړه_Dlatchsr_Gate(name, sig_aload, sig_سېټ, sig_clr, sig_ad, sig_q, pol_aload, pol_سېټ, pol_clr);
			وإلا إذا (has_arst)
				cell = module->جمع_کړه_Adlatch_Gate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			وإلا
				cell = module->جمع_کړه_Dlatch_Gate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} وإلا {
			إذا (has_sr) {
				إذا (has_ce)
					cell = module->جمع_کړه_Dffsre_Gate(name, sig_clk, sig_ce, sig_سېټ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_سېټ, pol_clr);
				وإلا
					cell = module->جمع_کړه_Dffsr_Gate(name, sig_clk, sig_سېټ, sig_clr, sig_d, sig_q, pol_clk, pol_سېټ, pol_clr);
			} وإلا إذا (has_arst) {
				إذا (has_ce)
					cell = module->جمع_کړه_Adffe_Gate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				وإلا
					cell = module->جمع_کړه_Adff_Gate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} وإلا إذا (has_aload) {
				إذا (has_ce)
					cell = module->جمع_کړه_Aldffe_Gate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				وإلا
					cell = module->جمع_کړه_Aldff_Gate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} وإلا إذا (has_srst) {
				إذا (has_ce)
					إذا (ce_over_srst)
						cell = module->جمع_کړه_Sdffce_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					وإلا
						cell = module->جمع_کړه_Sdffe_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				وإلا
					cell = module->جمع_کړه_Sdff_Gate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} وإلا {
				إذا (has_ce)
					cell = module->جمع_کړه_Dffe_Gate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				وإلا
					cell = module->جمع_کړه_Dff_Gate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->ځانګړتیاوې = ځانګړتیاوې;
	أرجع cell;
}

فراغ Ff_معلومات::remove() {
	إذا (cell) {
		remove_init();
		module->remove(cell);
		cell = تش_اشاره;
	}
}

نطاق {
	State invert(State s) {
		بدل (s) {
			حالة State::S0: أرجع State::S1;
			حالة State::S1: أرجع State::S0;
			افتراضي: أرجع s;
		}
	}
}

فراغ Ff_معلومات::flip_rst_بټونه(ثابت pool<صحيح> &بټونه) {
	إذا (!بټونه.اندازه())
		أرجع;

	remove_init();

	لكل (خپلکاری بټ: بټونه) {
		إذا (has_arst)
			val_arst.سېټ(بټ, invert(val_arst[بټ]));
		إذا (has_srst)
			val_srst.سېټ(بټ, invert(val_srst[بټ]));
		val_init.سېټ(بټ, invert(val_init[بټ]));
	}
}

فراغ Ff_معلومات::flip_بټونه(ثابت pool<صحيح> &بټونه) {
	إذا (!بټونه.اندازه())
		أرجع;

	flip_rst_بټونه(بټونه);

	Wire *new_q = module->جمع_کړه_Wire(NEW_ID, پلنوالی);

	إذا (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	إذا (is_fine) {
		إذا (has_sr) {
			منطقي new_pol_clr = pol_سېټ;
			SigSpec new_sig_clr;
			إذا (pol_سېټ) {
				إذا (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_سېټ, sig_clr);
				} وإلا {
					new_sig_clr = module->AndGate(NEW_ID, sig_سېټ, sig_clr);
				}
			} وإلا {
				إذا (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_سېټ, sig_clr);
				} وإلا {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_سېټ, sig_clr);
				}
			}
			pol_سېټ = pol_clr;
			sig_سېټ = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		إذا (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		إذا (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->جمع_کړه_Not_Gate(NEW_ID, new_q, sig_q);
	}
	وإلا
	{
		إذا (has_sr) {
			SigSpec not_clr;
			إذا (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = ریښتیا;
			} وإلا {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			إذا (!pol_سېټ) {
				sig_سېټ = module->Not(NEW_ID, sig_سېټ);
				pol_سېټ = ریښتیا;
			}

			SigSpec masked_سېټ = module->And(NEW_ID, sig_سېټ, not_clr);
			لكل (خپلکاری بټ: بټونه) {
				sig_سېټ[بټ] = sig_clr[بټ];
				sig_clr[بټ] = masked_سېټ[بټ];
			}
		}

		Const ماسک = Const(State::S0, پلنوالی);
		لكل (خپلکاری بټ: بټونه)
			ماسک.سېټ(بټ, State::S1);

		إذا (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, ماسک);
		إذا (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, ماسک);
		module->جمع_کړه_Xor(NEW_ID, new_q, ماسک, sig_q);
	}

	sig_q = new_q;
}
