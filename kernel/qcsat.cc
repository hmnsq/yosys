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

std::vector<ಪೂರ್ಣಾಙ್ಕ> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	ಪ್ರತಿ (auto bit : sig)
		bits_queue.ಸೇರಿಸು(bit);
	ಪ್ರತ್ಯಾಗಮ satgen.importSigSpec(sig);
}

ಪೂರ್ಣಾಙ್ಕ QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.ಸೇರಿಸು(bit);
	ಪ್ರತ್ಯಾಗಮ satgen.importSigBit(bit);
}

ಶೂನ್ಯ QuickConeSat::prepare()
{
	ಯಾವತ್ (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		ಪ್ರತಿ (auto bit : bits_queue)
			ಯದಿ (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.ಎಣಿಕೆ(bit.wire))
			{
				std::vector<ಪೂರ್ಣಾಙ್ಕ> bits = satgen.importSigSpec(bit.wire);
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i : bits)
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j : bits)
					ಯದಿ (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ಸೇರಿಸು(bit.wire);
			}

		bits_queue.clear();

		ಪ್ರತಿ (auto &pbit : portbits)
		{
			ಯದಿ (imported_cells.ಎಣಿಕೆ(pbit.cell))
				ಅನುವರ್ತ;
			ಯದಿ (cell_complexity(pbit.cell) > max_cell_complexity)
				ಅನುವರ್ತ;
			ಯದಿ (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				ಅನುವರ್ತ;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.ಸೇರಿಸು(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.ಸೇರಿಸು(pbit.cell);
		}

		ಯದಿ (max_cell_ಎಣಿಕೆ && GetSize(imported_cells) > max_cell_ಎಣಿಕೆ)
			ಭಙ್ಗ;
	}
}

ಪೂರ್ಣಾಙ್ಕ QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	ಯದಿ (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		ಪ್ರತ್ಯಾಗಮ 0;
	ಯದಿ (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		ಪ್ರತ್ಯಾಗಮ 1;
	ಯದಿ (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		ಪ್ರತ್ಯಾಗಮ 2;
	ಯದಿ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		ಪ್ರತ್ಯಾಗಮ 3;
	ಯದಿ (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		ಪ್ರತ್ಯಾಗಮ 4;
	// Unknown cell.
	ಪ್ರತ್ಯಾಗಮ 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	ಯದಿ (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	ಪ್ರತ್ಯಾಗಮ GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &assumptions)
{
	std::vector<ತಾರ್ಕಿಕ> modelVals;
	ಪ್ರತ್ಯಾಗಮ solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &modelExprs,
		std::vector<ತಾರ್ಕಿಕ> &modelVals, ಧ್ರುವ std::vector<ಪೂರ್ಣಾಙ್ಕ> &assumptions)
{
	ಯದಿ (spent())
		ಪ್ರತ್ಯಾಗಮ Result::LimitReached;
	ಯದಿ (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	ತಾರ್ಕಿಕ sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	ಯದಿ (enabled())
		remaining -= qcsat.ez->getSolverProps();
	ಯದಿ (!sat && qcsat.ez->getSolverPropLimitStatus())
		ಪ್ರತ್ಯಾಗಮ Result::LimitReached;
	ಪ್ರತ್ಯಾಗಮ sat ? Result::Sat : Result::Unsat;
}
