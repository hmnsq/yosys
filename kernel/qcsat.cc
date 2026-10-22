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

#অন্তর্ভাব "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<পূর্ণসংখ্যা> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	জন্য (auto bit : sig)
		bits_queue.ঢোকাও(bit);
	ফেরত satgen.importSigSpec(sig);
}

পূর্ণসংখ্যা QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.ঢোকাও(bit);
	ফেরত satgen.importSigBit(bit);
}

শূন্য QuickConeSat::prepare()
{
	যখন (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		জন্য (auto bit : bits_queue)
			যদি (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.গুনতি(bit.wire))
			{
				std::vector<পূর্ণসংখ্যা> bits = satgen.importSigSpec(bit.wire);
				জন্য (পূর্ণসংখ্যা i : bits)
				জন্য (পূর্ণসংখ্যা j : bits)
					যদি (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ঢোকাও(bit.wire);
			}

		bits_queue.clear();

		জন্য (auto &pbit : portbits)
		{
			যদি (imported_cells.গুনতি(pbit.cell))
				চালু;
			যদি (cell_complexity(pbit.cell) > max_cell_complexity)
				চালু;
			যদি (max_cell_outs && Get_আকার(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				চালু;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.ঢোকাও(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.ঢোকাও(pbit.cell);
		}

		যদি (max_cell_গুনতি && Get_আকার(imported_cells) > max_cell_গুনতি)
			বিরতি;
	}
}

পূর্ণসংখ্যা QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	যদি (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		ফেরত 0;
	যদি (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		ফেরত 1;
	যদি (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		ফেরত 2;
	যদি (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		ফেরত 3;
	যদি (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		ফেরত 4;
	// Unknown cell.
	ফেরত 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	যদি (enabled())
		remaining -= (Get_আকার(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	ফেরত Get_আকার(qcsat.imported_cells);
}

SatEffortBudget::ফলাফল_2 SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ধ্রুবক std::vector<পূর্ণসংখ্যা> &assumptions)
{
	std::vector<যৌক্তিক> modelVals;
	ফেরত solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::ফলাফল_2 SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ধ্রুবক std::vector<পূর্ণসংখ্যা> &modelExprs,
		std::vector<যৌক্তিক> &modelVals, ধ্রুবক std::vector<পূর্ণসংখ্যা> &assumptions)
{
	যদি (spent())
		ফেরত ফলাফল_2::LimitReached;
	যদি (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	যৌক্তিক sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	যদি (enabled())
		remaining -= qcsat.ez->getSolverProps();
	যদি (!sat && qcsat.ez->getSolverPropLimitStatus())
		ফেরত ফলাফল_2::LimitReached;
	ফেরত sat ? ফলাফল_2::Sat : ফলাফল_2::Unsat;
}
