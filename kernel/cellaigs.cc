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

#অন্তর্ভাব "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_গ্রন্থি::Aig_গ্রন্থি()
{
	portbit = -1;
	inverter = অসত্য;
	বাম_parent = -1;
	ডান_parent = -1;
}

তার্কিক Aig_গ্রন্থি::operator==(ধ্রুব Aig_গ্রন্থি &other) ধ্রুব
{
	যদি (portname != other.portname) প্রত্যাগম অসত্য;
	যদি (portbit != other.portbit) প্রত্যাগম অসত্য;
	যদি (inverter != other.inverter) প্রত্যাগম অসত্য;
	যদি (বাম_parent != other.বাম_parent) প্রত্যাগম অসত্য;
	যদি (ডান_parent != other.ডান_parent) প্রত্যাগম অসত্য;
	প্রত্যাগম সত্য;
}

Hasher Aig_গ্রন্থি::hash_into(Hasher h) ধ্রুব
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(বাম_parent);
	h.eat(ডান_parent);
	প্রত্যাগম h;
}

তার্কিক Aig::operator==(ধ্রুব Aig &other) ধ্রুব
{
	প্রত্যাগম name == other.name;
}

Hasher Aig::hash_into(Hasher h) ধ্রুব
{
	h.eat(name);
	প্রত্যাগম h;
}

