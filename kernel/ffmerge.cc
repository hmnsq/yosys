/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2021  Marcelina Kościelnicka <mwk@0x04.net>
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

#ಅನ್ತರ್ಭಾವ "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

ತಾರ್ಕಿಕ Ff_ವಿಲೀನ_Helper::is_output_ಬಳಸದ(RTLIL::SigSpec sig) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : (*sigmap)(sig))
		ಆದರೆ (sigbit_users_ಎಣಿಕೆ[ಬಿಟ್] != 0)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

ತಾರ್ಕಿಕ Ff_ವಿಲೀನ_Helper::ಕಂಡುಹಿಡಿ_output_ff(RTLIL::SigSpec sig, Ff_ದತ್ತಾಂಶ &ff, pool<std::pair<Cell *, ಪೂರ್ಣಾಂಕ>> &ಬಿಟ್ಟುಗಳು) {
	ff = Ff_ದತ್ತಾಂಶ(module, initvals, module->twines().ಕೂಡಿಸು(NEW_ID));
	sigmap->apply(sig);

	ತಾರ್ಕಿಕ ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : sig)
	{
		ಆದರೆ (ಬಿಟ್.wire == ಖಾಲಿ || sigbit_users_ಎಣಿಕೆ[ಬಿಟ್] == 0) {
			ff.ಅಗಲ++;
			ff.sig_q.append(ಬಿಟ್);
			ff.sig_d.append(ಬಿಟ್);
			ff.sig_clr.append(State::Sx);
			ff.sig_ಗಣ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (sigbit_users_ಎಣಿಕೆ[ಬಿಟ್] != 1)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		ಸ್ವಯಂಚಾಲಿತ &sinks = dff_sink[ಬಿಟ್];
		ಆದರೆ (sinks.ಗಾತ್ರ_2() != 1)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		Cell *cell;
		ಪೂರ್ಣಾಂಕ idx;
		std::tie(cell, idx) = *sinks.begin();
		ಬಿಟ್ಟುಗಳು.ಸೇರಿಸು(std::make_pair(cell, idx));

		Ff_ದತ್ತಾಂಶ cur_ff(initvals, cell);

		// Reject latches and $ff.
		ಆದರೆ (!cur_ff.has_clk)
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == ಬಿಟ್);

		ಆದರೆ (!ಸಿಕ್ಕಿತು) {
			ff.sig_clk = cur_ff.sig_clk;
			ff.sig_ce = cur_ff.sig_ce;
			ff.sig_aload = cur_ff.sig_aload;
			ff.sig_srst = cur_ff.sig_srst;
			ff.sig_arst = cur_ff.sig_arst;
			ff.has_clk = cur_ff.has_clk;
			ff.has_ce = cur_ff.has_ce;
			ff.has_aload = cur_ff.has_aload;
			ff.has_srst = cur_ff.has_srst;
			ff.has_arst = cur_ff.has_arst;
			ff.has_sr = cur_ff.has_sr;
			ff.ce_over_srst = cur_ff.ce_over_srst;
			ff.pol_clk = cur_ff.pol_clk;
			ff.pol_ce = cur_ff.pol_ce;
			ff.pol_aload = cur_ff.pol_aload;
			ff.pol_arst = cur_ff.pol_arst;
			ff.pol_srst = cur_ff.pol_srst;
			ff.pol_clr = cur_ff.pol_clr;
			ff.pol_ಗಣ = cur_ff.pol_ಗಣ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (ff.has_clk != cur_ff.has_clk)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_ce != cur_ff.has_ce)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_aload != cur_ff.has_aload)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_srst != cur_ff.has_srst)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_arst != cur_ff.has_arst)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_sr != cur_ff.has_sr)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_clk) {
				ಆದರೆ (ff.sig_clk != cur_ff.sig_clk)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_clk != cur_ff.pol_clk)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_ce) {
				ಆದರೆ (ff.sig_ce != cur_ff.sig_ce)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_ce != cur_ff.pol_ce)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_aload) {
				ಆದರೆ (ff.sig_aload != cur_ff.sig_aload)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_aload != cur_ff.pol_aload)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_srst) {
				ಆದರೆ (ff.sig_srst != cur_ff.sig_srst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_srst != cur_ff.pol_srst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_arst) {
				ಆದರೆ (ff.sig_arst != cur_ff.sig_arst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_arst != cur_ff.pol_arst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_sr) {
				ಆದರೆ (ff.pol_clr != cur_ff.pol_clr)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_ಗಣ != cur_ff.pol_ಗಣ)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
		}

		ff.ಅಗಲ++;
		ff.sig_d.append(cur_ff.sig_d[idx]);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_ಗಣ.append(ff.has_sr ? cur_ff.sig_ಗಣ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		ಸಿಕ್ಕಿತು = ನಿಜ;
	}

	ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}

