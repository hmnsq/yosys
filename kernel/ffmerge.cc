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

#समावेश "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

bool FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	क्रम (स्वतः bit : (*sigmap)(sig))
		अगर (sigbit_users_गिनती[bit] != 0)
			वापस false;
	वापस true;
}

bool FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, Ff_तथ्य &ff, pool<std::pair<Cell *, पूर्णांक>> &bits) {
	ff = Ff_तथ्य(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	क्रम (स्वतः bit : sig)
	{
		अगर (bit.wire == शून्य || sigbit_users_गिनती[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			जारी;
		}

		अगर (sigbit_users_गिनती[bit] != 1)
			वापस false;

		स्वतः &sinks = dff_sink[bit];
		अगर (sinks.आकार() != 1)
			वापस false;

		Cell *cell;
		पूर्णांक idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.जोड़ो(std::make_pair(cell, idx));

		Ff_तथ्य cur_ff(initvals, cell);

		// Reject latches and $ff.
		अगर (!cur_ff.has_clk)
			वापस false;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		अगर (!found) {
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
		} अन्यथा {
			अगर (ff.has_clk != cur_ff.has_clk)
				वापस false;
			अगर (ff.has_ce != cur_ff.has_ce)
				वापस false;
			अगर (ff.has_aload != cur_ff.has_aload)
				वापस false;
			अगर (ff.has_srst != cur_ff.has_srst)
				वापस false;
			अगर (ff.has_arst != cur_ff.has_arst)
				वापस false;
			अगर (ff.has_sr != cur_ff.has_sr)
				वापस false;
			अगर (ff.has_clk) {
				अगर (ff.sig_clk != cur_ff.sig_clk)
					वापस false;
				अगर (ff.pol_clk != cur_ff.pol_clk)
					वापस false;
			}
			अगर (ff.has_ce) {
				अगर (ff.sig_ce != cur_ff.sig_ce)
					वापस false;
				अगर (ff.pol_ce != cur_ff.pol_ce)
					वापस false;
			}
			अगर (ff.has_aload) {
				अगर (ff.sig_aload != cur_ff.sig_aload)
					वापस false;
				अगर (ff.pol_aload != cur_ff.pol_aload)
					वापस false;
			}
			अगर (ff.has_srst) {
				अगर (ff.sig_srst != cur_ff.sig_srst)
					वापस false;
				अगर (ff.pol_srst != cur_ff.pol_srst)
					वापस false;
				अगर (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					वापस false;
			}
			अगर (ff.has_arst) {
				अगर (ff.sig_arst != cur_ff.sig_arst)
					वापस false;
				अगर (ff.pol_arst != cur_ff.pol_arst)
					वापस false;
			}
			अगर (ff.has_sr) {
				अगर (ff.pol_clr != cur_ff.pol_clr)
					वापस false;
				अगर (ff.pol_set != cur_ff.pol_set)
					वापस false;
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
		found = true;
	}

	वापस found;
}

bool FfMergeHelper::find_निवेश_ff(RTLIL::SigSpec sig, Ff_तथ्य &ff, pool<std::pair<Cell *, पूर्णांक>> &bits) {
	ff = Ff_तथ्य(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	pool<पूर्णांक> const_bits;

	क्रम (स्वतः bit : sig)
	{
		अगर (bit.wire == शून्य) {
			const_bits.जोड़ो(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.तथ्य));
			ff.val_srst.append(RTLIL::Const(bit.तथ्य));
			ff.val_arst.append(RTLIL::Const(bit.तथ्य));
			जारी;
		}

		अगर (!dff_driver.गिनती(bit))
			वापस false;

		Cell *cell;
		पूर्णांक idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.जोड़ो(std::make_pair(cell, idx));

		Ff_तथ्य cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		अगर (!found) {
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
		} अन्यथा {
			अगर (ff.has_gclk != cur_ff.has_gclk)
				वापस false;
			अगर (ff.has_clk != cur_ff.has_clk)
				वापस false;
			अगर (ff.has_ce != cur_ff.has_ce)
				वापस false;
			अगर (ff.has_aload != cur_ff.has_aload)
				वापस false;
			अगर (ff.has_srst != cur_ff.has_srst)
				वापस false;
			अगर (ff.has_arst != cur_ff.has_arst)
				वापस false;
			अगर (ff.has_sr != cur_ff.has_sr)
				वापस false;
			अगर (ff.has_clk) {
				अगर (ff.sig_clk != cur_ff.sig_clk)
					वापस false;
				अगर (ff.pol_clk != cur_ff.pol_clk)
					वापस false;
			}
			अगर (ff.has_ce) {
				अगर (ff.sig_ce != cur_ff.sig_ce)
					वापस false;
				अगर (ff.pol_ce != cur_ff.pol_ce)
					वापस false;
			}
			अगर (ff.has_aload) {
				अगर (ff.sig_aload != cur_ff.sig_aload)
					वापस false;
				अगर (ff.pol_aload != cur_ff.pol_aload)
					वापस false;
			}
			अगर (ff.has_srst) {
				अगर (ff.sig_srst != cur_ff.sig_srst)
					वापस false;
				अगर (ff.pol_srst != cur_ff.pol_srst)
					वापस false;
				अगर (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					वापस false;
			}
			अगर (ff.has_arst) {
				अगर (ff.sig_arst != cur_ff.sig_arst)
					वापस false;
				अगर (ff.pol_arst != cur_ff.pol_arst)
					वापस false;
			}
			अगर (ff.has_sr) {
				अगर (ff.pol_clr != cur_ff.pol_clr)
					वापस false;
				अगर (ff.pol_set != cur_ff.pol_set)
					वापस false;
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
		found = true;
	}

	अगर (found && ff.has_sr) {
		क्रम (स्वतः i: const_bits) {
			अगर (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} अन्यथा अगर (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	वापस found;
}


व्योम FfMergeHelper::remove_output_ff(स्थिर pool<std::pair<Cell *, पूर्णांक>> &bits) {
	क्रम (स्वतः &it : bits) {
		Cell *cell = it.first;
		पूर्णांक idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

व्योम FfMergeHelper::mark_निवेश_ff(स्थिर pool<std::pair<Cell *, पूर्णांक>> &bits) {
	क्रम (स्वतः &it : bits) {
		Cell *cell = it.first;
		पूर्णांक idx = it.second;
		अगर (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_गिनती[d[idx]]++;
		}
	}
}

व्योम FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	सफा();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	क्रम (स्वतः wire : module->wires()) {
		अगर (wire->port_output)
			क्रम (स्वतः bit : (*sigmap)(wire))
				sigbit_users_गिनती[bit]++;
	}

	क्रम (स्वतः cell : module->cells()) {
		अगर (cell->is_builtin_ff()) {
			अगर (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				क्रम (पूर्णांक i = 0; i < Get_आकार(d); i++)
					dff_sink[d[i]].जोड़ो(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			क्रम (पूर्णांक i = 0; i < Get_आकार(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		क्रम (स्वतः &conn : cell->connections())
			अगर (!cell->known() || cell->निवेश(conn.first))
				क्रम (स्वतः bit : (*sigmap)(conn.second))
					sigbit_users_गिनती[bit]++;
	}
}

व्योम FfMergeHelper::सफा() {
	dff_driver.सफा();
	dff_sink.सफा();
	sigbit_users_गिनती.सफा();
}
