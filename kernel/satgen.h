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

#अगर_अघोषित SATGEN_H
#घोषणा SATGEN_H

#समावेश "kernel/rtlil.h"
#समावेश "kernel/sigtools.h"
#समावेश "kernel/celltypes.h"
#समावेश "kernel/macc.h"

#समावेश "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
बाह्य काष्ठा SatSolver *yosys_satsolver_list;
बाह्य काष्ठा SatSolver *yosys_satsolver;

काष्ठा SatSolver
{
	string name;
	SatSolver *अग्र;
	भव ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		अग्र = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	भव ~SatSolver() {
		स्वतः p = &yosys_satsolver_list;
		जबतक (*p) {
			अगर (*p == this)
				*p = अग्र;
			अन्यथा
				p = &(*p)->अग्र;
		}
		अगर (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

काष्ठा ezSatPtr : खुला std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

काष्ठा SatGen
{
	ezSAT *ez;
	स्थिर SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, पूर्णांक>> imported_signals;
	std::map<std::pair<std::string, पूर्णांक>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, स्थिर SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	व्योम setContext(स्थिर SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<पूर्णांक> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<पूर्णांक> vec;
		vec.reserve(Get_आकार(sig));

		क्रम (स्वतः &bit : sig)
			अगर (bit.wire == शून्य) {
				अगर (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.धकेलो_back(ez->frozen_literal());
				अन्यथा
					vec.धकेलो_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} अन्यथा {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.धकेलो_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		वापस vec;
	}

	std::vector<पूर्णांक> importSigSpec(RTLIL::SigSpec sig, पूर्णांक timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		वापस importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<पूर्णांक> importDefSigSpec(RTLIL::SigSpec sig, पूर्णांक timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		वापस importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<पूर्णांक> importUndefSigSpec(RTLIL::SigSpec sig, पूर्णांक timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		वापस importSigSpecWorker(sig, pf, true, false);
	}

	पूर्णांक importSigBit(RTLIL::SigBit bit, पूर्णांक timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		वापस importSigSpecWorker(bit, pf, false, false).front();
	}

	पूर्णांक importDefSigBit(RTLIL::SigBit bit, पूर्णांक timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		वापस importSigSpecWorker(bit, pf, false, true).front();
	}

	पूर्णांक importUndefSigBit(RTLIL::SigBit bit, पूर्णांक timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		वापस importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, पूर्णांक timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		वापस imported_signals[pf].गिनती(bit) != 0;
	}

	व्योम getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, पूर्णांक timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	व्योम getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, पूर्णांक timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	पूर्णांक importAsserts(पूर्णांक timestep = -1)
	{
		std::vector<पूर्णांक> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		अगर (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} अन्यथा {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		वापस ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	पूर्णांक importAssumes(पूर्णांक timestep = -1)
	{
		std::vector<पूर्णांक> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		अगर (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} अन्यथा {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		वापस ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	पूर्णांक signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, पूर्णांक timestep_lhs = -1, पूर्णांक timestep_rhs = -1)
	{
		अगर (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.आकार() == rhs.आकार());

		std::vector<पूर्णांक> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<पूर्णांक> vec_rhs = importSigSpec(rhs, timestep_rhs);

		अगर (!model_undef)
			वापस ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<पूर्णांक> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<पूर्णांक> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<पूर्णांक> eq_bits;
		क्रम (पूर्णांक i = 0; i < lhs.आकार(); i++)
			eq_bits.धकेलो_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		वापस ez->expression(ezSAT::OpAnd, eq_bits);
	}

	व्योम extendSignalWidth(std::vector<पूर्णांक> &vec_a, std::vector<पूर्णांक> &vec_b, RTLIL::Cell *cell, माप_प्रकार y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		अगर (!forced_signed && cell->parameters.गिनती(ID::A_SIGNED) > 0 && cell->parameters.गिनती(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		जबतक (vec_a.आकार() < vec_b.आकार() || vec_a.आकार() < y_width)
			vec_a.धकेलो_back(is_signed && vec_a.आकार() > 0 ? vec_a.back() : ez->CONST_FALSE);
		जबतक (vec_b.आकार() < vec_a.आकार() || vec_b.आकार() < y_width)
			vec_b.धकेलो_back(is_signed && vec_b.आकार() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	व्योम extendSignalWidth(std::vector<पूर्णांक> &vec_a, std::vector<पूर्णांक> &vec_b, std::vector<पूर्णांक> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.आकार(), forced_signed);
		जबतक (vec_y.आकार() < vec_a.आकार())
			vec_y.धकेलो_back(ez->literal());
	}

	व्योम extendSignalWidthUnary(std::vector<पूर्णांक> &vec_a, std::vector<पूर्णांक> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.गिनती(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		जबतक (vec_a.आकार() < vec_y.आकार())
			vec_a.धकेलो_back(is_signed && vec_a.आकार() > 0 ? vec_a.back() : ez->CONST_FALSE);
		जबतक (vec_y.आकार() < vec_a.आकार())
			vec_y.धकेलो_back(ez->literal());
	}

	व्योम undefGating(std::vector<पूर्णांक> &vec_y, std::vector<पूर्णांक> &vec_yy, std::vector<पूर्णांक> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.आकार() == vec_yy.आकार());
		अगर (vec_y.आकार() > vec_undef.आकार()) {
			std::vector<पूर्णांक> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.आकार());
			std::vector<पूर्णांक> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.आकार());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} अन्यथा {
			log_assert(vec_y.आकार() == vec_undef.आकार());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<पूर्णांक>, std::vector<पूर्णांक>> mux(पूर्णांक s, पूर्णांक undef_s, स्थिर std::vector<पूर्णांक> &a, स्थिर std::vector<पूर्णांक> &undef_a, स्थिर std::vector<पूर्णांक> &b, स्थिर std::vector<पूर्णांक> &undef_b) {
		std::vector<पूर्णांक> res;
		std::vector<पूर्णांक> undef_res;
		res = ez->vec_ite(s, b, a);
		अगर (model_undef) {
			std::vector<पूर्णांक> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<पूर्णांक> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		वापस std::make_pair(res, undef_res);
	}

	व्योम undefGating(पूर्णांक y, पूर्णांक yy, पूर्णांक undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	व्योम setInitState(पूर्णांक timestep)
	{
		स्वतः कुंजी = make_pair(prefix, timestep);
		log_assert(initstates.गिनती(कुंजी) == 0 || initstates.at(कुंजी) == true);
		initstates[कुंजी] = true;
	}

	bool importCell(RTLIL::Cell *cell, पूर्णांक timestep = -1);
};

व्योम report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#पूर्ण_अगर
