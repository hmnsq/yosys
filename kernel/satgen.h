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

#جيڪڏهن_وصف_نه_ٿيل SATGEN_H
#وصف_ڪريو SATGEN_H

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
	SatSolver *ايندڙ;
	مجازي ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		ايندڙ = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	مجازي ~SatSolver() {
		خودڪار p = &yosys_satsolver_list;
		طالما (*p) {
			إذا (*p == this)
				*p = ايندڙ;
			وإلا
				p = &(*p)->ايندڙ;
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
	std::string اڳياڙي_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_ٻٽو_انگ, صحيح>> imported_سگنل;
	std::map<std::pair<std::string, صحيح>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ثابت SigMap *sigmap, std::string اڳياڙي_2 = std::string()) :
			ez(ez), sigmap(sigmap), اڳياڙي_2(اڳياڙي_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	فراغ سيٽ_Context(ثابت SigMap *sigmap, std::string اڳياڙي_2 = std::string())
	{
		this->sigmap = sigmap;
		this->اڳياڙي_2 = اڳياڙي_2;
	}

	std::ويڪٽر<صحيح> import_Sig_Spec_ڪم_ڪندڙ(RTLIL::SigSpec sig, std::string &pf, bool undef_گهڻائي, bool dup_undef)
	{
		log_assert(!undef_گهڻائي || model_undef);
		sigmap->apply(sig);

		std::ويڪٽر<صحيح> vec;
		vec.reserve(Get_سائيز(sig));

		لكل (خودڪار &ٻٽو_انگ : sig)
			إذا (ٻٽو_انگ.wire == عدم) {
				إذا (model_undef && dup_undef && ٻٽو_انگ == RTLIL::State::Sx)
					vec.ڌڪو_پٺ(ez->frozen_literal());
				وإلا
					vec.ڌڪو_پٺ(ٻٽو_انگ == (undef_گهڻائي ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} وإلا {
				std::string wire_name = ٻٽو_انگ.wire->name.unescape();
				std::string name = pf +
					(ٻٽو_انگ.wire->ويڪر == 1 ? wire_name : stringf("%s [%d]", wire_name, ٻٽو_انگ.وٿي));
				vec.ڌڪو_پٺ(ez->frozen_literal(name));
				imported_سگنل[pf][ٻٽو_انگ] = vec.پٺ();
			}
		أرجع vec;
	}

	std::ويڪٽر<صحيح> importSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_ڪم_ڪندڙ(sig, pf, false, false);
	}

	std::ويڪٽر<صحيح> importDefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_ڪم_ڪندڙ(sig, pf, false, true);
	}

	std::ويڪٽر<صحيح> importUndefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_ڪم_ڪندڙ(sig, pf, true, false);
	}

	صحيح import_Sig_ٻٽو_انگ(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_ڪم_ڪندڙ(ٻٽو_انگ, pf, false, false).front();
	}

	صحيح import_Def_Sig_ٻٽو_انگ(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_ڪم_ڪندڙ(ٻٽو_انگ, pf, false, true).front();
	}

	صحيح import_Undef_Sig_ٻٽو_انگ(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_ڪم_ڪندڙ(ٻٽو_انگ, pf, true, false).front();
	}

	bool imported_Sig_ٻٽو_انگ(RTLIL::Sig_ٻٽو_انگ ٻٽو_انگ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع imported_سگنل[pf].ڳڻپ(ٻٽو_انگ) != 0;
	}

	فراغ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	فراغ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	صحيح importAsserts(صحيح timestep = -1)
	{
		std::ويڪٽر<صحيح> check_بٽ, enable_بٽ;
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بٽ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_بٽ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} وإلا {
			check_بٽ = importDefSigSpec(asserts_a[pf], timestep);
			enable_بٽ = importDefSigSpec(asserts_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بٽ, ez->vec_not(enable_بٽ)));
	}

	صحيح importAssumes(صحيح timestep = -1)
	{
		std::ويڪٽر<صحيح> check_بٽ, enable_بٽ;
		std::string pf = اڳياڙي_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بٽ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_بٽ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} وإلا {
			check_بٽ = importDefSigSpec(assumes_a[pf], timestep);
			enable_بٽ = importDefSigSpec(assumes_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بٽ, ez->vec_not(enable_بٽ)));
	}

	صحيح سگنل_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, صحيح timestep_lhs = -1, صحيح timestep_rhs = -1)
	{
		إذا (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.سائيز() == rhs.سائيز());

		std::ويڪٽر<صحيح> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::ويڪٽر<صحيح> vec_rhs = importSigSpec(rhs, timestep_rhs);

		إذا (!model_undef)
			أرجع ez->vec_eq(vec_lhs, vec_rhs);

		std::ويڪٽر<صحيح> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::ويڪٽر<صحيح> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::ويڪٽر<صحيح> eq_بٽ;
		لكل (صحيح i = 0; i < lhs.سائيز(); i++)
			eq_بٽ.ڌڪو_پٺ(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		أرجع ez->اظهار_2(ezSAT::OpAnd, eq_بٽ);
	}

	فراغ extend_Signal_ويڪر(std::ويڪٽر<صحيح> &vec_a, std::ويڪٽر<صحيح> &vec_b, RTLIL::Cell *cell, سائيز_t y_ويڪر = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		إذا (!forced_signed && cell->parameters.ڳڻپ(ID::A_SIGNED) > 0 && cell->parameters.ڳڻپ(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		طالما (vec_a.سائيز() < vec_b.سائيز() || vec_a.سائيز() < y_ويڪر)
			vec_a.ڌڪو_پٺ(is_signed && vec_a.سائيز() > 0 ? vec_a.پٺ() : ez->CONST_FALSE);
		طالما (vec_b.سائيز() < vec_a.سائيز() || vec_b.سائيز() < y_ويڪر)
			vec_b.ڌڪو_پٺ(is_signed && vec_b.سائيز() > 0 ? vec_b.پٺ() : ez->CONST_FALSE);
	}

	فراغ extend_Signal_ويڪر(std::ويڪٽر<صحيح> &vec_a, std::ويڪٽر<صحيح> &vec_b, std::ويڪٽر<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_ويڪر(vec_a, vec_b, cell, vec_y.سائيز(), forced_signed);
		طالما (vec_y.سائيز() < vec_a.سائيز())
			vec_y.ڌڪو_پٺ(ez->literal());
	}

	فراغ extend_Signal_ويڪر_Unary(std::ويڪٽر<صحيح> &vec_a, std::ويڪٽر<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.ڳڻپ(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		طالما (vec_a.سائيز() < vec_y.سائيز())
			vec_a.ڌڪو_پٺ(is_signed && vec_a.سائيز() > 0 ? vec_a.پٺ() : ez->CONST_FALSE);
		طالما (vec_y.سائيز() < vec_a.سائيز())
			vec_y.ڌڪو_پٺ(ez->literal());
	}

	فراغ undefGating(std::ويڪٽر<صحيح> &vec_y, std::ويڪٽر<صحيح> &vec_yy, std::ويڪٽر<صحيح> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.سائيز() == vec_yy.سائيز());
		إذا (vec_y.سائيز() > vec_undef.سائيز()) {
			std::ويڪٽر<صحيح> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.سائيز());
			std::ويڪٽر<صحيح> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.سائيز());
			ez->assume(ez->اظهار_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} وإلا {
			log_assert(vec_y.سائيز() == vec_undef.سائيز());
			ez->assume(ez->اظهار_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::ويڪٽر<صحيح>, std::ويڪٽر<صحيح>> mux(صحيح s, صحيح undef_s, ثابت std::ويڪٽر<صحيح> &a, ثابت std::ويڪٽر<صحيح> &undef_a, ثابت std::ويڪٽر<صحيح> &b, ثابت std::ويڪٽر<صحيح> &undef_b) {
		std::ويڪٽر<صحيح> res;
		std::ويڪٽر<صحيح> undef_res;
		res = ez->vec_ite(s, b, a);
		إذا (model_undef) {
			std::ويڪٽر<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ويڪٽر<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		أرجع std::make_pair(res, undef_res);
	}

	فراغ undefGating(صحيح y, صحيح yy, صحيح undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	فراغ سيٽ_Init_State(صحيح timestep)
	{
		خودڪار ڪنجي = make_pair(اڳياڙي_2, timestep);
		log_assert(initstates.ڳڻپ(ڪنجي) == 0 || initstates.at(ڪنجي) == true);
		initstates[ڪنجي] = true;
	}

	bool importCell(RTLIL::Cell *cell, صحيح timestep = -1);
};

فراغ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
