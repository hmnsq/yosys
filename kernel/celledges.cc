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

#شامل "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

تهی bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	منطقی is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح y_width = GetSize(cell->getPort(ID::Y));

	برای (صحیح i = 0; i < y_width; i++)
	{
		اگر (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		وگرنه اگر (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

تهی bitwise_دودویی_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));
	صحیح y_width = GetSize(cell->getPort(ID::Y));

	اگر (cell->type == ID($and) && !is_signed) {
		اگر (a_width > b_width)
			a_width = b_width;
		وگرنه
			b_width = a_width;
	}

	برای (صحیح i = 0; i < y_width; i++)
	{
		اگر (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		وگرنه اگر (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		اگر (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		وگرنه اگر (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

تهی arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح y_width = GetSize(cell->getPort(ID::Y));

	اگر (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	برای (صحیح i = 0; i < y_width; i++)
	برای (صحیح k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

تهی arith_دودویی_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));
	صحیح y_width = GetSize(cell->getPort(ID::Y));

	اگر (!is_signed && cell->type != ID($sub)) {
		صحیح ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	برای (صحیح i = 0; i < y_width; i++)
	{
		برای (صحیح k = 0; k <= i; k++)
		{
			اگر (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			اگر (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

تهی reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح a_width = GetSize(cell->getPort(ID::A));

	برای (صحیح i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

تهی logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));

	برای (صحیح i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	برای (صحیح i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

تهی concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));

	برای (صحیح i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	برای (صحیح i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

تهی slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح offset = cell->getParam(ID::OFFSET).as_int();
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح y_width = GetSize(cell->getPort(ID::Y));

	برای (صحیح i = 0; i < y_width; i++) {
		صحیح a_bit = offset + i;
		اگر (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

تهی compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));

	برای (صحیح i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	برای (صحیح i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

تهی mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));
	صحیح s_width = GetSize(cell->getPort(ID::S));

	برای (صحیح i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		برای (صحیح k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		برای (صحیح k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

تهی bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح width = GetSize(cell->getPort(ID::Y));
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح s_width = GetSize(cell->getPort(ID::S));

	برای (صحیح i = 0; i < width; i++)
	{
		برای (صحیح k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		برای (صحیح k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

تهی demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح width = GetSize(cell->getPort(ID::Y));
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح s_width = GetSize(cell->getPort(ID::S));

	برای (صحیح i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		برای (صحیح k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

تهی shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	منطقی is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	منطقی is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));
	صحیح y_width = GetSize(cell->getPort(ID::Y));

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
	صحیح b_width_capped = min(b_width, 30);

	صحیح b_بالا, b_کم;
	اگر (!is_b_signed) {
		b_بالا = (1 << b_width_capped) - 1;
		b_کم = 0;
	} وگرنه {
		b_بالا = (1 << (b_width_capped - 1)) - 1;
		b_کم = -(1 << (b_width_capped - 1));
	}

	برای (صحیح i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		صحیح b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		صحیح a_range_upper;
		// lowest position of A that can be moved to Y[i]
		صحیح a_range_lower;

		اگر (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_بالا;
			اگر (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_بالا);
			a_range_upper = min(i+1, a_width);
		} وگرنه اگر (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_بالا, a_width);
		} وگرنه اگر (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_کم;
			a_range_lower = max(0, i + b_کم);
			اگر (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_بالا, a_width);
		} وگرنه {
			log_assert(نادرست && "unreachable");
		}

		اگر (i < b_range_upper) {
			برای (صحیح k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} وگرنه {
			// only influence is through sign extension
			اگر (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		برای (صحیح k = 0; k < b_width_capped; k++) {
			// left shifts
			اگر (cell->type.in(ID($shl), ID($sshl))) {
				اگر (a_width == 1 && is_signed) {
					صحیح skip = 1 << (k + 1);
					صحیح base = skip -1;
					اگر (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} وگرنه اگر (is_signed) {
					اگر (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} وگرنه {
					اگر (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} وگرنه اگر (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				اگر (is_signed) {
					منطقی shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					منطقی zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					اگر (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} وگرنه {
					اگر (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} وگرنه اگر (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				اگر (is_signed) {
					اگر (k != b_width_capped - 1) {
						منطقی r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						منطقی r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_بالا));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						منطقی l = a_width - 2 - i >= b_کم;
						اگر (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						اگر (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} وگرنه {
						اگر (y_width - i <= b_بالا || a_width - 2 - i >= b_کم)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} وگرنه {
					اگر (a_width - 1 - i >= b_کم)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} وگرنه {
				log_assert(نادرست && "unreachable");
			}
		}
	}
}

تهی packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	صحیح n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	صحیح abits = cell->getParam(ID::ABITS).as_int();
	صحیح width = cell->getParam(ID::WIDTH).as_int();

	برای (صحیح i = 0; i < n_rd_ports; i++) {
		اگر (rd_clk_enable[i] != State::S0) {
			برای (صحیح k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			ادامه;
		}

		برای (صحیح j = 0; j < abits; j++)
			برای (صحیح k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

تهی memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	صحیح abits = cell->getParam(ID::ABITS).as_int();
	صحیح width = cell->getParam(ID::WIDTH).as_int();

	اگر (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		اگر (cell->type == ID($memrd_v2)) {
			برای (صحیح k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		بازگشت;
	}

	برای (صحیح j = 0; j < abits; j++)
		برای (صحیح k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

تهی mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	اگر (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	وگرنه اگر (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	وگرنه اگر (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		بازگشت; /* no edges here */
	وگرنه
		log_abort();
}

تهی ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح width = cell->getPort(ID::Q).size();

	اگر (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		برای (صحیح k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	اگر (cell->hasPort(ID::CLR))
		برای (صحیح k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	اگر (cell->hasPort(ID::SET))
		برای (صحیح k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	اگر (cell->hasPort(ID::ALOAD))
		برای (صحیح k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	اگر (cell->hasPort(ID::AD))
		برای (صحیح k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	اگر (cell->hasPort(ID::ARST))
		برای (صحیح k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

تهی full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> درون_داد_ports;
	std::vector<RTLIL::IdString> output_ports;

	برای (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		اگر (cell->درون_داد(port) || dir == RTLIL::PortDir::PD_INOUT)
			درون_داد_ports.افزودن_back(port);
		اگر (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.افزودن_back(port);
	}

	برای (auto out_port : output_ports)
	{
		صحیح out_width = GetSize(cell->getPort(out_port));
		برای (صحیح out_bit = 0; out_bit < out_width; out_bit++)
		{
			برای (auto in_port : درون_داد_ports)
			{
				صحیح in_width = GetSize(cell->getPort(in_port));
				برای (صحیح in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

تهی bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح width = GetSize(cell->getPort(ID::Y));
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));
	صحیح max_width = std::min(width, std::min(a_width, b_width));

	برای (صحیح i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

تهی bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	صحیح width = GetSize(cell->getPort(ID::Y));
	صحیح a_width = GetSize(cell->getPort(ID::A));
	صحیح b_width = GetSize(cell->getPort(ID::B));
	صحیح s_width = GetSize(cell->getPort(ID::S));
	صحیح max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	برای (صحیح i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

منطقی YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	اگر (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_دودویی_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type == ID($neg)) {
		arith_neg_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($add), ID($sub))) {
		arith_دودویی_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type == ID($slice)) {
		slice_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type == ID($concat)) {
		concat_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type == ID($bmux)) {
		bmux_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type == ID($demux)) {
		demux_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type == ID($bweqx)) {
		bweqx_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type == ID($bwmux)) {
		bwmux_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->is_builtin_ff()) {
		ff_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(این, cell);
		بازگشت درست;
	}

	اگر (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(این, cell);
		بازگشت درست;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	اگر (cell->type.in(ID($أكد), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		بازگشت درست; // no-op: these have either no inputs or no outputs

	بازگشت نادرست;
}
