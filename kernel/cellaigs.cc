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

#అన్తర్భావ "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_కణుపు::Aig_కణుపు()
{
	portbit = -1;
	inverter = అబద్ధం;
	ఎడమ_మాతృ = -1;
	కుడి_మాతృ = -1;
}

తార్కిక Aig_కణుపు::క్రియాకారకం==(స్థిరరాశి Aig_కణుపు &other) స్థిరరాశి
{
	ఒకవేళ (portname != other.portname) తిరిగి అబద్ధం;
	ఒకవేళ (portbit != other.portbit) తిరిగి అబద్ధం;
	ఒకవేళ (inverter != other.inverter) తిరిగి అబద్ధం;
	ఒకవేళ (ఎడమ_మాతృ != other.ఎడమ_మాతృ) తిరిగి అబద్ధం;
	ఒకవేళ (కుడి_మాతృ != other.కుడి_మాతృ) తిరిగి అబద్ధం;
	తిరిగి నిజం;
}

Hasher Aig_కణుపు::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(ఎడమ_మాతృ);
	h.eat(కుడి_మాతృ);
	తిరిగి h;
}

తార్కిక Aig::క్రియాకారకం==(స్థిరరాశి Aig &other) స్థిరరాశి
{
	తిరిగి name == other.name;
}

Hasher Aig::hash_into(Hasher h) స్థిరరాశి
{
	h.eat(name);
	తిరిగి h;
}

