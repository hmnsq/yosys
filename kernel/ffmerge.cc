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

#അന്തര്ഭാവ "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

തർക്കം FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	വേണ്ടി (auto bit : (*sigmap)(sig))
		എങ്കിൽ (sigbit_users_എണ്ണം[bit] != 0)
			തിരികെ അസത്യം;
	തിരികെ സത്യം;
}

തർക്കം FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, പൂർണ്ണസംഖ്യ>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	തർക്കം found = അസത്യം;

	വേണ്ടി (auto bit : sig)
	{
		എങ്കിൽ (bit.wire == ശൂന്യ || sigbit_users_എണ്ണം[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			തുടരുക;
		}

		എങ്കിൽ (sigbit_users_എണ്ണം[bit] != 1)
			തിരികെ അസത്യം;

		auto &sinks = dff_sink[bit];
		എങ്കിൽ (sinks.size() != 1)
			തിരികെ അസത്യം;

		Cell *cell;
		പൂർണ്ണസംഖ്യ idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.തിരുകുക(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		എങ്കിൽ (!cur_ff.has_clk)
			തിരികെ അസത്യം;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		എങ്കിൽ (!found) {
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
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (ff.has_clk != cur_ff.has_clk)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_ce != cur_ff.has_ce)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_aload != cur_ff.has_aload)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_srst != cur_ff.has_srst)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_arst != cur_ff.has_arst)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_sr != cur_ff.has_sr)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_clk) {
				എങ്കിൽ (ff.sig_clk != cur_ff.sig_clk)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_clk != cur_ff.pol_clk)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_ce) {
				എങ്കിൽ (ff.sig_ce != cur_ff.sig_ce)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_ce != cur_ff.pol_ce)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_aload) {
				എങ്കിൽ (ff.sig_aload != cur_ff.sig_aload)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_aload != cur_ff.pol_aload)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_srst) {
				എങ്കിൽ (ff.sig_srst != cur_ff.sig_srst)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_srst != cur_ff.pol_srst)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_arst) {
				എങ്കിൽ (ff.sig_arst != cur_ff.sig_arst)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_arst != cur_ff.pol_arst)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_sr) {
				എങ്കിൽ (ff.pol_clr != cur_ff.pol_clr)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_set != cur_ff.pol_set)
					തിരികെ അസത്യം;
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
		found = സത്യം;
	}

	തിരികെ found;
}

തർക്കം FfMergeHelper::find_നിവേശം_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, പൂർണ്ണസംഖ്യ>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	തർക്കം found = അസത്യം;

	pool<പൂർണ്ണസംഖ്യ> const_bits;

	വേണ്ടി (auto bit : sig)
	{
		എങ്കിൽ (bit.wire == ശൂന്യ) {
			const_bits.തിരുകുക(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.data));
			ff.val_srst.append(RTLIL::Const(bit.data));
			ff.val_arst.append(RTLIL::Const(bit.data));
			തുടരുക;
		}

		എങ്കിൽ (!dff_driver.എണ്ണം(bit))
			തിരികെ അസത്യം;

		Cell *cell;
		പൂർണ്ണസംഖ്യ idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.തിരുകുക(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		എങ്കിൽ (!found) {
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
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (ff.has_gclk != cur_ff.has_gclk)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_clk != cur_ff.has_clk)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_ce != cur_ff.has_ce)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_aload != cur_ff.has_aload)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_srst != cur_ff.has_srst)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_arst != cur_ff.has_arst)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_sr != cur_ff.has_sr)
				തിരികെ അസത്യം;
			എങ്കിൽ (ff.has_clk) {
				എങ്കിൽ (ff.sig_clk != cur_ff.sig_clk)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_clk != cur_ff.pol_clk)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_ce) {
				എങ്കിൽ (ff.sig_ce != cur_ff.sig_ce)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_ce != cur_ff.pol_ce)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_aload) {
				എങ്കിൽ (ff.sig_aload != cur_ff.sig_aload)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_aload != cur_ff.pol_aload)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_srst) {
				എങ്കിൽ (ff.sig_srst != cur_ff.sig_srst)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_srst != cur_ff.pol_srst)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_arst) {
				എങ്കിൽ (ff.sig_arst != cur_ff.sig_arst)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_arst != cur_ff.pol_arst)
					തിരികെ അസത്യം;
			}
			എങ്കിൽ (ff.has_sr) {
				എങ്കിൽ (ff.pol_clr != cur_ff.pol_clr)
					തിരികെ അസത്യം;
				എങ്കിൽ (ff.pol_set != cur_ff.pol_set)
					തിരികെ അസത്യം;
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
		found = സത്യം;
	}

	എങ്കിൽ (found && ff.has_sr) {
		വേണ്ടി (auto i: const_bits) {
			എങ്കിൽ (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} അല്ലെങ്കിൽ എങ്കിൽ (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	തിരികെ found;
}


ശൂന്യം FfMergeHelper::remove_output_ff(സ്ഥിരം pool<std::pair<Cell *, പൂർണ്ണസംഖ്യ>> &bits) {
	വേണ്ടി (auto &it : bits) {
		Cell *cell = it.first;
		പൂർണ്ണസംഖ്യ idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

ശൂന്യം FfMergeHelper::mark_നിവേശം_ff(സ്ഥിരം pool<std::pair<Cell *, പൂർണ്ണസംഖ്യ>> &bits) {
	വേണ്ടി (auto &it : bits) {
		Cell *cell = it.first;
		പൂർണ്ണസംഖ്യ idx = it.second;
		എങ്കിൽ (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_എണ്ണം[d[idx]]++;
		}
	}
}

ശൂന്യം FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	വേണ്ടി (auto wire : module->wires()) {
		എങ്കിൽ (wire->port_output)
			വേണ്ടി (auto bit : (*sigmap)(wire))
				sigbit_users_എണ്ണം[bit]++;
	}

	വേണ്ടി (auto cell : module->cells()) {
		എങ്കിൽ (cell->is_builtin_ff()) {
			എങ്കിൽ (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].തിരുകുക(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		വേണ്ടി (auto &conn : cell->connections())
			എങ്കിൽ (!cell->known() || cell->നിവേശം(conn.first))
				വേണ്ടി (auto bit : (*sigmap)(conn.second))
					sigbit_users_എണ്ണം[bit]++;
	}
}

ശൂന്യം FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_എണ്ണം.clear();
}
