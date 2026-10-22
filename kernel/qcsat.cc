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

#அந்தர்பாவ "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<முழு> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	க்கு (auto bit : sig)
		bits_queue.செருகு(bit);
	திரும்பு satgen.importSigSpec(sig);
}

முழு QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.செருகு(bit);
	திரும்பு satgen.importSigBit(bit);
}

வெறுமை QuickConeSat::prepare()
{
	போது (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		க்கு (auto bit : bits_queue)
			எனில் (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.மொத்தம்(bit.wire))
			{
				std::vector<முழு> bits = satgen.importSigSpec(bit.wire);
				க்கு (முழு i : bits)
				க்கு (முழு j : bits)
					எனில் (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.செருகு(bit.wire);
			}

		bits_queue.clear();

		க்கு (auto &pbit : portbits)
		{
			எனில் (imported_cells.மொத்தம்(pbit.cell))
				தொடர்;
			எனில் (cell_complexity(pbit.cell) > max_cell_complexity)
				தொடர்;
			எனில் (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				தொடர்;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.செருகு(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.செருகு(pbit.cell);
		}

		எனில் (max_cell_மொத்தம் && GetSize(imported_cells) > max_cell_மொத்தம்)
			நிறுத்து;
	}
}

முழு QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	எனில் (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		திரும்பு 0;
	எனில் (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		திரும்பு 1;
	எனில் (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		திரும்பு 2;
	எனில் (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		திரும்பு 3;
	எனில் (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		திரும்பு 4;
	// Unknown cell.
	திரும்பு 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	எனில் (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	திரும்பு GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, மாறிலி std::vector<முழு> &assumptions)
{
	std::vector<தருக்க> modelVals;
	திரும்பு solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, மாறிலி std::vector<முழு> &modelExprs,
		std::vector<தருக்க> &modelVals, மாறிலி std::vector<முழு> &assumptions)
{
	எனில் (spent())
		திரும்பு Result::LimitReached;
	எனில் (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	தருக்க sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	எனில் (enabled())
		remaining -= qcsat.ez->getSolverProps();
	எனில் (!sat && qcsat.ez->getSolverPropLimitStatus())
		திரும்பு Result::LimitReached;
	திரும்பு sat ? Result::Sat : Result::Unsat;
}
