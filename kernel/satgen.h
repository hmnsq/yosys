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

#ଅନ୍ତର୍ଭାଵ "kernel/rtlil.h"
#ଅନ୍ତର୍ଭାଵ "kernel/sigtools.h"
#ଅନ୍ତର୍ଭାଵ "kernel/celltypes.h"
#ଅନ୍ତର୍ଭାଵ "kernel/macc.h"

#ଅନ୍ତର୍ଭାଵ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
ବାହ୍ୟ ସଂରଚନା SatSolver *yosys_satsolver_list;
ବାହ୍ୟ ସଂରଚନା SatSolver *yosys_satsolver;

ସଂରଚନା SatSolver
{
	string name;
	SatSolver *next;
	ଆଭାସୀ ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	ଆଭାସୀ ~SatSolver() {
		auto p = &yosys_satsolver_list;
		ଯେତେବେଳେ (*p) {
			ଯଦି (*p == this)
				*p = next;
			ଅନ୍ୟଥା
				p = &(*p)->next;
		}
		ଯଦି (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

ସଂରଚନା ezSatPtr : ସର୍ବସାଧାରଣ std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

ସଂରଚନା SatGen
{
	ezSAT *ez;
	ଅଚଳ SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> imported_signals;
	std::map<std::pair<std::string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ଅଚଳ SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	ଶୂନ୍ୟ setContext(ଅଚଳ SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> vec;
		vec.reserve(GetSize(sig));

		ପାଇଁ (auto &bit : sig)
			ଯଦି (bit.wire == ଖାଲି) {
				ଯଦି (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.ଠେଲ_back(ez->frozen_literal());
				ଅନ୍ୟଥା
					vec.ଠେଲ_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} ଅନ୍ୟଥା {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.ଠେଲ_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		ଫେରନ୍ତୁ vec;
	}

	std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> importSigSpec(RTLIL::SigSpec sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଫେରନ୍ତୁ importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> importDefSigSpec(RTLIL::SigSpec sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଫେରନ୍ତୁ importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> importUndefSigSpec(RTLIL::SigSpec sig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଫେରନ୍ତୁ importSigSpecWorker(sig, pf, true, false);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା importSigBit(RTLIL::SigBit bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଫେରନ୍ତୁ importSigSpecWorker(bit, pf, false, false).front();
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା importDefSigBit(RTLIL::SigBit bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଫେରନ୍ତୁ importSigSpecWorker(bit, pf, false, true).front();
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା importUndefSigBit(RTLIL::SigBit bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଫେରନ୍ତୁ importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଫେରନ୍ତୁ imported_signals[pf].ଗଣନା_2(bit) != 0;
	}

	ଶୂନ୍ୟ getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	ଶୂନ୍ୟ getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା importAsserts(ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଯଦି (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} ଅନ୍ୟଥା {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		ଫେରନ୍ତୁ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା importAssumes(ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1)
	{
		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ଯଦି (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} ଅନ୍ୟଥା {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		ଫେରନ୍ତୁ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep_lhs = -1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep_rhs = -1)
	{
		ଯଦି (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> vec_rhs = importSigSpec(rhs, timestep_rhs);

		ଯଦି (!model_undef)
			ଫେରନ୍ତୁ ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> eq_bits;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < lhs.size(); i++)
			eq_bits.ଠେଲ_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ଫେରନ୍ତୁ ez->expression(ezSAT::OpAnd, eq_bits);
	}

	ଶୂନ୍ୟ extendSignalWidth(std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_a, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		ଯଦି (!forced_signed && cell->parameters.ଗଣନା_2(ID::A_SIGNED) > 0 && cell->parameters.ଗଣନା_2(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		ଯେତେବେଳେ (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.ଠେଲ_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ଯେତେବେଳେ (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.ଠେଲ_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	ଶୂନ୍ୟ extendSignalWidth(std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_a, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_b, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		ଯେତେବେଳେ (vec_y.size() < vec_a.size())
			vec_y.ଠେଲ_back(ez->literal());
	}

	ଶୂନ୍ୟ extendSignalWidthUnary(std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_a, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.ଗଣନା_2(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		ଯେତେବେଳେ (vec_a.size() < vec_y.size())
			vec_a.ଠେଲ_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ଯେତେବେଳେ (vec_y.size() < vec_a.size())
			vec_y.ଠେଲ_back(ez->literal());
	}

	ଶୂନ୍ୟ undefGating(std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_y, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_yy, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		ଯଦି (vec_y.size() > vec_undef.size()) {
			std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} ଅନ୍ୟଥା {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>, std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା>> mux(ପୂର୍ଣ୍ଣସଂଖ୍ୟା s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef_s, ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &a, ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &undef_a, ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &b, ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &undef_b) {
		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> res;
		std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> undef_res;
		res = ez->vec_ite(s, b, a);
		ଯଦି (model_undef) {
			std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ଫେରନ୍ତୁ std::make_pair(res, undef_res);
	}

	ଶୂନ୍ୟ undefGating(ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା yy, ପୂର୍ଣ୍ଣସଂଖ୍ୟା undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	ଶୂନ୍ୟ setInitState(ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep)
	{
		auto ଚାବି = make_pair(prefix, timestep);
		log_assert(initstates.ଗଣନା_2(ଚାବି) == 0 || initstates.at(ଚାବି) == true);
		initstates[ଚାବି] = true;
	}

	bool importCell(RTLIL::Cell *cell, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timestep = -1);
};

ଶୂନ୍ୟ report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
