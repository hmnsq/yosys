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

#ଅନ୍ତର୍ଭାଵ "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_ଗଣ୍ଠି::Aig_ଗଣ୍ଠି()
{
	portbit = -1;
	inverter = ମିଥ୍ୟା;
	ବାମ_parent = -1;
	ଡାହାଣ_parent = -1;
}

ତାର୍କିକ Aig_ଗଣ୍ଠି::operator==(ଅଚଳ Aig_ଗଣ୍ଠି &other) ଅଚଳ
{
	ଯଦି (portname != other.portname) ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (portbit != other.portbit) ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (inverter != other.inverter) ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (ବାମ_parent != other.ବାମ_parent) ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଯଦି (ଡାହାଣ_parent != other.ଡାହାଣ_parent) ଫେରନ୍ତୁ ମିଥ୍ୟା;
	ଫେରନ୍ତୁ ସତ୍ୟ;
}

Hasher Aig_ଗଣ୍ଠି::hash_into(Hasher h) ଅଚଳ
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(ବାମ_parent);
	h.eat(ଡାହାଣ_parent);
	ଫେରନ୍ତୁ h;
}

ତାର୍କିକ Aig::operator==(ଅଚଳ Aig &other) ଅଚଳ
{
	ଫେରନ୍ତୁ name == other.name;
}

Hasher Aig::hash_into(Hasher h) ଅଚଳ
{
	h.eat(name);
	ଫେରନ୍ତୁ h;
}

