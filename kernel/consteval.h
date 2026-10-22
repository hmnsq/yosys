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

#अगर_अघोषित CONSTEVAL_H
#घोषणा CONSTEVAL_H

#समावेश "kernel/rtlil.h"
#समावेश "kernel/sigtools.h"
#समावेश "kernel/celltypes.h"
#समावेश "kernel/macc.h"
#समावेश "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
काष्ठा ConstEval
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
		स्वतः ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		क्रम (स्वतः &it : module->cells_) {
			अगर (!ct.cell_known(it.second->type))
				जारी;
			क्रम (स्वतः &it2 : it.second->connections())
				अगर (ct.cell_output(it.second->type, it2.first))
					sig2driver.जोड़ो(assign_map(it2.second), it.second);
		}
	}

	व्योम clear()
	{
		मूल्यावली_map.clear();
		stop_signals.clear();
	}

	व्योम धकेलो()
	{
		ढेर.धकेलो_back(मूल्यावली_map);
	}

	व्योम निकालो()
	{
		मूल्यावली_map.swap(ढेर.back());
		ढेर.निकालो_back();
	}

	व्योम set(RTLIL::SigSpec sig, RTLIL::Const मूल्य)
	{
		assign_map.apply(sig);
#अगर_अघोषित न_संशोधन
		RTLIL::SigSpec current_val = मूल्यावली_map(sig);
		क्रम (पूर्णांक i = 0; i < Get_आकार(current_val); i++)
			log_assert(current_val[i].wire != शून्य || current_val[i] == मूल्य[i]);
#पूर्ण_अगर
		मूल्यावली_map.add(sig, RTLIL::SigSpec(मूल्य));
	}

	व्योम stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		अगर (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = मूल्यावली_map(assign_map(cell->getPort(ID::CO)));

			अगर (sig_co.is_fully_const())
				वापस true;

			अगर (!eval(sig_p, undef, cell))
				वापस false;

			अगर (!eval(sig_g, undef, cell))
				वापस false;

			अगर (!eval(sig_ci, undef, cell))
				वापस false;

			अगर (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, Get_आकार(sig_co));
				bool carry = sig_ci.as_bool();

				क्रम (पूर्णांक i = 0; i < Get_आकार(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			अन्यथा
				set(sig_co, RTLIL::Const(RTLIL::Sx, Get_आकार(sig_co)));

			वापस true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = मूल्यावली_map(assign_map(cell->getPort(ID::Y)));
		अगर (sig_y.is_fully_const())
			वापस true;

		अगर (cell->type == ID($barrier))
		{
			अगर (!model_barriers) {
				undef.append(sig_y);
				वापस false;
			}
			sig_a = cell->getPort(ID::A);
			अगर (!eval(sig_a, undef, cell))
				वापस false;
			set(sig_y, sig_a.as_const());
			वापस true;
		}

		अगर (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		अगर (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		अगर (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		अगर (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			पूर्णांक गिनती_set_s_bits = 0;

			अगर (!eval(sig_s, undef, cell))
				वापस false;

			क्रम (पूर्णांक i = 0; i < sig_s.आकार(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.आकार()*i, sig_y.आकार());

				अगर (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.धकेलो_back(b_slice);

				अगर (s_bit == RTLIL::State::S1)
					गिनती_set_s_bits++;
			}

			अगर (गिनती_set_s_bits == 0)
				y_candidates.धकेलो_back(sig_a);

			std::vector<RTLIL::Const> y_मूल्यावली;

			log_assert(y_candidates.आकार() > 0);
			क्रम (स्वतः &yc : y_candidates) {
				अगर (!eval(yc, undef, cell))
					वापस false;
				अगर (cell->type == ID($_NMUX_))
					y_मूल्यावली.धकेलो_back(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_आकार(yc)));
				अन्यथा
					y_मूल्यावली.धकेलो_back(yc.as_const());
			}

			अगर (y_मूल्यावली.आकार() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_मूल्यावली.at(0).to_bits();

				क्रम (माप_प्रकार i = 1; i < y_मूल्यावली.आकार(); i++) {
					std::vector<RTLIL::State> slave_bits = y_मूल्यावली.at(i).to_bits();
					log_assert(master_bits.आकार() == slave_bits.आकार());
					क्रम (माप_प्रकार j = 0; j < master_bits.आकार(); j++)
						अगर (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			अन्यथा
				set(sig_y, y_मूल्यावली.front());
		}
		अन्यथा अगर (cell->type == ID($bmux))
		{
			अगर (!eval(sig_s, undef, cell))
				वापस false;

			अगर (sig_s.is_fully_def()) {
				पूर्णांक sel = sig_s.as_int();
				पूर्णांक width = Get_आकार(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				अगर (!eval(res, undef, cell))
					वापस false;
				set(sig_y, res.as_const());
			} अन्यथा {
				अगर (!eval(sig_a, undef, cell))
					वापस false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अन्यथा अगर (cell->type == ID($demux))
		{
			अगर (!eval(sig_a, undef, cell))
				वापस false;
			अगर (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, Get_आकार(sig_y)));
			} अन्यथा {
				अगर (!eval(sig_s, undef, cell))
					वापस false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अन्यथा अगर (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			पूर्णांक width = Get_आकार(sig_c);

			अगर (!eval(sig_a, undef, cell))
				वापस false;

			अगर (!eval(sig_b, undef, cell))
				वापस false;

			अगर (!eval(sig_c, undef, cell))
				वापस false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			क्रम (पूर्णांक i = 0; i < Get_आकार(val_y); i++)
				अगर (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		अन्यथा अगर (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.गिनती(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.गिनती(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			अगर (!eval(sig_a, undef, cell))
				वापस false;

			अगर (!eval(sig_b, undef, cell))
				वापस false;

			अगर (!eval(sig_ci, undef, cell))
				वापस false;

			अगर (!eval(sig_bi, undef, cell))
				वापस false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_निवेश_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_आकार(sig_y), signed_a);
			sig_b.extend_u0(Get_आकार(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			क्रम (पूर्णांक i = 0; i < Get_आकार(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				अगर (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} अन्यथा {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				अगर (any_निवेश_undef) {
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
		अन्यथा अगर (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			क्रम (स्वतः &port : macc.terms) {
				अगर (!eval(port.in_a, undef, cell))
					वापस false;
				अगर (!eval(port.in_b, undef, cell))
					वापस false;
			}

			RTLIL::Const परिणाम(0, Get_आकार(cell->getPort(ID::Y)));
			अगर (!macc.eval(परिणाम))
				log_abort();

			set(cell->getPort(ID::Y), परिणाम);
		}
		अन्यथा
		{
			RTLIL::SigSpec sig_c, sig_d;

			अगर (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				अगर (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				अगर (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			अगर (sig_a.आकार() > 0 && !eval(sig_a, undef, cell))
				वापस false;
			अगर (sig_b.आकार() > 0 && !eval(sig_b, undef, cell))
				वापस false;
			अगर (sig_c.आकार() > 0 && !eval(sig_c, undef, cell))
				वापस false;
			अगर (sig_d.आकार() > 0 && !eval(sig_d, undef, cell))
				वापस false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			अगर (sig_s.आकार() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} अन्यथा
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			अगर (eval_err)
				वापस false;

			set(sig_y, eval_ret);
		}

		वापस true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = शून्य)
	{
		assign_map.apply(sig);
		मूल्यावली_map.apply(sig);

		अगर (sig.is_fully_const())
			वापस true;

		अगर (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			वापस false;
		}

		अगर (busy_cell) {
			अगर (busy.गिनती(busy_cell) > 0) {
				undef = sig;
				वापस false;
			}
			busy.जोड़ो(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		क्रम (स्वतः cell : driver_cells) {
			अगर (!eval(cell, undef)) {
				अगर (busy_cell)
					busy.erase(busy_cell);
				वापस false;
			}
		}

		अगर (busy_cell)
			busy.erase(busy_cell);

		मूल्यावली_map.apply(sig);
		अगर (sig.is_fully_const())
			वापस true;

		अगर (defaultval != RTLIL::State::Sm) {
			क्रम (स्वतः &bit : sig)
				अगर (bit.wire) bit = defaultval;
			वापस true;
		}

		क्रम (स्वतः &c : sig.chunks())
			अगर (c.wire != शून्य)
				undef.append(c);
		वापस false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		वापस eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#पूर्ण_अगर
