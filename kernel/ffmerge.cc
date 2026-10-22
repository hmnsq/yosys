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

منطقي Ff_ملاؤ_Helper::is_output_بن_ورتیا(RTLIL::SigSpec sig) {
	لكل (خودکار ثنائی_ہندسہ : (*sigmap)(sig))
		إذا (sigbit_users_گنتی[ثنائی_ہندسہ] != 0)
			أرجع خطأ;
	أرجع سچ;
}

منطقي Ff_ملاؤ_Helper::لبھو_output_ff(RTLIL::SigSpec sig, Ff_انکڑے &ff, pool<std::pair<Cell *, صحيح>> &بٹ) {
	ff = Ff_انکڑے(module, initvals, module->twines().جوڑو(NEW_ID));
	sigmap->apply(sig);

	منطقي لبھیا = خطأ;

	لكل (خودکار ثنائی_ہندسہ : sig)
	{
		إذا (ثنائی_ہندسہ.wire == عدم || sigbit_users_گنتی[ثنائی_ہندسہ] == 0) {
			ff.چوڑائی++;
			ff.sig_q.append(ثنائی_ہندسہ);
			ff.sig_d.append(ثنائی_ہندسہ);
			ff.sig_clr.append(State::Sx);
			ff.sig_سیٹ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			استمر;
		}

		إذا (sigbit_users_گنتی[ثنائی_ہندسہ] != 1)
			أرجع خطأ;

		خودکار &sinks = dff_sink[ثنائی_ہندسہ];
		إذا (sinks.ناپ() != 1)
			أرجع خطأ;

		Cell *cell;
		صحيح idx;
		std::tie(cell, idx) = *sinks.begin();
		بٹ.داخل_کرو(std::make_pair(cell, idx));

		Ff_انکڑے cur_ff(initvals, cell);

		// Reject latches and $ff.
		إذا (!cur_ff.has_clk)
			أرجع خطأ;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == ثنائی_ہندسہ);

		إذا (!لبھیا) {
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
		} وإلا {
			إذا (ff.has_clk != cur_ff.has_clk)
				أرجع خطأ;
			إذا (ff.has_ce != cur_ff.has_ce)
				أرجع خطأ;
			إذا (ff.has_aload != cur_ff.has_aload)
				أرجع خطأ;
			إذا (ff.has_srst != cur_ff.has_srst)
				أرجع خطأ;
			إذا (ff.has_arst != cur_ff.has_arst)
				أرجع خطأ;
			إذا (ff.has_sr != cur_ff.has_sr)
				أرجع خطأ;
			إذا (ff.has_clk) {
				إذا (ff.sig_clk != cur_ff.sig_clk)
					أرجع خطأ;
				إذا (ff.pol_clk != cur_ff.pol_clk)
					أرجع خطأ;
			}
			إذا (ff.has_ce) {
				إذا (ff.sig_ce != cur_ff.sig_ce)
					أرجع خطأ;
				إذا (ff.pol_ce != cur_ff.pol_ce)
					أرجع خطأ;
			}
			إذا (ff.has_aload) {
				إذا (ff.sig_aload != cur_ff.sig_aload)
					أرجع خطأ;
				إذا (ff.pol_aload != cur_ff.pol_aload)
					أرجع خطأ;
			}
			إذا (ff.has_srst) {
				إذا (ff.sig_srst != cur_ff.sig_srst)
					أرجع خطأ;
				إذا (ff.pol_srst != cur_ff.pol_srst)
					أرجع خطأ;
				إذا (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					أرجع خطأ;
			}
			إذا (ff.has_arst) {
				إذا (ff.sig_arst != cur_ff.sig_arst)
					أرجع خطأ;
				إذا (ff.pol_arst != cur_ff.pol_arst)
					أرجع خطأ;
			}
			إذا (ff.has_sr) {
				إذا (ff.pol_clr != cur_ff.pol_clr)
					أرجع خطأ;
				إذا (ff.pol_سیٹ != cur_ff.pol_سیٹ)
					أرجع خطأ;
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
		لبھیا = سچ;
	}

	أرجع لبھیا;
}

منطقي Ff_ملاؤ_Helper::لبھو_داخلہ_ff(RTLIL::SigSpec sig, Ff_انکڑے &ff, pool<std::pair<Cell *, صحيح>> &بٹ) {
	ff = Ff_انکڑے(module, initvals, module->twines().جوڑو(NEW_ID));
	sigmap->apply(sig);

	منطقي لبھیا = خطأ;

	pool<صحيح> const_بٹ;

	لكل (خودکار ثنائی_ہندسہ : sig)
	{
		إذا (ثنائی_ہندسہ.wire == عدم) {
			const_بٹ.داخل_کرو(ff.چوڑائی);
			ff.چوڑائی++;
			ff.sig_q.append(ثنائی_ہندسہ);
			ff.sig_d.append(ثنائی_ہندسہ);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_سیٹ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(ثنائی_ہندسہ.انکڑے));
			ff.val_srst.append(RTLIL::Const(ثنائی_ہندسہ.انکڑے));
			ff.val_arst.append(RTLIL::Const(ثنائی_ہندسہ.انکڑے));
			استمر;
		}

		إذا (!dff_driver.گنتی(ثنائی_ہندسہ))
			أرجع خطأ;

		Cell *cell;
		صحيح idx;
		std::tie(cell, idx) = dff_driver[ثنائی_ہندسہ];
		بٹ.داخل_کرو(std::make_pair(cell, idx));

		Ff_انکڑے cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == ثنائی_ہندسہ);

		إذا (!لبھیا) {
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
		} وإلا {
			إذا (ff.has_gclk != cur_ff.has_gclk)
				أرجع خطأ;
			إذا (ff.has_clk != cur_ff.has_clk)
				أرجع خطأ;
			إذا (ff.has_ce != cur_ff.has_ce)
				أرجع خطأ;
			إذا (ff.has_aload != cur_ff.has_aload)
				أرجع خطأ;
			إذا (ff.has_srst != cur_ff.has_srst)
				أرجع خطأ;
			إذا (ff.has_arst != cur_ff.has_arst)
				أرجع خطأ;
			إذا (ff.has_sr != cur_ff.has_sr)
				أرجع خطأ;
			إذا (ff.has_clk) {
				إذا (ff.sig_clk != cur_ff.sig_clk)
					أرجع خطأ;
				إذا (ff.pol_clk != cur_ff.pol_clk)
					أرجع خطأ;
			}
			إذا (ff.has_ce) {
				إذا (ff.sig_ce != cur_ff.sig_ce)
					أرجع خطأ;
				إذا (ff.pol_ce != cur_ff.pol_ce)
					أرجع خطأ;
			}
			إذا (ff.has_aload) {
				إذا (ff.sig_aload != cur_ff.sig_aload)
					أرجع خطأ;
				إذا (ff.pol_aload != cur_ff.pol_aload)
					أرجع خطأ;
			}
			إذا (ff.has_srst) {
				إذا (ff.sig_srst != cur_ff.sig_srst)
					أرجع خطأ;
				إذا (ff.pol_srst != cur_ff.pol_srst)
					أرجع خطأ;
				إذا (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					أرجع خطأ;
			}
			إذا (ff.has_arst) {
				إذا (ff.sig_arst != cur_ff.sig_arst)
					أرجع خطأ;
				إذا (ff.pol_arst != cur_ff.pol_arst)
					أرجع خطأ;
			}
			إذا (ff.has_sr) {
				إذا (ff.pol_clr != cur_ff.pol_clr)
					أرجع خطأ;
				إذا (ff.pol_سیٹ != cur_ff.pol_سیٹ)
					أرجع خطأ;
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
		لبھیا = سچ;
	}

	إذا (لبھیا && ff.has_sr) {
		لكل (خودکار i: const_بٹ) {
			إذا (ff.sig_d[i] == State::S0) {
				ff.sig_سیٹ[i] = ff.pol_سیٹ ? State::S0 : State::S1;
			} وإلا إذا (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	أرجع لبھیا;
}


فراغ Ff_ملاؤ_Helper::remove_output_ff(ثابت pool<std::pair<Cell *, صحيح>> &بٹ) {
	لكل (خودکار &it : بٹ) {
		Cell *cell = it.پہلا;
		صحيح idx = it.دوجا;
		SigSpec q = cell->get_پورٹ(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->جوڑو_Wire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->سیٹ_پورٹ(ID::Q, q);
		initvals->سیٹ_init(cell->get_پورٹ(ID::Q), (*initvals)(q));
	}
}

فراغ Ff_ملاؤ_Helper::mark_داخلہ_ff(ثابت pool<std::pair<Cell *, صحيح>> &بٹ) {
	لكل (خودکار &it : بٹ) {
		Cell *cell = it.پہلا;
		صحيح idx = it.دوجا;
		إذا (cell->has_پورٹ(ID::D)) {
			SigSpec d = cell->get_پورٹ(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_گنتی[d[idx]]++;
		}
	}
}

فراغ Ff_ملاؤ_Helper::سیٹ(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	لكل (خودکار wire : module->wires()) {
		إذا (wire->پورٹ_output)
			لكل (خودکار ثنائی_ہندسہ : (*sigmap)(wire))
				sigbit_users_گنتی[ثنائی_ہندسہ]++;
	}

	لكل (خودکار cell : module->cells()) {
		إذا (cell->is_builtin_ff()) {
			إذا (cell->has_پورٹ(ID::D)) {
				SigSpec d = (*sigmap)(cell->get_پورٹ(ID::D));
				لكل (صحيح i = 0; i < Get_ناپ(d); i++)
					dff_sink[d[i]].داخل_کرو(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->get_پورٹ(ID::Q));
			لكل (صحيح i = 0; i < Get_ناپ(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		لكل (خودکار &conn : cell->connections())
			إذا (!cell->معلوم() || cell->داخلہ(conn.پہلا))
				لكل (خودکار ثنائی_ہندسہ : (*sigmap)(conn.دوجا))
					sigbit_users_گنتی[ثنائی_ہندسہ]++;
	}
}

فراغ Ff_ملاؤ_Helper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_گنتی.clear();
}
