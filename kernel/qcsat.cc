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

std::vector<पूर्णांक> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	साठी (स्वतः bit : sig)
		bits_queue.घाला(bit);
	परत satgen.importSigSpec(sig);
}

पूर्णांक QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.घाला(bit);
	परत satgen.importSigBit(bit);
}

शून्य QuickConeSat::prepare()
{
	पर्यंत (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		साठी (स्वतः bit : bits_queue)
			जर (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.मोजणी(bit.wire))
			{
				std::vector<पूर्णांक> bits = satgen.importSigSpec(bit.wire);
				साठी (पूर्णांक i : bits)
				साठी (पूर्णांक j : bits)
					जर (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.घाला(bit.wire);
			}

		bits_queue.clear();

		साठी (स्वतः &pbit : portbits)
		{
			जर (imported_cells.मोजणी(pbit.cell))
				सुरूठेवा;
			जर (cell_complexity(pbit.cell) > max_cell_complexity)
				सुरूठेवा;
			जर (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				सुरूठेवा;
			स्वतः &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.घाला(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.घाला(pbit.cell);
		}

		जर (max_cell_मोजणी && GetSize(imported_cells) > max_cell_मोजणी)
			थांबा;
	}
}

पूर्णांक QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	जर (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		परत 0;
	जर (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		परत 1;
	जर (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		परत 2;
	जर (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		परत 3;
	जर (cell->type.in(ID($mul), ID($macc), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात)))
		परत 4;
	// Unknown cell.
	परत 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	जर (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	परत GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, स्थिरांक std::vector<पूर्णांक> &assumptions)
{
	std::vector<तार्किक> modelVals;
	परत solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, स्थिरांक std::vector<पूर्णांक> &modelExprs,
		std::vector<तार्किक> &modelVals, स्थिरांक std::vector<पूर्णांक> &assumptions)
{
	जर (spent())
		परत Result::LimitReached;
	जर (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	तार्किक sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	जर (enabled())
		remaining -= qcsat.ez->getSolverProps();
	जर (!sat && qcsat.ez->getSolverPropLimitStatus())
		परत Result::LimitReached;
	परत sat ? Result::Sat : Result::Unsat;
}
