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

#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sigtools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/celltypes.h"
#ଅନ୍ତର୍ଭାଵ "kernel/macc.h"
#ଅନ୍ତର୍ଭାଵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
ସଂରଚନା ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap ମୂଲ୍ୟଗୁଡ଼ିକ_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> ଗଦା;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		ପାଇଁ (auto &it : module->cells_) {
			ଯଦି (!ct.cell_known(it.second->type))
				ଜାରି;
			ପାଇଁ (auto &it2 : it.second->connections())
				ଯଦି (ct.cell_output(it.second->type, it2.first))
					sig2driver.ଭର୍ତ୍ତି(assign_map(it2.second), it.second);
		}
	}

	ଶୂନ୍ୟ clear()
	{
		ମୂଲ୍ୟଗୁଡ଼ିକ_map.clear();
		stop_signals.clear();
	}

	ଶୂନ୍ୟ ଠେଲ()
	{
		ଗଦା.ଠେଲ_back(ମୂଲ୍ୟଗୁଡ଼ିକ_map);
	}

	ଶୂନ୍ୟ କାଢ଼()
	{
		ମୂଲ୍ୟଗୁଡ଼ିକ_map.swap(ଗଦା.back());
		ଗଦା.କାଢ଼_back();
	}

	ଶୂନ୍ୟ set(RTLIL::SigSpec sig, RTLIL::Const ମୂଲ୍ୟ)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = ମୂଲ୍ୟଗୁଡ଼ିକ_map(sig);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != ଖାଲି || current_val[i] == ମୂଲ୍ୟ[i]);
