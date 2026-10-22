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

#అన్తర్భావ "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
మూస<రకం_పేరు ఆదానం_Type, రకం_పేరు OutputType, రకం_పేరు = std::enable_if_t<std::is_పునాది_యొక్క_v<Ff_Type_దత్తాంశం, OutputType>>>
శూన్య manufacture_info(ఆదానం_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = శూన్యసూచిక;
	IdString type;
	constexpr తార్కిక have_cell = std::is_same_v<ఆదానం_Type, Cell*>;
	ఒకవేళ constexpr (std::is_same_v<ఆదానం_Type, IdString>) {
		type = flop;
	} లేకపోతే {
		static_assert(std::is_same_v<ఆదానం_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	ఒకవేళ constexpr (have_cell) {
		info.sig_q = cell->get_పోర్టు(ID::Q);
		info.వెడల్పు = Get_కొలత(info.sig_q);
		info.లక్షణాలు = cell->లక్షణాలు;
		ఒకవేళ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	ఒకవేళ (type.లోపల(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		ఒకవేళ (type.లోపల(ID($anyinit), ID($ff))) {
			info.has_gclk = నిజం;
			ఒకవేళ constexpr (have_cell)
				info.sig_d = cell->get_పోర్టు(ID::D);
			ఒకవేళ (type == ID($anyinit)) {
				info.is_anyinit = నిజం;
				ఒకవేళ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} లేకపోతే ఒకవేళ (type == ID($sr)) {
			// No data input at all.
		} లేకపోతే ఒకవేళ (type.లోపల(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = నిజం;
			ఒకవేళ constexpr (have_cell) {
				info.sig_aload = cell->get_పోర్టు(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->get_పోర్టు(ID::D);
			}
		} లేకపోతే {
			info.has_clk = నిజం;
			ఒకవేళ constexpr (have_cell) {
				info.sig_clk = cell->get_పోర్టు(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->get_పోర్టు(ID::D);
			}
		}
		ఒకవేళ (type.లోపల(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = నిజం;
			ఒకవేళ constexpr (have_cell) {
				info.sig_ce = cell->get_పోర్టు(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		ఒకవేళ (type.లోపల(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = నిజం;
			ఒకవేళ constexpr (have_cell) {
				info.sig_clr = cell->get_పోర్టు(ID::CLR);
				info.sig_సమితి = cell->get_పోర్టు(ID::సమితి_2);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_సమితి = cell->getParam(ID::సమితి_POLARITY).as_bool();
			}
		}
		ఒకవేళ (type.లోపల(ID($aldff), ID($aldffe))) {
			info.has_aload = నిజం;
			ఒకవేళ constexpr (have_cell) {
				info.sig_aload = cell->get_పోర్టు(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->get_పోర్టు(ID::AD);
			}
		}
		ఒకవేళ (type.లోపల(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = నిజం;
			ఒకవేళ constexpr (have_cell) {
				info.sig_arst = cell->get_పోర్టు(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_విలువ);
			}
		}
		ఒకవేళ (type.లోపల(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = నిజం;
			ఒకవేళ constexpr (have_cell) {
				info.sig_srst = cell->get_పోర్టు(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_విలువ);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} లేకపోతే ఒకవేళ (type == ID($_FF_)) {
		info.is_fine = నిజం;
		info.has_gclk = నిజం;
		ఒకవేళ constexpr (have_cell)
			info.sig_d = cell->get_పోర్టు(ID::D);
	} లేకపోతే ఒకవేళ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = నిజం;
		info.has_sr = నిజం;
		info.pol_సమితి = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_సమితి = cell->get_పోర్టు(ID::S);
			info.sig_clr = cell->get_పోర్టు(ID::R);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 6) == "$_DFF_" && type_str.కొలత() == 8) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[6] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 7) == "$_DFFE_" && type_str.కొలత() == 10) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = నిజం;
		info.pol_ce = type_str[8] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_ce = cell->get_పోర్టు(ID::E);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 6) == "$_DFF_" && type_str.కొలత() == 10) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = నిజం;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_arst = cell->get_పోర్టు(ID::R);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 7) == "$_DFFE_" && type_str.కొలత() == 12) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = నిజం;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = నిజం;
		info.pol_ce = type_str[10] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_arst = cell->get_పోర్టు(ID::R);
			info.sig_ce = cell->get_పోర్టు(ID::E);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.కొలత() == 11) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = నిజం;
		info.pol_aload = type_str[9] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_aload = cell->get_పోర్టు(ID::L);
			info.sig_ad = cell->get_పోర్టు(ID::AD);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.కొలత() == 13) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = నిజం;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = నిజం;
		info.pol_ce = type_str[11] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_aload = cell->get_పోర్టు(ID::L);
			info.sig_ad = cell->get_పోర్టు(ID::AD);
			info.sig_ce = cell->get_పోర్టు(ID::E);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.కొలత() == 12) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = నిజం;
		info.pol_సమితి = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_సమితి = cell->get_పోర్టు(ID::S);
			info.sig_clr = cell->get_పోర్టు(ID::R);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.కొలత() == 14) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = నిజం;
		info.pol_సమితి = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = నిజం;
		info.pol_ce = type_str[12] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_సమితి = cell->get_పోర్టు(ID::S);
			info.sig_clr = cell->get_పోర్టు(ID::R);
			info.sig_ce = cell->get_పోర్టు(ID::E);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 7) == "$_SDFF_" && type_str.కొలత() == 11) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = నిజం;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_srst = cell->get_పోర్టు(ID::R);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.కొలత() == 13) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = నిజం;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = నిజం;
		info.pol_ce = type_str[11] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_srst = cell->get_పోర్టు(ID::R);
			info.sig_ce = cell->get_పోర్టు(ID::E);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.కొలత() == 14) {
		info.is_fine = నిజం;
		info.has_clk = నిజం;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = నిజం;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = నిజం;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = నిజం;
		ఒకవేళ constexpr (have_cell) {
			info.sig_d = cell->get_పోర్టు(ID::D);
			info.sig_clk = cell->get_పోర్టు(ID::C);
			info.sig_srst = cell->get_పోర్టు(ID::R);
			info.sig_ce = cell->get_పోర్టు(ID::E);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.కొలత() == 11) {
		info.is_fine = నిజం;
		info.has_aload = నిజం;
		info.has_aload = నిజం;
		info.pol_aload = type_str[9] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_ad = cell->get_పోర్టు(ID::D);
			info.sig_aload = cell->get_పోర్టు(ID::E);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.కొలత() == 13) {
		info.is_fine = నిజం;
		info.has_aload = నిజం;
		info.has_aload = నిజం;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = నిజం;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		ఒకవేళ constexpr (have_cell) {
			info.sig_ad = cell->get_పోర్టు(ID::D);
			info.sig_aload = cell->get_పోర్టు(ID::E);
			info.sig_arst = cell->get_పోర్టు(ID::R);
		}
	} లేకపోతే ఒకవేళ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.కొలత() == 15) {
		info.is_fine = నిజం;
		info.has_aload = నిజం;
		info.has_aload = నిజం;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = నిజం;
		info.pol_సమితి = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		ఒకవేళ constexpr (have_cell) {
			info.sig_ad = cell->get_పోర్టు(ID::D);
			info.sig_aload = cell->get_పోర్టు(ID::E);
			info.sig_సమితి = cell->get_పోర్టు(ID::S);
			info.sig_clr = cell->get_పోర్టు(ID::R);
		}
	} లేకపోతే {
		log_assert(0);
	}
	ఒకవేళ constexpr (have_cell)
		ఒకవేళ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = అబద్ధం;
			info.has_arst = నిజం;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_దత్తాంశం::Ff_Type_దత్తాంశం(IdString type) : Ff_Type_దత్తాంశం()
{
	manufacture_info(type, *ఇది, శూన్యసూచిక);
}

Ff_దత్తాంశం::Ff_దత్తాంశం(FfInitVals *initvals, Cell *cell_) : Ff_దత్తాంశం(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *ఇది, initvals);
}

Ff_దత్తాంశం Ff_దత్తాంశం::ముక్క(స్థిరరాశి std::సదిశ<పూర్ణాంకం> &బిట్లు) {
	Ff_దత్తాంశం res(module, initvals, module->twines().కూడు(NEW_ID));
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
	res.pol_సమితి = pol_సమితి;
	res.లక్షణాలు = లక్షణాలు;
	std::optional<Const::Builder> arst_బిట్లు;
	ఒకవేళ (has_arst)
		arst_బిట్లు.emplace(బిట్లు.కొలత());
	std::optional<Const::Builder> srst_బిట్లు;
	ఒకవేళ (has_srst)
		srst_బిట్లు.emplace(బిట్లు.కొలత());
	std::optional<Const::Builder> init_బిట్లు;
	ఒకవేళ (initvals)
		init_బిట్లు.emplace(బిట్లు.కొలత());

	కొరకు (పూర్ణాంకం i : బిట్లు) {
		res.sig_q.append(sig_q[i]);
		ఒకవేళ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		ఒకవేళ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		ఒకవేళ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_సమితి.append(sig_సమితి[i]);
		}
		ఒకవేళ (has_arst)
			arst_బిట్లు->నెట్టు_వెనుక(val_arst[i]);
		ఒకవేళ (has_srst)
			srst_బిట్లు->నెట్టు_వెనుక(val_srst[i]);
		ఒకవేళ (initvals)
			init_బిట్లు->నెట్టు_వెనుక(val_init[i]);
	}

	ఒకవేళ (has_arst)
		res.val_arst = arst_బిట్లు->build();
	ఒకవేళ (has_srst)
		res.val_srst = srst_బిట్లు->build();
	ఒకవేళ (initvals)
		res.val_init = init_బిట్లు->build();
	res.వెడల్పు = Get_కొలత(res.sig_q);
	తిరిగి res;
}

శూన్య Ff_దత్తాంశం::కూడు_dummy_ce() {
	ఒకవేళ (has_ce)
		తిరిగి;
	has_ce = నిజం;
	pol_ce = నిజం;
	sig_ce = State::S1;
	ce_over_srst = అబద్ధం;
}

శూన్య Ff_దత్తాంశం::కూడు_dummy_srst() {
	ఒకవేళ (has_srst)
		తిరిగి;
	has_srst = నిజం;
	pol_srst = నిజం;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, వెడల్పు);
	ce_over_srst = అబద్ధం;
}

