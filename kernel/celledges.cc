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
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक y_width = GetSize(cell->getPort(ID::Y));

	साठी (पूर्णांक i = 0; i < y_width; i++)
	{
		जर (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा जर (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);
	}
}

शून्य bitwise_द्विमान_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));
	पूर्णांक y_width = GetSize(cell->getPort(ID::Y));

	जर (cell->type == ID($and) && !is_signed) {
		जर (a_width > b_width)
			a_width = b_width;
		अन्यथा
			b_width = a_width;
	}

	साठी (पूर्णांक i = 0; i < y_width; i++)
	{
		जर (i < a_width)
			db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		अन्यथा जर (is_signed && a_width > 0)
			db->add_edge(cell, ID::A, a_width-1, ID::Y, i, -1);

		जर (i < b_width)
			db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		अन्यथा जर (is_signed && b_width > 0)
			db->add_edge(cell, ID::B, b_width-1, ID::Y, i, -1);
	}
}

शून्य arith_neg_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक y_width = GetSize(cell->getPort(ID::Y));

	जर (is_signed && a_width == 1)
		y_width = std::min(y_width, 1);

	साठी (पूर्णांक i = 0; i < y_width; i++)
	साठी (पूर्णांक k = 0; k <= i && k < a_width; k++)
		db->add_edge(cell, ID::A, k, ID::Y, i, -1);
}

शून्य arith_द्विमान_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));
	पूर्णांक y_width = GetSize(cell->getPort(ID::Y));

	जर (!is_signed && cell->type != ID($sub)) {
		पूर्णांक ab_width = std::max(a_width, b_width);
		y_width = std::min(y_width, ab_width+1);
	}

	साठी (पूर्णांक i = 0; i < y_width; i++)
	{
		साठी (पूर्णांक k = 0; k <= i; k++)
		{
			जर (k < a_width)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);

			जर (k < b_width)
				db->add_edge(cell, ID::B, k, ID::Y, i, -1);
		}
	}
}

शून्य reduce_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));

	साठी (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
}

शून्य logic_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));

	साठी (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);
	साठी (पूर्णांक i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

शून्य concat_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));

	साठी (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
	साठी (पूर्णांक i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, a_width + i, -1);
}

शून्य slice_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक offset = cell->getParam(ID::OFFSET).as_int();
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक y_width = GetSize(cell->getPort(ID::Y));

	साठी (पूर्णांक i = 0; i < y_width; i++) {
		पूर्णांक a_bit = offset + i;
		जर (a_bit >= 0 && a_bit < a_width)
			db->add_edge(cell, ID::A, a_bit, ID::Y, i, -1);
	}
}

शून्य compare_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));

	साठी (पूर्णांक i = 0; i < a_width; i++)
		db->add_edge(cell, ID::A, i, ID::Y, 0, -1);

	साठी (पूर्णांक i = 0; i < b_width; i++)
		db->add_edge(cell, ID::B, i, ID::Y, 0, -1);
}

