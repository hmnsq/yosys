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

std::सदिशम्<पूर्णाङ्क> शीघ्र_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	प्रति (स्वचालित bit : sig)
		द्वयङ्काः_queue.निवेशय(bit);
	प्रत्यागम satgen.importSigSpec(sig);
}

पूर्णाङ्क शीघ्र_Cone_Sat::importSigBit(SigBit bit)
{
	bit = modwalker.sigmap(bit);
	द्वयङ्काः_queue.निवेशय(bit);
	प्रत्यागम satgen.importSigBit(bit);
}

शून्य शीघ्र_Cone_Sat::prepare()
{
	यावत् (!द्वयङ्काः_queue.रिक्तानि())
	{
		pool<ModWalker::पत्तनम्_Bit> portbits;
		modwalker.get_drivers(portbits, द्वयङ्काः_queue);

		प्रति (स्वचालित bit : द्वयङ्काः_queue)
			यदि (bit.wire && bit.wire->get_bool_attribute(ID::onehot) && !imported_onehot.गणनम्(bit.wire))
			{
				std::सदिशम्<पूर्णाङ्क> द्वयङ्काः = satgen.importSigSpec(bit.wire);
				प्रति (पूर्णाङ्क i : द्वयङ्काः)
				प्रति (पूर्णाङ्क j : द्वयङ्काः)
					यदि (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.निवेशय(bit.wire);
			}

		द्वयङ्काः_queue.clear();

		प्रति (स्वचालित &pbit : portbits)
		{
			यदि (imported_cells.गणनम्(pbit.cell))
				अनुवर्त;
			यदि (cell_complexity(pbit.cell) > max_cell_complexity)
				अनुवर्त;
			यदि (max_cell_outs && Get_आकार(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				अनुवर्त;
			स्वचालित &inputs = modwalker.cell_inputs[pbit.cell];
			द्वयङ्काः_queue.निवेशय(inputs.begin(), inputs.अन्तः());
			satgen.importCell(pbit.cell);
			imported_cells.निवेशय(pbit.cell);
		}

		यदि (max_cell_गणनम् && Get_आकार(imported_cells) > max_cell_गणनम्)
			भङ्ग;
	}
}

पूर्णाङ्क शीघ्र_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	यदि (cell->type.अन्तर्(ID($concat), ID($खण्डिका), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		प्रत्यागम 0;
	यदि (cell->type.अन्तर्(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		प्रत्यागम 1;
	यदि (cell->type.अन्तर्(ID($neg), ID($सङ्कलय), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		प्रत्यागम 2;
	यदि (cell->type.अन्तर्(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($सरणम्), ID($shiftx)))
		प्रत्यागम 3;
	यदि (cell->type.अन्तर्(ID($mul), ID($macc), ID($भाजय), ID($mod), ID($divfloor), ID($modfloor), ID($घातः)))
		प्रत्यागम 4;
	// Unknown cell.
	प्रत्यागम 5;
}

int64_t SatEffortBudget::charge_import(शीघ्र_Cone_Sat &qcsat, int64_t cells_charged)
{
	यदि (enabled())
		remaining -= (Get_आकार(qcsat.imported_cells) - cells_charged) * import_cell_cost;
	प्रत्यागम Get_आकार(qcsat.imported_cells);
}

SatEffortBudget::परिणाम_2 SatEffortBudget::solve(शीघ्र_Cone_Sat &qcsat, int64_t cap, ध्रुव std::सदिशम्<पूर्णाङ्क> &assumptions)
{
	std::सदिशम्<तार्किक> modelVals;
	प्रत्यागम solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::परिणाम_2 SatEffortBudget::solve(शीघ्र_Cone_Sat &qcsat, int64_t cap, ध्रुव std::सदिशम्<पूर्णाङ्क> &modelExprs,
		std::सदिशम्<तार्किक> &modelVals, ध्रुव std::सदिशम्<पूर्णाङ्क> &assumptions)
{
	यदि (spent())
		प्रत्यागम परिणाम_2::सीमा_Reached;
	यदि (enabled())
		cap = (cap > 0) ? std::min(cap, remaining) : remaining;
	qcsat.ez->समुच्चयः_Solver_Prop_सीमा(cap);
	तार्किक sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	यदि (enabled())
		remaining -= qcsat.ez->getSolverProps();
	यदि (!sat && qcsat.ez->get_Solver_Prop_सीमा_स्थितिः())
		प्रत्यागम परिणाम_2::सीमा_Reached;
	प्रत्यागम sat ? परिणाम_2::Sat : परिणाम_2::Unsat;
}
