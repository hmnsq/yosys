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

#अन्तर्भाव "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<पूर्णाङ्क> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	अत्थाय (auto bit : sig)
		bits_queue.निवेशय(bit);
	पटिनिवत्त satgen.importSigSpec(sig);
}

पूर्णाङ्क QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.निवेशय(bit);
	पटिनिवत्त satgen.importSigBit(bit);
}

शून्य QuickConeSat::prepare()
{
	याव (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		अत्थाय (auto bit : bits_queue)
			सचे (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.गणनम्(bit.wire))
			{
				std::vector<पूर्णाङ्क> bits = satgen.importSigSpec(bit.wire);
				अत्थाय (पूर्णाङ्क i : bits)
				अत्थाय (पूर्णाङ्क j : bits)
					सचे (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.निवेशय(bit.wire);
			}

		bits_queue.clear();

		अत्थाय (auto &pbit : portbits)
		{
			सचे (imported_cells.गणनम्(pbit.cell))
				अनुवर्त;
			सचे (cell_complexity(pbit.cell) > max_cell_complexity)
				अनुवर्त;
			सचे (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				अनुवर्त;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.निवेशय(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.निवेशय(pbit.cell);
		}

		सचे (max_cell_गणनम् && GetSize(imported_cells) > max_cell_गणनम्)
			भङ्ग;
	}
}

पूर्णाङ्क QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	सचे (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		पटिनिवत्त 0;
	सचे (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		पटिनिवत्त 1;
	सचे (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		पटिनिवत्त 2;
	सचे (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		पटिनिवत्त 3;
	सचे (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		पटिनिवत्त 4;
	// Unknown cell.
	पटिनिवत्त 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	सचे (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	पटिनिवत्त GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ध्रुव std::vector<पूर्णाङ्क> &assumptions)
{
	std::vector<तार्किक> modelVals;
	पटिनिवत्त solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ध्रुव std::vector<पूर्णाङ्क> &modelExprs,
		std::vector<तार्किक> &modelVals, ध्रुव std::vector<पूर्णाङ्क> &assumptions)
{
	सचे (spent())
		पटिनिवत्त Result::LimitReached;
	सचे (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	तार्किक sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	सचे (enabled())
		remaining -= qcsat.ez->getSolverProps();
	सचे (!sat && qcsat.ez->getSolverPropLimitStatus())
		पटिनिवत्त Result::LimitReached;
	पटिनिवत्त sat ? Result::Sat : Result::Unsat;
}
