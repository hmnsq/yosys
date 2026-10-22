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

#अन्तर्भाव "kernel/ffmerge.h"

USING_YOSYS_NAMESPACE

तार्किक FfMergeHelper::is_output_अप्रयुक्तम्(RTLIL::SigSpec sig) {
	प्रति (स्वचालित bit : (*sigmap)(sig))
		यदि (sigbit_users_गणनम्[bit] != 0)
			प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

तार्किक FfMergeHelper::find_output_ff(RTLIL::SigSpec sig, Ff_दत्त &ff, pool<std::pair<Cell *, पूर्णाङ्क>> &द्वयङ्काः) {
	ff = Ff_दत्त(module, initvals, module->twines().सङ्कलय(NEW_ID));
	sigmap->apply(sig);

	तार्किक प्राप्तम् = असत्य;

	प्रति (स्वचालित bit : sig)
	{
		यदि (bit.wire == रिक्त || sigbit_users_गणनम्[bit] == 0) {
			ff.विस्तारः++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			ff.sig_clr.append(State::Sx);
			ff.sig_समुच्चयः.append(State::Sx);
			ff.val_init.append(RTLIL::Const(State::Sx));
			ff.val_srst.append(RTLIL::Const(State::Sx));
			ff.val_arst.append(RTLIL::Const(State::Sx));
			अनुवर्त;
		}

		यदि (sigbit_users_गणनम्[bit] != 1)
			प्रत्यागम असत्य;

		स्वचालित &sinks = dff_sink[bit];
		यदि (sinks.आकार() != 1)
			प्रत्यागम असत्य;

		Cell *cell;
		पूर्णाङ्क idx;
		std::tie(cell, idx) = *sinks.begin();
		द्वयङ्काः.निवेशय(std::make_pair(cell, idx));

		Ff_दत्त cur_ff(initvals, cell);

		// Reject latches and $ff.
		यदि (!cur_ff.has_clk)
			प्रत्यागम असत्य;

		log_assert((*sigmap)(cur_ff.sig_d[idx]) == bit);

		यदि (!प्राप्तम्) {
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
			ff.pol_समुच्चयः = cur_ff.pol_समुच्चयः;
		} अन्यथा {
			यदि (ff.has_clk != cur_ff.has_clk)
				प्रत्यागम असत्य;
			यदि (ff.has_ce != cur_ff.has_ce)
				प्रत्यागम असत्य;
			यदि (ff.has_aload != cur_ff.has_aload)
				प्रत्यागम असत्य;
			यदि (ff.has_srst != cur_ff.has_srst)
				प्रत्यागम असत्य;
			यदि (ff.has_arst != cur_ff.has_arst)
				प्रत्यागम असत्य;
			यदि (ff.has_sr != cur_ff.has_sr)
				प्रत्यागम असत्य;
			यदि (ff.has_clk) {
				यदि (ff.sig_clk != cur_ff.sig_clk)
					प्रत्यागम असत्य;
				यदि (ff.pol_clk != cur_ff.pol_clk)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_ce) {
				यदि (ff.sig_ce != cur_ff.sig_ce)
					प्रत्यागम असत्य;
				यदि (ff.pol_ce != cur_ff.pol_ce)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_aload) {
				यदि (ff.sig_aload != cur_ff.sig_aload)
					प्रत्यागम असत्य;
				यदि (ff.pol_aload != cur_ff.pol_aload)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_srst) {
				यदि (ff.sig_srst != cur_ff.sig_srst)
					प्रत्यागम असत्य;
				यदि (ff.pol_srst != cur_ff.pol_srst)
					प्रत्यागम असत्य;
				यदि (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_arst) {
				यदि (ff.sig_arst != cur_ff.sig_arst)
					प्रत्यागम असत्य;
				यदि (ff.pol_arst != cur_ff.pol_arst)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_sr) {
				यदि (ff.pol_clr != cur_ff.pol_clr)
					प्रत्यागम असत्य;
				यदि (ff.pol_समुच्चयः != cur_ff.pol_समुच्चयः)
					प्रत्यागम असत्य;
			}
		}

		ff.विस्तारः++;
		ff.sig_d.append(cur_ff.sig_d[idx]);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_समुच्चयः.append(ff.has_sr ? cur_ff.sig_समुच्चयः[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		प्राप्तम् = सत्य;
	}

	प्रत्यागम प्राप्तम्;
}

तार्किक FfMergeHelper::find_निवेशः_ff(RTLIL::SigSpec sig, Ff_दत्त &ff, pool<std::pair<Cell *, पूर्णाङ्क>> &द्वयङ्काः) {
	ff = Ff_दत्त(module, initvals, module->twines().सङ्कलय(NEW_ID));
	sigmap->apply(sig);

	तार्किक प्राप्तम् = असत्य;

	pool<पूर्णाङ्क> const_द्वयङ्काः;

	प्रति (स्वचालित bit : sig)
	{
		यदि (bit.wire == रिक्त) {
			const_द्वयङ्काः.निवेशय(ff.विस्तारः);
			ff.विस्तारः++;
			ff.sig_q.append(bit);
			ff.sig_d.append(bit);
			// These two will be fixed up later.
			ff.sig_clr.append(State::Sx);
			ff.sig_समुच्चयः.append(State::Sx);
			ff.val_init.append(RTLIL::Const(bit.दत्त));
			ff.val_srst.append(RTLIL::Const(bit.दत्त));
			ff.val_arst.append(RTLIL::Const(bit.दत्त));
			अनुवर्त;
		}

		यदि (!dff_driver.गणनम्(bit))
			प्रत्यागम असत्य;

		Cell *cell;
		पूर्णाङ्क idx;
		std::tie(cell, idx) = dff_driver[bit];
		द्वयङ्काः.निवेशय(std::make_pair(cell, idx));

		Ff_दत्त cur_ff(initvals, cell);

		log_assert((*sigmap)(cur_ff.sig_q[idx]) == bit);

		यदि (!प्राप्तम्) {
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
			ff.pol_समुच्चयः = cur_ff.pol_समुच्चयः;
		} अन्यथा {
			यदि (ff.has_gclk != cur_ff.has_gclk)
				प्रत्यागम असत्य;
			यदि (ff.has_clk != cur_ff.has_clk)
				प्रत्यागम असत्य;
			यदि (ff.has_ce != cur_ff.has_ce)
				प्रत्यागम असत्य;
			यदि (ff.has_aload != cur_ff.has_aload)
				प्रत्यागम असत्य;
			यदि (ff.has_srst != cur_ff.has_srst)
				प्रत्यागम असत्य;
			यदि (ff.has_arst != cur_ff.has_arst)
				प्रत्यागम असत्य;
			यदि (ff.has_sr != cur_ff.has_sr)
				प्रत्यागम असत्य;
			यदि (ff.has_clk) {
				यदि (ff.sig_clk != cur_ff.sig_clk)
					प्रत्यागम असत्य;
				यदि (ff.pol_clk != cur_ff.pol_clk)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_ce) {
				यदि (ff.sig_ce != cur_ff.sig_ce)
					प्रत्यागम असत्य;
				यदि (ff.pol_ce != cur_ff.pol_ce)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_aload) {
				यदि (ff.sig_aload != cur_ff.sig_aload)
					प्रत्यागम असत्य;
				यदि (ff.pol_aload != cur_ff.pol_aload)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_srst) {
				यदि (ff.sig_srst != cur_ff.sig_srst)
					प्रत्यागम असत्य;
				यदि (ff.pol_srst != cur_ff.pol_srst)
					प्रत्यागम असत्य;
				यदि (ff.has_ce && ff.ce_over_srst != cur_ff.ce_over_srst)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_arst) {
				यदि (ff.sig_arst != cur_ff.sig_arst)
					प्रत्यागम असत्य;
				यदि (ff.pol_arst != cur_ff.pol_arst)
					प्रत्यागम असत्य;
			}
			यदि (ff.has_sr) {
				यदि (ff.pol_clr != cur_ff.pol_clr)
					प्रत्यागम असत्य;
				यदि (ff.pol_समुच्चयः != cur_ff.pol_समुच्चयः)
					प्रत्यागम असत्य;
			}
		}

		ff.विस्तारः++;
		ff.sig_d.append((ff.has_clk || ff.has_gclk) ? cur_ff.sig_d[idx] : State::Sx);
		ff.sig_ad.append(ff.has_aload ? cur_ff.sig_ad[idx] : State::Sx);
		ff.sig_q.append(cur_ff.sig_q[idx]);
		ff.sig_clr.append(ff.has_sr ? cur_ff.sig_clr[idx] : State::S0);
		ff.sig_समुच्चयः.append(ff.has_sr ? cur_ff.sig_समुच्चयः[idx] : State::S0);
		ff.val_arst.append(RTLIL::Const(ff.has_arst ? cur_ff.val_arst[idx] : State::Sx));
		ff.val_srst.append(RTLIL::Const(ff.has_srst ? cur_ff.val_srst[idx] : State::Sx));
		ff.val_init.append(RTLIL::Const(cur_ff.val_init[idx]));
		प्राप्तम् = सत्य;
	}

	यदि (प्राप्तम् && ff.has_sr) {
		प्रति (स्वचालित i: const_द्वयङ्काः) {
			यदि (ff.sig_d[i] == State::S0) {
				ff.sig_समुच्चयः[i] = ff.pol_समुच्चयः ? State::S0 : State::S1;
			} अन्यथा यदि (ff.sig_d[i] == State::S1) {
				ff.sig_clr[i] = ff.pol_clr ? State::S0 : State::S1;
			}
		}
	}

	प्रत्यागम प्राप्तम्;
}


शून्य FfMergeHelper::remove_output_ff(ध्रुव pool<std::pair<Cell *, पूर्णाङ्क>> &द्वयङ्काः) {
	प्रति (स्वचालित &it : द्वयङ्काः) {
		Cell *cell = it.first;
		पूर्णाङ्क idx = it.second;
		SigSpec q = cell->get_पत्तनम्(ID::Q);
		initvals->remove_init(q[idx]);
		dff_driver.erase((*sigmap)(q[idx]));
		q[idx] = module->सङ्कलय_Wire(stringf("$ffmerge_disconnected$%d", autoidx++));
		cell->समुच्चयः_पत्तनम्(ID::Q, q);
		initvals->समुच्चयः_init(cell->get_पत्तनम्(ID::Q), (*initvals)(q));
	}
}

शून्य FfMergeHelper::mark_निवेशः_ff(ध्रुव pool<std::pair<Cell *, पूर्णाङ्क>> &द्वयङ्काः) {
	प्रति (स्वचालित &it : द्वयङ्काः) {
		Cell *cell = it.first;
		पूर्णाङ्क idx = it.second;
		यदि (cell->has_पत्तनम्(ID::D)) {
			SigSpec d = cell->get_पत्तनम्(ID::D);
			// The user count was already at least 1
			// (for the D port).  Bump it as it is now connected
			// to the merged-to cell as well.  This suffices for
			// it to not be considered for output merging.
			sigbit_users_गणनम्[d[idx]]++;
		}
	}
}

शून्य FfMergeHelper::समुच्चयः(FfInitVals *initvals_, RTLIL::Module *module_)
{
	clear();
	initvals = initvals_;
	sigmap = initvals->sigmap;
	module = module_;

	प्रति (स्वचालित wire : module->wires()) {
		यदि (wire->पत्तनम्_output)
			प्रति (स्वचालित bit : (*sigmap)(wire))
				sigbit_users_गणनम्[bit]++;
	}

	प्रति (स्वचालित cell : module->cells()) {
		यदि (cell->is_builtin_ff()) {
			यदि (cell->has_पत्तनम्(ID::D)) {
				SigSpec d = (*sigmap)(cell->get_पत्तनम्(ID::D));
				प्रति (पूर्णाङ्क i = 0; i < Get_आकार(d); i++)
					dff_sink[d[i]].निवेशय(std::make_pair(cell, i));
			}
			SigSpec q = (*sigmap)(cell->get_पत्तनम्(ID::Q));
			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(q); i++)
				dff_driver[q[i]] = std::make_pair(cell, i);
		}
		प्रति (स्वचालित &conn : cell->connections())
			यदि (!cell->ज्ञातम्() || cell->निवेशः(conn.first))
				प्रति (स्वचालित bit : (*sigmap)(conn.second))
					sigbit_users_गणनम्[bit]++;
	}
}

शून्य FfMergeHelper::clear() {
	dff_driver.clear();
	dff_sink.clear();
	sigbit_users_गणनम्.clear();
}
