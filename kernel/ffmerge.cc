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

#अन्तर्भाव "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

तार्किक FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	साठी (स्वतः bit : (*sigmap)(sig))
		जर (sigbit_users_मोजणी[bit] != 0)
			परत असत्य;
	परत सत्य;
}

तार्किक FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, पूर्णांक>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	तार्किक found = असत्य;

	साठी (स्वतः bit : sig)
	{
		जर (bit.wire == रिक्त || sigbit_users_मोजणी[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			सुरूठेवा;
		}

		जर (sigbit_users_मोजणी[bit] != 1)
			परत असत्य;

		स्वतः &sinks = dff_sink[bit];
		जर (sinks.size() != 1)
			परत असत्य;

		Cell *cell;
		पूर्णांक idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.घाला(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		जर (!cur_ff.has_clk)
			परत असत्य;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		जर (!found) {
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
			जर (ff.has_clk != cur_ff.has_clk)
				परत असत्य;
			जर (ff.has_ce != cur_ff.has_ce)
				परत असत्य;
			जर (ff.has_aload != cur_ff.has_aload)
				परत असत्य;
			जर (ff.has_srst != cur_ff.has_srst)
				परत असत्य;
			जर (ff.has_arst != cur_ff.has_arst)
				परत असत्य;
			जर (ff.has_sr != cur_ff.has_sr)
				परत असत्य;
			जर (ff.has_clk) {
				जर (ff.sig_clk != cur_ff.sig_clk)
					परत असत्य;
				जर (ff.pol_clk != cur_ff.pol_clk)
					परत असत्य;
			}
			जर (ff.has_ce) {
				जर (ff.sig_ce != cur_ff.sig_ce)
					परत असत्य;
				जर (ff.pol_ce != cur_ff.pol_ce)
					परत असत्य;
			}
			जर (ff.has_aload) {
				जर (ff.sig_aload != cur_ff.sig_aload)
					परत असत्य;
				जर (ff.pol_aload != cur_ff.pol_aload)
					परत असत्य;
			}
			जर (ff.has_srst) {
				जर (ff.sig_srst != cur_ff.sig_srst)
					परत असत्य;
				जर (ff.pol_srst != cur_ff.pol_srst)
					परत असत्य;
				जर (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					परत असत्य;
			}
			जर (ff.has_arst) {
				जर (ff.sig_arst != cur_ff.sig_arst)
					परत असत्य;
				जर (ff.pol_arst != cur_ff.pol_arst)
					परत असत्य;
			}
			जर (ff.has_sr) {
				जर (ff.pol_clr != cur_ff.pol_clr)
					परत असत्य;
				जर (ff.pol_set != cur_ff.pol_set)
					परत असत्य;
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
		found = सत्य;
	}

	परत found;
}

तार्किक FfMergeHelper::find_आदान_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, पूर्णांक>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	तार्किक found = असत्य;

	pool<पूर्णांक> const_bits;

	साठी (स्वतः bit : sig)
	{
		जर (bit.wire == रिक्त) {
			const_bits.घाला(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.data));
			ff.val_srst.append(RTLIL::Const(bit.data));
			ff.val_arst.append(RTLIL::Const(bit.data));
			सुरूठेवा;
		}

		जर (!dff_driver.मोजणी(bit))
			परत असत्य;

		Cell *cell;
		पूर्णांक idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.घाला(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		जर (!found) {
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
			जर (ff.has_gclk != cur_ff.has_gclk)
				परत असत्य;
			जर (ff.has_clk != cur_ff.has_clk)
				परत असत्य;
			जर (ff.has_ce != cur_ff.has_ce)
				परत असत्य;
			जर (ff.has_aload != cur_ff.has_aload)
				परत असत्य;
			जर (ff.has_srst != cur_ff.has_srst)
				परत असत्य;
			जर (ff.has_arst != cur_ff.has_arst)
				परत असत्य;
			जर (ff.has_sr != cur_ff.has_sr)
				परत असत्य;
			जर (ff.has_clk) {
				जर (ff.sig_clk != cur_ff.sig_clk)
					परत असत्य;
				जर (ff.pol_clk != cur_ff.pol_clk)
					परत असत्य;
			}
			जर (ff.has_ce) {
				जर (ff.sig_ce != cur_ff.sig_ce)
					परत असत्य;
				जर (ff.pol_ce != cur_ff.pol_ce)
					परत असत्य;
			}
			जर (ff.has_aload) {
				जर (ff.sig_aload != cur_ff.sig_aload)
					परत असत्य;
				जर (ff.pol_aload != cur_ff.pol_aload)
					परत असत्य;
			}
			जर (ff.has_srst) {
				जर (ff.sig_srst != cur_ff.sig_srst)
					परत असत्य;
				जर (ff.pol_srst != cur_ff.pol_srst)
					परत असत्य;
				जर (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					परत असत्य;
			}
			जर (ff.has_arst) {
				जर (ff.sig_arst != cur_ff.sig_arst)
					परत असत्य;
				जर (ff.pol_arst != cur_ff.pol_arst)
					परत असत्य;
			}
			जर (ff.has_sr) {
				जर (ff.pol_clr != cur_ff.pol_clr)
					परत असत्य;
				जर (ff.pol_set != cur_ff.pol_set)
					परत असत्य;
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
		found = सत्य;
	}

	जर (found && ff.has_sr) {
		साठी (स्वतः i: const_bits) {
			जर (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} अन्यथा जर (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	परत found;
}


शून्य FfMergeHelper::remove_output_ff(स्थिरांक pool<std::pair<Cell *, पूर्णांक>> &bits) {
	साठी (स्वतः &it : bits) {
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

शून्य FfMergeHelper::mark_आदान_ff(स्थिरांक pool<std::pair<Cell *, पूर्णांक>> &bits) {
	साठी (स्वतः &it : bits) {
		Cell *cell = it.first;
		पूर्णांक idx = it.second;
		जर (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_मोजणी[d[idx]]++;
		}
	}
}

शून्य FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	साठी (स्वतः wire : module->wires()) {
		जर (wire->port_output)
			साठी (स्वतः bit : (*sigmap)(wire))
				sigbit_users_मोजणी[bit]++;
	}

	साठी (स्वतः cell : module->cells()) {
		जर (cell->is_builtin_ff()) {
			जर (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				साठी (पूर्णांक i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].घाला(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			साठी (पूर्णांक i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		साठी (स्वतः &conn : cell->connections())
			जर (!cell->known() || cell->आदान(conn.first))
				साठी (स्वतः bit : (*sigmap)(conn.second))
					sigbit_users_मोजणी[bit]++;
	}
}

शून्य FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_मोजणी.clear();
}
