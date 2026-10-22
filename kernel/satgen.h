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

#include "kernel/rtlil.h"
#include "kernel/sigtools.h"
#include "kernel/celltypes.h"
#include "kernel/macc.h"

#include "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
extern sa_nracanaa SatSolver *yosys_satsolver_list;
extern sa_nracanaa SatSolver *yosys_satsolver;

sa_nracanaa SatSolver
{
	string name;
	SatSolver *next;
	virtual ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	virtual ~SatSolver() {
		auto p = &yosys_satsolver_list;
		སྐབས (*p) {
			གལ་སྲིད (*p == this)
				*p = next;
			དེ་མིན
				p = &(*p)->next;
		}
		གལ་སྲིད (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

sa_nracanaa ezSatPtr : public std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

sa_nracanaa SatGen
{
	ezSAT *ez;
	const SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, ཧྲིལ>> imported_signals;
	std::map<std::pair<std::string, ཧྲིལ>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, const SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	void setContext(const SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<ཧྲིལ> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<ཧྲིལ> vec;
		vec.reserve(GetSize(sig));

		དོན་ལུ (auto &bit : sig)
			གལ་སྲིད (bit.wire == NULL) {
				གལ་སྲིད (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.push_back(ez->frozen_literal());
				དེ་མིན
					vec.push_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} དེ་མིན {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.push_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		ལོག vec;
	}

	std::vector<ཧྲིལ> importSigSpec(RTLIL::SigSpec sig, ཧྲིལ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ལོག importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<ཧྲིལ> importDefSigSpec(RTLIL::SigSpec sig, ཧྲིལ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ལོག importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<ཧྲིལ> importUndefSigSpec(RTLIL::SigSpec sig, ཧྲིལ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ལོག importSigSpecWorker(sig, pf, true, false);
	}

	ཧྲིལ importSigBit(RTLIL::SigBit bit, ཧྲིལ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ལོག importSigSpecWorker(bit, pf, false, false).front();
	}

	ཧྲིལ importDefSigBit(RTLIL::SigBit bit, ཧྲིལ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ལོག importSigSpecWorker(bit, pf, false, true).front();
	}

	ཧྲིལ importUndefSigBit(RTLIL::SigBit bit, ཧྲིལ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ལོག importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, ཧྲིལ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ལོག imported_signals[pf].count(bit) != 0;
	}

	void getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ཧྲིལ timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	void getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ཧྲིལ timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	ཧྲིལ importAsserts(ཧྲིལ timestep = -1)
	{
		std::vector<ཧྲིལ> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		གལ་སྲིད (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} དེ་མིན {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		ལོག ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	ཧྲིལ importAssumes(ཧྲིལ timestep = -1)
	{
		std::vector<ཧྲིལ> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		གལ་སྲིད (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} དེ་མིན {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		ལོག ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	ཧྲིལ signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, ཧྲིལ timestep_lhs = -1, ཧྲིལ timestep_rhs = -1)
	{
		གལ་སྲིད (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<ཧྲིལ> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<ཧྲིལ> vec_rhs = importSigSpec(rhs, timestep_rhs);

		གལ་སྲིད (!model_undef)
			ལོག ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<ཧྲིལ> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<ཧྲིལ> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<ཧྲིལ> eq_bits;
		དོན་ལུ (ཧྲིལ i = 0; i < lhs.size(); i++)
			eq_bits.push_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ལོག ez->expression(ezSAT::OpAnd, eq_bits);
	}

	void extendSignalWidth(std::vector<ཧྲིལ> &vec_a, std::vector<ཧྲིལ> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		གལ་སྲིད (!forced_signed && cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters.count(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		སྐབས (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		སྐབས (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.push_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	void extendSignalWidth(std::vector<ཧྲིལ> &vec_a, std::vector<ཧྲིལ> &vec_b, std::vector<ཧྲིལ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		སྐབས (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	void extendSignalWidthUnary(std::vector<ཧྲིལ> &vec_a, std::vector<ཧྲིལ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		སྐབས (vec_a.size() < vec_y.size())
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		སྐབས (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	void undefGating(std::vector<ཧྲིལ> &vec_y, std::vector<ཧྲིལ> &vec_yy, std::vector<ཧྲིལ> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		གལ་སྲིད (vec_y.size() > vec_undef.size()) {
			std::vector<ཧྲིལ> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<ཧྲིལ> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} དེ་མིན {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<ཧྲིལ>, std::vector<ཧྲིལ>> mux(ཧྲིལ s, ཧྲིལ undef_s, const std::vector<ཧྲིལ> &a, const std::vector<ཧྲིལ> &undef_a, const std::vector<ཧྲིལ> &b, const std::vector<ཧྲིལ> &undef_b) {
		std::vector<ཧྲིལ> res;
		std::vector<ཧྲིལ> undef_res;
		res = ez->vec_ite(s, b, a);
		གལ་སྲིད (model_undef) {
			std::vector<ཧྲིལ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<ཧྲིལ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ལོག std::make_pair(res, undef_res);
	}

	void undefGating(ཧྲིལ y, ཧྲིལ yy, ཧྲིལ undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	void setInitState(ཧྲིལ timestep)
	{
		auto key = make_pair(prefix, timestep);
		log_assert(initstates.count(key) == 0 || initstates.at(key) == true);
		initstates[key] = true;
	}

	bool importCell(RTLIL::Cell *cell, ཧྲིལ timestep = -1);
};

void report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
