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
বাহ্য সংরচনা SatSolver *yosys_satsolver_list;
বাহ্য সংরচনা SatSolver *yosys_satsolver;

সংরচনা SatSolver
{
	string name;
	SatSolver *next;
	আভাস ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	আভাস ~SatSolver() {
		auto p = &yosys_satsolver_list;
		যাবত্ (*p) {
			যদি (*p == this)
				*p = next;
			অন্যথা
				p = &(*p)->next;
		}
		যদি (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

সংরচনা ezSatPtr : সার্বজনিক std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

সংরচনা SatGen
{
	ezSAT *ez;
	ধ্রুব SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, পূর্ণাঙ্ক>> imported_signals;
	std::map<std::pair<std::string, পূর্ণাঙ্ক>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ধ্রুব SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	শূন্য setContext(ধ্রুব SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<পূর্ণাঙ্ক> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<পূর্ণাঙ্ক> vec;
		vec.reserve(GetSize(sig));

		প্রতি (auto &bit : sig)
			যদি (bit.wire == রিক্ত) {
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
		প্রত্যাগম vec;
	}

	std::vector<পূর্ণাঙ্ক> importSigSpec(RTLIL::SigSpec sig, পূর্ণাঙ্ক timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		প্রত্যাগম importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<পূর্ণাঙ্ক> importDefSigSpec(RTLIL::SigSpec sig, পূর্ণাঙ্ক timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		প্রত্যাগম importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<পূর্ণাঙ্ক> importUndefSigSpec(RTLIL::SigSpec sig, পূর্ণাঙ্ক timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		প্রত্যাগম importSigSpecWorker(sig, pf, true, false);
	}

	পূর্ণাঙ্ক importSigBit(RTLIL::SigBit bit, পূর্ণাঙ্ক timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		প্রত্যাগম importSigSpecWorker(bit, pf, false, false).front();
	}

	পূর্ণাঙ্ক importDefSigBit(RTLIL::SigBit bit, পূর্ণাঙ্ক timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		প্রত্যাগম importSigSpecWorker(bit, pf, false, true).front();
	}

	পূর্ণাঙ্ক importUndefSigBit(RTLIL::SigBit bit, পূর্ণাঙ্ক timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		প্রত্যাগম importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, পূর্ণাঙ্ক timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		প্রত্যাগম imported_signals[pf].গুনতি(bit) != 0;
	}

	শূন্য getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, পূর্ণাঙ্ক timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	শূন্য getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, পূর্ণাঙ্ক timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	পূর্ণাঙ্ক importAsserts(পূর্ণাঙ্ক timestep = -1)
	{
		std::vector<পূর্ণাঙ্ক> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		যদি (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} অন্যথা {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		প্রত্যাগম ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	পূর্ণাঙ্ক importAssumes(পূর্ণাঙ্ক timestep = -1)
	{
		std::vector<পূর্ণাঙ্ক> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		যদি (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} অন্যথা {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		প্রত্যাগম ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	পূর্ণাঙ্ক signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, পূর্ণাঙ্ক timestep_lhs = -1, পূর্ণাঙ্ক timestep_rhs = -1)
	{
		যদি (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<পূর্ণাঙ্ক> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<পূর্ণাঙ্ক> vec_rhs = importSigSpec(rhs, timestep_rhs);

		যদি (!model_undef)
			প্রত্যাগম ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<পূর্ণাঙ্ক> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<পূর্ণাঙ্ক> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<পূর্ণাঙ্ক> eq_bits;
		প্রতি (পূর্ণাঙ্ক i = 0; i < lhs.size(); i++)
			eq_bits.ঠেলো_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		প্রত্যাগম ez->expression(ezSAT::OpAnd, eq_bits);
	}

	শূন্য extendSignalWidth(std::vector<পূর্ণাঙ্ক> &vec_a, std::vector<পূর্ণাঙ্ক> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		যদি (!forced_signed && cell->parameters.গুনতি(ID::A_SIGNED) > 0 && cell->parameters.গুনতি(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		যাবত্ (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.ঠেলো_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		যাবত্ (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.ঠেলো_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	শূন্য extendSignalWidth(std::vector<পূর্ণাঙ্ক> &vec_a, std::vector<পূর্ণাঙ্ক> &vec_b, std::vector<পূর্ণাঙ্ক> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		যাবত্ (vec_y.size() < vec_a.size())
			vec_y.ঠেলো_back(ez->literal());
	}

	শূন্য extendSignalWidthUnary(std::vector<পূর্ণাঙ্ক> &vec_a, std::vector<পূর্ণাঙ্ক> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.গুনতি(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		যাবত্ (vec_a.size() < vec_y.size())
			vec_a.ঠেলো_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		যাবত্ (vec_y.size() < vec_a.size())
			vec_y.ঠেলো_back(ez->literal());
	}

	শূন্য undefGating(std::vector<পূর্ণাঙ্ক> &vec_y, std::vector<পূর্ণাঙ্ক> &vec_yy, std::vector<পূর্ণাঙ্ক> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		যদি (vec_y.size() > vec_undef.size()) {
			std::vector<পূর্ণাঙ্ক> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<পূর্ণাঙ্ক> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} অন্যথা {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<পূর্ণাঙ্ক>, std::vector<পূর্ণাঙ্ক>> mux(পূর্ণাঙ্ক s, পূর্ণাঙ্ক undef_s, ধ্রুব std::vector<পূর্ণাঙ্ক> &a, ধ্রুব std::vector<পূর্ণাঙ্ক> &undef_a, ধ্রুব std::vector<পূর্ণাঙ্ক> &b, ধ্রুব std::vector<পূর্ণাঙ্ক> &undef_b) {
		std::vector<পূর্ণাঙ্ক> res;
		std::vector<পূর্ণাঙ্ক> undef_res;
		res = ez->vec_ite(s, b, a);
		যদি (model_undef) {
			std::vector<পূর্ণাঙ্ক> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<পূর্ণাঙ্ক> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		প্রত্যাগম std::make_pair(res, undef_res);
	}

	শূন্য undefGating(পূর্ণাঙ্ক y, পূর্ণাঙ্ক yy, পূর্ণাঙ্ক undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	শূন্য setInitState(পূর্ণাঙ্ক timestep)
	{
		auto চাবি = make_pair(prefix, timestep);
		log_assert(initstates.গুনতি(চাবি) == 0 || initstates.at(চাবি) == true);
		initstates[চাবি] = true;
	}

	bool importCell(RTLIL::Cell *cell, পূর্ণাঙ্ক timestep = -1);
};

শূন্য report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
