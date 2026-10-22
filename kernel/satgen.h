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

#اگر_تعریف_نشده SATGEN_H
#تعریف_کن SATGEN_H

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
	SatSolver *بعدی;
	مجازی ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		بعدی = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	مجازی ~SatSolver() {
		خودکار p = &yosys_satsolver_list;
		طالما (*p) {
			إذا (*p == this)
				*p = بعدی;
			وإلا
				p = &(*p)->بعدی;
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
	std::string پیشوند_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_بیت, صحيح>> imported_سیگنالها;
	std::map<std::pair<std::string, صحيح>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ثابت SigMap *sigmap, std::string پیشوند_2 = std::string()) :
			ez(ez), sigmap(sigmap), پیشوند_2(پیشوند_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	فراغ مجموعه_Context(ثابت SigMap *sigmap, std::string پیشوند_2 = std::string())
	{
		this->sigmap = sigmap;
		this->پیشوند_2 = پیشوند_2;
	}

	std::وکتور<صحيح> import_Sig_Spec_کارگر(RTLIL::SigSpec sig, std::string &pf, bool undef_نما, bool dup_undef)
	{
		log_assert(!undef_نما || model_undef);
		sigmap->apply(sig);

		std::وکتور<صحيح> vec;
		vec.reserve(Get_اندازه(sig));

		لكل (خودکار &بیت : sig)
			إذا (بیت.wire == عدم) {
				إذا (model_undef && dup_undef && بیت == RTLIL::State::Sx)
					vec.افزودن_پشت(ez->frozen_literal());
				وإلا
					vec.افزودن_پشت(بیت == (undef_نما ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} وإلا {
				std::string wire_name = بیت.wire->name.unescape();
				std::string name = pf +
					(بیت.wire->عرض == 1 ? wire_name : stringf("%s [%d]", wire_name, بیت.جابه_جایی));
				vec.افزودن_پشت(ez->frozen_literal(name));
				imported_سیگنالها[pf][بیت] = vec.پشت();
			}
		أرجع vec;
	}

	std::وکتور<صحيح> importSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارگر(sig, pf, false, false);
	}

	std::وکتور<صحيح> importDefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارگر(sig, pf, false, true);
	}

	std::وکتور<صحيح> importUndefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارگر(sig, pf, true, false);
	}

	صحيح import_Sig_بیت(RTLIL::Sig_بیت بیت, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارگر(بیت, pf, false, false).front();
	}

	صحيح import_Def_Sig_بیت(RTLIL::Sig_بیت بیت, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارگر(بیت, pf, false, true).front();
	}

	صحيح import_Undef_Sig_بیت(RTLIL::Sig_بیت بیت, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع import_Sig_Spec_کارگر(بیت, pf, true, false).front();
	}

	bool imported_Sig_بیت(RTLIL::Sig_بیت بیت, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		أرجع imported_سیگنالها[pf].شمار(بیت) != 0;
	}

	فراغ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	فراغ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	صحيح importAsserts(صحيح timestep = -1)
	{
		std::وکتور<صحيح> check_بیتها, enable_بیتها;
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بیتها = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_بیتها = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} وإلا {
			check_بیتها = importDefSigSpec(asserts_a[pf], timestep);
			enable_بیتها = importDefSigSpec(asserts_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بیتها, ez->vec_not(enable_بیتها)));
	}

	صحيح importAssumes(صحيح timestep = -1)
	{
		std::وکتور<صحيح> check_بیتها, enable_بیتها;
		std::string pf = پیشوند_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		إذا (model_undef) {
			check_بیتها = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_بیتها = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} وإلا {
			check_بیتها = importDefSigSpec(assumes_a[pf], timestep);
			enable_بیتها = importDefSigSpec(assumes_en[pf], timestep);
		}
		أرجع ez->vec_reduce_and(ez->vec_or(check_بیتها, ez->vec_not(enable_بیتها)));
	}

	صحيح سیگنالها_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, صحيح timestep_lhs = -1, صحيح timestep_rhs = -1)
	{
		إذا (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.اندازه() == rhs.اندازه());

		std::وکتور<صحيح> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::وکتور<صحيح> vec_rhs = importSigSpec(rhs, timestep_rhs);

		إذا (!model_undef)
			أرجع ez->vec_eq(vec_lhs, vec_rhs);

		std::وکتور<صحيح> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::وکتور<صحيح> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::وکتور<صحيح> eq_بیتها;
		لكل (صحيح i = 0; i < lhs.اندازه(); i++)
			eq_بیتها.افزودن_پشت(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		أرجع ez->عبارت_2(ezSAT::OpAnd, eq_بیتها);
	}

	فراغ extend_Signal_عرض(std::وکتور<صحيح> &vec_a, std::وکتور<صحيح> &vec_b, RTLIL::Cell *cell, اندازه_t y_عرض = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		إذا (!forced_signed && cell->parameters.شمار(ID::A_SIGNED) > 0 && cell->parameters.شمار(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		طالما (vec_a.اندازه() < vec_b.اندازه() || vec_a.اندازه() < y_عرض)
			vec_a.افزودن_پشت(is_signed && vec_a.اندازه() > 0 ? vec_a.پشت() : ez->CONST_FALSE);
		طالما (vec_b.اندازه() < vec_a.اندازه() || vec_b.اندازه() < y_عرض)
			vec_b.افزودن_پشت(is_signed && vec_b.اندازه() > 0 ? vec_b.پشت() : ez->CONST_FALSE);
	}

	فراغ extend_Signal_عرض(std::وکتور<صحيح> &vec_a, std::وکتور<صحيح> &vec_b, std::وکتور<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_عرض(vec_a, vec_b, cell, vec_y.اندازه(), forced_signed);
		طالما (vec_y.اندازه() < vec_a.اندازه())
			vec_y.افزودن_پشت(ez->literal());
	}

	فراغ extend_Signal_عرض_Unary(std::وکتور<صحيح> &vec_a, std::وکتور<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.شمار(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		طالما (vec_a.اندازه() < vec_y.اندازه())
			vec_a.افزودن_پشت(is_signed && vec_a.اندازه() > 0 ? vec_a.پشت() : ez->CONST_FALSE);
		طالما (vec_y.اندازه() < vec_a.اندازه())
			vec_y.افزودن_پشت(ez->literal());
	}

	فراغ undefGating(std::وکتور<صحيح> &vec_y, std::وکتور<صحيح> &vec_yy, std::وکتور<صحيح> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.اندازه() == vec_yy.اندازه());
		إذا (vec_y.اندازه() > vec_undef.اندازه()) {
			std::وکتور<صحيح> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.اندازه());
			std::وکتور<صحيح> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.اندازه());
			ez->assume(ez->عبارت_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} وإلا {
			log_assert(vec_y.اندازه() == vec_undef.اندازه());
			ez->assume(ez->عبارت_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::وکتور<صحيح>, std::وکتور<صحيح>> mux(صحيح s, صحيح undef_s, ثابت std::وکتور<صحيح> &a, ثابت std::وکتور<صحيح> &undef_a, ثابت std::وکتور<صحيح> &b, ثابت std::وکتور<صحيح> &undef_b) {
		std::وکتور<صحيح> res;
		std::وکتور<صحيح> undef_res;
		res = ez->vec_ite(s, b, a);
		إذا (model_undef) {
			std::وکتور<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::وکتور<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		أرجع std::make_pair(res, undef_res);
	}

	فراغ undefGating(صحيح y, صحيح yy, صحيح undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	فراغ مجموعه_Init_State(صحيح timestep)
	{
		خودکار کلید = make_pair(پیشوند_2, timestep);
		log_assert(initstates.شمار(کلید) == 0 || initstates.at(کلید) == true);
		initstates[کلید] = true;
	}

	bool importCell(RTLIL::Cell *cell, صحيح timestep = -1);
};

فراغ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_انتها

#پایان_اگر
