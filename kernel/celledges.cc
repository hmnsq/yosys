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

#ಅನ್ತರ್ಭಾವ "kernel/celledges.h"

USING_YOSYS_NAMESPACE
ಖಾಸಗಿ_NAMESPACE_BEGIN

ಶೂನ್ಯ bitwise_unary_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = !cell->type.ಒಳ(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ y_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < y_ಅಗಲ; i++)
	{
		ಆದರೆ (i < a_ಅಗಲ)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, i, -1);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_signed && a_ಅಗಲ > 0)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, a_ಅಗಲ-1, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ bitwise_ದ್ವಿಮಾನ_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));
	ಪೂರ್ಣಾಂಕ y_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));

	ಆದರೆ (cell->type == ID($and) && !is_signed) {
		ಆದರೆ (a_ಅಗಲ > b_ಅಗಲ)
			a_ಅಗಲ = b_ಅಗಲ;
		ಇಲ್ಲದಿದ್ದರೆ
			b_ಅಗಲ = a_ಅಗಲ;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < y_ಅಗಲ; i++)
	{
		ಆದರೆ (i < a_ಅಗಲ)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, i, -1);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_signed && a_ಅಗಲ > 0)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, a_ಅಗಲ-1, ID::Y, i, -1);

		ಆದರೆ (i < b_ಅಗಲ)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, i, ID::Y, i, -1);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_signed && b_ಅಗಲ > 0)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, b_ಅಗಲ-1, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ arith_neg_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ y_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));

	ಆದರೆ (is_signed && a_ಅಗಲ == 1)
		y_ಅಗಲ = std::min(y_ಅಗಲ, 1);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < y_ಅಗಲ; i++)
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k <= i && k < a_ಅಗಲ; k++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, k, ID::Y, i, -1);
}

ಶೂನ್ಯ arith_ದ್ವಿಮಾನ_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));
	ಪೂರ್ಣಾಂಕ y_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));

	ಆದರೆ (!is_signed && cell->type != ID($sub)) {
		ಪೂರ್ಣಾಂಕ ab_ಅಗಲ = std::max(a_ಅಗಲ, b_ಅಗಲ);
		y_ಅಗಲ = std::min(y_ಅಗಲ, ab_ಅಗಲ+1);
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < y_ಅಗಲ; i++)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k <= i; k++)
		{
			ಆದರೆ (k < a_ಅಗಲ)
				db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, k, ID::Y, i, -1);

			ಆದರೆ (k < b_ಅಗಲ)
				db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

ಶೂನ್ಯ reduce_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < a_ಅಗಲ; i++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, 0, -1);
}

ಶೂನ್ಯ logic_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < a_ಅಗಲ; i++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, 0, -1);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < b_ಅಗಲ; i++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, i, ID::Y, 0, -1);
}

ಶೂನ್ಯ concat_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < a_ಅಗಲ; i++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, i, -1);
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < b_ಅಗಲ; i++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, i, ID::Y, a_ಅಗಲ + i, -1);
}

ಶೂನ್ಯ ತುಂಡು_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = cell->getParam(ID::ಸ್ಥಾನಾಂತರ_2).as_int();
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ y_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < y_ಅಗಲ; i++) {
		ಪೂರ್ಣಾಂಕ a_ಬಿಟ್ = ಸ್ಥಾನಾಂತರ + i;
		ಆದರೆ (a_ಬಿಟ್ >= 0 && a_ಬಿಟ್ < a_ಅಗಲ)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, a_ಬಿಟ್, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ compare_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < a_ಅಗಲ; i++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, 0, -1);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < b_ಅಗಲ; i++)
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, i, ID::Y, 0, -1);
}

