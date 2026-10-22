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

#ਅਨ੍ਤਰ੍ਭਾਵ "kernel/celledges.h"

USING_YOSYS_NAMESPACE
ਨਿੱਜੀ_NAMESPACE_BEGIN

ਖਾਲੀ bitwise_unary_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਤਾਰਕਿਕ is_signed = !cell->type.ਵਿੱਚ(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ y_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));

	ਲਈ (ਪੂਰਨ i = 0; i < y_ਚੌੜਾਈ; i++)
	{
		ਜੇ (i < a_ਚੌੜਾਈ)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, i, -1);
		ਨਹੀਂਤਾਂ ਜੇ (is_signed && a_ਚੌੜਾਈ > 0)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, a_ਚੌੜਾਈ-1, ID::Y, i, -1);
	}
}

ਖਾਲੀ bitwise_ਦੁਆਧਾਰੀ_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਤਾਰਕਿਕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));
	ਪੂਰਨ y_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));

	ਜੇ (cell->type == ID($and) && !is_signed) {
		ਜੇ (a_ਚੌੜਾਈ > b_ਚੌੜਾਈ)
			a_ਚੌੜਾਈ = b_ਚੌੜਾਈ;
		ਨਹੀਂਤਾਂ
			b_ਚੌੜਾਈ = a_ਚੌੜਾਈ;
	}

	ਲਈ (ਪੂਰਨ i = 0; i < y_ਚੌੜਾਈ; i++)
	{
		ਜੇ (i < a_ਚੌੜਾਈ)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, i, -1);
		ਨਹੀਂਤਾਂ ਜੇ (is_signed && a_ਚੌੜਾਈ > 0)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, a_ਚੌੜਾਈ-1, ID::Y, i, -1);

		ਜੇ (i < b_ਚੌੜਾਈ)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, i, ID::Y, i, -1);
		ਨਹੀਂਤਾਂ ਜੇ (is_signed && b_ਚੌੜਾਈ > 0)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, b_ਚੌੜਾਈ-1, ID::Y, i, -1);
	}
}

ਖਾਲੀ arith_neg_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਤਾਰਕਿਕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ y_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));

	ਜੇ (is_signed && a_ਚੌੜਾਈ == 1)
		y_ਚੌੜਾਈ = std::min(y_ਚੌੜਾਈ, 1);

	ਲਈ (ਪੂਰਨ i = 0; i < y_ਚੌੜਾਈ; i++)
	ਲਈ (ਪੂਰਨ k = 0; k <= i && k < a_ਚੌੜਾਈ; k++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, k, ID::Y, i, -1);
}

ਖਾਲੀ arith_ਦੁਆਧਾਰੀ_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਤਾਰਕਿਕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));
	ਪੂਰਨ y_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));

	ਜੇ (!is_signed && cell->type != ID($sub)) {
		ਪੂਰਨ ab_ਚੌੜਾਈ = std::max(a_ਚੌੜਾਈ, b_ਚੌੜਾਈ);
		y_ਚੌੜਾਈ = std::min(y_ਚੌੜਾਈ, ab_ਚੌੜਾਈ+1);
	}

	ਲਈ (ਪੂਰਨ i = 0; i < y_ਚੌੜਾਈ; i++)
	{
		ਲਈ (ਪੂਰਨ k = 0; k <= i; k++)
		{
			ਜੇ (k < a_ਚੌੜਾਈ)
				db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, k, ID::Y, i, -1);

			ਜੇ (k < b_ਚੌੜਾਈ)
				db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

ਖਾਲੀ reduce_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));

	ਲਈ (ਪੂਰਨ i = 0; i < a_ਚੌੜਾਈ; i++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, 0, -1);
}

ਖਾਲੀ logic_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));

	ਲਈ (ਪੂਰਨ i = 0; i < a_ਚੌੜਾਈ; i++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, 0, -1);
	ਲਈ (ਪੂਰਨ i = 0; i < b_ਚੌੜਾਈ; i++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, i, ID::Y, 0, -1);
}

