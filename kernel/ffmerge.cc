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

#অন্তর্ভাব "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

যৌক্তিক FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	জন্য (auto bit : (*sigmap)(sig))
		যদি (sigbit_users_গুনতি[bit] != 0)
			ফেরত মিথ্যা;
	ফেরত সত্য;
}

যৌক্তিক FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, Ff_তথ্য &ff, pool<std::pair<Cell *, পূর্ণসংখ্যা>> &bits) {
	ff = Ff_তথ্য(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	যৌক্তিক found = মিথ্যা;

	জন্য (auto bit : sig)
	{
		যদি (bit.wire == ফাঁকা || sigbit_users_গুনতি[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			চালু;
		}

		যদি (sigbit_users_গুনতি[bit] != 1)
			ফেরত মিথ্যা;

		auto &sinks = dff_sink[bit];
		যদি (sinks.আকার_2() != 1)
			ফেরত মিথ্যা;

		Cell *cell;
		পূর্ণসংখ্যা idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.ঢোকাও(std::make_pair(cell, idx));

		Ff_তথ্য cur_ff(initvals, cell);

		// Reject latches and $ff.
		যদি (!cur_ff.has_clk)
			ফেরত মিথ্যা;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		যদি (!found) {
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
		} অন্যথা {
			যদি (ff.has_clk != cur_ff.has_clk)
				ফেরত মিথ্যা;
			যদি (ff.has_ce != cur_ff.has_ce)
				ফেরত মিথ্যা;
			যদি (ff.has_aload != cur_ff.has_aload)
				ফেরত মিথ্যা;
			যদি (ff.has_srst != cur_ff.has_srst)
				ফেরত মিথ্যা;
			যদি (ff.has_arst != cur_ff.has_arst)
				ফেরত মিথ্যা;
			যদি (ff.has_sr != cur_ff.has_sr)
				ফেরত মিথ্যা;
			যদি (ff.has_clk) {
				যদি (ff.sig_clk != cur_ff.sig_clk)
					ফেরত মিথ্যা;
				যদি (ff.pol_clk != cur_ff.pol_clk)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_ce) {
				যদি (ff.sig_ce != cur_ff.sig_ce)
					ফেরত মিথ্যা;
				যদি (ff.pol_ce != cur_ff.pol_ce)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_aload) {
				যদি (ff.sig_aload != cur_ff.sig_aload)
					ফেরত মিথ্যা;
				যদি (ff.pol_aload != cur_ff.pol_aload)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_srst) {
				যদি (ff.sig_srst != cur_ff.sig_srst)
					ফেরত মিথ্যা;
				যদি (ff.pol_srst != cur_ff.pol_srst)
					ফেরত মিথ্যা;
				যদি (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_arst) {
				যদি (ff.sig_arst != cur_ff.sig_arst)
					ফেরত মিথ্যা;
				যদি (ff.pol_arst != cur_ff.pol_arst)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_sr) {
				যদি (ff.pol_clr != cur_ff.pol_clr)
					ফেরত মিথ্যা;
				যদি (ff.pol_set != cur_ff.pol_set)
					ফেরত মিথ্যা;
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
		found = সত্য;
	}

	ফেরত found;
}

যৌক্তিক FfMergeHelper::find_প্রবেশ_ff(RTLIL::SigSpec sig, Ff_তথ্য &ff, pool<std::pair<Cell *, পূর্ণসংখ্যা>> &bits) {
	ff = Ff_তথ্য(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	যৌক্তিক found = মিথ্যা;

	pool<পূর্ণসংখ্যা> const_bits;

	জন্য (auto bit : sig)
	{
		যদি (bit.wire == ফাঁকা) {
			const_bits.ঢোকাও(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.তথ্য));
			ff.val_srst.append(RTLIL::Const(bit.তথ্য));
			ff.val_arst.append(RTLIL::Const(bit.তথ্য));
			চালু;
		}

		যদি (!dff_driver.গুনতি(bit))
			ফেরত মিথ্যা;

		Cell *cell;
		পূর্ণসংখ্যা idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.ঢোকাও(std::make_pair(cell, idx));

		Ff_তথ্য cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		যদি (!found) {
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
		} অন্যথা {
			যদি (ff.has_gclk != cur_ff.has_gclk)
				ফেরত মিথ্যা;
			যদি (ff.has_clk != cur_ff.has_clk)
				ফেরত মিথ্যা;
			যদি (ff.has_ce != cur_ff.has_ce)
				ফেরত মিথ্যা;
			যদি (ff.has_aload != cur_ff.has_aload)
				ফেরত মিথ্যা;
			যদি (ff.has_srst != cur_ff.has_srst)
				ফেরত মিথ্যা;
			যদি (ff.has_arst != cur_ff.has_arst)
				ফেরত মিথ্যা;
			যদি (ff.has_sr != cur_ff.has_sr)
				ফেরত মিথ্যা;
			যদি (ff.has_clk) {
				যদি (ff.sig_clk != cur_ff.sig_clk)
					ফেরত মিথ্যা;
				যদি (ff.pol_clk != cur_ff.pol_clk)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_ce) {
				যদি (ff.sig_ce != cur_ff.sig_ce)
					ফেরত মিথ্যা;
				যদি (ff.pol_ce != cur_ff.pol_ce)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_aload) {
				যদি (ff.sig_aload != cur_ff.sig_aload)
					ফেরত মিথ্যা;
				যদি (ff.pol_aload != cur_ff.pol_aload)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_srst) {
				যদি (ff.sig_srst != cur_ff.sig_srst)
					ফেরত মিথ্যা;
				যদি (ff.pol_srst != cur_ff.pol_srst)
					ফেরত মিথ্যা;
				যদি (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_arst) {
				যদি (ff.sig_arst != cur_ff.sig_arst)
					ফেরত মিথ্যা;
				যদি (ff.pol_arst != cur_ff.pol_arst)
					ফেরত মিথ্যা;
			}
			যদি (ff.has_sr) {
				যদি (ff.pol_clr != cur_ff.pol_clr)
					ফেরত মিথ্যা;
				যদি (ff.pol_set != cur_ff.pol_set)
					ফেরত মিথ্যা;
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
		found = সত্য;
	}

	যদি (found && ff.has_sr) {
		জন্য (auto i: const_bits) {
			যদি (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} অন্যথা যদি (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	ফেরত found;
}


শূন্য FfMergeHelper::remove_output_ff(ধ্রুবক pool<std::pair<Cell *, পূর্ণসংখ্যা>> &bits) {
	জন্য (auto &it : bits) {
		Cell *cell = it.first;
		পূর্ণসংখ্যা idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

শূন্য FfMergeHelper::mark_প্রবেশ_ff(ধ্রুবক pool<std::pair<Cell *, পূর্ণসংখ্যা>> &bits) {
	জন্য (auto &it : bits) {
		Cell *cell = it.first;
		পূর্ণসংখ্যা idx = it.second;
		যদি (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_গুনতি[d[idx]]++;
		}
	}
}

শূন্য FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	জন্য (auto wire : module->wires()) {
		যদি (wire->port_output)
			জন্য (auto bit : (*sigmap)(wire))
				sigbit_users_গুনতি[bit]++;
	}

	জন্য (auto cell : module->cells()) {
		যদি (cell->is_builtin_ff()) {
			যদি (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(d); i++)
					dff_sink[d[i]].ঢোকাও(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		জন্য (auto &conn : cell->connections())
			যদি (!cell->known() || cell->প্রবেশ(conn.first))
				জন্য (auto bit : (*sigmap)(conn.second))
					sigbit_users_গুনতি[bit]++;
	}
}

শূন্য FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_গুনতি.clear();
}
