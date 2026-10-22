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
	위해 (auto bit : (*sigmap)(sig))
		만약 (sigbit_users_count[bit] != 0)
			반환 false;
	반환 true;
}

bool FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, 정수>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	위해 (auto bit : sig)
	{
		만약 (bit.wire == NULL || sigbit_users_count[bit] == 0) {
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

		만약 (sigbit_users_count[bit] != 1)
			반환 false;

		auto &sinks = dff_sink[bit];
		만약 (sinks.size() != 1)
			반환 false;

		Cell *cell;
		정수 idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		만약 (!cur_ff.has_clk)
			반환 false;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		만약 (!found) {
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
		} 아니면 {
			만약 (ff.has_clk != cur_ff.has_clk)
				반환 false;
			만약 (ff.has_ce != cur_ff.has_ce)
				반환 false;
			만약 (ff.has_aload != cur_ff.has_aload)
				반환 false;
			만약 (ff.has_srst != cur_ff.has_srst)
				반환 false;
			만약 (ff.has_arst != cur_ff.has_arst)
				반환 false;
			만약 (ff.has_sr != cur_ff.has_sr)
				반환 false;
			만약 (ff.has_clk) {
				만약 (ff.sig_clk != cur_ff.sig_clk)
					반환 false;
				만약 (ff.pol_clk != cur_ff.pol_clk)
					반환 false;
			}
			만약 (ff.has_ce) {
				만약 (ff.sig_ce != cur_ff.sig_ce)
					반환 false;
				만약 (ff.pol_ce != cur_ff.pol_ce)
					반환 false;
			}
			만약 (ff.has_aload) {
				만약 (ff.sig_aload != cur_ff.sig_aload)
					반환 false;
				만약 (ff.pol_aload != cur_ff.pol_aload)
					반환 false;
			}
			만약 (ff.has_srst) {
				만약 (ff.sig_srst != cur_ff.sig_srst)
					반환 false;
				만약 (ff.pol_srst != cur_ff.pol_srst)
					반환 false;
				만약 (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					반환 false;
			}
			만약 (ff.has_arst) {
				만약 (ff.sig_arst != cur_ff.sig_arst)
					반환 false;
				만약 (ff.pol_arst != cur_ff.pol_arst)
					반환 false;
			}
			만약 (ff.has_sr) {
				만약 (ff.pol_clr != cur_ff.pol_clr)
					반환 false;
				만약 (ff.pol_set != cur_ff.pol_set)
					반환 false;
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

	반환 found;
}

bool FfMergeHelper::find_input_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, 정수>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	bool found = false;

	pool<정수> const_bits;

	위해 (auto bit : sig)
	{
		만약 (bit.wire == NULL) {
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

		만약 (!dff_driver.count(bit))
			반환 false;

		Cell *cell;
		정수 idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.insert(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		만약 (!found) {
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
		} 아니면 {
			만약 (ff.has_gclk != cur_ff.has_gclk)
				반환 false;
			만약 (ff.has_clk != cur_ff.has_clk)
				반환 false;
			만약 (ff.has_ce != cur_ff.has_ce)
				반환 false;
			만약 (ff.has_aload != cur_ff.has_aload)
				반환 false;
			만약 (ff.has_srst != cur_ff.has_srst)
				반환 false;
			만약 (ff.has_arst != cur_ff.has_arst)
				반환 false;
			만약 (ff.has_sr != cur_ff.has_sr)
				반환 false;
			만약 (ff.has_clk) {
				만약 (ff.sig_clk != cur_ff.sig_clk)
					반환 false;
				만약 (ff.pol_clk != cur_ff.pol_clk)
					반환 false;
			}
			만약 (ff.has_ce) {
				만약 (ff.sig_ce != cur_ff.sig_ce)
					반환 false;
				만약 (ff.pol_ce != cur_ff.pol_ce)
					반환 false;
			}
			만약 (ff.has_aload) {
				만약 (ff.sig_aload != cur_ff.sig_aload)
					반환 false;
				만약 (ff.pol_aload != cur_ff.pol_aload)
					반환 false;
			}
			만약 (ff.has_srst) {
				만약 (ff.sig_srst != cur_ff.sig_srst)
					반환 false;
				만약 (ff.pol_srst != cur_ff.pol_srst)
					반환 false;
				만약 (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					반환 false;
			}
			만약 (ff.has_arst) {
				만약 (ff.sig_arst != cur_ff.sig_arst)
					반환 false;
				만약 (ff.pol_arst != cur_ff.pol_arst)
					반환 false;
			}
			만약 (ff.has_sr) {
				만약 (ff.pol_clr != cur_ff.pol_clr)
					반환 false;
				만약 (ff.pol_set != cur_ff.pol_set)
					반환 false;
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

	만약 (found && ff.has_sr) {
		위해 (auto i: const_bits) {
			만약 (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} 아니면 만약 (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	반환 found;
}


void FfMergeHelper::remove_output_ff(const pool<std::pair<Cell *, 정수>> &bits) {
	위해 (auto &it : bits) {
		Cell *cell = it.first;
		정수 idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

void FfMergeHelper::mark_input_ff(const pool<std::pair<Cell *, 정수>> &bits) {
	위해 (auto &it : bits) {
		Cell *cell = it.first;
		정수 idx = it.second;
		만약 (cell->hasPort(ID::D)) {
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

	위해 (auto wire : module->wires()) {
		만약 (wire->port_output)
			위해 (auto bit : (*sigmap)(wire))
				sigbit_users_count[bit]++;
	}

	위해 (auto cell : module->cells()) {
		만약 (cell->is_builtin_ff()) {
			만약 (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				위해 (정수 i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].insert(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			위해 (정수 i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		위해 (auto &conn : cell->connections())
			만약 (!cell->known() || cell->input(conn.first))
				위해 (auto bit : (*sigmap)(conn.second))
					sigbit_users_count[bit]++;
	}
}

void FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_count.clear();
}
