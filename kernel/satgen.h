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

#अन्तर्भाव "kernel/rtlil.h"
#अन्तर्भाव "kernel/sigtools.h"
#अन्तर्भाव "kernel/celltypes.h"
#अन्तर्भाव "kernel/macc.h"

#अन्तर्भाव "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
बाह्य संरचना SatSolver *yosys_satsolver_list;
बाह्य संरचना SatSolver *yosys_satsolver;

संरचना SatSolver
{
	string name;
	SatSolver *next;
	आभास ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	आभास ~SatSolver() {
		auto p = &yosys_satsolver_list;
		यावत् (*p) {
			यदि (*p == this)
				*p = next;
			अन्यथा
				p = &(*p)->next;
		}
		यदि (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

संरचना ezSatPtr : सार्वजनिक std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

संरचना SatGen
{
	ezSAT *ez;
	ध्रुव SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, पूर्णाङ्क>> imported_signals;
	std::map<std::pair<std::string, पूर्णाङ्क>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ध्रुव SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	शून्य setContext(ध्रुव SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<पूर्णाङ्क> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<पूर्णाङ्क> vec;
		vec.reserve(GetSize(sig));

		प्रति (auto &bit : sig)
			यदि (bit.wire == रिक्त) {
				यदि (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.ढकला_back(ez->frozen_literal());
				अन्यथा
					vec.ढकला_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} अन्यथा {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.ढकला_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		प्रत्यागम vec;
	}

	std::vector<पूर्णाङ्क> importSigSpec(RTLIL::SigSpec sig, पूर्णाङ्क timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		प्रत्यागम importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<पूर्णाङ्क> importDefSigSpec(RTLIL::SigSpec sig, पूर्णाङ्क timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		प्रत्यागम importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<पूर्णाङ्क> importUndefSigSpec(RTLIL::SigSpec sig, पूर्णाङ्क timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		प्रत्यागम importSigSpecWorker(sig, pf, true, false);
	}

	पूर्णाङ्क importSigBit(RTLIL::SigBit bit, पूर्णाङ्क timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		प्रत्यागम importSigSpecWorker(bit, pf, false, false).front();
	}

	पूर्णाङ्क importDefSigBit(RTLIL::SigBit bit, पूर्णाङ्क timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		प्रत्यागम importSigSpecWorker(bit, pf, false, true).front();
	}

	पूर्णाङ्क importUndefSigBit(RTLIL::SigBit bit, पूर्णाङ्क timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		प्रत्यागम importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, पूर्णाङ्क timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		प्रत्यागम imported_signals[pf].मोजणी(bit) != 0;
	}

	शून्य getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, पूर्णाङ्क timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	शून्य getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, पूर्णाङ्क timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	पूर्णाङ्क importAsserts(पूर्णाङ्क timestep = -1)
	{
		std::vector<पूर्णाङ्क> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		यदि (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} अन्यथा {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		प्रत्यागम ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	पूर्णाङ्क importAssumes(पूर्णाङ्क timestep = -1)
	{
		std::vector<पूर्णाङ्क> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		यदि (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} अन्यथा {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		प्रत्यागम ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	पूर्णाङ्क signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, पूर्णाङ्क timestep_lhs = -1, पूर्णाङ्क timestep_rhs = -1)
	{
		यदि (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<पूर्णाङ्क> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<पूर्णाङ्क> vec_rhs = importSigSpec(rhs, timestep_rhs);

		यदि (!model_undef)
			प्रत्यागम ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<पूर्णाङ्क> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<पूर्णाङ्क> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<पूर्णाङ्क> eq_bits;
		प्रति (पूर्णाङ्क i = 0; i < lhs.size(); i++)
			eq_bits.ढकला_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		प्रत्यागम ez->expression(ezSAT::OpAnd, eq_bits);
	}

	शून्य extendSignalWidth(std::vector<पूर्णाङ्क> &vec_a, std::vector<पूर्णाङ्क> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		यदि (!forced_signed && cell->parameters.मोजणी(ID::A_SIGNED) > 0 && cell->parameters.मोजणी(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		यावत् (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.ढकला_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		यावत् (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.ढकला_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	शून्य extendSignalWidth(std::vector<पूर्णाङ्क> &vec_a, std::vector<पूर्णाङ्क> &vec_b, std::vector<पूर्णाङ्क> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		यावत् (vec_y.size() < vec_a.size())
			vec_y.ढकला_back(ez->literal());
	}

	शून्य extendSignalWidthUnary(std::vector<पूर्णाङ्क> &vec_a, std::vector<पूर्णाङ्क> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.मोजणी(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		यावत् (vec_a.size() < vec_y.size())
			vec_a.ढकला_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		यावत् (vec_y.size() < vec_a.size())
			vec_y.ढकला_back(ez->literal());
	}

	शून्य undefGating(std::vector<पूर्णाङ्क> &vec_y, std::vector<पूर्णाङ्क> &vec_yy, std::vector<पूर्णाङ्क> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		यदि (vec_y.size() > vec_undef.size()) {
			std::vector<पूर्णाङ्क> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<पूर्णाङ्क> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} अन्यथा {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<पूर्णाङ्क>, std::vector<पूर्णाङ्क>> mux(पूर्णाङ्क s, पूर्णाङ्क undef_s, ध्रुव std::vector<पूर्णाङ्क> &a, ध्रुव std::vector<पूर्णाङ्क> &undef_a, ध्रुव std::vector<पूर्णाङ्क> &b, ध्रुव std::vector<पूर्णाङ्क> &undef_b) {
		std::vector<पूर्णाङ्क> res;
		std::vector<पूर्णाङ्क> undef_res;
		res = ez->vec_ite(s, b, a);
		यदि (model_undef) {
			std::vector<पूर्णाङ्क> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<पूर्णाङ्क> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		प्रत्यागम std::make_pair(res, undef_res);
	}

	शून्य undefGating(पूर्णाङ्क y, पूर्णाङ्क yy, पूर्णाङ्क undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	शून्य setInitState(पूर्णाङ्क timestep)
	{
		auto किल्ली = make_pair(prefix, timestep);
		log_assert(initstates.मोजणी(किल्ली) == 0 || initstates.at(किल्ली) == true);
		initstates[किल्ली] = true;
	}

	bool importCell(RTLIL::Cell *cell, पूर्णाङ्क timestep = -1);
};

शून्य report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
