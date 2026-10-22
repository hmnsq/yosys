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

#که_نه_تعریف_شوی SATGEN_H
#تعریف_کړه SATGEN_H

#ضمن "kernel/rtlil.h"
#ضمن "kernel/sigtools.h"
#ضمن "kernel/celltypes.h"
#ضمن "kernel/macc.h"

#ضمن "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
خارجي بنية SatSolver *yosys_satsolver_list;
خارجي بنية SatSolver *yosys_satsolver;

بنية SatSolver
{
	string name;
	SatSolver *بل;
	مجازي ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		بل = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	مجازي ~SatSolver() {
		خپلکاری p = &yosys_satsolver_list;
		طالما (*p) {
			إذا (*p == this)
				*p = بل;
			وإلا
				p = &(*p)->بل;
		}
		إذا (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

بنية ezSatPtr : عام std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

بنية SatGen
{
	ezSAT *ez;
	ثابت SigMap *sigmap;
	std::string مختاړی_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_بټ, صحيح>> imported_سیګنالونه;
	std::map<std::pair<std::string, صحيح>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ثابت SigMap *sigmap, std::string مختاړی_2 = std::string()) :
			ez(ez), sigmap(sigmap), مختاړی_2(مختاړی_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	فراغ سېټ_Context(ثابت SigMap *sigmap, std::string مختاړی_2 = std::string())
	{
		this->sigmap = sigmap;
		this->مختاړی_2 = مختاړی_2;
	}

	std::ویکټور<صحيح> import_Sig_Spec_کارګر(RTLIL::SigSpec sig, std::string &pf, bool undef_ډېرپېښ, bool dup_undef)
	{
		log_assert(!undef_ډېرپېښ || model_undef);
		sigmap->apply(sig);

		std::ویکټور<صحيح> vec;
		vec.reserve(Get_اندازه(sig));

		لكل (خپلکاری &بټ : sig)
			إذا (بټ.wire == عدم) {
				إذا (model_undef && dup_undef && بټ == RTLIL::State::Sx)
					vec.ټېل_شا(ez->frozen_literal());
				وإلا
					vec.ټېل_شا(بټ == (undef_ډېرپېښ ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} وإلا {
				std::string wire_name = بټ.wire->name.unescape();
				std::string name = pf +
					(بټ.wire->پلنوالی == 1 ? wire_name : stringf("%s [%d]", wire_name, بټ.دننه_واټن));
				vec.ټېل_شا(ez->frozen_literal(name));
				imported_سیګنالونه[pf][بټ] = vec.شا();
			}
		أرجع vec;
	}

	std::ویکټور<صحيح> importSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارګر(sig, pf, false, false);
	}

	std::ویکټور<صحيح> importDefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارګر(sig, pf, false, true);
	}

	std::ویکټور<صحيح> importUndefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارګر(sig, pf, true, false);
	}

	صحيح import_Sig_بټ(RTLIL::Sig_بټ بټ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارګر(بټ, pf, false, false).front();
	}

	صحيح import_Def_Sig_بټ(RTLIL::Sig_بټ بټ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارګر(بټ, pf, false, true).front();
	}

	صحيح import_Undef_Sig_بټ(RTLIL::Sig_بټ بټ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارګر(بټ, pf, true, false).front();
	}

	bool imported_Sig_بټ(RTLIL::Sig_بټ بټ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع imported_سیګنالونه[pf].شمېر(بټ) != 0;
	}

	فراغ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	فراغ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	صحيح importAsserts(صحيح timestep = -1)
	{
		std::ویکټور<صحيح> check_بټونه, enable_بټونه;
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بټونه = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_بټونه = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} وإلا {
			check_بټونه = importDefSigSpec(asserts_a[pf], timestep);
			enable_بټونه = importDefSigSpec(asserts_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بټونه, ez->vec_not(enable_بټونه)));
	}

	صحيح importAssumes(صحيح timestep = -1)
	{
		std::ویکټور<صحيح> check_بټونه, enable_بټونه;
		std::string pf = مختاړی_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بټونه = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_بټونه = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} وإلا {
			check_بټونه = importDefSigSpec(assumes_a[pf], timestep);
			enable_بټونه = importDefSigSpec(assumes_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بټونه, ez->vec_not(enable_بټونه)));
	}

	صحيح سیګنالونه_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, صحيح timestep_lhs = -1, صحيح timestep_rhs = -1)
	{
		إذا (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.اندازه() == rhs.اندازه());

		std::ویکټور<صحيح> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::ویکټور<صحيح> vec_rhs = importSigSpec(rhs, timestep_rhs);

		إذا (!model_undef)
			أرجع ez->vec_eq(vec_lhs, vec_rhs);

		std::ویکټور<صحيح> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::ویکټور<صحيح> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::ویکټور<صحيح> eq_بټونه;
		لكل (صحيح i = 0; i < lhs.اندازه(); i++)
			eq_بټونه.ټېل_شا(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		أرجع ez->افاده(ezSAT::OpAnd, eq_بټونه);
	}

	فراغ extend_Signal_پلنوالی(std::ویکټور<صحيح> &vec_a, std::ویکټور<صحيح> &vec_b, RTLIL::Cell *cell, اندازه_t y_پلنوالی = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		إذا (!forced_signed && cell->parameters.شمېر(ID::A_SIGNED) > 0 && cell->parameters.شمېر(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		طالما (vec_a.اندازه() < vec_b.اندازه() || vec_a.اندازه() < y_پلنوالی)
			vec_a.ټېل_شا(is_signed && vec_a.اندازه() > 0 ? vec_a.شا() : ez->CONST_FALSE);
		طالما (vec_b.اندازه() < vec_a.اندازه() || vec_b.اندازه() < y_پلنوالی)
			vec_b.ټېل_شا(is_signed && vec_b.اندازه() > 0 ? vec_b.شا() : ez->CONST_FALSE);
	}

	فراغ extend_Signal_پلنوالی(std::ویکټور<صحيح> &vec_a, std::ویکټور<صحيح> &vec_b, std::ویکټور<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_پلنوالی(vec_a, vec_b, cell, vec_y.اندازه(), forced_signed);
		طالما (vec_y.اندازه() < vec_a.اندازه())
			vec_y.ټېل_شا(ez->literal());
	}

	فراغ extend_Signal_پلنوالی_Unary(std::ویکټور<صحيح> &vec_a, std::ویکټور<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.شمېر(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		طالما (vec_a.اندازه() < vec_y.اندازه())
			vec_a.ټېل_شا(is_signed && vec_a.اندازه() > 0 ? vec_a.شا() : ez->CONST_FALSE);
		طالما (vec_y.اندازه() < vec_a.اندازه())
			vec_y.ټېل_شا(ez->literal());
	}

	فراغ undefGating(std::ویکټور<صحيح> &vec_y, std::ویکټور<صحيح> &vec_yy, std::ویکټور<صحيح> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.اندازه() == vec_yy.اندازه());
		إذا (vec_y.اندازه() > vec_undef.اندازه()) {
			std::ویکټور<صحيح> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.اندازه());
			std::ویکټور<صحيح> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.اندازه());
			ez->assume(ez->افاده(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} وإلا {
			log_assert(vec_y.اندازه() == vec_undef.اندازه());
			ez->assume(ez->افاده(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::ویکټور<صحيح>, std::ویکټور<صحيح>> mux(صحيح s, صحيح undef_s, ثابت std::ویکټور<صحيح> &a, ثابت std::ویکټور<صحيح> &undef_a, ثابت std::ویکټور<صحيح> &b, ثابت std::ویکټور<صحيح> &undef_b) {
		std::ویکټور<صحيح> res;
		std::ویکټور<صحيح> undef_res;
		res = ez->vec_ite(s, b, a);
		إذا (model_undef) {
			std::ویکټور<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ویکټور<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		أرجع std::make_pair(res, undef_res);
	}

	فراغ undefGating(صحيح y, صحيح yy, صحيح undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	فراغ سېټ_Init_State(صحيح timestep)
	{
		خپلکاری کلی = make_pair(مختاړی_2, timestep);
		log_assert(initstates.شمېر(کلی) == 0 || initstates.at(کلی) == true);
		initstates[کلی] = true;
	}

	bool importCell(RTLIL::Cell *cell, صحيح timestep = -1);
};

فراغ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_پای_ټکی

#د_که_پای
