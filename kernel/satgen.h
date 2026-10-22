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
		අතරතුර (*p) {
			නම් (*p == this)
				*p = next;
			නැතහොත්
				p = &(*p)->next;
		}
		නම් (yosys_satsolver == this)
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
	std::map<std::string, std::map<RTLIL::SigBit, පූර්ණ_සංඛ්‍යා>> imported_signals;
	std::map<std::pair<std::string, පූර්ණ_සංඛ්‍යා>, bool> initstates;
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

	std::vector<පූර්ණ_සංඛ්‍යා> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<පූර්ණ_සංඛ්‍යා> vec;
		vec.reserve(GetSize(sig));

		සඳහා (auto &bit : sig)
			නම් (bit.wire == NULL) {
				නම් (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.push_back(ez->frozen_literal());
				නැතහොත්
					vec.push_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} නැතහොත් {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.push_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		ආපසු vec;
	}

	std::vector<පූර්ණ_සංඛ්‍යා> importSigSpec(RTLIL::SigSpec sig, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ආපසු importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<පූර්ණ_සංඛ්‍යා> importDefSigSpec(RTLIL::SigSpec sig, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ආපසු importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<පූර්ණ_සංඛ්‍යා> importUndefSigSpec(RTLIL::SigSpec sig, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ආපසු importSigSpecWorker(sig, pf, true, false);
	}

	පූර්ණ_සංඛ්‍යා importSigBit(RTLIL::SigBit bit, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ආපසු importSigSpecWorker(bit, pf, false, false).front();
	}

	පූර්ණ_සංඛ්‍යා importDefSigBit(RTLIL::SigBit bit, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ආපසු importSigSpecWorker(bit, pf, false, true).front();
	}

	පූර්ණ_සංඛ්‍යා importUndefSigBit(RTLIL::SigBit bit, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ආපසු importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ආපසු imported_signals[pf].count(bit) != 0;
	}

	void getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	void getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	පූර්ණ_සංඛ්‍යා importAsserts(පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		std::vector<පූර්ණ_සංඛ්‍යා> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		නම් (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} නැතහොත් {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		ආපසු ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	පූර්ණ_සංඛ්‍යා importAssumes(පූර්ණ_සංඛ්‍යා timestep = -1)
	{
		std::vector<පූර්ණ_සංඛ්‍යා> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		නම් (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} නැතහොත් {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		ආපසු ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	පූර්ණ_සංඛ්‍යා signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, පූර්ණ_සංඛ්‍යා timestep_lhs = -1, පූර්ණ_සංඛ්‍යා timestep_rhs = -1)
	{
		නම් (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<පූර්ණ_සංඛ්‍යා> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<පූර්ණ_සංඛ්‍යා> vec_rhs = importSigSpec(rhs, timestep_rhs);

		නම් (!model_undef)
			ආපසු ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<පූර්ණ_සංඛ්‍යා> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<පූර්ණ_සංඛ්‍යා> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<පූර්ණ_සංඛ්‍යා> eq_bits;
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < lhs.size(); i++)
			eq_bits.push_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ආපසු ez->expression(ezSAT::OpAnd, eq_bits);
	}

	void extendSignalWidth(std::vector<පූර්ණ_සංඛ්‍යා> &vec_a, std::vector<පූර්ණ_සංඛ්‍යා> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		නම් (!forced_signed && cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters.count(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		අතරතුර (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		අතරතුර (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.push_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	void extendSignalWidth(std::vector<පූර්ණ_සංඛ්‍යා> &vec_a, std::vector<පූර්ණ_සංඛ්‍යා> &vec_b, std::vector<පූර්ණ_සංඛ්‍යා> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		අතරතුර (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	void extendSignalWidthUnary(std::vector<පූර්ණ_සංඛ්‍යා> &vec_a, std::vector<පූර්ණ_සංඛ්‍යා> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		අතරතුර (vec_a.size() < vec_y.size())
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		අතරතුර (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	void undefGating(std::vector<පූර්ණ_සංඛ්‍යා> &vec_y, std::vector<පූර්ණ_සංඛ්‍යා> &vec_yy, std::vector<පූර්ණ_සංඛ්‍යා> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		නම් (vec_y.size() > vec_undef.size()) {
			std::vector<පූර්ණ_සංඛ්‍යා> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<පූර්ණ_සංඛ්‍යා> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} නැතහොත් {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<පූර්ණ_සංඛ්‍යා>, std::vector<පූර්ණ_සංඛ්‍යා>> mux(පූර්ණ_සංඛ්‍යා s, පූර්ණ_සංඛ්‍යා undef_s, const std::vector<පූර්ණ_සංඛ්‍යා> &a, const std::vector<පූර්ණ_සංඛ්‍යා> &undef_a, const std::vector<පූර්ණ_සංඛ්‍යා> &b, const std::vector<පූර්ණ_සංඛ්‍යා> &undef_b) {
		std::vector<පූර්ණ_සංඛ්‍යා> res;
		std::vector<පූර්ණ_සංඛ්‍යා> undef_res;
		res = ez->vec_ite(s, b, a);
		නම් (model_undef) {
			std::vector<පූර්ණ_සංඛ්‍යා> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<පූර්ණ_සංඛ්‍යා> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ආපසු std::make_pair(res, undef_res);
	}

	void undefGating(පූර්ණ_සංඛ්‍යා y, පූර්ණ_සංඛ්‍යා yy, පූර්ණ_සංඛ්‍යා undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	void setInitState(පූර්ණ_සංඛ්‍යා timestep)
	{
		auto key = make_pair(prefix, timestep);
		log_assert(initstates.count(key) == 0 || initstates.at(key) == true);
		initstates[key] = true;
	}

	bool importCell(RTLIL::Cell *cell, පූර්ණ_සංඛ්‍යා timestep = -1);
};

void report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
