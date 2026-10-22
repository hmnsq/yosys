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
	लागि (स्वतः bit : sig)
		bits_queue.घुसाउ(bit);
	फर्काऊ satgen.importSigSpec(sig);
}

पूर्णाङ्क QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.घुसाउ(bit);
	फर्काऊ satgen.importSigBit(bit);
}

शून्य QuickConeSat::prepare()
{
	जबसम्म (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		लागि (स्वतः bit : bits_queue)
			यदि (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.गन्ती(bit.wire))
			{
				std::vector<पूर्णाङ्क> bits = satgen.importSigSpec(bit.wire);
				लागि (पूर्णाङ्क i : bits)
				लागि (पूर्णाङ्क j : bits)
					यदि (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.घुसाउ(bit.wire);
			}

		bits_queue.clear();

		लागि (स्वतः &pbit : portbits)
		{
			यदि (imported_cells.गन्ती(pbit.cell))
				जारी;
			यदि (cell_complexity(pbit.cell) > max_cell_complexity)
				जारी;
			यदि (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				जारी;
			स्वतः &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.घुसाउ(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.घुसाउ(pbit.cell);
		}

		यदि (max_cell_गन्ती && GetSize(imported_cells) > max_cell_गन्ती)
			रोक;
	}
}

पूर्णाङ्क QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	यदि (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		फर्काऊ 0;
	यदि (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		फर्काऊ 1;
	यदि (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		फर्काऊ 2;
	यदि (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		फर्काऊ 3;
	यदि (cell->type.in(ID($mul), ID($macc), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात)))
		फर्काऊ 4;
	// Unknown cell.
	फर्काऊ 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	यदि (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	फर्काऊ GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, अचर std::vector<पूर्णाङ्क> &assumptions)
{
	std::vector<तार्किक> modelVals;
	फर्काऊ solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, अचर std::vector<पूर्णाङ्क> &modelExprs,
		std::vector<तार्किक> &modelVals, अचर std::vector<पूर्णाङ्क> &assumptions)
{
	यदि (spent())
		फर्काऊ Result::LimitReached;
	यदि (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	तार्किक sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	यदि (enabled())
		remaining -= qcsat.ez->getSolverProps();
	यदि (!sat && qcsat.ez->getSolverPropLimitStatus())
		फर्काऊ Result::LimitReached;
	फर्काऊ sat ? Result::Sat : Result::Unsat;
}
