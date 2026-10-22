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

#ଅନ୍ତର୍ଭାଵ "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

ଶୂନ୍ୟ bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ତାର୍କିକ is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = GetSize(cell->getPort(ID::Y));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < y_width; i++)
	{
		ଯଦି (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		ଅନ୍ୟଥା ଯଦି (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

ଶୂନ୍ୟ bitwise_ଦ୍ୱିମିକ_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ତାର୍କିକ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = GetSize(cell->getPort(ID::Y));

	ଯଦି (cell->type == ID($and) && !is_signed) {
		ଯଦି (a_width > b_width)
			a_width = b_width;
		ଅନ୍ୟଥା
			b_width = a_width;
	}

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < y_width; i++)
	{
		ଯଦି (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		ଅନ୍ୟଥା ଯଦି (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		ଯଦି (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		ଅନ୍ୟଥା ଯଦି (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

ଶୂନ୍ୟ arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ତାର୍କିକ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = GetSize(cell->getPort(ID::Y));

	ଯଦି (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < y_width; i++)
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

ଶୂନ୍ୟ arith_ଦ୍ୱିମିକ_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ତାର୍କିକ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = GetSize(cell->getPort(ID::Y));

	ଯଦି (!is_signed && cell->type != ID($sub)) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < y_width; i++)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k <= i; k++)
		{
			ଯଦି (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			ଯଦି (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

ଶୂନ୍ୟ reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

ଶୂନ୍ୟ logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

ଶୂନ୍ୟ concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

ଶୂନ୍ୟ slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = cell->getParam(ID::OFFSET).as_int();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = GetSize(cell->getPort(ID::Y));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < y_width; i++) {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_bit = offset + i;
		ଯଦି (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

ଶୂନ୍ୟ compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

ଶୂନ୍ୟ mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_width = GetSize(cell->getPort(ID::S));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ଶୂନ୍ୟ bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(cell->getPort(ID::Y));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_width = GetSize(cell->getPort(ID::S));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
	{
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ଶୂନ୍ୟ demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(cell->getPort(ID::Y));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_width = GetSize(cell->getPort(ID::S));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ଶୂନ୍ୟ shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ତାର୍କିକ is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	ତାର୍କିକ is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା y_width = GetSize(cell->getPort(ID::Y));

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
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width_capped = min(b_width, 30);

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_ଉପର, b_ତଳ;
	ଯଦି (!is_b_signed) {
		b_ଉପର = (1 << b_width_capped) - 1;
		b_ତଳ = 0;
	} ଅନ୍ୟଥା {
		b_ଉପର = (1 << (b_width_capped - 1)) - 1;
		b_ତଳ = -(1 << (b_width_capped - 1));
	}

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_range_upper;
		// lowest position of A that can be moved to Y[i]
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_range_lower;

		ଯଦି (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_ଉପର;
			ଯଦି (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_ଉପର);
			a_range_upper = min(i+1, a_width);
		} ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_ଉପର, a_width);
		} ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_ତଳ;
			a_range_lower = max(0, i + b_ତଳ);
			ଯଦି (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_ଉପର, a_width);
		} ଅନ୍ୟଥା {
			log_assert(ମିଥ୍ୟା && "unreachable");
		}

		ଯଦି (i < b_range_upper) {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} ଅନ୍ୟଥା {
			// only influence is through sign extension
			ଯଦି (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < b_width_capped; k++) {
			// left shifts
			ଯଦି (cell->type.in(ID($shl), ID($sshl))) {
				ଯଦି (a_width == 1 && is_signed) {
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା skip = 1 << (k + 1);
					ପୂର୍ଣ୍ଣସଂଖ୍ୟା base = skip -1;
					ଯଦି (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} ଅନ୍ୟଥା ଯଦି (is_signed) {
					ଯଦି (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} ଅନ୍ୟଥା {
					ଯଦି (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				ଯଦି (is_signed) {
					ତାର୍କିକ shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					ତାର୍କିକ zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					ଯଦି (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} ଅନ୍ୟଥା {
					ଯଦି (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				ଯଦି (is_signed) {
					ଯଦି (k != b_width_capped - 1) {
						ତାର୍କିକ r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						ତାର୍କିକ r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_ଉପର));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						ତାର୍କିକ l = a_width - 2 - i >= b_ତଳ;
						ଯଦି (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						ଯଦି (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} ଅନ୍ୟଥା {
						ଯଦି (y_width - i <= b_ଉପର || a_width - 2 - i >= b_ତଳ)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} ଅନ୍ୟଥା {
					ଯଦି (a_width - 1 - i >= b_ତଳ)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} ଅନ୍ୟଥା {
				log_assert(ମିଥ୍ୟା && "unreachable");
			}
		}
	}
}

ଶୂନ୍ୟ packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା abits = cell->getParam(ID::ABITS).as_int();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = cell->getParam(ID::WIDTH).as_int();

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_rd_ports; i++) {
		ଯଦି (rd_clk_enable[i] != State::S0) {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			ଜାରି;
		}

		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < abits; j++)
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

ଶୂନ୍ୟ memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା abits = cell->getParam(ID::ABITS).as_int();
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = cell->getParam(ID::WIDTH).as_int();

	ଯଦି (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		ଯଦି (cell->type == ID($memrd_v2)) {
			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		ଫେରନ୍ତୁ;
	}

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା j = 0; j < abits; j++)
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

ଶୂନ୍ୟ mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ଯଦି (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	ଅନ୍ୟଥା ଯଦି (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		ଫେରନ୍ତୁ; /* no edges here */
	ଅନ୍ୟଥା
		log_abort();
}

ଶୂନ୍ୟ ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = cell->getPort(ID::Q).size();

	ଯଦି (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	ଯଦି (cell->hasPort(ID::CLR))
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	ଯଦି (cell->hasPort(ID::SET))
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	ଯଦି (cell->hasPort(ID::ALOAD))
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	ଯଦି (cell->hasPort(ID::AD))
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	ଯଦି (cell->hasPort(ID::ARST))
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

ଶୂନ୍ୟ full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> ନିବେଶ_ports;
	std::vector<RTLIL::IdString> output_ports;

	ପାଇଁ (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		ଯଦି (cell->ନିବେଶ(port) || dir == RTLIL::PortDir::PD_INOUT)
			ନିବେଶ_ports.ଠେଲ_back(port);
		ଯଦି (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.ଠେଲ_back(port);
	}

	ପାଇଁ (auto out_port : output_ports)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_width = GetSize(cell->getPort(out_port));
		ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_bit = 0; out_bit < out_width; out_bit++)
		{
			ପାଇଁ (auto in_port : ନିବେଶ_ports)
			{
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_width = GetSize(cell->getPort(in_port));
				ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

ଶୂନ୍ୟ bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(cell->getPort(ID::Y));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_width = std::min(width, std::min(a_width, b_width));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

ଶୂନ୍ୟ bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = GetSize(cell->getPort(ID::Y));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା a_width = GetSize(cell->getPort(ID::A));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା b_width = GetSize(cell->getPort(ID::B));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା s_width = GetSize(cell->getPort(ID::S));
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

ତାର୍କିକ YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	ଯଦି (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ଦ୍ୱିମିକ_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type == ID($neg)) {
		arith_neg_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($add), ID($sub))) {
		arith_ଦ୍ୱିମିକ_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type == ID($slice)) {
		slice_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type == ID($concat)) {
		concat_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type == ID($bmux)) {
		bmux_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type == ID($demux)) {
		demux_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type == ID($bweqx)) {
		bweqx_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type == ID($bwmux)) {
		bwmux_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->is_builtin_ff()) {
		ff_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ଯଦି (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(ଏହି, cell);
		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	ଯଦି (cell->type.in(ID($ପ୍ରତିଜ୍ଞା), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		ଫେରନ୍ତୁ ସତ୍ୟ; // no-op: these have either no inputs or no outputs

	ଫେରନ୍ତୁ ମିଥ୍ୟା;
}
