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

#अन्तर्भाव "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_ग्रन्थिः::Aig_ग्रन्थिः()
{
	portbit = -1;
	inverter = मिच्छा;
	वामः_parent = -1;
	दक्षिणः_parent = -1;
}

तार्किक Aig_ग्रन्थिः::operator==(ध्रुव Aig_ग्रन्थिः &other) ध्रुव
{
	सचे (portname != other.portname) पटिनिवत्त मिच्छा;
	सचे (portbit != other.portbit) पटिनिवत्त मिच्छा;
	सचे (inverter != other.inverter) पटिनिवत्त मिच्छा;
	सचे (वामः_parent != other.वामः_parent) पटिनिवत्त मिच्छा;
	सचे (दक्षिणः_parent != other.दक्षिणः_parent) पटिनिवत्त मिच्छा;
	पटिनिवत्त सच्च;
}

Hasher Aig_ग्रन्थिः::hash_into(Hasher h) ध्रुव
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(वामः_parent);
	h.eat(दक्षिणः_parent);
	पटिनिवत्त h;
}

तार्किक Aig::operator==(ध्रुव Aig &other) ध्रुव
{
	पटिनिवत्त name == other.name;
}

Hasher Aig::hash_into(Hasher h) ध्रुव
{
	h.eat(name);
	पटिनिवत्त h;
}