ਖਾਲੀ concat_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));

	ਲਈ (ਪੂਰਨ i = 0; i < a_ਚੌੜਾਈ; i++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, i, -1);
	ਲਈ (ਪੂਰਨ i = 0; i < b_ਚੌੜਾਈ; i++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, i, ID::Y, a_ਚੌੜਾਈ + i, -1);
}

ਖਾਲੀ ਟੁਕੜਾ_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ ਵਿੱਥ = cell->getParam(ID::ਵਿੱਥ_2).as_int();
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ y_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));

	ਲਈ (ਪੂਰਨ i = 0; i < y_ਚੌੜਾਈ; i++) {
		ਪੂਰਨ a_ਦੋਆਧਾਰੀ_ਅੰਕ = ਵਿੱਥ + i;
		ਜੇ (a_ਦੋਆਧਾਰੀ_ਅੰਕ >= 0 && a_ਦੋਆਧਾਰੀ_ਅੰਕ < a_ਚੌੜਾਈ)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, a_ਦੋਆਧਾਰੀ_ਅੰਕ, ID::Y, i, -1);
	}
}

ਖਾਲੀ compare_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));

	ਲਈ (ਪੂਰਨ i = 0; i < a_ਚੌੜਾਈ; i++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, 0, -1);

	ਲਈ (ਪੂਰਨ i = 0; i < b_ਚੌੜਾਈ; i++)
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, i, ID::Y, 0, -1);
}

ਖਾਲੀ mux_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));
	ਪੂਰਨ s_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::S));

	ਲਈ (ਪੂਰਨ i = 0; i < a_ਚੌੜਾਈ; i++)
	{
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, i, -1);

		ਲਈ (ਪੂਰਨ k = i; k < b_ਚੌੜਾਈ; k += a_ਚੌੜਾਈ)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);

		ਲਈ (ਪੂਰਨ k = 0; k < s_ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::S, k, ID::Y, i, -1);
	}
}

ਖਾਲੀ bmux_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ s_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::S));

	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
	{
		ਲਈ (ਪੂਰਨ k = i; k < a_ਚੌੜਾਈ; k += ਚੌੜਾਈ)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, k, ID::Y, i, -1);

		ਲਈ (ਪੂਰਨ k = 0; k < s_ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::S, k, ID::Y, i, -1);
	}
}

ਖਾਲੀ demux_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ s_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::S));

	ਲਈ (ਪੂਰਨ i = 0; i < ਚੌੜਾਈ; i++)
	{
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i % a_ਚੌੜਾਈ, ID::Y, i, -1);
		ਲਈ (ਪੂਰਨ k = 0; k < s_ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::S, k, ID::Y, i, -1);
	}
}

