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
प्रतिरूपम्<प्रकारनाम निवेशः_Type, प्रकारनाम OutputType, प्रकारनाम = std::enable_if_t<std::is_आधारः_सम्बन्धि_v<Ff_Type_दत्त, OutputType>>>
शून्य manufacture_info(निवेशः_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = शून्यसूचकः;
	IdString type;
	constexpr तार्किक have_cell = std::is_same_v<निवेशः_Type, Cell*>;
	यदि constexpr (std::is_same_v<निवेशः_Type, IdString>) {
		type = flop;
	} अन्यथा {
		static_assert(std::is_same_v<निवेशः_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	यदि constexpr (have_cell) {
		info.sig_q = cell->get_पत्तनम्(ID::Q);
		info.विस्तारः = Get_आकार(info.sig_q);
		info.गुणाः = cell->गुणाः;
		यदि (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	यदि (type.अन्तर्(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		यदि (type.अन्तर्(ID($anyinit), ID($ff))) {
			info.has_gclk = सत्य;
			यदि constexpr (have_cell)
				info.sig_d = cell->get_पत्तनम्(ID::D);
			यदि (type == ID($anyinit)) {
				info.is_anyinit = सत्य;
				यदि constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} अन्यथा यदि (type == ID($sr)) {
			// No data input at all.
		} अन्यथा यदि (type.अन्तर्(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = सत्य;
			यदि constexpr (have_cell) {
				info.sig_aload = cell->get_पत्तनम्(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->get_पत्तनम्(ID::D);
			}
		} अन्यथा {
			info.has_clk = सत्य;
			यदि constexpr (have_cell) {
				info.sig_clk = cell->get_पत्तनम्(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->get_पत्तनम्(ID::D);
			}
		}
		यदि (type.अन्तर्(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = सत्य;
			यदि constexpr (have_cell) {
				info.sig_ce = cell->get_पत्तनम्(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		यदि (type.अन्तर्(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = सत्य;
			यदि constexpr (have_cell) {
				info.sig_clr = cell->get_पत्तनम्(ID::CLR);
				info.sig_समुच्चयः = cell->get_पत्तनम्(ID::समुच्चयः_2);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_समुच्चयः = cell->getParam(ID::समुच्चयः_POLARITY).as_bool();
			}
		}
		यदि (type.अन्तर्(ID($aldff), ID($aldffe))) {
			info.has_aload = सत्य;
			यदि constexpr (have_cell) {
				info.sig_aload = cell->get_पत्तनम्(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->get_पत्तनम्(ID::AD);
			}
		}
		यदि (type.अन्तर्(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = सत्य;
			यदि constexpr (have_cell) {
				info.sig_arst = cell->get_पत्तनम्(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_मानम्);
			}
		}
		यदि (type.अन्तर्(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = सत्य;
			यदि constexpr (have_cell) {
				info.sig_srst = cell->get_पत्तनम्(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_मानम्);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} अन्यथा यदि (type == ID($_FF_)) {
		info.is_fine = सत्य;
		info.has_gclk = सत्य;
		यदि constexpr (have_cell)
			info.sig_d = cell->get_पत्तनम्(ID::D);
	} अन्यथा यदि (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = सत्य;
		info.has_sr = सत्य;
		info.pol_समुच्चयः = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		यदि constexpr (have_cell) {
			info.sig_समुच्चयः = cell->get_पत्तनम्(ID::S);
			info.sig_clr = cell->get_पत्तनम्(ID::R);
		}
	} अन्यथा यदि (type_str.substr(0, 6) == "$_DFF_" && type_str.आकार() == 8) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[6] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
		}
	} अन्यथा यदि (type_str.substr(0, 7) == "$_DFFE_" && type_str.आकार() == 10) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = सत्य;
		info.pol_ce = type_str[8] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_ce = cell->get_पत्तनम्(ID::E);
		}
	} अन्यथा यदि (type_str.substr(0, 6) == "$_DFF_" && type_str.आकार() == 10) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = सत्य;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_arst = cell->get_पत्तनम्(ID::R);
		}
	} अन्यथा यदि (type_str.substr(0, 7) == "$_DFFE_" && type_str.आकार() == 12) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = सत्य;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = सत्य;
		info.pol_ce = type_str[10] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_arst = cell->get_पत्तनम्(ID::R);
			info.sig_ce = cell->get_पत्तनम्(ID::E);
		}
	} अन्यथा यदि (type_str.substr(0, 8) == "$_ALDFF_" && type_str.आकार() == 11) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = सत्य;
		info.pol_aload = type_str[9] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_aload = cell->get_पत्तनम्(ID::L);
			info.sig_ad = cell->get_पत्तनम्(ID::AD);
		}
	} अन्यथा यदि (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.आकार() == 13) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = सत्य;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = सत्य;
		info.pol_ce = type_str[11] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_aload = cell->get_पत्तनम्(ID::L);
			info.sig_ad = cell->get_पत्तनम्(ID::AD);
			info.sig_ce = cell->get_पत्तनम्(ID::E);
		}
	} अन्यथा यदि (type_str.substr(0, 8) == "$_DFFSR_" && type_str.आकार() == 12) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = सत्य;
		info.pol_समुच्चयः = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_समुच्चयः = cell->get_पत्तनम्(ID::S);
			info.sig_clr = cell->get_पत्तनम्(ID::R);
		}
	} अन्यथा यदि (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.आकार() == 14) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = सत्य;
		info.pol_समुच्चयः = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = सत्य;
		info.pol_ce = type_str[12] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_समुच्चयः = cell->get_पत्तनम्(ID::S);
			info.sig_clr = cell->get_पत्तनम्(ID::R);
			info.sig_ce = cell->get_पत्तनम्(ID::E);
		}
	} अन्यथा यदि (type_str.substr(0, 7) == "$_SDFF_" && type_str.आकार() == 11) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = सत्य;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_srst = cell->get_पत्तनम्(ID::R);
		}
	} अन्यथा यदि (type_str.substr(0, 8) == "$_SDFFE_" && type_str.आकार() == 13) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = सत्य;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = सत्य;
		info.pol_ce = type_str[11] == 'P';
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_srst = cell->get_पत्तनम्(ID::R);
			info.sig_ce = cell->get_पत्तनम्(ID::E);
		}
	} अन्यथा यदि (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.आकार() == 14) {
		info.is_fine = सत्य;
		info.has_clk = सत्य;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = सत्य;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = सत्य;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = सत्य;
		यदि constexpr (have_cell) {
			info.sig_d = cell->get_पत्तनम्(ID::D);
			info.sig_clk = cell->get_पत्तनम्(ID::C);
			info.sig_srst = cell->get_पत्तनम्(ID::R);
			info.sig_ce = cell->get_पत्तनम्(ID::E);
		}
	} अन्यथा यदि (type_str.substr(0, 9) == "$_DLATCH_" && type_str.आकार() == 11) {
		info.is_fine = सत्य;
		info.has_aload = सत्य;
		info.has_aload = सत्य;
		info.pol_aload = type_str[9] == 'P';
		यदि constexpr (have_cell) {
			info.sig_ad = cell->get_पत्तनम्(ID::D);
			info.sig_aload = cell->get_पत्तनम्(ID::E);
		}
	} अन्यथा यदि (type_str.substr(0, 9) == "$_DLATCH_" && type_str.आकार() == 13) {
		info.is_fine = सत्य;
		info.has_aload = सत्य;
		info.has_aload = सत्य;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = सत्य;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		यदि constexpr (have_cell) {
			info.sig_ad = cell->get_पत्तनम्(ID::D);
			info.sig_aload = cell->get_पत्तनम्(ID::E);
			info.sig_arst = cell->get_पत्तनम्(ID::R);
		}
	} अन्यथा यदि (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.आकार() == 15) {
		info.is_fine = सत्य;
		info.has_aload = सत्य;
		info.has_aload = सत्य;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = सत्य;
		info.pol_समुच्चयः = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		यदि constexpr (have_cell) {
			info.sig_ad = cell->get_पत्तनम्(ID::D);
			info.sig_aload = cell->get_पत्तनम्(ID::E);
			info.sig_समुच्चयः = cell->get_पत्तनम्(ID::S);
			info.sig_clr = cell->get_पत्तनम्(ID::R);
		}
	} अन्यथा {
		log_assert(0);
	}
	यदि constexpr (have_cell)
		यदि (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = असत्य;
			info.has_arst = सत्य;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_दत्त::Ff_Type_दत्त(IdString type) : Ff_Type_दत्त()
{
	manufacture_info(type, *एतत्, शून्यसूचकः);
}

Ff_दत्त::Ff_दत्त(FfInitVals *initvals, Cell *cell_) : Ff_दत्त(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *एतत्, initvals);
}

