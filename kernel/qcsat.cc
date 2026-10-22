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

#ضمن "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<صحيح> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	لكل (auto bit : sig)
		bits_queue.أدرج(bit);
	أرجع satgen.importSigSpec(sig);
}

صحيح QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.أدرج(bit);
	أرجع satgen.importSigBit(bit);
}

فراغ QuickConeSat::prepare()
{
	طالما (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		لكل (auto bit : bits_queue)
			إذا (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.عدد(bit.wire))
			{
				std::vector<صحيح> bits = satgen.importSigSpec(bit.wire);
				لكل (صحيح i : bits)
				لكل (صحيح j : bits)
					إذا (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.أدرج(bit.wire);
			}

		bits_queue.clear();

		لكل (auto &pbit : portbits)
		{
			إذا (imported_cells.عدد(pbit.cell))
				استمر;
			إذا (cell_complexity(pbit.cell) > max_cell_complexity)
				استمر;
			إذا (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				استمر;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.أدرج(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.أدرج(pbit.cell);
		}

		إذا (max_cell_عدد && GetSize(imported_cells) > max_cell_عدد)
			اكسر;
	}
}

صحيح QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	إذا (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		أرجع 0;
	إذا (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		أرجع 1;
	إذا (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		أرجع 2;
	إذا (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		أرجع 3;
	إذا (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		أرجع 4;
	// Unknown cell.
	أرجع 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	إذا (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	أرجع GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ثابت std::vector<صحيح> &assumptions)
{
	std::vector<منطقي> modelVals;
	أرجع solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ثابت std::vector<صحيح> &modelExprs,
		std::vector<منطقي> &modelVals, ثابت std::vector<صحيح> &assumptions)
{
	إذا (spent())
		أرجع Result::LimitReached;
	إذا (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	منطقي sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	إذا (enabled())
		remaining -= qcsat.ez->getSolverProps();
	إذا (!sat && qcsat.ez->getSolverPropLimitStatus())
		أرجع Result::LimitReached;
	أرجع sat ? Result::Sat : Result::Unsat;
}
