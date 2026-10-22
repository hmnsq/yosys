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

#నిర్వచితం_కాకపోతే CONSTEVAL_H
#నిర్వచించు CONSTEVAL_H

#అన్తర్భావ "kernel/rtlil.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/celltypes.h"
#అన్తర్భావ "kernel/macc.h"
#అన్తర్భావ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
నిర్మాణం ConstEval
{
	RTLIL::Module *module;
	SigMap కేటాయించు_map;
	SigMap విలువలు_map;
	SigPool stop_సంకేతాలు;
	Sig_సమితి<RTLIL::Cell*> sig2driver;
	std::సమితి<RTLIL::Cell*> busy;
	std::సదిశ<SigMap> దొంతర;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), కేటాయించు_map(module), defaultval(defaultval)
	{
		స్వయంచాలక ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		కొరకు (స్వయంచాలక &it : module->cells_) {
			ఒకవేళ (!ct.cell_తెలిసిన(it.రెండవ->type))
				కొనసాగు;
			కొరకు (స్వయంచాలక &it2 : it.రెండవ->connections())
				ఒకవేళ (ct.cell_output(it.రెండవ->type, it2.మొదటి))
					sig2driver.చొప్పించు(కేటాయించు_map(it2.రెండవ), it.రెండవ);
		}
	}

	శూన్య clear()
	{
		విలువలు_map.clear();
		stop_సంకేతాలు.clear();
	}

	శూన్య నెట్టు()
	{
		దొంతర.నెట్టు_వెనుక(విలువలు_map);
	}

	శూన్య తీయి()
	{
		విలువలు_map.మార్పిడి(దొంతర.వెనుక());
		దొంతర.తీయి_వెనుక();
	}

	శూన్య సమితి(RTLIL::SigSpec sig, RTLIL::Const విలువ)
	{
		కేటాయించు_map.apply(sig);
#నిర్వచితం_కాకపోతే NDEBUG
		RTLIL::SigSpec current_val = విలువలు_map(sig);
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(current_val); i++)
			log_assert(current_val[i].wire != ఖాళీ || current_val[i] == విలువ[i]);
