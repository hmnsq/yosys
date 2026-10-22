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

#که_نه_تعریف_شوی CONSTEVAL_H
#تعریف_کړه CONSTEVAL_H

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
	SigMap ټاکل_map;
	SigMap ارزښتونه_map;
	SigPool stop_سیګنالونه;
	Sig_سېټ<RTLIL::Cell*> sig2driver;
	std::سېټ<RTLIL::Cell*> busy;
	std::ویکټور<SigMap> ډېری;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), ټاکل_map(module), defaultval(defaultval)
	{
		خپلکاری ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		لكل (خپلکاری &it : module->cells_) {
			إذا (!ct.cell_معلوم(it.دوهم->type))
				استمر;
			لكل (خپلکاری &it2 : it.دوهم->connections())
				إذا (ct.cell_output(it.دوهم->type, it2.لومړی))
					sig2driver.ورننباسه(ټاکل_map(it2.دوهم), it.دوهم);
		}
	}

	فراغ clear()
	{
		ارزښتونه_map.clear();
		stop_سیګنالونه.clear();
	}

	فراغ ټېل()
	{
		ډېری.ټېل_شا(ارزښتونه_map);
	}

	فراغ راوباسه()
	{
		ارزښتونه_map.بدلون(ډېری.شا());
		ډېری.راوباسه_شا();
	}

	فراغ سېټ(RTLIL::SigSpec sig, RTLIL::Const ارزښت)
	{
		ټاکل_map.apply(sig);
#که_نه_تعریف_شوی NDEBUG
		RTLIL::SigSpec current_val = ارزښتونه_map(sig);
		لكل (صحيح i = 0; i < Get_اندازه(current_val); i++)
			log_assert(current_val[i].wire != عدم || current_val[i] == ارزښت[i]);
#د_که_پای
		ارزښتونه_map.جمع_کړه(sig, RTLIL::SigSpec(ارزښت));
	}

	فراغ stop(RTLIL::SigSpec sig)
	{
		ټاکل_map.apply(sig);
		stop_سیګنالونه.جمع_کړه(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		إذا (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_پورټ(ID::P);
			RTLIL::SigSpec sig_g = cell->get_پورټ(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_پورټ(ID::CI);
			RTLIL::SigSpec sig_co = ارزښتونه_map(ټاکل_map(cell->get_پورټ(ID::CO)));

			إذا (sig_co.is_fully_const())
				أرجع true;

			إذا (!eval(sig_p, undef, cell))
				أرجع false;

			إذا (!eval(sig_g, undef, cell))
				أرجع false;

			إذا (!eval(sig_ci, undef, cell))
				أرجع false;

			إذا (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, Get_اندازه(sig_co));
				bool لېږد = sig_ci.as_bool();

				لكل (صحيح i = 0; i < Get_اندازه(coval); i++) {
					لېږد = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && لېږد);
					coval.سېټ(i, لېږد ? State::S1 : State::S0);
				}

				سېټ(sig_co, coval);
			}
			وإلا
				سېټ(sig_co, RTLIL::Const(RTLIL::Sx, Get_اندازه(sig_co)));

			أرجع true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_پورټ(ID::Y));
		sig_y = ارزښتونه_map(ټاکل_map(cell->get_پورټ(ID::Y)));
		إذا (sig_y.is_fully_const())
			أرجع true;

		إذا (cell->type == ID($barrier))
		{
			إذا (!model_barriers) {
				undef.append(sig_y);
				أرجع false;
			}
			sig_a = cell->get_پورټ(ID::A);
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			سېټ(sig_y, sig_a.as_const());
			أرجع true;
		}

		إذا (cell->has_پورټ(ID::S)) {
			sig_s = cell->get_پورټ(ID::S);
		}

		إذا (cell->has_پورټ(ID::A))
			sig_a = cell->get_پورټ(ID::A);

		إذا (cell->has_پورټ(ID::B))
			sig_b = cell->get_پورټ(ID::B);

		إذا (cell->type.دننه(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::ویکټور<RTLIL::SigSpec> y_candidates;
			صحيح شمېر_سېټ_s_بټونه = 0;

			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			لكل (صحيح i = 0; i < sig_s.اندازه(); i++)
			{
				RTLIL::State s_بټ = sig_s.ایستل(i, 1).as_const().at(0);
				RTLIL::SigSpec b_ټوټه = sig_b.ایستل(sig_y.اندازه()*i, sig_y.اندازه());

				إذا (s_بټ == RTLIL::State::Sx || s_بټ == RTLIL::State::S1)
					y_candidates.ټېل_شا(b_ټوټه);

				إذا (s_بټ == RTLIL::State::S1)
					شمېر_سېټ_s_بټونه++;
			}

			إذا (شمېر_سېټ_s_بټونه == 0)
				y_candidates.ټېل_شا(sig_a);

			std::ویکټور<RTLIL::Const> y_ارزښتونه;

			log_assert(y_candidates.اندازه() > 0);
			لكل (خپلکاری &yc : y_candidates) {
				إذا (!eval(yc, undef, cell))
					أرجع false;
				إذا (cell->type == ID($_NMUX_))
					y_ارزښتونه.ټېل_شا(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_اندازه(yc)));
				وإلا
					y_ارزښتونه.ټېل_شا(yc.as_const());
			}

			إذا (y_ارزښتونه.اندازه() > 1)
			{
				std::ویکټور<RTLIL::State> master_بټونه = y_ارزښتونه.at(0).منزل_بټونه();

				لكل (اندازه_t i = 1; i < y_ارزښتونه.اندازه(); i++) {
					std::ویکټور<RTLIL::State> slave_بټونه = y_ارزښتونه.at(i).منزل_بټونه();
					log_assert(master_بټونه.اندازه() == slave_بټونه.اندازه());
					لكل (اندازه_t j = 0; j < master_بټونه.اندازه(); j++)
						إذا (master_بټونه[j] != slave_بټونه[j])
							master_بټونه[j] = RTLIL::State::Sx;
				}

				سېټ(sig_y, RTLIL::Const(master_بټونه));
			}
			وإلا
				سېټ(sig_y, y_ارزښتونه.front());
		}
		وإلا إذا (cell->type == ID($bmux))
		{
			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			إذا (sig_s.is_fully_def()) {
				صحيح sel = sig_s.as_int();
				صحيح پلنوالی = Get_اندازه(sig_y);
				SigSpec res = sig_a.ایستل(sel * پلنوالی, پلنوالی);
				إذا (!eval(res, undef, cell))
					أرجع false;
				سېټ(sig_y, res.as_const());
			} وإلا {
				إذا (!eval(sig_a, undef, cell))
					أرجع false;
				سېټ(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($demux))
		{
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			إذا (sig_a.is_fully_zero()) {
				سېټ(sig_y, Const(0, Get_اندازه(sig_y)));
			} وإلا {
				إذا (!eval(sig_s, undef, cell))
					أرجع false;
				سېټ(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_پورټ(ID::C);
			RTLIL::SigSpec sig_x = cell->get_پورټ(ID::X);
			صحيح پلنوالی = Get_اندازه(sig_c);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_c, undef, cell))
				أرجع false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, پلنوالی);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, پلنوالی);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, پلنوالی);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, پلنوالی);
			RTLIL::Const val_x = const_or(t2, t3, false, false, پلنوالی);

			لكل (صحيح i = 0; i < Get_اندازه(val_y); i++)
				إذا (val_y[i] == RTLIL::Sx)
					val_x.سېټ(i, RTLIL::Sx);

			سېټ(sig_y, val_y);
			سېټ(sig_x, val_x);
		}
		وإلا إذا (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.شمېر(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.شمېر(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_پورټ(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_پورټ(ID::BI);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_ci, undef, cell))
				أرجع false;

			إذا (!eval(sig_bi, undef, cell))
				أرجع false;

			RTLIL::SigSpec sig_x = cell->get_پورټ(ID::X);
			RTLIL::SigSpec sig_co = cell->get_پورټ(ID::CO);

			bool any_ننوت_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_اندازه(sig_y), signed_a);
			sig_b.extend_u0(Get_اندازه(sig_y), signed_b);

			bool لېږد = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			لكل (صحيح i = 0; i < Get_اندازه(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				إذا (!x_inputs.is_fully_def()) {
					سېټ(sig_x[i], RTLIL::Sx);
				} وإلا {
					bool بټ_a = sig_a[i] == State::S1;
					bool بټ_b = (sig_b[i] == State::S1) != b_inv;
					bool بټ_x = بټ_a != بټ_b;
					سېټ(sig_x[i], بټ_x ? State::S1 : State::S0);
				}

				إذا (any_ننوت_undef) {
					سېټ(sig_y[i], RTLIL::Sx);
					سېټ(sig_co[i], RTLIL::Sx);
				} وإلا {
					bool بټ_a = sig_a[i] == State::S1;
					bool بټ_b = (sig_b[i] == State::S1) != b_inv;
					bool بټ_y = (بټ_a != بټ_b) != لېږد;
					لېږد = (بټ_a && بټ_b) || (بټ_a && لېږد) || (بټ_b && لېږد);
					سېټ(sig_y[i], بټ_y ? State::S1 : State::S0);
					سېټ(sig_co[i], لېږد ? State::S1 : State::S0);
				}
			}
		}
		وإلا إذا (cell->type.دننه(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			لكل (خپلکاری &پورټ : macc.terms) {
				إذا (!eval(پورټ.دننه_a, undef, cell))
					أرجع false;
				إذا (!eval(پورټ.دننه_b, undef, cell))
					أرجع false;
			}

			RTLIL::Const پایله(0, Get_اندازه(cell->get_پورټ(ID::Y)));
			إذا (!macc.eval(پایله))
				log_abort();

			سېټ(cell->get_پورټ(ID::Y), پایله);
		}
		وإلا
		{
			RTLIL::SigSpec sig_c, sig_d;

			إذا (cell->type.دننه(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				إذا (cell->has_پورټ(ID::C))
					sig_c = cell->get_پورټ(ID::C);
				إذا (cell->has_پورټ(ID::D))
					sig_d = cell->get_پورټ(ID::D);
			}

			إذا (sig_a.اندازه() > 0 && !eval(sig_a, undef, cell))
				أرجع false;
			إذا (sig_b.اندازه() > 0 && !eval(sig_b, undef, cell))
				أرجع false;
			إذا (sig_c.اندازه() > 0 && !eval(sig_c, undef, cell))
				أرجع false;
			إذا (sig_d.اندازه() > 0 && !eval(sig_d, undef, cell))
				أرجع false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			إذا (sig_s.اندازه() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} وإلا
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			إذا (eval_err)
				أرجع false;

			سېټ(sig_y, eval_ret);
		}

		أرجع true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = عدم)
	{
		ټاکل_map.apply(sig);
		ارزښتونه_map.apply(sig);

		إذا (sig.is_fully_const())
			أرجع true;

		إذا (stop_سیګنالونه.check_any(sig)) {
			undef = stop_سیګنالونه.ایستل(sig);
			أرجع false;
		}

		إذا (busy_cell) {
			إذا (busy.شمېر(busy_cell) > 0) {
				undef = sig;
				أرجع false;
			}
			busy.ورننباسه(busy_cell);
		}

		std::سېټ<RTLIL::Cell*> driver_cells;
		sig2driver.پیدا_کړه(sig, driver_cells);
		لكل (خپلکاری cell : driver_cells) {
			إذا (!eval(cell, undef)) {
				إذا (busy_cell)
					busy.erase(busy_cell);
				أرجع false;
			}
		}

		إذا (busy_cell)
			busy.erase(busy_cell);

		ارزښتونه_map.apply(sig);
		إذا (sig.is_fully_const())
			أرجع true;

		إذا (defaultval != RTLIL::State::Sm) {
			لكل (خپلکاری &بټ : sig)
				إذا (بټ.wire) بټ = defaultval;
			أرجع true;
		}

		لكل (خپلکاری &c : sig.chunks())
			إذا (c.wire != عدم)
				undef.append(c);
		أرجع false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		أرجع eval(sig, undef);
	}
};

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
