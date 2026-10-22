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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SATGEN_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SATGEN_H

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/rtlil.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/sigtools.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/celltypes.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/macc.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
ਬਾਹਰੀ ਬਣਤਰ SatSolver *yosys_satsolver_list;
ਬਾਹਰੀ ਬਣਤਰ SatSolver *yosys_satsolver;

ਬਣਤਰ SatSolver
{
	string name;
	SatSolver *ਅਗਲਾ;
	ਆਭਾਸੀ ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		ਅਗਲਾ = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	ਆਭਾਸੀ ~SatSolver() {
		ਸਵੈਚਾਲਿਤ p = &yosys_satsolver_list;
		ਜਦੋਂ (*p) {
			ਜੇ (*p == this)
				*p = ਅਗਲਾ;
			ਨਹੀਂਤਾਂ
				p = &(*p)->ਅਗਲਾ;
		}
		ਜੇ (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

ਬਣਤਰ ezSatPtr : ਜਨਤਕ std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

ਬਣਤਰ SatGen
{
	ezSAT *ez;
	ਅਚਲ SigMap *sigmap;
	std::string ਅਗੇਤਰ_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ>> imported_ਸੰਕੇਤ;
	std::map<std::pair<std::string, ਪੂਰਨ>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ਅਚਲ SigMap *sigmap, std::string ਅਗੇਤਰ_2 = std::string()) :
			ez(ez), sigmap(sigmap), ਅਗੇਤਰ_2(ਅਗੇਤਰ_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	ਖਾਲੀ ਸੈੱਟ_Context(ਅਚਲ SigMap *sigmap, std::string ਅਗੇਤਰ_2 = std::string())
	{
		this->sigmap = sigmap;
		this->ਅਗੇਤਰ_2 = ਅਗੇਤਰ_2;
	}

	std::ਸਦਿਸ਼<ਪੂਰਨ> import_Sig_Spec_ਕਾਮਾ(RTLIL::SigSpec sig, std::string &pf, bool undef_ਬਹੁਲਕ, bool dup_undef)
	{
		log_assert(!undef_ਬਹੁਲਕ || model_undef);
		sigmap->apply(sig);

		std::ਸਦਿਸ਼<ਪੂਰਨ> vec;
		vec.reserve(Get_ਮਾਪ(sig));

		ਲਈ (ਸਵੈਚਾਲਿਤ &ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire == ਨਿਰਾ) {
				ਜੇ (model_undef && dup_undef && ਦੋਆਧਾਰੀ_ਅੰਕ == RTLIL::State::Sx)
					vec.ਧੱਕੋ_ਪਿੱਠ(ez->frozen_literal());
				ਨਹੀਂਤਾਂ
					vec.ਧੱਕੋ_ਪਿੱਠ(ਦੋਆਧਾਰੀ_ਅੰਕ == (undef_ਬਹੁਲਕ ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} ਨਹੀਂਤਾਂ {
				std::string wire_name = ਦੋਆਧਾਰੀ_ਅੰਕ.wire->name.unescape();
				std::string name = pf +
					(ਦੋਆਧਾਰੀ_ਅੰਕ.wire->ਚੌੜਾਈ == 1 ? wire_name : stringf("%s [%d]", wire_name, ਦੋਆਧਾਰੀ_ਅੰਕ.ਵਿੱਥ));
				vec.ਧੱਕੋ_ਪਿੱਠ(ez->frozen_literal(name));
				imported_ਸੰਕੇਤ[pf][ਦੋਆਧਾਰੀ_ਅੰਕ] = vec.ਪਿੱਠ();
			}
		ਵਾਪਸ vec;
	}

	std::ਸਦਿਸ਼<ਪੂਰਨ> importSigSpec(RTLIL::SigSpec sig, ਪੂਰਨ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਵਾਪਸ import_Sig_Spec_ਕਾਮਾ(sig, pf, false, false);
	}

	std::ਸਦਿਸ਼<ਪੂਰਨ> importDefSigSpec(RTLIL::SigSpec sig, ਪੂਰਨ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਵਾਪਸ import_Sig_Spec_ਕਾਮਾ(sig, pf, false, true);
	}

	std::ਸਦਿਸ਼<ਪੂਰਨ> importUndefSigSpec(RTLIL::SigSpec sig, ਪੂਰਨ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਵਾਪਸ import_Sig_Spec_ਕਾਮਾ(sig, pf, true, false);
	}

	ਪੂਰਨ import_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਵਾਪਸ import_Sig_Spec_ਕਾਮਾ(ਦੋਆਧਾਰੀ_ਅੰਕ, pf, false, false).front();
	}

	ਪੂਰਨ import_Def_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਵਾਪਸ import_Sig_Spec_ਕਾਮਾ(ਦੋਆਧਾਰੀ_ਅੰਕ, pf, false, true).front();
	}

	ਪੂਰਨ import_Undef_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਵਾਪਸ import_Sig_Spec_ਕਾਮਾ(ਦੋਆਧਾਰੀ_ਅੰਕ, pf, true, false).front();
	}

	bool imported_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(RTLIL::Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ, ਪੂਰਨ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਵਾਪਸ imported_ਸੰਕੇਤ[pf].ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ) != 0;
	}

	ਖਾਲੀ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ਪੂਰਨ timestep = -1)
	{
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	ਖਾਲੀ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ਪੂਰਨ timestep = -1)
	{
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	ਪੂਰਨ importAsserts(ਪੂਰਨ timestep = -1)
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> check_ਬਿੱਟ, enable_ਬਿੱਟ;
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਜੇ (model_undef) {
			check_ਬਿੱਟ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_ਬਿੱਟ = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} ਨਹੀਂਤਾਂ {
			check_ਬਿੱਟ = importDefSigSpec(asserts_a[pf], timestep);
			enable_ਬਿੱਟ = importDefSigSpec(asserts_en[pf], timestep);
		}
		ਵਾਪਸ ez->vec_reduce_and(ez->vec_or(check_ਬਿੱਟ, ez->vec_not(enable_ਬਿੱਟ)));
	}

	ਪੂਰਨ importAssumes(ਪੂਰਨ timestep = -1)
	{
		std::ਸਦਿਸ਼<ਪੂਰਨ> check_ਬਿੱਟ, enable_ਬਿੱਟ;
		std::string pf = ਅਗੇਤਰ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ਜੇ (model_undef) {
			check_ਬਿੱਟ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_ਬਿੱਟ = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} ਨਹੀਂਤਾਂ {
			check_ਬਿੱਟ = importDefSigSpec(assumes_a[pf], timestep);
			enable_ਬਿੱਟ = importDefSigSpec(assumes_en[pf], timestep);
		}
		ਵਾਪਸ ez->vec_reduce_and(ez->vec_or(check_ਬਿੱਟ, ez->vec_not(enable_ਬਿੱਟ)));
	}

	ਪੂਰਨ ਸੰਕੇਤ_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, ਪੂਰਨ timestep_lhs = -1, ਪੂਰਨ timestep_rhs = -1)
	{
		ਜੇ (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.ਮਾਪ() == rhs.ਮਾਪ());

		std::ਸਦਿਸ਼<ਪੂਰਨ> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::ਸਦਿਸ਼<ਪੂਰਨ> vec_rhs = importSigSpec(rhs, timestep_rhs);

		ਜੇ (!model_undef)
			ਵਾਪਸ ez->vec_eq(vec_lhs, vec_rhs);

		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::ਸਦਿਸ਼<ਪੂਰਨ> eq_ਬਿੱਟ;
		ਲਈ (ਪੂਰਨ i = 0; i < lhs.ਮਾਪ(); i++)
			eq_ਬਿੱਟ.ਧੱਕੋ_ਪਿੱਠ(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ਵਾਪਸ ez->ਵਿਅੰਜਕ_2(ezSAT::OpAnd, eq_ਬਿੱਟ);
	}

	ਖਾਲੀ extend_Signal_ਚੌੜਾਈ(std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_a, std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_b, RTLIL::Cell *cell, ਮਾਪ_t y_ਚੌੜਾਈ = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		ਜੇ (!forced_signed && cell->parameters.ਗਿਣਤੀ_2(ID::A_SIGNED) > 0 && cell->parameters.ਗਿਣਤੀ_2(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		ਜਦੋਂ (vec_a.ਮਾਪ() < vec_b.ਮਾਪ() || vec_a.ਮਾਪ() < y_ਚੌੜਾਈ)
			vec_a.ਧੱਕੋ_ਪਿੱਠ(is_signed && vec_a.ਮਾਪ() > 0 ? vec_a.ਪਿੱਠ() : ez->CONST_FALSE);
		ਜਦੋਂ (vec_b.ਮਾਪ() < vec_a.ਮਾਪ() || vec_b.ਮਾਪ() < y_ਚੌੜਾਈ)
			vec_b.ਧੱਕੋ_ਪਿੱਠ(is_signed && vec_b.ਮਾਪ() > 0 ? vec_b.ਪਿੱਠ() : ez->CONST_FALSE);
	}

	ਖਾਲੀ extend_Signal_ਚੌੜਾਈ(std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_a, std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_b, std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_ਚੌੜਾਈ(vec_a, vec_b, cell, vec_y.ਮਾਪ(), forced_signed);
		ਜਦੋਂ (vec_y.ਮਾਪ() < vec_a.ਮਾਪ())
			vec_y.ਧੱਕੋ_ਪਿੱਠ(ez->literal());
	}

	ਖਾਲੀ extend_Signal_ਚੌੜਾਈ_Unary(std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_a, std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.ਗਿਣਤੀ_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		ਜਦੋਂ (vec_a.ਮਾਪ() < vec_y.ਮਾਪ())
			vec_a.ਧੱਕੋ_ਪਿੱਠ(is_signed && vec_a.ਮਾਪ() > 0 ? vec_a.ਪਿੱਠ() : ez->CONST_FALSE);
		ਜਦੋਂ (vec_y.ਮਾਪ() < vec_a.ਮਾਪ())
			vec_y.ਧੱਕੋ_ਪਿੱਠ(ez->literal());
	}

	ਖਾਲੀ undefGating(std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_y, std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_yy, std::ਸਦਿਸ਼<ਪੂਰਨ> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.ਮਾਪ() == vec_yy.ਮਾਪ());
		ਜੇ (vec_y.ਮਾਪ() > vec_undef.ਮਾਪ()) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.ਮਾਪ());
			std::ਸਦਿਸ਼<ਪੂਰਨ> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.ਮਾਪ());
			ez->assume(ez->ਵਿਅੰਜਕ_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} ਨਹੀਂਤਾਂ {
			log_assert(vec_y.ਮਾਪ() == vec_undef.ਮਾਪ());
			ez->assume(ez->ਵਿਅੰਜਕ_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::ਸਦਿਸ਼<ਪੂਰਨ>, std::ਸਦਿਸ਼<ਪੂਰਨ>> mux(ਪੂਰਨ s, ਪੂਰਨ undef_s, ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &a, ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &undef_a, ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &b, ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &undef_b) {
		std::ਸਦਿਸ਼<ਪੂਰਨ> res;
		std::ਸਦਿਸ਼<ਪੂਰਨ> undef_res;
		res = ez->vec_ite(s, b, a);
		ਜੇ (model_undef) {
			std::ਸਦਿਸ਼<ਪੂਰਨ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ਸਦਿਸ਼<ਪੂਰਨ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ਵਾਪਸ std::make_pair(res, undef_res);
	}

	ਖਾਲੀ undefGating(ਪੂਰਨ y, ਪੂਰਨ yy, ਪੂਰਨ undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	ਖਾਲੀ ਸੈੱਟ_Init_State(ਪੂਰਨ timestep)
	{
		ਸਵੈਚਾਲਿਤ ਕੁੰਜੀ = make_pair(ਅਗੇਤਰ_2, timestep);
		log_assert(initstates.ਗਿਣਤੀ_2(ਕੁੰਜੀ) == 0 || initstates.at(ਕੁੰਜੀ) == true);
		initstates[ਕੁੰਜੀ] = true;
	}

	bool importCell(RTLIL::Cell *cell, ਪੂਰਨ timestep = -1);
};

ਖਾਲੀ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_ਸਿਰਾ

#ਜੇ_ਅੰਤ
