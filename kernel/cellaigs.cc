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

#ضمن "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_گرہ::Aig_گرہ()
{
	portbit = -1;
	inverter = خطأ;
	بایاں_ماپا = -1;
	دایاں_ماپا = -1;
}

منطقي Aig_گرہ::عامل==(ثابت Aig_گرہ &other) ثابت
{
	إذا (portname != other.portname) أرجع خطأ;
	إذا (portbit != other.portbit) أرجع خطأ;
	إذا (inverter != other.inverter) أرجع خطأ;
	إذا (بایاں_ماپا != other.بایاں_ماپا) أرجع خطأ;
	إذا (دایاں_ماپا != other.دایاں_ماپا) أرجع خطأ;
	أرجع سچ;
}

Hasher Aig_گرہ::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(بایاں_ماپا);
	h.eat(دایاں_ماپا);
	أرجع h;
}

منطقي Aig::عامل==(ثابت Aig &other) ثابت
{
	أرجع name == other.name;
}

Hasher Aig::hash_into(Hasher h) ثابت
{
	h.eat(name);
	أرجع h;
}

بنية AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_گرہ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحيح node2index(ثابت Aig_گرہ &گرہ)
	{
		إذا (گرہ.بایاں_ماپا > گرہ.دایاں_ماپا) {
			Aig_گرہ n(گرہ);
			std::وٹاندرا(n.بایاں_ماپا, n.دایاں_ماپا);
			أرجع node2index(n);
		}

		إذا (!aig_indices.گنتی(گرہ)) {
			aig_indices.expect(گرہ, Get_ناپ(aig->nodes));
			aig->nodes.ڈالو_پٹھ(گرہ);
		}

		أرجع aig_indices.at(گرہ);
	}

	صحيح bool_گرہ(منطقي قدر)
	{
		Aig_گرہ گرہ;
		گرہ.inverter = قدر;
		أرجع node2index(گرہ);
	}

	صحيح inport(IdString portname, صحيح portbit = 0, منطقي inverter = خطأ)
	{
		إذا (portbit >= Get_ناپ(cell->get_پورٹ(portname))) {
			IdString signed_param = cell->twines().لبھو(
				cell->twines().str(portname) + "_SIGNED");
		إذا (signed_param != IdString::Null && cell->parameters.گنتی(signed_param) && cell->getParam(signed_param).as_bool())
				أرجع inport(portname, Get_ناپ(cell->get_پورٹ(portname))-1, inverter);
			أرجع bool_گرہ(inverter);
		}

		Aig_گرہ گرہ;
		گرہ.portname = portname;
		گرہ.portbit = portbit;
		گرہ.inverter = inverter;
		أرجع node2index(گرہ);
	}

	سمتیہ<صحيح> inport_vec(IdString portname, صحيح چوڑائی)
	{
		سمتیہ<صحيح> vec;
		لكل (صحيح i = 0; i < چوڑائی; i++)
			vec.ڈالو_پٹھ(inport(portname, i));
		أرجع vec;
	}

	صحيح not_inport(IdString portname, صحيح portbit = 0)
	{
		أرجع inport(portname, portbit, سچ);
	}

	صحيح not_gate(صحيح A)
	{
		Aig_گرہ گرہ(aig_indices[A]);
		گرہ.outports.clear();
		گرہ.inverter = !گرہ.inverter;
		أرجع node2index(گرہ);
	}

	صحيح and_gate(صحيح A, صحيح B, منطقي inverter = خطأ)
	{
		إذا (A == B)
			أرجع inverter ? not_gate(A) : A;

		ثابت Aig_گرہ &nA = aig_indices[A];
		ثابت Aig_گرہ &nB = aig_indices[B];

		Aig_گرہ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		إذا (nA == nB_inv)
			أرجع bool_گرہ(inverter);

		منطقي nA_bool = nA.portbit < 0 && nA.بایاں_ماپا < 0 && nA.دایاں_ماپا < 0;
		منطقي nB_bool = nB.portbit < 0 && nB.بایاں_ماپا < 0 && nB.دایاں_ماپا < 0;

		إذا (nA_bool && nB_bool) {
			منطقي bA = nA.inverter;
			منطقي bB = nB.inverter;
			أرجع bool_گرہ(inverter != (bA && bB));
		}

		إذا (nA_bool) {
			منطقي bA = nA.inverter;
			إذا (inverter)
				أرجع bA ? not_gate(B) : bool_گرہ(سچ);
			أرجع bA ? B : bool_گرہ(خطأ);
		}

		إذا (nB_bool) {
			منطقي bB = nB.inverter;
			إذا (inverter)
				أرجع bB ? not_gate(A) : bool_گرہ(سچ);
			أرجع bB ? A : bool_گرہ(خطأ);
		}

		Aig_گرہ گرہ;
		گرہ.inverter = inverter;
		گرہ.بایاں_ماپا = A;
		گرہ.دایاں_ماپا = B;
		أرجع node2index(گرہ);
	}

	صحيح nand_gate(صحيح A, صحيح B)
	{
		أرجع and_gate(A, B, سچ);
	}

	صحيح or_gate(صحيح A, صحيح B)
	{
		صحيح not_a = not_gate(A);
		صحيح not_b = not_gate(B);
		أرجع nand_gate(not_a, not_b);
	}

	صحيح nor_gate(صحيح A, صحيح B)
	{
		صحيح not_a = not_gate(A);
		صحيح not_b = not_gate(B);
		أرجع and_gate(not_a, not_b);
	}

	صحيح xor_gate(صحيح A, صحيح B)
	{
		صحيح a_and_b = and_gate(A, B);
		صحيح a_nor_b = nor_gate(A, B);
		أرجع nor_gate(a_and_b, a_nor_b);
	}

	صحيح xnor_gate(صحيح A, صحيح B)
	{
		صحيح a_and_b = and_gate(A, B);
		صحيح a_nor_b = nor_gate(A, B);
		أرجع or_gate(a_and_b, a_nor_b);
	}

	صحيح andnot_gate(صحيح A, صحيح B)
	{
		صحيح not_b = not_gate(B);
		أرجع and_gate(A, not_b);
	}

	صحيح ornot_gate(صحيح A, صحيح B)
	{
		صحيح not_b = not_gate(B);
		أرجع or_gate(A, not_b);
	}

	صحيح mux_gate(صحيح A, صحيح B, صحيح S)
	{
		صحيح not_s = not_gate(S);
		صحيح a_سرگرم = and_gate(A, not_s);
		صحيح b_سرگرم = and_gate(B, S);
		أرجع or_gate(a_سرگرم, b_سرگرم);
	}

	سمتیہ<صحيح> adder(ثابت سمتیہ<صحيح> &A, ثابت سمتیہ<صحيح> &B, صحيح حاصل_وچ, سمتیہ<صحيح> *X = خالی_سوچک, سمتیہ<صحيح> *CO = خالی_سوچک)
	{
		سمتیہ<صحيح> Y(Get_ناپ(A));
		log_assert(Get_ناپ(A) == Get_ناپ(B));
		لكل (صحيح i = 0; i < Get_ناپ(A); i++) {
			صحيح a_xor_b = xor_gate(A[i], B[i]);
			صحيح a_or_b = or_gate(A[i], B[i]);
			صحيح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, حاصل_وچ);
			صحيح tmp = and_gate(a_or_b, حاصل_وچ);
			صحيح حاصل_باہرلا = or_gate(a_and_b, tmp);
			إذا (X != خالی_سوچک)
				X->at(i) = a_xor_b;
			إذا (CO != خالی_سوچک)
				CO->at(i) = حاصل_باہرلا;
			حاصل_وچ = حاصل_باہرلا;
		}
		أرجع Y;
	}

	فراغ outport(صحيح گرہ, IdString portname, صحيح portbit = 0)
	{
		إذا (portbit < Get_ناپ(cell->get_پورٹ(portname)))
			aig->nodes.at(گرہ).outports.ڈالو_پٹھ(pair<IdString, صحيح>(portname, portbit));
	}

	فراغ outport_bool(صحيح گرہ, IdString portname)
	{
		outport(گرہ, portname);
		لكل (صحيح i = 1; i < Get_ناپ(cell->get_پورٹ(portname)); i++)
			outport(bool_گرہ(خطأ), portname, i);
	}

	فراغ outport_vec(ثابت سمتیہ<صحيح> &vec, IdString portname)
	{
		لكل (صحيح i = 0; i < Get_ناپ(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	إذا (cell->type.is_عوامی())
		أرجع;

	AigMaker mk(هذا, cell);
	name = cell->type.str();

	string mkname_آخری;
	منطقي mkname_a_signed = خطأ;
	منطقي mkname_b_signed = خطأ;
	منطقي mkname_is_signed = خطأ;

	cell->parameters.ترتیب_دیو();
	لكل (خودکار p : cell->parameters)
	{
		إذا (p.پہلا == ID::A_چوڑائی && mkname_a_signed) {
			name = mkname_آخری + stringf(":%d%c", p.دوجا.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا إذا (p.پہلا == ID::B_چوڑائی && mkname_b_signed) {
			name = mkname_آخری + stringf(":%d%c", p.دوجا.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا {
			mkname_آخری = name;
			name += stringf(":%d", p.دوجا.as_int());
		}

		mkname_a_signed = خطأ;
		mkname_b_signed = خطأ;
		mkname_is_signed = خطأ;
		إذا (p.پہلا == ID::A_SIGNED) {
			mkname_a_signed = سچ;
			mkname_is_signed = p.دوجا.as_bool();
		}
		إذا (p.پہلا == ID::B_SIGNED) {
			mkname_b_signed = سچ;
			mkname_is_signed = p.دوجا.as_bool();
		}
	}

	إذا (cell->type.وچ(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		لكل (صحيح i = 0; i < Get_ناپ(cell->get_پورٹ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح Y = cell->type.وچ(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.وچ(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		لكل (صحيح i = 0; i < Get_ناپ(cell->get_پورٹ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = cell->type.وچ(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.وچ(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.وچ(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.وچ(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.وچ(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.وچ(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.وچ(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.وچ(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.وچ(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحيح S = mk.inport(ID::S);
		لكل (صحيح i = 0; i < Get_ناپ(cell->get_پورٹ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = mk.mux_gate(A, B, S);
			إذا (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.وچ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحيح Y = mk.inport(ID::A, 0);
		لكل (صحيح i = 1; i < Get_ناپ(cell->get_پورٹ(ID::A)); i++) {
			صحيح A = mk.inport(ID::A, i);
			إذا (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			إذا (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			إذا (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			إذا (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			إذا (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		إذا (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		لكل (صحيح i = 1; i < Get_ناپ(cell->get_پورٹ(ID::Y)); i++)
			mk.outport(mk.bool_گرہ(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type.وچ(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحيح A = mk.inport(ID::A, 0), Y = -1;
		لكل (صحيح i = 1; i < Get_ناپ(cell->get_پورٹ(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		إذا (cell->type.وچ(ID($logic_and), ID($logic_or))) {
			صحيح B = mk.inport(ID::B, 0);
			لكل (صحيح i = 1; i < Get_ناپ(cell->get_پورٹ(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			إذا (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			إذا (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} وإلا {
			إذا (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.وچ(ID($جوڑو), ID($sub)))
	{
		صحيح چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));
		سمتیہ<صحيح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحيح> B = mk.inport_vec(ID::B, چوڑائی);
		صحيح حاصل = mk.bool_گرہ(خطأ);
		إذا (cell->type == ID($sub)) {
			لكل (خودکار &n : B)
				n = mk.not_gate(n);
			حاصل = mk.not_gate(حاصل);
		}
		سمتیہ<صحيح> Y = mk.adder(A, B, حاصل);
		mk.outport_vec(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.وچ(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحيح چوڑائی = std::max(Get_ناپ(cell->get_پورٹ(ID::A)),
							 Get_ناپ(cell->get_پورٹ(ID::B))) + 1;
		سمتیہ<صحيح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحيح> B = mk.inport_vec(ID::B, چوڑائی);

		إذا (cell->type.وچ(ID($gt), ID($ge)))
			std::وٹاندرا(A, B);

		صحيح حاصل = mk.bool_گرہ(!cell->type.وچ(ID($le), ID($ge)));
		لكل (خودکار &n : B)
			n = mk.not_gate(n);
		سمتیہ<صحيح> Y = mk.adder(A, B, حاصل);
		mk.outport(Y.پٹھ(), ID::Y);
		لكل (صحيح i = 1; i < Get_ناپ(cell->get_پورٹ(ID::Y)); i++)
			mk.outport(mk.bool_گرہ(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type == ID($alu))
	{
		صحيح چوڑائی = Get_ناپ(cell->get_پورٹ(ID::Y));
		سمتیہ<صحيح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحيح> B = mk.inport_vec(ID::B, چوڑائی);
		صحيح حاصل = mk.inport(ID::CI);
		صحيح binv = mk.inport(ID::BI);
		لكل (خودکار &n : B)
			n = mk.xor_gate(n, binv);
		سمتیہ<صحيح> X(چوڑائی), CO(چوڑائی);
		سمتیہ<صحيح> Y = mk.adder(A, B, حاصل, &X, &CO);
		لكل (صحيح i = 0; i < چوڑائی; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		اذهب optimize;
	}

	إذا (cell->type.وچ(ID($eq), ID($ne)))
	{
		صحيح چوڑائی = max(Get_ناپ(cell->get_پورٹ(ID::A)), Get_ناپ(cell->get_پورٹ(ID::B)));
		سمتیہ<صحيح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحيح> B = mk.inport_vec(ID::B, چوڑائی);
		صحيح Y = mk.bool_گرہ(خطأ);
		لكل (صحيح i = 0; i < چوڑائی; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		إذا (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type == ID($_AOI3_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type == ID($_OAI3_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type == ID($_AOI4_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح D = mk.inport(ID::D);
		صحيح a_and_b = mk.and_gate(A, B);
		صحيح Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type == ID($_OAI4_))
	{
		صحيح A = mk.inport(ID::A);
		صحيح B = mk.inport(ID::B);
		صحيح C = mk.inport(ID::C);
		صحيح D = mk.inport(ID::D);
		صحيح a_or_b = mk.or_gate(A, B);
		صحيح Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		اذهب optimize;
	}

	name.clear();
	أرجع;

optimize:;
	pool<صحيح> ورتیا_old_ids;
	سمتیہ<Aig_گرہ> new_nodes;
	dict<صحيح, صحيح> old_منزل_new_ids;
	old_منزل_new_ids[-1] = -1;

	لكل (صحيح i = Get_ناپ(nodes)-1; i >= 0; i--) {
		إذا (!nodes[i].outports.خالی_تھاواں())
			ورتیا_old_ids.داخل_کرو(i);
		إذا (!ورتیا_old_ids.گنتی(i))
			استمر;
		إذا (nodes[i].بایاں_ماپا >= 0)
			ورتیا_old_ids.داخل_کرو(nodes[i].بایاں_ماپا);
		إذا (nodes[i].دایاں_ماپا >= 0)
			ورتیا_old_ids.داخل_کرو(nodes[i].دایاں_ماپا);
	}

	لكل (صحيح i = 0; i < Get_ناپ(nodes); i++) {
		إذا (!ورتیا_old_ids.گنتی(i))
			استمر;
		nodes[i].بایاں_ماپا = old_منزل_new_ids.at(nodes[i].بایاں_ماپا);
		nodes[i].دایاں_ماپا = old_منزل_new_ids.at(nodes[i].دایاں_ماپا);
		old_منزل_new_ids[i] = Get_ناپ(new_nodes);
		new_nodes.ڈالو_پٹھ(nodes[i]);
	}

	new_nodes.وٹاندرا(nodes);
}

YOSYS_NAMESPACE_سرا
