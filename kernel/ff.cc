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
سانچہ<قسم_نام داخلہ_Type, قسم_نام OutputType, قسم_نام = std::enable_if_t<std::is_بنیاد_کا_v<Ff_Type_ڈیٹا, OutputType>>>
خالی manufacture_info(داخلہ_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = خالی_اشاریہ;
	IdString type;
	constexpr منطقی have_cell = std::is_same_v<داخلہ_Type, Cell*>;
	اگر constexpr (std::is_same_v<داخلہ_Type, IdString>) {
		type = flop;
	} ورنہ {
		static_assert(std::is_same_v<داخلہ_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	اگر constexpr (have_cell) {
		info.sig_q = cell->get_پورٹ(ID::Q);
		info.چوڑائی = Get_سائز(info.sig_q);
		info.اوصاف = cell->اوصاف;
		اگر (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	اگر (type.اندر(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		اگر (type.اندر(ID($anyinit), ID($ff))) {
			info.has_gclk = سچ;
			اگر constexpr (have_cell)
				info.sig_d = cell->get_پورٹ(ID::D);
			اگر (type == ID($anyinit)) {
				info.is_anyinit = سچ;
				اگر constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} ورنہ اگر (type == ID($sr)) {
			// No data input at all.
		} ورنہ اگر (type.اندر(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = سچ;
			اگر constexpr (have_cell) {
				info.sig_aload = cell->get_پورٹ(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->get_پورٹ(ID::D);
			}
		} ورنہ {
			info.has_clk = سچ;
			اگر constexpr (have_cell) {
				info.sig_clk = cell->get_پورٹ(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->get_پورٹ(ID::D);
			}
		}
		اگر (type.اندر(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = سچ;
			اگر constexpr (have_cell) {
				info.sig_ce = cell->get_پورٹ(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		اگر (type.اندر(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = سچ;
			اگر constexpr (have_cell) {
				info.sig_clr = cell->get_پورٹ(ID::CLR);
				info.sig_سیٹ = cell->get_پورٹ(ID::سیٹ_2);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_سیٹ = cell->getParam(ID::سیٹ_POLARITY).as_bool();
			}
		}
		اگر (type.اندر(ID($aldff), ID($aldffe))) {
			info.has_aload = سچ;
			اگر constexpr (have_cell) {
				info.sig_aload = cell->get_پورٹ(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->get_پورٹ(ID::AD);
			}
		}
		اگر (type.اندر(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = سچ;
			اگر constexpr (have_cell) {
				info.sig_arst = cell->get_پورٹ(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_قدر);
			}
		}
		اگر (type.اندر(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = سچ;
			اگر constexpr (have_cell) {
				info.sig_srst = cell->get_پورٹ(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_قدر);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} ورنہ اگر (type == ID($_FF_)) {
		info.is_fine = سچ;
		info.has_gclk = سچ;
		اگر constexpr (have_cell)
			info.sig_d = cell->get_پورٹ(ID::D);
	} ورنہ اگر (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = سچ;
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		اگر constexpr (have_cell) {
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
		}
	} ورنہ اگر (type_str.substr(0, 6) == "$_DFF_" && type_str.سائز() == 8) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[6] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
		}
	} ورنہ اگر (type_str.substr(0, 7) == "$_DFFE_" && type_str.سائز() == 10) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = سچ;
		info.pol_ce = type_str[8] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} ورنہ اگر (type_str.substr(0, 6) == "$_DFF_" && type_str.سائز() == 10) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = سچ;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_arst = cell->get_پورٹ(ID::R);
		}
	} ورنہ اگر (type_str.substr(0, 7) == "$_DFFE_" && type_str.سائز() == 12) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = سچ;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = سچ;
		info.pol_ce = type_str[10] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_arst = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} ورنہ اگر (type_str.substr(0, 8) == "$_ALDFF_" && type_str.سائز() == 11) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = سچ;
		info.pol_aload = type_str[9] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_aload = cell->get_پورٹ(ID::L);
			info.sig_ad = cell->get_پورٹ(ID::AD);
		}
	} ورنہ اگر (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.سائز() == 13) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = سچ;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = سچ;
		info.pol_ce = type_str[11] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_aload = cell->get_پورٹ(ID::L);
			info.sig_ad = cell->get_پورٹ(ID::AD);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} ورنہ اگر (type_str.substr(0, 8) == "$_DFFSR_" && type_str.سائز() == 12) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
		}
	} ورنہ اگر (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.سائز() == 14) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = سچ;
		info.pol_ce = type_str[12] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} ورنہ اگر (type_str.substr(0, 7) == "$_SDFF_" && type_str.سائز() == 11) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = سچ;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_srst = cell->get_پورٹ(ID::R);
		}
	} ورنہ اگر (type_str.substr(0, 8) == "$_SDFFE_" && type_str.سائز() == 13) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = سچ;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = سچ;
		info.pol_ce = type_str[11] == 'P';
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_srst = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} ورنہ اگر (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.سائز() == 14) {
		info.is_fine = سچ;
		info.has_clk = سچ;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = سچ;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = سچ;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = سچ;
		اگر constexpr (have_cell) {
			info.sig_d = cell->get_پورٹ(ID::D);
			info.sig_clk = cell->get_پورٹ(ID::C);
			info.sig_srst = cell->get_پورٹ(ID::R);
			info.sig_ce = cell->get_پورٹ(ID::E);
		}
	} ورنہ اگر (type_str.substr(0, 9) == "$_DLATCH_" && type_str.سائز() == 11) {
		info.is_fine = سچ;
		info.has_aload = سچ;
		info.has_aload = سچ;
		info.pol_aload = type_str[9] == 'P';
		اگر constexpr (have_cell) {
			info.sig_ad = cell->get_پورٹ(ID::D);
			info.sig_aload = cell->get_پورٹ(ID::E);
		}
	} ورنہ اگر (type_str.substr(0, 9) == "$_DLATCH_" && type_str.سائز() == 13) {
		info.is_fine = سچ;
		info.has_aload = سچ;
		info.has_aload = سچ;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = سچ;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		اگر constexpr (have_cell) {
			info.sig_ad = cell->get_پورٹ(ID::D);
			info.sig_aload = cell->get_پورٹ(ID::E);
			info.sig_arst = cell->get_پورٹ(ID::R);
		}
	} ورنہ اگر (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.سائز() == 15) {
		info.is_fine = سچ;
		info.has_aload = سچ;
		info.has_aload = سچ;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = سچ;
		info.pol_سیٹ = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		اگر constexpr (have_cell) {
			info.sig_ad = cell->get_پورٹ(ID::D);
			info.sig_aload = cell->get_پورٹ(ID::E);
			info.sig_سیٹ = cell->get_پورٹ(ID::S);
			info.sig_clr = cell->get_پورٹ(ID::R);
		}
	} ورنہ {
		log_assert(0);
	}
	اگر constexpr (have_cell)
		اگر (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = جھوٹ;
			info.has_arst = سچ;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_ڈیٹا::Ff_Type_ڈیٹا(IdString type) : Ff_Type_ڈیٹا()
{
	manufacture_info(type, *یہ, خالی_اشاریہ);
}

Ff_ڈیٹا::Ff_ڈیٹا(FfInitVals *initvals, Cell *cell_) : Ff_ڈیٹا(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *یہ, initvals);
}

Ff_ڈیٹا Ff_ڈیٹا::قاش(ثابت std::سمتیہ<صحیح> &بٹ) {
	Ff_ڈیٹا res(module, initvals, module->twines().جوڑو(NEW_ID));
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
	res.اوصاف = اوصاف;
	std::optional<Const::Builder> arst_بٹ;
	اگر (has_arst)
		arst_بٹ.emplace(بٹ.سائز());
	std::optional<Const::Builder> srst_بٹ;
	اگر (has_srst)
		srst_بٹ.emplace(بٹ.سائز());
	std::optional<Const::Builder> init_بٹ;
	اگر (initvals)
		init_بٹ.emplace(بٹ.سائز());

	برائے (صحیح i : بٹ) {
		res.sig_q.append(sig_q[i]);
		اگر (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		اگر (has_aload)
			res.sig_ad.append(sig_ad[i]);
		اگر (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_سیٹ.append(sig_سیٹ[i]);
		}
		اگر (has_arst)
			arst_بٹ->ڈالو_عقب(val_arst[i]);
		اگر (has_srst)
			srst_بٹ->ڈالو_عقب(val_srst[i]);
		اگر (initvals)
			init_بٹ->ڈالو_عقب(val_init[i]);
	}

	اگر (has_arst)
		res.val_arst = arst_بٹ->build();
	اگر (has_srst)
		res.val_srst = srst_بٹ->build();
	اگر (initvals)
		res.val_init = init_بٹ->build();
	res.چوڑائی = Get_سائز(res.sig_q);
	واپس res;
}

خالی Ff_ڈیٹا::جوڑو_dummy_ce() {
	اگر (has_ce)
		واپس;
	has_ce = سچ;
	pol_ce = سچ;
	sig_ce = State::S1;
	ce_over_srst = جھوٹ;
}

خالی Ff_ڈیٹا::جوڑو_dummy_srst() {
	اگر (has_srst)
		واپس;
	has_srst = سچ;
	pol_srst = سچ;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, چوڑائی);
	ce_over_srst = جھوٹ;
}

خالی Ff_ڈیٹا::جوڑو_dummy_arst() {
	اگر (has_arst)
		واپس;
	has_arst = سچ;
	pol_arst = سچ;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, چوڑائی);
}

خالی Ff_ڈیٹا::جوڑو_dummy_aload() {
	اگر (has_aload)
		واپس;
	has_aload = سچ;
	pol_aload = سچ;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, چوڑائی);
}

خالی Ff_ڈیٹا::جوڑو_dummy_sr() {
	اگر (has_sr)
		واپس;
	has_sr = سچ;
	pol_clr = سچ;
	pol_سیٹ = سچ;
	sig_clr = Const(State::S0, چوڑائی);
	sig_سیٹ = Const(State::S0, چوڑائی);
}

خالی Ff_ڈیٹا::جوڑو_dummy_clk() {
	اگر (has_clk)
		واپس;
	has_clk = سچ;
	pol_clk = سچ;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, چوڑائی);
}

خالی Ff_ڈیٹا::arst_مقصد_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = سچ;
	has_arst = جھوٹ;
}

خالی Ff_ڈیٹا::arst_مقصد_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_سیٹ = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, چوڑائی);
	sig_سیٹ = Const(pol_arst ? State::S0 : State::S1, چوڑائی);
	has_sr = سچ;
	has_arst = جھوٹ;
	برائے (صحیح i = 0; i < چوڑائی; i++) {
		اگر (val_arst[i] == State::S1)
			sig_سیٹ[i] = sig_arst;
		ورنہ
			sig_clr[i] = sig_arst;
	}
}

