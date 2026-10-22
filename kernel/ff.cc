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
سانچا<قسم_ناں داخلہ_Type, قسم_ناں OutputType, قسم_ناں = std::enable_if_t<std::is_ادھار_دا_v<Ff_Type_انکڑے, OutputType>>>
فراغ manufacture_info(داخلہ_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = خالی_سوچک;
	IdString type;
	constexpr منطقي have_cell = std::is_same_v<داخلہ_Type, Cell*>;
	إذا constexpr (std::is_same_v<داخلہ_Type, IdString>) {
		type = flop;
	} وإلا {
		static_assert(std::is_same_v<داخلہ_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	إذا constexpr (have_cell) {
		info.sig_q = cell->get_پورٹ(ID::Q);
		info.چوڑائی = Get_ناپ(info.sig_q);
		info.گن = cell->گن;
		إذا (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	إذا (type.وچ(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		إذا (type.وچ(ID($anyinit), ID($ff))) {
			info.has_gclk = سچ;
			إذا constexpr (have_cell)
				info.sig_d = cell->get_پورٹ(ID::D);
			إذا (type == ID($anyinit)) {
				info.is_anyinit = سچ;
				إذا constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} وإلا إذا (type == ID($sr)) {
			// No data input at all.
		} وإلا إذا (type.وچ(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = سچ;
			إذا constexpr (have_cell) {
				info.sig_aload = cell->get_پورٹ(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->get_پورٹ(ID::D);
			}
		} وإلا {
			info.has_clk = سچ;
			إذا constexpr (have_cell) {
				info.sig_clk = cell->get_پورٹ(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->get_پورٹ(ID::D);
			}
		}
		إذا (type.وچ(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = سچ;
			إذا constexpr (have_cell) {
				info.sig_ce = cell->get_پورٹ(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		إذا (type.وچ(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = سچ;
			إذا constexpr (have_cell) {
				info.sig_clr = cell->get_پورٹ(ID::CLR);
				info.sig_سیٹ = cell->get_پورٹ(ID::سیٹ_2);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_سیٹ = cell->getParam(ID::سیٹ_POLARITY).as_bool();
			}
		}
		إذا (type.وچ(ID($aldff), ID($aldffe))) {
			info.has_aload = سچ;
			إذا constexpr (have_cell) {
				info.sig_aload = cell->get_پورٹ(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->get_پورٹ(ID::AD);
			}
		}
		إذا (type.وچ(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = سچ;
			إذا constexpr (have_cell) {
				info.sig_arst = cell->get_پورٹ(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_قدر);
			}
		}
		إذا (type.وچ(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = سچ;
			إذا constexpr (have_cell) {
				info.sig_srst = cell->get_پورٹ(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_قدر);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} وإلا إذا (type == ID($_FF_)) {
		info.is_fine = سچ;
		info.has_gclk = سچ;
		إذا constexpr (have_cell)
			info.sig_d = cell->get_پورٹ(ID::D);
	} وإلا إذا (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = سچ;
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		إذا constexpr (have_cell) {
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 6) == "$_DFF_" && type_str.ناپ() == 8) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[6] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
		}
	} وإلا إذا (type_str.substr(0, 7) == "$_DFFE_" && type_str.ناپ() == 10) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = سچ;
		info.pol_ce = type_str[8] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 6) == "$_DFF_" && type_str.ناپ() == 10) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = سچ;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_arst = cell->get_پورٹ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 7) == "$_DFFE_" && type_str.ناپ() == 12) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = سچ;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = سچ;
		info.pol_ce = type_str[10] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_arst = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 8) == "$_ALDFF_" && type_str.ناپ() == 11) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = سچ;
		info.pol_aload = type_str[9] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_aload = cell->get_پورٹ(ID::L);
			info.sig_ad = cell->get_پورٹ(ID::AD);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.ناپ() == 13) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = سچ;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = سچ;
		info.pol_ce = type_str[11] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_aload = cell->get_پورٹ(ID::L);
			info.sig_ad = cell->get_پورٹ(ID::AD);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 8) == "$_DFFSR_" && type_str.ناپ() == 12) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.ناپ() == 14) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = سچ;
		info.pol_ce = type_str[12] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 7) == "$_SDFF_" && type_str.ناپ() == 11) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = سچ;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_srst = cell->get_پورٹ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 8) == "$_SDFFE_" && type_str.ناپ() == 13) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = سچ;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = سچ;
		info.pol_ce = type_str[11] == 'P';
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_srst = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.ناپ() == 14) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = سچ;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = سچ;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = سچ;
		إذا constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_srst = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_DLATCH_" && type_str.ناپ() == 11) {
		info.is_fine = سچ;
		info.has_aload = سچ;
		info.has_aload = سچ;
		info.pol_aload = type_str[9] == 'P';
		إذا constexpr (have_cell) {
			info.sig_ad = cell->get_پورٹ(ID::D);
			info.sig_aload = cell->get_پورٹ(ID::E);
		}
	} وإلا إذا (type_str.substr(0, 9) == "$_DLATCH_" && type_str.ناپ() == 13) {
		info.is_fine = سچ;
		info.has_aload = سچ;
		info.has_aload = سچ;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = سچ;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		إذا constexpr (have_cell) {
			info.sig_ad = cell->get_پورٹ(ID::D);
			info.sig_aload = cell->get_پورٹ(ID::E);
			info.sig_arst = cell->get_پورٹ(ID::R);
		}
	} وإلا إذا (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.ناپ() == 15) {
		info.is_fine = سچ;
		info.has_aload = سچ;
		info.has_aload = سچ;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		إذا constexpr (have_cell) {
			info.sig_ad = cell->get_پورٹ(ID::D);
			info.sig_aload = cell->get_پورٹ(ID::E);
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
		}
	} وإلا {
		log_assert(0);
	}
	إذا constexpr (have_cell)
		إذا (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = خطأ;
			info.has_arst = سچ;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_انکڑے::Ff_Type_انکڑے(IdString type) : Ff_Type_انکڑے()
{
	manufacture_info(type, *هذا, خالی_سوچک);
}

Ff_انکڑے::Ff_انکڑے(FfInitVals *initvals, Cell *cell_) : Ff_انکڑے(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *هذا, initvals);
}

Ff_انکڑے Ff_انکڑے::ٹکڑا(ثابت std::سمتیہ<صحيح> &بٹ) {
	Ff_انکڑے res(module, initvals, module->twines().جوڑو(NEW_ID));
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
	res.pol_سیٹ = pol_سیٹ;
	res.گن = گن;
	std::optional<Const::Builder> arst_بٹ;
	إذا (has_arst)
		arst_بٹ.emplace(بٹ.ناپ());
	std::optional<Const::Builder> srst_بٹ;
	إذا (has_srst)
		srst_بٹ.emplace(بٹ.ناپ());
	std::optional<Const::Builder> init_بٹ;
	إذا (initvals)
		init_بٹ.emplace(بٹ.ناپ());

	لكل (صحيح i : بٹ) {
		res.sig_q.append(sig_q[i]);
		إذا (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		إذا (has_aload)
			res.sig_ad.append(sig_ad[i]);
		إذا (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_سیٹ.append(sig_سیٹ[i]);
		}
		إذا (has_arst)
			arst_بٹ->ڈالو_پٹھ(val_arst[i]);
		إذا (has_srst)
			srst_بٹ->ڈالو_پٹھ(val_srst[i]);
		إذا (initvals)
			init_بٹ->ڈالو_پٹھ(val_init[i]);
	}

	إذا (has_arst)
		res.val_arst = arst_بٹ->build();
	إذا (has_srst)
		res.val_srst = srst_بٹ->build();
	إذا (initvals)
		res.val_init = init_بٹ->build();
	res.چوڑائی = Get_ناپ(res.sig_q);
	أرجع res;
}

فراغ Ff_انکڑے::جوڑو_dummy_ce() {
	إذا (has_ce)
		أرجع;
	has_ce = سچ;
	pol_ce = سچ;
	sig_ce = State::S1;
	ce_over_srst = خطأ;
}

فراغ Ff_انکڑے::جوڑو_dummy_srst() {
	إذا (has_srst)
		أرجع;
	has_srst = سچ;
	pol_srst = سچ;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, چوڑائی);
	ce_over_srst = خطأ;
}

فراغ Ff_انکڑے::جوڑو_dummy_arst() {
	إذا (has_arst)
		أرجع;
	has_arst = سچ;
	pol_arst = سچ;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, چوڑائی);
}

فراغ Ff_انکڑے::جوڑو_dummy_aload() {
	إذا (has_aload)
		أرجع;
	has_aload = سچ;
	pol_aload = سچ;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, چوڑائی);
}

