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

#अन्तर्भाव "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_ग्रन्थिः::Aig_ग्रन्थिः()
{
	portbit = -1;
	inverter = असत्य;
	वामः_parent = -1;
	दक्षिणः_parent = -1;
}

तार्किक Aig_ग्रन्थिः::संक्रियकः==(ध्रुव Aig_ग्रन्थिः &other) ध्रुव
{
	यदि (portname != other.portname) प्रत्यागम असत्य;
	यदि (portbit != other.portbit) प्रत्यागम असत्य;
	यदि (inverter != other.inverter) प्रत्यागम असत्य;
	यदि (वामः_parent != other.वामः_parent) प्रत्यागम असत्य;
	यदि (दक्षिणः_parent != other.दक्षिणः_parent) प्रत्यागम असत्य;
	प्रत्यागम सत्य;
}

Hasher Aig_ग्रन्थिः::hash_into(Hasher h) ध्रुव
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(वामः_parent);
	h.eat(दक्षिणः_parent);
	प्रत्यागम h;
}

तार्किक Aig::संक्रियकः==(ध्रुव Aig &other) ध्रुव
{
	प्रत्यागम name == other.name;
}

Hasher Aig::hash_into(Hasher h) ध्रुव
{
	h.eat(name);
	प्रत्यागम h;
}