శూన్య Ff_దత్తాంశం::కూడు_dummy_arst() {
	ఒకవేళ (has_arst)
		తిరిగి;
	has_arst = నిజం;
	pol_arst = నిజం;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, వెడల్పు);
}

శూన్య Ff_దత్తాంశం::కూడు_dummy_aload() {
	ఒకవేళ (has_aload)
		తిరిగి;
	has_aload = నిజం;
	pol_aload = నిజం;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, వెడల్పు);
}

శూన్య Ff_దత్తాంశం::కూడు_dummy_sr() {
	ఒకవేళ (has_sr)
		తిరిగి;
	has_sr = నిజం;
	pol_clr = నిజం;
	pol_సమితి = నిజం;
	sig_clr = Const(State::S0, వెడల్పు);
	sig_సమితి = Const(State::S0, వెడల్పు);
}

శూన్య Ff_దత్తాంశం::కూడు_dummy_clk() {
	ఒకవేళ (has_clk)
		తిరిగి;
	has_clk = నిజం;
	pol_clk = నిజం;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, వెడల్పు);
}

శూన్య Ff_దత్తాంశం::arst_గమ్యం_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = నిజం;
	has_arst = అబద్ధం;
}

శూన్య Ff_దత్తాంశం::arst_గమ్యం_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_సమితి = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, వెడల్పు);
	sig_సమితి = Const(pol_arst ? State::S0 : State::S1, వెడల్పు);
	has_sr = నిజం;
	has_arst = అబద్ధం;
	కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++) {
		ఒకవేళ (val_arst[i] == State::S1)
			sig_సమితి[i] = sig_arst;
		లేకపోతే
			sig_clr[i] = sig_arst;
	}
}

