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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/qcsat.h"

USING_YOSYS_NAMESPACE

std::ਸਦਿਸ਼<ਪੂਰਨ> ਤੇਜ਼_Cone_Sat::importSig(SigSpec sig)
{
	sig = modwalker.sigmap(sig);
	ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : sig)
		ਬਿੱਟ_ਕਤਾਰ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਵਾਪਸ satgen.importSigSpec(sig);
}

ਪੂਰਨ ਤੇਜ਼_Cone_Sat::import_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(Sig_ਦੋਆਧਾਰੀ_ਅੰਕ ਦੋਆਧਾਰੀ_ਅੰਕ)
{
	ਦੋਆਧਾਰੀ_ਅੰਕ = modwalker.sigmap(ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਬਿੱਟ_ਕਤਾਰ.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ);
	ਵਾਪਸ satgen.import_Sig_ਦੋਆਧਾਰੀ_ਅੰਕ(ਦੋਆਧਾਰੀ_ਅੰਕ);
}

ਖਾਲੀ ਤੇਜ਼_Cone_Sat::prepare()
{
	ਜਦੋਂ (!ਬਿੱਟ_ਕਤਾਰ.ਖਾਲੀ_ਥਾਵਾਂ())
	{
		pool<ModWalker::ਪੋਰਟ_ਦੋਆਧਾਰੀ_ਅੰਕ_2> portbits;
		modwalker.get_drivers(portbits, ਬਿੱਟ_ਕਤਾਰ);

		ਲਈ (ਸਵੈਚਾਲਿਤ ਦੋਆਧਾਰੀ_ਅੰਕ : ਬਿੱਟ_ਕਤਾਰ)
			ਜੇ (ਦੋਆਧਾਰੀ_ਅੰਕ.wire && ਦੋਆਧਾਰੀ_ਅੰਕ.wire->get_bool_attribute(ID::onehot) && !imported_onehot.ਗਿਣਤੀ_2(ਦੋਆਧਾਰੀ_ਅੰਕ.wire))
			{
				std::ਸਦਿਸ਼<ਪੂਰਨ> ਬਿੱਟ = satgen.importSigSpec(ਦੋਆਧਾਰੀ_ਅੰਕ.wire);
				ਲਈ (ਪੂਰਨ i : ਬਿੱਟ)
				ਲਈ (ਪੂਰਨ j : ਬਿੱਟ)
					ਜੇ (i != j)
						ez->assume(ez->NOT(i), j);
				imported_onehot.ਪਾਓ(ਦੋਆਧਾਰੀ_ਅੰਕ.wire);
			}

		ਬਿੱਟ_ਕਤਾਰ.clear();

		ਲਈ (ਸਵੈਚਾਲਿਤ &pbit : portbits)
		{
			ਜੇ (imported_cells.ਗਿਣਤੀ_2(pbit.cell))
				ਜਾਰੀ;
			ਜੇ (cell_complexity(pbit.cell) > max_cell_complexity)
				ਜਾਰੀ;
			ਜੇ (max_cell_outs && Get_ਮਾਪ(modwalker.cell_outputs[pbit.cell]) > max_cell_outs)
				ਜਾਰੀ;
			ਸਵੈਚਾਲਿਤ &inputs = modwalker.cell_inputs[pbit.cell];
			ਬਿੱਟ_ਕਤਾਰ.ਪਾਓ(inputs.begin(), inputs.ਸਿਰਾ());
			satgen.importCell(pbit.cell);
			imported_cells.ਪਾਓ(pbit.cell);
		}

		ਜੇ (max_cell_ਗਿਣਤੀ && Get_ਮਾਪ(imported_cells) > max_cell_ਗਿਣਤੀ)
			ਰੋਕੋ;
	}
}