Ff_दत्त Ff_दत्त::खण्डिका(ध्रुव std::सदिशम्<पूर्णाङ्क> &द्वयङ्काः) {
	Ff_दत्त res(module, initvals, module->twines().सङ्कलय(NEW_ID));
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
	res.pol_समुच्चयः = pol_समुच्चयः;
	res.गुणाः = गुणाः;
	std::optional<Const::Builder> arst_द्वयङ्काः;
	यदि (has_arst)
		arst_द्वयङ्काः.emplace(द्वयङ्काः.आकार());
	std::optional<Const::Builder> srst_द्वयङ्काः;
	यदि (has_srst)
		srst_द्वयङ्काः.emplace(द्वयङ्काः.आकार());
	std::optional<Const::Builder> init_द्वयङ्काः;
	यदि (initvals)
		init_द्वयङ्काः.emplace(द्वयङ्काः.आकार());

	प्रति (पूर्णाङ्क i : द्वयङ्काः) {
		res.sig_q.append(sig_q[i]);
		यदि (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		यदि (has_aload)
			res.sig_ad.append(sig_ad[i]);
		यदि (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_समुच्चयः.append(sig_समुच्चयः[i]);
		}
		यदि (has_arst)
			arst_द्वयङ्काः->प्रक्षिप_पृष्ठभागः(val_arst[i]);
		यदि (has_srst)
			srst_द्वयङ्काः->प्रक्षिप_पृष्ठभागः(val_srst[i]);
		यदि (initvals)
			init_द्वयङ्काः->प्रक्षिप_पृष्ठभागः(val_init[i]);
	}

	यदि (has_arst)
		res.val_arst = arst_द्वयङ्काः->build();
	यदि (has_srst)
		res.val_srst = srst_द्वयङ्काः->build();
	यदि (initvals)
		res.val_init = init_द्वयङ्काः->build();
	res.विस्तारः = Get_आकार(res.sig_q);
	प्रत्यागम res;
}

शून्य Ff_दत्त::सङ्कलय_dummy_ce() {
	यदि (has_ce)
		प्रत्यागम;
	has_ce = सत्य;
	pol_ce = सत्य;
	sig_ce = State::S1;
	ce_over_srst = असत्य;
}

शून्य Ff_दत्त::सङ्कलय_dummy_srst() {
	यदि (has_srst)
		प्रत्यागम;
	has_srst = सत्य;
	pol_srst = सत्य;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, विस्तारः);
	ce_over_srst = असत्य;
}

