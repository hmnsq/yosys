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

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"
#ಅನ್ತರ್ಭಾವ "kernel/macc.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
ಸಂರಚನಾ ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap ಮೌಲ್ಯಗಳು_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> ರಾಶಿ;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		ಪ್ರತಿ (auto &it : module->cells_) {
			ಯದಿ (!ct.cell_known(it.second->type))
				ಅನುವರ್ತ;
			ಪ್ರತಿ (auto &it2 : it.second->connections())
				ಯದಿ (ct.cell_output(it.second->type, it2.first))
					sig2driver.ಸೇರಿಸು(assign_map(it2.second), it.second);
		}
	}

	ಶೂನ್ಯ clear()
	{
		ಮೌಲ್ಯಗಳು_map.clear();
		stop_signals.clear();
	}

	ಶೂನ್ಯ ತಳ್ಳು()
	{
		ರಾಶಿ.ತಳ್ಳು_back(ಮೌಲ್ಯಗಳು_map);
	}

	ಶೂನ್ಯ ತೆಗೆ()
	{
		ಮೌಲ್ಯಗಳು_map.swap(ರಾಶಿ.back());
		ರಾಶಿ.ತೆಗೆ_back();
	}

	ಶೂನ್ಯ set(RTLIL::SigSpec sig, RTLIL::Const ಮೌಲ್ಯ)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = ಮೌಲ್ಯಗಳು_map(sig);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != ರಿಕ್ತ || current_val[i] == ಮೌಲ್ಯ[i]);
