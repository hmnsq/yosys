/* -*- c++ -*-
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

#నిర్వచితం_కాకపోతే SATGEN_H
#నిర్వచించు SATGEN_H

#అన్తర్భావ "kernel/rtlil.h"
#అన్తర్భావ "kernel/sigtools.h"
#అన్తర్భావ "kernel/celltypes.h"
#అన్తర్భావ "kernel/macc.h"

#అన్తర్భావ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
బాహ్య నిర్మాణం SatSolver *yosys_satsolver_list;
బాహ్య నిర్మాణం SatSolver *yosys_satsolver;

నిర్మాణం SatSolver
{
	string name;
	SatSolver *తదుపరి;
	మాయా ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		తదుపరి = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	మాయా ~SatSolver() {
		స్వయంచాలక p = &yosys_satsolver_list;
		అయితే (*p) {
			ఒకవేళ (*p == this)
				*p = తదుపరి;
			లేకపోతే
				p = &(*p)->తదుపరి;
		}
		ఒకవేళ (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

నిర్మాణం ezSatPtr : బహిరంగ std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

నిర్మాణం SatGen
{
	ezSAT *ez;
	స్థిరరాశి SigMap *sigmap;
	std::string ఉపసర్గ_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_బిట్టు, పూర్ణాంకం>> imported_సంకేతాలు;
	std::map<std::pair<std::string, పూర్ణాంకం>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, స్థిరరాశి SigMap *sigmap, std::string ఉపసర్గ_2 = std::string()) :
			ez(ez), sigmap(sigmap), ఉపసర్గ_2(ఉపసర్గ_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	శూన్య సమితి_Context(స్థిరరాశి SigMap *sigmap, std::string ఉపసర్గ_2 = std::string())
	{
		this->sigmap = sigmap;
		this->ఉపసర్గ_2 = ఉపసర్గ_2;
	}

	std::సదిశ<పూర్ణాంకం> import_Sig_Spec_కార్మికుడు(RTLIL::SigSpec sig, std::string &pf, bool undef_బహుళకం, bool dup_undef)
	{
		log_assert(!undef_బహుళకం || model_undef);
		sigmap->apply(sig);

		std::సదిశ<పూర్ణాంకం> vec;
		vec.reserve(Get_కొలత(sig));

		కొరకు (స్వయంచాలక &బిట్టు : sig)
			ఒకవేళ (బిట్టు.wire == ఖాళీ) {
				ఒకవేళ (model_undef && dup_undef && బిట్టు == RTLIL::State::Sx)
					vec.నెట్టు_వెనుక(ez->frozen_literal());
				లేకపోతే
					vec.నెట్టు_వెనుక(బిట్టు == (undef_బహుళకం ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} లేకపోతే {
				std::string wire_name = బిట్టు.wire->name.unescape();
				std::string name = pf +
					(బిట్టు.wire->వెడల్పు == 1 ? wire_name : stringf("%s [%d]", wire_name, బిట్టు.స్థానభ్రంశం));
				vec.నెట్టు_వెనుక(ez->frozen_literal(name));
				imported_సంకేతాలు[pf][బిట్టు] = vec.వెనుక();
			}
		తిరిగి vec;
	}

	std::సదిశ<పూర్ణాంకం> importSigSpec(RTLIL::SigSpec sig, పూర్ణాంకం timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		తిరిగి import_Sig_Spec_కార్మికుడు(sig, pf, false, false);
	}

	std::సదిశ<పూర్ణాంకం> importDefSigSpec(RTLIL::SigSpec sig, పూర్ణాంకం timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		తిరిగి import_Sig_Spec_కార్మికుడు(sig, pf, false, true);
	}

	std::సదిశ<పూర్ణాంకం> importUndefSigSpec(RTLIL::SigSpec sig, పూర్ణాంకం timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		తిరిగి import_Sig_Spec_కార్మికుడు(sig, pf, true, false);
	}

	పూర్ణాంకం import_Sig_బిట్టు(RTLIL::Sig_బిట్టు బిట్టు, పూర్ణాంకం timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		తిరిగి import_Sig_Spec_కార్మికుడు(బిట్టు, pf, false, false).front();
	}

	పూర్ణాంకం import_Def_Sig_బిట్టు(RTLIL::Sig_బిట్టు బిట్టు, పూర్ణాంకం timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		తిరిగి import_Sig_Spec_కార్మికుడు(బిట్టు, pf, false, true).front();
	}

	పూర్ణాంకం import_Undef_Sig_బిట్టు(RTLIL::Sig_బిట్టు బిట్టు, పూర్ణాంకం timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		తిరిగి import_Sig_Spec_కార్మికుడు(బిట్టు, pf, true, false).front();
	}

	bool imported_Sig_బిట్టు(RTLIL::Sig_బిట్టు బిట్టు, పూర్ణాంకం timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		తిరిగి imported_సంకేతాలు[pf].లెక్క(బిట్టు) != 0;
	}

	శూన్య getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, పూర్ణాంకం timestep = -1)
	{
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	శూన్య getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, పూర్ణాంకం timestep = -1)
	{
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	పూర్ణాంకం importAsserts(పూర్ణాంకం timestep = -1)
	{
		std::సదిశ<పూర్ణాంకం> check_బిట్లు, enable_బిట్లు;
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ఒకవేళ (model_undef) {
			check_బిట్లు = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_బిట్లు = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} లేకపోతే {
			check_బిట్లు = importDefSigSpec(asserts_a[pf], timestep);
			enable_బిట్లు = importDefSigSpec(asserts_en[pf], timestep);
		}
		తిరిగి ez->vec_reduce_and(ez->vec_or(check_బిట్లు, ez->vec_not(enable_బిట్లు)));
	}

	పూర్ణాంకం importAssumes(పూర్ణాంకం timestep = -1)
	{
		std::సదిశ<పూర్ణాంకం> check_బిట్లు, enable_బిట్లు;
		std::string pf = ఉపసర్గ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ఒకవేళ (model_undef) {
			check_బిట్లు = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_బిట్లు = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} లేకపోతే {
			check_బిట్లు = importDefSigSpec(assumes_a[pf], timestep);
			enable_బిట్లు = importDefSigSpec(assumes_en[pf], timestep);
		}
		తిరిగి ez->vec_reduce_and(ez->vec_or(check_బిట్లు, ez->vec_not(enable_బిట్లు)));
	}

	పూర్ణాంకం సంకేతాలు_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, పూర్ణాంకం timestep_lhs = -1, పూర్ణాంకం timestep_rhs = -1)
	{
		ఒకవేళ (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.కొలత() == rhs.కొలత());

		std::సదిశ<పూర్ణాంకం> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::సదిశ<పూర్ణాంకం> vec_rhs = importSigSpec(rhs, timestep_rhs);

		ఒకవేళ (!model_undef)
			తిరిగి ez->vec_eq(vec_lhs, vec_rhs);

		std::సదిశ<పూర్ణాంకం> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::సదిశ<పూర్ణాంకం> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::సదిశ<పూర్ణాంకం> eq_బిట్లు;
		కొరకు (పూర్ణాంకం i = 0; i < lhs.కొలత(); i++)
			eq_బిట్లు.నెట్టు_వెనుక(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		తిరిగి ez->వ్యక్తీకరణ(ezSAT::OpAnd, eq_బిట్లు);
	}

	శూన్య extend_Signal_వెడల్పు(std::సదిశ<పూర్ణాంకం> &vec_a, std::సదిశ<పూర్ణాంకం> &vec_b, RTLIL::Cell *cell, కొలత_t y_వెడల్పు = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		ఒకవేళ (!forced_signed && cell->parameters.లెక్క(ID::A_SIGNED) > 0 && cell->parameters.లెక్క(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		అయితే (vec_a.కొలత() < vec_b.కొలత() || vec_a.కొలత() < y_వెడల్పు)
			vec_a.నెట్టు_వెనుక(is_signed && vec_a.కొలత() > 0 ? vec_a.వెనుక() : ez->CONST_FALSE);
		అయితే (vec_b.కొలత() < vec_a.కొలత() || vec_b.కొలత() < y_వెడల్పు)
			vec_b.నెట్టు_వెనుక(is_signed && vec_b.కొలత() > 0 ? vec_b.వెనుక() : ez->CONST_FALSE);
	}

	శూన్య extend_Signal_వెడల్పు(std::సదిశ<పూర్ణాంకం> &vec_a, std::సదిశ<పూర్ణాంకం> &vec_b, std::సదిశ<పూర్ణాంకం> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_వెడల్పు(vec_a, vec_b, cell, vec_y.కొలత(), forced_signed);
		అయితే (vec_y.కొలత() < vec_a.కొలత())
			vec_y.నెట్టు_వెనుక(ez->literal());
	}

	శూన్య extend_Signal_వెడల్పు_Unary(std::సదిశ<పూర్ణాంకం> &vec_a, std::సదిశ<పూర్ణాంకం> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.లెక్క(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		అయితే (vec_a.కొలత() < vec_y.కొలత())
			vec_a.నెట్టు_వెనుక(is_signed && vec_a.కొలత() > 0 ? vec_a.వెనుక() : ez->CONST_FALSE);
		అయితే (vec_y.కొలత() < vec_a.కొలత())
			vec_y.నెట్టు_వెనుక(ez->literal());
	}

	శూన్య undefGating(std::సదిశ<పూర్ణాంకం> &vec_y, std::సదిశ<పూర్ణాంకం> &vec_yy, std::సదిశ<పూర్ణాంకం> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.కొలత() == vec_yy.కొలత());
		ఒకవేళ (vec_y.కొలత() > vec_undef.కొలత()) {
			std::సదిశ<పూర్ణాంకం> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.కొలత());
			std::సదిశ<పూర్ణాంకం> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.కొలత());
			ez->assume(ez->వ్యక్తీకరణ(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} లేకపోతే {
			log_assert(vec_y.కొలత() == vec_undef.కొలత());
			ez->assume(ez->వ్యక్తీకరణ(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::సదిశ<పూర్ణాంకం>, std::సదిశ<పూర్ణాంకం>> mux(పూర్ణాంకం s, పూర్ణాంకం undef_s, స్థిరరాశి std::సదిశ<పూర్ణాంకం> &a, స్థిరరాశి std::సదిశ<పూర్ణాంకం> &undef_a, స్థిరరాశి std::సదిశ<పూర్ణాంకం> &b, స్థిరరాశి std::సదిశ<పూర్ణాంకం> &undef_b) {
		std::సదిశ<పూర్ణాంకం> res;
		std::సదిశ<పూర్ణాంకం> undef_res;
		res = ez->vec_ite(s, b, a);
		ఒకవేళ (model_undef) {
			std::సదిశ<పూర్ణాంకం> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::సదిశ<పూర్ణాంకం> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		తిరిగి std::make_pair(res, undef_res);
	}

	శూన్య undefGating(పూర్ణాంకం y, పూర్ణాంకం yy, పూర్ణాంకం undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	శూన్య సమితి_Init_State(పూర్ణాంకం timestep)
	{
		స్వయంచాలక కీలకం = make_pair(ఉపసర్గ_2, timestep);
		log_assert(initstates.లెక్క(కీలకం) == 0 || initstates.at(కీలకం) == true);
		initstates[కీలకం] = true;
	}

	bool importCell(RTLIL::Cell *cell, పూర్ణాంకం timestep = -1);
};

శూన్య report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_చివర

#అయితే_ముగింపు