সংরচনা AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_গ্রন্থি> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	পূর্ণাঙ্ক node2index(ধ্রুব Aig_গ্রন্থি &গ্রন্থি)
	{
		যদি (গ্রন্থি.বাম_parent > গ্রন্থি.ডান_parent) {
			Aig_গ্রন্থি n(গ্রন্থি);
			std::swap(n.বাম_parent, n.ডান_parent);
			প্রত্যাগম node2index(n);
		}

		যদি (!aig_indices.গুনতি(গ্রন্থি)) {
			aig_indices.expect(গ্রন্থি, GetSize(aig->nodes));
			aig->nodes.ঠেলো_back(গ্রন্থি);
		}

		প্রত্যাগম aig_indices.at(গ্রন্থি);
	}

	পূর্ণাঙ্ক bool_গ্রন্থি(তার্কিক মান)
	{
		Aig_গ্রন্থি গ্রন্থি;
		গ্রন্থি.inverter = মান;
		প্রত্যাগম node2index(গ্রন্থি);
	}

	পূর্ণাঙ্ক inport(IdString portname, পূর্ণাঙ্ক portbit = 0, তার্কিক inverter = অসত্য)
	{
		যদি (portbit >= GetSize(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		যদি (signed_param != IdString::Null && cell->parameters.গুনতি(signed_param) && cell->getParam(signed_param).as_bool())
				প্রত্যাগম inport(portname, GetSize(cell->getPort(portname))-1, inverter);
			প্রত্যাগম bool_গ্রন্থি(inverter);
		}

		Aig_গ্রন্থি গ্রন্থি;
		গ্রন্থি.portname = portname;
		গ্রন্থি.portbit = portbit;
		গ্রন্থি.inverter = inverter;
		প্রত্যাগম node2index(গ্রন্থি);
	}

	vector<পূর্ণাঙ্ক> inport_vec(IdString portname, পূর্ণাঙ্ক width)
	{
		vector<পূর্ণাঙ্ক> vec;
		প্রতি (পূর্ণাঙ্ক i = 0; i < width; i++)
			vec.ঠেলো_back(inport(portname, i));
		প্রত্যাগম vec;
	}

	পূর্ণাঙ্ক not_inport(IdString portname, পূর্ণাঙ্ক portbit = 0)
	{
		প্রত্যাগম inport(portname, portbit, সত্য);
	}

	পূর্ণাঙ্ক not_gate(পূর্ণাঙ্ক A)
	{
		Aig_গ্রন্থি গ্রন্থি(aig_indices[A]);
		গ্রন্থি.outports.clear();
		গ্রন্থি.inverter = !গ্রন্থি.inverter;
		প্রত্যাগম node2index(গ্রন্থি);
	}

	পূর্ণাঙ্ক and_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B, তার্কিক inverter = অসত্য)
	{
		যদি (A == B)
			প্রত্যাগম inverter ? not_gate(A) : A;

		ধ্রুব Aig_গ্রন্থি &nA = aig_indices[A];
		ধ্রুব Aig_গ্রন্থি &nB = aig_indices[B];

		Aig_গ্রন্থি nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		যদি (nA == nB_inv)
			প্রত্যাগম bool_গ্রন্থি(inverter);

		তার্কিক nA_bool = nA.portbit < 0 && nA.বাম_parent < 0 && nA.ডান_parent < 0;
		তার্কিক nB_bool = nB.portbit < 0 && nB.বাম_parent < 0 && nB.ডান_parent < 0;

		যদি (nA_bool && nB_bool) {
			তার্কিক bA = nA.inverter;
			তার্কিক bB = nB.inverter;
			প্রত্যাগম bool_গ্রন্থি(inverter != (bA && bB));
		}

		যদি (nA_bool) {
			তার্কিক bA = nA.inverter;
			যদি (inverter)
				প্রত্যাগম bA ? not_gate(B) : bool_গ্রন্থি(সত্য);
			প্রত্যাগম bA ? B : bool_গ্রন্থি(অসত্য);
		}

		যদি (nB_bool) {
			তার্কিক bB = nB.inverter;
			যদি (inverter)
				প্রত্যাগম bB ? not_gate(A) : bool_গ্রন্থি(সত্য);
			প্রত্যাগম bB ? A : bool_গ্রন্থি(অসত্য);
		}

		Aig_গ্রন্থি গ্রন্থি;
		গ্রন্থি.inverter = inverter;
		গ্রন্থি.বাম_parent = A;
		গ্রন্থি.ডান_parent = B;
		প্রত্যাগম node2index(গ্রন্থি);
	}

	পূর্ণাঙ্ক nand_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B)
	{
		প্রত্যাগম and_gate(A, B, সত্য);
	}

	পূর্ণাঙ্ক or_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B)
	{
		পূর্ণাঙ্ক not_a = not_gate(A);
		পূর্ণাঙ্ক not_b = not_gate(B);
		প্রত্যাগম nand_gate(not_a, not_b);
	}

	পূর্ণাঙ্ক nor_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B)
	{
		পূর্ণাঙ্ক not_a = not_gate(A);
		পূর্ণাঙ্ক not_b = not_gate(B);
		প্রত্যাগম and_gate(not_a, not_b);
	}

	পূর্ণাঙ্ক xor_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B)
	{
		পূর্ণাঙ্ক a_and_b = and_gate(A, B);
		পূর্ণাঙ্ক a_nor_b = nor_gate(A, B);
		প্রত্যাগম nor_gate(a_and_b, a_nor_b);
	}

	পূর্ণাঙ্ক xnor_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B)
	{
		পূর্ণাঙ্ক a_and_b = and_gate(A, B);
		পূর্ণাঙ্ক a_nor_b = nor_gate(A, B);
		প্রত্যাগম or_gate(a_and_b, a_nor_b);
	}

	পূর্ণাঙ্ক andnot_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B)
	{
		পূর্ণাঙ্ক not_b = not_gate(B);
		প্রত্যাগম and_gate(A, not_b);
	}

	পূর্ণাঙ্ক ornot_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B)
	{
		পূর্ণাঙ্ক not_b = not_gate(B);
		প্রত্যাগম or_gate(A, not_b);
	}

	পূর্ণাঙ্ক mux_gate(পূর্ণাঙ্ক A, পূর্ণাঙ্ক B, পূর্ণাঙ্ক S)
	{
		পূর্ণাঙ্ক not_s = not_gate(S);
		পূর্ণাঙ্ক a_active = and_gate(A, not_s);
		পূর্ণাঙ্ক b_active = and_gate(B, S);
		প্রত্যাগম or_gate(a_active, b_active);
	}

	vector<পূর্ণাঙ্ক> adder(ধ্রুব vector<পূর্ণাঙ্ক> &A, ধ্রুব vector<পূর্ণাঙ্ক> &B, পূর্ণাঙ্ক carry_in, vector<পূর্ণাঙ্ক> *X = nullptr, vector<পূর্ণাঙ্ক> *CO = nullptr)
	{
		vector<পূর্ণাঙ্ক> Y(GetSize(A));
		log_assert(GetSize(A) == GetSize(B));
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(A); i++) {
			পূর্ণাঙ্ক a_xor_b = xor_gate(A[i], B[i]);
			পূর্ণাঙ্ক a_or_b = or_gate(A[i], B[i]);
			পূর্ণাঙ্ক a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			পূর্ণাঙ্ক tmp = and_gate(a_or_b, carry_in);
			পূর্ণাঙ্ক carry_out = or_gate(a_and_b, tmp);
			যদি (X != nullptr)
				X->at(i) = a_xor_b;
			যদি (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		প্রত্যাগম Y;
	}

	শূন্য outport(পূর্ণাঙ্ক গ্রন্থি, IdString portname, পূর্ণাঙ্ক portbit = 0)
	{
		যদি (portbit < GetSize(cell->getPort(portname)))
			aig->nodes.at(গ্রন্থি).outports.ঠেলো_back(pair<IdString, পূর্ণাঙ্ক>(portname, portbit));
	}

	শূন্য outport_bool(পূর্ণাঙ্ক গ্রন্থি, IdString portname)
	{
		outport(গ্রন্থি, portname);
		প্রতি (পূর্ণাঙ্ক i = 1; i < GetSize(cell->getPort(portname)); i++)
			outport(bool_গ্রন্থি(অসত্য), portname, i);
	}

	শূন্য outport_vec(ধ্রুব vector<পূর্ণাঙ্ক> &vec, IdString portname)
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	যদি (cell->type.isPublic())
		প্রত্যাগম;

	AigMaker mk(এতত্, cell);
	name = cell->type.str();

	string mkname_last;
	তার্কিক mkname_a_signed = অসত্য;
	তার্কিক mkname_b_signed = অসত্য;
	তার্কিক mkname_is_signed = অসত্য;

	cell->parameters.sort();
	প্রতি (auto p : cell->parameters)
	{
		যদি (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} অন্যথা যদি (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} অন্যথা {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = অসত্য;
		mkname_b_signed = অসত্য;
		mkname_is_signed = অসত্য;
		যদি (p.first == ID::A_SIGNED) {
			mkname_a_signed = সত্য;
			mkname_is_signed = p.second.as_bool();
		}
		যদি (p.first == ID::B_SIGNED) {
			mkname_b_signed = সত্য;
			mkname_is_signed = p.second.as_bool();
		}
	}

	যদি (cell->type.in(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			পূর্ণাঙ্ক A = mk.inport(ID::A, i);
			পূর্ণাঙ্ক Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		গচ্ছ optimize;
	}

	যদি (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			পূর্ণাঙ্ক A = mk.inport(ID::A, i);
			পূর্ণাঙ্ক B = mk.inport(ID::B, i);
			পূর্ণাঙ্ক Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		গচ্ছ optimize;
	}

	যদি (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		পূর্ণাঙ্ক S = mk.inport(ID::S);
		প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(cell->getPort(ID::Y)); i++) {
			পূর্ণাঙ্ক A = mk.inport(ID::A, i);
			পূর্ণাঙ্ক B = mk.inport(ID::B, i);
			পূর্ণাঙ্ক Y = mk.mux_gate(A, B, S);
			যদি (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		গচ্ছ optimize;
	}

	যদি (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		পূর্ণাঙ্ক Y = mk.inport(ID::A, 0);
		প্রতি (পূর্ণাঙ্ক i = 1; i < GetSize(cell->getPort(ID::A)); i++) {
			পূর্ণাঙ্ক A = mk.inport(ID::A, i);
			যদি (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			যদি (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			যদি (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			যদি (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			যদি (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		যদি (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		প্রতি (পূর্ণাঙ্ক i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_গ্রন্থি(অসত্য), ID::Y, i);
		গচ্ছ optimize;
	}

	যদি (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		পূর্ণাঙ্ক A = mk.inport(ID::A, 0), Y = -1;
		প্রতি (পূর্ণাঙ্ক i = 1; i < GetSize(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		যদি (cell->type.in(ID($logic_and), ID($logic_or))) {
			পূর্ণাঙ্ক B = mk.inport(ID::B, 0);
			প্রতি (পূর্ণাঙ্ক i = 1; i < GetSize(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			যদি (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			যদি (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} অন্যথা {
			যদি (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		গচ্ছ optimize;
	}

	যদি (cell->type.in(ID($add), ID($sub)))
	{
		পূর্ণাঙ্ক width = GetSize(cell->getPort(ID::Y));
		vector<পূর্ণাঙ্ক> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণাঙ্ক> B = mk.inport_vec(ID::B, width);
		পূর্ণাঙ্ক carry = mk.bool_গ্রন্থি(অসত্য);
		যদি (cell->type == ID($sub)) {
			প্রতি (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<পূর্ণাঙ্ক> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		গচ্ছ optimize;
	}

	যদি (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		পূর্ণাঙ্ক width = std::max(GetSize(cell->getPort(ID::A)),
							 GetSize(cell->getPort(ID::B))) + 1;
		vector<পূর্ণাঙ্ক> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণাঙ্ক> B = mk.inport_vec(ID::B, width);

		যদি (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		পূর্ণাঙ্ক carry = mk.bool_গ্রন্থি(!cell->type.in(ID($le), ID($ge)));
		প্রতি (auto &n : B)
			n = mk.not_gate(n);
		vector<পূর্ণাঙ্ক> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		প্রতি (পূর্ণাঙ্ক i = 1; i < GetSize(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_গ্রন্থি(অসত্য), ID::Y, i);
		গচ্ছ optimize;
	}

	যদি (cell->type == ID($alu))
	{
		পূর্ণাঙ্ক width = GetSize(cell->getPort(ID::Y));
		vector<পূর্ণাঙ্ক> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণাঙ্ক> B = mk.inport_vec(ID::B, width);
		পূর্ণাঙ্ক carry = mk.inport(ID::CI);
		পূর্ণাঙ্ক binv = mk.inport(ID::BI);
		প্রতি (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<পূর্ণাঙ্ক> X(width), CO(width);
		vector<পূর্ণাঙ্ক> Y = mk.adder(A, B, carry, &X, &CO);
		প্রতি (পূর্ণাঙ্ক i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		গচ্ছ optimize;
	}

	যদি (cell->type.in(ID($eq), ID($ne)))
	{
		পূর্ণাঙ্ক width = max(GetSize(cell->getPort(ID::A)), GetSize(cell->getPort(ID::B)));
		vector<পূর্ণাঙ্ক> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণাঙ্ক> B = mk.inport_vec(ID::B, width);
		পূর্ণাঙ্ক Y = mk.bool_গ্রন্থি(অসত্য);
		প্রতি (পূর্ণাঙ্ক i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		যদি (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		গচ্ছ optimize;
	}

	যদি (cell->type == ID($_AOI3_))
	{
		পূর্ণাঙ্ক A = mk.inport(ID::A);
		পূর্ণাঙ্ক B = mk.inport(ID::B);
		পূর্ণাঙ্ক C = mk.inport(ID::C);
		পূর্ণাঙ্ক Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		গচ্ছ optimize;
	}

	যদি (cell->type == ID($_OAI3_))
	{
		পূর্ণাঙ্ক A = mk.inport(ID::A);
		পূর্ণাঙ্ক B = mk.inport(ID::B);
		পূর্ণাঙ্ক C = mk.inport(ID::C);
		পূর্ণাঙ্ক Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		গচ্ছ optimize;
	}

	যদি (cell->type == ID($_AOI4_))
	{
		পূর্ণাঙ্ক A = mk.inport(ID::A);
		পূর্ণাঙ্ক B = mk.inport(ID::B);
		পূর্ণাঙ্ক C = mk.inport(ID::C);
		পূর্ণাঙ্ক D = mk.inport(ID::D);
		পূর্ণাঙ্ক a_and_b = mk.and_gate(A, B);
		পূর্ণাঙ্ক Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		গচ্ছ optimize;
	}

	যদি (cell->type == ID($_OAI4_))
	{
		পূর্ণাঙ্ক A = mk.inport(ID::A);
		পূর্ণাঙ্ক B = mk.inport(ID::B);
		পূর্ণাঙ্ক C = mk.inport(ID::C);
		পূর্ণাঙ্ক D = mk.inport(ID::D);
		পূর্ণাঙ্ক a_or_b = mk.or_gate(A, B);
		পূর্ণাঙ্ক Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		গচ্ছ optimize;
	}

	name.clear();
	প্রত্যাগম;

optimize:;
	pool<পূর্ণাঙ্ক> used_old_ids;
	vector<Aig_গ্রন্থি> new_nodes;
	dict<পূর্ণাঙ্ক, পূর্ণাঙ্ক> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	প্রতি (পূর্ণাঙ্ক i = GetSize(nodes)-1; i >= 0; i--) {
		যদি (!nodes[i].outports.empty())
			used_old_ids.ঢোকাও(i);
		যদি (!used_old_ids.গুনতি(i))
			অনুবর্ত;
		যদি (nodes[i].বাম_parent >= 0)
			used_old_ids.ঢোকাও(nodes[i].বাম_parent);
		যদি (nodes[i].ডান_parent >= 0)
			used_old_ids.ঢোকাও(nodes[i].ডান_parent);
	}

	প্রতি (পূর্ণাঙ্ক i = 0; i < GetSize(nodes); i++) {
		যদি (!used_old_ids.গুনতি(i))
			অনুবর্ত;
		nodes[i].বাম_parent = old_to_new_ids.at(nodes[i].বাম_parent);
		nodes[i].ডান_parent = old_to_new_ids.at(nodes[i].ডান_parent);
		old_to_new_ids[i] = GetSize(new_nodes);
		new_nodes.ঠেলো_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
