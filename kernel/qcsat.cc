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

#شامل "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<صحیح> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	برای (auto bit : sig)
		bits_queue.درج(bit);
	بازگشت satgen.importSigSpec(sig);
}

صحیح QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.درج(bit);
	بازگشت satgen.importSigBit(bit);
}

تهی QuickConeSat::prepare()
{
	وقتی (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		برای (auto bit : bits_queue)
			اگر (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.شمار(bit.wire))
			{
				std::vector<صحیح> bits = satgen.importSigSpec(bit.wire);
				برای (صحیح i : bits)
				برای (صحیح j : bits)
					اگر (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.درج(bit.wire);
			}

		bits_queue.clear();

		برای (auto &pbit : portbits)
		{
			اگر (imported_cells.شمار(pbit.cell))
				ادامه;
			اگر (cell_complexity(pbit.cell) > max_cell_complexity)
				ادامه;
			اگر (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				ادامه;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.درج(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.درج(pbit.cell);
		}

		اگر (max_cell_شمار && GetSize(imported_cells) > max_cell_شمار)
			بشکن;
	}
}

صحیح QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	اگر (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		بازگشت 0;
	اگر (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		بازگشت 1;
	اگر (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		بازگشت 2;
	اگر (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		بازگشت 3;
	اگر (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		بازگشت 4;
	// Unknown cell.
	بازگشت 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	اگر (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	بازگشت GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ثابت std::vector<صحیح> &assumptions)
{
	std::vector<منطقی> modelVals;
	بازگشت solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ثابت std::vector<صحیح> &modelExprs,
		std::vector<منطقی> &modelVals, ثابت std::vector<صحیح> &assumptions)
{
	اگر (spent())
		بازگشت Result::LimitReached;
	اگر (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	منطقی sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	اگر (enabled())
		remaining -= qcsat.ez->getSolverProps();
	اگر (!sat && qcsat.ez->getSolverPropLimitStatus())
		بازگشت Result::LimitReached;
	بازگشت sat ? Result::Sat : Result::Unsat;
}
