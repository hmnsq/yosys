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

#यदि_अपरिभाषितम् CONSTEVAL_H
#परिभाषय CONSTEVAL_H

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/celltypes.h"
#अन्तर्भाव "kernel/macc.h"
#अन्तर्भाव "kernel/newcelltypes.h"

YOSYS_NAMESPACE_BEGIN

/**
 * ConstEval provides on-demand constant propagation by traversing input cones
 * with caching
 */
संरचना ConstEval
{
	RTLIL::Module *module;
	SigMap नियोजय_map;
	SigMap मानानि_map;
	SigPool stop_सङ्केताः;
	Sig_समुच्चयः<RTLIL::Cell*> sig2driver;
	std::समुच्चयः<RTLIL::Cell*> busy;
	std::सदिशम्<SigMap> राशिः;
	RTLIL::State defaultval;
	bool model_barriers = false; // Model $barrier as a buffer

	ConstEval(RTLIL::Module *module, RTLIL::State defaultval = RTLIL::State::Sm) : module(module), नियोजय_map(module), defaultval(defaultval)
	{
		स्वचालित ct = NewCellTypes();
		ct.static_cell_types = StaticCellTypes::Compat::nomem_noff;

		प्रति (स्वचालित &it : module->cells_) {
			यदि (!ct.cell_ज्ञातम्(it.second->type))
				अनुवर्त;
			प्रति (स्वचालित &it2 : it.second->connections())
				यदि (ct.cell_output(it.second->type, it2.first))
					sig2driver.निवेशय(नियोजय_map(it2.second), it.second);
		}
	}

	शून्य clear()
	{
		मानानि_map.clear();
		stop_सङ्केताः.clear();
	}

	शून्य प्रक्षिप()
	{
		राशिः.प्रक्षिप_पृष्ठभागः(मानानि_map);
	}

	शून्य निष्कासय()
	{
		मानानि_map.swap(राशिः.पृष्ठभागः());
		राशिः.निष्कासय_पृष्ठभागः();
	}

	शून्य समुच्चयः(RTLIL::SigSpec sig, RTLIL::Const मानम्)
	{
		नियोजय_map.apply(sig);
#यदि_अपरिभाषितम् NDEBUG
		RTLIL::SigSpec current_val = मानानि_map(sig);
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(current_val); i++)
			log_assert(current_val[i].wire != रिक्त || current_val[i] == मानम्[i]);
#यदि_अन्तः
		मानानि_map.सङ्कलय(sig, RTLIL::SigSpec(मानम्));
	}

	शून्य stop(RTLIL::SigSpec sig)
	{
		नियोजय_map.apply(sig);
		stop_सङ्केताः.सङ्कलय(sig);
	}

	bool eval(RTLIL::Cell *cell, RTLIL::SigSpec &undef)
	{
		यदि (cell->type == ID($lcu))
		{
			RTLIL::SigSpec sig_p = cell->get_पत्तनम्(ID::P);
			RTLIL::SigSpec sig_g = cell->get_पत्तनम्(ID::G);
			RTLIL::SigSpec sig_ci = cell->get_पत्तनम्(ID::CI);
			RTLIL::SigSpec sig_co = मानानि_map(नियोजय_map(cell->get_पत्तनम्(ID::CO)));

			यदि (sig_co.is_fully_const())
				प्रत्यागम true;

			यदि (!eval(sig_p, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_g, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_ci, undef, cell))
				प्रत्यागम false;

			यदि (sig_p.is_fully_def() && sig_g.is_fully_def() && sig_ci.is_fully_def())
			{
				RTLIL::Const coval(RTLIL::Sx, Get_आकार(sig_co));
				bool carry = sig_ci.as_bool();

				प्रति (पूर्णाङ्क i = 0; i < Get_आकार(coval); i++) {
					carry = (sig_g[i] == State::S1) || (sig_p[i] == RTLIL::S1 && carry);
					coval.समुच्चयः(i, carry ? State::S1 : State::S0);
				}

				समुच्चयः(sig_co, coval);
			}
			अन्यथा
				समुच्चयः(sig_co, RTLIL::Const(RTLIL::Sx, Get_आकार(sig_co)));

			प्रत्यागम true;
		}

		RTLIL::SigSpec sig_a, sig_b, sig_s, sig_y;

		log_assert(cell->has_पत्तनम्(ID::Y));
		sig_y = मानानि_map(नियोजय_map(cell->get_पत्तनम्(ID::Y)));
		यदि (sig_y.is_fully_const())
			प्रत्यागम true;

		यदि (cell->type == ID($barrier))
		{
			यदि (!model_barriers) {
				undef.append(sig_y);
				प्रत्यागम false;
			}
			sig_a = cell->get_पत्तनम्(ID::A);
			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;
			समुच्चयः(sig_y, sig_a.as_const());
			प्रत्यागम true;
		}

		यदि (cell->has_पत्तनम्(ID::S)) {
			sig_s = cell->get_पत्तनम्(ID::S);
		}

		यदि (cell->has_पत्तनम्(ID::A))
			sig_a = cell->get_पत्तनम्(ID::A);

		यदि (cell->has_पत्तनम्(ID::B))
			sig_b = cell->get_पत्तनम्(ID::B);

		यदि (cell->type.अन्तर्(ID($mux), ID($pmux), ID($_MUX_), ID($_NMUX_)))
		{
			std::सदिशम्<RTLIL::SigSpec> y_candidates;
			पूर्णाङ्क गणनम्_समुच्चयः_s_द्वयङ्काः = 0;

			यदि (!eval(sig_s, undef, cell))
				प्रत्यागम false;

			प्रति (पूर्णाङ्क i = 0; i < sig_s.आकार(); i++)
			{
				RTLIL::State s_bit = sig_s.extract(i, 1).as_const().at(0);
				RTLIL::SigSpec b_खण्डिका = sig_b.extract(sig_y.आकार()*i, sig_y.आकार());

				यदि (s_bit == RTLIL::State::Sx || s_bit == RTLIL::State::S1)
					y_candidates.प्रक्षिप_पृष्ठभागः(b_खण्डिका);

				यदि (s_bit == RTLIL::State::S1)
					गणनम्_समुच्चयः_s_द्वयङ्काः++;
			}

			यदि (गणनम्_समुच्चयः_s_द्वयङ्काः == 0)
				y_candidates.प्रक्षिप_पृष्ठभागः(sig_a);

			std::सदिशम्<RTLIL::Const> y_मानानि;

			log_assert(y_candidates.आकार() > 0);
			प्रति (स्वचालित &yc : y_candidates) {
				यदि (!eval(yc, undef, cell))
					प्रत्यागम false;
				यदि (cell->type == ID($_NMUX_))
					y_मानानि.प्रक्षिप_पृष्ठभागः(RTLIL::const_not(yc.as_const(), Const(), false, false, Get_आकार(yc)));
				अन्यथा
					y_मानानि.प्रक्षिप_पृष्ठभागः(yc.as_const());
			}

			यदि (y_मानानि.आकार() > 1)
			{
				std::सदिशम्<RTLIL::State> master_द्वयङ्काः = y_मानानि.at(0).गन्तव्यम्_द्वयङ्काः();

				प्रति (आकार_t i = 1; i < y_मानानि.आकार(); i++) {
					std::सदिशम्<RTLIL::State> slave_द्वयङ्काः = y_मानानि.at(i).गन्तव्यम्_द्वयङ्काः();
					log_assert(master_द्वयङ्काः.आकार() == slave_द्वयङ्काः.आकार());
					प्रति (आकार_t j = 0; j < master_द्वयङ्काः.आकार(); j++)
						यदि (master_द्वयङ्काः[j] != slave_द्वयङ्काः[j])
							master_द्वयङ्काः[j] = RTLIL::State::Sx;
				}

				समुच्चयः(sig_y, RTLIL::Const(master_द्वयङ्काः));
			}
			अन्यथा
				समुच्चयः(sig_y, y_मानानि.front());
		}
		अन्यथा यदि (cell->type == ID($bmux))
		{
			यदि (!eval(sig_s, undef, cell))
				प्रत्यागम false;

			यदि (sig_s.is_fully_def()) {
				पूर्णाङ्क sel = sig_s.as_int();
				पूर्णाङ्क विस्तारः = Get_आकार(sig_y);
				SigSpec res = sig_a.extract(sel * विस्तारः, विस्तारः);
				यदि (!eval(res, undef, cell))
					प्रत्यागम false;
				समुच्चयः(sig_y, res.as_const());
			} अन्यथा {
				यदि (!eval(sig_a, undef, cell))
					प्रत्यागम false;
				समुच्चयः(sig_y, const_bmux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अन्यथा यदि (cell->type == ID($demux))
		{
			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;
			यदि (sig_a.is_fully_zero()) {
				समुच्चयः(sig_y, Const(0, Get_आकार(sig_y)));
			} अन्यथा {
				यदि (!eval(sig_s, undef, cell))
					प्रत्यागम false;
				समुच्चयः(sig_y, const_demux(sig_a.as_const(), sig_s.as_const()));
			}
		}
		अन्यथा यदि (cell->type == ID($fa))
		{
			RTLIL::SigSpec sig_c = cell->get_पत्तनम्(ID::C);
			RTLIL::SigSpec sig_x = cell->get_पत्तनम्(ID::X);
			पूर्णाङ्क विस्तारः = Get_आकार(sig_c);

			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_b, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_c, undef, cell))
				प्रत्यागम false;

			RTLIL::Const t1 = const_xor(sig_a.as_const(), sig_b.as_const(), false, false, विस्तारः);
			RTLIL::Const val_y = const_xor(t1, sig_c.as_const(), false, false, विस्तारः);

			RTLIL::Const t2 = const_and(sig_a.as_const(), sig_b.as_const(), false, false, विस्तारः);
			RTLIL::Const t3 = const_and(sig_c.as_const(), t1, false, false, विस्तारः);
			RTLIL::Const val_x = const_or(t2, t3, false, false, विस्तारः);

			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(val_y); i++)
				यदि (val_y[i] == RTLIL::Sx)
					val_x.समुच्चयः(i, RTLIL::Sx);

			समुच्चयः(sig_y, val_y);
			समुच्चयः(sig_x, val_x);
		}
		अन्यथा यदि (cell->type == ID($alu))
		{
			bool signed_a = cell->parameters.गणनम्(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool();
			bool signed_b = cell->parameters.गणनम्(ID::B_SIGNED) > 0 && cell->parameters[ID::B_SIGNED].as_bool();

			RTLIL::SigSpec sig_ci = cell->get_पत्तनम्(ID::CI);
			RTLIL::SigSpec sig_bi = cell->get_पत्तनम्(ID::BI);

			यदि (!eval(sig_a, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_b, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_ci, undef, cell))
				प्रत्यागम false;

			यदि (!eval(sig_bi, undef, cell))
				प्रत्यागम false;

			RTLIL::SigSpec sig_x = cell->get_पत्तनम्(ID::X);
			RTLIL::SigSpec sig_co = cell->get_पत्तनम्(ID::CO);

			bool any_निवेशः_undef = !(sig_a.is_fully_def() && sig_b.is_fully_def() && sig_ci.is_fully_def() && sig_bi.is_fully_def());
			sig_a.extend_u0(Get_आकार(sig_y), signed_a);
			sig_b.extend_u0(Get_आकार(sig_y), signed_b);

			bool carry = sig_ci[0] == State::S1;
			bool b_inv = sig_bi[0] == State::S1;

			प्रति (पूर्णाङ्क i = 0; i < Get_आकार(sig_y); i++)
			{
				RTLIL::SigSpec x_inputs = { sig_a[i], sig_b[i], sig_bi[0] };

				यदि (!x_inputs.is_fully_def()) {
					समुच्चयः(sig_x[i], RTLIL::Sx);
				} अन्यथा {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_x = bit_a != bit_b;
					समुच्चयः(sig_x[i], bit_x ? State::S1 : State::S0);
				}

				यदि (any_निवेशः_undef) {
					समुच्चयः(sig_y[i], RTLIL::Sx);
					समुच्चयः(sig_co[i], RTLIL::Sx);
				} अन्यथा {
					bool bit_a = sig_a[i] == State::S1;
					bool bit_b = (sig_b[i] == State::S1) != b_inv;
					bool bit_y = (bit_a != bit_b) != carry;
					carry = (bit_a && bit_b) || (bit_a && carry) || (bit_b && carry);
					समुच्चयः(sig_y[i], bit_y ? State::S1 : State::S0);
					समुच्चयः(sig_co[i], carry ? State::S1 : State::S0);
				}
			}
		}
		अन्यथा यदि (cell->type.अन्तर्(ID($macc), ID($macc_v2)))
		{
			Macc macc;
			macc.from_cell(cell);

			प्रति (स्वचालित &पत्तनम् : macc.terms) {
				यदि (!eval(पत्तनम्.अन्तर्_a, undef, cell))
					प्रत्यागम false;
				यदि (!eval(पत्तनम्.अन्तर्_b, undef, cell))
					प्रत्यागम false;
			}

			RTLIL::Const परिणाम(0, Get_आकार(cell->get_पत्तनम्(ID::Y)));
			यदि (!macc.eval(परिणाम))
				log_abort();

			समुच्चयः(cell->get_पत्तनम्(ID::Y), परिणाम);
		}
		अन्यथा
		{
			RTLIL::SigSpec sig_c, sig_d;

			यदि (cell->type.अन्तर्(ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_))) {
				यदि (cell->has_पत्तनम्(ID::C))
					sig_c = cell->get_पत्तनम्(ID::C);
				यदि (cell->has_पत्तनम्(ID::D))
					sig_d = cell->get_पत्तनम्(ID::D);
			}

			यदि (sig_a.आकार() > 0 && !eval(sig_a, undef, cell))
				प्रत्यागम false;
			यदि (sig_b.आकार() > 0 && !eval(sig_b, undef, cell))
				प्रत्यागम false;
			यदि (sig_c.आकार() > 0 && !eval(sig_c, undef, cell))
				प्रत्यागम false;
			यदि (sig_d.आकार() > 0 && !eval(sig_d, undef, cell))
				प्रत्यागम false;

			bool eval_err = false;
			RTLIL::Const eval_ret;
			यदि (sig_s.आकार() > 0 && eval(sig_s, undef, cell)) {
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_s.as_const(), &eval_err);
			} अन्यथा
				eval_ret = CellTypes::eval(cell, sig_a.as_const(), sig_b.as_const(), sig_c.as_const(), sig_d.as_const(), &eval_err);

			यदि (eval_err)
				प्रत्यागम false;

			समुच्चयः(sig_y, eval_ret);
		}

		प्रत्यागम true;
	}

	bool eval(RTLIL::SigSpec &sig, RTLIL::SigSpec &undef, RTLIL::Cell *busy_cell = रिक्त)
	{
		नियोजय_map.apply(sig);
		मानानि_map.apply(sig);

		यदि (sig.is_fully_const())
			प्रत्यागम true;

		यदि (stop_सङ्केताः.check_any(sig)) {
			undef = stop_सङ्केताः.extract(sig);
			प्रत्यागम false;
		}

		यदि (busy_cell) {
			यदि (busy.गणनम्(busy_cell) > 0) {
				undef = sig;
				प्रत्यागम false;
			}
			busy.निवेशय(busy_cell);
		}

		std::समुच्चयः<RTLIL::Cell*> driver_cells;
		sig2driver.find(sig, driver_cells);
		प्रति (स्वचालित cell : driver_cells) {
			यदि (!eval(cell, undef)) {
				यदि (busy_cell)
					busy.erase(busy_cell);
				प्रत्यागम false;
			}
		}

		यदि (busy_cell)
			busy.erase(busy_cell);

		मानानि_map.apply(sig);
		यदि (sig.is_fully_const())
			प्रत्यागम true;

		यदि (defaultval != RTLIL::State::Sm) {
			प्रति (स्वचालित &bit : sig)
				यदि (bit.wire) bit = defaultval;
			प्रत्यागम true;
		}

		प्रति (स्वचालित &c : sig.chunks())
			यदि (c.wire != रिक्त)
				undef.append(c);
		प्रत्यागम false;
	}

	bool eval(RTLIL::SigSpec &sig)
	{
		RTLIL::SigSpec undef;
		प्रत्यागम eval(sig, undef);
	}
};

YOSYS_NAMESPACE_अन्तः

#यदि_अन्तः
