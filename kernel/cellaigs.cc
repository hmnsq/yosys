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

Aig_गाठ::Aig_गाठ()
{
	portbit = -1;
	inverter = असत्य;
	डावा_parent = -1;
	उजवा_parent = -1;
}

तार्किक Aig_गाठ::चालक==(स्थिरांक Aig_गाठ &other) स्थिरांक
{
	जर (portname != other.portname) परत असत्य;
	जर (portbit != other.portbit) परत असत्य;
	जर (inverter != other.inverter) परत असत्य;
	जर (डावा_parent != other.डावा_parent) परत असत्य;
	जर (उजवा_parent != other.उजवा_parent) परत असत्य;
	परत सत्य;
}

Hasher Aig_गाठ::hash_into(Hasher h) स्थिरांक
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(डावा_parent);
	h.eat(उजवा_parent);
	परत h;
}

तार्किक Aig::चालक==(स्थिरांक Aig &other) स्थिरांक
{
	परत name == other.name;
}

Hasher Aig::hash_into(Hasher h) स्थिरांक
{
	h.eat(name);
	परत h;
}

संरचना AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_गाठ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	पूर्णांक node2index(स्थिरांक Aig_गाठ &गाठ)
	{
		जर (गाठ.डावा_parent > गाठ.उजवा_parent) {
			Aig_गाठ n(गाठ);
			std::swap(n.डावा_parent, n.उजवा_parent);
			परत node2index(n);
		}

		जर (!aig_indices.मोजणी(गाठ)) {
			aig_indices.expect(गाठ, GetSize(aig->nodes));
			aig->nodes.ढकला_back(गाठ);
		}

		परत aig_indices.at(गाठ);
	}

	पूर्णांक bool_गाठ(तार्किक मूल्य)
	{
		Aig_गाठ गाठ;
		गाठ.inverter = मूल्य;
		परत node2index(गाठ);
	}

	पूर्णांक inport(IdString portname, पूर्णांक portbit = 0, तार्किक inverter = असत्य)
	{
		जर (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		जर (signed_param != IdString::Null && cell->parameters.मोजणी(signed_param) && cell->getParam(signed_param).as_bool())
				परत inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			परत bool_गाठ(inverter);
		}

		Aig_गाठ गाठ;
		गाठ.portname = portname;
		गाठ.portbit = portbit;
		गाठ.inverter = inverter;
		परत node2index(गाठ);
	}

	vector<पूर्णांक> inport_vec(IdString portname, पूर्णांक width)
	{
		vector<पूर्णांक> vec;
		साठी (पूर्णांक i = 0; i < width; i++)
			vec.ढकला_back(inport(portname, i));
		परत vec;
	}

	पूर्णांक not_inport(IdString portname, पूर्णांक portbit = 0)
	{
		परत inport(portname, portbit, सत्य);
	}

	पूर्णांक not_gate(पूर्णांक A)
	{
		Aig_गाठ गाठ(aig_indices[A]);
		गाठ.outports.clear();
		गाठ.inverter = !गाठ.inverter;
		परत node2index(गाठ);
	}

	पूर्णांक and_gate(पूर्णांक A, पूर्णांक B, तार्किक inverter = असत्य)
	{
		जर (A == B)
			परत inverter ? not_gate(A) : A;

		स्थिरांक Aig_गाठ &nA = aig_indices[A];
		स्थिरांक Aig_गाठ &nB = aig_indices[B];

		Aig_गाठ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		जर (nA == nB_inv)
			परत bool_गाठ(inverter);

		तार्किक nA_bool = nA.portbit < 0 && nA.डावा_parent < 0 && nA.उजवा_parent < 0;
		तार्किक nB_bool = nB.portbit < 0 && nB.डावा_parent < 0 && nB.उजवा_parent < 0;

		जर (nA_bool && nB_bool) {
			तार्किक bA = nA.inverter;
			तार्किक bB = nB.inverter;
			परत bool_गाठ(inverter != (bA && bB));
		}

		जर (nA_bool) {
			तार्किक bA = nA.inverter;
			जर (inverter)
				परत bA ? not_gate(B) : bool_गाठ(सत्य);
			परत bA ? B : bool_गाठ(असत्य);
		}

		जर (nB_bool) {
			तार्किक bB = nB.inverter;
			जर (inverter)
				परत bB ? not_gate(A) : bool_गाठ(सत्य);
			परत bB ? A : bool_गाठ(असत्य);
		}

		Aig_गाठ गाठ;
		गाठ.inverter = inverter;
		गाठ.डावा_parent = A;
		गाठ.उजवा_parent = B;
		परत node2index(गाठ);
	}

	पूर्णांक nand_gate(पूर्णांक A, पूर्णांक B)
	{
		परत and_gate(A, B, सत्य);
	}

	पूर्णांक or_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_a = not_gate(A);
		पूर्णांक not_b = not_gate(B);
		परत nand_gate(not_a, not_b);
	}

	पूर्णांक nor_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_a = not_gate(A);
		पूर्णांक not_b = not_gate(B);
		परत and_gate(not_a, not_b);
	}

	पूर्णांक xor_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक a_and_b = and_gate(A, B);
		पूर्णांक a_nor_b = nor_gate(A, B);
		परत nor_gate(a_and_b, a_nor_b);
	}

	पूर्णांक xnor_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक a_and_b = and_gate(A, B);
		पूर्णांक a_nor_b = nor_gate(A, B);
		परत or_gate(a_and_b, a_nor_b);
	}

	पूर्णांक andnot_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_b = not_gate(B);
		परत and_gate(A, not_b);
	}

	पूर्णांक ornot_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_b = not_gate(B);
		परत or_gate(A, not_b);
	}

	पूर्णांक mux_gate(पूर्णांक A, पूर्णांक B, पूर्णांक S)
	{
		पूर्णांक not_s = not_gate(S);
		पूर्णांक a_active = and_gate(A, not_s);
		पूर्णांक b_active = and_gate(B, S);
		परत or_gate(a_active, b_active);
	}

	vector<पूर्णांक> adder(स्थिरांक vector<पूर्णांक> &A, स्थिरांक vector<पूर्णांक> &B, पूर्णांक carry_in, vector<पूर्णांक> *X = nullptr, vector<पूर्णांक> *CO = nullptr)
	{
		vector<पूर्णांक> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		साठी (पूर्णांक i = 0; i < GetSize(A); i++) {
			पूर्णांक a_xor_b = xor_gate(A[i], B[i]);
			पूर्णांक a_or_b = or_gate(A[i], B[i]);
			पूर्णांक a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			पूर्णांक tmp = and_gate(a_or_b, carry_in);
			पूर्णांक carry_out = or_gate(a_and_b, tmp);
			जर (X != nullptr)
				X->at(i) = a_xor_b;
			जर (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		परत Y;
	}

	शून्य outport(पूर्णांक गाठ, IdString portname, पूर्णांक portbit = 0)
	{
		जर (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(गाठ).outports.ढकला_back(pair<IdString, पूर्णांक>(portname, portbit));
	}

	शून्य outport_bool(पूर्णांक गाठ, IdString portname)
	{
		outport(गाठ, portname);
		साठी (पूर्णांक i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_गाठ(असत्य), portname, i);
	}

	शून्य outport_vec(स्थिरांक vector<पूर्णांक> &vec, IdString portname)
	{
		साठी (पूर्णांक i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	जर (cell->type.isPublic())
		परत;

	AigMaker mk(हे, cell);
	name = cell->type.str();

	string mkname_last;
	तार्किक mkname_a_signed = असत्य;
	तार्किक mkname_b_signed = असत्य;
	तार्किक mkname_is_signed = असत्य;

	cell->parameters.sort();
	साठी (स्वतः p : cell->parameters)
	{
		जर (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा जर (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = असत्य;
		mkname_b_signed = असत्य;
		mkname_is_signed = असत्य;
		जर (p.first == ID::A_SIGNED) {
			mkname_a_signed = सत्य;
			mkname_is_signed = p.second.as_bool();
		}
		जर (p.first == ID::B_SIGNED) {
			mkname_b_signed = सत्य;
			mkname_is_signed = p.second.as_bool();
		}
	}

	जर (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		साठी (पूर्णांक i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णांक A = mk.inport(ID::A, i);
			पूर्णांक Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		जा optimize;
	}

	जर (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		साठी (पूर्णांक i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णांक A = mk.inport(ID::A, i);
			पूर्णांक B = mk.inport(ID::B, i);
			पूर्णांक Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		जा optimize;
	}

	जर (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		पूर्णांक S = mk.inport(ID::S);
		साठी (पूर्णांक i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			पूर्णांक A = mk.inport(ID::A, i);
			पूर्णांक B = mk.inport(ID::B, i);
			पूर्णांक Y = mk.mux_gate(A, B, S);
			जर (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		जा optimize;
	}

	जर (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		पूर्णांक Y = mk.inport(ID::A, 0);
		साठी (पूर्णांक i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			पूर्णांक A = mk.inport(ID::A, i);
			जर (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			जर (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			जर (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			जर (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			जर (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		जर (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		साठी (पूर्णांक i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_गाठ(असत्य), ID::Y, i);
		जा optimize;
	}

	जर (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		पूर्णांक A = mk.inport(ID::A, 0), Y = -1;
		साठी (पूर्णांक i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		जर (cell->type.in(ID($logic_and), ID($logic_or))) {
			पूर्णांक B = mk.inport(ID::B, 0);
			साठी (पूर्णांक i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			जर (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			जर (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} अन्यथा {
			जर (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		जा optimize;
	}

	जर (cell->type.in(ID($add), ID($sub)))
	{
		पूर्णांक width = GetSize(cell->getPort(ID::Y));
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);
		पूर्णांक carry = mk.bool_गाठ(असत्य);
		जर (cell->type == ID($sub)) {
			साठी (स्वतः &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<पूर्णांक> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		जा optimize;
	}

	जर (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		पूर्णांक width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);

		जर (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		पूर्णांक carry = mk.bool_गाठ(!cell->type.in(ID($le), ID($ge)));
		साठी (स्वतः &n : B)
			n = mk.not_gate(n);
		vector<पूर्णांक> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		साठी (पूर्णांक i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_गाठ(असत्य), ID::Y, i);
		जा optimize;
	}

	जर (cell->type == ID($alu))
	{
		पूर्णांक width = GetSize(cell->getPort(ID::Y));
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);
		पूर्णांक carry = mk.inport(ID::CI);
		पूर्णांक binv = mk.inport(ID::BI);
		साठी (स्वतः &n : B)
			n = mk.xor_gate(n, binv);
		vector<पूर्णांक> X(width), CO(width);
		vector<पूर्णांक> Y = mk.adder(A, B, carry, &X, &CO);
		साठी (पूर्णांक i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		जा optimize;
	}

	जर (cell->type.in(ID($eq), ID($ne)))
	{
		पूर्णांक width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);
		पूर्णांक Y = mk.bool_गाठ(असत्य);
		साठी (पूर्णांक i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		जर (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		जा optimize;
	}

	जर (cell->type == ID($_AOI3_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		जा optimize;
	}

	जर (cell->type == ID($_OAI3_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		जा optimize;
	}

	जर (cell->type == ID($_AOI4_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक D = mk.inport(ID::D);
		पूर्णांक a_and_b = mk.and_gate(A, B);
		पूर्णांक Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		जा optimize;
	}

	जर (cell->type == ID($_OAI4_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक D = mk.inport(ID::D);
		पूर्णांक a_or_b = mk.or_gate(A, B);
		पूर्णांक Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		जा optimize;
	}

	name.clear();
	परत;

optimize:;
	pool<पूर्णांक> used_old_ids;
	vector<Aig_गाठ> new_nodes;
	dict<पूर्णांक, पूर्णांक> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	साठी (पूर्णांक i = GetSize(nodes)-1; i >= 0; i--) {
		जर (!nodes[i].outports.empty())
			used_old_ids.घाला(i);
		जर (!used_old_ids.मोजणी(i))
			सुरूठेवा;
		जर (nodes[i].डावा_parent >= 0)
			used_old_ids.घाला(nodes[i].डावा_parent);
		जर (nodes[i].उजवा_parent >= 0)
			used_old_ids.घाला(nodes[i].उजवा_parent);
	}

	साठी (पूर्णांक i = 0; i < GetSize(nodes); i++) {
		जर (!used_old_ids.मोजणी(i))
			सुरूठेवा;
		nodes[i].डावा_parent = old_to_new_ids.at(nodes[i].डावा_parent);
		nodes[i].उजवा_parent = old_to_new_ids.at(nodes[i].उजवा_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.ढकला_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
