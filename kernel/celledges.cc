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
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên y_width = GetSize(cell->getPort(ID::Y));

	cho (số_nguyên i = 0; i < y_width; i++)
	{
		nếu (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		không_thì nếu (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

void bitwise_binary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));
	số_nguyên y_width = GetSize(cell->getPort(ID::Y));

	nếu (cell->type == ID($and) && !is_signed) {
		nếu (a_width > b_width)
			a_width = b_width;
		không_thì
			b_width = a_width;
	}

	cho (số_nguyên i = 0; i < y_width; i++)
	{
		nếu (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		không_thì nếu (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		nếu (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		không_thì nếu (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

void arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên y_width = GetSize(cell->getPort(ID::Y));

	nếu (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	cho (số_nguyên i = 0; i < y_width; i++)
	cho (số_nguyên k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

void arith_binary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));
	số_nguyên y_width = GetSize(cell->getPort(ID::Y));

	nếu (!is_signed && cell->type != ID($sub)) {
		số_nguyên ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	cho (số_nguyên i = 0; i < y_width; i++)
	{
		cho (số_nguyên k = 0; k <= i; k++)
		{
			nếu (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			nếu (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

void reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên a_width = GetSize(cell->getPort(ID::A));

	cho (số_nguyên i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

void logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));

	cho (số_nguyên i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	cho (số_nguyên i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

void concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));

	cho (số_nguyên i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	cho (số_nguyên i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

void slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên offset = cell->getParam(ID::OFFSET).as_int();
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên y_width = GetSize(cell->getPort(ID::Y));

	cho (số_nguyên i = 0; i < y_width; i++) {
		số_nguyên a_bit = offset + i;
		nếu (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

void compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));

	cho (số_nguyên i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	cho (số_nguyên i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

void mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));
	số_nguyên s_width = GetSize(cell->getPort(ID::S));

	cho (số_nguyên i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		cho (số_nguyên k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		cho (số_nguyên k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

void bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên width = GetSize(cell->getPort(ID::Y));
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên s_width = GetSize(cell->getPort(ID::S));

	cho (số_nguyên i = 0; i < width; i++)
	{
		cho (số_nguyên k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		cho (số_nguyên k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

void demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên width = GetSize(cell->getPort(ID::Y));
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên s_width = GetSize(cell->getPort(ID::S));

	cho (số_nguyên i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		cho (số_nguyên k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

void shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	bool is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	bool is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));
	số_nguyên y_width = GetSize(cell->getPort(ID::Y));

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
	số_nguyên b_width_capped = min(b_width, 30);

	số_nguyên b_high, b_low;
	nếu (!is_b_signed) {
		b_high = (1 << b_width_capped) - 1;
		b_low = 0;
	} không_thì {
		b_high = (1 << (b_width_capped - 1)) - 1;
		b_low = -(1 << (b_width_capped - 1));
	}

	cho (số_nguyên i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		số_nguyên b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		số_nguyên a_range_upper;
		// lowest position of A that can be moved to Y[i]
		số_nguyên a_range_lower;

		nếu (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_high;
			nếu (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_high);
			a_range_upper = min(i+1, a_width);
		} không_thì nếu (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_high, a_width);
		} không_thì nếu (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_low;
			a_range_lower = max(0, i + b_low);
			nếu (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_high, a_width);
		} không_thì {
			log_assert(false && "unreachable");
		}

		nếu (i < b_range_upper) {
			cho (số_nguyên k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} không_thì {
			// only influence is through sign extension
			nếu (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		cho (số_nguyên k = 0; k < b_width_capped; k++) {
			// left shifts
			nếu (cell->type.in(ID($shl), ID($sshl))) {
				nếu (a_width == 1 && is_signed) {
					số_nguyên skip = 1 << (k + 1);
					số_nguyên base = skip -1;
					nếu (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} không_thì nếu (is_signed) {
					nếu (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} không_thì {
					nếu (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} không_thì nếu (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				nếu (is_signed) {
					bool shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					bool zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					nếu (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} không_thì {
					nếu (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} không_thì nếu (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				nếu (is_signed) {
					nếu (k != b_width_capped - 1) {
						bool r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						bool r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_high));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						bool l = a_width - 2 - i >= b_low;
						nếu (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						nếu (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} không_thì {
						nếu (y_width - i <= b_high || a_width - 2 - i >= b_low)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} không_thì {
					nếu (a_width - 1 - i >= b_low)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} không_thì {
				log_assert(false && "unreachable");
			}
		}
	}
}

void packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	số_nguyên n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	số_nguyên abits = cell->getParam(ID::ABITS).as_int();
	số_nguyên width = cell->getParam(ID::WIDTH).as_int();

	cho (số_nguyên i = 0; i < n_rd_ports; i++) {
		nếu (rd_clk_enable[i] != State::S0) {
			cho (số_nguyên k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			continue;
		}

		cho (số_nguyên j = 0; j < abits; j++)
			cho (số_nguyên k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

void memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	số_nguyên abits = cell->getParam(ID::ABITS).as_int();
	số_nguyên width = cell->getParam(ID::WIDTH).as_int();

	nếu (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		nếu (cell->type == ID($memrd_v2)) {
			cho (số_nguyên k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		trả_về;
	}

	cho (số_nguyên j = 0; j < abits; j++)
		cho (số_nguyên k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

void mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	nếu (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	không_thì nếu (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	không_thì nếu (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		trả_về; /* no edges here */
	không_thì
		log_abort();
}

void ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên width = cell->getPort(ID::Q).size();

	nếu (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		cho (số_nguyên k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	nếu (cell->hasPort(ID::CLR))
		cho (số_nguyên k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	nếu (cell->hasPort(ID::SET))
		cho (số_nguyên k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	nếu (cell->hasPort(ID::ALOAD))
		cho (số_nguyên k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	nếu (cell->hasPort(ID::AD))
		cho (số_nguyên k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	nếu (cell->hasPort(ID::ARST))
		cho (số_nguyên k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

void full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> input_ports;
	std::vector<RTLIL::IdString> output_ports;

	cho (auto &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		nếu (cell->input(port) || dir == RTLIL::PortDir::PD_INOUT)
			input_ports.push_back(port);
		nếu (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.push_back(port);
	}

	cho (auto out_port : output_ports)
	{
		số_nguyên out_width = GetSize(cell->getPort(out_port));
		cho (số_nguyên out_bit = 0; out_bit < out_width; out_bit++)
		{
			cho (auto in_port : input_ports)
			{
				số_nguyên in_width = GetSize(cell->getPort(in_port));
				cho (số_nguyên in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

void bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên width = GetSize(cell->getPort(ID::Y));
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));
	số_nguyên max_width = std::min(width, std::min(a_width, b_width));

	cho (số_nguyên i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

void bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	số_nguyên width = GetSize(cell->getPort(ID::Y));
	số_nguyên a_width = GetSize(cell->getPort(ID::A));
	số_nguyên b_width = GetSize(cell->getPort(ID::B));
	số_nguyên s_width = GetSize(cell->getPort(ID::S));
	số_nguyên max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	cho (số_nguyên i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

bool YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	nếu (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_binary_op(this, cell);
		trả_về true;
	}

	nếu (cell->type == ID($neg)) {
		arith_neg_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($add), ID($sub))) {
		arith_binary_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(this, cell);
		trả_về true;
	}

	nếu (cell->type == ID($slice)) {
		slice_op(this, cell);
		trả_về true;
	}

	nếu (cell->type == ID($concat)) {
		concat_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(this, cell);
		trả_về true;
	}

	nếu (cell->type == ID($bmux)) {
		bmux_op(this, cell);
		trả_về true;
	}

	nếu (cell->type == ID($demux)) {
		demux_op(this, cell);
		trả_về true;
	}

	nếu (cell->type == ID($bweqx)) {
		bweqx_op(this, cell);
		trả_về true;
	}

	nếu (cell->type == ID($bwmux)) {
		bwmux_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(this, cell);
		trả_về true;
	}

	nếu (cell->is_builtin_ff()) {
		ff_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($mul), ID($div), ID($mod), ID($divfloor), ID($modfloor), ID($pow))) {
		full_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(this, cell);
		trả_về true;
	}

	nếu (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(this, cell);
		trả_về true;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	nếu (cell->type.in(ID($assert), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		trả_về true; // no-op: these have either no inputs or no outputs

	trả_về false;
}
