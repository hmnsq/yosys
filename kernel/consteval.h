/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#ifndef CONSTEVAL_H
#define CONSTEVAL_H

#അന്തര്ഭാവ "kernel/rtlil.h"
#അന്തര്ഭാവ "kernel/sigtools.h"
#അന്തര്ഭാവ "kernel/celltypes.h"
#അന്തര്ഭാവ "kernel/macc.h"
#അന്തര്ഭാവ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
ഘടന ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap മൂല്യങ്ങൾ_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> അട്ടി;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		വേണ്ടി (auto &it : module->cells_) {
			എങ്കിൽ (!ct.cell_known(it.second->type))
				തുടരുക;
			വേണ്ടി (auto &it2 : it.second->connections())
				എങ്കിൽ (ct.cell_output(it.second->type, it2.first))
					sig2driver.തിരുകുക(assign_map(it2.second), it.second);
		}
	}

	ശൂന്യം clear()
	{
		മൂല്യങ്ങൾ_map.clear();
		stop_signals.clear();
	}

	ശൂന്യം തള്ളുക()
	{
		അട്ടി.തള്ളുക_back(മൂല്യങ്ങൾ_map);
	}

	ശൂന്യം എടുക്കുക()
	{
		മൂല്യങ്ങൾ_map.swap(അട്ടി.back());
		അട്ടി.എടുക്കുക_back();
	}

	ശൂന്യം set(RTLIL::SigSpec sig, RTLIL::Const മൂല്യം)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = മൂല്യങ്ങൾ_map(sig);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != ശൂന്യ || current_val[i] == മൂല്യം[i]);
