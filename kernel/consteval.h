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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONSTEVAL_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONSTEVAL_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/celltypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/macc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
ਬਣਤਰ ConstEval
{
	RTLIL::Module *module;
	SigMap ਸੌਂਪੋ_map;
	SigMap ਮੁੱਲਾਂ_map;
	SigPool stop_ਸੰਕੇਤ;
	Sig_ਸੈੱਟ<RTLIL::Cell*> sig2driver;
	std::ਸੈੱਟ<RTLIL::Cell*> busy;
	std::ਸਦਿਸ਼<SigMap> ਢੇਰ;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), ਸੌਂਪੋ_map(module), defaultval(defaultval)
	{
		ਸਵੈਚਾਲਿਤ ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		ਲਈ (ਸਵੈਚਾਲਿਤ &it : module->cells_) {
			ਜੇ (!ct.cell_ਗਿਆਤ(it.ਦੂਜਾ->type))
				ਜਾਰੀ;
			ਲਈ (ਸਵੈਚਾਲਿਤ &it2 : it.ਦੂਜਾ->connections())
				ਜੇ (ct.cell_output(it.ਦੂਜਾ->type, it2.ਪਹਿਲਾ))
					sig2driver.ਪਾਓ(ਸੌਂਪੋ_map(it2.ਦੂਜਾ), it.ਦੂਜਾ);
		}
	}

	ਖਾਲੀ clear()
	{
		ਮੁੱਲਾਂ_map.clear();
		stop_ਸੰਕੇਤ.clear();
	}

	ਖਾਲੀ ਧੱਕੋ()
	{
		ਢੇਰ.ਧੱਕੋ_ਪਿੱਠ(ਮੁੱਲਾਂ_map);
	}

	ਖਾਲੀ ਕੱਢੋ()
	{
		ਮੁੱਲਾਂ_map.ਅਦਲਾ_ਬਦਲੀ(ਢੇਰ.ਪਿੱਠ());
		ਢੇਰ.ਕੱਢੋ_ਪਿੱਠ();
	}

	ਖਾਲੀ ਸੈੱਟ(RTLIL::SigSpec sig, RTLIL::Const ਮੁੱਲ)
	{
		ਸੌਂਪੋ_map.apply(sig);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ NDEBUG
		RTLIL::SigSpec current_val = ਮੁੱਲਾਂ_map(sig);
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(current_val); i++)
			log_assert(current_val[i].wire != ਨਿਰਾ || current_val[i] == ਮੁੱਲ[i]);