ಶೂನ್ಯ mux_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));
	ಪೂರ್ಣಾಂಕ s_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::S));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < a_ಅಗಲ; i++)
	{
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, i, -1);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = i; k < b_ಅಗಲ; k += a_ಅಗಲ)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < s_ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::S, k, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ bmux_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ s_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::S));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
	{
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = i; k < a_ಅಗಲ; k += ಅಗಲ)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, k, ID::Y, i, -1);

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < s_ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::S, k, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ demux_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ s_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::S));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ಅಗಲ; i++)
	{
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i % a_ಅಗಲ, ID::Y, i, -1);
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < s_ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::S, k, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ ಸರಿಸು_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ತಾರ್ಕಿಕ is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));
	ಪೂರ್ಣಾಂಕ y_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));

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
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ_capped = min(b_ಅಗಲ, 30);

	ಪೂರ್ಣಾಂಕ b_ಮೇಲಿನ, b_ಕೆಳಗಿನ;
	ಆದರೆ (!is_b_signed) {
		b_ಮೇಲಿನ = (1 << b_ಅಗಲ_capped) - 1;
		b_ಕೆಳಗಿನ = 0;
	} ಇಲ್ಲದಿದ್ದರೆ {
		b_ಮೇಲಿನ = (1 << (b_ಅಗಲ_capped - 1)) - 1;
		b_ಕೆಳಗಿನ = -(1 << (b_ಅಗಲ_capped - 1));
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < y_ಅಗಲ; i++){
		// highest position of Y that can change with the value of B
		ಪೂರ್ಣಾಂಕ b_range_ಊರ್ಧ್ವ = 0;
		// 1 + highest position of A that can be moved to Y[i]
		ಪೂರ್ಣಾಂಕ a_range_ಊರ್ಧ್ವ;
		// lowest position of A that can be moved to Y[i]
		ಪೂರ್ಣಾಂಕ a_range_ಅಧೋ;

		ಆದರೆ (cell->type.ಒಳ(ID($shl), ID($sshl))) {
			b_range_ಊರ್ಧ್ವ = a_ಅಗಲ + b_ಮೇಲಿನ;
			ಆದರೆ (is_signed) b_range_ಊರ್ಧ್ವ -= 1;
			a_range_ಅಧೋ = max(0, i - b_ಮೇಲಿನ);
			a_range_ಊರ್ಧ್ವ = min(i+1, a_ಅಗಲ);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($shr), ID($sshr)) || (cell->type.ಒಳ(ID($ಸರಿಸು), ID($shiftx)) && !is_b_signed)) {
			b_range_ಊರ್ಧ್ವ = a_ಅಗಲ;
			a_range_ಅಧೋ = min(i, a_ಅಗಲ - 1);
			a_range_ಊರ್ಧ್ವ = min(i+1 + b_ಮೇಲಿನ, a_ಅಗಲ);
		} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($ಸರಿಸು), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_ಊರ್ಧ್ವ = a_ಅಗಲ - b_ಕೆಳಗಿನ;
			a_range_ಅಧೋ = max(0, i + b_ಕೆಳಗಿನ);
			ಆದರೆ (is_signed)
				a_range_ಅಧೋ = min(a_range_ಅಧೋ, a_ಅಗಲ - 1);
			a_range_ಊರ್ಧ್ವ = min(i+1 + b_ಮೇಲಿನ, a_ಅಗಲ);
		} ಇಲ್ಲದಿದ್ದರೆ {
			log_assert(ಸುಳ್ಳು && "unreachable");
		}

		ಆದರೆ (i < b_range_ಊರ್ಧ್ವ) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = a_range_ಅಧೋ; k < a_range_ಊರ್ಧ್ವ; k++)
				db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, k, ID::Y, i, -1);
		} ಇಲ್ಲದಿದ್ದರೆ {
			// only influence is through sign extension
			ಆದರೆ (is_signed)
				db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, a_ಅಗಲ - 1, ID::Y, i, -1);
		}

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < b_ಅಗಲ_capped; k++) {
			// left shifts
			ಆದರೆ (cell->type.ಒಳ(ID($shl), ID($sshl))) {
				ಆದರೆ (a_ಅಗಲ == 1 && is_signed) {
					ಪೂರ್ಣಾಂಕ skip = 1 << (k + 1);
					ಪೂರ್ಣಾಂಕ ತಳ = skip -1;
					ಆದರೆ (i % skip != ತಳ && i - a_ಅಗಲ + 2 < 1 << b_ಅಗಲ_capped)
						db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
				} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_signed) {
					ಆದರೆ (i - a_ಅಗಲ + 2 < 1 << b_ಅಗಲ_capped)
						db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಆದರೆ (i - a_ಅಗಲ + 1 < 1 << b_ಅಗಲ_capped)
						db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($shr), ID($sshr)) || (cell->type.ಒಳ(ID($ಸರಿಸು), ID($shiftx)) && !is_b_signed)) {
				ಆದರೆ (is_signed) {
					ತಾರ್ಕಿಕ ಸರಿಸು_ಒಳ_bulk = i < a_ಅಗಲ - 1;
					// can we jump into the zero-padding by toggling B[k]?
					ತಾರ್ಕಿಕ zpad_jump = (((y_ಅಗಲ - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_ಅಗಲ - i) & ~(1 << k)) < (1 << b_ಅಗಲ_capped)));

					ಆದರೆ (ಸರಿಸು_ಒಳ_bulk || (cell->type.ಒಳ(ID($shr), ID($ಸರಿಸು), ID($shiftx)) && zpad_jump))
						db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಆದರೆ (i < a_ಅಗಲ)
						db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($ಸರಿಸು), ID($shiftx)) && is_b_signed) {
				ಆದರೆ (is_signed) {
					ಆದರೆ (k != b_ಅಗಲ_capped - 1) {
						ತಾರ್ಕಿಕ r_ಸರಿಸು_ಒಳ_bulk = i < a_ಅಗಲ - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						ತಾರ್ಕಿಕ r_zpad_jump = (((y_ಅಗಲ - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_ಅಗಲ - i) & ~(1 << k)) <= b_ಮೇಲಿನ));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						ತಾರ್ಕಿಕ l = a_ಅಗಲ - 2 - i >= b_ಕೆಳಗಿನ;
						ಆದರೆ (a_ಅಗಲ == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_ಅಗಲ) & ((1 << (k + 1)) - 1)) != 0;
						}
						ಆದರೆ (r_ಸರಿಸು_ಒಳ_bulk || r_zpad_jump || l)
							db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
					} ಇಲ್ಲದಿದ್ದರೆ {
						ಆದರೆ (y_ಅಗಲ - i <= b_ಮೇಲಿನ || a_ಅಗಲ - 2 - i >= b_ಕೆಳಗಿನ)
							db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
					}
				} ಇಲ್ಲದಿದ್ದರೆ {
					ಆದರೆ (a_ಅಗಲ - 1 - i >= b_ಕೆಳಗಿನ)
						db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, k, ID::Y, i, -1);
				}
			} ಇಲ್ಲದಿದ್ದರೆ {
				log_assert(ಸುಳ್ಳು && "unreachable");
			}
		}
	}
}

