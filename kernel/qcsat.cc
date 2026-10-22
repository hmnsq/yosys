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

#समावेश "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<पूर्णांक> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	क्रम (स्वतः bit : sig)
		bits_queue.जोड़ो(bit);
	वापस satgen.importSigSpec(sig);
}

पूर्णांक QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.जोड़ो(bit);
	वापस satgen.importSigBit(bit);
}

व्योम QuickConeSat::prepare()
{
	जबतक (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		क्रम (स्वतः bit : bits_queue)
			अगर (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.गिनती(bit.wire))
			{
				std::vector<पूर्णांक> bits = satgen.importSigSpec(bit.wire);
				क्रम (पूर्णांक i : bits)
				क्रम (पूर्णांक j : bits)
					अगर (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.जोड़ो(bit.wire);
			}

		bits_queue.सफा();

		क्रम (स्वतः &pbit : portbits)
		{
			अगर (imported_cells.गिनती(pbit.cell))
				जारी;
			अगर (cell_complexity(pbit.cell) > max_cell_complexity)
				जारी;
			अगर (max_cell_outs && Get_आकार(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				जारी;
			स्वतः &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.जोड़ो(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.जोड़ो(pbit.cell);
		}

		अगर (max_cell_गिनती && Get_आकार(imported_cells) > max_cell_गिनती)
			अवरोध;
	}
}

पूर्णांक QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	अगर (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		वापस 0;
	अगर (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		वापस 1;
	अगर (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		वापस 2;
	अगर (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		वापस 3;
	अगर (cell->type.in(ID($mul), ID($macc), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात)))
		वापस 4;
	// Unknown cell.
	वापस 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	अगर (enabled())
		remaining -= (Get_आकार(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	वापस Get_आकार(qcsat.imported_cells);
}

SatEffortBudget::परिणाम_2 SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, स्थिर std::vector<पूर्णांक> &assumptions)
{
	std::vector<bool> modelVals;
	वापस solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::परिणाम_2 SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, स्थिर std::vector<पूर्णांक> &modelExprs,
		std::vector<bool> &modelVals, स्थिर std::vector<पूर्णांक> &assumptions)
{
	अगर (spent())
		वापस परिणाम_2::LimitReached;
	अगर (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	bool sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	अगर (enabled())
		remaining -= qcsat.ez->getSolverProps();
	अगर (!sat && qcsat.ez->getSolverPropLimitStatus())
		वापस परिणाम_2::LimitReached;
	वापस sat ? परिणाम_2::Sat : परिणाम_2::Unsat;
}
