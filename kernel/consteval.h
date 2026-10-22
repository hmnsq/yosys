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

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/celltypes.h"
#अन्तर्भाव "kernel/macc.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
संरचना ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap मानानि_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> राशिः;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		अत्थाय (auto &it : module->cells_) {
			सचे (!ct.cell_known(it.second->type))
				अनुवर्त;
			अत्थाय (auto &it2 : it.second->connections())
				सचे (ct.cell_output(it.second->type, it2.first))
					sig2driver.निवेशय(assign_map(it2.second), it.second);
		}
	}

	शून्य clear()
	{
		मानानि_map.clear();
		stop_signals.clear();
	}

	शून्य प्रक्षिप()
	{
		राशिः.प्रक्षिप_back(मानानि_map);
	}

	शून्य निष्कासय()
	{
		मानानि_map.swap(राशिः.back());
		राशिः.निष्कासय_back();
	}

	शून्य set(RTLIL::SigSpec sig, RTLIL::Const मानम्)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = मानानि_map(sig);
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != रिक्त || current_val[i] == मानम्[i]);
#endif
		मानानि_map.add(sig, RTLIL::SigSpec(मानम्));
	}

	शून्य stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		सचे (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = मानानि_map(assign_map(cell->getPort(ID::CO)));

			सचे (sig_co.is_fully_const())
				पटिनिवत्त true;

			सचे (!eval(sig_p, undef, cell))
				पटिनिवत्त false;

			सचे (!eval(sig_g, undef, cell))
				पटिनिवत्त false;

			सचे (!eval(sig_ci, undef, cell))
				पटिनिवत्त false;

			सचे (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				अत्थाय (पूर्णाङ्क i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			अञ्ञथा
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			पटिनिवत्त true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = मानानि_map(assign_map(cell->getPort(ID::Y)));
		सचे (sig_y.is_fully_const())
			पटिनिवत्त true;

		सचे (cell->type == ID($barrier))
		{
			सचे (!model_barriers) {
				undef.append(sig_y);
				पटिनिवत्त false;
			}
			sig_a = cell->getPort(ID::A);
			सचे (!eval(sig_a, undef, cell))
				पटिनिवत्त false;
			set(sig_y, sig_a.as_const());
			पटिनिवत्त true;
		}

		सचे (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		सचे (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		सचे (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		सचे (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			पूर्णाङ्क गणनम्_set_s_bits = 0;

			सचे (!eval(sig_s, undef, cell))
				पटिनिवत्त false;

			अत्थाय (पूर्णाङ्क i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				सचे (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.प्रक्षिप_back(b_slice);

				सचे (s_bit == RTLIL::State::S1)
					गणनम्_set_s_bits++;
			}

			सचे (गणनम्_set_s_bits == 0)
				y_candidates.प्रक्षिप_back(sig_a);

			std::vector<RTLIL::Const> y_मानानि;

			log_assert(y_candidates.size() > 0);
			अत्थाय (auto &yc : y_candidates) {
				सचे (!eval(yc, undef, cell))
					पटिनिवत्त false;
				सचे (cell->type == ID($_NMUX_))
					y_मानानि.प्रक्षिप_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				अञ्ञथा
					y_मानानि.प्रक्षिप_back(yc.as_const());
			}

			सचे (y_मानानि.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_मानानि.at(0).to_bits();

				अत्थाय (size_t i = 1; i < y_मानानि.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_मानानि.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					अत्थाय (size_t j = 0; j < master_bits.size(); j++)
						सचे (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			अञ्ञथा
				set(sig_y, y_मानानि.front());
		}
		अञ्ञथा सचे (cell->type == ID($bmux))
		{
			सचे (!eval(sig_s, undef, cell))
				पटिनिवत्त false;

			सचे (sig_s.is_fully_def()) {
				पूर्णाङ्क sel = sig_s.as_int();
				पूर्णाङ्क width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				सचे (!eval(res, undef, cell))
					पटिनिवत्त false;
				set(sig_y, res.as_const());
			} अञ्ञथा {
				सचे (!eval(sig_a, undef, cell))
					पटिनिवत्त false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अञ्ञथा सचे (cell->type == ID($demux))
		{
			सचे (!eval(sig_a, undef, cell))
				पटिनिवत्त false;
			सचे (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} अञ्ञथा {
				सचे (!eval(sig_s, undef, cell))
					पटिनिवत्त false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अञ्ञथा सचे (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			पूर्णाङ्क width = GetSize(sig_c);

			सचे (!eval(sig_a, undef, cell))
				पटिनिवत्त false;

			सचे (!eval(sig_b, undef, cell))
				पटिनिवत्त false;

			सचे (!eval(sig_c, undef, cell))
				पटिनिवत्त false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			अत्थाय (पूर्णाङ्क i = 0; i < GetSize(val_y); i++)
				सचे (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		अञ्ञथा सचे (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.गणनम्(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.गणनम्(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			सचे (!eval(sig_a, undef, cell))
				पटिनिवत्त false;

			सचे (!eval(sig_b, undef, cell))
				पटिनिवत्त false;

			सचे (!eval(sig_ci, undef, cell))
				पटिनिवत्त false;

			सचे (!eval(sig_bi, undef, cell))
				पटिनिवत्त false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_निवेशः_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			अत्थाय (पूर्णाङ्क i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				सचे (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} अञ्ञथा {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				सचे (any_निवेशः_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} अञ्ञथा {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		अञ्ञथा सचे (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			अत्थाय (auto &port : macc.terms) {
				सचे (!eval(port.in_a, undef, cell))
					पटिनिवत्त false;
				सचे (!eval(port.in_b, undef, cell))
					पटिनिवत्त false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			सचे (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		अञ्ञथा
		{
			RTLIL::SigSpec sig_c, sig_d;

			सचे (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				सचे (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				सचे (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			सचे (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				पटिनिवत्त false;
			सचे (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				पटिनिवत्त false;
			सचे (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				पटिनिवत्त false;
			सचे (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				पटिनिवत्त false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			सचे (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} अञ्ञथा
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			सचे (eval_err)
				पटिनिवत्त false;

			set(sig_y, eval_ret);
		}

		पटिनिवत्त true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = रिक्त)
	{
		assign_map.apply(sig);
		मानानि_map.apply(sig);

		सचे (sig.is_fully_const())
			पटिनिवत्त true;

		सचे (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			पटिनिवत्त false;
		}

		सचे (busy_cell) {
			सचे (busy.गणनम्(busy_cell) > 0) {
				undef = sig;
				पटिनिवत्त false;
			}
			busy.निवेशय(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		अत्थाय (auto cell : driver_cells) {
			सचे (!eval(cell, undef)) {
				सचे (busy_cell)
					busy.erase(busy_cell);
				पटिनिवत्त false;
			}
		}

		सचे (busy_cell)
			busy.erase(busy_cell);

		मानानि_map.apply(sig);
		सचे (sig.is_fully_const())
			पटिनिवत्त true;

		सचे (defaultval != RTLIL::State::Sm) {
			अत्थाय (auto &bit : sig)
				सचे (bit.wire) bit = defaultval;
			पटिनिवत्त true;
		}

		अत्थाय (auto &c : sig.chunks())
			सचे (c.wire != रिक्त)
				undef.append(c);
		पटिनिवत्त false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		पटिनिवत्त eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