#endif
		ಮೌಲ್ಯಗಳು_map.add(sig, RTLIL::SigSpec(ಮೌಲ್ಯ));
	}

	ಶೂನ್ಯ stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		ಯದಿ (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = ಮೌಲ್ಯಗಳು_map(assign_map(cell->getPort(ID::CO)));

			ಯದಿ (sig_co.is_fully_const())
				ಪ್ರತ್ಯಾಗಮ true;

			ಯದಿ (!eval(sig_p, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (!eval(sig_g, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (!eval(sig_ci, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			ಅನ್ಯಥಾ
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			ಪ್ರತ್ಯಾಗಮ true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = ಮೌಲ್ಯಗಳು_map(assign_map(cell->getPort(ID::Y)));
		ಯದಿ (sig_y.is_fully_const())
			ಪ್ರತ್ಯಾಗಮ true;

		ಯದಿ (cell->type == ID($barrier))
		{
			ಯದಿ (!model_barriers) {
				undef.append(sig_y);
				ಪ್ರತ್ಯಾಗಮ false;
			}
			sig_a = cell->getPort(ID::A);
			ಯದಿ (!eval(sig_a, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;
			set(sig_y, sig_a.as_const());
			ಪ್ರತ್ಯಾಗಮ true;
		}

		ಯದಿ (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		ಯದಿ (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		ಯದಿ (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		ಯದಿ (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ_set_s_bits = 0;

			ಯದಿ (!eval(sig_s, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				ಯದಿ (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.ತಳ್ಳು_back(b_slice);

				ಯದಿ (s_bit == RTLIL::State::S1)
					ಎಣಿಕೆ_set_s_bits++;
			}

			ಯದಿ (ಎಣಿಕೆ_set_s_bits == 0)
				y_candidates.ತಳ್ಳು_back(sig_a);

			std::vector<RTLIL::Const> y_ಮೌಲ್ಯಗಳು;

			log_assert(y_candidates.size() > 0);
			ಪ್ರತಿ (auto &yc : y_candidates) {
				ಯದಿ (!eval(yc, undef, cell))
					ಪ್ರತ್ಯಾಗಮ false;
				ಯದಿ (cell->type == ID($_NMUX_))
					y_ಮೌಲ್ಯಗಳು.ತಳ್ಳು_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				ಅನ್ಯಥಾ
					y_ಮೌಲ್ಯಗಳು.ತಳ್ಳು_back(yc.as_const());
			}

			ಯದಿ (y_ಮೌಲ್ಯಗಳು.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_ಮೌಲ್ಯಗಳು.at(0).to_bits();

				ಪ್ರತಿ (size_t i = 1; i < y_ಮೌಲ್ಯಗಳು.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_ಮೌಲ್ಯಗಳು.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					ಪ್ರತಿ (size_t j = 0; j < master_bits.size(); j++)
						ಯದಿ (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			ಅನ್ಯಥಾ
				set(sig_y, y_ಮೌಲ್ಯಗಳು.front());
		}
		ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($bmux))
		{
			ಯದಿ (!eval(sig_s, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (sig_s.is_fully_def()) {
				ಪೂರ್ಣಾಙ್ಕ sel = sig_s.as_int();
				ಪೂರ್ಣಾಙ್ಕ width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				ಯದಿ (!eval(res, undef, cell))
					ಪ್ರತ್ಯಾಗಮ false;
				set(sig_y, res.as_const());
			} ಅನ್ಯಥಾ {
				ಯದಿ (!eval(sig_a, undef, cell))
					ಪ್ರತ್ಯಾಗಮ false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($demux))
		{
			ಯದಿ (!eval(sig_a, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;
			ಯದಿ (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} ಅನ್ಯಥಾ {
				ಯದಿ (!eval(sig_s, undef, cell))
					ಪ್ರತ್ಯಾಗಮ false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			ಪೂರ್ಣಾಙ್ಕ width = GetSize(sig_c);

			ಯದಿ (!eval(sig_a, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (!eval(sig_b, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (!eval(sig_c, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(val_y); i++)
				ಯದಿ (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		ಅನ್ಯಥಾ ಯದಿ (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.ಎಣಿಕೆ(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.ಎಣಿಕೆ(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			ಯದಿ (!eval(sig_a, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (!eval(sig_b, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (!eval(sig_ci, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			ಯದಿ (!eval(sig_bi, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_ಒಳಹರಿವು_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				ಯದಿ (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} ಅನ್ಯಥಾ {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				ಯದಿ (any_ಒಳಹರಿವು_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} ಅನ್ಯಥಾ {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			ಪ್ರತಿ (auto &port : macc.terms) {
				ಯದಿ (!eval(port.in_a, undef, cell))
					ಪ್ರತ್ಯಾಗಮ false;
				ಯದಿ (!eval(port.in_b, undef, cell))
					ಪ್ರತ್ಯಾಗಮ false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			ಯದಿ (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		ಅನ್ಯಥಾ
		{
			RTLIL::SigSpec sig_c, sig_d;

			ಯದಿ (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				ಯದಿ (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				ಯದಿ (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			ಯದಿ (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;
			ಯದಿ (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;
			ಯದಿ (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;
			ಯದಿ (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				ಪ್ರತ್ಯಾಗಮ false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			ಯದಿ (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} ಅನ್ಯಥಾ
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			ಯದಿ (eval_err)
				ಪ್ರತ್ಯಾಗಮ false;

			set(sig_y, eval_ret);
		}

		ಪ್ರತ್ಯಾಗಮ true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = ರಿಕ್ತ)
	{
		assign_map.apply(sig);
		ಮೌಲ್ಯಗಳು_map.apply(sig);

		ಯದಿ (sig.is_fully_const())
			ಪ್ರತ್ಯಾಗಮ true;

		ಯದಿ (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			ಪ್ರತ್ಯಾಗಮ false;
		}

		ಯದಿ (busy_cell) {
			ಯದಿ (busy.ಎಣಿಕೆ(busy_cell) > 0) {
				undef = sig;
				ಪ್ರತ್ಯಾಗಮ false;
			}
			busy.ಸೇರಿಸು(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		ಪ್ರತಿ (auto cell : driver_cells) {
			ಯದಿ (!eval(cell, undef)) {
				ಯದಿ (busy_cell)
					busy.erase(busy_cell);
				ಪ್ರತ್ಯಾಗಮ false;
			}
		}

		ಯದಿ (busy_cell)
			busy.erase(busy_cell);

		ಮೌಲ್ಯಗಳು_map.apply(sig);
		ಯದಿ (sig.is_fully_const())
			ಪ್ರತ್ಯಾಗಮ true;

		ಯದಿ (defaultval != RTLIL::State::Sm) {
			ಪ್ರತಿ (auto &bit : sig)
				ಯದಿ (bit.wire) bit = defaultval;
			ಪ್ರತ್ಯಾಗಮ true;
		}

		ಪ್ರತಿ (auto &c : sig.chunks())
			ಯದಿ (c.wire != ರಿಕ್ತ)
				undef.append(c);
		ಪ್ರತ್ಯಾಗಮ false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		ಪ್ರತ್ಯಾಗಮ eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
