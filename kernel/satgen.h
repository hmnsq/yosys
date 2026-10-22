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

#ifndef SATGEN_H
#define SATGEN_H

#ಅನ್ತರ್ಭಾವ "kernel/rtlil.h"
#ಅನ್ತರ್ಭಾವ "kernel/sigtools.h"
#ಅನ್ತರ್ಭಾವ "kernel/celltypes.h"
#ಅನ್ತರ್ಭಾವ "kernel/macc.h"

#ಅನ್ತರ್ಭಾವ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
ಬಾಹ್ಯ ಸಂರಚನಾ SatSolver *yosys_satsolver_list;
ಬಾಹ್ಯ ಸಂರಚನಾ SatSolver *yosys_satsolver;

ಸಂರಚನಾ SatSolver
{
	string name;
	SatSolver *next;
	ಆಭಾಸ ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	ಆಭಾಸ ~SatSolver() {
		auto p = &yosys_satsolver_list;
		ಯಾವತ್ (*p) {
			ಯದಿ (*p == this)
				*p = next;
			ಅನ್ಯಥಾ
				p = &(*p)->next;
		}
		ಯದಿ (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

ಸಂರಚನಾ ezSatPtr : ಸಾರ್ವಜನಿಕ std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

ಸಂರಚನಾ SatGen
{
	ezSAT *ez;
	ಧ್ರುವ SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, ಪೂರ್ಣಾಙ್ಕ>> imported_signals;
	std::map<std::pair<std::string, ಪೂರ್ಣಾಙ್ಕ>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ಧ್ರುವ SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	ಶೂನ್ಯ setContext(ಧ್ರುವ SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<ಪೂರ್ಣಾಙ್ಕ> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<ಪೂರ್ಣಾಙ್ಕ> vec;
		vec.reserve(GetSize(sig));

		ಪ್ರತಿ (auto &bit : sig)
			ಯದಿ (bit.wire == ರಿಕ್ತ) {
				ಯದಿ (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.ತಳ್ಳು_back(ez->frozen_literal());
				ಅನ್ಯಥಾ
					vec.ತಳ್ಳು_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} ಅನ್ಯಥಾ {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.ತಳ್ಳು_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		ಪ್ರತ್ಯಾಗಮ vec;
	}

	std::vector<ಪೂರ್ಣಾಙ್ಕ> importSigSpec(RTLIL::SigSpec sig, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಪ್ರತ್ಯಾಗಮ importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<ಪೂರ್ಣಾಙ್ಕ> importDefSigSpec(RTLIL::SigSpec sig, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಪ್ರತ್ಯಾಗಮ importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<ಪೂರ್ಣಾಙ್ಕ> importUndefSigSpec(RTLIL::SigSpec sig, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಪ್ರತ್ಯಾಗಮ importSigSpecWorker(sig, pf, true, false);
	}

	ಪೂರ್ಣಾಙ್ಕ importSigBit(RTLIL::SigBit bit, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಪ್ರತ್ಯಾಗಮ importSigSpecWorker(bit, pf, false, false).front();
	}

	ಪೂರ್ಣಾಙ್ಕ importDefSigBit(RTLIL::SigBit bit, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಪ್ರತ್ಯಾಗಮ importSigSpecWorker(bit, pf, false, true).front();
	}

	ಪೂರ್ಣಾಙ್ಕ importUndefSigBit(RTLIL::SigBit bit, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಪ್ರತ್ಯಾಗಮ importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಪ್ರತ್ಯಾಗಮ imported_signals[pf].ಎಣಿಕೆ(bit) != 0;
	}

	ಶೂನ್ಯ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	ಶೂನ್ಯ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	ಪೂರ್ಣಾಙ್ಕ importAsserts(ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		std::vector<ಪೂರ್ಣಾಙ್ಕ> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಯದಿ (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} ಅನ್ಯಥಾ {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		ಪ್ರತ್ಯಾಗಮ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	ಪೂರ್ಣಾಙ್ಕ importAssumes(ಪೂರ್ಣಾಙ್ಕ timestep = -1)
	{
		std::vector<ಪೂರ್ಣಾಙ್ಕ> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ಯದಿ (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} ಅನ್ಯಥಾ {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		ಪ್ರತ್ಯಾಗಮ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	ಪೂರ್ಣಾಙ್ಕ signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, ಪೂರ್ಣಾಙ್ಕ timestep_lhs = -1, ಪೂರ್ಣಾಙ್ಕ timestep_rhs = -1)
	{
		ಯದಿ (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<ಪೂರ್ಣಾಙ್ಕ> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<ಪೂರ್ಣಾಙ್ಕ> vec_rhs = importSigSpec(rhs, timestep_rhs);

		ಯದಿ (!model_undef)
			ಪ್ರತ್ಯಾಗಮ ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<ಪೂರ್ಣಾಙ್ಕ> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<ಪೂರ್ಣಾಙ್ಕ> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<ಪೂರ್ಣಾಙ್ಕ> eq_bits;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < lhs.size(); i++)
			eq_bits.ತಳ್ಳು_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ಪ್ರತ್ಯಾಗಮ ez->expression(ezSAT::OpAnd, eq_bits);
	}

	ಶೂನ್ಯ extendSignalWidth(std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_a, std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		ಯದಿ (!forced_signed && cell->parameters.ಎಣಿಕೆ(ID::A_SIGNED) > 0 && cell->parameters.ಎಣಿಕೆ(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		ಯಾವತ್ (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.ತಳ್ಳು_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ಯಾವತ್ (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.ತಳ್ಳು_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	ಶೂನ್ಯ extendSignalWidth(std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_a, std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_b, std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		ಯಾವತ್ (vec_y.size() < vec_a.size())
			vec_y.ತಳ್ಳು_back(ez->literal());
	}

	ಶೂನ್ಯ extendSignalWidthUnary(std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_a, std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.ಎಣಿಕೆ(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		ಯಾವತ್ (vec_a.size() < vec_y.size())
			vec_a.ತಳ್ಳು_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ಯಾವತ್ (vec_y.size() < vec_a.size())
			vec_y.ತಳ್ಳು_back(ez->literal());
	}

	ಶೂನ್ಯ undefGating(std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_y, std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_yy, std::vector<ಪೂರ್ಣಾಙ್ಕ> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		ಯದಿ (vec_y.size() > vec_undef.size()) {
			std::vector<ಪೂರ್ಣಾಙ್ಕ> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<ಪೂರ್ಣಾಙ್ಕ> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} ಅನ್ಯಥಾ {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<ಪೂರ್ಣಾಙ್ಕ>, std::vector<ಪೂರ್ಣಾಙ್ಕ>> mux(ಪೂರ್ಣಾಙ್ಕ s, ಪೂರ್ಣಾಙ್ಕ undef_s, ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &a, ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &undef_a, ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &b, ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &undef_b) {
		std::vector<ಪೂರ್ಣಾಙ್ಕ> res;
		std::vector<ಪೂರ್ಣಾಙ್ಕ> undef_res;
		res = ez->vec_ite(s, b, a);
		ಯದಿ (model_undef) {
			std::vector<ಪೂರ್ಣಾಙ್ಕ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<ಪೂರ್ಣಾಙ್ಕ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ಪ್ರತ್ಯಾಗಮ std::make_pair(res, undef_res);
	}

	ಶೂನ್ಯ undefGating(ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ yy, ಪೂರ್ಣಾಙ್ಕ undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	ಶೂನ್ಯ setInitState(ಪೂರ್ಣಾಙ್ಕ timestep)
	{
		auto ಕೀಲಿ = make_pair(prefix, timestep);
		log_assert(initstates.ಎಣಿಕೆ(ಕೀಲಿ) == 0 || initstates.at(ಕೀಲಿ) == true);
		initstates[ಕೀಲಿ] = true;
	}

	bool importCell(RTLIL::Cell *cell, ಪೂರ್ಣಾಙ್ಕ timestep = -1);
};

ಶೂನ್ಯ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
