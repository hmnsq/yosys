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

#অন্তর্ভাব "kernel/rtlil.h"
#অন্তর্ভাব "kernel/sigtools.h"
#অন্তর্ভাব "kernel/celltypes.h"
#অন্তর্ভাব "kernel/macc.h"

#অন্তর্ভাব "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
বাহ্যিক কাঠামো SatSolver *yosys_satsolver_list;
বাহ্যিক কাঠামো SatSolver *yosys_satsolver;

কাঠামো SatSolver
{
	string name;
	SatSolver *পরবর্তী;
	আভাসী ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		পরবর্তী = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	আভাসী ~SatSolver() {
		auto p = &yosys_satsolver_list;
		যখন (*p) {
			যদি (*p == this)
				*p = পরবর্তী;
			অন্যথা
				p = &(*p)->পরবর্তী;
		}
		যদি (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

কাঠামো ezSatPtr : সর্বজনীন std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

কাঠামো SatGen
{
	ezSAT *ez;
	ধ্রুবক SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, পূর্ণসংখ্যা>> imported_signals;
	std::map<std::pair<std::string, পূর্ণসংখ্যা>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ধ্রুবক SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	শূন্য setContext(ধ্রুবক SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<পূর্ণসংখ্যা> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<পূর্ণসংখ্যা> vec;
		vec.reserve(Get_আকার(sig));

		জন্য (auto &bit : sig)
			যদি (bit.wire == ফাঁকা) {
				যদি (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.ঠেলো_back(ez->frozen_literal());
				অন্যথা
					vec.ঠেলো_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} অন্যথা {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.ঠেলো_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		ফেরত vec;
	}

	std::vector<পূর্ণসংখ্যা> importSigSpec(RTLIL::SigSpec sig, পূর্ণসংখ্যা timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ফেরত importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<পূর্ণসংখ্যা> importDefSigSpec(RTLIL::SigSpec sig, পূর্ণসংখ্যা timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ফেরত importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<পূর্ণসংখ্যা> importUndefSigSpec(RTLIL::SigSpec sig, পূর্ণসংখ্যা timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ফেরত importSigSpecWorker(sig, pf, true, false);
	}

	পূর্ণসংখ্যা importSigBit(RTLIL::SigBit bit, পূর্ণসংখ্যা timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ফেরত importSigSpecWorker(bit, pf, false, false).front();
	}

	পূর্ণসংখ্যা importDefSigBit(RTLIL::SigBit bit, পূর্ণসংখ্যা timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ফেরত importSigSpecWorker(bit, pf, false, true).front();
	}

	পূর্ণসংখ্যা importUndefSigBit(RTLIL::SigBit bit, পূর্ণসংখ্যা timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ফেরত importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, পূর্ণসংখ্যা timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		ফেরত imported_signals[pf].গুনতি(bit) != 0;
	}

	শূন্য getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, পূর্ণসংখ্যা timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	শূন্য getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, পূর্ণসংখ্যা timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	পূর্ণসংখ্যা importAsserts(পূর্ণসংখ্যা timestep = -1)
	{
		std::vector<পূর্ণসংখ্যা> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		যদি (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} অন্যথা {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		ফেরত ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	পূর্ণসংখ্যা importAssumes(পূর্ণসংখ্যা timestep = -1)
	{
		std::vector<পূর্ণসংখ্যা> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		যদি (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} অন্যথা {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		ফেরত ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	পূর্ণসংখ্যা signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, পূর্ণসংখ্যা timestep_lhs = -1, পূর্ণসংখ্যা timestep_rhs = -1)
	{
		যদি (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.আকার_2() == rhs.আকার_2());

		std::vector<পূর্ণসংখ্যা> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<পূর্ণসংখ্যা> vec_rhs = importSigSpec(rhs, timestep_rhs);

		যদি (!model_undef)
			ফেরত ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<পূর্ণসংখ্যা> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<পূর্ণসংখ্যা> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<পূর্ণসংখ্যা> eq_bits;
		জন্য (পূর্ণসংখ্যা i = 0; i < lhs.আকার_2(); i++)
			eq_bits.ঠেলো_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		ফেরত ez->expression(ezSAT::OpAnd, eq_bits);
	}

	শূন্য extendSignalWidth(std::vector<পূর্ণসংখ্যা> &vec_a, std::vector<পূর্ণসংখ্যা> &vec_b, RTLIL::Cell *cell, আকার_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		যদি (!forced_signed && cell->parameters.গুনতি(ID::A_SIGNED) > 0 && cell->parameters.গুনতি(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		যখন (vec_a.আকার_2() < vec_b.আকার_2() || vec_a.আকার_2() < y_width)
			vec_a.ঠেলো_back(is_signed && vec_a.আকার_2() > 0 ? vec_a.back() : ez->CONST_FALSE);
		যখন (vec_b.আকার_2() < vec_a.আকার_2() || vec_b.আকার_2() < y_width)
			vec_b.ঠেলো_back(is_signed && vec_b.আকার_2() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	শূন্য extendSignalWidth(std::vector<পূর্ণসংখ্যা> &vec_a, std::vector<পূর্ণসংখ্যা> &vec_b, std::vector<পূর্ণসংখ্যা> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.আকার_2(), forced_signed);
		যখন (vec_y.আকার_2() < vec_a.আকার_2())
			vec_y.ঠেলো_back(ez->literal());
	}

	শূন্য extendSignalWidthUnary(std::vector<পূর্ণসংখ্যা> &vec_a, std::vector<পূর্ণসংখ্যা> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.গুনতি(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		যখন (vec_a.আকার_2() < vec_y.আকার_2())
			vec_a.ঠেলো_back(is_signed && vec_a.আকার_2() > 0 ? vec_a.back() : ez->CONST_FALSE);
		যখন (vec_y.আকার_2() < vec_a.আকার_2())
			vec_y.ঠেলো_back(ez->literal());
	}

	শূন্য undefGating(std::vector<পূর্ণসংখ্যা> &vec_y, std::vector<পূর্ণসংখ্যা> &vec_yy, std::vector<পূর্ণসংখ্যা> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.আকার_2() == vec_yy.আকার_2());
		যদি (vec_y.আকার_2() > vec_undef.আকার_2()) {
			std::vector<পূর্ণসংখ্যা> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.আকার_2());
			std::vector<পূর্ণসংখ্যা> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.আকার_2());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} অন্যথা {
			log_assert(vec_y.আকার_2() == vec_undef.আকার_2());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<পূর্ণসংখ্যা>, std::vector<পূর্ণসংখ্যা>> mux(পূর্ণসংখ্যা s, পূর্ণসংখ্যা undef_s, ধ্রুবক std::vector<পূর্ণসংখ্যা> &a, ধ্রুবক std::vector<পূর্ণসংখ্যা> &undef_a, ধ্রুবক std::vector<পূর্ণসংখ্যা> &b, ধ্রুবক std::vector<পূর্ণসংখ্যা> &undef_b) {
		std::vector<পূর্ণসংখ্যা> res;
		std::vector<পূর্ণসংখ্যা> undef_res;
		res = ez->vec_ite(s, b, a);
		যদি (model_undef) {
			std::vector<পূর্ণসংখ্যা> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<পূর্ণসংখ্যা> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		ফেরত std::make_pair(res, undef_res);
	}

	শূন্য undefGating(পূর্ণসংখ্যা y, পূর্ণসংখ্যা yy, পূর্ণসংখ্যা undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	শূন্য setInitState(পূর্ণসংখ্যা timestep)
	{
		auto চাবি = make_pair(prefix, timestep);
		log_assert(initstates.গুনতি(চাবি) == 0 || initstates.at(চাবি) == true);
		initstates[চাবি] = true;
	}

	bool importCell(RTLIL::Cell *cell, পূর্ণসংখ্যা timestep = -1);
};

শূন্য report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
