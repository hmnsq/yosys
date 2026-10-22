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

#അന്തര്ഭാവ "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

ശൂന്യം bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	തർക്കം is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ y_width = GetSize(cell->getPort(ID::Y));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < y_width; i++)
	{
		എങ്കിൽ (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		അല്ലെങ്കിൽ എങ്കിൽ (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

ശൂന്യം bitwise_ദ്വിമാന_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	തർക്കം is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));
	പൂർണ്ണസംഖ്യ y_width = GetSize(cell->getPort(ID::Y));

	എങ്കിൽ (cell->type == ID($and) && !is_signed) {
		എങ്കിൽ (a_width > b_width)
			a_width = b_width;
		അല്ലെങ്കിൽ
			b_width = a_width;
	}

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < y_width; i++)
	{
		എങ്കിൽ (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		അല്ലെങ്കിൽ എങ്കിൽ (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		എങ്കിൽ (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		അല്ലെങ്കിൽ എങ്കിൽ (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

ശൂന്യം arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	തർക്കം is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ y_width = GetSize(cell->getPort(ID::Y));

	എങ്കിൽ (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < y_width; i++)
	വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

ശൂന്യം arith_ദ്വിമാന_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	തർക്കം is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));
	പൂർണ്ണസംഖ്യ y_width = GetSize(cell->getPort(ID::Y));

	എങ്കിൽ (!is_signed && cell->type != ID($sub)) {
		പൂർണ്ണസംഖ്യ ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < y_width; i++)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k <= i; k++)
		{
			എങ്കിൽ (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			എങ്കിൽ (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

ശൂന്യം reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

ശൂന്യം logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

ശൂന്യം concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

ശൂന്യം slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ offset = cell->getParam(ID::OFFSET).as_int();
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ y_width = GetSize(cell->getPort(ID::Y));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < y_width; i++) {
		പൂർണ്ണസംഖ്യ a_bit = offset + i;
		എങ്കിൽ (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

ശൂന്യം compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

ശൂന്യം mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));
	പൂർണ്ണസംഖ്യ s_width = GetSize(cell->getPort(ID::S));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		വേണ്ടി (പൂർണ്ണസംഖ്യ k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ശൂന്യം bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ width = GetSize(cell->getPort(ID::Y));
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ s_width = GetSize(cell->getPort(ID::S));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
	{
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ശൂന്യം demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ width = GetSize(cell->getPort(ID::Y));
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ s_width = GetSize(cell->getPort(ID::S));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

ശൂന്യം shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	തർക്കം is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	തർക്കം is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));
	പൂർണ്ണസംഖ്യ y_width = GetSize(cell->getPort(ID::Y));

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
	പൂർണ്ണസംഖ്യ b_width_capped = min(b_width, 30);

	പൂർണ്ണസംഖ്യ b_മുകളിലെ, b_താഴെയുള്ള;
	എങ്കിൽ (!is_b_signed) {
		b_മുകളിലെ = (1 << b_width_capped) - 1;
		b_താഴെയുള്ള = 0;
	} അല്ലെങ്കിൽ {
		b_മുകളിലെ = (1 << (b_width_capped - 1)) - 1;
		b_താഴെയുള്ള = -(1 << (b_width_capped - 1));
	}

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		പൂർണ്ണസംഖ്യ b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		പൂർണ്ണസംഖ്യ a_range_upper;
		// lowest position of A that can be moved to Y[i]
		പൂർണ്ണസംഖ്യ a_range_lower;

		എങ്കിൽ (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_മുകളിലെ;
			എങ്കിൽ (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_മുകളിലെ);
			a_range_upper = min(i+1, a_width);
		} അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_മുകളിലെ, a_width);
		} അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_താഴെയുള്ള;
			a_range_lower = max(0, i + b_താഴെയുള്ള);
			എങ്കിൽ (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_മുകളിലെ, a_width);
		} അല്ലെങ്കിൽ {
			log_assert(അസത്യം && "unreachable");
		}

		എങ്കിൽ (i < b_range_upper) {
			വേണ്ടി (പൂർണ്ണസംഖ്യ k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} അല്ലെങ്കിൽ {
			// only influence is through sign extension
			എങ്കിൽ (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < b_width_capped; k++) {
			// left shifts
			എങ്കിൽ (cell->type.in(ID($shl), ID($sshl))) {
				എങ്കിൽ (a_width == 1 && is_signed) {
					പൂർണ്ണസംഖ്യ skip = 1 << (k + 1);
					പൂർണ്ണസംഖ്യ base = skip -1;
					എങ്കിൽ (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} അല്ലെങ്കിൽ എങ്കിൽ (is_signed) {
					എങ്കിൽ (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} അല്ലെങ്കിൽ {
					എങ്കിൽ (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				എങ്കിൽ (is_signed) {
					തർക്കം shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					തർക്കം zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					എങ്കിൽ (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} അല്ലെങ്കിൽ {
					എങ്കിൽ (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				എങ്കിൽ (is_signed) {
					എങ്കിൽ (k != b_width_capped - 1) {
						തർക്കം r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						തർക്കം r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_മുകളിലെ));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						തർക്കം l = a_width - 2 - i >= b_താഴെയുള്ള;
						എങ്കിൽ (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						എങ്കിൽ (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} അല്ലെങ്കിൽ {
						എങ്കിൽ (y_width - i <= b_മുകളിലെ || a_width - 2 - i >= b_താഴെയുള്ള)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} അല്ലെങ്കിൽ {
					എങ്കിൽ (a_width - 1 - i >= b_താഴെയുള്ള)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} അല്ലെങ്കിൽ {
				log_assert(അസത്യം && "unreachable");
			}
		}
	}
}

ശൂന്യം packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	പൂർണ്ണസംഖ്യ n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	പൂർണ്ണസംഖ്യ abits = cell->getParam(ID::ABITS).as_int();
	പൂർണ്ണസംഖ്യ width = cell->getParam(ID::WIDTH).as_int();

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_rd_ports; i++) {
		എങ്കിൽ (rd_clk_enable[i] != State::S0) {
			വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			തുടരുക;
		}

		വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < abits; j++)
			വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

ശൂന്യം memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	പൂർണ്ണസംഖ്യ abits = cell->getParam(ID::ABITS).as_int();
	പൂർണ്ണസംഖ്യ width = cell->getParam(ID::WIDTH).as_int();

	എങ്കിൽ (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		എങ്കിൽ (cell->type == ID($memrd_v2)) {
			വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		തിരികെ;
	}

	വേണ്ടി (പൂർണ്ണസംഖ്യ j = 0; j < abits; j++)
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

ശൂന്യം mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	എങ്കിൽ (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	അല്ലെങ്കിൽ എങ്കിൽ (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		തിരികെ; /* no edges here */
	അല്ലെങ്കിൽ
		log_abort();
}

ശൂന്യം ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ width = cell->getPort(ID::Q).size();

	എങ്കിൽ (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	എങ്കിൽ (cell->hasPort(ID::CLR))
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	എങ്കിൽ (cell->hasPort(ID::SET))
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	എങ്കിൽ (cell->hasPort(ID::ALOAD))
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	എങ്കിൽ (cell->hasPort(ID::AD))
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	എങ്കിൽ (cell->hasPort(ID::ARST))
		വേണ്ടി (പൂർണ്ണസംഖ്യ k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

ശൂന്യം full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> നിവേശം_ports;
	std::vector<RTLIL::IdString> output_ports;

	വേണ്ടി (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		എങ്കിൽ (cell->നിവേശം(port) || dir == RTLIL::PortDir::PD_INOUT)
			നിവേശം_ports.തള്ളുക_back(port);
		എങ്കിൽ (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.തള്ളുക_back(port);
	}

	വേണ്ടി (auto out_port : output_ports)
	{
		പൂർണ്ണസംഖ്യ out_width = GetSize(cell->getPort(out_port));
		വേണ്ടി (പൂർണ്ണസംഖ്യ out_bit = 0; out_bit < out_width; out_bit++)
		{
			വേണ്ടി (auto in_port : നിവേശം_ports)
			{
				പൂർണ്ണസംഖ്യ in_width = GetSize(cell->getPort(in_port));
				വേണ്ടി (പൂർണ്ണസംഖ്യ in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

ശൂന്യം bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ width = GetSize(cell->getPort(ID::Y));
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));
	പൂർണ്ണസംഖ്യ max_width = std::min(width, std::min(a_width, b_width));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

ശൂന്യം bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	പൂർണ്ണസംഖ്യ width = GetSize(cell->getPort(ID::Y));
	പൂർണ്ണസംഖ്യ a_width = GetSize(cell->getPort(ID::A));
	പൂർണ്ണസംഖ്യ b_width = GetSize(cell->getPort(ID::B));
	പൂർണ്ണസംഖ്യ s_width = GetSize(cell->getPort(ID::S));
	പൂർണ്ണസംഖ്യ max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

തർക്കം YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	എങ്കിൽ (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_ദ്വിമാന_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($neg)) {
		arith_neg_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($add), ID($sub))) {
		arith_ദ്വിമാന_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($slice)) {
		slice_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($concat)) {
		concat_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($bmux)) {
		bmux_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($demux)) {
		demux_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($bweqx)) {
		bweqx_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type == ID($bwmux)) {
		bwmux_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->is_builtin_ff()) {
		ff_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	എങ്കിൽ (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(ഇത്, cell);
		തിരികെ സത്യം;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	എങ്കിൽ (cell->type.in(ID($പ്രതിജ്ഞാ), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		തിരികെ സത്യം; // no-op: these have either no inputs or no outputs

	തിരികെ അസത്യം;
}