ਖਾਲੀ ਖਿਸਕਾਓ_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਤਾਰਕਿਕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ਤਾਰਕਿਕ is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));
	ਪੂਰਨ y_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));

	// Behavior of the different shift cells:
	//
	//  $shl, $sshl -- shifts left by the amount on B port, B always unsigned
	//  $shr, $sshr -- ditto right
	//  $shift, $shiftx -- shifts right by the amount on B port, B optionally signed
	//
	// Sign extension (if A signed):
	//
	//  $shl, $shr, $shift -- only sign-extends up to size of Y, then shifts in zeroes
	//  $sshl, $sshr -- fully sign-extends
	//  $shiftx -- no sign extension
	//
	// Because $shl, $sshl only shift left, and $shl sign-extens up to size of Y, they
	// are effectively the same.

	// the cap below makes sure we don't overflow in the arithmetic further down, though
	// it makes the edge data invalid once a_width approaches the order of 2**30
	// (that ever happening is considered improbable)
	ਪੂਰਨ b_ਚੌੜਾਈ_capped = min(b_ਚੌੜਾਈ, 30);

	ਪੂਰਨ b_ਉੱਚਾ, b_ਨੀਵਾਂ;
	ਜੇ (!is_b_signed) {
		b_ਉੱਚਾ = (1 << b_ਚੌੜਾਈ_capped) - 1;
		b_ਨੀਵਾਂ = 0;
	} ਨਹੀਂਤਾਂ {
		b_ਉੱਚਾ = (1 << (b_ਚੌੜਾਈ_capped - 1)) - 1;
		b_ਨੀਵਾਂ = -(1 << (b_ਚੌੜਾਈ_capped - 1));
	}

	ਲਈ (ਪੂਰਨ i = 0; i < y_ਚੌੜਾਈ; i++){
		// highest position of Y that can change with the value of B
		ਪੂਰਨ b_range_ਉੱਤਲਾ = 0;
		// 1 + highest position of A that can be moved to Y[i]
		ਪੂਰਨ a_range_ਉੱਤਲਾ;
		// lowest position of A that can be moved to Y[i]
		ਪੂਰਨ a_range_ਹੇਠਲਾ;

		ਜੇ (cell->type.ਵਿੱਚ(ID($shl), ID($sshl))) {
			b_range_ਉੱਤਲਾ = a_ਚੌੜਾਈ + b_ਉੱਚਾ;
			ਜੇ (is_signed) b_range_ਉੱਤਲਾ -= 1;
			a_range_ਹੇਠਲਾ = max(0, i - b_ਉੱਚਾ);
			a_range_ਉੱਤਲਾ = min(i+1, a_ਚੌੜਾਈ);
		} ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($shr), ID($sshr)) || (cell->type.ਵਿੱਚ(ID($ਖਿਸਕਾਓ), ID($shiftx)) && !is_b_signed)) {
			b_range_ਉੱਤਲਾ = a_ਚੌੜਾਈ;
			a_range_ਹੇਠਲਾ = min(i, a_ਚੌੜਾਈ - 1);
			a_range_ਉੱਤਲਾ = min(i+1 + b_ਉੱਚਾ, a_ਚੌੜਾਈ);
		} ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($ਖਿਸਕਾਓ), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_ਉੱਤਲਾ = a_ਚੌੜਾਈ - b_ਨੀਵਾਂ;
			a_range_ਹੇਠਲਾ = max(0, i + b_ਨੀਵਾਂ);
			ਜੇ (is_signed)
				a_range_ਹੇਠਲਾ = min(a_range_ਹੇਠਲਾ, a_ਚੌੜਾਈ - 1);
			a_range_ਉੱਤਲਾ = min(i+1 + b_ਉੱਚਾ, a_ਚੌੜਾਈ);
		} ਨਹੀਂਤਾਂ {
			log_assert(ਝੂਠ && "unreachable");
		}

		ਜੇ (i < b_range_ਉੱਤਲਾ) {
			ਲਈ (ਪੂਰਨ k = a_range_ਹੇਠਲਾ; k < a_range_ਉੱਤਲਾ; k++)
				db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, k, ID::Y, i, -1);
		} ਨਹੀਂਤਾਂ {
			// only influence is through sign extension
			ਜੇ (is_signed)
				db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, a_ਚੌੜਾਈ - 1, ID::Y, i, -1);
		}

		ਲਈ (ਪੂਰਨ k = 0; k < b_ਚੌੜਾਈ_capped; k++) {
			// left shifts
			ਜੇ (cell->type.ਵਿੱਚ(ID($shl), ID($sshl))) {
				ਜੇ (a_ਚੌੜਾਈ == 1 && is_signed) {
					ਪੂਰਨ skip = 1 << (k + 1);
					ਪੂਰਨ ਅਧਾਰ = skip -1;
					ਜੇ (i % skip != ਅਧਾਰ && i - a_ਚੌੜਾਈ + 2 < 1 << b_ਚੌੜਾਈ_capped)
						db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
				} ਨਹੀਂਤਾਂ ਜੇ (is_signed) {
					ਜੇ (i - a_ਚੌੜਾਈ + 2 < 1 << b_ਚੌੜਾਈ_capped)
						db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
				} ਨਹੀਂਤਾਂ {
					ਜੇ (i - a_ਚੌੜਾਈ + 1 < 1 << b_ਚੌੜਾਈ_capped)
						db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($shr), ID($sshr)) || (cell->type.ਵਿੱਚ(ID($ਖਿਸਕਾਓ), ID($shiftx)) && !is_b_signed)) {
				ਜੇ (is_signed) {
					ਤਾਰਕਿਕ ਖਿਸਕਾਓ_ਵਿੱਚ_bulk = i < a_ਚੌੜਾਈ - 1;
					// can we jump into the zero-padding by toggling B[k]?
					ਤਾਰਕਿਕ zpad_jump = (((y_ਚੌੜਾਈ - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_ਚੌੜਾਈ - i) & ~(1 << k)) < (1 << b_ਚੌੜਾਈ_capped)));

					ਜੇ (ਖਿਸਕਾਓ_ਵਿੱਚ_bulk || (cell->type.ਵਿੱਚ(ID($shr), ID($ਖਿਸਕਾਓ), ID($shiftx)) && zpad_jump))
						db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
				} ਨਹੀਂਤਾਂ {
					ਜੇ (i < a_ਚੌੜਾਈ)
						db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($ਖਿਸਕਾਓ), ID($shiftx)) && is_b_signed) {
				ਜੇ (is_signed) {
					ਜੇ (k != b_ਚੌੜਾਈ_capped - 1) {
						ਤਾਰਕਿਕ r_ਖਿਸਕਾਓ_ਵਿੱਚ_bulk = i < a_ਚੌੜਾਈ - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						ਤਾਰਕਿਕ r_zpad_jump = (((y_ਚੌੜਾਈ - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_ਚੌੜਾਈ - i) & ~(1 << k)) <= b_ਉੱਚਾ));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						ਤਾਰਕਿਕ l = a_ਚੌੜਾਈ - 2 - i >= b_ਨੀਵਾਂ;
						ਜੇ (a_ਚੌੜਾਈ == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_ਚੌੜਾਈ) & ((1 << (k + 1)) - 1)) != 0;
						}
						ਜੇ (r_ਖਿਸਕਾਓ_ਵਿੱਚ_bulk || r_zpad_jump || l)
							db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
					} ਨਹੀਂਤਾਂ {
						ਜੇ (y_ਚੌੜਾਈ - i <= b_ਉੱਚਾ || a_ਚੌੜਾਈ - 2 - i >= b_ਨੀਵਾਂ)
							db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
					}
				} ਨਹੀਂਤਾਂ {
					ਜੇ (a_ਚੌੜਾਈ - 1 - i >= b_ਨੀਵਾਂ)
						db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, k, ID::Y, i, -1);
				}
			} ਨਹੀਂਤਾਂ {
				log_assert(ਝੂਠ && "unreachable");
			}
		}
	}
}