शून्य Ff_दत्त::सङ्कलय_dummy_arst() {
	यदि (has_arst)
		प्रत्यागम;
	has_arst = सत्य;
	pol_arst = सत्य;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, विस्तारः);
}

शून्य Ff_दत्त::सङ्कलय_dummy_aload() {
	यदि (has_aload)
		प्रत्यागम;
	has_aload = सत्य;
	pol_aload = सत्य;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, विस्तारः);
}

शून्य Ff_दत्त::सङ्कलय_dummy_sr() {
	यदि (has_sr)
		प्रत्यागम;
	has_sr = सत्य;
	pol_clr = सत्य;
	pol_समुच्चयः = सत्य;
	sig_clr = Const(State::S0, विस्तारः);
	sig_समुच्चयः = Const(State::S0, विस्तारः);
}

शून्य Ff_दत्त::सङ्कलय_dummy_clk() {
	यदि (has_clk)
		प्रत्यागम;
	has_clk = सत्य;
	pol_clk = सत्य;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, विस्तारः);
}

शून्य Ff_दत्त::arst_गन्तव्यम्_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = सत्य;
	has_arst = असत्य;
}

शून्य Ff_दत्त::arst_गन्तव्यम्_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_समुच्चयः = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, विस्तारः);
	sig_समुच्चयः = Const(pol_arst ? State::S0 : State::S1, विस्तारः);
	has_sr = सत्य;
	has_arst = असत्य;
	प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++) {
		यदि (val_arst[i] == State::S1)
			sig_समुच्चयः[i] = sig_arst;
		अन्यथा
			sig_clr[i] = sig_arst;
	}
}

