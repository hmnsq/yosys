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

#అన్తర్భావ "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

తార్కిక Ff_విలీనం_Helper::is_output_వాడని(RTLIL::SigSpec sig) {
	కొరకు (స్వయంచాలక బిట్టు : (*sigmap)(sig))
		ఒకవేళ (sigbit_users_లెక్క[బిట్టు] != 0)
			తిరిగి అబద్ధం;
	తిరిగి నిజం;
}

తార్కిక Ff_విలీనం_Helper::కనుగొను_output_ff(RTLIL::SigSpec sig, Ff_దత్తాంశం &ff, pool<std::pair<Cell *, పూర్ణాంకం>> &బిట్లు) {
	ff = Ff_దత్తాంశం(module, initvals, module->twines().కూడు(NEW_ID));
	sigmap->apply(sig);

	తార్కిక దొరికింది = అబద్ధం;

	కొరకు (స్వయంచాలక బిట్టు : sig)
	{
		ఒకవేళ (బిట్టు.wire == ఖాళీ || sigbit_users_లెక్క[బిట్టు] == 0) {
			ff.వెడల్పు++;
			ff.sig_q.append(బిట్టు);
			ff.sig_d.append(బిట్టు);
			ff.sig_clr.append(State::Sx);
			ff.sig_సమితి.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			కొనసాగు;
		}

		ఒకవేళ (sigbit_users_లెక్క[బిట్టు] != 1)
			తిరిగి అబద్ధం;

		స్వయంచాలక &sinks = dff_sink[బిట్టు];
		ఒకవేళ (sinks.కొలత() != 1)
			తిరిగి అబద్ధం;

		Cell *cell;
		పూర్ణాంకం idx;
		std::tie(cell, idx) = *sinks.begin();
		బిట్లు.చొప్పించు(std::make_pair(cell, idx));

		Ff_దత్తాంశం cur_ff(initvals, cell);

		// Reject latches and $ff.
		ఒకవేళ (!cur_ff.has_clk)
			తిరిగి అబద్ధం;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == బిట్టు);

		ఒకవేళ (!దొరికింది) {
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
			ff.pol_సమితి = cur_ff.pol_సమితి;
		} లేకపోతే {
			ఒకవేళ (ff.has_clk != cur_ff.has_clk)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_ce != cur_ff.has_ce)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_aload != cur_ff.has_aload)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_srst != cur_ff.has_srst)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_arst != cur_ff.has_arst)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_sr != cur_ff.has_sr)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_clk) {
				ఒకవేళ (ff.sig_clk != cur_ff.sig_clk)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_clk != cur_ff.pol_clk)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_ce) {
				ఒకవేళ (ff.sig_ce != cur_ff.sig_ce)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_ce != cur_ff.pol_ce)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_aload) {
				ఒకవేళ (ff.sig_aload != cur_ff.sig_aload)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_aload != cur_ff.pol_aload)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_srst) {
				ఒకవేళ (ff.sig_srst != cur_ff.sig_srst)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_srst != cur_ff.pol_srst)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_arst) {
				ఒకవేళ (ff.sig_arst != cur_ff.sig_arst)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_arst != cur_ff.pol_arst)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_sr) {
				ఒకవేళ (ff.pol_clr != cur_ff.pol_clr)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_సమితి != cur_ff.pol_సమితి)
					తిరిగి అబద్ధం;
			}
		}

		ff.వెడల్పు++;
		ff.sig_d.append(cur_ff.sig_d[idx]);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_సమితి.append(ff.has_sr ? cur_ff.sig_సమితి[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		దొరికింది = నిజం;
	}

	తిరిగి దొరికింది;
}