ਖਾਲੀ packed_mem_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	ਪੂਰਨ n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	ਪੂਰਨ abits = cell->getParam(ID::ABITS).as_int();
	ਪੂਰਨ ਚੌੜਾਈ = cell->getParam(ID::ਚੌੜਾਈ_2).as_int();

	ਲਈ (ਪੂਰਨ i = 0; i < n_rd_ports; i++) {
		ਜੇ (rd_clk_enable[i] != State::S0) {
			ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
				db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::RD_ARST, i, ID::RD_ਅੰਕੜੇ, i * ਚੌੜਾਈ + k, -1);
			ਜਾਰੀ;
		}

		ਲਈ (ਪੂਰਨ j = 0; j < abits; j++)
			ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
				db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_ਅੰਕੜੇ, i * ਚੌੜਾਈ + k, -1);
	}
}

ਖਾਲੀ memrd_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.ਵਿੱਚ(ID($memrd), ID($memrd_v2)));
	ਪੂਰਨ abits = cell->getParam(ID::ABITS).as_int();
	ਪੂਰਨ ਚੌੜਾਈ = cell->getParam(ID::ਚੌੜਾਈ_2).as_int();

	ਜੇ (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		ਜੇ (cell->type == ID($memrd_v2)) {
			ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
				db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::ARST, 0, ID::ਅੰਕੜੇ_2, k, -1);
		}
		ਵਾਪਸ;
	}

	ਲਈ (ਪੂਰਨ j = 0; j < abits; j++)
		ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::ADDR, j, ID::ਅੰਕੜੇ_2, k, -1);
}