#ਜੇ_ਅੰਤ
		ਮੁੱਲਾਂ_map.ਜੋੜੋ(sig, RTLIL::SigSpec(ਮੁੱਲ));
	}

	ਖਾਲੀ stop(RTLIL::SigSpec sig)
	{
		ਸੌਂਪੋ_map.apply(sig);
		stop_ਸੰਕੇਤ.ਜੋੜੋ(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		ਜੇ (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_ਪੋਰਟ(ID::P);
			RTLIL::SigSpec sig_g = cell->get_ਪੋਰਟ(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_ਪੋਰਟ(ID::CI);
			RTLIL::SigSpec sig_co = ਮੁੱਲਾਂ_map(ਸੌਂਪੋ_map(cell->get_ਪੋਰਟ(ID::CO)));

			ਜੇ (sig_co.is_fully_const())
				ਵਾਪਸ true;

			ਜੇ (!eval(sig_p, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (!eval(sig_g, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (!eval(sig_ci, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, Get_ਮਾਪ(sig_co));
				bool ਹਾਸਲ = sig_ci.as_bool();

				ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(coval); i++) {
					ਹਾਸਲ = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && ਹਾਸਲ);
					coval.ਸੈੱਟ(i, ਹਾਸਲ ? State::S1 : State::S0);
				}

				ਸੈੱਟ(sig_co, coval);
			}
			ਨਹੀਂਤਾਂ
				ਸੈੱਟ(sig_co, RTLIL::Const(RTLIL::Sx, Get_ਮਾਪ(sig_co)));

			ਵਾਪਸ true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_ਪੋਰਟ(ID::Y));
		sig_y = ਮੁੱਲਾਂ_map(ਸੌਂਪੋ_map(cell->get_ਪੋਰਟ(ID::Y)));
		ਜੇ (sig_y.is_fully_const())
			ਵਾਪਸ true;

		ਜੇ (cell->type == ID($barrier))
		{
			ਜੇ (!model_barriers) {
				undef.append(sig_y);
				ਵਾਪਸ false;
			}
			sig_a = cell->get_ਪੋਰਟ(ID::A);
			ਜੇ (!eval(sig_a, undef, cell))
				ਵਾਪਸ false;
			ਸੈੱਟ(sig_y, sig_a.as_const());
			ਵਾਪਸ true;
		}

		ਜੇ (cell->has_ਪੋਰਟ(ID::S)) {
			sig_s = cell->get_ਪੋਰਟ(ID::S);
		}

		ਜੇ (cell->has_ਪੋਰਟ(ID::A))
			sig_a = cell->get_ਪੋਰਟ(ID::A);

		ਜੇ (cell->has_ਪੋਰਟ(ID::B))
			sig_b = cell->get_ਪੋਰਟ(ID::B);

		ਜੇ (cell->type.ਵਿੱਚ(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::ਸਦਿਸ਼<RTLIL::SigSpec> y_candidates;
			ਪੂਰਨ ਗਿਣਤੀ_ਸੈੱਟ_s_ਬਿੱਟ = 0;

			ਜੇ (!eval(sig_s, undef, cell))
				ਵਾਪਸ false;

			ਲਈ (ਪੂਰਨ i = 0; i < sig_s.ਮਾਪ(); i++)
			{
				RTLIL::State s_ਦੋਆਧਾਰੀ_ਅੰਕ = sig_s.ਕੱਢੋ_2(i, 1).as_const().at(0);
				RTLIL::SigSpec b_ਟੁਕੜਾ = sig_b.ਕੱਢੋ_2(sig_y.ਮਾਪ()*i, sig_y.ਮਾਪ());

				ਜੇ (s_ਦੋਆਧਾਰੀ_ਅੰਕ == RTLIL::State::Sx || s_ਦੋਆਧਾਰੀ_ਅੰਕ == RTLIL::State::S1)
					y_candidates.ਧੱਕੋ_ਪਿੱਠ(b_ਟੁਕੜਾ);

				ਜੇ (s_ਦੋਆਧਾਰੀ_ਅੰਕ == RTLIL::State::S1)
					ਗਿਣਤੀ_ਸੈੱਟ_s_ਬਿੱਟ++;
			}

			ਜੇ (ਗਿਣਤੀ_ਸੈੱਟ_s_ਬਿੱਟ == 0)
				y_candidates.ਧੱਕੋ_ਪਿੱਠ(sig_a);

			std::ਸਦਿਸ਼<RTLIL::Const> y_ਮੁੱਲਾਂ;

			log_assert(y_candidates.ਮਾਪ() > 0);
			ਲਈ (ਸਵੈਚਾਲਿਤ &yc : y_candidates) {
				ਜੇ (!eval(yc, undef, cell))
					ਵਾਪਸ false;
				ਜੇ (cell->type == ID($_NMUX_))
					y_ਮੁੱਲਾਂ.ਧੱਕੋ_ਪਿੱਠ(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_ਮਾਪ(yc)));
				ਨਹੀਂਤਾਂ
					y_ਮੁੱਲਾਂ.ਧੱਕੋ_ਪਿੱਠ(yc.as_const());
			}

			ਜੇ (y_ਮੁੱਲਾਂ.ਮਾਪ() > 1)
			{
				std::ਸਦਿਸ਼<RTLIL::State> master_ਬਿੱਟ = y_ਮੁੱਲਾਂ.at(0).ਮੰਜ਼ਿਲ_ਬਿੱਟ();

				ਲਈ (ਮਾਪ_t i = 1; i < y_ਮੁੱਲਾਂ.ਮਾਪ(); i++) {
					std::ਸਦਿਸ਼<RTLIL::State> slave_ਬਿੱਟ = y_ਮੁੱਲਾਂ.at(i).ਮੰਜ਼ਿਲ_ਬਿੱਟ();
					log_assert(master_ਬਿੱਟ.ਮਾਪ() == slave_ਬਿੱਟ.ਮਾਪ());
					ਲਈ (ਮਾਪ_t j = 0; j < master_ਬਿੱਟ.ਮਾਪ(); j++)
						ਜੇ (master_ਬਿੱਟ[j] != slave_ਬਿੱਟ[j])
							master_ਬਿੱਟ[j] = RTLIL::State::Sx;
				}

				ਸੈੱਟ(sig_y, RTLIL::Const(master_ਬਿੱਟ));
			}
			ਨਹੀਂਤਾਂ
				ਸੈੱਟ(sig_y, y_ਮੁੱਲਾਂ.front());
		}
		ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($bmux))
		{
			ਜੇ (!eval(sig_s, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (sig_s.is_fully_def()) {
				ਪੂਰਨ sel = sig_s.as_int();
				ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(sig_y);
				SigSpec res = sig_a.ਕੱਢੋ_2(sel * ਚੌੜਾਈ, ਚੌੜਾਈ);
				ਜੇ (!eval(res, undef, cell))
					ਵਾਪਸ false;
				ਸੈੱਟ(sig_y, res.as_const());
			} ਨਹੀਂਤਾਂ {
				ਜੇ (!eval(sig_a, undef, cell))
					ਵਾਪਸ false;
				ਸੈੱਟ(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($demux))
		{
			ਜੇ (!eval(sig_a, undef, cell))
				ਵਾਪਸ false;
			ਜੇ (sig_a.is_fully_zero()) {
				ਸੈੱਟ(sig_y, Const(0, Get_ਮਾਪ(sig_y)));
			} ਨਹੀਂਤਾਂ {
				ਜੇ (!eval(sig_s, undef, cell))
					ਵਾਪਸ false;
				ਸੈੱਟ(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_ਪੋਰਟ(ID::C);
			RTLIL::SigSpec sig_x = cell->get_ਪੋਰਟ(ID::X);
			ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(sig_c);

			ਜੇ (!eval(sig_a, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (!eval(sig_b, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (!eval(sig_c, undef, cell))
				ਵਾਪਸ false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, ਚੌੜਾਈ);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, ਚੌੜਾਈ);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, ਚੌੜਾਈ);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, ਚੌੜਾਈ);
			RTLIL::Const val_x = const_or(t2, t3, false, false, ਚੌੜਾਈ);

			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(val_y); i++)
				ਜੇ (val_y[i] == RTLIL::Sx)
					val_x.ਸੈੱਟ(i, RTLIL::Sx);

			ਸੈੱਟ(sig_y, val_y);
			ਸੈੱਟ(sig_x, val_x);
		}
		ਨਹੀਂਤਾਂ ਜੇ (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.ਗਿਣਤੀ_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.ਗਿਣਤੀ_2(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_ਪੋਰਟ(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_ਪੋਰਟ(ID::BI);

			ਜੇ (!eval(sig_a, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (!eval(sig_b, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (!eval(sig_ci, undef, cell))
				ਵਾਪਸ false;

			ਜੇ (!eval(sig_bi, undef, cell))
				ਵਾਪਸ false;

			RTLIL::SigSpec sig_x = cell->get_ਪੋਰਟ(ID::X);
			RTLIL::SigSpec sig_co = cell->get_ਪੋਰਟ(ID::CO);

			bool any_ਨਿਵੇਸ਼_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_ਮਾਪ(sig_y), signed_a);
			sig_b.extend_u0(Get_ਮਾਪ(sig_y), signed_b);

			bool ਹਾਸਲ = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				ਜੇ (!x_inputs.is_fully_def()) {
					ਸੈੱਟ(sig_x[i], RTLIL::Sx);
				} ਨਹੀਂਤਾਂ {
					bool ਦੋਆਧਾਰੀ_ਅੰਕ_a = sig_a[i] == State::S1;
					bool ਦੋਆਧਾਰੀ_ਅੰਕ_b = (sig_b[i] == State::S1) != b_inv;
					bool ਦੋਆਧਾਰੀ_ਅੰਕ_x = ਦੋਆਧਾਰੀ_ਅੰਕ_a != ਦੋਆਧਾਰੀ_ਅੰਕ_b;
					ਸੈੱਟ(sig_x[i], ਦੋਆਧਾਰੀ_ਅੰਕ_x ? State::S1 : State::S0);
				}

				ਜੇ (any_ਨਿਵੇਸ਼_undef) {
					ਸੈੱਟ(sig_y[i], RTLIL::Sx);
					ਸੈੱਟ(sig_co[i], RTLIL::Sx);
				} ਨਹੀਂਤਾਂ {
					bool ਦੋਆਧਾਰੀ_ਅੰਕ_a = sig_a[i] == State::S1;
					bool ਦੋਆਧਾਰੀ_ਅੰਕ_b = (sig_b[i] == State::S1) != b_inv;
					bool ਦੋਆਧਾਰੀ_ਅੰਕ_y = (ਦੋਆਧਾਰੀ_ਅੰਕ_a != ਦੋਆਧਾਰੀ_ਅੰਕ_b) != ਹਾਸਲ;
					ਹਾਸਲ = (ਦੋਆਧਾਰੀ_ਅੰਕ_a && ਦੋਆਧਾਰੀ_ਅੰਕ_b) || (ਦੋਆਧਾਰੀ_ਅੰਕ_a && ਹਾਸਲ) || (ਦੋਆਧਾਰੀ_ਅੰਕ_b && ਹਾਸਲ);
					ਸੈੱਟ(sig_y[i], ਦੋਆਧਾਰੀ_ਅੰਕ_y ? State::S1 : State::S0);
					ਸੈੱਟ(sig_co[i], ਹਾਸਲ ? State::S1 : State::S0);
				}
			}
		}
		ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			ਲਈ (ਸਵੈਚਾਲਿਤ &ਪੋਰਟ : macc.terms) {
				ਜੇ (!eval(ਪੋਰਟ.ਵਿੱਚ_a, undef, cell))
					ਵਾਪਸ false;
				ਜੇ (!eval(ਪੋਰਟ.ਵਿੱਚ_b, undef, cell))
					ਵਾਪਸ false;
			}

			RTLIL::Const ਨਤੀਜਾ(0, Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y)));
			ਜੇ (!macc.eval(ਨਤੀਜਾ))
				log_abort();

			ਸੈੱਟ(cell->get_ਪੋਰਟ(ID::Y), ਨਤੀਜਾ);
		}
		ਨਹੀਂਤਾਂ
		{
			RTLIL::SigSpec sig_c, sig_d;

			ਜੇ (cell->type.ਵਿੱਚ(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				ਜੇ (cell->has_ਪੋਰਟ(ID::C))
					sig_c = cell->get_ਪੋਰਟ(ID::C);
				ਜੇ (cell->has_ਪੋਰਟ(ID::D))
					sig_d = cell->get_ਪੋਰਟ(ID::D);
			}

			ਜੇ (sig_a.ਮਾਪ() > 0 && !eval(sig_a, undef, cell))
				ਵਾਪਸ false;
			ਜੇ (sig_b.ਮਾਪ() > 0 && !eval(sig_b, undef, cell))
				ਵਾਪਸ false;
			ਜੇ (sig_c.ਮਾਪ() > 0 && !eval(sig_c, undef, cell))
				ਵਾਪਸ false;
			ਜੇ (sig_d.ਮਾਪ() > 0 && !eval(sig_d, undef, cell))
				ਵਾਪਸ false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			ਜੇ (sig_s.ਮਾਪ() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} ਨਹੀਂਤਾਂ
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			ਜੇ (eval_err)
				ਵਾਪਸ false;

			ਸੈੱਟ(sig_y, eval_ret);
		}

		ਵਾਪਸ true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = ਨਿਰਾ)
	{
		ਸੌਂਪੋ_map.apply(sig);
		ਮੁੱਲਾਂ_map.apply(sig);

		ਜੇ (sig.is_fully_const())
			ਵਾਪਸ true;

		ਜੇ (stop_ਸੰਕੇਤ.check_any(sig)) {
			undef = stop_ਸੰਕੇਤ.ਕੱਢੋ_2(sig);
			ਵਾਪਸ false;
		}

		ਜੇ (busy_cell) {
			ਜੇ (busy.ਗਿਣਤੀ_2(busy_cell) > 0) {
				undef = sig;
				ਵਾਪਸ false;
			}
			busy.ਪਾਓ(busy_cell);
		}

		std::ਸੈੱਟ<RTLIL::Cell*> driver_cells;
		sig2driver.ਲੱਭੋ(sig, driver_cells);
		ਲਈ (ਸਵੈਚਾਲਿਤ cell : driver_cells) {
			ਜੇ (!eval(cell, undef)) {
				ਜੇ (busy_cell)
					busy.erase(busy_cell);
				ਵਾਪਸ false;
			}
		}

		ਜੇ (busy_cell)
			busy.erase(busy_cell);

		ਮੁੱਲਾਂ_map.apply(sig);
		ਜੇ (sig.is_fully_const())
			ਵਾਪਸ true;

		ਜੇ (defaultval != RTLIL::State::Sm) {
			ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
				ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire) ਦੋਆਧਾਰੀ_ਅੰਕ = defaultval;
			ਵਾਪਸ true;
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ &c : sig.chunks())
			ਜੇ (c.wire != ਨਿਰਾ)
				undef.append(c);
		ਵਾਪਸ false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		ਵਾਪਸ eval(sig, undef);
	}
};

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
