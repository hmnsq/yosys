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

منطقي Ff_یوځای_Helper::is_output_ناکارول_شوی(RTLIL::SigSpec sig) {
	لكل (خپلکاری بټ : (*sigmap)(sig))
		إذا (sigbit_users_شمېر[بټ] != 0)
			أرجع خطأ;
	أرجع ریښتیا;
}

منطقي Ff_یوځای_Helper::پیدا_کړه_output_ff(RTLIL::SigSpec sig, Ff_معلومات &ff, pool<std::pair<Cell *, صحيح>> &بټونه) {
	ff = Ff_معلومات(module, initvals, module->twines().جمع_کړه(NEW_ID));
	sigmap->apply(sig);

	منطقي وموندل = خطأ;

	لكل (خپلکاری بټ : sig)
	{
		إذا (بټ.wire == عدم || sigbit_users_شمېر[بټ] == 0) {
			ff.پلنوالی++;
			ff.sig_q.append(بټ);
			ff.sig_d.append(بټ);
			ff.sig_clr.append(State::Sx);
			ff.sig_سېټ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			استمر;
		}

		إذا (sigbit_users_شمېر[بټ] != 1)
			أرجع خطأ;

		خپلکاری &sinks = dff_sink[بټ];
		إذا (sinks.اندازه() != 1)
			أرجع خطأ;

		Cell *cell;
		صحيح idx;
		std::tie(cell, idx) = *sinks.begin();
		بټونه.ورننباسه(std::make_pair(cell, idx));

		Ff_معلومات cur_ff(initvals, cell);

		// Reject latches and $ff.
		إذا (!cur_ff.has_clk)
			أرجع خطأ;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == بټ);

		إذا (!وموندل) {
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
			ff.pol_سېټ = cur_ff.pol_سېټ;
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
				إذا (ff.pol_سېټ != cur_ff.pol_سېټ)
					أرجع خطأ;
			}
		}

		ff.پلنوالی++;
		ff.sig_d.append(cur_ff.sig_d[idx]);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_سېټ.append(ff.has_sr ? cur_ff.sig_سېټ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		وموندل = ریښتیا;
	}

	أرجع وموندل;
}

منطقي Ff_یوځای_Helper::پیدا_کړه_ننوت_ff(RTLIL::SigSpec sig, Ff_معلومات &ff, pool<std::pair<Cell *, صحيح>> &بټونه) {
	ff = Ff_معلومات(module, initvals, module->twines().جمع_کړه(NEW_ID));
	sigmap->apply(sig);

	منطقي وموندل = خطأ;

	pool<صحيح> const_بټونه;

	لكل (خپلکاری بټ : sig)
	{
		إذا (بټ.wire == عدم) {
			const_بټونه.ورننباسه(ff.پلنوالی);
			ff.پلنوالی++;
			ff.sig_q.append(بټ);
			ff.sig_d.append(بټ);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_سېټ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(بټ.معلومات));
			ff.val_srst.append(RTLIL::Const(بټ.معلومات));
			ff.val_arst.append(RTLIL::Const(بټ.معلومات));
			استمر;
		}

		إذا (!dff_driver.شمېر(بټ))
			أرجع خطأ;

		Cell *cell;
		صحيح idx;
		std::tie(cell, idx) = dff_driver[بټ];
		بټونه.ورننباسه(std::make_pair(cell, idx));

		Ff_معلومات cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == بټ);

		إذا (!وموندل) {
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
			ff.pol_سېټ = cur_ff.pol_سېټ;
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
				إذا (ff.pol_سېټ != cur_ff.pol_سېټ)
					أرجع خطأ;
			}
		}

		ff.پلنوالی++;
		ff.sig_d.append((ff.has_clk || ff.has_gclk) ? cur_ff.sig_d[idx] : State::Sx);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_سېټ.append(ff.has_sr ? cur_ff.sig_سېټ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		وموندل = ریښتیا;
	}

	إذا (وموندل && ff.has_sr) {
		لكل (خپلکاری i: const_بټونه) {
			إذا (ff.sig_d[i] == State::S0) {
				ff.sig_سېټ[i] = ff.pol_سېټ ? State::S0 : State::S1;
			} وإلا إذا (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	أرجع وموندل;
}


فراغ Ff_یوځای_Helper::remove_output_ff(ثابت pool<std::pair<Cell *, صحيح>> &بټونه) {
	لكل (خپلکاری &it : بټونه) {
		Cell *cell = it.لومړی;
		صحيح idx = it.دوهم;
		SigSpec q = cell->get_پورټ(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->جمع_کړه_Wire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->سېټ_پورټ(ID::Q, q);
		initvals->سېټ_init(cell->get_پورټ(ID::Q), (*initvals)(q));
	}
}

فراغ Ff_یوځای_Helper::mark_ننوت_ff(ثابت pool<std::pair<Cell *, صحيح>> &بټونه) {
	لكل (خپلکاری &it : بټونه) {
		Cell *cell = it.لومړی;
		صحيح idx = it.دوهم;
		إذا (cell->has_پورټ(ID::D)) {
			SigSpec d = cell->get_پورټ(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_شمېر[d[idx]]++;
		}
	}
}

فراغ Ff_یوځای_Helper::سېټ(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	لكل (خپلکاری wire : module->wires()) {
		إذا (wire->پورټ_output)
			لكل (خپلکاری بټ : (*sigmap)(wire))
				sigbit_users_شمېر[بټ]++;
	}

	لكل (خپلکاری cell : module->cells()) {
		إذا (cell->is_builtin_ff()) {
			إذا (cell->has_پورټ(ID::D)) {
				SigSpec d = (*sigmap)(cell->get_پورټ(ID::D));
				لكل (صحيح i = 0; i < Get_اندازه(d); i++)
					dff_sink[d[i]].ورننباسه(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->get_پورټ(ID::Q));
			لكل (صحيح i = 0; i < Get_اندازه(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		لكل (خپلکاری &conn : cell->connections())
			إذا (!cell->معلوم() || cell->ننوت(conn.لومړی))
				لكل (خپلکاری بټ : (*sigmap)(conn.دوهم))
					sigbit_users_شمېر[بټ]++;
	}
}

فراغ Ff_یوځای_Helper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_شمېر.clear();
}
