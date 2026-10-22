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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SATGEN_H
#ವ್ಯಾಖ್ಯಾನಿಸು SATGEN_H

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"
#ಅನ್ತರ್ಭಾವ "kernel/macc.h"

#ಅನ್ತರ್ಭಾವ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
ಬಾಹ್ಯ ರಚನೆ SatSolver *yosys_satsolver_list;
ಬಾಹ್ಯ ರಚನೆ SatSolver *yosys_satsolver;

ರಚನೆ SatSolver
{
	string name;
	SatSolver *ಮುಂದಿನ;
	ವಾಸ್ತವಿಕ ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		ಮುಂದಿನ = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	ವಾಸ್ತವಿಕ ~SatSolver() {
		ಸ್ವಯಂಚಾಲಿತ p = &yosys_satsolver_list;
		ಆಗ (*p) {
			ಆದರೆ (*p == this)
				*p = ಮುಂದಿನ;
			ಇಲ್ಲದಿದ್ದರೆ
				p = &(*p)->ಮುಂದಿನ;
		}
		ಆದರೆ (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

ರಚನೆ ezSatPtr : ಸಾರ್ವಜನಿಕ std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

ರಚನೆ SatGen
{
	ezSAT *ez;
	ಸ್ಥಿರಾಂಕ SigMap *sigmap;
	std::string ಪೂರ್ವಪ್ರತ್ಯಯ_2;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::Sig_ಬಿಟ್, ಪೂರ್ಣಾಂಕ>> imported_ಸಂಕೇತಗಳು;
	std::map<std::pair<std::string, ಪೂರ್ಣಾಂಕ>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ಸ್ಥಿರಾಂಕ SigMap *sigmap, std::string ಪೂರ್ವಪ್ರತ್ಯಯ_2 = std::string()) :
			ez(ez), sigmap(sigmap), ಪೂರ್ವಪ್ರತ್ಯಯ_2(ಪೂರ್ವಪ್ರತ್ಯಯ_2), ignore_div_by_zero(false), model_undef(false)
	{
	}

	ಶೂನ್ಯ ಗಣ_Context(ಸ್ಥಿರಾಂಕ SigMap *sigmap, std::string ಪೂರ್ವಪ್ರತ್ಯಯ_2 = std::string())
	{
		this->sigmap = sigmap;
		this->ಪೂರ್ವಪ್ರತ್ಯಯ_2 = ಪೂರ್ವಪ್ರತ್ಯಯ_2;
	}

	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> import_Sig_Spec_ಕೆಲಸಗಾರ(RTLIL::SigSpec sig, std::string &pf, bool undef_ಬಹುಲಕ, bool dup_undef)
	{
		log_assert(!undef_ಬಹುಲಕ || model_undef);
		sigmap->apply(sig);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> vec;
		vec.reserve(Get_ಗಾತ್ರ(sig));

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &ಬಿಟ್ : sig)
			ಆದರೆ (ಬಿಟ್.wire == ಖಾಲಿ) {
				ಆದರೆ (model_undef && dup_undef && ಬಿಟ್ == RTLIL::State::Sx)
					vec.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->frozen_literal());
				ಇಲ್ಲದಿದ್ದರೆ
					vec.ತಳ್ಳು_ಹಿಂಭಾಗ(ಬಿಟ್ == (undef_ಬಹುಲಕ ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} ಇಲ್ಲದಿದ್ದರೆ {
				std::string wire_name = ಬಿಟ್.wire->name.unescape();
				std::string name = pf +
					(ಬಿಟ್.wire->ಅಗಲ == 1 ? wire_name : stringf("%s [%d]", wire_name, ಬಿಟ್.ಸ್ಥಾನಾಂತರ));
				vec.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->frozen_literal(name));
				imported_ಸಂಕೇತಗಳು[pf][ಬಿಟ್] = vec.ಹಿಂಭಾಗ();
			}
		ಹಿಂತಿರುಗಿ vec;
	}

	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> importSigSpec(RTLIL::SigSpec sig, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಹಿಂತಿರುಗಿ import_Sig_Spec_ಕೆಲಸಗಾರ(sig, pf, false, false);
	}

	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> importDefSigSpec(RTLIL::SigSpec sig, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಹಿಂತಿರುಗಿ import_Sig_Spec_ಕೆಲಸಗಾರ(sig, pf, false, true);
	}

	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> importUndefSigSpec(RTLIL::SigSpec sig, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಹಿಂತಿರುಗಿ import_Sig_Spec_ಕೆಲಸಗಾರ(sig, pf, true, false);
	}

	ಪೂರ್ಣಾಂಕ import_Sig_ಬಿಟ್(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಹಿಂತಿರುಗಿ import_Sig_Spec_ಕೆಲಸಗಾರ(ಬಿಟ್, pf, false, false).front();
	}

	ಪೂರ್ಣಾಂಕ import_Def_Sig_ಬಿಟ್(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಹಿಂತಿರುಗಿ import_Sig_Spec_ಕೆಲಸಗಾರ(ಬಿಟ್, pf, false, true).front();
	}

	ಪೂರ್ಣಾಂಕ import_Undef_Sig_ಬಿಟ್(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಹಿಂತಿರುಗಿ import_Sig_Spec_ಕೆಲಸಗಾರ(ಬಿಟ್, pf, true, false).front();
	}

	bool imported_Sig_ಬಿಟ್(RTLIL::Sig_ಬಿಟ್ ಬಿಟ್, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಹಿಂತಿರುಗಿ imported_ಸಂಕೇತಗಳು[pf].ಎಣಿಕೆ_2(ಬಿಟ್) != 0;
	}

	ಶೂನ್ಯ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	ಶೂನ್ಯ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	ಪೂರ್ಣಾಂಕ importAsserts(ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> check_ಬಿಟ್ಟುಗಳು, enable_ಬಿಟ್ಟುಗಳು;
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಆದರೆ (model_undef) {
			check_ಬಿಟ್ಟುಗಳು = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_ಬಿಟ್ಟುಗಳು = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} ಇಲ್ಲದಿದ್ದರೆ {
			check_ಬಿಟ್ಟುಗಳು = importDefSigSpec(asserts_a[pf], timestep);
			enable_ಬಿಟ್ಟುಗಳು = importDefSigSpec(asserts_en[pf], timestep);
		}
		ಹಿಂತಿರುಗಿ ez->vec_reduce_and(ez->vec_or(check_ಬಿಟ್ಟುಗಳು, ez->vec_not(enable_ಬಿಟ್ಟುಗಳು)));
	}

	ಪೂರ್ಣಾಂಕ importAssumes(ಪೂರ್ಣಾಂಕ timestep = -1)
	{
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> check_ಬಿಟ್ಟುಗಳು, enable_ಬಿಟ್ಟುಗಳು;
		std::string pf = ಪೂರ್ವಪ್ರತ್ಯಯ_2 + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಆದರೆ (model_undef) {
			check_ಬಿಟ್ಟುಗಳು = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_ಬಿಟ್ಟುಗಳು = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} ಇಲ್ಲದಿದ್ದರೆ {
			check_ಬಿಟ್ಟುಗಳು = importDefSigSpec(assumes_a[pf], timestep);
			enable_ಬಿಟ್ಟುಗಳು = importDefSigSpec(assumes_en[pf], timestep);
		}
		ಹಿಂತಿರುಗಿ ez->vec_reduce_and(ez->vec_or(check_ಬಿಟ್ಟುಗಳು, ez->vec_not(enable_ಬಿಟ್ಟುಗಳು)));
	}

	ಪೂರ್ಣಾಂಕ ಸಂಕೇತಗಳು_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, ಪೂರ್ಣಾಂಕ timestep_lhs = -1, ಪೂರ್ಣಾಂಕ timestep_rhs = -1)
	{
		ಆದರೆ (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.ಗಾತ್ರ_2() == rhs.ಗಾತ್ರ_2());

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> vec_rhs = importSigSpec(rhs, timestep_rhs);

		ಆದರೆ (!model_undef)
			ಹಿಂತಿರುಗಿ ez->vec_eq(vec_lhs, vec_rhs);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> eq_ಬಿಟ್ಟುಗಳು;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < lhs.ಗಾತ್ರ_2(); i++)
			eq_ಬಿಟ್ಟುಗಳು.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ಹಿಂತಿರುಗಿ ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpAnd, eq_ಬಿಟ್ಟುಗಳು);
	}

	ಶೂನ್ಯ extend_Signal_ಅಗಲ(std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_a, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_b, RTLIL::Cell *cell, ಗಾತ್ರ_t y_ಅಗಲ = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		ಆದರೆ (!forced_signed && cell->parameters.ಎಣಿಕೆ_2(ID::A_SIGNED) > 0 && cell->parameters.ಎಣಿಕೆ_2(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		ಆಗ (vec_a.ಗಾತ್ರ_2() < vec_b.ಗಾತ್ರ_2() || vec_a.ಗಾತ್ರ_2() < y_ಅಗಲ)
			vec_a.ತಳ್ಳು_ಹಿಂಭಾಗ(is_signed && vec_a.ಗಾತ್ರ_2() > 0 ? vec_a.ಹಿಂಭಾಗ() : ez->CONST_FALSE);
		ಆಗ (vec_b.ಗಾತ್ರ_2() < vec_a.ಗಾತ್ರ_2() || vec_b.ಗಾತ್ರ_2() < y_ಅಗಲ)
			vec_b.ತಳ್ಳು_ಹಿಂಭಾಗ(is_signed && vec_b.ಗಾತ್ರ_2() > 0 ? vec_b.ಹಿಂಭಾಗ() : ez->CONST_FALSE);
	}

	ಶೂನ್ಯ extend_Signal_ಅಗಲ(std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_a, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_b, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extend_Signal_ಅಗಲ(vec_a, vec_b, cell, vec_y.ಗಾತ್ರ_2(), forced_signed);
		ಆಗ (vec_y.ಗಾತ್ರ_2() < vec_a.ಗಾತ್ರ_2())
			vec_y.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->literal());
	}

	ಶೂನ್ಯ extend_Signal_ಅಗಲ_Unary(std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_a, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.ಎಣಿಕೆ_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		ಆಗ (vec_a.ಗಾತ್ರ_2() < vec_y.ಗಾತ್ರ_2())
			vec_a.ತಳ್ಳು_ಹಿಂಭಾಗ(is_signed && vec_a.ಗಾತ್ರ_2() > 0 ? vec_a.ಹಿಂಭಾಗ() : ez->CONST_FALSE);
		ಆಗ (vec_y.ಗಾತ್ರ_2() < vec_a.ಗಾತ್ರ_2())
			vec_y.ತಳ್ಳು_ಹಿಂಭಾಗ(ez->literal());
	}

	ಶೂನ್ಯ undefGating(std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_y, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_yy, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.ಗಾತ್ರ_2() == vec_yy.ಗಾತ್ರ_2());
		ಆದರೆ (vec_y.ಗಾತ್ರ_2() > vec_undef.ಗಾತ್ರ_2()) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.ಗಾತ್ರ_2());
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.ಗಾತ್ರ_2());
			ez->assume(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} ಇಲ್ಲದಿದ್ದರೆ {
			log_assert(vec_y.ಗಾತ್ರ_2() == vec_undef.ಗಾತ್ರ_2());
			ez->assume(ez->ಅಭಿವ್ಯಕ್ತಿ_2(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>, std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ>> mux(ಪೂರ್ಣಾಂಕ s, ಪೂರ್ಣಾಂಕ undef_s, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &a, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &undef_a, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &b, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &undef_b) {
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> res;
		std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_res;
		res = ez->vec_ite(s, b, a);
		ಆದರೆ (model_undef) {
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ಹಿಂತಿರುಗಿ std::make_pair(res, undef_res);
	}

	ಶೂನ್ಯ undefGating(ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ yy, ಪೂರ್ಣಾಂಕ undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	ಶೂನ್ಯ ಗಣ_Init_State(ಪೂರ್ಣಾಂಕ timestep)
	{
		ಸ್ವಯಂಚಾಲಿತ ಕೀಲಿ = make_pair(ಪೂರ್ವಪ್ರತ್ಯಯ_2, timestep);
		log_assert(initstates.ಎಣಿಕೆ_2(ಕೀಲಿ) == 0 || initstates.at(ಕೀಲಿ) == true);
		initstates[ಕೀಲಿ] = true;
	}

	bool importCell(RTLIL::Cell *cell, ಪೂರ್ಣಾಂಕ timestep = -1);
};

ಶೂನ್ಯ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
