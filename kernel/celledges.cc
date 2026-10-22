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

#include "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

void bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม y_width = GetSize(cell->getPort(ID::Y));

	สำหรับ (จำนวนเต็ม i = 0; i < y_width; i++)
	{
		ถ้า (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		มิฉะนั้น ถ้า (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

void bitwise_binary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));
	จำนวนเต็ม y_width = GetSize(cell->getPort(ID::Y));

	ถ้า (cell->type == ID($and) && !is_signed) {
		ถ้า (a_width > b_width)
			a_width = b_width;
		มิฉะนั้น
			b_width = a_width;
	}

	สำหรับ (จำนวนเต็ม i = 0; i < y_width; i++)
	{
		ถ้า (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		มิฉะนั้น ถ้า (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		ถ้า (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		มิฉะนั้น ถ้า (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

void arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม y_width = GetSize(cell->getPort(ID::Y));

	ถ้า (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	สำหรับ (จำนวนเต็ม i = 0; i < y_width; i++)
	สำหรับ (จำนวนเต็ม k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

void arith_binary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));
	จำนวนเต็ม y_width = GetSize(cell->getPort(ID::Y));

	ถ้า (!is_signed && cell->type != ID($sub)) {
		จำนวนเต็ม ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	สำหรับ (จำนวนเต็ม i = 0; i < y_width; i++)
	{
		สำหรับ (จำนวนเต็ม k = 0; k <= i; k++)
		{
			ถ้า (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			ถ้า (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

void reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));

	สำหรับ (จำนวนเต็ม i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

void logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));

	สำหรับ (จำนวนเต็ม i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	สำหรับ (จำนวนเต็ม i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

void concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));

	สำหรับ (จำนวนเต็ม i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	สำหรับ (จำนวนเต็ม i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

void slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม offset = cell->getParam(ID::OFFSET).as_int();
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม y_width = GetSize(cell->getPort(ID::Y));

	สำหรับ (จำนวนเต็ม i = 0; i < y_width; i++) {
		จำนวนเต็ม a_bit = offset + i;
		ถ้า (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

void compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));

	สำหรับ (จำนวนเต็ม i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	สำหรับ (จำนวนเต็ม i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

void mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));
	จำนวนเต็ม s_width = GetSize(cell->getPort(ID::S));

	สำหรับ (จำนวนเต็ม i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		สำหรับ (จำนวนเต็ม k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		สำหรับ (จำนวนเต็ม k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

void bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม width = GetSize(cell->getPort(ID::Y));
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม s_width = GetSize(cell->getPort(ID::S));

	สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
	{
		สำหรับ (จำนวนเต็ม k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		สำหรับ (จำนวนเต็ม k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

void demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม width = GetSize(cell->getPort(ID::Y));
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม s_width = GetSize(cell->getPort(ID::S));

	สำหรับ (จำนวนเต็ม i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		สำหรับ (จำนวนเต็ม k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

void shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	bool is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));
	จำนวนเต็ม y_width = GetSize(cell->getPort(ID::Y));

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
	จำนวนเต็ม b_width_capped = min(b_width, 30);

	จำนวนเต็ม b_high, b_low;
	ถ้า (!is_b_signed) {
		b_high = (1 << b_width_capped) - 1;
		b_low = 0;
	} มิฉะนั้น {
		b_high = (1 << (b_width_capped - 1)) - 1;
		b_low = -(1 << (b_width_capped - 1));
	}

	สำหรับ (จำนวนเต็ม i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		จำนวนเต็ม b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		จำนวนเต็ม a_range_upper;
		// lowest position of A that can be moved to Y[i]
		จำนวนเต็ม a_range_lower;

		ถ้า (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_high;
			ถ้า (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_high);
			a_range_upper = min(i+1, a_width);
		} มิฉะนั้น ถ้า (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_high, a_width);
		} มิฉะนั้น ถ้า (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_low;
			a_range_lower = max(0, i + b_low);
			ถ้า (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_high, a_width);
		} มิฉะนั้น {
			log_assert(false && "unreachable");
		}

		ถ้า (i < b_range_upper) {
			สำหรับ (จำนวนเต็ม k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} มิฉะนั้น {
			// only influence is through sign extension
			ถ้า (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		สำหรับ (จำนวนเต็ม k = 0; k < b_width_capped; k++) {
			// left shifts
			ถ้า (cell->type.in(ID($shl), ID($sshl))) {
				ถ้า (a_width == 1 && is_signed) {
					จำนวนเต็ม skip = 1 << (k + 1);
					จำนวนเต็ม base = skip -1;
					ถ้า (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} มิฉะนั้น ถ้า (is_signed) {
					ถ้า (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} มิฉะนั้น {
					ถ้า (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} มิฉะนั้น ถ้า (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				ถ้า (is_signed) {
					bool shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					bool zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					ถ้า (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} มิฉะนั้น {
					ถ้า (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} มิฉะนั้น ถ้า (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				ถ้า (is_signed) {
					ถ้า (k != b_width_capped - 1) {
						bool r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						bool r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_high));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						bool l = a_width - 2 - i >= b_low;
						ถ้า (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						ถ้า (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} มิฉะนั้น {
						ถ้า (y_width - i <= b_high || a_width - 2 - i >= b_low)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} มิฉะนั้น {
					ถ้า (a_width - 1 - i >= b_low)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} มิฉะนั้น {
				log_assert(false && "unreachable");
			}
		}
	}
}

void packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	จำนวนเต็ม n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	จำนวนเต็ม abits = cell->getParam(ID::ABITS).as_int();
	จำนวนเต็ม width = cell->getParam(ID::WIDTH).as_int();

	สำหรับ (จำนวนเต็ม i = 0; i < n_rd_ports; i++) {
		ถ้า (rd_clk_enable[i] != State::S0) {
			สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			continue;
		}

		สำหรับ (จำนวนเต็ม j = 0; j < abits; j++)
			สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

void memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	จำนวนเต็ม abits = cell->getParam(ID::ABITS).as_int();
	จำนวนเต็ม width = cell->getParam(ID::WIDTH).as_int();

	ถ้า (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		ถ้า (cell->type == ID($memrd_v2)) {
			สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		คืน;
	}

	สำหรับ (จำนวนเต็ม j = 0; j < abits; j++)
		สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

void mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	ถ้า (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	มิฉะนั้น ถ้า (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	มิฉะนั้น ถ้า (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		คืน; /* no edges here */
	มิฉะนั้น
		log_abort();
}

void ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม width = cell->getPort(ID::Q).size();

	ถ้า (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		สำหรับ (จำนวนเต็ม k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	ถ้า (cell->hasPort(ID::CLR))
		สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	ถ้า (cell->hasPort(ID::SET))
		สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	ถ้า (cell->hasPort(ID::ALOAD))
		สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	ถ้า (cell->hasPort(ID::AD))
		สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	ถ้า (cell->hasPort(ID::ARST))
		สำหรับ (จำนวนเต็ม k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

void full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> input_ports;
	std::vector<RTLIL::IdString> output_ports;

	สำหรับ (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		ถ้า (cell->input(port) || dir == RTLIL::PortDir::PD_INOUT)
			input_ports.push_back(port);
		ถ้า (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.push_back(port);
	}

	สำหรับ (auto out_port : output_ports)
	{
		จำนวนเต็ม out_width = GetSize(cell->getPort(out_port));
		สำหรับ (จำนวนเต็ม out_bit = 0; out_bit < out_width; out_bit++)
		{
			สำหรับ (auto in_port : input_ports)
			{
				จำนวนเต็ม in_width = GetSize(cell->getPort(in_port));
				สำหรับ (จำนวนเต็ม in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

void bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม width = GetSize(cell->getPort(ID::Y));
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));
	จำนวนเต็ม max_width = std::min(width, std::min(a_width, b_width));

	สำหรับ (จำนวนเต็ม i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

void bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	จำนวนเต็ม width = GetSize(cell->getPort(ID::Y));
	จำนวนเต็ม a_width = GetSize(cell->getPort(ID::A));
	จำนวนเต็ม b_width = GetSize(cell->getPort(ID::B));
	จำนวนเต็ม s_width = GetSize(cell->getPort(ID::S));
	จำนวนเต็ม max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	สำหรับ (จำนวนเต็ม i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

bool YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	ถ้า (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_binary_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type == ID($neg)) {
		arith_neg_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($add), ID($sub))) {
		arith_binary_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type == ID($slice)) {
		slice_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type == ID($concat)) {
		concat_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type == ID($bmux)) {
		bmux_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type == ID($demux)) {
		demux_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type == ID($bweqx)) {
		bweqx_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type == ID($bwmux)) {
		bwmux_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(this, cell);
		คืน true;
	}

	ถ้า (cell->is_builtin_ff()) {
		ff_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(this, cell);
		คืน true;
	}

	ถ้า (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(this, cell);
		คืน true;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	ถ้า (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		คืน true; // no-op: these have either no inputs or no outputs

	คืน false;
}
