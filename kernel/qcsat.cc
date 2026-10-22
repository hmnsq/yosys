/*
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

#ಅನ್ತರ್ಭಾವ "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಶೀಘ್ರ_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : sig)
		ಬಿಟ್ಟುಗಳು_ಸರತಿ.ಸೇರಿಸು(ಬಿಟ್);
	ಹಿಂತಿರುಗಿ satgen.importSigSpec(sig);
}

ಪೂರ್ಣಾಂಕ ಶೀಘ್ರ_Cone_Sat::import_Sig_ಬಿಟ್(Sig_ಬಿಟ್ ಬಿಟ್)
{
	ಬಿಟ್ = modwalker.sigmap(ಬಿಟ್);
	ಬಿಟ್ಟುಗಳು_ಸರತಿ.ಸೇರಿಸು(ಬಿಟ್);
	ಹಿಂತಿರುಗಿ satgen.import_Sig_ಬಿಟ್(ಬಿಟ್);
}

ಶೂನ್ಯ ಶೀಘ್ರ_Cone_Sat::prepare()
{
	ಆಗ (!ಬಿಟ್ಟುಗಳು_ಸರತಿ.ಖಾಲಿಗಳು())
	{
		pool<ModWalker::ಪೋರ್ಟ್_ಬಿಟ್_2> portbits;
		modwalker.get_drivers(portbits, ಬಿಟ್ಟುಗಳು_ಸರತಿ);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಬಿಟ್ : ಬಿಟ್ಟುಗಳು_ಸರತಿ)
			ಆದರೆ (ಬಿಟ್.wire && ಬಿಟ್.wire->get_bool_attribute(ID::onehot) && !imported_onehot.ಎಣಿಕೆ_2(ಬಿಟ್.wire))
			{
				std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> ಬಿಟ್ಟುಗಳು = satgen.importSigSpec(ಬಿಟ್.wire);
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i : ಬಿಟ್ಟುಗಳು)
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j : ಬಿಟ್ಟುಗಳು)
					ಆದರೆ (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ಸೇರಿಸು(ಬಿಟ್.wire);
			}

		ಬಿಟ್ಟುಗಳು_ಸರತಿ.clear();

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &pbit : portbits)
		{
			ಆದರೆ (imported_cells.ಎಣಿಕೆ_2(pbit.cell))
				ಮುಂದುವರಿ;
			ಆದರೆ (cell_complexity(pbit.cell) > max_cell_complexity)
				ಮುಂದುವರಿ;
			ಆದರೆ (max_cell_outs && Get_ಗಾತ್ರ(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				ಮುಂದುವರಿ;
			ಸ್ವಯಂಚಾಲಿತ &inputs = modwalker.cell_inputs[pbit.cell];
			ಬಿಟ್ಟುಗಳು_ಸರತಿ.ಸೇರಿಸು(inputs.begin(), inputs.ಕೊನೆ());
			satgen.importCell(pbit.cell);
			imported_cells.ಸೇರಿಸು(pbit.cell);
		}

		ಆದರೆ (max_cell_ಎಣಿಕೆ && Get_ಗಾತ್ರ(imported_cells) > max_cell_ಎಣಿಕೆ)
			ನಿಲ್ಲಿಸು;
	}
}

ಪೂರ್ಣಾಂಕ ಶೀಘ್ರ_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	ಆದರೆ (cell->type.ಒಳ(ID($concat), ID($ತುಂಡು), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		ಹಿಂತಿರುಗಿ 0;
	ಆದರೆ (cell->type.ಒಳ(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		ಹಿಂತಿರುಗಿ 1;
	ಆದರೆ (cell->type.ಒಳ(ID($neg), ID($ಕೂಡಿಸು), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		ಹಿಂತಿರುಗಿ 2;
	ಆದರೆ (cell->type.ಒಳ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ಸರಿಸು), ID($shiftx)))
		ಹಿಂತಿರುಗಿ 3;
	ಆದರೆ (cell->type.ಒಳ(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ಘಾತ)))
		ಹಿಂತಿರುಗಿ 4;
	// Unknown cell.
	ಹಿಂತಿರುಗಿ 5;
}

int64_t SatEffortBudget::charge_import(ಶೀಘ್ರ_Cone_Sat &qcsat, int64_t cells_charged)
{
	ಆದರೆ (enabled())
		ಉಳಿದ -= (Get_ಗಾತ್ರ(qcsat.imported_cells) - cells_charged) * import_cell_ವೆಚ್ಚ;
	ಹಿಂತಿರುಗಿ Get_ಗಾತ್ರ(qcsat.imported_cells);
}

SatEffortBudget::ಫಲಿತಾಂಶ_2 SatEffortBudget::solve(ಶೀಘ್ರ_Cone_Sat &qcsat, int64_t cap, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &assumptions)
{
	std::ಸದಿಶ<ತಾರ್ಕಿಕ> modelVals;
	ಹಿಂತಿರುಗಿ solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::ಫಲಿತಾಂಶ_2 SatEffortBudget::solve(ಶೀಘ್ರ_Cone_Sat &qcsat, int64_t cap, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &modelExprs,
		std::ಸದಿಶ<ತಾರ್ಕಿಕ> &modelVals, ಸ್ಥಿರಾಂಕ std::ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &assumptions)
{
	ಆದರೆ (spent())
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::ಎಲ್ಲೆ_Reached;
	ಆದರೆ (enabled())
		cap = (cap > 0) ? std::min(cap, ಉಳಿದ) : ಉಳಿದ;
	qcsat.ez->ಗಣ_Solver_Prop_ಎಲ್ಲೆ(cap);
	ತಾರ್ಕಿಕ sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	ಆದರೆ (enabled())
		ಉಳಿದ -= qcsat.ez->getSolverProps();
	ಆದರೆ (!sat && qcsat.ez->get_Solver_Prop_ಎಲ್ಲೆ_ಸ್ಥಿತಿ())
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::ಎಲ್ಲೆ_Reached;
	ಹಿಂತಿರುಗಿ sat ? ಫಲಿತಾಂಶ_2::Sat : ಫಲಿತಾಂಶ_2::Unsat;
}
