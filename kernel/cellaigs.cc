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

#समावेश "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_गाँठ::Aig_गाँठ()
{
	portbit = -1;
	inverter = false;
	बायाँ_parent = -1;
	दायाँ_parent = -1;
}

bool Aig_गाँठ::चालक==(स्थिर Aig_गाँठ &other) स्थिर
{
	अगर (portname != other.portname) वापस false;
	अगर (portbit != other.portbit) वापस false;
	अगर (inverter != other.inverter) वापस false;
	अगर (बायाँ_parent != other.बायाँ_parent) वापस false;
	अगर (दायाँ_parent != other.दायाँ_parent) वापस false;
	वापस true;
}

Hasher Aig_गाँठ::hash_into(Hasher h) स्थिर
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(बायाँ_parent);
	h.eat(दायाँ_parent);
	वापस h;
}

bool Aig::चालक==(स्थिर Aig &other) स्थिर
{
	वापस name == other.name;
}

Hasher Aig::hash_into(Hasher h) स्थिर
{
	h.eat(name);
	वापस h;
}

काष्ठा AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_गाँठ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	पूर्णांक node2index(स्थिर Aig_गाँठ &गाँठ)
	{
		अगर (गाँठ.बायाँ_parent > गाँठ.दायाँ_parent) {
			Aig_गाँठ n(गाँठ);
			std::swap(n.बायाँ_parent, n.दायाँ_parent);
			वापस node2index(n);
		}

		अगर (!aig_indices.गिनती(गाँठ)) {
			aig_indices.expect(गाँठ, Get_आकार(aig->nodes));
			aig->nodes.धकेलो_back(गाँठ);
		}

		वापस aig_indices.at(गाँठ);
	}

	पूर्णांक bool_गाँठ(bool मूल्य)
	{
		Aig_गाँठ गाँठ;
		गाँठ.inverter = मूल्य;
		वापस node2index(गाँठ);
	}

	पूर्णांक inport(IdString portname, पूर्णांक portbit = 0, bool inverter = false)
	{
		अगर (portbit >= Get_आकार(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		अगर (signed_param != IdString::Null && cell->parameters.गिनती(signed_param) && cell->getParam(signed_param).as_bool())
				वापस inport(portname, Get_आकार(cell->getPort(portname))-1, inverter);
			वापस bool_गाँठ(inverter);
		}

		Aig_गाँठ गाँठ;
		गाँठ.portname = portname;
		गाँठ.portbit = portbit;
		गाँठ.inverter = inverter;
		वापस node2index(गाँठ);
	}

	vector<पूर्णांक> inport_vec(IdString portname, पूर्णांक width)
	{
		vector<पूर्णांक> vec;
		क्रम (पूर्णांक i = 0; i < width; i++)
			vec.धकेलो_back(inport(portname, i));
		वापस vec;
	}

	पूर्णांक not_inport(IdString portname, पूर्णांक portbit = 0)
	{
		वापस inport(portname, portbit, true);
	}

	पूर्णांक not_gate(पूर्णांक A)
	{
		Aig_गाँठ गाँठ(aig_indices[A]);
		गाँठ.outports.सफा();
		गाँठ.inverter = !गाँठ.inverter;
		वापस node2index(गाँठ);
	}

	पूर्णांक and_gate(पूर्णांक A, पूर्णांक B, bool inverter = false)
	{
		अगर (A == B)
			वापस inverter ? not_gate(A) : A;

		स्थिर Aig_गाँठ &nA = aig_indices[A];
		स्थिर Aig_गाँठ &nB = aig_indices[B];

		Aig_गाँठ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		अगर (nA == nB_inv)
			वापस bool_गाँठ(inverter);

		bool nA_bool = nA.portbit < 0 && nA.बायाँ_parent < 0 && nA.दायाँ_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.बायाँ_parent < 0 && nB.दायाँ_parent < 0;

		अगर (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			वापस bool_गाँठ(inverter != (bA && bB));
		}

		अगर (nA_bool) {
			bool bA = nA.inverter;
			अगर (inverter)
				वापस bA ? not_gate(B) : bool_गाँठ(true);
			वापस bA ? B : bool_गाँठ(false);
		}

		अगर (nB_bool) {
			bool bB = nB.inverter;
			अगर (inverter)
				वापस bB ? not_gate(A) : bool_गाँठ(true);
			वापस bB ? A : bool_गाँठ(false);
		}

		Aig_गाँठ गाँठ;
		गाँठ.inverter = inverter;
		गाँठ.बायाँ_parent = A;
		गाँठ.दायाँ_parent = B;
		वापस node2index(गाँठ);
	}

	पूर्णांक nand_gate(पूर्णांक A, पूर्णांक B)
	{
		वापस and_gate(A, B, true);
	}

	पूर्णांक or_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_a = not_gate(A);
		पूर्णांक not_b = not_gate(B);
		वापस nand_gate(not_a, not_b);
	}

	पूर्णांक nor_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_a = not_gate(A);
		पूर्णांक not_b = not_gate(B);
		वापस and_gate(not_a, not_b);
	}

	पूर्णांक xor_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक a_and_b = and_gate(A, B);
		पूर्णांक a_nor_b = nor_gate(A, B);
		वापस nor_gate(a_and_b, a_nor_b);
	}

	पूर्णांक xnor_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक a_and_b = and_gate(A, B);
		पूर्णांक a_nor_b = nor_gate(A, B);
		वापस or_gate(a_and_b, a_nor_b);
	}

	पूर्णांक andnot_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_b = not_gate(B);
		वापस and_gate(A, not_b);
	}

	पूर्णांक ornot_gate(पूर्णांक A, पूर्णांक B)
	{
		पूर्णांक not_b = not_gate(B);
		वापस or_gate(A, not_b);
	}

	पूर्णांक mux_gate(पूर्णांक A, पूर्णांक B, पूर्णांक S)
	{
		पूर्णांक not_s = not_gate(S);
		पूर्णांक a_active = and_gate(A, not_s);
		पूर्णांक b_active = and_gate(B, S);
		वापस or_gate(a_active, b_active);
	}

	vector<पूर्णांक> adder(स्थिर vector<पूर्णांक> &A, स्थिर vector<पूर्णांक> &B, पूर्णांक carry_in, vector<पूर्णांक> *X = nullptr, vector<पूर्णांक> *CO = nullptr)
	{
		vector<पूर्णांक> Y(Get_आकार(A));
		log_assert(Get_आकार(A) == Get_आकार(B));
		क्रम (पूर्णांक i = 0; i < Get_आकार(A); i++) {
			पूर्णांक a_xor_b = xor_gate(A[i], B[i]);
			पूर्णांक a_or_b = or_gate(A[i], B[i]);
			पूर्णांक a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			पूर्णांक tmp = and_gate(a_or_b, carry_in);
			पूर्णांक carry_out = or_gate(a_and_b, tmp);
			अगर (X != nullptr)
				X->at(i) = a_xor_b;
			अगर (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		वापस Y;
	}

	व्योम outport(पूर्णांक गाँठ, IdString portname, पूर्णांक portbit = 0)
	{
		अगर (portbit < Get_आकार(cell->getPort(portname)))
			aig->nodes.at(गाँठ).outports.धकेलो_back(pair<IdString, पूर्णांक>(portname, portbit));
	}

	व्योम outport_bool(पूर्णांक गाँठ, IdString portname)
	{
		outport(गाँठ, portname);
		क्रम (पूर्णांक i = 1; i < Get_आकार(cell->getPort(portname)); i++)
			outport(bool_गाँठ(false), portname, i);
	}

	व्योम outport_vec(स्थिर vector<पूर्णांक> &vec, IdString portname)
	{
		क्रम (पूर्णांक i = 0; i < Get_आकार(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	अगर (cell->type.isPublic())
		वापस;

	AigMaker mk(यह, cell);
	name = cell->type.str();

	string mkname_last;
	bool mkname_a_signed = false;
	bool mkname_b_signed = false;
	bool mkname_is_signed = false;

	cell->parameters.sort();
	क्रम (स्वतः p : cell->parameters)
	{
		अगर (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा अगर (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = false;
		mkname_b_signed = false;
		mkname_is_signed = false;
		अगर (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		अगर (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	अगर (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		क्रम (पूर्णांक i = 0; i < Get_आकार(cell->getPort(ID::Y)); i++) {
			पूर्णांक A = mk.inport(ID::A, i);
			पूर्णांक Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		जाओ optimize;
	}

	अगर (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		क्रम (पूर्णांक i = 0; i < Get_आकार(cell->getPort(ID::Y)); i++) {
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
		जाओ optimize;
	}

	अगर (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		पूर्णांक S = mk.inport(ID::S);
		क्रम (पूर्णांक i = 0; i < Get_आकार(cell->getPort(ID::Y)); i++) {
			पूर्णांक A = mk.inport(ID::A, i);
			पूर्णांक B = mk.inport(ID::B, i);
			पूर्णांक Y = mk.mux_gate(A, B, S);
			अगर (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		जाओ optimize;
	}

	अगर (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		पूर्णांक Y = mk.inport(ID::A, 0);
		क्रम (पूर्णांक i = 1; i < Get_आकार(cell->getPort(ID::A)); i++) {
			पूर्णांक A = mk.inport(ID::A, i);
			अगर (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			अगर (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			अगर (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			अगर (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			अगर (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		अगर (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		क्रम (पूर्णांक i = 1; i < Get_आकार(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_गाँठ(false), ID::Y, i);
		जाओ optimize;
	}

	अगर (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		पूर्णांक A = mk.inport(ID::A, 0), Y = -1;
		क्रम (पूर्णांक i = 1; i < Get_आकार(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		अगर (cell->type.in(ID($logic_and), ID($logic_or))) {
			पूर्णांक B = mk.inport(ID::B, 0);
			क्रम (पूर्णांक i = 1; i < Get_आकार(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			अगर (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			अगर (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} अन्यथा {
			अगर (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		जाओ optimize;
	}

	अगर (cell->type.in(ID($add), ID($sub)))
	{
		पूर्णांक width = Get_आकार(cell->getPort(ID::Y));
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);
		पूर्णांक carry = mk.bool_गाँठ(false);
		अगर (cell->type == ID($sub)) {
			क्रम (स्वतः &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<पूर्णांक> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		जाओ optimize;
	}

	अगर (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		पूर्णांक width = std::max(Get_आकार(cell->getPort(ID::A)),
							 Get_आकार(cell->getPort(ID::B))) + 1;
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);

		अगर (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		पूर्णांक carry = mk.bool_गाँठ(!cell->type.in(ID($le), ID($ge)));
		क्रम (स्वतः &n : B)
			n = mk.not_gate(n);
		vector<पूर्णांक> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		क्रम (पूर्णांक i = 1; i < Get_आकार(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_गाँठ(false), ID::Y, i);
		जाओ optimize;
	}

	अगर (cell->type == ID($alu))
	{
		पूर्णांक width = Get_आकार(cell->getPort(ID::Y));
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);
		पूर्णांक carry = mk.inport(ID::CI);
		पूर्णांक binv = mk.inport(ID::BI);
		क्रम (स्वतः &n : B)
			n = mk.xor_gate(n, binv);
		vector<पूर्णांक> X(width), CO(width);
		vector<पूर्णांक> Y = mk.adder(A, B, carry, &X, &CO);
		क्रम (पूर्णांक i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		जाओ optimize;
	}

	अगर (cell->type.in(ID($eq), ID($ne)))
	{
		पूर्णांक width = max(Get_आकार(cell->getPort(ID::A)), Get_आकार(cell->getPort(ID::B)));
		vector<पूर्णांक> A = mk.inport_vec(ID::A, width);
		vector<पूर्णांक> B = mk.inport_vec(ID::B, width);
		पूर्णांक Y = mk.bool_गाँठ(false);
		क्रम (पूर्णांक i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		अगर (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		जाओ optimize;
	}

	अगर (cell->type == ID($_AOI3_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		जाओ optimize;
	}

	अगर (cell->type == ID($_OAI3_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		जाओ optimize;
	}

	अगर (cell->type == ID($_AOI4_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक D = mk.inport(ID::D);
		पूर्णांक a_and_b = mk.and_gate(A, B);
		पूर्णांक Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		जाओ optimize;
	}

	अगर (cell->type == ID($_OAI4_))
	{
		पूर्णांक A = mk.inport(ID::A);
		पूर्णांक B = mk.inport(ID::B);
		पूर्णांक C = mk.inport(ID::C);
		पूर्णांक D = mk.inport(ID::D);
		पूर्णांक a_or_b = mk.or_gate(A, B);
		पूर्णांक Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		जाओ optimize;
	}

	name.सफा();
	वापस;

optimize:;
	pool<पूर्णांक> used_old_ids;
	vector<Aig_गाँठ> new_nodes;
	dict<पूर्णांक, पूर्णांक> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	क्रम (पूर्णांक i = Get_आकार(nodes)-1; i >= 0; i--) {
		अगर (!nodes[i].outports.empty())
			used_old_ids.जोड़ो(i);
		अगर (!used_old_ids.गिनती(i))
			जारी;
		अगर (nodes[i].बायाँ_parent >= 0)
			used_old_ids.जोड़ो(nodes[i].बायाँ_parent);
		अगर (nodes[i].दायाँ_parent >= 0)
			used_old_ids.जोड़ो(nodes[i].दायाँ_parent);
	}

	क्रम (पूर्णांक i = 0; i < Get_आकार(nodes); i++) {
		अगर (!used_old_ids.गिनती(i))
			जारी;
		nodes[i].बायाँ_parent = old_to_new_ids.at(nodes[i].बायाँ_parent);
		nodes[i].दायाँ_parent = old_to_new_ids.at(nodes[i].दायाँ_parent);
		old_to_new_ids[i] = Get_आकार(new_nodes);
		new_nodes.धकेलो_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
