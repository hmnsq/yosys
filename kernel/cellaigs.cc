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

#ضمن "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

AigNode::AigNode()
{
	portbit = -1;
	inverter = שקר;
	left_parent = -1;
	right_parent = -1;
}

منطقي AigNode::operator==(ثابت AigNode &other) ثابت
{
	אם (portname != other.portname) החזר שקר;
	אם (portbit != other.portbit) החזר שקר;
	אם (inverter != other.inverter) החזר שקר;
	אם (left_parent != other.left_parent) החזר שקר;
	אם (right_parent != other.right_parent) החזר שקר;
	החזר אמת;
}

Hasher AigNode::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	החזר h;
}

منطقي Aig::operator==(ثابت Aig &other) ثابت
{
	החזר name == other.name;
}

Hasher Aig::hash_into(Hasher h) ثابت
{
	h.eat(name);
	החזר h;
}

بنية AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحيح node2index(ثابت AigNode &node)
	{
		אם (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			החזר node2index(n);
		}

		אם (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		החזר aig_indices.at(node);
	}

	صحيح bool_node(منطقي value)
	{
		AigNode node;
		node.inverter = value;
		החזר node2index(node);
	}

	صحيح inport(IdString portname, صحيح portbit = 0, منطقي inverter = שקר)
	{
		אם (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		אם (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				החזר inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			החזר bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		החזר node2index(node);
	}

	vector<صحيح> inport_vec(IdString portname, صحيح width)
	{
		vector<صحيح> vec;
		עבור (صحيح i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		החזר vec;
	}

	صحيح not_inport(IdString portname, صحيح portbit = 0)
	{
		החזר inport(portname, portbit, אמת);
	}

	صحيح not_gate(صحيح A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		החזר node2index(node);
	}

	صحيح and_gate(صحيح A, صحيح B, منطقي inverter = שקר)
	{
		אם (A == B)
			החזר inverter ? not_gate(A) : A;

		ثابت AigNode &nA = aig_indices[A];
		ثابت AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		אם (nA == nB_inv)
			החזר bool_node(inverter);

		منطقي nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		منطقي nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		אם (nA_bool && nB_bool) {
			منطقي bA = nA.inverter;
			منطقي bB = nB.inverter;
			החזר bool_node(inverter != (bA && bB));
		}

		אם (nA_bool) {
			منطقي bA = nA.inverter;
			אם (inverter)
				החזר bA ? not_gate(B) : bool_node(אמת);
			החזר bA ? B : bool_node(שקר);
		}

		אם (nB_bool) {
			منطقي bB = nB.inverter;
			אם (inverter)
				החזר bB ? not_gate(A) : bool_node(אמת);
			החזר bB ? A : bool_node(שקר);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		החזר node2index(node);
	}

	صحيح nand_gate(صحيح A, صحيح B)
	{
		החזר and_gate(A, B, אמת);
	}

	صحيح or_gate(صحيح A, صحيح B)
	{
		صحيح not_a = not_gate(A);
		صحيح not_b = not_gate(B);
		החזר nand_gate(not_a, not_b);
	}

	صحيح nor_gate(صحيح A, صحيح B)
	{
		صحيح not_a = not_gate(A);
		صحيح not_b = not_gate(B);
		החזר and_gate(not_a, not_b);
	}

	صحيح xor_gate(صحيح A, صحيح B)
	{
		صحيح a_and_b = and_gate(A, B);
		صحيح a_nor_b = nor_gate(A, B);
		החזר nor_gate(a_and_b, a_nor_b);
	}

	صحيح xnor_gate(صحيح A, صحيح B)
	{
		صحيح a_and_b = and_gate(A, B);
		صحيح a_nor_b = nor_gate(A, B);
		החזר or_gate(a_and_b, a_nor_b);
	}

	صحيح andnot_gate(صحيح A, صحيح B)
	{
		صحيح not_b = not_gate(B);
		החזר and_gate(A, not_b);
	}

	صحيح ornot_gate(صحيح A, صحيح B)
	{
		صحيح not_b = not_gate(B);
		החזר or_gate(A, not_b);
	}

	صحيح mux_gate(صحيح A, صحيح B, صحيح S)
	{
		صحيح not_s = not_gate(S);
		صحيح a_active = and_gate(A, not_s);
		صحيح b_active = and_gate(B, S);
		החזר or_gate(a_active, b_active);
	}

	vector<صحيح> adder(ثابت vector<صحيح> &A, ثابت vector<صحيح> &B, صحيح carry_in, vector<صحيح> *X = nullptr, vector<صحيح> *CO = nullptr)
	{
		vector<صحيح> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		עבור (صحيح i = 0; i < GetSize(A); i++) {
			صحيح a_xor_b = xor_gate(A[i], B[i]);
			صحيح a_or_b = or_gate(A[i], B[i]);
			صحيح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			صحيح tmp = and_gate(a_or_b, carry_in);
			صحيح carry_out = or_gate(a_and_b, tmp);
			אם (X != nullptr)
				X->at(i) = a_xor_b;
			אם (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		החזר Y;
	}

	فراغ outport(صحيح node, IdString portname, صحيح portbit = 0)
	{
		אם (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, صحيح>(portname, portbit));
	}

	فراغ outport_bool(صحيح node, IdString portname)
	{
		outport(node, portname);
		עבור (صحيح i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(שקר), portname, i);
	}

	فراغ outport_vec(ثابت vector<صحيح> &vec, IdString portname)
	{
		עבור (صحيح i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	אם (cell->type.isPublic())
		החזר;

	AigMaker mk(هذا, cell);
	name = cell->type.str();

	string mkname_last;
	منطقي mkname_a_signed = שקר;
	منطقي mkname_b_signed = שקר;
	منطقي mkname_is_signed = שקר;

	cell->parameters.sort();
	עבור (auto p : cell->parameters)
	{
		אם (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} אחרת אם (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} אחרת {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = שקר;
		mkname_b_signed = שקר;
		mkname_is_signed = שקר;
		אם (p.first == ID::A_SIGNED) {
			mkname_a_signed = אמת;
			mkname_is_signed = p.second.as_bool();
		}
		אם (p.first == ID::B_SIGNED) {
			mkname_b_signed = אמת;
			mkname_is_signed = p.second.as_bool();
		}
	}

	אם (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		עבור (صحيح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	אם (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		עבור (صحيح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	אם (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحيح S = mk.inport(ID::S);
		עבור (صحيح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = mk.mux_gate(A, B, S);
			אם (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	אם (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحيح Y = mk.inport(ID::A, 0);
		עבור (صحيح i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			صحيح A = mk.inport(ID::A, i);
			אם (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			אם (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			אם (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			אם (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			אם (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		אם (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		עבור (صحيح i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(שקר), ID::Y, i);
		اذهب optimize;
	}

	אם (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحيح A = mk.inport(ID::A, 0), Y = -1;
		עבור (صحيح i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		אם (cell->type.in(ID($logic_and), ID($logic_or))) {
			صحيح B = mk.inport(ID::B, 0);
			עבור (صحيح i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			אם (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			אם (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} אחרת {
			אם (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	אם (cell->type.in(ID($add), ID($sub)))
	{
		صحيح width = GetSize(cell->getPort(ID::Y));
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);
		صحيح carry = mk.bool_node(שקר);
		אם (cell->type == ID($sub)) {
			עבור (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<صحيح> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		اذهب optimize;
	}

	אם (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحيح width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);

		אם (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		صحيح carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		עבור (auto &n : B)
			n = mk.not_gate(n);
		vector<صحيح> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		עבור (صحيح i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(שקר), ID::Y, i);
		اذهب optimize;
	}

	אם (cell->type == ID($alu))
	{
		صحيح width = GetSize(cell->getPort(ID::Y));
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);
		صحيح carry = mk.inport(ID::CI);
		صحيح binv = mk.inport(ID::BI);
		עבור (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<صحيح> X(width), CO(width);
		vector<صحيح> Y = mk.adder(A, B, carry, &X, &CO);
		עבור (صحيح i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		اذهب optimize;
	}

	אם (cell->type.in(ID($eq), ID($ne)))
	{
		صحيح width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);
		صحيح Y = mk.bool_node(שקר);
		עבור (صحيح i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		אם (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	אם (cell->type == ID($_AOI3_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	אם (cell->type == ID($_OAI3_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	אם (cell->type == ID($_AOI4_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح D = mk.inport(ID::D);
		صحيح a_and_b = mk.and_gate(A, B);
		صحيح Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	אם (cell->type == ID($_OAI4_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح D = mk.inport(ID::D);
		صحيح a_or_b = mk.or_gate(A, B);
		صحيح Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	name.clear();
	החזר;

optimize:;
	pool<صحيح> used_old_ids;
	vector<AigNode> new_nodes;
	dict<صحيح, صحيح> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	עבור (صحيح i = GetSize(nodes)-1; i >= 0; i--) {
		אם (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		אם (!used_old_ids.count(i))
			استمر;
		אם (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		אם (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	עבור (صحيح i = 0; i < GetSize(nodes); i++) {
		אם (!used_old_ids.count(i))
			استمر;
		nodes[i].left_parent = old_to_new_ids.at(nodes[i].left_parent);
		nodes[i].right_parent = old_to_new_ids.at(nodes[i].right_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.push_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
