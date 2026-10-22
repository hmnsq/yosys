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

#اگر_غیر_تعریف_شدہ CONSTEVAL_H
#تعریف_کرو CONSTEVAL_H

#شامل "kernel/rtlil.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/celltypes.h"
#شامل "kernel/macc.h"
#شامل "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
ساخت ConstEval
{
	RTLIL::Module *module;
	SigMap تفویض_map;
	SigMap اقدار_map;
	SigPool stop_اشارات;
	Sig_سیٹ<RTLIL::Cell*> sig2driver;
	std::سیٹ<RTLIL::Cell*> busy;
	std::سمتیہ<SigMap> ڈھیر;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), تفویض_map(module), defaultval(defaultval)
	{
		خودکار ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		برائے (خودکار &it : module->cells_) {
			اگر (!ct.cell_معلوم(it.دوسرا->type))
				جاری;
			برائے (خودکار &it2 : it.دوسرا->connections())
				اگر (ct.cell_output(it.دوسرا->type, it2.پہلا))
					sig2driver.داخل_کرو(تفویض_map(it2.دوسرا), it.دوسرا);
		}
	}

	خالی clear()
	{
		اقدار_map.clear();
		stop_اشارات.clear();
	}

	خالی ڈالو()
	{
		ڈھیر.ڈالو_عقب(اقدار_map);
	}

	خالی نکالو()
	{
		اقدار_map.تبادلہ(ڈھیر.عقب());
		ڈھیر.نکالو_عقب();
	}

	خالی سیٹ(RTLIL::SigSpec sig, RTLIL::Const قدر)
	{
		تفویض_map.apply(sig);
#اگر_غیر_تعریف_شدہ NDEBUG
		RTLIL::SigSpec current_val = اقدار_map(sig);
		برائے (صحیح i = 0; i < Get_سائز(current_val); i++)
			log_assert(current_val[i].wire != تہی || current_val[i] == قدر[i]);
#اگر_ختم
		اقدار_map.جوڑو(sig, RTLIL::SigSpec(قدر));
	}

	خالی stop(RTLIL::SigSpec sig)
	{
		تفویض_map.apply(sig);
		stop_اشارات.جوڑو(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		اگر (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_پورٹ(ID::P);
			RTLIL::SigSpec sig_g = cell->get_پورٹ(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_پورٹ(ID::CI);
			RTLIL::SigSpec sig_co = اقدار_map(تفویض_map(cell->get_پورٹ(ID::CO)));

			اگر (sig_co.is_fully_const())
				واپس true;

			اگر (!eval(sig_p, undef, cell))
				واپس false;

			اگر (!eval(sig_g, undef, cell))
				واپس false;

			اگر (!eval(sig_ci, undef, cell))
				واپس false;

			اگر (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, Get_سائز(sig_co));
				bool حاصل = sig_ci.as_bool();

				برائے (صحیح i = 0; i < Get_سائز(coval); i++) {
					حاصل = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && حاصل);
					coval.سیٹ(i, حاصل ? State::S1 : State::S0);
				}

				سیٹ(sig_co, coval);
			}
			ورنہ
				سیٹ(sig_co, RTLIL::Const(RTLIL::Sx, Get_سائز(sig_co)));

			واپس true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_پورٹ(ID::Y));
		sig_y = اقدار_map(تفویض_map(cell->get_پورٹ(ID::Y)));
		اگر (sig_y.is_fully_const())
			واپس true;

		اگر (cell->type == ID($barrier))
		{
			اگر (!model_barriers) {
				undef.append(sig_y);
				واپس false;
			}
			sig_a = cell->get_پورٹ(ID::A);
			اگر (!eval(sig_a, undef, cell))
				واپس false;
			سیٹ(sig_y, sig_a.as_const());
			واپس true;
		}

		اگر (cell->has_پورٹ(ID::S)) {
			sig_s = cell->get_پورٹ(ID::S);
		}

		اگر (cell->has_پورٹ(ID::A))
			sig_a = cell->get_پورٹ(ID::A);

		اگر (cell->has_پورٹ(ID::B))
			sig_b = cell->get_پورٹ(ID::B);

		اگر (cell->type.اندر(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::سمتیہ<RTLIL::SigSpec> y_candidates;
			صحیح گنتی_سیٹ_s_بٹ = 0;

			اگر (!eval(sig_s, undef, cell))
				واپس false;

			برائے (صحیح i = 0; i < sig_s.سائز(); i++)
			{
				RTLIL::State s_ثنائی_ہندسہ = sig_s.نکالو_2(i, 1).as_const().at(0);
				RTLIL::SigSpec b_قاش = sig_b.نکالو_2(sig_y.سائز()*i, sig_y.سائز());

				اگر (s_ثنائی_ہندسہ == RTLIL::State::Sx || s_ثنائی_ہندسہ == RTLIL::State::S1)
					y_candidates.ڈالو_عقب(b_قاش);

				اگر (s_ثنائی_ہندسہ == RTLIL::State::S1)
					گنتی_سیٹ_s_بٹ++;
			}

			اگر (گنتی_سیٹ_s_بٹ == 0)
				y_candidates.ڈالو_عقب(sig_a);

			std::سمتیہ<RTLIL::Const> y_اقدار;

			log_assert(y_candidates.سائز() > 0);
			برائے (خودکار &yc : y_candidates) {
				اگر (!eval(yc, undef, cell))
					واپس false;
				اگر (cell->type == ID($_NMUX_))
					y_اقدار.ڈالو_عقب(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_سائز(yc)));
				ورنہ
					y_اقدار.ڈالو_عقب(yc.as_const());
			}

			اگر (y_اقدار.سائز() > 1)
			{
				std::سمتیہ<RTLIL::State> master_بٹ = y_اقدار.at(0).مقصد_بٹ();

				برائے (سائز_t i = 1; i < y_اقدار.سائز(); i++) {
					std::سمتیہ<RTLIL::State> slave_بٹ = y_اقدار.at(i).مقصد_بٹ();
					log_assert(master_بٹ.سائز() == slave_بٹ.سائز());
					برائے (سائز_t j = 0; j < master_بٹ.سائز(); j++)
						اگر (master_بٹ[j] != slave_بٹ[j])
							master_بٹ[j] = RTLIL::State::Sx;
				}

				سیٹ(sig_y, RTLIL::Const(master_بٹ));
			}
			ورنہ
				سیٹ(sig_y, y_اقدار.front());
		}
		ورنہ اگر (cell->type == ID($bmux))
		{
			اگر (!eval(sig_s, undef, cell))
				واپس false;

			اگر (sig_s.is_fully_def()) {
				صحیح sel = sig_s.as_int();
				صحیح چوڑائی = Get_سائز(sig_y);
				SigSpec res = sig_a.نکالو_2(sel * چوڑائی, چوڑائی);
				اگر (!eval(res, undef, cell))
					واپس false;
				سیٹ(sig_y, res.as_const());
			} ورنہ {
				اگر (!eval(sig_a, undef, cell))
					واپس false;
				سیٹ(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ورنہ اگر (cell->type == ID($demux))
		{
			اگر (!eval(sig_a, undef, cell))
				واپس false;
			اگر (sig_a.is_fully_zero()) {
				سیٹ(sig_y, Const(0, Get_سائز(sig_y)));
			} ورنہ {
				اگر (!eval(sig_s, undef, cell))
					واپس false;
				سیٹ(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ورنہ اگر (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_پورٹ(ID::C);
			RTLIL::SigSpec sig_x = cell->get_پورٹ(ID::X);
			صحیح چوڑائی = Get_سائز(sig_c);

			اگر (!eval(sig_a, undef, cell))
				واپس false;

			اگر (!eval(sig_b, undef, cell))
				واپس false;

			اگر (!eval(sig_c, undef, cell))
				واپس false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, چوڑائی);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, چوڑائی);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, چوڑائی);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, چوڑائی);
			RTLIL::Const val_x = const_or(t2, t3, false, false, چوڑائی);

			برائے (صحیح i = 0; i < Get_سائز(val_y); i++)
				اگر (val_y[i] == RTLIL::Sx)
					val_x.سیٹ(i, RTLIL::Sx);

			سیٹ(sig_y, val_y);
			سیٹ(sig_x, val_x);
		}
		ورنہ اگر (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.گنتی(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.گنتی(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_پورٹ(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_پورٹ(ID::BI);

			اگر (!eval(sig_a, undef, cell))
				واپس false;

			اگر (!eval(sig_b, undef, cell))
				واپس false;

			اگر (!eval(sig_ci, undef, cell))
				واپس false;

			اگر (!eval(sig_bi, undef, cell))
				واپس false;

			RTLIL::SigSpec sig_x = cell->get_پورٹ(ID::X);
			RTLIL::SigSpec sig_co = cell->get_پورٹ(ID::CO);

			bool any_داخلہ_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_سائز(sig_y), signed_a);
			sig_b.extend_u0(Get_سائز(sig_y), signed_b);

			bool حاصل = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			برائے (صحیح i = 0; i < Get_سائز(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				اگر (!x_inputs.is_fully_def()) {
					سیٹ(sig_x[i], RTLIL::Sx);
				} ورنہ {
					bool ثنائی_ہندسہ_a = sig_a[i] == State::S1;
					bool ثنائی_ہندسہ_b = (sig_b[i] == State::S1) != b_inv;
					bool ثنائی_ہندسہ_x = ثنائی_ہندسہ_a != ثنائی_ہندسہ_b;
					سیٹ(sig_x[i], ثنائی_ہندسہ_x ? State::S1 : State::S0);
				}

				اگر (any_داخلہ_undef) {
					سیٹ(sig_y[i], RTLIL::Sx);
					سیٹ(sig_co[i], RTLIL::Sx);
				} ورنہ {
					bool ثنائی_ہندسہ_a = sig_a[i] == State::S1;
					bool ثنائی_ہندسہ_b = (sig_b[i] == State::S1) != b_inv;
					bool ثنائی_ہندسہ_y = (ثنائی_ہندسہ_a != ثنائی_ہندسہ_b) != حاصل;
					حاصل = (ثنائی_ہندسہ_a && ثنائی_ہندسہ_b) || (ثنائی_ہندسہ_a && حاصل) || (ثنائی_ہندسہ_b && حاصل);
					سیٹ(sig_y[i], ثنائی_ہندسہ_y ? State::S1 : State::S0);
					سیٹ(sig_co[i], حاصل ? State::S1 : State::S0);
				}
			}
		}
		ورنہ اگر (cell->type.اندر(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			برائے (خودکار &پورٹ : macc.terms) {
				اگر (!eval(پورٹ.اندر_a, undef, cell))
					واپس false;
				اگر (!eval(پورٹ.اندر_b, undef, cell))
					واپس false;
			}

			RTLIL::Const نتیجہ(0, Get_سائز(cell->get_پورٹ(ID::Y)));
			اگر (!macc.eval(نتیجہ))
				log_abort();

			سیٹ(cell->get_پورٹ(ID::Y), نتیجہ);
		}
		ورنہ
		{
			RTLIL::SigSpec sig_c, sig_d;

			اگر (cell->type.اندر(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				اگر (cell->has_پورٹ(ID::C))
					sig_c = cell->get_پورٹ(ID::C);
				اگر (cell->has_پورٹ(ID::D))
					sig_d = cell->get_پورٹ(ID::D);
			}

			اگر (sig_a.سائز() > 0 && !eval(sig_a, undef, cell))
				واپس false;
			اگر (sig_b.سائز() > 0 && !eval(sig_b, undef, cell))
				واپس false;
			اگر (sig_c.سائز() > 0 && !eval(sig_c, undef, cell))
				واپس false;
			اگر (sig_d.سائز() > 0 && !eval(sig_d, undef, cell))
				واپس false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			اگر (sig_s.سائز() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} ورنہ
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			اگر (eval_err)
				واپس false;

			سیٹ(sig_y, eval_ret);
		}

		واپس true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = تہی)
	{
		تفویض_map.apply(sig);
		اقدار_map.apply(sig);

		اگر (sig.is_fully_const())
			واپس true;

		اگر (stop_اشارات.check_any(sig)) {
			undef = stop_اشارات.نکالو_2(sig);
			واپس false;
		}

		اگر (busy_cell) {
			اگر (busy.گنتی(busy_cell) > 0) {
				undef = sig;
				واپس false;
			}
			busy.داخل_کرو(busy_cell);
		}

		std::سیٹ<RTLIL::Cell*> driver_cells;
		sig2driver.ڈھونڈو(sig, driver_cells);
		برائے (خودکار cell : driver_cells) {
			اگر (!eval(cell, undef)) {
				اگر (busy_cell)
					busy.erase(busy_cell);
				واپس false;
			}
		}

		اگر (busy_cell)
			busy.erase(busy_cell);

		اقدار_map.apply(sig);
		اگر (sig.is_fully_const())
			واپس true;

		اگر (defaultval != RTLIL::State::Sm) {
			برائے (خودکار &ثنائی_ہندسہ : sig)
				اگر (ثنائی_ہندسہ.wire) ثنائی_ہندسہ = defaultval;
			واپس true;
		}

		برائے (خودکار &c : sig.chunks())
			اگر (c.wire != تہی)
				undef.append(c);
		واپس false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		واپس eval(sig, undef);
	}
};

YOSYS_NAMESPACE_انتہا

#اگر_ختم
