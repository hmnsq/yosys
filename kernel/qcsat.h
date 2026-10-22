/* -*- c++ -*-
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2021  Marcelina Kościelnicka <mwk@0x04.net>
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

#جيڪڏهن_وصف_نه_ٿيل QCSAT_H
#وصف_ڪريو QCSAT_H

#ضمن "kernel/satgen.h"
#ضمن "kernel/modtools.h"

YOSYS_NAMESPACE_BEGIN

// This is a helper class meant for easy construction of quick SAT queries
// to a combinatorial input cone of some set of signals, meant for SAT-based
// optimizations.  Various knobs are provided to set just how much of the
// cone should be included in the model — since this class is meant for
// optimization, it should not be a correctness problem when some cells are
// skipped and the solver spuriously returns SAT with a solution that
// cannot exist in reality due to skipped constraints (ie. only UNSAT results
// from this class should be considered binding).
بنية تيز_Cone_Sat {
	ModWalker &modwalker;
	ezSatPtr ez;
	SatGen satgen;

	// The effort level knobs.

	// The maximum "complexity level" of cells that will be imported.
	// - 1: bitwise operations, muxes, equality comparisons, lut, sop, fa
	// - 2: addition, subtraction, greater/less than comparisons, lcu
	// - 3: shifts
	// - 4: multiplication, division, power
	صحيح max_cell_complexity = 2;
	// The maximum number of cells to import, or 0 for no limit.
	صحيح max_cell_ڳڻپ = 0;
	// If non-0, skip importing cells with more than this number of output bits.
	صحيح max_cell_outs = 0;

	// Internal state.
	pool<RTLIL::Cell*> imported_cells;
	pool<RTLIL::Wire*> imported_onehot;
	pool<RTLIL::Sig_ٻٽو_انگ> بٽ_قطار;

	تيز_Cone_Sat(ModWalker &modwalker) : modwalker(modwalker), ez(), satgen(ez.get(), &modwalker.sigmap) {}

	// Imports a signal into the SAT solver, queues its input cone to be
	// imported in the next prepare() call.
	std::ويڪٽر<صحيح> importSig(SigSpec sig);
	صحيح import_Sig_ٻٽو_انگ(Sig_ٻٽو_انگ ٻٽو_انگ);

	// Imports the input cones of all previously importSig'd signals into
	// the SAT solver.
	فراغ prepare();

	// Returns the "complexity level" of a given cell.
	ساكن صحيح cell_complexity(RTLIL::Cell *cell);
};

// A deterministic effort budget for SAT-based optimizations, measured in solver
// propagation steps (see ezSAT::getSolverProps). The budget is held once per unit
// of work (typically a module) and reused across many QuickConeSat instances: a
// pass that rebuilds the solver in batches keeps a single SatEffortBudget and
// spends from it in every batch, rather than resetting the budget per solver.
// Spending the budget only skips optimizations, and it never affects correctness,
// because a skipped proof just leaves a candidate un-optimized.
بنية SatEffortBudget {
	تعداد class نتيجو_2 { Sat, Unsat, حد_بندي_Reached };

	// Effort charged per imported cell
	ساكن constexpr int64_t import_cell_لاڳت = 100;

	// starting budget (0 = unlimited)
	int64_t ڪل = 0;
	int64_t بچيل = 0;

	SatEffortBudget() {}
	explicit SatEffortBudget(int64_t ڪل) : ڪل(ڪل), بچيل(ڪل) {}

	bool enabled() ثابت { أرجع ڪل > 0; }

	// True once the budget is used up
	bool spent() ثابت { أرجع enabled() && بچيل <= 0; }

	// Charge for the cells imported into qcsat since the previous call (pricing the cells pulled in)
	// cells_charged is how many of qcsat's imported cells are already paid for, returns the new count
	int64_t charge_import(تيز_Cone_Sat &qcsat, int64_t cells_charged);

	نتيجو_2 solve(تيز_Cone_Sat &qcsat, int64_t cap, ثابت std::ويڪٽر<صحيح> &modelExprs,
			std::ويڪٽر<bool> &modelVals, ثابت std::ويڪٽر<صحيح> &assumptions);

	// Assumptions-only query without a model
	نتيجو_2 solve(تيز_Cone_Sat &qcsat, int64_t cap, ثابت std::ويڪٽر<صحيح> &assumptions);
};

YOSYS_NAMESPACE_پڇاڙي

#جيڪڏهن_پورو
