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

std::وکتور<صحيح> سریع_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	لكل (خودکار بیت : sig)
		بیتها_صف.درج(بیت);
	أرجع satgen.importSigSpec(sig);
}

صحيح سریع_Cone_Sat::import_Sig_بیت(Sig_بیت بیت)
{
	بیت = modwalker.sigmap(بیت);
	بیتها_صف.درج(بیت);
	أرجع satgen.import_Sig_بیت(بیت);
}

فراغ سریع_Cone_Sat::prepare()
{
	طالما (!بیتها_صف.خالی())
	{
		pool<ModWalker::پورت_بیت_2> portbits;
		modwalker.get_drivers(portbits, بیتها_صف);

		لكل (خودکار بیت : بیتها_صف)
			إذا (بیت.wire && بیت.wire->get_bool_attribute(ID::onehot) && !imported_onehot.شمار(بیت.wire))
			{
				std::وکتور<صحيح> بیتها = satgen.importSigSpec(بیت.wire);
				لكل (صحيح i : بیتها)
				لكل (صحيح j : بیتها)
					إذا (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.درج(بیت.wire);
			}

		بیتها_صف.clear();

		لكل (خودکار &pbit : portbits)
		{
			إذا (imported_cells.شمار(pbit.cell))
				استمر;
			إذا (cell_complexity(pbit.cell) > max_cell_complexity)
				استمر;
			إذا (max_cell_outs && Get_اندازه(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				استمر;
			خودکار &inputs = modwalker.cell_inputs[pbit.cell];
			بیتها_صف.درج(inputs.begin(), inputs.انتها());
			satgen.importCell(pbit.cell);
			imported_cells.درج(pbit.cell);
		}

		إذا (max_cell_شمار && Get_اندازه(imported_cells) > max_cell_شمار)
			اكسر;
	}
}

صحيح سریع_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	إذا (cell->type.درون(ID($concat), ID($برش), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		أرجع 0;
	إذا (cell->type.درون(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
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
	إذا (cell->type.درون(ID($neg), ID($جمع_کن), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		أرجع 2;
	إذا (cell->type.درون(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($لغزش), ID($shiftx)))
		أرجع 3;
	إذا (cell->type.درون(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان)))
		أرجع 4;
	// Unknown cell.
	أرجع 5;
}

int64_t SatEffortBudget::charge_import(سریع_Cone_Sat &qcsat, int64_t cells_charged)
{
	إذا (enabled())
		مانده -= (Get_اندازه(qcsat.imported_cells) - cells_charged) * import_cell_هزینه;
	أرجع Get_اندازه(qcsat.imported_cells);
}

SatEffortBudget::نتیجه_2 SatEffortBudget::solve(سریع_Cone_Sat &qcsat, int64_t cap, ثابت std::وکتور<صحيح> &assumptions)
{
	std::وکتور<منطقي> modelVals;
	أرجع solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::نتیجه_2 SatEffortBudget::solve(سریع_Cone_Sat &qcsat, int64_t cap, ثابت std::وکتور<صحيح> &modelExprs,
		std::وکتور<منطقي> &modelVals, ثابت std::وکتور<صحيح> &assumptions)
{
	إذا (spent())
		أرجع نتیجه_2::حد_Reached;
	إذا (enabled())
		cap = (cap > 0) ? std::min(cap, مانده) : مانده;
	qcsat.ez->مجموعه_Solver_Prop_حد(cap);
	منطقي sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	إذا (enabled())
		مانده -= qcsat.ez->getSolverProps();
	إذا (!sat && qcsat.ez->get_Solver_Prop_حد_وضعیت())
		أرجع نتیجه_2::حد_Reached;
	أرجع sat ? نتیجه_2::Sat : نتیجه_2::Unsat;
}