संरचना AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_ग्रन्थिः> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	पूर्णाङ्क node2index(ध्रुव Aig_ग्रन्थिः &ग्रन्थिः)
	{
		सचे (ग्रन्थिः.वामः_parent > ग्रन्थिः.दक्षिणः_parent) {
			Aig_ग्रन्थिः n(ग्रन्थिः);
			std::swap(n.वामः_parent, n.दक्षिणः_parent);
			पटिनिवत्त node2index(n);
		}

		सचे (!aig_indices.गणनम्(ग्रन्थिः)) {
			aig_indices.expect(ग्रन्थिः, GetSize(aig->nodes));
			aig->nodes.प्रक्षिप_back(ग्रन्थिः);
		}

		पटिनिवत्त aig_indices.at(ग्रन्थिः);
	}

	पूर्णाङ्क bool_ग्रन्थिः(तार्किक मानम्)
	{
		Aig_ग्रन्थिः ग्रन्थिः;
		ग्रन्थिः.inverter = मानम्;
		पटिनिवत्त node2index(ग्रन्थिः);
	}

	पूर्णाङ्क inport(IdString portname, पूर्णाङ्क portbit = 0, तार्किक inverter = मिच्छा)
	{
		सचे (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		सचे (signed_param != IdString::Null && cell->parameters.गणनम्(signed_param) && cell->getParam(signed_param).as_bool())
				पटिनिवत्त inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			पटिनिवत्त bool_ग्रन्थिः(inverter);
		}

		Aig_ग्रन्थिः ग्रन्थिः;
		ग्रन्थिः.portname = portname;
		ग्रन्थिः.portbit = portbit;
		ग्रन्थिः.inverter = inverter;
		पटिनिवत्त node2index(ग्रन्थिः);
	}

	vector<पूर्णाङ्क> inport_vec(IdString portname, पूर्णाङ्क width)
	{
		vector<पूर्णाङ्क> vec;
		अत्थाय (पूर्णाङ्क i = 0; i < width; i++)
			vec.प्रक्षिप_back(inport(portname, i));
		पटिनिवत्त vec;
	}

	पूर्णाङ्क not_inport(IdString portname, पूर्णाङ्क portbit = 0)
	{
		पटिनिवत्त inport(portname, portbit, सच्च);
	}

	पूर्णाङ्क not_gate(पूर्णाङ्क A)
	{
		Aig_ग्रन्थिः ग्रन्थिः(aig_indices[A]);
		ग्रन्थिः.outports.clear();
		ग्रन्थिः.inverter = !ग्रन्थिः.inverter;
		पटिनिवत्त node2index(ग्रन्थिः);
	}

	पूर्णाङ्क and_gate(पूर्णाङ्क A, पूर्णाङ्क B, तार्किक inverter = मिच्छा)
	{
		सचे (A == B)
			पटिनिवत्त inverter ? not_gate(A) : A;

		ध्रुव Aig_ग्रन्थिः &nA = aig_indices[A];
		ध्रुव Aig_ग्रन्थिः &nB = aig_indices[B];

		Aig_ग्रन्थिः nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		सचे (nA == nB_inv)
			पटिनिवत्त bool_ग्रन्थिः(inverter);

		तार्किक nA_bool = nA.portbit < 0 && nA.वामः_parent < 0 && nA.दक्षिणः_parent < 0;
		तार्किक nB_bool = nB.portbit < 0 && nB.वामः_parent < 0 && nB.दक्षिणः_parent < 0;

		सचे (nA_bool && nB_bool) {
			तार्किक bA = nA.inverter;
			तार्किक bB = nB.inverter;
			पटिनिवत्त bool_ग्रन्थिः(inverter != (bA && bB));
		}

		सचे (nA_bool) {
			तार्किक bA = nA.inverter;
			सचे (inverter)
				पटिनिवत्त bA ? not_gate(B) : bool_ग्रन्थिः(सच्च);
			पटिनिवत्त bA ? B : bool_ग्रन्थिः(मिच्छा);
		}

		सचे (nB_bool) {
			तार्किक bB = nB.inverter;
			सचे (inverter)
				पटिनिवत्त bB ? not_gate(A) : bool_ग्रन्थिः(सच्च);
			पटिनिवत्त bB ? A : bool_ग्रन्थिः(मिच्छा);
		}

		Aig_ग्रन्थिः ग्रन्थिः;
		ग्रन्थिः.inverter = inverter;
		ग्रन्थिः.वामः_parent = A;
		ग्रन्थिः.दक्षिणः_parent = B;
		पटिनिवत्त node2index(ग्रन्थिः);
	}

	पूर्णाङ्क nand_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पटिनिवत्त and_gate(A, B, सच्च);
	}

	पूर्णाङ्क or_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_a = not_gate(A);
		पूर्णाङ्क not_b = not_gate(B);
		पटिनिवत्त nand_gate(not_a, not_b);
	}

	पूर्णाङ्क nor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_a = not_gate(A);
		पूर्णाङ्क not_b = not_gate(B);
		पटिनिवत्त and_gate(not_a, not_b);
	}

	पूर्णाङ्क xor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क a_and_b = and_gate(A, B);
		पूर्णाङ्क a_nor_b = nor_gate(A, B);
		पटिनिवत्त nor_gate(a_and_b, a_nor_b);
	}

	पूर्णाङ्क xnor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क a_and_b = and_gate(A, B);
		पूर्णाङ्क a_nor_b = nor_gate(A, B);
		पटिनिवत्त or_gate(a_and_b, a_nor_b);
	}

	पूर्णाङ्क andnot_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_b = not_gate(B);
		पटिनिवत्त and_gate(A, not_b);
	}

	पूर्णाङ्क ornot_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_b = not_gate(B);
		पटिनिवत्त or_gate(A, not_b);
	}

	पूर्णाङ्क mux_gate(पूर्णाङ्क A, पूर्णाङ्क B, पूर्णाङ्क S)
	{
		पूर्णाङ्क not_s = not_gate(S);
		पूर्णाङ्क a_active = and_gate(A, not_s);
		पूर्णाङ्क b_active = and_gate(B, S);
		पटिनिवत्त or_gate(a_active, b_active);
	}

	vector<पूर्णाङ्क> adder(ध्रुव vector<पूर्णाङ्क> &A, ध्रुव vector<पूर्णाङ्क> &B, पूर्णाङ्क carry_in, vector<पूर्णाङ्क> *X = nullptr, vector<पूर्णाङ्क> *CO = nullptr)
	{
		vector<पूर्णाङ्क> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(A); i++) {
			पूर्णाङ्क a_xor_b = xor_gate(A[i], B[i]);
			पूर्णाङ्क a_or_b = or_gate(A[i], B[i]);
			पूर्णाङ्क a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			पूर्णाङ्क tmp = and_gate(a_or_b, carry_in);
			पूर्णाङ्क carry_out = or_gate(a_and_b, tmp);
			सचे (X != nullptr)
				X->at(i) = a_xor_b;
			सचे (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		पटिनिवत्त Y;
	}

	शून्य outport(पूर्णाङ्क ग्रन्थिः, IdString portname, पूर्णाङ्क portbit = 0)
	{
		सचे (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(ग्रन्थिः).outports.प्रक्षिप_back(pair<IdString, पूर्णाङ्क>(portname, portbit));
	}

	शून्य outport_bool(पूर्णाङ्क ग्रन्थिः, IdString portname)
	{
		outport(ग्रन्थिः, portname);
		अत्थाय (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_ग्रन्थिः(मिच्छा), portname, i);
	}

	शून्य outport_vec(ध्रुव vector<पूर्णाङ्क> &vec, IdString portname)
	{
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	सचे (cell->type.isPublic())
		पटिनिवत्त;

	AigMaker mk(एतत्, cell);
	name = cell->type.str();

	string mkname_last;
	तार्किक mkname_a_signed = मिच्छा;
	तार्किक mkname_b_signed = मिच्छा;
	तार्किक mkname_is_signed = मिच्छा;

	cell->parameters.sort();
	अत्थाय (auto p : cell->parameters)
	{
		सचे (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अञ्ञथा सचे (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अञ्ञथा {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = मिच्छा;
		mkname_b_signed = मिच्छा;
		mkname_is_signed = मिच्छा;
		सचे (p.first == ID::A_SIGNED) {
			mkname_a_signed = सच्च;
			mkname_is_signed = p.second.as_bool();
		}
		सचे (p.first == ID::B_SIGNED) {
			mkname_b_signed = सच्च;
			mkname_is_signed = p.second.as_bool();
		}
	}

	सचे (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	सचे (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क B = mk.inport(ID::B, i);
			पूर्णाङ्क Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	सचे (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		पूर्णाङ्क S = mk.inport(ID::S);
		अत्थाय (पूर्णाङ्क i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क B = mk.inport(ID::B, i);
			पूर्णाङ्क Y = mk.mux_gate(A, B, S);
			सचे (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	सचे (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		पूर्णाङ्क Y = mk.inport(ID::A, 0);
		अत्थाय (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			सचे (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			सचे (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			सचे (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			सचे (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			सचे (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		सचे (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		अत्थाय (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ग्रन्थिः(मिच्छा), ID::Y, i);
		गच्छ optimize;
	}

	सचे (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		पूर्णाङ्क A = mk.inport(ID::A, 0), Y = -1;
		अत्थाय (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		सचे (cell->type.in(ID($logic_and), ID($logic_or))) {
			पूर्णाङ्क B = mk.inport(ID::B, 0);
			अत्थाय (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			सचे (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			सचे (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} अञ्ञथा {
			सचे (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		गच्छ optimize;
	}

	सचे (cell->type.in(ID($add), ID($sub)))
	{
		पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);
		पूर्णाङ्क carry = mk.bool_ग्रन्थिः(मिच्छा);
		सचे (cell->type == ID($sub)) {
			अत्थाय (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<पूर्णाङ्क> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		गच्छ optimize;
	}

	सचे (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		पूर्णाङ्क width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);

		सचे (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		पूर्णाङ्क carry = mk.bool_ग्रन्थिः(!cell->type.in(ID($le), ID($ge)));
		अत्थाय (auto &n : B)
			n = mk.not_gate(n);
		vector<पूर्णाङ्क> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		अत्थाय (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ग्रन्थिः(मिच्छा), ID::Y, i);
		गच्छ optimize;
	}

	सचे (cell->type == ID($alu))
	{
		पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);
		पूर्णाङ्क carry = mk.inport(ID::CI);
		पूर्णाङ्क binv = mk.inport(ID::BI);
		अत्थाय (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<पूर्णाङ्क> X(width), CO(width);
		vector<पूर्णाङ्क> Y = mk.adder(A, B, carry, &X, &CO);
		अत्थाय (पूर्णाङ्क i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		गच्छ optimize;
	}

	सचे (cell->type.in(ID($eq), ID($ne)))
	{
		पूर्णाङ्क width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);
		पूर्णाङ्क Y = mk.bool_ग्रन्थिः(मिच्छा);
		अत्थाय (पूर्णाङ्क i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		सचे (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		गच्छ optimize;
	}

	सचे (cell->type == ID($_AOI3_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	सचे (cell->type == ID($_OAI3_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	सचे (cell->type == ID($_AOI4_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क D = mk.inport(ID::D);
		पूर्णाङ्क a_and_b = mk.and_gate(A, B);
		पूर्णाङ्क Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	सचे (cell->type == ID($_OAI4_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क D = mk.inport(ID::D);
		पूर्णाङ्क a_or_b = mk.or_gate(A, B);
		पूर्णाङ्क Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	name.clear();
	पटिनिवत्त;

optimize:;
	pool<पूर्णाङ्क> used_old_ids;
	vector<Aig_ग्रन्थिः> new_nodes;
	dict<पूर्णाङ्क, पूर्णाङ्क> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	अत्थाय (पूर्णाङ्क i = GetSize(nodes)-1; i >= 0; i--) {
		सचे (!nodes[i].outports.empty())
			used_old_ids.निवेशय(i);
		सचे (!used_old_ids.गणनम्(i))
			अनुवर्त;
		सचे (nodes[i].वामः_parent >= 0)
			used_old_ids.निवेशय(nodes[i].वामः_parent);
		सचे (nodes[i].दक्षिणः_parent >= 0)
			used_old_ids.निवेशय(nodes[i].दक्षिणः_parent);
	}

	अत्थाय (पूर्णाङ्क i = 0; i < GetSize(nodes); i++) {
		सचे (!used_old_ids.गणनम्(i))
			अनुवर्त;
		nodes[i].वामः_parent = old_to_new_ids.at(nodes[i].वामः_parent);
		nodes[i].दक्षिणः_parent = old_to_new_ids.at(nodes[i].दक्षिणः_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.प्रक्षिप_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