शून्य mux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));
	पूर्णांक s_width = GetSize(cell->getPort(ID::S));

	साठी (पूर्णांक i = 0; i < a_width; i++)
	{
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);

		साठी (पूर्णांक k = i; k < b_width; k += a_width)
			db->add_edge(cell, ID::B, k, ID::Y, i, -1);

		साठी (पूर्णांक k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य bmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = GetSize(cell->getPort(ID::Y));
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक s_width = GetSize(cell->getPort(ID::S));

	साठी (पूर्णांक i = 0; i < width; i++)
	{
		साठी (पूर्णांक k = i; k < a_width; k += width)
			db->add_edge(cell, ID::A, k, ID::Y, i, -1);

		साठी (पूर्णांक k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य demux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = GetSize(cell->getPort(ID::Y));
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक s_width = GetSize(cell->getPort(ID::S));

	साठी (पूर्णांक i = 0; i < width; i++)
	{
		db->add_edge(cell, ID::A, i % a_width, ID::Y, i, -1);
		साठी (पूर्णांक k = 0; k < s_width; k++)
			db->add_edge(cell, ID::S, k, ID::Y, i, -1);
	}
}

शून्य shift_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	तार्किक is_signed = cell->getParam(ID::A_SIGNED).as_bool();
	तार्किक is_b_signed = cell->getParam(ID::B_SIGNED).as_bool();
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));
	पूर्णांक y_width = GetSize(cell->getPort(ID::Y));

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
	पूर्णांक b_width_capped = min(b_width, 30);

	पूर्णांक b_वरचा, b_खालचा;
	जर (!is_b_signed) {
		b_वरचा = (1 << b_width_capped) - 1;
		b_खालचा = 0;
	} अन्यथा {
		b_वरचा = (1 << (b_width_capped - 1)) - 1;
		b_खालचा = -(1 << (b_width_capped - 1));
	}

	साठी (पूर्णांक i = 0; i < y_width; i++){
		// highest position of Y that can change with the value of B
		पूर्णांक b_range_upper = 0;
		// 1 + highest position of A that can be moved to Y[i]
		पूर्णांक a_range_upper;
		// lowest position of A that can be moved to Y[i]
		पूर्णांक a_range_lower;

		जर (cell->type.in(ID($shl), ID($sshl))) {
			b_range_upper = a_width + b_वरचा;
			जर (is_signed) b_range_upper -= 1;
			a_range_lower = max(0, i - b_वरचा);
			a_range_upper = min(i+1, a_width);
		} अन्यथा जर (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
			b_range_upper = a_width;
			a_range_lower = min(i, a_width - 1);
			a_range_upper = min(i+1 + b_वरचा, a_width);
		} अन्यथा जर (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
			// can go both ways depending on sign of B
			// 2's complement range is different depending on direction
			b_range_upper = a_width - b_खालचा;
			a_range_lower = max(0, i + b_खालचा);
			जर (is_signed)
				a_range_lower = min(a_range_lower, a_width - 1);
			a_range_upper = min(i+1 + b_वरचा, a_width);
		} अन्यथा {
			log_assert(असत्य && "unreachable");
		}

		जर (i < b_range_upper) {
			साठी (पूर्णांक k = a_range_lower; k < a_range_upper; k++)
				db->add_edge(cell, ID::A, k, ID::Y, i, -1);
		} अन्यथा {
			// only influence is through sign extension
			जर (is_signed)
				db->add_edge(cell, ID::A, a_width - 1, ID::Y, i, -1);
		}

		साठी (पूर्णांक k = 0; k < b_width_capped; k++) {
			// left shifts
			जर (cell->type.in(ID($shl), ID($sshl))) {
				जर (a_width == 1 && is_signed) {
					पूर्णांक skip = 1 << (k + 1);
					पूर्णांक base = skip -1;
					जर (i % skip != base && i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा जर (is_signed) {
					जर (i - a_width + 2 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					जर (i - a_width + 1 < 1 << b_width_capped)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// right shifts
			} अन्यथा जर (cell->type.in(ID($shr), ID($sshr)) || (cell->type.in(ID($shift), ID($shiftx)) && !is_b_signed)) {
				जर (is_signed) {
					तार्किक shift_in_bulk = i < a_width - 1;
					// can we jump into the zero-padding by toggling B[k]?
					तार्किक zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
									&& (((y_width - i) & ~(1 << k)) < (1 << b_width_capped)));

					जर (shift_in_bulk || (cell->type.in(ID($shr), ID($shift), ID($shiftx)) && zpad_jump))
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				} अन्यथा {
					जर (i < a_width)
						db->add_edge(cell, ID::B, k, ID::Y, i, -1);
				}
			// bidirectional shifts (positive B shifts right, negative left)
			} अन्यथा जर (cell->type.in(ID($shift), ID($shiftx)) && is_b_signed) {
				जर (is_signed) {
					जर (k != b_width_capped - 1) {
						तार्किक r_shift_in_bulk = i < a_width - 1;
						// assuming B is positive, can we jump into the upper zero-padding by toggling B[k]?
						तार्किक r_zpad_jump = (((y_width - i) & ((1 << (k + 1)) - 1)) != 0 \
											&& (((y_width - i) & ~(1 << k)) <= b_वरचा));
						// assuming B is negative, can we influence Y[i] by toggling B[k]?
						तार्किक l = a_width - 2 - i >= b_खालचा;
						जर (a_width == 1) {
							// in case of a_width==1 we go into more detailed reasoning
							l = l && (~(i - a_width) & ((1 << (k + 1)) - 1)) != 0;
						}
						जर (r_shift_in_bulk || r_zpad_jump || l)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					} अन्यथा {
						जर (y_width - i <= b_वरचा || a_width - 2 - i >= b_खालचा)
							db->add_edge(cell, ID::B, k, ID::Y, i, -1);
					}
				} अन्यथा {
					जर (a_width - 1 - i >= b_खालचा)
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
	पूर्णांक n_rd_ports = cell->getParam(ID::RD_PORTS).as_int();
	पूर्णांक abits = cell->getParam(ID::ABITS).as_int();
	पूर्णांक width = cell->getParam(ID::WIDTH).as_int();

	साठी (पूर्णांक i = 0; i < n_rd_ports; i++) {
		जर (rd_clk_enable[i] != State::S0) {
			साठी (पूर्णांक k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ARST, i, ID::RD_DATA, i * width + k, -1);
			सुरूठेवा;
		}

		साठी (पूर्णांक j = 0; j < abits; j++)
			साठी (पूर्णांक k = 0; k < width; k++)
				db->add_edge(cell, ID::RD_ADDR, i * abits + j,
								   ID::RD_DATA, i * width + k, -1);
	}
}

शून्य memrd_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	log_assert(cell->type.in(ID($memrd), ID($memrd_v2)));
	पूर्णांक abits = cell->getParam(ID::ABITS).as_int();
	पूर्णांक width = cell->getParam(ID::WIDTH).as_int();

	जर (cell->getParam(ID::CLK_ENABLE).as_bool()) {
		जर (cell->type == ID($memrd_v2)) {
			साठी (पूर्णांक k = 0; k < width; k++)
				db->add_edge(cell, ID::ARST, 0, ID::DATA, k, -1);
		}
		परत;
	}

	साठी (पूर्णांक j = 0; j < abits; j++)
		साठी (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::ADDR, j, ID::DATA, k, -1);
}

शून्य mem_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	जर (cell->type == ID($mem_v2))
		packed_mem_op(db, cell);
	अन्यथा जर (cell->type.in(ID($memrd), ID($memrd_v2)))
		memrd_op(db, cell);
	अन्यथा जर (cell->type.in(ID($memwr), ID($memwr_v2), ID($meminit)))
		परत; /* no edges here */
	अन्यथा
		log_abort();
}

शून्य ff_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = cell->getPort(ID::Q).size();

	जर (cell->type.in(ID($dlatch), ID($adlatch), ID($dlatchsr))) {
		साठी (पूर्णांक k = 0; k < width; k++) {
			db->add_edge(cell, ID::D, k, ID::Q, k, -1);
			db->add_edge(cell, ID::EN, 0, ID::Q, k, -1);
		}
	}

	जर (cell->hasPort(ID::CLR))
		साठी (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::CLR, 0, ID::Q, k, -1);
	जर (cell->hasPort(ID::SET))
		साठी (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::SET, 0, ID::Q, k, -1);
	जर (cell->hasPort(ID::ALOAD))
		साठी (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::ALOAD, 0, ID::Q, k, -1);
	जर (cell->hasPort(ID::AD))
		साठी (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::AD, k, ID::Q, k, -1);
	जर (cell->hasPort(ID::ARST))
		साठी (पूर्णांक k = 0; k < width; k++)
			db->add_edge(cell, ID::ARST, 0, ID::Q, k, -1);
}

शून्य full_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	std::vector<RTLIL::IdString> आदान_ports;
	std::vector<RTLIL::IdString> output_ports;

	साठी (स्वतः &conn : cell->connections())
	{
		RTLIL::IdString port = conn.first;
		RTLIL::PortDir dir = cell->port_dir(port);
		जर (cell->आदान(port) || dir == RTLIL::PortDir::PD_INOUT)
			आदान_ports.ढकला_back(port);
		जर (cell->output(port) || dir == RTLIL::PortDir::PD_INOUT)
			output_ports.ढकला_back(port);
	}

	साठी (स्वतः out_port : output_ports)
	{
		पूर्णांक out_width = GetSize(cell->getPort(out_port));
		साठी (पूर्णांक out_bit = 0; out_bit < out_width; out_bit++)
		{
			साठी (स्वतः in_port : आदान_ports)
			{
				पूर्णांक in_width = GetSize(cell->getPort(in_port));
				साठी (पूर्णांक in_bit = 0; in_bit < in_width; in_bit++)
					db->add_edge(cell, in_port, in_bit, out_port, out_bit, -1);
			}
		}
	}
}

शून्य bweqx_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = GetSize(cell->getPort(ID::Y));
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));
	पूर्णांक max_width = std::min(width, std::min(a_width, b_width));

	साठी (पूर्णांक i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
	}
}

