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

#شامل "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

منطقی Ff_ملاؤ_Helper::is_output_غیر_مستعمل(RTLIL::SigSpec sig) {
	برائے (خودکار ثنائی_ہندسہ : (*sigmap)(sig))
		اگر (sigbit_users_گنتی[ثنائی_ہندسہ] != 0)
			واپس جھوٹ;
	واپس سچ;
}

منطقی Ff_ملاؤ_Helper::ڈھونڈو_output_ff(RTLIL::SigSpec sig, Ff_ڈیٹا &ff, pool<std::pair<Cell *, صحیح>> &بٹ) {
	ff = Ff_ڈیٹا(module, initvals, module->twines().جوڑو(NEW_ID));
	sigmap->apply(sig);

	منطقی ملا = جھوٹ;

	برائے (خودکار ثنائی_ہندسہ : sig)
	{
		اگر (ثنائی_ہندسہ.wire == تہی || sigbit_users_گنتی[ثنائی_ہندسہ] == 0) {
			ff.چوڑائی++;
			ff.sig_q.append(ثنائی_ہندسہ);
			ff.sig_d.append(ثنائی_ہندسہ);
			ff.sig_clr.append(State::Sx);
			ff.sig_سیٹ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			جاری;
		}

		اگر (sigbit_users_گنتی[ثنائی_ہندسہ] != 1)
			واپس جھوٹ;

		خودکار &sinks = dff_sink[ثنائی_ہندسہ];
		اگر (sinks.سائز() != 1)
			واپس جھوٹ;

		Cell *cell;
		صحیح idx;
		std::tie(cell, idx) = *sinks.begin();
		بٹ.داخل_کرو(std::make_pair(cell, idx));

		Ff_ڈیٹا cur_ff(initvals, cell);

		// Reject latches and $ff.
		اگر (!cur_ff.has_clk)
			واپس جھوٹ;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == ثنائی_ہندسہ);

		اگر (!ملا) {
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
			ff.pol_سیٹ = cur_ff.pol_سیٹ;
		} ورنہ {
			اگر (ff.has_clk != cur_ff.has_clk)
				واپس جھوٹ;
			اگر (ff.has_ce != cur_ff.has_ce)
				واپس جھوٹ;
			اگر (ff.has_aload != cur_ff.has_aload)
				واپس جھوٹ;
			اگر (ff.has_srst != cur_ff.has_srst)
				واپس جھوٹ;
			اگر (ff.has_arst != cur_ff.has_arst)
				واپس جھوٹ;
			اگر (ff.has_sr != cur_ff.has_sr)
				واپس جھوٹ;
			اگر (ff.has_clk) {
				اگر (ff.sig_clk != cur_ff.sig_clk)
					واپس جھوٹ;
				اگر (ff.pol_clk != cur_ff.pol_clk)
					واپس جھوٹ;
			}
			اگر (ff.has_ce) {
				اگر (ff.sig_ce != cur_ff.sig_ce)
					واپس جھوٹ;
				اگر (ff.pol_ce != cur_ff.pol_ce)
					واپس جھوٹ;
			}
			اگر (ff.has_aload) {
				اگر (ff.sig_aload != cur_ff.sig_aload)
					واپس جھوٹ;
				اگر (ff.pol_aload != cur_ff.pol_aload)
					واپس جھوٹ;
			}
			اگر (ff.has_srst) {
				اگر (ff.sig_srst != cur_ff.sig_srst)
					واپس جھوٹ;
				اگر (ff.pol_srst != cur_ff.pol_srst)
					واپس جھوٹ;
				اگر (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					واپس جھوٹ;
			}
			اگر (ff.has_arst) {
				اگر (ff.sig_arst != cur_ff.sig_arst)
					واپس جھوٹ;
				اگر (ff.pol_arst != cur_ff.pol_arst)
					واپس جھوٹ;
			}
			اگر (ff.has_sr) {
				اگر (ff.pol_clr != cur_ff.pol_clr)
					واپس جھوٹ;
				اگر (ff.pol_سیٹ != cur_ff.pol_سیٹ)
					واپس جھوٹ;
			}
		}

		ff.چوڑائی++;
		ff.sig_d.append(cur_ff.sig_d[idx]);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_سیٹ.append(ff.has_sr ? cur_ff.sig_سیٹ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		ملا = سچ;
	}

	واپس ملا;
}

منطقی Ff_ملاؤ_Helper::ڈھونڈو_داخلہ_ff(RTLIL::SigSpec sig, Ff_ڈیٹا &ff, pool<std::pair<Cell *, صحیح>> &بٹ) {
	ff = Ff_ڈیٹا(module, initvals, module->twines().جوڑو(NEW_ID));
	sigmap->apply(sig);

	منطقی ملا = جھوٹ;

	pool<صحیح> const_بٹ;

	برائے (خودکار ثنائی_ہندسہ : sig)
	{
		اگر (ثنائی_ہندسہ.wire == تہی) {
			const_بٹ.داخل_کرو(ff.چوڑائی);
			ff.چوڑائی++;
			ff.sig_q.append(ثنائی_ہندسہ);
			ff.sig_d.append(ثنائی_ہندسہ);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_سیٹ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(ثنائی_ہندسہ.ڈیٹا));
			ff.val_srst.append(RTLIL::Const(ثنائی_ہندسہ.ڈیٹا));
			ff.val_arst.append(RTLIL::Const(ثنائی_ہندسہ.ڈیٹا));
			جاری;
		}

		اگر (!dff_driver.گنتی(ثنائی_ہندسہ))
			واپس جھوٹ;

		Cell *cell;
		صحیح idx;
		std::tie(cell, idx) = dff_driver[ثنائی_ہندسہ];
		بٹ.داخل_کرو(std::make_pair(cell, idx));

		Ff_ڈیٹا cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == ثنائی_ہندسہ);

		اگر (!ملا) {
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
			ff.pol_سیٹ = cur_ff.pol_سیٹ;
		} ورنہ {
			اگر (ff.has_gclk != cur_ff.has_gclk)
				واپس جھوٹ;
			اگر (ff.has_clk != cur_ff.has_clk)
				واپس جھوٹ;
			اگر (ff.has_ce != cur_ff.has_ce)
				واپس جھوٹ;
			اگر (ff.has_aload != cur_ff.has_aload)
				واپس جھوٹ;
			اگر (ff.has_srst != cur_ff.has_srst)
				واپس جھوٹ;
			اگر (ff.has_arst != cur_ff.has_arst)
				واپس جھوٹ;
			اگر (ff.has_sr != cur_ff.has_sr)
				واپس جھوٹ;
			اگر (ff.has_clk) {
				اگر (ff.sig_clk != cur_ff.sig_clk)
					واپس جھوٹ;
				اگر (ff.pol_clk != cur_ff.pol_clk)
					واپس جھوٹ;
			}
			اگر (ff.has_ce) {
				اگر (ff.sig_ce != cur_ff.sig_ce)
					واپس جھوٹ;
				اگر (ff.pol_ce != cur_ff.pol_ce)
					واپس جھوٹ;
			}
			اگر (ff.has_aload) {
				اگر (ff.sig_aload != cur_ff.sig_aload)
					واپس جھوٹ;
				اگر (ff.pol_aload != cur_ff.pol_aload)
					واپس جھوٹ;
			}
			اگر (ff.has_srst) {
				اگر (ff.sig_srst != cur_ff.sig_srst)
					واپس جھوٹ;
				اگر (ff.pol_srst != cur_ff.pol_srst)
					واپس جھوٹ;
				اگر (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					واپس جھوٹ;
			}
			اگر (ff.has_arst) {
				اگر (ff.sig_arst != cur_ff.sig_arst)
					واپس جھوٹ;
				اگر (ff.pol_arst != cur_ff.pol_arst)
					واپس جھوٹ;
			}
			اگر (ff.has_sr) {
				اگر (ff.pol_clr != cur_ff.pol_clr)
					واپس جھوٹ;
				اگر (ff.pol_سیٹ != cur_ff.pol_سیٹ)
					واپس جھوٹ;
			}
		}

		ff.چوڑائی++;
		ff.sig_d.append((ff.has_clk || ff.has_gclk) ? cur_ff.sig_d[idx] : State::Sx);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_سیٹ.append(ff.has_sr ? cur_ff.sig_سیٹ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		ملا = سچ;
	}

	اگر (ملا && ff.has_sr) {
		برائے (خودکار i: const_بٹ) {
			اگر (ff.sig_d[i] == State::S0) {
				ff.sig_سیٹ[i] = ff.pol_سیٹ ? State::S0 : State::S1;
			} ورنہ اگر (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	واپس ملا;
}


خالی Ff_ملاؤ_Helper::remove_output_ff(ثابت pool<std::pair<Cell *, صحیح>> &بٹ) {
	برائے (خودکار &it : بٹ) {
		Cell *cell = it.پہلا;
		صحیح idx = it.دوسرا;
		SigSpec q = cell->get_پورٹ(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->جوڑو_Wire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->سیٹ_پورٹ(ID::Q, q);
		initvals->سیٹ_init(cell->get_پورٹ(ID::Q), (*initvals)(q));
	}
}

خالی Ff_ملاؤ_Helper::mark_داخلہ_ff(ثابت pool<std::pair<Cell *, صحیح>> &بٹ) {
	برائے (خودکار &it : بٹ) {
		Cell *cell = it.پہلا;
		صحیح idx = it.دوسرا;
		اگر (cell->has_پورٹ(ID::D)) {
			SigSpec d = cell->get_پورٹ(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_گنتی[d[idx]]++;
		}
	}
}

خالی Ff_ملاؤ_Helper::سیٹ(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	برائے (خودکار wire : module->wires()) {
		اگر (wire->پورٹ_output)
			برائے (خودکار ثنائی_ہندسہ : (*sigmap)(wire))
				sigbit_users_گنتی[ثنائی_ہندسہ]++;
	}

	برائے (خودکار cell : module->cells()) {
		اگر (cell->is_builtin_ff()) {
			اگر (cell->has_پورٹ(ID::D)) {
				SigSpec d = (*sigmap)(cell->get_پورٹ(ID::D));
				برائے (صحیح i = 0; i < Get_سائز(d); i++)
					dff_sink[d[i]].داخل_کرو(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->get_پورٹ(ID::Q));
			برائے (صحیح i = 0; i < Get_سائز(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		برائے (خودکار &conn : cell->connections())
			اگر (!cell->معلوم() || cell->داخلہ(conn.پہلا))
				برائے (خودکار ثنائی_ہندسہ : (*sigmap)(conn.دوسرا))
					sigbit_users_گنتی[ثنائی_ہندسہ]++;
	}
}

خالی Ff_ملاؤ_Helper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_گنتی.clear();
}
