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

#அந்தர்பாவ "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

தருக்க FfMergeHelper::is_output_unused(RTLIL::SigSpec sig) {
	க்கு (auto bit : (*sigmap)(sig))
		எனில் (sigbit_users_மொத்தம்[bit] != 0)
			திரும்பு பொய்;
	திரும்பு உண்மை;
}

தருக்க FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, முழு>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	தருக்க found = பொய்;

	க்கு (auto bit : sig)
	{
		எனில் (bit.wire == வெற்று || sigbit_users_மொத்தம்[bit] == 0) {
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			தொடர்;
		}

		எனில் (sigbit_users_மொத்தம்[bit] != 1)
			திரும்பு பொய்;

		auto &sinks = dff_sink[bit];
		எனில் (sinks.size() != 1)
			திரும்பு பொய்;

		Cell *cell;
		முழு idx;
		std::tie(cell, idx) = *sinks.begin();
		bits.செருகு(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		// Reject latches and $ff.
		எனில் (!cur_ff.has_clk)
			திரும்பு பொய்;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		எனில் (!found) {
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
		} இல்லையெனில் {
			எனில் (ff.has_clk != cur_ff.has_clk)
				திரும்பு பொய்;
			எனில் (ff.has_ce != cur_ff.has_ce)
				திரும்பு பொய்;
			எனில் (ff.has_aload != cur_ff.has_aload)
				திரும்பு பொய்;
			எனில் (ff.has_srst != cur_ff.has_srst)
				திரும்பு பொய்;
			எனில் (ff.has_arst != cur_ff.has_arst)
				திரும்பு பொய்;
			எனில் (ff.has_sr != cur_ff.has_sr)
				திரும்பு பொய்;
			எனில் (ff.has_clk) {
				எனில் (ff.sig_clk != cur_ff.sig_clk)
					திரும்பு பொய்;
				எனில் (ff.pol_clk != cur_ff.pol_clk)
					திரும்பு பொய்;
			}
			எனில் (ff.has_ce) {
				எனில் (ff.sig_ce != cur_ff.sig_ce)
					திரும்பு பொய்;
				எனில் (ff.pol_ce != cur_ff.pol_ce)
					திரும்பு பொய்;
			}
			எனில் (ff.has_aload) {
				எனில் (ff.sig_aload != cur_ff.sig_aload)
					திரும்பு பொய்;
				எனில் (ff.pol_aload != cur_ff.pol_aload)
					திரும்பு பொய்;
			}
			எனில் (ff.has_srst) {
				எனில் (ff.sig_srst != cur_ff.sig_srst)
					திரும்பு பொய்;
				எனில் (ff.pol_srst != cur_ff.pol_srst)
					திரும்பு பொய்;
				எனில் (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					திரும்பு பொய்;
			}
			எனில் (ff.has_arst) {
				எனில் (ff.sig_arst != cur_ff.sig_arst)
					திரும்பு பொய்;
				எனில் (ff.pol_arst != cur_ff.pol_arst)
					திரும்பு பொய்;
			}
			எனில் (ff.has_sr) {
				எனில் (ff.pol_clr != cur_ff.pol_clr)
					திரும்பு பொய்;
				எனில் (ff.pol_set != cur_ff.pol_set)
					திரும்பு பொய்;
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
		found = உண்மை;
	}

	திரும்பு found;
}

தருக்க FfMergeHelper::find_உள்ளீடு_ff(RTLIL::SigSpec sig, FfData &ff, pool<std::pair<Cell *, முழு>> &bits) {
	ff = FfData(module, initvals, module->twines().add(NEW_ID));
	sigmap->apply(sig);

	தருக்க found = பொய்;

	pool<முழு> const_bits;

	க்கு (auto bit : sig)
	{
		எனில் (bit.wire == வெற்று) {
			const_bits.செருகு(ff.width);
			ff.width++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_set.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.data));
			ff.val_srst.append(RTLIL::Const(bit.data));
			ff.val_arst.append(RTLIL::Const(bit.data));
			தொடர்;
		}

		எனில் (!dff_driver.மொத்தம்(bit))
			திரும்பு பொய்;

		Cell *cell;
		முழு idx;
		std::tie(cell, idx) = dff_driver[bit];
		bits.செருகு(std::make_pair(cell, idx));

		FfData cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		எனில் (!found) {
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
		} இல்லையெனில் {
			எனில் (ff.has_gclk != cur_ff.has_gclk)
				திரும்பு பொய்;
			எனில் (ff.has_clk != cur_ff.has_clk)
				திரும்பு பொய்;
			எனில் (ff.has_ce != cur_ff.has_ce)
				திரும்பு பொய்;
			எனில் (ff.has_aload != cur_ff.has_aload)
				திரும்பு பொய்;
			எனில் (ff.has_srst != cur_ff.has_srst)
				திரும்பு பொய்;
			எனில் (ff.has_arst != cur_ff.has_arst)
				திரும்பு பொய்;
			எனில் (ff.has_sr != cur_ff.has_sr)
				திரும்பு பொய்;
			எனில் (ff.has_clk) {
				எனில் (ff.sig_clk != cur_ff.sig_clk)
					திரும்பு பொய்;
				எனில் (ff.pol_clk != cur_ff.pol_clk)
					திரும்பு பொய்;
			}
			எனில் (ff.has_ce) {
				எனில் (ff.sig_ce != cur_ff.sig_ce)
					திரும்பு பொய்;
				எனில் (ff.pol_ce != cur_ff.pol_ce)
					திரும்பு பொய்;
			}
			எனில் (ff.has_aload) {
				எனில் (ff.sig_aload != cur_ff.sig_aload)
					திரும்பு பொய்;
				எனில் (ff.pol_aload != cur_ff.pol_aload)
					திரும்பு பொய்;
			}
			எனில் (ff.has_srst) {
				எனில் (ff.sig_srst != cur_ff.sig_srst)
					திரும்பு பொய்;
				எனில் (ff.pol_srst != cur_ff.pol_srst)
					திரும்பு பொய்;
				எனில் (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					திரும்பு பொய்;
			}
			எனில் (ff.has_arst) {
				எனில் (ff.sig_arst != cur_ff.sig_arst)
					திரும்பு பொய்;
				எனில் (ff.pol_arst != cur_ff.pol_arst)
					திரும்பு பொய்;
			}
			எனில் (ff.has_sr) {
				எனில் (ff.pol_clr != cur_ff.pol_clr)
					திரும்பு பொய்;
				எனில் (ff.pol_set != cur_ff.pol_set)
					திரும்பு பொய்;
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
		found = உண்மை;
	}

	எனில் (found && ff.has_sr) {
		க்கு (auto i: const_bits) {
			எனில் (ff.sig_d[i] == State::S0) {
				ff.sig_set[i] = ff.pol_set ? State::S0 : State::S1;
			} இல்லையெனில் எனில் (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	திரும்பு found;
}


வெறுமை FfMergeHelper::remove_output_ff(மாறிலி pool<std::pair<Cell *, முழு>> &bits) {
	க்கு (auto &it : bits) {
		Cell *cell = it.first;
		முழு idx = it.second;
		SigSpec q = cell->getPort(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->addWire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->setPort(ID::Q, q);
		initvals->set_init(cell->getPort(ID::Q), (*initvals)(q));
	}
}

வெறுமை FfMergeHelper::mark_உள்ளீடு_ff(மாறிலி pool<std::pair<Cell *, முழு>> &bits) {
	க்கு (auto &it : bits) {
		Cell *cell = it.first;
		முழு idx = it.second;
		எனில் (cell->hasPort(ID::D)) {
			SigSpec d = cell->getPort(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_மொத்தம்[d[idx]]++;
		}
	}
}

வெறுமை FfMergeHelper::set(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	க்கு (auto wire : module->wires()) {
		எனில் (wire->port_output)
			க்கு (auto bit : (*sigmap)(wire))
				sigbit_users_மொத்தம்[bit]++;
	}

	க்கு (auto cell : module->cells()) {
		எனில் (cell->is_builtin_ff()) {
			எனில் (cell->hasPort(ID::D)) {
				SigSpec d = (*sigmap)(cell->getPort(ID::D));
				க்கு (முழு i = 0; i < GetSize(d); i++)
					dff_sink[d[i]].செருகு(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->getPort(ID::Q));
			க்கு (முழு i = 0; i < GetSize(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		க்கு (auto &conn : cell->connections())
			எனில் (!cell->known() || cell->உள்ளீடு(conn.first))
				க்கு (auto bit : (*sigmap)(conn.second))
					sigbit_users_மொத்தம்[bit]++;
	}
}

வெறுமை FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_மொத்தம்.clear();
}
