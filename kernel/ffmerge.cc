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

#અન્તર્ભાવ "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

તાર્કિક FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	માટે (auto bit : (*sigmap)(sig))
		જો (sigbit_users_ગણતરી[bit] != 0)
			પાછા ખોટું;
	પાછા સાચું;
}

તાર્કિક FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, પૂર્ણાંક>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	તાર્કિક found = ખોટું;

	માટે (auto bit : sig)
	{
		જો (bit.wire == ખાલી || sigbit_users_ગણતરી[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			ચાલુ;
		}

		જો (sigbit_users_ગણતરી[bit] != 1)
			પાછા ખોટું;

		auto &sinks = dff_sink[bit];
		જો (sinks.size() != 1)
			પાછા ખોટું;

		Cell *cell;
		પૂર્ણાંક idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.ઉમેરો(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		જો (!cur_ff.has_clk)
			પાછા ખોટું;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		જો (!found) {
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
		} નહીંતર {
			જો (ff.has_clk != cur_ff.has_clk)
				પાછા ખોટું;
			જો (ff.has_ce != cur_ff.has_ce)
				પાછા ખોટું;
			જો (ff.has_aload != cur_ff.has_aload)
				પાછા ખોટું;
			જો (ff.has_srst != cur_ff.has_srst)
				પાછા ખોટું;
			જો (ff.has_arst != cur_ff.has_arst)
				પાછા ખોટું;
			જો (ff.has_sr != cur_ff.has_sr)
				પાછા ખોટું;
			જો (ff.has_clk) {
				જો (ff.sig_clk != cur_ff.sig_clk)
					પાછા ખોટું;
				જો (ff.pol_clk != cur_ff.pol_clk)
					પાછા ખોટું;
			}
			જો (ff.has_ce) {
				જો (ff.sig_ce != cur_ff.sig_ce)
					પાછા ખોટું;
				જો (ff.pol_ce != cur_ff.pol_ce)
					પાછા ખોટું;
			}
			જો (ff.has_aload) {
				જો (ff.sig_aload != cur_ff.sig_aload)
					પાછા ખોટું;
				જો (ff.pol_aload != cur_ff.pol_aload)
					પાછા ખોટું;
			}
			જો (ff.has_srst) {
				જો (ff.sig_srst != cur_ff.sig_srst)
					પાછા ખોટું;
				જો (ff.pol_srst != cur_ff.pol_srst)
					પાછા ખોટું;
				જો (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					પાછા ખોટું;
			}
			જો (ff.has_arst) {
				જો (ff.sig_arst != cur_ff.sig_arst)
					પાછા ખોટું;
				જો (ff.pol_arst != cur_ff.pol_arst)
					પાછા ખોટું;
			}
			જો (ff.has_sr) {
				જો (ff.pol_clr != cur_ff.pol_clr)
					પાછા ખોટું;
				જો (ff.pol_set != cur_ff.pol_set)
					પાછા ખોટું;
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
		found = સાચું;
	}

	પાછા found;
}

તાર્કિક FfMergeHelper::find_આગત_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, પૂર્ણાંક>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	તાર્કિક found = ખોટું;

	pool<પૂર્ણાંક> const_bits;

	માટે (auto bit : sig)
	{
		જો (bit.wire == ખાલી) {
			const_bits.ઉમેરો(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.data));
			ff.val_srst.append(RTLIL::Const(bit.data));
			ff.val_arst.append(RTLIL::Const(bit.data));
			ચાલુ;
		}

		જો (!dff_driver.ગણતરી(bit))
			પાછા ખોટું;

		Cell *cell;
		પૂર્ણાંક idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.ઉમેરો(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		જો (!found) {
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
		} નહીંતર {
			જો (ff.has_gclk != cur_ff.has_gclk)
				પાછા ખોટું;
			જો (ff.has_clk != cur_ff.has_clk)
				પાછા ખોટું;
			જો (ff.has_ce != cur_ff.has_ce)
				પાછા ખોટું;
			જો (ff.has_aload != cur_ff.has_aload)
				પાછા ખોટું;
			જો (ff.has_srst != cur_ff.has_srst)
				પાછા ખોટું;
			જો (ff.has_arst != cur_ff.has_arst)
				પાછા ખોટું;
			જો (ff.has_sr != cur_ff.has_sr)
				પાછા ખોટું;
			જો (ff.has_clk) {
				જો (ff.sig_clk != cur_ff.sig_clk)
					પાછા ખોટું;
				જો (ff.pol_clk != cur_ff.pol_clk)
					પાછા ખોટું;
			}
			જો (ff.has_ce) {
				જો (ff.sig_ce != cur_ff.sig_ce)
					પાછા ખોટું;
				જો (ff.pol_ce != cur_ff.pol_ce)
					પાછા ખોટું;
			}
			જો (ff.has_aload) {
				જો (ff.sig_aload != cur_ff.sig_aload)
					પાછા ખોટું;
				જો (ff.pol_aload != cur_ff.pol_aload)
					પાછા ખોટું;
			}
			જો (ff.has_srst) {
				જો (ff.sig_srst != cur_ff.sig_srst)
					પાછા ખોટું;
				જો (ff.pol_srst != cur_ff.pol_srst)
					પાછા ખોટું;
				જો (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					પાછા ખોટું;
			}
			જો (ff.has_arst) {
				જો (ff.sig_arst != cur_ff.sig_arst)
					પાછા ખોટું;
				જો (ff.pol_arst != cur_ff.pol_arst)
					પાછા ખોટું;
			}
			જો (ff.has_sr) {
				જો (ff.pol_clr != cur_ff.pol_clr)
					પાછા ખોટું;
				જો (ff.pol_set != cur_ff.pol_set)
					પાછા ખોટું;
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
		found = સાચું;
	}

	જો (found && ff.has_sr) {
		માટે (auto i: const_bits) {
			જો (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} નહીંતર જો (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	પાછા found;
}


શૂન્ય FfMergeHelper::remove_output_ff(અચલ pool<std::pair<Cell *, પૂર્ણાંક>> &bits) {
	માટે (auto &it : bits) {
		Cell *cell = it.first;
		પૂર્ણાંક idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

શૂન્ય FfMergeHelper::mark_આગત_ff(અચલ pool<std::pair<Cell *, પૂર્ણાંક>> &bits) {
	માટે (auto &it : bits) {
		Cell *cell = it.first;
		પૂર્ણાંક idx = it.second;
		જો (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_ગણતરી[d[idx]]++;
		}
	}
}

શૂન્ય FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	માટે (auto wire : module->wires()) {
		જો (wire->port_output)
			માટે (auto bit : (*sigmap)(wire))
				sigbit_users_ગણતરી[bit]++;
	}

	માટે (auto cell : module->cells()) {
		જો (cell->is_builtin_ff()) {
			જો (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				માટે (પૂર્ણાંક i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].ઉમેરો(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			માટે (પૂર્ણાંક i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		માટે (auto &conn : cell->connections())
			જો (!cell->known() || cell->આગત(conn.first))
				માટે (auto bit : (*sigmap)(conn.second))
					sigbit_users_ગણતરી[bit]++;
	}
}

શૂન્ય FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_ગણતરી.clear();
}