శూన్య Ff_దత్తాంశం::aload_గమ్యం_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = నిజం;
	has_aload = అబద్ధం;
	ఒకవేళ (!is_fine) {
		pol_clr = అబద్ధం;
		pol_సమితి = నిజం;
		ఒకవేళ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, వెడల్పు), sig_ad, sig_aload);
			sig_సమితి = module->Mux(NEW_ID, Const(State::S0, వెడల్పు), sig_ad, sig_aload);
		} లేకపోతే {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, వెడల్పు), sig_aload);
			sig_సమితి = module->Mux(NEW_ID, sig_ad, Const(State::S0, వెడల్పు), sig_aload);
		}
	} లేకపోతే {
		pol_clr = pol_aload;
		pol_సమితి = pol_aload;
		ఒకవేళ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_సమితి = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} లేకపోతే {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_సమితి = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

శూన్య Ff_దత్తాంశం::convert_ce_over_srst(తార్కిక val) {
	ఒకవేళ (!has_ce || !has_srst || ce_over_srst == val)
		తిరిగి;
	ఒకవేళ (val) {
		// sdffe to sdffce
		ఒకవేళ (!is_fine) {
			ఒకవేళ (pol_ce) {
				ఒకవేళ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} లేకపోతే {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} లేకపోతే {
				ఒకవేళ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} లేకపోతే {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} లేకపోతే {
			ఒకవేళ (pol_ce) {
				ఒకవేళ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} లేకపోతే {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} లేకపోతే {
				ఒకవేళ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} లేకపోతే {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} లేకపోతే {
		// sdffce to sdffe
		ఒకవేళ (!is_fine) {
			ఒకవేళ (pol_srst) {
				ఒకవేళ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} లేకపోతే {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} లేకపోతే {
				ఒకవేళ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} లేకపోతే {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} లేకపోతే {
			ఒకవేళ (pol_srst) {
				ఒకవేళ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} లేకపోతే {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} లేకపోతే {
				ఒకవేళ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} లేకపోతే {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

శూన్య Ff_దత్తాంశం::unmap_ce() {
	ఒకవేళ (!has_ce)
		తిరిగి;
	log_assert(has_clk);
	ఒకవేళ (has_srst && ce_over_srst)
		unmap_srst();

	ఒకవేళ (!is_fine) {
		ఒకవేళ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		లేకపోతే
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} లేకపోతే {
		ఒకవేళ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		లేకపోతే
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = అబద్ధం;
}

శూన్య Ff_దత్తాంశం::unmap_srst() {
	ఒకవేళ (!has_srst)
		తిరిగి;
	ఒకవేళ (has_ce && !ce_over_srst)
		unmap_ce();

	ఒకవేళ (!is_fine) {
		ఒకవేళ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		లేకపోతే
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} లేకపోతే {
		ఒకవేళ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		లేకపోతే
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = అబద్ధం;
}

Cell *Ff_దత్తాంశం::emit() {
	remove();
	ఒకవేళ (!వెడల్పు)
		తిరిగి శూన్యసూచిక;
	ఒకవేళ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		ఒకవేళ (has_arst) {
			// Convert this case to a D latch.
			arst_గమ్యం_aload();
		} లేకపోతే {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			తిరిగి శూన్యసూచిక;
		}
	}
	ఒకవేళ (initvals && !is_anyinit)
		initvals->సమితి_init(sig_q, val_init);
	ఒకవేళ (!is_fine) {
		ఒకవేళ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			ఒకవేళ (is_anyinit) {
				cell = module->కూడు_Anyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} లేకపోతే {
				cell = module->కూడు_Ff(name, sig_d, sig_q);
			}
		} లేకపోతే ఒకవేళ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->కూడు_Sr(name, sig_సమితి, sig_clr, sig_q, pol_సమితి, pol_clr);
		} లేకపోతే ఒకవేళ (!has_clk) {
			log_assert(!has_srst);
			ఒకవేళ (has_sr)
				cell = module->కూడు_Dlatchsr(name, sig_aload, sig_సమితి, sig_clr, sig_ad, sig_q, pol_aload, pol_సమితి, pol_clr);
			లేకపోతే ఒకవేళ (has_arst)
				cell = module->కూడు_Adlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			లేకపోతే
				cell = module->కూడు_Dlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} లేకపోతే {
			ఒకవేళ (has_sr) {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Dffsre(name, sig_clk, sig_ce, sig_సమితి, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_సమితి, pol_clr);
				లేకపోతే
					cell = module->కూడు_Dffsr(name, sig_clk, sig_సమితి, sig_clr, sig_d, sig_q, pol_clk, pol_సమితి, pol_clr);
			} లేకపోతే ఒకవేళ (has_arst) {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Adffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				లేకపోతే
					cell = module->కూడు_Adff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} లేకపోతే ఒకవేళ (has_aload) {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Aldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				లేకపోతే
					cell = module->కూడు_Aldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} లేకపోతే ఒకవేళ (has_srst) {
				ఒకవేళ (has_ce)
					ఒకవేళ (ce_over_srst)
						cell = module->కూడు_Sdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					లేకపోతే
						cell = module->కూడు_Sdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				లేకపోతే
					cell = module->కూడు_Sdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} లేకపోతే {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Dffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				లేకపోతే
					cell = module->కూడు_Dff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} లేకపోతే {
		ఒకవేళ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->కూడు_Ff_Gate(name, sig_d, sig_q);
		} లేకపోతే ఒకవేళ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->కూడు_Sr_Gate(name, sig_సమితి, sig_clr, sig_q, pol_సమితి, pol_clr);
		} లేకపోతే ఒకవేళ (!has_clk) {
			log_assert(!has_srst);
			ఒకవేళ (has_sr)
				cell = module->కూడు_Dlatchsr_Gate(name, sig_aload, sig_సమితి, sig_clr, sig_ad, sig_q, pol_aload, pol_సమితి, pol_clr);
			లేకపోతే ఒకవేళ (has_arst)
				cell = module->కూడు_Adlatch_Gate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			లేకపోతే
				cell = module->కూడు_Dlatch_Gate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} లేకపోతే {
			ఒకవేళ (has_sr) {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Dffsre_Gate(name, sig_clk, sig_ce, sig_సమితి, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_సమితి, pol_clr);
				లేకపోతే
					cell = module->కూడు_Dffsr_Gate(name, sig_clk, sig_సమితి, sig_clr, sig_d, sig_q, pol_clk, pol_సమితి, pol_clr);
			} లేకపోతే ఒకవేళ (has_arst) {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Adffe_Gate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				లేకపోతే
					cell = module->కూడు_Adff_Gate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} లేకపోతే ఒకవేళ (has_aload) {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Aldffe_Gate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				లేకపోతే
					cell = module->కూడు_Aldff_Gate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} లేకపోతే ఒకవేళ (has_srst) {
				ఒకవేళ (has_ce)
					ఒకవేళ (ce_over_srst)
						cell = module->కూడు_Sdffce_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					లేకపోతే
						cell = module->కూడు_Sdffe_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				లేకపోతే
					cell = module->కూడు_Sdff_Gate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} లేకపోతే {
				ఒకవేళ (has_ce)
					cell = module->కూడు_Dffe_Gate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				లేకపోతే
					cell = module->కూడు_Dff_Gate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->లక్షణాలు = లక్షణాలు;
	తిరిగి cell;
}

శూన్య Ff_దత్తాంశం::remove() {
	ఒకవేళ (cell) {
		remove_init();
		module->remove(cell);
		cell = శూన్యసూచిక;
	}
}

నామాకాశ {
	State invert(State s) {
		ఎంపిక (s) {
			స్థితి State::S0: తిరిగి State::S1;
			స్థితి State::S1: తిరిగి State::S0;
			అప్రమేయ: తిరిగి s;
		}
	}
}

శూన్య Ff_దత్తాంశం::flip_rst_బిట్లు(స్థిరరాశి pool<పూర్ణాంకం> &బిట్లు) {
	ఒకవేళ (!బిట్లు.కొలత())
		తిరిగి;

	remove_init();

	కొరకు (స్వయంచాలక బిట్టు: బిట్లు) {
		ఒకవేళ (has_arst)
			val_arst.సమితి(బిట్టు, invert(val_arst[బిట్టు]));
		ఒకవేళ (has_srst)
			val_srst.సమితి(బిట్టు, invert(val_srst[బిట్టు]));
		val_init.సమితి(బిట్టు, invert(val_init[బిట్టు]));
	}
}

శూన్య Ff_దత్తాంశం::flip_బిట్లు(స్థిరరాశి pool<పూర్ణాంకం> &బిట్లు) {
	ఒకవేళ (!బిట్లు.కొలత())
		తిరిగి;

	flip_rst_బిట్లు(బిట్లు);

	Wire *new_q = module->కూడు_Wire(NEW_ID, వెడల్పు);

	ఒకవేళ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	ఒకవేళ (is_fine) {
		ఒకవేళ (has_sr) {
			తార్కిక new_pol_clr = pol_సమితి;
			SigSpec new_sig_clr;
			ఒకవేళ (pol_సమితి) {
				ఒకవేళ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_సమితి, sig_clr);
				} లేకపోతే {
					new_sig_clr = module->AndGate(NEW_ID, sig_సమితి, sig_clr);
				}
			} లేకపోతే {
				ఒకవేళ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_సమితి, sig_clr);
				} లేకపోతే {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_సమితి, sig_clr);
				}
			}
			pol_సమితి = pol_clr;
			sig_సమితి = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		ఒకవేళ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		ఒకవేళ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->కూడు_Not_Gate(NEW_ID, new_q, sig_q);
	}
	లేకపోతే
	{
		ఒకవేళ (has_sr) {
			SigSpec not_clr;
			ఒకవేళ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = నిజం;
			} లేకపోతే {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			ఒకవేళ (!pol_సమితి) {
				sig_సమితి = module->Not(NEW_ID, sig_సమితి);
				pol_సమితి = నిజం;
			}

			SigSpec masked_సమితి = module->And(NEW_ID, sig_సమితి, not_clr);
			కొరకు (స్వయంచాలక బిట్టు: బిట్లు) {
				sig_సమితి[బిట్టు] = sig_clr[బిట్టు];
				sig_clr[బిట్టు] = masked_సమితి[బిట్టు];
			}
		}

		Const ముసుగు = Const(State::S0, వెడల్పు);
		కొరకు (స్వయంచాలక బిట్టు: బిట్లు)
			ముసుగు.సమితి(బిట్టు, State::S1);

		ఒకవేళ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, ముసుగు);
		ఒకవేళ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, ముసుగు);
		module->కూడు_Xor(NEW_ID, new_q, ముసుగు, sig_q);
	}

	sig_q = new_q;
}
