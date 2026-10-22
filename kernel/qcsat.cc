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

#অন্তর্ভাৱ "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<পূর্ণাঙ্ক> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	বাবে (auto bit : sig)
		bits_queue.ঢোকাও(bit);
	ঘূৰাই satgen.importSigSpec(sig);
}

পূর্ণাঙ্ক QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.ঢোকাও(bit);
	ঘূৰাই satgen.importSigBit(bit);
}

শূন্য QuickConeSat::prepare()
{
	যেতিয়ালৈ (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		বাবে (auto bit : bits_queue)
			যদি (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.গুনতি(bit.wire))
			{
				std::vector<পূর্ণাঙ্ক> bits = satgen.importSigSpec(bit.wire);
				বাবে (পূর্ণাঙ্ক i : bits)
				বাবে (পূর্ণাঙ্ক j : bits)
					যদি (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ঢোকাও(bit.wire);
			}

		bits_queue.clear();

		বাবে (auto &pbit : portbits)
		{
			যদি (imported_cells.গুনতি(pbit.cell))
				অনুৱর্ত;
			যদি (cell_complexity(pbit.cell) > max_cell_complexity)
				অনুৱর্ত;
			যদি (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				অনুৱর্ত;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.ঢোকাও(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.ঢোকাও(pbit.cell);
		}

		যদি (max_cell_গুনতি && GetSize(imported_cells) > max_cell_গুনতি)
			ভঙ্গ;
	}
}

পূর্ণাঙ্ক QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	যদি (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		ঘূৰাই 0;
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
		ঘূৰাই 1;
	যদি (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		ঘূৰাই 2;
	যদি (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		ঘূৰাই 3;
	যদি (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		ঘূৰাই 4;
	// Unknown cell.
	ঘূৰাই 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	যদি (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	ঘূৰাই GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ধ্রুৱ std::vector<পূর্ণাঙ্ক> &assumptions)
{
	std::vector<তার্কিক> modelVals;
	ঘূৰাই solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ধ্রুৱ std::vector<পূর্ণাঙ্ক> &modelExprs,
		std::vector<তার্কিক> &modelVals, ধ্রুৱ std::vector<পূর্ণাঙ্ক> &assumptions)
{
	যদি (spent())
		ঘূৰাই Result::LimitReached;
	যদি (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	তার্কিক sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	যদি (enabled())
		remaining -= qcsat.ez->getSolverProps();
	যদি (!sat && qcsat.ez->getSolverPropLimitStatus())
		ঘূৰাই Result::LimitReached;
	ঘূৰাই sat ? Result::Sat : Result::Unsat;
}
