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
	для (auto bit : (*sigmap)(sig))
		если (sigbit_users_count[bit] != 0)
			вернуть false;
	вернуть true;
}

bool FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, целое>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	для (auto bit : sig)
	{
		если (bit.wire == NULL || sigbit_users_count[bit] == 0) {
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

		если (sigbit_users_count[bit] != 1)
			вернуть false;

		auto &sinks = dff_sink[bit];
		если (sinks.size() != 1)
			вернуть false;

		Cell *cell;
		целое idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		если (!cur_ff.has_clk)
			вернуть false;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		если (!found) {
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
		} иначе {
			если (ff.has_clk != cur_ff.has_clk)
				вернуть false;
			если (ff.has_ce != cur_ff.has_ce)
				вернуть false;
			если (ff.has_aload != cur_ff.has_aload)
				вернуть false;
			если (ff.has_srst != cur_ff.has_srst)
				вернуть false;
			если (ff.has_arst != cur_ff.has_arst)
				вернуть false;
			если (ff.has_sr != cur_ff.has_sr)
				вернуть false;
			если (ff.has_clk) {
				если (ff.sig_clk != cur_ff.sig_clk)
					вернуть false;
				если (ff.pol_clk != cur_ff.pol_clk)
					вернуть false;
			}
			если (ff.has_ce) {
				если (ff.sig_ce != cur_ff.sig_ce)
					вернуть false;
				если (ff.pol_ce != cur_ff.pol_ce)
					вернуть false;
			}
			если (ff.has_aload) {
				если (ff.sig_aload != cur_ff.sig_aload)
					вернуть false;
				если (ff.pol_aload != cur_ff.pol_aload)
					вернуть false;
			}
			если (ff.has_srst) {
				если (ff.sig_srst != cur_ff.sig_srst)
					вернуть false;
				если (ff.pol_srst != cur_ff.pol_srst)
					вернуть false;
				если (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					вернуть false;
			}
			если (ff.has_arst) {
				если (ff.sig_arst != cur_ff.sig_arst)
					вернуть false;
				если (ff.pol_arst != cur_ff.pol_arst)
					вернуть false;
			}
			если (ff.has_sr) {
				если (ff.pol_clr != cur_ff.pol_clr)
					вернуть false;
				если (ff.pol_set != cur_ff.pol_set)
					вернуть false;
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

	вернуть found;
}

bool FfMergeHelper::find_input_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, целое>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	pool<целое> const_bits;

	для (auto bit : sig)
	{
		если (bit.wire == NULL) {
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

		если (!dff_driver.count(bit))
			вернуть false;

		Cell *cell;
		целое idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		если (!found) {
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
		} иначе {
			если (ff.has_gclk != cur_ff.has_gclk)
				вернуть false;
			если (ff.has_clk != cur_ff.has_clk)
				вернуть false;
			если (ff.has_ce != cur_ff.has_ce)
				вернуть false;
			если (ff.has_aload != cur_ff.has_aload)
				вернуть false;
			если (ff.has_srst != cur_ff.has_srst)
				вернуть false;
			если (ff.has_arst != cur_ff.has_arst)
				вернуть false;
			если (ff.has_sr != cur_ff.has_sr)
				вернуть false;
			если (ff.has_clk) {
				если (ff.sig_clk != cur_ff.sig_clk)
					вернуть false;
				если (ff.pol_clk != cur_ff.pol_clk)
					вернуть false;
			}
			если (ff.has_ce) {
				если (ff.sig_ce != cur_ff.sig_ce)
					вернуть false;
				если (ff.pol_ce != cur_ff.pol_ce)
					вернуть false;
			}
			если (ff.has_aload) {
				если (ff.sig_aload != cur_ff.sig_aload)
					вернуть false;
				если (ff.pol_aload != cur_ff.pol_aload)
					вернуть false;
			}
			если (ff.has_srst) {
				если (ff.sig_srst != cur_ff.sig_srst)
					вернуть false;
				если (ff.pol_srst != cur_ff.pol_srst)
					вернуть false;
				если (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					вернуть false;
			}
			если (ff.has_arst) {
				если (ff.sig_arst != cur_ff.sig_arst)
					вернуть false;
				если (ff.pol_arst != cur_ff.pol_arst)
					вернуть false;
			}
			если (ff.has_sr) {
				если (ff.pol_clr != cur_ff.pol_clr)
					вернуть false;
				если (ff.pol_set != cur_ff.pol_set)
					вернуть false;
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

	если (found && ff.has_sr) {
		для (auto i: const_bits) {
			если (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} иначе если (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	вернуть found;
}


void FfMergeHelper::remove_output_ff(const pool<std::pair<Cell *, целое>> &bits) {
	для (auto &it : bits) {
		Cell *cell = it.first;
		целое idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

void FfMergeHelper::mark_input_ff(const pool<std::pair<Cell *, целое>> &bits) {
	для (auto &it : bits) {
		Cell *cell = it.first;
		целое idx = it.second;
		если (cell->hasPort(ID::D)) {
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

	для (auto wire : module->wires()) {
		если (wire->port_output)
			для (auto bit : (*sigmap)(wire))
				sigbit_users_count[bit]++;
	}

	для (auto cell : module->cells()) {
		если (cell->is_builtin_ff()) {
			если (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				для (целое i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].insert(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			для (целое i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		для (auto &conn : cell->connections())
			если (!cell->known() || cell->input(conn.first))
				для (auto bit : (*sigmap)(conn.second))
					sigbit_users_count[bit]++;
	}
}

void FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_count.clear();
}
