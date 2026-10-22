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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/cellaigs.h"

YOSYS_NAMESPACE_BEGIN

Aig_ਗੰਢ::Aig_ਗੰਢ()
{
	portbit = -1;
	inverter = ਝੂਠ;
	ਖੱਬਾ_ਮਾਪਾ = -1;
	ਸੱਜਾ_ਮਾਪਾ = -1;
}

ਤਾਰਕਿਕ Aig_ਗੰਢ::ਸੰਚਾਲਕ==(ਅਚਲ Aig_ਗੰਢ &other) ਅਚਲ
{
	ਜੇ (portname != other.portname) ਵਾਪਸ ਝੂਠ;
	ਜੇ (portbit != other.portbit) ਵਾਪਸ ਝੂਠ;
	ਜੇ (inverter != other.inverter) ਵਾਪਸ ਝੂਠ;
	ਜੇ (ਖੱਬਾ_ਮਾਪਾ != other.ਖੱਬਾ_ਮਾਪਾ) ਵਾਪਸ ਝੂਠ;
	ਜੇ (ਸੱਜਾ_ਮਾਪਾ != other.ਸੱਜਾ_ਮਾਪਾ) ਵਾਪਸ ਝੂਠ;
	ਵਾਪਸ ਸੱਚ;
}

Hasher Aig_ਗੰਢ::hash_into(Hasher h) ਅਚਲ
{
	h.eat(portname);
	h.eat(portbit);
	h.eat(inverter);
	h.eat(ਖੱਬਾ_ਮਾਪਾ);
	h.eat(ਸੱਜਾ_ਮਾਪਾ);
	ਵਾਪਸ h;
}

ਤਾਰਕਿਕ Aig::ਸੰਚਾਲਕ==(ਅਚਲ Aig &other) ਅਚਲ
{
	ਵਾਪਸ name == other.name;
}

Hasher Aig::hash_into(Hasher h) ਅਚਲ
{
	h.eat(name);
	ਵਾਪਸ h;
}

