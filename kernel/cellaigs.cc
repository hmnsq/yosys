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
	inverter = ಅಸತ್ಯ;
	ಎಡ_parent = -1;
	ಬಲ_parent = -1;
}

ತಾರ್ಕಿಕ Aig_ಗಂಟು::operator==(ಧ್ರುವ Aig_ಗಂಟು &other) ಧ್ರುವ
{
	ಯದಿ (portname != other.portname) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (portbit != other.portbit) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (inverter != other.inverter) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (ಎಡ_parent != other.ಎಡ_parent) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಯದಿ (ಬಲ_parent != other.ಬಲ_parent) ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
	ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
}

Hasher Aig_ಗಂಟು::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(ಎಡ_parent);
	h.eat(ಬಲ_parent);
	ಪ್ರತ್ಯಾಗಮ h;
}

ತಾರ್ಕಿಕ Aig::operator==(ಧ್ರುವ Aig &other) ಧ್ರುವ
{
	ಪ್ರತ್ಯಾಗಮ name == other.name;
}

Hasher Aig::hash_into(Hasher h) ಧ್ರುವ
{
	h.eat(name);
	ಪ್ರತ್ಯಾಗಮ h;
}

ಸಂರಚನಾ AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_ಗಂಟು> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	ಪೂರ್ಣಾಙ್ಕ node2index(ಧ್ರುವ Aig_ಗಂಟು &ಗಂಟು)
	{
		ಯದಿ (ಗಂಟು.ಎಡ_parent > ಗಂಟು.ಬಲ_parent) {
			Aig_ಗಂಟು n(ಗಂಟು);
			std::swap(n.ಎಡ_parent, n.ಬಲ_parent);
			ಪ್ರತ್ಯಾಗಮ node2index(n);
		}

		ಯದಿ (!aig_indices.ಎಣಿಕೆ(ಗಂಟು)) {
			aig_indices.expect(ಗಂಟು, GetSize(aig->nodes));
			aig->nodes.ತಳ್ಳು_back(ಗಂಟು);
		}

		ಪ್ರತ್ಯಾಗಮ aig_indices.at(ಗಂಟು);
	}

	ಪೂರ್ಣಾಙ್ಕ bool_ಗಂಟು(ತಾರ್ಕಿಕ ಮೌಲ್ಯ)
	{
		Aig_ಗಂಟು ಗಂಟು;
		ಗಂಟು.inverter = ಮೌಲ್ಯ;
		ಪ್ರತ್ಯಾಗಮ node2index(ಗಂಟು);
	}

	ಪೂರ್ಣಾಙ್ಕ inport(IdString portname, ಪೂರ್ಣಾಙ್ಕ portbit = 0, ತಾರ್ಕಿಕ inverter = ಅಸತ್ಯ)
	{
		ಯದಿ (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		ಯದಿ (signed_param != IdString::Null && cell->parameters.ಎಣಿಕೆ(signed_param) && cell->getParam(signed_param).as_bool())
				ಪ್ರತ್ಯಾಗಮ inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			ಪ್ರತ್ಯಾಗಮ bool_ಗಂಟು(inverter);
		}

		Aig_ಗಂಟು ಗಂಟು;
		ಗಂಟು.portname = portname;
		ಗಂಟು.portbit = portbit;
		ಗಂಟು.inverter = inverter;
		ಪ್ರತ್ಯಾಗಮ node2index(ಗಂಟು);
	}

	vector<ಪೂರ್ಣಾಙ್ಕ> inport_vec(IdString portname, ಪೂರ್ಣಾಙ್ಕ width)
	{
		vector<ಪೂರ್ಣಾಙ್ಕ> vec;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
			vec.ತಳ್ಳು_back(inport(portname, i));
		ಪ್ರತ್ಯಾಗಮ vec;
	}

	ಪೂರ್ಣಾಙ್ಕ not_inport(IdString portname, ಪೂರ್ಣಾಙ್ಕ portbit = 0)
	{
		ಪ್ರತ್ಯಾಗಮ inport(portname, portbit, ಸತ್ಯ);
	}

	ಪೂರ್ಣಾಙ್ಕ not_gate(ಪೂರ್ಣಾಙ್ಕ A)
	{
		Aig_ಗಂಟು ಗಂಟು(aig_indices[A]);
		ಗಂಟು.outports.clear();
		ಗಂಟು.inverter = !ಗಂಟು.inverter;
		ಪ್ರತ್ಯಾಗಮ node2index(ಗಂಟು);
	}

	ಪೂರ್ಣಾಙ್ಕ and_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B, ತಾರ್ಕಿಕ inverter = ಅಸತ್ಯ)
	{
		ಯದಿ (A == B)
			ಪ್ರತ್ಯಾಗಮ inverter ? not_gate(A) : A;

		ಧ್ರುವ Aig_ಗಂಟು &nA = aig_indices[A];
		ಧ್ರುವ Aig_ಗಂಟು &nB = aig_indices[B];

		Aig_ಗಂಟು nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ಯದಿ (nA == nB_inv)
			ಪ್ರತ್ಯಾಗಮ bool_ಗಂಟು(inverter);

		ತಾರ್ಕಿಕ nA_bool = nA.portbit < 0 && nA.ಎಡ_parent < 0 && nA.ಬಲ_parent < 0;
		ತಾರ್ಕಿಕ nB_bool = nB.portbit < 0 && nB.ಎಡ_parent < 0 && nB.ಬಲ_parent < 0;

		ಯದಿ (nA_bool && nB_bool) {
			ತಾರ್ಕಿಕ bA = nA.inverter;
			ತಾರ್ಕಿಕ bB = nB.inverter;
			ಪ್ರತ್ಯಾಗಮ bool_ಗಂಟು(inverter != (bA && bB));
		}

		ಯದಿ (nA_bool) {
			ತಾರ್ಕಿಕ bA = nA.inverter;
			ಯದಿ (inverter)
				ಪ್ರತ್ಯಾಗಮ bA ? not_gate(B) : bool_ಗಂಟು(ಸತ್ಯ);
			ಪ್ರತ್ಯಾಗಮ bA ? B : bool_ಗಂಟು(ಅಸತ್ಯ);
		}

		ಯದಿ (nB_bool) {
			ತಾರ್ಕಿಕ bB = nB.inverter;
			ಯದಿ (inverter)
				ಪ್ರತ್ಯಾಗಮ bB ? not_gate(A) : bool_ಗಂಟು(ಸತ್ಯ);
			ಪ್ರತ್ಯಾಗಮ bB ? A : bool_ಗಂಟು(ಅಸತ್ಯ);
		}

		Aig_ಗಂಟು ಗಂಟು;
		ಗಂಟು.inverter = inverter;
		ಗಂಟು.ಎಡ_parent = A;
		ಗಂಟು.ಬಲ_parent = B;
		ಪ್ರತ್ಯಾಗಮ node2index(ಗಂಟು);
	}

	ಪೂರ್ಣಾಙ್ಕ nand_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B)
	{
		ಪ್ರತ್ಯಾಗಮ and_gate(A, B, ಸತ್ಯ);
	}

	ಪೂರ್ಣಾಙ್ಕ or_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B)
	{
		ಪೂರ್ಣಾಙ್ಕ not_a = not_gate(A);
		ಪೂರ್ಣಾಙ್ಕ not_b = not_gate(B);
		ಪ್ರತ್ಯಾಗಮ nand_gate(not_a, not_b);
	}

	ಪೂರ್ಣಾಙ್ಕ nor_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B)
	{
		ಪೂರ್ಣಾಙ್ಕ not_a = not_gate(A);
		ಪೂರ್ಣಾಙ್ಕ not_b = not_gate(B);
		ಪ್ರತ್ಯಾಗಮ and_gate(not_a, not_b);
	}

	ಪೂರ್ಣಾಙ್ಕ xor_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B)
	{
		ಪೂರ್ಣಾಙ್ಕ a_and_b = and_gate(A, B);
		ಪೂರ್ಣಾಙ್ಕ a_nor_b = nor_gate(A, B);
		ಪ್ರತ್ಯಾಗಮ nor_gate(a_and_b, a_nor_b);
	}

	ಪೂರ್ಣಾಙ್ಕ xnor_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B)
	{
		ಪೂರ್ಣಾಙ್ಕ a_and_b = and_gate(A, B);
		ಪೂರ್ಣಾಙ್ಕ a_nor_b = nor_gate(A, B);
		ಪ್ರತ್ಯಾಗಮ or_gate(a_and_b, a_nor_b);
	}

	ಪೂರ್ಣಾಙ್ಕ andnot_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B)
	{
		ಪೂರ್ಣಾಙ್ಕ not_b = not_gate(B);
		ಪ್ರತ್ಯಾಗಮ and_gate(A, not_b);
	}

	ಪೂರ್ಣಾಙ್ಕ ornot_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B)
	{
		ಪೂರ್ಣಾಙ್ಕ not_b = not_gate(B);
		ಪ್ರತ್ಯಾಗಮ or_gate(A, not_b);
	}

	ಪೂರ್ಣಾಙ್ಕ mux_gate(ಪೂರ್ಣಾಙ್ಕ A, ಪೂರ್ಣಾಙ್ಕ B, ಪೂರ್ಣಾಙ್ಕ S)
	{
		ಪೂರ್ಣಾಙ್ಕ not_s = not_gate(S);
		ಪೂರ್ಣಾಙ್ಕ a_active = and_gate(A, not_s);
		ಪೂರ್ಣಾಙ್ಕ b_active = and_gate(B, S);
		ಪ್ರತ್ಯಾಗಮ or_gate(a_active, b_active);
	}

	vector<ಪೂರ್ಣಾಙ್ಕ> adder(ಧ್ರುವ vector<ಪೂರ್ಣಾಙ್ಕ> &A, ಧ್ರುವ vector<ಪೂರ್ಣಾಙ್ಕ> &B, ಪೂರ್ಣಾಙ್ಕ carry_in, vector<ಪೂರ್ಣಾಙ್ಕ> *X = nullptr, vector<ಪೂರ್ಣಾಙ್ಕ> *CO = nullptr)
	{
		vector<ಪೂರ್ಣಾಙ್ಕ> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(A); i++) {
			ಪೂರ್ಣಾಙ್ಕ a_xor_b = xor_gate(A[i], B[i]);
			ಪೂರ್ಣಾಙ್ಕ a_or_b = or_gate(A[i], B[i]);
			ಪೂರ್ಣಾಙ್ಕ a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			ಪೂರ್ಣಾಙ್ಕ tmp = and_gate(a_or_b, carry_in);
			ಪೂರ್ಣಾಙ್ಕ carry_out = or_gate(a_and_b, tmp);
			ಯದಿ (X != nullptr)
				X->at(i) = a_xor_b;
			ಯದಿ (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		ಪ್ರತ್ಯಾಗಮ Y;
	}

	ಶೂನ್ಯ outport(ಪೂರ್ಣಾಙ್ಕ ಗಂಟು, IdString portname, ಪೂರ್ಣಾಙ್ಕ portbit = 0)
	{
		ಯದಿ (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(ಗಂಟು).outports.ತಳ್ಳು_back(pair<IdString, ಪೂರ್ಣಾಙ್ಕ>(portname, portbit));
	}

	ಶೂನ್ಯ outport_bool(ಪೂರ್ಣಾಙ್ಕ ಗಂಟು, IdString portname)
	{
		outport(ಗಂಟು, portname);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_ಗಂಟು(ಅಸತ್ಯ), portname, i);
	}

	ಶೂನ್ಯ outport_vec(ಧ್ರುವ vector<ಪೂರ್ಣಾಙ್ಕ> &vec, IdString portname)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ಯದಿ (cell->type.isPublic())
		ಪ್ರತ್ಯಾಗಮ;

	AigMaker mk(ಏತತ್, cell);
	name = cell->type.str();

	string mkname_last;
	ತಾರ್ಕಿಕ mkname_a_signed = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ mkname_b_signed = ಅಸತ್ಯ;
	ತಾರ್ಕಿಕ mkname_is_signed = ಅಸತ್ಯ;

	cell->parameters.sort();
	ಪ್ರತಿ (auto p : cell->parameters)
	{
		ಯದಿ (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} ಅನ್ಯಥಾ ಯದಿ (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} ಅನ್ಯಥಾ {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = ಅಸತ್ಯ;
		mkname_b_signed = ಅಸತ್ಯ;
		mkname_is_signed = ಅಸತ್ಯ;
		ಯದಿ (p.first == ID::A_SIGNED) {
			mkname_a_signed = ಸತ್ಯ;
			mkname_is_signed = p.second.as_bool();
		}
		ಯದಿ (p.first == ID::B_SIGNED) {
			mkname_b_signed = ಸತ್ಯ;
			mkname_is_signed = p.second.as_bool();
		}
	}

	ಯದಿ (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A, i);
			ಪೂರ್ಣಾಙ್ಕ Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A, i);
			ಪೂರ್ಣಾಙ್ಕ B = mk.inport(ID::B, i);
			ಪೂರ್ಣಾಙ್ಕ Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		ಪೂರ್ಣಾಙ್ಕ S = mk.inport(ID::S);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A, i);
			ಪೂರ್ಣಾಙ್ಕ B = mk.inport(ID::B, i);
			ಪೂರ್ಣಾಙ್ಕ Y = mk.mux_gate(A, B, S);
			ಯದಿ (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		ಪೂರ್ಣಾಙ್ಕ Y = mk.inport(ID::A, 0);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A, i);
			ಯದಿ (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ಯದಿ (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ಯದಿ (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ಯದಿ (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ಯದಿ (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ಯದಿ (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ಗಂಟು(ಅಸತ್ಯ), ID::Y, i);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A, 0), Y = -1;
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ಯದಿ (cell->type.in(ID($logic_and), ID($logic_or))) {
			ಪೂರ್ಣಾಙ್ಕ B = mk.inport(ID::B, 0);
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ಯದಿ (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ಯದಿ (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} ಅನ್ಯಥಾ {
			ಯದಿ (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type.in(ID($add), ID($sub)))
	{
		ಪೂರ್ಣಾಙ್ಕ width = GetSize(cell->getPort(ID::Y));
		vector<ಪೂರ್ಣಾಙ್ಕ> A = mk.inport_vec(ID::A, width);
		vector<ಪೂರ್ಣಾಙ್ಕ> B = mk.inport_vec(ID::B, width);
		ಪೂರ್ಣಾಙ್ಕ carry = mk.bool_ಗಂಟು(ಅಸತ್ಯ);
		ಯದಿ (cell->type == ID($sub)) {
			ಪ್ರತಿ (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<ಪೂರ್ಣಾಙ್ಕ> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		ಪೂರ್ಣಾಙ್ಕ width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<ಪೂರ್ಣಾಙ್ಕ> A = mk.inport_vec(ID::A, width);
		vector<ಪೂರ್ಣಾಙ್ಕ> B = mk.inport_vec(ID::B, width);

		ಯದಿ (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		ಪೂರ್ಣಾಙ್ಕ carry = mk.bool_ಗಂಟು(!cell->type.in(ID($le), ID($ge)));
		ಪ್ರತಿ (auto &n : B)
			n = mk.not_gate(n);
		vector<ಪೂರ್ಣಾಙ್ಕ> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ಗಂಟು(ಅಸತ್ಯ), ID::Y, i);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type == ID($alu))
	{
		ಪೂರ್ಣಾಙ್ಕ width = GetSize(cell->getPort(ID::Y));
		vector<ಪೂರ್ಣಾಙ್ಕ> A = mk.inport_vec(ID::A, width);
		vector<ಪೂರ್ಣಾಙ್ಕ> B = mk.inport_vec(ID::B, width);
		ಪೂರ್ಣಾಙ್ಕ carry = mk.inport(ID::CI);
		ಪೂರ್ಣಾಙ್ಕ binv = mk.inport(ID::BI);
		ಪ್ರತಿ (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<ಪೂರ್ಣಾಙ್ಕ> X(width), CO(width);
		vector<ಪೂರ್ಣಾಙ್ಕ> Y = mk.adder(A, B, carry, &X, &CO);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type.in(ID($eq), ID($ne)))
	{
		ಪೂರ್ಣಾಙ್ಕ width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<ಪೂರ್ಣಾಙ್ಕ> A = mk.inport_vec(ID::A, width);
		vector<ಪೂರ್ಣಾಙ್ಕ> B = mk.inport_vec(ID::B, width);
		ಪೂರ್ಣಾಙ್ಕ Y = mk.bool_ಗಂಟು(ಅಸತ್ಯ);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ಯದಿ (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type == ID($_AOI3_))
	{
		ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಙ್ಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಙ್ಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಙ್ಕ Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type == ID($_OAI3_))
	{
		ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಙ್ಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಙ್ಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಙ್ಕ Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type == ID($_AOI4_))
	{
		ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಙ್ಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಙ್ಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಙ್ಕ D = mk.inport(ID::D);
		ಪೂರ್ಣಾಙ್ಕ a_and_b = mk.and_gate(A, B);
		ಪೂರ್ಣಾಙ್ಕ Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		ಗಚ್ಛ optimize;
	}

	ಯದಿ (cell->type == ID($_OAI4_))
	{
		ಪೂರ್ಣಾಙ್ಕ A = mk.inport(ID::A);
		ಪೂರ್ಣಾಙ್ಕ B = mk.inport(ID::B);
		ಪೂರ್ಣಾಙ್ಕ C = mk.inport(ID::C);
		ಪೂರ್ಣಾಙ್ಕ D = mk.inport(ID::D);
		ಪೂರ್ಣಾಙ್ಕ a_or_b = mk.or_gate(A, B);
		ಪೂರ್ಣಾಙ್ಕ Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		ಗಚ್ಛ optimize;
	}

	name.clear();
	ಪ್ರತ್ಯಾಗಮ;

optimize:;
	pool<ಪೂರ್ಣಾಙ್ಕ> used_old_ids;
	vector<Aig_ಗಂಟು> new_nodes;
	dict<ಪೂರ್ಣಾಙ್ಕ, ಪೂರ್ಣಾಙ್ಕ> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = GetSize(nodes)-1; i >= 0; i--) {
		ಯದಿ (!nodes[i].outports.empty())
			used_old_ids.ಸೇರಿಸು(i);
		ಯದಿ (!used_old_ids.ಎಣಿಕೆ(i))
			ಅನುವರ್ತ;
		ಯದಿ (nodes[i].ಎಡ_parent >= 0)
			used_old_ids.ಸೇರಿಸು(nodes[i].ಎಡ_parent);
		ಯದಿ (nodes[i].ಬಲ_parent >= 0)
			used_old_ids.ಸೇರಿಸು(nodes[i].ಬಲ_parent);
	}

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < GetSize(nodes); i++) {
		ಯದಿ (!used_old_ids.ಎಣಿಕೆ(i))
			ಅನುವರ್ತ;
		nodes[i].ಎಡ_parent = old_to_new_ids.at(nodes[i].ಎಡ_parent);
		nodes[i].ಬಲ_parent = old_to_new_ids.at(nodes[i].ಬಲ_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.ತಳ್ಳು_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
