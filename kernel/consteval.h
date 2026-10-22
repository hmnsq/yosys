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

#شامل "kernel/rtlil.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/celltypes.h"
#شامل "kernel/macc.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
ساختار ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap مقادیر_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> پشته;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		برای (auto &it : module->cells_) {
			اگر (!ct.cell_known(it.second->type))
				ادامه;
			برای (auto &it2 : it.second->connections())
				اگر (ct.cell_output(it.second->type, it2.first))
					sig2driver.درج(assign_map(it2.second), it.second);
		}
	}

	تهی clear()
	{
		مقادیر_map.clear();
		stop_signals.clear();
	}

	تهی افزودن()
	{
		پشته.افزودن_back(مقادیر_map);
	}

	تهی برداشت()
	{
		مقادیر_map.swap(پشته.back());
		پشته.برداشت_back();
	}

	تهی set(RTLIL::SigSpec sig, RTLIL::Const مقدار)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = مقادیر_map(sig);
		برای (صحیح i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != پوچ || current_val[i] == مقدار[i]);
#endif
		مقادیر_map.add(sig, RTLIL::SigSpec(مقدار));
	}

	تهی stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		اگر (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = مقادیر_map(assign_map(cell->getPort(ID::CO)));

			اگر (sig_co.is_fully_const())
				بازگشت true;

			اگر (!eval(sig_p, undef, cell))
				بازگشت false;

			اگر (!eval(sig_g, undef, cell))
				بازگشت false;

			اگر (!eval(sig_ci, undef, cell))
				بازگشت false;

			اگر (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				برای (صحیح i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			وگرنه
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			بازگشت true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = مقادیر_map(assign_map(cell->getPort(ID::Y)));
		اگر (sig_y.is_fully_const())
			بازگشت true;

		اگر (cell->type == ID($barrier))
		{
			اگر (!model_barriers) {
				undef.append(sig_y);
				بازگشت false;
			}
			sig_a = cell->getPort(ID::A);
			اگر (!eval(sig_a, undef, cell))
				بازگشت false;
			set(sig_y, sig_a.as_const());
			بازگشت true;
		}

		اگر (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		اگر (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		اگر (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		اگر (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			صحیح شمار_set_s_bits = 0;

			اگر (!eval(sig_s, undef, cell))
				بازگشت false;

			برای (صحیح i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				اگر (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.افزودن_back(b_slice);

				اگر (s_bit == RTLIL::State::S1)
					شمار_set_s_bits++;
			}

			اگر (شمار_set_s_bits == 0)
				y_candidates.افزودن_back(sig_a);

			std::vector<RTLIL::Const> y_مقادیر;

			log_assert(y_candidates.size() > 0);
			برای (auto &yc : y_candidates) {
				اگر (!eval(yc, undef, cell))
					بازگشت false;
				اگر (cell->type == ID($_NMUX_))
					y_مقادیر.افزودن_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				وگرنه
					y_مقادیر.افزودن_back(yc.as_const());
			}

			اگر (y_مقادیر.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_مقادیر.at(0).to_bits();

				برای (size_t i = 1; i < y_مقادیر.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_مقادیر.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					برای (size_t j = 0; j < master_bits.size(); j++)
						اگر (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			وگرنه
				set(sig_y, y_مقادیر.front());
		}
		وگرنه اگر (cell->type == ID($bmux))
		{
			اگر (!eval(sig_s, undef, cell))
				بازگشت false;

			اگر (sig_s.is_fully_def()) {
				صحیح sel = sig_s.as_int();
				صحیح width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				اگر (!eval(res, undef, cell))
					بازگشت false;
				set(sig_y, res.as_const());
			} وگرنه {
				اگر (!eval(sig_a, undef, cell))
					بازگشت false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وگرنه اگر (cell->type == ID($demux))
		{
			اگر (!eval(sig_a, undef, cell))
				بازگشت false;
			اگر (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} وگرنه {
				اگر (!eval(sig_s, undef, cell))
					بازگشت false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وگرنه اگر (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			صحیح width = GetSize(sig_c);

			اگر (!eval(sig_a, undef, cell))
				بازگشت false;

			اگر (!eval(sig_b, undef, cell))
				بازگشت false;

			اگر (!eval(sig_c, undef, cell))
				بازگشت false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			برای (صحیح i = 0; i < GetSize(val_y); i++)
				اگر (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		وگرنه اگر (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.شمار(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.شمار(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			اگر (!eval(sig_a, undef, cell))
				بازگشت false;

			اگر (!eval(sig_b, undef, cell))
				بازگشت false;

			اگر (!eval(sig_ci, undef, cell))
				بازگشت false;

			اگر (!eval(sig_bi, undef, cell))
				بازگشت false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_درون_داد_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			برای (صحیح i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				اگر (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} وگرنه {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				اگر (any_درون_داد_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} وگرنه {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		وگرنه اگر (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			برای (auto &port : macc.terms) {
				اگر (!eval(port.in_a, undef, cell))
					بازگشت false;
				اگر (!eval(port.in_b, undef, cell))
					بازگشت false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			اگر (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		وگرنه
		{
			RTLIL::SigSpec sig_c, sig_d;

			اگر (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				اگر (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				اگر (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			اگر (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				بازگشت false;
			اگر (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				بازگشت false;
			اگر (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				بازگشت false;
			اگر (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				بازگشت false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			اگر (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} وگرنه
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			اگر (eval_err)
				بازگشت false;

			set(sig_y, eval_ret);
		}

		بازگشت true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = پوچ)
	{
		assign_map.apply(sig);
		مقادیر_map.apply(sig);

		اگر (sig.is_fully_const())
			بازگشت true;

		اگر (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			بازگشت false;
		}

		اگر (busy_cell) {
			اگر (busy.شمار(busy_cell) > 0) {
				undef = sig;
				بازگشت false;
			}
			busy.درج(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		برای (auto cell : driver_cells) {
			اگر (!eval(cell, undef)) {
				اگر (busy_cell)
					busy.erase(busy_cell);
				بازگشت false;
			}
		}

		اگر (busy_cell)
			busy.erase(busy_cell);

		مقادیر_map.apply(sig);
		اگر (sig.is_fully_const())
			بازگشت true;

		اگر (defaultval != RTLIL::State::Sm) {
			برای (auto &bit : sig)
				اگر (bit.wire) bit = defaultval;
			بازگشت true;
		}

		برای (auto &c : sig.chunks())
			اگر (c.wire != پوچ)
				undef.append(c);
		بازگشت false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		بازگشت eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
