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

#अन्तर्भाव "kernel/celledges.h"

USING_YOSYS_NAMESPACE
PRIVATE_NAMESPACE_BEGIN

शून्य bitwise_unary_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = !cell->type.in(ID($buf), ID($barrier)) && cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क y_width = GetSize(cell->getPort(ID::Y));

	लागि (पूर्णाङ्क i = 0; i < y_width; i++)
	{
		यदि (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा यदि (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

शून्य bitwise_द्विआधारी_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));
	पूर्णाङ्क y_width = GetSize(cell->getPort(ID::Y));

	यदि (cell->type == ID($and) && !is_signed) {
		यदि (a_width > b_width)
			a_width = b_width;
		अन्यथा
			b_width = a_width;
	}

	लागि (पूर्णाङ्क i = 0; i < y_width; i++)
	{
		यदि (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा यदि (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		यदि (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		अन्यथा यदि (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

शून्य arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क y_width = GetSize(cell->getPort(ID::Y));

	यदि (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	लागि (पूर्णाङ्क i = 0; i < y_width; i++)
	लागि (पूर्णाङ्क k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

शून्य arith_द्विआधारी_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));
	पूर्णाङ्क y_width = GetSize(cell->getPort(ID::Y));

	यदि (!is_signed && cell->type != ID($sub)) {
		पूर्णाङ्क ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	लागि (पूर्णाङ्क i = 0; i < y_width; i++)
	{
		लागि (पूर्णाङ्क k = 0; k <= i; k++)
		{
			यदि (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			यदि (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

शून्य reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));

	लागि (पूर्णाङ्क i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

शून्य logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));

	लागि (पूर्णाङ्क i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	लागि (पूर्णाङ्क i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

शून्य concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));

	लागि (पूर्णाङ्क i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	लागि (पूर्णाङ्क i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

शून्य slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क offset = cell->getParam(ID::OFFSET).as_int();
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क y_width = GetSize(cell->getPort(ID::Y));

	लागि (पूर्णाङ्क i = 0; i < y_width; i++) {
		पूर्णाङ्क a_bit = offset + i;
		यदि (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

शून्य compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));

	लागि (पूर्णाङ्क i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	लागि (पूर्णाङ्क i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

शून्य mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));
	पूर्णाङ्क s_width = GetSize(cell->getPort(ID::S));

	लागि (पूर्णाङ्क i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		लागि (पूर्णाङ्क k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		लागि (पूर्णाङ्क k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क s_width = GetSize(cell->getPort(ID::S));

	लागि (पूर्णाङ्क i = 0; i < width; i++)
	{
		लागि (पूर्णाङ्क k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		लागि (पूर्णाङ्क k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क s_width = GetSize(cell->getPort(ID::S));

	लागि (पूर्णाङ्क i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		लागि (पूर्णाङ्क k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	तार्किक is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));
	पूर्णाङ्क y_width = GetSize(cell->getPort(ID::Y));

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
	पूर्णाङ्क b_width_capped = min(b_width, 30);

	पूर्णाङ्क b_माथिल्लो, b_तल्लो;
	यदि (!is_b_signed) {
		b_माथिल्लो = (1 << b_width_capped) - 1;
		b_तल्लो = 0;
	} अन्यथा {
		b_माथिल्लो = (1 << (b_width_capped - 1)) - 1;
		b_तल्लो = -(1 << (b_width_capped - 1));
	}

	लागि (पूर्णाङ्क i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		पूर्णाङ्क b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		पूर्णाङ्क a_range_upper;
		// lowest position of A that can be moved to Y[i]
		पूर्णाङ्क a_range_lower;

		यदि (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_माथिल्लो;
			यदि (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_माथिल्लो);
			a_range_upper = min(i+1, a_width);
		} अन्यथा यदि (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_माथिल्लो, a_width);
		} अन्यथा यदि (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_तल्लो;
			a_range_lower = max(0, i + b_तल्लो);
			यदि (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_माथिल्लो, a_width);
		} अन्यथा {
			log_assert(असत्य && "unreachable");
		}

		यदि (i < b_range_upper) {
			लागि (पूर्णाङ्क k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} अन्यथा {
			// only influence is through sign extension
			यदि (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		लागि (पूर्णाङ्क k = 0; k < b_width_capped; k++) {
			// left shifts
			यदि (cell->type.in(ID($shl), ID($sshl))) {
				यदि (a_width == 1 && is_signed) {
					पूर्णाङ्क skip = 1 << (k + 1);
					पूर्णाङ्क base = skip -1;
					यदि (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा यदि (is_signed) {
					यदि (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					यदि (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} अन्यथा यदि (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				यदि (is_signed) {
					तार्किक shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					तार्किक zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					यदि (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					यदि (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} अन्यथा यदि (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				यदि (is_signed) {
					यदि (k != b_width_capped - 1) {
						तार्किक r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						तार्किक r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_माथिल्लो));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						तार्किक l = a_width - 2 - i >= b_तल्लो;
						यदि (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						यदि (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} अन्यथा {
						यदि (y_width - i <= b_माथिल्लो || a_width - 2 - i >= b_तल्लो)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} अन्यथा {
					यदि (a_width - 1 - i >= b_तल्लो)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			} अन्यथा {
				log_assert(असत्य && "unreachable");
			}
		}
	}
}

शून्य packed_mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type == ID($mem_v2));
	Const rd_clk_enable = cell->getParam(ID::RD_CLK_ENABLE);
	पूर्णाङ्क n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	पूर्णाङ्क abits = cell->getParam(ID::ABITS).as_int();
	पूर्णाङ्क width = cell->getParam(ID::WIDTH).as_int();

	लागि (पूर्णाङ्क i = 0; i < n_rd_ports; i++) {
		यदि (rd_clk_enable[i] != State::S0) {
			लागि (पूर्णाङ्क k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			जारी;
		}

		लागि (पूर्णाङ्क j = 0; j < abits; j++)
			लागि (पूर्णाङ्क k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

शून्य memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	पूर्णाङ्क abits = cell->getParam(ID::ABITS).as_int();
	पूर्णाङ्क width = cell->getParam(ID::WIDTH).as_int();

	यदि (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		यदि (cell->type == ID($memrd_v2)) {
			लागि (पूर्णाङ्क k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		फर्काऊ;
	}

	लागि (पूर्णाङ्क j = 0; j < abits; j++)
		लागि (पूर्णाङ्क k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

शून्य mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	यदि (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	अन्यथा यदि (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	अन्यथा यदि (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		फर्काऊ; /* no edges here */
	अन्यथा
		log_abort();
}

शून्य ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क width = cell->getPort(ID::Q).size();

	यदि (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		लागि (पूर्णाङ्क k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	यदि (cell->hasPort(ID::CLR))
		लागि (पूर्णाङ्क k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	यदि (cell->hasPort(ID::SET))
		लागि (पूर्णाङ्क k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	यदि (cell->hasPort(ID::ALOAD))
		लागि (पूर्णाङ्क k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	यदि (cell->hasPort(ID::AD))
		लागि (पूर्णाङ्क k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	यदि (cell->hasPort(ID::ARST))
		लागि (पूर्णाङ्क k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

शून्य full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> आगत_ports;
	std::vector<RTLIL::IdString> output_ports;

	लागि (स्वतः &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		यदि (cell->आगत(port) || dir == RTLIL::PortDir::PD_INOUT)
			आगत_ports.थप_back(port);
		यदि (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.थप_back(port);
	}

	लागि (स्वतः out_port : output_ports)
	{
		पूर्णाङ्क out_width = GetSize(cell->getPort(out_port));
		लागि (पूर्णाङ्क out_bit = 0; out_bit < out_width; out_bit++)
		{
			लागि (स्वतः in_port : आगत_ports)
			{
				पूर्णाङ्क in_width = GetSize(cell->getPort(in_port));
				लागि (पूर्णाङ्क in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

शून्य bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));
	पूर्णाङ्क max_width = std::min(width, std::min(a_width, b_width));

	लागि (पूर्णाङ्क i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

शून्य bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णाङ्क width = GetSize(cell->getPort(ID::Y));
	पूर्णाङ्क a_width = GetSize(cell->getPort(ID::A));
	पूर्णाङ्क b_width = GetSize(cell->getPort(ID::B));
	पूर्णाङ्क s_width = GetSize(cell->getPort(ID::S));
	पूर्णाङ्क max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	लागि (पूर्णाङ्क i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

तार्किक YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	यदि (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_द्विआधारी_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type == ID($neg)) {
		arith_neg_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($add), ID($sub))) {
		arith_द्विआधारी_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type == ID($slice)) {
		slice_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type == ID($concat)) {
		concat_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type == ID($bmux)) {
		bmux_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type == ID($demux)) {
		demux_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type == ID($bweqx)) {
		bweqx_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type == ID($bwmux)) {
		bwmux_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->is_builtin_ff()) {
		ff_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($mul), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात))) {
		full_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	यदि (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(एतत्, cell);
		फर्काऊ सत्य;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	यदि (cell->type.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		फर्काऊ सत्य; // no-op: these have either no inputs or no outputs

	फर्काऊ असत्य;
}
