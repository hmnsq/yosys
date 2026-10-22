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

#அந்தர்பாவ "kernel/rtlil.h"
#அந்தர்பாவ "kernel/sigtools.h"
#அந்தர்பாவ "kernel/celltypes.h"
#அந்தர்பாவ "kernel/macc.h"

#அந்தர்பாவ "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
வெளி கட்டமைப்பு SatSolver *yosys_satsolver_list;
வெளி கட்டமைப்பு SatSolver *yosys_satsolver;

கட்டமைப்பு SatSolver
{
	string name;
	SatSolver *next;
	மெய்நிகர் ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	மெய்நிகர் ~SatSolver() {
		auto p = &yosys_satsolver_list;
		போது (*p) {
			எனில் (*p == this)
				*p = next;
			இல்லையெனில்
				p = &(*p)->next;
		}
		எனில் (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

கட்டமைப்பு ezSatPtr : பொது std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

கட்டமைப்பு SatGen
{
	ezSAT *ez;
	மாறிலி SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, முழு>> imported_signals;
	std::map<std::pair<std::string, முழு>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, மாறிலி SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	வெறுமை setContext(மாறிலி SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<முழு> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<முழு> vec;
		vec.reserve(GetSize(sig));

		க்கு (auto &bit : sig)
			எனில் (bit.wire == வெற்று) {
				எனில் (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.தள்ளு_back(ez->frozen_literal());
				இல்லையெனில்
					vec.தள்ளு_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} இல்லையெனில் {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.தள்ளு_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		திரும்பு vec;
	}

	std::vector<முழு> importSigSpec(RTLIL::SigSpec sig, முழு timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		திரும்பு importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<முழு> importDefSigSpec(RTLIL::SigSpec sig, முழு timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		திரும்பு importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<முழு> importUndefSigSpec(RTLIL::SigSpec sig, முழு timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		திரும்பு importSigSpecWorker(sig, pf, true, false);
	}

	முழு importSigBit(RTLIL::SigBit bit, முழு timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		திரும்பு importSigSpecWorker(bit, pf, false, false).front();
	}

	முழு importDefSigBit(RTLIL::SigBit bit, முழு timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		திரும்பு importSigSpecWorker(bit, pf, false, true).front();
	}

	முழு importUndefSigBit(RTLIL::SigBit bit, முழு timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		திரும்பு importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, முழு timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		திரும்பு imported_signals[pf].மொத்தம்(bit) != 0;
	}

	வெறுமை getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, முழு timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	வெறுமை getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, முழு timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	முழு importAsserts(முழு timestep = -1)
	{
		std::vector<முழு> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		எனில் (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} இல்லையெனில் {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		திரும்பு ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	முழு importAssumes(முழு timestep = -1)
	{
		std::vector<முழு> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		எனில் (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} இல்லையெனில் {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		திரும்பு ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	முழு signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, முழு timestep_lhs = -1, முழு timestep_rhs = -1)
	{
		எனில் (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<முழு> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<முழு> vec_rhs = importSigSpec(rhs, timestep_rhs);

		எனில் (!model_undef)
			திரும்பு ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<முழு> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<முழு> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<முழு> eq_bits;
		க்கு (முழு i = 0; i < lhs.size(); i++)
			eq_bits.தள்ளு_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		திரும்பு ez->expression(ezSAT::OpAnd, eq_bits);
	}

	வெறுமை extendSignalWidth(std::vector<முழு> &vec_a, std::vector<முழு> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		எனில் (!forced_signed && cell->parameters.மொத்தம்(ID::A_SIGNED) > 0 && cell->parameters.மொத்தம்(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		போது (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.தள்ளு_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		போது (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.தள்ளு_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	வெறுமை extendSignalWidth(std::vector<முழு> &vec_a, std::vector<முழு> &vec_b, std::vector<முழு> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		போது (vec_y.size() < vec_a.size())
			vec_y.தள்ளு_back(ez->literal());
	}

	வெறுமை extendSignalWidthUnary(std::vector<முழு> &vec_a, std::vector<முழு> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.மொத்தம்(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		போது (vec_a.size() < vec_y.size())
			vec_a.தள்ளு_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		போது (vec_y.size() < vec_a.size())
			vec_y.தள்ளு_back(ez->literal());
	}

	வெறுமை undefGating(std::vector<முழு> &vec_y, std::vector<முழு> &vec_yy, std::vector<முழு> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		எனில் (vec_y.size() > vec_undef.size()) {
			std::vector<முழு> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<முழு> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} இல்லையெனில் {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<முழு>, std::vector<முழு>> mux(முழு s, முழு undef_s, மாறிலி std::vector<முழு> &a, மாறிலி std::vector<முழு> &undef_a, மாறிலி std::vector<முழு> &b, மாறிலி std::vector<முழு> &undef_b) {
		std::vector<முழு> res;
		std::vector<முழு> undef_res;
		res = ez->vec_ite(s, b, a);
		எனில் (model_undef) {
			std::vector<முழு> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<முழு> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		திரும்பு std::make_pair(res, undef_res);
	}

	வெறுமை undefGating(முழு y, முழு yy, முழு undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	வெறுமை setInitState(முழு timestep)
	{
		auto சாவி = make_pair(prefix, timestep);
		log_assert(initstates.மொத்தம்(சாவி) == 0 || initstates.at(சாவி) == true);
		initstates[சாவி] = true;
	}

	bool importCell(RTLIL::Cell *cell, முழு timestep = -1);
};

வெறுமை report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
