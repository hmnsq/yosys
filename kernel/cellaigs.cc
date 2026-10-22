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

Aig_گره::Aig_گره()
{
	portbit = -1;
	inverter = خطأ;
	چپ_والد = -1;
	راست_والد = -1;
}

منطقي Aig_گره::عملگر==(ثابت Aig_گره &other) ثابت
{
	إذا (portname != other.portname) أرجع خطأ;
	إذا (portbit != other.portbit) أرجع خطأ;
	إذا (inverter != other.inverter) أرجع خطأ;
	إذا (چپ_والد != other.چپ_والد) أرجع خطأ;
	إذا (راست_والد != other.راست_والد) أرجع خطأ;
	أرجع درست;
}

Hasher Aig_گره::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(چپ_والد);
	h.eat(راست_والد);
	أرجع h;
}

منطقي Aig::عملگر==(ثابت Aig &other) ثابت
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
	idict<Aig_گره> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحيح node2index(ثابت Aig_گره &گره)
	{
		إذا (گره.چپ_والد > گره.راست_والد) {
			Aig_گره n(گره);
			std::جابجا(n.چپ_والد, n.راست_والد);
			أرجع node2index(n);
		}

		إذا (!aig_indices.شمار(گره)) {
			aig_indices.expect(گره, Get_اندازه(aig->nodes));
			aig->nodes.افزودن_پشت(گره);
		}

		أرجع aig_indices.at(گره);
	}

	صحيح bool_گره(منطقي مقدار)
	{
		Aig_گره گره;
		گره.inverter = مقدار;
		أرجع node2index(گره);
	}

	صحيح inport(IdString portname, صحيح portbit = 0, منطقي inverter = خطأ)
	{
		إذا (portbit >= Get_اندازه(cell->get_پورت(portname))) {
			IdString signed_param = cell->twines().پیدا_کن(
				cell->twines().str(portname) + "_SIGNED");
		إذا (signed_param != IdString::Null && cell->parameters.شمار(signed_param) && cell->getParam(signed_param).as_bool())
				أرجع inport(portname, Get_اندازه(cell->get_پورت(portname))-1, inverter);
			أرجع bool_گره(inverter);
		}

		Aig_گره گره;
		گره.portname = portname;
		گره.portbit = portbit;
		گره.inverter = inverter;
		أرجع node2index(گره);
	}

	وکتور<صحيح> inport_vec(IdString portname, صحيح عرض)
	{
		وکتور<صحيح> vec;
		لكل (صحيح i = 0; i < عرض; i++)
			vec.افزودن_پشت(inport(portname, i));
		أرجع vec;
	}

	صحيح not_inport(IdString portname, صحيح portbit = 0)
	{
		أرجع inport(portname, portbit, درست);
	}

	صحيح not_gate(صحيح A)
	{
		Aig_گره گره(aig_indices[A]);
		گره.outports.clear();
		گره.inverter = !گره.inverter;
		أرجع node2index(گره);
	}

	صحيح and_gate(صحيح A, صحيح B, منطقي inverter = خطأ)
	{
		إذا (A == B)
			أرجع inverter ? not_gate(A) : A;

		ثابت Aig_گره &nA = aig_indices[A];
		ثابت Aig_گره &nB = aig_indices[B];

		Aig_گره nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		إذا (nA == nB_inv)
			أرجع bool_گره(inverter);

		منطقي nA_bool = nA.portbit < 0 && nA.چپ_والد < 0 && nA.راست_والد < 0;
		منطقي nB_bool = nB.portbit < 0 && nB.چپ_والد < 0 && nB.راست_والد < 0;

		إذا (nA_bool && nB_bool) {
			منطقي bA = nA.inverter;
			منطقي bB = nB.inverter;
			أرجع bool_گره(inverter != (bA && bB));
		}

		إذا (nA_bool) {
			منطقي bA = nA.inverter;
			إذا (inverter)
				أرجع bA ? not_gate(B) : bool_گره(درست);
			أرجع bA ? B : bool_گره(خطأ);
		}

		إذا (nB_bool) {
			منطقي bB = nB.inverter;
			إذا (inverter)
				أرجع bB ? not_gate(A) : bool_گره(درست);
			أرجع bB ? A : bool_گره(خطأ);
		}

		Aig_گره گره;
		گره.inverter = inverter;
		گره.چپ_والد = A;
		گره.راست_والد = B;
		أرجع node2index(گره);
	}

	صحيح nand_gate(صحيح A, صحيح B)
	{
		أرجع and_gate(A, B, درست);
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
		صحيح a_فعال = and_gate(A, not_s);
		صحيح b_فعال = and_gate(B, S);
		أرجع or_gate(a_فعال, b_فعال);
	}

	وکتور<صحيح> adder(ثابت وکتور<صحيح> &A, ثابت وکتور<صحيح> &B, صحيح انتقال_درون, وکتور<صحيح> *X = اشاره_تهی, وکتور<صحيح> *CO = اشاره_تهی)
	{
		وکتور<صحيح> Y(Get_اندازه(A));
		log_assert(Get_اندازه(A) == Get_اندازه(B));
		لكل (صحيح i = 0; i < Get_اندازه(A); i++) {
			صحيح a_xor_b = xor_gate(A[i], B[i]);
			صحيح a_or_b = or_gate(A[i], B[i]);
			صحيح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, انتقال_درون);
			صحيح tmp = and_gate(a_or_b, انتقال_درون);
			صحيح انتقال_بیرون = or_gate(a_and_b, tmp);
			إذا (X != اشاره_تهی)
				X->at(i) = a_xor_b;
			إذا (CO != اشاره_تهی)
				CO->at(i) = انتقال_بیرون;
			انتقال_درون = انتقال_بیرون;
		}
		أرجع Y;
	}

	فراغ outport(صحيح گره, IdString portname, صحيح portbit = 0)
	{
		إذا (portbit < Get_اندازه(cell->get_پورت(portname)))
			aig->nodes.at(گره).outports.افزودن_پشت(pair<IdString, صحيح>(portname, portbit));
	}

	فراغ outport_bool(صحيح گره, IdString portname)
	{
		outport(گره, portname);
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورت(portname)); i++)
			outport(bool_گره(خطأ), portname, i);
	}

	فراغ outport_vec(ثابت وکتور<صحيح> &vec, IdString portname)
	{
		لكل (صحيح i = 0; i < Get_اندازه(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	إذا (cell->type.is_عمومی())
		أرجع;

	AigMaker mk(هذا, cell);
	name = cell->type.str();

	string mkname_آخرین;
	منطقي mkname_a_signed = خطأ;
	منطقي mkname_b_signed = خطأ;
	منطقي mkname_is_signed = خطأ;

	cell->parameters.مرتب_سازی();
	لكل (خودکار p : cell->parameters)
	{
		إذا (p.اولی == ID::A_عرض && mkname_a_signed) {
			name = mkname_آخرین + stringf(":%d%c", p.دومی.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا إذا (p.اولی == ID::B_عرض && mkname_b_signed) {
			name = mkname_آخرین + stringf(":%d%c", p.دومی.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا {
			mkname_آخرین = name;
			name += stringf(":%d", p.دومی.as_int());
		}

		mkname_a_signed = خطأ;
		mkname_b_signed = خطأ;
		mkname_is_signed = خطأ;
		إذا (p.اولی == ID::A_SIGNED) {
			mkname_a_signed = درست;
			mkname_is_signed = p.دومی.as_bool();
		}
		إذا (p.اولی == ID::B_SIGNED) {
			mkname_b_signed = درست;
			mkname_is_signed = p.دومی.as_bool();
		}
	}

	إذا (cell->type.درون(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		لكل (صحيح i = 0; i < Get_اندازه(cell->get_پورت(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح Y = cell->type.درون(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.درون(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		لكل (صحيح i = 0; i < Get_اندازه(cell->get_پورت(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = cell->type.درون(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.درون(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.درون(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.درون(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.درون(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.درون(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.درون(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.درون(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.درون(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحيح S = mk.inport(ID::S);
		لكل (صحيح i = 0; i < Get_اندازه(cell->get_پورت(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = mk.mux_gate(A, B, S);
			إذا (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.درون(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحيح Y = mk.inport(ID::A, 0);
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورت(ID::A)); i++) {
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
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورت(ID::Y)); i++)
			mk.outport(mk.bool_گره(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type.درون(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحيح A = mk.inport(ID::A, 0), Y = -1;
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورت(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		إذا (cell->type.درون(ID($logic_and), ID($logic_or))) {
			صحيح B = mk.inport(ID::B, 0);
			لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورت(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			إذا (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			إذا (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} وإلا {
			إذا (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.درون(ID($جمع_کن), ID($sub)))
	{
		صحيح عرض = Get_اندازه(cell->get_پورت(ID::Y));
		وکتور<صحيح> A = mk.inport_vec(ID::A, عرض);
		وکتور<صحيح> B = mk.inport_vec(ID::B, عرض);
		صحيح انتقال = mk.bool_گره(خطأ);
		إذا (cell->type == ID($sub)) {
			لكل (خودکار &n : B)
				n = mk.not_gate(n);
			انتقال = mk.not_gate(انتقال);
		}
		وکتور<صحيح> Y = mk.adder(A, B, انتقال);
		mk.outport_vec(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.درون(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحيح عرض = std::max(Get_اندازه(cell->get_پورت(ID::A)),
							 Get_اندازه(cell->get_پورت(ID::B))) + 1;
		وکتور<صحيح> A = mk.inport_vec(ID::A, عرض);
		وکتور<صحيح> B = mk.inport_vec(ID::B, عرض);

		إذا (cell->type.درون(ID($gt), ID($ge)))
			std::جابجا(A, B);

		صحيح انتقال = mk.bool_گره(!cell->type.درون(ID($le), ID($ge)));
		لكل (خودکار &n : B)
			n = mk.not_gate(n);
		وکتور<صحيح> Y = mk.adder(A, B, انتقال);
		mk.outport(Y.پشت(), ID::Y);
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورت(ID::Y)); i++)
			mk.outport(mk.bool_گره(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type == ID($alu))
	{
		صحيح عرض = Get_اندازه(cell->get_پورت(ID::Y));
		وکتور<صحيح> A = mk.inport_vec(ID::A, عرض);
		وکتور<صحيح> B = mk.inport_vec(ID::B, عرض);
		صحيح انتقال = mk.inport(ID::CI);
		صحيح binv = mk.inport(ID::BI);
		لكل (خودکار &n : B)
			n = mk.xor_gate(n, binv);
		وکتور<صحيح> X(عرض), CO(عرض);
		وکتور<صحيح> Y = mk.adder(A, B, انتقال, &X, &CO);
		لكل (صحيح i = 0; i < عرض; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		اذهب optimize;
	}

	إذا (cell->type.درون(ID($eq), ID($ne)))
	{
		صحيح عرض = max(Get_اندازه(cell->get_پورت(ID::A)), Get_اندازه(cell->get_پورت(ID::B)));
		وکتور<صحيح> A = mk.inport_vec(ID::A, عرض);
		وکتور<صحيح> B = mk.inport_vec(ID::B, عرض);
		صحيح Y = mk.bool_گره(خطأ);
		لكل (صحيح i = 0; i < عرض; i++)
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
	pool<صحيح> استفاده_شده_old_ids;
	وکتور<Aig_گره> new_nodes;
	dict<صحيح, صحيح> old_مقصد_new_ids;
	old_مقصد_new_ids[-1] = -1;

	لكل (صحيح i = Get_اندازه(nodes)-1; i >= 0; i--) {
		إذا (!nodes[i].outports.خالی())
			استفاده_شده_old_ids.درج(i);
		إذا (!استفاده_شده_old_ids.شمار(i))
			استمر;
		إذا (nodes[i].چپ_والد >= 0)
			استفاده_شده_old_ids.درج(nodes[i].چپ_والد);
		إذا (nodes[i].راست_والد >= 0)
			استفاده_شده_old_ids.درج(nodes[i].راست_والد);
	}

	لكل (صحيح i = 0; i < Get_اندازه(nodes); i++) {
		إذا (!استفاده_شده_old_ids.شمار(i))
			استمر;
		nodes[i].چپ_والد = old_مقصد_new_ids.at(nodes[i].چپ_والد);
		nodes[i].راست_والد = old_مقصد_new_ids.at(nodes[i].راست_والد);
		old_مقصد_new_ids[i] = Get_اندازه(new_nodes);
		new_nodes.افزودن_پشت(nodes[i]);
	}

	new_nodes.جابجا(nodes);
}

YOSYS_NAMESPACE_انتها