#endif
		മൂല്യങ്ങൾ_map.add(sig, RTLIL::SigSpec(മൂല്യം));
	}

	ശൂന്യം stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		എങ്കിൽ (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = മൂല്യങ്ങൾ_map(assign_map(cell->getPort(ID::CO)));

			എങ്കിൽ (sig_co.is_fully_const())
				തിരികെ true;

			എങ്കിൽ (!eval(sig_p, undef, cell))
				തിരികെ false;

			എങ്കിൽ (!eval(sig_g, undef, cell))
				തിരികെ false;

			എങ്കിൽ (!eval(sig_ci, undef, cell))
				തിരികെ false;

			എങ്കിൽ (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			അല്ലെങ്കിൽ
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			തിരികെ true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = മൂല്യങ്ങൾ_map(assign_map(cell->getPort(ID::Y)));
		എങ്കിൽ (sig_y.is_fully_const())
			തിരികെ true;

		എങ്കിൽ (cell->type == ID($barrier))
		{
			എങ്കിൽ (!model_barriers) {
				undef.append(sig_y);
				തിരികെ false;
			}
			sig_a = cell->getPort(ID::A);
			എങ്കിൽ (!eval(sig_a, undef, cell))
				തിരികെ false;
			set(sig_y, sig_a.as_const());
			തിരികെ true;
		}

		എങ്കിൽ (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		എങ്കിൽ (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		എങ്കിൽ (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		എങ്കിൽ (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			പൂർണ്ണസംഖ്യ എണ്ണം_set_s_bits = 0;

			എങ്കിൽ (!eval(sig_s, undef, cell))
				തിരികെ false;

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				എങ്കിൽ (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.തള്ളുക_back(b_slice);

				എങ്കിൽ (s_bit == RTLIL::State::S1)
					എണ്ണം_set_s_bits++;
			}

			എങ്കിൽ (എണ്ണം_set_s_bits == 0)
				y_candidates.തള്ളുക_back(sig_a);

			std::vector<RTLIL::Const> y_മൂല്യങ്ങൾ;

			log_assert(y_candidates.size() > 0);
			വേണ്ടി (auto &yc : y_candidates) {
				എങ്കിൽ (!eval(yc, undef, cell))
					തിരികെ false;
				എങ്കിൽ (cell->type == ID($_NMUX_))
					y_മൂല്യങ്ങൾ.തള്ളുക_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				അല്ലെങ്കിൽ
					y_മൂല്യങ്ങൾ.തള്ളുക_back(yc.as_const());
			}

			എങ്കിൽ (y_മൂല്യങ്ങൾ.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_മൂല്യങ്ങൾ.at(0).to_bits();

				വേണ്ടി (size_t i = 1; i < y_മൂല്യങ്ങൾ.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_മൂല്യങ്ങൾ.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					വേണ്ടി (size_t j = 0; j < master_bits.size(); j++)
						എങ്കിൽ (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			അല്ലെങ്കിൽ
				set(sig_y, y_മൂല്യങ്ങൾ.front());
		}
		അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($bmux))
		{
			എങ്കിൽ (!eval(sig_s, undef, cell))
				തിരികെ false;

			എങ്കിൽ (sig_s.is_fully_def()) {
				പൂർണ്ണസംഖ്യ sel = sig_s.as_int();
				പൂർണ്ണസംഖ്യ width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				എങ്കിൽ (!eval(res, undef, cell))
					തിരികെ false;
				set(sig_y, res.as_const());
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (!eval(sig_a, undef, cell))
					തിരികെ false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($demux))
		{
			എങ്കിൽ (!eval(sig_a, undef, cell))
				തിരികെ false;
			എങ്കിൽ (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} അല്ലെങ്കിൽ {
				എങ്കിൽ (!eval(sig_s, undef, cell))
					തിരികെ false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			പൂർണ്ണസംഖ്യ width = GetSize(sig_c);

			എങ്കിൽ (!eval(sig_a, undef, cell))
				തിരികെ false;

			എങ്കിൽ (!eval(sig_b, undef, cell))
				തിരികെ false;

			എങ്കിൽ (!eval(sig_c, undef, cell))
				തിരികെ false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(val_y); i++)
				എങ്കിൽ (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.എണ്ണം(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.എണ്ണം(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			എങ്കിൽ (!eval(sig_a, undef, cell))
				തിരികെ false;

			എങ്കിൽ (!eval(sig_b, undef, cell))
				തിരികെ false;

			എങ്കിൽ (!eval(sig_ci, undef, cell))
				തിരികെ false;

			എങ്കിൽ (!eval(sig_bi, undef, cell))
				തിരികെ false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_നിവേശം_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				എങ്കിൽ (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} അല്ലെങ്കിൽ {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				എങ്കിൽ (any_നിവേശം_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} അല്ലെങ്കിൽ {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			വേണ്ടി (auto &port : macc.terms) {
				എങ്കിൽ (!eval(port.in_a, undef, cell))
					തിരികെ false;
				എങ്കിൽ (!eval(port.in_b, undef, cell))
					തിരികെ false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			എങ്കിൽ (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		അല്ലെങ്കിൽ
		{
			RTLIL::SigSpec sig_c, sig_d;

			എങ്കിൽ (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				എങ്കിൽ (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				എങ്കിൽ (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			എങ്കിൽ (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				തിരികെ false;
			എങ്കിൽ (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				തിരികെ false;
			എങ്കിൽ (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				തിരികെ false;
			എങ്കിൽ (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				തിരികെ false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			എങ്കിൽ (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} അല്ലെങ്കിൽ
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			എങ്കിൽ (eval_err)
				തിരികെ false;

			set(sig_y, eval_ret);
		}

		തിരികെ true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = ശൂന്യ)
	{
		assign_map.apply(sig);
		മൂല്യങ്ങൾ_map.apply(sig);

		എങ്കിൽ (sig.is_fully_const())
			തിരികെ true;

		എങ്കിൽ (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			തിരികെ false;
		}

		എങ്കിൽ (busy_cell) {
			എങ്കിൽ (busy.എണ്ണം(busy_cell) > 0) {
				undef = sig;
				തിരികെ false;
			}
			busy.തിരുകുക(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		വേണ്ടി (auto cell : driver_cells) {
			എങ്കിൽ (!eval(cell, undef)) {
				എങ്കിൽ (busy_cell)
					busy.erase(busy_cell);
				തിരികെ false;
			}
		}

		എങ്കിൽ (busy_cell)
			busy.erase(busy_cell);

		മൂല്യങ്ങൾ_map.apply(sig);
		എങ്കിൽ (sig.is_fully_const())
			തിരികെ true;

		എങ്കിൽ (defaultval != RTLIL::State::Sm) {
			വേണ്ടി (auto &bit : sig)
				എങ്കിൽ (bit.wire) bit = defaultval;
			തിരികെ true;
		}

		വേണ്ടി (auto &c : sig.chunks())
			എങ്കിൽ (c.wire != ശൂന്യ)
				undef.append(c);
		തിരികെ false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		തിരികെ eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
