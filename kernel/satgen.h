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

#شامل "kernel/rtlil.h"
#شامل "kernel/sigtools.h"
#شامل "kernel/celltypes.h"
#شامل "kernel/macc.h"

#شامل "libs/ezsat/ezminisat.h"

YOSYS_NAMESPACE_BEGIN

// defined in kernel/register.cc
بیرونی ساختار SatSolver *yosys_satsolver_list;
بیرونی ساختار SatSolver *yosys_satsolver;

ساختار SatSolver
{
	string name;
	SatSolver *next;
	مجازی ezSAT *create() = 0;

	SatSolver(string name) : name(name) {
		next = yosys_satsolver_list;
		yosys_satsolver_list = this;
	}

	مجازی ~SatSolver() {
		auto p = &yosys_satsolver_list;
		وقتی (*p) {
			اگر (*p == this)
				*p = next;
			وگرنه
				p = &(*p)->next;
		}
		اگر (yosys_satsolver == this)
			yosys_satsolver = yosys_satsolver_list;
	}
};

ساختار ezSatPtr : عمومی std::unique_ptr<ezSAT> {
	ezSatPtr() : unique_ptr<ezSAT>(yosys_satsolver->create()) { }
	explicit ezSatPtr(SatSolver *solver) : unique_ptr<ezSAT>((solver ? solver : yosys_satsolver)->create()) { }
};

