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

#અન્તર્ભાવ "kernel/rtlil.h"
#અન્તર્ભાવ "kernel/sigtools.h"
#અન્તર્ભાવ "kernel/celltypes.h"
#અન્તર્ભાવ "kernel/macc.h"

#અન્તર્ભાવ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
બાહ્ય સંરચના SatSolver *yosys_satsolver_list;
બાહ્ય સંરચના SatSolver *yosys_satsolver;

સંરચના SatSolver
{
	string name;
	SatSolver *next;
	આભાસી ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	આભાસી ~SatSolver() {
		auto p = &yosys_satsolver_list;
		જ્યારે (*p) {
			જો (*p == this)
				*p = next;
			નહીંતર
				p = &(*p)->next;
		}
		જો (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

સંરચના ezSatPtr : જાહેર std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

સંરચના SatGen
{
	ezSAT *ez;
	અચલ SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, પૂર્ણાંક>> imported_signals;
	std::map<std::pair<std::string, પૂર્ણાંક>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, અચલ SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	શૂન્ય setContext(અચલ SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<પૂર્ણાંક> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<પૂર્ણાંક> vec;
		vec.reserve(GetSize(sig));

		માટે (auto &bit : sig)
			જો (bit.wire == ખાલી) {
				જો (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.ધકેલો_back(ez->frozen_literal());
				નહીંતર
					vec.ધકેલો_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} નહીંતર {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.ધકેલો_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		પાછા vec;
	}

	std::vector<પૂર્ણાંક> importSigSpec(RTLIL::SigSpec sig, પૂર્ણાંક timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		પાછા importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<પૂર્ણાંક> importDefSigSpec(RTLIL::SigSpec sig, પૂર્ણાંક timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		પાછા importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<પૂર્ણાંક> importUndefSigSpec(RTLIL::SigSpec sig, પૂર્ણાંક timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		પાછા importSigSpecWorker(sig, pf, true, false);
	}

	પૂર્ણાંક importSigBit(RTLIL::SigBit bit, પૂર્ણાંક timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		પાછા importSigSpecWorker(bit, pf, false, false).front();
	}

	પૂર્ણાંક importDefSigBit(RTLIL::SigBit bit, પૂર્ણાંક timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		પાછા importSigSpecWorker(bit, pf, false, true).front();
	}

	પૂર્ણાંક importUndefSigBit(RTLIL::SigBit bit, પૂર્ણાંક timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		પાછા importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, પૂર્ણાંક timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		પાછા imported_signals[pf].ગણતરી(bit) != 0;
	}

	શૂન્ય getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, પૂર્ણાંક timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	શૂન્ય getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, પૂર્ણાંક timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	પૂર્ણાંક importAsserts(પૂર્ણાંક timestep = -1)
	{
		std::vector<પૂર્ણાંક> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		જો (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} નહીંતર {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		પાછા ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	પૂર્ણાંક importAssumes(પૂર્ણાંક timestep = -1)
	{
		std::vector<પૂર્ણાંક> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		જો (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} નહીંતર {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		પાછા ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	પૂર્ણાંક signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, પૂર્ણાંક timestep_lhs = -1, પૂર્ણાંક timestep_rhs = -1)
	{
		જો (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<પૂર્ણાંક> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<પૂર્ણાંક> vec_rhs = importSigSpec(rhs, timestep_rhs);

		જો (!model_undef)
			પાછા ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<પૂર્ણાંક> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<પૂર્ણાંક> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<પૂર્ણાંક> eq_bits;
		માટે (પૂર્ણાંક i = 0; i < lhs.size(); i++)
			eq_bits.ધકેલો_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		પાછા ez->expression(ezSAT::OpAnd, eq_bits);
	}

	શૂન્ય extendSignalWidth(std::vector<પૂર્ણાંક> &vec_a, std::vector<પૂર્ણાંક> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		જો (!forced_signed && cell->parameters.ગણતરી(ID::A_SIGNED) > 0 && cell->parameters.ગણતરી(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		જ્યારે (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.ધકેલો_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		જ્યારે (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.ધકેલો_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	શૂન્ય extendSignalWidth(std::vector<પૂર્ણાંક> &vec_a, std::vector<પૂર્ણાંક> &vec_b, std::vector<પૂર્ણાંક> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		જ્યારે (vec_y.size() < vec_a.size())
			vec_y.ધકેલો_back(ez->literal());
	}

	શૂન્ય extendSignalWidthUnary(std::vector<પૂર્ણાંક> &vec_a, std::vector<પૂર્ણાંક> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.ગણતરી(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		જ્યારે (vec_a.size() < vec_y.size())
			vec_a.ધકેલો_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		જ્યારે (vec_y.size() < vec_a.size())
			vec_y.ધકેલો_back(ez->literal());
	}

	શૂન્ય undefGating(std::vector<પૂર્ણાંક> &vec_y, std::vector<પૂર્ણાંક> &vec_yy, std::vector<પૂર્ણાંક> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		જો (vec_y.size() > vec_undef.size()) {
			std::vector<પૂર્ણાંક> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<પૂર્ણાંક> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} નહીંતર {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<પૂર્ણાંક>, std::vector<પૂર્ણાંક>> mux(પૂર્ણાંક s, પૂર્ણાંક undef_s, અચલ std::vector<પૂર્ણાંક> &a, અચલ std::vector<પૂર્ણાંક> &undef_a, અચલ std::vector<પૂર્ણાંક> &b, અચલ std::vector<પૂર્ણાંક> &undef_b) {
		std::vector<પૂર્ણાંક> res;
		std::vector<પૂર્ણાંક> undef_res;
		res = ez->vec_ite(s, b, a);
		જો (model_undef) {
			std::vector<પૂર્ણાંક> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<પૂર્ણાંક> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		પાછા std::make_pair(res, undef_res);
	}

	શૂન્ય undefGating(પૂર્ણાંક y, પૂર્ણાંક yy, પૂર્ણાંક undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	શૂન્ય setInitState(પૂર્ણાંક timestep)
	{
		auto ચાવી = make_pair(prefix, timestep);
		log_assert(initstates.ગણતરી(ચાવી) == 0 || initstates.at(ચાવી) == true);
		initstates[ચાવી] = true;
	}

	bool importCell(RTLIL::Cell *cell, પૂર્ણાંક timestep = -1);
};

શૂન્ય report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
