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

std::سمتیہ<صحیح> تیز_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	برائے (خودکار ثنائی_ہندسہ : sig)
		بٹ_قطار.داخل_کرو(ثنائی_ہندسہ);
	واپس satgen.importSigSpec(sig);
}

صحیح تیز_Cone_Sat::import_Sig_ثنائی_ہندسہ(Sig_ثنائی_ہندسہ ثنائی_ہندسہ)
{
	ثنائی_ہندسہ = modwalker.sigmap(ثنائی_ہندسہ);
	بٹ_قطار.داخل_کرو(ثنائی_ہندسہ);
	واپس satgen.import_Sig_ثنائی_ہندسہ(ثنائی_ہندسہ);
}

خالی تیز_Cone_Sat::prepare()
{
	جب (!بٹ_قطار.خالی_خانے())
	{
		pool<ModWalker::پورٹ_ثنائی_ہندسہ_2> portbits;
		modwalker.get_drivers(portbits, بٹ_قطار);

		برائے (خودکار ثنائی_ہندسہ : بٹ_قطار)
			اگر (ثنائی_ہندسہ.wire && ثنائی_ہندسہ.wire->get_bool_attribute(ID::onehot) && !imported_onehot.گنتی(ثنائی_ہندسہ.wire))
			{
				std::سمتیہ<صحیح> بٹ = satgen.importSigSpec(ثنائی_ہندسہ.wire);
				برائے (صحیح i : بٹ)
				برائے (صحیح j : بٹ)
					اگر (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.داخل_کرو(ثنائی_ہندسہ.wire);
			}

		بٹ_قطار.clear();

		برائے (خودکار &pbit : portbits)
		{
			اگر (imported_cells.گنتی(pbit.cell))
				جاری;
			اگر (cell_complexity(pbit.cell) > max_cell_complexity)
				جاری;
			اگر (max_cell_outs && Get_سائز(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				جاری;
			خودکار &inputs = modwalker.cell_inputs[pbit.cell];
			بٹ_قطار.داخل_کرو(inputs.begin(), inputs.انتہا());
			satgen.importCell(pbit.cell);
			imported_cells.داخل_کرو(pbit.cell);
		}

		اگر (max_cell_گنتی && Get_سائز(imported_cells) > max_cell_گنتی)
			توڑو;
	}
}

صحیح تیز_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	اگر (cell->type.اندر(ID($concat), ID($قاش), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		واپس 0;
	اگر (cell->type.اندر(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		واپس 1;
	اگر (cell->type.اندر(ID($neg), ID($جوڑو), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		واپس 2;
	اگر (cell->type.اندر(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($سرکاؤ), ID($shiftx)))
		واپس 3;
	اگر (cell->type.اندر(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($قوت)))
		واپس 4;
	// Unknown cell.
	واپس 5;
}

int64_t SatEffortBudget::charge_import(تیز_Cone_Sat &qcsat, int64_t cells_charged)
{
	اگر (enabled())
		بقایا -= (Get_سائز(qcsat.imported_cells) - cells_charged) * import_cell_لاگت;
	واپس Get_سائز(qcsat.imported_cells);
}

SatEffortBudget::نتیجہ_2 SatEffortBudget::solve(تیز_Cone_Sat &qcsat, int64_t cap, ثابت std::سمتیہ<صحیح> &assumptions)
{
	std::سمتیہ<منطقی> modelVals;
	واپس solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::نتیجہ_2 SatEffortBudget::solve(تیز_Cone_Sat &qcsat, int64_t cap, ثابت std::سمتیہ<صحیح> &modelExprs,
		std::سمتیہ<منطقی> &modelVals, ثابت std::سمتیہ<صحیح> &assumptions)
{
	اگر (spent())
		واپس نتیجہ_2::حد_بندی_Reached;
	اگر (enabled())
		cap = (cap > 0) ? std::min(cap, بقایا) : بقایا;
	qcsat.ez->سیٹ_Solver_Prop_حد_بندی(cap);
	منطقی sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	اگر (enabled())
		بقایا -= qcsat.ez->getSolverProps();
	اگر (!sat && qcsat.ez->get_Solver_Prop_حد_بندی_حالت())
		واپس نتیجہ_2::حد_بندی_Reached;
	واپس sat ? نتیجہ_2::Sat : نتیجہ_2::Unsat;
}
