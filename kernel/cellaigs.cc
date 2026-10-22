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
	もし (portname != other.portname) 戻る false;
	もし (portbit != other.portbit) 戻る false;
	もし (inverter != other.inverter) 戻る false;
	もし (left_parent != other.left_parent) 戻る false;
	もし (right_parent != other.right_parent) 戻る false;
	戻る true;
}

Hasher AigNode::hash_into(Hasher h) const
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	戻る h;
}

bool Aig::operator==(const Aig &other) const
{
	戻る name == other.name;
}

Hasher Aig::hash_into(Hasher h) const
{
	h.eat(name);
	戻る h;
}

struct AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	整数 node2index(const AigNode &node)
	{
		もし (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			戻る node2index(n);
		}

		もし (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		戻る aig_indices.at(node);
	}

	整数 bool_node(bool value)
	{
		AigNode node;
		node.inverter = value;
		戻る node2index(node);
	}

	整数 inport(IdString portname, 整数 portbit = 0, bool inverter = false)
	{
		もし (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		もし (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				戻る inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			戻る bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		戻る node2index(node);
	}

	vector<整数> inport_vec(IdString portname, 整数 width)
	{
		vector<整数> vec;
		ために (整数 i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		戻る vec;
	}

	整数 not_inport(IdString portname, 整数 portbit = 0)
	{
		戻る inport(portname, portbit, true);
	}

	整数 not_gate(整数 A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		戻る node2index(node);
	}

	整数 and_gate(整数 A, 整数 B, bool inverter = false)
	{
		もし (A == B)
			戻る inverter ? not_gate(A) : A;

		const AigNode &nA = aig_indices[A];
		const AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		もし (nA == nB_inv)
			戻る bool_node(inverter);

		bool nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		もし (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			戻る bool_node(inverter != (bA && bB));
		}

		もし (nA_bool) {
			bool bA = nA.inverter;
			もし (inverter)
				戻る bA ? not_gate(B) : bool_node(true);
			戻る bA ? B : bool_node(false);
		}

		もし (nB_bool) {
			bool bB = nB.inverter;
			もし (inverter)
				戻る bB ? not_gate(A) : bool_node(true);
			戻る bB ? A : bool_node(false);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		戻る node2index(node);
	}

	整数 nand_gate(整数 A, 整数 B)
	{
		戻る and_gate(A, B, true);
	}

	整数 or_gate(整数 A, 整数 B)
	{
		整数 not_a = not_gate(A);
		整数 not_b = not_gate(B);
		戻る nand_gate(not_a, not_b);
	}

	整数 nor_gate(整数 A, 整数 B)
	{
		整数 not_a = not_gate(A);
		整数 not_b = not_gate(B);
		戻る and_gate(not_a, not_b);
	}

	整数 xor_gate(整数 A, 整数 B)
	{
		整数 a_and_b = and_gate(A, B);
		整数 a_nor_b = nor_gate(A, B);
		戻る nor_gate(a_and_b, a_nor_b);
	}

	整数 xnor_gate(整数 A, 整数 B)
	{
		整数 a_and_b = and_gate(A, B);
		整数 a_nor_b = nor_gate(A, B);
		戻る or_gate(a_and_b, a_nor_b);
	}

	整数 andnot_gate(整数 A, 整数 B)
	{
		整数 not_b = not_gate(B);
		戻る and_gate(A, not_b);
	}

	整数 ornot_gate(整数 A, 整数 B)
	{
		整数 not_b = not_gate(B);
		戻る or_gate(A, not_b);
	}

	整数 mux_gate(整数 A, 整数 B, 整数 S)
	{
		整数 not_s = not_gate(S);
		整数 a_active = and_gate(A, not_s);
		整数 b_active = and_gate(B, S);
		戻る or_gate(a_active, b_active);
	}

	vector<整数> adder(const vector<整数> &A, const vector<整数> &B, 整数 carry_in, vector<整数> *X = nullptr, vector<整数> *CO = nullptr)
	{
		vector<整数> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		ために (整数 i = 0; i < GetSize(A); i++) {
			整数 a_xor_b = xor_gate(A[i], B[i]);
			整数 a_or_b = or_gate(A[i], B[i]);
			整数 a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			整数 tmp = and_gate(a_or_b, carry_in);
			整数 carry_out = or_gate(a_and_b, tmp);
			もし (X != nullptr)
				X->at(i) = a_xor_b;
			もし (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		戻る Y;
	}

	void outport(整数 node, IdString portname, 整数 portbit = 0)
	{
		もし (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, 整数>(portname, portbit));
	}

	void outport_bool(整数 node, IdString portname)
	{
		outport(node, portname);
		ために (整数 i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(false), portname, i);
	}

	void outport_vec(const vector<整数> &vec, IdString portname)
	{
		ために (整数 i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	もし (cell->type.isPublic())
		戻る;

	AigMaker mk(this, cell);
	name = cell->type.str();

	string mkname_last;
	bool mkname_a_signed = false;
	bool mkname_b_signed = false;
	bool mkname_is_signed = false;

	cell->parameters.sort();
	ために (auto p : cell->parameters)
	{
		もし (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} 否则 もし (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} 否则 {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = false;
		mkname_b_signed = false;
		mkname_is_signed = false;
		もし (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		もし (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	もし (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		ために (整数 i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			整数 A = mk.inport(ID::A, i);
			整数 Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	もし (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		ために (整数 i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			整数 A = mk.inport(ID::A, i);
			整数 B = mk.inport(ID::B, i);
			整数 Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
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

	もし (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		整数 S = mk.inport(ID::S);
		ために (整数 i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			整数 A = mk.inport(ID::A, i);
			整数 B = mk.inport(ID::B, i);
			整数 Y = mk.mux_gate(A, B, S);
			もし (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	もし (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		整数 Y = mk.inport(ID::A, 0);
		ために (整数 i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			整数 A = mk.inport(ID::A, i);
			もし (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			もし (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			もし (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			もし (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			もし (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		もし (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		ために (整数 i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	もし (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		整数 A = mk.inport(ID::A, 0), Y = -1;
		ために (整数 i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		もし (cell->type.in(ID($logic_and), ID($logic_or))) {
			整数 B = mk.inport(ID::B, 0);
			ために (整数 i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			もし (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			もし (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} 否则 {
			もし (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	もし (cell->type.in(ID($add), ID($sub)))
	{
		整数 width = GetSize(cell->getPort(ID::Y));
		vector<整数> A = mk.inport_vec(ID::A, width);
		vector<整数> B = mk.inport_vec(ID::B, width);
		整数 carry = mk.bool_node(false);
		もし (cell->type == ID($sub)) {
			ために (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<整数> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		goto optimize;
	}

	もし (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		整数 width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<整数> A = mk.inport_vec(ID::A, width);
		vector<整数> B = mk.inport_vec(ID::B, width);

		もし (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		整数 carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		ために (auto &n : B)
			n = mk.not_gate(n);
		vector<整数> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		ために (整数 i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	もし (cell->type == ID($alu))
	{
		整数 width = GetSize(cell->getPort(ID::Y));
		vector<整数> A = mk.inport_vec(ID::A, width);
		vector<整数> B = mk.inport_vec(ID::B, width);
		整数 carry = mk.inport(ID::CI);
		整数 binv = mk.inport(ID::BI);
		ために (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<整数> X(width), CO(width);
		vector<整数> Y = mk.adder(A, B, carry, &X, &CO);
		ために (整数 i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		goto optimize;
	}

	もし (cell->type.in(ID($eq), ID($ne)))
	{
		整数 width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<整数> A = mk.inport_vec(ID::A, width);
		vector<整数> B = mk.inport_vec(ID::B, width);
		整数 Y = mk.bool_node(false);
		ために (整数 i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		もし (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	もし (cell->type == ID($_AOI3_))
	{
		整数 A = mk.inport(ID::A);
		整数 B = mk.inport(ID::B);
		整数 C = mk.inport(ID::C);
		整数 Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	もし (cell->type == ID($_OAI3_))
	{
		整数 A = mk.inport(ID::A);
		整数 B = mk.inport(ID::B);
		整数 C = mk.inport(ID::C);
		整数 Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	もし (cell->type == ID($_AOI4_))
	{
		整数 A = mk.inport(ID::A);
		整数 B = mk.inport(ID::B);
		整数 C = mk.inport(ID::C);
		整数 D = mk.inport(ID::D);
		整数 a_and_b = mk.and_gate(A, B);
		整数 Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	もし (cell->type == ID($_OAI4_))
	{
		整数 A = mk.inport(ID::A);
		整数 B = mk.inport(ID::B);
		整数 C = mk.inport(ID::C);
		整数 D = mk.inport(ID::D);
		整数 a_or_b = mk.or_gate(A, B);
		整数 Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	name.clear();
	戻る;

optimize:;
	pool<整数> used_old_ids;
	vector<AigNode> new_nodes;
	dict<整数, 整数> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	ために (整数 i = GetSize(nodes)-1; i >= 0; i--) {
		もし (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		もし (!used_old_ids.count(i))
			continue;
		もし (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		もし (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	ために (整数 i = 0; i < GetSize(nodes); i++) {
		もし (!used_old_ids.count(i))
			continue;
		nodes[i].left_parent = old_to_new_ids.at(nodes[i].left_parent);
		nodes[i].right_parent = old_to_new_ids.at(nodes[i].right_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.push_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
