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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONSTEVAL_H
#ವ್ಯಾಖ್ಯಾನಿಸು CONSTEVAL_H

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"
#ಅನ್ತರ್ಭಾವ "kernel/macc.h"
#ಅನ್ತರ್ಭಾವ "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
ರಚನೆ ConstEval
{
	RTLIL::Module *module;
	SigMap ನಿಯೋಜಿಸು_map;
	SigMap ಮೌಲ್ಯಗಳು_map;
	SigPool stop_ಸಂಕೇತಗಳು;
	Sig_ಗಣ<RTLIL::Cell*> sig2driver;
	std::ಗಣ<RTLIL::Cell*> busy;
	std::ಸದಿಶ<SigMap> ರಾಶಿ;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), ನಿಯೋಜಿಸು_map(module), defaultval(defaultval)
	{
		ಸ್ವಯಂಚಾಲಿತ ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it : module->cells_) {
			ಆದರೆ (!ct.cell_ತಿಳಿದ(it.ಎರಡನೇ->type))
				ಮುಂದುವರಿ;
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &it2 : it.ಎರಡನೇ->connections())
				ಆದರೆ (ct.cell_output(it.ಎರಡನೇ->type, it2.ಮೊದಲ))
					sig2driver.ಸೇರಿಸು(ನಿಯೋಜಿಸು_map(it2.ಎರಡನೇ), it.ಎರಡನೇ);
		}
	}

	ಶೂನ್ಯ clear()
	{
		ಮೌಲ್ಯಗಳು_map.clear();
		stop_ಸಂಕೇತಗಳು.clear();
	}

	ಶೂನ್ಯ ತಳ್ಳು()
	{
		ರಾಶಿ.ತಳ್ಳು_ಹಿಂಭಾಗ(ಮೌಲ್ಯಗಳು_map);
	}

	ಶೂನ್ಯ ತೆಗೆ_2()
	{
		ಮೌಲ್ಯಗಳು_map.ಅದಲುಬದಲು(ರಾಶಿ.ಹಿಂಭಾಗ());
		ರಾಶಿ.ತೆಗೆ_ಹಿಂಭಾಗ();
	}

	ಶೂನ್ಯ ಗಣ(RTLIL::SigSpec sig, RTLIL::Const ಮೌಲ್ಯ)
	{
		ನಿಯೋಜಿಸು_map.apply(sig);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ NDEBUG
		RTLIL::SigSpec current_val = ಮೌಲ್ಯಗಳು_map(sig);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(current_val); i++)
			log_assert(current_val[i].wire != ಖಾಲಿ || current_val[i] == ಮೌಲ್ಯ[i]);
