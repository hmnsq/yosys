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

#include "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

bool FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	សម្រាប់ (auto bit : (*sigmap)(sig))
		ប្រសិនបើ (sigbit_users_count[bit] != 0)
			ត្រឡប់ false;
	ត្រឡប់ true;
}

bool FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, ចំនួនគត់>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	សម្រាប់ (auto bit : sig)
	{
		ប្រសិនបើ (bit.wire == NULL || sigbit_users_count[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			continue;
		}

		ប្រសិនបើ (sigbit_users_count[bit] != 1)
			ត្រឡប់ false;

		auto &sinks = dff_sink[bit];
		ប្រសិនបើ (sinks.size() != 1)
			ត្រឡប់ false;

		Cell *cell;
		ចំនួនគត់ idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		ប្រសិនបើ (!cur_ff.has_clk)
			ត្រឡប់ false;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		ប្រសិនបើ (!found) {
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
		} បើមិនដូច្នេះ {
			ប្រសិនបើ (ff.has_clk != cur_ff.has_clk)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_ce != cur_ff.has_ce)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_aload != cur_ff.has_aload)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_srst != cur_ff.has_srst)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_arst != cur_ff.has_arst)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_sr != cur_ff.has_sr)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_clk) {
				ប្រសិនបើ (ff.sig_clk != cur_ff.sig_clk)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_clk != cur_ff.pol_clk)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_ce) {
				ប្រសិនបើ (ff.sig_ce != cur_ff.sig_ce)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_ce != cur_ff.pol_ce)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_aload) {
				ប្រសិនបើ (ff.sig_aload != cur_ff.sig_aload)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_aload != cur_ff.pol_aload)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_srst) {
				ប្រសិនបើ (ff.sig_srst != cur_ff.sig_srst)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_srst != cur_ff.pol_srst)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_arst) {
				ប្រសិនបើ (ff.sig_arst != cur_ff.sig_arst)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_arst != cur_ff.pol_arst)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_sr) {
				ប្រសិនបើ (ff.pol_clr != cur_ff.pol_clr)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_set != cur_ff.pol_set)
					ត្រឡប់ false;
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

	ត្រឡប់ found;
}

bool FfMergeHelper::find_input_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, ចំនួនគត់>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	pool<ចំនួនគត់> const_bits;

	សម្រាប់ (auto bit : sig)
	{
		ប្រសិនបើ (bit.wire == NULL) {
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
			continue;
		}

		ប្រសិនបើ (!dff_driver.count(bit))
			ត្រឡប់ false;

		Cell *cell;
		ចំនួនគត់ idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		ប្រសិនបើ (!found) {
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
		} បើមិនដូច្នេះ {
			ប្រសិនបើ (ff.has_gclk != cur_ff.has_gclk)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_clk != cur_ff.has_clk)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_ce != cur_ff.has_ce)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_aload != cur_ff.has_aload)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_srst != cur_ff.has_srst)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_arst != cur_ff.has_arst)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_sr != cur_ff.has_sr)
				ត្រឡប់ false;
			ប្រសិនបើ (ff.has_clk) {
				ប្រសិនបើ (ff.sig_clk != cur_ff.sig_clk)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_clk != cur_ff.pol_clk)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_ce) {
				ប្រសិនបើ (ff.sig_ce != cur_ff.sig_ce)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_ce != cur_ff.pol_ce)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_aload) {
				ប្រសិនបើ (ff.sig_aload != cur_ff.sig_aload)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_aload != cur_ff.pol_aload)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_srst) {
				ប្រសិនបើ (ff.sig_srst != cur_ff.sig_srst)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_srst != cur_ff.pol_srst)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_arst) {
				ប្រសិនបើ (ff.sig_arst != cur_ff.sig_arst)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_arst != cur_ff.pol_arst)
					ត្រឡប់ false;
			}
			ប្រសិនបើ (ff.has_sr) {
				ប្រសិនបើ (ff.pol_clr != cur_ff.pol_clr)
					ត្រឡប់ false;
				ប្រសិនបើ (ff.pol_set != cur_ff.pol_set)
					ត្រឡប់ false;
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

	ប្រសិនបើ (found && ff.has_sr) {
		សម្រាប់ (auto i: const_bits) {
			ប្រសិនបើ (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} បើមិនដូច្នេះ ប្រសិនបើ (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	ត្រឡប់ found;
}


void FfMergeHelper::remove_output_ff(const pool<std::pair<Cell *, ចំនួនគត់>> &bits) {
	សម្រាប់ (auto &it : bits) {
		Cell *cell = it.first;
		ចំនួនគត់ idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

void FfMergeHelper::mark_input_ff(const pool<std::pair<Cell *, ចំនួនគត់>> &bits) {
	សម្រាប់ (auto &it : bits) {
		Cell *cell = it.first;
		ចំនួនគត់ idx = it.second;
		ប្រសិនបើ (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_count[d[idx]]++;
		}
	}
}

void FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	សម្រាប់ (auto wire : module->wires()) {
		ប្រសិនបើ (wire->port_output)
			សម្រាប់ (auto bit : (*sigmap)(wire))
				sigbit_users_count[bit]++;
	}

	សម្រាប់ (auto cell : module->cells()) {
		ប្រសិនបើ (cell->is_builtin_ff()) {
			ប្រសិនបើ (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				សម្រាប់ (ចំនួនគត់ i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].insert(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			សម្រាប់ (ចំនួនគត់ i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		សម្រាប់ (auto &conn : cell->connections())
			ប្រសិនបើ (!cell->known() || cell->input(conn.first))
				សម្រាប់ (auto bit : (*sigmap)(conn.second))
					sigbit_users_count[bit]++;
	}
}

void FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_count.clear();
}
