/*
 *  yosys -- Yosys Open SYnthesis Suite
 *
 *  Copyright (C) 2012  Claire Xenia Wolf <claire@yosyshq.com>
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

#ಅನ್ತರ್ಭಾವ "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_ಗಂಟು::Aig_ಗಂಟು()
{
	portbit = -1;
	inverter = ಸುಳ್ಳು;
	ಎಡ_ಪೋಷಕ = -1;
	ಬಲ_ಪೋಷಕ = -1;
}

ತಾರ್ಕಿಕ Aig_ಗಂಟು::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Aig_ಗಂಟು &other) ಸ್ಥಿರಾಂಕ
{
	ಆದರೆ (portname != other.portname) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (portbit != other.portbit) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (inverter != other.inverter) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (ಎಡ_ಪೋಷಕ != other.ಎಡ_ಪೋಷಕ) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಆದರೆ (ಬಲ_ಪೋಷಕ != other.ಬಲ_ಪೋಷಕ) ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
	ಹಿಂತಿರುಗಿ ನಿಜ;
}

Hasher Aig_ಗಂಟು::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(ಎಡ_ಪೋಷಕ);
	h.eat(ಬಲ_ಪೋಷಕ);
	ಹಿಂತಿರುಗಿ h;
}

ತಾರ್ಕಿಕ Aig::ಕ್ರಿಯಾಕಾರಕ==(ಸ್ಥಿರಾಂಕ Aig &other) ಸ್ಥಿರಾಂಕ
{
	ಹಿಂತಿರುಗಿ name == other.name;
}

Hasher Aig::hash_into(Hasher h) ಸ್ಥಿರಾಂಕ
{
	h.eat(name);
	ಹಿಂತಿರುಗಿ h;
}

ರಚನೆ AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_ಗಂಟು> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	ಪೂರ್ಣಾಂಕ node2index(ಸ್ಥಿರಾಂಕ Aig_ಗಂಟು &ಗಂಟು)
	{
		ಆದರೆ (ಗಂಟು.ಎಡ_ಪೋಷಕ > ಗಂಟು.ಬಲ_ಪೋಷಕ) {
			Aig_ಗಂಟು n(ಗಂಟು);
			std::ಅದಲುಬದಲು(n.ಎಡ_ಪೋಷಕ, n.ಬಲ_ಪೋಷಕ);
			ಹಿಂತಿರುಗಿ node2index(n);
		}

		ಆದರೆ (!aig_indices.ಎಣಿಕೆ_2(ಗಂಟು)) {
			aig_indices.expect(ಗಂಟು, Get_ಗಾತ್ರ(aig->nodes));
			aig->nodes.ತಳ್ಳು_ಹಿಂಭಾಗ(ಗಂಟು);
		}

		ಹಿಂತಿರುಗಿ aig_indices.at(ಗಂಟು);
	}

	ಪೂರ್ಣಾಂಕ bool_ಗಂಟು(ತಾರ್ಕಿಕ ಮೌಲ್ಯ)
	{
		Aig_ಗಂಟು ಗಂಟು;
		ಗಂಟು.inverter = ಮೌಲ್ಯ;
		ಹಿಂತಿರುಗಿ node2index(ಗಂಟು);
	}

	ಪೂರ್ಣಾಂಕ inport(IdString portname, ಪೂರ್ಣಾಂಕ portbit = 0, ತಾರ್ಕಿಕ inverter = ಸುಳ್ಳು)
	{
		ಆದರೆ (portbit >= Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(portname))) {
			IdString signed_param = cell->twines().ಕಂಡುಹಿಡಿ(
				cell->twines().str(portname) + "_SIGNED");
		ಆದರೆ (signed_param != IdString::Null && cell->parameters.ಎಣಿಕೆ_2(signed_param) && cell->getParam(signed_param).as_bool())
				ಹಿಂತಿರುಗಿ inport(portname, Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(portname))-1, inverter);
			ಹಿಂತಿರುಗಿ bool_ಗಂಟು(inverter);
		}

		Aig_ಗಂಟು ಗಂಟು;
		ಗಂಟು.portname = portname;
		ಗಂಟು.portbit = portbit;
		ಗಂಟು.inverter = inverter;
		ಹಿಂತಿರುಗಿ node2index(ಗಂಟು);
	}

	ಸದಿಶ<ಪೂರ್ಣಾಂಕ> inport_vec(IdString portname, ಪೂರ್ಣಾಂಕ ಅಗಲ)
	{
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> vec;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
			vec.ತಳ್ಳು_ಹಿಂಭಾಗ(inport(portname, i));
		ಹಿಂತಿರುಗಿ vec;
	}

	ಪೂರ್ಣಾಂಕ not_inport(IdString portname, ಪೂರ್ಣಾಂಕ portbit = 0)
	{
		ಹಿಂತಿರುಗಿ inport(portname, portbit, ನಿಜ);
	}

	ಪೂರ್ಣಾಂಕ not_gate(ಪೂರ್ಣಾಂಕ A)
	{
		Aig_ಗಂಟು ಗಂಟು(aig_indices[A]);
		ಗಂಟು.outports.clear();
		ಗಂಟು.inverter = !ಗಂಟು.inverter;
		ಹಿಂತಿರುಗಿ node2index(ಗಂಟು);
	}

	ಪೂರ್ಣಾಂಕ and_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B, ತಾರ್ಕಿಕ inverter = ಸುಳ್ಳು)
	{
		ಆದರೆ (A == B)
			ಹಿಂತಿರುಗಿ inverter ? not_gate(A) : A;

		ಸ್ಥಿರಾಂಕ Aig_ಗಂಟು &nA = aig_indices[A];
		ಸ್ಥಿರಾಂಕ Aig_ಗಂಟು &nB = aig_indices[B];

		Aig_ಗಂಟು nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ಆದರೆ (nA == nB_inv)
			ಹಿಂತಿರುಗಿ bool_ಗಂಟು(inverter);

		ತಾರ್ಕಿಕ nA_bool = nA.portbit < 0 && nA.ಎಡ_ಪೋಷಕ < 0 && nA.ಬಲ_ಪೋಷಕ < 0;
		ತಾರ್ಕಿಕ nB_bool = nB.portbit < 0 && nB.ಎಡ_ಪೋಷಕ < 0 && nB.ಬಲ_ಪೋಷಕ < 0;

		ಆದರೆ (nA_bool && nB_bool) {
			ತಾರ್ಕಿಕ bA = nA.inverter;
			ತಾರ್ಕಿಕ bB = nB.inverter;
			ಹಿಂತಿರುಗಿ bool_ಗಂಟು(inverter != (bA && bB));
		}

		ಆದರೆ (nA_bool) {
			ತಾರ್ಕಿಕ bA = nA.inverter;
			ಆದರೆ (inverter)
				ಹಿಂತಿರುಗಿ bA ? not_gate(B) : bool_ಗಂಟು(ನಿಜ);
			ಹಿಂತಿರುಗಿ bA ? B : bool_ಗಂಟು(ಸುಳ್ಳು);
		}

		ಆದರೆ (nB_bool) {
			ತಾರ್ಕಿಕ bB = nB.inverter;
			ಆದರೆ (inverter)
				ಹಿಂತಿರುಗಿ bB ? not_gate(A) : bool_ಗಂಟು(ನಿಜ);
			ಹಿಂತಿರುಗಿ bB ? A : bool_ಗಂಟು(ಸುಳ್ಳು);
		}

		Aig_ಗಂಟು ಗಂಟು;
		ಗಂಟು.inverter = inverter;
		ಗಂಟು.ಎಡ_ಪೋಷಕ = A;
		ಗಂಟು.ಬಲ_ಪೋಷಕ = B;
		ಹಿಂತಿರುಗಿ node2index(ಗಂಟು);
	}

	ಪೂರ್ಣಾಂಕ nand_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B)
	{
		ಹಿಂತಿರುಗಿ and_gate(A, B, ನಿಜ);
	}

	ಪೂರ್ಣಾಂಕ or_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B)
	{
		ಪೂರ್ಣಾಂಕ not_a = not_gate(A);
		ಪೂರ್ಣಾಂಕ not_b = not_gate(B);
		ಹಿಂತಿರುಗಿ nand_gate(not_a, not_b);
	}

	ಪೂರ್ಣಾಂಕ nor_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B)
	{
		ಪೂರ್ಣಾಂಕ not_a = not_gate(A);
		ಪೂರ್ಣಾಂಕ not_b = not_gate(B);
		ಹಿಂತಿರುಗಿ and_gate(not_a, not_b);
	}

	ಪೂರ್ಣಾಂಕ xor_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B)
	{
		ಪೂರ್ಣಾಂಕ a_and_b = and_gate(A, B);
		ಪೂರ್ಣಾಂಕ a_nor_b = nor_gate(A, B);
		ಹಿಂತಿರುಗಿ nor_gate(a_and_b, a_nor_b);
	}

	ಪೂರ್ಣಾಂಕ xnor_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B)
	{
		ಪೂರ್ಣಾಂಕ a_and_b = and_gate(A, B);
		ಪೂರ್ಣಾಂಕ a_nor_b = nor_gate(A, B);
		ಹಿಂತಿರುಗಿ or_gate(a_and_b, a_nor_b);
	}

	ಪೂರ್ಣಾಂಕ andnot_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B)
	{
		ಪೂರ್ಣಾಂಕ not_b = not_gate(B);
		ಹಿಂತಿರುಗಿ and_gate(A, not_b);
	}

	ಪೂರ್ಣಾಂಕ ornot_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B)
	{
		ಪೂರ್ಣಾಂಕ not_b = not_gate(B);
		ಹಿಂತಿರುಗಿ or_gate(A, not_b);
	}

	ಪೂರ್ಣಾಂಕ mux_gate(ಪೂರ್ಣಾಂಕ A, ಪೂರ್ಣಾಂಕ B, ಪೂರ್ಣಾಂಕ S)
	{
		ಪೂರ್ಣಾಂಕ not_s = not_gate(S);
		ಪೂರ್ಣಾಂಕ a_ಸಕ್ರಿಯ = and_gate(A, not_s);
		ಪೂರ್ಣಾಂಕ b_ಸಕ್ರಿಯ = and_gate(B, S);
		ಹಿಂತಿರುಗಿ or_gate(a_ಸಕ್ರಿಯ, b_ಸಕ್ರಿಯ);
	}

	ಸದಿಶ<ಪೂರ್ಣಾಂಕ> adder(ಸ್ಥಿರಾಂಕ ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &A, ಸ್ಥಿರಾಂಕ ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &B, ಪೂರ್ಣಾಂಕ ವಾಹಕ_ಒಳ, ಸದಿಶ<ಪೂರ್ಣಾಂಕ> *X = ಶೂನ್ಯಸೂಚಕ, ಸದಿಶ<ಪೂರ್ಣಾಂಕ> *CO = ಶೂನ್ಯಸೂಚಕ)
	{
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> Y(Get_ಗಾತ್ರ(A));
		log_assert(Get_ಗಾತ್ರ(A) == Get_ಗಾತ್ರ(B));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(A); i++) {
			ಪೂರ್ಣಾಂಕ a_xor_b = xor_gate(A[i], B[i]);
			ಪೂರ್ಣಾಂಕ a_or_b = or_gate(A[i], B[i]);
			ಪೂರ್ಣಾಂಕ a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, ವಾಹಕ_ಒಳ);
			ಪೂರ್ಣಾಂಕ tmp = and_gate(a_or_b, ವಾಹಕ_ಒಳ);
			ಪೂರ್ಣಾಂಕ ವಾಹಕ_ಹೊರಹೋಗು = or_gate(a_and_b, tmp);
			ಆದರೆ (X != ಶೂನ್ಯಸೂಚಕ)
				X->at(i) = a_xor_b;
			ಆದರೆ (CO != ಶೂನ್ಯಸೂಚಕ)
				CO->at(i) = ವಾಹಕ_ಹೊರಹೋಗು;
			ವಾಹಕ_ಒಳ = ವಾಹಕ_ಹೊರಹೋಗು;
		}
		ಹಿಂತಿರುಗಿ Y;
	}

	ಶೂನ್ಯ outport(ಪೂರ್ಣಾಂಕ ಗಂಟು, IdString portname, ಪೂರ್ಣಾಂಕ portbit = 0)
	{
		ಆದರೆ (portbit < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(portname)))
			aig->nodes.at(ಗಂಟು).outports.ತಳ್ಳು_ಹಿಂಭಾಗ(pair<IdString, ಪೂರ್ಣಾಂಕ>(portname, portbit));
	}

	ಶೂನ್ಯ outport_bool(ಪೂರ್ಣಾಂಕ ಗಂಟು, IdString portname)
	{
		outport(ಗಂಟು, portname);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(portname)); i++)
			outport(bool_ಗಂಟು(ಸುಳ್ಳು), portname, i);
	}

	ಶೂನ್ಯ outport_vec(ಸ್ಥಿರಾಂಕ ಸದಿಶ<ಪೂರ್ಣಾಂಕ> &vec, IdString portname)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ಆದರೆ (cell->type.is_ಬಹಿರಂಗ())
		ಹಿಂತಿರುಗಿ;

	AigMaker mk(ಇದು, cell);
	name = cell->type.str();

	string mkname_ಕೊನೆಯ;
	ತಾರ್ಕಿಕ mkname_a_signed = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ mkname_b_signed = ಸುಳ್ಳು;
	ತಾರ್ಕಿಕ mkname_is_signed = ಸುಳ್ಳು;

	cell->parameters.ವಿಂಗಡಿಸು();
	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ p : cell->parameters)
	{
		ಆದರೆ (p.ಮೊದಲ == ID::A_ಅಗಲ && mkname_a_signed) {
			name = mkname_ಕೊನೆಯ + stringf(":%d%c", p.ಎರಡನೇ.as_int(), mkname_is_signed ? 'S' : 'U');
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p.ಮೊದಲ == ID::B_ಅಗಲ && mkname_b_signed) {
			name = mkname_ಕೊನೆಯ + stringf(":%d%c", p.ಎರಡನೇ.as_int(), mkname_is_signed ? 'S' : 'U');
		} ಇಲ್ಲದಿದ್ದರೆ {
			mkname_ಕೊನೆಯ = name;
			name += stringf(":%d", p.ಎರಡನೇ.as_int());
		}

		mkname_a_signed = ಸುಳ್ಳು;
		mkname_b_signed = ಸುಳ್ಳು;
		mkname_is_signed = ಸುಳ್ಳು;
		ಆದರೆ (p.ಮೊದಲ == ID::A_SIGNED) {
			mkname_a_signed = ನಿಜ;
			mkname_is_signed = p.ಎರಡನೇ.as_bool();
		}
		ಆದರೆ (p.ಮೊದಲ == ID::B_SIGNED) {
			mkname_b_signed = ನಿಜ;
			mkname_is_signed = p.ಎರಡನೇ.as_bool();
		}
	}

	ಆದರೆ (cell->type.ಒಳ(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y)); i++) {
			ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A, i);
			ಪೂರ್ಣಾಂಕ Y = cell->type.ಒಳ(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y)); i++) {
			ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A, i);
			ಪೂರ್ಣಾಂಕ B = mk.inport(ID::B, i);
			ಪೂರ್ಣಾಂಕ Y = cell->type.ಒಳ(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.ಒಳ(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.ಒಳ(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.ಒಳ(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.ಒಳ(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.ಒಳ(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.ಒಳ(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.ಒಳ(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		ಪೂರ್ಣಾಂಕ S = mk.inport(ID::S);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y)); i++) {
			ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A, i);
			ಪೂರ್ಣಾಂಕ B = mk.inport(ID::B, i);
			ಪೂರ್ಣಾಂಕ Y = mk.mux_gate(A, B, S);
			ಆದರೆ (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		ಪೂರ್ಣಾಂಕ Y = mk.inport(ID::A, 0);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A)); i++) {
			ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A, i);
			ಆದರೆ (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ಆದರೆ (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ಆದರೆ (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ಆದರೆ (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ಆದರೆ (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ಆದರೆ (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y)); i++)
			mk.outport(mk.bool_ಗಂಟು(ಸುಳ್ಳು), ID::Y, i);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A, 0), Y = -1;
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ಆದರೆ (cell->type.ಒಳ(ID($logic_and), ID($logic_or))) {
			ಪೂರ್ಣಾಂಕ B = mk.inport(ID::B, 0);
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ಆದರೆ (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ಆದರೆ (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} ಇಲ್ಲದಿದ್ದರೆ {
			ಆದರೆ (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($ಕೂಡಿಸು), ID($sub)))
	{
		ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> A = mk.inport_vec(ID::A, ಅಗಲ);
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> B = mk.inport_vec(ID::B, ಅಗಲ);
		ಪೂರ್ಣಾಂಕ ವಾಹಕ = mk.bool_ಗಂಟು(ಸುಳ್ಳು);
		ಆದರೆ (cell->type == ID($sub)) {
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &n : B)
				n = mk.not_gate(n);
			ವಾಹಕ = mk.not_gate(ವಾಹಕ);
		}
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> Y = mk.adder(A, B, ವಾಹಕ);
		mk.outport_vec(Y, ID::Y);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		ಪೂರ್ಣಾಂಕ ಅಗಲ = std::max(Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A)),
							 Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B))) + 1;
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> A = mk.inport_vec(ID::A, ಅಗಲ);
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> B = mk.inport_vec(ID::B, ಅಗಲ);

		ಆದರೆ (cell->type.ಒಳ(ID($gt), ID($ge)))
			std::ಅದಲುಬದಲು(A, B);

		ಪೂರ್ಣಾಂಕ ವಾಹಕ = mk.bool_ಗಂಟು(!cell->type.ಒಳ(ID($le), ID($ge)));
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &n : B)
			n = mk.not_gate(n);
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> Y = mk.adder(A, B, ವಾಹಕ);
		mk.outport(Y.ಹಿಂಭಾಗ(), ID::Y);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 1; i < Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y)); i++)
			mk.outport(mk.bool_ಗಂಟು(ಸುಳ್ಳು), ID::Y, i);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type == ID($alu))
	{
		ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> A = mk.inport_vec(ID::A, ಅಗಲ);
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> B = mk.inport_vec(ID::B, ಅಗಲ);
		ಪೂರ್ಣಾಂಕ ವಾಹಕ = mk.inport(ID::CI);
		ಪೂರ್ಣಾಂಕ binv = mk.inport(ID::BI);
		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &n : B)
			n = mk.xor_gate(n, binv);
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> X(ಅಗಲ), CO(ಅಗಲ);
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> Y = mk.adder(A, B, ವಾಹಕ, &X, &CO);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($eq), ID($ne)))
	{
		ಪೂರ್ಣಾಂಕ ಅಗಲ = max(Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A)), Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B)));
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> A = mk.inport_vec(ID::A, ಅಗಲ);
		ಸದಿಶ<ಪೂರ್ಣಾಂಕ> B = mk.inport_vec(ID::B, ಅಗಲ);
		ಪೂರ್ಣಾಂಕ Y = mk.bool_ಗಂಟು(ಸುಳ್ಳು);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ಆದರೆ (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type == ID($_AOI3_))
	{
		ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಂಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಂಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಂಕ Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type == ID($_OAI3_))
	{
		ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಂಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಂಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಂಕ Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type == ID($_AOI4_))
	{
		ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಂಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಂಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಂಕ D = mk.inport(ID::D);
		ಪೂರ್ಣಾಂಕ a_and_b = mk.and_gate(A, B);
		ಪೂರ್ಣಾಂಕ Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		ಹೋಗು optimize;
	}

	ಆದರೆ (cell->type == ID($_OAI4_))
	{
		ಪೂರ್ಣಾಂಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಂಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಂಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಂಕ D = mk.inport(ID::D);
		ಪೂರ್ಣಾಂಕ a_or_b = mk.or_gate(A, B);
		ಪೂರ್ಣಾಂಕ Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		ಹೋಗು optimize;
	}

	name.clear();
	ಹಿಂತಿರುಗಿ;

optimize:;
	pool<ಪೂರ್ಣಾಂಕ> ಬಳಸಿದ_old_ids;
	ಸದಿಶ<Aig_ಗಂಟು> new_nodes;
	dict<ಪೂರ್ಣಾಂಕ, ಪೂರ್ಣಾಂಕ> old_ಗಮ್ಯ_new_ids;
	old_ಗಮ್ಯ_new_ids[-1] = -1;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = Get_ಗಾತ್ರ(nodes)-1; i >= 0; i--) {
		ಆದರೆ (!nodes[i].outports.ಖಾಲಿಗಳು())
			ಬಳಸಿದ_old_ids.ಸೇರಿಸು(i);
		ಆದರೆ (!ಬಳಸಿದ_old_ids.ಎಣಿಕೆ_2(i))
			ಮುಂದುವರಿ;
		ಆದರೆ (nodes[i].ಎಡ_ಪೋಷಕ >= 0)
			ಬಳಸಿದ_old_ids.ಸೇರಿಸು(nodes[i].ಎಡ_ಪೋಷಕ);
		ಆದರೆ (nodes[i].ಬಲ_ಪೋಷಕ >= 0)
			ಬಳಸಿದ_old_ids.ಸೇರಿಸು(nodes[i].ಬಲ_ಪೋಷಕ);
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < Get_ಗಾತ್ರ(nodes); i++) {
		ಆದರೆ (!ಬಳಸಿದ_old_ids.ಎಣಿಕೆ_2(i))
			ಮುಂದುವರಿ;
		nodes[i].ಎಡ_ಪೋಷಕ = old_ಗಮ್ಯ_new_ids.at(nodes[i].ಎಡ_ಪೋಷಕ);
		nodes[i].ಬಲ_ಪೋಷಕ = old_ಗಮ್ಯ_new_ids.at(nodes[i].ಬಲ_ಪೋಷಕ);
		old_ಗಮ್ಯ_new_ids[i] = Get_ಗಾತ್ರ(new_nodes);
		new_nodes.ತಳ್ಳು_ಹಿಂಭಾಗ(nodes[i]);
	}

	new_nodes.ಅದಲುಬದಲು(nodes);
}

YOSYS_NAMESPACE_ಕೊನೆ
