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

#അന്തര്ഭാവ "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::vector<പൂർണ്ണസംഖ്യ> QuickConeSat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	വേണ്ടി (auto bit : sig)
		bits_queue.തിരുകുക(bit);
	തിരികെ satgen.importSigSpec(sig);
}

പൂർണ്ണസംഖ്യ QuickConeSat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	bits_queue.തിരുകുക(bit);
	തിരികെ satgen.importSigBit(bit);
}

ശൂന്യം QuickConeSat::prepare()
{
	ആകുമ്പോൾ (!bits_queue.empty())
	{
		pool<ModWalker::PortBit> portbits;
		modwalker.get_drivers(portbits, bits_queue);

		വേണ്ടി (auto bit : bits_queue)
			എങ്കിൽ (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.എണ്ണം(bit.wire))
			{
				std::vector<പൂർണ്ണസംഖ്യ> bits = satgen.importSigSpec(bit.wire);
				വേണ്ടി (പൂർണ്ണസംഖ്യ i : bits)
				വേണ്ടി (പൂർണ്ണസംഖ്യ j : bits)
					എങ്കിൽ (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.തിരുകുക(bit.wire);
			}

		bits_queue.clear();

		വേണ്ടി (auto &pbit : portbits)
		{
			എങ്കിൽ (imported_cells.എണ്ണം(pbit.cell))
				തുടരുക;
			എങ്കിൽ (cell_complexity(pbit.cell) > max_cell_complexity)
				തുടരുക;
			എങ്കിൽ (max_cell_outs && GetSize(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				തുടരുക;
			auto &inputs = modwalker.cell_inputs[pbit.cell];
			bits_queue.തിരുകുക(inputs.begin(), inputs.end());
			satgen.importCell(pbit.cell);
			imported_cells.തിരുകുക(pbit.cell);
		}

		എങ്കിൽ (max_cell_എണ്ണം && GetSize(imported_cells) > max_cell_എണ്ണം)
			നിർത്തുക;
	}
}

പൂർണ്ണസംഖ്യ QuickConeSat::cell_complexity(RTLIL::Cell *cell)
{
	എങ്കിൽ (cell->type.in(ID($concat), ID($slice), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		തിരികെ 0;
	എങ്കിൽ (cell->type.in(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		തിരികെ 1;
	എങ്കിൽ (cell->type.in(ID($neg), ID($add), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		തിരികെ 2;
	എങ്കിൽ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx)))
		തിരികെ 3;
	എങ്കിൽ (cell->type.in(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow)))
		തിരികെ 4;
	// Unknown cell.
	തിരികെ 5;
}

int64_t SatEffortBudget::charge_import(QuickConeSat &qcsat, int64_t cells_charged)
{
	എങ്കിൽ (enabled())
		remaining -= (GetSize(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	തിരികെ GetSize(qcsat.imported_cells);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &assumptions)
{
	std::vector<തർക്കം> modelVals;
	തിരികെ solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::Result SatEffortBudget::solve(QuickConeSat &qcsat, int64_t cap, സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &modelExprs,
		std::vector<തർക്കം> &modelVals, സ്ഥിരം std::vector<പൂർണ്ണസംഖ്യ> &assumptions)
{
	എങ്കിൽ (spent())
		തിരികെ Result::LimitReached;
	എങ്കിൽ (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->setSolverPropLimit(cap);
	തർക്കം sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	എങ്കിൽ (enabled())
		remaining -= qcsat.ez->getSolverProps();
	എങ്കിൽ (!sat && qcsat.ez->getSolverPropLimitStatus())
		തിരികെ Result::LimitReached;
	തിരികെ sat ? Result::Sat : Result::Unsat;
}