संरचना AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_ग्रन्थिः> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	पूर्णाङ्क node2index(ध्रुव Aig_ग्रन्थिः &ग्रन्थिः)
	{
		यदि (ग्रन्थिः.वामः_parent > ग्रन्थिः.दक्षिणः_parent) {
			Aig_ग्रन्थिः n(ग्रन्थिः);
			std::swap(n.वामः_parent, n.दक्षिणः_parent);
			प्रत्यागम node2index(n);
		}

		यदि (!aig_indices.गणनम्(ग्रन्थिः)) {
			aig_indices.expect(ग्रन्थिः, Get_आकार(aig->nodes));
			aig->nodes.प्रक्षिप_पृष्ठभागः(ग्रन्थिः);
		}

		प्रत्यागम aig_indices.at(ग्रन्थिः);
	}

	पूर्णाङ्क bool_ग्रन्थिः(तार्किक मानम्)
	{
		Aig_ग्रन्थिः ग्रन्थिः;
		ग्रन्थिः.inverter = मानम्;
		प्रत्यागम node2index(ग्रन्थिः);
	}

	पूर्णाङ्क inport(IdString portname, पूर्णाङ्क portbit = 0, तार्किक inverter = असत्य)
	{
		यदि (portbit >= Get_आकार(cell->get_पत्तनम्(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		यदि (signed_param != IdString::Null && cell->parameters.गणनम्(signed_param) && cell->getParam(signed_param).as_bool())
				प्रत्यागम inport(portname, Get_आकार(cell->get_पत्तनम्(portname))-1, inverter);
			प्रत्यागम bool_ग्रन्थिः(inverter);
		}

		Aig_ग्रन्थिः ग्रन्थिः;
		ग्रन्थिः.portname = portname;
		ग्रन्थिः.portbit = portbit;
		ग्रन्थिः.inverter = inverter;
		प्रत्यागम node2index(ग्रन्थिः);
	}

	सदिशम्<पूर्णाङ्क> inport_vec(IdString portname, पूर्णाङ्क विस्तारः)
	{
		सदिशम्<पूर्णाङ्क> vec;
		प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
			vec.प्रक्षिप_पृष्ठभागः(inport(portname, i));
		प्रत्यागम vec;
	}

	पूर्णाङ्क not_inport(IdString portname, पूर्णाङ्क portbit = 0)
	{
		प्रत्यागम inport(portname, portbit, सत्य);
	}

	पूर्णाङ्क not_gate(पूर्णाङ्क A)
	{
		Aig_ग्रन्थिः ग्रन्थिः(aig_indices[A]);
		ग्रन्थिः.outports.clear();
		ग्रन्थिः.inverter = !ग्रन्थिः.inverter;
		प्रत्यागम node2index(ग्रन्थिः);
	}

	पूर्णाङ्क and_gate(पूर्णाङ्क A, पूर्णाङ्क B, तार्किक inverter = असत्य)
	{
		यदि (A == B)
			प्रत्यागम inverter ? not_gate(A) : A;

		ध्रुव Aig_ग्रन्थिः &nA = aig_indices[A];
		ध्रुव Aig_ग्रन्थिः &nB = aig_indices[B];

		Aig_ग्रन्थिः nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		यदि (nA == nB_inv)
			प्रत्यागम bool_ग्रन्थिः(inverter);

		तार्किक nA_bool = nA.portbit < 0 && nA.वामः_parent < 0 && nA.दक्षिणः_parent < 0;
		तार्किक nB_bool = nB.portbit < 0 && nB.वामः_parent < 0 && nB.दक्षिणः_parent < 0;

		यदि (nA_bool && nB_bool) {
			तार्किक bA = nA.inverter;
			तार्किक bB = nB.inverter;
			प्रत्यागम bool_ग्रन्थिः(inverter != (bA && bB));
		}

		यदि (nA_bool) {
			तार्किक bA = nA.inverter;
			यदि (inverter)
				प्रत्यागम bA ? not_gate(B) : bool_ग्रन्थिः(सत्य);
			प्रत्यागम bA ? B : bool_ग्रन्थिः(असत्य);
		}

		यदि (nB_bool) {
			तार्किक bB = nB.inverter;
			यदि (inverter)
				प्रत्यागम bB ? not_gate(A) : bool_ग्रन्थिः(सत्य);
			प्रत्यागम bB ? A : bool_ग्रन्थिः(असत्य);
		}

		Aig_ग्रन्थिः ग्रन्थिः;
		ग्रन्थिः.inverter = inverter;
		ग्रन्थिः.वामः_parent = A;
		ग्रन्थिः.दक्षिणः_parent = B;
		प्रत्यागम node2index(ग्रन्थिः);
	}

	पूर्णाङ्क nand_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		प्रत्यागम and_gate(A, B, सत्य);
	}

	पूर्णाङ्क or_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_a = not_gate(A);
		पूर्णाङ्क not_b = not_gate(B);
		प्रत्यागम nand_gate(not_a, not_b);
	}

	पूर्णाङ्क nor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_a = not_gate(A);
		पूर्णाङ्क not_b = not_gate(B);
		प्रत्यागम and_gate(not_a, not_b);
	}

	पूर्णाङ्क xor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क a_and_b = and_gate(A, B);
		पूर्णाङ्क a_nor_b = nor_gate(A, B);
		प्रत्यागम nor_gate(a_and_b, a_nor_b);
	}

	पूर्णाङ्क xnor_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क a_and_b = and_gate(A, B);
		पूर्णाङ्क a_nor_b = nor_gate(A, B);
		प्रत्यागम or_gate(a_and_b, a_nor_b);
	}

	पूर्णाङ्क andnot_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_b = not_gate(B);
		प्रत्यागम and_gate(A, not_b);
	}

	पूर्णाङ्क ornot_gate(पूर्णाङ्क A, पूर्णाङ्क B)
	{
		पूर्णाङ्क not_b = not_gate(B);
		प्रत्यागम or_gate(A, not_b);
	}

	पूर्णाङ्क mux_gate(पूर्णाङ्क A, पूर्णाङ्क B, पूर्णाङ्क S)
	{
		पूर्णाङ्क not_s = not_gate(S);
		पूर्णाङ्क a_सक्रिय = and_gate(A, not_s);
		पूर्णाङ्क b_सक्रिय = and_gate(B, S);
		प्रत्यागम or_gate(a_सक्रिय, b_सक्रिय);
	}

	सदिशम्<पूर्णाङ्क> adder(ध्रुव सदिशम्<पूर्णाङ्क> &A, ध्रुव सदिशम्<पूर्णाङ्क> &B, पूर्णाङ्क carry_अन्तर्, सदिशम्<पूर्णाङ्क> *X = शून्यसूचकः, सदिशम्<पूर्णाङ्क> *CO = शून्यसूचकः)
	{
		सदिशम्<पूर्णाङ्क> Y(Get_आकार(A));
		log_assert(Get_आकार(A) == Get_आकार(B));
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(A); i++) {
			पूर्णाङ्क a_xor_b = xor_gate(A[i], B[i]);
			पूर्णाङ्क a_or_b = or_gate(A[i], B[i]);
			पूर्णाङ्क a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_अन्तर्);
			पूर्णाङ्क tmp = and_gate(a_or_b, carry_अन्तर्);
			पूर्णाङ्क carry_बहिर्गमनम् = or_gate(a_and_b, tmp);
			यदि (X != शून्यसूचकः)
				X->at(i) = a_xor_b;
			यदि (CO != शून्यसूचकः)
				CO->at(i) = carry_बहिर्गमनम्;
			carry_अन्तर् = carry_बहिर्गमनम्;
		}
		प्रत्यागम Y;
	}

	शून्य outport(पूर्णाङ्क ग्रन्थिः, IdString portname, पूर्णाङ्क portbit = 0)
	{
		यदि (portbit < Get_आकार(cell->get_पत्तनम्(portname)))
			aig->nodes.at(ग्रन्थिः).outports.प्रक्षिप_पृष्ठभागः(pair<IdString, पूर्णाङ्क>(portname, portbit));
	}

	शून्य outport_bool(पूर्णाङ्क ग्रन्थिः, IdString portname)
	{
		outport(ग्रन्थिः, portname);
		प्रति (पूर्णाङ्क i = 1; i < Get_आकार(cell->get_पत्तनम्(portname)); i++)
			outport(bool_ग्रन्थिः(असत्य), portname, i);
	}

	शून्य outport_vec(ध्रुव सदिशम्<पूर्णाङ्क> &vec, IdString portname)
	{
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	यदि (cell->type.is_लौकिकम्())
		प्रत्यागम;

	AigMaker mk(एतत्, cell);
	name = cell->type.str();

	string mkname_चरमः;
	तार्किक mkname_a_signed = असत्य;
	तार्किक mkname_b_signed = असत्य;
	तार्किक mkname_is_signed = असत्य;

	cell->parameters.क्रमय();
	प्रति (स्वचालित p : cell->parameters)
	{
		यदि (p.first == ID::A_विस्तारः && mkname_a_signed) {
			name = mkname_चरमः + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा यदि (p.first == ID::B_विस्तारः && mkname_b_signed) {
			name = mkname_चरमः + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} अन्यथा {
			mkname_चरमः = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = असत्य;
		mkname_b_signed = असत्य;
		mkname_is_signed = असत्य;
		यदि (p.first == ID::A_SIGNED) {
			mkname_a_signed = सत्य;
			mkname_is_signed = p.second.as_bool();
		}
		यदि (p.first == ID::B_SIGNED) {
			mkname_b_signed = सत्य;
			mkname_is_signed = p.second.as_bool();
		}
	}

	यदि (cell->type.अन्तर्(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(cell->get_पत्तनम्(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क Y = cell->type.अन्तर्(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	यदि (cell->type.अन्तर्(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(cell->get_पत्तनम्(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क B = mk.inport(ID::B, i);
			पूर्णाङ्क Y = cell->type.अन्तर्(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.अन्तर्(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.अन्तर्(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.अन्तर्(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.अन्तर्(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.अन्तर्(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.अन्तर्(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.अन्तर्(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	यदि (cell->type.अन्तर्(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		पूर्णाङ्क S = mk.inport(ID::S);
		प्रति (पूर्णाङ्क i = 0; i < Get_आकार(cell->get_पत्तनम्(ID::Y)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			पूर्णाङ्क B = mk.inport(ID::B, i);
			पूर्णाङ्क Y = mk.mux_gate(A, B, S);
			यदि (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		गच्छ optimize;
	}

	यदि (cell->type.अन्तर्(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		पूर्णाङ्क Y = mk.inport(ID::A, 0);
		प्रति (पूर्णाङ्क i = 1; i < Get_आकार(cell->get_पत्तनम्(ID::A)); i++) {
			पूर्णाङ्क A = mk.inport(ID::A, i);
			यदि (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			यदि (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			यदि (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			यदि (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			यदि (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		यदि (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		प्रति (पूर्णाङ्क i = 1; i < Get_आकार(cell->get_पत्तनम्(ID::Y)); i++)
			mk.outport(mk.bool_ग्रन्थिः(असत्य), ID::Y, i);
		गच्छ optimize;
	}

	यदि (cell->type.अन्तर्(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		पूर्णाङ्क A = mk.inport(ID::A, 0), Y = -1;
		प्रति (पूर्णाङ्क i = 1; i < Get_आकार(cell->get_पत्तनम्(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		यदि (cell->type.अन्तर्(ID($logic_and), ID($logic_or))) {
			पूर्णाङ्क B = mk.inport(ID::B, 0);
			प्रति (पूर्णाङ्क i = 1; i < Get_आकार(cell->get_पत्तनम्(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			यदि (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			यदि (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} अन्यथा {
			यदि (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type.अन्तर्(ID($सङ्कलय), ID($sub)))
	{
		पूर्णाङ्क विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));
		सदिशम्<पूर्णाङ्क> A = mk.inport_vec(ID::A, विस्तारः);
		सदिशम्<पूर्णाङ्क> B = mk.inport_vec(ID::B, विस्तारः);
		पूर्णाङ्क carry = mk.bool_ग्रन्थिः(असत्य);
		यदि (cell->type == ID($sub)) {
			प्रति (स्वचालित &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		सदिशम्<पूर्णाङ्क> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type.अन्तर्(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		पूर्णाङ्क विस्तारः = std::max(Get_आकार(cell->get_पत्तनम्(ID::A)),
							 Get_आकार(cell->get_पत्तनम्(ID::B))) + 1;
		सदिशम्<पूर्णाङ्क> A = mk.inport_vec(ID::A, विस्तारः);
		सदिशम्<पूर्णाङ्क> B = mk.inport_vec(ID::B, विस्तारः);

		यदि (cell->type.अन्तर्(ID($gt), ID($ge)))
			std::swap(A, B);

		पूर्णाङ्क carry = mk.bool_ग्रन्थिः(!cell->type.अन्तर्(ID($le), ID($ge)));
		प्रति (स्वचालित &n : B)
			n = mk.not_gate(n);
		सदिशम्<पूर्णाङ्क> Y = mk.adder(A, B, carry);
		mk.outport(Y.पृष्ठभागः(), ID::Y);
		प्रति (पूर्णाङ्क i = 1; i < Get_आकार(cell->get_पत्तनम्(ID::Y)); i++)
			mk.outport(mk.bool_ग्रन्थिः(असत्य), ID::Y, i);
		गच्छ optimize;
	}

	यदि (cell->type == ID($alu))
	{
		पूर्णाङ्क विस्तारः = Get_आकार(cell->get_पत्तनम्(ID::Y));
		सदिशम्<पूर्णाङ्क> A = mk.inport_vec(ID::A, विस्तारः);
		सदिशम्<पूर्णाङ्क> B = mk.inport_vec(ID::B, विस्तारः);
		पूर्णाङ्क carry = mk.inport(ID::CI);
		पूर्णाङ्क binv = mk.inport(ID::BI);
		प्रति (स्वचालित &n : B)
			n = mk.xor_gate(n, binv);
		सदिशम्<पूर्णाङ्क> X(विस्तारः), CO(विस्तारः);
		सदिशम्<पूर्णाङ्क> Y = mk.adder(A, B, carry, &X, &CO);
		प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		गच्छ optimize;
	}

	यदि (cell->type.अन्तर्(ID($eq), ID($ne)))
	{
		पूर्णाङ्क विस्तारः = max(Get_आकार(cell->get_पत्तनम्(ID::A)), Get_आकार(cell->get_पत्तनम्(ID::B)));
		सदिशम्<पूर्णाङ्क> A = mk.inport_vec(ID::A, विस्तारः);
		सदिशम्<पूर्णाङ्क> B = mk.inport_vec(ID::B, विस्तारः);
		पूर्णाङ्क Y = mk.bool_ग्रन्थिः(असत्य);
		प्रति (पूर्णाङ्क i = 0; i < विस्तारः; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		यदि (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type == ID($_AOI3_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type == ID($_OAI3_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type == ID($_AOI4_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क D = mk.inport(ID::D);
		पूर्णाङ्क a_and_b = mk.and_gate(A, B);
		पूर्णाङ्क Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	यदि (cell->type == ID($_OAI4_))
	{
		पूर्णाङ्क A = mk.inport(ID::A);
		पूर्णाङ्क B = mk.inport(ID::B);
		पूर्णाङ्क C = mk.inport(ID::C);
		पूर्णाङ्क D = mk.inport(ID::D);
		पूर्णाङ्क a_or_b = mk.or_gate(A, B);
		पूर्णाङ्क Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		गच्छ optimize;
	}

	name.clear();
	प्रत्यागम;

optimize:;
	pool<पूर्णाङ्क> प्रयुक्तम्_old_ids;
	सदिशम्<Aig_ग्रन्थिः> new_nodes;
	dict<पूर्णाङ्क, पूर्णाङ्क> old_गन्तव्यम्_new_ids;
	old_गन्तव्यम्_new_ids[-1] = -1;

	प्रति (पूर्णाङ्क i = Get_आकार(nodes)-1; i >= 0; i--) {
		यदि (!nodes[i].outports.रिक्तानि())
			प्रयुक्तम्_old_ids.निवेशय(i);
		यदि (!प्रयुक्तम्_old_ids.गणनम्(i))
			अनुवर्त;
		यदि (nodes[i].वामः_parent >= 0)
			प्रयुक्तम्_old_ids.निवेशय(nodes[i].वामः_parent);
		यदि (nodes[i].दक्षिणः_parent >= 0)
			प्रयुक्तम्_old_ids.निवेशय(nodes[i].दक्षिणः_parent);
	}

	प्रति (पूर्णाङ्क i = 0; i < Get_आकार(nodes); i++) {
		यदि (!प्रयुक्तम्_old_ids.गणनम्(i))
			अनुवर्त;
		nodes[i].वामः_parent = old_गन्तव्यम्_new_ids.at(nodes[i].वामः_parent);
		nodes[i].दक्षिणः_parent = old_गन्तव्यम्_new_ids.at(nodes[i].दक्षिणः_parent);
		old_गन्तव्यम्_new_ids[i] = Get_आकार(new_nodes);
		new_nodes.प्रक्षिप_पृष्ठभागः(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_अन्तः
