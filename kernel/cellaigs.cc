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
	အကယ်၍ (portname != other.portname) ပြန်ပေး false;
	အကယ်၍ (portbit != other.portbit) ပြန်ပေး false;
	အကယ်၍ (inverter != other.inverter) ပြန်ပေး false;
	အကယ်၍ (left_parent != other.left_parent) ပြန်ပေး false;
	အကယ်၍ (right_parent != other.right_parent) ပြန်ပေး false;
	ပြန်ပေး true;
}

Hasher AigNode::hash_into(Hasher h) const
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	ပြန်ပေး h;
}

bool Aig::operator==(const Aig &other) const
{
	ပြန်ပေး name == other.name;
}

Hasher Aig::hash_into(Hasher h) const
{
	h.eat(name);
	ပြန်ပေး h;
}

sa_nracanaa AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	ကိန်းပြည့် node2index(const AigNode &node)
	{
		အကယ်၍ (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			ပြန်ပေး node2index(n);
		}

		အကယ်၍ (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		ပြန်ပေး aig_indices.at(node);
	}

	ကိန်းပြည့် bool_node(bool value)
	{
		AigNode node;
		node.inverter = value;
		ပြန်ပေး node2index(node);
	}

	ကိန်းပြည့် inport(IdString portname, ကိန်းပြည့် portbit = 0, bool inverter = false)
	{
		အကယ်၍ (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		အကယ်၍ (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				ပြန်ပေး inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			ပြန်ပေး bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		ပြန်ပေး node2index(node);
	}

	vector<ကိန်းပြည့်> inport_vec(IdString portname, ကိန်းပြည့် width)
	{
		vector<ကိန်းပြည့်> vec;
		အတွက် (ကိန်းပြည့် i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		ပြန်ပေး vec;
	}

	ကိန်းပြည့် not_inport(IdString portname, ကိန်းပြည့် portbit = 0)
	{
		ပြန်ပေး inport(portname, portbit, true);
	}

	ကိန်းပြည့် not_gate(ကိန်းပြည့် A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		ပြန်ပေး node2index(node);
	}

	ကိန်းပြည့် and_gate(ကိန်းပြည့် A, ကိန်းပြည့် B, bool inverter = false)
	{
		အကယ်၍ (A == B)
			ပြန်ပေး inverter ? not_gate(A) : A;

		const AigNode &nA = aig_indices[A];
		const AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		အကယ်၍ (nA == nB_inv)
			ပြန်ပေး bool_node(inverter);

		bool nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		အကယ်၍ (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			ပြန်ပေး bool_node(inverter != (bA && bB));
		}

		အကယ်၍ (nA_bool) {
			bool bA = nA.inverter;
			အကယ်၍ (inverter)
				ပြန်ပေး bA ? not_gate(B) : bool_node(true);
			ပြန်ပေး bA ? B : bool_node(false);
		}

		အကယ်၍ (nB_bool) {
			bool bB = nB.inverter;
			အကယ်၍ (inverter)
				ပြန်ပေး bB ? not_gate(A) : bool_node(true);
			ပြန်ပေး bB ? A : bool_node(false);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		ပြန်ပေး node2index(node);
	}

	ကိန်းပြည့် nand_gate(ကိန်းပြည့် A, ကိန်းပြည့် B)
	{
		ပြန်ပေး and_gate(A, B, true);
	}

	ကိန်းပြည့် or_gate(ကိန်းပြည့် A, ကိန်းပြည့် B)
	{
		ကိန်းပြည့် not_a = not_gate(A);
		ကိန်းပြည့် not_b = not_gate(B);
		ပြန်ပေး nand_gate(not_a, not_b);
	}

	ကိန်းပြည့် nor_gate(ကိန်းပြည့် A, ကိန်းပြည့် B)
	{
		ကိန်းပြည့် not_a = not_gate(A);
		ကိန်းပြည့် not_b = not_gate(B);
		ပြန်ပေး and_gate(not_a, not_b);
	}

	ကိန်းပြည့် xor_gate(ကိန်းပြည့် A, ကိန်းပြည့် B)
	{
		ကိန်းပြည့် a_and_b = and_gate(A, B);
		ကိန်းပြည့် a_nor_b = nor_gate(A, B);
		ပြန်ပေး nor_gate(a_and_b, a_nor_b);
	}

	ကိန်းပြည့် xnor_gate(ကိန်းပြည့် A, ကိန်းပြည့် B)
	{
		ကိန်းပြည့် a_and_b = and_gate(A, B);
		ကိန်းပြည့် a_nor_b = nor_gate(A, B);
		ပြန်ပေး or_gate(a_and_b, a_nor_b);
	}

	ကိန်းပြည့် andnot_gate(ကိန်းပြည့် A, ကိန်းပြည့် B)
	{
		ကိန်းပြည့် not_b = not_gate(B);
		ပြန်ပေး and_gate(A, not_b);
	}

	ကိန်းပြည့် ornot_gate(ကိန်းပြည့် A, ကိန်းပြည့် B)
	{
		ကိန်းပြည့် not_b = not_gate(B);
		ပြန်ပေး or_gate(A, not_b);
	}

	ကိန်းပြည့် mux_gate(ကိန်းပြည့် A, ကိန်းပြည့် B, ကိန်းပြည့် S)
	{
		ကိန်းပြည့် not_s = not_gate(S);
		ကိန်းပြည့် a_active = and_gate(A, not_s);
		ကိန်းပြည့် b_active = and_gate(B, S);
		ပြန်ပေး or_gate(a_active, b_active);
	}

	vector<ကိန်းပြည့်> adder(const vector<ကိန်းပြည့်> &A, const vector<ကိန်းပြည့်> &B, ကိန်းပြည့် carry_in, vector<ကိန်းပြည့်> *X = nullptr, vector<ကိန်းပြည့်> *CO = nullptr)
	{
		vector<ကိန်းပြည့်> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		အတွက် (ကိန်းပြည့် i = 0; i < GetSize(A); i++) {
			ကိန်းပြည့် a_xor_b = xor_gate(A[i], B[i]);
			ကိန်းပြည့် a_or_b = or_gate(A[i], B[i]);
			ကိန်းပြည့် a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			ကိန်းပြည့် tmp = and_gate(a_or_b, carry_in);
			ကိန်းပြည့် carry_out = or_gate(a_and_b, tmp);
			အကယ်၍ (X != nullptr)
				X->at(i) = a_xor_b;
			အကယ်၍ (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		ပြန်ပေး Y;
	}

	void outport(ကိန်းပြည့် node, IdString portname, ကိန်းပြည့် portbit = 0)
	{
		အကယ်၍ (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, ကိန်းပြည့်>(portname, portbit));
	}

	void outport_bool(ကိန်းပြည့် node, IdString portname)
	{
		outport(node, portname);
		အတွက် (ကိန်းပြည့် i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(false), portname, i);
	}

	void outport_vec(const vector<ကိန်းပြည့်> &vec, IdString portname)
	{
		အတွက် (ကိန်းပြည့် i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	အကယ်၍ (cell->type.isPublic())
		ပြန်ပေး;

	AigMaker mk(this, cell);
	name = cell->type.str();

	string mkname_last;
	bool mkname_a_signed = false;
	bool mkname_b_signed = false;
	bool mkname_is_signed = false;

	cell->parameters.sort();
	အတွက် (auto p : cell->parameters)
	{
		အကယ်၍ (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} မဟုတ်ပါက အကယ်၍ (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} မဟုတ်ပါက {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = false;
		mkname_b_signed = false;
		mkname_is_signed = false;
		အကယ်၍ (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		အကယ်၍ (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	အကယ်၍ (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		အတွက် (ကိန်းပြည့် i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ကိန်းပြည့် A = mk.inport(ID::A, i);
			ကိန်းပြည့် Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	အကယ်၍ (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		အတွက် (ကိန်းပြည့် i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ကိန်းပြည့် A = mk.inport(ID::A, i);
			ကိန်းပြည့် B = mk.inport(ID::B, i);
			ကိန်းပြည့် Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
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

	အကယ်၍ (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		ကိန်းပြည့် S = mk.inport(ID::S);
		အတွက် (ကိန်းပြည့် i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ကိန်းပြည့် A = mk.inport(ID::A, i);
			ကိန်းပြည့် B = mk.inport(ID::B, i);
			ကိန်းပြည့် Y = mk.mux_gate(A, B, S);
			အကယ်၍ (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	အကယ်၍ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		ကိန်းပြည့် Y = mk.inport(ID::A, 0);
		အတွက် (ကိန်းပြည့် i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			ကိန်းပြည့် A = mk.inport(ID::A, i);
			အကယ်၍ (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			အကယ်၍ (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			အကယ်၍ (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			အကယ်၍ (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			အကယ်၍ (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		အကယ်၍ (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		အတွက် (ကိန်းပြည့် i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	အကယ်၍ (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		ကိန်းပြည့် A = mk.inport(ID::A, 0), Y = -1;
		အတွက် (ကိန်းပြည့် i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		အကယ်၍ (cell->type.in(ID($logic_and), ID($logic_or))) {
			ကိန်းပြည့် B = mk.inport(ID::B, 0);
			အတွက် (ကိန်းပြည့် i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			အကယ်၍ (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			အကယ်၍ (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} မဟုတ်ပါက {
			အကယ်၍ (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	အကယ်၍ (cell->type.in(ID($add), ID($sub)))
	{
		ကိန်းပြည့် width = GetSize(cell->getPort(ID::Y));
		vector<ကိန်းပြည့်> A = mk.inport_vec(ID::A, width);
		vector<ကိန်းပြည့်> B = mk.inport_vec(ID::B, width);
		ကိန်းပြည့် carry = mk.bool_node(false);
		အကယ်၍ (cell->type == ID($sub)) {
			အတွက် (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<ကိန်းပြည့်> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		goto optimize;
	}

	အကယ်၍ (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		ကိန်းပြည့် width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<ကိန်းပြည့်> A = mk.inport_vec(ID::A, width);
		vector<ကိန်းပြည့်> B = mk.inport_vec(ID::B, width);

		အကယ်၍ (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		ကိန်းပြည့် carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		အတွက် (auto &n : B)
			n = mk.not_gate(n);
		vector<ကိန်းပြည့်> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		အတွက် (ကိန်းပြည့် i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	အကယ်၍ (cell->type == ID($alu))
	{
		ကိန်းပြည့် width = GetSize(cell->getPort(ID::Y));
		vector<ကိန်းပြည့်> A = mk.inport_vec(ID::A, width);
		vector<ကိန်းပြည့်> B = mk.inport_vec(ID::B, width);
		ကိန်းပြည့် carry = mk.inport(ID::CI);
		ကိန်းပြည့် binv = mk.inport(ID::BI);
		အတွက် (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<ကိန်းပြည့်> X(width), CO(width);
		vector<ကိန်းပြည့်> Y = mk.adder(A, B, carry, &X, &CO);
		အတွက် (ကိန်းပြည့် i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		goto optimize;
	}

	အကယ်၍ (cell->type.in(ID($eq), ID($ne)))
	{
		ကိန်းပြည့် width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<ကိန်းပြည့်> A = mk.inport_vec(ID::A, width);
		vector<ကိန်းပြည့်> B = mk.inport_vec(ID::B, width);
		ကိန်းပြည့် Y = mk.bool_node(false);
		အတွက် (ကိန်းပြည့် i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		အကယ်၍ (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	အကယ်၍ (cell->type == ID($_AOI3_))
	{
		ကိန်းပြည့် A = mk.inport(ID::A);
		ကိန်းပြည့် B = mk.inport(ID::B);
		ကိန်းပြည့် C = mk.inport(ID::C);
		ကိန်းပြည့် Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	အကယ်၍ (cell->type == ID($_OAI3_))
	{
		ကိန်းပြည့် A = mk.inport(ID::A);
		ကိန်းပြည့် B = mk.inport(ID::B);
		ကိန်းပြည့် C = mk.inport(ID::C);
		ကိန်းပြည့် Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	အကယ်၍ (cell->type == ID($_AOI4_))
	{
		ကိန်းပြည့် A = mk.inport(ID::A);
		ကိန်းပြည့် B = mk.inport(ID::B);
		ကိန်းပြည့် C = mk.inport(ID::C);
		ကိန်းပြည့် D = mk.inport(ID::D);
		ကိန်းပြည့် a_and_b = mk.and_gate(A, B);
		ကိန်းပြည့် Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	အကယ်၍ (cell->type == ID($_OAI4_))
	{
		ကိန်းပြည့် A = mk.inport(ID::A);
		ကိန်းပြည့် B = mk.inport(ID::B);
		ကိန်းပြည့် C = mk.inport(ID::C);
		ကိန်းပြည့် D = mk.inport(ID::D);
		ကိန်းပြည့် a_or_b = mk.or_gate(A, B);
		ကိန်းပြည့် Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	name.clear();
	ပြန်ပေး;

optimize:;
	pool<ကိန်းပြည့်> used_old_ids;
	vector<AigNode> new_nodes;
	dict<ကိန်းပြည့်, ကိန်းပြည့်> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	အတွက် (ကိန်းပြည့် i = GetSize(nodes)-1; i >= 0; i--) {
		အကယ်၍ (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		အကယ်၍ (!used_old_ids.count(i))
			continue;
		အကယ်၍ (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		အကယ်၍ (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	အတွက် (ကိန်းပြည့် i = 0; i < GetSize(nodes); i++) {
		အကယ်၍ (!used_old_ids.count(i))
			continue;
		nodes[i].left_parent = old_to_new_ids.at(nodes[i].left_parent);
		nodes[i].right_parent = old_to_new_ids.at(nodes[i].right_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.push_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
