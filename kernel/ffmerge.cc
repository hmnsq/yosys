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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

ਤਾਰਕਿਕ Ff_ਮਿਲਾਓ_Helper::is_output_ਅਣਵਰਤਿਆ(RTLIL::SigSpec sig) {
	ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : (*sigmap)(sig))
		ਜੇ (sigbit_users_ਗਿਣਤੀ[ਦੋਆਧਾਰੀ_ਅੰਕ] != 0)
			ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ Ff_ਮਿਲਾਓ_Helper::ਲੱਭੋ_output_ff(RTLIL::SigSpec sig, Ff_ਅੰਕੜੇ &ff, pool<std::pair<Cell *, ਪੂਰਨ>> &ਬਿੱਟ) {
	ff = Ff_ਅੰਕੜੇ(module, initvals, module->twines().ਜੋੜੋ(NEW_ID));
	sigmap->apply(sig);

	ਤਾਰਕਿਕ ਮਿਲਿਆ = ਝੂਠ;

	ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
	{
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire == ਨਿਰਾ || sigbit_users_ਗਿਣਤੀ[ਦੋਆਧਾਰੀ_ਅੰਕ] == 0) {
			ff.ਚੌੜਾਈ++;
			ff.sig_q.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
			ff.sig_d.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
			ff.sig_clr.append(State::Sx);
			ff.sig_ਸੈੱਟ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			ਜਾਰੀ;
		}

		ਜੇ (sigbit_users_ਗਿਣਤੀ[ਦੋਆਧਾਰੀ_ਅੰਕ] != 1)
			ਵਾਪਸ ਝੂਠ;

		ਸਵੈਚਾਲਿਤ &sinks = dff_sink[ਦੋਆਧਾਰੀ_ਅੰਕ];
		ਜੇ (sinks.ਮਾਪ() != 1)
			ਵਾਪਸ ਝੂਠ;

		Cell *cell;
		ਪੂਰਨ idx;
		std::tie(cell, idx) = *sinks.begin();
		ਬਿੱਟ.ਪਾਓ(std::make_pair(cell, idx));

		Ff_ਅੰਕੜੇ cur_ff(initvals, cell);

		// Reject latches and $ff.
		ਜੇ (!cur_ff.has_clk)
			ਵਾਪਸ ਝੂਠ;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == ਦੋਆਧਾਰੀ_ਅੰਕ);

		ਜੇ (!ਮਿਲਿਆ) {
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
			ff.pol_ਸੈੱਟ = cur_ff.pol_ਸੈੱਟ;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (ff.has_clk != cur_ff.has_clk)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_ce != cur_ff.has_ce)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_aload != cur_ff.has_aload)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_srst != cur_ff.has_srst)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_arst != cur_ff.has_arst)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_sr != cur_ff.has_sr)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_clk) {
				ਜੇ (ff.sig_clk != cur_ff.sig_clk)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_clk != cur_ff.pol_clk)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_ce) {
				ਜੇ (ff.sig_ce != cur_ff.sig_ce)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_ce != cur_ff.pol_ce)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_aload) {
				ਜੇ (ff.sig_aload != cur_ff.sig_aload)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_aload != cur_ff.pol_aload)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_srst) {
				ਜੇ (ff.sig_srst != cur_ff.sig_srst)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_srst != cur_ff.pol_srst)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_arst) {
				ਜੇ (ff.sig_arst != cur_ff.sig_arst)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_arst != cur_ff.pol_arst)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_sr) {
				ਜੇ (ff.pol_clr != cur_ff.pol_clr)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_ਸੈੱਟ != cur_ff.pol_ਸੈੱਟ)
					ਵਾਪਸ ਝੂਠ;
			}
		}

		ff.ਚੌੜਾਈ++;
		ff.sig_d.append(cur_ff.sig_d[idx]);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_ਸੈੱਟ.append(ff.has_sr ? cur_ff.sig_ਸੈੱਟ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		ਮਿਲਿਆ = ਸੱਚ;
	}

	ਵਾਪਸ ਮਿਲਿਆ;
}