#endif
		ମୂଲ୍ୟଗୁଡ଼ିକ_map.add(sig, RTLIL::SigSpec(ମୂଲ୍ୟ));
	}

	ଶୂନ୍ୟ stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		ଯଦି (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = ମୂଲ୍ୟଗୁଡ଼ିକ_map(assign_map(cell->getPort(ID::CO)));

			ଯଦି (sig_co.is_fully_const())
				ଫେରନ୍ତୁ true;

			ଯଦି (!eval(sig_p, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (!eval(sig_g, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (!eval(sig_ci, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			ଅନ୍ୟଥା
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			ଫେରନ୍ତୁ true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = ମୂଲ୍ୟଗୁଡ଼ିକ_map(assign_map(cell->getPort(ID::Y)));
		ଯଦି (sig_y.is_fully_const())
			ଫେରନ୍ତୁ true;

		ଯଦି (cell->type == ID($barrier))
		{
			ଯଦି (!model_barriers) {
				undef.append(sig_y);
				ଫେରନ୍ତୁ false;
			}
			sig_a = cell->getPort(ID::A);
			ଯଦି (!eval(sig_a, undef, cell))
				ଫେରନ୍ତୁ false;
			set(sig_y, sig_a.as_const());
			ଫେରନ୍ତୁ true;
		}

		ଯଦି (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		ଯଦି (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		ଯଦି (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		ଯଦି (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_set_s_bits = 0;

			ଯଦି (!eval(sig_s, undef, cell))
				ଫେରନ୍ତୁ false;

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				ଯଦି (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.ଠେଲ_back(b_slice);

				ଯଦି (s_bit == RTLIL::State::S1)
					ଗଣନା_set_s_bits++;
			}

			ଯଦି (ଗଣନା_set_s_bits == 0)
				y_candidates.ଠେଲ_back(sig_a);

			std::vector<RTLIL::Const> y_ମୂଲ୍ୟଗୁଡ଼ିକ;

			log_assert(y_candidates.size() > 0);
			ପାଇଁ (auto &yc : y_candidates) {
				ଯଦି (!eval(yc, undef, cell))
					ଫେରନ୍ତୁ false;
				ଯଦି (cell->type == ID($_NMUX_))
					y_ମୂଲ୍ୟଗୁଡ଼ିକ.ଠେଲ_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				ଅନ୍ୟଥା
					y_ମୂଲ୍ୟଗୁଡ଼ିକ.ଠେଲ_back(yc.as_const());
			}

			ଯଦି (y_ମୂଲ୍ୟଗୁଡ଼ିକ.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_ମୂଲ୍ୟଗୁଡ଼ିକ.at(0).to_bits();

				ପାଇଁ (size_t i = 1; i < y_ମୂଲ୍ୟଗୁଡ଼ିକ.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_ମୂଲ୍ୟଗୁଡ଼ିକ.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					ପାଇଁ (size_t j = 0; j < master_bits.size(); j++)
						ଯଦି (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			ଅନ୍ୟଥା
				set(sig_y, y_ମୂଲ୍ୟଗୁଡ଼ିକ.front());
		}
		ଅନ୍ୟଥା ଯଦି (cell->type == ID($bmux))
		{
			ଯଦି (!eval(sig_s, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (sig_s.is_fully_def()) {
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା sel = sig_s.as_int();
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				ଯଦି (!eval(res, undef, cell))
					ଫେରନ୍ତୁ false;
				set(sig_y, res.as_const());
			} ଅନ୍ୟଥା {
				ଯଦି (!eval(sig_a, undef, cell))
					ଫେରନ୍ତୁ false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ଅନ୍ୟଥା ଯଦି (cell->type == ID($demux))
		{
			ଯଦି (!eval(sig_a, undef, cell))
				ଫେରନ୍ତୁ false;
			ଯଦି (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} ଅନ୍ୟଥା {
				ଯଦି (!eval(sig_s, undef, cell))
					ଫେରନ୍ତୁ false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ଅନ୍ୟଥା ଯଦି (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(sig_c);

			ଯଦି (!eval(sig_a, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (!eval(sig_b, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (!eval(sig_c, undef, cell))
				ଫେରନ୍ତୁ false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(val_y); i++)
				ଯଦି (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		ଅନ୍ୟଥା ଯଦି (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.ଗଣନା_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.ଗଣନା_2(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			ଯଦି (!eval(sig_a, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (!eval(sig_b, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (!eval(sig_ci, undef, cell))
				ଫେରନ୍ତୁ false;

			ଯଦି (!eval(sig_bi, undef, cell))
				ଫେରନ୍ତୁ false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_ନିବେଶ_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				ଯଦି (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} ଅନ୍ୟଥା {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				ଯଦି (any_ନିବେଶ_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} ଅନ୍ୟଥା {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			ପାଇଁ (auto &port : macc.terms) {
				ଯଦି (!eval(port.in_a, undef, cell))
					ଫେରନ୍ତୁ false;
				ଯଦି (!eval(port.in_b, undef, cell))
					ଫେରନ୍ତୁ false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			ଯଦି (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		ଅନ୍ୟଥା
		{
			RTLIL::SigSpec sig_c, sig_d;

			ଯଦି (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				ଯଦି (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				ଯଦି (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			ଯଦି (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				ଫେରନ୍ତୁ false;
			ଯଦି (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				ଫେରନ୍ତୁ false;
			ଯଦି (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				ଫେରନ୍ତୁ false;
			ଯଦି (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				ଫେରନ୍ତୁ false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			ଯଦି (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} ଅନ୍ୟଥା
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			ଯଦି (eval_err)
				ଫେରନ୍ତୁ false;

			set(sig_y, eval_ret);
		}

		ଫେରନ୍ତୁ true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = ଖାଲି)
	{
		assign_map.apply(sig);
		ମୂଲ୍ୟଗୁଡ଼ିକ_map.apply(sig);

		ଯଦି (sig.is_fully_const())
			ଫେରନ୍ତୁ true;

		ଯଦି (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			ଫେରନ୍ତୁ false;
		}

		ଯଦି (busy_cell) {
			ଯଦି (busy.ଗଣନା_2(busy_cell) > 0) {
				undef = sig;
				ଫେରନ୍ତୁ false;
			}
			busy.ଭର୍ତ୍ତି(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		ପାଇଁ (auto cell : driver_cells) {
			ଯଦି (!eval(cell, undef)) {
				ଯଦି (busy_cell)
					busy.erase(busy_cell);
				ଫେରନ୍ତୁ false;
			}
		}

		ଯଦି (busy_cell)
			busy.erase(busy_cell);

		ମୂଲ୍ୟଗୁଡ଼ିକ_map.apply(sig);
		ଯଦି (sig.is_fully_const())
			ଫେରନ୍ତୁ true;

		ଯଦି (defaultval != RTLIL::State::Sm) {
			ପାଇଁ (auto &bit : sig)
				ଯଦି (bit.wire) bit = defaultval;
			ଫେରନ୍ତୁ true;
		}

		ପାଇଁ (auto &c : sig.chunks())
			ଯଦି (c.wire != ଖାଲି)
				undef.append(c);
		ଫେରନ୍ତୁ false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		ଫେରନ୍ତୁ eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
