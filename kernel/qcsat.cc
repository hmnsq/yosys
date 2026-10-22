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

#ଅନ୍ତର୍ଭାଵ "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	ପାଇଁ (auto bit : sig)
		bits_queue.ଭର୍ତ୍ତି(bit);
	ଫେରନ୍ତୁ satgen.importSigSpec(sig);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.ଭର୍ତ୍ତି(bit);
	ଫେରନ୍ତୁ satgen.importSigBit(bit);
}

ଶୂନ୍ୟ QuickConeSat::prepare()
{
	ଯେତେବେଳେ (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		ପାଇଁ (auto bit : bits_queue)
			ଯଦି (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.ଗଣନା_2(bit.wire))
			{
				std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> bits = satgen.importSigSpec(bit.wire);
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i : bits)
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j : bits)
					ଯଦି (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ଭର୍ତ୍ତି(bit.wire);
			}

		bits_queue.clear();

		ପାଇଁ (auto &pbit : portbits)
		{
			ଯଦି (imported_cells.ଗଣନା_2(pbit.cell))
				ଜାରି;
			ଯଦି (cell_complexity(pbit.cell) > max_cell_complexity)
				ଜାରି;
			ଯଦି (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				ଜାରି;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.ଭର୍ତ୍ତି(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.ଭର୍ତ୍ତି(pbit.cell);
		}

		ଯଦି (max_cell_ଗଣନା && GetSize(imported_cells) > max_cell_ଗଣନା)
			ଭାଙ୍ଗ;
	}
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	ଯଦି (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		ଫେରନ୍ତୁ 0;
	ଯଦି (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		ଫେରନ୍ତୁ 1;
	ଯଦି (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		ଫେରନ୍ତୁ 2;
	ଯଦି (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		ଫେରନ୍ତୁ 3;
	ଯଦି (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		ଫେରନ୍ତୁ 4;
	// Unknown cell.
	ଫେରନ୍ତୁ 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	ଯଦି (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	ଫେରନ୍ତୁ GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &assumptions)
{
	std::vector<ତାର୍କିକ> modelVals;
	ଫେରନ୍ତୁ solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &modelExprs,
		std::vector<ତାର୍କିକ> &modelVals, ଅଚଳ std::vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &assumptions)
{
	ଯଦି (spent())
		ଫେରନ୍ତୁ Result::LimitReached;
	ଯଦି (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	ତାର୍କିକ sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	ଯଦି (enabled())
		remaining -= qcsat.ez->getSolverProps();
	ଯଦି (!sat && qcsat.ez->getSolverPropLimitStatus())
		ଫେରନ୍ତୁ Result::LimitReached;
	ଫେରନ୍ତୁ sat ? Result::Sat : Result::Unsat;
}