ਪੂਰਨ ਤੇਜ਼_Cone_Sat::cell_complexity(RTLIL::Cell *cell)
{
	ਜੇ (cell->type.ਵਿੱਚ(ID($concat), ID($ਟੁਕੜਾ), ID($pos), ID($buf), ID($barrier), ID($_BUF_)))
		ਵਾਪਸ 0;
	ਜੇ (cell->type.ਵਿੱਚ(ID($not), ID($and), ID($or), ID($xor), ID($xnor),
			ID($reduce_and), ID($reduce_or), ID($reduce_xor),
			ID($reduce_xnor), ID($reduce_bool),
			ID($logic_not), ID($logic_and), ID($logic_or),
			ID($eq), ID($ne), ID($eqx), ID($nex), ID($fa),
			ID($mux), ID($pmux), ID($bmux), ID($demux), ID($lut), ID($sop),
			ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_),
			ID($_MUX_), ID($_NMUX_), ID($_MUX4_), ID($_MUX8_), ID($_MUX16_),
			ID($_AOI3_), ID($_OAI3_), ID($_AOI4_), ID($_OAI4_)))
		ਵਾਪਸ 1;
	ਜੇ (cell->type.ਵਿੱਚ(ID($neg), ID($ਜੋੜੋ), ID($sub), ID($alu), ID($lcu),
			ID($lt), ID($le), ID($gt), ID($ge)))
		ਵਾਪਸ 2;
	ਜੇ (cell->type.ਵਿੱਚ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ਖਿਸਕਾਓ), ID($shiftx)))
		ਵਾਪਸ 3;
	ਜੇ (cell->type.ਵਿੱਚ(ID($mul), ID($macc), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ਘਾਤ)))
		ਵਾਪਸ 4;
	// Unknown cell.
	ਵਾਪਸ 5;
}

int64_t SatEffortBudget::charge_import(ਤੇਜ਼_Cone_Sat &qcsat, int64_t cells_charged)
{
	ਜੇ (enabled())
		ਬਚਿਆ -= (Get_ਮਾਪ(qcsat.imported_cells) - cells_charged) * import_cell_ਲਾਗਤ;
	ਵਾਪਸ Get_ਮਾਪ(qcsat.imported_cells);
}

SatEffortBudget::ਨਤੀਜਾ_2 SatEffortBudget::solve(ਤੇਜ਼_Cone_Sat &qcsat, int64_t cap, ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &assumptions)
{
	std::ਸਦਿਸ਼<ਤਾਰਕਿਕ> modelVals;
	ਵਾਪਸ solve(qcsat, cap, {}, modelVals, assumptions);
}

SatEffortBudget::ਨਤੀਜਾ_2 SatEffortBudget::solve(ਤੇਜ਼_Cone_Sat &qcsat, int64_t cap, ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &modelExprs,
		std::ਸਦਿਸ਼<ਤਾਰਕਿਕ> &modelVals, ਅਚਲ std::ਸਦਿਸ਼<ਪੂਰਨ> &assumptions)
{
	ਜੇ (spent())
		ਵਾਪਸ ਨਤੀਜਾ_2::ਹੱਦਬੰਦੀ_Reached;
	ਜੇ (enabled())
		cap = (cap > 0) ? std::min(cap, ਬਚਿਆ) : ਬਚਿਆ;
	qcsat.ez->ਸੈੱਟ_Solver_Prop_ਹੱਦਬੰਦੀ(cap);
	ਤਾਰਕਿਕ sat = qcsat.ez->solve(modelExprs, modelVals, assumptions);
	ਜੇ (enabled())
		ਬਚਿਆ -= qcsat.ez->getSolverProps();
	ਜੇ (!sat && qcsat.ez->get_Solver_Prop_ਹੱਦਬੰਦੀ_ਸਥਿਤੀ())
		ਵਾਪਸ ਨਤੀਜਾ_2::ਹੱਦਬੰਦੀ_Reached;
	ਵਾਪਸ sat ? ਨਤੀਜਾ_2::Sat : ਨਤੀਜਾ_2::Unsat;
}