ਬਣਤਰ AigMaker
{
	Aig *aig;
	Cell *cell;
	idict<Aig_ਗੰਢ> aig_indices;

	AigMaker(Aig *aig, Cell *cell) : aig(aig), cell(cell) {}

	ਪੂਰਨ node2index(ਅਚਲ Aig_ਗੰਢ &ਗੰਢ)
	{
		ਜੇ (ਗੰਢ.ਖੱਬਾ_ਮਾਪਾ > ਗੰਢ.ਸੱਜਾ_ਮਾਪਾ) {
			Aig_ਗੰਢ n(ਗੰਢ);
			std::ਅਦਲਾ_ਬਦਲੀ(n.ਖੱਬਾ_ਮਾਪਾ, n.ਸੱਜਾ_ਮਾਪਾ);
			ਵਾਪਸ node2index(n);
		}

		ਜੇ (!aig_indices.ਗਿਣਤੀ_2(ਗੰਢ)) {
			aig_indices.expect(ਗੰਢ, Get_ਮਾਪ(aig->nodes));
			aig->nodes.ਧੱਕੋ_ਪਿੱਠ(ਗੰਢ);
		}

		ਵਾਪਸ aig_indices.at(ਗੰਢ);
	}

	ਪੂਰਨ bool_ਗੰਢ(ਤਾਰਕਿਕ ਮੁੱਲ)
	{
		Aig_ਗੰਢ ਗੰਢ;
		ਗੰਢ.inverter = ਮੁੱਲ;
		ਵਾਪਸ node2index(ਗੰਢ);
	}

	ਪੂਰਨ inport(IdString portname, ਪੂਰਨ portbit = 0, ਤਾਰਕਿਕ inverter = ਝੂਠ)
	{
		ਜੇ (portbit >= Get_ਮਾਪ(cell->get_ਪੋਰਟ(portname))) {
			IdString signed_param = cell->twines().ਲੱਭੋ(
				cell->twines().str(portname) + "_SIGNED");
		ਜੇ (signed_param != IdString::Null && cell->parameters.ਗਿਣਤੀ_2(signed_param) && cell->getParam(signed_param).as_bool())
				ਵਾਪਸ inport(portname, Get_ਮਾਪ(cell->get_ਪੋਰਟ(portname))-1, inverter);
			ਵਾਪਸ bool_ਗੰਢ(inverter);
		}

		Aig_ਗੰਢ ਗੰਢ;
		ਗੰਢ.portname = portname;
		ਗੰਢ.portbit = portbit;
		ਗੰਢ.inverter = inverter;
		ਵਾਪਸ node2index(ਗੰਢ);
	}

	ਸਦਿਸ਼<ਪੂਰਨ> inport_vec(IdString portname, ਪੂਰਨ ਚੌੜਾਈ)
	{
		ਸਦਿਸ਼<ਪੂਰਨ> vec;
		ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
			vec.ਧੱਕੋ_ਪਿੱਠ(inport(portname, i));
		ਵਾਪਸ vec;
	}

	ਪੂਰਨ not_inport(IdString portname, ਪੂਰਨ portbit = 0)
	{
		ਵਾਪਸ inport(portname, portbit, ਸੱਚ);
	}

	ਪੂਰਨ not_gate(ਪੂਰਨ A)
	{
		Aig_ਗੰਢ ਗੰਢ(aig_indices[A]);
		ਗੰਢ.outports.clear();
		ਗੰਢ.inverter = !ਗੰਢ.inverter;
		ਵਾਪਸ node2index(ਗੰਢ);
	}

	ਪੂਰਨ and_gate(ਪੂਰਨ A, ਪੂਰਨ B, ਤਾਰਕਿਕ inverter = ਝੂਠ)
	{
		ਜੇ (A == B)
			ਵਾਪਸ inverter ? not_gate(A) : A;

		ਅਚਲ Aig_ਗੰਢ &nA = aig_indices[A];
		ਅਚਲ Aig_ਗੰਢ &nB = aig_indices[B];

		Aig_ਗੰਢ nB_inv(nB);
		nB_inv.inverter = !nB_inv.inverter;

		ਜੇ (nA == nB_inv)
			ਵਾਪਸ bool_ਗੰਢ(inverter);

		ਤਾਰਕਿਕ nA_bool = nA.portbit < 0 && nA.ਖੱਬਾ_ਮਾਪਾ < 0 && nA.ਸੱਜਾ_ਮਾਪਾ < 0;
		ਤਾਰਕਿਕ nB_bool = nB.portbit < 0 && nB.ਖੱਬਾ_ਮਾਪਾ < 0 && nB.ਸੱਜਾ_ਮਾਪਾ < 0;

		ਜੇ (nA_bool && nB_bool) {
			ਤਾਰਕਿਕ bA = nA.inverter;
			ਤਾਰਕਿਕ bB = nB.inverter;
			ਵਾਪਸ bool_ਗੰਢ(inverter != (bA && bB));
		}

		ਜੇ (nA_bool) {
			ਤਾਰਕਿਕ bA = nA.inverter;
			ਜੇ (inverter)
				ਵਾਪਸ bA ? not_gate(B) : bool_ਗੰਢ(ਸੱਚ);
			ਵਾਪਸ bA ? B : bool_ਗੰਢ(ਝੂਠ);
		}

		ਜੇ (nB_bool) {
			ਤਾਰਕਿਕ bB = nB.inverter;
			ਜੇ (inverter)
				ਵਾਪਸ bB ? not_gate(A) : bool_ਗੰਢ(ਸੱਚ);
			ਵਾਪਸ bB ? A : bool_ਗੰਢ(ਝੂਠ);
		}

		Aig_ਗੰਢ ਗੰਢ;
		ਗੰਢ.inverter = inverter;
		ਗੰਢ.ਖੱਬਾ_ਮਾਪਾ = A;
		ਗੰਢ.ਸੱਜਾ_ਮਾਪਾ = B;
		ਵਾਪਸ node2index(ਗੰਢ);
	}

	ਪੂਰਨ nand_gate(ਪੂਰਨ A, ਪੂਰਨ B)
	{
		ਵਾਪਸ and_gate(A, B, ਸੱਚ);
	}

	ਪੂਰਨ or_gate(ਪੂਰਨ A, ਪੂਰਨ B)
	{
		ਪੂਰਨ not_a = not_gate(A);
		ਪੂਰਨ not_b = not_gate(B);
		ਵਾਪਸ nand_gate(not_a, not_b);
	}

	ਪੂਰਨ nor_gate(ਪੂਰਨ A, ਪੂਰਨ B)
	{
		ਪੂਰਨ not_a = not_gate(A);
		ਪੂਰਨ not_b = not_gate(B);
		ਵਾਪਸ and_gate(not_a, not_b);
	}

	ਪੂਰਨ xor_gate(ਪੂਰਨ A, ਪੂਰਨ B)
	{
		ਪੂਰਨ a_and_b = and_gate(A, B);
		ਪੂਰਨ a_nor_b = nor_gate(A, B);
		ਵਾਪਸ nor_gate(a_and_b, a_nor_b);
	}

	ਪੂਰਨ xnor_gate(ਪੂਰਨ A, ਪੂਰਨ B)
	{
		ਪੂਰਨ a_and_b = and_gate(A, B);
		ਪੂਰਨ a_nor_b = nor_gate(A, B);
		ਵਾਪਸ or_gate(a_and_b, a_nor_b);
	}

	ਪੂਰਨ andnot_gate(ਪੂਰਨ A, ਪੂਰਨ B)
	{
		ਪੂਰਨ not_b = not_gate(B);
		ਵਾਪਸ and_gate(A, not_b);
	}

	ਪੂਰਨ ornot_gate(ਪੂਰਨ A, ਪੂਰਨ B)
	{
		ਪੂਰਨ not_b = not_gate(B);
		ਵਾਪਸ or_gate(A, not_b);
	}

	ਪੂਰਨ mux_gate(ਪੂਰਨ A, ਪੂਰਨ B, ਪੂਰਨ S)
	{
		ਪੂਰਨ not_s = not_gate(S);
		ਪੂਰਨ a_ਸਰਗਰਮ = and_gate(A, not_s);
		ਪੂਰਨ b_ਸਰਗਰਮ = and_gate(B, S);
		ਵਾਪਸ or_gate(a_ਸਰਗਰਮ, b_ਸਰਗਰਮ);
	}

	ਸਦਿਸ਼<ਪੂਰਨ> adder(ਅਚਲ ਸਦਿਸ਼<ਪੂਰਨ> &A, ਅਚਲ ਸਦਿਸ਼<ਪੂਰਨ> &B, ਪੂਰਨ ਹਾਸਲ_ਵਿੱਚ, ਸਦਿਸ਼<ਪੂਰਨ> *X = ਖਾਲੀ_ਸੂਚਕ, ਸਦਿਸ਼<ਪੂਰਨ> *CO = ਖਾਲੀ_ਸੂਚਕ)
	{
		ਸਦਿਸ਼<ਪੂਰਨ> Y(Get_ਮਾਪ(A));
		log_assert(Get_ਮਾਪ(A) == Get_ਮਾਪ(B));
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(A); i++) {
			ਪੂਰਨ a_xor_b = xor_gate(A[i], B[i]);
			ਪੂਰਨ a_or_b = or_gate(A[i], B[i]);
			ਪੂਰਨ a_and_b = and_gate(A[i], B[i]);
			Y[i] = xor_gate(a_xor_b, ਹਾਸਲ_ਵਿੱਚ);
			ਪੂਰਨ tmp = and_gate(a_or_b, ਹਾਸਲ_ਵਿੱਚ);
			ਪੂਰਨ ਹਾਸਲ_ਬਾਹਰਲਾ = or_gate(a_and_b, tmp);
			ਜੇ (X != ਖਾਲੀ_ਸੂਚਕ)
				X->at(i) = a_xor_b;
			ਜੇ (CO != ਖਾਲੀ_ਸੂਚਕ)
				CO->at(i) = ਹਾਸਲ_ਬਾਹਰਲਾ;
			ਹਾਸਲ_ਵਿੱਚ = ਹਾਸਲ_ਬਾਹਰਲਾ;
		}
		ਵਾਪਸ Y;
	}

	ਖਾਲੀ outport(ਪੂਰਨ ਗੰਢ, IdString portname, ਪੂਰਨ portbit = 0)
	{
		ਜੇ (portbit < Get_ਮਾਪ(cell->get_ਪੋਰਟ(portname)))
			aig->nodes.at(ਗੰਢ).outports.ਧੱਕੋ_ਪਿੱਠ(pair<IdString, ਪੂਰਨ>(portname, portbit));
	}

	ਖਾਲੀ outport_bool(ਪੂਰਨ ਗੰਢ, IdString portname)
	{
		outport(ਗੰਢ, portname);
		ਲਈ (ਪੂਰਨ i = 1; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(portname)); i++)
			outport(bool_ਗੰਢ(ਝੂਠ), portname, i);
	}

	ਖਾਲੀ outport_vec(ਅਚਲ ਸਦਿਸ਼<ਪੂਰਨ> &vec, IdString portname)
	{
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(vec); i++)
			outport(vec.at(i), portname, i);
	}
};