ਖਾਲੀ mem_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਜੇ (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	ਨਹੀਂਤਾਂ ਜੇ (cell->type.ਵਿੱਚ(ID($memwr), ID($memwr_v2), ID($meminit)))
		ਵਾਪਸ; /* no edges here */
	ਨਹੀਂਤਾਂ
		log_abort();
}

ਖਾਲੀ ff_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ ਚੌੜਾਈ = cell->get_ਪੋਰਟ(ID::Q).ਮਾਪ();

	ਜੇ (cell->type.ਵਿੱਚ(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++) {
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::D, k, ID::Q, k, -1);
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	ਜੇ (cell->has_ਪੋਰਟ(ID::CLR))
		ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::CLR, 0, ID::Q, k, -1);
	ਜੇ (cell->has_ਪੋਰਟ(ID::ਸੈੱਟ_2))
		ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::ਸੈੱਟ_2, 0, ID::Q, k, -1);
	ਜੇ (cell->has_ਪੋਰਟ(ID::ALOAD))
		ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::ALOAD, 0, ID::Q, k, -1);
	ਜੇ (cell->has_ਪੋਰਟ(ID::AD))
		ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::AD, k, ID::Q, k, -1);
	ਜੇ (cell->has_ਪੋਰਟ(ID::ARST))
		ਲਈ (ਪੂਰਨ k = 0; k < ਚੌੜਾਈ; k++)
			db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::ARST, 0, ID::Q, k, -1);
}

ਖਾਲੀ ਭਰਿਆ_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	std::ਸਦਿਸ਼<RTLIL::IdString> ਨਿਵੇਸ਼_ports;
	std::ਸਦਿਸ਼<RTLIL::IdString> output_ports;

	ਲਈ (ਸਵੈਚਾਲਿਤ &conn : cell->connections())
	{
		RTLIL::IdString ਪੋਰਟ = conn.ਪਹਿਲਾ;
		RTLIL::ਪੋਰਟ_Dir dir = cell->ਪੋਰਟ_dir(ਪੋਰਟ);
		ਜੇ (cell->ਨਿਵੇਸ਼(ਪੋਰਟ) || dir == RTLIL::ਪੋਰਟ_Dir::PD_INOUT)
			ਨਿਵੇਸ਼_ports.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ);
		ਜੇ (cell->output(ਪੋਰਟ) || dir == RTLIL::ਪੋਰਟ_Dir::PD_INOUT)
			output_ports.ਧੱਕੋ_ਪਿੱਠ(ਪੋਰਟ);
	}

	ਲਈ (ਸਵੈਚਾਲਿਤ ਬਾਹਰਲਾ_ਪੋਰਟ : output_ports)
	{
		ਪੂਰਨ ਬਾਹਰਲਾ_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ਬਾਹਰਲਾ_ਪੋਰਟ));
		ਲਈ (ਪੂਰਨ ਬਾਹਰਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ = 0; ਬਾਹਰਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ < ਬਾਹਰਲਾ_ਚੌੜਾਈ; ਬਾਹਰਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ++)
		{
			ਲਈ (ਸਵੈਚਾਲਿਤ ਵਿੱਚ_ਪੋਰਟ : ਨਿਵੇਸ਼_ports)
			{
				ਪੂਰਨ ਵਿੱਚ_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ਵਿੱਚ_ਪੋਰਟ));
				ਲਈ (ਪੂਰਨ ਵਿੱਚ_ਦੋਆਧਾਰੀ_ਅੰਕ = 0; ਵਿੱਚ_ਦੋਆਧਾਰੀ_ਅੰਕ < ਵਿੱਚ_ਚੌੜਾਈ; ਵਿੱਚ_ਦੋਆਧਾਰੀ_ਅੰਕ++)
					db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ਵਿੱਚ_ਪੋਰਟ, ਵਿੱਚ_ਦੋਆਧਾਰੀ_ਅੰਕ, ਬਾਹਰਲਾ_ਪੋਰਟ, ਬਾਹਰਲਾ_ਦੋਆਧਾਰੀ_ਅੰਕ, -1);
			}
		}
	}
}