فراغ Ff_انکڑے::جوڑو_dummy_sr() {
	إذا (has_sr)
		أرجع;
	has_sr = سچ;
	pol_clr = سچ;
	pol_سیٹ = سچ;
	sig_clr = Const(State::S0, چوڑائی);
	sig_سیٹ = Const(State::S0, چوڑائی);
}

فراغ Ff_انکڑے::جوڑو_dummy_clk() {
	إذا (has_clk)
		أرجع;
	has_clk = سچ;
	pol_clk = سچ;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, چوڑائی);
}

فراغ Ff_انکڑے::arst_منزل_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = سچ;
	has_arst = خطأ;
}

فراغ Ff_انکڑے::arst_منزل_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_سیٹ = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, چوڑائی);
	sig_سیٹ = Const(pol_arst ? State::S0 : State::S1, چوڑائی);
	has_sr = سچ;
	has_arst = خطأ;
	لكل (صحيح i = 0; i < چوڑائی; i++) {
		إذا (val_arst[i] == State::S1)
			sig_سیٹ[i] = sig_arst;
		وإلا
			sig_clr[i] = sig_arst;
	}
}

فراغ Ff_انکڑے::aload_منزل_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = سچ;
	has_aload = خطأ;
	إذا (!is_fine) {
		pol_clr = خطأ;
		pol_سیٹ = سچ;
		إذا (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, چوڑائی), sig_ad, sig_aload);
			sig_سیٹ = module->Mux(NEW_ID, Const(State::S0, چوڑائی), sig_ad, sig_aload);
		} وإلا {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, چوڑائی), sig_aload);
			sig_سیٹ = module->Mux(NEW_ID, sig_ad, Const(State::S0, چوڑائی), sig_aload);
		}
	} وإلا {
		pol_clr = pol_aload;
		pol_سیٹ = pol_aload;
		إذا (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_سیٹ = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} وإلا {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_سیٹ = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

فراغ Ff_انکڑے::convert_ce_over_srst(منطقي val) {
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

فراغ Ff_انکڑے::unmap_ce() {
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

فراغ Ff_انکڑے::unmap_srst() {
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

Cell *Ff_انکڑے::emit() {
	remove();
	إذا (!چوڑائی)
		أرجع خالی_سوچک;
	إذا (!has_aload && !has_clk && !has_gclk && !has_sr) {
		إذا (has_arst) {
			// Convert this case to a D latch.
			arst_منزل_aload();
		} وإلا {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			أرجع خالی_سوچک;
		}
	}
	إذا (initvals && !is_anyinit)
		initvals->سیٹ_init(sig_q, val_init);
	إذا (!is_fine) {
		إذا (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			إذا (is_anyinit) {
				cell = module->جوڑو_Anyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} وإلا {
				cell = module->جوڑو_Ff(name, sig_d, sig_q);
			}
		} وإلا إذا (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->جوڑو_Sr(name, sig_سیٹ, sig_clr, sig_q, pol_سیٹ, pol_clr);
		} وإلا إذا (!has_clk) {
			log_assert(!has_srst);
			إذا (has_sr)
				cell = module->جوڑو_Dlatchsr(name, sig_aload, sig_سیٹ, sig_clr, sig_ad, sig_q, pol_aload, pol_سیٹ, pol_clr);
			وإلا إذا (has_arst)
				cell = module->جوڑو_Adlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			وإلا
				cell = module->جوڑو_Dlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} وإلا {
			إذا (has_sr) {
				إذا (has_ce)
					cell = module->جوڑو_Dffsre(name, sig_clk, sig_ce, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_سیٹ, pol_clr);
				وإلا
					cell = module->جوڑو_Dffsr(name, sig_clk, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_سیٹ, pol_clr);
			} وإلا إذا (has_arst) {
				إذا (has_ce)
					cell = module->جوڑو_Adffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				وإلا
					cell = module->جوڑو_Adff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} وإلا إذا (has_aload) {
				إذا (has_ce)
					cell = module->جوڑو_Aldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				وإلا
					cell = module->جوڑو_Aldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} وإلا إذا (has_srst) {
				إذا (has_ce)
					إذا (ce_over_srst)
						cell = module->جوڑو_Sdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					وإلا
						cell = module->جوڑو_Sdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				وإلا
					cell = module->جوڑو_Sdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} وإلا {
				إذا (has_ce)
					cell = module->جوڑو_Dffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				وإلا
					cell = module->جوڑو_Dff(name, sig_clk, sig_d, sig_q, pol_clk);
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
			cell = module->جوڑو_Ff_Gate(name, sig_d, sig_q);
		} وإلا إذا (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->جوڑو_Sr_Gate(name, sig_سیٹ, sig_clr, sig_q, pol_سیٹ, pol_clr);
		} وإلا إذا (!has_clk) {
			log_assert(!has_srst);
			إذا (has_sr)
				cell = module->جوڑو_Dlatchsr_Gate(name, sig_aload, sig_سیٹ, sig_clr, sig_ad, sig_q, pol_aload, pol_سیٹ, pol_clr);
			وإلا إذا (has_arst)
				cell = module->جوڑو_Adlatch_Gate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			وإلا
				cell = module->جوڑو_Dlatch_Gate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} وإلا {
			إذا (has_sr) {
				إذا (has_ce)
					cell = module->جوڑو_Dffsre_Gate(name, sig_clk, sig_ce, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_سیٹ, pol_clr);
				وإلا
					cell = module->جوڑو_Dffsr_Gate(name, sig_clk, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_سیٹ, pol_clr);
			} وإلا إذا (has_arst) {
				إذا (has_ce)
					cell = module->جوڑو_Adffe_Gate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				وإلا
					cell = module->جوڑو_Adff_Gate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} وإلا إذا (has_aload) {
				إذا (has_ce)
					cell = module->جوڑو_Aldffe_Gate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				وإلا
					cell = module->جوڑو_Aldff_Gate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} وإلا إذا (has_srst) {
				إذا (has_ce)
					إذا (ce_over_srst)
						cell = module->جوڑو_Sdffce_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					وإلا
						cell = module->جوڑو_Sdffe_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				وإلا
					cell = module->جوڑو_Sdff_Gate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} وإلا {
				إذا (has_ce)
					cell = module->جوڑو_Dffe_Gate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				وإلا
					cell = module->جوڑو_Dff_Gate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->گن = گن;
	أرجع cell;
}

فراغ Ff_انکڑے::remove() {
	إذا (cell) {
		remove_init();
		module->remove(cell);
		cell = خالی_سوچک;
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

فراغ Ff_انکڑے::flip_rst_بٹ(ثابت pool<صحيح> &بٹ) {
	إذا (!بٹ.ناپ())
		أرجع;

	remove_init();

	لكل (خودکار ثنائی_ہندسہ: بٹ) {
		إذا (has_arst)
			val_arst.سیٹ(ثنائی_ہندسہ, invert(val_arst[ثنائی_ہندسہ]));
		إذا (has_srst)
			val_srst.سیٹ(ثنائی_ہندسہ, invert(val_srst[ثنائی_ہندسہ]));
		val_init.سیٹ(ثنائی_ہندسہ, invert(val_init[ثنائی_ہندسہ]));
	}
}

فراغ Ff_انکڑے::flip_بٹ(ثابت pool<صحيح> &بٹ) {
	إذا (!بٹ.ناپ())
		أرجع;

	flip_rst_بٹ(بٹ);

	Wire *new_q = module->جوڑو_Wire(NEW_ID, چوڑائی);

	إذا (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	إذا (is_fine) {
		إذا (has_sr) {
			منطقي new_pol_clr = pol_سیٹ;
			SigSpec new_sig_clr;
			إذا (pol_سیٹ) {
				إذا (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_سیٹ, sig_clr);
				} وإلا {
					new_sig_clr = module->AndGate(NEW_ID, sig_سیٹ, sig_clr);
				}
			} وإلا {
				إذا (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_سیٹ, sig_clr);
				} وإلا {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_سیٹ, sig_clr);
				}
			}
			pol_سیٹ = pol_clr;
			sig_سیٹ = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		إذا (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		إذا (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->جوڑو_Not_Gate(NEW_ID, new_q, sig_q);
	}
	وإلا
	{
		إذا (has_sr) {
			SigSpec not_clr;
			إذا (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = سچ;
			} وإلا {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			إذا (!pol_سیٹ) {
				sig_سیٹ = module->Not(NEW_ID, sig_سیٹ);
				pol_سیٹ = سچ;
			}

			SigSpec masked_سیٹ = module->And(NEW_ID, sig_سیٹ, not_clr);
			لكل (خودکار ثنائی_ہندسہ: بٹ) {
				sig_سیٹ[ثنائی_ہندسہ] = sig_clr[ثنائی_ہندسہ];
				sig_clr[ثنائی_ہندسہ] = masked_سیٹ[ثنائی_ہندسہ];
			}
		}

		Const نقاب = Const(State::S0, چوڑائی);
		لكل (خودکار ثنائی_ہندسہ: بٹ)
			نقاب.سیٹ(ثنائی_ہندسہ, State::S1);

		إذا (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, نقاب);
		إذا (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, نقاب);
		module->جوڑو_Xor(NEW_ID, new_q, نقاب, sig_q);
	}

	sig_q = new_q;
}