తార్కిక Ff_విలీనం_Helper::కనుగొను_ఆదానం_ff(RTLIL::SigSpec sig, Ff_దత్తాంశం &ff, pool<std::pair<Cell *, పూర్ణాంకం>> &బిట్లు) {
	ff = Ff_దత్తాంశం(module, initvals, module->twines().కూడు(NEW_ID));
	sigmap->apply(sig);

	తార్కిక దొరికింది = అబద్ధం;

	pool<పూర్ణాంకం> const_బిట్లు;

	కొరకు (స్వయంచాలక బిట్టు : sig)
	{
		ఒకవేళ (బిట్టు.wire == ఖాళీ) {
			const_బిట్లు.చొప్పించు(ff.వెడల్పు);
			ff.వెడల్పు++;
			ff.sig_q.append(బిట్టు);
			ff.sig_d.append(బిట్టు);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_సమితి.append(State::Sx);
			ff.val_init.append(RTLIL::Const(బిట్టు.దత్తాంశం));
			ff.val_srst.append(RTLIL::Const(బిట్టు.దత్తాంశం));
			ff.val_arst.append(RTLIL::Const(బిట్టు.దత్తాంశం));
			కొనసాగు;
		}

		ఒకవేళ (!dff_driver.లెక్క(బిట్టు))
			తిరిగి అబద్ధం;

		Cell *cell;
		పూర్ణాంకం idx;
		std::tie(cell, idx) = dff_driver[బిట్టు];
		బిట్లు.చొప్పించు(std::make_pair(cell, idx));

		Ff_దత్తాంశం cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == బిట్టు);

		ఒకవేళ (!దొరికింది) {
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
			ff.pol_సమితి = cur_ff.pol_సమితి;
		} లేకపోతే {
			ఒకవేళ (ff.has_gclk != cur_ff.has_gclk)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_clk != cur_ff.has_clk)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_ce != cur_ff.has_ce)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_aload != cur_ff.has_aload)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_srst != cur_ff.has_srst)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_arst != cur_ff.has_arst)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_sr != cur_ff.has_sr)
				తిరిగి అబద్ధం;
			ఒకవేళ (ff.has_clk) {
				ఒకవేళ (ff.sig_clk != cur_ff.sig_clk)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_clk != cur_ff.pol_clk)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_ce) {
				ఒకవేళ (ff.sig_ce != cur_ff.sig_ce)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_ce != cur_ff.pol_ce)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_aload) {
				ఒకవేళ (ff.sig_aload != cur_ff.sig_aload)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_aload != cur_ff.pol_aload)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_srst) {
				ఒకవేళ (ff.sig_srst != cur_ff.sig_srst)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_srst != cur_ff.pol_srst)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_arst) {
				ఒకవేళ (ff.sig_arst != cur_ff.sig_arst)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_arst != cur_ff.pol_arst)
					తిరిగి అబద్ధం;
			}
			ఒకవేళ (ff.has_sr) {
				ఒకవేళ (ff.pol_clr != cur_ff.pol_clr)
					తిరిగి అబద్ధం;
				ఒకవేళ (ff.pol_సమితి != cur_ff.pol_సమితి)
					తిరిగి అబద్ధం;
			}
		}

		ff.వెడల్పు++;
		ff.sig_d.append((ff.has_clk || ff.has_gclk) ? cur_ff.sig_d[idx] : State::Sx);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_సమితి.append(ff.has_sr ? cur_ff.sig_సమితి[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		దొరికింది = నిజం;
	}

	ఒకవేళ (దొరికింది && ff.has_sr) {
		కొరకు (స్వయంచాలక i: const_బిట్లు) {
			ఒకవేళ (ff.sig_d[i] == State::S0) {
				ff.sig_సమితి[i] = ff.pol_సమితి ? State::S0 : State::S1;
			} లేకపోతే ఒకవేళ (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	తిరిగి దొరికింది;
}


శూన్య Ff_విలీనం_Helper::remove_output_ff(స్థిరరాశి pool<std::pair<Cell *, పూర్ణాంకం>> &బిట్లు) {
	కొరకు (స్వయంచాలక &it : బిట్లు) {
		Cell *cell = it.మొదటి;
		పూర్ణాంకం idx = it.రెండవ;
		SigSpec q = cell->get_పోర్టు(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->కూడు_Wire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->సమితి_పోర్టు(ID::Q, q);
		initvals->సమితి_init(cell->get_పోర్టు(ID::Q), (*initvals)(q));
	}
}

శూన్య Ff_విలీనం_Helper::mark_ఆదానం_ff(స్థిరరాశి pool<std::pair<Cell *, పూర్ణాంకం>> &బిట్లు) {
	కొరకు (స్వయంచాలక &it : బిట్లు) {
		Cell *cell = it.మొదటి;
		పూర్ణాంకం idx = it.రెండవ;
		ఒకవేళ (cell->has_పోర్టు(ID::D)) {
			SigSpec d = cell->get_పోర్టు(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_లెక్క[d[idx]]++;
		}
	}
}

శూన్య Ff_విలీనం_Helper::సమితి(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	కొరకు (స్వయంచాలక wire : module->wires()) {
		ఒకవేళ (wire->పోర్టు_output)
			కొరకు (స్వయంచాలక బిట్టు : (*sigmap)(wire))
				sigbit_users_లెక్క[బిట్టు]++;
	}

	కొరకు (స్వయంచాలక cell : module->cells()) {
		ఒకవేళ (cell->is_builtin_ff()) {
			ఒకవేళ (cell->has_పోర్టు(ID::D)) {
				SigSpec d = (*sigmap)(cell->get_పోర్టు(ID::D));
				కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(d); i++)
					dff_sink[d[i]].చొప్పించు(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->get_పోర్టు(ID::Q));
			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		కొరకు (స్వయంచాలక &conn : cell->connections())
			ఒకవేళ (!cell->తెలిసిన() || cell->ఆదానం(conn.మొదటి))
				కొరకు (స్వయంచాలక బిట్టు : (*sigmap)(conn.రెండవ))
					sigbit_users_లెక్క[బిట్టు]++;
	}
}

శూన్య Ff_విలీనం_Helper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_లెక్క.clear();
}
