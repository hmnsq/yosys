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

#جے_بنا_تعریف SATGEN_H
#تعریف_کرو SATGEN_H

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
	SatSolver *اگلا;
	مجازی ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		اگلا = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	مجازی ~SatSolver() {
		خودکار p = &yosys_satsolver_list;
		طالما (*p) {
			إذا (*p == this)
				*p = اگلا;
			وإلا
				p = &(*p)->اگلا;
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
	std::string اگیتر_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_ثنائی_ہندسہ, صحيح>> imported_اشارے;
	std::map<std::pair<std::string, صحيح>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ثابت SigMap *sigmap, std::string اگیتر_2 = std::string()) :
			ez(ez), sigmap(sigmap), اگیتر_2(اگیتر_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	فراغ سیٹ_Context(ثابت SigMap *sigmap, std::string اگیتر_2 = std::string())
	{
		this->sigmap = sigmap;
		this->اگیتر_2 = اگیتر_2;
	}

	std::سمتیہ<صحيح> import_Sig_Spec_کامہ(RTLIL::SigSpec sig, std::string &pf, bool undef_منوال, bool dup_undef)
	{
		log_assert(!undef_منوال || model_undef);
		sigmap->apply(sig);

		std::سمتیہ<صحيح> vec;
		vec.reserve(Get_ناپ(sig));

		لكل (خودکار &ثنائی_ہندسہ : sig)
			إذا (ثنائی_ہندسہ.wire == عدم) {
				إذا (model_undef && dup_undef && ثنائی_ہندسہ == RTLIL::State::Sx)
					vec.ڈالو_پٹھ(ez->frozen_literal());
				وإلا
					vec.ڈالو_پٹھ(ثنائی_ہندسہ == (undef_منوال ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} وإلا {
				std::string wire_name = ثنائی_ہندسہ.wire->name.unescape();
				std::string name = pf +
					(ثنائی_ہندسہ.wire->چوڑائی == 1 ? wire_name : stringf("%s [%d]", wire_name, ثنائی_ہندسہ.وتھ));
				vec.ڈالو_پٹھ(ez->frozen_literal(name));
				imported_اشارے[pf][ثنائی_ہندسہ] = vec.پٹھ();
			}
		أرجع vec;
	}

	std::سمتیہ<صحيح> importSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کامہ(sig, pf, false, false);
	}

	std::سمتیہ<صحيح> importDefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کامہ(sig, pf, false, true);
	}

	std::سمتیہ<صحيح> importUndefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کامہ(sig, pf, true, false);
	}

	صحيح import_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کامہ(ثنائی_ہندسہ, pf, false, false).front();
	}

	صحيح import_Def_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کامہ(ثنائی_ہندسہ, pf, false, true).front();
	}

	صحيح import_Undef_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کامہ(ثنائی_ہندسہ, pf, true, false).front();
	}

	bool imported_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع imported_اشارے[pf].گنتی(ثنائی_ہندسہ) != 0;
	}

	فراغ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	فراغ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	صحيح importAsserts(صحيح timestep = -1)
	{
		std::سمتیہ<صحيح> check_بٹ, enable_بٹ;
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} وإلا {
			check_بٹ = importDefSigSpec(asserts_a[pf], timestep);
			enable_بٹ = importDefSigSpec(asserts_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بٹ, ez->vec_not(enable_بٹ)));
	}

	صحيح importAssumes(صحيح timestep = -1)
	{
		std::سمتیہ<صحيح> check_بٹ, enable_بٹ;
		std::string pf = اگیتر_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} وإلا {
			check_بٹ = importDefSigSpec(assumes_a[pf], timestep);
			enable_بٹ = importDefSigSpec(assumes_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بٹ, ez->vec_not(enable_بٹ)));
	}

	صحيح اشارے_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, صحيح timestep_lhs = -1, صحيح timestep_rhs = -1)
	{
		إذا (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.ناپ() == rhs.ناپ());

		std::سمتیہ<صحيح> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::سمتیہ<صحيح> vec_rhs = importSigSpec(rhs, timestep_rhs);

		إذا (!model_undef)
			أرجع ez->vec_eq(vec_lhs, vec_rhs);

		std::سمتیہ<صحيح> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::سمتیہ<صحيح> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::سمتیہ<صحيح> eq_بٹ;
		لكل (صحيح i = 0; i < lhs.ناپ(); i++)
			eq_بٹ.ڈالو_پٹھ(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		أرجع ez->عبارت(ezSAT::OpAnd, eq_بٹ);
	}

	فراغ extend_Signal_چوڑائی(std::سمتیہ<صحيح> &vec_a, std::سمتیہ<صحيح> &vec_b, RTLIL::Cell *cell, ناپ_t y_چوڑائی = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		إذا (!forced_signed && cell->parameters.گنتی(ID::A_SIGNED) > 0 && cell->parameters.گنتی(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		طالما (vec_a.ناپ() < vec_b.ناپ() || vec_a.ناپ() < y_چوڑائی)
			vec_a.ڈالو_پٹھ(is_signed && vec_a.ناپ() > 0 ? vec_a.پٹھ() : ez->CONST_FALSE);
		طالما (vec_b.ناپ() < vec_a.ناپ() || vec_b.ناپ() < y_چوڑائی)
			vec_b.ڈالو_پٹھ(is_signed && vec_b.ناپ() > 0 ? vec_b.پٹھ() : ez->CONST_FALSE);
	}

	فراغ extend_Signal_چوڑائی(std::سمتیہ<صحيح> &vec_a, std::سمتیہ<صحيح> &vec_b, std::سمتیہ<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_چوڑائی(vec_a, vec_b, cell, vec_y.ناپ(), forced_signed);
		طالما (vec_y.ناپ() < vec_a.ناپ())
			vec_y.ڈالو_پٹھ(ez->literal());
	}

	فراغ extend_Signal_چوڑائی_Unary(std::سمتیہ<صحيح> &vec_a, std::سمتیہ<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.گنتی(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		طالما (vec_a.ناپ() < vec_y.ناپ())
			vec_a.ڈالو_پٹھ(is_signed && vec_a.ناپ() > 0 ? vec_a.پٹھ() : ez->CONST_FALSE);
		طالما (vec_y.ناپ() < vec_a.ناپ())
			vec_y.ڈالو_پٹھ(ez->literal());
	}

	فراغ undefGating(std::سمتیہ<صحيح> &vec_y, std::سمتیہ<صحيح> &vec_yy, std::سمتیہ<صحيح> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.ناپ() == vec_yy.ناپ());
		إذا (vec_y.ناپ() > vec_undef.ناپ()) {
			std::سمتیہ<صحيح> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.ناپ());
			std::سمتیہ<صحيح> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.ناپ());
			ez->assume(ez->عبارت(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} وإلا {
			log_assert(vec_y.ناپ() == vec_undef.ناپ());
			ez->assume(ez->عبارت(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::سمتیہ<صحيح>, std::سمتیہ<صحيح>> mux(صحيح s, صحيح undef_s, ثابت std::سمتیہ<صحيح> &a, ثابت std::سمتیہ<صحيح> &undef_a, ثابت std::سمتیہ<صحيح> &b, ثابت std::سمتیہ<صحيح> &undef_b) {
		std::سمتیہ<صحيح> res;
		std::سمتیہ<صحيح> undef_res;
		res = ez->vec_ite(s, b, a);
		إذا (model_undef) {
			std::سمتیہ<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::سمتیہ<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		أرجع std::make_pair(res, undef_res);
	}

	فراغ undefGating(صحيح y, صحيح yy, صحيح undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	فراغ سیٹ_Init_State(صحيح timestep)
	{
		خودکار کلید = make_pair(اگیتر_2, timestep);
		log_assert(initstates.گنتی(کلید) == 0 || initstates.at(کلید) == true);
		initstates[کلید] = true;
	}

	bool importCell(RTLIL::Cell *cell, صحيح timestep = -1);
};

فراغ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_سرا

#جے_انت