ସଂରଚନା AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_ଗଣ୍ଠି> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା node2index(ଅଚଳ Aig_ଗଣ୍ଠି &ଗଣ୍ଠି)
	{
		ଯଦି (ଗଣ୍ଠି.ବାମ_parent > ଗଣ୍ଠି.ଡାହାଣ_parent) {
			Aig_ଗଣ୍ଠି n(ଗଣ୍ଠି);
			std::swap(n.ବାମ_parent, n.ଡାହାଣ_parent);
			ଫେରନ୍ତୁ node2index(n);
		}

		ଯଦି (!aig_indices.ଗଣନା_2(ଗଣ୍ଠି)) {
			aig_indices.expect(ଗଣ୍ଠି, GetSize(aig->nodes));
			aig->nodes.ଠେଲ_back(ଗଣ୍ଠି);
		}

		ଫେରନ୍ତୁ aig_indices.at(ଗଣ୍ଠି);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା bool_ଗଣ୍ଠି(ତାର୍କିକ ମୂଲ୍ୟ)
	{
		Aig_ଗଣ୍ଠି ଗଣ୍ଠି;
		ଗଣ୍ଠି.inverter = ମୂଲ୍ୟ;
		ଫେରନ୍ତୁ node2index(ଗଣ୍ଠି);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା inport(IdString portname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା portbit = 0, ତାର୍କିକ inverter = ମିଥ୍ୟା)
	{
		ଯଦି (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		ଯଦି (signed_param != IdString::Null && cell->parameters.ଗଣନା_2(signed_param) && cell->getParam(signed_param).as_bool())
				ଫେରନ୍ତୁ inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			ଫେରନ୍ତୁ bool_ଗଣ୍ଠି(inverter);
		}

		Aig_ଗଣ୍ଠି ଗଣ୍ଠି;
		ଗଣ୍ଠି.portname = portname;
		ଗଣ୍ଠି.portbit = portbit;
		ଗଣ୍ଠି.inverter = inverter;
		ଫେରନ୍ତୁ node2index(ଗଣ୍ଠି);
	}

	vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> inport_vec(IdString portname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା width)
	{
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> vec;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
			vec.ଠେଲ_back(inport(portname, i));
		ଫେରନ୍ତୁ vec;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_inport(IdString portname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା portbit = 0)
	{
		ଫେରନ୍ତୁ inport(portname, portbit, ସତ୍ୟ);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A)
	{
		Aig_ଗଣ୍ଠି ଗଣ୍ଠି(aig_indices[A]);
		ଗଣ୍ଠି.outports.clear();
		ଗଣ୍ଠି.inverter = !ଗଣ୍ଠି.inverter;
		ଫେରନ୍ତୁ node2index(ଗଣ୍ଠି);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା and_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B, ତାର୍କିକ inverter = ମିଥ୍ୟା)
	{
		ଯଦି (A == B)
			ଫେରନ୍ତୁ inverter ? not_gate(A) : A;

		ଅଚଳ Aig_ଗଣ୍ଠି &nA = aig_indices[A];
		ଅଚଳ Aig_ଗଣ୍ଠି &nB = aig_indices[B];

		Aig_ଗଣ୍ଠି nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ଯଦି (nA == nB_inv)
			ଫେରନ୍ତୁ bool_ଗଣ୍ଠି(inverter);

		ତାର୍କିକ nA_bool = nA.portbit < 0 && nA.ବାମ_parent < 0 && nA.ଡାହାଣ_parent < 0;
		ତାର୍କିକ nB_bool = nB.portbit < 0 && nB.ବାମ_parent < 0 && nB.ଡାହାଣ_parent < 0;

		ଯଦି (nA_bool && nB_bool) {
			ତାର୍କିକ bA = nA.inverter;
			ତାର୍କିକ bB = nB.inverter;
			ଫେରନ୍ତୁ bool_ଗଣ୍ଠି(inverter != (bA && bB));
		}

		ଯଦି (nA_bool) {
			ତାର୍କିକ bA = nA.inverter;
			ଯଦି (inverter)
				ଫେରନ୍ତୁ bA ? not_gate(B) : bool_ଗଣ୍ଠି(ସତ୍ୟ);
			ଫେରନ୍ତୁ bA ? B : bool_ଗଣ୍ଠି(ମିଥ୍ୟା);
		}

		ଯଦି (nB_bool) {
			ତାର୍କିକ bB = nB.inverter;
			ଯଦି (inverter)
				ଫେରନ୍ତୁ bB ? not_gate(A) : bool_ଗଣ୍ଠି(ସତ୍ୟ);
			ଫେରନ୍ତୁ bB ? A : bool_ଗଣ୍ଠି(ମିଥ୍ୟା);
		}

		Aig_ଗଣ୍ଠି ଗଣ୍ଠି;
		ଗଣ୍ଠି.inverter = inverter;
		ଗଣ୍ଠି.ବାମ_parent = A;
		ଗଣ୍ଠି.ଡାହାଣ_parent = B;
		ଫେରନ୍ତୁ node2index(ଗଣ୍ଠି);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nand_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B)
	{
		ଫେରନ୍ତୁ and_gate(A, B, ସତ୍ୟ);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା or_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_a = not_gate(A);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_b = not_gate(B);
		ଫେରନ୍ତୁ nand_gate(not_a, not_b);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା nor_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_a = not_gate(A);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_b = not_gate(B);
		ଫେରନ୍ତୁ and_gate(not_a, not_b);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା xor_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_and_b = and_gate(A, B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_nor_b = nor_gate(A, B);
		ଫେରନ୍ତୁ nor_gate(a_and_b, a_nor_b);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା xnor_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_and_b = and_gate(A, B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_nor_b = nor_gate(A, B);
		ଫେରନ୍ତୁ or_gate(a_and_b, a_nor_b);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା andnot_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_b = not_gate(B);
		ଫେରନ୍ତୁ and_gate(A, not_b);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ornot_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_b = not_gate(B);
		ଫେରନ୍ତୁ or_gate(A, not_b);
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା mux_gate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା A, ପୂର୍ଣ୍ଣସଂଖ୍ୟା B, ପୂର୍ଣ୍ଣସଂଖ୍ୟା S)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା not_s = not_gate(S);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_active = and_gate(A, not_s);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_active = and_gate(B, S);
		ଫେରନ୍ତୁ or_gate(a_active, b_active);
	}

	vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> adder(ଅଚଳ vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &A, ଅଚଳ vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &B, ପୂର୍ଣ୍ଣସଂଖ୍ୟା carry_in, vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> *X = nullptr, vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> *CO = nullptr)
	{
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(A); i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_xor_b = xor_gate(A[i], B[i]);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_or_b = or_gate(A[i], B[i]);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା tmp = and_gate(a_or_b, carry_in);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା carry_out = or_gate(a_and_b, tmp);
			ଯଦି (X != nullptr)
				X->at(i) = a_xor_b;
			ଯଦି (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		ଫେରନ୍ତୁ Y;
	}

	ଶୂନ୍ୟ outport(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି, IdString portname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା portbit = 0)
	{
		ଯଦି (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(ଗଣ୍ଠି).outports.ଠେଲ_back(pair<IdString, ପୂର୍ଣ୍ଣସଂଖ୍ୟା>(portname, portbit));
	}

	ଶୂନ୍ୟ outport_bool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣ୍ଠି, IdString portname)
	{
		outport(ଗଣ୍ଠି, portname);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_ଗଣ୍ଠି(ମିଥ୍ୟା), portname, i);
	}

	ଶୂନ୍ୟ outport_vec(ଅଚଳ vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> &vec, IdString portname)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ଯଦି (cell->type.isPublic())
		ଫେରନ୍ତୁ;

	AigMaker mk(ଏହି, cell);
	name = cell->type.str();

	string mkname_last;
	ତାର୍କିକ mkname_a_signed = ମିଥ୍ୟା;
	ତାର୍କିକ mkname_b_signed = ମିଥ୍ୟା;
	ତାର୍କିକ mkname_is_signed = ମିଥ୍ୟା;

	cell->parameters.sort();
	ପାଇଁ (auto p : cell->parameters)
	{
		ଯଦି (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} ଅନ୍ୟଥା ଯଦି (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} ଅନ୍ୟଥା {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = ମିଥ୍ୟା;
		mkname_b_signed = ମିଥ୍ୟା;
		mkname_is_signed = ମିଥ୍ୟା;
		ଯଦି (p.first == ID::A_SIGNED) {
			mkname_a_signed = ସତ୍ୟ;
			mkname_is_signed = p.second.as_bool();
		}
		ଯଦି (p.first == ID::B_SIGNED) {
			mkname_b_signed = ସତ୍ୟ;
			mkname_is_signed = p.second.as_bool();
		}
	}

	ଯଦି (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A, i);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		ଯାଅ optimize;
	}

	ଯଦି (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A, i);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା B = mk.inport(ID::B, i);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		ଯାଅ optimize;
	}

	ଯଦି (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା S = mk.inport(ID::S);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A, i);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା B = mk.inport(ID::B, i);
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = mk.mux_gate(A, B, S);
			ଯଦି (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		ଯାଅ optimize;
	}

	ଯଦି (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = mk.inport(ID::A, 0);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A, i);
			ଯଦି (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ଯଦି (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ଯଦି (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ଯଦି (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ଯଦି (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ଯଦି (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ଗଣ୍ଠି(ମିଥ୍ୟା), ID::Y, i);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A, 0), Y = -1;
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ଯଦି (cell->type.in(ID($logic_and), ID($logic_or))) {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା B = mk.inport(ID::B, 0);
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ଯଦି (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ଯଦି (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} ଅନ୍ୟଥା {
			ଯଦି (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type.in(ID($add), ID($sub)))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(cell->getPort(ID::Y));
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> A = mk.inport_vec(ID::A, width);
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> B = mk.inport_vec(ID::B, width);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା carry = mk.bool_ଗଣ୍ଠି(ମିଥ୍ୟା);
		ଯଦି (cell->type == ID($sub)) {
			ପାଇଁ (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> A = mk.inport_vec(ID::A, width);
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> B = mk.inport_vec(ID::B, width);

		ଯଦି (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା carry = mk.bool_ଗଣ୍ଠି(!cell->type.in(ID($le), ID($ge)));
		ପାଇଁ (auto &n : B)
			n = mk.not_gate(n);
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_ଗଣ୍ଠି(ମିଥ୍ୟା), ID::Y, i);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type == ID($alu))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(cell->getPort(ID::Y));
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> A = mk.inport_vec(ID::A, width);
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> B = mk.inport_vec(ID::B, width);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା carry = mk.inport(ID::CI);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା binv = mk.inport(ID::BI);
		ପାଇଁ (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> X(width), CO(width);
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> Y = mk.adder(A, B, carry, &X, &CO);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type.in(ID($eq), ID($ne)))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> A = mk.inport_vec(ID::A, width);
		vector<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> B = mk.inport_vec(ID::B, width);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = mk.bool_ଗଣ୍ଠି(ମିଥ୍ୟା);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ଯଦି (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type == ID($_AOI3_))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା B = mk.inport(ID::B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା C = mk.inport(ID::C);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type == ID($_OAI3_))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା B = mk.inport(ID::B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା C = mk.inport(ID::C);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type == ID($_AOI4_))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା B = mk.inport(ID::B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା C = mk.inport(ID::C);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା D = mk.inport(ID::D);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_and_b = mk.and_gate(A, B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		ଯାଅ optimize;
	}

	ଯଦି (cell->type == ID($_OAI4_))
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା A = mk.inport(ID::A);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା B = mk.inport(ID::B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା C = mk.inport(ID::C);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା D = mk.inport(ID::D);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_or_b = mk.or_gate(A, B);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		ଯାଅ optimize;
	}

	name.clear();
	ଫେରନ୍ତୁ;

optimize:;
	pool<ପୂର୍ଣ୍ଣସଂଖ୍ୟା> used_old_ids;
	vector<Aig_ଗଣ୍ଠି> new_nodes;
	dict<ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = GetSize(nodes)-1; i >= 0; i--) {
		ଯଦି (!nodes[i].outports.empty())
			used_old_ids.ଭର୍ତ୍ତି(i);
		ଯଦି (!used_old_ids.ଗଣନା_2(i))
			ଜାରି;
		ଯଦି (nodes[i].ବାମ_parent >= 0)
			used_old_ids.ଭର୍ତ୍ତି(nodes[i].ବାମ_parent);
		ଯଦି (nodes[i].ଡାହାଣ_parent >= 0)
			used_old_ids.ଭର୍ତ୍ତି(nodes[i].ଡାହାଣ_parent);
	}

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < GetSize(nodes); i++) {
		ଯଦି (!used_old_ids.ଗଣନା_2(i))
			ଜାରି;
		nodes[i].ବାମ_parent = old_to_new_ids.at(nodes[i].ବାମ_parent);
		nodes[i].ଡାହାଣ_parent = old_to_new_ids.at(nodes[i].ଡାହାଣ_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.ଠେଲ_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
