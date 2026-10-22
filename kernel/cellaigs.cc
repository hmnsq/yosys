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

Aig_गाँठो::Aig_गाँठो()
{
	portbit = -1;
	inverter = असत्य;
	बायाँ_parent = -1;
	दायाँ_parent = -1;
}

तार्किक Aig_गाँठो::चालक==(अचर Aig_गाँठो &other) अचर
{
	यदि (portname != other.portname) फर्काऊ असत्य;
	यदि (portbit != other.portbit) फर्काऊ असत्य;
	यदि (inverter != other.inverter) फर्काऊ असत्य;
	यदि (बायाँ_parent != other.बायाँ_parent) फर्काऊ असत्य;
	यदि (दायाँ_parent != other.दायाँ_parent) फर्काऊ असत्य;
	फर्काऊ सत्य;
}

Hasher Aig_गाँठो::hash_into(Hasher h) अचर
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(बायाँ_parent);
	h.eat(दायाँ_parent);
	फर्काऊ h;
}

तार्किक Aig::चालक==(अचर Aig &other) अचर
{
	फर्काऊ name == other.name;
}

Hasher Aig::hash_into(Hasher h) अचर
{
	h.eat(name);
	फर्काऊ h;
}

संरचना AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_गाँठो> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	पूर्णाङ्क node2index(अचर Aig_गाँठो &गाँठो)
	{
		यदि (गाँठो.बायाँ_parent > गाँठो.दायाँ_parent) {
			Aig_गाँठो n(गाँठो);
			std::swap(n.बायाँ_parent, n.दायाँ_parent);
			फर्काऊ node2index(n);
		}

		यदि (!aig_indices.गन्ती(गाँठो)) {
			aig_indices.expect(गाँठो, GetSize(aig->nodes));
			aig->nodes.थप_back(गाँठो);
		}

		फर्काऊ aig_indices.at(गाँठो);
	}

	पूर्णाङ्क bool_गाँठो(तार्किक मान)
	{
		Aig_गाँठो गाँठो;
		गाँठो.inverter = मान;
		फर्काऊ node2index(गाँठो);
	}

	पूर्णाङ्क inport(IdString portname, पूर्णाङ्क portbit = 0, तार्किक inverter = असत्य)
	{
		यदि (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		यदि (signed_param != IdString::Null && cell->parameters.गन्ती(signed_param) && cell->getParam(signed_param).as_bool())
				फर्काऊ inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			फर्काऊ bool_गाँठो(inverter);
		}

		Aig_गाँठो गाँठो;
		गाँठो.portname = portname;
		गाँठो.portbit = portbit;
		गाँठो.inverter = inverter;
		फर्काऊ node2index(गाँठो);
	}

	vector<पूर्णाङ्क> inport_vec(IdString portname, पूर्णाङ्क width)
	{
		vector<पूर्णाङ्क> vec;
		लागि (पूर्णाङ्क i = 0; i < width; i++)
			vec.थप_back(inport(portname, i));
		फर्काऊ vec;
	}

	पूर्णाङ्क not_inport(IdString portname, पूर्णाङ्क portbit = 0)
	{
		फर्काऊ inport(portname, portbit, सत्य);
	}

	पूर्णाङ्क not_gate(पूर्णाङ्क A)
	{
		Aig_गाँठो गाँठो(aig_indices[A]);
		गाँठो.outports.clear();
		गाँठो.inverter = !गाँठो.inverter;
		फर्काऊ node2index(गाँठो);
	}

	पूर्णाङ्क and_gate(पूर्णाङ्क A, पूर्णाङ्क B, तार्किक inverter = असत्य)
	{
		यदि (A == B)
			फर्काऊ inverter ? not_gate(A) : A;

		अचर Aig_गाँठो &nA = aig_indices[A];
		अचर Aig_गाँठो &nB = aig_indices[B];

		Aig_गाँठो nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		यदि (nA == nB_inv)
			फर्काऊ bool_गाँठो(inverter);

		तार्किक nA_bool = nA.portbit < 0 && nA.बायाँ_parent < 0 && nA.दायाँ_parent < 0;
		तार्किक nB_bool = nB.portbit < 0 && nB.बायाँ_parent < 0 && nB.दायाँ_parent < 0;

		यदि (nA_bool && nB_bool) {
			तार्किक bA = nA.inverter;
			तार्किक bB = nB.inverter;
			फर्काऊ bool_गाँठो(inverter != (bA && bB));
		}

		यदि (nA_bool) {
			तार्किक bA = nA.inverter;
			यदि (inverter)
				फर्काऊ bA ? not_gate(B) : bool_गाँठो(सत्य);
			फर्काऊ bA ? B : bool_गाँठो(असत्य);
		}

		यदि (nB_bool) {
			तार्किक bB = nB.inverter;
			यदि (inverter)
				फर्काऊ bB ? not_gate(A) : bool_गाँठो(सत्य);
			फर्काऊ bB ? A : bool_गाँठो(असत्य);
		}

		Aig_गाँठो गाँठो;
		गाँठो.inverter = inverter;
		गाँठो.बायाँ_parent = A;
		गाँठो.दायाँ_parent = B;
		फर्काऊ node2index(गाँठो);
	}

	पूर्णाङ्क nand_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		फर्काऊ and_gate(A, B, सत्य);
	}

	पूर्णाङ्क or_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_a = not_gate(A);
		पूर्णाङ्क not_b = not_gate(B);
		फर्काऊ nand_gate(not_a, not_b);
	}

	पूर्णाङ्क nor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_a = not_gate(A);
		पूर्णाङ्क not_b = not_gate(B);
		फर्काऊ and_gate(not_a, not_b);
	}

	पूर्णाङ्क xor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क a_and_b = and_gate(A, B);
		पूर्णाङ्क a_nor_b = nor_gate(A, B);
		फर्काऊ nor_gate(a_and_b, a_nor_b);
	}

	पूर्णाङ्क xnor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क a_and_b = and_gate(A, B);
		पूर्णाङ्क a_nor_b = nor_gate(A, B);
		फर्काऊ or_gate(a_and_b, a_nor_b);
	}

	पूर्णाङ्क andnot_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_b = not_gate(B);
		फर्काऊ and_gate(A, not_b);
	}

	पूर्णाङ्क ornot_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_b = not_gate(B);
		फर्काऊ or_gate(A, not_b);
	}

	पूर्णाङ्क mux_gate(पूर्णाङ्क A, पूर्णाङ्क B, पूर्णाङ्क S)
	{
		पूर्णाङ्क not_s = not_gate(S);
		पूर्णाङ्क a_active = and_gate(A, not_s);
		पूर्णाङ्क b_active = and_gate(B, S);
		फर्काऊ or_gate(a_active, b_active);
	}

	vector<पूर्णाङ्क> adder(अचर vector<पूर्णाङ्क> &A, अचर vector<पूर्णाङ्क> &B, पूर्णाङ्क carry_in, vector<पूर्णाङ्क> *X = nullptr, vector<पूर्णाङ्क> *CO = nullptr)
	{
		vector<पूर्णाङ्क> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		लागि (पूर्णाङ्क i = 0; i < GetSize(A); i++) {
			पूर्णाङ्क a_xor_b = xor_gate(A[i], B[i]);
			पूर्णाङ्क a_or_b = or_gate(A[i], B[i]);
			पूर्णाङ्क a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			पूर्णाङ्क tmp = and_gate(a_or_b, carry_in);
			पूर्णाङ्क carry_out = or_gate(a_and_b, tmp);
			यदि (X != nullptr)
				X->at(i) = a_xor_b;
			यदि (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		फर्काऊ Y;
	}

	शून्य outport(पूर्णाङ्क गाँठो, IdString portname, पूर्णाङ्क portbit = 0)
	{
		यदि (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(गाँठो).outports.थप_back(pair<IdString, पूर्णाङ्क>(portname, portbit));
	}

	शून्य outport_bool(पूर्णाङ्क गाँठो, IdString portname)
	{
		outport(गाँठो, portname);
		लागि (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_गाँठो(असत्य), portname, i);
	}

	शून्य outport_vec(अचर vector<पूर्णाङ्क> &vec, IdString portname)
	{
		लागि (पूर्णाङ्क i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	यदि (cell->type.isPublic())
		फर्काऊ;

	AigMaker mk(एतत्, cell);
	name = cell->type.str();

	string mkname_last;
	तार्किक mkname_a_signed = असत्य;
	तार्किक mkname_b_signed = असत्य;
	तार्किक mkname_is_signed = असत्य;

	cell->parameters.sort();
	लागि (स्वतः p : cell->parameters)
	{
		यदि (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा यदि (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = असत्य;
		mkname_b_signed = असत्य;
		mkname_is_signed = असत्य;
		यदि (p.first == ID::A_SIGNED) {
			mkname_a_signed = सत्य;
			mkname_is_signed = p.second.as_bool();
		}
		यदि (p.first == ID::B_SIGNED) {
			mkname_b_signed = सत्य;
			mkname_is_signed = p.second.as_bool();
		}
	}

	यदि (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		लागि (पूर्णाङ्क i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	यदि (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		लागि (पूर्णाङ्क i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
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

	यदि (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		पूर्णाङ्क S = mk.inport(ID::S);
		लागि (पूर्णाङ्क i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क B = mk.inport(ID::B, i);
			पूर्णाङ्क Y = mk.mux_gate(A, B, S);
			यदि (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	यदि (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		पूर्णाङ्क Y = mk.inport(ID::A, 0);
		लागि (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			यदि (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			यदि (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			यदि (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			यदि (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			यदि (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		यदि (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		लागि (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_गाँठो(असत्य), ID::Y, i);
		गच्छ optimize;
	}

	यदि (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		पूर्णाङ्क A = mk.inport(ID::A, 0), Y = -1;
		लागि (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		यदि (cell->type.in(ID($logic_and), ID($logic_or))) {
			पूर्णाङ्क B = mk.inport(ID::B, 0);
			लागि (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			यदि (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			यदि (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} अन्यथा {
			यदि (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type.in(ID($add), ID($sub)))
	{
		पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);
		पूर्णाङ्क carry = mk.bool_गाँठो(असत्य);
		यदि (cell->type == ID($sub)) {
			लागि (स्वतः &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<पूर्णाङ्क> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		पूर्णाङ्क width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);

		यदि (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		पूर्णाङ्क carry = mk.bool_गाँठो(!cell->type.in(ID($le), ID($ge)));
		लागि (स्वतः &n : B)
			n = mk.not_gate(n);
		vector<पूर्णाङ्क> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		लागि (पूर्णाङ्क i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_गाँठो(असत्य), ID::Y, i);
		गच्छ optimize;
	}

	यदि (cell->type == ID($alu))
	{
		पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);
		पूर्णाङ्क carry = mk.inport(ID::CI);
		पूर्णाङ्क binv = mk.inport(ID::BI);
		लागि (स्वतः &n : B)
			n = mk.xor_gate(n, binv);
		vector<पूर्णाङ्क> X(width), CO(width);
		vector<पूर्णाङ्क> Y = mk.adder(A, B, carry, &X, &CO);
		लागि (पूर्णाङ्क i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		गच्छ optimize;
	}

	यदि (cell->type.in(ID($eq), ID($ne)))
	{
		पूर्णाङ्क width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<पूर्णाङ्क> A = mk.inport_vec(ID::A, width);
		vector<पूर्णाङ्क> B = mk.inport_vec(ID::B, width);
		पूर्णाङ्क Y = mk.bool_गाँठो(असत्य);
		लागि (पूर्णाङ्क i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		यदि (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type == ID($_AOI3_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type == ID($_OAI3_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type == ID($_AOI4_))
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

	यदि (cell->type == ID($_OAI4_))
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
	फर्काऊ;

optimize:;
	pool<पूर्णाङ्क> used_old_ids;
	vector<Aig_गाँठो> new_nodes;
	dict<पूर्णाङ्क, पूर्णाङ्क> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	लागि (पूर्णाङ्क i = GetSize(nodes)-1; i >= 0; i--) {
		यदि (!nodes[i].outports.empty())
			used_old_ids.घुसाउ(i);
		यदि (!used_old_ids.गन्ती(i))
			जारी;
		यदि (nodes[i].बायाँ_parent >= 0)
			used_old_ids.घुसाउ(nodes[i].बायाँ_parent);
		यदि (nodes[i].दायाँ_parent >= 0)
			used_old_ids.घुसाउ(nodes[i].दायाँ_parent);
	}

	लागि (पूर्णाङ्क i = 0; i < GetSize(nodes); i++) {
		यदि (!used_old_ids.गन्ती(i))
			जारी;
		nodes[i].बायाँ_parent = old_to_new_ids.at(nodes[i].बायाँ_parent);
		nodes[i].दायाँ_parent = old_to_new_ids.at(nodes[i].दायाँ_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.थप_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
