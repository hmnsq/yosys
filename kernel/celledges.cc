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
PRIVATE_NAMESPACE_BEGIN

ಶೂನ್ಯ bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ y_width = GetSize(cell->getPort(ID::Y));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < y_width; i++)
	{
		ಯದಿ (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		ಅನ್ಯಥಾ ಯದಿ (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ bitwise_ದ್ವಿಮಾನ_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));
	ಪೂರ್ಣಾಙ್ಕ y_width = GetSize(cell->getPort(ID::Y));

	ಯದಿ (cell->type == ID($and) && !is_signed) {
		ಯದಿ (a_width > b_width)
			a_width = b_width;
		ಅನ್ಯಥಾ
			b_width = a_width;
	}

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < y_width; i++)
	{
		ಯದಿ (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		ಅನ್ಯಥಾ ಯದಿ (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		ಯದಿ (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		ಅನ್ಯಥಾ ಯದಿ (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ y_width = GetSize(cell->getPort(ID::Y));

	ಯದಿ (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < y_width; i++)
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

ಶೂನ್ಯ arith_ದ್ವಿಮಾನ_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));
	ಪೂರ್ಣಾಙ್ಕ y_width = GetSize(cell->getPort(ID::Y));

	ಯದಿ (!is_signed && cell->type != ID($sub)) {
		ಪೂರ್ಣಾಙ್ಕ ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < y_width; i++)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k <= i; k++)
		{
			ಯದಿ (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			ಯದಿ (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

ಶೂನ್ಯ reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

ಶೂನ್ಯ logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

ಶೂನ್ಯ concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

ಶೂನ್ಯ slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ offset = cell->getParam(ID::OFFSET).as_int();
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ y_width = GetSize(cell->getPort(ID::Y));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < y_width; i++) {
		ಪೂರ್ಣಾಙ್ಕ a_bit = offset + i;
		ಯದಿ (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

ಶೂನ್ಯ mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));
	ಪೂರ್ಣಾಙ್ಕ s_width = GetSize(cell->getPort(ID::S));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ width = GetSize(cell->getPort(ID::Y));
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ s_width = GetSize(cell->getPort(ID::S));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
	{
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ width = GetSize(cell->getPort(ID::Y));
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ s_width = GetSize(cell->getPort(ID::S));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ತಾರ್ಕಿಕ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ತಾರ್ಕಿಕ is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));
	ಪೂರ್ಣಾಙ್ಕ y_width = GetSize(cell->getPort(ID::Y));

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
	ಪೂರ್ಣಾಙ್ಕ b_width_capped = min(b_width, 30);

	ಪೂರ್ಣಾಙ್ಕ b_ಮೇಲಿನ, b_ಕೆಳಗಿನ;
	ಯದಿ (!is_b_signed) {
		b_ಮೇಲಿನ = (1 << b_width_capped) - 1;
		b_ಕೆಳಗಿನ = 0;
	} ಅನ್ಯಥಾ {
		b_ಮೇಲಿನ = (1 << (b_width_capped - 1)) - 1;
		b_ಕೆಳಗಿನ = -(1 << (b_width_capped - 1));
	}

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		ಪೂರ್ಣಾಙ್ಕ b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		ಪೂರ್ಣಾಙ್ಕ a_range_upper;
		// lowest position of A that can be moved to Y[i]
		ಪೂರ್ಣಾಙ್ಕ a_range_lower;

		ಯದಿ (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_ಮೇಲಿನ;
			ಯದಿ (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_ಮೇಲಿನ);
			a_range_upper = min(i+1, a_width);
		} ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_ಮೇಲಿನ, a_width);
		} ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_ಕೆಳಗಿನ;
			a_range_lower = max(0, i + b_ಕೆಳಗಿನ);
			ಯದಿ (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_ಮೇಲಿನ, a_width);
		} ಅನ್ಯಥಾ {
			log_assert(ಅಸತ್ಯ && "unreachable");
		}

		ಯದಿ (i < b_range_upper) {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} ಅನ್ಯಥಾ {
			// only influence is through sign extension
			ಯದಿ (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < b_width_capped; k++) {
			// left shifts
			ಯದಿ (cell->type.in(ID($shl), ID($sshl))) {
				ಯದಿ (a_width == 1 && is_signed) {
					ಪೂರ್ಣಾಙ್ಕ skip = 1 << (k + 1);
					ಪೂರ್ಣಾಙ್ಕ base = skip -1;
					ಯದಿ (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} ಅನ್ಯಥಾ ಯದಿ (is_signed) {
					ಯದಿ (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} ಅನ್ಯಥಾ {
					ಯದಿ (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				ಯದಿ (is_signed) {
					ತಾರ್ಕಿಕ shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					ತಾರ್ಕಿಕ zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					ಯದಿ (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} ಅನ್ಯಥಾ {
					ಯದಿ (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				ಯದಿ (is_signed) {
					ಯದಿ (k != b_width_capped - 1) {
						ತಾರ್ಕಿಕ r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						ತಾರ್ಕಿಕ r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_ಮೇಲಿನ));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						ತಾರ್ಕಿಕ l = a_width - 2 - i >= b_ಕೆಳಗಿನ;
						ಯದಿ (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						ಯದಿ (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} ಅನ್ಯಥಾ {
						ಯದಿ (y_width - i <= b_ಮೇಲಿನ || a_width - 2 - i >= b_ಕೆಳಗಿನ)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} ಅನ್ಯಥಾ {
					ಯದಿ (a_width - 1 - i >= b_ಕೆಳಗಿನ)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} ಅನ್ಯಥಾ {
				log_assert(ಅಸತ್ಯ && "unreachable");
			}
		}
	}
}

ಶೂನ್ಯ packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	ಪೂರ್ಣಾಙ್ಕ n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	ಪೂರ್ಣಾಙ್ಕ abits = cell->getParam(ID::ABITS).as_int();
	ಪೂರ್ಣಾಙ್ಕ width = cell->getParam(ID::WIDTH).as_int();

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_rd_ports; i++) {
		ಯದಿ (rd_clk_enable[i] != State::S0) {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			ಅನುವರ್ತ;
		}

		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < abits; j++)
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

ಶೂನ್ಯ memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	ಪೂರ್ಣಾಙ್ಕ abits = cell->getParam(ID::ABITS).as_int();
	ಪೂರ್ಣಾಙ್ಕ width = cell->getParam(ID::WIDTH).as_int();

	ಯದಿ (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		ಯದಿ (cell->type == ID($memrd_v2)) {
			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		ಪ್ರತ್ಯಾಗಮ;
	}

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ j = 0; j < abits; j++)
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

ಶೂನ್ಯ mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಯದಿ (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	ಅನ್ಯಥಾ ಯದಿ (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		ಪ್ರತ್ಯಾಗಮ; /* no edges here */
	ಅನ್ಯಥಾ
		log_abort();
}

ಶೂನ್ಯ ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ width = cell->getPort(ID::Q).size();

	ಯದಿ (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	ಯದಿ (cell->hasPort(ID::CLR))
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	ಯದಿ (cell->hasPort(ID::SET))
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	ಯದಿ (cell->hasPort(ID::ALOAD))
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	ಯದಿ (cell->hasPort(ID::AD))
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	ಯದಿ (cell->hasPort(ID::ARST))
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

ಶೂನ್ಯ full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> ಒಳಹರಿವು_ports;
	std::vector<RTLIL::IdString> output_ports;

	ಪ್ರತಿ (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		ಯದಿ (cell->ಒಳಹರಿವು(port) || dir == RTLIL::PortDir::PD_INOUT)
			ಒಳಹರಿವು_ports.ತಳ್ಳು_back(port);
		ಯದಿ (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.ತಳ್ಳು_back(port);
	}

	ಪ್ರತಿ (auto out_port : output_ports)
	{
		ಪೂರ್ಣಾಙ್ಕ out_width = GetSize(cell->getPort(out_port));
		ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ out_bit = 0; out_bit < out_width; out_bit++)
		{
			ಪ್ರತಿ (auto in_port : ಒಳಹರಿವು_ports)
			{
				ಪೂರ್ಣಾಙ್ಕ in_width = GetSize(cell->getPort(in_port));
				ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

ಶೂನ್ಯ bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ width = GetSize(cell->getPort(ID::Y));
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));
	ಪೂರ್ಣಾಙ್ಕ max_width = std::min(width, std::min(a_width, b_width));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

ಶೂನ್ಯ bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ಪೂರ್ಣಾಙ್ಕ width = GetSize(cell->getPort(ID::Y));
	ಪೂರ್ಣಾಙ್ಕ a_width = GetSize(cell->getPort(ID::A));
	ಪೂರ್ಣಾಙ್ಕ b_width = GetSize(cell->getPort(ID::B));
	ಪೂರ್ಣಾಙ್ಕ s_width = GetSize(cell->getPort(ID::S));
	ಪೂರ್ಣಾಙ್ಕ max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

ತಾರ್ಕಿಕ YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	ಯದಿ (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ದ್ವಿಮಾನ_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type == ID($neg)) {
		arith_neg_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($add), ID($sub))) {
		arith_ದ್ವಿಮಾನ_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type == ID($slice)) {
		slice_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type == ID($concat)) {
		concat_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type == ID($bmux)) {
		bmux_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type == ID($demux)) {
		demux_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type == ID($bweqx)) {
		bweqx_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type == ID($bwmux)) {
		bwmux_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->is_builtin_ff()) {
		ff_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಯದಿ (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(ಏತತ್, cell);
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	ಯದಿ (cell->type.in(ID($ಪ್ರತಿಜ್ಞಾ), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ; // no-op: these have either no inputs or no outputs

	ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
}
