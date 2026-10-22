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

#اگر_غیر_تعریف_شدہ SATGEN_H
#تعریف_کرو SATGEN_H

#شامل "kernel/rtlil.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/celltypes.h"
#شامل "kernel/macc.h"

#شامل "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
خارجی ساخت SatSolver *yosys_satsolver_list;
خارجی ساخت SatSolver *yosys_satsolver;

ساخت SatSolver
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
		جب (*p) {
			اگر (*p == this)
				*p = اگلا;
			ورنہ
				p = &(*p)->اگلا;
		}
		اگر (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

ساخت ezSatPtr : عوامی std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

ساخت SatGen
{
	ezSAT *ez;
	ثابت SigMap *sigmap;
	std::string سابقہ_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_ثنائی_ہندسہ, صحیح>> imported_اشارات;
	std::map<std::pair<std::string, صحیح>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ثابت SigMap *sigmap, std::string سابقہ_2 = std::string()) :
			ez(ez), sigmap(sigmap), سابقہ_2(سابقہ_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	خالی سیٹ_Context(ثابت SigMap *sigmap, std::string سابقہ_2 = std::string())
	{
		this->sigmap = sigmap;
		this->سابقہ_2 = سابقہ_2;
	}

	std::سمتیہ<صحیح> import_Sig_Spec_کارکن(RTLIL::SigSpec sig, std::string &pf, bool undef_منوال, bool dup_undef)
	{
		log_assert(!undef_منوال || model_undef);
		sigmap->apply(sig);

		std::سمتیہ<صحیح> vec;
		vec.reserve(Get_سائز(sig));

		برائے (خودکار &ثنائی_ہندسہ : sig)
			اگر (ثنائی_ہندسہ.wire == تہی) {
				اگر (model_undef && dup_undef && ثنائی_ہندسہ == RTLIL::State::Sx)
					vec.ڈالو_عقب(ez->frozen_literal());
				ورنہ
					vec.ڈالو_عقب(ثنائی_ہندسہ == (undef_منوال ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} ورنہ {
				std::string wire_name = ثنائی_ہندسہ.wire->name.unescape();
				std::string name = pf +
					(ثنائی_ہندسہ.wire->چوڑائی == 1 ? wire_name : stringf("%s [%d]", wire_name, ثنائی_ہندسہ.انحراف));
				vec.ڈالو_عقب(ez->frozen_literal(name));
				imported_اشارات[pf][ثنائی_ہندسہ] = vec.عقب();
			}
		واپس vec;
	}

	std::سمتیہ<صحیح> importSigSpec(RTLIL::SigSpec sig, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		واپس import_Sig_Spec_کارکن(sig, pf, false, false);
	}

	std::سمتیہ<صحیح> importDefSigSpec(RTLIL::SigSpec sig, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		واپس import_Sig_Spec_کارکن(sig, pf, false, true);
	}

	std::سمتیہ<صحیح> importUndefSigSpec(RTLIL::SigSpec sig, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		واپس import_Sig_Spec_کارکن(sig, pf, true, false);
	}

	صحیح import_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		واپس import_Sig_Spec_کارکن(ثنائی_ہندسہ, pf, false, false).front();
	}

	صحیح import_Def_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		واپس import_Sig_Spec_کارکن(ثنائی_ہندسہ, pf, false, true).front();
	}

	صحیح import_Undef_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		واپس import_Sig_Spec_کارکن(ثنائی_ہندسہ, pf, true, false).front();
	}

	bool imported_Sig_ثنائی_ہندسہ(RTLIL::Sig_ثنائی_ہندسہ ثنائی_ہندسہ, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		واپس imported_اشارات[pf].گنتی(ثنائی_ہندسہ) != 0;
	}

	خالی getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحیح timestep = -1)
	{
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	خالی getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحیح timestep = -1)
	{
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	صحیح importAsserts(صحیح timestep = -1)
	{
		std::سمتیہ<صحیح> check_بٹ, enable_بٹ;
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		اگر (model_undef) {
			check_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} ورنہ {
			check_بٹ = importDefSigSpec(asserts_a[pf], timestep);
			enable_بٹ = importDefSigSpec(asserts_en[pf], timestep);
		}
		واپس ez->vec_reduce_and(ez->vec_or(check_بٹ, ez->vec_not(enable_بٹ)));
	}

	صحیح importAssumes(صحیح timestep = -1)
	{
		std::سمتیہ<صحیح> check_بٹ, enable_بٹ;
		std::string pf = سابقہ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		اگر (model_undef) {
			check_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_بٹ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} ورنہ {
			check_بٹ = importDefSigSpec(assumes_a[pf], timestep);
			enable_بٹ = importDefSigSpec(assumes_en[pf], timestep);
		}
		واپس ez->vec_reduce_and(ez->vec_or(check_بٹ, ez->vec_not(enable_بٹ)));
	}

	صحیح اشارات_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, صحیح timestep_lhs = -1, صحیح timestep_rhs = -1)
	{
		اگر (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.سائز() == rhs.سائز());

		std::سمتیہ<صحیح> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::سمتیہ<صحیح> vec_rhs = importSigSpec(rhs, timestep_rhs);

		اگر (!model_undef)
			واپس ez->vec_eq(vec_lhs, vec_rhs);

		std::سمتیہ<صحیح> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::سمتیہ<صحیح> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::سمتیہ<صحیح> eq_بٹ;
		برائے (صحیح i = 0; i < lhs.سائز(); i++)
			eq_بٹ.ڈالو_عقب(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		واپس ez->عبارت(ezSAT::OpAnd, eq_بٹ);
	}

	خالی extend_Signal_چوڑائی(std::سمتیہ<صحیح> &vec_a, std::سمتیہ<صحیح> &vec_b, RTLIL::Cell *cell, سائز_t y_چوڑائی = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		اگر (!forced_signed && cell->parameters.گنتی(ID::A_SIGNED) > 0 && cell->parameters.گنتی(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		جب (vec_a.سائز() < vec_b.سائز() || vec_a.سائز() < y_چوڑائی)
			vec_a.ڈالو_عقب(is_signed && vec_a.سائز() > 0 ? vec_a.عقب() : ez->CONST_FALSE);
		جب (vec_b.سائز() < vec_a.سائز() || vec_b.سائز() < y_چوڑائی)
			vec_b.ڈالو_عقب(is_signed && vec_b.سائز() > 0 ? vec_b.عقب() : ez->CONST_FALSE);
	}

	خالی extend_Signal_چوڑائی(std::سمتیہ<صحیح> &vec_a, std::سمتیہ<صحیح> &vec_b, std::سمتیہ<صحیح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_چوڑائی(vec_a, vec_b, cell, vec_y.سائز(), forced_signed);
		جب (vec_y.سائز() < vec_a.سائز())
			vec_y.ڈالو_عقب(ez->literal());
	}

	خالی extend_Signal_چوڑائی_Unary(std::سمتیہ<صحیح> &vec_a, std::سمتیہ<صحیح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.گنتی(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		جب (vec_a.سائز() < vec_y.سائز())
			vec_a.ڈالو_عقب(is_signed && vec_a.سائز() > 0 ? vec_a.عقب() : ez->CONST_FALSE);
		جب (vec_y.سائز() < vec_a.سائز())
			vec_y.ڈالو_عقب(ez->literal());
	}

	خالی undefGating(std::سمتیہ<صحیح> &vec_y, std::سمتیہ<صحیح> &vec_yy, std::سمتیہ<صحیح> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.سائز() == vec_yy.سائز());
		اگر (vec_y.سائز() > vec_undef.سائز()) {
			std::سمتیہ<صحیح> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.سائز());
			std::سمتیہ<صحیح> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.سائز());
			ez->assume(ez->عبارت(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} ورنہ {
			log_assert(vec_y.سائز() == vec_undef.سائز());
			ez->assume(ez->عبارت(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::سمتیہ<صحیح>, std::سمتیہ<صحیح>> mux(صحیح s, صحیح undef_s, ثابت std::سمتیہ<صحیح> &a, ثابت std::سمتیہ<صحیح> &undef_a, ثابت std::سمتیہ<صحیح> &b, ثابت std::سمتیہ<صحیح> &undef_b) {
		std::سمتیہ<صحیح> res;
		std::سمتیہ<صحیح> undef_res;
		res = ez->vec_ite(s, b, a);
		اگر (model_undef) {
			std::سمتیہ<صحیح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::سمتیہ<صحیح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		واپس std::make_pair(res, undef_res);
	}

	خالی undefGating(صحیح y, صحیح yy, صحیح undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	خالی سیٹ_Init_State(صحیح timestep)
	{
		خودکار کلید = make_pair(سابقہ_2, timestep);
		log_assert(initstates.گنتی(کلید) == 0 || initstates.at(کلید) == true);
		initstates[کلید] = true;
	}

	bool importCell(RTLIL::Cell *cell, صحیح timestep = -1);
};

خالی report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_انتہا

#اگر_ختم
