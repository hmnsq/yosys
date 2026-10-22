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
	प्रति (auto bit : sig)
		bits_queue.निवेशय(bit);
	प्रत्यागम satgen.importSigSpec(sig);
}

पूर्णाङ्क QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.निवेशय(bit);
	प्रत्यागम satgen.importSigBit(bit);
}

शून्य QuickConeSat::prepare()
{
	जाव (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		प्रति (auto bit : bits_queue)
			जइ (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.गणनम्(bit.wire))
			{
				std::vector<पूर्णाङ्क> bits = satgen.importSigSpec(bit.wire);
				प्रति (पूर्णाङ्क i : bits)
				प्रति (पूर्णाङ्क j : bits)
					जइ (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.निवेशय(bit.wire);
			}

		bits_queue.clear();

		प्रति (auto &pbit : portbits)
		{
			जइ (imported_cells.गणनम्(pbit.cell))
				अनुवर्त;
			जइ (cell_complexity(pbit.cell) > max_cell_complexity)
				अनुवर्त;
			जइ (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				अनुवर्त;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.निवेशय(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.निवेशय(pbit.cell);
		}

		जइ (max_cell_गणनम् && GetSize(imported_cells) > max_cell_गणनम्)
			भङ्ग;
	}
}

पूर्णाङ्क QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	जइ (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		प्रत्यागम 0;
	जइ (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		प्रत्यागम 1;
	जइ (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		प्रत्यागम 2;
	जइ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		प्रत्यागम 3;
	जइ (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		प्रत्यागम 4;
	// Unknown cell.
	प्रत्यागम 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	जइ (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	प्रत्यागम GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ध्रुव std::vector<पूर्णाङ्क> &assumptions)
{
	std::vector<तार्किक> modelVals;
	प्रत्यागम solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ध्रुव std::vector<पूर्णाङ्क> &modelExprs,
		std::vector<तार्किक> &modelVals, ध्रुव std::vector<पूर्णाङ्क> &assumptions)
{
	जइ (spent())
		प्रत्यागम Result::LimitReached;
	जइ (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	तार्किक sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	जइ (enabled())
		remaining -= qcsat.ez->getSolverProps();
	जइ (!sat && qcsat.ez->getSolverPropLimitStatus())
		प्रत्यागम Result::LimitReached;
	प्रत्यागम sat ? Result::Sat : Result::Unsat;
}
