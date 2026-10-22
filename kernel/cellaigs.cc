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
	නම් (portname != other.portname) ආපසු false;
	නම් (portbit != other.portbit) ආපසු false;
	නම් (inverter != other.inverter) ආපසු false;
	නම් (left_parent != other.left_parent) ආපසු false;
	නම් (right_parent != other.right_parent) ආපසු false;
	ආපසු true;
}

Hasher AigNode::hash_into(Hasher h) const
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	ආපසු h;
}

bool Aig::operator==(const Aig &other) const
{
	ආපසු name == other.name;
}

Hasher Aig::hash_into(Hasher h) const
{
	h.eat(name);
	ආපසු h;
}

sa_nracanaa AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	පූර්ණ_සංඛ්‍යා node2index(const AigNode &node)
	{
		නම් (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			ආපසු node2index(n);
		}

		නම් (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		ආපසු aig_indices.at(node);
	}

	පූර්ණ_සංඛ්‍යා bool_node(bool value)
	{
		AigNode node;
		node.inverter = value;
		ආපසු node2index(node);
	}

	පූර්ණ_සංඛ්‍යා inport(IdString portname, පූර්ණ_සංඛ්‍යා portbit = 0, bool inverter = false)
	{
		නම් (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		නම් (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				ආපසු inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			ආපසු bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		ආපසු node2index(node);
	}

	vector<පූර්ණ_සංඛ්‍යා> inport_vec(IdString portname, පූර්ණ_සංඛ්‍යා width)
	{
		vector<පූර්ණ_සංඛ්‍යා> vec;
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		ආපසු vec;
	}

	පූර්ණ_සංඛ්‍යා not_inport(IdString portname, පූර්ණ_සංඛ්‍යා portbit = 0)
	{
		ආපසු inport(portname, portbit, true);
	}

	පූර්ණ_සංඛ්‍යා not_gate(පූර්ණ_සංඛ්‍යා A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		ආපසු node2index(node);
	}

	පූර්ණ_සංඛ්‍යා and_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B, bool inverter = false)
	{
		නම් (A == B)
			ආපසු inverter ? not_gate(A) : A;

		const AigNode &nA = aig_indices[A];
		const AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		නම් (nA == nB_inv)
			ආපසු bool_node(inverter);

		bool nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		නම් (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			ආපසු bool_node(inverter != (bA && bB));
		}

		නම් (nA_bool) {
			bool bA = nA.inverter;
			නම් (inverter)
				ආපසු bA ? not_gate(B) : bool_node(true);
			ආපසු bA ? B : bool_node(false);
		}

		නම් (nB_bool) {
			bool bB = nB.inverter;
			නම් (inverter)
				ආපසු bB ? not_gate(A) : bool_node(true);
			ආපසු bB ? A : bool_node(false);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		ආපසු node2index(node);
	}

	පූර්ණ_සංඛ්‍යා nand_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B)
	{
		ආපසු and_gate(A, B, true);
	}

	පූර්ණ_සංඛ්‍යා or_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B)
	{
		පූර්ණ_සංඛ්‍යා not_a = not_gate(A);
		පූර්ණ_සංඛ්‍යා not_b = not_gate(B);
		ආපසු nand_gate(not_a, not_b);
	}

	පූර්ණ_සංඛ්‍යා nor_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B)
	{
		පූර්ණ_සංඛ්‍යා not_a = not_gate(A);
		පූර්ණ_සංඛ්‍යා not_b = not_gate(B);
		ආපසු and_gate(not_a, not_b);
	}

	පූර්ණ_සංඛ්‍යා xor_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B)
	{
		පූර්ණ_සංඛ්‍යා a_and_b = and_gate(A, B);
		පූර්ණ_සංඛ්‍යා a_nor_b = nor_gate(A, B);
		ආපසු nor_gate(a_and_b, a_nor_b);
	}

	පූර්ණ_සංඛ්‍යා xnor_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B)
	{
		පූර්ණ_සංඛ්‍යා a_and_b = and_gate(A, B);
		පූර්ණ_සංඛ්‍යා a_nor_b = nor_gate(A, B);
		ආපසු or_gate(a_and_b, a_nor_b);
	}

	පූර්ණ_සංඛ්‍යා andnot_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B)
	{
		පූර්ණ_සංඛ්‍යා not_b = not_gate(B);
		ආපසු and_gate(A, not_b);
	}

	පූර්ණ_සංඛ්‍යා ornot_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B)
	{
		පූර්ණ_සංඛ්‍යා not_b = not_gate(B);
		ආපසු or_gate(A, not_b);
	}

	පූර්ණ_සංඛ්‍යා mux_gate(පූර්ණ_සංඛ්‍යා A, පූර්ණ_සංඛ්‍යා B, පූර්ණ_සංඛ්‍යා S)
	{
		පූර්ණ_සංඛ්‍යා not_s = not_gate(S);
		පූර්ණ_සංඛ්‍යා a_active = and_gate(A, not_s);
		පූර්ණ_සංඛ්‍යා b_active = and_gate(B, S);
		ආපසු or_gate(a_active, b_active);
	}

	vector<පූර්ණ_සංඛ්‍යා> adder(const vector<පූර්ණ_සංඛ්‍යා> &A, const vector<පූර්ණ_සංඛ්‍යා> &B, පූර්ණ_සංඛ්‍යා carry_in, vector<පූර්ණ_සංඛ්‍යා> *X = nullptr, vector<පූර්ණ_සංඛ්‍යා> *CO = nullptr)
	{
		vector<පූර්ණ_සංඛ්‍යා> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < GetSize(A); i++) {
			පූර්ණ_සංඛ්‍යා a_xor_b = xor_gate(A[i], B[i]);
			පූර්ණ_සංඛ්‍යා a_or_b = or_gate(A[i], B[i]);
			පූර්ණ_සංඛ්‍යා a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			පූර්ණ_සංඛ්‍යා tmp = and_gate(a_or_b, carry_in);
			පූර්ණ_සංඛ්‍යා carry_out = or_gate(a_and_b, tmp);
			නම් (X != nullptr)
				X->at(i) = a_xor_b;
			නම් (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		ආපසු Y;
	}

	void outport(පූර්ණ_සංඛ්‍යා node, IdString portname, පූර්ණ_සංඛ්‍යා portbit = 0)
	{
		නම් (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, පූර්ණ_සංඛ්‍යා>(portname, portbit));
	}

	void outport_bool(පූර්ණ_සංඛ්‍යා node, IdString portname)
	{
		outport(node, portname);
		සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(false), portname, i);
	}

	void outport_vec(const vector<පූර්ණ_සංඛ්‍යා> &vec, IdString portname)
	{
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	නම් (cell->type.isPublic())
		ආපසු;

	AigMaker mk(this, cell);
	name = cell->type.str();

	string mkname_last;
	bool mkname_a_signed = false;
	bool mkname_b_signed = false;
	bool mkname_is_signed = false;

	cell->parameters.sort();
	සඳහා (auto p : cell->parameters)
	{
		නම් (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} නැතහොත් නම් (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} නැතහොත් {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = false;
		mkname_b_signed = false;
		mkname_is_signed = false;
		නම් (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		නම් (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	නම් (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A, i);
			පූර්ණ_සංඛ්‍යා Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	නම් (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A, i);
			පූර්ණ_සංඛ්‍යා B = mk.inport(ID::B, i);
			පූර්ණ_සංඛ්‍යා Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
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

	නම් (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		පූර්ණ_සංඛ්‍යා S = mk.inport(ID::S);
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A, i);
			පූර්ණ_සංඛ්‍යා B = mk.inport(ID::B, i);
			පූර්ණ_සංඛ්‍යා Y = mk.mux_gate(A, B, S);
			නම් (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	නම් (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		පූර්ණ_සංඛ්‍යා Y = mk.inport(ID::A, 0);
		සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A, i);
			නම් (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			නම් (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			නම් (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			නම් (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			නම් (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		නම් (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	නම් (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A, 0), Y = -1;
		සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		නම් (cell->type.in(ID($logic_and), ID($logic_or))) {
			පූර්ණ_සංඛ්‍යා B = mk.inport(ID::B, 0);
			සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			නම් (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			නම් (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} නැතහොත් {
			නම් (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	නම් (cell->type.in(ID($add), ID($sub)))
	{
		පූර්ණ_සංඛ්‍යා width = GetSize(cell->getPort(ID::Y));
		vector<පූර්ණ_සංඛ්‍යා> A = mk.inport_vec(ID::A, width);
		vector<පූර්ණ_සංඛ්‍යා> B = mk.inport_vec(ID::B, width);
		පූර්ණ_සංඛ්‍යා carry = mk.bool_node(false);
		නම් (cell->type == ID($sub)) {
			සඳහා (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<පූර්ණ_සංඛ්‍යා> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		goto optimize;
	}

	නම් (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		පූර්ණ_සංඛ්‍යා width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<පූර්ණ_සංඛ්‍යා> A = mk.inport_vec(ID::A, width);
		vector<පූර්ණ_සංඛ්‍යා> B = mk.inport_vec(ID::B, width);

		නම් (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		පූර්ණ_සංඛ්‍යා carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		සඳහා (auto &n : B)
			n = mk.not_gate(n);
		vector<පූර්ණ_සංඛ්‍යා> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		සඳහා (පූර්ණ_සංඛ්‍යා i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	නම් (cell->type == ID($alu))
	{
		පූර්ණ_සංඛ්‍යා width = GetSize(cell->getPort(ID::Y));
		vector<පූර්ණ_සංඛ්‍යා> A = mk.inport_vec(ID::A, width);
		vector<පූර්ණ_සංඛ්‍යා> B = mk.inport_vec(ID::B, width);
		පූර්ණ_සංඛ්‍යා carry = mk.inport(ID::CI);
		පූර්ණ_සංඛ්‍යා binv = mk.inport(ID::BI);
		සඳහා (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<පූර්ණ_සංඛ්‍යා> X(width), CO(width);
		vector<පූර්ණ_සංඛ්‍යා> Y = mk.adder(A, B, carry, &X, &CO);
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		goto optimize;
	}

	නම් (cell->type.in(ID($eq), ID($ne)))
	{
		පූර්ණ_සංඛ්‍යා width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<පූර්ණ_සංඛ්‍යා> A = mk.inport_vec(ID::A, width);
		vector<පූර්ණ_සංඛ්‍යා> B = mk.inport_vec(ID::B, width);
		පූර්ණ_සංඛ්‍යා Y = mk.bool_node(false);
		සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		නම් (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	නම් (cell->type == ID($_AOI3_))
	{
		පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A);
		පූර්ණ_සංඛ්‍යා B = mk.inport(ID::B);
		පූර්ණ_සංඛ්‍යා C = mk.inport(ID::C);
		පූර්ණ_සංඛ්‍යා Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	නම් (cell->type == ID($_OAI3_))
	{
		පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A);
		පූර්ණ_සංඛ්‍යා B = mk.inport(ID::B);
		පූර්ණ_සංඛ්‍යා C = mk.inport(ID::C);
		පූර්ණ_සංඛ්‍යා Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	නම් (cell->type == ID($_AOI4_))
	{
		පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A);
		පූර්ණ_සංඛ්‍යා B = mk.inport(ID::B);
		පූර්ණ_සංඛ්‍යා C = mk.inport(ID::C);
		පූර්ණ_සංඛ්‍යා D = mk.inport(ID::D);
		පූර්ණ_සංඛ්‍යා a_and_b = mk.and_gate(A, B);
		පූර්ණ_සංඛ්‍යා Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	නම් (cell->type == ID($_OAI4_))
	{
		පූර්ණ_සංඛ්‍යා A = mk.inport(ID::A);
		පූර්ණ_සංඛ්‍යා B = mk.inport(ID::B);
		පූර්ණ_සංඛ්‍යා C = mk.inport(ID::C);
		පූර්ණ_සංඛ්‍යා D = mk.inport(ID::D);
		පූර්ණ_සංඛ්‍යා a_or_b = mk.or_gate(A, B);
		පූර්ණ_සංඛ්‍යා Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	name.clear();
	ආපසු;

optimize:;
	pool<පූර්ණ_සංඛ්‍යා> used_old_ids;
	vector<AigNode> new_nodes;
	dict<පූර්ණ_සංඛ්‍යා, පූර්ණ_සංඛ්‍යා> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	සඳහා (පූර්ණ_සංඛ්‍යා i = GetSize(nodes)-1; i >= 0; i--) {
		නම් (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		නම් (!used_old_ids.count(i))
			continue;
		නම් (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		නම් (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < GetSize(nodes); i++) {
		නම් (!used_old_ids.count(i))
			continue;
		nodes[i].left_parent = old_to_new_ids.at(nodes[i].left_parent);
		nodes[i].right_parent = old_to_new_ids.at(nodes[i].right_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.push_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