शून्य Ff_दत्त::aload_गन्तव्यम्_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = सत्य;
	has_aload = असत्य;
	यदि (!is_fine) {
		pol_clr = असत्य;
		pol_समुच्चयः = सत्य;
		यदि (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, विस्तारः), sig_ad, sig_aload);
			sig_समुच्चयः = module->Mux(NEW_ID, Const(State::S0, विस्तारः), sig_ad, sig_aload);
		} अन्यथा {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, विस्तारः), sig_aload);
			sig_समुच्चयः = module->Mux(NEW_ID, sig_ad, Const(State::S0, विस्तारः), sig_aload);
		}
	} अन्यथा {
		pol_clr = pol_aload;
		pol_समुच्चयः = pol_aload;
		यदि (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_समुच्चयः = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} अन्यथा {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_समुच्चयः = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

शून्य Ff_दत्त::convert_ce_over_srst(तार्किक val) {
	यदि (!has_ce || !has_srst || ce_over_srst == val)
		प्रत्यागम;
	यदि (val) {
		// sdffe to sdffce
		यदि (!is_fine) {
			यदि (pol_ce) {
				यदि (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} अन्यथा {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} अन्यथा {
				यदि (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} अन्यथा {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} अन्यथा {
			यदि (pol_ce) {
				यदि (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} अन्यथा {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} अन्यथा {
				यदि (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} अन्यथा {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} अन्यथा {
		// sdffce to sdffe
		यदि (!is_fine) {
			यदि (pol_srst) {
				यदि (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} अन्यथा {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} अन्यथा {
				यदि (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} अन्यथा {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} अन्यथा {
			यदि (pol_srst) {
				यदि (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} अन्यथा {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} अन्यथा {
				यदि (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} अन्यथा {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

शून्य Ff_दत्त::unmap_ce() {
	यदि (!has_ce)
		प्रत्यागम;
	log_assert(has_clk);
	यदि (has_srst && ce_over_srst)
		unmap_srst();

	यदि (!is_fine) {
		यदि (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		अन्यथा
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} अन्यथा {
		यदि (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		अन्यथा
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = असत्य;
}

शून्य Ff_दत्त::unmap_srst() {
	यदि (!has_srst)
		प्रत्यागम;
	यदि (has_ce && !ce_over_srst)
		unmap_ce();

	यदि (!is_fine) {
		यदि (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		अन्यथा
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} अन्यथा {
		यदि (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		अन्यथा
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = असत्य;
}

Cell *Ff_दत्त::emit() {
	remove();
	यदि (!विस्तारः)
		प्रत्यागम शून्यसूचकः;
	यदि (!has_aload && !has_clk && !has_gclk && !has_sr) {
		यदि (has_arst) {
			// Convert this case to a D latch.
			arst_गन्तव्यम्_aload();
		} अन्यथा {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			प्रत्यागम शून्यसूचकः;
		}
	}
	यदि (initvals && !is_anyinit)
		initvals->समुच्चयः_init(sig_q, val_init);
	यदि (!is_fine) {
		यदि (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			यदि (is_anyinit) {
				cell = module->सङ्कलय_Anyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} अन्यथा {
				cell = module->सङ्कलय_Ff(name, sig_d, sig_q);
			}
		} अन्यथा यदि (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->सङ्कलय_Sr(name, sig_समुच्चयः, sig_clr, sig_q, pol_समुच्चयः, pol_clr);
		} अन्यथा यदि (!has_clk) {
			log_assert(!has_srst);
			यदि (has_sr)
				cell = module->सङ्कलय_Dlatchsr(name, sig_aload, sig_समुच्चयः, sig_clr, sig_ad, sig_q, pol_aload, pol_समुच्चयः, pol_clr);
			अन्यथा यदि (has_arst)
				cell = module->सङ्कलय_Adlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			अन्यथा
				cell = module->सङ्कलय_Dlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} अन्यथा {
			यदि (has_sr) {
				यदि (has_ce)
					cell = module->सङ्कलय_Dffsre(name, sig_clk, sig_ce, sig_समुच्चयः, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_समुच्चयः, pol_clr);
				अन्यथा
					cell = module->सङ्कलय_Dffsr(name, sig_clk, sig_समुच्चयः, sig_clr, sig_d, sig_q, pol_clk, pol_समुच्चयः, pol_clr);
			} अन्यथा यदि (has_arst) {
				यदि (has_ce)
					cell = module->सङ्कलय_Adffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				अन्यथा
					cell = module->सङ्कलय_Adff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} अन्यथा यदि (has_aload) {
				यदि (has_ce)
					cell = module->सङ्कलय_Aldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				अन्यथा
					cell = module->सङ्कलय_Aldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} अन्यथा यदि (has_srst) {
				यदि (has_ce)
					यदि (ce_over_srst)
						cell = module->सङ्कलय_Sdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					अन्यथा
						cell = module->सङ्कलय_Sdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				अन्यथा
					cell = module->सङ्कलय_Sdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} अन्यथा {
				यदि (has_ce)
					cell = module->सङ्कलय_Dffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				अन्यथा
					cell = module->सङ्कलय_Dff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} अन्यथा {
		यदि (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->सङ्कलय_Ff_Gate(name, sig_d, sig_q);
		} अन्यथा यदि (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->सङ्कलय_Sr_Gate(name, sig_समुच्चयः, sig_clr, sig_q, pol_समुच्चयः, pol_clr);
		} अन्यथा यदि (!has_clk) {
			log_assert(!has_srst);
			यदि (has_sr)
				cell = module->सङ्कलय_Dlatchsr_Gate(name, sig_aload, sig_समुच्चयः, sig_clr, sig_ad, sig_q, pol_aload, pol_समुच्चयः, pol_clr);
			अन्यथा यदि (has_arst)
				cell = module->सङ्कलय_Adlatch_Gate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			अन्यथा
				cell = module->सङ्कलय_Dlatch_Gate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} अन्यथा {
			यदि (has_sr) {
				यदि (has_ce)
					cell = module->सङ्कलय_Dffsre_Gate(name, sig_clk, sig_ce, sig_समुच्चयः, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_समुच्चयः, pol_clr);
				अन्यथा
					cell = module->सङ्कलय_Dffsr_Gate(name, sig_clk, sig_समुच्चयः, sig_clr, sig_d, sig_q, pol_clk, pol_समुच्चयः, pol_clr);
			} अन्यथा यदि (has_arst) {
				यदि (has_ce)
					cell = module->सङ्कलय_Adffe_Gate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				अन्यथा
					cell = module->सङ्कलय_Adff_Gate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} अन्यथा यदि (has_aload) {
				यदि (has_ce)
					cell = module->सङ्कलय_Aldffe_Gate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				अन्यथा
					cell = module->सङ्कलय_Aldff_Gate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} अन्यथा यदि (has_srst) {
				यदि (has_ce)
					यदि (ce_over_srst)
						cell = module->सङ्कलय_Sdffce_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					अन्यथा
						cell = module->सङ्कलय_Sdffe_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				अन्यथा
					cell = module->सङ्कलय_Sdff_Gate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} अन्यथा {
				यदि (has_ce)
					cell = module->सङ्कलय_Dffe_Gate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				अन्यथा
					cell = module->सङ्कलय_Dff_Gate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->गुणाः = गुणाः;
	प्रत्यागम cell;
}

शून्य Ff_दत्त::remove() {
	यदि (cell) {
		remove_init();
		module->remove(cell);
		cell = शून्यसूचकः;
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

शून्य Ff_दत्त::flip_rst_द्वयङ्काः(ध्रुव pool<पूर्णाङ्क> &द्वयङ्काः) {
	यदि (!द्वयङ्काः.आकार())
		प्रत्यागम;

	remove_init();

	प्रति (स्वचालित bit: द्वयङ्काः) {
		यदि (has_arst)
			val_arst.समुच्चयः(bit, invert(val_arst[bit]));
		यदि (has_srst)
			val_srst.समुच्चयः(bit, invert(val_srst[bit]));
		val_init.समुच्चयः(bit, invert(val_init[bit]));
	}
}

शून्य Ff_दत्त::flip_द्वयङ्काः(ध्रुव pool<पूर्णाङ्क> &द्वयङ्काः) {
	यदि (!द्वयङ्काः.आकार())
		प्रत्यागम;

	flip_rst_द्वयङ्काः(द्वयङ्काः);

	Wire *new_q = module->सङ्कलय_Wire(NEW_ID, विस्तारः);

	यदि (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	यदि (is_fine) {
		यदि (has_sr) {
			तार्किक new_pol_clr = pol_समुच्चयः;
			SigSpec new_sig_clr;
			यदि (pol_समुच्चयः) {
				यदि (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_समुच्चयः, sig_clr);
				} अन्यथा {
					new_sig_clr = module->AndGate(NEW_ID, sig_समुच्चयः, sig_clr);
				}
			} अन्यथा {
				यदि (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_समुच्चयः, sig_clr);
				} अन्यथा {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_समुच्चयः, sig_clr);
				}
			}
			pol_समुच्चयः = pol_clr;
			sig_समुच्चयः = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		यदि (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		यदि (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->सङ्कलय_Not_Gate(NEW_ID, new_q, sig_q);
	}
	अन्यथा
	{
		यदि (has_sr) {
			SigSpec not_clr;
			यदि (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = सत्य;
			} अन्यथा {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			यदि (!pol_समुच्चयः) {
				sig_समुच्चयः = module->Not(NEW_ID, sig_समुच्चयः);
				pol_समुच्चयः = सत्य;
			}

			SigSpec masked_समुच्चयः = module->And(NEW_ID, sig_समुच्चयः, not_clr);
			प्रति (स्वचालित bit: द्वयङ्काः) {
				sig_समुच्चयः[bit] = sig_clr[bit];
				sig_clr[bit] = masked_समुच्चयः[bit];
			}
		}

		Const आवरणम् = Const(State::S0, विस्तारः);
		प्रति (स्वचालित bit: द्वयङ्काः)
			आवरणम्.समुच्चयः(bit, State::S1);

		यदि (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, आवरणम्);
		यदि (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, आवरणम्);
		module->सङ्कलय_Xor(NEW_ID, new_q, आवरणम्, sig_q);
	}

	sig_q = new_q;
}