ಶೂನ್ಯ packed_mem_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	ಪೂರ್ಣಾಂಕ n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	ಪೂರ್ಣಾಂಕ abits = cell->getParam(ID::ABITS).as_int();
	ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->getParam(ID::ಅಗಲ_2).as_int();

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_rd_ports; i++) {
		ಆದರೆ (rd_clk_enable[i] != State::S0) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
				db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::RD_ARST, i, ID::RD_ದತ್ತಾಂಶ, i * ಅಗಲ + k, -1);
			ಮುಂದುವರಿ;
		}

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < abits; j++)
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
				db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_ದತ್ತಾಂಶ, i * ಅಗಲ + k, -1);
	}
}

ಶೂನ್ಯ memrd_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.ಒಳ(ID($memrd), ID($memrd_v2)));
	ಪೂರ್ಣಾಂಕ abits = cell->getParam(ID::ABITS).as_int();
	ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->getParam(ID::ಅಗಲ_2).as_int();

	ಆದರೆ (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		ಆದರೆ (cell->type == ID($memrd_v2)) {
			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
				db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::ARST, 0, ID::ದತ್ತಾಂಶ_2, k, -1);
		}
		ಹಿಂತಿರುಗಿ;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ j = 0; j < abits; j++)
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::ADDR, j, ID::ದತ್ತಾಂಶ_2, k, -1);
}

ಶೂನ್ಯ mem_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಆದರೆ (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cell->type.ಒಳ(ID($memwr), ID($memwr_v2), ID($meminit)))
		ಹಿಂತಿರುಗಿ; /* no edges here */
	ಇಲ್ಲದಿದ್ದರೆ
		log_abort();
}

ಶೂನ್ಯ ff_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ = cell->get_ಪೋರ್ಟ್(ID::Q).ಗಾತ್ರ_2();

	ಆದರೆ (cell->type.ಒಳ(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++) {
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::D, k, ID::Q, k, -1);
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::CLR))
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::CLR, 0, ID::Q, k, -1);
	ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::ಗಣ_2))
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::ಗಣ_2, 0, ID::Q, k, -1);
	ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::ALOAD))
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::ALOAD, 0, ID::Q, k, -1);
	ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::AD))
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::AD, k, ID::Q, k, -1);
	ಆದರೆ (cell->has_ಪೋರ್ಟ್(ID::ARST))
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ k = 0; k < ಅಗಲ; k++)
			db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::ARST, 0, ID::Q, k, -1);
}