ਤਾਰਕਿਕ Ff_ਮਿਲਾਓ_Helper::ਲੱਭੋ_ਨਿਵੇਸ਼_ff(RTLIL::SigSpec sig, Ff_ਅੰਕੜੇ &ff, pool<std::pair<Cell *, ਪੂਰਨ>> &ਬਿੱਟ) {
	ff = Ff_ਅੰਕੜੇ(module, initvals, module->twines().ਜੋੜੋ(NEW_ID));
	sigmap->apply(sig);

	ਤਾਰਕਿਕ ਮਿਲਿਆ = ਝੂਠ;

	pool<ਪੂਰਨ> const_ਬਿੱਟ;

	ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
	{
		ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire == ਨਿਰਾ) {
			const_ਬਿੱਟ.ਪਾਓ(ff.ਚੌੜਾਈ);
			ff.ਚੌੜਾਈ++;
			ff.sig_q.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
			ff.sig_d.append(ਦੋਆਧਾਰੀ_ਅੰਕ);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_ਸੈੱਟ.append(State::Sx);
			ff.val_init.append(RTLIL::Const(ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ));
			ff.val_srst.append(RTLIL::Const(ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ));
			ff.val_arst.append(RTLIL::Const(ਦੋਆਧਾਰੀ_ਅੰਕ.ਅੰਕੜੇ));
			ਜਾਰੀ;
		}

		ਜੇ (!dff_driver.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ))
			ਵਾਪਸ ਝੂਠ;

		Cell *cell;
		ਪੂਰਨ idx;
		std::tie(cell, idx) = dff_driver[ਦੋਆਧਾਰੀ_ਅੰਕ];
		ਬਿੱਟ.ਪਾਓ(std::make_pair(cell, idx));

		Ff_ਅੰਕੜੇ cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == ਦੋਆਧਾਰੀ_ਅੰਕ);

		ਜੇ (!ਮਿਲਿਆ) {
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
			ff.pol_ਸੈੱਟ = cur_ff.pol_ਸੈੱਟ;
		} ਨਹੀਂਤਾਂ {
			ਜੇ (ff.has_gclk != cur_ff.has_gclk)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_clk != cur_ff.has_clk)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_ce != cur_ff.has_ce)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_aload != cur_ff.has_aload)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_srst != cur_ff.has_srst)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_arst != cur_ff.has_arst)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_sr != cur_ff.has_sr)
				ਵਾਪਸ ਝੂਠ;
			ਜੇ (ff.has_clk) {
				ਜੇ (ff.sig_clk != cur_ff.sig_clk)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_clk != cur_ff.pol_clk)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_ce) {
				ਜੇ (ff.sig_ce != cur_ff.sig_ce)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_ce != cur_ff.pol_ce)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_aload) {
				ਜੇ (ff.sig_aload != cur_ff.sig_aload)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_aload != cur_ff.pol_aload)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_srst) {
				ਜੇ (ff.sig_srst != cur_ff.sig_srst)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_srst != cur_ff.pol_srst)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_arst) {
				ਜੇ (ff.sig_arst != cur_ff.sig_arst)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_arst != cur_ff.pol_arst)
					ਵਾਪਸ ਝੂਠ;
			}
			ਜੇ (ff.has_sr) {
				ਜੇ (ff.pol_clr != cur_ff.pol_clr)
					ਵਾਪਸ ਝੂਠ;
				ਜੇ (ff.pol_ਸੈੱਟ != cur_ff.pol_ਸੈੱਟ)
					ਵਾਪਸ ਝੂਠ;
			}
		}

		ff.ਚੌੜਾਈ++;
		ff.sig_d.append((ff.has_clk || ff.has_gclk) ? cur_ff.sig_d[idx] : State::Sx);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_ਸੈੱਟ.append(ff.has_sr ? cur_ff.sig_ਸੈੱਟ[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		ਮਿਲਿਆ = ਸੱਚ;
	}

	ਜੇ (ਮਿਲਿਆ && ff.has_sr) {
		ਲਈ (ਸਵੈਚਾਲਿਤ i: const_ਬਿੱਟ) {
			ਜੇ (ff.sig_d[i] == State::S0) {
				ff.sig_ਸੈੱਟ[i] = ff.pol_ਸੈੱਟ ? State::S0 : State::S1;
			} ਨਹੀਂਤਾਂ ਜੇ (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	ਵਾਪਸ ਮਿਲਿਆ;
}


ਖਾਲੀ Ff_ਮਿਲਾਓ_Helper::remove_output_ff(ਅਚਲ pool<std::pair<Cell *, ਪੂਰਨ>> &ਬਿੱਟ) {
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : ਬਿੱਟ) {
		Cell *cell = it.ਪਹਿਲਾ;
		ਪੂਰਨ idx = it.ਦੂਜਾ;
		SigSpec q = cell->get_ਪੋਰਟ(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->ਜੋੜੋ_Wire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->ਸੈੱਟ_ਪੋਰਟ(ID::Q, q);
		initvals->ਸੈੱਟ_init(cell->get_ਪੋਰਟ(ID::Q), (*initvals)(q));
	}
}

ਖਾਲੀ Ff_ਮਿਲਾਓ_Helper::mark_ਨਿਵੇਸ਼_ff(ਅਚਲ pool<std::pair<Cell *, ਪੂਰਨ>> &ਬਿੱਟ) {
	ਲਈ (ਸਵੈਚਾਲਿਤ &it : ਬਿੱਟ) {
		Cell *cell = it.ਪਹਿਲਾ;
		ਪੂਰਨ idx = it.ਦੂਜਾ;
		ਜੇ (cell->has_ਪੋਰਟ(ID::D)) {
			SigSpec d = cell->get_ਪੋਰਟ(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_ਗਿਣਤੀ[d[idx]]++;
		}
	}
}

ਖਾਲੀ Ff_ਮਿਲਾਓ_Helper::ਸੈੱਟ(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	ਲਈ (ਸਵੈਚਾਲਿਤ wire : module->wires()) {
		ਜੇ (wire->ਪੋਰਟ_output)
			ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : (*sigmap)(wire))
				sigbit_users_ਗਿਣਤੀ[ਦੋਆਧਾਰੀ_ਅੰਕ]++;
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ cell : module->cells()) {
		ਜੇ (cell->is_builtin_ff()) {
			ਜੇ (cell->has_ਪੋਰਟ(ID::D)) {
				SigSpec d = (*sigmap)(cell->get_ਪੋਰਟ(ID::D));
				ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(d); i++)
					dff_sink[d[i]].ਪਾਓ(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->get_ਪੋਰਟ(ID::Q));
			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
			ਜੇ (!cell->ਗਿਆਤ() || cell->ਨਿਵੇਸ਼(conn.ਪਹਿਲਾ))
				ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : (*sigmap)(conn.ਦੂਜਾ))
					sigbit_users_ਗਿਣਤੀ[ਦੋਆਧਾਰੀ_ਅੰਕ]++;
	}
}

ਖਾਲੀ Ff_ਮਿਲਾਓ_Helper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_ਗਿਣਤੀ.clear();
}
