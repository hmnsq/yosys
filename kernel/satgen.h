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
	SatSolver *next;
	virtual ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	virtual ~SatSolver() {
		auto p = &yosys_satsolver_list;
		ܟܡܐ (*p) {
			ܐܢ (*p == this)
				*p = next;
			ܐܚܪܢܐ
				p = &(*p)->next;
		}
		ܐܢ (yosys_satsolver == this)
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
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, صحيح>> imported_signals;
	std::map<std::pair<std::string, صحيح>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ثابت SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	فراغ setContext(ثابت SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<صحيح> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<صحيح> vec;
		vec.reserve(GetSize(sig));

		لكل (auto &bit : sig)
			ܐܢ (bit.wire == عدم) {
				ܐܢ (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.push_back(ez->frozen_literal());
				ܐܚܪܢܐ
					vec.push_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} ܐܚܪܢܐ {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.push_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		ܦܢܐ vec;
	}

	std::vector<صحيح> importSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܦܢܐ importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<صحيح> importDefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܦܢܐ importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<صحيح> importUndefSigSpec(RTLIL::SigSpec sig, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܦܢܐ importSigSpecWorker(sig, pf, true, false);
	}

	صحيح importSigBit(RTLIL::SigBit bit, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܦܢܐ importSigSpecWorker(bit, pf, false, false).front();
	}

	صحيح importDefSigBit(RTLIL::SigBit bit, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܦܢܐ importSigSpecWorker(bit, pf, false, true).front();
	}

	صحيح importUndefSigBit(RTLIL::SigBit bit, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܦܢܐ importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, صحيح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܦܢܐ imported_signals[pf].count(bit) != 0;
	}

	فراغ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	فراغ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحيح timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	صحيح importAsserts(صحيح timestep = -1)
	{
		std::vector<صحيح> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܐܢ (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} ܐܚܪܢܐ {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		ܦܢܐ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	صحيح importAssumes(صحيح timestep = -1)
	{
		std::vector<صحيح> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ܐܢ (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} ܐܚܪܢܐ {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		ܦܢܐ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	صحيح signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, صحيح timestep_lhs = -1, صحيح timestep_rhs = -1)
	{
		ܐܢ (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<صحيح> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<صحيح> vec_rhs = importSigSpec(rhs, timestep_rhs);

		ܐܢ (!model_undef)
			ܦܢܐ ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<صحيح> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<صحيح> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<صحيح> eq_bits;
		لكل (صحيح i = 0; i < lhs.size(); i++)
			eq_bits.push_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ܦܢܐ ez->expression(ezSAT::OpAnd, eq_bits);
	}

	فراغ extendSignalWidth(std::vector<صحيح> &vec_a, std::vector<صحيح> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		ܐܢ (!forced_signed && cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters.count(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		ܟܡܐ (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ܟܡܐ (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.push_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	فراغ extendSignalWidth(std::vector<صحيح> &vec_a, std::vector<صحيح> &vec_b, std::vector<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		ܟܡܐ (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	فراغ extendSignalWidthUnary(std::vector<صحيح> &vec_a, std::vector<صحيح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.count(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		ܟܡܐ (vec_a.size() < vec_y.size())
			vec_a.push_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ܟܡܐ (vec_y.size() < vec_a.size())
			vec_y.push_back(ez->literal());
	}

	فراغ undefGating(std::vector<صحيح> &vec_y, std::vector<صحيح> &vec_yy, std::vector<صحيح> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		ܐܢ (vec_y.size() > vec_undef.size()) {
			std::vector<صحيح> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<صحيح> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} ܐܚܪܢܐ {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<صحيح>, std::vector<صحيح>> mux(صحيح s, صحيح undef_s, ثابت std::vector<صحيح> &a, ثابت std::vector<صحيح> &undef_a, ثابت std::vector<صحيح> &b, ثابت std::vector<صحيح> &undef_b) {
		std::vector<صحيح> res;
		std::vector<صحيح> undef_res;
		res = ez->vec_ite(s, b, a);
		ܐܢ (model_undef) {
			std::vector<صحيح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<صحيح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ܦܢܐ std::make_pair(res, undef_res);
	}

	فراغ undefGating(صحيح y, صحيح yy, صحيح undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	فراغ setInitState(صحيح timestep)
	{
		auto key = make_pair(prefix, timestep);
		log_assert(initstates.count(key) == 0 || initstates.at(key) == true);
		initstates[key] = true;
	}

	bool importCell(RTLIL::Cell *cell, صحيح timestep = -1);
};

فراغ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