خالی Ff_ڈیٹا::aload_مقصد_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = سچ;
	has_aload = جھوٹ;
	اگر (!is_fine) {
		pol_clr = جھوٹ;
		pol_سیٹ = سچ;
		اگر (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, چوڑائی), sig_ad, sig_aload);
			sig_سیٹ = module->Mux(NEW_ID, Const(State::S0, چوڑائی), sig_ad, sig_aload);
		} ورنہ {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, چوڑائی), sig_aload);
			sig_سیٹ = module->Mux(NEW_ID, sig_ad, Const(State::S0, چوڑائی), sig_aload);
		}
	} ورنہ {
		pol_clr = pol_aload;
		pol_سیٹ = pol_aload;
		اگر (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_سیٹ = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} ورنہ {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_سیٹ = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

خالی Ff_ڈیٹا::convert_ce_over_srst(منطقی val) {
	اگر (!has_ce || !has_srst || ce_over_srst == val)
		واپس;
	اگر (val) {
		// sdffe to sdffce
		اگر (!is_fine) {
			اگر (pol_ce) {
				اگر (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} ورنہ {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} ورنہ {
				اگر (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} ورنہ {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} ورنہ {
			اگر (pol_ce) {
				اگر (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} ورنہ {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} ورنہ {
				اگر (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} ورنہ {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} ورنہ {
		// sdffce to sdffe
		اگر (!is_fine) {
			اگر (pol_srst) {
				اگر (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} ورنہ {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} ورنہ {
				اگر (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} ورنہ {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} ورنہ {
			اگر (pol_srst) {
				اگر (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} ورنہ {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} ورنہ {
				اگر (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} ورنہ {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

خالی Ff_ڈیٹا::unmap_ce() {
	اگر (!has_ce)
		واپس;
	log_assert(has_clk);
	اگر (has_srst && ce_over_srst)
		unmap_srst();

	اگر (!is_fine) {
		اگر (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		ورنہ
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} ورنہ {
		اگر (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		ورنہ
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = جھوٹ;
}

خالی Ff_ڈیٹا::unmap_srst() {
	اگر (!has_srst)
		واپس;
	اگر (has_ce && !ce_over_srst)
		unmap_ce();

	اگر (!is_fine) {
		اگر (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		ورنہ
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} ورنہ {
		اگر (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		ورنہ
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = جھوٹ;
}

Cell *Ff_ڈیٹا::emit() {
	remove();
	اگر (!چوڑائی)
		واپس خالی_اشاریہ;
	اگر (!has_aload && !has_clk && !has_gclk && !has_sr) {
		اگر (has_arst) {
			// Convert this case to a D latch.
			arst_مقصد_aload();
		} ورنہ {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			واپس خالی_اشاریہ;
		}
	}
	اگر (initvals && !is_anyinit)
		initvals->سیٹ_init(sig_q, val_init);
	اگر (!is_fine) {
		اگر (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			اگر (is_anyinit) {
				cell = module->جوڑو_Anyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} ورنہ {
				cell = module->جوڑو_Ff(name, sig_d, sig_q);
			}
		} ورنہ اگر (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->جوڑو_Sr(name, sig_سیٹ, sig_clr, sig_q, pol_سیٹ, pol_clr);
		} ورنہ اگر (!has_clk) {
			log_assert(!has_srst);
			اگر (has_sr)
				cell = module->جوڑو_Dlatchsr(name, sig_aload, sig_سیٹ, sig_clr, sig_ad, sig_q, pol_aload, pol_سیٹ, pol_clr);
			ورنہ اگر (has_arst)
				cell = module->جوڑو_Adlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			ورنہ
				cell = module->جوڑو_Dlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ورنہ {
			اگر (has_sr) {
				اگر (has_ce)
					cell = module->جوڑو_Dffsre(name, sig_clk, sig_ce, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_سیٹ, pol_clr);
				ورنہ
					cell = module->جوڑو_Dffsr(name, sig_clk, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_سیٹ, pol_clr);
			} ورنہ اگر (has_arst) {
				اگر (has_ce)
					cell = module->جوڑو_Adffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				ورنہ
					cell = module->جوڑو_Adff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} ورنہ اگر (has_aload) {
				اگر (has_ce)
					cell = module->جوڑو_Aldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ورنہ
					cell = module->جوڑو_Aldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ورنہ اگر (has_srst) {
				اگر (has_ce)
					اگر (ce_over_srst)
						cell = module->جوڑو_Sdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					ورنہ
						cell = module->جوڑو_Sdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				ورنہ
					cell = module->جوڑو_Sdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} ورنہ {
				اگر (has_ce)
					cell = module->جوڑو_Dffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ورنہ
					cell = module->جوڑو_Dff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} ورنہ {
		اگر (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->جوڑو_Ff_Gate(name, sig_d, sig_q);
		} ورنہ اگر (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->جوڑو_Sr_Gate(name, sig_سیٹ, sig_clr, sig_q, pol_سیٹ, pol_clr);
		} ورنہ اگر (!has_clk) {
			log_assert(!has_srst);
			اگر (has_sr)
				cell = module->جوڑو_Dlatchsr_Gate(name, sig_aload, sig_سیٹ, sig_clr, sig_ad, sig_q, pol_aload, pol_سیٹ, pol_clr);
			ورنہ اگر (has_arst)
				cell = module->جوڑو_Adlatch_Gate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			ورنہ
				cell = module->جوڑو_Dlatch_Gate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ورنہ {
			اگر (has_sr) {
				اگر (has_ce)
					cell = module->جوڑو_Dffsre_Gate(name, sig_clk, sig_ce, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_سیٹ, pol_clr);
				ورنہ
					cell = module->جوڑو_Dffsr_Gate(name, sig_clk, sig_سیٹ, sig_clr, sig_d, sig_q, pol_clk, pol_سیٹ, pol_clr);
			} ورنہ اگر (has_arst) {
				اگر (has_ce)
					cell = module->جوڑو_Adffe_Gate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				ورنہ
					cell = module->جوڑو_Adff_Gate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} ورنہ اگر (has_aload) {
				اگر (has_ce)
					cell = module->جوڑو_Aldffe_Gate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ورنہ
					cell = module->جوڑو_Aldff_Gate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ورنہ اگر (has_srst) {
				اگر (has_ce)
					اگر (ce_over_srst)
						cell = module->جوڑو_Sdffce_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					ورنہ
						cell = module->جوڑو_Sdffe_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				ورنہ
					cell = module->جوڑو_Sdff_Gate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} ورنہ {
				اگر (has_ce)
					cell = module->جوڑو_Dffe_Gate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ورنہ
					cell = module->جوڑو_Dff_Gate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->اوصاف = اوصاف;
	واپس cell;
}

خالی Ff_ڈیٹا::remove() {
	اگر (cell) {
		remove_init();
		module->remove(cell);
		cell = خالی_اشاریہ;
	}
}

نامخلا {
	State invert(State s) {
		انتخاب (s) {
			حالت State::S0: واپس State::S1;
			حالت State::S1: واپس State::S0;
			طےشدہ: واپس s;
		}
	}
}

خالی Ff_ڈیٹا::flip_rst_بٹ(ثابت pool<صحیح> &بٹ) {
	اگر (!بٹ.سائز())
		واپس;

	remove_init();

	برائے (خودکار ثنائی_ہندسہ: بٹ) {
		اگر (has_arst)
			val_arst.سیٹ(ثنائی_ہندسہ, invert(val_arst[ثنائی_ہندسہ]));
		اگر (has_srst)
			val_srst.سیٹ(ثنائی_ہندسہ, invert(val_srst[ثنائی_ہندسہ]));
		val_init.سیٹ(ثنائی_ہندسہ, invert(val_init[ثنائی_ہندسہ]));
	}
}

خالی Ff_ڈیٹا::flip_بٹ(ثابت pool<صحیح> &بٹ) {
	اگر (!بٹ.سائز())
		واپس;

	flip_rst_بٹ(بٹ);

	Wire *new_q = module->جوڑو_Wire(NEW_ID, چوڑائی);

	اگر (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	اگر (is_fine) {
		اگر (has_sr) {
			منطقی new_pol_clr = pol_سیٹ;
			SigSpec new_sig_clr;
			اگر (pol_سیٹ) {
				اگر (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_سیٹ, sig_clr);
				} ورنہ {
					new_sig_clr = module->AndGate(NEW_ID, sig_سیٹ, sig_clr);
				}
			} ورنہ {
				اگر (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_سیٹ, sig_clr);
				} ورنہ {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_سیٹ, sig_clr);
				}
			}
			pol_سیٹ = pol_clr;
			sig_سیٹ = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		اگر (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		اگر (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->جوڑو_Not_Gate(NEW_ID, new_q, sig_q);
	}
	ورنہ
	{
		اگر (has_sr) {
			SigSpec not_clr;
			اگر (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = سچ;
			} ورنہ {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			اگر (!pol_سیٹ) {
				sig_سیٹ = module->Not(NEW_ID, sig_سیٹ);
				pol_سیٹ = سچ;
			}

			SigSpec masked_سیٹ = module->And(NEW_ID, sig_سیٹ, not_clr);
			برائے (خودکار ثنائی_ہندسہ: بٹ) {
				sig_سیٹ[ثنائی_ہندسہ] = sig_clr[ثنائی_ہندسہ];
				sig_clr[ثنائی_ہندسہ] = masked_سیٹ[ثنائی_ہندسہ];
			}
		}

		Const نقاب = Const(State::S0, چوڑائی);
		برائے (خودکار ثنائی_ہندسہ: بٹ)
			نقاب.سیٹ(ثنائی_ہندسہ, State::S1);

		اگر (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, نقاب);
		اگر (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, نقاب);
		module->جوڑو_Xor(NEW_ID, new_q, نقاب, sig_q);
	}

	sig_q = new_q;
}