నిర్మాణం AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_కణుపు> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	పూర్ణాంకం node2index(స్థిరరాశి Aig_కణుపు &కణుపు)
	{
		ఒకవేళ (కణుపు.ఎడమ_మాతృ > కణుపు.కుడి_మాతృ) {
			Aig_కణుపు n(కణుపు);
			std::మార్పిడి(n.ఎడమ_మాతృ, n.కుడి_మాతృ);
			తిరిగి node2index(n);
		}

		ఒకవేళ (!aig_indices.లెక్క(కణుపు)) {
			aig_indices.expect(కణుపు, Get_కొలత(aig->nodes));
			aig->nodes.నెట్టు_వెనుక(కణుపు);
		}

		తిరిగి aig_indices.at(కణుపు);
	}

	పూర్ణాంకం bool_కణుపు(తార్కిక విలువ)
	{
		Aig_కణుపు కణుపు;
		కణుపు.inverter = విలువ;
		తిరిగి node2index(కణుపు);
	}

	పూర్ణాంకం inport(IdString portname, పూర్ణాంకం portbit = 0, తార్కిక inverter = అబద్ధం)
	{
		ఒకవేళ (portbit >= Get_కొలత(cell->get_పోర్టు(portname))) {
			IdString signed_param = cell->twines().కనుగొను(
				cell->twines().str(portname) + "_SIGNED");
		ఒకవేళ (signed_param != IdString::Null && cell->parameters.లెక్క(signed_param) && cell->getParam(signed_param).as_bool())
				తిరిగి inport(portname, Get_కొలత(cell->get_పోర్టు(portname))-1, inverter);
			తిరిగి bool_కణుపు(inverter);
		}

		Aig_కణుపు కణుపు;
		కణుపు.portname = portname;
		కణుపు.portbit = portbit;
		కణుపు.inverter = inverter;
		తిరిగి node2index(కణుపు);
	}

	సదిశ<పూర్ణాంకం> inport_vec(IdString portname, పూర్ణాంకం వెడల్పు)
	{
		సదిశ<పూర్ణాంకం> vec;
		కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
			vec.నెట్టు_వెనుక(inport(portname, i));
		తిరిగి vec;
	}

	పూర్ణాంకం not_inport(IdString portname, పూర్ణాంకం portbit = 0)
	{
		తిరిగి inport(portname, portbit, నిజం);
	}

	పూర్ణాంకం not_gate(పూర్ణాంకం A)
	{
		Aig_కణుపు కణుపు(aig_indices[A]);
		కణుపు.outports.clear();
		కణుపు.inverter = !కణుపు.inverter;
		తిరిగి node2index(కణుపు);
	}

	పూర్ణాంకం and_gate(పూర్ణాంకం A, పూర్ణాంకం B, తార్కిక inverter = అబద్ధం)
	{
		ఒకవేళ (A == B)
			తిరిగి inverter ? not_gate(A) : A;

		స్థిరరాశి Aig_కణుపు &nA = aig_indices[A];
		స్థిరరాశి Aig_కణుపు &nB = aig_indices[B];

		Aig_కణుపు nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ఒకవేళ (nA == nB_inv)
			తిరిగి bool_కణుపు(inverter);

		తార్కిక nA_bool = nA.portbit < 0 && nA.ఎడమ_మాతృ < 0 && nA.కుడి_మాతృ < 0;
		తార్కిక nB_bool = nB.portbit < 0 && nB.ఎడమ_మాతృ < 0 && nB.కుడి_మాతృ < 0;

		ఒకవేళ (nA_bool && nB_bool) {
			తార్కిక bA = nA.inverter;
			తార్కిక bB = nB.inverter;
			తిరిగి bool_కణుపు(inverter != (bA && bB));
		}

		ఒకవేళ (nA_bool) {
			తార్కిక bA = nA.inverter;
			ఒకవేళ (inverter)
				తిరిగి bA ? not_gate(B) : bool_కణుపు(నిజం);
			తిరిగి bA ? B : bool_కణుపు(అబద్ధం);
		}

		ఒకవేళ (nB_bool) {
			తార్కిక bB = nB.inverter;
			ఒకవేళ (inverter)
				తిరిగి bB ? not_gate(A) : bool_కణుపు(నిజం);
			తిరిగి bB ? A : bool_కణుపు(అబద్ధం);
		}

		Aig_కణుపు కణుపు;
		కణుపు.inverter = inverter;
		కణుపు.ఎడమ_మాతృ = A;
		కణుపు.కుడి_మాతృ = B;
		తిరిగి node2index(కణుపు);
	}

	పూర్ణాంకం nand_gate(పూర్ణాంకం A, పూర్ణాంకం B)
	{
		తిరిగి and_gate(A, B, నిజం);
	}

	పూర్ణాంకం or_gate(పూర్ణాంకం A, పూర్ణాంకం B)
	{
		పూర్ణాంకం not_a = not_gate(A);
		పూర్ణాంకం not_b = not_gate(B);
		తిరిగి nand_gate(not_a, not_b);
	}

	పూర్ణాంకం nor_gate(పూర్ణాంకం A, పూర్ణాంకం B)
	{
		పూర్ణాంకం not_a = not_gate(A);
		పూర్ణాంకం not_b = not_gate(B);
		తిరిగి and_gate(not_a, not_b);
	}

	పూర్ణాంకం xor_gate(పూర్ణాంకం A, పూర్ణాంకం B)
	{
		పూర్ణాంకం a_and_b = and_gate(A, B);
		పూర్ణాంకం a_nor_b = nor_gate(A, B);
		తిరిగి nor_gate(a_and_b, a_nor_b);
	}

	పూర్ణాంకం xnor_gate(పూర్ణాంకం A, పూర్ణాంకం B)
	{
		పూర్ణాంకం a_and_b = and_gate(A, B);
		పూర్ణాంకం a_nor_b = nor_gate(A, B);
		తిరిగి or_gate(a_and_b, a_nor_b);
	}

	పూర్ణాంకం andnot_gate(పూర్ణాంకం A, పూర్ణాంకం B)
	{
		పూర్ణాంకం not_b = not_gate(B);
		తిరిగి and_gate(A, not_b);
	}

	పూర్ణాంకం ornot_gate(పూర్ణాంకం A, పూర్ణాంకం B)
	{
		పూర్ణాంకం not_b = not_gate(B);
		తిరిగి or_gate(A, not_b);
	}

	పూర్ణాంకం mux_gate(పూర్ణాంకం A, పూర్ణాంకం B, పూర్ణాంకం S)
	{
		పూర్ణాంకం not_s = not_gate(S);
		పూర్ణాంకం a_చురుకైన = and_gate(A, not_s);
		పూర్ణాంకం b_చురుకైన = and_gate(B, S);
		తిరిగి or_gate(a_చురుకైన, b_చురుకైన);
	}

	సదిశ<పూర్ణాంకం> adder(స్థిరరాశి సదిశ<పూర్ణాంకం> &A, స్థిరరాశి సదిశ<పూర్ణాంకం> &B, పూర్ణాంకం వాహకం_లోపల, సదిశ<పూర్ణాంకం> *X = శూన్యసూచిక, సదిశ<పూర్ణాంకం> *CO = శూన్యసూచిక)
	{
		సదిశ<పూర్ణాంకం> Y(Get_కొలత(A));
		log_assert(Get_కొలత(A) == Get_కొలత(B));
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(A); i++) {
			పూర్ణాంకం a_xor_b = xor_gate(A[i], B[i]);
			పూర్ణాంకం a_or_b = or_gate(A[i], B[i]);
			పూర్ణాంకం a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, వాహకం_లోపల);
			పూర్ణాంకం tmp = and_gate(a_or_b, వాహకం_లోపల);
			పూర్ణాంకం వాహకం_బహిర్గతం = or_gate(a_and_b, tmp);
			ఒకవేళ (X != శూన్యసూచిక)
				X->at(i) = a_xor_b;
			ఒకవేళ (CO != శూన్యసూచిక)
				CO->at(i) = వాహకం_బహిర్గతం;
			వాహకం_లోపల = వాహకం_బహిర్గతం;
		}
		తిరిగి Y;
	}

	శూన్య outport(పూర్ణాంకం కణుపు, IdString portname, పూర్ణాంకం portbit = 0)
	{
		ఒకవేళ (portbit < Get_కొలత(cell->get_పోర్టు(portname)))
			aig->nodes.at(కణుపు).outports.నెట్టు_వెనుక(pair<IdString, పూర్ణాంకం>(portname, portbit));
	}

	శూన్య outport_bool(పూర్ణాంకం కణుపు, IdString portname)
	{
		outport(కణుపు, portname);
		కొరకు (పూర్ణాంకం i = 1; i < Get_కొలత(cell->get_పోర్టు(portname)); i++)
			outport(bool_కణుపు(అబద్ధం), portname, i);
	}

	శూన్య outport_vec(స్థిరరాశి సదిశ<పూర్ణాంకం> &vec, IdString portname)
	{
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ఒకవేళ (cell->type.is_ప్రజా())
		తిరిగి;

	AigMaker mk(ఇది, cell);
	name = cell->type.str();

	string mkname_చివరి;
	తార్కిక mkname_a_signed = అబద్ధం;
	తార్కిక mkname_b_signed = అబద్ధం;
	తార్కిక mkname_is_signed = అబద్ధం;

	cell->parameters.క్రమీకరించు();
	కొరకు (స్వయంచాలక p : cell->parameters)
	{
		ఒకవేళ (p.మొదటి == ID::A_వెడల్పు && mkname_a_signed) {
			name = mkname_చివరి + stringf(":%d%c", p.రెండవ.as_int(), mkname_is_signed ? 'S' : 'U');
		} లేకపోతే ఒకవేళ (p.మొదటి == ID::B_వెడల్పు && mkname_b_signed) {
			name = mkname_చివరి + stringf(":%d%c", p.రెండవ.as_int(), mkname_is_signed ? 'S' : 'U');
		} లేకపోతే {
			mkname_చివరి = name;
			name += stringf(":%d", p.రెండవ.as_int());
		}

		mkname_a_signed = అబద్ధం;
		mkname_b_signed = అబద్ధం;
		mkname_is_signed = అబద్ధం;
		ఒకవేళ (p.మొదటి == ID::A_SIGNED) {
			mkname_a_signed = నిజం;
			mkname_is_signed = p.రెండవ.as_bool();
		}
		ఒకవేళ (p.మొదటి == ID::B_SIGNED) {
			mkname_b_signed = నిజం;
			mkname_is_signed = p.రెండవ.as_bool();
		}
	}

	ఒకవేళ (cell->type.లోపల(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(cell->get_పోర్టు(ID::Y)); i++) {
			పూర్ణాంకం A = mk.inport(ID::A, i);
			పూర్ణాంకం Y = cell->type.లోపల(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type.లోపల(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(cell->get_పోర్టు(ID::Y)); i++) {
			పూర్ణాంకం A = mk.inport(ID::A, i);
			పూర్ణాంకం B = mk.inport(ID::B, i);
			పూర్ణాంకం Y = cell->type.లోపల(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.లోపల(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.లోపల(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.లోపల(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.లోపల(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.లోపల(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.లోపల(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.లోపల(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type.లోపల(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		పూర్ణాంకం S = mk.inport(ID::S);
		కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(cell->get_పోర్టు(ID::Y)); i++) {
			పూర్ణాంకం A = mk.inport(ID::A, i);
			పూర్ణాంకం B = mk.inport(ID::B, i);
			పూర్ణాంకం Y = mk.mux_gate(A, B, S);
			ఒకవేళ (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type.లోపల(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		పూర్ణాంకం Y = mk.inport(ID::A, 0);
		కొరకు (పూర్ణాంకం i = 1; i < Get_కొలత(cell->get_పోర్టు(ID::A)); i++) {
			పూర్ణాంకం A = mk.inport(ID::A, i);
			ఒకవేళ (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ఒకవేళ (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ఒకవేళ (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ఒకవేళ (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ఒకవేళ (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ఒకవేళ (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		కొరకు (పూర్ణాంకం i = 1; i < Get_కొలత(cell->get_పోర్టు(ID::Y)); i++)
			mk.outport(mk.bool_కణుపు(అబద్ధం), ID::Y, i);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type.లోపల(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		పూర్ణాంకం A = mk.inport(ID::A, 0), Y = -1;
		కొరకు (పూర్ణాంకం i = 1; i < Get_కొలత(cell->get_పోర్టు(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ఒకవేళ (cell->type.లోపల(ID($logic_and), ID($logic_or))) {
			పూర్ణాంకం B = mk.inport(ID::B, 0);
			కొరకు (పూర్ణాంకం i = 1; i < Get_కొలత(cell->get_పోర్టు(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ఒకవేళ (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ఒకవేళ (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} లేకపోతే {
			ఒకవేళ (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type.లోపల(ID($కూడు), ID($sub)))
	{
		పూర్ణాంకం వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));
		సదిశ<పూర్ణాంకం> A = mk.inport_vec(ID::A, వెడల్పు);
		సదిశ<పూర్ణాంకం> B = mk.inport_vec(ID::B, వెడల్పు);
		పూర్ణాంకం వాహకం = mk.bool_కణుపు(అబద్ధం);
		ఒకవేళ (cell->type == ID($sub)) {
			కొరకు (స్వయంచాలక &n : B)
				n = mk.not_gate(n);
			వాహకం = mk.not_gate(వాహకం);
		}
		సదిశ<పూర్ణాంకం> Y = mk.adder(A, B, వాహకం);
		mk.outport_vec(Y, ID::Y);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type.లోపల(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		పూర్ణాంకం వెడల్పు = std::max(Get_కొలత(cell->get_పోర్టు(ID::A)),
							 Get_కొలత(cell->get_పోర్టు(ID::B))) + 1;
		సదిశ<పూర్ణాంకం> A = mk.inport_vec(ID::A, వెడల్పు);
		సదిశ<పూర్ణాంకం> B = mk.inport_vec(ID::B, వెడల్పు);

		ఒకవేళ (cell->type.లోపల(ID($gt), ID($ge)))
			std::మార్పిడి(A, B);

		పూర్ణాంకం వాహకం = mk.bool_కణుపు(!cell->type.లోపల(ID($le), ID($ge)));
		కొరకు (స్వయంచాలక &n : B)
			n = mk.not_gate(n);
		సదిశ<పూర్ణాంకం> Y = mk.adder(A, B, వాహకం);
		mk.outport(Y.వెనుక(), ID::Y);
		కొరకు (పూర్ణాంకం i = 1; i < Get_కొలత(cell->get_పోర్టు(ID::Y)); i++)
			mk.outport(mk.bool_కణుపు(అబద్ధం), ID::Y, i);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type == ID($alu))
	{
		పూర్ణాంకం వెడల్పు = Get_కొలత(cell->get_పోర్టు(ID::Y));
		సదిశ<పూర్ణాంకం> A = mk.inport_vec(ID::A, వెడల్పు);
		సదిశ<పూర్ణాంకం> B = mk.inport_vec(ID::B, వెడల్పు);
		పూర్ణాంకం వాహకం = mk.inport(ID::CI);
		పూర్ణాంకం binv = mk.inport(ID::BI);
		కొరకు (స్వయంచాలక &n : B)
			n = mk.xor_gate(n, binv);
		సదిశ<పూర్ణాంకం> X(వెడల్పు), CO(వెడల్పు);
		సదిశ<పూర్ణాంకం> Y = mk.adder(A, B, వాహకం, &X, &CO);
		కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type.లోపల(ID($eq), ID($ne)))
	{
		పూర్ణాంకం వెడల్పు = max(Get_కొలత(cell->get_పోర్టు(ID::A)), Get_కొలత(cell->get_పోర్టు(ID::B)));
		సదిశ<పూర్ణాంకం> A = mk.inport_vec(ID::A, వెడల్పు);
		సదిశ<పూర్ణాంకం> B = mk.inport_vec(ID::B, వెడల్పు);
		పూర్ణాంకం Y = mk.bool_కణుపు(అబద్ధం);
		కొరకు (పూర్ణాంకం i = 0; i < వెడల్పు; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ఒకవేళ (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type == ID($_AOI3_))
	{
		పూర్ణాంకం A = mk.inport(ID::A);
		పూర్ణాంకం B = mk.inport(ID::B);
		పూర్ణాంకం C = mk.inport(ID::C);
		పూర్ణాంకం Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type == ID($_OAI3_))
	{
		పూర్ణాంకం A = mk.inport(ID::A);
		పూర్ణాంకం B = mk.inport(ID::B);
		పూర్ణాంకం C = mk.inport(ID::C);
		పూర్ణాంకం Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type == ID($_AOI4_))
	{
		పూర్ణాంకం A = mk.inport(ID::A);
		పూర్ణాంకం B = mk.inport(ID::B);
		పూర్ణాంకం C = mk.inport(ID::C);
		పూర్ణాంకం D = mk.inport(ID::D);
		పూర్ణాంకం a_and_b = mk.and_gate(A, B);
		పూర్ణాంకం Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		వెళ్ళు optimize;
	}

	ఒకవేళ (cell->type == ID($_OAI4_))
	{
		పూర్ణాంకం A = mk.inport(ID::A);
		పూర్ణాంకం B = mk.inport(ID::B);
		పూర్ణాంకం C = mk.inport(ID::C);
		పూర్ణాంకం D = mk.inport(ID::D);
		పూర్ణాంకం a_or_b = mk.or_gate(A, B);
		పూర్ణాంకం Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		వెళ్ళు optimize;
	}

	name.clear();
	తిరిగి;

optimize:;
	pool<పూర్ణాంకం> వాడిన_old_ids;
	సదిశ<Aig_కణుపు> new_nodes;
	dict<పూర్ణాంకం, పూర్ణాంకం> old_గమ్యం_new_ids;
	old_గమ్యం_new_ids[-1] = -1;

	కొరకు (పూర్ణాంకం i = Get_కొలత(nodes)-1; i >= 0; i--) {
		ఒకవేళ (!nodes[i].outports.ఖాళీలు())
			వాడిన_old_ids.చొప్పించు(i);
		ఒకవేళ (!వాడిన_old_ids.లెక్క(i))
			కొనసాగు;
		ఒకవేళ (nodes[i].ఎడమ_మాతృ >= 0)
			వాడిన_old_ids.చొప్పించు(nodes[i].ఎడమ_మాతృ);
		ఒకవేళ (nodes[i].కుడి_మాతృ >= 0)
			వాడిన_old_ids.చొప్పించు(nodes[i].కుడి_మాతృ);
	}

	కొరకు (పూర్ణాంకం i = 0; i < Get_కొలత(nodes); i++) {
		ఒకవేళ (!వాడిన_old_ids.లెక్క(i))
			కొనసాగు;
		nodes[i].ఎడమ_మాతృ = old_గమ్యం_new_ids.at(nodes[i].ఎడమ_మాతృ);
		nodes[i].కుడి_మాతృ = old_గమ్యం_new_ids.at(nodes[i].కుడి_మాతృ);
		old_గమ్యం_new_ids[i] = Get_కొలత(new_nodes);
		new_nodes.నెట్టు_వెనుక(nodes[i]);
	}

	new_nodes.మార్పిడి(nodes);
}

YOSYS_NAMESPACE_చివర
