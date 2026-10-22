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

#അന്തര്ഭാവ "kernel/rtlil.h"
#അന്തര്ഭാവ "kernel/sigtools.h"
#അന്തര്ഭാവ "kernel/celltypes.h"
#അന്തര്ഭാവ "kernel/macc.h"

#അന്തര്ഭാവ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
ബാഹ്യ ഘടന SatSolver *yosys_satsolver_list;
ബാഹ്യ ഘടന SatSolver *yosys_satsolver;

ഘടന SatSolver
{
	string name;
	SatSolver *next;
	മായാ ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	മായാ ~SatSolver() {
		auto p = &yosys_satsolver_list;
		ആകുമ്പോൾ (*p) {
			എങ്കിൽ (*p == this)
				*p = next;
			അല്ലെങ്കിൽ
				p = &(*p)->next;
		}
		എങ്കിൽ (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

ഘടന ezSatPtr : പൊതു std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

ഘടന SatGen
{
	ezSAT *ez;
	സ്ഥിരം SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, പൂർണ്ണസംഖ്യ>> imported_signals;
	std::map<std::pair<std::string, പൂർണ്ണസംഖ്യ>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, സ്ഥിരം SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	ശൂന്യം setContext(സ്ഥിരം SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<പൂർണ്ണസംഖ്യ> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<പൂർണ്ണസംഖ്യ> vec;
		vec.reserve(GetSize(sig));

		വേണ്ടി (auto &bit : sig)
			എങ്കിൽ (bit.wire == ശൂന്യ) {
				എങ്കിൽ (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.തള്ളുക_back(ez->frozen_literal());
				അല്ലെങ്കിൽ
					vec.തള്ളുക_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} അല്ലെങ്കിൽ {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.തള്ളുക_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		തിരികെ vec;
	}

	std::vector<പൂർണ്ണസംഖ്യ> importSigSpec(RTLIL::SigSpec sig, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		തിരികെ importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<പൂർണ്ണസംഖ്യ> importDefSigSpec(RTLIL::SigSpec sig, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		തിരികെ importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<പൂർണ്ണസംഖ്യ> importUndefSigSpec(RTLIL::SigSpec sig, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		തിരികെ importSigSpecWorker(sig, pf, true, false);
	}

	പൂർണ്ണസംഖ്യ importSigBit(RTLIL::SigBit bit, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		തിരികെ importSigSpecWorker(bit, pf, false, false).front();
	}

	പൂർണ്ണസംഖ്യ importDefSigBit(RTLIL::SigBit bit, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		തിരികെ importSigSpecWorker(bit, pf, false, true).front();
	}

	പൂർണ്ണസംഖ്യ importUndefSigBit(RTLIL::SigBit bit, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		തിരികെ importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		തിരികെ imported_signals[pf].എണ്ണം(bit) != 0;
	}

	ശൂന്യം getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	ശൂന്യം getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, പൂർണ്ണസംഖ്യ timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	പൂർണ്ണസംഖ്യ importAsserts(പൂർണ്ണസംഖ്യ timestep = -1)
	{
		std::vector<പൂർണ്ണസംഖ്യ> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		എങ്കിൽ (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} അല്ലെങ്കിൽ {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		തിരികെ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	പൂർണ്ണസംഖ്യ importAssumes(പൂർണ്ണസംഖ്യ timestep = -1)
	{
		std::vector<പൂർണ്ണസംഖ്യ> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		എങ്കിൽ (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} അല്ലെങ്കിൽ {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		തിരികെ ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	പൂർണ്ണസംഖ്യ signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, പൂർണ്ണസംഖ്യ timestep_lhs = -1, പൂർണ്ണസംഖ്യ timestep_rhs = -1)
	{
		എങ്കിൽ (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<പൂർണ്ണസംഖ്യ> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<പൂർണ്ണസംഖ്യ> vec_rhs = importSigSpec(rhs, timestep_rhs);

		എങ്കിൽ (!model_undef)
			തിരികെ ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<പൂർണ്ണസംഖ്യ> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<പൂർണ്ണസംഖ്യ> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<പൂർണ്ണസംഖ്യ> eq_bits;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < lhs.size(); i++)
			eq_bits.തള്ളുക_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		തിരികെ ez->expression(ezSAT::OpAnd, eq_bits);
	}

	ശൂന്യം extendSignalWidth(std::vector<പൂർണ്ണസംഖ്യ> &vec_a, std::vector<പൂർണ്ണസംഖ്യ> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		എങ്കിൽ (!forced_signed && cell->parameters.എണ്ണം(ID::A_SIGNED) > 0 && cell->parameters.എണ്ണം(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		ആകുമ്പോൾ (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.തള്ളുക_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ആകുമ്പോൾ (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.തള്ളുക_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	ശൂന്യം extendSignalWidth(std::vector<പൂർണ്ണസംഖ്യ> &vec_a, std::vector<പൂർണ്ണസംഖ്യ> &vec_b, std::vector<പൂർണ്ണസംഖ്യ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		ആകുമ്പോൾ (vec_y.size() < vec_a.size())
			vec_y.തള്ളുക_back(ez->literal());
	}

	ശൂന്യം extendSignalWidthUnary(std::vector<പൂർണ്ണസംഖ്യ> &vec_a, std::vector<പൂർണ്ണസംഖ്യ> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.എണ്ണം(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		ആകുമ്പോൾ (vec_a.size() < vec_y.size())
			vec_a.തള്ളുക_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		ആകുമ്പോൾ (vec_y.size() < vec_a.size())
			vec_y.തള്ളുക_back(ez->literal());
	}

	ശൂന്യം undefGating(std::vector<പൂർണ്ണസംഖ്യ> &vec_y, std::vector<പൂർണ്ണസംഖ്യ> &vec_yy, std::vector<പൂർണ്ണസംഖ്യ> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		എങ്കിൽ (vec_y.size() > vec_undef.size()) {
			std::vector<പൂർണ്ണസംഖ്യ> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<പൂർണ്ണസംഖ്യ> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} അല്ലെങ്കിൽ {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<പൂർണ്ണസംഖ്യ>, std::vector<പൂർണ്ണസംഖ്യ>> mux(പൂർണ്ണസംഖ്യ s, പൂർണ്ണസംഖ്യ undef_s, സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &a, സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &undef_a, സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &b, സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &undef_b) {
		std::vector<പൂർണ്ണസംഖ്യ> res;
		std::vector<പൂർണ്ണസംഖ്യ> undef_res;
		res = ez->vec_ite(s, b, a);
		എങ്കിൽ (model_undef) {
			std::vector<പൂർണ്ണസംഖ്യ> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<പൂർണ്ണസംഖ്യ> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		തിരികെ std::make_pair(res, undef_res);
	}

	ശൂന്യം undefGating(പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ yy, പൂർണ്ണസംഖ്യ undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	ശൂന്യം setInitState(പൂർണ്ണസംഖ്യ timestep)
	{
		auto താക്കോൽ = make_pair(prefix, timestep);
		log_assert(initstates.എണ്ണം(താക്കോൽ) == 0 || initstates.at(താക്കോൽ) == true);
		initstates[താക്കോൽ] = true;
	}

	bool importCell(RTLIL::Cell *cell, പൂർണ്ണസംഖ്യ timestep = -1);
};

ശൂന്യം report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
