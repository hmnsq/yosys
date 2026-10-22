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

#جے_بنا_تعریف CONSTEVAL_H
#تعریف_کرو CONSTEVAL_H

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
	SigMap سونپو_map;
	SigMap اقدار_map;
	SigPool stop_اشارے;
	Sig_سیٹ<RTLIL::Cell*> sig2driver;
	std::سیٹ<RTLIL::Cell*> busy;
	std::سمتیہ<SigMap> ڈھیر;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), سونپو_map(module), defaultval(defaultval)
	{
		خودکار ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		لكل (خودکار &it : module->cells_) {
			إذا (!ct.cell_معلوم(it.دوجا->type))
				استمر;
			لكل (خودکار &it2 : it.دوجا->connections())
				إذا (ct.cell_output(it.دوجا->type, it2.پہلا))
					sig2driver.داخل_کرو(سونپو_map(it2.دوجا), it.دوجا);
		}
	}

	فراغ clear()
	{
		اقدار_map.clear();
		stop_اشارے.clear();
	}

	فراغ ڈالو()
	{
		ڈھیر.ڈالو_پٹھ(اقدار_map);
	}

	فراغ نکالو()
	{
		اقدار_map.وٹاندرا(ڈھیر.پٹھ());
		ڈھیر.نکالو_پٹھ();
	}

	فراغ سیٹ(RTLIL::SigSpec sig, RTLIL::Const قدر)
	{
		سونپو_map.apply(sig);
#جے_بنا_تعریف NDEBUG
		RTLIL::SigSpec current_val = اقدار_map(sig);
		لكل (صحيح i = 0; i < Get_ناپ(current_val); i++)
			log_assert(current_val[i].wire != عدم || current_val[i] == قدر[i]);
#جے_انت
		اقدار_map.جوڑو(sig, RTLIL::SigSpec(قدر));
	}

	فراغ stop(RTLIL::SigSpec sig)
	{
		سونپو_map.apply(sig);
		stop_اشارے.جوڑو(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		إذا (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_پورٹ(ID::P);
			RTLIL::SigSpec sig_g = cell->get_پورٹ(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_پورٹ(ID::CI);
			RTLIL::SigSpec sig_co = اقدار_map(سونپو_map(cell->get_پورٹ(ID::CO)));

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
				RTLIL::Const coval(RTLIL::Sx, Get_ناپ(sig_co));
				bool حاصل = sig_ci.as_bool();

				لكل (صحيح i = 0; i < Get_ناپ(coval); i++) {
					حاصل = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && حاصل);
					coval.سیٹ(i, حاصل ? State::S1 : State::S0);
				}

				سیٹ(sig_co, coval);
			}
			وإلا
				سیٹ(sig_co, RTLIL::Const(RTLIL::Sx, Get_ناپ(sig_co)));

			أرجع true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_پورٹ(ID::Y));
		sig_y = اقدار_map(سونپو_map(cell->get_پورٹ(ID::Y)));
		إذا (sig_y.is_fully_const())
			أرجع true;

		إذا (cell->type == ID($barrier))
		{
			إذا (!model_barriers) {
				undef.append(sig_y);
				أرجع false;
			}
			sig_a = cell->get_پورٹ(ID::A);
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			سیٹ(sig_y, sig_a.as_const());
			أرجع true;
		}

		إذا (cell->has_پورٹ(ID::S)) {
			sig_s = cell->get_پورٹ(ID::S);
		}

		إذا (cell->has_پورٹ(ID::A))
			sig_a = cell->get_پورٹ(ID::A);

		إذا (cell->has_پورٹ(ID::B))
			sig_b = cell->get_پورٹ(ID::B);

		إذا (cell->type.وچ(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::سمتیہ<RTLIL::SigSpec> y_candidates;
			صحيح گنتی_سیٹ_s_بٹ = 0;

			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			لكل (صحيح i = 0; i < sig_s.ناپ(); i++)
			{
				RTLIL::State s_ثنائی_ہندسہ = sig_s.کڈھو(i, 1).as_const().at(0);
				RTLIL::SigSpec b_ٹکڑا = sig_b.کڈھو(sig_y.ناپ()*i, sig_y.ناپ());

				إذا (s_ثنائی_ہندسہ == RTLIL::State::Sx || s_ثنائی_ہندسہ == RTLIL::State::S1)
					y_candidates.ڈالو_پٹھ(b_ٹکڑا);

				إذا (s_ثنائی_ہندسہ == RTLIL::State::S1)
					گنتی_سیٹ_s_بٹ++;
			}

			إذا (گنتی_سیٹ_s_بٹ == 0)
				y_candidates.ڈالو_پٹھ(sig_a);

			std::سمتیہ<RTLIL::Const> y_اقدار;

			log_assert(y_candidates.ناپ() > 0);
			لكل (خودکار &yc : y_candidates) {
				إذا (!eval(yc, undef, cell))
					أرجع false;
				إذا (cell->type == ID($_NMUX_))
					y_اقدار.ڈالو_پٹھ(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_ناپ(yc)));
				وإلا
					y_اقدار.ڈالو_پٹھ(yc.as_const());
			}

			إذا (y_اقدار.ناپ() > 1)
			{
				std::سمتیہ<RTLIL::State> master_بٹ = y_اقدار.at(0).منزل_بٹ();

				لكل (ناپ_t i = 1; i < y_اقدار.ناپ(); i++) {
					std::سمتیہ<RTLIL::State> slave_بٹ = y_اقدار.at(i).منزل_بٹ();
					log_assert(master_بٹ.ناپ() == slave_بٹ.ناپ());
					لكل (ناپ_t j = 0; j < master_بٹ.ناپ(); j++)
						إذا (master_بٹ[j] != slave_بٹ[j])
							master_بٹ[j] = RTLIL::State::Sx;
				}

				سیٹ(sig_y, RTLIL::Const(master_بٹ));
			}
			وإلا
				سیٹ(sig_y, y_اقدار.front());
		}
		وإلا إذا (cell->type == ID($bmux))
		{
			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			إذا (sig_s.is_fully_def()) {
				صحيح sel = sig_s.as_int();
				صحيح چوڑائی = Get_ناپ(sig_y);
				SigSpec res = sig_a.کڈھو(sel * چوڑائی, چوڑائی);
				إذا (!eval(res, undef, cell))
					أرجع false;
				سیٹ(sig_y, res.as_const());
			} وإلا {
				إذا (!eval(sig_a, undef, cell))
					أرجع false;
				سیٹ(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($demux))
		{
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			إذا (sig_a.is_fully_zero()) {
				سیٹ(sig_y, Const(0, Get_ناپ(sig_y)));
			} وإلا {
				إذا (!eval(sig_s, undef, cell))
					أرجع false;
				سیٹ(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_پورٹ(ID::C);
			RTLIL::SigSpec sig_x = cell->get_پورٹ(ID::X);
			صحيح چوڑائی = Get_ناپ(sig_c);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_c, undef, cell))
				أرجع false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, چوڑائی);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, چوڑائی);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, چوڑائی);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, چوڑائی);
			RTLIL::Const val_x = const_or(t2, t3, false, false, چوڑائی);

			لكل (صحيح i = 0; i < Get_ناپ(val_y); i++)
				إذا (val_y[i] == RTLIL::Sx)
					val_x.سیٹ(i, RTLIL::Sx);

			سیٹ(sig_y, val_y);
			سیٹ(sig_x, val_x);
		}
		وإلا إذا (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.گنتی(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.گنتی(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_پورٹ(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_پورٹ(ID::BI);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_ci, undef, cell))
				أرجع false;

			إذا (!eval(sig_bi, undef, cell))
				أرجع false;

			RTLIL::SigSpec sig_x = cell->get_پورٹ(ID::X);
			RTLIL::SigSpec sig_co = cell->get_پورٹ(ID::CO);

			bool any_داخلہ_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_ناپ(sig_y), signed_a);
			sig_b.extend_u0(Get_ناپ(sig_y), signed_b);

			bool حاصل = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			لكل (صحيح i = 0; i < Get_ناپ(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				إذا (!x_inputs.is_fully_def()) {
					سیٹ(sig_x[i], RTLIL::Sx);
				} وإلا {
					bool ثنائی_ہندسہ_a = sig_a[i] == State::S1;
					bool ثنائی_ہندسہ_b = (sig_b[i] == State::S1) != b_inv;
					bool ثنائی_ہندسہ_x = ثنائی_ہندسہ_a != ثنائی_ہندسہ_b;
					سیٹ(sig_x[i], ثنائی_ہندسہ_x ? State::S1 : State::S0);
				}

				إذا (any_داخلہ_undef) {
					سیٹ(sig_y[i], RTLIL::Sx);
					سیٹ(sig_co[i], RTLIL::Sx);
				} وإلا {
					bool ثنائی_ہندسہ_a = sig_a[i] == State::S1;
					bool ثنائی_ہندسہ_b = (sig_b[i] == State::S1) != b_inv;
					bool ثنائی_ہندسہ_y = (ثنائی_ہندسہ_a != ثنائی_ہندسہ_b) != حاصل;
					حاصل = (ثنائی_ہندسہ_a && ثنائی_ہندسہ_b) || (ثنائی_ہندسہ_a && حاصل) || (ثنائی_ہندسہ_b && حاصل);
					سیٹ(sig_y[i], ثنائی_ہندسہ_y ? State::S1 : State::S0);
					سیٹ(sig_co[i], حاصل ? State::S1 : State::S0);
				}
			}
		}
		وإلا إذا (cell->type.وچ(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			لكل (خودکار &پورٹ : macc.terms) {
				إذا (!eval(پورٹ.وچ_a, undef, cell))
					أرجع false;
				إذا (!eval(پورٹ.وچ_b, undef, cell))
					أرجع false;
			}

			RTLIL::Const نتیجہ(0, Get_ناپ(cell->get_پورٹ(ID::Y)));
			إذا (!macc.eval(نتیجہ))
				log_abort();

			سیٹ(cell->get_پورٹ(ID::Y), نتیجہ);
		}
		وإلا
		{
			RTLIL::SigSpec sig_c, sig_d;

			إذا (cell->type.وچ(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				إذا (cell->has_پورٹ(ID::C))
					sig_c = cell->get_پورٹ(ID::C);
				إذا (cell->has_پورٹ(ID::D))
					sig_d = cell->get_پورٹ(ID::D);
			}

			إذا (sig_a.ناپ() > 0 && !eval(sig_a, undef, cell))
				أرجع false;
			إذا (sig_b.ناپ() > 0 && !eval(sig_b, undef, cell))
				أرجع false;
			إذا (sig_c.ناپ() > 0 && !eval(sig_c, undef, cell))
				أرجع false;
			إذا (sig_d.ناپ() > 0 && !eval(sig_d, undef, cell))
				أرجع false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			إذا (sig_s.ناپ() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} وإلا
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			إذا (eval_err)
				أرجع false;

			سیٹ(sig_y, eval_ret);
		}

		أرجع true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = عدم)
	{
		سونپو_map.apply(sig);
		اقدار_map.apply(sig);

		إذا (sig.is_fully_const())
			أرجع true;

		إذا (stop_اشارے.check_any(sig)) {
			undef = stop_اشارے.کڈھو(sig);
			أرجع false;
		}

		إذا (busy_cell) {
			إذا (busy.گنتی(busy_cell) > 0) {
				undef = sig;
				أرجع false;
			}
			busy.داخل_کرو(busy_cell);
		}

		std::سیٹ<RTLIL::Cell*> driver_cells;
		sig2driver.لبھو(sig, driver_cells);
		لكل (خودکار cell : driver_cells) {
			إذا (!eval(cell, undef)) {
				إذا (busy_cell)
					busy.erase(busy_cell);
				أرجع false;
			}
		}

		إذا (busy_cell)
			busy.erase(busy_cell);

		اقدار_map.apply(sig);
		إذا (sig.is_fully_const())
			أرجع true;

		إذا (defaultval != RTLIL::State::Sm) {
			لكل (خودکار &ثنائی_ہندسہ : sig)
				إذا (ثنائی_ہندسہ.wire) ثنائی_ہندسہ = defaultval;
			أرجع true;
		}

		لكل (خودکار &c : sig.chunks())
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

YOSYS_NAMESPACE_سرا

#جے_انت
