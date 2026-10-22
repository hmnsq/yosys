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

#അന്തര്ഭാവ "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_കണ്ണി::Aig_കണ്ണി()
{
	portbit = -1;
	inverter = അസത്യം;
	ഇടത്_parent = -1;
	വലത്_parent = -1;
}

തർക്കം Aig_കണ്ണി::operator==(സ്ഥിരം Aig_കണ്ണി &other) സ്ഥിരം
{
	എങ്കിൽ (portname != other.portname) തിരികെ അസത്യം;
	എങ്കിൽ (portbit != other.portbit) തിരികെ അസത്യം;
	എങ്കിൽ (inverter != other.inverter) തിരികെ അസത്യം;
	എങ്കിൽ (ഇടത്_parent != other.ഇടത്_parent) തിരികെ അസത്യം;
	എങ്കിൽ (വലത്_parent != other.വലത്_parent) തിരികെ അസത്യം;
	തിരികെ സത്യം;
}

Hasher Aig_കണ്ണി::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(ഇടത്_parent);
	h.eat(വലത്_parent);
	തിരികെ h;
}

തർക്കം Aig::operator==(സ്ഥിരം Aig &other) സ്ഥിരം
{
	തിരികെ name == other.name;
}

Hasher Aig::hash_into(Hasher h) സ്ഥിരം
{
	h.eat(name);
	തിരികെ h;
}

