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

Aig_گرێ::Aig_گرێ()
{
	portbit = -1;
	inverter = خطأ;
	چەپ_parent = -1;
	ڕاست_parent = -1;
}

منطقي Aig_گرێ::operator==(ثابت Aig_گرێ &other) ثابت
{
	إذا (portname != other.portname) أرجع خطأ;
	إذا (portbit != other.portbit) أرجع خطأ;
	إذا (inverter != other.inverter) أرجع خطأ;
	إذا (چەپ_parent != other.چەپ_parent) أرجع خطأ;
	إذا (ڕاست_parent != other.ڕاست_parent) أرجع خطأ;
	أرجع true;
}

Hasher Aig_گرێ::hash_into(Hasher h) ثابت
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(چەپ_parent);
	h.eat(ڕاست_parent);
	أرجع h;
}

منطقي Aig::operator==(ثابت Aig &other) ثابت
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
	idict<Aig_گرێ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	صحيح node2index(ثابت Aig_گرێ &گرێ)
	{
		إذا (گرێ.چەپ_parent > گرێ.ڕاست_parent) {
			Aig_گرێ n(گرێ);
			std::swap(n.چەپ_parent, n.ڕاست_parent);
			أرجع node2index(n);
		}

		إذا (!aig_indices.ژماردن(گرێ)) {
			aig_indices.expect(گرێ, GetSize(aig->nodes));
			aig->nodes.پاڵنان_back(گرێ);
		}

		أرجع aig_indices.at(گرێ);
	}

	صحيح bool_گرێ(منطقي بەها)
	{
		Aig_گرێ گرێ;
		گرێ.inverter = بەها;
		أرجع node2index(گرێ);
	}

	صحيح inport(IdString portname, صحيح portbit = 0, منطقي inverter = خطأ)
	{
		إذا (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		إذا (signed_param != IdString::Null && cell->parameters.ژماردن(signed_param) && cell->getParam(signed_param).as_bool())
				أرجع inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			أرجع bool_گرێ(inverter);
		}

		Aig_گرێ گرێ;
		گرێ.portname = portname;
		گرێ.portbit = portbit;
		گرێ.inverter = inverter;
		أرجع node2index(گرێ);
	}

	vector<صحيح> inport_vec(IdString portname, صحيح width)
	{
		vector<صحيح> vec;
		لكل (صحيح i = 0; i < width; i++)
			vec.پاڵنان_back(inport(portname, i));
		أرجع vec;
	}

	صحيح not_inport(IdString portname, صحيح portbit = 0)
	{
		أرجع inport(portname, portbit, true);
	}

	صحيح not_gate(صحيح A)
	{
		Aig_گرێ گرێ(aig_indices[A]);
		گرێ.outports.clear();
		گرێ.inverter = !گرێ.inverter;
		أرجع node2index(گرێ);
	}

	صحيح and_gate(صحيح A, صحيح B, منطقي inverter = خطأ)
	{
		إذا (A == B)
			أرجع inverter ? not_gate(A) : A;

		ثابت Aig_گرێ &nA = aig_indices[A];
		ثابت Aig_گرێ &nB = aig_indices[B];

		Aig_گرێ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		إذا (nA == nB_inv)
			أرجع bool_گرێ(inverter);

		منطقي nA_bool = nA.portbit < 0 && nA.چەپ_parent < 0 && nA.ڕاست_parent < 0;
		منطقي nB_bool = nB.portbit < 0 && nB.چەپ_parent < 0 && nB.ڕاست_parent < 0;

		إذا (nA_bool && nB_bool) {
			منطقي bA = nA.inverter;
			منطقي bB = nB.inverter;
			أرجع bool_گرێ(inverter != (bA && bB));
		}

		إذا (nA_bool) {
			منطقي bA = nA.inverter;
			إذا (inverter)
				أرجع bA ? not_gate(B) : bool_گرێ(true);
			أرجع bA ? B : bool_گرێ(خطأ);
		}

		إذا (nB_bool) {
			منطقي bB = nB.inverter;
			إذا (inverter)
				أرجع bB ? not_gate(A) : bool_گرێ(true);
			أرجع bB ? A : bool_گرێ(خطأ);
		}

		Aig_گرێ گرێ;
		گرێ.inverter = inverter;
		گرێ.چەپ_parent = A;
		گرێ.ڕاست_parent = B;
		أرجع node2index(گرێ);
	}

	صحيح nand_gate(صحيح A, صحيح B)
	{
		أرجع and_gate(A, B, true);
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
		صحيح a_active = and_gate(A, not_s);
		صحيح b_active = and_gate(B, S);
		أرجع or_gate(a_active, b_active);
	}

	vector<صحيح> adder(ثابت vector<صحيح> &A, ثابت vector<صحيح> &B, صحيح carry_in, vector<صحيح> *X = nullptr, vector<صحيح> *CO = nullptr)
	{
		vector<صحيح> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		لكل (صحيح i = 0; i < GetSize(A); i++) {
			صحيح a_xor_b = xor_gate(A[i], B[i]);
			صحيح a_or_b = or_gate(A[i], B[i]);
			صحيح a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			صحيح tmp = and_gate(a_or_b, carry_in);
			صحيح carry_out = or_gate(a_and_b, tmp);
			إذا (X != nullptr)
				X->at(i) = a_xor_b;
			إذا (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		أرجع Y;
	}

	فراغ outport(صحيح گرێ, IdString portname, صحيح portbit = 0)
	{
		إذا (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(گرێ).outports.پاڵنان_back(pair<IdString, صحيح>(portname, portbit));
	}

	فراغ outport_bool(صحيح گرێ, IdString portname)
	{
		outport(گرێ, portname);
		لكل (صحيح i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_گرێ(خطأ), portname, i);
	}

	فراغ outport_vec(ثابت vector<صحيح> &vec, IdString portname)
	{
		لكل (صحيح i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	إذا (cell->type.isPublic())
		أرجع;

	AigMaker mk(هذا, cell);
	name = cell->type.str();

	string mkname_last;
	منطقي mkname_a_signed = خطأ;
	منطقي mkname_b_signed = خطأ;
	منطقي mkname_is_signed = خطأ;

	cell->parameters.sort();
	لكل (auto p : cell->parameters)
	{
		إذا (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا إذا (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} وإلا {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = خطأ;
		mkname_b_signed = خطأ;
		mkname_is_signed = خطأ;
		إذا (p.first == ID::A_SIGNED) {
			mkname_a_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
		إذا (p.first == ID::B_SIGNED) {
			mkname_b_signed = true;
			mkname_is_signed = p.second.as_bool();
		}
	}

	إذا (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		لكل (صحيح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		لكل (صحيح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		صحيح S = mk.inport(ID::S);
		لكل (صحيح i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			صحيح A = mk.inport(ID::A, i);
			صحيح B = mk.inport(ID::B, i);
			صحيح Y = mk.mux_gate(A, B, S);
			إذا (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		اذهب optimize;
	}

	إذا (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		صحيح Y = mk.inport(ID::A, 0);
		لكل (صحيح i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
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
		لكل (صحيح i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_گرێ(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		صحيح A = mk.inport(ID::A, 0), Y = -1;
		لكل (صحيح i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		إذا (cell->type.in(ID($logic_and), ID($logic_or))) {
			صحيح B = mk.inport(ID::B, 0);
			لكل (صحيح i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			إذا (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			إذا (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} وإلا {
			إذا (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.in(ID($add), ID($sub)))
	{
		صحيح width = GetSize(cell->getPort(ID::Y));
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);
		صحيح carry = mk.bool_گرێ(خطأ);
		إذا (cell->type == ID($sub)) {
			لكل (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<صحيح> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		اذهب optimize;
	}

	إذا (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		صحيح width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);

		إذا (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		صحيح carry = mk.bool_گرێ(!cell->type.in(ID($le), ID($ge)));
		لكل (auto &n : B)
			n = mk.not_gate(n);
		vector<صحيح> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		لكل (صحيح i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_گرێ(خطأ), ID::Y, i);
		اذهب optimize;
	}

	إذا (cell->type == ID($alu))
	{
		صحيح width = GetSize(cell->getPort(ID::Y));
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);
		صحيح carry = mk.inport(ID::CI);
		صحيح binv = mk.inport(ID::BI);
		لكل (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<صحيح> X(width), CO(width);
		vector<صحيح> Y = mk.adder(A, B, carry, &X, &CO);
		لكل (صحيح i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		اذهب optimize;
	}

	إذا (cell->type.in(ID($eq), ID($ne)))
	{
		صحيح width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<صحيح> A = mk.inport_vec(ID::A, width);
		vector<صحيح> B = mk.inport_vec(ID::B, width);
		صحيح Y = mk.bool_گرێ(خطأ);
		لكل (صحيح i = 0; i < width; i++)
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
	pool<صحيح> used_old_ids;
	vector<Aig_گرێ> new_nodes;
	dict<صحيح, صحيح> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	لكل (صحيح i = GetSize(nodes)-1; i >= 0; i--) {
		إذا (!nodes[i].outports.empty())
			used_old_ids.زیادکردن(i);
		إذا (!used_old_ids.ژماردن(i))
			استمر;
		إذا (nodes[i].چەپ_parent >= 0)
			used_old_ids.زیادکردن(nodes[i].چەپ_parent);
		إذا (nodes[i].ڕاست_parent >= 0)
			used_old_ids.زیادکردن(nodes[i].ڕاست_parent);
	}

	لكل (صحيح i = 0; i < GetSize(nodes); i++) {
		إذا (!used_old_ids.ژماردن(i))
			استمر;
		nodes[i].چەپ_parent = old_to_new_ids.at(nodes[i].چەپ_parent);
		nodes[i].ڕاست_parent = old_to_new_ids.at(nodes[i].ڕاست_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.پاڵنان_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