ساختار SatGen
{
	ezSAT *ez;
	ثابت SigMap *sigmap;
	std::string prefix;
	SigPool initial_state;
	std::map<std::string, RTLIL::SigSpec> asserts_a, asserts_en;
	std::map<std::string, RTLIL::SigSpec> assumes_a, assumes_en;
	std::map<std::string, std::map<RTLIL::SigBit, صحیح>> imported_signals;
	std::map<std::pair<std::string, صحیح>, bool> initstates;
	bool ignore_div_by_zero;
	bool model_undef;
	bool def_formal = false;
	bool model_barriers = false;

	SatGen(ezSAT *ez, ثابت SigMap *sigmap, std::string prefix = std::string()) :
			ez(ez), sigmap(sigmap), prefix(prefix), ignore_div_by_zero(false), model_undef(false)
	{
	}

	تهی setContext(ثابت SigMap *sigmap, std::string prefix = std::string())
	{
		this->sigmap = sigmap;
		this->prefix = prefix;
	}

	std::vector<صحیح> importSigSpecWorker(RTLIL::SigSpec sig, std::string &pf, bool undef_mode, bool dup_undef)
	{
		log_assert(!undef_mode || model_undef);
		sigmap->apply(sig);

		std::vector<صحیح> vec;
		vec.reserve(GetSize(sig));

		برای (auto &bit : sig)
			اگر (bit.wire == پوچ) {
				اگر (model_undef && dup_undef && bit == RTLIL::State::Sx)
					vec.افزودن_back(ez->frozen_literal());
				وگرنه
					vec.افزودن_back(bit == (undef_mode ? RTLIL::State::Sx : RTLIL::State::S1) ? ez->CONST_TRUE : ez->CONST_FALSE);
			} وگرنه {
				std::string wire_name = bit.wire->name.unescape();
				std::string name = pf +
					(bit.wire->width == 1 ? wire_name : stringf("%s [%d]", wire_name, bit.offset));
				vec.افزودن_back(ez->frozen_literal(name));
				imported_signals[pf][bit] = vec.back();
			}
		بازگشت vec;
	}

	std::vector<صحیح> importSigSpec(RTLIL::SigSpec sig, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		بازگشت importSigSpecWorker(sig, pf, false, false);
	}

	std::vector<صحیح> importDefSigSpec(RTLIL::SigSpec sig, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		بازگشت importSigSpecWorker(sig, pf, false, true);
	}

	std::vector<صحیح> importUndefSigSpec(RTLIL::SigSpec sig, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		بازگشت importSigSpecWorker(sig, pf, true, false);
	}

	صحیح importSigBit(RTLIL::SigBit bit, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		بازگشت importSigSpecWorker(bit, pf, false, false).front();
	}

	صحیح importDefSigBit(RTLIL::SigBit bit, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		بازگشت importSigSpecWorker(bit, pf, false, true).front();
	}

	صحیح importUndefSigBit(RTLIL::SigBit bit, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = "undef:" + prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		بازگشت importSigSpecWorker(bit, pf, true, false).front();
	}

	bool importedSigBit(RTLIL::SigBit bit, صحیح timestep = -1)
	{
		log_assert(timestep != 0);
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		بازگشت imported_signals[pf].شمار(bit) != 0;
	}

	تهی getAsserts(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحیح timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = asserts_a[pf];
		sig_en = asserts_en[pf];
	}

	تهی getAssumes(RTLIL::SigSpec &sig_a, RTLIL::SigSpec &sig_en, صحیح timestep = -1)
	{
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		sig_a = assumes_a[pf];
		sig_en = assumes_en[pf];
	}

	صحیح importAsserts(صحیح timestep = -1)
	{
		std::vector<صحیح> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		اگر (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_a[pf], timestep)), importDefSigSpec(asserts_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(asserts_en[pf], timestep)), importDefSigSpec(asserts_en[pf], timestep));
		} وگرنه {
			check_bits = importDefSigSpec(asserts_a[pf], timestep);
			enable_bits = importDefSigSpec(asserts_en[pf], timestep);
		}
		بازگشت ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	صحیح importAssumes(صحیح timestep = -1)
	{
		std::vector<صحیح> check_bits, enable_bits;
		std::string pf = prefix + (timestep == -1 ? "" : stringf("@%d:", timestep));
		اگر (model_undef) {
			check_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_a[pf], timestep)), importDefSigSpec(assumes_a[pf], timestep));
			enable_bits = ez->vec_and(ez->vec_not(importUndefSigSpec(assumes_en[pf], timestep)), importDefSigSpec(assumes_en[pf], timestep));
		} وگرنه {
			check_bits = importDefSigSpec(assumes_a[pf], timestep);
			enable_bits = importDefSigSpec(assumes_en[pf], timestep);
		}
		بازگشت ez->vec_reduce_and(ez->vec_or(check_bits, ez->vec_not(enable_bits)));
	}

	صحیح signals_eq(RTLIL::SigSpec lhs, RTLIL::SigSpec rhs, صحیح timestep_lhs = -1, صحیح timestep_rhs = -1)
	{
		اگر (timestep_rhs < 0)
			timestep_rhs = timestep_lhs;

		log_assert(lhs.size() == rhs.size());

		std::vector<صحیح> vec_lhs = importSigSpec(lhs, timestep_lhs);
		std::vector<صحیح> vec_rhs = importSigSpec(rhs, timestep_rhs);

		اگر (!model_undef)
			بازگشت ez->vec_eq(vec_lhs, vec_rhs);

		std::vector<صحیح> undef_lhs = importUndefSigSpec(lhs, timestep_lhs);
		std::vector<صحیح> undef_rhs = importUndefSigSpec(rhs, timestep_rhs);

		std::vector<صحیح> eq_bits;
		برای (صحیح i = 0; i < lhs.size(); i++)
			eq_bits.افزودن_back(ez->AND(ez->IFF(undef_lhs.at(i), undef_rhs.at(i)),
					ez->IFF(ez->OR(vec_lhs.at(i), undef_lhs.at(i)), ez->OR(vec_rhs.at(i), undef_rhs.at(i)))));
		بازگشت ez->expression(ezSAT::OpAnd, eq_bits);
	}

	تهی extendSignalWidth(std::vector<صحیح> &vec_a, std::vector<صحیح> &vec_b, RTLIL::Cell *cell, size_t y_width = 0, bool forced_signed = false)
	{
		bool is_signed = forced_signed;
		اگر (!forced_signed && cell->parameters.شمار(ID::A_SIGNED) > 0 && cell->parameters.شمار(ID::B_SIGNED) > 0)
			is_signed = cell->parameters[ID::A_SIGNED].as_bool() && cell->parameters[ID::B_SIGNED].as_bool();
		وقتی (vec_a.size() < vec_b.size() || vec_a.size() < y_width)
			vec_a.افزودن_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		وقتی (vec_b.size() < vec_a.size() || vec_b.size() < y_width)
			vec_b.افزودن_back(is_signed && vec_b.size() > 0 ? vec_b.back() : ez->CONST_FALSE);
	}

	تهی extendSignalWidth(std::vector<صحیح> &vec_a, std::vector<صحیح> &vec_b, std::vector<صحیح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		extendSignalWidth(vec_a, vec_b, cell, vec_y.size(), forced_signed);
		وقتی (vec_y.size() < vec_a.size())
			vec_y.افزودن_back(ez->literal());
	}

	تهی extendSignalWidthUnary(std::vector<صحیح> &vec_a, std::vector<صحیح> &vec_y, RTLIL::Cell *cell, bool forced_signed = false)
	{
		bool is_signed = forced_signed || (cell->parameters.شمار(ID::A_SIGNED) > 0 && cell->parameters[ID::A_SIGNED].as_bool());
		وقتی (vec_a.size() < vec_y.size())
			vec_a.افزودن_back(is_signed && vec_a.size() > 0 ? vec_a.back() : ez->CONST_FALSE);
		وقتی (vec_y.size() < vec_a.size())
			vec_y.افزودن_back(ez->literal());
	}

	تهی undefGating(std::vector<صحیح> &vec_y, std::vector<صحیح> &vec_yy, std::vector<صحیح> &vec_undef)
	{
		log_assert(model_undef);
		log_assert(vec_y.size() == vec_yy.size());
		اگر (vec_y.size() > vec_undef.size()) {
			std::vector<صحیح> trunc_y(vec_y.begin(), vec_y.begin() + vec_undef.size());
			std::vector<صحیح> trunc_yy(vec_yy.begin(), vec_yy.begin() + vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(trunc_y, trunc_yy))));
		} وگرنه {
			log_assert(vec_y.size() == vec_undef.size());
			ez->assume(ez->expression(ezSAT::OpAnd, ez->vec_or(vec_undef, ez->vec_iff(vec_y, vec_yy))));
		}
	}

	std::pair<std::vector<صحیح>, std::vector<صحیح>> mux(صحیح s, صحیح undef_s, ثابت std::vector<صحیح> &a, ثابت std::vector<صحیح> &undef_a, ثابت std::vector<صحیح> &b, ثابت std::vector<صحیح> &undef_b) {
		std::vector<صحیح> res;
		std::vector<صحیح> undef_res;
		res = ez->vec_ite(s, b, a);
		اگر (model_undef) {
			std::vector<صحیح> unequal_ab = ez->vec_not(ez->vec_iff(a, b));
			std::vector<صحیح> undef_ab = ez->vec_or(unequal_ab, ez->vec_or(undef_a, undef_b));
			undef_res = ez->vec_ite(undef_s, undef_ab, ez->vec_ite(s, undef_b, undef_a));
		}
		بازگشت std::make_pair(res, undef_res);
	}

	تهی undefGating(صحیح y, صحیح yy, صحیح undef)
	{
		ez->assume(ez->OR(undef, ez->IFF(y, yy)));
	}

	تهی setInitState(صحیح timestep)
	{
		auto کلید = make_pair(prefix, timestep);
		log_assert(initstates.شمار(کلید) == 0 || initstates.at(کلید) == true);
		initstates[کلید] = true;
	}

	bool importCell(RTLIL::Cell *cell, صحیح timestep = -1);
};

تهی report_missing_model(bool warn_only, RTLIL::Cell* cell);

YOSYS_NAMESPACE_END

#endif
