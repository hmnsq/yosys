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

#அந்தர்பாவ "kernel/rtlil.h"
#அந்தர்பாவ "kernel/sigtools.h"
#அந்தர்பாவ "kernel/celltypes.h"
#அந்தர்பாவ "kernel/macc.h"
#அந்தர்பாவ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
கட்டமைப்பு ConstEval
{
	RTLIL::Module *module;
	SigMap assign_map;
	SigMap மதிப்புகள்_map;
	SigPool stop_signals;
	SigSet<RTLIL::Cell*> sig2driver;
	std::set<RTLIL::Cell*> busy;
	std::vector<SigMap> அடுக்கு;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), assign_map(module), defaultval(defaultval)
	{
		auto ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		க்கு (auto &it : module->cells_) {
			எனில் (!ct.cell_known(it.second->type))
				தொடர்;
			க்கு (auto &it2 : it.second->connections())
				எனில் (ct.cell_output(it.second->type, it2.first))
					sig2driver.செருகு(assign_map(it2.second), it.second);
		}
	}

	வெறுமை clear()
	{
		மதிப்புகள்_map.clear();
		stop_signals.clear();
	}

	வெறுமை தள்ளு()
	{
		அடுக்கு.தள்ளு_back(மதிப்புகள்_map);
	}

	வெறுமை எடு()
	{
		மதிப்புகள்_map.swap(அடுக்கு.back());
		அடுக்கு.எடு_back();
	}

	வெறுமை set(RTLIL::SigSpec sig, RTLIL::Const மதிப்பு)
	{
		assign_map.apply(sig);
#ifndef NDEBUG
		RTLIL::SigSpec current_val = மதிப்புகள்_map(sig);
		க்கு (முழு i = 0; i < GetSize(current_val); i++)
			log_assert(current_val[i].wire != வெற்று || current_val[i] == மதிப்பு[i]);
#endif
		மதிப்புகள்_map.add(sig, RTLIL::SigSpec(மதிப்பு));
	}

	வெறுமை stop(RTLIL::SigSpec sig)
	{
		assign_map.apply(sig);
		stop_signals.add(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		எனில் (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->getPort(ID::P);
			RTLIL::SigSpec sig_g = cell->getPort(ID::G);
			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_co = மதிப்புகள்_map(assign_map(cell->getPort(ID::CO)));

			எனில் (sig_co.is_fully_const())
				திரும்பு true;

			எனில் (!eval(sig_p, undef, cell))
				திரும்பு false;

			எனில் (!eval(sig_g, undef, cell))
				திரும்பு false;

			எனில் (!eval(sig_ci, undef, cell))
				திரும்பு false;

			எனில் (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, GetSize(sig_co));
				bool carry = sig_ci.as_bool();

				க்கு (முழு i = 0; i < GetSize(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.set(i, carry ? State::S1 : State::S0);
				}

				set(sig_co, coval);
			}
			இல்லையெனில்
				set(sig_co, RTLIL::Const(RTLIL::Sx, GetSize(sig_co)));

			திரும்பு true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->hasPort(ID::Y));
		sig_y = மதிப்புகள்_map(assign_map(cell->getPort(ID::Y)));
		எனில் (sig_y.is_fully_const())
			திரும்பு true;

		எனில் (cell->type == ID($barrier))
		{
			எனில் (!model_barriers) {
				undef.append(sig_y);
				திரும்பு false;
			}
			sig_a = cell->getPort(ID::A);
			எனில் (!eval(sig_a, undef, cell))
				திரும்பு false;
			set(sig_y, sig_a.as_const());
			திரும்பு true;
		}

		எனில் (cell->hasPort(ID::S)) {
			sig_s = cell->getPort(ID::S);
		}

		எனில் (cell->hasPort(ID::A))
			sig_a = cell->getPort(ID::A);

		எனில் (cell->hasPort(ID::B))
			sig_b = cell->getPort(ID::B);

		எனில் (cell->type.in(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::vector<RTLIL::SigSpec> y_candidates;
			முழு மொத்தம்_set_s_bits = 0;

			எனில் (!eval(sig_s, undef, cell))
				திரும்பு false;

			க்கு (முழு i = 0; i < sig_s.size(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_slice = sig_b.extract(sig_y.size()*i, sig_y.size());

				எனில் (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.தள்ளு_back(b_slice);

				எனில் (s_bit == RTLIL::State::S1)
					மொத்தம்_set_s_bits++;
			}

			எனில் (மொத்தம்_set_s_bits == 0)
				y_candidates.தள்ளு_back(sig_a);

			std::vector<RTLIL::Const> y_மதிப்புகள்;

			log_assert(y_candidates.size() > 0);
			க்கு (auto &yc : y_candidates) {
				எனில் (!eval(yc, undef, cell))
					திரும்பு false;
				எனில் (cell->type == ID($_NMUX_))
					y_மதிப்புகள்.தள்ளு_back(RTLIL::const_not(yc.as_const(), Const(), false, false, GetSize(yc)));
				இல்லையெனில்
					y_மதிப்புகள்.தள்ளு_back(yc.as_const());
			}

			எனில் (y_மதிப்புகள்.size() > 1)
			{
				std::vector<RTLIL::State> master_bits = y_மதிப்புகள்.at(0).to_bits();

				க்கு (size_t i = 1; i < y_மதிப்புகள்.size(); i++) {
					std::vector<RTLIL::State> slave_bits = y_மதிப்புகள்.at(i).to_bits();
					log_assert(master_bits.size() == slave_bits.size());
					க்கு (size_t j = 0; j < master_bits.size(); j++)
						எனில் (master_bits[j] != slave_bits[j])
							master_bits[j] = RTLIL::State::Sx;
				}

				set(sig_y, RTLIL::Const(master_bits));
			}
			இல்லையெனில்
				set(sig_y, y_மதிப்புகள்.front());
		}
		இல்லையெனில் எனில் (cell->type == ID($bmux))
		{
			எனில் (!eval(sig_s, undef, cell))
				திரும்பு false;

			எனில் (sig_s.is_fully_def()) {
				முழு sel = sig_s.as_int();
				முழு width = GetSize(sig_y);
				SigSpec res = sig_a.extract(sel * width, width);
				எனில் (!eval(res, undef, cell))
					திரும்பு false;
				set(sig_y, res.as_const());
			} இல்லையெனில் {
				எனில் (!eval(sig_a, undef, cell))
					திரும்பு false;
				set(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		இல்லையெனில் எனில் (cell->type == ID($demux))
		{
			எனில் (!eval(sig_a, undef, cell))
				திரும்பு false;
			எனில் (sig_a.is_fully_zero()) {
				set(sig_y, Const(0, GetSize(sig_y)));
			} இல்லையெனில் {
				எனில் (!eval(sig_s, undef, cell))
					திரும்பு false;
				set(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		இல்லையெனில் எனில் (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->getPort(ID::C);
			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			முழு width = GetSize(sig_c);

			எனில் (!eval(sig_a, undef, cell))
				திரும்பு false;

			எனில் (!eval(sig_b, undef, cell))
				திரும்பு false;

			எனில் (!eval(sig_c, undef, cell))
				திரும்பு false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, width);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, width);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, width);
			RTLIL::Const val_x = const_or(t2, t3, false, false, width);

			க்கு (முழு i = 0; i < GetSize(val_y); i++)
				எனில் (val_y[i] == RTLIL::Sx)
					val_x.set(i, RTLIL::Sx);

			set(sig_y, val_y);
			set(sig_x, val_x);
		}
		இல்லையெனில் எனில் (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.மொத்தம்(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.மொத்தம்(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->getPort(ID::CI);
			RTLIL::SigSpec sig_bi = cell->getPort(ID::BI);

			எனில் (!eval(sig_a, undef, cell))
				திரும்பு false;

			எனில் (!eval(sig_b, undef, cell))
				திரும்பு false;

			எனில் (!eval(sig_ci, undef, cell))
				திரும்பு false;

			எனில் (!eval(sig_bi, undef, cell))
				திரும்பு false;

			RTLIL::SigSpec sig_x = cell->getPort(ID::X);
			RTLIL::SigSpec sig_co = cell->getPort(ID::CO);

			bool any_உள்ளீடு_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(GetSize(sig_y), signed_a);
			sig_b.extend_u0(GetSize(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			க்கு (முழு i = 0; i < GetSize(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				எனில் (!x_inputs.is_fully_def()) {
					set(sig_x[i], RTLIL::Sx);
				} இல்லையெனில் {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					set(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				எனில் (any_உள்ளீடு_undef) {
					set(sig_y[i], RTLIL::Sx);
					set(sig_co[i], RTLIL::Sx);
				} இல்லையெனில் {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					set(sig_y[i], bit_y ? State::S1 : State::S0);
					set(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		இல்லையெனில் எனில் (cell->type.in(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			க்கு (auto &port : macc.terms) {
				எனில் (!eval(port.in_a, undef, cell))
					திரும்பு false;
				எனில் (!eval(port.in_b, undef, cell))
					திரும்பு false;
			}

			RTLIL::Const result(0, GetSize(cell->getPort(ID::Y)));
			எனில் (!macc.eval(result))
				log_abort();

			set(cell->getPort(ID::Y), result);
		}
		இல்லையெனில்
		{
			RTLIL::SigSpec sig_c, sig_d;

			எனில் (cell->type.in(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				எனில் (cell->hasPort(ID::C))
					sig_c = cell->getPort(ID::C);
				எனில் (cell->hasPort(ID::D))
					sig_d = cell->getPort(ID::D);
			}

			எனில் (sig_a.size() > 0 && !eval(sig_a, undef, cell))
				திரும்பு false;
			எனில் (sig_b.size() > 0 && !eval(sig_b, undef, cell))
				திரும்பு false;
			எனில் (sig_c.size() > 0 && !eval(sig_c, undef, cell))
				திரும்பு false;
			எனில் (sig_d.size() > 0 && !eval(sig_d, undef, cell))
				திரும்பு false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			எனில் (sig_s.size() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} இல்லையெனில்
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			எனில் (eval_err)
				திரும்பு false;

			set(sig_y, eval_ret);
		}

		திரும்பு true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = வெற்று)
	{
		assign_map.apply(sig);
		மதிப்புகள்_map.apply(sig);

		எனில் (sig.is_fully_const())
			திரும்பு true;

		எனில் (stop_signals.check_any(sig)) {
			undef = stop_signals.extract(sig);
			திரும்பு false;
		}

		எனில் (busy_cell) {
			எனில் (busy.மொத்தம்(busy_cell) > 0) {
				undef = sig;
				திரும்பு false;
			}
			busy.செருகு(busy_cell);
		}

		std::set<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		க்கு (auto cell : driver_cells) {
			எனில் (!eval(cell, undef)) {
				எனில் (busy_cell)
					busy.erase(busy_cell);
				திரும்பு false;
			}
		}

		எனில் (busy_cell)
			busy.erase(busy_cell);

		மதிப்புகள்_map.apply(sig);
		எனில் (sig.is_fully_const())
			திரும்பு true;

		எனில் (defaultval != RTLIL::State::Sm) {
			க்கு (auto &bit : sig)
				எனில் (bit.wire) bit = defaultval;
			திரும்பு true;
		}

		க்கு (auto &c : sig.chunks())
			எனில் (c.wire != வெற்று)
				undef.append(c);
		திரும்பு false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		திரும்பு eval(sig, undef);
	}
};

YOSYS_NAMESPACE_END

#endif