#అయితే_ముగింపు
		విలువలు_map.కూడు(sig, RTLIL::SigSpec(విలువ));
	}

	శూన్య stop(RTLIL::SigSpec sig)
	{
		కేటాయించు_map.apply(sig);
		stop_సంకేతాలు.కూడు(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		ఒకవేళ (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_పోర్టు(ID::P);
			RTLIL::SigSpec sig_g = cell->get_పోర్టు(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_పోర్టు(ID::CI);
			RTLIL::SigSpec sig_co = విలువలు_map(కేటాయించు_map(cell->get_పోర్టు(ID::CO)));

			ఒకవేళ (sig_co.is_fully_const())
				తిరిగి true;

			ఒకవేళ (!eval(sig_p, undef, cell))
				తిరిగి false;

			ఒకవేళ (!eval(sig_g, undef, cell))
				తిరిగి false;

			ఒకవేళ (!eval(sig_ci, undef, cell))
				తిరిగి false;

			ఒకవేళ (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, Get_కొలత(sig_co));
				bool వాహకం = sig_ci.as_bool();

				కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(coval); i++) {
					వాహకం = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && వాహకం);
					coval.సమితి(i, వాహకం ? State::S1 : State::S0);
				}

				సమితి(sig_co, coval);
			}
			లేకపోతే
				సమితి(sig_co, RTLIL::Const(RTLIL::Sx, Get_కొలత(sig_co)));

			తిరిగి true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_పోర్టు(ID::Y));
		sig_y = విలువలు_map(కేటాయించు_map(cell->get_పోర్టు(ID::Y)));
		ఒకవేళ (sig_y.is_fully_const())
			తిరిగి true;

		ఒకవేళ (cell->type == ID($barrier))
		{
			ఒకవేళ (!model_barriers) {
				undef.append(sig_y);
				తిరిగి false;
			}
			sig_a = cell->get_పోర్టు(ID::A);
			ఒకవేళ (!eval(sig_a, undef, cell))
				తిరిగి false;
			సమితి(sig_y, sig_a.as_const());
			తిరిగి true;
		}

		ఒకవేళ (cell->has_పోర్టు(ID::S)) {
			sig_s = cell->get_పోర్టు(ID::S);
		}

		ఒకవేళ (cell->has_పోర్టు(ID::A))
			sig_a = cell->get_పోర్టు(ID::A);

		ఒకవేళ (cell->has_పోర్టు(ID::B))
			sig_b = cell->get_పోర్టు(ID::B);

		ఒకవేళ (cell->type.లోపల(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::సదిశ<RTLIL::SigSpec> y_candidates;
			పూర్ణాంకం లెక్క_సమితి_s_బిట్లు = 0;

			ఒకవేళ (!eval(sig_s, undef, cell))
				తిరిగి false;

			కొరకు (పూర్ణాంకం i = 0; i < sig_s.కొలత(); i++)
			{
				RTLIL::State s_బిట్టు = sig_s.వెలికితీయి(i, 1).as_const().at(0);
				RTLIL::SigSpec b_ముక్క = sig_b.వెలికితీయి(sig_y.కొలత()*i, sig_y.కొలత());

				ఒకవేళ (s_బిట్టు == RTLIL::State::Sx || s_బిట్టు == RTLIL::State::S1)
					y_candidates.నెట్టు_వెనుక(b_ముక్క);

				ఒకవేళ (s_బిట్టు == RTLIL::State::S1)
					లెక్క_సమితి_s_బిట్లు++;
			}

			ఒకవేళ (లెక్క_సమితి_s_బిట్లు == 0)
				y_candidates.నెట్టు_వెనుక(sig_a);

			std::సదిశ<RTLIL::Const> y_విలువలు;

			log_assert(y_candidates.కొలత() > 0);
			కొరకు (స్వయంచాలక &yc : y_candidates) {
				ఒకవేళ (!eval(yc, undef, cell))
					తిరిగి false;
				ఒకవేళ (cell->type == ID($_NMUX_))
					y_విలువలు.నెట్టు_వెనుక(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_కొలత(yc)));
				లేకపోతే
					y_విలువలు.నెట్టు_వెనుక(yc.as_const());
			}

			ఒకవేళ (y_విలువలు.కొలత() > 1)
			{
				std::సదిశ<RTLIL::State> master_బిట్లు = y_విలువలు.at(0).గమ్యం_బిట్లు();

				కొరకు (కొలత_t i = 1; i < y_విలువలు.కొలత(); i++) {
					std::సదిశ<RTLIL::State> slave_బిట్లు = y_విలువలు.at(i).గమ్యం_బిట్లు();
					log_assert(master_బిట్లు.కొలత() == slave_బిట్లు.కొలత());
					కొరకు (కొలత_t j = 0; j < master_బిట్లు.కొలత(); j++)
						ఒకవేళ (master_బిట్లు[j] != slave_బిట్లు[j])
							master_బిట్లు[j] = RTLIL::State::Sx;
				}

				సమితి(sig_y, RTLIL::Const(master_బిట్లు));
			}
			లేకపోతే
				సమితి(sig_y, y_విలువలు.front());
		}
		లేకపోతే ఒకవేళ (cell->type == ID($bmux))
		{
			ఒకవేళ (!eval(sig_s, undef, cell))
				తిరిగి false;

			ఒకవేళ (sig_s.is_fully_def()) {
				పూర్ణాంకం sel = sig_s.as_int();
				పూర్ణాంకం వెడల్పు = Get_కొలత(sig_y);
				SigSpec res = sig_a.వెలికితీయి(sel * వెడల్పు, వెడల్పు);
				ఒకవేళ (!eval(res, undef, cell))
					తిరిగి false;
				సమితి(sig_y, res.as_const());
			} లేకపోతే {
				ఒకవేళ (!eval(sig_a, undef, cell))
					తిరిగి false;
				సమితి(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		లేకపోతే ఒకవేళ (cell->type == ID($demux))
		{
			ఒకవేళ (!eval(sig_a, undef, cell))
				తిరిగి false;
			ఒకవేళ (sig_a.is_fully_zero()) {
				సమితి(sig_y, Const(0, Get_కొలత(sig_y)));
			} లేకపోతే {
				ఒకవేళ (!eval(sig_s, undef, cell))
					తిరిగి false;
				సమితి(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		లేకపోతే ఒకవేళ (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_పోర్టు(ID::C);
			RTLIL::SigSpec sig_x = cell->get_పోర్టు(ID::X);
			పూర్ణాంకం వెడల్పు = Get_కొలత(sig_c);

			ఒకవేళ (!eval(sig_a, undef, cell))
				తిరిగి false;

			ఒకవేళ (!eval(sig_b, undef, cell))
				తిరిగి false;

			ఒకవేళ (!eval(sig_c, undef, cell))
				తిరిగి false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, వెడల్పు);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, వెడల్పు);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, వెడల్పు);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, వెడల్పు);
			RTLIL::Const val_x = const_or(t2, t3, false, false, వెడల్పు);

			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(val_y); i++)
				ఒకవేళ (val_y[i] == RTLIL::Sx)
					val_x.సమితి(i, RTLIL::Sx);

			సమితి(sig_y, val_y);
			సమితి(sig_x, val_x);
		}
		లేకపోతే ఒకవేళ (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.లెక్క(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.లెక్క(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_పోర్టు(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_పోర్టు(ID::BI);

			ఒకవేళ (!eval(sig_a, undef, cell))
				తిరిగి false;

			ఒకవేళ (!eval(sig_b, undef, cell))
				తిరిగి false;

			ఒకవేళ (!eval(sig_ci, undef, cell))
				తిరిగి false;

			ఒకవేళ (!eval(sig_bi, undef, cell))
				తిరిగి false;

			RTLIL::SigSpec sig_x = cell->get_పోర్టు(ID::X);
			RTLIL::SigSpec sig_co = cell->get_పోర్టు(ID::CO);

			bool any_ఆదానం_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_కొలత(sig_y), signed_a);
			sig_b.extend_u0(Get_కొలత(sig_y), signed_b);

			bool వాహకం = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				ఒకవేళ (!x_inputs.is_fully_def()) {
					సమితి(sig_x[i], RTLIL::Sx);
				} లేకపోతే {
					bool బిట్టు_a = sig_a[i] == State::S1;
					bool బిట్టు_b = (sig_b[i] == State::S1) != b_inv;
					bool బిట్టు_x = బిట్టు_a != బిట్టు_b;
					సమితి(sig_x[i], బిట్టు_x ? State::S1 : State::S0);
				}

				ఒకవేళ (any_ఆదానం_undef) {
					సమితి(sig_y[i], RTLIL::Sx);
					సమితి(sig_co[i], RTLIL::Sx);
				} లేకపోతే {
					bool బిట్టు_a = sig_a[i] == State::S1;
					bool బిట్టు_b = (sig_b[i] == State::S1) != b_inv;
					bool బిట్టు_y = (బిట్టు_a != బిట్టు_b) != వాహకం;
					వాహకం = (బిట్టు_a && బిట్టు_b) || (బిట్టు_a && వాహకం) || (బిట్టు_b && వాహకం);
					సమితి(sig_y[i], బిట్టు_y ? State::S1 : State::S0);
					సమితి(sig_co[i], వాహకం ? State::S1 : State::S0);
				}
			}
		}
		లేకపోతే ఒకవేళ (cell->type.లోపల(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			కొరకు (స్వయంచాలక &పోర్టు : macc.terms) {
				ఒకవేళ (!eval(పోర్టు.లోపల_a, undef, cell))
					తిరిగి false;
				ఒకవేళ (!eval(పోర్టు.లోపల_b, undef, cell))
					తిరిగి false;
			}

			RTLIL::Const ఫలితం(0, Get_కొలత(cell->get_పోర్టు(ID::Y)));
			ఒకవేళ (!macc.eval(ఫలితం))
				log_abort();

			సమితి(cell->get_పోర్టు(ID::Y), ఫలితం);
		}
		లేకపోతే
		{
			RTLIL::SigSpec sig_c, sig_d;

			ఒకవేళ (cell->type.లోపల(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				ఒకవేళ (cell->has_పోర్టు(ID::C))
					sig_c = cell->get_పోర్టు(ID::C);
				ఒకవేళ (cell->has_పోర్టు(ID::D))
					sig_d = cell->get_పోర్టు(ID::D);
			}

			ఒకవేళ (sig_a.కొలత() > 0 && !eval(sig_a, undef, cell))
				తిరిగి false;
			ఒకవేళ (sig_b.కొలత() > 0 && !eval(sig_b, undef, cell))
				తిరిగి false;
			ఒకవేళ (sig_c.కొలత() > 0 && !eval(sig_c, undef, cell))
				తిరిగి false;
			ఒకవేళ (sig_d.కొలత() > 0 && !eval(sig_d, undef, cell))
				తిరిగి false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			ఒకవేళ (sig_s.కొలత() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} లేకపోతే
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			ఒకవేళ (eval_err)
				తిరిగి false;

			సమితి(sig_y, eval_ret);
		}

		తిరిగి true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = ఖాళీ)
	{
		కేటాయించు_map.apply(sig);
		విలువలు_map.apply(sig);

		ఒకవేళ (sig.is_fully_const())
			తిరిగి true;

		ఒకవేళ (stop_సంకేతాలు.check_any(sig)) {
			undef = stop_సంకేతాలు.వెలికితీయి(sig);
			తిరిగి false;
		}

		ఒకవేళ (busy_cell) {
			ఒకవేళ (busy.లెక్క(busy_cell) > 0) {
				undef = sig;
				తిరిగి false;
			}
			busy.చొప్పించు(busy_cell);
		}

		std::సమితి<RTLIL::Cell*> driver_cells;
		sig2driver.కనుగొను(sig, driver_cells);
		కొరకు (స్వయంచాలక cell : driver_cells) {
			ఒకవేళ (!eval(cell, undef)) {
				ఒకవేళ (busy_cell)
					busy.erase(busy_cell);
				తిరిగి false;
			}
		}

		ఒకవేళ (busy_cell)
			busy.erase(busy_cell);

		విలువలు_map.apply(sig);
		ఒకవేళ (sig.is_fully_const())
			తిరిగి true;

		ఒకవేళ (defaultval != RTLIL::State::Sm) {
			కొరకు (స్వయంచాలక &బిట్టు : sig)
				ఒకవేళ (బిట్టు.wire) బిట్టు = defaultval;
			తిరిగి true;
		}

		కొరకు (స్వయంచాలక &c : sig.chunks())
			ఒకవేళ (c.wire != ఖాళీ)
				undef.append(c);
		తిరిగి false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		తిరిగి eval(sig, undef);
	}
};

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