Aig::Aig(Cell *cell)
{
	ਜੇ (cell->type.is_ਜਨਤਕ())
		ਵਾਪਸ;

	AigMaker mk(ਇਹ, cell);
	name = cell->type.str();

	string mkname_ਆਖਰੀ;
	ਤਾਰਕਿਕ mkname_a_signed = ਝੂਠ;
	ਤਾਰਕਿਕ mkname_b_signed = ਝੂਠ;
	ਤਾਰਕਿਕ mkname_is_signed = ਝੂਠ;

	cell->parameters.ਛਾਂਟੋ();
	ਲਈ (ਸਵੈਚਾਲਿਤ p : cell->parameters)
	{
		ਜੇ (p.ਪਹਿਲਾ == ID::A_ਚੌੜਾਈ && mkname_a_signed) {
			name = mkname_ਆਖਰੀ + stringf(":%d%c", p.ਦੂਜਾ.as_int(), mkname_is_signed ? 'S' : 'U');
		} ਨਹੀਂਤਾਂ ਜੇ (p.ਪਹਿਲਾ == ID::B_ਚੌੜਾਈ && mkname_b_signed) {
			name = mkname_ਆਖਰੀ + stringf(":%d%c", p.ਦੂਜਾ.as_int(), mkname_is_signed ? 'S' : 'U');
		} ਨਹੀਂਤਾਂ {
			mkname_ਆਖਰੀ = name;
			name += stringf(":%d", p.ਦੂਜਾ.as_int());
		}

		mkname_a_signed = ਝੂਠ;
		mkname_b_signed = ਝੂਠ;
		mkname_is_signed = ਝੂਠ;
		ਜੇ (p.ਪਹਿਲਾ == ID::A_SIGNED) {
			mkname_a_signed = ਸੱਚ;
			mkname_is_signed = p.ਦੂਜਾ.as_bool();
		}
		ਜੇ (p.ਪਹਿਲਾ == ID::B_SIGNED) {
			mkname_b_signed = ਸੱਚ;
			mkname_is_signed = p.ਦੂਜਾ.as_bool();
		}
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($not), ID($_NOT_), ID($pos), ID($buf), ID($_BUF_)))
	{
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y)); i++) {
			ਪੂਰਨ A = mk.inport(ID::A, i);
			ਪੂਰਨ Y = cell->type.ਵਿੱਚ(ID($not), ID($_NOT_)) ? mk.not_gate(A) : A;
			mk.outport(Y, ID::Y, i);
		}
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($and), ID($_AND_), ID($_NAND_), ID($or), ID($_OR_), ID($_NOR_), ID($xor), ID($xnor), ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_)))
	{
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y)); i++) {
			ਪੂਰਨ A = mk.inport(ID::A, i);
			ਪੂਰਨ B = mk.inport(ID::B, i);
			ਪੂਰਨ Y = cell->type.ਵਿੱਚ(ID($and), ID($_AND_))   ? mk.and_gate(A, B) :
			        cell->type.ਵਿੱਚ(ID($_NAND_))            ? mk.nand_gate(A, B) :
			        cell->type.ਵਿੱਚ(ID($or), ID($_OR_))     ? mk.or_gate(A, B) :
			        cell->type.ਵਿੱਚ(ID($_NOR_))             ? mk.nor_gate(A, B) :
			        cell->type.ਵਿੱਚ(ID($xor), ID($_XOR_))   ? mk.xor_gate(A, B) :
			        cell->type.ਵਿੱਚ(ID($xnor), ID($_XNOR_)) ? mk.xnor_gate(A, B) :
			        cell->type.ਵਿੱਚ(ID($_ANDNOT_))          ? mk.andnot_gate(A, B) :
			        cell->type.ਵਿੱਚ(ID($_ORNOT_))           ? mk.ornot_gate(A, B) : -1;
			mk.outport(Y, ID::Y, i);
		}
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($mux), ID($_MUX_), ID($_NMUX_)))
	{
		ਪੂਰਨ S = mk.inport(ID::S);
		ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y)); i++) {
			ਪੂਰਨ A = mk.inport(ID::A, i);
			ਪੂਰਨ B = mk.inport(ID::B, i);
			ਪੂਰਨ Y = mk.mux_gate(A, B, S);
			ਜੇ (cell->type == ID($_NMUX_))
				Y = mk.not_gate(Y);
			mk.outport(Y, ID::Y, i);
		}
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool)))
	{
		ਪੂਰਨ Y = mk.inport(ID::A, 0);
		ਲਈ (ਪੂਰਨ i = 1; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A)); i++) {
			ਪੂਰਨ A = mk.inport(ID::A, i);
			ਜੇ (cell->type == ID($reduce_and))  Y = mk.and_gate(A, Y);
			ਜੇ (cell->type == ID($reduce_or))   Y = mk.or_gate(A, Y);
			ਜੇ (cell->type == ID($reduce_bool)) Y = mk.or_gate(A, Y);
			ਜੇ (cell->type == ID($reduce_xor))  Y = mk.xor_gate(A, Y);
			ਜੇ (cell->type == ID($reduce_xnor)) Y = mk.xor_gate(A, Y);
		}
		ਜੇ (cell->type == ID($reduce_xnor))
			Y = mk.not_gate(Y);
		mk.outport(Y, ID::Y, 0);
		ਲਈ (ਪੂਰਨ i = 1; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y)); i++)
			mk.outport(mk.bool_ਗੰਢ(ਝੂਠ), ID::Y, i);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($logic_not), ID($logic_and), ID($logic_or)))
	{
		ਪੂਰਨ A = mk.inport(ID::A, 0), Y = -1;
		ਲਈ (ਪੂਰਨ i = 1; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A)); i++)
			A = mk.or_gate(mk.inport(ID::A, i), A);
		ਜੇ (cell->type.ਵਿੱਚ(ID($logic_and), ID($logic_or))) {
			ਪੂਰਨ B = mk.inport(ID::B, 0);
			ਲਈ (ਪੂਰਨ i = 1; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B)); i++)
				B = mk.or_gate(mk.inport(ID::B, i), B);
			ਜੇ (cell->type == ID($logic_and)) Y = mk.and_gate(A, B);
			ਜੇ (cell->type == ID($logic_or))  Y = mk.or_gate(A, B);
		} ਨਹੀਂਤਾਂ {
			ਜੇ (cell->type == ID($logic_not)) Y = mk.not_gate(A);
		}
		mk.outport_bool(Y, ID::Y);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($ਜੋੜੋ), ID($sub)))
	{
		ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));
		ਸਦਿਸ਼<ਪੂਰਨ> A = mk.inport_vec(ID::A, ਚੌੜਾਈ);
		ਸਦਿਸ਼<ਪੂਰਨ> B = mk.inport_vec(ID::B, ਚੌੜਾਈ);
		ਪੂਰਨ ਹਾਸਲ = mk.bool_ਗੰਢ(ਝੂਠ);
		ਜੇ (cell->type == ID($sub)) {
			ਲਈ (ਸਵੈਚਾਲਿਤ &n : B)
				n = mk.not_gate(n);
			ਹਾਸਲ = mk.not_gate(ਹਾਸਲ);
		}
		ਸਦਿਸ਼<ਪੂਰਨ> Y = mk.adder(A, B, ਹਾਸਲ);
		mk.outport_vec(Y, ID::Y);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($lt), ID($gt), ID($le), ID($ge)))
	{
		ਪੂਰਨ ਚੌੜਾਈ = std::max(Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A)),
							 Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B))) + 1;
		ਸਦਿਸ਼<ਪੂਰਨ> A = mk.inport_vec(ID::A, ਚੌੜਾਈ);
		ਸਦਿਸ਼<ਪੂਰਨ> B = mk.inport_vec(ID::B, ਚੌੜਾਈ);

		ਜੇ (cell->type.ਵਿੱਚ(ID($gt), ID($ge)))
			std::ਅਦਲਾ_ਬਦਲੀ(A, B);

		ਪੂਰਨ ਹਾਸਲ = mk.bool_ਗੰਢ(!cell->type.ਵਿੱਚ(ID($le), ID($ge)));
		ਲਈ (ਸਵੈਚਾਲਿਤ &n : B)
			n = mk.not_gate(n);
		ਸਦਿਸ਼<ਪੂਰਨ> Y = mk.adder(A, B, ਹਾਸਲ);
		mk.outport(Y.ਪਿੱਠ(), ID::Y);
		ਲਈ (ਪੂਰਨ i = 1; i < Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y)); i++)
			mk.outport(mk.bool_ਗੰਢ(ਝੂਠ), ID::Y, i);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type == ID($alu))
	{
		ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));
		ਸਦਿਸ਼<ਪੂਰਨ> A = mk.inport_vec(ID::A, ਚੌੜਾਈ);
		ਸਦਿਸ਼<ਪੂਰਨ> B = mk.inport_vec(ID::B, ਚੌੜਾਈ);
		ਪੂਰਨ ਹਾਸਲ = mk.inport(ID::CI);
		ਪੂਰਨ binv = mk.inport(ID::BI);
		ਲਈ (ਸਵੈਚਾਲਿਤ &n : B)
			n = mk.xor_gate(n, binv);
		ਸਦਿਸ਼<ਪੂਰਨ> X(ਚੌੜਾਈ), CO(ਚੌੜਾਈ);
		ਸਦਿਸ਼<ਪੂਰਨ> Y = mk.adder(A, B, ਹਾਸਲ, &X, &CO);
		ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
			X[i] = mk.xor_gate(A[i], B[i]);
		mk.outport_vec(Y, ID::Y);
		mk.outport_vec(X, ID::X);
		mk.outport_vec(CO, ID::CO);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($eq), ID($ne)))
	{
		ਪੂਰਨ ਚੌੜਾਈ = max(Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A)), Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B)));
		ਸਦਿਸ਼<ਪੂਰਨ> A = mk.inport_vec(ID::A, ਚੌੜਾਈ);
		ਸਦਿਸ਼<ਪੂਰਨ> B = mk.inport_vec(ID::B, ਚੌੜਾਈ);
		ਪੂਰਨ Y = mk.bool_ਗੰਢ(ਝੂਠ);
		ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
			Y = mk.or_gate(Y, mk.xor_gate(A[i], B[i]));
		ਜੇ (cell->type == ID($eq))
			Y = mk.not_gate(Y);
		mk.outport_bool(Y, ID::Y);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type == ID($_AOI3_))
	{
		ਪੂਰਨ A = mk.inport(ID::A);
		ਪੂਰਨ B = mk.inport(ID::B);
		ਪੂਰਨ C = mk.inport(ID::C);
		ਪੂਰਨ Y = mk.nor_gate(mk.and_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type == ID($_OAI3_))
	{
		ਪੂਰਨ A = mk.inport(ID::A);
		ਪੂਰਨ B = mk.inport(ID::B);
		ਪੂਰਨ C = mk.inport(ID::C);
		ਪੂਰਨ Y = mk.nand_gate(mk.or_gate(A, B), C);
		mk.outport(Y, ID::Y);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type == ID($_AOI4_))
	{
		ਪੂਰਨ A = mk.inport(ID::A);
		ਪੂਰਨ B = mk.inport(ID::B);
		ਪੂਰਨ C = mk.inport(ID::C);
		ਪੂਰਨ D = mk.inport(ID::D);
		ਪੂਰਨ a_and_b = mk.and_gate(A, B);
		ਪੂਰਨ Y = mk.nor_gate(a_and_b, mk.and_gate(C, D));
		mk.outport(Y, ID::Y);
		ਜਾਓ optimize;
	}

	ਜੇ (cell->type == ID($_OAI4_))
	{
		ਪੂਰਨ A = mk.inport(ID::A);
		ਪੂਰਨ B = mk.inport(ID::B);
		ਪੂਰਨ C = mk.inport(ID::C);
		ਪੂਰਨ D = mk.inport(ID::D);
		ਪੂਰਨ a_or_b = mk.or_gate(A, B);
		ਪੂਰਨ Y = mk.nand_gate(a_or_b, mk.or_gate(C, D));
		mk.outport(Y, ID::Y);
		ਜਾਓ optimize;
	}

	name.clear();
	ਵਾਪਸ;

optimize:;
	pool<ਪੂਰਨ> ਵਰਤਿਆ_old_ids;
	ਸਦਿਸ਼<Aig_ਗੰਢ> new_nodes;
	dict<ਪੂਰਨ, ਪੂਰਨ> old_ਮੰਜ਼ਿਲ_new_ids;
	old_ਮੰਜ਼ਿਲ_new_ids[-1] = -1;

	ਲਈ (ਪੂਰਨ i = Get_ਮਾਪ(nodes)-1; i >= 0; i--) {
		ਜੇ (!nodes[i].outports.ਖਾਲੀ_ਥਾਵਾਂ())
			ਵਰਤਿਆ_old_ids.ਪਾਓ(i);
		ਜੇ (!ਵਰਤਿਆ_old_ids.ਗਿਣਤੀ_2(i))
			ਜਾਰੀ;
		ਜੇ (nodes[i].ਖੱਬਾ_ਮਾਪਾ >= 0)
			ਵਰਤਿਆ_old_ids.ਪਾਓ(nodes[i].ਖੱਬਾ_ਮਾਪਾ);
		ਜੇ (nodes[i].ਸੱਜਾ_ਮਾਪਾ >= 0)
			ਵਰਤਿਆ_old_ids.ਪਾਓ(nodes[i].ਸੱਜਾ_ਮਾਪਾ);
	}

	ਲਈ (ਪੂਰਨ i = 0; i < Get_ਮਾਪ(nodes); i++) {
		ਜੇ (!ਵਰਤਿਆ_old_ids.ਗਿਣਤੀ_2(i))
			ਜਾਰੀ;
		nodes[i].ਖੱਬਾ_ਮਾਪਾ = old_ਮੰਜ਼ਿਲ_new_ids.at(nodes[i].ਖੱਬਾ_ਮਾਪਾ);
		nodes[i].ਸੱਜਾ_ਮਾਪਾ = old_ਮੰਜ਼ਿਲ_new_ids.at(nodes[i].ਸੱਜਾ_ਮਾਪਾ);
		old_ਮੰਜ਼ਿਲ_new_ids[i] = Get_ਮਾਪ(new_nodes);
		new_nodes.ਧੱਕੋ_ਪਿੱਠ(nodes[i]);
	}

	new_nodes.ਅਦਲਾ_ਬਦਲੀ(nodes);
}

YOSYS_NAMESPACE_ਸਿਰਾ