ਖਾਲੀ bweqx_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));
	ਪੂਰਨ max_ਚੌੜਾਈ = std::min(ਚੌੜਾਈ, std::min(a_ਚੌੜਾਈ, b_ਚੌੜਾਈ));

	ਲਈ (ਪੂਰਨ i = 0; i < max_ਚੌੜਾਈ; i++) {
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, i, -1);
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, i, ID::Y, i, -1);
	}
}

ਖਾਲੀ bwmux_op(Abstract_Cell_ਕਿਨਾਰੇ_Database *db, RTLIL::Cell *cell)
{
	ਪੂਰਨ ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::Y));
	ਪੂਰਨ a_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::A));
	ਪੂਰਨ b_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::B));
	ਪੂਰਨ s_ਚੌੜਾਈ = Get_ਮਾਪ(cell->get_ਪੋਰਟ(ID::S));
	ਪੂਰਨ max_ਚੌੜਾਈ = std::min(ਚੌੜਾਈ, std::min(a_ਚੌੜਾਈ, std::min(b_ਚੌੜਾਈ, s_ਚੌੜਾਈ)));

	ਲਈ (ਪੂਰਨ i = 0; i < max_ਚੌੜਾਈ; i++) {
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::A, i, ID::Y, i, -1);
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::B, i, ID::Y, i, -1);
		db->ਜੋੜੋ_ਕਿਨਾਰਾ(cell, ID::S, i, ID::Y, i, -1);
	}
}

ਨਿੱਜੀ_NAMESPACE_ਸਿਰਾ

ਤਾਰਕਿਕ YOSYS_NAMESPACE_ਅਗੇਤਰ Abstract_Cell_ਕਿਨਾਰੇ_Database::ਜੋੜੋ_ਕਿਨਾਰੇ_from_cell(RTLIL::Cell *cell)
{
	ਜੇ (cell->type.ਵਿੱਚ(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ਦੁਆਧਾਰੀ_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($neg)) {
		arith_neg_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($ਜੋੜੋ), ID($sub))) {
		arith_ਦੁਆਧਾਰੀ_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($logic_and), ID($logic_or))) {
		logic_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($ਟੁਕੜਾ)) {
		ਟੁਕੜਾ_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($concat)) {
		concat_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ਖਿਸਕਾਓ), ID($shiftx))) {
		ਖਿਸਕਾਓ_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($mux), ID($pmux))) {
		mux_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($bmux)) {
		bmux_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($demux)) {
		demux_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($bweqx)) {
		bweqx_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type == ID($bwmux)) {
		bwmux_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->is_builtin_ff()) {
		ff_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ਘਾਤ))) {
		ਭਰਿਆ_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		ਭਰਿਆ_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	ਜੇ (cell->type.ਵਿੱਚ(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		ਭਰਿਆ_op(ਇਹ, cell);
		ਵਾਪਸ ਸੱਚ;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	ਜੇ (cell->type.ਵਿੱਚ(ID($ਪ੍ਰਤਿਜ੍ਞਾ), ID($assume), ID($ਜੀਵਤ), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		ਵਾਪਸ ਸੱਚ; // no-op: these have either no inputs or no outputs

	ਵਾਪਸ ਝੂਠ;
}
