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

#شامل "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_گرہ::Aig_گرہ()
{
	portbit = -1;
	inverter = جھوٹ;
	بایاں_والد = -1;
	دایاں_والد = -1;
}

منطقی Aig_گرہ::عامل==(ثابت Aig_گرہ &other) ثابت
{
	اگر (portname != other.portname) واپس جھوٹ;
	اگر (portbit != other.portbit) واپس جھوٹ;
	اگر (inverter != other.inverter) واپس جھوٹ;
	اگر (بایاں_والد != other.بایاں_والد) واپس جھوٹ;
	اگر (دایاں_والد != other.دایاں_والد) واپس جھوٹ;
	واپس سچ;
}

Hasher Aig_گرہ::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(بایاں_والد);
	h.eat(دایاں_والد);
	واپس h;
}

منطقی Aig::عامل==(ثابت Aig &other) ثابت
{
	واپس name == other.name;
}

Hasher Aig::hash_into(Hasher h) ثابت
{
	h.eat(name);
	واپس h;
}

ساخت AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_گرہ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحیح node2index(ثابت Aig_گرہ &گرہ)
	{
		اگر (گرہ.بایاں_والد > گرہ.دایاں_والد) {
			Aig_گرہ n(گرہ);
			std::تبادلہ(n.بایاں_والد, n.دایاں_والد);
			واپس node2index(n);
		}

		اگر (!aig_indices.گنتی(گرہ)) {
			aig_indices.expect(گرہ, Get_سائز(aig->nodes));
			aig->nodes.ڈالو_عقب(گرہ);
		}

		واپس aig_indices.at(گرہ);
	}

	صحیح bool_گرہ(منطقی قدر)
	{
		Aig_گرہ گرہ;
		گرہ.inverter = قدر;
		واپس node2index(گرہ);
	}

	صحیح inport(IdString portname, صحیح portbit = 0, منطقی inverter = جھوٹ)
	{
		اگر (portbit >= Get_سائز(cell->get_پورٹ(portname))) {
			IdString signed_param = cell->twines().ڈھونڈو(
				cell->twines().str(portname) + "_SIGNED");
		اگر (signed_param != IdString::Null && cell->parameters.گنتی(signed_param) && cell->getParam(signed_param).as_bool())
				واپس inport(portname, Get_سائز(cell->get_پورٹ(portname))-1, inverter);
			واپس bool_گرہ(inverter);
		}

		Aig_گرہ گرہ;
		گرہ.portname = portname;
		گرہ.portbit = portbit;
		گرہ.inverter = inverter;
		واپس node2index(گرہ);
	}

	سمتیہ<صحیح> inport_vec(IdString portname, صحیح چوڑائی)
	{
		سمتیہ<صحیح> vec;
		برائے (صحیح i = 0; i < چوڑائی; i++)
			vec.ڈالو_عقب(inport(portname, i));
		واپس vec;
	}

	صحیح not_inport(IdString portname, صحیح portbit = 0)
	{
		واپس inport(portname, portbit, سچ);
	}

	صحیح not_gate(صحیح A)
	{
		Aig_گرہ گرہ(aig_indices[A]);
		گرہ.outports.clear();
		گرہ.inverter = !گرہ.inverter;
		واپس node2index(گرہ);
	}

	صحیح and_gate(صحیح A, صحیح B, منطقی inverter = جھوٹ)
	{
		اگر (A == B)
			واپس inverter ? not_gate(A) : A;

		ثابت Aig_گرہ &nA = aig_indices[A];
		ثابت Aig_گرہ &nB = aig_indices[B];

		Aig_گرہ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		اگر (nA == nB_inv)
			واپس bool_گرہ(inverter);

		منطقی nA_bool = nA.portbit < 0 && nA.بایاں_والد < 0 && nA.دایاں_والد < 0;
		منطقی nB_bool = nB.portbit < 0 && nB.بایاں_والد < 0 && nB.دایاں_والد < 0;

		اگر (nA_bool && nB_bool) {
			منطقی bA = nA.inverter;
			منطقی bB = nB.inverter;
			واپس bool_گرہ(inverter != (bA && bB));
		}

		اگر (nA_bool) {
			منطقی bA = nA.inverter;
			اگر (inverter)
				واپس bA ? not_gate(B) : bool_گرہ(سچ);
			واپس bA ? B : bool_گرہ(جھوٹ);
		}

		اگر (nB_bool) {
			منطقی bB = nB.inverter;
			اگر (inverter)
				واپس bB ? not_gate(A) : bool_گرہ(سچ);
			واپس bB ? A : bool_گرہ(جھوٹ);
		}

		Aig_گرہ گرہ;
		گرہ.inverter = inverter;
		گرہ.بایاں_والد = A;
		گرہ.دایاں_والد = B;
		واپس node2index(گرہ);
	}

	صحیح nand_gate(صحیح A, صحیح B)
	{
		واپس and_gate(A, B, سچ);
	}

	صحیح or_gate(صحیح A, صحیح B)
	{
		صحیح not_a = not_gate(A);
		صحیح not_b = not_gate(B);
		واپس nand_gate(not_a, not_b);
	}

	صحیح nor_gate(صحیح A, صحیح B)
	{
		صحیح not_a = not_gate(A);
		صحیح not_b = not_gate(B);
		واپس and_gate(not_a, not_b);
	}

	صحیح xor_gate(صحیح A, صحیح B)
	{
		صحیح a_and_b = and_gate(A, B);
		صحیح a_nor_b = nor_gate(A, B);
		واپس nor_gate(a_and_b, a_nor_b);
	}

	صحیح xnor_gate(صحیح A, صحیح B)
	{
		صحیح a_and_b = and_gate(A, B);
		صحیح a_nor_b = nor_gate(A, B);
		واپس or_gate(a_and_b, a_nor_b);
	}

	صحیح andnot_gate(صحیح A, صحیح B)
	{
		صحیح not_b = not_gate(B);
		واپس and_gate(A, not_b);
	}

	صحیح ornot_gate(صحیح A, صحیح B)
	{
		صحیح not_b = not_gate(B);
		واپس or_gate(A, not_b);
	}

	صحیح mux_gate(صحیح A, صحیح B, صحیح S)
	{
		صحیح not_s = not_gate(S);
		صحیح a_فعال = and_gate(A, not_s);
		صحیح b_فعال = and_gate(B, S);
		واپس or_gate(a_فعال, b_فعال);
	}

	سمتیہ<صحیح> adder(ثابت سمتیہ<صحیح> &A, ثابت سمتیہ<صحیح> &B, صحیح حاصل_اندر, سمتیہ<صحیح> *X = خالی_اشاریہ, سمتیہ<صحیح> *CO = خالی_اشاریہ)
	{
		سمتیہ<صحیح> Y(Get_سائز(A));
		log_assert(Get_سائز(A) == Get_سائز(B));
		برائے (صحیح i = 0; i < Get_سائز(A); i++) {
			صحیح a_xor_b = xor_gate(A[i], B[i]);
			صحیح a_or_b = or_gate(A[i], B[i]);
			صحیح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, حاصل_اندر);
			صحیح tmp = and_gate(a_or_b, حاصل_اندر);
			صحیح حاصل_برآمد = or_gate(a_and_b, tmp);
			اگر (X != خالی_اشاریہ)
				X->at(i) = a_xor_b;
			اگر (CO != خالی_اشاریہ)
				CO->at(i) = حاصل_برآمد;
			حاصل_اندر = حاصل_برآمد;
		}
		واپس Y;
	}

	خالی outport(صحیح گرہ, IdString portname, صحیح portbit = 0)
	{
		اگر (portbit < Get_سائز(cell->get_پورٹ(portname)))
			aig->nodes.at(گرہ).outports.ڈالو_عقب(pair<IdString, صحیح>(portname, portbit));
	}

	خالی outport_bool(صحیح گرہ, IdString portname)
	{
		outport(گرہ, portname);
		برائے (صحیح i = 1; i < Get_سائز(cell->get_پورٹ(portname)); i++)
			outport(bool_گرہ(جھوٹ), portname, i);
	}

	خالی outport_vec(ثابت سمتیہ<صحیح> &vec, IdString portname)
	{
		برائے (صحیح i = 0; i < Get_سائز(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	اگر (cell->type.is_عوامی())
		واپس;

	AigMaker mk(یہ, cell);
	name = cell->type.str();

	string mkname_آخری;
	منطقی mkname_a_signed = جھوٹ;
	منطقی mkname_b_signed = جھوٹ;
	منطقی mkname_is_signed = جھوٹ;

	cell->parameters.ترتیب_دو();
	برائے (خودکار p : cell->parameters)
	{
		اگر (p.پہلا == ID::A_چوڑائی && mkname_a_signed) {
			name = mkname_آخری + stringf(":%d%c", p.دوسرا.as_int(), mkname_is_signed ? 'S' : 'U');
		} ورنہ اگر (p.پہلا == ID::B_چوڑائی && mkname_b_signed) {
			name = mkname_آخری + stringf(":%d%c", p.دوسرا.as_int(), mkname_is_signed ? 'S' : 'U');
		} ورنہ {
			mkname_آخری = name;
			name += stringf(":%d", p.دوسرا.as_int());
		}

		mkname_a_signed = جھوٹ;
		mkname_b_signed = جھوٹ;
		mkname_is_signed = جھوٹ;
		اگر (p.پہلا == ID::A_SIGNED) {
			mkname_a_signed = سچ;
			mkname_is_signed = p.دوسرا.as_bool();
		}
		اگر (p.پہلا == ID::B_SIGNED) {
			mkname_b_signed = سچ;
			mkname_is_signed = p.دوسرا.as_bool();
		}
	}

	اگر (cell->type.اندر(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		برائے (صحیح i = 0; i < Get_سائز(cell->get_پورٹ(ID::Y)); i++) {
			صحیح A = mk.inport(ID::A, i);
			صحیح Y = cell->type.اندر(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		جاؤ optimize;
	}

	اگر (cell->type.اندر(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		برائے (صحیح i = 0; i < Get_سائز(cell->get_پورٹ(ID::Y)); i++) {
			صحیح A = mk.inport(ID::A, i);
			صحیح B = mk.inport(ID::B, i);
			صحیح Y = cell->type.اندر(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.اندر(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.اندر(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.اندر(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.اندر(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.اندر(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.اندر(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.اندر(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		جاؤ optimize;
	}

	اگر (cell->type.اندر(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحیح S = mk.inport(ID::S);
		برائے (صحیح i = 0; i < Get_سائز(cell->get_پورٹ(ID::Y)); i++) {
			صحیح A = mk.inport(ID::A, i);
			صحیح B = mk.inport(ID::B, i);
			صحیح Y = mk.mux_gate(A, B, S);
			اگر (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		جاؤ optimize;
	}

	اگر (cell->type.اندر(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحیح Y = mk.inport(ID::A, 0);
		برائے (صحیح i = 1; i < Get_سائز(cell->get_پورٹ(ID::A)); i++) {
			صحیح A = mk.inport(ID::A, i);
			اگر (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			اگر (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			اگر (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			اگر (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			اگر (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		اگر (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		برائے (صحیح i = 1; i < Get_سائز(cell->get_پورٹ(ID::Y)); i++)
			mk.outport(mk.bool_گرہ(جھوٹ), ID::Y, i);
		جاؤ optimize;
	}

	اگر (cell->type.اندر(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحیح A = mk.inport(ID::A, 0), Y = -1;
		برائے (صحیح i = 1; i < Get_سائز(cell->get_پورٹ(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		اگر (cell->type.اندر(ID($logic_and), ID($logic_or))) {
			صحیح B = mk.inport(ID::B, 0);
			برائے (صحیح i = 1; i < Get_سائز(cell->get_پورٹ(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			اگر (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			اگر (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} ورنہ {
			اگر (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		جاؤ optimize;
	}

	اگر (cell->type.اندر(ID($جوڑو), ID($sub)))
	{
		صحیح چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));
		سمتیہ<صحیح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحیح> B = mk.inport_vec(ID::B, چوڑائی);
		صحیح حاصل = mk.bool_گرہ(جھوٹ);
		اگر (cell->type == ID($sub)) {
			برائے (خودکار &n : B)
				n = mk.not_gate(n);
			حاصل = mk.not_gate(حاصل);
		}
		سمتیہ<صحیح> Y = mk.adder(A, B, حاصل);
		mk.outport_vec(Y, ID::Y);
		جاؤ optimize;
	}

	اگر (cell->type.اندر(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحیح چوڑائی = std::max(Get_سائز(cell->get_پورٹ(ID::A)),
							 Get_سائز(cell->get_پورٹ(ID::B))) + 1;
		سمتیہ<صحیح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحیح> B = mk.inport_vec(ID::B, چوڑائی);

		اگر (cell->type.اندر(ID($gt), ID($ge)))
			std::تبادلہ(A, B);

		صحیح حاصل = mk.bool_گرہ(!cell->type.اندر(ID($le), ID($ge)));
		برائے (خودکار &n : B)
			n = mk.not_gate(n);
		سمتیہ<صحیح> Y = mk.adder(A, B, حاصل);
		mk.outport(Y.عقب(), ID::Y);
		برائے (صحیح i = 1; i < Get_سائز(cell->get_پورٹ(ID::Y)); i++)
			mk.outport(mk.bool_گرہ(جھوٹ), ID::Y, i);
		جاؤ optimize;
	}

	اگر (cell->type == ID($alu))
	{
		صحیح چوڑائی = Get_سائز(cell->get_پورٹ(ID::Y));
		سمتیہ<صحیح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحیح> B = mk.inport_vec(ID::B, چوڑائی);
		صحیح حاصل = mk.inport(ID::CI);
		صحیح binv = mk.inport(ID::BI);
		برائے (خودکار &n : B)
			n = mk.xor_gate(n, binv);
		سمتیہ<صحیح> X(چوڑائی), CO(چوڑائی);
		سمتیہ<صحیح> Y = mk.adder(A, B, حاصل, &X, &CO);
		برائے (صحیح i = 0; i < چوڑائی; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		جاؤ optimize;
	}

	اگر (cell->type.اندر(ID($eq), ID($ne)))
	{
		صحیح چوڑائی = max(Get_سائز(cell->get_پورٹ(ID::A)), Get_سائز(cell->get_پورٹ(ID::B)));
		سمتیہ<صحیح> A = mk.inport_vec(ID::A, چوڑائی);
		سمتیہ<صحیح> B = mk.inport_vec(ID::B, چوڑائی);
		صحیح Y = mk.bool_گرہ(جھوٹ);
		برائے (صحیح i = 0; i < چوڑائی; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		اگر (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		جاؤ optimize;
	}

	اگر (cell->type == ID($_AOI3_))
	{
		صحیح A = mk.inport(ID::A);
		صحیح B = mk.inport(ID::B);
		صحیح C = mk.inport(ID::C);
		صحیح Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		جاؤ optimize;
	}

	اگر (cell->type == ID($_OAI3_))
	{
		صحیح A = mk.inport(ID::A);
		صحیح B = mk.inport(ID::B);
		صحیح C = mk.inport(ID::C);
		صحیح Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		جاؤ optimize;
	}

	اگر (cell->type == ID($_AOI4_))
	{
		صحیح A = mk.inport(ID::A);
		صحیح B = mk.inport(ID::B);
		صحیح C = mk.inport(ID::C);
		صحیح D = mk.inport(ID::D);
		صحیح a_and_b = mk.and_gate(A, B);
		صحیح Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		جاؤ optimize;
	}

	اگر (cell->type == ID($_OAI4_))
	{
		صحیح A = mk.inport(ID::A);
		صحیح B = mk.inport(ID::B);
		صحیح C = mk.inport(ID::C);
		صحیح D = mk.inport(ID::D);
		صحیح a_or_b = mk.or_gate(A, B);
		صحیح Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		جاؤ optimize;
	}

	name.clear();
	واپس;

optimize:;
	pool<صحیح> مستعمل_old_ids;
	سمتیہ<Aig_گرہ> new_nodes;
	dict<صحیح, صحیح> old_مقصد_new_ids;
	old_مقصد_new_ids[-1] = -1;

	برائے (صحیح i = Get_سائز(nodes)-1; i >= 0; i--) {
		اگر (!nodes[i].outports.خالی_خانے())
			مستعمل_old_ids.داخل_کرو(i);
		اگر (!مستعمل_old_ids.گنتی(i))
			جاری;
		اگر (nodes[i].بایاں_والد >= 0)
			مستعمل_old_ids.داخل_کرو(nodes[i].بایاں_والد);
		اگر (nodes[i].دایاں_والد >= 0)
			مستعمل_old_ids.داخل_کرو(nodes[i].دایاں_والد);
	}

	برائے (صحیح i = 0; i < Get_سائز(nodes); i++) {
		اگر (!مستعمل_old_ids.گنتی(i))
			جاری;
		nodes[i].بایاں_والد = old_مقصد_new_ids.at(nodes[i].بایاں_والد);
		nodes[i].دایاں_والد = old_مقصد_new_ids.at(nodes[i].دایاں_والد);
		old_مقصد_new_ids[i] = Get_سائز(new_nodes);
		new_nodes.ڈالو_عقب(nodes[i]);
	}

	new_nodes.تبادلہ(nodes);
}

YOSYS_NAMESPACE_انتہا
