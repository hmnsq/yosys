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

#અન્તર્ભાવ "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<પૂર્ણાંક> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	માટે (auto bit : sig)
		bits_queue.ઉમેરો(bit);
	પાછા satgen.importSigSpec(sig);
}

પૂર્ણાંક QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.ઉમેરો(bit);
	પાછા satgen.importSigBit(bit);
}

શૂન્ય QuickConeSat::prepare()
{
	જ્યારે (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		માટે (auto bit : bits_queue)
			જો (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.ગણતરી(bit.wire))
			{
				std::vector<પૂર્ણાંક> bits = satgen.importSigSpec(bit.wire);
				માટે (પૂર્ણાંક i : bits)
				માટે (પૂર્ણાંક j : bits)
					જો (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ઉમેરો(bit.wire);
			}

		bits_queue.clear();

		માટે (auto &pbit : portbits)
		{
			જો (imported_cells.ગણતરી(pbit.cell))
				ચાલુ;
			જો (cell_complexity(pbit.cell) > max_cell_complexity)
				ચાલુ;
			જો (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				ચાલુ;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.ઉમેરો(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.ઉમેરો(pbit.cell);
		}

		જો (max_cell_ગણતરી && GetSize(imported_cells) > max_cell_ગણતરી)
			તોડો;
	}
}

પૂર્ણાંક QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	જો (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		પાછા 0;
	જો (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		પાછા 1;
	જો (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		પાછા 2;
	જો (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		પાછા 3;
	જો (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		પાછા 4;
	// Unknown cell.
	પાછા 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	જો (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	પાછા GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, અચલ std::vector<પૂર્ણાંક> &assumptions)
{
	std::vector<તાર્કિક> modelVals;
	પાછા solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, અચલ std::vector<પૂર્ણાંક> &modelExprs,
		std::vector<તાર્કિક> &modelVals, અચલ std::vector<પૂર્ણાંક> &assumptions)
{
	જો (spent())
		પાછા Result::LimitReached;
	જો (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	તાર્કિક sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	જો (enabled())
		remaining -= qcsat.ez->getSolverProps();
	જો (!sat && qcsat.ez->getSolverPropLimitStatus())
		પાછા Result::LimitReached;
	પાછા sat ? Result::Sat : Result::Unsat;
}