ಶೂನ್ಯ ತುಂಬಿದ_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	std::ಸದಿಶ<RTLIL::IdString> ಒಳಹರಿವು_ports;
	std::ಸದಿಶ<RTLIL::IdString> output_ports;

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &conn : cell->connections())
	{
		RTLIL::IdString ಪೋರ್ಟ್ = conn.ಮೊದಲ;
		RTLIL::ಪೋರ್ಟ್_Dir dir = cell->ಪೋರ್ಟ್_dir(ಪೋರ್ಟ್);
		ಆದರೆ (cell->ಒಳಹರಿವು(ಪೋರ್ಟ್) || dir == RTLIL::ಪೋರ್ಟ್_Dir::PD_INOUT)
			ಒಳಹರಿವು_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್);
		ಆದರೆ (cell->output(ಪೋರ್ಟ್) || dir == RTLIL::ಪೋರ್ಟ್_Dir::PD_INOUT)
			output_ports.ತಳ್ಳು_ಹಿಂಭಾಗ(ಪೋರ್ಟ್);
	}

	ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಹೊರಹೋಗು_ಪೋರ್ಟ್ : output_ports)
	{
		ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ಹೊರಹೋಗು_ಪೋರ್ಟ್));
		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_ಬಿಟ್ = 0; ಹೊರಹೋಗು_ಬಿಟ್ < ಹೊರಹೋಗು_ಅಗಲ; ಹೊರಹೋಗು_ಬಿಟ್++)
		{
			ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ ಒಳ_ಪೋರ್ಟ್ : ಒಳಹರಿವು_ports)
			{
				ಪೂರ್ಣಾಂಕ ಒಳ_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ಒಳ_ಪೋರ್ಟ್));
				ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಒಳ_ಬಿಟ್ = 0; ಒಳ_ಬಿಟ್ < ಒಳ_ಅಗಲ; ಒಳ_ಬಿಟ್++)
					db->ಕೂಡಿಸು_ಅಂಚು(cell, ಒಳ_ಪೋರ್ಟ್, ಒಳ_ಬಿಟ್, ಹೊರಹೋಗು_ಪೋರ್ಟ್, ಹೊರಹೋಗು_ಬಿಟ್, -1);
			}
		}
	}
}

ಶೂನ್ಯ bweqx_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));
	ಪೂರ್ಣಾಂಕ max_ಅಗಲ = std::min(ಅಗಲ, std::min(a_ಅಗಲ, b_ಅಗಲ));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < max_ಅಗಲ; i++) {
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, i, -1);
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, i, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ bwmux_op(Abstract_Cell_ಅಂಚುಗಳು_Database *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಂಕ ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::Y));
	ಪೂರ್ಣಾಂಕ a_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::A));
	ಪೂರ್ಣಾಂಕ b_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::B));
	ಪೂರ್ಣಾಂಕ s_ಅಗಲ = Get_ಗಾತ್ರ(cell->get_ಪೋರ್ಟ್(ID::S));
	ಪೂರ್ಣಾಂಕ max_ಅಗಲ = std::min(ಅಗಲ, std::min(a_ಅಗಲ, std::min(b_ಅಗಲ, s_ಅಗಲ)));

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < max_ಅಗಲ; i++) {
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::A, i, ID::Y, i, -1);
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::B, i, ID::Y, i, -1);
		db->ಕೂಡಿಸು_ಅಂಚು(cell, ID::S, i, ID::Y, i, -1);
	}
}

ಖಾಸಗಿ_NAMESPACE_ಕೊನೆ

ತಾರ್ಕಿಕ YOSYS_NAMESPACE_ಪೂರ್ವಪ್ರತ್ಯಯ Abstract_Cell_ಅಂಚುಗಳು_Database::ಕೂಡಿಸು_ಅಂಚುಗಳು_from_cell(RTLIL::Cell *cell)
{
	ಆದರೆ (cell->type.ಒಳ(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ದ್ವಿಮಾನ_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($neg)) {
		arith_neg_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($ಕೂಡಿಸು), ID($sub))) {
		arith_ದ್ವಿಮಾನ_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($logic_and), ID($logic_or))) {
		logic_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($ತುಂಡು)) {
		ತುಂಡು_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($concat)) {
		concat_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($ಸರಿಸು), ID($shiftx))) {
		ಸರಿಸು_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($mux), ID($pmux))) {
		mux_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($bmux)) {
		bmux_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($demux)) {
		demux_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($bweqx)) {
		bweqx_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type == ID($bwmux)) {
		bwmux_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->is_builtin_ff()) {
		ff_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($ಘಾತ))) {
		ತುಂಬಿದ_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		ತುಂಬಿದ_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಆದರೆ (cell->type.ಒಳ(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		ತುಂಬಿದ_op(ಇದು, cell);
		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	ಆದರೆ (cell->type.ಒಳ(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($ಜೀವಂತ), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		ಹಿಂತಿರುಗಿ ನಿಜ; // no-op: these have either no inputs or no outputs

	ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
}
