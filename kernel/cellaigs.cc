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

Aig_غوټه::Aig_غوټه()
{
	portbit = -1;
	inverter = خطأ;
	کیڼ_پلار = -1;
	ښی_پلار = -1;
}

منطقي Aig_غوټه::چلوونکی==(ثابت Aig_غوټه &other) ثابت
{
	إذا (portname != other.portname) أرجع خطأ;
	إذا (portbit != other.portbit) أرجع خطأ;
	إذا (inverter != other.inverter) أرجع خطأ;
	إذا (کیڼ_پلار != other.کیڼ_پلار) أرجع خطأ;
	إذا (ښی_پلار != other.ښی_پلار) أرجع خطأ;
	أرجع ریښتیا;
}

Hasher Aig_غوټه::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(کیڼ_پلار);
	h.eat(ښی_پلار);
	أرجع h;
}

منطقي Aig::چلوونکی==(ثابت Aig &other) ثابت
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
	idict<Aig_غوټه> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحيح node2index(ثابت Aig_غوټه &غوټه)
	{
		إذا (غوټه.کیڼ_پلار > غوټه.ښی_پلار) {
			Aig_غوټه n(غوټه);
			std::بدلون(n.کیڼ_پلار, n.ښی_پلار);
			أرجع node2index(n);
		}

		إذا (!aig_indices.شمېر(غوټه)) {
			aig_indices.expect(غوټه, Get_اندازه(aig->nodes));
			aig->nodes.ټېل_شا(غوټه);
		}

		أرجع aig_indices.at(غوټه);
	}

	صحيح bool_غوټه(منطقي ارزښت)
	{
		Aig_غوټه غوټه;
		غوټه.inverter = ارزښت;
		أرجع node2index(غوټه);
	}

	صحيح inport(IdString portname, صحيح portbit = 0, منطقي inverter = خطأ)
	{
		إذا (portbit >= Get_اندازه(cell->get_پورټ(portname))) {
			IdString signed_param = cell->twines().پیدا_کړه(
				cell->twines().str(portname) + "_SIGNED");
		إذا (signed_param != IdString::Null && cell->parameters.شمېر(signed_param) && cell->getParam(signed_param).as_bool())
				أرجع inport(portname, Get_اندازه(cell->get_پورټ(portname))-1, inverter);
			أرجع bool_غوټه(inverter);
		}

		Aig_غوټه غوټه;
		غوټه.portname = portname;
		غوټه.portbit = portbit;
		غوټه.inverter = inverter;
		أرجع node2index(غوټه);
	}

	ویکټور<صحيح> inport_vec(IdString portname, صحيح پلنوالی)
	{
		ویکټور<صحيح> vec;
		لكل (صحيح i = 0; i < پلنوالی; i++)
			vec.ټېل_شا(inport(portname, i));
		أرجع vec;
	}

	صحيح not_inport(IdString portname, صحيح portbit = 0)
	{
		أرجع inport(portname, portbit, ریښتیا);
	}

	صحيح not_gate(صحيح A)
	{
		Aig_غوټه غوټه(aig_indices[A]);
		غوټه.outports.clear();
		غوټه.inverter = !غوټه.inverter;
		أرجع node2index(غوټه);
	}

	صحيح and_gate(صحيح A, صحيح B, منطقي inverter = خطأ)
	{
		إذا (A == B)
			أرجع inverter ? not_gate(A) : A;

		ثابت Aig_غوټه &nA = aig_indices[A];
		ثابت Aig_غوټه &nB = aig_indices[B];

		Aig_غوټه nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		إذا (nA == nB_inv)
			أرجع bool_غوټه(inverter);

		منطقي nA_bool = nA.portbit < 0 && nA.کیڼ_پلار < 0 && nA.ښی_پلار < 0;
		منطقي nB_bool = nB.portbit < 0 && nB.کیڼ_پلار < 0 && nB.ښی_پلار < 0;

		إذا (nA_bool && nB_bool) {
			منطقي bA = nA.inverter;
			منطقي bB = nB.inverter;
			أرجع bool_غوټه(inverter != (bA && bB));
		}

		إذا (nA_bool) {
			منطقي bA = nA.inverter;
			إذا (inverter)
				أرجع bA ? not_gate(B) : bool_غوټه(ریښتیا);
			أرجع bA ? B : bool_غوټه(خطأ);
		}

		إذا (nB_bool) {
			منطقي bB = nB.inverter;
			إذا (inverter)
				أرجع bB ? not_gate(A) : bool_غوټه(ریښتیا);
			أرجع bB ? A : bool_غوټه(خطأ);
		}

		Aig_غوټه غوټه;
		غوټه.inverter = inverter;
		غوټه.کیڼ_پلار = A;
		غوټه.ښی_پلار = B;
		أرجع node2index(غوټه);
	}

	صحيح nand_gate(صحيح A, صحيح B)
	{
		أرجع and_gate(A, B, ریښتیا);
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

	ویکټور<صحيح> adder(ثابت ویکټور<صحيح> &A, ثابت ویکټور<صحيح> &B, صحيح لېږد_دننه, ویکټور<صحيح> *X = تش_اشاره, ویکټور<صحيح> *CO = تش_اشاره)
	{
		ویکټور<صحيح> Y(Get_اندازه(A));
		log_assert(Get_اندازه(A) == Get_اندازه(B));
		لكل (صحيح i = 0; i < Get_اندازه(A); i++) {
			صحيح a_xor_b = xor_gate(A[i], B[i]);
			صحيح a_or_b = or_gate(A[i], B[i]);
			صحيح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, لېږد_دننه);
			صحيح tmp = and_gate(a_or_b, لېږد_دننه);
			صحيح لېږد_بهر = or_gate(a_and_b, tmp);
			إذا (X != تش_اشاره)
				X->at(i) = a_xor_b;
			إذا (CO != تش_اشاره)
				CO->at(i) = لېږد_بهر;
			لېږد_دننه = لېږد_بهر;
		}
		أرجع Y;
	}

	فراغ outport(صحيح غوټه, IdString portname, صحيح portbit = 0)
	{
		إذا (portbit < Get_اندازه(cell->get_پورټ(portname)))
			aig->nodes.at(غوټه).outports.ټېل_شا(pair<IdString, صحيح>(portname, portbit));
	}

	فراغ outport_bool(صحيح غوټه, IdString portname)
	{
		outport(غوټه, portname);
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورټ(portname)); i++)
			outport(bool_غوټه(خطأ), portname, i);
	}

	فراغ outport_vec(ثابت ویکټور<صحيح> &vec, IdString portname)
	{
		لكل (صحيح i = 0; i < Get_اندازه(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	إذا (cell->type.is_عامه())
		أرجع;

	AigMaker mk(هذا, cell);
	name = cell->type.str();

	string mkname_وروستی;
	منطقي mkname_a_signed = خطأ;
	منطقي mkname_b_signed = خطأ;
	منطقي mkname_is_signed = خطأ;

	cell->parameters.ترتیبول();
	لكل (خپلکاری p : cell->parameters)
	{
		إذا (p.لومړی == ID::A_پلنوالی && mkname_a_signed) {
			name = mkname_وروستی + stringf(":%d%c", p.دوهم.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا إذا (p.لومړی == ID::B_پلنوالی && mkname_b_signed) {
			name = mkname_وروستی + stringf(":%d%c", p.دوهم.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا {
			mkname_وروستی = name;
			name += stringf(":%d", p.دوهم.as_int());
		}

		mkname_a_signed = خطأ;
		mkname_b_signed = خطأ;
		mkname_is_signed = خطأ;
		إذا (p.لومړی == ID::A_SIGNED) {
			mkname_a_signed = ریښتیا;
			mkname_is_signed = p.دوهم.as_bool();
		}
		إذا (p.لومړی == ID::B_SIGNED) {
			mkname_b_signed = ریښتیا;
			mkname_is_signed = p.دوهم.as_bool();
		}
	}

	إذا (cell->type.دننه(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		لكل (صحيح i = 0; i < Get_اندازه(cell->get_پورټ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح Y = cell->type.دننه(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.دننه(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		لكل (صحيح i = 0; i < Get_اندازه(cell->get_پورټ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = cell->type.دننه(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.دننه(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.دننه(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.دننه(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.دننه(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.دننه(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.دننه(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.دننه(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.دننه(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحيح S = mk.inport(ID::S);
		لكل (صحيح i = 0; i < Get_اندازه(cell->get_پورټ(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = mk.mux_gate(A, B, S);
			إذا (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.دننه(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحيح Y = mk.inport(ID::A, 0);
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورټ(ID::A)); i++) {
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
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورټ(ID::Y)); i++)
			mk.outport(mk.bool_غوټه(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type.دننه(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحيح A = mk.inport(ID::A, 0), Y = -1;
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورټ(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		إذا (cell->type.دننه(ID($logic_and), ID($logic_or))) {
			صحيح B = mk.inport(ID::B, 0);
			لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورټ(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			إذا (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			إذا (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} وإلا {
			إذا (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.دننه(ID($جمع_کړه), ID($sub)))
	{
		صحيح پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));
		ویکټور<صحيح> A = mk.inport_vec(ID::A, پلنوالی);
		ویکټور<صحيح> B = mk.inport_vec(ID::B, پلنوالی);
		صحيح لېږد = mk.bool_غوټه(خطأ);
		إذا (cell->type == ID($sub)) {
			لكل (خپلکاری &n : B)
				n = mk.not_gate(n);
			لېږد = mk.not_gate(لېږد);
		}
		ویکټور<صحيح> Y = mk.adder(A, B, لېږد);
		mk.outport_vec(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.دننه(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحيح پلنوالی = std::max(Get_اندازه(cell->get_پورټ(ID::A)),
							 Get_اندازه(cell->get_پورټ(ID::B))) + 1;
		ویکټور<صحيح> A = mk.inport_vec(ID::A, پلنوالی);
		ویکټور<صحيح> B = mk.inport_vec(ID::B, پلنوالی);

		إذا (cell->type.دننه(ID($gt), ID($ge)))
			std::بدلون(A, B);

		صحيح لېږد = mk.bool_غوټه(!cell->type.دننه(ID($le), ID($ge)));
		لكل (خپلکاری &n : B)
			n = mk.not_gate(n);
		ویکټور<صحيح> Y = mk.adder(A, B, لېږد);
		mk.outport(Y.شا(), ID::Y);
		لكل (صحيح i = 1; i < Get_اندازه(cell->get_پورټ(ID::Y)); i++)
			mk.outport(mk.bool_غوټه(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type == ID($alu))
	{
		صحيح پلنوالی = Get_اندازه(cell->get_پورټ(ID::Y));
		ویکټور<صحيح> A = mk.inport_vec(ID::A, پلنوالی);
		ویکټور<صحيح> B = mk.inport_vec(ID::B, پلنوالی);
		صحيح لېږد = mk.inport(ID::CI);
		صحيح binv = mk.inport(ID::BI);
		لكل (خپلکاری &n : B)
			n = mk.xor_gate(n, binv);
		ویکټور<صحيح> X(پلنوالی), CO(پلنوالی);
		ویکټور<صحيح> Y = mk.adder(A, B, لېږد, &X, &CO);
		لكل (صحيح i = 0; i < پلنوالی; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		اذهب optimize;
	}

	إذا (cell->type.دننه(ID($eq), ID($ne)))
	{
		صحيح پلنوالی = max(Get_اندازه(cell->get_پورټ(ID::A)), Get_اندازه(cell->get_پورټ(ID::B)));
		ویکټور<صحيح> A = mk.inport_vec(ID::A, پلنوالی);
		ویکټور<صحيح> B = mk.inport_vec(ID::B, پلنوالی);
		صحيح Y = mk.bool_غوټه(خطأ);
		لكل (صحيح i = 0; i < پلنوالی; i++)
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
	pool<صحيح> کارول_شوی_old_ids;
	ویکټور<Aig_غوټه> new_nodes;
	dict<صحيح, صحيح> old_منزل_new_ids;
	old_منزل_new_ids[-1] = -1;

	لكل (صحيح i = Get_اندازه(nodes)-1; i >= 0; i--) {
		إذا (!nodes[i].outports.تشه())
			کارول_شوی_old_ids.ورننباسه(i);
		إذا (!کارول_شوی_old_ids.شمېر(i))
			استمر;
		إذا (nodes[i].کیڼ_پلار >= 0)
			کارول_شوی_old_ids.ورننباسه(nodes[i].کیڼ_پلار);
		إذا (nodes[i].ښی_پلار >= 0)
			کارول_شوی_old_ids.ورننباسه(nodes[i].ښی_پلار);
	}

	لكل (صحيح i = 0; i < Get_اندازه(nodes); i++) {
		إذا (!کارول_شوی_old_ids.شمېر(i))
			استمر;
		nodes[i].کیڼ_پلار = old_منزل_new_ids.at(nodes[i].کیڼ_پلار);
		nodes[i].ښی_پلار = old_منزل_new_ids.at(nodes[i].ښی_پلار);
		old_منزل_new_ids[i] = Get_اندازه(new_nodes);
		new_nodes.ټېل_شا(nodes[i]);
	}

	new_nodes.بدلون(nodes);
}

YOSYS_NAMESPACE_پای_ټکی
