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
	ако (portname != other.portname) върни false;
	ако (portbit != other.portbit) върни false;
	ако (inverter != other.inverter) върни false;
	ако (left_parent != other.left_parent) върни false;
	ако (right_parent != other.right_parent) върни false;
	върни true;
}

Hasher AigNode::hash_into(Hasher h) const
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	върни h;
}

bool Aig::operator==(const Aig &other) const
{
	върни name == other.name;
}

Hasher Aig::hash_into(Hasher h) const
{
	h.eat(name);
	върни h;
}

struct AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	цяло node2index(const AigNode &node)
	{
		ако (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			върни node2index(n);
		}

		ако (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		върни aig_indices.at(node);
	}

	цяло bool_node(bool value)
	{
		AigNode node;
		node.inverter = value;
		върни node2index(node);
	}

	цяло inport(IdString portname, цяло portbit = 0, bool inverter = false)
	{
		ако (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		ако (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				върни inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			върни bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		върни node2index(node);
	}

	vector<цяло> inport_vec(IdString portname, цяло width)
	{
		vector<цяло> vec;
		за (цяло i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		върни vec;
	}

	цяло not_inport(IdString portname, цяло portbit = 0)
	{
		върни inport(portname, portbit, true);
	}

	цяло not_gate(цяло A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		върни node2index(node);
	}

	цяло and_gate(цяло A, цяло B, bool inverter = false)
	{
		ако (A == B)
			върни inverter ? not_gate(A) : A;

		const AigNode &nA = aig_indices[A];
		const AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ако (nA == nB_inv)
			върни bool_node(inverter);

		bool nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		ако (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			върни bool_node(inverter != (bA && bB));
		}

		ако (nA_bool) {
			bool bA = nA.inverter;
			ако (inverter)
				върни bA ? not_gate(B) : bool_node(true);
			върни bA ? B : bool_node(false);
		}

		ако (nB_bool) {
			bool bB = nB.inverter;
			ако (inverter)
				върни bB ? not_gate(A) : bool_node(true);
			върни bB ? A : bool_node(false);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		върни node2index(node);
	}

	цяло nand_gate(цяло A, цяло B)
	{
		върни and_gate(A, B, true);
	}

	цяло or_gate(цяло A, цяло B)
	{
		цяло not_a = not_gate(A);
		цяло not_b = not_gate(B);
		върни nand_gate(not_a, not_b);
	}

	цяло nor_gate(цяло A, цяло B)
	{
		цяло not_a = not_gate(A);
		цяло not_b = not_gate(B);
		върни and_gate(not_a, not_b);
	}

	цяло xor_gate(цяло A, цяло B)
	{
		цяло a_and_b = and_gate(A, B);
		цяло a_nor_b = nor_gate(A, B);
		върни nor_gate(a_and_b, a_nor_b);
	}

	цяло xnor_gate(цяло A, цяло B)
	{
		цяло a_and_b = and_gate(A, B);
		цяло a_nor_b = nor_gate(A, B);
		върни or_gate(a_and_b, a_nor_b);
	}

	цяло andnot_gate(цяло A, цяло B)
	{
		цяло not_b = not_gate(B);
		върни and_gate(A, not_b);
	}

	цяло ornot_gate(цяло A, цяло B)
	{
		цяло not_b = not_gate(B);
		върни or_gate(A, not_b);
	}

	цяло mux_gate(цяло A, цяло B, цяло S)
	{
		цяло not_s = not_gate(S);
		цяло a_active = and_gate(A, not_s);
		цяло b_active = and_gate(B, S);
		върни or_gate(a_active, b_active);
	}

	vector<цяло> adder(const vector<цяло> &A, const vector<цяло> &B, цяло carry_in, vector<цяло> *X = nullptr, vector<цяло> *CO = nullptr)
	{
		vector<цяло> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		за (цяло i = 0; i < GetSize(A); i++) {
			цяло a_xor_b = xor_gate(A[i], B[i]);
			цяло a_or_b = or_gate(A[i], B[i]);
			цяло a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			цяло tmp = and_gate(a_or_b, carry_in);
			цяло carry_out = or_gate(a_and_b, tmp);
			ако (X != nullptr)
				X->at(i) = a_xor_b;
			ако (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		върни Y;
	}

	void outport(цяло node, IdString portname, цяло portbit = 0)
	{
		ако (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, цяло>(portname, portbit));
	}

	void outport_bool(цяло node, IdString portname)
	{
		outport(node, portname);
		за (цяло i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(false), portname, i);
	}

	void outport_vec(const vector<цяло> &vec, IdString portname)
	{
		за (цяло i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ако (cell->type.isPublic())
		върни;

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
		} иначе ако (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} иначе {
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
		за (цяло i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			цяло A = mk.inport(ID::A, i);
			цяло Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ако (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		за (цяло i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			цяло A = mk.inport(ID::A, i);
			цяло B = mk.inport(ID::B, i);
			цяло Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
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
		цяло S = mk.inport(ID::S);
		за (цяло i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			цяло A = mk.inport(ID::A, i);
			цяло B = mk.inport(ID::B, i);
			цяло Y = mk.mux_gate(A, B, S);
			ако (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ако (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		цяло Y = mk.inport(ID::A, 0);
		за (цяло i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			цяло A = mk.inport(ID::A, i);
			ако (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ако (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ако (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ако (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ако (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ако (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		за (цяло i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ако (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		цяло A = mk.inport(ID::A, 0), Y = -1;
		за (цяло i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ако (cell->type.in(ID($logic_and), ID($logic_or))) {
			цяло B = mk.inport(ID::B, 0);
			за (цяло i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ако (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ако (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} иначе {
			ако (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type.in(ID($add), ID($sub)))
	{
		цяло width = GetSize(cell->getPort(ID::Y));
		vector<цяло> A = mk.inport_vec(ID::A, width);
		vector<цяло> B = mk.inport_vec(ID::B, width);
		цяло carry = mk.bool_node(false);
		ако (cell->type == ID($sub)) {
			за (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<цяло> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		цяло width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<цяло> A = mk.inport_vec(ID::A, width);
		vector<цяло> B = mk.inport_vec(ID::B, width);

		ако (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		цяло carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		за (auto &n : B)
			n = mk.not_gate(n);
		vector<цяло> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		за (цяло i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ако (cell->type == ID($alu))
	{
		цяло width = GetSize(cell->getPort(ID::Y));
		vector<цяло> A = mk.inport_vec(ID::A, width);
		vector<цяло> B = mk.inport_vec(ID::B, width);
		цяло carry = mk.inport(ID::CI);
		цяло binv = mk.inport(ID::BI);
		за (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<цяло> X(width), CO(width);
		vector<цяло> Y = mk.adder(A, B, carry, &X, &CO);
		за (цяло i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		goto optimize;
	}

	ако (cell->type.in(ID($eq), ID($ne)))
	{
		цяло width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<цяло> A = mk.inport_vec(ID::A, width);
		vector<цяло> B = mk.inport_vec(ID::B, width);
		цяло Y = mk.bool_node(false);
		за (цяло i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ако (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_AOI3_))
	{
		цяло A = mk.inport(ID::A);
		цяло B = mk.inport(ID::B);
		цяло C = mk.inport(ID::C);
		цяло Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_OAI3_))
	{
		цяло A = mk.inport(ID::A);
		цяло B = mk.inport(ID::B);
		цяло C = mk.inport(ID::C);
		цяло Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_AOI4_))
	{
		цяло A = mk.inport(ID::A);
		цяло B = mk.inport(ID::B);
		цяло C = mk.inport(ID::C);
		цяло D = mk.inport(ID::D);
		цяло a_and_b = mk.and_gate(A, B);
		цяло Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ако (cell->type == ID($_OAI4_))
	{
		цяло A = mk.inport(ID::A);
		цяло B = mk.inport(ID::B);
		цяло C = mk.inport(ID::C);
		цяло D = mk.inport(ID::D);
		цяло a_or_b = mk.or_gate(A, B);
		цяло Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	name.clear();
	върни;

optimize:;
	pool<цяло> used_old_ids;
	vector<AigNode> new_nodes;
	dict<цяло, цяло> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	за (цяло i = GetSize(nodes)-1; i >= 0; i--) {
		ако (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		ако (!used_old_ids.count(i))
			continue;
		ако (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		ако (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	за (цяло i = 0; i < GetSize(nodes); i++) {
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
