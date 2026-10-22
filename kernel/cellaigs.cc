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

Aig_ڳنڍ::Aig_ڳنڍ()
{
	portbit = -1;
	inverter = خطأ;
	کاٻو_والد = -1;
	ساڄو_والد = -1;
}

منطقي Aig_ڳنڍ::عامل==(ثابت Aig_ڳنڍ &other) ثابت
{
	إذا (portname != other.portname) أرجع خطأ;
	إذا (portbit != other.portbit) أرجع خطأ;
	إذا (inverter != other.inverter) أرجع خطأ;
	إذا (کاٻو_والد != other.کاٻو_والد) أرجع خطأ;
	إذا (ساڄو_والد != other.ساڄو_والد) أرجع خطأ;
	أرجع سچ;
}

Hasher Aig_ڳنڍ::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(کاٻو_والد);
	h.eat(ساڄو_والد);
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
	idict<Aig_ڳنڍ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحيح node2index(ثابت Aig_ڳنڍ &ڳنڍ)
	{
		إذا (ڳنڍ.کاٻو_والد > ڳنڍ.ساڄو_والد) {
			Aig_ڳنڍ n(ڳنڍ);
			std::مٽا_سٽا(n.کاٻو_والد, n.ساڄو_والد);
			أرجع node2index(n);
		}

		إذا (!aig_indices.ڳڻپ(ڳنڍ)) {
			aig_indices.expect(ڳنڍ, Get_سائيز(aig->nodes));
			aig->nodes.ڌڪو_پٺ(ڳنڍ);
		}

		أرجع aig_indices.at(ڳنڍ);
	}

	صحيح bool_ڳنڍ(منطقي قدر)
	{
		Aig_ڳنڍ ڳنڍ;
		ڳنڍ.inverter = قدر;
		أرجع node2index(ڳنڍ);
	}

	صحيح inport(IdString portname, صحيح portbit = 0, منطقي inverter = خطأ)
	{
		إذا (portbit >= Get_سائيز(cell->get_پورٽ(portname))) {
			IdString signed_param = cell->twines().ڳوليو(
				cell->twines().str(portname) + "_SIGNED");
		إذا (signed_param != IdString::Null && cell->parameters.ڳڻپ(signed_param) && cell->getParam(signed_param).as_bool())
				أرجع inport(portname, Get_سائيز(cell->get_پورٽ(portname))-1, inverter);
			أرجع bool_ڳنڍ(inverter);
		}

		Aig_ڳنڍ ڳنڍ;
		ڳنڍ.portname = portname;
		ڳنڍ.portbit = portbit;
		ڳنڍ.inverter = inverter;
		أرجع node2index(ڳنڍ);
	}

	ويڪٽر<صحيح> inport_vec(IdString portname, صحيح ويڪر)
	{
		ويڪٽر<صحيح> vec;
		لكل (صحيح i = 0; i < ويڪر; i++)
			vec.ڌڪو_پٺ(inport(portname, i));
		أرجع vec;
	}

	صحيح not_inport(IdString portname, صحيح portbit = 0)
	{
		أرجع inport(portname, portbit, سچ);
	}

	صحيح not_gate(صحيح A)
	{
		Aig_ڳنڍ ڳنڍ(aig_indices[A]);
		ڳنڍ.outports.clear();
		ڳنڍ.inverter = !ڳنڍ.inverter;
		أرجع node2index(ڳنڍ);
	}

	صحيح and_gate(صحيح A, صحيح B, منطقي inverter = خطأ)
	{
		إذا (A == B)
			أرجع inverter ? not_gate(A) : A;

		ثابت Aig_ڳنڍ &nA = aig_indices[A];
		ثابت Aig_ڳنڍ &nB = aig_indices[B];

		Aig_ڳنڍ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		إذا (nA == nB_inv)
			أرجع bool_ڳنڍ(inverter);

		منطقي nA_bool = nA.portbit < 0 && nA.کاٻو_والد < 0 && nA.ساڄو_والد < 0;
		منطقي nB_bool = nB.portbit < 0 && nB.کاٻو_والد < 0 && nB.ساڄو_والد < 0;

		إذا (nA_bool && nB_bool) {
			منطقي bA = nA.inverter;
			منطقي bB = nB.inverter;
			أرجع bool_ڳنڍ(inverter != (bA && bB));
		}

		إذا (nA_bool) {
			منطقي bA = nA.inverter;
			إذا (inverter)
				أرجع bA ? not_gate(B) : bool_ڳنڍ(سچ);
			أرجع bA ? B : bool_ڳنڍ(خطأ);
		}

		إذا (nB_bool) {
			منطقي bB = nB.inverter;
			إذا (inverter)
				أرجع bB ? not_gate(A) : bool_ڳنڍ(سچ);
			أرجع bB ? A : bool_ڳنڍ(خطأ);
		}

		Aig_ڳنڍ ڳنڍ;
		ڳنڍ.inverter = inverter;
		ڳنڍ.کاٻو_والد = A;
		ڳنڍ.ساڄو_والد = B;
		أرجع node2index(ڳنڍ);
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

	ويڪٽر<صحيح> adder(ثابت ويڪٽر<صحيح> &A, ثابت ويڪٽر<صحيح> &B, صحيح حاصل_اندريون, ويڪٽر<صحيح> *X = خالي_اشارو, ويڪٽر<صحيح> *CO = خالي_اشارو)
	{
		ويڪٽر<صحيح> Y(Get_سائيز(A));
		log_assert(Get_سائيز(A) == Get_سائيز(B));
		لكل (صحيح i = 0; i < Get_سائيز(A); i++) {
			صحيح a_xor_b = xor_gate(A[i], B[i]);
			صحيح a_or_b = or_gate(A[i], B[i]);
			صحيح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, حاصل_اندريون);
			صحيح tmp = and_gate(a_or_b, حاصل_اندريون);
			صحيح حاصل_ٻاهريون = or_gate(a_and_b, tmp);
			إذا (X != خالي_اشارو)
				X->at(i) = a_xor_b;
			إذا (CO != خالي_اشارو)
				CO->at(i) = حاصل_ٻاهريون;
			حاصل_اندريون = حاصل_ٻاهريون;
		}
		أرجع Y;
	}

	فراغ outport(صحيح ڳنڍ, IdString portname, صحيح portbit = 0)
	{
		إذا (portbit < Get_سائيز(cell->get_پورٽ(portname)))
			aig->nodes.at(ڳنڍ).outports.ڌڪو_پٺ(pair<IdString, صحيح>(portname, portbit));
	}

	فراغ outport_bool(صحيح ڳنڍ, IdString portname)
	{
		outport(ڳنڍ, portname);
		لكل (صحيح i = 1; i < Get_سائيز(cell->get_پورٽ(portname)); i++)
			outport(bool_ڳنڍ(خطأ), portname, i);
	}

	فراغ outport_vec(ثابت ويڪٽر<صحيح> &vec, IdString portname)
	{
		لكل (صحيح i = 0; i < Get_سائيز(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	إذا (cell->type.is_عوامي())
		أرجع;

	AigMaker mk(هذا, cell);
	name = cell->type.str();

	string mkname_آخري;
	منطقي mkname_a_signed = خطأ;
	منطقي mkname_b_signed = خطأ;
	منطقي mkname_is_signed = خطأ;

	cell->parameters.ترتيب_ڏيو();
	لكل (خودڪار p : cell->parameters)
	{
		إذا (p.پهريون == ID::A_ويڪر && mkname_a_signed) {
			name = mkname_آخري + stringf(":%d%c", p.ٻيو.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا إذا (p.پهريون == ID::B_ويڪر && mkname_b_signed) {
			name = mkname_آخري + stringf(":%d%c", p.ٻيو.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا {
			mkname_آخري = name;
			name += stringf(":%d", p.ٻيو.as_int());
		}

		mkname_a_signed = خطأ;
		mkname_b_signed = خطأ;
		mkname_is_signed = خطأ;
		إذا (p.پهريون == ID::A_SIGNED) {
			mkname_a_signed = سچ;
			mkname_is_signed = p.ٻيو.as_bool();
		}
		إذا (p.پهريون == ID::B_SIGNED) {
			mkname_b_signed = سچ;
			mkname_is_signed = p.ٻيو.as_bool();
		}
	}

	إذا (cell->type.اندريون(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		لكل (صحيح i = 0; i < Get_سائيز(cell->get_پورٽ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح Y = cell->type.اندريون(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.اندريون(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		لكل (صحيح i = 0; i < Get_سائيز(cell->get_پورٽ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = cell->type.اندريون(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.اندريون(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.اندريون(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.اندريون(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.اندريون(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.اندريون(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.اندريون(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.اندريون(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.اندريون(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحيح S = mk.inport(ID::S);
		لكل (صحيح i = 0; i < Get_سائيز(cell->get_پورٽ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = mk.mux_gate(A, B, S);
			إذا (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.اندريون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحيح Y = mk.inport(ID::A, 0);
		لكل (صحيح i = 1; i < Get_سائيز(cell->get_پورٽ(ID::A)); i++) {
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
		لكل (صحيح i = 1; i < Get_سائيز(cell->get_پورٽ(ID::Y)); i++)
			mk.outport(mk.bool_ڳنڍ(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type.اندريون(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحيح A = mk.inport(ID::A, 0), Y = -1;
		لكل (صحيح i = 1; i < Get_سائيز(cell->get_پورٽ(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		إذا (cell->type.اندريون(ID($logic_and), ID($logic_or))) {
			صحيح B = mk.inport(ID::B, 0);
			لكل (صحيح i = 1; i < Get_سائيز(cell->get_پورٽ(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			إذا (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			إذا (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} وإلا {
			إذا (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.اندريون(ID($جوڙيو), ID($sub)))
	{
		صحيح ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));
		ويڪٽر<صحيح> A = mk.inport_vec(ID::A, ويڪر);
		ويڪٽر<صحيح> B = mk.inport_vec(ID::B, ويڪر);
		صحيح حاصل = mk.bool_ڳنڍ(خطأ);
		إذا (cell->type == ID($sub)) {
			لكل (خودڪار &n : B)
				n = mk.not_gate(n);
			حاصل = mk.not_gate(حاصل);
		}
		ويڪٽر<صحيح> Y = mk.adder(A, B, حاصل);
		mk.outport_vec(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.اندريون(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحيح ويڪر = std::max(Get_سائيز(cell->get_پورٽ(ID::A)),
							 Get_سائيز(cell->get_پورٽ(ID::B))) + 1;
		ويڪٽر<صحيح> A = mk.inport_vec(ID::A, ويڪر);
		ويڪٽر<صحيح> B = mk.inport_vec(ID::B, ويڪر);

		إذا (cell->type.اندريون(ID($gt), ID($ge)))
			std::مٽا_سٽا(A, B);

		صحيح حاصل = mk.bool_ڳنڍ(!cell->type.اندريون(ID($le), ID($ge)));
		لكل (خودڪار &n : B)
			n = mk.not_gate(n);
		ويڪٽر<صحيح> Y = mk.adder(A, B, حاصل);
		mk.outport(Y.پٺ(), ID::Y);
		لكل (صحيح i = 1; i < Get_سائيز(cell->get_پورٽ(ID::Y)); i++)
			mk.outport(mk.bool_ڳنڍ(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type == ID($alu))
	{
		صحيح ويڪر = Get_سائيز(cell->get_پورٽ(ID::Y));
		ويڪٽر<صحيح> A = mk.inport_vec(ID::A, ويڪر);
		ويڪٽر<صحيح> B = mk.inport_vec(ID::B, ويڪر);
		صحيح حاصل = mk.inport(ID::CI);
		صحيح binv = mk.inport(ID::BI);
		لكل (خودڪار &n : B)
			n = mk.xor_gate(n, binv);
		ويڪٽر<صحيح> X(ويڪر), CO(ويڪر);
		ويڪٽر<صحيح> Y = mk.adder(A, B, حاصل, &X, &CO);
		لكل (صحيح i = 0; i < ويڪر; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		اذهب optimize;
	}

	إذا (cell->type.اندريون(ID($eq), ID($ne)))
	{
		صحيح ويڪر = max(Get_سائيز(cell->get_پورٽ(ID::A)), Get_سائيز(cell->get_پورٽ(ID::B)));
		ويڪٽر<صحيح> A = mk.inport_vec(ID::A, ويڪر);
		ويڪٽر<صحيح> B = mk.inport_vec(ID::B, ويڪر);
		صحيح Y = mk.bool_ڳنڍ(خطأ);
		لكل (صحيح i = 0; i < ويڪر; i++)
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
	pool<صحيح> استعمال_ٿيل_old_ids;
	ويڪٽر<Aig_ڳنڍ> new_nodes;
	dict<صحيح, صحيح> old_منزل_new_ids;
	old_منزل_new_ids[-1] = -1;

	لكل (صحيح i = Get_سائيز(nodes)-1; i >= 0; i--) {
		إذا (!nodes[i].outports.خالي_جايون())
			استعمال_ٿيل_old_ids.وجهو(i);
		إذا (!استعمال_ٿيل_old_ids.ڳڻپ(i))
			استمر;
		إذا (nodes[i].کاٻو_والد >= 0)
			استعمال_ٿيل_old_ids.وجهو(nodes[i].کاٻو_والد);
		إذا (nodes[i].ساڄو_والد >= 0)
			استعمال_ٿيل_old_ids.وجهو(nodes[i].ساڄو_والد);
	}

	لكل (صحيح i = 0; i < Get_سائيز(nodes); i++) {
		إذا (!استعمال_ٿيل_old_ids.ڳڻپ(i))
			استمر;
		nodes[i].کاٻو_والد = old_منزل_new_ids.at(nodes[i].کاٻو_والد);
		nodes[i].ساڄو_والد = old_منزل_new_ids.at(nodes[i].ساڄو_والد);
		old_منزل_new_ids[i] = Get_سائيز(new_nodes);
		new_nodes.ڌڪو_پٺ(nodes[i]);
	}

	new_nodes.مٽا_سٽا(nodes);
}

YOSYS_NAMESPACE_پڇاڙي
