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

#ضمن "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

منطقي FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	لكل (auto bit : (*sigmap)(sig))
		ܐܢ (sigbit_users_count[bit] != 0)
			ܦܢܐ ܕܓܠܐ;
	ܦܢܐ ܫܪܪܐ;
}

منطقي FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, صحيح>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	منطقي found = ܕܓܠܐ;

	لكل (auto bit : sig)
	{
		ܐܢ (bit.wire == عدم || sigbit_users_count[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			استمر;
		}

		ܐܢ (sigbit_users_count[bit] != 1)
			ܦܢܐ ܕܓܠܐ;

		auto &sinks = dff_sink[bit];
		ܐܢ (sinks.size() != 1)
			ܦܢܐ ܕܓܠܐ;

		Cell *cell;
		صحيح idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		ܐܢ (!cur_ff.has_clk)
			ܦܢܐ ܕܓܠܐ;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		ܐܢ (!found) {
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
		} ܐܚܪܢܐ {
			ܐܢ (ff.has_clk != cur_ff.has_clk)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_ce != cur_ff.has_ce)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_aload != cur_ff.has_aload)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_srst != cur_ff.has_srst)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_arst != cur_ff.has_arst)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_sr != cur_ff.has_sr)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_clk) {
				ܐܢ (ff.sig_clk != cur_ff.sig_clk)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_clk != cur_ff.pol_clk)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_ce) {
				ܐܢ (ff.sig_ce != cur_ff.sig_ce)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_ce != cur_ff.pol_ce)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_aload) {
				ܐܢ (ff.sig_aload != cur_ff.sig_aload)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_aload != cur_ff.pol_aload)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_srst) {
				ܐܢ (ff.sig_srst != cur_ff.sig_srst)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_srst != cur_ff.pol_srst)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_arst) {
				ܐܢ (ff.sig_arst != cur_ff.sig_arst)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_arst != cur_ff.pol_arst)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_sr) {
				ܐܢ (ff.pol_clr != cur_ff.pol_clr)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_set != cur_ff.pol_set)
					ܦܢܐ ܕܓܠܐ;
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
		found = ܫܪܪܐ;
	}

	ܦܢܐ found;
}

منطقي FfMergeHelper::find_input_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, صحيح>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	منطقي found = ܕܓܠܐ;

	pool<صحيح> const_bits;

	لكل (auto bit : sig)
	{
		ܐܢ (bit.wire == عدم) {
			const_bits.insert(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.data));
			ff.val_srst.append(RTLIL::Const(bit.data));
			ff.val_arst.append(RTLIL::Const(bit.data));
			استمر;
		}

		ܐܢ (!dff_driver.count(bit))
			ܦܢܐ ܕܓܠܐ;

		Cell *cell;
		صحيح idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		ܐܢ (!found) {
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
		} ܐܚܪܢܐ {
			ܐܢ (ff.has_gclk != cur_ff.has_gclk)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_clk != cur_ff.has_clk)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_ce != cur_ff.has_ce)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_aload != cur_ff.has_aload)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_srst != cur_ff.has_srst)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_arst != cur_ff.has_arst)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_sr != cur_ff.has_sr)
				ܦܢܐ ܕܓܠܐ;
			ܐܢ (ff.has_clk) {
				ܐܢ (ff.sig_clk != cur_ff.sig_clk)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_clk != cur_ff.pol_clk)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_ce) {
				ܐܢ (ff.sig_ce != cur_ff.sig_ce)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_ce != cur_ff.pol_ce)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_aload) {
				ܐܢ (ff.sig_aload != cur_ff.sig_aload)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_aload != cur_ff.pol_aload)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_srst) {
				ܐܢ (ff.sig_srst != cur_ff.sig_srst)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_srst != cur_ff.pol_srst)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_arst) {
				ܐܢ (ff.sig_arst != cur_ff.sig_arst)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_arst != cur_ff.pol_arst)
					ܦܢܐ ܕܓܠܐ;
			}
			ܐܢ (ff.has_sr) {
				ܐܢ (ff.pol_clr != cur_ff.pol_clr)
					ܦܢܐ ܕܓܠܐ;
				ܐܢ (ff.pol_set != cur_ff.pol_set)
					ܦܢܐ ܕܓܠܐ;
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
		found = ܫܪܪܐ;
	}

	ܐܢ (found && ff.has_sr) {
		لكل (auto i: const_bits) {
			ܐܢ (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} ܐܚܪܢܐ ܐܢ (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	ܦܢܐ found;
}


فراغ FfMergeHelper::remove_output_ff(ثابت pool<std::pair<Cell *, صحيح>> &bits) {
	لكل (auto &it : bits) {
		Cell *cell = it.first;
		صحيح idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

فراغ FfMergeHelper::mark_input_ff(ثابت pool<std::pair<Cell *, صحيح>> &bits) {
	لكل (auto &it : bits) {
		Cell *cell = it.first;
		صحيح idx = it.second;
		ܐܢ (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_count[d[idx]]++;
		}
	}
}

فراغ FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	لكل (auto wire : module->wires()) {
		ܐܢ (wire->port_output)
			لكل (auto bit : (*sigmap)(wire))
				sigbit_users_count[bit]++;
	}

	لكل (auto cell : module->cells()) {
		ܐܢ (cell->is_builtin_ff()) {
			ܐܢ (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				لكل (صحيح i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].insert(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			لكل (صحيح i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		لكل (auto &conn : cell->connections())
			ܐܢ (!cell->known() || cell->input(conn.first))
				لكل (auto bit : (*sigmap)(conn.second))
					sigbit_users_count[bit]++;
	}
}

فراغ FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_count.clear();
}
