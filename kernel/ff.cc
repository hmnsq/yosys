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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/ff.h"

USING_YOSYS_NAMESPACE

// sorry
ਸਾਂਚਾ<ਕਿਸਮ_ਨਾਂ ਨਿਵੇਸ਼_Type, ਕਿਸਮ_ਨਾਂ OutputType, ਕਿਸਮ_ਨਾਂ = std::enable_if_t<std::is_ਅਧਾਰ_ਦਾ_v<Ff_Type_ਅੰਕੜੇ, OutputType>>>
ਖਾਲੀ manufacture_info(ਨਿਵੇਸ਼_Type flop, OutputType& info, FfInitVals *initvals) {
	Cell* cell = ਖਾਲੀ_ਸੂਚਕ;
	IdString type;
	constexpr ਤਾਰਕਿਕ have_cell = std::is_same_v<ਨਿਵੇਸ਼_Type, Cell*>;
	ਜੇ constexpr (std::is_same_v<ਨਿਵੇਸ਼_Type, IdString>) {
		type = flop;
	} ਨਹੀਂਤਾਂ {
		static_assert(std::is_same_v<ਨਿਵੇਸ਼_Type, Cell*>);
		cell = flop;
		type = flop->type;
	}
	ਜੇ constexpr (have_cell) {
		info.sig_q = cell->get_ਪੋਰਟ(ID::Q);
		info.ਚੌੜਾਈ = Get_ਮਾਪ(info.sig_q);
		info.ਗੁਣ = cell->ਗੁਣ;
		ਜੇ (initvals)
			info.val_init = (*initvals)(info.sig_q);
	}


	std::string type_str = ID::str(type);

	ਜੇ (type.ਵਿੱਚ(ID($anyinit), ID($ff), ID($dff), ID($dffe), ID($dffsr), ID($dffsre), ID($adff), ID($adffe), ID($aldff), ID($aldffe), ID($sdff), ID($sdffe), ID($sdffce), ID($dlatch), ID($adlatch), ID($dlatchsr), ID($sr))) {
		ਜੇ (type.ਵਿੱਚ(ID($anyinit), ID($ff))) {
			info.has_gclk = ਸੱਚ;
			ਜੇ constexpr (have_cell)
				info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			ਜੇ (type == ID($anyinit)) {
				info.is_anyinit = ਸੱਚ;
				ਜੇ constexpr (have_cell)
					log_assert(info.val_init.is_fully_undef());
			}
		} ਨਹੀਂਤਾਂ ਜੇ (type == ID($sr)) {
			// No data input at all.
		} ਨਹੀਂਤਾਂ ਜੇ (type.ਵਿੱਚ(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
			info.has_aload = ਸੱਚ;
			ਜੇ constexpr (have_cell) {
				info.sig_aload = cell->get_ਪੋਰਟ(ID::EN);
				info.pol_aload = cell->getParam(ID::EN_POLARITY).as_bool();
				info.sig_ad = cell->get_ਪੋਰਟ(ID::D);
			}
		} ਨਹੀਂਤਾਂ {
			info.has_clk = ਸੱਚ;
			ਜੇ constexpr (have_cell) {
				info.sig_clk = cell->get_ਪੋਰਟ(ID::CLK);
				info.pol_clk = cell->getParam(ID::CLK_POLARITY).as_bool();
				info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			}
		}
		ਜੇ (type.ਵਿੱਚ(ID($dffe), ID($dffsre), ID($adffe), ID($aldffe), ID($sdffe), ID($sdffce))) {
			info.has_ce = ਸੱਚ;
			ਜੇ constexpr (have_cell) {
				info.sig_ce = cell->get_ਪੋਰਟ(ID::EN);
				info.pol_ce = cell->getParam(ID::EN_POLARITY).as_bool();
			}
		}
		ਜੇ (type.ਵਿੱਚ(ID($dffsr), ID($dffsre), ID($dlatchsr), ID($sr))) {
			info.has_sr = ਸੱਚ;
			ਜੇ constexpr (have_cell) {
				info.sig_clr = cell->get_ਪੋਰਟ(ID::CLR);
				info.sig_ਸੈੱਟ = cell->get_ਪੋਰਟ(ID::ਸੈੱਟ_2);
				info.pol_clr = cell->getParam(ID::CLR_POLARITY).as_bool();
				info.pol_ਸੈੱਟ = cell->getParam(ID::ਸੈੱਟ_POLARITY).as_bool();
			}
		}
		ਜੇ (type.ਵਿੱਚ(ID($aldff), ID($aldffe))) {
			info.has_aload = ਸੱਚ;
			ਜੇ constexpr (have_cell) {
				info.sig_aload = cell->get_ਪੋਰਟ(ID::ALOAD);
				info.pol_aload = cell->getParam(ID::ALOAD_POLARITY).as_bool();
				info.sig_ad = cell->get_ਪੋਰਟ(ID::AD);
			}
		}
		ਜੇ (type.ਵਿੱਚ(ID($adff), ID($adffe), ID($adlatch))) {
			info.has_arst = ਸੱਚ;
			ਜੇ constexpr (have_cell) {
				info.sig_arst = cell->get_ਪੋਰਟ(ID::ARST);
				info.pol_arst = cell->getParam(ID::ARST_POLARITY).as_bool();
				info.val_arst = cell->getParam(ID::ARST_ਮੁੱਲ);
			}
		}
		ਜੇ (type.ਵਿੱਚ(ID($sdff), ID($sdffe), ID($sdffce))) {
			info.has_srst = ਸੱਚ;
			ਜੇ constexpr (have_cell) {
				info.sig_srst = cell->get_ਪੋਰਟ(ID::SRST);
				info.pol_srst = cell->getParam(ID::SRST_POLARITY).as_bool();
				info.val_srst = cell->getParam(ID::SRST_ਮੁੱਲ);
			}
			info.ce_over_srst = type == ID($sdffce);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type == ID($_FF_)) {
		info.is_fine = ਸੱਚ;
		info.has_gclk = ਸੱਚ;
		ਜੇ constexpr (have_cell)
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 5) == "$_SR_") {
		info.is_fine = ਸੱਚ;
		info.has_sr = ਸੱਚ;
		info.pol_ਸੈੱਟ = type_str[5] == 'P';
		info.pol_clr = type_str[6] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_ਸੈੱਟ = cell->get_ਪੋਰਟ(ID::S);
			info.sig_clr = cell->get_ਪੋਰਟ(ID::R);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 6) == "$_DFF_" && type_str.ਮਾਪ() == 8) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[6] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 7) == "$_DFFE_" && type_str.ਮਾਪ() == 10) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[7] == 'P';
		info.has_ce = ਸੱਚ;
		info.pol_ce = type_str[8] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_ce = cell->get_ਪੋਰਟ(ID::E);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 6) == "$_DFF_" && type_str.ਮਾਪ() == 10) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[6] == 'P';
		info.has_arst = ਸੱਚ;
		info.pol_arst = type_str[7] == 'P';
		info.val_arst = type_str[8] == '1' ? State::S1 : State::S0;
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_arst = cell->get_ਪੋਰਟ(ID::R);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 7) == "$_DFFE_" && type_str.ਮਾਪ() == 12) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[7] == 'P';
		info.has_arst = ਸੱਚ;
		info.pol_arst = type_str[8] == 'P';
		info.val_arst = type_str[9] == '1' ? State::S1 : State::S0;
		info.has_ce = ਸੱਚ;
		info.pol_ce = type_str[10] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_arst = cell->get_ਪੋਰਟ(ID::R);
			info.sig_ce = cell->get_ਪੋਰਟ(ID::E);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 8) == "$_ALDFF_" && type_str.ਮਾਪ() == 11) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[8] == 'P';
		info.has_aload = ਸੱਚ;
		info.pol_aload = type_str[9] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_aload = cell->get_ਪੋਰਟ(ID::L);
			info.sig_ad = cell->get_ਪੋਰਟ(ID::AD);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 9) == "$_ALDFFE_" && type_str.ਮਾਪ() == 13) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[9] == 'P';
		info.has_aload = ਸੱਚ;
		info.pol_aload = type_str[10] == 'P';
		info.has_ce = ਸੱਚ;
		info.pol_ce = type_str[11] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_aload = cell->get_ਪੋਰਟ(ID::L);
			info.sig_ad = cell->get_ਪੋਰਟ(ID::AD);
			info.sig_ce = cell->get_ਪੋਰਟ(ID::E);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 8) == "$_DFFSR_" && type_str.ਮਾਪ() == 12) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[8] == 'P';
		info.has_sr = ਸੱਚ;
		info.pol_ਸੈੱਟ = type_str[9] == 'P';
		info.pol_clr = type_str[10] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_ਸੈੱਟ = cell->get_ਪੋਰਟ(ID::S);
			info.sig_clr = cell->get_ਪੋਰਟ(ID::R);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 9) == "$_DFFSRE_" && type_str.ਮਾਪ() == 14) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[9] == 'P';
		info.has_sr = ਸੱਚ;
		info.pol_ਸੈੱਟ = type_str[10] == 'P';
		info.pol_clr = type_str[11] == 'P';
		info.has_ce = ਸੱਚ;
		info.pol_ce = type_str[12] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_ਸੈੱਟ = cell->get_ਪੋਰਟ(ID::S);
			info.sig_clr = cell->get_ਪੋਰਟ(ID::R);
			info.sig_ce = cell->get_ਪੋਰਟ(ID::E);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 7) == "$_SDFF_" && type_str.ਮਾਪ() == 11) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[7] == 'P';
		info.has_srst = ਸੱਚ;
		info.pol_srst = type_str[8] == 'P';
		info.val_srst = type_str[9] == '1' ? State::S1 : State::S0;
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_srst = cell->get_ਪੋਰਟ(ID::R);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 8) == "$_SDFFE_" && type_str.ਮਾਪ() == 13) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[8] == 'P';
		info.has_srst = ਸੱਚ;
		info.pol_srst = type_str[9] == 'P';
		info.val_srst = type_str[10] == '1' ? State::S1 : State::S0;
		info.has_ce = ਸੱਚ;
		info.pol_ce = type_str[11] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_srst = cell->get_ਪੋਰਟ(ID::R);
			info.sig_ce = cell->get_ਪੋਰਟ(ID::E);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 9) == "$_SDFFCE_" && type_str.ਮਾਪ() == 14) {
		info.is_fine = ਸੱਚ;
		info.has_clk = ਸੱਚ;
		info.pol_clk = type_str[9] == 'P';
		info.has_srst = ਸੱਚ;
		info.pol_srst = type_str[10] == 'P';
		info.val_srst = type_str[11] == '1' ? State::S1 : State::S0;
		info.has_ce = ਸੱਚ;
		info.pol_ce = type_str[12] == 'P';
		info.ce_over_srst = ਸੱਚ;
		ਜੇ constexpr (have_cell) {
			info.sig_d = cell->get_ਪੋਰਟ(ID::D);
			info.sig_clk = cell->get_ਪੋਰਟ(ID::C);
			info.sig_srst = cell->get_ਪੋਰਟ(ID::R);
			info.sig_ce = cell->get_ਪੋਰਟ(ID::E);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.ਮਾਪ() == 11) {
		info.is_fine = ਸੱਚ;
		info.has_aload = ਸੱਚ;
		info.has_aload = ਸੱਚ;
		info.pol_aload = type_str[9] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_ad = cell->get_ਪੋਰਟ(ID::D);
			info.sig_aload = cell->get_ਪੋਰਟ(ID::E);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 9) == "$_DLATCH_" && type_str.ਮਾਪ() == 13) {
		info.is_fine = ਸੱਚ;
		info.has_aload = ਸੱਚ;
		info.has_aload = ਸੱਚ;
		info.pol_aload = type_str[9] == 'P';
		info.has_arst = ਸੱਚ;
		info.pol_arst = type_str[10] == 'P';
		info.val_arst = type_str[11] == '1' ? State::S1 : State::S0;
		ਜੇ constexpr (have_cell) {
			info.sig_ad = cell->get_ਪੋਰਟ(ID::D);
			info.sig_aload = cell->get_ਪੋਰਟ(ID::E);
			info.sig_arst = cell->get_ਪੋਰਟ(ID::R);
		}
	} ਨਹੀਂਤਾਂ ਜੇ (type_str.substr(0, 11) == "$_DLATCHSR_" && type_str.ਮਾਪ() == 15) {
		info.is_fine = ਸੱਚ;
		info.has_aload = ਸੱਚ;
		info.has_aload = ਸੱਚ;
		info.pol_aload = type_str[11] == 'P';
		info.has_sr = ਸੱਚ;
		info.pol_ਸੈੱਟ = type_str[12] == 'P';
		info.pol_clr = type_str[13] == 'P';
		ਜੇ constexpr (have_cell) {
			info.sig_ad = cell->get_ਪੋਰਟ(ID::D);
			info.sig_aload = cell->get_ਪੋਰਟ(ID::E);
			info.sig_ਸੈੱਟ = cell->get_ਪੋਰਟ(ID::S);
			info.sig_clr = cell->get_ਪੋਰਟ(ID::R);
		}
	} ਨਹੀਂਤਾਂ {
		log_assert(0);
	}
	ਜੇ constexpr (have_cell)
		ਜੇ (info.has_aload && !info.has_clk && !info.has_sr && !info.has_arst && info.sig_ad.is_fully_const()) {
			// Plain D latches with const D treated specially.
			info.has_aload = ਝੂਠ;
			info.has_arst = ਸੱਚ;
			info.sig_arst = info.sig_aload;
			info.pol_arst = info.pol_aload;
			info.val_arst = info.sig_ad.as_const();
		}
}

