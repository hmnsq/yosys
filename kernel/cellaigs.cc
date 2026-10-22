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

Aig_گره::Aig_گره()
{
	portbit = -1;
	inverter = نادرست;
	چپ_parent = -1;
	راست_parent = -1;
}

منطقی Aig_گره::operator==(ثابت Aig_گره &other) ثابت
{
	اگر (portname != other.portname) بازگشت نادرست;
	اگر (portbit != other.portbit) بازگشت نادرست;
	اگر (inverter != other.inverter) بازگشت نادرست;
	اگر (چپ_parent != other.چپ_parent) بازگشت نادرست;
	اگر (راست_parent != other.راست_parent) بازگشت نادرست;
	بازگشت درست;
}

Hasher Aig_گره::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(چپ_parent);
	h.eat(راست_parent);
	بازگشت h;
}

منطقی Aig::operator==(ثابت Aig &other) ثابت
{
	بازگشت name == other.name;
}

Hasher Aig::hash_into(Hasher h) ثابت
{
	h.eat(name);
	بازگشت h;
}

ساختار AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_گره> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحیح node2index(ثابت Aig_گره &گره)
	{
		اگر (گره.چپ_parent > گره.راست_parent) {
			Aig_گره n(گره);
			std::swap(n.چپ_parent, n.راست_parent);
			بازگشت node2index(n);
		}

		اگر (!aig_indices.شمار(گره)) {
			aig_indices.expect(گره, GetSize(aig->nodes));
			aig->nodes.افزودن_back(گره);
		}

		بازگشت aig_indices.at(گره);
	}

	صحیح bool_گره(منطقی مقدار)
	{
		Aig_گره گره;
		گره.inverter = مقدار;
		بازگشت node2index(گره);
	}

	صحیح inport(IdString portname, صحیح portbit = 0, منطقی inverter = نادرست)
	{
		اگر (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		اگر (signed_param != IdString::Null && cell->parameters.شمار(signed_param) && cell->getParam(signed_param).as_bool())
				بازگشت inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			بازگشت bool_گره(inverter);
		}

		Aig_گره گره;
		گره.portname = portname;
		گره.portbit = portbit;
		گره.inverter = inverter;
		بازگشت node2index(گره);
	}

	vector<صحیح> inport_vec(IdString portname, صحیح width)
	{
		vector<صحیح> vec;
		برای (صحیح i = 0; i < width; i++)
			vec.افزودن_back(inport(portname, i));
		بازگشت vec;
	}

	صحیح not_inport(IdString portname, صحیح portbit = 0)
	{
		بازگشت inport(portname, portbit, درست);
	}

	صحیح not_gate(صحیح A)
	{
		Aig_گره گره(aig_indices[A]);
		گره.outports.clear();
		گره.inverter = !گره.inverter;
		بازگشت node2index(گره);
	}

	صحیح and_gate(صحیح A, صحیح B, منطقی inverter = نادرست)
	{
		اگر (A == B)
			بازگشت inverter ? not_gate(A) : A;

		ثابت Aig_گره &nA = aig_indices[A];
		ثابت Aig_گره &nB = aig_indices[B];

		Aig_گره nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		اگر (nA == nB_inv)
			بازگشت bool_گره(inverter);

		منطقی nA_bool = nA.portbit < 0 && nA.چپ_parent < 0 && nA.راست_parent < 0;
		منطقی nB_bool = nB.portbit < 0 && nB.چپ_parent < 0 && nB.راست_parent < 0;

		اگر (nA_bool && nB_bool) {
			منطقی bA = nA.inverter;
			منطقی bB = nB.inverter;
			بازگشت bool_گره(inverter != (bA && bB));
		}

		اگر (nA_bool) {
			منطقی bA = nA.inverter;
			اگر (inverter)
				بازگشت bA ? not_gate(B) : bool_گره(درست);
			بازگشت bA ? B : bool_گره(نادرست);
		}

		اگر (nB_bool) {
			منطقی bB = nB.inverter;
			اگر (inverter)
				بازگشت bB ? not_gate(A) : bool_گره(درست);
			بازگشت bB ? A : bool_گره(نادرست);
		}

		Aig_گره گره;
		گره.inverter = inverter;
		گره.چپ_parent = A;
		گره.راست_parent = B;
		بازگشت node2index(گره);
	}

	صحیح nand_gate(صحیح A, صحیح B)
	{
		بازگشت and_gate(A, B, درست);
	}

	صحیح or_gate(صحیح A, صحیح B)
	{
		صحیح not_a = not_gate(A);
		صحیح not_b = not_gate(B);
		بازگشت nand_gate(not_a, not_b);
	}

	صحیح nor_gate(صحیح A, صحیح B)
	{
		صحیح not_a = not_gate(A);
		صحیح not_b = not_gate(B);
		بازگشت and_gate(not_a, not_b);
	}

	صحیح xor_gate(صحیح A, صحیح B)
	{
		صحیح a_and_b = and_gate(A, B);
		صحیح a_nor_b = nor_gate(A, B);
		بازگشت nor_gate(a_and_b, a_nor_b);
	}

	صحیح xnor_gate(صحیح A, صحیح B)
	{
		صحیح a_and_b = and_gate(A, B);
		صحیح a_nor_b = nor_gate(A, B);
		بازگشت or_gate(a_and_b, a_nor_b);
	}

	صحیح andnot_gate(صحیح A, صحیح B)
	{
		صحیح not_b = not_gate(B);
		بازگشت and_gate(A, not_b);
	}

	صحیح ornot_gate(صحیح A, صحیح B)
	{
		صحیح not_b = not_gate(B);
		بازگشت or_gate(A, not_b);
	}

	صحیح mux_gate(صحیح A, صحیح B, صحیح S)
	{
		صحیح not_s = not_gate(S);
		صحیح a_active = and_gate(A, not_s);
		صحیح b_active = and_gate(B, S);
		بازگشت or_gate(a_active, b_active);
	}

	vector<صحیح> adder(ثابت vector<صحیح> &A, ثابت vector<صحیح> &B, صحیح carry_in, vector<صحیح> *X = nullptr, vector<صحیح> *CO = nullptr)
	{
		vector<صحیح> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		برای (صحیح i = 0; i < GetSize(A); i++) {
			صحیح a_xor_b = xor_gate(A[i], B[i]);
			صحیح a_or_b = or_gate(A[i], B[i]);
			صحیح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			صحیح tmp = and_gate(a_or_b, carry_in);
			صحیح carry_out = or_gate(a_and_b, tmp);
			اگر (X != nullptr)
				X->at(i) = a_xor_b;
			اگر (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		بازگشت Y;
	}

	تهی outport(صحیح گره, IdString portname, صحیح portbit = 0)
	{
		اگر (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(گره).outports.افزودن_back(pair<IdString, صحیح>(portname, portbit));
	}

	تهی outport_bool(صحیح گره, IdString portname)
	{
		outport(گره, portname);
		برای (صحیح i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_گره(نادرست), portname, i);
	}

	تهی outport_vec(ثابت vector<صحیح> &vec, IdString portname)
	{
		برای (صحیح i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	اگر (cell->type.isPublic())
		بازگشت;

	AigMaker mk(این, cell);
	name = cell->type.str();

	string mkname_last;
	منطقی mkname_a_signed = نادرست;
	منطقی mkname_b_signed = نادرست;
	منطقی mkname_is_signed = نادرست;

	cell->parameters.sort();
	برای (auto p : cell->parameters)
	{
		اگر (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} وگرنه اگر (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} وگرنه {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = نادرست;
		mkname_b_signed = نادرست;
		mkname_is_signed = نادرست;
		اگر (p.first == ID::A_SIGNED) {
			mkname_a_signed = درست;
			mkname_is_signed = p.second.as_bool();
		}
		اگر (p.first == ID::B_SIGNED) {
			mkname_b_signed = درست;
			mkname_is_signed = p.second.as_bool();
		}
	}

	اگر (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		برای (صحیح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحیح A = mk.inport(ID::A, i);
			صحیح Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		برو optimize;
	}

	اگر (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		برای (صحیح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحیح A = mk.inport(ID::A, i);
			صحیح B = mk.inport(ID::B, i);
			صحیح Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		برو optimize;
	}

	اگر (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحیح S = mk.inport(ID::S);
		برای (صحیح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحیح A = mk.inport(ID::A, i);
			صحیح B = mk.inport(ID::B, i);
			صحیح Y = mk.mux_gate(A, B, S);
			اگر (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		برو optimize;
	}

	اگر (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحیح Y = mk.inport(ID::A, 0);
		برای (صحیح i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
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
		برای (صحیح i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_گره(نادرست), ID::Y, i);
		برو optimize;
	}

	اگر (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحیح A = mk.inport(ID::A, 0), Y = -1;
		برای (صحیح i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		اگر (cell->type.in(ID($logic_and), ID($logic_or))) {
			صحیح B = mk.inport(ID::B, 0);
			برای (صحیح i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			اگر (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			اگر (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} وگرنه {
			اگر (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		برو optimize;
	}

	اگر (cell->type.in(ID($add), ID($sub)))
	{
		صحیح width = GetSize(cell->getPort(ID::Y));
		vector<صحیح> A = mk.inport_vec(ID::A, width);
		vector<صحیح> B = mk.inport_vec(ID::B, width);
		صحیح carry = mk.bool_گره(نادرست);
		اگر (cell->type == ID($sub)) {
			برای (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<صحیح> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		برو optimize;
	}

	اگر (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحیح width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<صحیح> A = mk.inport_vec(ID::A, width);
		vector<صحیح> B = mk.inport_vec(ID::B, width);

		اگر (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		صحیح carry = mk.bool_گره(!cell->type.in(ID($le), ID($ge)));
		برای (auto &n : B)
			n = mk.not_gate(n);
		vector<صحیح> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		برای (صحیح i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_گره(نادرست), ID::Y, i);
		برو optimize;
	}

	اگر (cell->type == ID($alu))
	{
		صحیح width = GetSize(cell->getPort(ID::Y));
		vector<صحیح> A = mk.inport_vec(ID::A, width);
		vector<صحیح> B = mk.inport_vec(ID::B, width);
		صحیح carry = mk.inport(ID::CI);
		صحیح binv = mk.inport(ID::BI);
		برای (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<صحیح> X(width), CO(width);
		vector<صحیح> Y = mk.adder(A, B, carry, &X, &CO);
		برای (صحیح i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		برو optimize;
	}

	اگر (cell->type.in(ID($eq), ID($ne)))
	{
		صحیح width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<صحیح> A = mk.inport_vec(ID::A, width);
		vector<صحیح> B = mk.inport_vec(ID::B, width);
		صحیح Y = mk.bool_گره(نادرست);
		برای (صحیح i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		اگر (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		برو optimize;
	}

	اگر (cell->type == ID($_AOI3_))
	{
		صحیح A = mk.inport(ID::A);
		صحیح B = mk.inport(ID::B);
		صحیح C = mk.inport(ID::C);
		صحیح Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		برو optimize;
	}

	اگر (cell->type == ID($_OAI3_))
	{
		صحیح A = mk.inport(ID::A);
		صحیح B = mk.inport(ID::B);
		صحیح C = mk.inport(ID::C);
		صحیح Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		برو optimize;
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
		برو optimize;
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
		برو optimize;
	}

	name.clear();
	بازگشت;

optimize:;
	pool<صحیح> used_old_ids;
	vector<Aig_گره> new_nodes;
	dict<صحیح, صحیح> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	برای (صحیح i = GetSize(nodes)-1; i >= 0; i--) {
		اگر (!nodes[i].outports.empty())
			used_old_ids.درج(i);
		اگر (!used_old_ids.شمار(i))
			ادامه;
		اگر (nodes[i].چپ_parent >= 0)
			used_old_ids.درج(nodes[i].چپ_parent);
		اگر (nodes[i].راست_parent >= 0)
			used_old_ids.درج(nodes[i].راست_parent);
	}

	برای (صحیح i = 0; i < GetSize(nodes); i++) {
		اگر (!used_old_ids.شمار(i))
			ادامه;
		nodes[i].چپ_parent = old_to_new_ids.at(nodes[i].چپ_parent);
		nodes[i].راست_parent = old_to_new_ids.at(nodes[i].راست_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.افزودن_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