शून्य bwmux_op(AbstractCellEdgesDatabase *db, RTLIL::Cell *cell)
{
	पूर्णांक width = GetSize(cell->getPort(ID::Y));
	पूर्णांक a_width = GetSize(cell->getPort(ID::A));
	पूर्णांक b_width = GetSize(cell->getPort(ID::B));
	पूर्णांक s_width = GetSize(cell->getPort(ID::S));
	पूर्णांक max_width = std::min(width, std::min(a_width, std::min(b_width, s_width)));

	साठी (पूर्णांक i = 0; i < max_width; i++) {
		db->add_edge(cell, ID::A, i, ID::Y, i, -1);
		db->add_edge(cell, ID::B, i, ID::Y, i, -1);
		db->add_edge(cell, ID::S, i, ID::Y, i, -1);
	}
}

PRIVATE_NAMESPACE_END

तार्किक YOSYS_NAMESPACE_PREFIX AbstractCellEdgesDatabase::add_edges_from_cell(RTLIL::Cell *cell)
{
	जर (cell->type.in(ID($not), ID($pos), ID($buf), ID($barrier))) {
		bitwise_unary_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($and), ID($or), ID($xor), ID($xnor))) {
		bitwise_द्विमान_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type == ID($neg)) {
		arith_neg_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($add), ID($sub))) {
		arith_द्विमान_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($reduce_and), ID($reduce_or), ID($reduce_xor), ID($reduce_xnor), ID($reduce_bool), ID($logic_not))) {
		reduce_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($logic_and), ID($logic_or))) {
		logic_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type == ID($slice)) {
		slice_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type == ID($concat)) {
		concat_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($shl), ID($shr), ID($sshl), ID($sshr), ID($shift), ID($shiftx))) {
		shift_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($lt), ID($le), ID($eq), ID($ne), ID($eqx), ID($nex), ID($ge), ID($gt))) {
		compare_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($mux), ID($pmux))) {
		mux_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type == ID($bmux)) {
		bmux_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type == ID($demux)) {
		demux_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type == ID($bweqx)) {
		bweqx_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type == ID($bwmux)) {
		bwmux_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($mem_v2), ID($memrd), ID($memrd_v2), ID($memwr), ID($memwr_v2), ID($meminit))) {
		mem_op(हे, cell);
		परत सत्य;
	}

	जर (cell->is_builtin_ff()) {
		ff_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($mul), ID($भाग), ID($mod), ID($divfloor), ID($modfloor), ID($घात))) {
		full_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(ID($lut), ID($sop), ID($alu), ID($lcu), ID($macc), ID($macc_v2))) {
		full_op(हे, cell);
		परत सत्य;
	}

	जर (cell->type.in(
			ID($_BUF_), ID($_NOT_), ID($_AND_), ID($_NAND_), ID($_OR_), ID($_NOR_),
			ID($_XOR_), ID($_XNOR_), ID($_ANDNOT_), ID($_ORNOT_), ID($_MUX_), ID($_NMUX_),
			ID($_MUX4_), ID($_MUX8_), ID($_MUX16_), ID($_AOI3_), ID($_OAI3_), ID($_AOI4_),
			ID($_OAI4_), ID($_TBUF_))) {
		full_op(हे, cell);
		परत सत्य;
	}

	// FIXME: $specify2 $specify3 $specrule ???
	// FIXME: $equiv $set_tag $get_tag $overwrite_tag $original_tag

	जर (cell->type.in(ID($प्रतिज्ञा), ID($assume), ID($live), ID($fair), ID($cover), ID($initstate), ID($anyconst), ID($anyseq), ID($allconst), ID($allseq)))
		परत सत्य; // no-op: these have either no inputs or no outputs

	परत असत्य;
}
