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

#અન્તર્ભાવ "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_ગાંઠ::Aig_ગાંઠ()
{
	portbit = -1;
	inverter = ખોટું;
	ડાબું_parent = -1;
	જમણું_parent = -1;
}

તાર્કિક Aig_ગાંઠ::operator==(અચલ Aig_ગાંઠ &other) અચલ
{
	જો (portname != other.portname) પાછા ખોટું;
	જો (portbit != other.portbit) પાછા ખોટું;
	જો (inverter != other.inverter) પાછા ખોટું;
	જો (ડાબું_parent != other.ડાબું_parent) પાછા ખોટું;
	જો (જમણું_parent != other.જમણું_parent) પાછા ખોટું;
	પાછા સાચું;
}

Hasher Aig_ગાંઠ::hash_into(Hasher h) અચલ
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(ડાબું_parent);
	h.eat(જમણું_parent);
	પાછા h;
}

તાર્કિક Aig::operator==(અચલ Aig &other) અચલ
{
	પાછા name == other.name;
}

Hasher Aig::hash_into(Hasher h) અચલ
{
	h.eat(name);
	પાછા h;
}

સંરચના AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_ગાંઠ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	પૂર્ણાંક node2index(અચલ Aig_ગાંઠ &ગાંઠ)
	{
		જો (ગાંઠ.ડાબું_parent > ગાંઠ.જમણું_parent) {
			Aig_ગાંઠ n(ગાંઠ);
			std::swap(n.ડાબું_parent, n.જમણું_parent);
			પાછા node2index(n);
		}

		જો (!aig_indices.ગણતરી(ગાંઠ)) {
			aig_indices.expect(ગાંઠ, GetSize(aig->nodes));
			aig->nodes.ધકેલો_back(ગાંઠ);
		}

		પાછા aig_indices.at(ગાંઠ);
	}

	પૂર્ણાંક bool_ગાંઠ(તાર્કિક કિંમત)
	{
		Aig_ગાંઠ ગાંઠ;
		ગાંઠ.inverter = કિંમત;
		પાછા node2index(ગાંઠ);
	}

	પૂર્ણાંક inport(IdString portname, પૂર્ણાંક portbit = 0, તાર્કિક inverter = ખોટું)
	{
		જો (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		જો (signed_param != IdString::Null && cell->parameters.ગણતરી(signed_param) && cell->getParam(signed_param).as_bool())
				પાછા inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			પાછા bool_ગાંઠ(inverter);
		}

		Aig_ગાંઠ ગાંઠ;
		ગાંઠ.portname = portname;
		ગાંઠ.portbit = portbit;
		ગાંઠ.inverter = inverter;
		પાછા node2index(ગાંઠ);
	}

	vector<પૂર્ણાંક> inport_vec(IdString portname, પૂર્ણાંક width)
	{
		vector<પૂર્ણાંક> vec;
		માટે (પૂર્ણાંક i = 0; i < width; i++)
			vec.ધકેલો_back(inport(portname, i));
		પાછા vec;
	}

	પૂર્ણાંક not_inport(IdString portname, પૂર્ણાંક portbit = 0)
	{
		પાછા inport(portname, portbit, સાચું);
	}

	પૂર્ણાંક not_gate(પૂર્ણાંક A)
	{
		Aig_ગાંઠ ગાંઠ(aig_indices[A]);
		ગાંઠ.outports.clear();
		ગાંઠ.inverter = !ગાંઠ.inverter;
		પાછા node2index(ગાંઠ);
	}

	પૂર્ણાંક and_gate(પૂર્ણાંક A, પૂર્ણાંક B, તાર્કિક inverter = ખોટું)
	{
		જો (A == B)
			પાછા inverter ? not_gate(A) : A;

		અચલ Aig_ગાંઠ &nA = aig_indices[A];
		અચલ Aig_ગાંઠ &nB = aig_indices[B];

		Aig_ગાંઠ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		જો (nA == nB_inv)
			પાછા bool_ગાંઠ(inverter);

		તાર્કિક nA_bool = nA.portbit < 0 && nA.ડાબું_parent < 0 && nA.જમણું_parent < 0;
		તાર્કિક nB_bool = nB.portbit < 0 && nB.ડાબું_parent < 0 && nB.જમણું_parent < 0;

		જો (nA_bool && nB_bool) {
			તાર્કિક bA = nA.inverter;
			તાર્કિક bB = nB.inverter;
			પાછા bool_ગાંઠ(inverter != (bA && bB));
		}

		જો (nA_bool) {
			તાર્કિક bA = nA.inverter;
			જો (inverter)
				પાછા bA ? not_gate(B) : bool_ગાંઠ(સાચું);
			પાછા bA ? B : bool_ગાંઠ(ખોટું);
		}

		જો (nB_bool) {
			તાર્કિક bB = nB.inverter;
			જો (inverter)
				પાછા bB ? not_gate(A) : bool_ગાંઠ(સાચું);
			પાછા bB ? A : bool_ગાંઠ(ખોટું);
		}

		Aig_ગાંઠ ગાંઠ;
		ગાંઠ.inverter = inverter;
		ગાંઠ.ડાબું_parent = A;
		ગાંઠ.જમણું_parent = B;
		પાછા node2index(ગાંઠ);
	}

	પૂર્ણાંક nand_gate(પૂર્ણાંક A, પૂર્ણાંક B)
	{
		પાછા and_gate(A, B, સાચું);
	}

	પૂર્ણાંક or_gate(પૂર્ણાંક A, પૂર્ણાંક B)
	{
		પૂર્ણાંક not_a = not_gate(A);
		પૂર્ણાંક not_b = not_gate(B);
		પાછા nand_gate(not_a, not_b);
	}

	પૂર્ણાંક nor_gate(પૂર્ણાંક A, પૂર્ણાંક B)
	{
		પૂર્ણાંક not_a = not_gate(A);
		પૂર્ણાંક not_b = not_gate(B);
		પાછા and_gate(not_a, not_b);
	}

	પૂર્ણાંક xor_gate(પૂર્ણાંક A, પૂર્ણાંક B)
	{
		પૂર્ણાંક a_and_b = and_gate(A, B);
		પૂર્ણાંક a_nor_b = nor_gate(A, B);
		પાછા nor_gate(a_and_b, a_nor_b);
	}

	પૂર્ણાંક xnor_gate(પૂર્ણાંક A, પૂર્ણાંક B)
	{
		પૂર્ણાંક a_and_b = and_gate(A, B);
		પૂર્ણાંક a_nor_b = nor_gate(A, B);
		પાછા or_gate(a_and_b, a_nor_b);
	}

	પૂર્ણાંક andnot_gate(પૂર્ણાંક A, પૂર્ણાંક B)
	{
		પૂર્ણાંક not_b = not_gate(B);
		પાછા and_gate(A, not_b);
	}

	પૂર્ણાંક ornot_gate(પૂર્ણાંક A, પૂર્ણાંક B)
	{
		પૂર્ણાંક not_b = not_gate(B);
		પાછા or_gate(A, not_b);
	}

	પૂર્ણાંક mux_gate(પૂર્ણાંક A, પૂર્ણાંક B, પૂર્ણાંક S)
	{
		પૂર્ણાંક not_s = not_gate(S);
		પૂર્ણાંક a_active = and_gate(A, not_s);
		પૂર્ણાંક b_active = and_gate(B, S);
		પાછા or_gate(a_active, b_active);
	}

	vector<પૂર્ણાંક> adder(અચલ vector<પૂર્ણાંક> &A, અચલ vector<પૂર્ણાંક> &B, પૂર્ણાંક carry_in, vector<પૂર્ણાંક> *X = nullptr, vector<પૂર્ણાંક> *CO = nullptr)
	{
		vector<પૂર્ણાંક> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		માટે (પૂર્ણાંક i = 0; i < GetSize(A); i++) {
			પૂર્ણાંક a_xor_b = xor_gate(A[i], B[i]);
			પૂર્ણાંક a_or_b = or_gate(A[i], B[i]);
			પૂર્ણાંક a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			પૂર્ણાંક tmp = and_gate(a_or_b, carry_in);
			પૂર્ણાંક carry_out = or_gate(a_and_b, tmp);
			જો (X != nullptr)
				X->at(i) = a_xor_b;
			જો (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		પાછા Y;
	}

	શૂન્ય outport(પૂર્ણાંક ગાંઠ, IdString portname, પૂર્ણાંક portbit = 0)
	{
		જો (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(ગાંઠ).outports.ધકેલો_back(pair<IdString, પૂર્ણાંક>(portname, portbit));
	}

	શૂન્ય outport_bool(પૂર્ણાંક ગાંઠ, IdString portname)
	{
		outport(ગાંઠ, portname);
		માટે (પૂર્ણાંક i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_ગાંઠ(ખોટું), portname, i);
	}

	શૂન્ય outport_vec(અચલ vector<પૂર્ણાંક> &vec, IdString portname)
	{
		માટે (પૂર્ણાંક i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	જો (cell->type.isPublic())
		પાછા;

	AigMaker mk(આ, cell);
	name = cell->type.str();

	string mkname_last;
	તાર્કિક mkname_a_signed = ખોટું;
	તાર્કિક mkname_b_signed = ખોટું;
	તાર્કિક mkname_is_signed = ખોટું;

	cell->parameters.sort();
	માટે (auto p : cell->parameters)
	{
		જો (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} નહીંતર જો (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} નહીંતર {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = ખોટું;
		mkname_b_signed = ખોટું;
		mkname_is_signed = ખોટું;
		જો (p.first == ID::A_SIGNED) {
			mkname_a_signed = સાચું;
			mkname_is_signed = p.second.as_bool();
		}
		જો (p.first == ID::B_SIGNED) {
			mkname_b_signed = સાચું;
			mkname_is_signed = p.second.as_bool();
		}
	}

	જો (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		માટે (પૂર્ણાંક i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			પૂર્ણાંક A = mk.inport(ID::A, i);
			પૂર્ણાંક Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		જાઓ optimize;
	}

	જો (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		માટે (પૂર્ણાંક i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			પૂર્ણાંક A = mk.inport(ID::A, i);
			પૂર્ણાંક B = mk.inport(ID::B, i);
			પૂર્ણાંક Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		જાઓ optimize;
	}

	જો (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		પૂર્ણાંક S = mk.inport(ID::S);
		માટે (પૂર્ણાંક i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			પૂર્ણાંક A = mk.inport(ID::A, i);
			પૂર્ણાંક B = mk.inport(ID::B, i);
			પૂર્ણાંક Y = mk.mux_gate(A, B, S);
			જો (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		જાઓ optimize;
	}

	જો (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		પૂર્ણાંક Y = mk.inport(ID::A, 0);
		માટે (પૂર્ણાંક i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			પૂર્ણાંક A = mk.inport(ID::A, i);
			જો (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			જો (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			જો (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			જો (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			જો (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		જો (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		માટે (પૂર્ણાંક i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ગાંઠ(ખોટું), ID::Y, i);
		જાઓ optimize;
	}

	જો (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		પૂર્ણાંક A = mk.inport(ID::A, 0), Y = -1;
		માટે (પૂર્ણાંક i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		જો (cell->type.in(ID($logic_and), ID($logic_or))) {
			પૂર્ણાંક B = mk.inport(ID::B, 0);
			માટે (પૂર્ણાંક i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			જો (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			જો (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} નહીંતર {
			જો (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		જાઓ optimize;
	}

	જો (cell->type.in(ID($add), ID($sub)))
	{
		પૂર્ણાંક width = GetSize(cell->getPort(ID::Y));
		vector<પૂર્ણાંક> A = mk.inport_vec(ID::A, width);
		vector<પૂર્ણાંક> B = mk.inport_vec(ID::B, width);
		પૂર્ણાંક carry = mk.bool_ગાંઠ(ખોટું);
		જો (cell->type == ID($sub)) {
			માટે (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<પૂર્ણાંક> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		જાઓ optimize;
	}

	જો (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		પૂર્ણાંક width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<પૂર્ણાંક> A = mk.inport_vec(ID::A, width);
		vector<પૂર્ણાંક> B = mk.inport_vec(ID::B, width);

		જો (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		પૂર્ણાંક carry = mk.bool_ગાંઠ(!cell->type.in(ID($le), ID($ge)));
		માટે (auto &n : B)
			n = mk.not_gate(n);
		vector<પૂર્ણાંક> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		માટે (પૂર્ણાંક i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ગાંઠ(ખોટું), ID::Y, i);
		જાઓ optimize;
	}

	જો (cell->type == ID($alu))
	{
		પૂર્ણાંક width = GetSize(cell->getPort(ID::Y));
		vector<પૂર્ણાંક> A = mk.inport_vec(ID::A, width);
		vector<પૂર્ણાંક> B = mk.inport_vec(ID::B, width);
		પૂર્ણાંક carry = mk.inport(ID::CI);
		પૂર્ણાંક binv = mk.inport(ID::BI);
		માટે (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<પૂર્ણાંક> X(width), CO(width);
		vector<પૂર્ણાંક> Y = mk.adder(A, B, carry, &X, &CO);
		માટે (પૂર્ણાંક i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		જાઓ optimize;
	}

	જો (cell->type.in(ID($eq), ID($ne)))
	{
		પૂર્ણાંક width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<પૂર્ણાંક> A = mk.inport_vec(ID::A, width);
		vector<પૂર્ણાંક> B = mk.inport_vec(ID::B, width);
		પૂર્ણાંક Y = mk.bool_ગાંઠ(ખોટું);
		માટે (પૂર્ણાંક i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		જો (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		જાઓ optimize;
	}

	જો (cell->type == ID($_AOI3_))
	{
		પૂર્ણાંક A = mk.inport(ID::A);
		પૂર્ણાંક B = mk.inport(ID::B);
		પૂર્ણાંક C = mk.inport(ID::C);
		પૂર્ણાંક Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		જાઓ optimize;
	}

	જો (cell->type == ID($_OAI3_))
	{
		પૂર્ણાંક A = mk.inport(ID::A);
		પૂર્ણાંક B = mk.inport(ID::B);
		પૂર્ણાંક C = mk.inport(ID::C);
		પૂર્ણાંક Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		જાઓ optimize;
	}

	જો (cell->type == ID($_AOI4_))
	{
		પૂર્ણાંક A = mk.inport(ID::A);
		પૂર્ણાંક B = mk.inport(ID::B);
		પૂર્ણાંક C = mk.inport(ID::C);
		પૂર્ણાંક D = mk.inport(ID::D);
		પૂર્ણાંક a_and_b = mk.and_gate(A, B);
		પૂર્ણાંક Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		જાઓ optimize;
	}

	જો (cell->type == ID($_OAI4_))
	{
		પૂર્ણાંક A = mk.inport(ID::A);
		પૂર્ણાંક B = mk.inport(ID::B);
		પૂર્ણાંક C = mk.inport(ID::C);
		પૂર્ણાંક D = mk.inport(ID::D);
		પૂર્ણાંક a_or_b = mk.or_gate(A, B);
		પૂર્ણાંક Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		જાઓ optimize;
	}

	name.clear();
	પાછા;

optimize:;
	pool<પૂર્ણાંક> used_old_ids;
	vector<Aig_ગાંઠ> new_nodes;
	dict<પૂર્ણાંક, પૂર્ણાંક> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	માટે (પૂર્ણાંક i = GetSize(nodes)-1; i >= 0; i--) {
		જો (!nodes[i].outports.empty())
			used_old_ids.ઉમેરો(i);
		જો (!used_old_ids.ગણતરી(i))
			ચાલુ;
		જો (nodes[i].ડાબું_parent >= 0)
			used_old_ids.ઉમેરો(nodes[i].ડાબું_parent);
		જો (nodes[i].જમણું_parent >= 0)
			used_old_ids.ઉમેરો(nodes[i].જમણું_parent);
	}

	માટે (પૂર્ણાંક i = 0; i < GetSize(nodes); i++) {
		જો (!used_old_ids.ગણતરી(i))
			ચાલુ;
		nodes[i].ડાબું_parent = old_to_new_ids.at(nodes[i].ડાબું_parent);
		nodes[i].જમણું_parent = old_to_new_ids.at(nodes[i].જમણું_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.ધકેલો_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
