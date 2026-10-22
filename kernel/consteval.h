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

#অন্তর্ভাব "kernel/rtlil.h"
#অন্তর্ভাব "kernel/sigtools.h"
#অন্তর্ভাব "kernel/celltypes.h"
#অন্তর্ভাব "kernel/macc.h"
#অন্তর্ভাব "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
সংরচনা ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap মানসমূহ_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> স্তূপ;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		প্রতি (auto &it : module->cells_) {
			যদি (!ct.cell_known(it.second->type))
				অনুবর্ত;
			প্রতি (auto &it2 : it.second->connections())
				যদি (ct.cell_output(it.second->type, it2.first))
					sig2driver.ঢোকাও(assign_map(it2.second), it.second);
		}
	}

	শূন্য clear()
	{
		মানসমূহ_map.clear();
		stop_signals.clear();
	}

	শূন্য ঠেলো()
	{
		স্তূপ.ঠেলো_back(মানসমূহ_map);
	}

	শূন্য বের_করো()
	{
		মানসমূহ_map.swap(স্তূপ.back());
		স্তূপ.বের_করো_back();
	}

	শূন্য set(RTLIL::SigSpec sig, RTLIL::Const মান)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = মানসমূহ_map(sig);
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != রিক্ত || current_val[i] == মান[i]);
#endif
		মানসমূহ_map.add(sig, RTLIL::SigSpec(মান));
	}

	শূন্য stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		যদি (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = মানসমূহ_map(assign_map(cell->getPort(ID::CO)));

			যদি (sig_co.is_fully_const())
				প্রত্যাগম true;

			যদি (!eval(sig_p, undef, cell))
				প্রত্যাগম false;

			যদি (!eval(sig_g, undef, cell))
				প্রত্যাগম false;

			যদি (!eval(sig_ci, undef, cell))
				প্রত্যাগম false;

			যদি (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			অন্যথা
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			প্রত্যাগম true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = মানসমূহ_map(assign_map(cell->getPort(ID::Y)));
		যদি (sig_y.is_fully_const())
			প্রত্যাগম true;

		যদি (cell->type == ID($barrier))
		{
			যদি (!model_barriers) {
				undef.append(sig_y);
				প্রত্যাগম false;
			}
			sig_a = cell->getPort(ID::A);
			যদি (!eval(sig_a, undef, cell))
				প্রত্যাগম false;
			set(sig_y, sig_a.as_const());
			প্রত্যাগম true;
		}

		যদি (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		যদি (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		যদি (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		যদি (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			পূর্ণাঙ্ক গুনতি_set_s_bits = 0;

			যদি (!eval(sig_s, undef, cell))
				প্রত্যাগম false;

			প্রতি (পূর্ণাঙ্ক i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				যদি (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.ঠেলো_back(b_slice);

				যদি (s_bit == RTLIL::State::S1)
					গুনতি_set_s_bits++;
			}

			যদি (গুনতি_set_s_bits == 0)
				y_candidates.ঠেলো_back(sig_a);

			std::vector<RTLIL::Const> y_মানসমূহ;

			log_assert(y_candidates.size() > 0);
			প্রতি (auto &yc : y_candidates) {
				যদি (!eval(yc, undef, cell))
					প্রত্যাগম false;
				যদি (cell->type == ID($_NMUX_))
					y_মানসমূহ.ঠেলো_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				অন্যথা
					y_মানসমূহ.ঠেলো_back(yc.as_const());
			}

			যদি (y_মানসমূহ.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_মানসমূহ.at(0).to_bits();

				প্রতি (size_t i = 1; i < y_মানসমূহ.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_মানসমূহ.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					প্রতি (size_t j = 0; j < master_bits.size(); j++)
						যদি (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			অন্যথা
				set(sig_y, y_মানসমূহ.front());
		}
		অন্যথা যদি (cell->type == ID($bmux))
		{
			যদি (!eval(sig_s, undef, cell))
				প্রত্যাগম false;

			যদি (sig_s.is_fully_def()) {
				পূর্ণাঙ্ক sel = sig_s.as_int();
				পূর্ণাঙ্ক width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				যদি (!eval(res, undef, cell))
					প্রত্যাগম false;
				set(sig_y, res.as_const());
			} অন্যথা {
				যদি (!eval(sig_a, undef, cell))
					প্রত্যাগম false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		অন্যথা যদি (cell->type == ID($demux))
		{
			যদি (!eval(sig_a, undef, cell))
				প্রত্যাগম false;
			যদি (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} অন্যথা {
				যদি (!eval(sig_s, undef, cell))
					প্রত্যাগম false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		অন্যথা যদি (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			পূর্ণাঙ্ক width = GetSize(sig_c);

			যদি (!eval(sig_a, undef, cell))
				প্রত্যাগম false;

			যদি (!eval(sig_b, undef, cell))
				প্রত্যাগম false;

			যদি (!eval(sig_c, undef, cell))
				প্রত্যাগম false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(val_y); i++)
				যদি (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		অন্যথা যদি (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.গুনতি(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.গুনতি(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			যদি (!eval(sig_a, undef, cell))
				প্রত্যাগম false;

			যদি (!eval(sig_b, undef, cell))
				প্রত্যাগম false;

			যদি (!eval(sig_ci, undef, cell))
				প্রত্যাগম false;

			যদি (!eval(sig_bi, undef, cell))
				প্রত্যাগম false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_প্রবেশ_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				যদি (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} অন্যথা {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				যদি (any_প্রবেশ_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} অন্যথা {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		অন্যথা যদি (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			প্রতি (auto &port : macc.terms) {
				যদি (!eval(port.in_a, undef, cell))
					প্রত্যাগম false;
				যদি (!eval(port.in_b, undef, cell))
					প্রত্যাগম false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			যদি (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		অন্যথা
		{
			RTLIL::SigSpec sig_c, sig_d;

			যদি (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				যদি (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				যদি (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			যদি (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				প্রত্যাগম false;
			যদি (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				প্রত্যাগম false;
			যদি (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				প্রত্যাগম false;
			যদি (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				প্রত্যাগম false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			যদি (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} অন্যথা
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			যদি (eval_err)
				প্রত্যাগম false;

			set(sig_y, eval_ret);
		}

		প্রত্যাগম true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = রিক্ত)
	{
		assign_map.apply(sig);
		মানসমূহ_map.apply(sig);

		যদি (sig.is_fully_const())
			প্রত্যাগম true;

		যদি (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			প্রত্যাগম false;
		}

		যদি (busy_cell) {
			যদি (busy.গুনতি(busy_cell) > 0) {
				undef = sig;
				প্রত্যাগম false;
			}
			busy.ঢোকাও(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		প্রতি (auto cell : driver_cells) {
			যদি (!eval(cell, undef)) {
				যদি (busy_cell)
					busy.erase(busy_cell);
				প্রত্যাগম false;
			}
		}

		যদি (busy_cell)
			busy.erase(busy_cell);

		মানসমূহ_map.apply(sig);
		যদি (sig.is_fully_const())
			প্রত্যাগম true;

		যদি (defaultval != RTLIL::State::Sm) {
			প্রতি (auto &bit : sig)
				যদি (bit.wire) bit = defaultval;
			প্রত্যাগম true;
		}

		প্রতি (auto &c : sig.chunks())
			যদি (c.wire != রিক্ত)
				undef.append(c);
		প্রত্যাগম false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		প্রত্যাগম eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
