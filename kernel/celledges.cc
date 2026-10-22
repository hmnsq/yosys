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

#அந்தர்பாவ "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

வெறுமை bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	தருக்க is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு y_width = GetSize(cell->getPort(ID::Y));

	க்கு (முழு i = 0; i < y_width; i++)
	{
		எனில் (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		இல்லையெனில் எனில் (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

வெறுமை bitwise_இருமம்_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	தருக்க is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));
	முழு y_width = GetSize(cell->getPort(ID::Y));

	எனில் (cell->type == ID($and) && !is_signed) {
		எனில் (a_width > b_width)
			a_width = b_width;
		இல்லையெனில்
			b_width = a_width;
	}

	க்கு (முழு i = 0; i < y_width; i++)
	{
		எனில் (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		இல்லையெனில் எனில் (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		எனில் (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		இல்லையெனில் எனில் (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

வெறுமை arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	தருக்க is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு y_width = GetSize(cell->getPort(ID::Y));

	எனில் (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	க்கு (முழு i = 0; i < y_width; i++)
	க்கு (முழு k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

வெறுமை arith_இருமம்_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	தருக்க is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));
	முழு y_width = GetSize(cell->getPort(ID::Y));

	எனில் (!is_signed && cell->type != ID($sub)) {
		முழு ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	க்கு (முழு i = 0; i < y_width; i++)
	{
		க்கு (முழு k = 0; k <= i; k++)
		{
			எனில் (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			எனில் (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

வெறுமை reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு a_width = GetSize(cell->getPort(ID::A));

	க்கு (முழு i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

வெறுமை logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));

	க்கு (முழு i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	க்கு (முழு i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

வெறுமை concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));

	க்கு (முழு i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	க்கு (முழு i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

வெறுமை slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு offset = cell->getParam(ID::OFFSET).as_int();
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு y_width = GetSize(cell->getPort(ID::Y));

	க்கு (முழு i = 0; i < y_width; i++) {
		முழு a_bit = offset + i;
		எனில் (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

வெறுமை compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));

	க்கு (முழு i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	க்கு (முழு i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

வெறுமை mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));
	முழு s_width = GetSize(cell->getPort(ID::S));

	க்கு (முழு i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		க்கு (முழு k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		க்கு (முழு k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

வெறுமை bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு width = GetSize(cell->getPort(ID::Y));
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு s_width = GetSize(cell->getPort(ID::S));

	க்கு (முழு i = 0; i < width; i++)
	{
		க்கு (முழு k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		க்கு (முழு k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

வெறுமை demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு width = GetSize(cell->getPort(ID::Y));
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு s_width = GetSize(cell->getPort(ID::S));

	க்கு (முழு i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		க்கு (முழு k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

வெறுமை shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	தருக்க is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	தருக்க is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));
	முழு y_width = GetSize(cell->getPort(ID::Y));

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
	முழு b_width_capped = min(b_width, 30);

	முழு b_உயர்வு, b_கீழ்;
	எனில் (!is_b_signed) {
		b_உயர்வு = (1 << b_width_capped) - 1;
		b_கீழ் = 0;
	} இல்லையெனில் {
		b_உயர்வு = (1 << (b_width_capped - 1)) - 1;
		b_கீழ் = -(1 << (b_width_capped - 1));
	}

	க்கு (முழு i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		முழு b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		முழு a_range_upper;
		// lowest position of A that can be moved to Y[i]
		முழு a_range_lower;

		எனில் (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_உயர்வு;
			எனில் (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_உயர்வு);
			a_range_upper = min(i+1, a_width);
		} இல்லையெனில் எனில் (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_உயர்வு, a_width);
		} இல்லையெனில் எனில் (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_கீழ்;
			a_range_lower = max(0, i + b_கீழ்);
			எனில் (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_உயர்வு, a_width);
		} இல்லையெனில் {
			log_assert(பொய் && "unreachable");
		}

		எனில் (i < b_range_upper) {
			க்கு (முழு k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} இல்லையெனில் {
			// only influence is through sign extension
			எனில் (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		க்கு (முழு k = 0; k < b_width_capped; k++) {
			// left shifts
			எனில் (cell->type.in(ID($shl), ID($sshl))) {
				எனில் (a_width == 1 && is_signed) {
					முழு skip = 1 << (k + 1);
					முழு base = skip -1;
					எனில் (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} இல்லையெனில் எனில் (is_signed) {
					எனில் (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} இல்லையெனில் {
					எனில் (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} இல்லையெனில் எனில் (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				எனில் (is_signed) {
					தருக்க shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					தருக்க zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					எனில் (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} இல்லையெனில் {
					எனில் (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} இல்லையெனில் எனில் (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				எனில் (is_signed) {
					எனில் (k != b_width_capped - 1) {
						தருக்க r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						தருக்க r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_உயர்வு));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						தருக்க l = a_width - 2 - i >= b_கீழ்;
						எனில் (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						எனில் (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} இல்லையெனில் {
						எனில் (y_width - i <= b_உயர்வு || a_width - 2 - i >= b_கீழ்)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} இல்லையெனில் {
					எனில் (a_width - 1 - i >= b_கீழ்)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} இல்லையெனில் {
				log_assert(பொய் && "unreachable");
			}
		}
	}
}

வெறுமை packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	முழு n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	முழு abits = cell->getParam(ID::ABITS).as_int();
	முழு width = cell->getParam(ID::WIDTH).as_int();

	க்கு (முழு i = 0; i < n_rd_ports; i++) {
		எனில் (rd_clk_enable[i] != State::S0) {
			க்கு (முழு k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			தொடர்;
		}

		க்கு (முழு j = 0; j < abits; j++)
			க்கு (முழு k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

வெறுமை memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	முழு abits = cell->getParam(ID::ABITS).as_int();
	முழு width = cell->getParam(ID::WIDTH).as_int();

	எனில் (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		எனில் (cell->type == ID($memrd_v2)) {
			க்கு (முழு k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		திரும்பு;
	}

	க்கு (முழு j = 0; j < abits; j++)
		க்கு (முழு k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

வெறுமை mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	எனில் (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	இல்லையெனில் எனில் (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	இல்லையெனில் எனில் (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		திரும்பு; /* no edges here */
	இல்லையெனில்
		log_abort();
}

வெறுமை ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு width = cell->getPort(ID::Q).size();

	எனில் (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		க்கு (முழு k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	எனில் (cell->hasPort(ID::CLR))
		க்கு (முழு k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	எனில் (cell->hasPort(ID::SET))
		க்கு (முழு k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	எனில் (cell->hasPort(ID::ALOAD))
		க்கு (முழு k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	எனில் (cell->hasPort(ID::AD))
		க்கு (முழு k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	எனில் (cell->hasPort(ID::ARST))
		க்கு (முழு k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

வெறுமை full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> உள்ளீடு_ports;
	std::vector<RTLIL::IdString> output_ports;

	க்கு (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		எனில் (cell->உள்ளீடு(port) || dir == RTLIL::PortDir::PD_INOUT)
			உள்ளீடு_ports.தள்ளு_back(port);
		எனில் (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.தள்ளு_back(port);
	}

	க்கு (auto out_port : output_ports)
	{
		முழு out_width = GetSize(cell->getPort(out_port));
		க்கு (முழு out_bit = 0; out_bit < out_width; out_bit++)
		{
			க்கு (auto in_port : உள்ளீடு_ports)
			{
				முழு in_width = GetSize(cell->getPort(in_port));
				க்கு (முழு in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

வெறுமை bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு width = GetSize(cell->getPort(ID::Y));
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));
	முழு max_width = std::min(width, std::min(a_width, b_width));

	க்கு (முழு i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

வெறுமை bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	முழு width = GetSize(cell->getPort(ID::Y));
	முழு a_width = GetSize(cell->getPort(ID::A));
	முழு b_width = GetSize(cell->getPort(ID::B));
	முழு s_width = GetSize(cell->getPort(ID::S));
	முழு max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	க்கு (முழு i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

தருக்க YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	எனில் (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_இருமம்_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type == ID($neg)) {
		arith_neg_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($add), ID($sub))) {
		arith_இருமம்_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type == ID($slice)) {
		slice_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type == ID($concat)) {
		concat_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type == ID($bmux)) {
		bmux_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type == ID($demux)) {
		demux_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type == ID($bweqx)) {
		bweqx_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type == ID($bwmux)) {
		bwmux_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->is_builtin_ff()) {
		ff_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(இது, cell);
		திரும்பு உண்மை;
	}

	எனில் (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(இது, cell);
		திரும்பு உண்மை;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	எனில் (cell->type.in(ID($ப்ரதிஜ்ஞா), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		திரும்பு உண்மை; // no-op: these have either no inputs or no outputs

	திரும்பு பொய்;
}