Ff_Type_ਅੰਕੜੇ::Ff_Type_ਅੰਕੜੇ(IdString type) : Ff_Type_ਅੰਕੜੇ()
{
	manufacture_info(type, *ਇਹ, ਖਾਲੀ_ਸੂਚਕ);
}

Ff_ਅੰਕੜੇ::Ff_ਅੰਕੜੇ(FfInitVals *initvals, Cell *cell_) : Ff_ਅੰਕੜੇ(cell_->module, initvals, cell_->name)
{
	cell = cell_;
	manufacture_info(cell, *ਇਹ, initvals);
}

Ff_ਅੰਕੜੇ Ff_ਅੰਕੜੇ::ਟੁਕੜਾ(ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &ਬਿੱਟ) {
	Ff_ਅੰਕੜੇ res(module, initvals, module->twines().ਜੋੜੋ(NEW_ID));
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
	res.pol_ਸੈੱਟ = pol_ਸੈੱਟ;
	res.ਗੁਣ = ਗੁਣ;
	std::optional<Const::Builder> arst_ਬਿੱਟ;
	ਜੇ (has_arst)
		arst_ਬਿੱਟ.emplace(ਬਿੱਟ.ਮਾਪ());
	std::optional<Const::Builder> srst_ਬਿੱਟ;
	ਜੇ (has_srst)
		srst_ਬਿੱਟ.emplace(ਬਿੱਟ.ਮਾਪ());
	std::optional<Const::Builder> init_ਬਿੱਟ;
	ਜੇ (initvals)
		init_ਬਿੱਟ.emplace(ਬਿੱਟ.ਮਾਪ());

	ਲਈ (ਪੂਰਨ i : ਬਿੱਟ) {
		res.sig_q.append(sig_q[i]);
		ਜੇ (has_clk || has_gclk)
			res.sig_d.append(sig_d[i]);
		ਜੇ (has_aload)
			res.sig_ad.append(sig_ad[i]);
		ਜੇ (has_sr) {
			res.sig_clr.append(sig_clr[i]);
			res.sig_ਸੈੱਟ.append(sig_ਸੈੱਟ[i]);
		}
		ਜੇ (has_arst)
			arst_ਬਿੱਟ->ਧੱਕੋ_ਪਿੱਠ(val_arst[i]);
		ਜੇ (has_srst)
			srst_ਬਿੱਟ->ਧੱਕੋ_ਪਿੱਠ(val_srst[i]);
		ਜੇ (initvals)
			init_ਬਿੱਟ->ਧੱਕੋ_ਪਿੱਠ(val_init[i]);
	}

	ਜੇ (has_arst)
		res.val_arst = arst_ਬਿੱਟ->build();
	ਜੇ (has_srst)
		res.val_srst = srst_ਬਿੱਟ->build();
	ਜੇ (initvals)
		res.val_init = init_ਬਿੱਟ->build();
	res.ਚੌੜਾਈ = Get_ਮਾਪ(res.sig_q);
	ਵਾਪਸ res;
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::ਜੋੜੋ_dummy_ce() {
	ਜੇ (has_ce)
		ਵਾਪਸ;
	has_ce = ਸੱਚ;
	pol_ce = ਸੱਚ;
	sig_ce = State::S1;
	ce_over_srst = ਝੂਠ;
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::ਜੋੜੋ_dummy_srst() {
	ਜੇ (has_srst)
		ਵਾਪਸ;
	has_srst = ਸੱਚ;
	pol_srst = ਸੱਚ;
	sig_srst = State::S0;
	val_srst = Const(State::Sx, ਚੌੜਾਈ);
	ce_over_srst = ਝੂਠ;
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::ਜੋੜੋ_dummy_arst() {
	ਜੇ (has_arst)
		ਵਾਪਸ;
	has_arst = ਸੱਚ;
	pol_arst = ਸੱਚ;
	sig_arst = State::S0;
	val_arst = Const(State::Sx, ਚੌੜਾਈ);
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::ਜੋੜੋ_dummy_aload() {
	ਜੇ (has_aload)
		ਵਾਪਸ;
	has_aload = ਸੱਚ;
	pol_aload = ਸੱਚ;
	sig_aload = State::S0;
	sig_ad = Const(State::Sx, ਚੌੜਾਈ);
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::ਜੋੜੋ_dummy_sr() {
	ਜੇ (has_sr)
		ਵਾਪਸ;
	has_sr = ਸੱਚ;
	pol_clr = ਸੱਚ;
	pol_ਸੈੱਟ = ਸੱਚ;
	sig_clr = Const(State::S0, ਚੌੜਾਈ);
	sig_ਸੈੱਟ = Const(State::S0, ਚੌੜਾਈ);
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::ਜੋੜੋ_dummy_clk() {
	ਜੇ (has_clk)
		ਵਾਪਸ;
	has_clk = ਸੱਚ;
	pol_clk = ਸੱਚ;
	sig_clk = State::S0;
	sig_d = Const(State::Sx, ਚੌੜਾਈ);
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::arst_ਮੰਜ਼ਿਲ_aload() {
	log_assert(has_arst);
	log_assert(!has_aload);
	pol_aload = pol_arst;
	sig_aload = sig_arst;
	sig_ad = val_arst;
	has_aload = ਸੱਚ;
	has_arst = ਝੂਠ;
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::arst_ਮੰਜ਼ਿਲ_sr() {
	log_assert(has_arst);
	log_assert(!has_sr);
	pol_clr = pol_arst;
	pol_ਸੈੱਟ = pol_arst;
	sig_clr = Const(pol_arst ? State::S0 : State::S1, ਚੌੜਾਈ);
	sig_ਸੈੱਟ = Const(pol_arst ? State::S0 : State::S1, ਚੌੜਾਈ);
	has_sr = ਸੱਚ;
	has_arst = ਝੂਠ;
	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++) {
		ਜੇ (val_arst[i] == State::S1)
			sig_ਸੈੱਟ[i] = sig_arst;
		ਨਹੀਂਤਾਂ
			sig_clr[i] = sig_arst;
	}
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::aload_ਮੰਜ਼ਿਲ_sr() {
	log_assert(has_aload);
	log_assert(!has_sr);
	has_sr = ਸੱਚ;
	has_aload = ਝੂਠ;
	ਜੇ (!is_fine) {
		pol_clr = ਝੂਠ;
		pol_ਸੈੱਟ = ਸੱਚ;
		ਜੇ (pol_aload) {
			sig_clr = module->Mux(NEW_ID, Const(State::S1, ਚੌੜਾਈ), sig_ad, sig_aload);
			sig_ਸੈੱਟ = module->Mux(NEW_ID, Const(State::S0, ਚੌੜਾਈ), sig_ad, sig_aload);
		} ਨਹੀਂਤਾਂ {
			sig_clr = module->Mux(NEW_ID, sig_ad, Const(State::S1, ਚੌੜਾਈ), sig_aload);
			sig_ਸੈੱਟ = module->Mux(NEW_ID, sig_ad, Const(State::S0, ਚੌੜਾਈ), sig_aload);
		}
	} ਨਹੀਂਤਾਂ {
		pol_clr = pol_aload;
		pol_ਸੈੱਟ = pol_aload;
		ਜੇ (pol_aload) {
			sig_clr = module->AndnotGate(NEW_ID, sig_aload, sig_ad);
			sig_ਸੈੱਟ = module->AndGate(NEW_ID, sig_aload, sig_ad);
		} ਨਹੀਂਤਾਂ {
			sig_clr = module->OrGate(NEW_ID, sig_aload, sig_ad);
			sig_ਸੈੱਟ = module->OrnotGate(NEW_ID, sig_aload, sig_ad);
		}
	}
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::convert_ce_over_srst(ਤਾਰਕਿਕ val) {
	ਜੇ (!has_ce || !has_srst || ce_over_srst == val)
		ਵਾਪਸ;
	ਜੇ (val) {
		// sdffe to sdffce
		ਜੇ (!is_fine) {
			ਜੇ (pol_ce) {
				ਜੇ (pol_srst) {
					sig_ce = module->Or(NEW_ID, sig_ce, sig_srst);
				} ਨਹੀਂਤਾਂ {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->Or(NEW_ID, sig_ce, tmp);
				}
			} ਨਹੀਂਤਾਂ {
				ਜੇ (pol_srst) {
					SigSpec tmp = module->Not(NEW_ID, sig_srst);
					sig_ce = module->And(NEW_ID, sig_ce, tmp);
				} ਨਹੀਂਤਾਂ {
					sig_ce = module->And(NEW_ID, sig_ce, sig_srst);
				}
			}
		} ਨਹੀਂਤਾਂ {
			ਜੇ (pol_ce) {
				ਜੇ (pol_srst) {
					sig_ce = module->OrGate(NEW_ID, sig_ce, sig_srst);
				} ਨਹੀਂਤਾਂ {
					sig_ce = module->OrnotGate(NEW_ID, sig_ce, sig_srst);
				}
			} ਨਹੀਂਤਾਂ {
				ਜੇ (pol_srst) {
					sig_ce = module->AndnotGate(NEW_ID, sig_ce, sig_srst);
				} ਨਹੀਂਤਾਂ {
					sig_ce = module->AndGate(NEW_ID, sig_ce, sig_srst);
				}
			}
		}
	} ਨਹੀਂਤਾਂ {
		// sdffce to sdffe
		ਜੇ (!is_fine) {
			ਜੇ (pol_srst) {
				ਜੇ (pol_ce) {
					sig_srst = cell->module->And(NEW_ID, sig_srst, sig_ce);
				} ਨਹੀਂਤਾਂ {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->And(NEW_ID, sig_srst, tmp);
				}
			} ਨਹੀਂਤਾਂ {
				ਜੇ (pol_ce) {
					SigSpec tmp = module->Not(NEW_ID, sig_ce);
					sig_srst = cell->module->Or(NEW_ID, sig_srst, tmp);
				} ਨਹੀਂਤਾਂ {
					sig_srst = cell->module->Or(NEW_ID, sig_srst, sig_ce);
				}
			}
		} ਨਹੀਂਤਾਂ {
			ਜੇ (pol_srst) {
				ਜੇ (pol_ce) {
					sig_srst = cell->module->AndGate(NEW_ID, sig_srst, sig_ce);
				} ਨਹੀਂਤਾਂ {
					sig_srst = cell->module->AndnotGate(NEW_ID, sig_srst, sig_ce);
				}
			} ਨਹੀਂਤਾਂ {
				ਜੇ (pol_ce) {
					sig_srst = cell->module->OrnotGate(NEW_ID, sig_srst, sig_ce);
				} ਨਹੀਂਤਾਂ {
					sig_srst = cell->module->OrGate(NEW_ID, sig_srst, sig_ce);
				}
			}
		}
	}
	ce_over_srst = val;
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::unmap_ce() {
	ਜੇ (!has_ce)
		ਵਾਪਸ;
	log_assert(has_clk);
	ਜੇ (has_srst && ce_over_srst)
		unmap_srst();

	ਜੇ (!is_fine) {
		ਜੇ (pol_ce)
			sig_d = module->Mux(NEW_ID, sig_q, sig_d, sig_ce);
		ਨਹੀਂਤਾਂ
			sig_d = module->Mux(NEW_ID, sig_d, sig_q, sig_ce);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (pol_ce)
			sig_d = module->MuxGate(NEW_ID, sig_q, sig_d, sig_ce);
		ਨਹੀਂਤਾਂ
			sig_d = module->MuxGate(NEW_ID, sig_d, sig_q, sig_ce);
	}
	has_ce = ਝੂਠ;
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::unmap_srst() {
	ਜੇ (!has_srst)
		ਵਾਪਸ;
	ਜੇ (has_ce && !ce_over_srst)
		unmap_ce();

	ਜੇ (!is_fine) {
		ਜੇ (pol_srst)
			sig_d = module->Mux(NEW_ID, sig_d, val_srst, sig_srst);
		ਨਹੀਂਤਾਂ
			sig_d = module->Mux(NEW_ID, val_srst, sig_d, sig_srst);
	} ਨਹੀਂਤਾਂ {
		ਜੇ (pol_srst)
			sig_d = module->MuxGate(NEW_ID, sig_d, val_srst[0], sig_srst);
		ਨਹੀਂਤਾਂ
			sig_d = module->MuxGate(NEW_ID, val_srst[0], sig_d, sig_srst);
	}
	has_srst = ਝੂਠ;
}

Cell *Ff_ਅੰਕੜੇ::emit() {
	remove();
	ਜੇ (!ਚੌੜਾਈ)
		ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
	ਜੇ (!has_aload && !has_clk && !has_gclk && !has_sr) {
		ਜੇ (has_arst) {
			// Convert this case to a D latch.
			arst_ਮੰਜ਼ਿਲ_aload();
		} ਨਹੀਂਤਾਂ {
			// No control inputs left.  Turn into a const driver.
			module->connect(sig_q, val_init);
			ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
		}
	}
	ਜੇ (initvals && !is_anyinit)
		initvals->ਸੈੱਟ_init(sig_q, val_init);
	ਜੇ (!is_fine) {
		ਜੇ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			ਜੇ (is_anyinit) {
				cell = module->ਜੋੜੋ_Anyinit(name, sig_d, sig_q);
				log_assert(val_init.is_fully_undef());
			} ਨਹੀਂਤਾਂ {
				cell = module->ਜੋੜੋ_Ff(name, sig_d, sig_q);
			}
		} ਨਹੀਂਤਾਂ ਜੇ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->ਜੋੜੋ_Sr(name, sig_ਸੈੱਟ, sig_clr, sig_q, pol_ਸੈੱਟ, pol_clr);
		} ਨਹੀਂਤਾਂ ਜੇ (!has_clk) {
			log_assert(!has_srst);
			ਜੇ (has_sr)
				cell = module->ਜੋੜੋ_Dlatchsr(name, sig_aload, sig_ਸੈੱਟ, sig_clr, sig_ad, sig_q, pol_aload, pol_ਸੈੱਟ, pol_clr);
			ਨਹੀਂਤਾਂ ਜੇ (has_arst)
				cell = module->ਜੋੜੋ_Adlatch(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst, pol_aload, pol_arst);
			ਨਹੀਂਤਾਂ
				cell = module->ਜੋੜੋ_Dlatch(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ਨਹੀਂਤਾਂ {
			ਜੇ (has_sr) {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Dffsre(name, sig_clk, sig_ce, sig_ਸੈੱਟ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_ਸੈੱਟ, pol_clr);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Dffsr(name, sig_clk, sig_ਸੈੱਟ, sig_clr, sig_d, sig_q, pol_clk, pol_ਸੈੱਟ, pol_clr);
			} ਨਹੀਂਤਾਂ ਜੇ (has_arst) {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Adffe(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_ce, pol_arst);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Adff(name, sig_clk, sig_arst, sig_d, sig_q, val_arst, pol_clk, pol_arst);
			} ਨਹੀਂਤਾਂ ਜੇ (has_aload) {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Aldffe(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Aldff(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ਨਹੀਂਤਾਂ ਜੇ (has_srst) {
				ਜੇ (has_ce)
					ਜੇ (ce_over_srst)
						cell = module->ਜੋੜੋ_Sdffce(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
					ਨਹੀਂਤਾਂ
						cell = module->ਜੋੜੋ_Sdffe(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_ce, pol_srst);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Sdff(name, sig_clk, sig_srst, sig_d, sig_q, val_srst, pol_clk, pol_srst);
			} ਨਹੀਂਤਾਂ {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Dffe(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Dff(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	} ਨਹੀਂਤਾਂ {
		ਜੇ (has_gclk) {
			log_assert(!has_clk);
			log_assert(!has_ce);
			log_assert(!has_aload);
			log_assert(!has_arst);
			log_assert(!has_srst);
			log_assert(!has_sr);
			log_assert(!is_anyinit);
			cell = module->ਜੋੜੋ_Ff_Gate(name, sig_d, sig_q);
		} ਨਹੀਂਤਾਂ ਜੇ (!has_aload && !has_clk) {
			log_assert(has_sr);
			cell = module->ਜੋੜੋ_Sr_Gate(name, sig_ਸੈੱਟ, sig_clr, sig_q, pol_ਸੈੱਟ, pol_clr);
		} ਨਹੀਂਤਾਂ ਜੇ (!has_clk) {
			log_assert(!has_srst);
			ਜੇ (has_sr)
				cell = module->ਜੋੜੋ_Dlatchsr_Gate(name, sig_aload, sig_ਸੈੱਟ, sig_clr, sig_ad, sig_q, pol_aload, pol_ਸੈੱਟ, pol_clr);
			ਨਹੀਂਤਾਂ ਜੇ (has_arst)
				cell = module->ਜੋੜੋ_Adlatch_Gate(name, sig_aload, sig_arst, sig_ad, sig_q, val_arst.as_bool(), pol_aload, pol_arst);
			ਨਹੀਂਤਾਂ
				cell = module->ਜੋੜੋ_Dlatch_Gate(name, sig_aload, sig_ad, sig_q, pol_aload);
		} ਨਹੀਂਤਾਂ {
			ਜੇ (has_sr) {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Dffsre_Gate(name, sig_clk, sig_ce, sig_ਸੈੱਟ, sig_clr, sig_d, sig_q, pol_clk, pol_ce, pol_ਸੈੱਟ, pol_clr);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Dffsr_Gate(name, sig_clk, sig_ਸੈੱਟ, sig_clr, sig_d, sig_q, pol_clk, pol_ਸੈੱਟ, pol_clr);
			} ਨਹੀਂਤਾਂ ਜੇ (has_arst) {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Adffe_Gate(name, sig_clk, sig_ce, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_ce, pol_arst);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Adff_Gate(name, sig_clk, sig_arst, sig_d, sig_q, val_arst.as_bool(), pol_clk, pol_arst);
			} ਨਹੀਂਤਾਂ ਜੇ (has_aload) {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Aldffe_Gate(name, sig_clk, sig_ce, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_ce, pol_aload);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Aldff_Gate(name, sig_clk, sig_aload, sig_d, sig_q, sig_ad, pol_clk, pol_aload);
			} ਨਹੀਂਤਾਂ ਜੇ (has_srst) {
				ਜੇ (has_ce)
					ਜੇ (ce_over_srst)
						cell = module->ਜੋੜੋ_Sdffce_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
					ਨਹੀਂਤਾਂ
						cell = module->ਜੋੜੋ_Sdffe_Gate(name, sig_clk, sig_ce, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_ce, pol_srst);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Sdff_Gate(name, sig_clk, sig_srst, sig_d, sig_q, val_srst.as_bool(), pol_clk, pol_srst);
			} ਨਹੀਂਤਾਂ {
				ਜੇ (has_ce)
					cell = module->ਜੋੜੋ_Dffe_Gate(name, sig_clk, sig_ce, sig_d, sig_q, pol_clk, pol_ce);
				ਨਹੀਂਤਾਂ
					cell = module->ਜੋੜੋ_Dff_Gate(name, sig_clk, sig_d, sig_q, pol_clk);
			}
		}
	}
	cell->ਗੁਣ = ਗੁਣ;
	ਵਾਪਸ cell;
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::remove() {
	ਜੇ (cell) {
		remove_init();
		module->remove(cell);
		cell = ਖਾਲੀ_ਸੂਚਕ;
	}
}

ਨਾਮਾਕਾਸ਼ {
	State invert(State s) {
		ਚੋਣ (s) {
			ਹਾਲਤ State::S0: ਵਾਪਸ State::S1;
			ਹਾਲਤ State::S1: ਵਾਪਸ State::S0;
			ਮੂਲ: ਵਾਪਸ s;
		}
	}
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::flip_rst_ਬਿੱਟ(ਅਚਲ pool<ਪੂਰਨ> &ਬਿੱਟ) {
	ਜੇ (!ਬਿੱਟ.ਮਾਪ())
		ਵਾਪਸ;

	remove_init();

	ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ: ਬਿੱਟ) {
		ਜੇ (has_arst)
			val_arst.ਸੈੱਟ(ਦੋਆਧਾਰੀ_ਅੰਕ, invert(val_arst[ਦੋਆਧਾਰੀ_ਅੰਕ]));
		ਜੇ (has_srst)
			val_srst.ਸੈੱਟ(ਦੋਆਧਾਰੀ_ਅੰਕ, invert(val_srst[ਦੋਆਧਾਰੀ_ਅੰਕ]));
		val_init.ਸੈੱਟ(ਦੋਆਧਾਰੀ_ਅੰਕ, invert(val_init[ਦੋਆਧਾਰੀ_ਅੰਕ]));
	}
}

ਖਾਲੀ Ff_ਅੰਕੜੇ::flip_ਬਿੱਟ(ਅਚਲ pool<ਪੂਰਨ> &ਬਿੱਟ) {
	ਜੇ (!ਬਿੱਟ.ਮਾਪ())
		ਵਾਪਸ;

	flip_rst_ਬਿੱਟ(ਬਿੱਟ);

	Wire *new_q = module->ਜੋੜੋ_Wire(NEW_ID, ਚੌੜਾਈ);

	ਜੇ (has_sr && cell) {
		log_warning("Flipping D/Q/init and inserting priority fixup to legalize %s.%s [%s].\n", module->name.unescape(), cell->name.unescape(), cell->type.unescape());
	}

	ਜੇ (is_fine) {
		ਜੇ (has_sr) {
			ਤਾਰਕਿਕ new_pol_clr = pol_ਸੈੱਟ;
			SigSpec new_sig_clr;
			ਜੇ (pol_ਸੈੱਟ) {
				ਜੇ (pol_clr) {
					new_sig_clr = module->AndnotGate(NEW_ID, sig_ਸੈੱਟ, sig_clr);
				} ਨਹੀਂਤਾਂ {
					new_sig_clr = module->AndGate(NEW_ID, sig_ਸੈੱਟ, sig_clr);
				}
			} ਨਹੀਂਤਾਂ {
				ਜੇ (pol_clr) {
					new_sig_clr = module->OrGate(NEW_ID, sig_ਸੈੱਟ, sig_clr);
				} ਨਹੀਂਤਾਂ {
					new_sig_clr = module->OrnotGate(NEW_ID, sig_ਸੈੱਟ, sig_clr);
				}
			}
			pol_ਸੈੱਟ = pol_clr;
			sig_ਸੈੱਟ = sig_clr;
			pol_clr = new_pol_clr;
			sig_clr = new_sig_clr;
		}
		ਜੇ (has_clk || has_gclk)
			sig_d = module->NotGate(NEW_ID, sig_d);
		ਜੇ (has_aload)
			sig_ad = module->NotGate(NEW_ID, sig_ad);
		module->ਜੋੜੋ_Not_Gate(NEW_ID, new_q, sig_q);
	}
	ਨਹੀਂਤਾਂ
	{
		ਜੇ (has_sr) {
			SigSpec not_clr;
			ਜੇ (!pol_clr) {
				not_clr = sig_clr;
				sig_clr = module->Not(NEW_ID, sig_clr);
				pol_clr = ਸੱਚ;
			} ਨਹੀਂਤਾਂ {
				not_clr = module->Not(NEW_ID, sig_clr);
			}
			ਜੇ (!pol_ਸੈੱਟ) {
				sig_ਸੈੱਟ = module->Not(NEW_ID, sig_ਸੈੱਟ);
				pol_ਸੈੱਟ = ਸੱਚ;
			}

			SigSpec masked_ਸੈੱਟ = module->And(NEW_ID, sig_ਸੈੱਟ, not_clr);
			ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ: ਬਿੱਟ) {
				sig_ਸੈੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ] = sig_clr[ਦੋਆਧਾਰੀ_ਅੰਕ];
				sig_clr[ਦੋਆਧਾਰੀ_ਅੰਕ] = masked_ਸੈੱਟ[ਦੋਆਧਾਰੀ_ਅੰਕ];
			}
		}

		Const ਨਕਾਬ = Const(State::S0, ਚੌੜਾਈ);
		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ: ਬਿੱਟ)
			ਨਕਾਬ.ਸੈੱਟ(ਦੋਆਧਾਰੀ_ਅੰਕ, State::S1);

		ਜੇ (has_clk || has_gclk)
			sig_d = module->Xor(NEW_ID, sig_d, ਨਕਾਬ);
		ਜੇ (has_aload)
			sig_ad = module->Xor(NEW_ID, sig_ad, ਨਕਾਬ);
		module->ਜੋੜੋ_Xor(NEW_ID, new_q, ਨਕਾਬ, sig_q);
	}

	sig_q = new_q;
}
