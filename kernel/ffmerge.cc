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

ತಾರ್ಕಿಕ FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	ಪ್ರತಿ (auto bit : (*sigmap)(sig))
		ಯದಿ (sigbit_users_ಎಣಿಕೆ[bit] != 0)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

ತಾರ್ಕಿಕ FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, ಪೂರ್ಣಾಙ್ಕ>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	ತಾರ್ಕಿಕ found = ಅಸತ್ಯ;

	ಪ್ರತಿ (auto bit : sig)
	{
		ಯದಿ (bit.wire == ರಿಕ್ತ || sigbit_users_ಎಣಿಕೆ[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			ಅನುವರ್ತ;
		}

		ಯದಿ (sigbit_users_ಎಣಿಕೆ[bit] != 1)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

		auto &sinks = dff_sink[bit];
		ಯದಿ (sinks.size() != 1)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

		Cell *cell;
		ಪೂರ್ಣಾಙ್ಕ idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.ಸೇರಿಸು(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		ಯದಿ (!cur_ff.has_clk)
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		ಯದಿ (!found) {
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
			ff.pol_set = cur_ff.pol_set;
		} ಅನ್ಯಥಾ {
			ಯದಿ (ff.has_clk != cur_ff.has_clk)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_ce != cur_ff.has_ce)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_aload != cur_ff.has_aload)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_srst != cur_ff.has_srst)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_arst != cur_ff.has_arst)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_sr != cur_ff.has_sr)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_clk) {
				ಯದಿ (ff.sig_clk != cur_ff.sig_clk)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_clk != cur_ff.pol_clk)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_ce) {
				ಯದಿ (ff.sig_ce != cur_ff.sig_ce)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_ce != cur_ff.pol_ce)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_aload) {
				ಯದಿ (ff.sig_aload != cur_ff.sig_aload)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_aload != cur_ff.pol_aload)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_srst) {
				ಯದಿ (ff.sig_srst != cur_ff.sig_srst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_srst != cur_ff.pol_srst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_arst) {
				ಯದಿ (ff.sig_arst != cur_ff.sig_arst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_arst != cur_ff.pol_arst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_sr) {
				ಯದಿ (ff.pol_clr != cur_ff.pol_clr)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_set != cur_ff.pol_set)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
		}

		ff.width++;
		ff.sig_d.append(cur_ff.sig_d[idx]);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_set.append(ff.has_sr ? cur_ff.sig_set[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		found = ಸತ್ಯ;
	}

	ಪ್ರತ್ಯಾಗಮ found;
}

ತಾರ್ಕಿಕ FfMergeHelper::find_ಒಳಹರಿವು_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, ಪೂರ್ಣಾಙ್ಕ>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	ತಾರ್ಕಿಕ found = ಅಸತ್ಯ;

	pool<ಪೂರ್ಣಾಙ್ಕ> const_bits;

	ಪ್ರತಿ (auto bit : sig)
	{
		ಯದಿ (bit.wire == ರಿಕ್ತ) {
			const_bits.ಸೇರಿಸು(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.data));
			ff.val_srst.append(RTLIL::Const(bit.data));
			ff.val_arst.append(RTLIL::Const(bit.data));
			ಅನುವರ್ತ;
		}

		ಯದಿ (!dff_driver.ಎಣಿಕೆ(bit))
			ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;

		Cell *cell;
		ಪೂರ್ಣಾಙ್ಕ idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.ಸೇರಿಸು(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		ಯದಿ (!found) {
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
			ff.pol_set = cur_ff.pol_set;
		} ಅನ್ಯಥಾ {
			ಯದಿ (ff.has_gclk != cur_ff.has_gclk)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_clk != cur_ff.has_clk)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_ce != cur_ff.has_ce)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_aload != cur_ff.has_aload)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_srst != cur_ff.has_srst)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_arst != cur_ff.has_arst)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_sr != cur_ff.has_sr)
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			ಯದಿ (ff.has_clk) {
				ಯದಿ (ff.sig_clk != cur_ff.sig_clk)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_clk != cur_ff.pol_clk)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_ce) {
				ಯದಿ (ff.sig_ce != cur_ff.sig_ce)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_ce != cur_ff.pol_ce)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_aload) {
				ಯದಿ (ff.sig_aload != cur_ff.sig_aload)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_aload != cur_ff.pol_aload)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_srst) {
				ಯದಿ (ff.sig_srst != cur_ff.sig_srst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_srst != cur_ff.pol_srst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_arst) {
				ಯದಿ (ff.sig_arst != cur_ff.sig_arst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_arst != cur_ff.pol_arst)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
			ಯದಿ (ff.has_sr) {
				ಯದಿ (ff.pol_clr != cur_ff.pol_clr)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
				ಯದಿ (ff.pol_set != cur_ff.pol_set)
					ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
		}

		ff.width++;
		ff.sig_d.append((ff.has_clk || ff.has_gclk) ? cur_ff.sig_d[idx] : State::Sx);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_set.append(ff.has_sr ? cur_ff.sig_set[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		found = ಸತ್ಯ;
	}

	ಯದಿ (found && ff.has_sr) {
		ಪ್ರತಿ (auto i: const_bits) {
			ಯದಿ (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} ಅನ್ಯಥಾ ಯದಿ (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	ಪ್ರತ್ಯಾಗಮ found;
}


ಶೂನ್ಯ FfMergeHelper::remove_output_ff(ಧ್ರುವ pool<std::pair<Cell *, ಪೂರ್ಣಾಙ್ಕ>> &bits) {
	ಪ್ರತಿ (auto &it : bits) {
		Cell *cell = it.first;
		ಪೂರ್ಣಾಙ್ಕ idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

ಶೂನ್ಯ FfMergeHelper::mark_ಒಳಹರಿವು_ff(ಧ್ರುವ pool<std::pair<Cell *, ಪೂರ್ಣಾಙ್ಕ>> &bits) {
	ಪ್ರತಿ (auto &it : bits) {
		Cell *cell = it.first;
		ಪೂರ್ಣಾಙ್ಕ idx = it.second;
		ಯದಿ (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_ಎಣಿಕೆ[d[idx]]++;
		}
	}
}

ಶೂನ್ಯ FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	ಪ್ರತಿ (auto wire : module->wires()) {
		ಯದಿ (wire->port_output)
			ಪ್ರತಿ (auto bit : (*sigmap)(wire))
				sigbit_users_ಎಣಿಕೆ[bit]++;
	}

	ಪ್ರತಿ (auto cell : module->cells()) {
		ಯದಿ (cell->is_builtin_ff()) {
			ಯದಿ (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].ಸೇರಿಸು(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		ಪ್ರತಿ (auto &conn : cell->connections())
			ಯದಿ (!cell->known() || cell->ಒಳಹರಿವು(conn.first))
				ಪ್ರತಿ (auto bit : (*sigmap)(conn.second))
					sigbit_users_ಎಣಿಕೆ[bit]++;
	}
}

ಶೂನ್ಯ FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_ಎಣಿಕೆ.clear();
}