ತಾರ್ಕಿಕ Ff_ವಿಲೀನ_Helper::ಕಂಡುಹಿಡಿ_ಒಳಹರಿವು_ff(RTLIL::SigSpec sig, Ff_ದತ್ತಾಂಶ &ff, pool<std::pair<Cell *, ಪೂರ್ಣಾಂಕ>> &ಬಿಟ್ಟುಗಳು) {
	ff = Ff_ದತ್ತಾಂಶ(module, initvals, module->twines().ಕೂಡಿಸು(NEW_ID));
	sigmap->apply(sig);

	ತಾರ್ಕಿಕ ಸಿಕ್ಕಿತು = ಸುಳ್ಳು;

	pool<ಪೂರ್ಣಾಂಕ> const_ಬಿಟ್ಟುಗಳು;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : sig)
	{
		ಆದರೆ (ಬಿಟ್.wire == ಖಾಲಿ) {
			const_ಬಿಟ್ಟುಗಳು.ಸೇರಿಸು(ff.ಅಗಲ);
			ff.ಅಗಲ++;
			ff.sig_q.append(ಬಿಟ್);
			ff.sig_d.append(ಬಿಟ್);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_ಗಣ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(ಬಿಟ್.ದತ್ತಾಂಶ));
			ff.val_srst.append(RTLIL::Const(ಬಿಟ್.ದತ್ತಾಂಶ));
			ff.val_arst.append(RTLIL::Const(ಬಿಟ್.ದತ್ತಾಂಶ));
			ಮುಂದುವರಿ;
		}

		ಆದರೆ (!dff_driver.ಎಣಿಕೆ_2(ಬಿಟ್))
			ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;

		Cell *cell;
		ಪೂರ್ಣಾಂಕ idx;
		std::tie(cell, idx) = dff_driver[ಬಿಟ್];
		ಬಿಟ್ಟುಗಳು.ಸೇರಿಸು(std::make_pair(cell, idx));

		Ff_ದತ್ತಾಂಶ cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == ಬಿಟ್);

		ಆದರೆ (!ಸಿಕ್ಕಿತು) {
			ff.sig_clk = cur_ff.sig_clk;
			ff.sig_ce = cur_ff.sig_ce;
			ff.sig_aload = cur_ff.sig_aload;
			ff.sig_srst = cur_ff.sig_srst;
			ff.sig_arst = cur_ff.sig_arst;
			ff.has_clk = cur_ff.has_clk;
			ff.has_gclk = cur_ff.has_gclk;
			ff.has_ce = cur_ff.has_ce;
			ff.has_aload = cur_ff.has_aload;
			ff.has_srst = cur_ff.has_srst;
			ff.has_arst = cur_ff.has_arst;
			ff.has_sr = cur_ff.has_sr;
			ff.ce_over_srst = cur_ff.ce_over_srst;
			ff.pol_clk = cur_ff.pol_clk;
			ff.pol_ce = cur_ff.pol_ce;
			ff.pol_aload = cur_ff.pol_aload;
			ff.pol_arst = cur_ff.pol_arst;
			ff.pol_srst = cur_ff.pol_srst;
			ff.pol_clr = cur_ff.pol_clr;
			ff.pol_ಗಣ = cur_ff.pol_ಗಣ;
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (ff.has_gclk != cur_ff.has_gclk)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_clk != cur_ff.has_clk)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_ce != cur_ff.has_ce)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_aload != cur_ff.has_aload)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_srst != cur_ff.has_srst)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_arst != cur_ff.has_arst)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_sr != cur_ff.has_sr)
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			ಆದರೆ (ff.has_clk) {
				ಆದರೆ (ff.sig_clk != cur_ff.sig_clk)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_clk != cur_ff.pol_clk)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_ce) {
				ಆದರೆ (ff.sig_ce != cur_ff.sig_ce)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_ce != cur_ff.pol_ce)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_aload) {
				ಆದರೆ (ff.sig_aload != cur_ff.sig_aload)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_aload != cur_ff.pol_aload)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_srst) {
				ಆದರೆ (ff.sig_srst != cur_ff.sig_srst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_srst != cur_ff.pol_srst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_arst) {
				ಆದರೆ (ff.sig_arst != cur_ff.sig_arst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_arst != cur_ff.pol_arst)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
			ಆದರೆ (ff.has_sr) {
				ಆದರೆ (ff.pol_clr != cur_ff.pol_clr)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
				ಆದರೆ (ff.pol_ಗಣ != cur_ff.pol_ಗಣ)
					ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
		}

		ff.ಅಗಲ++;
		ff.sig_d.append((ff.has_clk || ff.has_gclk) ? cur_ff.sig_d[idx] : State::Sx);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_ಗಣ.append(ff.has_sr ? cur_ff.sig_ಗಣ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		ಸಿಕ್ಕಿತು = ನಿಜ;
	}

	ಆದರೆ (ಸಿಕ್ಕಿತು && ff.has_sr) {
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ i: const_ಬಿಟ್ಟುಗಳು) {
			ಆದರೆ (ff.sig_d[i] == State::S0) {
				ff.sig_ಗಣ[i] = ff.pol_ಗಣ ? State::S0 : State::S1;
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	ಹಿಂತಿರುಗಿ ಸಿಕ್ಕಿತು;
}


ಶೂನ್ಯ Ff_ವಿಲೀನ_Helper::remove_output_ff(ಸ್ಥಿರಾಂಕ pool<std::pair<Cell *, ಪೂರ್ಣಾಂಕ>> &ಬಿಟ್ಟುಗಳು) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : ಬಿಟ್ಟುಗಳು) {
		Cell *cell = it.ಮೊದಲ;
		ಪೂರ್ಣಾಂಕ idx = it.ಎರಡನೇ;
		SigSpec q = cell->get_ಪೋರ್ಟ್(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->ಕೂಡಿಸು_Wire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->ಗಣ_ಪೋರ್ಟ್(ID::Q, q);
		initvals->ಗಣ_init(cell->get_ಪೋರ್ಟ್(ID::Q), (*initvals)(q));
	}
}

ಶೂನ್ಯ Ff_ವಿಲೀನ_Helper::mark_ಒಳಹರಿವು_ff(ಸ್ಥಿರಾಂಕ pool<std::pair<Cell *, ಪೂರ್ಣಾಂಕ>> &ಬಿಟ್ಟುಗಳು) {
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : ಬಿಟ್ಟುಗಳು) {
		Cell *cell = it.ಮೊದಲ;
		ಪೂರ್ಣಾಂಕ idx = it.ಎರಡನೇ;
		ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::D)) {
			SigSpec d = cell->get_ಪೋರ್ಟ್(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_ಎಣಿಕೆ[d[idx]]++;
		}
	}
}

ಶೂನ್ಯ Ff_ವಿಲೀನ_Helper::ಗಣ(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ wire : module->wires()) {
		ಆದರೆ (wire->ಪೋರ್ಟ್_output)
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : (*sigmap)(wire))
				sigbit_users_ಎಣಿಕೆ[ಬಿಟ್]++;
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : module->cells()) {
		ಆದರೆ (cell->is_builtin_ff()) {
			ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::D)) {
				SigSpec d = (*sigmap)(cell->get_ಪೋರ್ಟ್(ID::D));
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(d); i++)
					dff_sink[d[i]].ಸೇರಿಸು(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->get_ಪೋರ್ಟ್(ID::Q));
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
			ಆದರೆ (!cell->ತಿಳಿದ() || cell->ಒಳಹರಿವು(conn.ಮೊದಲ))
				ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : (*sigmap)(conn.ಎರಡನೇ))
					sigbit_users_ಎಣಿಕೆ[ಬಿಟ್]++;
	}
}

ಶೂನ್ಯ Ff_ವಿಲೀನ_Helper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_ಎಣಿಕೆ.clear();
}
