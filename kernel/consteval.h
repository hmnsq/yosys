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

#جيڪڏهن_وصف_نه_ٿيل CONSTEVAL_H
#وصف_ڪريو CONSTEVAL_H

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
	SigMap سونپيو_map;
	SigMap قدرون_map;
	SigPool stop_سگنل;
	Sig_سيٽ<RTLIL::Cell*> sig2driver;
	std::سيٽ<RTLIL::Cell*> busy;
	std::ويڪٽر<SigMap> ڍير;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), سونپيو_map(module), defaultval(defaultval)
	{
		خودڪار ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		لكل (خودڪار &it : module->cells_) {
			إذا (!ct.cell_معلوم(it.ٻيو->type))
				استمر;
			لكل (خودڪار &it2 : it.ٻيو->connections())
				إذا (ct.cell_output(it.ٻيو->type, it2.پهريون))
					sig2driver.وجهو(سونپيو_map(it2.ٻيو), it.ٻيو);
		}
	}

	فراغ clear()
	{
		قدرون_map.clear();
		stop_سگنل.clear();
	}

	فراغ ڌڪو()
	{
		ڍير.ڌڪو_پٺ(قدرون_map);
	}

	فراغ ڪڍو()
	{
		قدرون_map.مٽا_سٽا(ڍير.پٺ());
		ڍير.ڪڍو_پٺ();
	}

	فراغ سيٽ(RTLIL::SigSpec sig, RTLIL::Const قدر)
	{
		سونپيو_map.apply(sig);
#جيڪڏهن_وصف_نه_ٿيل NDEBUG
		RTLIL::SigSpec current_val = قدرون_map(sig);
		لكل (صحيح i = 0; i < Get_سائيز(current_val); i++)
			log_assert(current_val[i].wire != عدم || current_val[i] == قدر[i]);
#جيڪڏهن_پورو
		قدرون_map.جوڙيو(sig, RTLIL::SigSpec(قدر));
	}

	فراغ stop(RTLIL::SigSpec sig)
	{
		سونپيو_map.apply(sig);
		stop_سگنل.جوڙيو(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		إذا (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_پورٽ(ID::P);
			RTLIL::SigSpec sig_g = cell->get_پورٽ(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_پورٽ(ID::CI);
			RTLIL::SigSpec sig_co = قدرون_map(سونپيو_map(cell->get_پورٽ(ID::CO)));

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
				RTLIL::Const coval(RTLIL::Sx, Get_سائيز(sig_co));
				bool حاصل = sig_ci.as_bool();

				لكل (صحيح i = 0; i < Get_سائيز(coval); i++) {
					حاصل = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && حاصل);
					coval.سيٽ(i, حاصل ? State::S1 : State::S0);
				}

				سيٽ(sig_co, coval);
			}
			وإلا
				سيٽ(sig_co, RTLIL::Const(RTLIL::Sx, Get_سائيز(sig_co)));

			أرجع true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_پورٽ(ID::Y));
		sig_y = قدرون_map(سونپيو_map(cell->get_پورٽ(ID::Y)));
		إذا (sig_y.is_fully_const())
			أرجع true;

		إذا (cell->type == ID($barrier))
		{
			إذا (!model_barriers) {
				undef.append(sig_y);
				أرجع false;
			}
			sig_a = cell->get_پورٽ(ID::A);
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			سيٽ(sig_y, sig_a.as_const());
			أرجع true;
		}

		إذا (cell->has_پورٽ(ID::S)) {
			sig_s = cell->get_پورٽ(ID::S);
		}

		إذا (cell->has_پورٽ(ID::A))
			sig_a = cell->get_پورٽ(ID::A);

		إذا (cell->has_پورٽ(ID::B))
			sig_b = cell->get_پورٽ(ID::B);

		إذا (cell->type.اندريون(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::ويڪٽر<RTLIL::SigSpec> y_candidates;
			صحيح ڳڻپ_سيٽ_s_بٽ = 0;

			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			لكل (صحيح i = 0; i < sig_s.سائيز(); i++)
			{
				RTLIL::State s_ٻٽو_انگ = sig_s.ڪڍو_2(i, 1).as_const().at(0);
				RTLIL::SigSpec b_ٽڪرو = sig_b.ڪڍو_2(sig_y.سائيز()*i, sig_y.سائيز());

				إذا (s_ٻٽو_انگ == RTLIL::State::Sx || s_ٻٽو_انگ == RTLIL::State::S1)
					y_candidates.ڌڪو_پٺ(b_ٽڪرو);

				إذا (s_ٻٽو_انگ == RTLIL::State::S1)
					ڳڻپ_سيٽ_s_بٽ++;
			}

			إذا (ڳڻپ_سيٽ_s_بٽ == 0)
				y_candidates.ڌڪو_پٺ(sig_a);

			std::ويڪٽر<RTLIL::Const> y_قدرون;

			log_assert(y_candidates.سائيز() > 0);
			لكل (خودڪار &yc : y_candidates) {
				إذا (!eval(yc, undef, cell))
					أرجع false;
				إذا (cell->type == ID($_NMUX_))
					y_قدرون.ڌڪو_پٺ(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_سائيز(yc)));
				وإلا
					y_قدرون.ڌڪو_پٺ(yc.as_const());
			}

			إذا (y_قدرون.سائيز() > 1)
			{
				std::ويڪٽر<RTLIL::State> master_بٽ = y_قدرون.at(0).منزل_بٽ();

				لكل (سائيز_t i = 1; i < y_قدرون.سائيز(); i++) {
					std::ويڪٽر<RTLIL::State> slave_بٽ = y_قدرون.at(i).منزل_بٽ();
					log_assert(master_بٽ.سائيز() == slave_بٽ.سائيز());
					لكل (سائيز_t j = 0; j < master_بٽ.سائيز(); j++)
						إذا (master_بٽ[j] != slave_بٽ[j])
							master_بٽ[j] = RTLIL::State::Sx;
				}

				سيٽ(sig_y, RTLIL::Const(master_بٽ));
			}
			وإلا
				سيٽ(sig_y, y_قدرون.front());
		}
		وإلا إذا (cell->type == ID($bmux))
		{
			إذا (!eval(sig_s, undef, cell))
				أرجع false;

			إذا (sig_s.is_fully_def()) {
				صحيح sel = sig_s.as_int();
				صحيح ويڪر = Get_سائيز(sig_y);
				SigSpec res = sig_a.ڪڍو_2(sel * ويڪر, ويڪر);
				إذا (!eval(res, undef, cell))
					أرجع false;
				سيٽ(sig_y, res.as_const());
			} وإلا {
				إذا (!eval(sig_a, undef, cell))
					أرجع false;
				سيٽ(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($demux))
		{
			إذا (!eval(sig_a, undef, cell))
				أرجع false;
			إذا (sig_a.is_fully_zero()) {
				سيٽ(sig_y, Const(0, Get_سائيز(sig_y)));
			} وإلا {
				إذا (!eval(sig_s, undef, cell))
					أرجع false;
				سيٽ(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		وإلا إذا (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_پورٽ(ID::C);
			RTLIL::SigSpec sig_x = cell->get_پورٽ(ID::X);
			صحيح ويڪر = Get_سائيز(sig_c);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_c, undef, cell))
				أرجع false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, ويڪر);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, ويڪر);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, ويڪر);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, ويڪر);
			RTLIL::Const val_x = const_or(t2, t3, false, false, ويڪر);

			لكل (صحيح i = 0; i < Get_سائيز(val_y); i++)
				إذا (val_y[i] == RTLIL::Sx)
					val_x.سيٽ(i, RTLIL::Sx);

			سيٽ(sig_y, val_y);
			سيٽ(sig_x, val_x);
		}
		وإلا إذا (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.ڳڻپ(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.ڳڻپ(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_پورٽ(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_پورٽ(ID::BI);

			إذا (!eval(sig_a, undef, cell))
				أرجع false;

			إذا (!eval(sig_b, undef, cell))
				أرجع false;

			إذا (!eval(sig_ci, undef, cell))
				أرجع false;

			إذا (!eval(sig_bi, undef, cell))
				أرجع false;

			RTLIL::SigSpec sig_x = cell->get_پورٽ(ID::X);
			RTLIL::SigSpec sig_co = cell->get_پورٽ(ID::CO);

			bool any_داخلا_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_سائيز(sig_y), signed_a);
			sig_b.extend_u0(Get_سائيز(sig_y), signed_b);

			bool حاصل = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			لكل (صحيح i = 0; i < Get_سائيز(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				إذا (!x_inputs.is_fully_def()) {
					سيٽ(sig_x[i], RTLIL::Sx);
				} وإلا {
					bool ٻٽو_انگ_a = sig_a[i] == State::S1;
					bool ٻٽو_انگ_b = (sig_b[i] == State::S1) != b_inv;
					bool ٻٽو_انگ_x = ٻٽو_انگ_a != ٻٽو_انگ_b;
					سيٽ(sig_x[i], ٻٽو_انگ_x ? State::S1 : State::S0);
				}

				إذا (any_داخلا_undef) {
					سيٽ(sig_y[i], RTLIL::Sx);
					سيٽ(sig_co[i], RTLIL::Sx);
				} وإلا {
					bool ٻٽو_انگ_a = sig_a[i] == State::S1;
					bool ٻٽو_انگ_b = (sig_b[i] == State::S1) != b_inv;
					bool ٻٽو_انگ_y = (ٻٽو_انگ_a != ٻٽو_انگ_b) != حاصل;
					حاصل = (ٻٽو_انگ_a && ٻٽو_انگ_b) || (ٻٽو_انگ_a && حاصل) || (ٻٽو_انگ_b && حاصل);
					سيٽ(sig_y[i], ٻٽو_انگ_y ? State::S1 : State::S0);
					سيٽ(sig_co[i], حاصل ? State::S1 : State::S0);
				}
			}
		}
		وإلا إذا (cell->type.اندريون(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			لكل (خودڪار &پورٽ : macc.terms) {
				إذا (!eval(پورٽ.اندريون_a, undef, cell))
					أرجع false;
				إذا (!eval(پورٽ.اندريون_b, undef, cell))
					أرجع false;
			}

			RTLIL::Const نتيجو(0, Get_سائيز(cell->get_پورٽ(ID::Y)));
			إذا (!macc.eval(نتيجو))
				log_abort();

			سيٽ(cell->get_پورٽ(ID::Y), نتيجو);
		}
		وإلا
		{
			RTLIL::SigSpec sig_c, sig_d;

			إذا (cell->type.اندريون(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				إذا (cell->has_پورٽ(ID::C))
					sig_c = cell->get_پورٽ(ID::C);
				إذا (cell->has_پورٽ(ID::D))
					sig_d = cell->get_پورٽ(ID::D);
			}

			إذا (sig_a.سائيز() > 0 && !eval(sig_a, undef, cell))
				أرجع false;
			إذا (sig_b.سائيز() > 0 && !eval(sig_b, undef, cell))
				أرجع false;
			إذا (sig_c.سائيز() > 0 && !eval(sig_c, undef, cell))
				أرجع false;
			إذا (sig_d.سائيز() > 0 && !eval(sig_d, undef, cell))
				أرجع false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			إذا (sig_s.سائيز() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} وإلا
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			إذا (eval_err)
				أرجع false;

			سيٽ(sig_y, eval_ret);
		}

		أرجع true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = عدم)
	{
		سونپيو_map.apply(sig);
		قدرون_map.apply(sig);

		إذا (sig.is_fully_const())
			أرجع true;

		إذا (stop_سگنل.check_any(sig)) {
			undef = stop_سگنل.ڪڍو_2(sig);
			أرجع false;
		}

		إذا (busy_cell) {
			إذا (busy.ڳڻپ(busy_cell) > 0) {
				undef = sig;
				أرجع false;
			}
			busy.وجهو(busy_cell);
		}

		std::سيٽ<RTLIL::Cell*> driver_cells;
		sig2driver.ڳوليو(sig, driver_cells);
		لكل (خودڪار cell : driver_cells) {
			إذا (!eval(cell, undef)) {
				إذا (busy_cell)
					busy.erase(busy_cell);
				أرجع false;
			}
		}

		إذا (busy_cell)
			busy.erase(busy_cell);

		قدرون_map.apply(sig);
		إذا (sig.is_fully_const())
			أرجع true;

		إذا (defaultval != RTLIL::State::Sm) {
			لكل (خودڪار &ٻٽو_انگ : sig)
				إذا (ٻٽو_انگ.wire) ٻٽو_انگ = defaultval;
			أرجع true;
		}

		لكل (خودڪار &c : sig.chunks())
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

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
