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

#অন্তর্ভাব "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

শূন্য bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	যৌক্তিক is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা y_width = Get_আকার(cell->getPort(ID::Y));

	জন্য (পূর্ণসংখ্যা i = 0; i < y_width; i++)
	{
		যদি (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		অন্যথা যদি (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

শূন্য bitwise_দ্বিমিক_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	যৌক্তিক is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));
	পূর্ণসংখ্যা y_width = Get_আকার(cell->getPort(ID::Y));

	যদি (cell->type == ID($and) && !is_signed) {
		যদি (a_width > b_width)
			a_width = b_width;
		অন্যথা
			b_width = a_width;
	}

	জন্য (পূর্ণসংখ্যা i = 0; i < y_width; i++)
	{
		যদি (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		অন্যথা যদি (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		যদি (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		অন্যথা যদি (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

শূন্য arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	যৌক্তিক is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা y_width = Get_আকার(cell->getPort(ID::Y));

	যদি (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	জন্য (পূর্ণসংখ্যা i = 0; i < y_width; i++)
	জন্য (পূর্ণসংখ্যা k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

শূন্য arith_দ্বিমিক_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	যৌক্তিক is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));
	পূর্ণসংখ্যা y_width = Get_আকার(cell->getPort(ID::Y));

	যদি (!is_signed && cell->type != ID($sub)) {
		পূর্ণসংখ্যা ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	জন্য (পূর্ণসংখ্যা i = 0; i < y_width; i++)
	{
		জন্য (পূর্ণসংখ্যা k = 0; k <= i; k++)
		{
			যদি (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			যদি (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

শূন্য reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));

	জন্য (পূর্ণসংখ্যা i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

শূন্য logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));

	জন্য (পূর্ণসংখ্যা i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	জন্য (পূর্ণসংখ্যা i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

শূন্য concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));

	জন্য (পূর্ণসংখ্যা i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	জন্য (পূর্ণসংখ্যা i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

শূন্য slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা offset = cell->getParam(ID::OFFSET).as_int();
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা y_width = Get_আকার(cell->getPort(ID::Y));

	জন্য (পূর্ণসংখ্যা i = 0; i < y_width; i++) {
		পূর্ণসংখ্যা a_bit = offset + i;
		যদি (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

শূন্য compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));

	জন্য (পূর্ণসংখ্যা i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	জন্য (পূর্ণসংখ্যা i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

শূন্য mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));
	পূর্ণসংখ্যা s_width = Get_আকার(cell->getPort(ID::S));

	জন্য (পূর্ণসংখ্যা i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		জন্য (পূর্ণসংখ্যা k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		জন্য (পূর্ণসংখ্যা k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

শূন্য bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা width = Get_আকার(cell->getPort(ID::Y));
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা s_width = Get_আকার(cell->getPort(ID::S));

	জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
	{
		জন্য (পূর্ণসংখ্যা k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		জন্য (পূর্ণসংখ্যা k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

শূন্য demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা width = Get_আকার(cell->getPort(ID::Y));
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা s_width = Get_আকার(cell->getPort(ID::S));

	জন্য (পূর্ণসংখ্যা i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		জন্য (পূর্ণসংখ্যা k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

শূন্য shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	যৌক্তিক is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	যৌক্তিক is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));
	পূর্ণসংখ্যা y_width = Get_আকার(cell->getPort(ID::Y));

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
	পূর্ণসংখ্যা b_width_capped = min(b_width, 30);

	পূর্ণসংখ্যা b_উপরের, b_নিচের;
	যদি (!is_b_signed) {
		b_উপরের = (1 << b_width_capped) - 1;
		b_নিচের = 0;
	} অন্যথা {
		b_উপরের = (1 << (b_width_capped - 1)) - 1;
		b_নিচের = -(1 << (b_width_capped - 1));
	}

	জন্য (পূর্ণসংখ্যা i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		পূর্ণসংখ্যা b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		পূর্ণসংখ্যা a_range_upper;
		// lowest position of A that can be moved to Y[i]
		পূর্ণসংখ্যা a_range_lower;

		যদি (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_উপরের;
			যদি (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_উপরের);
			a_range_upper = min(i+1, a_width);
		} অন্যথা যদি (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_উপরের, a_width);
		} অন্যথা যদি (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_নিচের;
			a_range_lower = max(0, i + b_নিচের);
			যদি (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_উপরের, a_width);
		} অন্যথা {
			log_assert(মিথ্যা && "unreachable");
		}

		যদি (i < b_range_upper) {
			জন্য (পূর্ণসংখ্যা k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} অন্যথা {
			// only influence is through sign extension
			যদি (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		জন্য (পূর্ণসংখ্যা k = 0; k < b_width_capped; k++) {
			// left shifts
			যদি (cell->type.in(ID($shl), ID($sshl))) {
				যদি (a_width == 1 && is_signed) {
					পূর্ণসংখ্যা skip = 1 << (k + 1);
					পূর্ণসংখ্যা base = skip -1;
					যদি (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} অন্যথা যদি (is_signed) {
					যদি (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} অন্যথা {
					যদি (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} অন্যথা যদি (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				যদি (is_signed) {
					যৌক্তিক shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					যৌক্তিক zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					যদি (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} অন্যথা {
					যদি (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} অন্যথা যদি (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				যদি (is_signed) {
					যদি (k != b_width_capped - 1) {
						যৌক্তিক r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						যৌক্তিক r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_উপরের));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						যৌক্তিক l = a_width - 2 - i >= b_নিচের;
						যদি (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						যদি (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} অন্যথা {
						যদি (y_width - i <= b_উপরের || a_width - 2 - i >= b_নিচের)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} অন্যথা {
					যদি (a_width - 1 - i >= b_নিচের)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} অন্যথা {
				log_assert(মিথ্যা && "unreachable");
			}
		}
	}
}

শূন্য packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	পূর্ণসংখ্যা n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	পূর্ণসংখ্যা abits = cell->getParam(ID::ABITS).as_int();
	পূর্ণসংখ্যা width = cell->getParam(ID::WIDTH).as_int();

	জন্য (পূর্ণসংখ্যা i = 0; i < n_rd_ports; i++) {
		যদি (rd_clk_enable[i] != State::S0) {
			জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_তথ্য, i * width + k, -1);
			চালু;
		}

		জন্য (পূর্ণসংখ্যা j = 0; j < abits; j++)
			জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_তথ্য, i * width + k, -1);
	}
}

শূন্য memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	পূর্ণসংখ্যা abits = cell->getParam(ID::ABITS).as_int();
	পূর্ণসংখ্যা width = cell->getParam(ID::WIDTH).as_int();

	যদি (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		যদি (cell->type == ID($memrd_v2)) {
			জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::তথ্য_2, k, -1);
		}
		ফেরত;
	}

	জন্য (পূর্ণসংখ্যা j = 0; j < abits; j++)
		জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::তথ্য_2, k, -1);
}

শূন্য mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	যদি (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	অন্যথা যদি (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	অন্যথা যদি (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		ফেরত; /* no edges here */
	অন্যথা
		log_abort();
}

শূন্য ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা width = cell->getPort(ID::Q).আকার_2();

	যদি (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		জন্য (পূর্ণসংখ্যা k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	যদি (cell->hasPort(ID::CLR))
		জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	যদি (cell->hasPort(ID::SET))
		জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	যদি (cell->hasPort(ID::ALOAD))
		জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	যদি (cell->hasPort(ID::AD))
		জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	যদি (cell->hasPort(ID::ARST))
		জন্য (পূর্ণসংখ্যা k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

শূন্য full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> প্রবেশ_ports;
	std::vector<RTLIL::IdString> output_ports;

	জন্য (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		যদি (cell->প্রবেশ(port) || dir == RTLIL::PortDir::PD_INOUT)
			প্রবেশ_ports.ঠেলো_back(port);
		যদি (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.ঠেলো_back(port);
	}

	জন্য (auto out_port : output_ports)
	{
		পূর্ণসংখ্যা out_width = Get_আকার(cell->getPort(out_port));
		জন্য (পূর্ণসংখ্যা out_bit = 0; out_bit < out_width; out_bit++)
		{
			জন্য (auto in_port : প্রবেশ_ports)
			{
				পূর্ণসংখ্যা in_width = Get_আকার(cell->getPort(in_port));
				জন্য (পূর্ণসংখ্যা in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

শূন্য bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা width = Get_আকার(cell->getPort(ID::Y));
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));
	পূর্ণসংখ্যা max_width = std::min(width, std::min(a_width, b_width));

	জন্য (পূর্ণসংখ্যা i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

শূন্য bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	পূর্ণসংখ্যা width = Get_আকার(cell->getPort(ID::Y));
	পূর্ণসংখ্যা a_width = Get_আকার(cell->getPort(ID::A));
	পূর্ণসংখ্যা b_width = Get_আকার(cell->getPort(ID::B));
	পূর্ণসংখ্যা s_width = Get_আকার(cell->getPort(ID::S));
	পূর্ণসংখ্যা max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	জন্য (পূর্ণসংখ্যা i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

যৌক্তিক YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	যদি (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_দ্বিমিক_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type == ID($neg)) {
		arith_neg_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($add), ID($sub))) {
		arith_দ্বিমিক_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type == ID($slice)) {
		slice_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type == ID($concat)) {
		concat_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type == ID($bmux)) {
		bmux_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type == ID($demux)) {
		demux_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type == ID($bweqx)) {
		bweqx_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type == ID($bwmux)) {
		bwmux_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->is_builtin_ff()) {
		ff_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(এই, cell);
		ফেরত সত্য;
	}

	যদি (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(এই, cell);
		ফেরত সত্য;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	যদি (cell->type.in(ID($প্রতিজ্ঞা), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		ফেরত সত্য; // no-op: these have either no inputs or no outputs

	ফেরত মিথ্যা;
}