ഘടന AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_കണ്ണി> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	പൂർണ്ണസംഖ്യ node2index(സ്ഥിരം Aig_കണ്ണി &കണ്ണി)
	{
		എങ്കിൽ (കണ്ണി.ഇടത്_parent > കണ്ണി.വലത്_parent) {
			Aig_കണ്ണി n(കണ്ണി);
			std::swap(n.ഇടത്_parent, n.വലത്_parent);
			തിരികെ node2index(n);
		}

		എങ്കിൽ (!aig_indices.എണ്ണം(കണ്ണി)) {
			aig_indices.expect(കണ്ണി, GetSize(aig->nodes));
			aig->nodes.തള്ളുക_back(കണ്ണി);
		}

		തിരികെ aig_indices.at(കണ്ണി);
	}

	പൂർണ്ണസംഖ്യ bool_കണ്ണി(തർക്കം മൂല്യം)
	{
		Aig_കണ്ണി കണ്ണി;
		കണ്ണി.inverter = മൂല്യം;
		തിരികെ node2index(കണ്ണി);
	}

	പൂർണ്ണസംഖ്യ inport(IdString portname, പൂർണ്ണസംഖ്യ portbit = 0, തർക്കം inverter = അസത്യം)
	{
		എങ്കിൽ (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		എങ്കിൽ (signed_param != IdString::Null && cell->parameters.എണ്ണം(signed_param) && cell->getParam(signed_param).as_bool())
				തിരികെ inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			തിരികെ bool_കണ്ണി(inverter);
		}

		Aig_കണ്ണി കണ്ണി;
		കണ്ണി.portname = portname;
		കണ്ണി.portbit = portbit;
		കണ്ണി.inverter = inverter;
		തിരികെ node2index(കണ്ണി);
	}

	vector<പൂർണ്ണസംഖ്യ> inport_vec(IdString portname, പൂർണ്ണസംഖ്യ width)
	{
		vector<പൂർണ്ണസംഖ്യ> vec;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
			vec.തള്ളുക_back(inport(portname, i));
		തിരികെ vec;
	}

	പൂർണ്ണസംഖ്യ not_inport(IdString portname, പൂർണ്ണസംഖ്യ portbit = 0)
	{
		തിരികെ inport(portname, portbit, സത്യം);
	}

	പൂർണ്ണസംഖ്യ not_gate(പൂർണ്ണസംഖ്യ A)
	{
		Aig_കണ്ണി കണ്ണി(aig_indices[A]);
		കണ്ണി.outports.clear();
		കണ്ണി.inverter = !കണ്ണി.inverter;
		തിരികെ node2index(കണ്ണി);
	}

	പൂർണ്ണസംഖ്യ and_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B, തർക്കം inverter = അസത്യം)
	{
		എങ്കിൽ (A == B)
			തിരികെ inverter ? not_gate(A) : A;

		സ്ഥിരം Aig_കണ്ണി &nA = aig_indices[A];
		സ്ഥിരം Aig_കണ്ണി &nB = aig_indices[B];

		Aig_കണ്ണി nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		എങ്കിൽ (nA == nB_inv)
			തിരികെ bool_കണ്ണി(inverter);

		തർക്കം nA_bool = nA.portbit < 0 && nA.ഇടത്_parent < 0 && nA.വലത്_parent < 0;
		തർക്കം nB_bool = nB.portbit < 0 && nB.ഇടത്_parent < 0 && nB.വലത്_parent < 0;

		എങ്കിൽ (nA_bool && nB_bool) {
			തർക്കം bA = nA.inverter;
			തർക്കം bB = nB.inverter;
			തിരികെ bool_കണ്ണി(inverter != (bA && bB));
		}

		എങ്കിൽ (nA_bool) {
			തർക്കം bA = nA.inverter;
			എങ്കിൽ (inverter)
				തിരികെ bA ? not_gate(B) : bool_കണ്ണി(സത്യം);
			തിരികെ bA ? B : bool_കണ്ണി(അസത്യം);
		}

		എങ്കിൽ (nB_bool) {
			തർക്കം bB = nB.inverter;
			എങ്കിൽ (inverter)
				തിരികെ bB ? not_gate(A) : bool_കണ്ണി(സത്യം);
			തിരികെ bB ? A : bool_കണ്ണി(അസത്യം);
		}

		Aig_കണ്ണി കണ്ണി;
		കണ്ണി.inverter = inverter;
		കണ്ണി.ഇടത്_parent = A;
		കണ്ണി.വലത്_parent = B;
		തിരികെ node2index(കണ്ണി);
	}

	പൂർണ്ണസംഖ്യ nand_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B)
	{
		തിരികെ and_gate(A, B, സത്യം);
	}

	പൂർണ്ണസംഖ്യ or_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B)
	{
		പൂർണ്ണസംഖ്യ not_a = not_gate(A);
		പൂർണ്ണസംഖ്യ not_b = not_gate(B);
		തിരികെ nand_gate(not_a, not_b);
	}

	പൂർണ്ണസംഖ്യ nor_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B)
	{
		പൂർണ്ണസംഖ്യ not_a = not_gate(A);
		പൂർണ്ണസംഖ്യ not_b = not_gate(B);
		തിരികെ and_gate(not_a, not_b);
	}

	പൂർണ്ണസംഖ്യ xor_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B)
	{
		പൂർണ്ണസംഖ്യ a_and_b = and_gate(A, B);
		പൂർണ്ണസംഖ്യ a_nor_b = nor_gate(A, B);
		തിരികെ nor_gate(a_and_b, a_nor_b);
	}

	പൂർണ്ണസംഖ്യ xnor_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B)
	{
		പൂർണ്ണസംഖ്യ a_and_b = and_gate(A, B);
		പൂർണ്ണസംഖ്യ a_nor_b = nor_gate(A, B);
		തിരികെ or_gate(a_and_b, a_nor_b);
	}

	പൂർണ്ണസംഖ്യ andnot_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B)
	{
		പൂർണ്ണസംഖ്യ not_b = not_gate(B);
		തിരികെ and_gate(A, not_b);
	}

	പൂർണ്ണസംഖ്യ ornot_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B)
	{
		പൂർണ്ണസംഖ്യ not_b = not_gate(B);
		തിരികെ or_gate(A, not_b);
	}

	പൂർണ്ണസംഖ്യ mux_gate(പൂർണ്ണസംഖ്യ A, പൂർണ്ണസംഖ്യ B, പൂർണ്ണസംഖ്യ S)
	{
		പൂർണ്ണസംഖ്യ not_s = not_gate(S);
		പൂർണ്ണസംഖ്യ a_active = and_gate(A, not_s);
		പൂർണ്ണസംഖ്യ b_active = and_gate(B, S);
		തിരികെ or_gate(a_active, b_active);
	}

	vector<പൂർണ്ണസംഖ്യ> adder(സ്ഥിരം vector<പൂർണ്ണസംഖ്യ> &A, സ്ഥിരം vector<പൂർണ്ണസംഖ്യ> &B, പൂർണ്ണസംഖ്യ carry_in, vector<പൂർണ്ണസംഖ്യ> *X = nullptr, vector<പൂർണ്ണസംഖ്യ> *CO = nullptr)
	{
		vector<പൂർണ്ണസംഖ്യ> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(A); i++) {
			പൂർണ്ണസംഖ്യ a_xor_b = xor_gate(A[i], B[i]);
			പൂർണ്ണസംഖ്യ a_or_b = or_gate(A[i], B[i]);
			പൂർണ്ണസംഖ്യ a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			പൂർണ്ണസംഖ്യ tmp = and_gate(a_or_b, carry_in);
			പൂർണ്ണസംഖ്യ carry_out = or_gate(a_and_b, tmp);
			എങ്കിൽ (X != nullptr)
				X->at(i) = a_xor_b;
			എങ്കിൽ (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		തിരികെ Y;
	}

	ശൂന്യം outport(പൂർണ്ണസംഖ്യ കണ്ണി, IdString portname, പൂർണ്ണസംഖ്യ portbit = 0)
	{
		എങ്കിൽ (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(കണ്ണി).outports.തള്ളുക_back(pair<IdString, പൂർണ്ണസംഖ്യ>(portname, portbit));
	}

	ശൂന്യം outport_bool(പൂർണ്ണസംഖ്യ കണ്ണി, IdString portname)
	{
		outport(കണ്ണി, portname);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_കണ്ണി(അസത്യം), portname, i);
	}

	ശൂന്യം outport_vec(സ്ഥിരം vector<പൂർണ്ണസംഖ്യ> &vec, IdString portname)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	എങ്കിൽ (cell->type.isPublic())
		തിരികെ;

	AigMaker mk(ഇത്, cell);
	name = cell->type.str();

	string mkname_last;
	തർക്കം mkname_a_signed = അസത്യം;
	തർക്കം mkname_b_signed = അസത്യം;
	തർക്കം mkname_is_signed = അസത്യം;

	cell->parameters.sort();
	വേണ്ടി (auto p : cell->parameters)
	{
		എങ്കിൽ (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} അല്ലെങ്കിൽ എങ്കിൽ (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} അല്ലെങ്കിൽ {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = അസത്യം;
		mkname_b_signed = അസത്യം;
		mkname_is_signed = അസത്യം;
		എങ്കിൽ (p.first == ID::A_SIGNED) {
			mkname_a_signed = സത്യം;
			mkname_is_signed = p.second.as_bool();
		}
		എങ്കിൽ (p.first == ID::B_SIGNED) {
			mkname_b_signed = സത്യം;
			mkname_is_signed = p.second.as_bool();
		}
	}

	എങ്കിൽ (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			പൂർണ്ണസംഖ്യ A = mk.inport(ID::A, i);
			പൂർണ്ണസംഖ്യ Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			പൂർണ്ണസംഖ്യ A = mk.inport(ID::A, i);
			പൂർണ്ണസംഖ്യ B = mk.inport(ID::B, i);
			പൂർണ്ണസംഖ്യ Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		പൂർണ്ണസംഖ്യ S = mk.inport(ID::S);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			പൂർണ്ണസംഖ്യ A = mk.inport(ID::A, i);
			പൂർണ്ണസംഖ്യ B = mk.inport(ID::B, i);
			പൂർണ്ണസംഖ്യ Y = mk.mux_gate(A, B, S);
			എങ്കിൽ (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		പൂർണ്ണസംഖ്യ Y = mk.inport(ID::A, 0);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			പൂർണ്ണസംഖ്യ A = mk.inport(ID::A, i);
			എങ്കിൽ (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			എങ്കിൽ (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			എങ്കിൽ (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			എങ്കിൽ (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			എങ്കിൽ (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		എങ്കിൽ (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_കണ്ണി(അസത്യം), ID::Y, i);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		പൂർണ്ണസംഖ്യ A = mk.inport(ID::A, 0), Y = -1;
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		എങ്കിൽ (cell->type.in(ID($logic_and), ID($logic_or))) {
			പൂർണ്ണസംഖ്യ B = mk.inport(ID::B, 0);
			വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			എങ്കിൽ (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			എങ്കിൽ (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} അല്ലെങ്കിൽ {
			എങ്കിൽ (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type.in(ID($add), ID($sub)))
	{
		പൂർണ്ണസംഖ്യ width = GetSize(cell->getPort(ID::Y));
		vector<പൂർണ്ണസംഖ്യ> A = mk.inport_vec(ID::A, width);
		vector<പൂർണ്ണസംഖ്യ> B = mk.inport_vec(ID::B, width);
		പൂർണ്ണസംഖ്യ carry = mk.bool_കണ്ണി(അസത്യം);
		എങ്കിൽ (cell->type == ID($sub)) {
			വേണ്ടി (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<പൂർണ്ണസംഖ്യ> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		പൂർണ്ണസംഖ്യ width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<പൂർണ്ണസംഖ്യ> A = mk.inport_vec(ID::A, width);
		vector<പൂർണ്ണസംഖ്യ> B = mk.inport_vec(ID::B, width);

		എങ്കിൽ (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		പൂർണ്ണസംഖ്യ carry = mk.bool_കണ്ണി(!cell->type.in(ID($le), ID($ge)));
		വേണ്ടി (auto &n : B)
			n = mk.not_gate(n);
		vector<പൂർണ്ണസംഖ്യ> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_കണ്ണി(അസത്യം), ID::Y, i);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type == ID($alu))
	{
		പൂർണ്ണസംഖ്യ width = GetSize(cell->getPort(ID::Y));
		vector<പൂർണ്ണസംഖ്യ> A = mk.inport_vec(ID::A, width);
		vector<പൂർണ്ണസംഖ്യ> B = mk.inport_vec(ID::B, width);
		പൂർണ്ണസംഖ്യ carry = mk.inport(ID::CI);
		പൂർണ്ണസംഖ്യ binv = mk.inport(ID::BI);
		വേണ്ടി (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<പൂർണ്ണസംഖ്യ> X(width), CO(width);
		vector<പൂർണ്ണസംഖ്യ> Y = mk.adder(A, B, carry, &X, &CO);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type.in(ID($eq), ID($ne)))
	{
		പൂർണ്ണസംഖ്യ width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<പൂർണ്ണസംഖ്യ> A = mk.inport_vec(ID::A, width);
		vector<പൂർണ്ണസംഖ്യ> B = mk.inport_vec(ID::B, width);
		പൂർണ്ണസംഖ്യ Y = mk.bool_കണ്ണി(അസത്യം);
		വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		എങ്കിൽ (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type == ID($_AOI3_))
	{
		പൂർണ്ണസംഖ്യ A = mk.inport(ID::A);
		പൂർണ്ണസംഖ്യ B = mk.inport(ID::B);
		പൂർണ്ണസംഖ്യ C = mk.inport(ID::C);
		പൂർണ്ണസംഖ്യ Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type == ID($_OAI3_))
	{
		പൂർണ്ണസംഖ്യ A = mk.inport(ID::A);
		പൂർണ്ണസംഖ്യ B = mk.inport(ID::B);
		പൂർണ്ണസംഖ്യ C = mk.inport(ID::C);
		പൂർണ്ണസംഖ്യ Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type == ID($_AOI4_))
	{
		പൂർണ്ണസംഖ്യ A = mk.inport(ID::A);
		പൂർണ്ണസംഖ്യ B = mk.inport(ID::B);
		പൂർണ്ണസംഖ്യ C = mk.inport(ID::C);
		പൂർണ്ണസംഖ്യ D = mk.inport(ID::D);
		പൂർണ്ണസംഖ്യ a_and_b = mk.and_gate(A, B);
		പൂർണ്ണസംഖ്യ Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		പോകുക optimize;
	}

	എങ്കിൽ (cell->type == ID($_OAI4_))
	{
		പൂർണ്ണസംഖ്യ A = mk.inport(ID::A);
		പൂർണ്ണസംഖ്യ B = mk.inport(ID::B);
		പൂർണ്ണസംഖ്യ C = mk.inport(ID::C);
		പൂർണ്ണസംഖ്യ D = mk.inport(ID::D);
		പൂർണ്ണസംഖ്യ a_or_b = mk.or_gate(A, B);
		പൂർണ്ണസംഖ്യ Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		പോകുക optimize;
	}

	name.clear();
	തിരികെ;

optimize:;
	pool<പൂർണ്ണസംഖ്യ> used_old_ids;
	vector<Aig_കണ്ണി> new_nodes;
	dict<പൂർണ്ണസംഖ്യ, പൂർണ്ണസംഖ്യ> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = GetSize(nodes)-1; i >= 0; i--) {
		എങ്കിൽ (!nodes[i].outports.empty())
			used_old_ids.തിരുകുക(i);
		എങ്കിൽ (!used_old_ids.എണ്ണം(i))
			തുടരുക;
		എങ്കിൽ (nodes[i].ഇടത്_parent >= 0)
			used_old_ids.തിരുകുക(nodes[i].ഇടത്_parent);
		എങ്കിൽ (nodes[i].വലത്_parent >= 0)
			used_old_ids.തിരുകുക(nodes[i].വലത്_parent);
	}

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < GetSize(nodes); i++) {
		എങ്കിൽ (!used_old_ids.എണ്ണം(i))
			തുടരുക;
		nodes[i].ഇടത്_parent = old_to_new_ids.at(nodes[i].ഇടത്_parent);
		nodes[i].വലത്_parent = old_to_new_ids.at(nodes[i].വലത്_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.തള്ളുക_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
