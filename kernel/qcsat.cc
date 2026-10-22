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

std::ویکټور<صحيح> چټک_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	لكل (خپلکاری بټ : sig)
		بټونه_کتار.ورننباسه(بټ);
	أرجع satgen.importSigSpec(sig);
}

صحيح چټک_Cone_Sat::import_Sig_بټ(Sig_بټ بټ)
{
	بټ = modwalker.sigmap(بټ);
	بټونه_کتار.ورننباسه(بټ);
	أرجع satgen.import_Sig_بټ(بټ);
}

فراغ چټک_Cone_Sat::prepare()
{
	طالما (!بټونه_کتار.تشه())
	{
		pool<ModWalker::پورټ_بټ_2> portbits;
		modwalker.get_drivers(portbits, بټونه_کتار);

		لكل (خپلکاری بټ : بټونه_کتار)
			إذا (بټ.wire && بټ.wire->get_bool_attribute(ID::onehot) && !imported_onehot.شمېر(بټ.wire))
			{
				std::ویکټور<صحيح> بټونه = satgen.importSigSpec(بټ.wire);
				لكل (صحيح i : بټونه)
				لكل (صحيح j : بټونه)
					إذا (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ورننباسه(بټ.wire);
			}

		بټونه_کتار.clear();

		لكل (خپلکاری &pbit : portbits)
		{
			إذا (imported_cells.شمېر(pbit.cell))
				استمر;
			إذا (cell_complexity(pbit.cell) > max_cell_complexity)
				استمر;
			إذا (max_cell_outs && Get_اندازه(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				استمر;
			خپلکاری &inputs = modwalker.cell_inputs[pbit.cell];
			بټونه_کتار.ورننباسه(inputs.begin(), inputs.پای_ټکی());
			satgen.importCell(pbit.cell);
			imported_cells.ورننباسه(pbit.cell);
		}

		إذا (max_cell_شمېر && Get_اندازه(imported_cells) > max_cell_شمېر)
			اكسر;
	}
}

صحيح چټک_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	إذا (cell->type.دننه(ID($concat), ID($ټوټه), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		أرجع 0;
	إذا (cell->type.دننه(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
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
	إذا (cell->type.دننه(ID($neg), ID($جمع_کړه), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		أرجع 2;
	إذا (cell->type.دننه(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ښويدنه), ID($shiftx)))
		أرجع 3;
	إذا (cell->type.دننه(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($توان)))
		أرجع 4;
	// Unknown cell.
	أرجع 5;
}

int64_t SatEffortBudget::charge_import(چټک_Cone_Sat &qcsat, int64_t cells_charged)
{
	إذا (enabled())
		پاتې_شوی -= (Get_اندازه(qcsat.imported_cells) - cells_charged) * import_cell_لګښت;
	أرجع Get_اندازه(qcsat.imported_cells);
}

SatEffortBudget::پایله_2 SatEffortBudget::solve(چټک_Cone_Sat &qcsat, int64_t cap, ثابت std::ویکټور<صحيح> &assumptions)
{
	std::ویکټور<منطقي> modelVals;
	أرجع solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::پایله_2 SatEffortBudget::solve(چټک_Cone_Sat &qcsat, int64_t cap, ثابت std::ویکټور<صحيح> &modelExprs,
		std::ویکټور<منطقي> &modelVals, ثابت std::ویکټور<صحيح> &assumptions)
{
	إذا (spent())
		أرجع پایله_2::حد_Reached;
	إذا (enabled())
		cap = (cap > 0) ? std::min(cap, پاتې_شوی) : پاتې_شوی;
	qcsat.ez->سېټ_Solver_Prop_حد(cap);
	منطقي sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	إذا (enabled())
		پاتې_شوی -= qcsat.ez->getSolverProps();
	إذا (!sat && qcsat.ez->get_Solver_Prop_حد_حالت())
		أرجع پایله_2::حد_Reached;
	أرجع sat ? پایله_2::Sat : پایله_2::Unsat;
}
