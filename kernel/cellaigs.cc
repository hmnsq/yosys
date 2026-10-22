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

#include "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

AigNode::AigNode()
{
	portbit = -1;
	inverter = false;
	left_parent = -1;
	right_parent = -1;
}

bool AigNode::operator==(const AigNode &other) const
{
	ако (portname != other.portname) врати false;
	ако (portbit != other.portbit) врати false;
	ако (inverter != other.inverter) врати false;
	ако (left_parent != other.left_parent) врати false;
	ако (right_parent != other.right_parent) врати false;
	врати true;
}

Hasher AigNode::hash_into(Hasher h) const
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	врати h;
}

bool Aig::operator==(const Aig &other) const
{
	врати name == other.name;
}

Hasher Aig::hash_into(Hasher h) const
{
	h.eat(name);
	врати h;
}

struct AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	цео node2index(const AigNode &node)
	{
		ако (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			врати node2index(n);
		}

		ако (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		врати aig_indices.at(node);
	}

	цео bool_node(bool value)
	{
		AigNode node;
		node.inverter = value;
		врати node2index(node);
	}

	цео inport(IdString portname, цео portbit = 0, bool inverter = false)
	{
		ако (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		ако (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				врати inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			врати bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		врати node2index(node);
	}

	vector<цео> inport_vec(IdString portname, цео width)
	{
		vector<цео> vec;
		за (цео i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		врати vec;
	}

	цео not_inport(IdString portname, цео portbit = 0)
	{
		врати inport(portname, portbit, true);
	}

	цео not_gate(цео A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		врати node2index(node);
	}

	цео and_gate(цео A, цео B, bool inverter = false)
	{
		ако (A == B)
			врати inverter ? not_gate(A) : A;

		const AigNode &nA = aig_indices[A];
		const AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ако (nA == nB_inv)
			врати bool_node(inverter);

		bool nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		ако (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			врати bool_node(inverter != (bA && bB));
		}

		ако (nA_bool) {
			bool bA = nA.inverter;
			ако (inverter)
				врати bA ? not_gate(B) : bool_node(true);
			врати bA ? B : bool_node(false);
		}

		ако (nB_bool) {
			bool bB = nB.inverter;
			ако (inverter)
				врати bB ? not_gate(A) : bool_node(true);
			врати bB ? A : bool_node(false);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		врати node2index(node);
	}

	цео nand_gate(цео A, цео B)
	{
		врати and_gate(A, B, true);
	}

	цео or_gate(цео A, цео B)
	{
		цео not_a = not_gate(A);
		цео not_b = not_gate(B);
		врати nand_gate(not_a, not_b);
	}

	цео nor_gate(цео A, цео B)
	{
		цео not_a = not_gate(A);
		цео not_b = not_gate(B);
		врати and_gate(not_a, not_b);
	}

	цео xor_gate(цео A, цео B)
	{
		цео a_and_b = and_gate(A, B);
		цео a_nor_b = nor_gate(A, B);
		врати nor_gate(a_and_b, a_nor_b);
	}

	цео xnor_gate(цео A, цео B)
	{
		цео a_and_b = and_gate(A, B);
		цео a_nor_b = nor_gate(A, B);
		врати or_gate(a_and_b, a_nor_b);
	}

	цео andnot_gate(цео A, цео B)
	{
		цео not_b = not_gate(B);
		врати and_gate(A, not_b);
	}

	цео ornot_gate(цео A, цео B)
	{
		цео not_b = not_gate(B);
		врати or_gate(A, not_b);
	}

	цео mux_gate(цео A, цео B, цео S)
	{
		цео not_s = not_gate(S);
		цео a_active = and_gate(A, not_s);
		цео b_active = and_gate(B, S);
		врати or_gate(a_active, b_active);
	}

	vector<цео> adder(const vector<цео> &A, const vector<цео> &B, цео carry_in, vector<цео> *X = nullptr, vector<цео> *CO = nullptr)
	{
		vector<цео> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		за (цео i = 0; i < GetSize(A); i++) {
			цео a_xor_b = xor_gate(A[i], B[i]);
			цео a_or_b = or_gate(A[i], B[i]);
			цео a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			цео tmp = and_gate(a_or_b, carry_in);
			цео carry_out = or_gate(a_and_b, tmp);
			ако (X != nullptr)
				X->at(i) = a_xor_b;
			ако (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		врати Y;
	}

	void outport(цео node, IdString portname, цео portbit = 0)
	{
		ако (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, цео>(portname, portbit));
	}

	void outport_bool(цео node, IdString portname)
	{
		outport(node, portname);
		за (цео i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(false), portname, i);
	}

	void outport_vec(const vector<цео> &vec, IdString portname)
	{
		за (цео i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ако (cell->type.isPublic())
		врати;

	AigMaker mk(this, cell);
	name = cell->type.str();

	string mkname_last;
	bool mkname_a_signed = false;
	bool mkname_b_signed = false;
	bool mkname_is_signed = false;

	cell->parameters.sort();
	за (auto p : cell->parameters)
	{
		ако (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} инако ако (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} инако {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = false;
		mkname_b_signed = false;
		mkname_is_signed = false;
		ако (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		ако (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	ако (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		за (цео i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			цео A = mk.inport(ID::A, i);
			цео Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ако (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		за (цео i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			цео A = mk.inport(ID::A, i);
			цео B = mk.inport(ID::B, i);
			цео Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ако (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		цео S = mk.inport(ID::S);
		за (цео i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			цео A = mk.inport(ID::A, i);
			цео B = mk.inport(ID::B, i);
			цео Y = mk.mux_gate(A, B, S);
			ако (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ако (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		цео Y = mk.inport(ID::A, 0);
		за (цео i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			цео A = mk.inport(ID::A, i);
			ако (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ако (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ако (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ако (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ако (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ако (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		за (цео i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ако (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		цео A = mk.inport(ID::A, 0), Y = -1;
		за (цео i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ако (cell->type.in(ID($logic_and), ID($logic_or))) {
			цео B = mk.inport(ID::B, 0);
			за (цео i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ако (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ако (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} инако {
			ако (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type.in(ID($add), ID($sub)))
	{
		цео width = GetSize(cell->getPort(ID::Y));
		vector<цео> A = mk.inport_vec(ID::A, width);
		vector<цео> B = mk.inport_vec(ID::B, width);
		цео carry = mk.bool_node(false);
		ако (cell->type == ID($sub)) {
			за (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<цео> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		цео width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<цео> A = mk.inport_vec(ID::A, width);
		vector<цео> B = mk.inport_vec(ID::B, width);

		ако (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		цео carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		за (auto &n : B)
			n = mk.not_gate(n);
		vector<цео> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		за (цео i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ако (cell->type == ID($alu))
	{
		цео width = GetSize(cell->getPort(ID::Y));
		vector<цео> A = mk.inport_vec(ID::A, width);
		vector<цео> B = mk.inport_vec(ID::B, width);
		цео carry = mk.inport(ID::CI);
		цео binv = mk.inport(ID::BI);
		за (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<цео> X(width), CO(width);
		vector<цео> Y = mk.adder(A, B, carry, &X, &CO);
		за (цео i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		goto optimize;
	}

	ако (cell->type.in(ID($eq), ID($ne)))
	{
		цео width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<цео> A = mk.inport_vec(ID::A, width);
		vector<цео> B = mk.inport_vec(ID::B, width);
		цео Y = mk.bool_node(false);
		за (цео i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ако (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_AOI3_))
	{
		цео A = mk.inport(ID::A);
		цео B = mk.inport(ID::B);
		цео C = mk.inport(ID::C);
		цео Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_OAI3_))
	{
		цео A = mk.inport(ID::A);
		цео B = mk.inport(ID::B);
		цео C = mk.inport(ID::C);
		цео Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_AOI4_))
	{
		цео A = mk.inport(ID::A);
		цео B = mk.inport(ID::B);
		цео C = mk.inport(ID::C);
		цео D = mk.inport(ID::D);
		цео a_and_b = mk.and_gate(A, B);
		цео Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_OAI4_))
	{
		цео A = mk.inport(ID::A);
		цео B = mk.inport(ID::B);
		цео C = mk.inport(ID::C);
		цео D = mk.inport(ID::D);
		цео a_or_b = mk.or_gate(A, B);
		цео Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	name.clear();
	врати;

optimize:;
	pool<цео> used_old_ids;
	vector<AigNode> new_nodes;
	dict<цео, цео> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	за (цео i = GetSize(nodes)-1; i >= 0; i--) {
		ако (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		ако (!used_old_ids.count(i))
			continue;
		ако (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		ако (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	за (цео i = 0; i < GetSize(nodes); i++) {
		ако (!used_old_ids.count(i))
			continue;
		nodes[i].left_parent = old_to_new_ids.at(nodes[i].left_parent);
		nodes[i].right_parent = old_to_new_ids.at(nodes[i].right_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.push_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