#ಆದರೆ_ಅಂತ್ಯ
		ಮೌಲ್ಯಗಳು_map.ಕೂಡಿಸು(sig, RTLIL::SigSpec(ಮೌಲ್ಯ));
	}

	ಶೂನ್ಯ stop(RTLIL::SigSpec sig)
	{
		ನಿಯೋಜಿಸು_map.apply(sig);
		stop_ಸಂಕೇತಗಳು.ಕೂಡಿಸು(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		ಆದರೆ (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_ಪೋರ್ಟ್(ID::P);
			RTLIL::SigSpec sig_g = cell->get_ಪೋರ್ಟ್(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_ಪೋರ್ಟ್(ID::CI);
			RTLIL::SigSpec sig_co = ಮೌಲ್ಯಗಳು_map(ನಿಯೋಜಿಸು_map(cell->get_ಪೋರ್ಟ್(ID::CO)));

			ಆದರೆ (sig_co.is_fully_const())
				ಹಿಂತಿರುಗಿ true;

			ಆದರೆ (!eval(sig_p, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (!eval(sig_g, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (!eval(sig_ci, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, Get_ಗಾತ್ರ(sig_co));
				bool ವಾಹಕ = sig_ci.as_bool();

				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(coval); i++) {
					ವಾಹಕ = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && ವಾಹಕ);
					coval.ಗಣ(i, ವಾಹಕ ? State::S1 : State::S0);
				}

				ಗಣ(sig_co, coval);
			}
			ಇಲ್ಲದಿದ್ದರೆ
				ಗಣ(sig_co, RTLIL::Const(RTLIL::Sx, Get_ಗಾತ್ರ(sig_co)));

			ಹಿಂತಿರುಗಿ true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_ಪೋರ್ಟ್(ID::Y));
		sig_y = ಮೌಲ್ಯಗಳು_map(ನಿಯೋಜಿಸು_map(cell->get_ಪೋರ್ಟ್(ID::Y)));
		ಆದರೆ (sig_y.is_fully_const())
			ಹಿಂತಿರುಗಿ true;

		ಆದರೆ (cell->type == ID($barrier))
		{
			ಆದರೆ (!model_barriers) {
				undef.append(sig_y);
				ಹಿಂತಿರುಗಿ false;
			}
			sig_a = cell->get_ಪೋರ್ಟ್(ID::A);
			ಆದರೆ (!eval(sig_a, undef, cell))
				ಹಿಂತಿರುಗಿ false;
			ಗಣ(sig_y, sig_a.as_const());
			ಹಿಂತಿರುಗಿ true;
		}

		ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::S)) {
			sig_s = cell->get_ಪೋರ್ಟ್(ID::S);
		}

		ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::A))
			sig_a = cell->get_ಪೋರ್ಟ್(ID::A);

		ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::B))
			sig_b = cell->get_ಪೋರ್ಟ್(ID::B);

		ಆದರೆ (cell->type.ಒಳ(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::ಸದಿಶ<RTLIL::SigSpec> y_candidates;
			ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_ಗಣ_s_ಬಿಟ್ಟುಗಳು = 0;

			ಆದರೆ (!eval(sig_s, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < sig_s.ಗಾತ್ರ_2(); i++)
			{
				RTLIL::State s_ಬಿಟ್ = sig_s.ಹೊರತೆಗೆ(i, 1).as_const().at(0);
				RTLIL::SigSpec b_ತುಂಡು = sig_b.ಹೊರತೆಗೆ(sig_y.ಗಾತ್ರ_2()*i, sig_y.ಗಾತ್ರ_2());

				ಆದರೆ (s_ಬಿಟ್ == RTLIL::State::Sx || s_ಬಿಟ್ == RTLIL::State::S1)
					y_candidates.ತಳ್ಳು_ಹಿಂಭಾಗ(b_ತುಂಡು);

				ಆದರೆ (s_ಬಿಟ್ == RTLIL::State::S1)
					ಎಣಿಕೆ_ಗಣ_s_ಬಿಟ್ಟುಗಳು++;
			}

			ಆದರೆ (ಎಣಿಕೆ_ಗಣ_s_ಬಿಟ್ಟುಗಳು == 0)
				y_candidates.ತಳ್ಳು_ಹಿಂಭಾಗ(sig_a);

			std::ಸದಿಶ<RTLIL::Const> y_ಮೌಲ್ಯಗಳು;

			log_assert(y_candidates.ಗಾತ್ರ_2() > 0);
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &yc : y_candidates) {
				ಆದರೆ (!eval(yc, undef, cell))
					ಹಿಂತಿರುಗಿ false;
				ಆದರೆ (cell->type == ID($_NMUX_))
					y_ಮೌಲ್ಯಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_ಗಾತ್ರ(yc)));
				ಇಲ್ಲದಿದ್ದರೆ
					y_ಮೌಲ್ಯಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(yc.as_const());
			}

			ಆದರೆ (y_ಮೌಲ್ಯಗಳು.ಗಾತ್ರ_2() > 1)
			{
				std::ಸದಿಶ<RTLIL::State> master_ಬಿಟ್ಟುಗಳು = y_ಮೌಲ್ಯಗಳು.at(0).ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು();

				ಗಾಗಿ (ಗಾತ್ರ_t i = 1; i < y_ಮೌಲ್ಯಗಳು.ಗಾತ್ರ_2(); i++) {
					std::ಸದಿಶ<RTLIL::State> slave_ಬಿಟ್ಟುಗಳು = y_ಮೌಲ್ಯಗಳು.at(i).ಗಮ್ಯ_ಬಿಟ್ಟುಗಳು();
					log_assert(master_ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2() == slave_ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2());
					ಗಾಗಿ (ಗಾತ್ರ_t j = 0; j < master_ಬಿಟ್ಟುಗಳು.ಗಾತ್ರ_2(); j++)
						ಆದರೆ (master_ಬಿಟ್ಟುಗಳು[j] != slave_ಬಿಟ್ಟುಗಳು[j])
							master_ಬಿಟ್ಟುಗಳು[j] = RTLIL::State::Sx;
				}

				ಗಣ(sig_y, RTLIL::Const(master_ಬಿಟ್ಟುಗಳು));
			}
			ಇಲ್ಲದಿದ್ದರೆ
				ಗಣ(sig_y, y_ಮೌಲ್ಯಗಳು.front());
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($bmux))
		{
			ಆದರೆ (!eval(sig_s, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (sig_s.is_fully_def()) {
				ಪೂರ್ಣಾಂಕ sel = sig_s.as_int();
				ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(sig_y);
				SigSpec res = sig_a.ಹೊರತೆಗೆ(sel * ಅಗಲ, ಅಗಲ);
				ಆದರೆ (!eval(res, undef, cell))
					ಹಿಂತಿರುಗಿ false;
				ಗಣ(sig_y, res.as_const());
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (!eval(sig_a, undef, cell))
					ಹಿಂತಿರುಗಿ false;
				ಗಣ(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($demux))
		{
			ಆದರೆ (!eval(sig_a, undef, cell))
				ಹಿಂತಿರುಗಿ false;
			ಆದರೆ (sig_a.is_fully_zero()) {
				ಗಣ(sig_y, Const(0, Get_ಗಾತ್ರ(sig_y)));
			} ಇಲ್ಲದಿದ್ದರೆ {
				ಆದರೆ (!eval(sig_s, undef, cell))
					ಹಿಂತಿರುಗಿ false;
				ಗಣ(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_ಪೋರ್ಟ್(ID::C);
			RTLIL::SigSpec sig_x = cell->get_ಪೋರ್ಟ್(ID::X);
			ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(sig_c);

			ಆದರೆ (!eval(sig_a, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (!eval(sig_b, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (!eval(sig_c, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, ಅಗಲ);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, ಅಗಲ);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, ಅಗಲ);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, ಅಗಲ);
			RTLIL::Const val_x = const_or(t2, t3, false, false, ಅಗಲ);

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(val_y); i++)
				ಆದರೆ (val_y[i] == RTLIL::Sx)
					val_x.ಗಣ(i, RTLIL::Sx);

			ಗಣ(sig_y, val_y);
			ಗಣ(sig_x, val_x);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.ಎಣಿಕೆ_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.ಎಣಿಕೆ_2(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_ಪೋರ್ಟ್(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_ಪೋರ್ಟ್(ID::BI);

			ಆದರೆ (!eval(sig_a, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (!eval(sig_b, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (!eval(sig_ci, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			ಆದರೆ (!eval(sig_bi, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			RTLIL::SigSpec sig_x = cell->get_ಪೋರ್ಟ್(ID::X);
			RTLIL::SigSpec sig_co = cell->get_ಪೋರ್ಟ್(ID::CO);

			bool any_ಒಳಹರಿವು_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_ಗಾತ್ರ(sig_y), signed_a);
			sig_b.extend_u0(Get_ಗಾತ್ರ(sig_y), signed_b);

			bool ವಾಹಕ = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				ಆದರೆ (!x_inputs.is_fully_def()) {
					ಗಣ(sig_x[i], RTLIL::Sx);
				} ಇಲ್ಲದಿದ್ದರೆ {
					bool ಬಿಟ್_a = sig_a[i] == State::S1;
					bool ಬಿಟ್_b = (sig_b[i] == State::S1) != b_inv;
					bool ಬಿಟ್_x = ಬಿಟ್_a != ಬಿಟ್_b;
					ಗಣ(sig_x[i], ಬಿಟ್_x ? State::S1 : State::S0);
				}

				ಆದರೆ (any_ಒಳಹರಿವು_undef) {
					ಗಣ(sig_y[i], RTLIL::Sx);
					ಗಣ(sig_co[i], RTLIL::Sx);
				} ಇಲ್ಲದಿದ್ದರೆ {
					bool ಬಿಟ್_a = sig_a[i] == State::S1;
					bool ಬಿಟ್_b = (sig_b[i] == State::S1) != b_inv;
					bool ಬಿಟ್_y = (ಬಿಟ್_a != ಬಿಟ್_b) != ವಾಹಕ;
					ವಾಹಕ = (ಬಿಟ್_a && ಬಿಟ್_b) || (ಬಿಟ್_a && ವಾಹಕ) || (ಬಿಟ್_b && ವಾಹಕ);
					ಗಣ(sig_y[i], ಬಿಟ್_y ? State::S1 : State::S0);
					ಗಣ(sig_co[i], ವಾಹಕ ? State::S1 : State::S0);
				}
			}
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಪೋರ್ಟ್ : macc.terms) {
				ಆದರೆ (!eval(ಪೋರ್ಟ್.ಒಳ_a, undef, cell))
					ಹಿಂತಿರುಗಿ false;
				ಆದರೆ (!eval(ಪೋರ್ಟ್.ಒಳ_b, undef, cell))
					ಹಿಂತಿರುಗಿ false;
			}

			RTLIL::Const ಫಲಿತಾಂಶ(0, Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y)));
			ಆದರೆ (!macc.eval(ಫಲಿತಾಂಶ))
				log_abort();

			ಗಣ(cell->get_ಪೋರ್ಟ್(ID::Y), ಫಲಿತಾಂಶ);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
			RTLIL::SigSpec sig_c, sig_d;

			ಆದರೆ (cell->type.ಒಳ(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::C))
					sig_c = cell->get_ಪೋರ್ಟ್(ID::C);
				ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::D))
					sig_d = cell->get_ಪೋರ್ಟ್(ID::D);
			}

			ಆದರೆ (sig_a.ಗಾತ್ರ_2() > 0 && !eval(sig_a, undef, cell))
				ಹಿಂತಿರುಗಿ false;
			ಆದರೆ (sig_b.ಗಾತ್ರ_2() > 0 && !eval(sig_b, undef, cell))
				ಹಿಂತಿರುಗಿ false;
			ಆದರೆ (sig_c.ಗಾತ್ರ_2() > 0 && !eval(sig_c, undef, cell))
				ಹಿಂತಿರುಗಿ false;
			ಆದರೆ (sig_d.ಗಾತ್ರ_2() > 0 && !eval(sig_d, undef, cell))
				ಹಿಂತಿರುಗಿ false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			ಆದರೆ (sig_s.ಗಾತ್ರ_2() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} ಇಲ್ಲದಿದ್ದರೆ
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			ಆದರೆ (eval_err)
				ಹಿಂತಿರುಗಿ false;

			ಗಣ(sig_y, eval_ret);
		}

		ಹಿಂತಿರುಗಿ true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = ಖಾಲಿ)
	{
		ನಿಯೋಜಿಸು_map.apply(sig);
		ಮೌಲ್ಯಗಳು_map.apply(sig);

		ಆದರೆ (sig.is_fully_const())
			ಹಿಂತಿರುಗಿ true;

		ಆದರೆ (stop_ಸಂಕೇತಗಳು.check_any(sig)) {
			undef = stop_ಸಂಕೇತಗಳು.ಹೊರತೆಗೆ(sig);
			ಹಿಂತಿರುಗಿ false;
		}

		ಆದರೆ (busy_cell) {
			ಆದರೆ (busy.ಎಣಿಕೆ_2(busy_cell) > 0) {
				undef = sig;
				ಹಿಂತಿರುಗಿ false;
			}
			busy.ಸೇರಿಸು(busy_cell);
		}

		std::ಗಣ<RTLIL::Cell*> driver_cells;
		sig2driver.ಕಂಡುಹಿಡಿ(sig, driver_cells);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ cell : driver_cells) {
			ಆದರೆ (!eval(cell, undef)) {
				ಆದರೆ (busy_cell)
					busy.erase(busy_cell);
				ಹಿಂತಿರುಗಿ false;
			}
		}

		ಆದರೆ (busy_cell)
			busy.erase(busy_cell);

		ಮೌಲ್ಯಗಳು_map.apply(sig);
		ಆದರೆ (sig.is_fully_const())
			ಹಿಂತಿರುಗಿ true;

		ಆದರೆ (defaultval != RTLIL::State::Sm) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
				ಆದರೆ (ಬಿಟ್.wire) ಬಿಟ್ = defaultval;
			ಹಿಂತಿರುಗಿ true;
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &c : sig.chunks())
			ಆದರೆ (c.wire != ಖಾಲಿ)
				undef.append(c);
		ಹಿಂತಿರುಗಿ false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		ಹಿಂತಿರುಗಿ eval(sig, undef);
	}
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
