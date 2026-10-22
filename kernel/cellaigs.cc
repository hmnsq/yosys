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
	ꦪꦼꦤ꧀ (portname != other.portname) ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (portbit != other.portbit) ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (inverter != other.inverter) ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (left_parent != other.left_parent) ꦮꦭꦶ false;
	ꦪꦼꦤ꧀ (right_parent != other.right_parent) ꦮꦭꦶ false;
	ꦮꦭꦶ true;
}

Hasher AigNode::hash_into(Hasher h) const
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(left_parent);
	h.eat(right_parent);
	ꦮꦭꦶ h;
}

bool Aig::operator==(const Aig &other) const
{
	ꦮꦭꦶ name == other.name;
}

Hasher Aig::hash_into(Hasher h) const
{
	h.eat(name);
	ꦮꦭꦶ h;
}

sa_nracanaa AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<AigNode> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	puur_naa_nka node2index(const AigNode &node)
	{
		ꦪꦼꦤ꧀ (node.left_parent > node.right_parent) {
			AigNode n(node);
			std::swap(n.left_parent, n.right_parent);
			ꦮꦭꦶ node2index(n);
		}

		ꦪꦼꦤ꧀ (!aig_indices.count(node)) {
			aig_indices.expect(node, GetSize(aig->nodes));
			aig->nodes.push_back(node);
		}

		ꦮꦭꦶ aig_indices.at(node);
	}

	puur_naa_nka bool_node(bool value)
	{
		AigNode node;
		node.inverter = value;
		ꦮꦭꦶ node2index(node);
	}

	puur_naa_nka inport(IdString portname, puur_naa_nka portbit = 0, bool inverter = false)
	{
		ꦪꦼꦤ꧀ (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		ꦪꦼꦤ꧀ (signed_param != IdString::Null && cell->parameters.count(signed_param) && cell->getParam(signed_param).as_bool())
				ꦮꦭꦶ inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			ꦮꦭꦶ bool_node(inverter);
		}

		AigNode node;
		node.portname = portname;
		node.portbit = portbit;
		node.inverter = inverter;
		ꦮꦭꦶ node2index(node);
	}

	vector<puur_naa_nka> inport_vec(IdString portname, puur_naa_nka width)
	{
		vector<puur_naa_nka> vec;
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < width; i++)
			vec.push_back(inport(portname, i));
		ꦮꦭꦶ vec;
	}

	puur_naa_nka not_inport(IdString portname, puur_naa_nka portbit = 0)
	{
		ꦮꦭꦶ inport(portname, portbit, true);
	}

	puur_naa_nka not_gate(puur_naa_nka A)
	{
		AigNode node(aig_indices[A]);
		node.outports.clear();
		node.inverter = !node.inverter;
		ꦮꦭꦶ node2index(node);
	}

	puur_naa_nka and_gate(puur_naa_nka A, puur_naa_nka B, bool inverter = false)
	{
		ꦪꦼꦤ꧀ (A == B)
			ꦮꦭꦶ inverter ? not_gate(A) : A;

		const AigNode &nA = aig_indices[A];
		const AigNode &nB = aig_indices[B];

		AigNode nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ꦪꦼꦤ꧀ (nA == nB_inv)
			ꦮꦭꦶ bool_node(inverter);

		bool nA_bool = nA.portbit < 0 && nA.left_parent < 0 && nA.right_parent < 0;
		bool nB_bool = nB.portbit < 0 && nB.left_parent < 0 && nB.right_parent < 0;

		ꦪꦼꦤ꧀ (nA_bool && nB_bool) {
			bool bA = nA.inverter;
			bool bB = nB.inverter;
			ꦮꦭꦶ bool_node(inverter != (bA && bB));
		}

		ꦪꦼꦤ꧀ (nA_bool) {
			bool bA = nA.inverter;
			ꦪꦼꦤ꧀ (inverter)
				ꦮꦭꦶ bA ? not_gate(B) : bool_node(true);
			ꦮꦭꦶ bA ? B : bool_node(false);
		}

		ꦪꦼꦤ꧀ (nB_bool) {
			bool bB = nB.inverter;
			ꦪꦼꦤ꧀ (inverter)
				ꦮꦭꦶ bB ? not_gate(A) : bool_node(true);
			ꦮꦭꦶ bB ? A : bool_node(false);
		}

		AigNode node;
		node.inverter = inverter;
		node.left_parent = A;
		node.right_parent = B;
		ꦮꦭꦶ node2index(node);
	}

	puur_naa_nka nand_gate(puur_naa_nka A, puur_naa_nka B)
	{
		ꦮꦭꦶ and_gate(A, B, true);
	}

	puur_naa_nka or_gate(puur_naa_nka A, puur_naa_nka B)
	{
		puur_naa_nka not_a = not_gate(A);
		puur_naa_nka not_b = not_gate(B);
		ꦮꦭꦶ nand_gate(not_a, not_b);
	}

	puur_naa_nka nor_gate(puur_naa_nka A, puur_naa_nka B)
	{
		puur_naa_nka not_a = not_gate(A);
		puur_naa_nka not_b = not_gate(B);
		ꦮꦭꦶ and_gate(not_a, not_b);
	}

	puur_naa_nka xor_gate(puur_naa_nka A, puur_naa_nka B)
	{
		puur_naa_nka a_and_b = and_gate(A, B);
		puur_naa_nka a_nor_b = nor_gate(A, B);
		ꦮꦭꦶ nor_gate(a_and_b, a_nor_b);
	}

	puur_naa_nka xnor_gate(puur_naa_nka A, puur_naa_nka B)
	{
		puur_naa_nka a_and_b = and_gate(A, B);
		puur_naa_nka a_nor_b = nor_gate(A, B);
		ꦮꦭꦶ or_gate(a_and_b, a_nor_b);
	}

	puur_naa_nka andnot_gate(puur_naa_nka A, puur_naa_nka B)
	{
		puur_naa_nka not_b = not_gate(B);
		ꦮꦭꦶ and_gate(A, not_b);
	}

	puur_naa_nka ornot_gate(puur_naa_nka A, puur_naa_nka B)
	{
		puur_naa_nka not_b = not_gate(B);
		ꦮꦭꦶ or_gate(A, not_b);
	}

	puur_naa_nka mux_gate(puur_naa_nka A, puur_naa_nka B, puur_naa_nka S)
	{
		puur_naa_nka not_s = not_gate(S);
		puur_naa_nka a_active = and_gate(A, not_s);
		puur_naa_nka b_active = and_gate(B, S);
		ꦮꦭꦶ or_gate(a_active, b_active);
	}

	vector<puur_naa_nka> adder(const vector<puur_naa_nka> &A, const vector<puur_naa_nka> &B, puur_naa_nka carry_in, vector<puur_naa_nka> *X = nullptr, vector<puur_naa_nka> *CO = nullptr)
	{
		vector<puur_naa_nka> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(A); i++) {
			puur_naa_nka a_xor_b = xor_gate(A[i], B[i]);
			puur_naa_nka a_or_b = or_gate(A[i], B[i]);
			puur_naa_nka a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			puur_naa_nka tmp = and_gate(a_or_b, carry_in);
			puur_naa_nka carry_out = or_gate(a_and_b, tmp);
			ꦪꦼꦤ꧀ (X != nullptr)
				X->at(i) = a_xor_b;
			ꦪꦼꦤ꧀ (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		ꦮꦭꦶ Y;
	}

	void outport(puur_naa_nka node, IdString portname, puur_naa_nka portbit = 0)
	{
		ꦪꦼꦤ꧀ (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(node).outports.push_back(pair<IdString, puur_naa_nka>(portname, portbit));
	}

	void outport_bool(puur_naa_nka node, IdString portname)
	{
		outport(node, portname);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_node(false), portname, i);
	}

	void outport_vec(const vector<puur_naa_nka> &vec, IdString portname)
	{
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ꦪꦼꦤ꧀ (cell->type.isPublic())
		ꦮꦭꦶ;

	AigMaker mk(this, cell);
	name = cell->type.str();

	string mkname_last;
	bool mkname_a_signed = false;
	bool mkname_b_signed = false;
	bool mkname_is_signed = false;

	cell->parameters.sort();
	ꦏꦁꦒꦺꦴ (auto p : cell->parameters)
	{
		ꦪꦼꦤ꧀ (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} ꦭꦶꦪ ꦪꦼꦤ꧀ (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} ꦭꦶꦪ {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = false;
		mkname_b_signed = false;
		mkname_is_signed = false;
		ꦪꦼꦤ꧀ (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		ꦪꦼꦤ꧀ (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	ꦪꦼꦤ꧀ (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			puur_naa_nka A = mk.inport(ID::A, i);
			puur_naa_nka Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			puur_naa_nka A = mk.inport(ID::A, i);
			puur_naa_nka B = mk.inport(ID::B, i);
			puur_naa_nka Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
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

	ꦪꦼꦤ꧀ (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		puur_naa_nka S = mk.inport(ID::S);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			puur_naa_nka A = mk.inport(ID::A, i);
			puur_naa_nka B = mk.inport(ID::B, i);
			puur_naa_nka Y = mk.mux_gate(A, B, S);
			ꦪꦼꦤ꧀ (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		puur_naa_nka Y = mk.inport(ID::A, 0);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			puur_naa_nka A = mk.inport(ID::A, i);
			ꦪꦼꦤ꧀ (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ꦪꦼꦤ꧀ (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ꦪꦼꦤ꧀ (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ꦪꦼꦤ꧀ (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ꦪꦼꦤ꧀ (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ꦪꦼꦤ꧀ (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		puur_naa_nka A = mk.inport(ID::A, 0), Y = -1;
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ꦪꦼꦤ꧀ (cell->type.in(ID($logic_and), ID($logic_or))) {
			puur_naa_nka B = mk.inport(ID::B, 0);
			ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ꦪꦼꦤ꧀ (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ꦪꦼꦤ꧀ (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} ꦭꦶꦪ {
			ꦪꦼꦤ꧀ (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type.in(ID($add), ID($sub)))
	{
		puur_naa_nka width = GetSize(cell->getPort(ID::Y));
		vector<puur_naa_nka> A = mk.inport_vec(ID::A, width);
		vector<puur_naa_nka> B = mk.inport_vec(ID::B, width);
		puur_naa_nka carry = mk.bool_node(false);
		ꦪꦼꦤ꧀ (cell->type == ID($sub)) {
			ꦏꦁꦒꦺꦴ (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<puur_naa_nka> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		puur_naa_nka width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<puur_naa_nka> A = mk.inport_vec(ID::A, width);
		vector<puur_naa_nka> B = mk.inport_vec(ID::B, width);

		ꦪꦼꦤ꧀ (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		puur_naa_nka carry = mk.bool_node(!cell->type.in(ID($le), ID($ge)));
		ꦏꦁꦒꦺꦴ (auto &n : B)
			n = mk.not_gate(n);
		vector<puur_naa_nka> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_node(false), ID::Y, i);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type == ID($alu))
	{
		puur_naa_nka width = GetSize(cell->getPort(ID::Y));
		vector<puur_naa_nka> A = mk.inport_vec(ID::A, width);
		vector<puur_naa_nka> B = mk.inport_vec(ID::B, width);
		puur_naa_nka carry = mk.inport(ID::CI);
		puur_naa_nka binv = mk.inport(ID::BI);
		ꦏꦁꦒꦺꦴ (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<puur_naa_nka> X(width), CO(width);
		vector<puur_naa_nka> Y = mk.adder(A, B, carry, &X, &CO);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type.in(ID($eq), ID($ne)))
	{
		puur_naa_nka width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<puur_naa_nka> A = mk.inport_vec(ID::A, width);
		vector<puur_naa_nka> B = mk.inport_vec(ID::B, width);
		puur_naa_nka Y = mk.bool_node(false);
		ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ꦪꦼꦤ꧀ (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type == ID($_AOI3_))
	{
		puur_naa_nka A = mk.inport(ID::A);
		puur_naa_nka B = mk.inport(ID::B);
		puur_naa_nka C = mk.inport(ID::C);
		puur_naa_nka Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type == ID($_OAI3_))
	{
		puur_naa_nka A = mk.inport(ID::A);
		puur_naa_nka B = mk.inport(ID::B);
		puur_naa_nka C = mk.inport(ID::C);
		puur_naa_nka Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type == ID($_AOI4_))
	{
		puur_naa_nka A = mk.inport(ID::A);
		puur_naa_nka B = mk.inport(ID::B);
		puur_naa_nka C = mk.inport(ID::C);
		puur_naa_nka D = mk.inport(ID::D);
		puur_naa_nka a_and_b = mk.and_gate(A, B);
		puur_naa_nka Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	ꦪꦼꦤ꧀ (cell->type == ID($_OAI4_))
	{
		puur_naa_nka A = mk.inport(ID::A);
		puur_naa_nka B = mk.inport(ID::B);
		puur_naa_nka C = mk.inport(ID::C);
		puur_naa_nka D = mk.inport(ID::D);
		puur_naa_nka a_or_b = mk.or_gate(A, B);
		puur_naa_nka Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		goto optimize;
	}

	name.clear();
	ꦮꦭꦶ;

optimize:;
	pool<puur_naa_nka> used_old_ids;
	vector<AigNode> new_nodes;
	dict<puur_naa_nka, puur_naa_nka> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = GetSize(nodes)-1; i >= 0; i--) {
		ꦪꦼꦤ꧀ (!nodes[i].outports.empty())
			used_old_ids.insert(i);
		ꦪꦼꦤ꧀ (!used_old_ids.count(i))
			continue;
		ꦪꦼꦤ꧀ (nodes[i].left_parent >= 0)
			used_old_ids.insert(nodes[i].left_parent);
		ꦪꦼꦤ꧀ (nodes[i].right_parent >= 0)
			used_old_ids.insert(nodes[i].right_parent);
	}

	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < GetSize(nodes); i++) {
		ꦪꦼꦤ꧀ (!used_old_ids.count(i))
			continue;
		nodes[i].left_parent = old_to_new_ids.at(nodes[i].left_parent);
		nodes[i].right_parent = old_to_new_ids.at(nodes[i].right_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.push_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
