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

std::ويڪٽر<صحيح> تيز_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	لكل (خودڪار ٻٽو_انگ : sig)
		بٽ_قطار.وجهو(ٻٽو_انگ);
	أرجع satgen.importSigSpec(sig);
}

صحيح تيز_Cone_Sat::import_Sig_ٻٽو_انگ(Sig_ٻٽو_انگ ٻٽو_انگ)
{
	ٻٽو_انگ = modwalker.sigmap(ٻٽو_انگ);
	بٽ_قطار.وجهو(ٻٽو_انگ);
	أرجع satgen.import_Sig_ٻٽو_انگ(ٻٽو_انگ);
}

فراغ تيز_Cone_Sat::prepare()
{
	طالما (!بٽ_قطار.خالي_جايون())
	{
		pool<ModWalker::پورٽ_ٻٽو_انگ_2> portbits;
		modwalker.get_drivers(portbits, بٽ_قطار);

		لكل (خودڪار ٻٽو_انگ : بٽ_قطار)
			إذا (ٻٽو_انگ.wire && ٻٽو_انگ.wire->get_bool_attribute(ID::onehot) && !imported_onehot.ڳڻپ(ٻٽو_انگ.wire))
			{
				std::ويڪٽر<صحيح> بٽ = satgen.importSigSpec(ٻٽو_انگ.wire);
				لكل (صحيح i : بٽ)
				لكل (صحيح j : بٽ)
					إذا (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.وجهو(ٻٽو_انگ.wire);
			}

		بٽ_قطار.clear();

		لكل (خودڪار &pbit : portbits)
		{
			إذا (imported_cells.ڳڻپ(pbit.cell))
				استمر;
			إذا (cell_complexity(pbit.cell) > max_cell_complexity)
				استمر;
			إذا (max_cell_outs && Get_سائيز(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				استمر;
			خودڪار &inputs = modwalker.cell_inputs[pbit.cell];
			بٽ_قطار.وجهو(inputs.begin(), inputs.پڇاڙي());
			satgen.importCell(pbit.cell);
			imported_cells.وجهو(pbit.cell);
		}

		إذا (max_cell_ڳڻپ && Get_سائيز(imported_cells) > max_cell_ڳڻپ)
			اكسر;
	}
}

صحيح تيز_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	إذا (cell->type.اندريون(ID($concat), ID($ٽڪرو), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		أرجع 0;
	إذا (cell->type.اندريون(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
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
	إذا (cell->type.اندريون(ID($neg), ID($جوڙيو), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		أرجع 2;
	إذا (cell->type.اندريون(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($سرڪايو), ID($shiftx)))
		أرجع 3;
	إذا (cell->type.اندريون(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت)))
		أرجع 4;
	// Unknown cell.
	أرجع 5;
}

int64_t SatEffortBudget::charge_import(تيز_Cone_Sat &qcsat, int64_t cells_charged)
{
	إذا (enabled())
		بچيل -= (Get_سائيز(qcsat.imported_cells) - cells_charged) * import_cell_لاڳت;
	أرجع Get_سائيز(qcsat.imported_cells);
}

SatEffortBudget::نتيجو_2 SatEffortBudget::solve(تيز_Cone_Sat &qcsat, int64_t cap, ثابت std::ويڪٽر<صحيح> &assumptions)
{
	std::ويڪٽر<منطقي> modelVals;
	أرجع solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::نتيجو_2 SatEffortBudget::solve(تيز_Cone_Sat &qcsat, int64_t cap, ثابت std::ويڪٽر<صحيح> &modelExprs,
		std::ويڪٽر<منطقي> &modelVals, ثابت std::ويڪٽر<صحيح> &assumptions)
{
	إذا (spent())
		أرجع نتيجو_2::حد_بندي_Reached;
	إذا (enabled())
		cap = (cap > 0) ? std::min(cap, بچيل) : بچيل;
	qcsat.ez->سيٽ_Solver_Prop_حد_بندي(cap);
	منطقي sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	إذا (enabled())
		بچيل -= qcsat.ez->getSolverProps();
	إذا (!sat && qcsat.ez->get_Solver_Prop_حد_بندي_حالت())
		أرجع نتيجو_2::حد_بندي_Reached;
	أرجع sat ? نتيجو_2::Sat : نتيجو_2::Unsat;
}
