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

#ضمن "kernel/rtlil.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/celltypes.h"
#ضمن "kernel/macc.h"
#ضمن "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
بنية ConstEval
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

		עבור (auto &it : module->cells_) {
			אם (!ct.cell_known(it.second->type))
				استمر;
			עבור (auto &it2 : it.second->connections())
				אם (ct.cell_output(it.second->type, it2.first))
					sig2driver.insert(assign_map(it2.second), it.second);
		}
	}

	فراغ clear()
	{
		values_map.clear();
		stop_signals.clear();
	}

	فراغ push()
	{
		stack.push_back(values_map);
	}

	فراغ pop()
	{
		values_map.swap(stack.back());
		stack.pop_back();
	}

	فراغ set(RTLIL::SigSpec sig, RTLIL::Const value)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = values_map(sig);
		עבור (صحيح i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != عدم || current_val[i] == value[i]);
#endif
		values_map.add(sig, RTLIL::SigSpec(value));
	}

	فراغ stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		אם (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = values_map(assign_map(cell->getPort(ID::CO)));

			אם (sig_co.is_fully_const())
				החזר true;

			אם (!eval(sig_p, undef, cell))
				החזר false;

			אם (!eval(sig_g, undef, cell))
				החזר false;

			אם (!eval(sig_ci, undef, cell))
				החזר false;

			אם (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				עבור (صحيح i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			אחרת
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			החזר true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = values_map(assign_map(cell->getPort(ID::Y)));
		אם (sig_y.is_fully_const())
			החזר true;

		אם (cell->type == ID($barrier))
		{
			אם (!model_barriers) {
				undef.append(sig_y);
				החזר false;
			}
			sig_a = cell->getPort(ID::A);
			אם (!eval(sig_a, undef, cell))
				החזר false;
			set(sig_y, sig_a.as_const());
			החזר true;
		}

		אם (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		אם (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		אם (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		אם (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			صحيح count_set_s_bits = 0;

			אם (!eval(sig_s, undef, cell))
				החזר false;

			עבור (صحيح i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				אם (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.push_back(b_slice);

				אם (s_bit == RTLIL::State::S1)
					count_set_s_bits++;
			}

			אם (count_set_s_bits == 0)
				y_candidates.push_back(sig_a);

			std::vector<RTLIL::Const> y_values;

			log_assert(y_candidates.size() > 0);
			עבור (auto &yc : y_candidates) {
				אם (!eval(yc, undef, cell))
					החזר false;
				אם (cell->type == ID($_NMUX_))
					y_values.push_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				אחרת
					y_values.push_back(yc.as_const());
			}

			אם (y_values.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_values.at(0).to_bits();

				עבור (size_t i = 1; i < y_values.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_values.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					עבור (size_t j = 0; j < master_bits.size(); j++)
						אם (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			אחרת
				set(sig_y, y_values.front());
		}
		אחרת אם (cell->type == ID($bmux))
		{
			אם (!eval(sig_s, undef, cell))
				החזר false;

			אם (sig_s.is_fully_def()) {
				صحيح sel = sig_s.as_int();
				صحيح width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				אם (!eval(res, undef, cell))
					החזר false;
				set(sig_y, res.as_const());
			} אחרת {
				אם (!eval(sig_a, undef, cell))
					החזר false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		אחרת אם (cell->type == ID($demux))
		{
			אם (!eval(sig_a, undef, cell))
				החזר false;
			אם (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} אחרת {
				אם (!eval(sig_s, undef, cell))
					החזר false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		אחרת אם (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			صحيح width = GetSize(sig_c);

			אם (!eval(sig_a, undef, cell))
				החזר false;

			אם (!eval(sig_b, undef, cell))
				החזר false;

			אם (!eval(sig_c, undef, cell))
				החזר false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			עבור (صحيح i = 0; i < GetSize(val_y); i++)
				אם (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		אחרת אם (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.count(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			אם (!eval(sig_a, undef, cell))
				החזר false;

			אם (!eval(sig_b, undef, cell))
				החזר false;

			אם (!eval(sig_ci, undef, cell))
				החזר false;

			אם (!eval(sig_bi, undef, cell))
				החזר false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_input_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			עבור (صحيح i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				אם (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} אחרת {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				אם (any_input_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} אחרת {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		אחרת אם (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			עבור (auto &port : macc.terms) {
				אם (!eval(port.in_a, undef, cell))
					החזר false;
				אם (!eval(port.in_b, undef, cell))
					החזר false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			אם (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		אחרת
		{
			RTLIL::SigSpec sig_c, sig_d;

			אם (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				אם (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				אם (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			אם (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				החזר false;
			אם (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				החזר false;
			אם (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				החזר false;
			אם (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				החזר false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			אם (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} אחרת
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			אם (eval_err)
				החזר false;

			set(sig_y, eval_ret);
		}

		החזר true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = عدم)
	{
		assign_map.apply(sig);
		values_map.apply(sig);

		אם (sig.is_fully_const())
			החזר true;

		אם (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			החזר false;
		}

		אם (busy_cell) {
			אם (busy.count(busy_cell) > 0) {
				undef = sig;
				החזר false;
			}
			busy.insert(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		עבור (auto cell : driver_cells) {
			אם (!eval(cell, undef)) {
				אם (busy_cell)
					busy.erase(busy_cell);
				החזר false;
			}
		}

		אם (busy_cell)
			busy.erase(busy_cell);

		values_map.apply(sig);
		אם (sig.is_fully_const())
			החזר true;

		אם (defaultval != RTLIL::State::Sm) {
			עבור (auto &bit : sig)
				אם (bit.wire) bit = defaultval;
			החזר true;
		}

		עבור (auto &c : sig.chunks())
			אם (c.wire != عدم)
				undef.append(c);
		החזר false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		החזר eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
