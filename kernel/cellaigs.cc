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
	inverter = মিথ্যা;
	বাম_parent = -1;
	ডান_parent = -1;
}

যৌক্তিক Aig_গ্রন্থি::operator==(ধ্রুবক Aig_গ্রন্থি &other) ধ্রুবক
{
	যদি (portname != other.portname) ফেরত মিথ্যা;
	যদি (portbit != other.portbit) ফেরত মিথ্যা;
	যদি (inverter != other.inverter) ফেরত মিথ্যা;
	যদি (বাম_parent != other.বাম_parent) ফেরত মিথ্যা;
	যদি (ডান_parent != other.ডান_parent) ফেরত মিথ্যা;
	ফেরত সত্য;
}

Hasher Aig_গ্রন্থি::hash_into(Hasher h) ধ্রুবক
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(বাম_parent);
	h.eat(ডান_parent);
	ফেরত h;
}

যৌক্তিক Aig::operator==(ধ্রুবক Aig &other) ধ্রুবক
{
	ফেরত name == other.name;
}

Hasher Aig::hash_into(Hasher h) ধ্রুবক
{
	h.eat(name);
	ফেরত h;
}

কাঠামো AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_গ্রন্থি> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	পূর্ণসংখ্যা node2index(ধ্রুবক Aig_গ্রন্থি &গ্রন্থি)
	{
		যদি (গ্রন্থি.বাম_parent > গ্রন্থি.ডান_parent) {
			Aig_গ্রন্থি n(গ্রন্থি);
			std::swap(n.বাম_parent, n.ডান_parent);
			ফেরত node2index(n);
		}

		যদি (!aig_indices.গুনতি(গ্রন্থি)) {
			aig_indices.expect(গ্রন্থি, Get_আকার(aig->nodes));
			aig->nodes.ঠেলো_back(গ্রন্থি);
		}

		ফেরত aig_indices.at(গ্রন্থি);
	}

	পূর্ণসংখ্যা bool_গ্রন্থি(যৌক্তিক মান)
	{
		Aig_গ্রন্থি গ্রন্থি;
		গ্রন্থি.inverter = মান;
		ফেরত node2index(গ্রন্থি);
	}

	পূর্ণসংখ্যা inport(IdString portname, পূর্ণসংখ্যা portbit = 0, যৌক্তিক inverter = মিথ্যা)
	{
		যদি (portbit >= Get_আকার(cell->getPort(portname))) {
			IdString signed_param = cell->twines().find(
				cell->twines().str(portname) + "_SIGNED");
		যদি (signed_param != IdString::Null && cell->parameters.গুনতি(signed_param) && cell->getParam(signed_param).as_bool())
				ফেরত inport(portname, Get_আকার(cell->getPort(portname))-1, inverter);
			ফেরত bool_গ্রন্থি(inverter);
		}

		Aig_গ্রন্থি গ্রন্থি;
		গ্রন্থি.portname = portname;
		গ্রন্থি.portbit = portbit;
		গ্রন্থি.inverter = inverter;
		ফেরত node2index(গ্রন্থি);
	}

	vector<পূর্ণসংখ্যা> inport_vec(IdString portname, পূর্ণসংখ্যা width)
	{
		vector<পূর্ণসংখ্যা> vec;
		জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
			vec.ঠেলো_back(inport(portname, i));
		ফেরত vec;
	}

	পূর্ণসংখ্যা not_inport(IdString portname, পূর্ণসংখ্যা portbit = 0)
	{
		ফেরত inport(portname, portbit, সত্য);
	}

	পূর্ণসংখ্যা not_gate(পূর্ণসংখ্যা A)
	{
		Aig_গ্রন্থি গ্রন্থি(aig_indices[A]);
		গ্রন্থি.outports.clear();
		গ্রন্থি.inverter = !গ্রন্থি.inverter;
		ফেরত node2index(গ্রন্থি);
	}

	পূর্ণসংখ্যা and_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B, যৌক্তিক inverter = মিথ্যা)
	{
		যদি (A == B)
			ফেরত inverter ? not_gate(A) : A;

		ধ্রুবক Aig_গ্রন্থি &nA = aig_indices[A];
		ধ্রুবক Aig_গ্রন্থি &nB = aig_indices[B];

		Aig_গ্রন্থি nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		যদি (nA == nB_inv)
			ফেরত bool_গ্রন্থি(inverter);

		যৌক্তিক nA_bool = nA.portbit < 0 && nA.বাম_parent < 0 && nA.ডান_parent < 0;
		যৌক্তিক nB_bool = nB.portbit < 0 && nB.বাম_parent < 0 && nB.ডান_parent < 0;

		যদি (nA_bool && nB_bool) {
			যৌক্তিক bA = nA.inverter;
			যৌক্তিক bB = nB.inverter;
			ফেরত bool_গ্রন্থি(inverter != (bA && bB));
		}

		যদি (nA_bool) {
			যৌক্তিক bA = nA.inverter;
			যদি (inverter)
				ফেরত bA ? not_gate(B) : bool_গ্রন্থি(সত্য);
			ফেরত bA ? B : bool_গ্রন্থি(মিথ্যা);
		}

		যদি (nB_bool) {
			যৌক্তিক bB = nB.inverter;
			যদি (inverter)
				ফেরত bB ? not_gate(A) : bool_গ্রন্থি(সত্য);
			ফেরত bB ? A : bool_গ্রন্থি(মিথ্যা);
		}

		Aig_গ্রন্থি গ্রন্থি;
		গ্রন্থি.inverter = inverter;
		গ্রন্থি.বাম_parent = A;
		গ্রন্থি.ডান_parent = B;
		ফেরত node2index(গ্রন্থি);
	}

	পূর্ণসংখ্যা nand_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B)
	{
		ফেরত and_gate(A, B, সত্য);
	}

	পূর্ণসংখ্যা or_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B)
	{
		পূর্ণসংখ্যা not_a = not_gate(A);
		পূর্ণসংখ্যা not_b = not_gate(B);
		ফেরত nand_gate(not_a, not_b);
	}

	পূর্ণসংখ্যা nor_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B)
	{
		পূর্ণসংখ্যা not_a = not_gate(A);
		পূর্ণসংখ্যা not_b = not_gate(B);
		ফেরত and_gate(not_a, not_b);
	}

	পূর্ণসংখ্যা xor_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B)
	{
		পূর্ণসংখ্যা a_and_b = and_gate(A, B);
		পূর্ণসংখ্যা a_nor_b = nor_gate(A, B);
		ফেরত nor_gate(a_and_b, a_nor_b);
	}

	পূর্ণসংখ্যা xnor_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B)
	{
		পূর্ণসংখ্যা a_and_b = and_gate(A, B);
		পূর্ণসংখ্যা a_nor_b = nor_gate(A, B);
		ফেরত or_gate(a_and_b, a_nor_b);
	}

	পূর্ণসংখ্যা andnot_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B)
	{
		পূর্ণসংখ্যা not_b = not_gate(B);
		ফেরত and_gate(A, not_b);
	}

	পূর্ণসংখ্যা ornot_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B)
	{
		পূর্ণসংখ্যা not_b = not_gate(B);
		ফেরত or_gate(A, not_b);
	}

	পূর্ণসংখ্যা mux_gate(পূর্ণসংখ্যা A, পূর্ণসংখ্যা B, পূর্ণসংখ্যা S)
	{
		পূর্ণসংখ্যা not_s = not_gate(S);
		পূর্ণসংখ্যা a_active = and_gate(A, not_s);
		পূর্ণসংখ্যা b_active = and_gate(B, S);
		ফেরত or_gate(a_active, b_active);
	}

	vector<পূর্ণসংখ্যা> adder(ধ্রুবক vector<পূর্ণসংখ্যা> &A, ধ্রুবক vector<পূর্ণসংখ্যা> &B, পূর্ণসংখ্যা carry_in, vector<পূর্ণসংখ্যা> *X = nullptr, vector<পূর্ণসংখ্যা> *CO = nullptr)
	{
		vector<পূর্ণসংখ্যা> Y(Get_আকার(A));
		log_assert(Get_আকার(A) == Get_আকার(B));
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(A); i++) {
			পূর্ণসংখ্যা a_xor_b = xor_gate(A[i], B[i]);
			পূর্ণসংখ্যা a_or_b = or_gate(A[i], B[i]);
			পূর্ণসংখ্যা a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, carry_in);
			পূর্ণসংখ্যা tmp = and_gate(a_or_b, carry_in);
			পূর্ণসংখ্যা carry_out = or_gate(a_and_b, tmp);
			যদি (X != nullptr)
				X->at(i) = a_xor_b;
			যদি (CO != nullptr)
				CO->at(i) = carry_out;
			carry_in = carry_out;
		}
		ফেরত Y;
	}

	শূন্য outport(পূর্ণসংখ্যা গ্রন্থি, IdString portname, পূর্ণসংখ্যা portbit = 0)
	{
		যদি (portbit < Get_আকার(cell->getPort(portname)))
			aig->nodes.at(গ্রন্থি).outports.ঠেলো_back(pair<IdString, পূর্ণসংখ্যা>(portname, portbit));
	}

	শূন্য outport_bool(পূর্ণসংখ্যা গ্রন্থি, IdString portname)
	{
		outport(গ্রন্থি, portname);
		জন্য (পূর্ণসংখ্যা i = 1; i < Get_আকার(cell->getPort(portname)); i++)
			outport(bool_গ্রন্থি(মিথ্যা), portname, i);
	}

	শূন্য outport_vec(ধ্রুবক vector<পূর্ণসংখ্যা> &vec, IdString portname)
	{
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	যদি (cell->type.isPublic())
		ফেরত;

	AigMaker mk(এই, cell);
	name = cell->type.str();

	string mkname_last;
	যৌক্তিক mkname_a_signed = মিথ্যা;
	যৌক্তিক mkname_b_signed = মিথ্যা;
	যৌক্তিক mkname_is_signed = মিথ্যা;

	cell->parameters.sort();
	জন্য (auto p : cell->parameters)
	{
		যদি (p.first == ID::A_WIDTH && mkname_a_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} অন্যথা যদি (p.first == ID::B_WIDTH && mkname_b_signed) {
			name = mkname_last + stringf(":%d%c", p.second.as_int(), mkname_is_signed ? 'S' : 'U');
		} অন্যথা {
			mkname_last = name;
			name += stringf(":%d", p.second.as_int());
		}

		mkname_a_signed = মিথ্যা;
		mkname_b_signed = মিথ্যা;
		mkname_is_signed = মিথ্যা;
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
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(cell->getPort(ID::Y)); i++) {
			পূর্ণসংখ্যা A = mk.inport(ID::A, i);
			পূর্ণসংখ্যা Y = cell->type.in(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		যাও optimize;
	}

	যদি (cell->type.in(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(cell->getPort(ID::Y)); i++) {
			পূর্ণসংখ্যা A = mk.inport(ID::A, i);
			পূর্ণসংখ্যা B = mk.inport(ID::B, i);
			পূর্ণসংখ্যা Y = cell->type.in(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.in(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.in(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.in(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.in(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.in(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.in(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.in(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		যাও optimize;
	}

	যদি (cell->type.in(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		পূর্ণসংখ্যা S = mk.inport(ID::S);
		জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(cell->getPort(ID::Y)); i++) {
			পূর্ণসংখ্যা A = mk.inport(ID::A, i);
			পূর্ণসংখ্যা B = mk.inport(ID::B, i);
			পূর্ণসংখ্যা Y = mk.mux_gate(A, B, S);
			যদি (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		যাও optimize;
	}

	যদি (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		পূর্ণসংখ্যা Y = mk.inport(ID::A, 0);
		জন্য (পূর্ণসংখ্যা i = 1; i < Get_আকার(cell->getPort(ID::A)); i++) {
			পূর্ণসংখ্যা A = mk.inport(ID::A, i);
			যদি (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			যদি (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			যদি (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			যদি (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			যদি (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		যদি (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		জন্য (পূর্ণসংখ্যা i = 1; i < Get_আকার(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_গ্রন্থি(মিথ্যা), ID::Y, i);
		যাও optimize;
	}

	যদি (cell->type.in(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		পূর্ণসংখ্যা A = mk.inport(ID::A, 0), Y = -1;
		জন্য (পূর্ণসংখ্যা i = 1; i < Get_আকার(cell->getPort(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		যদি (cell->type.in(ID($logic_and), ID($logic_or))) {
			পূর্ণসংখ্যা B = mk.inport(ID::B, 0);
			জন্য (পূর্ণসংখ্যা i = 1; i < Get_আকার(cell->getPort(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			যদি (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			যদি (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} অন্যথা {
			যদি (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		যাও optimize;
	}

	যদি (cell->type.in(ID($add), ID($sub)))
	{
		পূর্ণসংখ্যা width = Get_আকার(cell->getPort(ID::Y));
		vector<পূর্ণসংখ্যা> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণসংখ্যা> B = mk.inport_vec(ID::B, width);
		পূর্ণসংখ্যা carry = mk.bool_গ্রন্থি(মিথ্যা);
		যদি (cell->type == ID($sub)) {
			জন্য (auto &n : B)
				n = mk.not_gate(n);
			carry = mk.not_gate(carry);
		}
		vector<পূর্ণসংখ্যা> Y = mk.adder(A, B, carry);
		mk.outport_vec(Y, ID::Y);
		যাও optimize;
	}

	যদি (cell->type.in(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		পূর্ণসংখ্যা width = std::max(Get_আকার(cell->getPort(ID::A)),
							 Get_আকার(cell->getPort(ID::B))) + 1;
		vector<পূর্ণসংখ্যা> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণসংখ্যা> B = mk.inport_vec(ID::B, width);

		যদি (cell->type.in(ID($gt), ID($ge)))
			std::swap(A, B);

		পূর্ণসংখ্যা carry = mk.bool_গ্রন্থি(!cell->type.in(ID($le), ID($ge)));
		জন্য (auto &n : B)
			n = mk.not_gate(n);
		vector<পূর্ণসংখ্যা> Y = mk.adder(A, B, carry);
		mk.outport(Y.back(), ID::Y);
		জন্য (পূর্ণসংখ্যা i = 1; i < Get_আকার(cell->getPort(ID::Y)); i++)
			mk.outport(mk.bool_গ্রন্থি(মিথ্যা), ID::Y, i);
		যাও optimize;
	}

	যদি (cell->type == ID($alu))
	{
		পূর্ণসংখ্যা width = Get_আকার(cell->getPort(ID::Y));
		vector<পূর্ণসংখ্যা> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণসংখ্যা> B = mk.inport_vec(ID::B, width);
		পূর্ণসংখ্যা carry = mk.inport(ID::CI);
		পূর্ণসংখ্যা binv = mk.inport(ID::BI);
		জন্য (auto &n : B)
			n = mk.xor_gate(n, binv);
		vector<পূর্ণসংখ্যা> X(width), CO(width);
		vector<পূর্ণসংখ্যা> Y = mk.adder(A, B, carry, &X, &CO);
		জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		যাও optimize;
	}

	যদি (cell->type.in(ID($eq), ID($ne)))
	{
		পূর্ণসংখ্যা width = max(Get_আকার(cell->getPort(ID::A)), Get_আকার(cell->getPort(ID::B)));
		vector<পূর্ণসংখ্যা> A = mk.inport_vec(ID::A, width);
		vector<পূর্ণসংখ্যা> B = mk.inport_vec(ID::B, width);
		পূর্ণসংখ্যা Y = mk.bool_গ্রন্থি(মিথ্যা);
		জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		যদি (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		যাও optimize;
	}

	যদি (cell->type == ID($_AOI3_))
	{
		পূর্ণসংখ্যা A = mk.inport(ID::A);
		পূর্ণসংখ্যা B = mk.inport(ID::B);
		পূর্ণসংখ্যা C = mk.inport(ID::C);
		পূর্ণসংখ্যা Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		যাও optimize;
	}

	যদি (cell->type == ID($_OAI3_))
	{
		পূর্ণসংখ্যা A = mk.inport(ID::A);
		পূর্ণসংখ্যা B = mk.inport(ID::B);
		পূর্ণসংখ্যা C = mk.inport(ID::C);
		পূর্ণসংখ্যা Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		যাও optimize;
	}

	যদি (cell->type == ID($_AOI4_))
	{
		পূর্ণসংখ্যা A = mk.inport(ID::A);
		পূর্ণসংখ্যা B = mk.inport(ID::B);
		পূর্ণসংখ্যা C = mk.inport(ID::C);
		পূর্ণসংখ্যা D = mk.inport(ID::D);
		পূর্ণসংখ্যা a_and_b = mk.and_gate(A, B);
		পূর্ণসংখ্যা Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		যাও optimize;
	}

	যদি (cell->type == ID($_OAI4_))
	{
		পূর্ণসংখ্যা A = mk.inport(ID::A);
		পূর্ণসংখ্যা B = mk.inport(ID::B);
		পূর্ণসংখ্যা C = mk.inport(ID::C);
		পূর্ণসংখ্যা D = mk.inport(ID::D);
		পূর্ণসংখ্যা a_or_b = mk.or_gate(A, B);
		পূর্ণসংখ্যা Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		যাও optimize;
	}

	name.clear();
	ফেরত;

optimize:;
	pool<পূর্ণসংখ্যা> used_old_ids;
	vector<Aig_গ্রন্থি> new_nodes;
	dict<পূর্ণসংখ্যা, পূর্ণসংখ্যা> old_to_new_ids;
	old_to_new_ids[-1] = -1;

	জন্য (পূর্ণসংখ্যা i = Get_আকার(nodes)-1; i >= 0; i--) {
		যদি (!nodes[i].outports.empty())
			used_old_ids.ঢোকাও(i);
		যদি (!used_old_ids.গুনতি(i))
			চালু;
		যদি (nodes[i].বাম_parent >= 0)
			used_old_ids.ঢোকাও(nodes[i].বাম_parent);
		যদি (nodes[i].ডান_parent >= 0)
			used_old_ids.ঢোকাও(nodes[i].ডান_parent);
	}

	জন্য (পূর্ণসংখ্যা i = 0; i < Get_আকার(nodes); i++) {
		যদি (!used_old_ids.গুনতি(i))
			চালু;
		nodes[i].বাম_parent = old_to_new_ids.at(nodes[i].বাম_parent);
		nodes[i].ডান_parent = old_to_new_ids.at(nodes[i].ডান_parent);
		old_to_new_ids[i] = Get_আকার(new_nodes);
		new_nodes.ঠেলো_back(nodes[i]);
	}

	new_nodes.swap(nodes);
}

YOSYS_NAMESPACE_END
