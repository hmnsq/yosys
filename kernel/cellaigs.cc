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
	ถ้า (portname != other.portname) คืน false;
	ถ้า (portbit != other.portbit) คืน false;
	ถ้า (inverter != other.inverter) คืน false;
	ถ้า (left_parent != other.left_parent) คืน false;
	ถ้า (right_parent != other.right_parent) คืน false;
	คืน true;
}

Hasher AigNode::hash_into(Hasher h) const
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	คืน h;
}

bool Aig::operator==(const Aig &other) const
{
	คืน name == other.name;
}

Hasher Aig::hash_into(Hasher h) const
{
	h.eat(name);
	คืน h;
}

sa_nracanaa AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	จำนวนเต็ม node2index(const AigNode &node)
	{
		ถ้า (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			คืน node2index(n);
		}

		ถ้า (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		คืน aig_indices.at(node);
	}

	จำนวนเต็ม bool_node(bool value)
	{
		AigNode node;
		node.inverter = value;
		คืน node2index(node);
	}

	จำนวนเต็ม inport(IdString portname, จำนวนเต็ม portbit = 0, bool inverter = false)
	{
		ถ้า (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		ถ้า (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				คืน inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			คืน bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		คืน node2index(node);
	}

	vector<จำนวนเต็ม> inport_vec(IdString portname, จำนวนเต็ม width)
	{
		vector<จำนวนเต็ม> vec;
		สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		คืน vec;
	}

	จำนวนเต็ม not_inport(IdString portname, จำนวนเต็ม portbit = 0)
	{
		คืน inport(portname, portbit, true);
	}

	จำนวนเต็ม not_gate(จำนวนเต็ม A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		คืน node2index(node);
	}

	จำนวนเต็ม and_gate(จำนวนเต็ม A, จำนวนเต็ม B, bool inverter = false)
	{
		ถ้า (A == B)
			คืน inverter ? not_gate(A) : A;

		const AigNode &nA = aig_indices[A];
		const AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ถ้า (nA == nB_inv)
			คืน bool_node(inverter);

		bool nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		ถ้า (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			คืน bool_node(inverter != (bA && bB));
		}

		ถ้า (nA_bool) {
			bool bA = nA.inverter;
			ถ้า (inverter)
				คืน bA ? not_gate(B) : bool_node(true);
			คืน bA ? B : bool_node(false);
		}

		ถ้า (nB_bool) {
			bool bB = nB.inverter;
			ถ้า (inverter)
				คืน bB ? not_gate(A) : bool_node(true);
			คืน bB ? A : bool_node(false);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		คืน node2index(node);
	}

	จำนวนเต็ม nand_gate(จำนวนเต็ม A, จำนวนเต็ม B)
	{
		คืน and_gate(A, B, true);
	}

	จำนวนเต็ม or_gate(จำนวนเต็ม A, จำนวนเต็ม B)
	{
		จำนวนเต็ม not_a = not_gate(A);
		จำนวนเต็ม not_b = not_gate(B);
		คืน nand_gate(not_a, not_b);
	}

	จำนวนเต็ม nor_gate(จำนวนเต็ม A, จำนวนเต็ม B)
	{
		จำนวนเต็ม not_a = not_gate(A);
		จำนวนเต็ม not_b = not_gate(B);
		คืน and_gate(not_a, not_b);
	}

	จำนวนเต็ม xor_gate(จำนวนเต็ม A, จำนวนเต็ม B)
	{
		จำนวนเต็ม a_and_b = and_gate(A, B);
		จำนวนเต็ม a_nor_b = nor_gate(A, B);
		คืน nor_gate(a_and_b, a_nor_b);
	}

	จำนวนเต็ม xnor_gate(จำนวนเต็ม A, จำนวนเต็ม B)
	{
		จำนวนเต็ม a_and_b = and_gate(A, B);
		จำนวนเต็ม a_nor_b = nor_gate(A, B);
		คืน or_gate(a_and_b, a_nor_b);
	}

	จำนวนเต็ม andnot_gate(จำนวนเต็ม A, จำนวนเต็ม B)
	{
		จำนวนเต็ม not_b = not_gate(B);
		คืน and_gate(A, not_b);
	}

	จำนวนเต็ม ornot_gate(จำนวนเต็ม A, จำนวนเต็ม B)
	{
		จำนวนเต็ม not_b = not_gate(B);
		คืน or_gate(A, not_b);
	}

	จำนวนเต็ม mux_gate(จำนวนเต็ม A, จำนวนเต็ม B, จำนวนเต็ม S)
	{
		จำนวนเต็ม not_s = not_gate(S);
		จำนวนเต็ม a_active = and_gate(A, not_s);
		จำนวนเต็ม b_active = and_gate(B, S);
		คืน or_gate(a_active, b_active);
	}

	vector<จำนวนเต็ม> adder(const vector<จำนวนเต็ม> &A, const vector<จำนวนเต็ม> &B, จำนวนเต็ม carry_in, vector<จำนวนเต็ม> *X = nullptr, vector<จำนวนเต็ม> *CO = nullptr)
	{
		vector<จำนวนเต็ม> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		สำหรับ (จำนวนเต็ม i = 0; i < GetSize(A); i++) {
			จำนวนเต็ม a_xor_b = xor_gate(A[i], B[i]);
			จำนวนเต็ม a_or_b = or_gate(A[i], B[i]);
			จำนวนเต็ม a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			จำนวนเต็ม tmp = and_gate(a_or_b, carry_in);
			จำนวนเต็ม carry_out = or_gate(a_and_b, tmp);
			ถ้า (X != nullptr)
				X->at(i) = a_xor_b;
			ถ้า (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		คืน Y;
	}

	void outport(จำนวนเต็ม node, IdString portname, จำนวนเต็ม portbit = 0)
	{
		ถ้า (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, จำนวนเต็ม>(portname, portbit));
	}

	void outport_bool(จำนวนเต็ม node, IdString portname)
	{
		outport(node, portname);
		สำหรับ (จำนวนเต็ม i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(false), portname, i);
	}

	void outport_vec(const vector<จำนวนเต็ม> &vec, IdString portname)
	{
		สำหรับ (จำนวนเต็ม i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ถ้า (cell->type.isPublic())
		คืน;

	AigMaker mk(this, cell);
	name = cell->type.str();

	string mkname_last;
	bool mkname_a_signed = false;
	bool mkname_b_signed = false;
	bool mkname_is_signed = false;

	cell->parameters.sort();
	สำหรับ (auto p : cell->parameters)
	{
		ถ้า (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} มิฉะนั้น ถ้า (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} มิฉะนั้น {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = false;
		mkname_b_signed = false;
		mkname_is_signed = false;
		ถ้า (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		ถ้า (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	ถ้า (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		สำหรับ (จำนวนเต็ม i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			จำนวนเต็ม A = mk.inport(ID::A, i);
			จำนวนเต็ม Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ถ้า (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		สำหรับ (จำนวนเต็ม i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			จำนวนเต็ม A = mk.inport(ID::A, i);
			จำนวนเต็ม B = mk.inport(ID::B, i);
			จำนวนเต็ม Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
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

	ถ้า (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		จำนวนเต็ม S = mk.inport(ID::S);
		สำหรับ (จำนวนเต็ม i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			จำนวนเต็ม A = mk.inport(ID::A, i);
			จำนวนเต็ม B = mk.inport(ID::B, i);
			จำนวนเต็ม Y = mk.mux_gate(A, B, S);
			ถ้า (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ถ้า (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		จำนวนเต็ม Y = mk.inport(ID::A, 0);
		สำหรับ (จำนวนเต็ม i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			จำนวนเต็ม A = mk.inport(ID::A, i);
			ถ้า (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ถ้า (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ถ้า (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ถ้า (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ถ้า (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ถ้า (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		สำหรับ (จำนวนเต็ม i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ถ้า (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		จำนวนเต็ม A = mk.inport(ID::A, 0), Y = -1;
		สำหรับ (จำนวนเต็ม i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ถ้า (cell->type.in(ID($logic_and), ID($logic_or))) {
			จำนวนเต็ม B = mk.inport(ID::B, 0);
			สำหรับ (จำนวนเต็ม i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ถ้า (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ถ้า (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} มิฉะนั้น {
			ถ้า (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ถ้า (cell->type.in(ID($add), ID($sub)))
	{
		จำนวนเต็ม width = GetSize(cell->getPort(ID::Y));
		vector<จำนวนเต็ม> A = mk.inport_vec(ID::A, width);
		vector<จำนวนเต็ม> B = mk.inport_vec(ID::B, width);
		จำนวนเต็ม carry = mk.bool_node(false);
		ถ้า (cell->type == ID($sub)) {
			สำหรับ (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<จำนวนเต็ม> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		goto optimize;
	}

	ถ้า (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		จำนวนเต็ม width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<จำนวนเต็ม> A = mk.inport_vec(ID::A, width);
		vector<จำนวนเต็ม> B = mk.inport_vec(ID::B, width);

		ถ้า (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		จำนวนเต็ม carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		สำหรับ (auto &n : B)
			n = mk.not_gate(n);
		vector<จำนวนเต็ม> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		สำหรับ (จำนวนเต็ม i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ถ้า (cell->type == ID($alu))
	{
		จำนวนเต็ม width = GetSize(cell->getPort(ID::Y));
		vector<จำนวนเต็ม> A = mk.inport_vec(ID::A, width);
		vector<จำนวนเต็ม> B = mk.inport_vec(ID::B, width);
		จำนวนเต็ม carry = mk.inport(ID::CI);
		จำนวนเต็ม binv = mk.inport(ID::BI);
		สำหรับ (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<จำนวนเต็ม> X(width), CO(width);
		vector<จำนวนเต็ม> Y = mk.adder(A, B, carry, &X, &CO);
		สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		goto optimize;
	}

	ถ้า (cell->type.in(ID($eq), ID($ne)))
	{
		จำนวนเต็ม width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<จำนวนเต็ม> A = mk.inport_vec(ID::A, width);
		vector<จำนวนเต็ม> B = mk.inport_vec(ID::B, width);
		จำนวนเต็ม Y = mk.bool_node(false);
		สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ถ้า (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ถ้า (cell->type == ID($_AOI3_))
	{
		จำนวนเต็ม A = mk.inport(ID::A);
		จำนวนเต็ม B = mk.inport(ID::B);
		จำนวนเต็ม C = mk.inport(ID::C);
		จำนวนเต็ม Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ถ้า (cell->type == ID($_OAI3_))
	{
		จำนวนเต็ม A = mk.inport(ID::A);
		จำนวนเต็ม B = mk.inport(ID::B);
		จำนวนเต็ม C = mk.inport(ID::C);
		จำนวนเต็ม Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ถ้า (cell->type == ID($_AOI4_))
	{
		จำนวนเต็ม A = mk.inport(ID::A);
		จำนวนเต็ม B = mk.inport(ID::B);
		จำนวนเต็ม C = mk.inport(ID::C);
		จำนวนเต็ม D = mk.inport(ID::D);
		จำนวนเต็ม a_and_b = mk.and_gate(A, B);
		จำนวนเต็ม Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ถ้า (cell->type == ID($_OAI4_))
	{
		จำนวนเต็ม A = mk.inport(ID::A);
		จำนวนเต็ม B = mk.inport(ID::B);
		จำนวนเต็ม C = mk.inport(ID::C);
		จำนวนเต็ม D = mk.inport(ID::D);
		จำนวนเต็ม a_or_b = mk.or_gate(A, B);
		จำนวนเต็ม Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	name.clear();
	คืน;

optimize:;
	pool<จำนวนเต็ม> used_old_ids;
	vector<AigNode> new_nodes;
	dict<จำนวนเต็ม, จำนวนเต็ม> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	สำหรับ (จำนวนเต็ม i = GetSize(nodes)-1; i >= 0; i--) {
		ถ้า (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		ถ้า (!used_old_ids.count(i))
			continue;
		ถ้า (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		ถ้า (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	สำหรับ (จำนวนเต็ม i = 0; i < GetSize(nodes); i++) {
		ถ้า (!used_old_ids.count(i))
			continue;
		nodes[i].left_parent = old_to_new_ids.at(nodes[i].left_parent);
		nodes[i].right_parent = old_to_new_ids.at(nodes[i].right_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.push_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
