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
		ၶၢဝ်း (*p) {
			ပေႃး (*p == this)
				*p = next;
			ပေႃးဢမ်ႇ
				p = &(*p)->next;
		}
		ပေႃး (yosys_satsolver == this)
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
	std::map<std::string, std::map<RTLIL::SigBit, puur_naa_nka>> imported_signals;
	std::map<std::pair<std::string, puur_naa_nka>, bool> initstates;
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

	std::vector<puur_naa_nka> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<puur_naa_nka> vec;
		vec.reserve(GetSize(sig));

		for (auto &bit : sig)
			ပေႃး (bit.wire == NULL) {
				ပေႃး (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.push_back(ez->frozen_literal());
				ပေႃးဢမ်ႇ
					vec.push_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} ပေႃးဢမ်ႇ {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.push_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		ပွၵ်ႈ vec;
	}

	std::vector<puur_naa_nka> importSigSpec(RTLIL::SigSpec sig, puur_naa_nka timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပွၵ်ႈ importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<puur_naa_nka> importDefSigSpec(RTLIL::SigSpec sig, puur_naa_nka timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပွၵ်ႈ importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<puur_naa_nka> importUndefSigSpec(RTLIL::SigSpec sig, puur_naa_nka timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပွၵ်ႈ importSigSpecWorker(sig, pf, true, false);
	}

	puur_naa_nka importSigBit(RTLIL::SigBit bit, puur_naa_nka timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပွၵ်ႈ importSigSpecWorker(bit, pf, false, false).front();
	}

	puur_naa_nka importDefSigBit(RTLIL::SigBit bit, puur_naa_nka timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပွၵ်ႈ importSigSpecWorker(bit, pf, false, true).front();
	}

	puur_naa_nka importUndefSigBit(RTLIL::SigBit bit, puur_naa_nka timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပွၵ်ႈ importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, puur_naa_nka timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပွၵ်ႈ imported_signals[pf].count(bit) != 0;
	}

	void getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, puur_naa_nka timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	void getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, puur_naa_nka timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	puur_naa_nka importAsserts(puur_naa_nka timestep = -1)
	{
		std::vector<puur_naa_nka> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပေႃး (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} ပေႃးဢမ်ႇ {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		ပွၵ်ႈ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	puur_naa_nka importAssumes(puur_naa_nka timestep = -1)
	{
		std::vector<puur_naa_nka> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ပေႃး (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} ပေႃးဢမ်ႇ {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		ပွၵ်ႈ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	puur_naa_nka signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, puur_naa_nka timestep_lhs = -1, puur_naa_nka timestep_rhs = -1)
	{
		ပေႃး (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<puur_naa_nka> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<puur_naa_nka> vec_rhs = importSigSpec(rhs, timestep_rhs);

		ပေႃး (!model_undef)
			ပွၵ်ႈ ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<puur_naa_nka> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<puur_naa_nka> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<puur_naa_nka> eq_bits;
		for (puur_naa_nka i = 0; i < lhs.size(); i++)
			eq_bits.push_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ပွၵ်ႈ ez->expression(ezSAT::OpAnd, eq_bits);
	}

	void extendSignalWidth(std::vector<puur_naa_nka> &vec_a, std::vector<puur_naa_nka> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		ပေႃး (!forced_signed && cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters.count(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		ၶၢဝ်း (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ၶၢဝ်း (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.push_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	void extendSignalWidth(std::vector<puur_naa_nka> &vec_a, std::vector<puur_naa_nka> &vec_b, std::vector<puur_naa_nka> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		ၶၢဝ်း (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	void extendSignalWidthUnary(std::vector<puur_naa_nka> &vec_a, std::vector<puur_naa_nka> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		ၶၢဝ်း (vec_a.size() < vec_y.size())
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ၶၢဝ်း (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	void undefGating(std::vector<puur_naa_nka> &vec_y, std::vector<puur_naa_nka> &vec_yy, std::vector<puur_naa_nka> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		ပေႃး (vec_y.size() > vec_undef.size()) {
			std::vector<puur_naa_nka> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<puur_naa_nka> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} ပေႃးဢမ်ႇ {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<puur_naa_nka>, std::vector<puur_naa_nka>> mux(puur_naa_nka s, puur_naa_nka undef_s, const std::vector<puur_naa_nka> &a, const std::vector<puur_naa_nka> &undef_a, const std::vector<puur_naa_nka> &b, const std::vector<puur_naa_nka> &undef_b) {
		std::vector<puur_naa_nka> res;
		std::vector<puur_naa_nka> undef_res;
		res = ez->vec_ite(s, b, a);
		ပေႃး (model_undef) {
			std::vector<puur_naa_nka> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<puur_naa_nka> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ပွၵ်ႈ std::make_pair(res, undef_res);
	}

	void undefGating(puur_naa_nka y, puur_naa_nka yy, puur_naa_nka undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	void setInitState(puur_naa_nka timestep)
	{
		auto key = make_pair(prefix, timestep);
		log_assert(initstates.count(key) == 0 || initstates.at(key) == true);
		initstates[key] = true;
	}

	bool importCell(RTLIL::Cell *cell, puur_naa_nka timestep = -1);
};

void report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
