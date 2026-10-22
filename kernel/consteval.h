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
	SigMap मूल्यावली_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> ढेर;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		प्रति (auto &it : module->cells_) {
			यदि (!ct.cell_known(it.second->type))
				अनुवर्त;
			प्रति (auto &it2 : it.second->connections())
				यदि (ct.cell_output(it.second->type, it2.first))
					sig2driver.जोड़ो(assign_map(it2.second), it.second);
		}
	}

	शून्य clear()
	{
		मूल्यावली_map.clear();
		stop_signals.clear();
	}

	शून्य धकेलो()
	{
		ढेर.धकेलो_back(मूल्यावली_map);
	}

	शून्य निकालो()
	{
		मूल्यावली_map.swap(ढेर.back());
		ढेर.निकालो_back();
	}

	शून्य set(RTLIL::SigSpec sig, RTLIL::Const मूल्य)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = मूल्यावली_map(sig);
		प्रति (पूर्णाङ्क i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != रिक्त || current_val[i] == मूल्य[i]);
#endif
		मूल्यावली_map.add(sig, RTLIL::SigSpec(मूल्य));
	}

	शून्य stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		यदि (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = मूल्यावली_map(assign_map(cell->getPort(ID::CO)));

			यदि (sig_co.is_fully_const())
				प्रत्यागम true;

			यदि (!eval(sig_p, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_g, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_ci, undef, cell))
				प्रत्यागम false;

			यदि (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				प्रति (पूर्णाङ्क i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			अन्यथा
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			प्रत्यागम true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = मूल्यावली_map(assign_map(cell->getPort(ID::Y)));
		यदि (sig_y.is_fully_const())
			प्रत्यागम true;

		यदि (cell->type == ID($barrier))
		{
			यदि (!model_barriers) {
				undef.append(sig_y);
				प्रत्यागम false;
			}
			sig_a = cell->getPort(ID::A);
			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;
			set(sig_y, sig_a.as_const());
			प्रत्यागम true;
		}

		यदि (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		यदि (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		यदि (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		यदि (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			पूर्णाङ्क गिनती_set_s_bits = 0;

			यदि (!eval(sig_s, undef, cell))
				प्रत्यागम false;

			प्रति (पूर्णाङ्क i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				यदि (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.धकेलो_back(b_slice);

				यदि (s_bit == RTLIL::State::S1)
					गिनती_set_s_bits++;
			}

			यदि (गिनती_set_s_bits == 0)
				y_candidates.धकेलो_back(sig_a);

			std::vector<RTLIL::Const> y_मूल्यावली;

			log_assert(y_candidates.size() > 0);
			प्रति (auto &yc : y_candidates) {
				यदि (!eval(yc, undef, cell))
					प्रत्यागम false;
				यदि (cell->type == ID($_NMUX_))
					y_मूल्यावली.धकेलो_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				अन्यथा
					y_मूल्यावली.धकेलो_back(yc.as_const());
			}

			यदि (y_मूल्यावली.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_मूल्यावली.at(0).to_bits();

				प्रति (size_t i = 1; i < y_मूल्यावली.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_मूल्यावली.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					प्रति (size_t j = 0; j < master_bits.size(); j++)
						यदि (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			अन्यथा
				set(sig_y, y_मूल्यावली.front());
		}
		अन्यथा यदि (cell->type == ID($bmux))
		{
			यदि (!eval(sig_s, undef, cell))
				प्रत्यागम false;

			यदि (sig_s.is_fully_def()) {
				पूर्णाङ्क sel = sig_s.as_int();
				पूर्णाङ्क width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				यदि (!eval(res, undef, cell))
					प्रत्यागम false;
				set(sig_y, res.as_const());
			} अन्यथा {
				यदि (!eval(sig_a, undef, cell))
					प्रत्यागम false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अन्यथा यदि (cell->type == ID($demux))
		{
			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;
			यदि (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} अन्यथा {
				यदि (!eval(sig_s, undef, cell))
					प्रत्यागम false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अन्यथा यदि (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			पूर्णाङ्क width = GetSize(sig_c);

			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_b, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_c, undef, cell))
				प्रत्यागम false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			प्रति (पूर्णाङ्क i = 0; i < GetSize(val_y); i++)
				यदि (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		अन्यथा यदि (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.गिनती(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.गिनती(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_b, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_ci, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_bi, undef, cell))
				प्रत्यागम false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_निवेश_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			प्रति (पूर्णाङ्क i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				यदि (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} अन्यथा {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				यदि (any_निवेश_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} अन्यथा {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		अन्यथा यदि (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			प्रति (auto &port : macc.terms) {
				यदि (!eval(port.in_a, undef, cell))
					प्रत्यागम false;
				यदि (!eval(port.in_b, undef, cell))
					प्रत्यागम false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			यदि (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		अन्यथा
		{
			RTLIL::SigSpec sig_c, sig_d;

			यदि (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				यदि (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				यदि (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			यदि (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				प्रत्यागम false;
			यदि (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				प्रत्यागम false;
			यदि (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				प्रत्यागम false;
			यदि (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				प्रत्यागम false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			यदि (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} अन्यथा
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			यदि (eval_err)
				प्रत्यागम false;

			set(sig_y, eval_ret);
		}

		प्रत्यागम true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = रिक्त)
	{
		assign_map.apply(sig);
		मूल्यावली_map.apply(sig);

		यदि (sig.is_fully_const())
			प्रत्यागम true;

		यदि (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			प्रत्यागम false;
		}

		यदि (busy_cell) {
			यदि (busy.गिनती(busy_cell) > 0) {
				undef = sig;
				प्रत्यागम false;
			}
			busy.जोड़ो(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		प्रति (auto cell : driver_cells) {
			यदि (!eval(cell, undef)) {
				यदि (busy_cell)
					busy.erase(busy_cell);
				प्रत्यागम false;
			}
		}

		यदि (busy_cell)
			busy.erase(busy_cell);

		मूल्यावली_map.apply(sig);
		यदि (sig.is_fully_const())
			प्रत्यागम true;

		यदि (defaultval != RTLIL::State::Sm) {
			प्रति (auto &bit : sig)
				यदि (bit.wire) bit = defaultval;
			प्रत्यागम true;
		}

		प्रति (auto &c : sig.chunks())
			यदि (c.wire != रिक्त)
				undef.append(c);
		प्रत्यागम false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		प्रत्यागम eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
