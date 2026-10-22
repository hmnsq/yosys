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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ QCSAT_H
#ವ್ಯಾಖ್ಯಾನಿಸು QCSAT_H

#ಅನ್ತರ್ಭಾವ "kernel/satgen.h"
#ಅನ್ತರ್ಭಾವ "kernel/modtools.h"

YOSYS_NAMESPACE_BEGIN

// This is a helper class meant for easy construction of quick SAT queries
// to a combinatorial input cone of some set of signals, meant for SAT-based
// optimizations.  Various knobs are provided to set just how much of the
// cone should be included in the model — since this class is meant for
// optimization, it should not be a correctness problem when some cells are
// skipped and the solver spuriously returns SAT with a solution that
// cannot exist in reality due to skipped constraints (ie. only UNSAT results
// from this class should be considered binding).
ರಚನೆ ಶೀಘ್ರ_Cone_Sat {
	ModWalker &modwalker;
	ezSatPtr ez;
	SatGen satgen;

	// The effort level knobs.

	// The maximum "complexity level" of cells that will be imported.
	// - 1: bitwise operations, muxes, equality comparisons, lut, sop, fa
	// - 2: addition, subtraction, greater/less than comparisons, lcu
	// - 3: shifts
	// - 4: multiplication, division, power
	ಪೂರ್ಣಾಂಕ max_cell_complexity = 2;
	// The maximum number of cells to import, or 0 for no limit.
	ಪೂರ್ಣಾಂಕ max_cell_ಎಣಿಕೆ = 0;
	// If non-0, skip importing cells with more than this number of output bits.
	ಪೂರ್ಣಾಂಕ max_cell_outs = 0;

	// Internal state.
	pool<RTLIL::Cell*> imported_cells;
	pool<RTLIL::Wire*> imported_onehot;
	pool<RTLIL::Sig_ಬಿಟ್> ಬಿಟ್ಟುಗಳು_ಸರತಿ;

	ಶೀಘ್ರ_Cone_Sat(ModWalker &modwalker) : modwalker(modwalker), ez(), satgen(ez.get(), &modwalker.sigmap) {}

	// Imports a signal into the SAT solver, queues its input cone to be
	// imported in the next prepare() call.
	std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> importSig(SigSpec sig);
	ಪೂರ್ಣಾಂಕ import_Sig_ಬಿಟ್(Sig_ಬಿಟ್ ಬಿಟ್);

	// Imports the input cones of all previously importSig'd signals into
	// the SAT solver.
	ಶೂನ್ಯ prepare();

	// Returns the "complexity level" of a given cell.
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cell_complexity(RTLIL::Cell *cell);
};

// A deterministic effort budget for SAT-based optimizations, measured in solver
// propagation steps (see ezSAT::getSolverProps). The budget is held once per unit
// of work (typically a module) and reused across many QuickConeSat instances: a
// pass that rebuilds the solver in batches keeps a single SatEffortBudget and
// spends from it in every batch, rather than resetting the budget per solver.
// Spending the budget only skips optimizations, and it never affects correctness,
// because a skipped proof just leaves a candidate un-optimized.
ರಚನೆ SatEffortBudget {
	ಎಣಿಕೆ class ಫಲಿತಾಂಶ_2 { Sat, Unsat, ಎಲ್ಲೆ_Reached };

	// Effort charged per imported cell
	ಸ್ಥಿರ constexpr int64_t import_cell_ವೆಚ್ಚ = 100;

	// starting budget (0 = unlimited)
	int64_t ಒಟ್ಟು = 0;
	int64_t ಉಳಿದ = 0;

	SatEffortBudget() {}
	explicit SatEffortBudget(int64_t ಒಟ್ಟು) : ಒಟ್ಟು(ಒಟ್ಟು), ಉಳಿದ(ಒಟ್ಟು) {}

	bool enabled() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ ಒಟ್ಟು > 0; }

	// True once the budget is used up
	bool spent() ಸ್ಥಿರಾಂಕ { ಹಿಂತಿರುಗಿ enabled() && ಉಳಿದ <= 0; }

	// Charge for the cells imported into qcsat since the previous call (pricing the cells pulled in)
	// cells_charged is how many of qcsat's imported cells are already paid for, returns the new count
	int64_t charge_import(ಶೀಘ್ರ_Cone_Sat &qcsat, int64_t cells_charged);

	ಫಲಿತಾಂಶ_2 solve(ಶೀಘ್ರ_Cone_Sat &qcsat, int64_t cap, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &modelExprs,
			std::ಸದಿಶ<bool> &modelVals, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &assumptions);

	// Assumptions-only query without a model
	ಫಲಿತಾಂಶ_2 solve(ಶೀಘ್ರ_Cone_Sat &qcsat, int64_t cap, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &assumptions);
};

YOSYS_NAMESPACE_ಕೊನೆ

#ಆದರೆ_ಅಂತ್ಯ
