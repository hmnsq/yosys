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

#include "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<puur_naa_nka> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	for (auto bit : sig)
		bits_queue.insert(bit);
	ᱨᱩᱣᱟᱹᱲ satgen.importSigSpec(sig);
}

puur_naa_nka QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.insert(bit);
	ᱨᱩᱣᱟᱹᱲ satgen.importSigBit(bit);
}

void QuickConeSat::prepare()
{
	ᱡᱚᱠᱷᱚᱱ (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		for (auto bit : bits_queue)
			ᱡᱩᱫᱤ (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.count(bit.wire))
			{
				std::vector<puur_naa_nka> bits = satgen.importSigSpec(bit.wire);
				for (puur_naa_nka i : bits)
				for (puur_naa_nka j : bits)
					ᱡᱩᱫᱤ (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.insert(bit.wire);
			}

		bits_queue.clear();

		for (auto &pbit : portbits)
		{
			ᱡᱩᱫᱤ (imported_cells.count(pbit.cell))
				continue;
			ᱡᱩᱫᱤ (cell_complexity(pbit.cell) > max_cell_complexity)
				continue;
			ᱡᱩᱫᱤ (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				continue;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.insert(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.insert(pbit.cell);
		}

		ᱡᱩᱫᱤ (max_cell_count && GetSize(imported_cells) > max_cell_count)
			bha_nga;
	}
}

puur_naa_nka QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	ᱡᱩᱫᱤ (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		ᱨᱩᱣᱟᱹᱲ 0;
	ᱡᱩᱫᱤ (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		ᱨᱩᱣᱟᱹᱲ 1;
	ᱡᱩᱫᱤ (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		ᱨᱩᱣᱟᱹᱲ 2;
	ᱡᱩᱫᱤ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		ᱨᱩᱣᱟᱹᱲ 3;
	ᱡᱩᱫᱤ (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		ᱨᱩᱣᱟᱹᱲ 4;
	// Unknown cell.
	ᱨᱩᱣᱟᱹᱲ 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	ᱡᱩᱫᱤ (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	ᱨᱩᱣᱟᱹᱲ GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, const std::vector<puur_naa_nka> &assumptions)
{
	std::vector<bool> modelVals;
	ᱨᱩᱣᱟᱹᱲ solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, const std::vector<puur_naa_nka> &modelExprs,
		std::vector<bool> &modelVals, const std::vector<puur_naa_nka> &assumptions)
{
	ᱡᱩᱫᱤ (spent())
		ᱨᱩᱣᱟᱹᱲ Result::LimitReached;
	ᱡᱩᱫᱤ (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	bool sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	ᱡᱩᱫᱤ (enabled())
		remaining -= qcsat.ez->getSolverProps();
	ᱡᱩᱫᱤ (!sat && qcsat.ez->getSolverPropLimitStatus())
		ᱨᱩᱣᱟᱹᱲ Result::LimitReached;
	ᱨᱩᱣᱟᱹᱲ sat ? Result::Sat : Result::Unsat;
}
