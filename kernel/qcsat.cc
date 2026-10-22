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

std::سمتیہ<صحيح> تیز_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	لكل (خودکار ثنائی_ہندسہ : sig)
		بٹ_قطار.داخل_کرو(ثنائی_ہندسہ);
	أرجع satgen.importSigSpec(sig);
}

صحيح تیز_Cone_Sat::import_Sig_ثنائی_ہندسہ(Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
{
	ثنائی_ہندسہ = modwalker.sigmap(ثنائی_ہندسہ);
	بٹ_قطار.داخل_کرو(ثنائی_ہندسہ);
	أرجع satgen.import_Sig_ثنائی_ہندسہ(ثنائی_ہندسہ);
}

فراغ تیز_Cone_Sat::prepare()
{
	طالما (!بٹ_قطار.خالی_تھاواں())
	{
		pool<ModWalker::پورٹ_ثنائی_ہندسہ_2> portbits;
		modwalker.get_drivers(portbits, بٹ_قطار);

		لكل (خودکار ثنائی_ہندسہ : بٹ_قطار)
			إذا (ثنائی_ہندسہ.wire && ثنائی_ہندسہ.wire->get_bool_attribute(ID::onehot) && !imported_onehot.گنتی(ثنائی_ہندسہ.wire))
			{
				std::سمتیہ<صحيح> بٹ = satgen.importSigSpec(ثنائی_ہندسہ.wire);
				لكل (صحيح i : بٹ)
				لكل (صحيح j : بٹ)
					إذا (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.داخل_کرو(ثنائی_ہندسہ.wire);
			}

		بٹ_قطار.clear();

		لكل (خودکار &pbit : portbits)
		{
			إذا (imported_cells.گنتی(pbit.cell))
				استمر;
			إذا (cell_complexity(pbit.cell) > max_cell_complexity)
				استمر;
			إذا (max_cell_outs && Get_ناپ(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				استمر;
			خودکار &inputs = modwalker.cell_inputs[pbit.cell];
			بٹ_قطار.داخل_کرو(inputs.begin(), inputs.سرا());
			satgen.importCell(pbit.cell);
			imported_cells.داخل_کرو(pbit.cell);
		}

		إذا (max_cell_گنتی && Get_ناپ(imported_cells) > max_cell_گنتی)
			اكسر;
	}
}

صحيح تیز_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	إذا (cell->type.وچ(ID($concat), ID($ٹکڑا), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		أرجع 0;
	إذا (cell->type.وچ(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
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
	إذا (cell->type.وچ(ID($neg), ID($جوڑو), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		أرجع 2;
	إذا (cell->type.وچ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($کھسکاؤ), ID($shiftx)))
		أرجع 3;
	إذا (cell->type.وچ(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت)))
		أرجع 4;
	// Unknown cell.
	أرجع 5;
}

int64_t SatEffortBudget::charge_import(تیز_Cone_Sat &qcsat, int64_t cells_charged)
{
	إذا (enabled())
		بچیا -= (Get_ناپ(qcsat.imported_cells) - cells_charged) * import_cell_لاگت;
	أرجع Get_ناپ(qcsat.imported_cells);
}

SatEffortBudget::نتیجہ_2 SatEffortBudget::solve(تیز_Cone_Sat &qcsat, int64_t cap, ثابت std::سمتیہ<صحيح> &assumptions)
{
	std::سمتیہ<منطقي> modelVals;
	أرجع solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::نتیجہ_2 SatEffortBudget::solve(تیز_Cone_Sat &qcsat, int64_t cap, ثابت std::سمتیہ<صحيح> &modelExprs,
		std::سمتیہ<منطقي> &modelVals, ثابت std::سمتیہ<صحيح> &assumptions)
{
	إذا (spent())
		أرجع نتیجہ_2::حد_بندی_Reached;
	إذا (enabled())
		cap = (cap > 0) ? std::min(cap, بچیا) : بچیا;
	qcsat.ez->سیٹ_Solver_Prop_حد_بندی(cap);
	منطقي sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	إذا (enabled())
		بچیا -= qcsat.ez->getSolverProps();
	إذا (!sat && qcsat.ez->get_Solver_Prop_حد_بندی_حالت())
		أرجع نتیجہ_2::حد_بندی_Reached;
	أرجع sat ? نتیجہ_2::Sat : نتیجہ_2::Unsat;
}
