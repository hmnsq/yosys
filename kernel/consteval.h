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

#include "kernel/rtlil.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"
#include "kernel/macc.h"
#include "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
struct ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap values_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> stack;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		за (auto &it : module->cells_) {
			ако (!ct.cell_known(it.second->type))
				continue;
			за (auto &it2 : it.second->connections())
				ако (ct.cell_output(it.second->type, it2.first))
					sig2driver.insert(assign_map(it2.second), it.second);
		}
	}

	void clear()
	{
		values_map.clear();
		stop_signals.clear();
	}

	void push()
	{
		stack.push_back(values_map);
	}

	void pop()
	{
		values_map.swap(stack.back());
		stack.pop_back();
	}

	void set(RTLIL::SigSpec sig, RTLIL::Const value)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = values_map(sig);
		за (цяло i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != NULL || current_val[i] == value[i]);
#endif
		values_map.add(sig, RTLIL::SigSpec(value));
	}

	void stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		ако (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = values_map(assign_map(cell->getPort(ID::CO)));

			ако (sig_co.is_fully_const())
				върни true;

			ако (!eval(sig_p, undef, cell))
				върни false;

			ако (!eval(sig_g, undef, cell))
				върни false;

			ако (!eval(sig_ci, undef, cell))
				върни false;

			ако (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				за (цяло i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			иначе
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			върни true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = values_map(assign_map(cell->getPort(ID::Y)));
		ако (sig_y.is_fully_const())
			върни true;

		ако (cell->type == ID($barrier))
		{
			ако (!model_barriers) {
				undef.append(sig_y);
				върни false;
			}
			sig_a = cell->getPort(ID::A);
			ако (!eval(sig_a, undef, cell))
				върни false;
			set(sig_y, sig_a.as_const());
			върни true;
		}

		ако (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		ако (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		ако (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		ако (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			цяло count_set_s_bits = 0;

			ако (!eval(sig_s, undef, cell))
				върни false;

			за (цяло i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				ако (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.push_back(b_slice);

				ако (s_bit == RTLIL::State::S1)
					count_set_s_bits++;
			}

			ако (count_set_s_bits == 0)
				y_candidates.push_back(sig_a);

			std::vector<RTLIL::Const> y_values;

			log_assert(y_candidates.size() > 0);
			за (auto &yc : y_candidates) {
				ако (!eval(yc, undef, cell))
					върни false;
				ако (cell->type == ID($_NMUX_))
					y_values.push_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				иначе
					y_values.push_back(yc.as_const());
			}

			ако (y_values.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_values.at(0).to_bits();

				за (size_t i = 1; i < y_values.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_values.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					за (size_t j = 0; j < master_bits.size(); j++)
						ако (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			иначе
				set(sig_y, y_values.front());
		}
		иначе ако (cell->type == ID($bmux))
		{
			ако (!eval(sig_s, undef, cell))
				върни false;

			ако (sig_s.is_fully_def()) {
				цяло sel = sig_s.as_int();
				цяло width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				ако (!eval(res, undef, cell))
					върни false;
				set(sig_y, res.as_const());
			} иначе {
				ако (!eval(sig_a, undef, cell))
					върни false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		иначе ако (cell->type == ID($demux))
		{
			ако (!eval(sig_a, undef, cell))
				върни false;
			ако (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} иначе {
				ако (!eval(sig_s, undef, cell))
					върни false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		иначе ако (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			цяло width = GetSize(sig_c);

			ако (!eval(sig_a, undef, cell))
				върни false;

			ако (!eval(sig_b, undef, cell))
				върни false;

			ако (!eval(sig_c, undef, cell))
				върни false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			за (цяло i = 0; i < GetSize(val_y); i++)
				ако (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		иначе ако (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.count(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			ако (!eval(sig_a, undef, cell))
				върни false;

			ако (!eval(sig_b, undef, cell))
				върни false;

			ако (!eval(sig_ci, undef, cell))
				върни false;

			ако (!eval(sig_bi, undef, cell))
				върни false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_input_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			за (цяло i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				ако (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} иначе {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				ако (any_input_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} иначе {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		иначе ако (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			за (auto &port : macc.terms) {
				ако (!eval(port.in_a, undef, cell))
					върни false;
				ако (!eval(port.in_b, undef, cell))
					върни false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			ако (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		иначе
		{
			RTLIL::SigSpec sig_c, sig_d;

			ако (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				ако (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				ако (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			ако (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				върни false;
			ако (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				върни false;
			ако (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				върни false;
			ако (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				върни false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			ако (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} иначе
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			ако (eval_err)
				върни false;

			set(sig_y, eval_ret);
		}

		върни true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = NULL)
	{
		assign_map.apply(sig);
		values_map.apply(sig);

		ако (sig.is_fully_const())
			върни true;

		ако (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			върни false;
		}

		ако (busy_cell) {
			ако (busy.count(busy_cell) > 0) {
				undef = sig;
				върни false;
			}
			busy.insert(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		за (auto cell : driver_cells) {
			ако (!eval(cell, undef)) {
				ако (busy_cell)
					busy.erase(busy_cell);
				върни false;
			}
		}

		ако (busy_cell)
			busy.erase(busy_cell);

		values_map.apply(sig);
		ако (sig.is_fully_const())
			върни true;

		ако (defaultval != RTLIL::State::Sm) {
			за (auto &bit : sig)
				ако (bit.wire) bit = defaultval;
			върни true;
		}

		за (auto &c : sig.chunks())
			ако (c.wire != NULL)
				undef.append(c);
		върни false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		върни eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
