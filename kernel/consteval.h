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

#اگر_تعریف_نشده CONSTEVAL_H
#تعریف_کن CONSTEVAL_H

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
	SigMap تخصیص_بده_map;
	SigMap مقادیر_map;
	SigPool stop_سیگنالها;
	Sig_مجموعه<RTLIL::Cell*> sig2driver;
	std::مجموعه<RTLIL::Cell*> busy;
	std::وکتور<SigMap> پشته;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), تخصیص_بده_map(module), defaultval(defaultval)
	{
		خودکار ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		لكل (خودکار &it : module->cells_) {
			إذا (!ct.cell_معلوم(it.دومی->type))
				استمر;
			لكل (خودکار &it2 : it.دومی->connections())
				إذا (ct.cell_output(it.دومی->type, it2.اولی))
					sig2driver.درج(تخصیص_بده_map(it2.دومی), it.دومی);
		}
	}

	فراغ clear()
	{
		مقادیر_map.clear();
		stop_سیگنالها.clear();
	}

	فراغ افزودن()
	{
		پشته.افزودن_پشت(مقادیر_map);
	}

	فراغ برداشت()
	{
		مقادیر_map.جابجا(پشته.پشت());
		پشته.برداشت_پشت();
	}

	فراغ مجموعه(RTLIL::SigSpec sig, RTLIL::Const مقدار)
	{
		تخصیص_بده_map.apply(sig);
#اگر_تعریف_نشده NDEBUG
		RTLIL::SigSpec current_val = مقادیر_map(sig);
		لكل (صحيح i = 0; i < Get_اندازه(current_val); i++)
			log_assert(current_val[i].wire != عدم || current_val[i] == مقدار[i]);
#پایان_اگر
		مقادیر_map.جمع_کن(sig, RTLIL::SigSpec(مقدار));
	}

	فراغ stop(RTLIL::SigSpec sig)
	{
		تخصیص_بده_map.apply(sig);
		stop_سیگنالها.جمع_کن(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		إذا (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_پورت(ID::P);
			RTLIL::SigSpec sig_g = cell->get_پورت(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_پورت(ID::CI);
			RTLIL::SigSpec sig_co = مقادیر_map(تخصیص_بده_map(cell->get_پورت(ID::CO)));

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
				bool انتقال = sig_ci.as_bool();

				لكل (صحيح i = 0; i < Get_اندازه(coval); i++) {
					انتقال = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && انتقال);
					coval.مجموعه(i, انتقال ? State::S1 : State::S0);
				}

				مجموعه(sig_co, coval);
			}
			وإلا
				مجموعه(sig_co, RTLIL::Const(RTLIL::Sx, Get_اندازه(sig_co)));

			أرجع true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_پورت(ID::Y));
		sig_y = مقادیر_map(تخصیص_بده_map(cell->get_پورت(ID::Y)));
		إذا (sig_y.is_fully_const())
			أرجع true;

		إذا (cell->type == ID($barrier))
		{
			إذا (!model_barriers) {
				undef.append(sig_y);
				أرجع false;
			}
			sig_a = cell->get_پورت(ID::A);
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			مجموعه(sig_y, sig_a.as_const());
			أرجع true;
		}

		إذا (cell->has_پورت(ID::S)) {
			sig_s = cell->get_پورت(ID::S);
		}

		إذا (cell->has_پورت(ID::A))
			sig_a = cell->get_پورت(ID::A);

		إذا (cell->has_پورت(ID::B))
			sig_b = cell->get_پورت(ID::B);

		إذا (cell->type.درون(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::وکتور<RTLIL::SigSpec> y_candidates;
			صحيح شمار_مجموعه_s_بیتها = 0;

			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			لكل (صحيح i = 0; i < sig_s.اندازه(); i++)
			{
				RTLIL::State s_بیت = sig_s.استخراج(i, 1).as_const().at(0);
				RTLIL::SigSpec b_برش = sig_b.استخراج(sig_y.اندازه()*i, sig_y.اندازه());

				إذا (s_بیت == RTLIL::State::Sx || s_بیت == RTLIL::State::S1)
					y_candidates.افزودن_پشت(b_برش);

				إذا (s_بیت == RTLIL::State::S1)
					شمار_مجموعه_s_بیتها++;
			}

			إذا (شمار_مجموعه_s_بیتها == 0)
				y_candidates.افزودن_پشت(sig_a);

			std::وکتور<RTLIL::Const> y_مقادیر;

			log_assert(y_candidates.اندازه() > 0);
			لكل (خودکار &yc : y_candidates) {
				إذا (!eval(yc, undef, cell))
					أرجع false;
				إذا (cell->type == ID($_NMUX_))
					y_مقادیر.افزودن_پشت(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_اندازه(yc)));
				وإلا
					y_مقادیر.افزودن_پشت(yc.as_const());
			}

			إذا (y_مقادیر.اندازه() > 1)
			{
				std::وکتور<RTLIL::State> master_بیتها = y_مقادیر.at(0).مقصد_بیتها();

				لكل (اندازه_t i = 1; i < y_مقادیر.اندازه(); i++) {
					std::وکتور<RTLIL::State> slave_بیتها = y_مقادیر.at(i).مقصد_بیتها();
					log_assert(master_بیتها.اندازه() == slave_بیتها.اندازه());
					لكل (اندازه_t j = 0; j < master_بیتها.اندازه(); j++)
						إذا (master_بیتها[j] != slave_بیتها[j])
							master_بیتها[j] = RTLIL::State::Sx;
				}

				مجموعه(sig_y, RTLIL::Const(master_بیتها));
			}
			وإلا
				مجموعه(sig_y, y_مقادیر.front());
		}
		وإلا إذا (cell->type == ID($bmux))
		{
			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			إذا (sig_s.is_fully_def()) {
				صحيح sel = sig_s.as_int();
				صحيح عرض = Get_اندازه(sig_y);
				SigSpec res = sig_a.استخراج(sel * عرض, عرض);
				إذا (!eval(res, undef, cell))
					أرجع false;
				مجموعه(sig_y, res.as_const());
			} وإلا {
				إذا (!eval(sig_a, undef, cell))
					أرجع false;
				مجموعه(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($demux))
		{
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			إذا (sig_a.is_fully_zero()) {
				مجموعه(sig_y, Const(0, Get_اندازه(sig_y)));
			} وإلا {
				إذا (!eval(sig_s, undef, cell))
					أرجع false;
				مجموعه(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_پورت(ID::C);
			RTLIL::SigSpec sig_x = cell->get_پورت(ID::X);
			صحيح عرض = Get_اندازه(sig_c);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_c, undef, cell))
				أرجع false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, عرض);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, عرض);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, عرض);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, عرض);
			RTLIL::Const val_x = const_or(t2, t3, false, false, عرض);

			لكل (صحيح i = 0; i < Get_اندازه(val_y); i++)
				إذا (val_y[i] == RTLIL::Sx)
					val_x.مجموعه(i, RTLIL::Sx);

			مجموعه(sig_y, val_y);
			مجموعه(sig_x, val_x);
		}
		وإلا إذا (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.شمار(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.شمار(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_پورت(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_پورت(ID::BI);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_ci, undef, cell))
				أرجع false;

			إذا (!eval(sig_bi, undef, cell))
				أرجع false;

			RTLIL::SigSpec sig_x = cell->get_پورت(ID::X);
			RTLIL::SigSpec sig_co = cell->get_پورت(ID::CO);

			bool any_درون_داد_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_اندازه(sig_y), signed_a);
			sig_b.extend_u0(Get_اندازه(sig_y), signed_b);

			bool انتقال = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			لكل (صحيح i = 0; i < Get_اندازه(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				إذا (!x_inputs.is_fully_def()) {
					مجموعه(sig_x[i], RTLIL::Sx);
				} وإلا {
					bool بیت_a = sig_a[i] == State::S1;
					bool بیت_b = (sig_b[i] == State::S1) != b_inv;
					bool بیت_x = بیت_a != بیت_b;
					مجموعه(sig_x[i], بیت_x ? State::S1 : State::S0);
				}

				إذا (any_درون_داد_undef) {
					مجموعه(sig_y[i], RTLIL::Sx);
					مجموعه(sig_co[i], RTLIL::Sx);
				} وإلا {
					bool بیت_a = sig_a[i] == State::S1;
					bool بیت_b = (sig_b[i] == State::S1) != b_inv;
					bool بیت_y = (بیت_a != بیت_b) != انتقال;
					انتقال = (بیت_a && بیت_b) || (بیت_a && انتقال) || (بیت_b && انتقال);
					مجموعه(sig_y[i], بیت_y ? State::S1 : State::S0);
					مجموعه(sig_co[i], انتقال ? State::S1 : State::S0);
				}
			}
		}
		وإلا إذا (cell->type.درون(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			لكل (خودکار &پورت : macc.terms) {
				إذا (!eval(پورت.درون_a, undef, cell))
					أرجع false;
				إذا (!eval(پورت.درون_b, undef, cell))
					أرجع false;
			}

			RTLIL::Const نتیجه(0, Get_اندازه(cell->get_پورت(ID::Y)));
			إذا (!macc.eval(نتیجه))
				log_abort();

			مجموعه(cell->get_پورت(ID::Y), نتیجه);
		}
		وإلا
		{
			RTLIL::SigSpec sig_c, sig_d;

			إذا (cell->type.درون(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				إذا (cell->has_پورت(ID::C))
					sig_c = cell->get_پورت(ID::C);
				إذا (cell->has_پورت(ID::D))
					sig_d = cell->get_پورت(ID::D);
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

			مجموعه(sig_y, eval_ret);
		}

		أرجع true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = عدم)
	{
		تخصیص_بده_map.apply(sig);
		مقادیر_map.apply(sig);

		إذا (sig.is_fully_const())
			أرجع true;

		إذا (stop_سیگنالها.check_any(sig)) {
			undef = stop_سیگنالها.استخراج(sig);
			أرجع false;
		}

		إذا (busy_cell) {
			إذا (busy.شمار(busy_cell) > 0) {
				undef = sig;
				أرجع false;
			}
			busy.درج(busy_cell);
		}

		std::مجموعه<RTLIL::Cell*> driver_cells;
		sig2driver.پیدا_کن(sig, driver_cells);
		لكل (خودکار cell : driver_cells) {
			إذا (!eval(cell, undef)) {
				إذا (busy_cell)
					busy.erase(busy_cell);
				أرجع false;
			}
		}

		إذا (busy_cell)
			busy.erase(busy_cell);

		مقادیر_map.apply(sig);
		إذا (sig.is_fully_const())
			أرجع true;

		إذا (defaultval != RTLIL::State::Sm) {
			لكل (خودکار &بیت : sig)
				إذا (بیت.wire) بیت = defaultval;
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

YOSYS_NAMESPACE_انتها

#پایان_اگر
