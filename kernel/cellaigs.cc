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

#அந்தர்பாவ "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_முனை::Aig_முனை()
{
	portbit = -1;
	inverter = பொய்;
	இடது_parent = -1;
	வலது_parent = -1;
}

தருக்க Aig_முனை::operator==(மாறிலி Aig_முனை &other) மாறிலி
{
	எனில் (portname != other.portname) திரும்பு பொய்;
	எனில் (portbit != other.portbit) திரும்பு பொய்;
	எனில் (inverter != other.inverter) திரும்பு பொய்;
	எனில் (இடது_parent != other.இடது_parent) திரும்பு பொய்;
	எனில் (வலது_parent != other.வலது_parent) திரும்பு பொய்;
	திரும்பு உண்மை;
}

Hasher Aig_முனை::hash_into(Hasher h) மாறிலி
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(இடது_parent);
	h.eat(வலது_parent);
	திரும்பு h;
}

தருக்க Aig::operator==(மாறிலி Aig &other) மாறிலி
{
	திரும்பு name == other.name;
}

Hasher Aig::hash_into(Hasher h) மாறிலி
{
	h.eat(name);
	திரும்பு h;
}

கட்டமைப்பு AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_முனை> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	முழு node2index(மாறிலி Aig_முனை &முனை)
	{
		எனில் (முனை.இடது_parent > முனை.வலது_parent) {
			Aig_முனை n(முனை);
			std::swap(n.இடது_parent, n.வலது_parent);
			திரும்பு node2index(n);
		}

		எனில் (!aig_indices.மொத்தம்(முனை)) {
			aig_indices.expect(முனை, GetSize(aig->nodes));
			aig->nodes.தள்ளு_back(முனை);
		}

		திரும்பு aig_indices.at(முனை);
	}

	முழு bool_முனை(தருக்க மதிப்பு)
	{
		Aig_முனை முனை;
		முனை.inverter = மதிப்பு;
		திரும்பு node2index(முனை);
	}

	முழு inport(IdString portname, முழு portbit = 0, தருக்க inverter = பொய்)
	{
		எனில் (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		எனில் (signed_param != IdString::Null && cell->parameters.மொத்தம்(signed_param) && cell->getParam(signed_param).as_bool())
				திரும்பு inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			திரும்பு bool_முனை(inverter);
		}

		Aig_முனை முனை;
		முனை.portname = portname;
		முனை.portbit = portbit;
		முனை.inverter = inverter;
		திரும்பு node2index(முனை);
	}

	vector<முழு> inport_vec(IdString portname, முழு width)
	{
		vector<முழு> vec;
		க்கு (முழு i = 0; i < width; i++)
			vec.தள்ளு_back(inport(portname, i));
		திரும்பு vec;
	}

	முழு not_inport(IdString portname, முழு portbit = 0)
	{
		திரும்பு inport(portname, portbit, உண்மை);
	}

	முழு not_gate(முழு A)
	{
		Aig_முனை முனை(aig_indices[A]);
		முனை.outports.clear();
		முனை.inverter = !முனை.inverter;
		திரும்பு node2index(முனை);
	}

	முழு and_gate(முழு A, முழு B, தருக்க inverter = பொய்)
	{
		எனில் (A == B)
			திரும்பு inverter ? not_gate(A) : A;

		மாறிலி Aig_முனை &nA = aig_indices[A];
		மாறிலி Aig_முனை &nB = aig_indices[B];

		Aig_முனை nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		எனில் (nA == nB_inv)
			திரும்பு bool_முனை(inverter);

		தருக்க nA_bool = nA.portbit < 0 && nA.இடது_parent < 0 && nA.வலது_parent < 0;
		தருக்க nB_bool = nB.portbit < 0 && nB.இடது_parent < 0 && nB.வலது_parent < 0;

		எனில் (nA_bool && nB_bool) {
			தருக்க bA = nA.inverter;
			தருக்க bB = nB.inverter;
			திரும்பு bool_முனை(inverter != (bA && bB));
		}

		எனில் (nA_bool) {
			தருக்க bA = nA.inverter;
			எனில் (inverter)
				திரும்பு bA ? not_gate(B) : bool_முனை(உண்மை);
			திரும்பு bA ? B : bool_முனை(பொய்);
		}

		எனில் (nB_bool) {
			தருக்க bB = nB.inverter;
			எனில் (inverter)
				திரும்பு bB ? not_gate(A) : bool_முனை(உண்மை);
			திரும்பு bB ? A : bool_முனை(பொய்);
		}

		Aig_முனை முனை;
		முனை.inverter = inverter;
		முனை.இடது_parent = A;
		முனை.வலது_parent = B;
		திரும்பு node2index(முனை);
	}

	முழு nand_gate(முழு A, முழு B)
	{
		திரும்பு and_gate(A, B, உண்மை);
	}

	முழு or_gate(முழு A, முழு B)
	{
		முழு not_a = not_gate(A);
		முழு not_b = not_gate(B);
		திரும்பு nand_gate(not_a, not_b);
	}

	முழு nor_gate(முழு A, முழு B)
	{
		முழு not_a = not_gate(A);
		முழு not_b = not_gate(B);
		திரும்பு and_gate(not_a, not_b);
	}

	முழு xor_gate(முழு A, முழு B)
	{
		முழு a_and_b = and_gate(A, B);
		முழு a_nor_b = nor_gate(A, B);
		திரும்பு nor_gate(a_and_b, a_nor_b);
	}

	முழு xnor_gate(முழு A, முழு B)
	{
		முழு a_and_b = and_gate(A, B);
		முழு a_nor_b = nor_gate(A, B);
		திரும்பு or_gate(a_and_b, a_nor_b);
	}

	முழு andnot_gate(முழு A, முழு B)
	{
		முழு not_b = not_gate(B);
		திரும்பு and_gate(A, not_b);
	}

	முழு ornot_gate(முழு A, முழு B)
	{
		முழு not_b = not_gate(B);
		திரும்பு or_gate(A, not_b);
	}

	முழு mux_gate(முழு A, முழு B, முழு S)
	{
		முழு not_s = not_gate(S);
		முழு a_active = and_gate(A, not_s);
		முழு b_active = and_gate(B, S);
		திரும்பு or_gate(a_active, b_active);
	}

	vector<முழு> adder(மாறிலி vector<முழு> &A, மாறிலி vector<முழு> &B, முழு carry_in, vector<முழு> *X = nullptr, vector<முழு> *CO = nullptr)
	{
		vector<முழு> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		க்கு (முழு i = 0; i < GetSize(A); i++) {
			முழு a_xor_b = xor_gate(A[i], B[i]);
			முழு a_or_b = or_gate(A[i], B[i]);
			முழு a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			முழு tmp = and_gate(a_or_b, carry_in);
			முழு carry_out = or_gate(a_and_b, tmp);
			எனில் (X != nullptr)
				X->at(i) = a_xor_b;
			எனில் (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		திரும்பு Y;
	}

	வெறுமை outport(முழு முனை, IdString portname, முழு portbit = 0)
	{
		எனில் (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(முனை).outports.தள்ளு_back(pair<IdString, முழு>(portname, portbit));
	}

	வெறுமை outport_bool(முழு முனை, IdString portname)
	{
		outport(முனை, portname);
		க்கு (முழு i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_முனை(பொய்), portname, i);
	}

	வெறுமை outport_vec(மாறிலி vector<முழு> &vec, IdString portname)
	{
		க்கு (முழு i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	எனில் (cell->type.isPublic())
		திரும்பு;

	AigMaker mk(இது, cell);
	name = cell->type.str();

	string mkname_last;
	தருக்க mkname_a_signed = பொய்;
	தருக்க mkname_b_signed = பொய்;
	தருக்க mkname_is_signed = பொய்;

	cell->parameters.sort();
	க்கு (auto p : cell->parameters)
	{
		எனில் (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} இல்லையெனில் எனில் (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} இல்லையெனில் {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = பொய்;
		mkname_b_signed = பொய்;
		mkname_is_signed = பொய்;
		எனில் (p.first == ID::A_SIGNED) {
			mkname_a_signed = உண்மை;
			mkname_is_signed = p.second.as_bool();
		}
		எனில் (p.first == ID::B_SIGNED) {
			mkname_b_signed = உண்மை;
			mkname_is_signed = p.second.as_bool();
		}
	}

	எனில் (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		க்கு (முழு i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			முழு A = mk.inport(ID::A, i);
			முழு Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		செல் optimize;
	}

	எனில் (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		க்கு (முழு i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			முழு A = mk.inport(ID::A, i);
			முழு B = mk.inport(ID::B, i);
			முழு Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		செல் optimize;
	}

	எனில் (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		முழு S = mk.inport(ID::S);
		க்கு (முழு i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			முழு A = mk.inport(ID::A, i);
			முழு B = mk.inport(ID::B, i);
			முழு Y = mk.mux_gate(A, B, S);
			எனில் (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		செல் optimize;
	}

	எனில் (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		முழு Y = mk.inport(ID::A, 0);
		க்கு (முழு i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			முழு A = mk.inport(ID::A, i);
			எனில் (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			எனில் (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			எனில் (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			எனில் (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			எனில் (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		எனில் (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		க்கு (முழு i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_முனை(பொய்), ID::Y, i);
		செல் optimize;
	}

	எனில் (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		முழு A = mk.inport(ID::A, 0), Y = -1;
		க்கு (முழு i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		எனில் (cell->type.in(ID($logic_and), ID($logic_or))) {
			முழு B = mk.inport(ID::B, 0);
			க்கு (முழு i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			எனில் (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			எனில் (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} இல்லையெனில் {
			எனில் (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		செல் optimize;
	}

	எனில் (cell->type.in(ID($add), ID($sub)))
	{
		முழு width = GetSize(cell->getPort(ID::Y));
		vector<முழு> A = mk.inport_vec(ID::A, width);
		vector<முழு> B = mk.inport_vec(ID::B, width);
		முழு carry = mk.bool_முனை(பொய்);
		எனில் (cell->type == ID($sub)) {
			க்கு (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<முழு> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		செல் optimize;
	}

	எனில் (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		முழு width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<முழு> A = mk.inport_vec(ID::A, width);
		vector<முழு> B = mk.inport_vec(ID::B, width);

		எனில் (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		முழு carry = mk.bool_முனை(!cell->type.in(ID($le), ID($ge)));
		க்கு (auto &n : B)
			n = mk.not_gate(n);
		vector<முழு> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		க்கு (முழு i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_முனை(பொய்), ID::Y, i);
		செல் optimize;
	}

	எனில் (cell->type == ID($alu))
	{
		முழு width = GetSize(cell->getPort(ID::Y));
		vector<முழு> A = mk.inport_vec(ID::A, width);
		vector<முழு> B = mk.inport_vec(ID::B, width);
		முழு carry = mk.inport(ID::CI);
		முழு binv = mk.inport(ID::BI);
		க்கு (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<முழு> X(width), CO(width);
		vector<முழு> Y = mk.adder(A, B, carry, &X, &CO);
		க்கு (முழு i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		செல் optimize;
	}

	எனில் (cell->type.in(ID($eq), ID($ne)))
	{
		முழு width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<முழு> A = mk.inport_vec(ID::A, width);
		vector<முழு> B = mk.inport_vec(ID::B, width);
		முழு Y = mk.bool_முனை(பொய்);
		க்கு (முழு i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		எனில் (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		செல் optimize;
	}

	எனில் (cell->type == ID($_AOI3_))
	{
		முழு A = mk.inport(ID::A);
		முழு B = mk.inport(ID::B);
		முழு C = mk.inport(ID::C);
		முழு Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		செல் optimize;
	}

	எனில் (cell->type == ID($_OAI3_))
	{
		முழு A = mk.inport(ID::A);
		முழு B = mk.inport(ID::B);
		முழு C = mk.inport(ID::C);
		முழு Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		செல் optimize;
	}

	எனில் (cell->type == ID($_AOI4_))
	{
		முழு A = mk.inport(ID::A);
		முழு B = mk.inport(ID::B);
		முழு C = mk.inport(ID::C);
		முழு D = mk.inport(ID::D);
		முழு a_and_b = mk.and_gate(A, B);
		முழு Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		செல் optimize;
	}

	எனில் (cell->type == ID($_OAI4_))
	{
		முழு A = mk.inport(ID::A);
		முழு B = mk.inport(ID::B);
		முழு C = mk.inport(ID::C);
		முழு D = mk.inport(ID::D);
		முழு a_or_b = mk.or_gate(A, B);
		முழு Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		செல் optimize;
	}

	name.clear();
	திரும்பு;

optimize:;
	pool<முழு> used_old_ids;
	vector<Aig_முனை> new_nodes;
	dict<முழு, முழு> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	க்கு (முழு i = GetSize(nodes)-1; i >= 0; i--) {
		எனில் (!nodes[i].outports.empty())
			used_old_ids.செருகு(i);
		எனில் (!used_old_ids.மொத்தம்(i))
			தொடர்;
		எனில் (nodes[i].இடது_parent >= 0)
			used_old_ids.செருகு(nodes[i].இடது_parent);
		எனில் (nodes[i].வலது_parent >= 0)
			used_old_ids.செருகு(nodes[i].வலது_parent);
	}

	க்கு (முழு i = 0; i < GetSize(nodes); i++) {
		எனில் (!used_old_ids.மொத்தம்(i))
			தொடர்;
		nodes[i].இடது_parent = old_to_new_ids.at(nodes[i].இடது_parent);
		nodes[i].வலது_parent = old_to_new_ids.at(nodes[i].வலது_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.தள்ளு_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
